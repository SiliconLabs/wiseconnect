/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_protocol_types.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi_device.h"

extern sl_performance_profile_t current_performance_profile;

sl_status_t convert_si91x_wifi_client_info(sl_wifi_client_info_response *client_info_response,
                                           sl_si91x_client_info_response *sl_si91x_client_info_response);
sl_wifi_event_t convert_si91x_event_to_sl_wifi_event(rsi_wlan_cmd_response_t command, uint16_t frame_status);

sl_status_t save_sl_wifi_rate(sl_wifi_rate_t transfer_rate);
sl_status_t get_saved_sl_wifi_rate(sl_wifi_rate_t *transfer_rate);
void reset_sl_wifi_rate();

sl_status_t get_rate_protocol_and_data_rate(const uint8_t data_rate,
                                            sl_wifi_rate_protocol_t *rate_protocol,
                                            sl_wifi_rate_t *transfer_rate);

sl_status_t save_ap_configuration(const sl_wifi_ap_configuration_t *wifi_ap_configuration);
sl_status_t get_saved_ap_configuration(sl_wifi_ap_configuration_t *wifi_ap_confuguration);
void reset_ap_configuration();

void save_tcp_auto_close_choice(bool is_tcp_auto_close_enabled);
bool is_tcp_auto_close_enabled();
void set_card_ready_required(bool card_ready_required);
bool get_card_ready_required();

void save_max_tx_power(uint8_t max_tx_power);
uint8_t get_max_tx_power();
void reset_max_tx_power();

void save_wifi_current_performance_profile(const sl_wifi_performance_profile_t *profile);
void get_wifi_current_performance_profile(sl_wifi_performance_profile_t *profile);

void save_bt_current_performance_profile(const sl_bt_performance_profile_t *profile);
void get_bt_current_performance_profile(sl_bt_performance_profile_t *profile);

void get_coex_performance_profile(sl_performance_profile_t *profile);
void reset_coex_current_performance_profile(void);

void save_boot_configuration(const sl_si91x_boot_configuration_t *boot_configuration);
void get_saved_boot_configuration(sl_si91x_boot_configuration_t *boot_configuration);

void save_coex_mode(sl_si91x_coex_mode_t coex_mode);
sl_si91x_coex_mode_t get_coex_mode(void);

/*********************************************************************************************
 * @brief
 * 	 An utility function computes coex performance profile internally and converts into sl_si91x_power_save_request_t.
 * @param profile
 *   performance profile which needs to be converted to its equivalent si91x_power_save_request structure.
 * @param power_save_request
 *   si91x specific structure that holds required configuration for the given performance profile.
 *   This is optional parameter.
 * @return coex_profile
 *   Coex power profile
 * @return
 * 	 sl_status_t
 *********************************************************************************************/
sl_status_t sl_si91x_send_power_save_request(sl_performance_profile_t profile);

/**
 * An utility function to convert dBm value  to si91x specific power value
 * @param power_value_in_dbm power value with dBm as units.
 * @return si91x power level
 */
static inline uint8_t convert_dbm_to_si91x_power_level(uint8_t power_value_in_dBm)
{
  if (power_value_in_dBm >= SI91X_LOW_TRANSMIT_POWER_THRESHOLD) {
    return SL_SI91X_LOW_POWER_LEVEL;
  } else if (power_value_in_dBm >= SI91X_MEDIUM_TRANSMIT_POWER_THRESHOLD) {
    return SL_SI91X_MEDIUM_POWER_LEVEL;
  } else {
    return SL_SI91X_HIGH_POWER_LEVEL;
  }
}
