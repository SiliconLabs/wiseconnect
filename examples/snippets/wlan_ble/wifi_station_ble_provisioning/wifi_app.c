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

#include "cmsis_os2.h"
#include <string.h>

#include "wifi_config.h"
#include "rsi_common_apis.h"

// WLAN include file for configuration

extern osSemaphoreId_t wlan_thread_sem;

#define DHCP_HOST_NAME    NULL
#define TIMEOUT_MS        5000
#define WIFI_SCAN_TIMEOUT 10000

/*
 *********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

sl_wifi_scan_result_t *scan_result          = NULL;
static volatile bool scan_complete          = false;
static volatile sl_status_t callback_status = SL_STATUS_OK;
uint16_t scanbuf_size = (sizeof(sl_wifi_scan_result_t) + (SL_WIFI_MAX_SCANNED_AP * sizeof(scan_result->scan_info[0])));

uint8_t connected = 0, timeout = 0;
uint8_t disconnected = 0, disassosiated = 0;
uint8_t a = 0;

sl_wifi_client_configuration_t access_point = { 0 };
sl_net_ip_configuration_t ip_address        = { 0 };

static uint32_t wlan_app_event_map;

/*
 *********************************************************************************************************
 *                                               DATA TYPES
 *********************************************************************************************************
 */

extern void sl_wifi_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);
static sl_status_t show_scan_results();
void sl_wifi_app_set_event(uint32_t event_num);
extern uint8_t coex_ssid[50], pwd[34], sec_type;
uint8_t retry = 1;

uint8_t conn_status;
extern uint8_t magic_word;

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
void sl_wifi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(buffer);
  UNUSED_PARAMETER(length);

  // update wlan application state
  disconnected = 1;
  connected    = 0;

  sl_wifi_app_set_event(SL_WIFI_DISCONNECTED_STATE);
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
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(result_length);

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

void sl_wifi_app_task()
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
        LOG_PRINT("SL_WIFI_INITIAL_STATE\n");

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
        LOG_PRINT("SL_WIFI_UNCONNECTED_STATE\n");

        // Any additional code if required

        osSemaphoreRelease(wlan_thread_sem);
      } break;

      case SL_WIFI_SCAN_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_SCAN_STATE);

        sl_wifi_scan_configuration_t wifi_scan_configuration = { 0 };
        wifi_scan_configuration                              = default_wifi_scan_configuration;

        sl_wifi_set_scan_callback(wlan_app_scan_callback_handler, NULL);

        status = sl_wifi_start_scan(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, NULL, &wifi_scan_configuration);
        if (SL_STATUS_IN_PROGRESS == status) {
          LOG_PRINT("Scanning...\r\n");
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
        osSemaphoreRelease(wlan_thread_sem);
      } break;

      case SL_WIFI_JOIN_STATE: {
        sl_wifi_credential_t cred  = { 0 };
        sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
        memset(&access_point, 0, sizeof(sl_wifi_client_configuration_t));

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

          LOG_PRINT("SSID=%s\n", access_point.ssid.value);
          status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
        }
        if (status != RSI_SUCCESS) {
          timeout = 1;
          sl_wifi_app_send_to_ble(SL_WIFI_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
          sl_wifi_app_clear_event(SL_WIFI_JOIN_STATE);
          LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);

          // update wlan application state
          disconnected = 1;
          connected    = 0;
        } else {
          LOG_PRINT("\n WLAN connection is successful\n");
          // update wlan application state
          sl_wifi_app_clear_event(SL_WIFI_JOIN_STATE);
          sl_wifi_app_set_event(SL_WIFI_CONNECTED_STATE);
        }
        osSemaphoreRelease(wlan_thread_sem);
        LOG_PRINT("RSI_WLAN_JOIN_STATE\n");

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

        osSemaphoreRelease(wlan_thread_sem);
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

        osSemaphoreRelease(wlan_thread_sem);
        LOG_PRINT("SL_WIFI_CONNECTED_STATE\n");

      } break;

      case SL_WIFI_IPCONFIG_DONE_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_IPCONFIG_DONE_STATE);

        osSemaphoreRelease(wlan_thread_sem);
        LOG_PRINT("SL_WIFI_IPCONFIG_DONE_STATE\n");
      } break;

      case SL_WIFI_ERROR_STATE: {

      } break;

      case SL_WIFI_DISCONNECTED_STATE: {
        sl_wifi_app_clear_event(SL_WIFI_DISCONNECTED_STATE);
        retry = 1;
        sl_wifi_app_send_to_ble(SL_WIFI_DISCONNECTION_STATUS, (uint8_t *)&disconnected, 1);
        sl_wifi_app_set_event(SL_WIFI_FLASH_STATE);

        osSemaphoreRelease(wlan_thread_sem);
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
          disassosiated = 1;
          connected     = 0;
          sl_wifi_app_send_to_ble(SL_WIFI_DISCONNECTION_NOTIFY, (uint8_t *)&disassosiated, 1);
          sl_wifi_app_set_event(SL_WIFI_UNCONNECTED_STATE);
        } else {
          LOG_PRINT("\r\nWIFI Disconnect Failed, Error Code : 0x%lX\r\n", status);
        }

        osSemaphoreRelease(wlan_thread_sem);
        LOG_PRINT("SL_WIFI_DISCONN_NOTIFY_STATE\n");
      } break;
      case SL_WIFI_SOCKET_RECEIVE_STATE:
        break;
      case SL_WIFI_MQTT_INIT_DONE_STATE:
        break;
      case SL_WIFI_MQTT_SUBSCRIBE_DONE_STATE:
        break;
      case SL_BLE_GATT_WRITE_EVENT:
        break;
      case SL_WIFI_DATA_RECEIVE_STATE:
        break;
      case SL_SD_WRITE_STATE:
        break;
      case SL_WIFI_DEMO_COMPLETE_STATE:
        break;
      default:
        break;
    }
  }

  if (scan_result != NULL) {
    free(scan_result);
  }
}
