/*******************************************************************************
 * @file
 * @brief Wireless Roaming Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "errno.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_wifi_callback_framework.h"
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_m4_ps.h"
#include "sl_si91x_power_manager.h"
#endif

/******************************************************
  *                      Macros
  ******************************************************/
#define WIFI_SCAN_TIMEOUT 10000

#define ADV_SCAN_THRESHOLD           -40
#define ADV_RSSI_TOLERANCE_THRESHOLD 5
#define ADV_ACTIVE_SCAN_DURATION     15
#define ADV_PASSIVE_SCAN_DURATION    20
#define ADV_MULTIPROBE               1
#define ADV_SCAN_PERIODICITY         10
#define ENABLE_INSTANT_SCAN          1
#define ROAMING_THRESHOLD            -50
#define ROAMING_TRIGGER_DELTA_CHANGE 5

/* Defaults migrated from sl_wifi_filter_broadcast(5000, 1, 1). */
#define BCAST_FILTER_ENABLE      (1U)
#define MCAST_FILTER_ENABLE      (1U)
#define FILTER_MODE              (0U)
#define BEACON_DROP_THRESHOLD_MS (5000U)

/******************************************************
  *               Function Declarations
  ******************************************************/
static void application_start(void *argument);
sl_status_t send_data(void);
static sl_status_t module_status_handler(sl_wifi_event_t event,
                                         sl_status_t status_code,
                                         void *data,
                                         uint32_t data_length,
                                         void *arg);
void print_status_info(uint8_t state_code, uint8_t reason_code);
sl_status_t show_scan_results(sl_wifi_scan_result_t *scan_result);
sl_status_t scan_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  sl_wifi_scan_result_t *result,
                                  uint32_t result_length,
                                  void *arg);

/******************************************************
  *               Static Inline Functions
  ******************************************************/

/******************************************************
  *               Variable Definitions
  ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
};

static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_WIFI_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map = SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT,
                   .custom_feature_bit_map =
                     (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID | SL_WIFI_CUSTOM_FEAT_ASYNC_CONNECTION_STATUS
                      | SL_WIFI_CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULL_DATA),
                   .ext_custom_feature_bit_map =
                     (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK | MEMORY_CONFIG
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
};

volatile bool scan_results_complete  = false;
volatile sl_status_t callback_status = SL_STATUS_OK;

/******************************************************
  *               Function Definitions
  ******************************************************/
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  sl_wifi_performance_profile_v2_t performance_profile              = { .profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY };
  sl_wifi_scan_configuration_t wifi_scan_configuration              = { 0 };
  sl_wifi_advanced_scan_configuration_t advanced_scan_configuration = { 0 };
  sl_wifi_roam_configuration_t roam_configuration;

  //! Initialize the Wi-Fi client interface
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to start Wi-Fi Client interface: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client init success\r\n");

  //! Bring up the Wi-Fi client interface
  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to bring Wi-Fi client interface up: 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Wi-Fi client connected\r\n");

  sl_wifi_groupcast_filter_config_t groupcast_filter_config = { 0 };
  groupcast_filter_config.enable_bcast_filter               = (uint8_t)BCAST_FILTER_ENABLE;
  groupcast_filter_config.enable_mcast_filter               = (uint8_t)MCAST_FILTER_ENABLE;
  groupcast_filter_config.filter_mode                       = (uint8_t)FILTER_MODE;

  status = sl_wifi_set_groupcast_filter_config(&groupcast_filter_config);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_groupcast_filter_config failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  status = sl_wifi_set_beacon_drop_threshold(SL_WIFI_CLIENT_INTERFACE, (uint16_t)BEACON_DROP_THRESHOLD_MS);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_beacon_drop_threshold failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }

  //! set performance profile
  status = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Power save configuration Failed, Error Code : 0x%" PRIx32, (uint32_t)status);
    return;
  }
  SL_DEBUG_LOG_V2(INFO, "Power save configuration success");

  //! Register the module status handler
  sl_wifi_set_callback_v2(SL_WIFI_STATS_RESPONSE_EVENTS, module_status_handler, NULL);

  //! Set the advanced scan configuration
  advanced_scan_configuration.active_channel_time  = ADV_ACTIVE_SCAN_DURATION;
  advanced_scan_configuration.passive_channel_time = ADV_PASSIVE_SCAN_DURATION;
  advanced_scan_configuration.trigger_level        = ADV_SCAN_THRESHOLD;
  advanced_scan_configuration.trigger_level_change = ADV_RSSI_TOLERANCE_THRESHOLD;
  advanced_scan_configuration.enable_multi_probe   = ADV_MULTIPROBE;
  advanced_scan_configuration.enable_instant_scan  = ENABLE_INSTANT_SCAN;

  status = sl_wifi_set_advanced_scan_configuration(&advanced_scan_configuration);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "sl_wifi_set_advanced_scan_configuration failed with status 0x%" PRIx32, (uint32_t)status);
    return;
  }

  //! Register the scan callback handler
  wifi_scan_configuration.type                   = SL_WIFI_SCAN_TYPE_ADV_SCAN;
  wifi_scan_configuration.periodic_scan_interval = ADV_SCAN_PERIODICITY;
  scan_results_complete                          = false;
  callback_status                                = SL_STATUS_FAIL;

  sl_wifi_set_scan_callback_v2(scan_callback_handler, NULL);

  //! Set roaming configuration
  roam_configuration.trigger_level        = ROAMING_THRESHOLD;
  roam_configuration.trigger_level_change = ROAMING_TRIGGER_DELTA_CHANGE;

  status = sl_wifi_set_roam_configuration(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &roam_configuration);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "Roam failed with status 0x%" PRIx32, (uint32_t)status);
    return;
  } else {
    SL_DEBUG_LOG_V2(INFO, "Roaming configuration set successfully\r\n");
  }

  //! Start the scan
  status = sl_wifi_start_scan(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, NULL, &wifi_scan_configuration);
  if (SL_STATUS_IN_PROGRESS == status) {
    SL_DEBUG_LOG_V2(INFO, "Scanning...\r\n");
    const uint32_t start = osKernelGetTickCount();

    while (!scan_results_complete && (osKernelGetTickCount() - start) <= WIFI_SCAN_TIMEOUT) {
      osThreadYield();
    }

    status = scan_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }

  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "scan failed with status 0x%" PRIx32, (uint32_t)status);
    return;
  }

#ifdef SLI_SI91X_MCU_INTERFACE
  osSemaphoreId_t wait_semaphore;
  wait_semaphore = osSemaphoreNew(1, 0, NULL);
  if (wait_semaphore == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Failed to create semaphore\r\n");
    return;
  }
  // Waiting forever using semaphore to put M4 to sleep in tick less mode
  osSemaphoreAcquire(wait_semaphore, osWaitForever);
#else
  while (1) {
    osThreadYield();
  }
#endif
}

static sl_status_t module_status_handler(sl_wifi_event_t event,
                                         sl_status_t status_code,
                                         void *data,
                                         uint32_t data_length,
                                         void *arg)
{
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }

  sl_wifi_module_state_stats_response_t *notif = (sl_wifi_module_state_stats_response_t *)data;

  SL_DEBUG_LOG_V2(DEBUG, "---> Module status handler event with length : %" PRIu32, data_length);
  SL_DEBUG_LOG_V2(DEBUG,
                  "  <> Timestamp : %" PRIu32 ", state_code : 0x%02X, reason_code : 0x%02X.",
                  notif->timestamp,
                  notif->state_code,
                  notif->reason_code);
  SL_DEBUG_LOG_V2(DEBUG, "  <> channel : %u, rssi : -%u.", notif->channel, notif->rssi);
  char roam_bssid_notif_log[64];
  snprintf(roam_bssid_notif_log,
           sizeof(roam_bssid_notif_log),
           "  <> BSSID : %x:%x:%x:%x:%x:%x.",
           notif->bssid[0],
           notif->bssid[1],
           notif->bssid[2],
           notif->bssid[3],
           notif->bssid[4],
           notif->bssid[5]);
  SL_DEBUG_LOG_V2(DEBUG, "%s", (uintptr_t)roam_bssid_notif_log);

  //! Display the status information
  print_status_info(notif->state_code, notif->reason_code);

  return SL_STATUS_OK;
}

sl_status_t show_scan_results(sl_wifi_scan_result_t *scan_result)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(scan_result);
  uint8_t *bssid = NULL;
  SL_DEBUG_LOG_V2(INFO, "%ld scan results:\r\n", scan_result->scan_count);
  if (scan_result->scan_count) {
    SL_DEBUG_LOG_V2(INFO, "   %s %24s %s", (uintptr_t) "SSID", (uintptr_t) "SECURITY", (uintptr_t) "NETWORK");
  }
  SL_DEBUG_LOG_V2(INFO, "%12s %12s %s\r\n", (uintptr_t) "BSSID", (uintptr_t) "CHANNEL", (uintptr_t) "RSSI");
  for (uint32_t a = 0; a < scan_result->scan_count; ++a) {
    bssid = (uint8_t *)&scan_result->scan_info[a].bssid;
    SL_DEBUG_LOG_V2(INFO,
                    "%-24s %4u,  %4u, ",
                    (uintptr_t)scan_result->scan_info[a].ssid,
                    scan_result->scan_info[a].security_mode,
                    scan_result->scan_info[a].network_type);
    char roam_scan_bssid_log[48];
    snprintf(roam_scan_bssid_log,
             sizeof(roam_scan_bssid_log),
             "  %02x:%02x:%02x:%02x:%02x:%02x, ",
             bssid[0],
             bssid[1],
             bssid[2],
             bssid[3],
             bssid[4],
             bssid[5]);
    SL_DEBUG_LOG_V2(INFO, "%s", (uintptr_t)roam_scan_bssid_log);
    SL_DEBUG_LOG_V2(INFO, "%4u,  -%u\r\n", scan_result->scan_info[a].rf_channel, scan_result->scan_info[a].rssi_val);
  }

  return SL_STATUS_OK;
}

sl_status_t scan_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  sl_wifi_scan_result_t *result,
                                  uint32_t result_length,
                                  void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status       = status_code;
    scan_results_complete = true;
    return status_code;
  }

  if (result_length != 0) {
    callback_status = show_scan_results(result);
  }

  scan_results_complete = true;
  return SL_STATUS_OK;
}

void print_status_info(uint8_t state_code, uint8_t reason_code)
{
  /**
   * Module State Code (Upper Nibble) Description:
   * 0x00 - Startup (Initial state or idle state)
   * 0x10 - Beacon Loss
   * 0x20 - De-authentication from AP
   * 0x50 - Current AP is best
   * 0x60 - Better AP found while roaming
   * 0x70 - No AP found
   * 0x80 - Associated or joined to an Access point
   * 0x90 - Unassociated (Disconnected from host or join failure)
   * 
   */
  switch (state_code & 0xF0) {
    case 0x00:
      SL_DEBUG_LOG_V2(DEBUG, "State: Startup (Initial state or idle state)\r\n");
      break;
    case 0x10:
      SL_DEBUG_LOG_V2(DEBUG, "State: Beacon Loss\r\n");
      break;
    case 0x20:
      SL_DEBUG_LOG_V2(DEBUG, "State: De-authentication from AP\r\n");
      break;
    case 0x50:
      SL_DEBUG_LOG_V2(DEBUG, "State: Current AP is best\r\n");
      break;
    case 0x60:
      SL_DEBUG_LOG_V2(DEBUG, "State: Better AP found while roaming\r\n");
      break;
    case 0x70:
      SL_DEBUG_LOG_V2(DEBUG, "State: No AP found\r\n");
      break;
    case 0x80:
      SL_DEBUG_LOG_V2(DEBUG, "State: Associated or joined to an Access point\r\n");
      break;
    case 0x90:
      SL_DEBUG_LOG_V2(DEBUG, "State: Unassociated (Disconnected from host or join failure)\r\n");
      break;
    default:
      SL_DEBUG_LOG_V2(DEBUG, "State: Unknown state code\r\n");
      break;
  }

  /**
   * Reason Code Details:
   * 0x00 - No reason specified (Initial state or idle state)
   * 0x01 - No response from AP for authentication request (Authentication denial)
   * 0x02 - Association denial (Association timeout or failure due to unknown reasons)
   * 0x03 - User-configured AP is not present
   * 0x05 - Four-way Handshake failure
   * 0x06 - Deauthentication from user
   * 0x07 - PSK not configured
   * 0x08 - Key-handshake failure during rejoin/roaming/after connection (Disconnection from supplicant)
   * 0x09 - Roaming not enabled
   * 0x10 - Beacon Loss (failover Roam)
   * 0x20 - De-authentication from AP
   * 0x28 - TLS CA Cert not present
   * 0x29 - TLS PRIVATE key not present
   * 0x2A - TLS Client Cert not present
   * 0x2B - TLS no Cert present
   * 0x2C - PEAP CA Cert not present
   * 0x2D - Server Cert Invalid Key Type
   * 0x2E - Server Intermediate CA Invalid Key Type
   * 0x2F - Server Root CA Invalid Key Type
   * 0x30 - Client Cert Invalid Key Type
   * 0x31 - Client Root CA Invalid Key Type
   * 0x37 - Server Cert 4096-bit length support is not enabled
   * 0x38 - Server Intermediate CA 4096-bit length support is not enabled
   * 0x39 - Server Root CA 4096-bit length support is not enabled
   * 0x3A - Client Cert 4096-bit length support is not enabled
   * 0x3B - Client Root CA 4096-bit length support is not enabled
   * 0x3C - Server Cert Invalid Sign Alg
   * 0x3D - Server Intermediate CA Invalid Sign Alg
   * 0x3E - Server Root CA Invalid Sign Length
   * 0x3F - Client Cert Invalid Sign Alg
   * 0x40 - Client Root CA Invalid Sign Length
   * 0x41 - Server Intermediate CA not Present
   * 0x42 - Server Root CA Parse Error
   * 0x43 - Server Intermediate Root CA Parse Error
   * 0x44 - Server Cert Parse Error
   * 0x45 - Client Cert Parse Error
   * 0x46 - Incorrect Private Key Password
   * 0x47 - EAP Failure Received
   * 0x48 - Client Cert Bad Date Error
   * 0x49 - Server Cert Bad Date Error
   * 0x4A - Server Root CA Bad Date Error
   * 0x4B - Client Root CA Bad Date Error
   * 0x4C - Server Intermediate Root CA Bad Date Error
   * 0x4D - Pem Header Error
   * 0x4E - Pem Footer Error
   * 0x4F - Client Intermediate CA Invalid Sign Length
   * 0x50 - Client Intermediate CA Invalid Length
   * 0x52 - Client Intermediate CA invalid Key Type
   * 0x53 - Pem Error
   * 0x54 - Pathlen certificate is Invalid
   * 
   */
  switch (reason_code) {
    case 0x00:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Initial state or idle state\r\n");
      break;
    case 0x01:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: No response from AP for authentication request (Authentication denial)\r\n");
      break;
    case 0x02:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Association denial (Association timeout or failure)\r\n");
      break;
    case 0x03:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: User-configured AP is not present\r\n");
      break;
    case 0x05:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Four-way Handshake failure\r\n");
      break;
    case 0x06:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Deauthentication from user\r\n");
      break;
    case 0x07:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: PSK not configured\r\n");
      break;
    case 0x08:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Key-handshake failure during rejoin/roaming/after connection\r\n");
      break;
    case 0x09:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Roaming not enabled\r\n");
      break;
    case 0x10:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Beacon Loss (failover Roam)\r\n");
      break;
    case 0x20:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: De-authentication from AP\r\n");
      break;
    case 0x28:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: TLS CA Cert not present\r\n");
      break;
    case 0x29:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: TLS PRIVATE key not present\r\n");
      break;
    case 0x2A:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: TLS Client Cert not present\r\n");
      break;
    case 0x2B:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: TLS no Cert present\r\n");
      break;
    case 0x2C:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: PEAP CA Cert not present\r\n");
      break;
    case 0x2D:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Cert Invalid Key Type\r\n");
      break;
    case 0x2E:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate CA Invalid Key Type\r\n");
      break;
    case 0x2F:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Root CA Invalid Key Type\r\n");
      break;
    case 0x30:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Cert Invalid Key Type\r\n");
      break;
    case 0x31:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Root CA Invalid Key Type\r\n");
      break;
    case 0x37:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Cert 4096-bit length support is not enabled\r\n");
      break;
    case 0x38:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate CA 4096-bit length support is not enabled\r\n");
      break;
    case 0x39:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Root CA 4096-bit length support is not enabled\r\n");
      break;
    case 0x3A:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Cert 4096-bit length support is not enabled\r\n");
      break;
    case 0x3B:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Root CA 4096-bit length support is not enabled\r\n");
      break;
    case 0x3C:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Cert Invalid Sign Alg\r\n");
      break;
    case 0x3D:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate CA Invalid Sign Alg\r\n");
      break;
    case 0x3E:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Root CA Invalid Sign Length\r\n");
      break;
    case 0x3F:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Cert Invalid Sign Alg\r\n");
      break;
    case 0x40:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Root CA Invalid Sign Length\r\n");
      break;
    case 0x41:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate CA not Present\r\n");
      break;
    case 0x42:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Root CA Parse Error\r\n");
      break;
    case 0x43:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate Root CA Parse Error\r\n");
      break;
    case 0x44:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Cert Parse Error\r\n");
      break;
    case 0x45:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Cert Parse Error\r\n");
      break;
    case 0x46:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Incorrect Private Key Password\r\n");
      break;
    case 0x47:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: EAP Failure Received\r\n");
      break;
    case 0x48:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Cert Bad Date Error\r\n");
      break;
    case 0x49:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Cert Bad Date Error\r\n");
      break;
    case 0x4A:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Root CA Bad Date Error\r\n");
      break;
    case 0x4B:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Root CA Bad Date Error\r\n");
      break;
    case 0x4C:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Server Intermediate Root CA Bad Date Error\r\n");
      break;
    case 0x4D:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Pem Header Error\r\n");
      break;
    case 0x4E:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Pem Footer Error\r\n");
      break;
    case 0x4F:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Intermediate CA Invalid Sign Length\r\n");
      break;
    case 0x50:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Intermediate CA Invalid Length\r\n");
      break;
    case 0x52:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Client Intermediate CA invalid Key Type\r\n");
      break;
    case 0x53:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Pem Error\r\n");
      break;
    case 0x54:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Pathlen certificate is Invalid\r\n");
      break;
    default:
      SL_DEBUG_LOG_V2(DEBUG, "Reason: Unknown reason code\r\n");
      break;
  }
}
