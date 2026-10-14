/*******************************************************************************
* @file  gap_init.c
* @brief GAP Component Initialization
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @brief GAP Component Initialization
 * 
 * This file contains the GAP initialization function that encapsulates ALL
 * GAP-specific initialization including:
 * - Callback registration
 * - Connection state initialization
 * - User configuration
 * - Device setup (name, address, IRK)
 * - Coex role priority configuration
 * - Connection timeout timer setup
 * - Advertising and scan parameter setup
 * 
 * EXTRACTION SOURCE: ble_unified_app.c
 * - Lines ~1308-1385: ble_private_default_init()
 * - Lines ~1274-1292: rsi_fill_ble_user_config()
 * - Lines ~1043-1272: rsi_ble_initialize_conn_buffer()
 * - Lines ~1500-1577: rsi_ble_dual_role() (GAP-specific portions)
 * - Lines ~1474-1498: rsi_change_ble_adv_and_scan_params()
 */

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "sl_constants.h"
#include "rsi_common_apis.h"
#include "rsi_bt_common_apis.h"
#include <gap_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gap.h"
#include <inttypes.h>

/*=======================================================================*/
//! Forward declarations of GAP utility functions
/*=======================================================================*/
void ble_private_default_init(void);
int8_t rsi_fill_ble_user_config(void);
void rsi_change_ble_adv_and_scan_params(void);
void connect_timeout_handler(TimerHandle_t xTimer);

/*=======================================================================*/
//! External references to callback wrapper functions
/*=======================================================================*/
extern void sl_btc_ble_gap_register_callbacks_wrapper(void);
extern void sl_btc_ble_gap_extended_register_callbacks_wrapper(void);
// TODO: GATT Client init code - will be moved to GATT client component
// extern void rsi_assign_remote_data_serv_and_char(void);

/*=======================================================================*/
//! GAP Module Variables
/*=======================================================================*/
uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
TimerHandle_t connect_timeout_timer;

/*=======================================================================*/
/**
 * @fn         ble_gap_init
 * @brief      GAP component initialization function
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 * @section description
 * This function encapsulates ALL GAP initialization including:
 * - GAP callback registration (NOT called from ble_unified_app.c)
 * - Utility function calls (NOT called from ble_unified_app.c)
 * - All GAP-specific setup
 */
int32_t ble_gap_init(void)
{
  int32_t status;
  uint8_t local_dev_addr[RSI_REM_DEV_ADDR_LEN] = { 0 };

  printf("\r\n Initializing GAP Component...\r\n");

  // Step 1: Register GAP callbacks
  // EXTRACTED FROM: ble_init_hook() lines ~1448, ~1456
  printf("\r\n Registering GAP callbacks...\r\n");
  sl_btc_ble_gap_register_callbacks_wrapper();
  sl_btc_ble_gap_extended_register_callbacks_wrapper();

  // Step 2: Initialize connection state
  // EXTRACTED FROM: ble_unified_app.c lines ~1308-1385
  printf("\r\n Initializing connection state...\r\n");
  ble_private_default_init();

  // Step 3: Fill user configuration
  // EXTRACTED FROM: ble_unified_app.c lines ~1274-1292
  printf("\r\n Filling user configuration...\r\n");
  status = rsi_fill_ble_user_config();
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to fill user configuration: 0x%" PRIX32 "\r\n", status);
    return status;
  }
  printf("\r\n User configuration successful\r\n");

  // Step 4: Set local device name
  // EXTRACTED FROM: rsi_ble_dual_role() line ~1522
  printf("\r\n Setting device name...\r\n");
  rsi_bt_set_local_name((void *)RSI_BT_LOCAL_NAME);

  // Step 5: Get local device address
  // EXTRACTED FROM: rsi_ble_dual_role() lines ~1524-1531
  printf("\r\n Getting local device address...\r\n");
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to get local device address: 0x%" PRIX32 "\r\n", status);
  } else {
    rsi_6byte_dev_address_to_ascii(local_dev_addr, rsi_app_resp_get_dev_addr);
    printf("\r\n Local device address = %s\r\n", local_dev_addr);
  }

  // Step 6: Set local IRK value
  // EXTRACTED FROM: rsi_ble_dual_role() lines ~1533-1540
  // This should vary from one device to another. Present RSI doesn't have support of FIXED IRK on every Reset
  uint8_t local_irk[16] = { 0x4d, 0xd7, 0xbd, 0x3e, 0xec, 0x10, 0xda, 0xab,
                            0x1f, 0x85, 0x56, 0xee, 0xa5, 0xc8, 0xe6, 0x93 };
  printf("\r\n Setting the Local IRK Value\r\n");
  status = rsi_ble_set_local_irk_value(local_irk);
  if (status != RSI_SUCCESS) {
    printf("\r\n Setting the Local IRK Value Failed: 0x%" PRIX32 "\r\n", status);
    return status;
  }

  // Step 7: Set coex role priorities
  // EXTRACTED FROM: rsi_ble_dual_role() lines ~1541-1553
  uint8_t role_priority_payload[21] = {
    RSI_COEX_ROLE_BLE_SCAN,           175, 250, RSI_COEX_ROLE_BLE_INIT,      170, 205,
    RSI_COEX_ROLE_BLE_CONNECTION,     20,  220, RSI_COEX_ROLE_BLE_ADVERTISE, 120, 230,
    RSI_COEX_ROLE_BLE_EXT_ADVERTISE,  120, 230, RSI_COEX_ROLE_BLE_EXT_SCAN,  175, 250,
    RSI_COEX_ROLE_BLE_EXT_CONNECTION, 20,  220
  };
  printf("\r\n Setting coex role priorities...\r\n");
  status = rsi_ble_set_coex_roles_priority(role_priority_payload);
  if (status != RSI_SUCCESS) {
    printf("\r\n Setting the coex roles priority Failed: 0x%" PRIX32 "\r\n", status);
    return status;
  } else {
    printf("\r\n Setting the coex roles priority Successful\r\n");
  }

  // Step 8: Initialize remote data service and characteristics (GATT Client)
  // TODO: This will be moved to GATT client component
  // EXTRACTED FROM: rsi_ble_dual_role() line ~1568
  // rsi_assign_remote_data_serv_and_char();

  // Step 9: Setup connection timeout timer
  // EXTRACTED FROM: rsi_ble_dual_role() lines ~1570-1577
  printf("\r\n Creating connection timeout timer...\r\n");
  connect_timeout_timer = xTimerCreate("ConnTimeout",
                                       pdMS_TO_TICKS(10000), // 10 seconds
                                       pdFALSE,              // One-shot
                                       NULL,
                                       connect_timeout_handler);
  if (connect_timeout_timer == NULL) {
    printf("\r\n Failed to create connection timeout timer!\r\n");
    return RSI_FAILURE;
  }

// Step 10: Setup default advertising and scan parameters
// NOTE: For extended advertising, this should be called AFTER ext_adv_init()
// For legacy advertising, this can be called here
// EXTRACTED FROM: rsi_change_ble_adv_and_scan_params() lines ~1474-1498
#if !defined(SL_SI91X_SI917_BLE_ADV_SCAN_TYPE) || (SL_SI91X_SI917_BLE_ADV_SCAN_TYPE != 1)
  printf("\r\n Setting up legacy advertising and scan parameters...\r\n");
  rsi_change_ble_adv_and_scan_params();
#endif

  printf("\r\n GAP Component initialized successfully\r\n");
  return SL_STATUS_OK;
}
