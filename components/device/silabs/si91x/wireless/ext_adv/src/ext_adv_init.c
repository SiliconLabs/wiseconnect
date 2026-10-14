/*******************************************************************************
* @file  ext_adv_init.c
* @brief Extended Advertising Component Initialization
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
 * @brief Extended Advertising Component Initialization
 * 
 * This file contains the Extended Advertising initialization function that
 * encapsulates ALL Extended Advertising initialization including:
 * - Callback registration
 * - Advertising data initialization
 * - Advertising set configuration
 * - Scan configuration (if enabled)
 * 
 * EXTRACTION SOURCE: ble_unified_app.c (ble_unified_ae_coex_app)
 * - Lines ~1538-1685: rsi_ble_dual_role() - Extended advertising init portions
 */

#include <stdio.h>
#include <string.h>
#include "rsi_ble_apis.h" // BLE APIs: rsi_ble_get_max_adv_data_len(), rsi_ble_set_ae_set_random_address(), etc.
#include "rsi_utils.h"    // rsi_ascii_dev_address_to_6bytes_rev()
#include "sl_constants.h" // SL_STATUS_OK
#include <gap_config.h>   // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gap.h"
#include <ext_adv_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "ext_adv.h"        // Component API and config

/*=======================================================================*/
//  ! EXTENDED ADVERTISING COMPONENT VARIABLES
//  ! These are specific to Extended Advertising and defined here since
//  ! Extended and Legacy advertising are mutually exclusive
/*=======================================================================*/
uint8_t adv[BLE_AE_ADV_DATA_LEN]              = { 0 };
uint32_t rsi_app_resp_max_no_of_supp_adv_sets = 0;
uint32_t rsi_app_resp_max_adv_data_len        = 0;
int8_t rsi_app_resp_tx_power                  = 0;

// Weak symbols for advertising/scanning state (can be overridden by application)
__attribute__((weak)) uint8_t ble_adv_is_there      = 0;
__attribute__((weak)) uint8_t ble_scanning_is_there = 0;

/*=======================================================================*/
//! External references to callback wrapper functions
/*=======================================================================*/
extern void sl_btc_ble_adv_extended_register_callbacks_wrapper(void);

/*=======================================================================*/
//! NOTE: Utility function prototypes are declared in ext_adv.h
//!       No need for extern declarations here since we include ext_adv.h
/*=======================================================================*/
/**
 * @fn         ble_ext_adv_init
 * @brief      Extended Advertising component initialization function
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 * @section description
 * This function encapsulates ALL Extended Advertising initialization.
 */
int32_t ble_ext_adv_init(void)
{
  int32_t status;
  uint8_t adv_random_address[6] = { 0 };

  printf("\r\n Initializing Extended Advertising Component...\r\n");

  // Step 1: Register Extended Advertising callbacks
  // EXTRACTED FROM: ble_init_hook() line ~1433
  printf("\r\n Registering Extended Advertising callbacks...\r\n");
  sl_btc_ble_adv_extended_register_callbacks_wrapper();
  printf("\r\n Extended Advertising callbacks registered\r\n");

  // Step 2: Initialize advertising data string
  strncpy((char *)adv, BLE_AE_ADV_DATA, BLE_AE_ADV_DATA_LEN);

  // Step 3: Get max advertising data length
  status = rsi_ble_get_max_adv_data_len((uint8_t *)&rsi_app_resp_max_adv_data_len);
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to get max adv data length: 0x%lx\r\n", status);
  } else {
    printf("\r\n Max supported Adv Data length is %ld\r\n", rsi_app_resp_max_adv_data_len);
  }

  // Step 4: Get max number of supported advertising sets
  status = rsi_ble_get_max_no_of_supp_adv_sets((uint8_t *)&rsi_app_resp_max_no_of_supp_adv_sets);
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to get max supported adv sets: 0x%lx\r\n", status);
  } else {
    printf("\r\n Max number of supported Adv sets are %ld\r\n", rsi_app_resp_max_no_of_supp_adv_sets);
  }

#if ADV_ENABLED_DEFAULT

  // ================================================================
  // PHASE 1: Configure Advertising Parameters (All Sets)
  // Reference: app.c lines 1559-1590
  // ================================================================

  // Step 5: Configure advertising parameters for Set 1
  status = ble_ae_set_1_parameters();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae params failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n Setting AE params of set 1 successful and selected TX Power is %d dbm\r\n", rsi_app_resp_tx_power);

#if BLE_AE_PERIODIC_ADV_EN
  // Step 6: Configure periodic advertising parameters (if enabled)
  // EXTRACTED FROM: Reference app line 1566
  status = ble_ae_set_periodic_parameters();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae Periodic adv data failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae periodic adv data success\r\n");

  // Step 7: Enable periodic advertising
  // EXTRACTED FROM: Reference app line 1574
  status = rsi_ble_app_set_periodic_ae_enable(BLE_AE_PER_ADV_EN, BLE_AE_ADV_HNDL_SET_1);
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae Periodic adv enable failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae periodic adv enable success\r\n");
#endif // BLE_AE_PERIODIC_ADV_EN

#if ADV_SET2
  // Step 8: Configure advertising parameters for Set 2
  status = ble_ae_set_2_parameters();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae params failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n Setting AE params of set 2 successful and selected TX Power is %d dbm\r\n", rsi_app_resp_tx_power);
#endif // ADV_SET2

  // ================================================================
  // PHASE 2: Configure Advertising Data (All Sets)
  // Reference: app.c lines 1592-1632
  // NOTE: Periodic data is set here if enabled (line 1593)
  // ================================================================

#if BLE_AE_PERIODIC_ADV_EN
  // Step 9: Configure periodic advertising data
  // EXTRACTED FROM: Reference app line 1593
  status = ble_ae_set_periodic_data();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae adv enable failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae adv enable success\r\n");
#else
  // Step 10: Configure advertising data for Set 1 (when periodic not enabled)
  status = ble_ae_set_1_adv_data();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae adv data for set 1 failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae adv data for set 1 success\r\n");

  // Step 11: Configure scan response data for Set 1
  status = ble_ae_set_1_scan_resp_data();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae scan resp data for set 1 failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae scan resp data for set 1 success\r\n");
#endif // BLE_AE_PERIODIC_ADV_EN

#if ADV_SET2
  // Step 12: Configure advertising data for Set 2
  status = ble_ae_set_2_adv_data();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae adv data for set 2 failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae adv data for set 2 success\r\n");

  // Step 13: Configure scan response data for Set 2
  status = ble_ae_set_2_scan_resp_data();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae scan resp data for set 2 failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae scan resp data for set 2 success\r\n");
#endif // ADV_SET2

  // ================================================================
  // PHASE 3: Set Random Address (ONCE for both sets)
  // CRITICAL: Reference app line 1635 - Called ONLY ONCE!
  // ================================================================

  // Step 14: Set random address (applies to both sets)
  rsi_ascii_dev_address_to_6bytes_rev(adv_random_address, (int8_t *)RSI_BLE_SET_RAND_ADDR);
  status = rsi_ble_set_ae_set_random_address(BLE_AE_ADV_HNDL_SET_1, (uint8_t *)adv_random_address);
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae set random address failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae set random address successful\r\n");

  // ================================================================
  // PHASE 4: Enable Advertising (All Sets)
  // Reference: app.c lines 1642-1658
  // ================================================================

  // Step 15: Enable advertising for Set 1
  status = ble_ae_set_1_advertising_enable();
  if (status != RSI_SUCCESS) {
    printf("\r\n set 1 ae adv enable failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set 1 ae adv enable success\r\n");

#if ADV_SET2
  // Step 13: Enable advertising for Set 2
  status = ble_ae_set_2_advertising_enable();
  if (status != RSI_SUCCESS) {
    printf("\r\n set 2 ae adv enable failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set 2 ae adv enable success\r\n");
#endif // ADV_SET2

#endif // ADV_ENABLED_DEFAULT

#if SCAN_ENABLED_DEFAULT
  // ================================================================
  // PHASE 5: Extended Scanning Configuration
  // Reference: app.c lines 1665-1684
  // ================================================================

  // Step 17: Configure extended scan parameters
  status = ble_ext_scan_params();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae scan params failed with status 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae scan params success\r\n");

  // Step 18: Enable extended scanning
  status = ble_ext_scan_enable();
  if (status != RSI_SUCCESS) {
    printf("\r\n set ae scan enable failed with 0x%lX\r\n", status);
    return status;
  }
  printf("\r\n set ae scan enable success\r\n");

#if WLAN_TRANSIENT_CASE
  ble_scanning_is_there = 1;
#endif

#endif // SCAN_ENABLED_DEFAULT

  // ================================================================
  // PHASE 6: Final Setup
  // Reference: app.c line 1685
  // ================================================================

  // Step 19: Setup advertising/scan parameter structures (must be done AFTER ext adv setup)
  // CRITICAL: This updates the change_adv_param and change_scan_param structures
  // that are used by the application for runtime parameter changes
  // EXTRACTED FROM: Reference app line 1685 - called at END of initialization
  printf("\r\n Setting up advertising and scan parameter structures...\r\n");
  extern void rsi_change_ble_adv_and_scan_params(void);
  rsi_change_ble_adv_and_scan_params();

  printf("\r\n Extended Advertising Component initialized successfully\r\n");
  return SL_STATUS_OK;
}
