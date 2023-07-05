/*******************************************************************************
* @file  wlan_app.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_tls.h"
#include "sl_net_wifi_types.h"

#include "errno.h"
#include "socket.h"
#include "select.h"

#include "cmsis_os2.h"
#include <rsi_common_apis.h>
#include <string.h>

//! Include SSL CA certificate
#include "cacert.pem.h"

#define APP_BUF_SIZE 256

//! SEND event number used in the application
#define RSI_SEND_EVENT BIT(0)

//! Access point SSID to connect
#define SSID "AndroidAP5655"

//! Security type
#define SECURITY_TYPE SL_WIFI_WPA2

//! Password
#define PSK "12345678"

//! Connection Type
#define CONNECT_WITH_PMK 0

//! Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.43.99"

//! Load certificate to device flash for SSL client:
//! Certificate could be loaded once and need not be loaded for every boot up
#define SSL_CLIENT 1

#define DHCP_HOST_NAME NULL
#define TIMEOUT_MS     15000

//! Enumeration for states in applcation
typedef enum wifi_app_state_e {
  SL_WIFI_APP_INITIAL_STATE          = 0,
  SL_WIFI_APP_UNCONNECTED_STATE      = 1,
  SL_WIFI_APP_CONNECTED_STATE        = 2,
  SL_WIFI_APP_IPCONFIG_DONE_STATE    = 3,
  SL_WIFI_APP_SOCKET_CONNECTED_STATE = 4
} wifi_app_state_t;

//! wifi application control block
typedef struct wifi_app_cb_s {
  //! WiFi application state
  wifi_app_state_t state;

  //! length of buffer to copy
  uint32_t length;

  //! application buffer
  uint8_t buffer[APP_BUF_SIZE];

  //! to check application buffer availability
  uint8_t buf_in_use;

  //! application events bit map
  uint32_t event_map;
} wifi_app_cb_t;

//! application control block
wifi_app_cb_t wifi_app_cb;

//! Enumeration for commands used in application
typedef enum rsi_app_cmd_e { RSI_DATA = 0 } rsi_app_cmd_t;
sl_wifi_client_configuration_t access_point       = { 0 };
sl_net_ip_configuration_t ip_address              = { 0 };
static sl_wifi_performance_profile_t wifi_profile = { ASSOCIATED_POWER_SAVE, 0, 0, 1000, { 0 } };
static int client_socket                          = -1;
osThreadId_t wifi_recv_thread                     = 0;
uint8_t recv_buffer[100];
uint8_t pairwise_master_key[32] = { 0 };

//! Function prototypes
extern void rsi_wlan_app_task(void);
extern int32_t rsi_ble_app_send_to_wlan(uint8_t msg_type, uint8_t *buffer, uint32_t length);
extern void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);

void wifi_client_send_data(void)
{
  int32_t status = RSI_SUCCESS;

  if (wifi_app_cb.event_map & RSI_SEND_EVENT) {
    LOG_PRINT("Data from BLE to Wi-Fi: %s\n", wifi_app_cb.buffer);
    status = send(client_socket, wifi_app_cb.buffer, wifi_app_cb.length, 0);
    if (status < 0) {
      LOG_PRINT("\r\nSend failed with BSD error:%d\r\n", errno);
      close(client_socket);
      wifi_app_cb.state = SL_WIFI_APP_IPCONFIG_DONE_STATE;
    } else {
      wifi_app_cb.buf_in_use = 0;
      wifi_app_cb.event_map &= ~(RSI_SEND_EVENT);
    }
  }

  return;
}

void wifi_client_receive_data(void)
{
  int32_t status = RSI_SUCCESS;

  fd_set read_fds;
  int read_bytes         = 0;
  struct timeval timeout = { 0 };
  timeout.tv_sec         = 1;

  FD_ZERO(&read_fds);
  FD_SET(client_socket, &read_fds);

  status = select(client_socket + 1, &read_fds, NULL, NULL, &timeout);
  if (status > 0) {
    //Receive data on socket
    read_bytes = recv(client_socket, recv_buffer, sizeof(recv_buffer), 0);
    if (read_bytes) {
      //! send packet to ble
      rsi_wlan_app_send_to_ble(RSI_DATA, recv_buffer, read_bytes);
    }
  } else if (status < 0) {
    LOG_PRINT("\r\nSocket receive failed with BSD error : %d\r\n", errno);
    close(client_socket);
    wifi_app_cb.state = SL_WIFI_APP_IPCONFIG_DONE_STATE;
  }
  return;
}

sl_status_t join_callback_handler(sl_wifi_event_t event, char *result, uint32_t result_length, void *arg)
{
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(arg);

  if (CHECK_IF_EVENT_FAILED(event)) {
    LOG_PRINT("F: Join Event received with %lu bytes payload\n", result_length);
    if (client_socket) {
      close(client_socket);
    }
    wifi_app_cb.state = SL_WIFI_APP_UNCONNECTED_STATE;
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

void wifi_app_call_backs_init(void)
{
  //! Initialize join fail call back
  sl_wifi_set_join_callback(join_callback_handler, NULL);
}

sl_status_t clear_and_load_certificates_in_flash(void)
{
  sl_tls_store_configuration_t tls_configuration = { 0 };
  //! Erase SSL CA certificate
  sl_status_t status = sl_tls_clear_global_ca_store();
  if (status != SL_STATUS_OK) {
    printf("\r\nClient certificate erase failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  tls_configuration.cacert             = (uint8_t *)cacert;
  tls_configuration.cacert_length      = (sizeof(cacert) - 1);
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.use_secure_element = false;

  //! Loading SSL CA certificate in to FLASH
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nLoading SSL CA certificate in to FLASH Success\r\n");

  return status;
}

void rsi_wlan_app_task(void)
{
  int32_t status = RSI_SUCCESS;

  switch (wifi_app_cb.state) {
    case SL_WIFI_APP_INITIAL_STATE: {
      wifi_app_call_backs_init();
#if SSL_CLIENT
      //! Load certificates
      status = clear_and_load_certificates_in_flash();
      if (status != SL_STATUS_OK) {
        printf("\r\nUnexpected error while loading certificate: 0x%lX\r\n", status);
        return;
      } else
#endif
      {
#if CONNECT_WITH_PMK
        sl_wifi_ssid_t ssid;
        uint8_t type = 3;
        ssid.length  = (uint8_t)strnlen(SSID, sizeof(ssid.value));
        memcpy(ssid.value, SSID, ssid.length);

        status =
          sl_wifi_get_pairwise_master_key(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, type, &ssid, PSK, pairwise_master_key);
        if (status != SL_STATUS_OK) {
          printf("\r\nGet Pairwise Master Key Failed, Error Code : 0x%lX\r\n", status);
          return;
        }
        printf("\r\nGet Pairwise Master Key Success\r\n");
#endif
        //! update wlan application state
        wifi_app_cb.state = SL_WIFI_APP_UNCONNECTED_STATE;
      }
    } break;
    case SL_WIFI_APP_UNCONNECTED_STATE: {
      //! Connect to an Access point
#if CONNECT_WITH_PMK
      sl_wifi_credential_id_t id = SL_NET_DEFAULT_PMK_CREDENTIAL_ID;
      status = sl_net_set_credential(id, SL_NET_WIFI_PMK, pairwise_master_key, sizeof(sl_wifi_pmk_credential_t));
#else
      sl_wifi_credential_t cred  = { 0 };
      sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
      cred.type                  = SL_WIFI_CRED_PSK;
      memcpy(cred.psk.value, PSK, strlen((char *)PSK));

      status = sl_net_set_credential(id, SL_NET_WIFI_PSK, PSK, strlen((char *)PSK));
#endif
      if (status == SL_STATUS_OK) {
        LOG_PRINT("Credentials set, id : %lu\n", id);

        access_point.ssid.length = strlen((char *)SSID);
        memcpy(access_point.ssid.value, SSID, access_point.ssid.length);
        access_point.security      = SECURITY_TYPE;
        access_point.encryption    = SL_WIFI_CCMP_ENCRYPTION;
        access_point.credential_id = id;

        LOG_PRINT("SSID %s\n", access_point.ssid.value);
        status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
        } else {
          LOG_PRINT("\n WLAN connection is successful\n");
          wifi_app_cb.state = SL_WIFI_APP_CONNECTED_STATE;
        }
      } else {
        LOG_PRINT("\r\nFailed to set credentials; status: %lu\n", status);
      }
    } break;
    case SL_WIFI_APP_CONNECTED_STATE: {
      //! Configure IP
      ip_address.type      = SL_IPV4;
      ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
      ip_address.host_name = DHCP_HOST_NAME;

      status = sl_si91x_configure_ip_address(&ip_address, CLIENT_MODE);
      if (status == RSI_SUCCESS) {
        //! update wlan application state
        LOG_PRINT("\r\nConfigured IP\r\n");
        wifi_app_cb.state = SL_WIFI_APP_IPCONFIG_DONE_STATE;
      }

      //! initiating power save in wlan mode
      status = sl_wifi_set_performance_profile(&wifi_profile);
      if (status != SL_STATUS_OK) {
        LOG_PRINT("\r\n Failed to initiate power save in Wi-Fi mode :%lX\r\n", status);
      }

      // Enable Broadcast data filter
      status = sl_wifi_filter_broadcast(5000, 1, 1);
      if (status == RSI_SUCCESS) {
        LOG_PRINT("\r\nEnabled Broadcast Data Filter\n");
      }
      break;
    }
    case SL_WIFI_APP_IPCONFIG_DONE_STATE: {
      int return_value                  = 0;
      sl_ipv4_address_t ip              = { 0 };
      struct sockaddr_in server_address = { 0 };

      convert_string_to_sl_ipv4_address(SERVER_IP_ADDRESS, &ip);

      server_address.sin_family      = AF_INET;
      server_address.sin_port        = SERVER_PORT;
      server_address.sin_addr.s_addr = ip.value;

      //!Create socket
      client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (client_socket < 0) {
        printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
        return;
      }
      printf("\r\nTCP Socket Create Success\r\n");

#if SSL_CLIENT
      //! Setting SSL socket option
      status = setsockopt(client_socket, SOL_TCP, TCP_ULP, TLS, sizeof(TLS));
      if (status < 0) {
        printf("\r\nSet socket failed with bsd error: %d\r\n", errno);
        close(client_socket);
        return;
      }
#endif
      //! Socket connect
      return_value = connect(client_socket, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
      if (return_value < 0) {
        printf("\r\nSocket connect failed with BSD error: %d, return value %d\r\n", errno, return_value);
        close(client_socket);
        return;
      } else {
        wifi_app_cb.state = SL_WIFI_APP_SOCKET_CONNECTED_STATE;
        LOG_PRINT("\r\nTCP Socket Connect Success\r\n");
      }
    } break;
    case SL_WIFI_APP_SOCKET_CONNECTED_STATE: {
      wifi_client_send_data();
      wifi_client_receive_data();
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
