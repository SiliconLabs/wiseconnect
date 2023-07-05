/*******************************************************************************
 * @file
 * @brief Instant BG Scan Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"
#include <stdbool.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define WIFI_SCAN_TIMEOUT 10000

#define ADV_SCAN_THRESHOLD           -40
#define ADV_RSSI_TOLERANCE_THRESHOLD 5
#define ADV_ACTIVE_SCAN_DURATION     15
#define ADV_PASSIVE_SCAN_DURATION    20
#define ADV_MULTIPROBE               1
#define ADV_SCAN_PERIODICITY         10

/******************************************************
 *               Global Variable
 ******************************************************/

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
  .reserved   = 0,
};
volatile bool scan_results_complete  = false;
volatile sl_status_t callback_status = SL_STATUS_OK;

/******************************************************
 *               Function Declarations
 ******************************************************/

void application_start(const void *unused);
sl_status_t connect_client();
sl_status_t show_scan_results(sl_wifi_scan_result_t *scan_result);
sl_status_t scan_callback_handler(sl_wifi_event_t event,
                                  sl_wifi_scan_result_t *result,
                                  uint32_t result_length,
                                  void *arg);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

void application_start(const void *unused)
{
  sl_status_t status;
  status = connect_client();

  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while connecting to AP: 0x%lX\r\n", status);
    return;
  }

  printf("\r\nClient Connected Successfully\r\n");
}

sl_status_t connect_client()
{
  sl_wifi_scan_configuration_t wifi_scan_configuration              = { 0 };
  sl_wifi_interface_t interface                                     = SL_WIFI_CLIENT_2_4GHZ_INTERFACE;
  sl_wifi_advanced_scan_configuration_t advanced_scan_configuration = { 0 };

  sl_status_t status =
    sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &sl_wifi_default_client_configuration, NULL, NULL);
  VERIFY_STATUS_AND_RETURN(status);
  printf("Wi-Fi init done\r\n");

  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, 0);
  VERIFY_STATUS_AND_RETURN(status);
  printf("Wi-Fi client connected\r\n");

  advanced_scan_configuration.active_channel_time  = ADV_ACTIVE_SCAN_DURATION;
  advanced_scan_configuration.passive_channel_time = ADV_PASSIVE_SCAN_DURATION;
  advanced_scan_configuration.trigger_level        = ADV_SCAN_THRESHOLD;
  advanced_scan_configuration.trigger_level_change = ADV_RSSI_TOLERANCE_THRESHOLD;
  advanced_scan_configuration.enable_multi_probe   = ADV_MULTIPROBE;
  status = sl_wifi_set_advanced_scan_configuration(&advanced_scan_configuration);
  VERIFY_STATUS_AND_RETURN(status);

  wifi_scan_configuration.type                   = SL_WIFI_SCAN_TYPE_ADV_SCAN;
  wifi_scan_configuration.periodic_scan_interval = ADV_SCAN_PERIODICITY;
  sl_wifi_set_scan_callback(scan_callback_handler, NULL);
  status = sl_wifi_start_scan(interface, NULL, &wifi_scan_configuration);

  if (SL_STATUS_IN_PROGRESS == status) {
    printf("Scanning...\r\n");
    const uint32_t start = osKernelGetTickCount();

    while (!scan_results_complete && (osKernelGetTickCount() - start) <= WIFI_SCAN_TIMEOUT) {
      osThreadYield();
    }

    status = scan_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }
  return status;
}

sl_status_t show_scan_results(sl_wifi_scan_result_t *scan_result)
{
  ARGS_CHECK_NULL_POINTER(scan_result);
  printf("%ld scan results:\n", scan_result->scan_count);
  if (scan_result->scan_count)
    printf("\n   %s %24s %s", "SSID", "SECURITY", "NETWORK");
  printf("%12s %12s %s\n", "BSSID", "CHANNEL", "RSSI");
  for (int a = 0; a < scan_result->scan_count; ++a) {
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
  printf("End");

  return SL_STATUS_OK;
}

sl_status_t scan_callback_handler(sl_wifi_event_t event,
                                  sl_wifi_scan_result_t *result,
                                  uint32_t result_length,
                                  void *arg)
{
  if (CHECK_IF_EVENT_FAILED(event)) {
    callback_status       = *(sl_status_t *)result;
    scan_results_complete = true;
    return SL_STATUS_FAIL;
  }

  callback_status = show_scan_results(result);

  scan_results_complete = true;
  return SL_STATUS_OK;
}
