/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_driver.h"
#include "sl_net_constants.h"
#include "sl_wifi_constants.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_wifi_types.h"
#include "sl_rsi_utility.h"
#include <string.h>
#include "cmsis_os2.h" // CMSIS RTOS2
#include "sl_si91x_types.h"

// Macro to check the status and return it if it's not SL_STATUS_OK
#define VERIFY_STATUS(s)   \
  do {                     \
    if (s != SL_STATUS_OK) \
      return s;            \
  } while (0)

osThreadId_t si91x_thread           = 0;
osThreadId_t si91x_event_thread     = 0;
osEventFlagsId_t si91x_events       = 0;
osEventFlagsId_t si91x_bus_events   = 0;
osEventFlagsId_t si91x_async_events = 0;
osMutexId_t malloc_free_mutex       = 0;
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
osMutexId_t side_band_crypto_mutex = 0;
#endif

si91x_packet_queue_t cmd_queues[SI91X_QUEUE_MAX];

static bool sli_si91x_packet_status = 0;

// Declaration of external functions
extern void si91x_bus_thread(void *args);
extern void si91x_event_handler_thread(void *args);
sl_status_t sl_si91x_host_power_cycle(void);
void convert_performance_profile_to_power_save_command(sl_si91x_performance_profile_t profile,
                                                       sl_si91x_power_save_request_t *power_save_request);
extern sl_status_t sl_create_generic_rx_packet_from_params(sl_si91x_queue_packet_t **queue_packet,
                                                           sl_wifi_buffer_t **packet_buffer,
                                                           uint16_t packet_id,
                                                           uint8_t flags,
                                                           void *sdk_context,
                                                           sl_si91x_command_type_t command_type);
void sl_debug_log(const char *format, ...);

extern sli_si91x_performance_profile_t performance_profile;

// NOTE: Boolean value determines whether firmware automatically closes the TCP socket in case of receiving termination from remote node or not.
static bool tcp_auto_close_enabled;

// This value will be used in connect request/ AP configurations to set the TX power of the module
static sl_wifi_max_tx_power_t wifi_max_tx_power = {
  .scan_tx_power = 0x1f, //Default power value set to max value supported in dBm
  .join_tx_power = 0x1f, //Default power value set to max value supported in dBm
};

static sl_wifi_rate_t saved_wifi_data_rate = SL_WIFI_AUTO_RATE;

static sl_wifi_ap_configuration_t ap_configuration;

static bool is_card_ready_required = true;

sl_si91x_performance_profile_t current_performance_profile = HIGH_PERFORMANCE;

static sl_si91x_boot_configuration_t saved_boot_configuration = { 0 };

static sl_si91x_coex_mode_t coex_mode = 0;

void save_wifi_current_performance_profile(const sl_wifi_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(&performance_profile.wifi_performance_profile, profile, sizeof(sl_wifi_performance_profile_t));

  performance_profile.coex_mode = get_coex_mode();
}

// Get the current Wi-Fi performance profile
void get_wifi_current_performance_profile(sl_wifi_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(profile, &performance_profile.wifi_performance_profile, sizeof(sl_wifi_performance_profile_t));
}

// Get the coexistence performance profile based on the current coexistence mode
void get_coex_performance_profile(sl_si91x_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  uint8_t mode_decision = 0;
  if (performance_profile.coex_mode == 0) { // Enabling power save checks for SL_SI91X_WLAN_ONLY_MODE
    performance_profile.coex_mode |= BIT(0);
  }
  // Determine the mode decision based on the coexistence mode
  switch (performance_profile.coex_mode) {
    case SL_SI91X_WLAN_MODE: {
      // Wi-Fi only mode
      mode_decision = ((performance_profile.wifi_performance_profile.profile << 4)
                       | (performance_profile.wifi_performance_profile.profile));
    } break;
    case SL_SI91X_BLUETOOTH_MODE:
    case SL_SI91X_BLE_MODE:
    case SL_SI91X_DUAL_MODE: {
      // Bluetooth only or dual-mode (BT + Wi-Fi) mode
      mode_decision = ((performance_profile.bt_performance_profile.profile << 4)
                       | (performance_profile.bt_performance_profile.profile));
    } break;
    case SL_SI91X_WLAN_BLUETOOTH_MODE:
    case SL_SI91X_WLAN_DUAL_MODE:
    case SL_SI91X_WLAN_BLE_MODE: {
      // Wi-Fi + Bluetooth mode
      mode_decision = ((performance_profile.wifi_performance_profile.profile << 4)
                       | (performance_profile.bt_performance_profile.profile));
    } break;
    default:
      break;
  }

  // Determine the performance profile based on the mode decision
  switch (mode_decision) {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x10:
    case 0x20:
    case 0x30:
    case 0x40: {
      *profile = HIGH_PERFORMANCE; // High performance mode
    } break;
    case 0x11:
    case 0x12:
    case 0x31:
    case 0x13:
    case 0x14:
    case 0x41: {
      *profile = ASSOCIATED_POWER_SAVE; // Power save mode
    } break;
    case 0x22:
    case 0x21:
    case 0x32:
    case 0x23:
    case 0x42:
    case 0x24: {
      *profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY; // Low latency power save mode
    } break;
    case 0x33: {
      *profile = STANDBY_POWER_SAVE; // Power save mode
    } break;
    case 0x44: {
      *profile = STANDBY_POWER_SAVE_WITH_RAM_RETENTION; // Power save mode with RAM retention
    } break;
    default: {
      // Do nothing
    } break;
  }
  if (performance_profile.coex_mode == 1) { // clearing the BIT(0) of coex_mode
    performance_profile.coex_mode &= ~BIT(0);
  }
  return;
}

void reset_coex_current_performance_profile(void)
{
  memset(&performance_profile, 0, sizeof(sli_si91x_performance_profile_t));
}

void save_boot_configuration(const sl_si91x_boot_configuration_t *boot_configuration)
{
  memcpy(&saved_boot_configuration, boot_configuration, sizeof(sl_si91x_boot_configuration_t));
}

void get_saved_boot_configuration(sl_si91x_boot_configuration_t *boot_configuration)
{
  memcpy(boot_configuration, &saved_boot_configuration, sizeof(sl_si91x_boot_configuration_t));
}

void save_bt_current_performance_profile(const sl_bt_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(&performance_profile.bt_performance_profile, profile, sizeof(sl_bt_performance_profile_t));
}

void get_bt_current_performance_profile(sl_bt_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(profile, &performance_profile.bt_performance_profile, sizeof(sl_bt_performance_profile_t));
}

void save_tcp_auto_close_choice(bool is_tcp_auto_close_enabled)
{
  tcp_auto_close_enabled = is_tcp_auto_close_enabled;
}

bool is_tcp_auto_close_enabled()
{
  return tcp_auto_close_enabled;
}

void save_max_tx_power(uint8_t max_scan_tx_power, uint8_t max_join_tx_power)
{
  wifi_max_tx_power.scan_tx_power = max_scan_tx_power;
  wifi_max_tx_power.join_tx_power = max_join_tx_power;
}

sl_wifi_max_tx_power_t get_max_tx_power()
{
  return wifi_max_tx_power;
}

void reset_max_tx_power()
{
  wifi_max_tx_power.scan_tx_power = 0x1f;
  wifi_max_tx_power.join_tx_power = 0x1f;
}

void set_card_ready_required(bool card_ready_required)
{
  is_card_ready_required = card_ready_required;
}

bool get_card_ready_required()
{
  return is_card_ready_required;
}

void save_coex_mode(sl_si91x_coex_mode_t mode)
{
  coex_mode = mode;
}

sl_si91x_coex_mode_t get_coex_mode(void)
{
  return coex_mode;
}

sl_status_t convert_si91x_wifi_client_info(sl_wifi_client_info_response_t *client_info_response,
                                           sl_si91x_client_info_response *sl_si91x_client_info_response)
{

  SL_WIFI_ARGS_CHECK_NULL_POINTER(sl_si91x_client_info_response);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(client_info_response);

  client_info_response->client_count = sl_si91x_client_info_response->sta_count[0]
                                       | sl_si91x_client_info_response->sta_count[1] << 8;

  for (uint8_t station_index = 0; station_index < client_info_response->client_count; station_index++) {
    uint8_t *si91x_ip_address, *sl_ip_address;

    sl_wifi_client_info_t *sl_client_info      = &client_info_response->client_info[station_index];
    sl_si91x_station_info_t *si91x_client_info = &sl_si91x_client_info_response->sta_info[station_index];

    uint8_t ip_address_size = si91x_client_info->ip_version[0] | si91x_client_info->ip_version[1] << 8;

    si91x_ip_address = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? si91x_client_info->ip_address.ipv4_address
                                                                 : si91x_client_info->ip_address.ipv6_address;
    sl_ip_address    = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? sl_client_info->ip_address.ip.v4.bytes
                                                                 : sl_client_info->ip_address.ip.v6.bytes;

    sl_client_info->ip_address.type = ip_address_size == SL_IPV4_ADDRESS_LENGTH ? SL_IPV4 : SL_IPV6;

    memcpy(&sl_client_info->mac_adddress, si91x_client_info->mac, sizeof(sl_mac_address_t));
    memcpy(sl_ip_address, si91x_ip_address, ip_address_size);
  }

  return SL_STATUS_OK;
}

sl_wifi_event_t convert_si91x_event_to_sl_wifi_event(rsi_wlan_cmd_response_t command, uint16_t frame_status)
{
  // Define a constant indicating a fail indication event
  const sl_wifi_event_t fail_indication = (frame_status != RSI_SUCCESS) ? SL_WIFI_EVENT_FAIL_INDICATION : 0;

  // Switch-case to map SI91x events to SL Wi-Fi events
  switch (command) {
    case RSI_WLAN_RSP_BG_SCAN:
    case RSI_WLAN_RSP_SCAN:
      return SL_WIFI_SCAN_RESULT_EVENT | fail_indication;
    case RSI_WLAN_RSP_JOIN:
      return SL_WIFI_JOIN_EVENT | fail_indication;
    case RSI_WLAN_RSP_GET_STATS:
      if (frame_status != RSI_SUCCESS) {
        return SL_WIFI_STATS_RESPONSE_EVENTS | fail_indication;
      }
      return SL_WIFI_STATS_EVENT;
    case RSI_WLAN_RSP_RX_STATS:
      if (frame_status != RSI_SUCCESS) {
        return SL_WIFI_STATS_RESPONSE_EVENTS | fail_indication;
      }
      return SL_WIFI_STATS_AYSNC_EVENT;
    case RSI_WLAN_RATE_RSP_STATS:
      if (frame_status != RSI_SUCCESS) {
        return SL_WIFI_STATS_RESPONSE_EVENTS | fail_indication;
      }
      return SL_WIFI_STATS_TEST_MODE_EVENT;
    case RSI_WLAN_RSP_EXT_STATS:
      if (frame_status != RSI_SUCCESS) {
        return SL_WIFI_STATS_RESPONSE_EVENTS | fail_indication;
      }
      return SL_WIFI_STATS_ADVANCE_EVENT;
    case RSI_WLAN_RSP_MODULE_STATE:
      if (frame_status != RSI_SUCCESS) {
        return SL_WIFI_STATS_RESPONSE_EVENTS | fail_indication;
      }
      return SL_WIFI_STATS_MODULE_STATE_EVENT;
    case RSI_WLAN_RSP_HTTP_OTAF:
      return SL_WIFI_HTTP_OTA_FW_UPDATE_EVENT | fail_indication;
    case RSI_WLAN_RSP_CLIENT_CONNECTED:
      return SL_WIFI_CLIENT_CONNECTED_EVENT | fail_indication;
    case RSI_WLAN_RSP_CLIENT_DISCONNECTED:
      return SL_WIFI_CLIENT_DISCONNECTED_EVENT | fail_indication;
    case RSI_WLAN_RSP_TWT_ASYNC:
      switch (frame_status) {
        case RSI_SUCCESS:
          return SL_WIFI_TWT_RESPONSE_EVENT;
        default:
          return SL_WIFI_TWT_RESPONSE_EVENT | (frame_status << 16);
      }
    case RSI_WLAN_RSP_TRANSCEIVER_TX_DATA_STATUS:
      return SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB | fail_indication;
    case SL_SI91X_WIFI_RX_DOT11_DATA:
      return SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB | fail_indication;
    default:
      return SL_WIFI_INVALID_EVENT;
  }
}

sl_status_t save_sl_wifi_rate(sl_wifi_rate_t transfer_rate)
{
  // Check if the provided transfer rate is valid
  if (transfer_rate == SL_WIFI_RATE_INVALID) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  saved_wifi_data_rate = transfer_rate;

  return SL_STATUS_OK;
}

sl_status_t get_saved_sl_wifi_rate(sl_wifi_rate_t *transfer_rate)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(transfer_rate);

  *transfer_rate = saved_wifi_data_rate;
  return SL_STATUS_OK;
}

void reset_sl_wifi_rate()
{
  saved_wifi_data_rate = 0;
}

sl_status_t save_ap_configuration(const sl_wifi_ap_configuration_t *wifi_ap_configuration)
{
  // Check if the input pointer is valid
  SL_WIFI_ARGS_CHECK_NULL_POINTER(wifi_ap_configuration);
  ap_configuration = *wifi_ap_configuration;

  return SL_STATUS_OK;
}

sl_status_t get_saved_ap_configuration(sl_wifi_ap_configuration_t *wifi_ap_configuration)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(wifi_ap_configuration);
  *wifi_ap_configuration = ap_configuration;

  return SL_STATUS_OK;
}

void reset_ap_configuration()
{
  // Initialize the ap_configuration structure with zeros
  ap_configuration = (sl_wifi_ap_configuration_t){ 0 };
}

sl_status_t get_rate_protocol_and_data_rate(const uint8_t data_rate,
                                            sl_wifi_rate_protocol_t *rate_protocol,
                                            sl_wifi_rate_t *mask)
{
  // Map the provided data_rate to a rate_protocol and set the mask accordingly
  switch (data_rate) {
    case SL_WIFI_AUTO_RATE: {
      *rate_protocol = SL_WIFI_RATE_PROTOCOL_AUTO;
      break;
    }
    case SL_WIFI_RATE_11B_1:
    case SL_WIFI_RATE_11B_2:
    case SL_WIFI_RATE_11B_5_5:
    case SL_WIFI_RATE_11B_11: {
      *rate_protocol = SL_WIFI_RATE_PROTOCOL_B_ONLY;
      break;
    }
    case SL_WIFI_RATE_11G_6:
    case SL_WIFI_RATE_11G_9:
    case SL_WIFI_RATE_11G_12:
    case SL_WIFI_RATE_11G_18:
    case SL_WIFI_RATE_11G_24:
    case SL_WIFI_RATE_11G_36:
    case SL_WIFI_RATE_11G_48:
    case SL_WIFI_RATE_11G_54: {
      *rate_protocol = SL_WIFI_RATE_PROTOCOL_G_ONLY;
      break;
    }
    case SL_WIFI_RATE_11N_MCS0:
    case SL_WIFI_RATE_11N_MCS1:
    case SL_WIFI_RATE_11N_MCS2:
    case SL_WIFI_RATE_11N_MCS3:
    case SL_WIFI_RATE_11N_MCS4:
    case SL_WIFI_RATE_11N_MCS5:
    case SL_WIFI_RATE_11N_MCS6:
    case SL_WIFI_RATE_11N_MCS7: {
      *rate_protocol = SL_WIFI_RATE_PROTOCOL_N_ONLY;
      break;
    }
    case SL_WIFI_RATE_11AX_MCS0:
    case SL_WIFI_RATE_11AX_MCS1:
    case SL_WIFI_RATE_11AX_MCS2:
    case SL_WIFI_RATE_11AX_MCS3:
    case SL_WIFI_RATE_11AX_MCS4:
    case SL_WIFI_RATE_11AX_MCS5:
    case SL_WIFI_RATE_11AX_MCS6:
    case SL_WIFI_RATE_11AX_MCS7: {
      *rate_protocol = SL_WIFI_RATE_PROTOCOL_AX_ONLY;
      break;
    }
    default: {
      return SL_STATUS_INVALID_CONFIGURATION;
    }
  }

  *mask = data_rate;

  return SL_STATUS_OK;
}

void convert_performance_profile_to_power_save_command(sl_si91x_performance_profile_t profile,
                                                       sl_si91x_power_save_request_t *power_save_request)
{
  SL_ASSERT(power_save_request != NULL);
  if (performance_profile.wifi_performance_profile.monitor_interval) {
    power_save_request->monitor_interval = performance_profile.wifi_performance_profile.monitor_interval;
  } else {
    power_save_request->monitor_interval = DEFAULT_MONITOR_INTERVAL;
  }

  power_save_request->ulp_mode_enable   = SL_ULP_WITH_RAM_RETENTION;
  power_save_request->dtim_aligned_type = performance_profile.wifi_performance_profile.dtim_aligned_type;
  power_save_request->num_of_dtim_skip  = performance_profile.wifi_performance_profile.num_of_dtim_skip;
  power_save_request->listen_interval   = performance_profile.wifi_performance_profile.listen_interval;
  power_save_request->psp_type          = SL_MAX_PSP;

  // Depending on the specified performance profile, configure the power_save_request structure
  switch (profile) {
    case HIGH_PERFORMANCE: {
      // For HIGH_PERFORMANCE profile, reset all fields in the power_save_request structure to zero
      memset(power_save_request, 0, sizeof(sl_si91x_power_save_request_t));
      break;
    }

    case ASSOCIATED_POWER_SAVE: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SL_CONNECTED_M4_BASED_PS;
#else
      power_save_request->power_mode = SL_CONNECTED_GPIO_BASED_PS;
#endif
      break;
    }

    case ASSOCIATED_POWER_SAVE_LOW_LATENCY: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SL_CONNECTED_M4_BASED_PS;
#else
      power_save_request->power_mode = SL_CONNECTED_GPIO_BASED_PS;
#endif
      power_save_request->psp_type = SL_FAST_PSP;
      break;
    }

    case STANDBY_POWER_SAVE: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SL_M4_BASED_DEEP_SLEEP;
#else
      power_save_request->power_mode = SL_GPIO_BASED_DEEP_SLEEP;
#endif
      power_save_request->ulp_mode_enable = SL_ULP_WITHOUT_RAM_RET_RETENTION;
      break;
    }

    case STANDBY_POWER_SAVE_WITH_RAM_RETENTION: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SL_M4_BASED_DEEP_SLEEP;
#else
      power_save_request->power_mode = SL_GPIO_BASED_DEEP_SLEEP;
#endif
      break;
    }
    default: {
      // Do nothing
    } break;
  }

  return;
}

/*********************************************************************************************
 * @brief
 * 	 An utility function powersave request to TA firmware. Host has to disable powersave before sending powersave mode
 * @param profile
 *   performance profile which needs to be converted to its equivalent si91x_power_save_request structure and send to TA
 * @return
 * 	 sl_status_t
 *********************************************************************************************/

sl_status_t sl_si91x_send_power_save_request(sl_si91x_performance_profile_t profile)
{
  sl_status_t status;
  sl_si91x_power_save_request_t power_save_request = { 0 };
  // Set power save mode to disable first
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_PWRMODE,
                                        SI91X_COMMON_CMD_QUEUE,
                                        &power_save_request,
                                        sizeof(sl_si91x_power_save_request_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  // Power save need not to be sent if the current_performance_profile is HIGH_PERFORMANCE
  if (profile == HIGH_PERFORMANCE) {
    return SL_STATUS_OK;
  }

  // Convert the performance profile to a power save command and send it to the SI91x module.
  convert_performance_profile_to_power_save_command(profile, &power_save_request);

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_PWRMODE,
                                        SI91X_COMMON_CMD_QUEUE,
                                        &power_save_request,
                                        sizeof(sl_si91x_power_save_request_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(3000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

//In Access point mode TA only supports No Encryption, TKIP and CCMP encryptions.
sl_status_t convert_sl_wifi_to_sl_si91x_encryption(sl_wifi_encryption_t encryption_mode, uint8_t *encryption_request)
{
  switch (encryption_mode) {
    case SL_WIFI_NO_ENCRYPTION:
      *encryption_request = SL_SI91X_NO_ENCRYPTION;
      break;
    case SL_WIFI_TKIP_ENCRYPTION:
      *encryption_request = SL_SI91X_TKIP_ENCRYPTION;
      break;
    case SL_WIFI_CCMP_ENCRYPTION:
      *encryption_request = SL_SI91X_CCMP_ENCRYPTION;
      break;
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_platform_init(void)
{
  int i;
  sl_status_t status = SL_STATUS_OK;

  // Initialize the command queues
  memset(cmd_queues, 0, sizeof(cmd_queues));

  // Create event flags
  if (NULL == si91x_events) {
    si91x_events = osEventFlagsNew(NULL);
  }

  if (NULL == si91x_bus_events) {
    si91x_bus_events = osEventFlagsNew(NULL);
  }

  if (NULL == si91x_async_events) {
    si91x_async_events = osEventFlagsNew(NULL);
  }

  // Create and start SI91X bus thread
  if (NULL == si91x_thread) {
    const osThreadAttr_t attr = {

      .name       = "si91x_bus",
      .priority   = osPriorityRealtime,
      .stack_mem  = 0,
      .stack_size = 1636,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };
    si91x_thread = osThreadNew(si91x_bus_thread, NULL, &attr);
  }

  // Create and start SI91X event handler thread
  if (NULL == si91x_event_thread) {
    const osThreadAttr_t attr = {
      .name       = "si91x_event",
      .priority   = osPriorityRealtime1,
      .stack_mem  = 0,
      .stack_size = 1536,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };
    si91x_event_thread = osThreadNew(si91x_event_handler_thread, NULL, &attr);
  }

  // Initialize command queues and associated mutexes
  for (i = 0; i < SI91X_QUEUE_MAX; i++) {
    cmd_queues[i].head                = NULL;
    cmd_queues[i].tail                = NULL;
    cmd_queues[i].mutex               = osMutexNew(NULL);
    cmd_queues[i].flag                = (1 << i);
    cmd_queues[i].queued_packet_count = 0;
  }

  // Create malloc/free mutex
  malloc_free_mutex = osMutexNew(NULL);

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  // Create side_band_crypto_mutex mutex
  side_band_crypto_mutex = osMutexNew(NULL);
#endif

  return status;
}

sl_status_t sl_si91x_platform_deinit(void)
{
  int i;

  // Deallocate all threads, mutexes and event handlers

  // Terminate SI91X bus thread
  if (NULL != si91x_thread) {
    osThreadTerminate(si91x_thread);
    si91x_thread = NULL;
  }

  // Terminate SI91X event handler thread
  if (NULL != si91x_event_thread) {
    osThreadTerminate(si91x_event_thread);
    si91x_event_thread = NULL;
  }

  // Delete event flags
  if (NULL != si91x_events) {
    osEventFlagsDelete(si91x_events);
    si91x_events = NULL;
  }

  if (NULL != si91x_bus_events) {
    osEventFlagsDelete(si91x_bus_events);
    si91x_bus_events = NULL;
  }

  if (NULL != si91x_async_events) {
    osEventFlagsDelete(si91x_async_events);
    si91x_async_events = NULL;
  }

  // Delete command queue mutexes
  for (i = 0; i < SI91X_QUEUE_MAX; i++) {
    osMutexDelete(cmd_queues[i].mutex);
    cmd_queues[i].mutex = NULL;
  }

  // Delete malloc/free mutex
  osMutexDelete(malloc_free_mutex);
  malloc_free_mutex = NULL;
  return SL_STATUS_OK;
}

sl_si91x_host_timestamp_t sl_si91x_host_get_timestamp(void)
{
  return osKernelGetTickCount();
}

// Calculate elapsed time from the given starting timestamp
sl_si91x_host_timestamp_t sl_si91x_host_elapsed_time(uint32_t starting_timestamp)
{
  return (osKernelGetTickCount() - starting_timestamp);
}

// Delay execution for a specified number of milliseconds using an OS-level delay
void sl_si91x_host_delay_ms(uint32_t delay_milliseconds)
{
  osDelay(delay_milliseconds);
}

void sl_si91x_host_set_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_events, event_mask);
}

void sl_si91x_host_set_bus_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_bus_events, event_mask);
}

void sl_si91x_host_set_async_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_async_events, event_mask);
}

sl_status_t sl_si91x_host_add_to_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t *buffer)
{
  sl_wifi_buffer_t *packet = (sl_wifi_buffer_t *)buffer;
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);
  packet->node.node = NULL;

  if (cmd_queues[queue].tail == NULL) {
    // The queue is empty, set both head and tail to the new packet
    cmd_queues[queue].head = packet;
    cmd_queues[queue].tail = packet;
  } else {
    // Add the packet to the end of the queue and update the tail
    cmd_queues[queue].tail->node.node = (sl_slist_node_t *)packet;
    cmd_queues[queue].tail            = packet;
  }
  cmd_queues[queue].queued_packet_count++;
  osMutexRelease(cmd_queues[queue].mutex);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_add_to_queue_with_atomic_action(sl_si91x_queue_type_t queue,
                                                          sl_wifi_buffer_t *buffer,
                                                          void *user_data,
                                                          sl_si91x_host_atomic_action_function_t handler)
{
  sl_wifi_buffer_t *packet = (sl_wifi_buffer_t *)buffer;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL); // Acquire the mutex with a specified timeout
  if (NULL != handler) {
    handler(user_data); // Perform an atomic action with user data
  }
  packet->node.node = NULL;

  if (cmd_queues[queue].tail == NULL) {
    // The queue is empty, set both head and tail to the new packet
    cmd_queues[queue].head = packet;
    cmd_queues[queue].tail = packet;
  } else {
    // Add the packet to the end of the queue and update the tail
    cmd_queues[queue].tail->node.node = (sl_slist_node_t *)packet;
    cmd_queues[queue].tail            = packet;
  }
  // This flag is typically used to track the number of commands currently
  // enqueued or in progress for the specific queue type.
  cmd_queues[queue].queued_packet_count++;

  osMutexRelease(cmd_queues[queue].mutex); // Release the mutex
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_remove_from_queue(sl_si91x_queue_type_t queue, sl_wifi_buffer_t **buffer)
{
  sl_wifi_buffer_t *packet = NULL;
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex); // Release the mutex
    return SL_STATUS_EMPTY;                  // The queue is empty, no buffer to remove
  }

  packet                 = cmd_queues[queue].head;
  cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
  packet->node.node      = NULL;

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  // Decrement the flag associated with the specified command queue type.
  cmd_queues[queue].queued_packet_count--;

  *buffer = (sl_wifi_buffer_t *)packet;
  osMutexRelease(cmd_queues[queue].mutex); // Release the mutex

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_host_remove_node_from_queue(sl_si91x_queue_type_t queue,
                                                 sl_wifi_buffer_t **buffer,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function)
{
  sl_wifi_buffer_t *packet   = NULL;
  sl_wifi_buffer_t *data     = NULL;
  sl_wifi_buffer_t *previous = NULL;
  sl_status_t status         = SL_STATUS_NOT_FOUND;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex); // Release the mutex
    return SL_STATUS_EMPTY;                  // The queue is empty, no buffer to remove
  }

  packet   = cmd_queues[queue].head;
  previous = NULL;

  while (NULL != packet) {
    if (true == compare_function((sl_wifi_buffer_t *)packet, user_data)) {
      // Remove the matched packet from the queue
      if (NULL == previous) {
        cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
      } else {
        previous->node.node = packet->node.node;
      }

      if (cmd_queues[queue].tail == packet) {
        cmd_queues[queue].tail = previous;
      }

      packet->node.node = NULL;
      data              = packet;
      cmd_queues[queue].queued_packet_count--;
      status = SL_STATUS_OK;
      break;
    }

    previous = packet;
    packet   = (sl_wifi_buffer_t *)packet->node.node;
  }

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  *buffer = (sl_wifi_buffer_t *)data;
  osMutexRelease(cmd_queues[queue].mutex); // Release the mutex

  return status;
}

/* This function is used to flush the pending packets from the specified queue */
sl_status_t sl_si91x_host_flush_nodes_from_queue(sl_si91x_queue_type_t queue,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function,
                                                 sl_si91x_node_free_function_t node_free_function)
{
  sl_status_t status         = SL_STATUS_NOT_FOUND;
  sl_wifi_buffer_t *packet   = NULL;
  sl_wifi_buffer_t *data     = NULL;
  sl_wifi_buffer_t *previous = NULL;

  // Acquire the mutex for the specified queue with a timeout
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex);
    return SL_STATUS_EMPTY; // The queue is empty, no packets to flush
  }

  packet   = cmd_queues[queue].head;
  previous = NULL;

  while (NULL != packet) {
    if (true == compare_function((sl_wifi_buffer_t *)packet, user_data)) {
      data = packet;
      // Remove the matched packet from the queue
      if (NULL == previous) {
        cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
        packet                 = cmd_queues[queue].head;
      } else {
        previous->node.node = packet->node.node;
        packet              = (sl_wifi_buffer_t *)packet->node.node;
      }

      if (cmd_queues[queue].tail == data) {
        cmd_queues[queue].tail = previous;
      }

      data->node.node = NULL;
      cmd_queues[queue].queued_packet_count--;
      status = SL_STATUS_OK;
    } else {
      previous = packet;
      packet   = (sl_wifi_buffer_t *)packet->node.node;
    }

    if (data != NULL) {
      // Free the matched packet if required by the user
      node_free_function(data);
      data = NULL;
    }
  }

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail = NULL;
  }

  osMutexRelease(cmd_queues[queue].mutex);
  return status;
}

sl_status_t sl_si91x_flush_queue_based_on_type(sl_si91x_queue_type_t queue,
                                               sl_si91x_node_free_function_t node_free_function)
{
  sl_status_t status                  = SL_STATUS_OK;
  sl_wifi_buffer_t *packet            = NULL;
  sl_wifi_buffer_t *data              = NULL;
  sl_si91x_queue_packet_t *node       = NULL;
  sl_si91x_queue_packet_t *queue_node = NULL;
  uint16_t length                     = 0;
  uint32_t response_event             = 0;
  uint32_t response_type              = 0;

  // Acquire the mutex for the specified queue with a timeout
  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL);

  if (cmd_queues[queue].tail == NULL) {
    osMutexRelease(cmd_queues[queue].mutex);
    return SL_STATUS_EMPTY; // The queue is empty, no packets to flush
  }

  packet = cmd_queues[queue].head;

  while (NULL != packet) {
    sl_wifi_buffer_t *temp_packet = NULL;
    queue_node                    = sl_si91x_host_get_buffer_data(packet, 0, &length);
    if (queue_node == NULL) {
      osMutexRelease(cmd_queues[queue].mutex);
      return SL_STATUS_NOT_AVAILABLE;
    }

    if (queue_node->flags & SI91X_PACKET_RESPONSE_PACKET) {
      status = sl_create_generic_rx_packet_from_params(&node,
                                                       &temp_packet,
                                                       queue_node->packet_id,
                                                       queue_node->flags,
                                                       queue_node->sdk_context,
                                                       queue_node->command_type);
      if (status != SL_STATUS_OK) {
        osMutexRelease(cmd_queues[queue].mutex);
        return status;
      }

      if (queue_node->command_type == SI91X_COMMON_CMD) {
        response_event = NCP_HOST_COMMON_RESPONSE_EVENT;
        response_type  = SI91X_COMMON_RESPONSE_QUEUE;
      } else if (queue_node->command_type == SI91X_WLAN_CMD) {
        response_event = NCP_HOST_WLAN_RESPONSE_EVENT;
        response_type  = SI91X_WLAN_RESPONSE_QUEUE;
      } else if (queue_node->command_type == SI91X_NETWORK_CMD) {
        response_event = NCP_HOST_NETWORK_RESPONSE_EVENT;
        response_type  = SI91X_NETWORK_RESPONSE_QUEUE;
      } else if (queue_node->command_type == SI91X_SOCKET_CMD) {
        response_event = NCP_HOST_SOCKET_RESPONSE_EVENT;
        response_type  = SI91X_SOCKET_RESPONSE_QUEUE;
      }
      // add to response queue and raise event
      sl_si91x_host_add_to_queue(response_type, temp_packet);
      sl_si91x_host_set_event(response_event); // TODO: Handle partial async packets with appropriate event
    } else {
      data = packet;
      node_free_function(data);
    }

    cmd_queues[queue].head = (sl_wifi_buffer_t *)packet->node.node;
    packet                 = (sl_wifi_buffer_t *)packet->node.node;
    cmd_queues[queue].queued_packet_count--;
  }

  if (NULL == cmd_queues[queue].head) {
    cmd_queues[queue].tail                = NULL;
    cmd_queues[queue].queued_packet_count = 0;
  }

  osMutexRelease(cmd_queues[queue].mutex);
  return SL_STATUS_OK;
}

uint32_t sl_si91x_host_queue_status(sl_si91x_queue_type_t queue)
{
  uint32_t status = 0;

  osMutexAcquire(cmd_queues[queue].mutex, 0xFFFFFFFFUL); // Acquire the mutex with an timeout

  // Check if the queue is empty based on the tail pointer
  status = ((NULL == cmd_queues[queue].tail) ? 0 : cmd_queues[queue].flag);
  osMutexRelease(cmd_queues[queue].mutex); // Release the mutex

  return status;
}

uint32_t sl_si91x_host_get_queue_packet_count(sl_si91x_queue_type_t queue)
{
  return (uint32_t)(cmd_queues[queue].queued_packet_count);
}

uint32_t si91x_host_wait_for_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_events, event_mask, (osFlagsWaitAny | osFlagsNoClear), timeout);

  // Clear the received event flags
  osEventFlagsClear(si91x_events, event_mask);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_wait_for_bus_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_bus_events, event_mask, (osFlagsWaitAny), timeout);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_wait_for_async_event(uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(si91x_async_events, event_mask, (osFlagsWaitAny | osFlagsNoClear), timeout);

  // Clear the received event flags
  osEventFlagsClear(si91x_async_events, event_mask);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_bus_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_bus_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

uint32_t si91x_host_clear_async_events(uint32_t event_mask)
{
  uint32_t result = osEventFlagsClear(si91x_async_events, event_mask);
  if (result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

sl_status_t sl_si91x_host_power_cycle(void)
{
  sl_si91x_host_hold_in_reset();
  sl_si91x_host_delay_ms(100);

  sl_si91x_host_release_from_reset();
  sl_si91x_host_delay_ms(100);

  return SL_STATUS_OK;
}

void print_80211_packet(uint8_t *packet, uint32_t packet_length, uint16_t max_payload_length)
{
  uint32_t dump_bytes    = 0;
  uint32_t header_length = MAC80211_HDR_MIN_LEN;

  header_length += (packet[0] & BIT(7)) ? 2 : 0;                           /* 2 bytes QoS control */
  header_length += ((packet[1] & BIT(0)) && (packet[1] & BIT(1))) ? 6 : 0; /* 6 byte Addr4 */

  sl_debug_log("%02x %02x | ", packet[0], packet[1]); /* FC */
  sl_debug_log("%02x %02x | ", packet[2], packet[3]); /* Dur */
  sl_debug_log("%02x:%02x:%02x:%02x:%02x:%02x | ",
               packet[4],
               packet[5],
               packet[6],
               packet[7],
               packet[8],
               packet[9]); /* Addr1/RA */
  sl_debug_log("%02x:%02x:%02x:%02x:%02x:%02x | ",
               packet[10],
               packet[11],
               packet[12],
               packet[13],
               packet[14],
               packet[15]); /* Addr2/TA */
  sl_debug_log("%02x:%02x:%02x:%02x:%02x:%02x | ",
               packet[16],
               packet[17],
               packet[18],
               packet[19],
               packet[20],
               packet[21]);                             /* Addr3/DA */
  sl_debug_log("%02x %02x | ", packet[22], packet[23]); /* Seq control */
  if ((packet[1] & BIT(0)) && (packet[1] & BIT(1))) {   /* Addr4 */
    sl_debug_log("%02x:%02x:%02x:%02x:%02x:%02x | ",
                 packet[24],
                 packet[25],
                 packet[26],
                 packet[27],
                 packet[28],
                 packet[29]);
  }
  if (packet[0] & BIT(7)) {
    sl_debug_log("%02x %02x | ", packet[30], packet[31]); /* QoS control */
  }

  // Determine number of payload bytes to print
  dump_bytes = packet_length - header_length;
  dump_bytes = max_payload_length > dump_bytes ? dump_bytes : max_payload_length;

  for (uint32_t i = header_length; i < header_length + dump_bytes; i++) {
    sl_debug_log("%02x ", packet[i]);
  }

  sl_debug_log("|\r\n");
}

uint8_t sli_lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end)
{
  uint8_t din[8];
  uint8_t state[8];
  uint8_t state_c[8];
  uint8_t crc8_out;

  din[0] = ((crc8_din & BIT(7)) >> 7);
  din[1] = ((crc8_din & BIT(6)) >> 6);
  din[2] = ((crc8_din & BIT(5)) >> 5);
  din[3] = ((crc8_din & BIT(4)) >> 4);
  din[4] = ((crc8_din & BIT(3)) >> 3);
  din[5] = ((crc8_din & BIT(2)) >> 2);
  din[6] = ((crc8_din & BIT(1)) >> 1);
  din[7] = ((crc8_din & BIT(0)) >> 0);

  state[0] = ((crc8_state & BIT(0)) >> 0);
  state[1] = ((crc8_state & BIT(1)) >> 1);
  state[2] = ((crc8_state & BIT(2)) >> 2);
  state[3] = ((crc8_state & BIT(3)) >> 3);
  state[4] = ((crc8_state & BIT(4)) >> 4);
  state[5] = ((crc8_state & BIT(5)) >> 5);
  state[6] = ((crc8_state & BIT(6)) >> 6);
  state[7] = ((crc8_state & BIT(7)) >> 7);

  state_c[7] = (state[7] ^ din[7]) ^ (state[6] ^ din[6]) ^ (state[5] ^ din[5]);

  state_c[6] = (state[6] ^ din[6]) ^ (state[5] ^ din[5]) ^ (state[4] ^ din[4]);

  state_c[5] = (state[5] ^ din[5]) ^ (state[4] ^ din[4]) ^ (state[3] ^ din[3]);

  state_c[4] = (state[4] ^ din[4]) ^ (state[3] ^ din[3]) ^ (state[2] ^ din[2]);

  state_c[3] = (state[1] ^ din[1]) ^ (state[2] ^ din[2]) ^ (state[3] ^ din[3]) ^ (state[7] ^ din[7]);

  state_c[2] = (state[0] ^ din[0]) ^ (state[1] ^ din[1]) ^ (state[2] ^ din[2]) ^ (state[6] ^ din[6]);

  state_c[1] = (state[0] ^ din[0]) ^ (state[1] ^ din[1]) ^ (state[6] ^ din[6]);

  state_c[0] = (state[0] ^ din[0]) ^ (state[7] ^ din[7]) ^ (state[6] ^ din[6]);
  if (!end) {
    crc8_out = ((state_c[0] & BIT(0)) << 0) | ((state_c[1] & BIT(0)) << 1) | ((state_c[2] & BIT(0)) << 2)
               | ((state_c[3] & BIT(0)) << 3) | ((state_c[4] & BIT(0)) << 4) | ((state_c[5] & BIT(0)) << 5)
               | ((state_c[6] & BIT(0)) << 6) | ((state_c[7] & BIT(0)) << 7);
  } else {
    crc8_out = ((state_c[7] & BIT(0)) << 0) | ((state_c[6] & BIT(0)) << 1) | ((state_c[5] & BIT(0)) << 2)
               | ((state_c[4] & BIT(0)) << 3) | ((state_c[3] & BIT(0)) << 4) | ((state_c[2] & BIT(0)) << 5);

    crc8_out = ~crc8_out;
    crc8_out &= 0x3f;
  }
  return (crc8_out);
}

uint8_t sli_multicast_mac_hash(uint8_t *mac)
{
  uint8_t i, crc = 0xff;
  for (i = 0; i < 6; i++) {
    crc = sli_lmac_crc8_c(mac[i], crc, ((i == 5) ? 1 : 0));
  }
  return (crc);
}

bool sli_si91x_get_flash_command_status()
{
  return sli_si91x_packet_status;
}

void sli_si91x_update_flash_command_status(bool flag)
{
  sli_si91x_packet_status = flag;
}

// This function is used to update the power manager to see whether the device is ready for sleep or not.
// True indicates ready for sleep, and false indicates not ready for sleep.
bool sli_si91x_is_sdk_ok_to_sleep()
{
  bool tx_queues_empty = (sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_COMMON_CMD)
                          || sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_WLAN_CMD)
                          || sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_NETWORK_CMD)
                          || sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_SOCKET_CMD)
                          || sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_BT_CMD)
                          || sl_si91x_host_get_queue_packet_count((sl_si91x_queue_type_t)SI91X_SOCKET_DATA));
  return ((!sli_si91x_get_flash_command_status()) && (!tx_queues_empty));
}
