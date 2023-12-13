/*******************************************************************************
 * @file  wifi_app.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Wlan Station BLE
 * Provisioning
 * @section Description :
 * This application explains how to get the WLAN connection functionality using
 * BLE provisioning.
 * Silicon Labs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * Silicon Labs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

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
#include "sl_status.h"
#include "sl_wifi_device.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_driver.h"
#include "sl_board_configuration.h"
#include "errno.h"
#include "socket.h"
#include "sl_si91x_socket.h"
#include "sl_wifi_device.h"
#include "sl_event_handler.h"
#include "sl_si91x_driver.h"

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_m4_ps.h"
#include "rsi_rom_power_save.h"

//! I2C related files
#include "i2c_leader_example.h"
#include "rsi_ps_config.h"
#endif

#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

#include "wifi_config.h"
#include "rsi_common_apis.h"
#include "rsi_bt_common_apis.h"

#include "aws_iot_config.h"
#include "aws_iot_shadow_interface.h"
#include "ble_config.h"
#include "wifi_config.h"
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

//! AWS files
#include "aws_iot_log.h"
#include "aws_iot_error.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include <rsi_ble_apis.h>

extern rsi_ble_event_conn_status_t conn_event_to_app;

/******************************************************
 *                    Constants
 ******************************************************/
#define DHCP_HOST_NAME    NULL
#define TIMEOUT_MS        5000
#define WIFI_SCAN_TIMEOUT 10000

#define MQTT_TOPIC1               "aws_status"   //! Subscribe Topic to receive the message from cloud
#define MQTT_TOPIC2               "si91x_status" //! Publish Topic to send the status from application to cloud
#define MQTT_publish_QOS0_PAYLOAD "Hi from SiWx917"
#define PUBLISH_PERIODICITY       (30000) //! Publish periodicity in milliseconds
#define MQTT_USERNAME             "username"
#define MQTT_PASSWORD             "password"

#define ENABLE_POWER_SAVE      1
#define I2C_SENSOR_PERI_ENABLE 0
/*
 *********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

sl_wifi_scan_result_t *scan_result          = NULL;
static volatile bool scan_complete          = false;
static volatile sl_status_t callback_status = SL_STATUS_OK;
uint16_t scanbuf_size = (sizeof(sl_wifi_scan_result_t) + (SL_WIFI_MAX_SCANNED_AP * sizeof(scan_result->scan_info[0])));

sl_wifi_performance_profile_t wifi_profile = { ASSOCIATED_POWER_SAVE, 0, 0, 1000, { 0 } };

uint8_t connected = 0, timeout = 0;
uint8_t disconnected = 0, disassosiated = 0;
uint8_t a = 0;

sl_wifi_client_configuration_t access_point = { 0 };
sl_net_ip_configuration_t ip_address        = { 0 };

static uint32_t wlan_app_event_map;
#if !(defined(SLI_SI91X_MCU_INTERFACE) && ENABLE_POWER_SAVE)
volatile uint8_t publish_msg = 0;
#endif

uint8_t retry = 1;

uint8_t yield;
AWS_IoT_Client client   = { 0 };
uint8_t disconnect_flag = 0;

uint32_t first_time                     = 0;
IoT_Publish_Message_Params publish_QOS0 = { 0 };
volatile uint8_t nvic_enable1[240];

fd_set read_fds;
volatile uint8_t check_for_recv_data = 0;
extern volatile uint8_t select_given;

int32_t status1            = RSI_SUCCESS;
AWS_IoT_Client mqtt_client = { 0 };
#define RSI_FD_ISSET(x, y) rsi_fd_isset(x, y)

#ifdef SLI_SI91X_MCU_INTERFACE
static RTC_TIME_CONFIG_T rtc_configuration, alarm_configuration, rtc_get_time;
#endif

typedef struct sl_wlan_app_cb_s {
  //! wlan application state
  volatile sl_wifi_app_state_t state;

  //! length of buffer to copy
  uint32_t length;

  //! application buffer
  uint8_t buffer[RSI_APP_BUF_SIZE];

  //! to check application buffer availability
  uint8_t buf_in_use;

  //! application events bit map
  uint32_t event_map;

} sl_wlan_app_cb_t;
sl_wlan_app_cb_t sl_wlan_app_cb; //! application control block

/*
 *********************************************************************************************************
 *                                               DATA TYPES
 *********************************************************************************************************
 */
extern void sl_wifi_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);
extern uint8_t coex_ssid[50], pwd[34], sec_type;
void sl_wifi_mqtt_task(void);
void m4_sleep_wakeup(void);
void wakeup_source_config(void);
static sl_status_t show_scan_results();

uint8_t conn_status;
extern uint8_t magic_word;

// WLAN include file for configuration
osSemaphoreId_t rsi_mqtt_sem;
extern osSemaphoreId_t wlan_thread_sem;
/*==============================================*/
/**
 * @fn         sl_wifi_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void sl_wifi_app_set_event(uint32_t event_num)
{
  wlan_app_event_map |= BIT(event_num);

  osSemaphoreRelease(wlan_thread_sem);

  return;
}

/*==============================================*/
/**
 * @fn         sl_wifi_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
void sl_wifi_app_clear_event(uint32_t event_num)
{
  wlan_app_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         sl_wifi_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
int32_t sl_wifi_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (wlan_app_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}

// rejoin failure call back handler in station mode
sl_status_t join_callback_handler(sl_wifi_event_t event, char *result, uint32_t result_length, void *arg)
{
  UNUSED_PARAMETER(event);
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  // update wlan application state
  disconnected = 1;
  connected    = 0;

  sl_wifi_app_set_event(SL_WIFI_DISCONNECTED_STATE);

  return SL_STATUS_OK;
}

void rsi_wlan_app_call_backs_init(void)
{
  //! Initialize join fail call back
  sl_wifi_set_join_callback(join_callback_handler, NULL);
}

void async_socket_select(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status1)
{
  UNUSED_PARAMETER(fd_except);
  UNUSED_PARAMETER(fd_write);
  UNUSED_PARAMETER(status1);
  //!Check the data pending on this particular socket descriptor
  if (FD_ISSET(mqtt_client.networkStack.socket_id, fd_read)) {
    check_for_recv_data = 1;
  }
  sl_wifi_app_set_event(RSI_AWS_SELECT_CONNECT_STATE);
}

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
static void iot_subscribe_callback_handler(AWS_IoT_Client *pClient,
                                           char *topicName,
                                           uint16_t topicNameLen,
                                           IoT_Publish_Message_Params *params,
                                           void *pData)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(topicName);
  UNUSED_PARAMETER(topicNameLen);
  UNUSED_PARAMETER(pData);
  char dt[1000] = { 0 };
  uint32_t len;
  len = params->payloadLen;

  strncpy(dt, params->payload, len);
  LOG_PRINT("\r\n Data received = %s\r\n", dt);
#if !(defined(SLI_SI91X_MCU_INTERFACE) && ENABLE_POWER_SAVE)
  publish_msg = 1;
#endif

  sl_wifi_app_set_event(RSI_AWS_SELECT_CONNECT_STATE);
}

static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{
  IoT_Error_t rc = FAILURE;
  LOG_PRINT("MQTT Disconnect");
  if (NULL == pClient) {
    return;
  }
  IOT_UNUSED(data);
  if (aws_iot_is_autoreconnect_enabled(pClient)) {
    LOG_PRINT("Auto Reconnect is enabled, Reconnecting attempt will start now\n");
  } else {
    LOG_PRINT("Auto Reconnect not enabled. Starting manual reconnect...\n");
    rc = aws_iot_mqtt_attempt_reconnect(pClient);
    if (NETWORK_RECONNECTED == rc) {
      LOG_PRINT("Manual Reconnect Successful");
    } else {
      LOG_PRINT("Manual Reconnect \n");
    }
  }
}

sl_status_t load_certificates_in_flash(void)
{
  sl_status_t status;

  // Load SSL CA certificate
  status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(0),
                                 SL_NET_SIGNING_CERTIFICATE,
                                 aws_starfield_ca,
                                 sizeof(aws_starfield_ca) - 1);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nLoading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nLoading TLS CA certificate at index %d Successfull\r\n", 0);

  // Load SSL Client certificate
  status = sl_net_set_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(0),
                                 SL_NET_CERTIFICATE,
                                 aws_client_certificate,
                                 sizeof(aws_client_certificate) - 1);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nLoading TLS Client certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nLoading TLS Client certificate at index %d Successfull\r\n", 0);

  // Load SSL Client private key
  status = sl_net_set_credential(SL_NET_TLS_CLIENT_CREDENTIAL_ID(0),
                                 SL_NET_PRIVATE_KEY,
                                 aws_client_private_key,
                                 sizeof(aws_client_private_key) - 1);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nLoading TLS Client private key in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nLoading TLS Client private key at index %d Successfull\r\n", 0);

  return SL_STATUS_OK;
}

int32_t rsi_wlan_mqtt_certs_init(void)
{
  sl_status_t status = RSI_SUCCESS;

  status = load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nUnexpected error while loading certificates: 0x%lx\r\n", status);
    return status;
  }

  rsi_wlan_app_call_backs_init();

  return status;
}

static sl_status_t show_scan_results()
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(scan_result);
  uint8_t *bssid = NULL;
  LOG_PRINT("%lu Scan results:\n", scan_result->scan_count);

  if (scan_result->scan_count) {
    LOG_PRINT("\n   %s %24s %s", "SSID", "SECURITY", "NETWORK");
    LOG_PRINT("%12s %12s %s\n", "BSSID", "CHANNEL", "RSSI");

    for (int a = 0; a < (int)scan_result->scan_count; ++a) {
      bssid = (uint8_t *)&scan_result->scan_info[a].bssid;
      LOG_PRINT("%-24s %4u,  %4u, ",
                scan_result->scan_info[a].ssid,
                scan_result->scan_info[a].security_mode,
                scan_result->scan_info[a].network_type);
      LOG_PRINT("  %02x:%02x:%02x:%02x:%02x:%02x, %4u,  -%u\n",
                bssid[0],
                bssid[1],
                bssid[2],
                bssid[3],
                bssid[4],
                bssid[5],
                scan_result->scan_info[a].rf_channel,
                scan_result->scan_info[a].rssi_val);
    }
  }

  return SL_STATUS_OK;
}

sl_status_t wlan_app_scan_callback_handler(sl_wifi_event_t event,
                                           sl_wifi_scan_result_t *result,
                                           uint32_t result_length,
                                           void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  scan_complete = true;

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)result;
    return SL_STATUS_FAIL;
  }
  SL_VERIFY_POINTER_OR_RETURN(scan_result, SL_STATUS_NULL_POINTER);
  memset(scan_result, 0, scanbuf_size);
  memcpy(scan_result, result, scanbuf_size);

  callback_status = show_scan_results();

  //  scan_complete = true;
  return SL_STATUS_OK;
}

void sl_wifi_app_task(void)
{
  int32_t status   = RSI_SUCCESS;
  int32_t event_id = 0;

  // Allocate memory for scan buffer
  scan_result = (sl_wifi_scan_result_t *)malloc(scanbuf_size);
  if (scan_result == NULL) {
    LOG_PRINT("Failed to allocate memory for scan result\n");
    return;
  }
  memset(scan_result, 0, scanbuf_size);
  while (1) {
    // checking for events list
    event_id = sl_wifi_app_get_event();
    if (event_id == -1) {
      osSemaphoreAcquire(wlan_thread_sem, osWaitForever);

      // if events are not received loop will be continued.
      continue;
    }

    switch (event_id) {
      case SL_WIFI_INITIAL_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_INITIAL_STATE);

        // update wlan application state
        if (magic_word) {
          // clear the served event
          sl_wifi_app_set_event(SL_WIFI_FLASH_STATE);
        } else {
          sl_wifi_app_set_event(SL_WIFI_SCAN_STATE);
        }
      } break;

      case SL_WIFI_UNCONNECTED_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_UNCONNECTED_STATE);

        osSemaphoreRelease(wlan_thread_sem);
      } break;

      case SL_WIFI_SCAN_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_SCAN_STATE);

        sl_wifi_scan_configuration_t wifi_scan_configuration = { 0 };
        wifi_scan_configuration                              = default_wifi_scan_configuration;

        sl_wifi_set_scan_callback(wlan_app_scan_callback_handler, NULL);

        status = sl_wifi_start_scan(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, NULL, &wifi_scan_configuration);
        if (SL_STATUS_IN_PROGRESS == status) {
          const uint32_t start = osKernelGetTickCount();

          while (!scan_complete && (osKernelGetTickCount() - start) <= WIFI_SCAN_TIMEOUT) {
            osThreadYield();
          }
          status = scan_complete ? callback_status : SL_STATUS_TIMEOUT;
        }
        if (status != SL_STATUS_OK) {
          LOG_PRINT("\r\nWLAN Scan Wait Failed, Error Code : 0x%lX\r\n", status);
          sl_wifi_app_set_event(SL_WIFI_SCAN_STATE);
          osDelay(1000);
        } else {
          // update wlan application state
          sl_wifi_app_send_to_ble(SL_WIFI_SCAN_RESP, (uint8_t *)scan_result, scanbuf_size);
        }
      } break;

      case SL_WIFI_JOIN_STATE: {
        sl_wifi_credential_t cred  = { 0 };
        sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
        memset(&access_point, 0, sizeof(sl_wifi_client_configuration_t));

        sl_wifi_app_clear_event(SL_WIFI_JOIN_STATE);

        cred.type = SL_WIFI_PSK_CREDENTIAL;
        memcpy(cred.psk.value, pwd, strlen((char *)pwd));

        status = sl_net_set_credential(id, SL_NET_WIFI_PSK, pwd, strlen((char *)pwd));
        if (SL_STATUS_OK == status) {
          LOG_PRINT("Credentials set, id : %lu\n", id);

          access_point.ssid.length = strlen((char *)coex_ssid);
          memcpy(access_point.ssid.value, coex_ssid, access_point.ssid.length);
          access_point.security      = sec_type;
          access_point.encryption    = SL_WIFI_CCMP_ENCRYPTION;
          access_point.credential_id = id;

          LOG_PRINT("SSID %s\n", access_point.ssid.value);
          status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
        }
        if (status != RSI_SUCCESS) {
          timeout = 1;
          sl_wifi_app_send_to_ble(SL_WIFI_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
          LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);

          // update wlan application state
          disconnected = 1;
          connected    = 0;
        } else {
          LOG_PRINT("\n WLAN Connection Success\n");
          // update wlan application state
          sl_wifi_app_set_event(SL_WIFI_CONNECTED_STATE);
        }
        //LOG_PRINT("RSI_WLAN_JOIN_STATE\n");
      } break;

      case SL_WIFI_FLASH_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_FLASH_STATE);

        if (retry) {
          status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
            break;
          } else {
            sl_wifi_app_set_event(SL_WIFI_CONNECTED_STATE);
          }
        }
      } break;

      case SL_WIFI_CONNECTED_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_CONNECTED_STATE);

        ip_address.type      = SL_IPV4;
        ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
        ip_address.host_name = DHCP_HOST_NAME;

        // Configure IP
        status = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
        if (status != RSI_SUCCESS) {
          a++;
          if (a == 3) {
            a       = 0;
            timeout = 1;
            status  = sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
            if (status == RSI_SUCCESS) {
              connected     = 0;
              disassosiated = 1;
              sl_wifi_app_send_to_ble(SL_WIFI_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
              sl_wifi_app_set_event(SL_WIFI_ERROR_STATE);
            }
          }
          LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
          break;
        } else {
          a             = 0;
          connected     = 1;
          conn_status   = 1;
          disconnected  = 0;
          disassosiated = 0;

#if defined(SL_SI91X_PRINT_DBG_LOG)
          sl_ip_address_t ip = { 0 };
          ip.type            = ip_address.type;
          ip.ip.v4.value     = ip_address.ip.v4.ip_address.value;
          print_sl_ip_address(&ip);
#endif

          // update wlan application state
          sl_wifi_app_set_event(SL_WIFI_IPCONFIG_DONE_STATE);
          sl_wifi_app_send_to_ble(SL_WIFI_CONNECTION_STATUS, (uint8_t *)&connected, 1);
        }
      } break;

      case SL_WIFI_IPCONFIG_DONE_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_IPCONFIG_DONE_STATE);
        sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;

        sl_wifi_mqtt_task();

        LOG_PRINT("SL_WIFI_IPCONFIG_DONE_STATE\n");
      } break;

      case SL_WIFI_ERROR_STATE: {

      } break;

      case SL_WIFI_DISCONNECTED_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_DISCONNECTED_STATE);
        retry = 1;
        sl_wifi_app_send_to_ble(SL_WIFI_DISCONNECTION_STATUS, (uint8_t *)&disconnected, 1);
        sl_wifi_app_set_event(SL_WIFI_FLASH_STATE);

        LOG_PRINT("SL_WIFI_DISCONNECTED_STATE\n");
      } break;

      case SL_WIFI_DISCONN_NOTIFY_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_DISCONN_NOTIFY_STATE);

        status = sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
        if (status == RSI_SUCCESS) {
#if RSI_WISE_MCU_ENABLE
          rsi_flash_erase((uint32_t)FLASH_ADDR_TO_STORE_AP_DETAILS);
#endif
          LOG_PRINT("\r\nWLAN Disconnected\r\n");
          disassosiated   = 1;
          connected       = 0;
          yield           = 0;
          disconnect_flag = 0; // reset flag to allow disconnecting again

          sl_wifi_app_send_to_ble(SL_WIFI_DISCONNECTION_NOTIFY, (uint8_t *)&disassosiated, 1);
          sl_wifi_app_set_event(SL_WIFI_UNCONNECTED_STATE);
        } else {
          LOG_PRINT("\r\nWIFI Disconnect Failed, Error Code : 0x%lX\r\n", status);
        }
      } break;
      default:
        break;
    }
  }

  if (scan_result != NULL) {
    free(scan_result);
  }
}

void sl_wifi_mqtt_task(void)
{
  IoT_Error_t rc = FAILURE;
  uint32_t status;

  IoT_Client_Init_Params mqttInitParams   = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
#if !(defined(SLI_SI91X_MCU_INTERFACE) && ENABLE_POWER_SAVE)
  uint32_t start_time         = 0;
  uint8_t publish_timer_start = 0;
#endif

  sl_mac_address_t mac_addr = { 0 };
  char mac_id[18];
  char client_id[25];
  sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  sprintf(mac_id,
          "%x:%x:%x:%x:%x:%x",
          mac_addr.octet[0],
          mac_addr.octet[1],
          mac_addr.octet[2],
          mac_addr.octet[3],
          mac_addr.octet[4],
          mac_addr.octet[5]);
  printf("\r\nMAC ID: %s \r\n", mac_id);
  sprintf(client_id, "silabs_%s", mac_id);
  printf("\r\nClient ID: %s\r\n", client_id);

  mqttInitParams.enableAutoReconnect       = false;
  mqttInitParams.pHostURL                  = AWS_IOT_MQTT_HOST;
  mqttInitParams.port                      = AWS_IOT_MQTT_PORT;
  mqttInitParams.pRootCALocation           = (char *)aws_starfield_ca;
  mqttInitParams.pDeviceCertLocation       = (char *)aws_client_certificate;
  mqttInitParams.pDevicePrivateKeyLocation = (char *)aws_client_private_key;
  mqttInitParams.mqttCommandTimeout_ms     = 20000;
  mqttInitParams.tlsHandshakeTimeout_ms    = 5000;
  mqttInitParams.isSSLHostnameVerify       = true;
  mqttInitParams.disconnectHandler         = disconnectCallbackHandler;
  mqttInitParams.disconnectHandlerData     = NULL;

  connectParams.keepAliveIntervalInSec = 600;
  connectParams.isCleanSession         = true;
  connectParams.MQTTVersion            = MQTT_3_1_1;
  connectParams.pClientID              = client_id;
  connectParams.clientIDLen            = (uint16_t)strlen(client_id);
  connectParams.isWillMsgPresent       = false;

  connectParams.pUsername   = MQTT_USERNAME;
  connectParams.usernameLen = strlen(MQTT_USERNAME);
  connectParams.pPassword   = MQTT_PASSWORD;
  connectParams.passwordLen = strlen(MQTT_PASSWORD);

  sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;

  osSemaphoreRelease(rsi_mqtt_sem);

  while (1) {
    osSemaphoreAcquire(rsi_mqtt_sem, osWaitForever);

    if (sl_wifi_app_get_event() == SL_WIFI_DISCONN_NOTIFY_STATE) {
      LOG_PRINT("WLAN disconnect initiated\n");
      return;
    }

    switch (sl_wlan_app_cb.state) {

      case SL_WIFI_MQTT_INIT_STATE: {
        rc = aws_iot_mqtt_init(&mqtt_client, &mqttInitParams);
        if (SUCCESS != rc) {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
          LOG_PRINT("\r\nMqtt Init failed with error: 0x%x\r\n", rc);
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
        }

        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

      case SL_WIFI_MQTT_CONNECT_STATE: {
        LOG_PRINT("AWS IOT MQTT Connecting...\n");
        rc = aws_iot_mqtt_connect(&mqtt_client, &connectParams);
        if (SUCCESS != rc) {
          if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
            LOG_PRINT("Network is already connected\n");
            //sl_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
          } else {
            LOG_PRINT("\r\nMqtt Subscribe failed with error: 0x%x\r\n", rc);
            sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
          }
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE;
        }
        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

      case SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE: {
        rc = aws_iot_mqtt_autoreconnect_set_status(&mqtt_client, false);
        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            LOG_PRINT("MQTT auto reconnect error\n");
            sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          LOG_PRINT("Unable to set Auto Reconnect to true\n ");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE;
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_SUBSCRIBE_STATE;
        }
        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

      case SL_WIFI_MQTT_SUBSCRIBE_STATE: {
        LOG_PRINT("\r\nAWS IOT MQTT Subscribe...\n");
        rc = aws_iot_mqtt_subscribe(&mqtt_client,
                                    MQTT_TOPIC1,
                                    strlen(MQTT_TOPIC1),
                                    QOS0,
                                    iot_subscribe_callback_handler,
                                    NULL);

        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            LOG_PRINT("\r\nSubscribe error\n");
            sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          sl_wlan_app_cb.state = SL_WIFI_MQTT_SUBSCRIBE_STATE;
        }
        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;
#if ENABLE_POWER_SAVE
        //! initiating power save in BLE mode
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
        }

        sl_wifi_performance_profile_t performance_profile = { .profile         = ASSOCIATED_POWER_SAVE,
                                                              .listen_interval = 1000 };

        rc = sl_wifi_set_performance_profile(&performance_profile);
        if (rc != SL_STATUS_OK) {
          LOG_PRINT("\r\nPower save configuration Failed, Error Code : 0x%X\r\n", rc);
        }
        LOG_PRINT("\r\nAssociated Power Save Enabled\n");
#endif

        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

      case RSI_AWS_SELECT_CONNECT_STATE: {
        {

          if (!select_given) {
            select_given = 1;
            memset(&read_fds, 0, sizeof(fd_set));

            FD_SET(mqtt_client.networkStack.socket_id, &read_fds);
            LOG_PRINT("\r\n Socket ID: %d", mqtt_client.networkStack.socket_id);

            status1 =
              sl_si91x_select(mqtt_client.networkStack.socket_id + 1, &read_fds, NULL, NULL, NULL, async_socket_select);
          }

          if (check_for_recv_data) {
            check_for_recv_data = 0;
            select_given        = 0;
            status1             = aws_iot_shadow_yield(&mqtt_client, 1);

            sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;
          } else {
            sl_wlan_app_cb.state = SL_WIFI_MQTT_PUBLISH_STATE;
          }
        }
        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

      case SL_WIFI_MQTT_PUBLISH_STATE: {
        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
          // If the client is attempting to reconnect we will skip the rest of the loop.
          continue;
        }
#if !(defined(SLI_SI91X_MCU_INTERFACE) && ENABLE_POWER_SAVE)
        if ((!publish_timer_start) || publish_msg) {
#endif

          publish_QOS0.qos        = QOS0;
          publish_QOS0.payload    = MQTT_publish_QOS0_PAYLOAD;
          publish_QOS0.isRetained = 0;
          publish_QOS0.payloadLen = strlen(MQTT_publish_QOS0_PAYLOAD);

#if I2C_SENSOR_PERI_ENABLE
          char temp_data[11];
          // Initialize I2C
          I2C_Init();
          i2c_leader_example_process_action();
          //float sensor_data=temp_data;
          gcvt(sensor_data, 9, temp_data);
          char temp_string[] = "Current Temperature in Celsius: ";
          strcat(temp_string, temp_data);

          publish_QOS0.payload    = temp_string;
          publish_QOS0.payloadLen = strlen(temp_string);

#endif

          // mqtt publish with QOS0
          rc = aws_iot_mqtt_publish(&mqtt_client, MQTT_TOPIC2, strlen(MQTT_TOPIC2), &publish_QOS0);

          if (rc != SUCCESS) {
            LOG_PRINT("\r\nMqtt Publish for QOS0 failed with error: 0x%x\r\n", rc);
            sl_wlan_app_cb.state = SL_WLAN_MQTT_DISCONNECT;
          }

          if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
            LOG_PRINT("QOS0 publish ack not received.\n");
          }
          LOG_PRINT("\r\nMQTT Publish Successful\r\n");
#if !(defined(SLI_SI91X_MCU_INTERFACE) && ENABLE_POWER_SAVE)
          publish_msg = 0;
          if (!publish_timer_start) {
            publish_timer_start = 1;
            start_time          = sl_si91x_host_get_timestamp();
          }
        } else {
          if (sl_si91x_host_elapsed_time(start_time) >= PUBLISH_PERIODICITY) {
            start_time          = 0;
            publish_timer_start = 0;
            publish_msg         = 1;
            continue;
          }
        }

#endif

        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;

#if ENABLE_POWER_SAVE
        sl_wlan_app_cb.state = RSI_SLEEP_STATE;
#endif

        osSemaphoreRelease(rsi_mqtt_sem);
      } break;

#if ENABLE_POWER_SAVE
      case RSI_SLEEP_STATE: {
        osDelay(200);

#ifdef SLI_SI91X_MCU_INTERFACE

        if (select_given == 1 && (check_for_recv_data != 1)) {
          printf("M4 in sleep\r\n");
          sl_si91x_m4_sleep_wakeup();
          printf("M4 Wake up\r\n");
        }

#endif

        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;

        osSemaphoreRelease(rsi_mqtt_sem);
      } break;
#endif

      case SL_WLAN_MQTT_DISCONNECT: {
        rc = aws_iot_mqtt_disconnect(&mqtt_client);
        if (SUCCESS != rc) {
          LOG_PRINT("MQTT Disconnection error\n");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
        } else {
          LOG_PRINT("MQTT Disconnection Successful\n");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
        }
        osSemaphoreRelease(rsi_mqtt_sem);
      } break;
      case SL_WIFI_DISCONN_NOTIFY_STATE: {
        return;
      }
      default:
        break;
    }
  }
}
