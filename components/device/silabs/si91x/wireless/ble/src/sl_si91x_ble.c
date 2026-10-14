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
#include "sli_wifi_power_profile.h"
#include "rsi_ble_apis.h"
#include "rsi_common.h"
#include "sli_si91x_driver.h"
#include <string.h>

extern bool device_initialized;

/*=======================================================================*/
static sl_status_t sli_map_rsi_ble_status(int32_t rsi_status)
{
  if (rsi_status == RSI_SUCCESS) {
    return SL_STATUS_OK;
  }

  if (rsi_status < 0) {
    return sli_convert_si91x_status_to_sl_status((si91x_status_t)rsi_status);
  }

  return (sl_status_t)rsi_status;
}
/**
 * @brief Sets the performance profile for the Si91x Bluetooth module.
 *
 * This function sets the performance profile for the Si91x Bluetooth module based on the provided profile.
 * The function takes a backup of the current Bluetooth profile and computes the selected coexistence (coex) profile.
 * If the selected coex profile is the same as the current coex profile, the function returns SL_STATUS_OK.
 * Otherwise, the function sends a power save request with the selected coex profile.
 * If the power save request fails, the function restores the previous Bluetooth profile, and returns the corresponding status.
 * If the selected coex profile is DEEP_SLEEP_WITHOUT_RAM_RETENTION, the device_initialized flag is set to false, and the coex current performance profile is reset.
 *
 * @param profile Pointer to the performance profile to be set.
 * @return SL_STATUS_OK if the performance profile is set successfully, or an appropriate error code if an error occurs.
 */
sl_status_t sl_si91x_bt_set_performance_profile(const sl_bt_performance_profile_t *profile)
{
  sl_status_t status;
  sl_wifi_system_performance_profile_t selected_coex_profile_mode = { 0 };
  sl_bt_performance_profile_t current_bt_profile_mode             = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  SL_WIFI_ARGS_CHECK_NULL_POINTER(profile);

  // Take backup of current bt profile
  sli_get_bt_current_performance_profile(&current_bt_profile_mode);

  // Send the power save command for the requested profile
  status = sli_wifi_send_power_save_request(NULL, profile);
  if (status != SL_STATUS_OK) {
    sli_save_bt_current_performance_profile(&current_bt_profile_mode);
    return status;
  }
  sli_get_coex_performance_profile(&selected_coex_profile_mode);

  // Set device_initialized as false since RAM of module would not be retained
  // in ULTRA_POWER_SAVE and module needs to be started from init again.
  if (selected_coex_profile_mode == DEEP_SLEEP_WITHOUT_RAM_RETENTION) {
    device_initialized = false;
    sli_reset_coex_current_performance_profile();
  }
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_bt_get_performance_profile(sl_bt_performance_profile_t *profile)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sli_get_bt_current_performance_profile(profile);
  return SL_STATUS_OK;
}

/**
 * @brief
 *   Utility to issue a disconnect request for each tracked BLE connection.
 * @details
 *   Application helper that iterates connected remote BLE devices tracked by
 *   the driver and calls rsi_ble_disconnect() for each one. It is **not**
 *   invoked automatically from sl_wifi_deinit() / sl_net_deinit(); the
 *   application owns the full teardown sequence (stop ADV/SCAN, disconnect,
 *   wait for events, then deinit).
 *
 *   For each peer, this API waits only for the disconnect **command response**
 *   (command ACK). It does **not** wait for disconnect-complete events.
 *   On a per-peer command failure the API continues remaining peers and
 *   returns the first rsi_ble_disconnect() status after the loop.
 *
 *   The application is responsible for interpreting and mapping returned
 *   error codes (host RSI_ERROR_* and firmware BLE statuses) as needed.
 *
 *   Recommended application teardown sequence before Wi-Fi/NWP deinit:
 *   -# Stop BLE advertising and scanning (classic and/or AE) if active
 *   -# Call @ref sl_si91x_ble_disconnect_all
 *   -# Wait for BLE disconnect event(s), or until rsi_ble_is_device_connected()
 *      returns false
 *   -# Call sl_wifi_deinit() / sl_net_deinit() / rsi_ble_disable()
 * @note
 *   If this API is invoked as part of the sl_wifi_deinit() sequence, ensure
 *   that any active BLE advertising and scanning roles are stopped before
 *   calling this API. After the API call, wait for the BLE disconnect event(s)
 *   to be received before invoking sl_wifi_deinit(). Additionally, the
 *   application should not restart advertising or scanning from the BLE
 *   disconnect event handler during this deinitialization flow.
 * @return
 *   SL_STATUS_OK if every disconnect command response succeeds.
 *   Otherwise returns the first rsi_ble_disconnect() status value; the
 *   application must map/interpret error codes.
 */
sl_status_t sl_si91x_ble_disconnect_all(void)
{
  int32_t rsi_status;
  int32_t first_error = RSI_SUCCESS;
  uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN];
  const rsi_bt_cb_t *le_cb;

  if (!sl_si91x_is_device_initialized()) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((rsi_driver_cb == NULL) || (rsi_driver_cb->ble_cb == NULL)) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  le_cb = rsi_driver_cb->ble_cb;

  for (uint8_t inx = 0; inx < MAX_REMOTE_BLE_DEVICES; inx++) {
    if (le_cb->remote_ble_info[inx].used == 0) {
      continue;
    }

    memcpy(remote_dev_addr, le_cb->remote_ble_info[inx].remote_dev_bd_addr, RSI_DEV_ADDR_LEN);
    rsi_status = rsi_ble_disconnect((const int8_t *)remote_dev_addr);
    if ((rsi_status != RSI_SUCCESS) && (first_error == SL_STATUS_OK)) {
      first_error = sli_map_rsi_ble_status(rsi_status);
    }
  }
  /* Pass through rsi_ble_disconnect status; application owns mapping. */
  return (sl_status_t)first_error;
}
