/*******************************************************************************
* @file  wlan_app.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

/**
 * Include files
 * */
//! SL Wi-Fi SDK includes
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_utility.h"
#include "sl_net_wifi_types.h"
#include "errno.h"
#include "socket.h"
#include "select.h"
#include "cmsis_os2.h"
#include "rsi_common_apis.h"
#include <inttypes.h>
#include <string.h>
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_driver.h"
#include "wifi_app.h"
#include "ble_config.h"
#include "rsi_ble_apis.h"
#include "sl_string.h"
#include "app_config.h"
#include "sl_si91x_socket.h"
#include "FreeRTOSConfig.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_driver_gpio.h"
#endif

//! Access point SSID to connect
#define SSID "YOUR_AP_SSID"

//! Security type
#define SECURITY_TYPE SL_WIFI_WPA2

//! Password
#define PSK "YOUR_AP_PASSPHRASE"

//! Connection Type
#define CONNECT_WITH_PMK 0

//! @ref sl_wifi_get_pairwise_master_key `type` argument (3 = PMK generation; see sl_wifi.h)
#define SL_WIFI_PAIRWISE_MASTER_KEY_TYPE (3U)

//! Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.0.156"

#define DHCP_HOST_NAME NULL
#define TIMEOUT_MS     25000
#define LOW            0

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

//! application control block
wifi_app_cb_t wifi_app_cb;

//! Enumeration for commands used in application
typedef enum rsi_app_cmd_e { RSI_DATA = 0 } rsi_app_cmd_t;
sl_wifi_client_configuration_t access_point = { 0 };
sl_net_ip_configuration_t ip_address        = { 0 };
static int client_socket                    = -1;
osThreadId_t wifi_recv_thread               = 0;
uint8_t recv_buffer[100];
uint8_t pairwise_master_key[32] = { 0 };
uint32_t bytes_read             = 0;
uint8_t data[]                  = "Data from Si917\r\n";

#if SL_SI91X_TICKLESS_MODE
extern osSemaphoreId_t data_received_semaphore;
#endif

extern uint8_t volatile ble_connection_done, wlan_socket_connection_done;
extern uint16_t rsi_ble_att2_val_hndl;
//! Function prototypes
extern void rsi_wlan_app_task(void);
extern int32_t rsi_ble_app_send_to_wlan(uint8_t msg_type, uint8_t *buffer, uint32_t length);
extern void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);
extern int32_t rsi_ble_app_get_event(void);
extern int32_t rsi_initiate_power_save(void);
extern bool button_is_pressed;

#if (SLI_SI91X_MCU_INTERFACE && SL_SI91X_TICKLESS_MODE)
void gpio_uulp_pin_interrupt_callback(uint32_t pin_intr)
{
  while (sl_si91x_gpio_get_uulp_npss_pin(pin_intr) == LOW)
    ; // waiting for the button release
  osSemaphoreRelease(data_received_semaphore);
  button_is_pressed = 1;
}
#endif

void wifi_ble_send_data(void)
{
  int32_t status = RSI_SUCCESS;
#if (SL_SI91X_TICKLESS_MODE == 0)
  if (!(wifi_app_cb.event_map & RSI_SEND_EVENT) && rsi_ble_app_get_event() == -1) {
#endif
    //! send packet to wifi
    status = send(client_socket, data, sizeof(data), 0);
    if (status < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Send failed with BSD error:%d\r\n", errno);
      close(client_socket);
      wifi_app_cb.state = WIFI_APP_IPCONFIG_DONE_STATE;
    }
    //! send packet to ble
    //! set the local attribute value.
#if !(BLE_ADVERTISE_ONLY)
    rsi_ble_set_local_att_value(rsi_ble_att2_val_hndl, sizeof(data), data);
#endif
#if (SL_SI91X_TICKLESS_MODE == 0)
  }
#endif
}

void wifi_client_send_data(void)
{
  int32_t status = RSI_SUCCESS;

  if (wifi_app_cb.event_map & RSI_SEND_EVENT) {
    SL_DEBUG_LOG_V2(INFO, "Data from BLE to Wi-Fi: %s\r\n", (uintptr_t)wifi_app_cb.buffer);
    status = send(client_socket, wifi_app_cb.buffer, wifi_app_cb.length, 0);
    if (status < 0) {
      SL_DEBUG_LOG_V2(ERROR, "Send failed with BSD error:%d\r\n", errno);
      close(client_socket);
      wifi_app_cb.state = WIFI_APP_IPCONFIG_DONE_STATE;
    } else {
      wifi_app_cb.buf_in_use = 0;
      wifi_app_cb.event_map &= ~(RSI_SEND_EVENT);
    }
  }

  return;
}

void data_callback(uint32_t sock_no,
                   uint8_t *buffer,
                   uint32_t length,
                   const sl_si91x_socket_metadata_t *firmware_socket_response)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(firmware_socket_response);

  bytes_read += length;
  //! send packet to ble
  rsi_wlan_app_send_to_ble(RSI_DATA, buffer, bytes_read);
#if SL_SI91X_TICKLESS_MODE
  osSemaphoreRelease(data_received_semaphore);
#endif
  bytes_read = 0;
}

sl_status_t join_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  char *result,
                                  uint32_t result_length,
                                  void *arg)
{
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    SL_DEBUG_LOG_V2(ERROR, "F: Join Event received with %" PRIu32 " bytes payload\r\n", result_length);
    if (client_socket) {
      close(client_socket);
    }
    wifi_app_cb.state = WIFI_APP_UNCONNECTED_STATE;
    return status_code;
  }

  return SL_STATUS_OK;
}

void wifi_app_callbacks_init(void)
{
  //! Initialize join fail call back
  sl_wifi_set_join_callback_v2(join_callback_handler, NULL);
}

void rsi_wlan_app_task(void)
{
  int32_t status = RSI_SUCCESS;

  switch (wifi_app_cb.state) {
    case WIFI_APP_INITIAL_STATE: {
      wifi_app_callbacks_init();
#if CONNECT_WITH_PMK
      sl_wifi_ssid_t ssid;
      ssid.length = (uint8_t)sl_strnlen(SSID, sizeof(ssid.value));
      memcpy(ssid.value, SSID, ssid.length);

      status = sl_wifi_get_pairwise_master_key(SL_WIFI_CLIENT_INTERFACE,
                                               SL_WIFI_PAIRWISE_MASTER_KEY_TYPE,
                                               &ssid,
                                               PSK,
                                               pairwise_master_key);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG_V2(ERROR, "Get Pairwise Master Key Failed, Error Code : 0x%" PRIx32 "\r\n", (uint32_t)status);
        return;
      }
      SL_DEBUG_LOG_V2(INFO, "Get Pairwise Master Key Success\r\n");
#endif
      //! update wlan application state
      wifi_app_cb.state = WIFI_APP_UNCONNECTED_STATE;
    } break;
    case WIFI_APP_UNCONNECTED_STATE: {
      //! Connect to an Access point
#if CONNECT_WITH_PMK
      sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
      status = sl_net_set_credential(id, SL_NET_WIFI_PMK, pairwise_master_key, sizeof(pairwise_master_key));
#else
      sl_wifi_credential_t cred  = { 0 };
      sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
      cred.type                  = SL_WIFI_PSK_CREDENTIAL;
      memcpy(cred.psk.value, PSK, strlen((char *)PSK));

      status = sl_net_set_credential(id, SL_NET_WIFI_PSK, PSK, strlen((char *)PSK));
#endif
      if (status == SL_STATUS_OK) {
        SL_DEBUG_LOG_V2(INFO, "Credentials set, id : %" PRIu32 "\r\n", (uint32_t)id);

        access_point.ssid.length = strlen((char *)SSID);
        memcpy(access_point.ssid.value, SSID, access_point.ssid.length);
        access_point.security      = SECURITY_TYPE;
        access_point.encryption    = SL_WIFI_DEFAULT_ENCRYPTION;
        access_point.credential_id = id;

        status = sl_wifi_set_join_configuration(SL_WIFI_CLIENT_INTERFACE, SL_WIFI_JOIN_FEAT_LISTEN_INTERVAL_VALID);
        if (status != SL_STATUS_OK) {
          SL_DEBUG_LOG_V2(ERROR, "Failed to start set join configuration: 0x%" PRIx32 "\r\n", (uint32_t)status);
          return;
        }

        SL_DEBUG_LOG_V2(INFO, "SSID %s\r\n", (uintptr_t)access_point.ssid.value);
        status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "WLAN Connect Failed, Error Code : 0x%" PRIx32 "\r\n", (uint32_t)status);
        } else {
          SL_DEBUG_LOG_V2(INFO, "WLAN connection is successful\r\n");
          wifi_app_cb.state = WIFI_APP_CONNECTED_STATE;
        }
      } else {
        SL_DEBUG_LOG_V2(ERROR, "Failed to set credentials; status: 0x%" PRIx32 "\r\n", (uint32_t)status);
      }
    } break;
    case WIFI_APP_CONNECTED_STATE: {
      //! Configure IP
      ip_address.type      = SL_IPV4;
      ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
      ip_address.host_name = DHCP_HOST_NAME;

      status = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
      if (status == RSI_SUCCESS) {
        //! update wlan application state
        SL_DEBUG_LOG_V2(INFO, "Configured IP\r\n");
        wifi_app_cb.state = WIFI_APP_IPCONFIG_DONE_STATE;
      }
#if ENABLE_NWP_POWER_SAVE

      SL_DEBUG_LOG_V2(INFO, "Initiating PowerSave\r\n");
      status = rsi_initiate_power_save();
      if (status != RSI_SUCCESS) {
        SL_DEBUG_LOG_V2(ERROR, "Failed to initiate power save in BLE mode\r\n");
        return;
      }

      // Enable groupcast filter and beacon drop threshold
      sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
      groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
      groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
      groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

      status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_groupcast_filter_config failed: 0x%" PRIx32 "\r\n", (uint32_t)status);
        return;
      }
      status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
      if (status != SL_STATUS_OK) {
        SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_beacon_drop_threshold failed: 0x%" PRIx32 "\r\n", (uint32_t)status);
        return;
      }
      SL_DEBUG_LOG_V2(INFO, "Enabled Broadcast Data Filter\r\n");

#endif
      break;
    }
    case WIFI_APP_IPCONFIG_DONE_STATE: {
      int return_value                  = 0;
      sl_ipv4_address_t ip              = { 0 };
      struct sockaddr_in server_address = { 0 };

      sl_net_inet_addr(SERVER_IP_ADDRESS, (uint32_t *)&ip);

      server_address.sin_family      = AF_INET;
      server_address.sin_port        = SERVER_PORT;
      server_address.sin_addr.s_addr = ip.value;
      wlan_socket_connection_done    = 0;

      //!Create socket
      client_socket = sl_si91x_socket_async(AF_INET, SOCK_STREAM, IPPROTO_TCP, &data_callback);
      if (client_socket < 0) {
        SL_DEBUG_LOG_V2(ERROR, "Socket creation failed with bsd error: %d\r\n", errno);
        return;
      }
      SL_DEBUG_LOG_V2(INFO, "Socket ID : %d\r\n", client_socket);

#if SSL_CLIENT
      status = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
      if (status < 0) {
        SL_DEBUG_LOG_V2(ERROR, "Set Socket option failed with bsd error: %d\r\n", errno);
        close(client_socket);
        return;
      }
#endif

      //! Socket connect
      return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
      if (return_value < 0) {
        SL_DEBUG_LOG_V2(ERROR, "Socket connect failed with BSD error: %d, return value %d\r\n", errno, return_value);
        close(client_socket);
        return;
      } else {
        wifi_app_cb.state = WIFI_APP_SOCKET_CONNECTED_STATE;
        SL_DEBUG_LOG_V2(INFO, "TCP Socket Connect Success\r\n");
        wlan_socket_connection_done = 1;
      }

    } break;
    case WIFI_APP_SOCKET_CONNECTED_STATE: {
      wifi_client_send_data();
    } break;
    default:
      break;
  }
}

int32_t rsi_ble_app_send_to_wlan(uint8_t msg_type, uint8_t *buffer, uint32_t length)
{
  switch (msg_type) {
    case RSI_DATA: {
      //! buffer is in use or not
      if (!wifi_app_cb.buf_in_use) {
        //! if not in use
        memset(wifi_app_cb.buffer, 0, sizeof(wifi_app_cb.buffer));

        //! copy the buffer to wlan app cb tx buffer
        memcpy(wifi_app_cb.buffer, buffer, length);

        //! hold length information
        wifi_app_cb.length = length;

        //! make buffer in use
        wifi_app_cb.buf_in_use = 1;

        //! raise event to wlan app task
        wifi_app_cb.event_map |= RSI_SEND_EVENT;

#if SL_SI91X_TICKLESS_MODE
        osSemaphoreRelease(data_received_semaphore);
#endif

      } else
      //!if buffer is in use
      {
        return -1;
        //! return error
      }
    } break;
    default:
      break;
  }
  return 0;
}
