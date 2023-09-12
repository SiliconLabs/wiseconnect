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
#include "sl_si91x_ble.h"
#include "sl_rsi_utility.h"

extern bool device_initialized;

/*=======================================================================*/

sl_status_t sl_si91x_bt_set_performance_profile(const sl_bt_performance_profile_t *profile)
{
  sl_status_t status;
  sl_performance_profile_t selected_coex_profile_mode = { 0 };
  sl_performance_profile_t current_coex_profile_mode  = { 0 };
  sl_bt_performance_profile_t current_bt_profile_mode = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  ARGS_CHECK_NULL_POINTER(profile);

  // Take backup of current bt profile
  get_bt_current_performance_profile(&current_bt_profile_mode);
  get_coex_performance_profile(&current_coex_profile_mode);
  // Compute selected coex profile
  // Check if current coex profile and selected coex profile are the same
  save_bt_current_performance_profile(profile);
  get_coex_performance_profile(&selected_coex_profile_mode);

  if (selected_coex_profile_mode == current_coex_profile_mode) {
    return SL_STATUS_OK;
  }

  status = sl_si91x_send_power_save_request(selected_coex_profile_mode);
  if (status != SL_STATUS_OK) {
    save_bt_current_performance_profile(&current_bt_profile_mode);
    return status;
  }

  // Set device_initialized as false since RAM of module will be not retained
  // in ULTRA_POWER_SAVE and module needs to be started from init again
  if (selected_coex_profile_mode == STANDBY_POWER_SAVE) {
    device_initialized = false;
    reset_coex_current_performance_profile();
  }
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_bt_get_performance_profile(sl_bt_performance_profile_t *profile)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  get_bt_current_performance_profile(profile);
  return SL_STATUS_OK;
}
