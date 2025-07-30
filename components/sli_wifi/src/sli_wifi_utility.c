/***************************************************************************/ /**
 * @file    sli_wifi_utility.c
 * @brief
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
#include "sli_wifi.h"
#include "sli_wifi_utility.h"
#include "sl_wifi_device.h"
#include "sli_wifi_constants.h"
#include "sli_wifi_types.h"
#include "sl_core.h"
#include "sl_wifi_credentials.h"
#include "sl_rsi_utility.h"
#include <assert.h>

#define DEFAULT_BEACON_MISS_IGNORE_LIMIT 1

// This value will be used in connect request/ AP configurations to set the TX power of the module
static sl_wifi_max_tx_power_t wifi_max_tx_power = {
  .scan_tx_power = 0x1f, //Default power value set to max value supported in dBm
  .join_tx_power = 0x1f, //Default power value set to max value supported in dBm
};
static bool is_card_ready_required         = true;
static sl_wifi_rate_t saved_wifi_data_rate = SL_WIFI_AUTO_RATE;
static sl_wifi_ap_configuration_t ap_configuration;
sli_wifi_performance_profile_t performance_profile;
static sli_scan_info_t *scan_info_database = NULL;

extern osEventFlagsId_t si91x_events;
void sli_convert_performance_profile_to_power_save_command(sl_wifi_system_performance_profile_t profile,
                                                           sli_si91x_power_save_request_t *power_save_request)
{
  SL_ASSERT(power_save_request != NULL);
  if (performance_profile.wifi_performance_profile.monitor_interval) {
    power_save_request->monitor_interval = performance_profile.wifi_performance_profile.monitor_interval;
  } else {
    power_save_request->monitor_interval = SLI_DEFAULT_MONITOR_INTERVAL;
  }

  power_save_request->ulp_mode_enable   = SLI_ULP_WITH_RAM_RETENTION;
  power_save_request->dtim_aligned_type = performance_profile.wifi_performance_profile.dtim_aligned_type;
  power_save_request->num_of_dtim_skip  = performance_profile.wifi_performance_profile.num_of_dtim_skip;
  power_save_request->listen_interval   = (uint16_t)performance_profile.wifi_performance_profile.listen_interval;
  power_save_request->psp_type          = SLI_MAX_PSP;
  if (performance_profile.wifi_performance_profile.beacon_miss_ignore_limit) {
    power_save_request->beacon_miss_ignore_limit =
      performance_profile.wifi_performance_profile.beacon_miss_ignore_limit;
  } else {
    power_save_request->beacon_miss_ignore_limit = DEFAULT_BEACON_MISS_IGNORE_LIMIT;
  }

  // Depending on the specified performance profile, configure the power_save_request structure
  switch (profile) {
    case HIGH_PERFORMANCE: {
      // For HIGH_PERFORMANCE profile, reset all fields in the power_save_request structure to zero
      memset(power_save_request, 0, sizeof(sli_si91x_power_save_request_t));
      break;
    }

    case ASSOCIATED_POWER_SAVE: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SLI_CONNECTED_M4_BASED_PS;
#else
      power_save_request->power_mode = SLI_CONNECTED_GPIO_BASED_PS;
#endif
      break;
    }

    case ASSOCIATED_POWER_SAVE_LOW_LATENCY: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SLI_CONNECTED_M4_BASED_PS;
#else
      power_save_request->power_mode = SLI_CONNECTED_GPIO_BASED_PS;
#endif
      power_save_request->psp_type = SLI_FAST_PSP;
      break;
    }

    case DEEP_SLEEP_WITHOUT_RAM_RETENTION: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SLI_M4_BASED_DEEP_SLEEP;
#else
      power_save_request->power_mode = SLI_GPIO_BASED_DEEP_SLEEP;
#endif
      power_save_request->ulp_mode_enable = SLI_ULP_WITHOUT_RAM_RET_RETENTION;
      break;
    }

    case DEEP_SLEEP_WITH_RAM_RETENTION: {
#ifdef SLI_SI91X_MCU_INTERFACE
      power_save_request->power_mode = SLI_M4_BASED_DEEP_SLEEP;
#else
      power_save_request->power_mode = SLI_GPIO_BASED_DEEP_SLEEP;
#endif
      break;
    }
    default: {
      // Do nothing
    } break;
  }

  return;
}

// Function to identify expected scan result based on filter
static bool sli_filter_scan_info(const sli_scan_info_t *scan_info,
                                 const sl_wifi_extended_scan_result_parameters_t *extended_scan_parameters)
{
  if (NULL == scan_info) {
    return false;
  }

  if ((NULL != extended_scan_parameters->channel_filter)
      && (*(extended_scan_parameters->channel_filter) != scan_info->channel)) {
    return false;
  }

  if ((NULL != extended_scan_parameters->security_mode_filter)
      && (*(extended_scan_parameters->security_mode_filter) != scan_info->security_mode)) {
    return false;
  }

  if ((NULL != extended_scan_parameters->rssi_filter)
      && (*(extended_scan_parameters->rssi_filter) <= scan_info->rssi)) {
    return false;
  }

  if ((NULL != extended_scan_parameters->network_type_filter)
      && (*(extended_scan_parameters->network_type_filter) != scan_info->network_type)) {
    return false;
  }

  return true;
}

// Function to get all or filtered scan results from scan result database
sl_status_t sli_wifi_get_stored_scan_results(sl_wifi_interface_t interface,
                                             sl_wifi_extended_scan_result_parameters_t *extended_scan_parameters) //Done
{
  UNUSED_PARAMETER(interface);
  if (NULL == extended_scan_parameters) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_wifi_extended_scan_result_t *scan_results = extended_scan_parameters->scan_results;
  uint16_t *result_count                       = extended_scan_parameters->result_count;
  uint16_t length                              = extended_scan_parameters->array_length;
  sli_scan_info_t *scan_info                   = scan_info_database;

  if ((NULL == scan_results) || (NULL == result_count) || (0 == length)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  *result_count = 0;

  while ((0 != length) && (NULL != scan_info)) {
    if (true == sli_filter_scan_info(scan_info, extended_scan_parameters)) {
      scan_results[*result_count].rf_channel    = scan_info->channel;
      scan_results[*result_count].security_mode = scan_info->security_mode;
      scan_results[*result_count].rssi          = scan_info->rssi;
      scan_results[*result_count].network_type  = scan_info->network_type;
      memcpy(scan_results[*result_count].bssid, scan_info->bssid, SLI_WIFI_HARDWARE_ADDRESS_LENGTH);
      memcpy(scan_results[*result_count].ssid, scan_info->ssid, 34);
      (*result_count)++;
      length--;
    }
    scan_info = scan_info->next;
  }

  return SL_STATUS_OK;
}

// Get the current Wi-Fi performance profile
void sli_wifi_get_current_performance_profile(sl_wifi_performance_profile_v2_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(profile, &performance_profile.wifi_performance_profile, sizeof(sl_wifi_performance_profile_v2_t));
}

void *sli_wifi_host_get_buffer_data(sl_wifi_buffer_t *buffer, uint16_t offset, uint16_t *data_length)
{
  if (offset >= buffer->length) {
    return NULL;
  }
  if (data_length) {
    *data_length = (uint16_t)(buffer->length) - offset;
  }
  return (void *)&buffer->data[offset];
}

// Function to Clean up all the scan results in scan result database
void sli_wifi_flush_scan_results_database(void)
{
  sli_scan_info_t *scan_info = scan_info_database;
  sli_scan_info_t *node      = NULL;

  while (NULL != scan_info) {
    node      = scan_info;
    scan_info = scan_info->next;
    free(node);
  }
  scan_info_database = NULL;

  return;
}

sl_status_t sli_wifi_allocate_command_buffer(sl_wifi_buffer_t **host_buffer,
                                             void **buffer,
                                             uint32_t requested_buffer_size,
                                             uint32_t wait_duration_ms)
{
  // Allocate a buffer from the SI91x host for WLAN control messages
  sl_status_t status =
    sli_si91x_host_allocate_buffer(host_buffer, SL_WIFI_CONTROL_BUFFER, requested_buffer_size, wait_duration_ms);
  VERIFY_STATUS_AND_RETURN(status);

  uint16_t temp;
  // Get a pointer to the allocated buffer's data area
  *buffer = sli_wifi_host_get_buffer_data(*host_buffer, 0, &temp);
  return SL_STATUS_OK;
}

void sli_wifi_append_to_buffer_queue(sli_wifi_buffer_queue_t *queue, sl_wifi_buffer_t *buffer)
{
  CORE_irqState_t state = CORE_EnterAtomic();
  if (queue->tail == NULL) {
    assert(queue->head == NULL); // Both should be NULL at the same time
    queue->head = buffer;
    queue->tail = buffer;
  } else {
    queue->tail->node.node = &buffer->node;
    queue->tail            = buffer;
  }
  CORE_ExitAtomic(state);
}

void sli_wifi_set_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_events, event_mask);
}

sl_status_t sli_wifi_remove_buffer_from_queue_by_comparator(sli_wifi_buffer_queue_t *queue,
                                                            const void *user_data,
                                                            sli_wifi_buffer_comparator comparator,
                                                            sl_wifi_buffer_t **buffer)
{
  // Check if the queue is empty
  if (queue->head == NULL) {
    assert(queue->tail == NULL);
    return SL_STATUS_EMPTY;
  }

  CORE_irqState_t state            = CORE_EnterAtomic();
  sl_wifi_buffer_t *current_buffer = queue->head;
  sl_wifi_buffer_t *prev_buffer    = NULL;

  // Iterate through the queue to find a matching buffer based on the comparator function
  while (current_buffer != NULL) {
    // Skip to the next buffer if this one doesn't match
    if (!comparator(current_buffer, user_data)) {
      prev_buffer    = current_buffer;
      current_buffer = (sl_wifi_buffer_t *)current_buffer->node.node;
      continue;
    }

    // Buffer matches, proceed to remove it
    *buffer = current_buffer;

    if (current_buffer == queue->head) {
      // Removing the head of the queue
      queue->head = (sl_wifi_buffer_t *)current_buffer->node.node;
      if (queue->head == NULL) {
        queue->tail = NULL; // Queue is now empty
      }
    } else if (current_buffer == queue->tail) {
      // Removing the tail of the queue
      queue->tail            = prev_buffer;
      prev_buffer->node.node = NULL;
    } else {
      // Removing a buffer from the middle
      prev_buffer->node.node = current_buffer->node.node;
    }

    CORE_ExitAtomic(state);
    return SL_STATUS_OK;
  }

  // No matching buffer was found
  CORE_ExitAtomic(state);
  return SL_STATUS_NOT_FOUND;
}

sl_wifi_max_tx_power_t sli_get_max_tx_power()
{
  return wifi_max_tx_power;
}

sl_status_t sli_wifi_get_saved_rate(sl_wifi_rate_t *transfer_rate)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(transfer_rate);

  *transfer_rate = saved_wifi_data_rate;
  return SL_STATUS_OK;
}

sl_status_t sli_wifi_host_get_credentials(sl_wifi_credential_id_t id, uint8_t type, sl_wifi_credential_t *cred)
{
  uint32_t credential_length = sizeof(sl_wifi_credential_t) - offsetof(sl_wifi_credential_t, pmk);
  sl_status_t status         = sl_wifi_get_credential(id, &cred->type, &cred->pmk, &credential_length);
  VERIFY_STATUS_AND_RETURN(status);

  if (type == SL_WIFI_PSK_CREDENTIAL) {
    if ((cred->type == SL_WIFI_PSK_CREDENTIAL) || (cred->type == SL_WIFI_PMK_CREDENTIAL)) {
      return SL_STATUS_OK;
    } else {
      return SL_STATUS_FAIL;
    }
  } else if (type != cred->type) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

void sli_save_max_tx_power(uint8_t max_scan_tx_power, uint8_t max_join_tx_power)
{
  wifi_max_tx_power.scan_tx_power = max_scan_tx_power;
  wifi_max_tx_power.join_tx_power = max_join_tx_power;
}

sl_status_t sli_wifi_save_ap_configuration(const sl_wifi_ap_configuration_t *wifi_ap_configuration)
{
  // Check if the input pointer is valid
  SL_WIFI_ARGS_CHECK_NULL_POINTER(wifi_ap_configuration);
  ap_configuration = *wifi_ap_configuration;

  return SL_STATUS_OK;
}

void sli_wifi_save_current_performance_profile(const sl_wifi_performance_profile_v2_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(&performance_profile.wifi_performance_profile, profile, sizeof(sl_wifi_performance_profile_v2_t));

  performance_profile.coex_mode = sli_get_coex_mode();
}

// Get the coexistence performance profile based on the current coexistence mode
void sli_get_coex_performance_profile(sl_wifi_system_performance_profile_t *profile)
{
  SL_ASSERT(profile != NULL);
  uint8_t mode_decision                       = 0;
  sl_wifi_system_coex_mode_t stored_coex_mode = performance_profile.coex_mode;
  if (stored_coex_mode == SL_SI91X_WLAN_ONLY_MODE) { // Treat SL_SI91X_WLAN_ONLY_MODE as SL_SI91X_WLAN_MODE
    stored_coex_mode = SL_SI91X_WLAN_MODE;
  }
  // Determine the mode decision based on the coexistence mode
  switch (stored_coex_mode) {
    case SL_SI91X_WLAN_MODE: {
      // Wi-Fi only mode
      mode_decision = (uint8_t)((performance_profile.wifi_performance_profile.profile << 4)
                                | (performance_profile.wifi_performance_profile.profile));
    } break;
    case SL_SI91X_BLUETOOTH_MODE:
    case SL_SI91X_BLE_MODE:
    case SL_SI91X_DUAL_MODE: {
      // Bluetooth only or dual-mode (BT + Wi-Fi) mode
      mode_decision = (uint8_t)((performance_profile.bt_performance_profile.profile << 4)
                                | (performance_profile.bt_performance_profile.profile));
    } break;
    case SL_SI91X_WLAN_BLUETOOTH_MODE:
    case SL_SI91X_WLAN_DUAL_MODE:
    case SL_SI91X_WLAN_BLE_MODE: {
      // Wi-Fi + Bluetooth mode
      mode_decision = (uint8_t)((performance_profile.wifi_performance_profile.profile << 4)
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
      *profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION; // Power save mode
    } break;
    case 0x44: {
      *profile = DEEP_SLEEP_WITH_RAM_RETENTION; // Power save mode with RAM retention
    } break;
    default: {
      // Do nothing
    } break;
  }
  return;
}

void sli_wifi_reset_ap_configuration()
{
  // Initialize the ap_configuration structure with zeros
  ap_configuration = (sl_wifi_ap_configuration_t){ 0 };
}

sl_status_t sli_wifi_send_power_save_request(const sl_wifi_performance_profile_v2_t *wifi_profile,
                                             const sl_bt_performance_profile_t *bt_profile)
{
  sl_status_t status;
  sli_si91x_power_save_request_t power_save_request               = { 0 };
  sl_wifi_system_performance_profile_t selected_coex_profile_mode = { 0 };
  // Disable power save mode by setting it to HIGH_PERFORMANCE profile
  status = sli_wifi_send_command(SLI_WIFI_REQ_PWRMODE,
                                 SI91X_COMMON_CMD,
                                 &power_save_request,
                                 sizeof(sli_si91x_power_save_request_t),
                                 SLI_WIFI_RSP_PWRMODE_WAIT_TIME,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);

  if (NULL != wifi_profile) {
    // Save the new Wi-Fi profile
    sli_wifi_save_current_performance_profile(wifi_profile);
  }

  if (NULL != bt_profile) {
    // Save the new BT/BLE profile
    sli_save_bt_current_performance_profile(bt_profile);
  }

  // get the updated coex profile
  sli_get_coex_performance_profile(&selected_coex_profile_mode);

  // If the requested performance profile is HIGH_PERFORMANCE, no need to send the request to firmware
  if (selected_coex_profile_mode == HIGH_PERFORMANCE) {
    return SL_STATUS_OK;
  }

  // Convert the performance profile to a power save request.
  sli_convert_performance_profile_to_power_save_command(selected_coex_profile_mode, &power_save_request);

  status = sli_wifi_send_command(SLI_WIFI_REQ_PWRMODE,
                                 SI91X_COMMON_CMD,
                                 &power_save_request,
                                 sizeof(sli_si91x_power_save_request_t),
                                 SL_SI91X_WAIT_FOR_RESPONSE(SLI_WIFI_RSP_PWRMODE_WAIT_TIME),
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

void sli_wifi_save_wifi_current_performance_profile(const sl_wifi_performance_profile_v2_t *profile)
{
  SL_ASSERT(profile != NULL);
  memcpy(&performance_profile.wifi_performance_profile, profile, sizeof(sl_wifi_performance_profile_v2_t));

  performance_profile.coex_mode = sli_get_coex_mode();
}

sl_status_t sli_wifi_get_saved_ap_configuration(sl_wifi_ap_configuration_t *wifi_ap_configuration)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(wifi_ap_configuration);
  *wifi_ap_configuration = ap_configuration;

  return SL_STATUS_OK;
}

sl_status_t sli_wifi_get_rate_protocol_and_data_rate(const uint8_t data_rate,
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
sl_status_t sli_wifi_save_rate(sl_wifi_rate_t transfer_rate)
{
  // Check if the provided transfer rate is valid
  if (transfer_rate == SL_WIFI_RATE_INVALID) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  saved_wifi_data_rate = transfer_rate;

  return SL_STATUS_OK;
}

void sli_wifi_reset_coex_current_performance_profile(void)
{
  memset(&performance_profile, 0, sizeof(sli_wifi_performance_profile_t));
}

void sli_wifi_reset_max_tx_power()
{
  wifi_max_tx_power.scan_tx_power = 0x1f;
  wifi_max_tx_power.join_tx_power = 0x1f;
}

void sli_wifi_reset_sl_wifi_rate()
{
  saved_wifi_data_rate = 0;
}

bool sli_wifi_get_card_ready_required()
{
  return is_card_ready_required;
}

void sli_wifi_set_card_ready_required(bool card_ready_required)
{
  is_card_ready_required = card_ready_required;
}

//In Access point mode NWP only supports No Encryption, TKIP and CCMP encryptions.
sl_status_t sli_wifi_get_nwp_encryption(sl_wifi_encryption_t encryption_mode, uint8_t *encryption_request)
{
  switch (encryption_mode) {
    case SL_WIFI_NO_ENCRYPTION:
      *encryption_request = SLI_WIFI_NO_ENCRYPTION;
      break;
    case SL_WIFI_TKIP_ENCRYPTION:
      *encryption_request = SLI_WIFI_TKIP_ENCRYPTION;
      break;
    case SL_WIFI_CCMP_ENCRYPTION:
    case SL_WIFI_DEFAULT_ENCRYPTION:
      *encryption_request = SLI_WIFI_CCMP_ENCRYPTION;
      break;
    default:
      return SL_STATUS_NOT_SUPPORTED;
  }

  return SL_STATUS_OK;
}