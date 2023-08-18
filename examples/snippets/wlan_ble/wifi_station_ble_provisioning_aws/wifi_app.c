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
#include "sl_tls.h"
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
#include "rsi_rom_power_save.h"
#include "sl_si91x_driver.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#include "rsi_chip.h"
#include "rsi_wisemcu_hardware_setup.h"
#endif

#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

#include "wifi_config.h"
#include "rsi_common_apis.h"
#include "rsi_bt_common_apis.h"

#include "aws_iot_config.h"
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
#include "rsi_m4.h"

//! I2C related files
#include "i2c_leader_example.h"

extern rsi_ble_event_conn_status_t conn_event_to_app;

/******************************************************
 *                    Constants
 ******************************************************/
#define DHCP_HOST_NAME    NULL
#define TIMEOUT_MS        5000
#define WIFI_SCAN_TIMEOUT 10000

#define MQTT_TOPIC1               "aws_status"
#define MQTT_TOPIC2               "si91x_status"
#define MQTT_publish_QOS0_PAYLOAD "Hi from SiWx917"
#define MQTT_USERNAME             "username"
#define MQTT_PASSWORD             "password"

#define ENABLE_POWER_SAVE        1
#define ALARM_TIMER_BASED_WAKEUP 1
#define BUTTON_BASED_WAKEUP      1
#define I2C_SENSOR_PERI_ENABLE   0
/*
 *********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

sl_wifi_scan_result_t *scan_result          = NULL;
static volatile bool scan_complete          = false;
static volatile sl_status_t callback_status = SL_STATUS_OK;
uint16_t scanbuf_size = (sizeof(sl_wifi_scan_result_t) + (SL_MAX_SCANNED_AP * sizeof(scan_result->scan_info[0])));

sl_wifi_performance_profile_t wifi_profile = { ASSOCIATED_POWER_SAVE, 0, 0, 1000, { 0 } };

uint8_t connected = 0, timeout = 0;
uint8_t disconnected = 0, disassosiated = 0;
uint8_t a = 0;

sl_wifi_client_configuration_t access_point = { 0 };
sl_net_ip_configuration_t ip_address        = { 0 };

static uint32_t wlan_app_event_map;

uint8_t retry = 1;

uint8_t yield;
AWS_IoT_Client client   = { 0 };
uint8_t disconnect_flag = 0;

#ifdef RSI_M4_INTERFACE
#ifdef COMMON_FLASH_EN
#ifdef CHIP_917B0
#define IVT_OFFSET_ADDR 0x81C2000 /*<!Application IVT location VTOR offset>          B0 common flash Board*/
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset>          A0 Common flash Board*/
#endif
#else
#define IVT_OFFSET_ADDR \
  0x8012000 /*<!Application IVT location VTOR offset>          Dual Flash  (both A0 and B0) Board*/
#endif
#ifdef CHIP_917B0
#define WKP_RAM_USAGE_LOCATION \
  0x24061EFC /*<!Bootloader RAM usage location upon wake up  */ // B0 Boards (common flash & Dual flash)
#else
#define WKP_RAM_USAGE_LOCATION \
  0x24061000 /*<!Bootloader RAM usage location upon wake up  */ // A0 Boards (common flash & Dual flash)
#endif

#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn

#define WIRELESS_WAKEUP_IRQHandler_Periority 8

#define ALARM_PERIODIC_TIME    30 /*<! periodic alarm configuration in SEC */
#define RTC_ALARM_INTR         BIT(16)
#define NPSS_GPIO_2            2
#define NPSSGPIO_PIN_MUX_MODE2 2
#define NPSS_GPIO_INTR_LOW     0
#define NPSS_GPIO_DIR_INPUT    1
#define NPSS_GPIO_2_INTR       BIT(2)

/*Update time configurations for next boundary alarm*/
#define RC_TRIGGER_TIME           5
#define RO_TRIGGER_TIME           0
#define NO_OF_HOURS_IN_A_DAY      24
#define NO_OF_MINUTES_IN_AN_HOUR  60
#define NO_OF_SECONDS_IN_A_MINUTE 60
#define NO_OF_MONTHS_IN_A_YEAR    12
#define BASE_YEAR                 2000
#define NO_OF_DAYS_IN_A_MONTH_1   28
#define NO_OF_DAYS_IN_A_MONTH_2   29
#define NO_OF_DAYS_IN_A_MONTH_3   30
#define NO_OF_DAYS_IN_A_MONTH_4   31

/* Constants required to manipulate the NVIC. */
#define portNVIC_SHPR3_REG   (*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI  (((uint32_t)(0x3f << 4)) << 16UL)
#define portNVIC_SYSTICK_PRI (((uint32_t)(0x3f << 4)) << 24UL)

#define RTC_ALARM_IRQHandler IRQ028_Handler
#define NVIC_RTC_ALARM       MCU_CAL_ALARM_IRQn
#endif

uint32_t first_time                     = 0;
IoT_Publish_Message_Params publish_QOS0 = { 0 };
volatile uint8_t nvic_enable1[240];

fd_set read_fds;
volatile uint8_t check_for_recv_data = 0, select_given = 0;
int32_t status1            = RSI_SUCCESS;
AWS_IoT_Client mqtt_client = { 0 };
#define RSI_FD_ISSET(x, y) rsi_fd_isset(x, y)

#ifdef RSI_M4_INTERFACE
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
  // printf("select call back triggered\r\n");
  //!Check the data pending on this particular socket descriptor
  if (FD_ISSET(mqtt_client.networkStack.socket_id, fd_read)) {
    check_for_recv_data = 1;
    //    printf("data Pending\r\n");
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
  sl_tls_store_configuration_t tls_configuration = { 0 };
  sl_status_t status                             = SL_STATUS_FAIL;

  tls_configuration.cacert             = (uint8_t *)aws_starfield_ca;
  tls_configuration.cacert_length      = (sizeof(aws_starfield_ca));
  tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
  tls_configuration.clientcert         = (uint8_t *)aws_client_certificate;
  tls_configuration.clientcert_length  = (sizeof(aws_client_certificate));
  tls_configuration.clientcert_type    = SL_TLS_SSL_CLIENT;
  tls_configuration.clientkey          = (uint8_t *)aws_client_private_key;
  tls_configuration.clientkey_length   = (sizeof(aws_client_private_key));
  tls_configuration.clientkey_type     = SL_TLS_SSL_CLIENT_PRIVATE_KEY;
  tls_configuration.use_secure_element = false;

  //! Loading SSL Client certificate in FLASH
  status = sl_tls_set_global_ca_store(tls_configuration);
  if (status != SL_STATUS_OK) {
    printf("\r\nLoading SSL certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  return SL_STATUS_OK;
}

int32_t rsi_wlan_mqtt_certs_init(void)
{
  sl_status_t status = RSI_SUCCESS;

  status = load_certificates_in_flash();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while loading certificates: 0x%lx\r\n", status);
    return status;
  }

  rsi_wlan_app_call_backs_init();

  return status;
}

static sl_status_t show_scan_results()
{
  printf("%lu Scan results:\n", scan_result->scan_count);

  if (scan_result->scan_count) {
    printf("\n   %s %24s %s", "SSID", "SECURITY", "NETWORK");
    printf("%12s %12s %s\n", "BSSID", "CHANNEL", "RSSI");

    for (int a = 0; a < (int)scan_result->scan_count; ++a) {
      uint8_t *bssid = (uint8_t *)&scan_result->scan_info[a].bssid;
      printf("%-24s %4u,  %4u, ",
             scan_result->scan_info[a].ssid,
             scan_result->scan_info[a].security_mode,
             scan_result->scan_info[a].network_type);
      printf("  %02x:%02x:%02x:%02x:%02x:%02x, %4u,  -%u\n",
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

  if (CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)result;
    return SL_STATUS_FAIL;
  }

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

        cred.type = SL_WIFI_CRED_PSK;
        memcpy(cred.psk.value, pwd, strlen((char *)pwd));

        status = sl_net_set_credential(id, SL_NET_WIFI_PSK, pwd, strlen((char *)pwd));
        if (SL_STATUS_OK == status) {
          printf("Credentials set, id : %lu\n", id);

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
        status = sl_si91x_configure_ip_address(&ip_address, CLIENT_MODE);
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

#if defined(RSI_DEBUG_PRINTS)
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

  char cpayload[150] = { 0 };

  IoT_Client_Init_Params mqttInitParams   = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

  IoT_Publish_Message_Params paramsQOS0;

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
  connectParams.pClientID              = AWS_IOT_MQTT_CLIENT_ID;
  connectParams.clientIDLen            = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);
  connectParams.isWillMsgPresent       = false;

  connectParams.pUsername   = MQTT_USERNAME;
  connectParams.usernameLen = strlen(MQTT_USERNAME);
  connectParams.pPassword   = MQTT_PASSWORD;
  connectParams.passwordLen = strlen(MQTT_PASSWORD);

  sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;

#if ALARM_TIMER_BASED_WAKEUP
  initialize_m4_alarm();
#endif
#ifdef RSI_WITH_OS
  osSemaphoreRelease(rsi_mqtt_sem);
#endif

  while (1) {
#ifdef RSI_WITH_OS
    osSemaphoreAcquire(rsi_mqtt_sem, osWaitForever);
#endif
    switch (sl_wlan_app_cb.state) {

      case SL_WIFI_MQTT_INIT_STATE: {
        rc = aws_iot_mqtt_init(&mqtt_client, &mqttInitParams);
        if (SUCCESS != rc) {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
          printf("\r\nMqtt Init failed with error: 0x%x\r\n", rc);
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
        }

#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;
      case SL_WIFI_MQTT_CONNECT_STATE: {
        printf("AWS IOT MQTT Connecting...\n");
        rc = aws_iot_mqtt_connect(&mqtt_client, &connectParams);
        if (SUCCESS != rc) {
          if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
            printf("Network is already connected\n");
            //sl_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
          } else {
            printf("\r\nMqtt Subscribe failed with error: 0x%x\r\n", rc);
            sl_wlan_app_cb.state = SL_WLAN_MQTT_DISCONNECT;
          }
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE;
        }
#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;
      case SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE: {
        rc = aws_iot_mqtt_autoreconnect_set_status(&mqtt_client, false);
        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            printf("MQTT auto reconnect error\n");
            sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          printf("Unable to set Auto Reconnect to true\n ");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE;
        } else {
          sl_wlan_app_cb.state = SL_WIFI_MQTT_SUBSCRIBE_STATE;
        }
#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;
      case SL_WIFI_MQTT_SUBSCRIBE_STATE: {
        printf("\r\nAWS IOT MQTT Subscribe...\n");
        rc = aws_iot_mqtt_subscribe(&mqtt_client,
                                    MQTT_TOPIC1,
                                    strlen(MQTT_TOPIC1),
                                    QOS0,
                                    iot_subscribe_callback_handler,
                                    NULL);

        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            printf("\r\nSubscribe error\n");
            sl_wlan_app_cb.state = SL_WIFI_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          sl_wlan_app_cb.state = SL_WIFI_MQTT_SUBSCRIBE_STATE;
        }
        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;

        //! initiating power save in BLE mode
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
        }

        sl_wifi_performance_profile_t performance_profile = { .profile         = ASSOCIATED_POWER_SAVE,
                                                              .listen_interval = 1000 };

        rc = sl_wifi_set_performance_profile(&performance_profile);
        if (rc != SL_STATUS_OK) {
          printf("\r\nPower save configuration Failed, Error Code : 0x%X\r\n", rc);
        }
        printf("\r\nAssociated Power Save Enabled\n");

#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;

      case RSI_AWS_SELECT_CONNECT_STATE: {
        {

          if (!select_given) {
            select_given = 1;
            memset(&read_fds, 0, sizeof(fd_set));

            FD_SET(mqtt_client.networkStack.socket_id, &read_fds);
            printf('\r\n Socket ID: %d', mqtt_client.networkStack.socket_id);

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
#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;

      case SL_WIFI_MQTT_PUBLISH_STATE: {
        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
          // If the client is attempting to reconnect we will skip the rest of the loop.
          continue;
        }

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
          printf("\r\nMqtt Publish for QOS0 failed with error: 0x%x\r\n", rc);
        }

        if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
          printf("QOS0 publish ack not received.\n");
        }
        printf("\r\nMQTT Publish Successful\r\n");

        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;

#if ENABLE_POWER_SAVE
        sl_wlan_app_cb.state = RSI_SLEEP_STATE;
#endif

#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;

#if ENABLE_POWER_SAVE
      case RSI_SLEEP_STATE: {
        osDelay(200);
#ifdef RSI_M4_INTERFACE

        m4_sleep_wakeup();
#endif

        sl_wlan_app_cb.state = RSI_AWS_SELECT_CONNECT_STATE;

#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;
#endif

      case SL_WLAN_MQTT_DISCONNECT: {
        rc = aws_iot_mqtt_disconnect(&mqtt_client);
        if (SUCCESS != rc) {
          printf("MQTT Disconnection error\n");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
        } else {
          printf("MQTT Disconnection Successful\n");
          sl_wlan_app_cb.state = SL_WIFI_MQTT_INIT_STATE;
        }
#ifdef RSI_WITH_OS
        osSemaphoreRelease(rsi_mqtt_sem);
#endif
      } break;
      default:
        break;
    }
  }
}

#ifdef RSI_M4_INTERFACE
void m4_sleep_wakeup(void)
{
#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  sl_si91x_disable_default_ldo_mode();

  /* bypass_ldorf_ctrl needs to be enabled */
  sl_si91x_enable_bypass_ldo_rf();

  sl_si91x_disable_flash_ldo();

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         0,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         0,
                         RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else

#if ALARM_TIMER_BASED_WAKEUP
  /* Update the alarm time interval, when to get next interrupt  */
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);

  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);
#endif

  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQHandler_Periority);

  /* Enable NVIC */
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#if BUTTON_BASED_WAKEUP
  /*Configure the UULP GPIO 2 as wakeup source */
  wakeup_source_config();
#endif

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  printf("\r\nM4 Sleep\r\n");

  /* Trigger M4 Sleep*/
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);

  /* Enable M4_TA interrupt */
  sli_m4_ta_interrupt_init();

  //  /*Start of M4 init after wake up  */

  printf("\r\nM4 Wake Up\r\n");

#endif
}
#if ALARM_TIMER_BASED_WAKEUP
void set_alarm_interrupt_timer(uint16_t interval)
{
  /* Get the RTC time,which is used to update alarm time as per RTC time  */
  RSI_RTC_GetDateTime(RTC, &rtc_get_time);

  /*RTC alarm configuration */
  alarm_configuration.DayOfWeek    = rtc_get_time.DayOfWeek;
  alarm_configuration.Month        = rtc_get_time.Month;
  alarm_configuration.Century      = rtc_get_time.Century;
  alarm_configuration.MilliSeconds = rtc_get_time.MilliSeconds;
  alarm_configuration.Day          = rtc_get_time.Day;
  alarm_configuration.Year         = rtc_get_time.Year;
  alarm_configuration.Minute       = rtc_get_time.Minute;
  alarm_configuration.Hour         = rtc_get_time.Hour;
  alarm_configuration.Second       = rtc_get_time.Second;

  /*Update seconds for next boundary alarm */
  alarm_configuration.Second = alarm_configuration.Second + (interval % 60);
  if (alarm_configuration.Second >= (NO_OF_SECONDS_IN_A_MINUTE)) {
    alarm_configuration.Second -= NO_OF_SECONDS_IN_A_MINUTE;
    alarm_configuration.Minute += 1;
  }

  /*Update minutes for next boundary alarm */
  alarm_configuration.Minute = alarm_configuration.Minute + ((interval / 60) % 60);
  if (alarm_configuration.Minute >= (NO_OF_MINUTES_IN_AN_HOUR)) {
    alarm_configuration.Minute -= NO_OF_MINUTES_IN_AN_HOUR;
    alarm_configuration.Hour += 1;
  }

  /*Update hour for next boundary alarm */
  alarm_configuration.Hour = alarm_configuration.Hour + (interval / 3600) % 24;
  if (alarm_configuration.Hour >= (NO_OF_HOURS_IN_A_DAY)) {
    alarm_configuration.Hour -= NO_OF_HOURS_IN_A_DAY;
    alarm_configuration.Day += 1;
  }

  /*Update month for next boundary alarm */
  if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_1) {
    if (alarm_configuration.Month == Febuary) {
      if (alarm_configuration.Year % 4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_2) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    }
    if (alarm_configuration.Month <= July) {
      if (alarm_configuration.Month % 2 == 0) {
        if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
          alarm_configuration.Day = 1;
          alarm_configuration.Month += 1;
        }
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }

    } else if (alarm_configuration.Month % 2 == 0) {
      if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
      alarm_configuration.Day = 1;
      alarm_configuration.Month += 1;
    }
  }

  /*Update year  for next boundary alarm */
  if (alarm_configuration.Month > (NO_OF_MONTHS_IN_A_YEAR)) {
    alarm_configuration.Month = 1;
    alarm_configuration.Year += 1;
  }

  /*Set Alarm configuration */
  RSI_RTC_SetAlarmDateTime(RTC, &alarm_configuration);
}

void initialize_m4_alarm(void)
{
  /*Init RTC*/
  RSI_RTC_Init(RTC);

  /*RTC configuration with some default time */
  rtc_configuration.DayOfWeek    = Saturday;
  rtc_configuration.Month        = March;
  rtc_configuration.Day          = 19;
  rtc_configuration.Century      = 0;
  rtc_configuration.Year         = 19;
  rtc_configuration.Hour         = 23;
  rtc_configuration.Minute       = 59;
  rtc_configuration.Second       = 50;
  rtc_configuration.MilliSeconds = 0;

  /*Set the RTC configuration*/
  RSI_RTC_SetDateTime(RTC, &rtc_configuration);
  /*Enable Alarm feature*/
  RSI_RTC_AlamEnable(RTC, ENABLE);
  /*Enable RTC ALARM interrupts*/
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Initialization of RTC CALIBRATION*/
  RSI_RTC_CalibInitilization();
  /*To calibrate rc and ro */
  RSI_RTC_ROCLK_Calib(TIME_PERIOD, ENABLE, ENABLE, RC_TRIGGER_TIME, ENABLE, ENABLE, RO_TRIGGER_TIME);
  /*Set Alarm as a  wake up source to wake up from deep sleep */
  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);
  /*Enable the RTC alarm interrupts */
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Enable NVIC for RTC */
  NVIC_EnableIRQ(NVIC_RTC_ALARM);
}

/*RTC Alarm interrupt*/
void RTC_ALARM_IRQHandler(void)
{
  volatile uint32_t statusRead = 0;
  /*Get the interrupt status */
  statusRead = RSI_RTC_GetIntrStatus();

  if (statusRead & NPSS_TO_MCU_ALARM_INTR) {
    /* TRIGGER SLEEP STATE */
    /*Clear wake up interrupt */
    RSI_RTC_IntrClear(RTC_ALARM_INTR);
  }
  return;
}

#endif
void IRQ026_Handler()
{
  RSI_PS_GetWkpUpStatus();
  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);
  return;
}

#if BUTTON_BASED_WAKEUP
/**
 * @brief  Configure the UULP GPIO 2 as wakeup source
 * @param  none
 * @return none
 */
void wakeup_source_config(void)
{
  /*Configure the NPSS GPIO mode to wake up  */
  RSI_NPSSGPIO_SetPinMux(NPSS_GPIO_2, NPSSGPIO_PIN_MUX_MODE2);

  /*Configure the NPSS GPIO direction to input */
  RSI_NPSSGPIO_SetDir(NPSS_GPIO_2, NPSS_GPIO_DIR_INPUT);

  /*Configure the NPSS GPIO interrupt polarity */
  RSI_NPSSGPIO_SetPolarity(NPSS_GPIO_2, NPSS_GPIO_INTR_LOW);

  /*Enable the REN*/
  RSI_NPSSGPIO_InputBufferEn(NPSS_GPIO_2, 1);

  /* Set the GPIO to wake from deep sleep */
  RSI_NPSSGPIO_SetWkpGpio(NPSS_GPIO_2_INTR);

  /* Un mask the NPSS GPIO interrupt*/
  RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);

  /*Select wake up sources */
  RSI_PS_SetWkpSources(GPIO_BASED_WAKEUP);

  /*Enable the NPSS GPIO interrupt slot*/
  NVIC_EnableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/**
 * @brief  GPIO based wake up IRQ
 * @param  none
 * @return none
 */
void IRQ021_Handler(void)
{
  /* clear NPSS GPIO interrupt*/
  RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
}
#endif
#endif
