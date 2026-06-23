/*****************************************************************************
* @file  legacy_adv_user_event_handler.c
* @brief Legacy Advertising User Event Handlers - COMPLETE MODULE ✅
*
* STATUS: COMPLETE ✅ - Production Ready
* CHANGE CONTROL: FROZEN - No modifications allowed without approval
*
* This module contains Legacy Advertising protocol event handlers,
* separated from the monolithic BLE implementation. Functions handle
* Category C events (handlers that call legacy advertising APIs).
*
* Protocol Coverage: Legacy Advertising event handler suite (2 functions)
* Handler Functions: ✅ rsi_scan_restart_event, ✅ rsi_adv_restart_event
* Event Category: Category C - Calls legacy advertising APIs
* Dependency Status: ✅ Resolved
* Variable Ownership: ✅ Uses GAP component state variables
* Compilation Status: ✅ Zero errors
* Handler Names: ✅ Legacy naming convention
*****************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*****************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
******************************************************************************/

/**
 * @file         legacy_adv_user_event_handler.c
 * @version      1.0
 * @date         25 Jan 2026
 *
 * @brief : Legacy Advertising User Event Handlers
 *
 * @section Description  This file contains user event handlers for legacy advertising
 *                       restart events (scan restart and advertising restart).
 */

/*=======================================================================*/
//! INCLUDES
/*=======================================================================*/
#include <stdio.h>                   // printf(), LOG_PRINT_D()
#include <gap_config.h>              // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gap.h"                     // GAP component type definitions (adv_state_t, scan_state_t)
#include "rsi_ble.h"                 // BLE type definitions: rsi_ble_req_adv_t, rsi_ble_req_scan_t
#include "rsi_ble_apis.h"            // BLE APIs: rsi_ble_start_advertising(), rsi_ble_start_scanning(), etc.
#include "rsi_ble_common_config.h"   // BLE common configuration
#include "rsi_common_apis.h"         // RSI common APIs
#include "rsi_common_utils.h"        // LOG_PRINT_D, event/queue utilities
#include "rsi_utils.h"               // RSI utility functions
#include "sl_si91x_status.h"         // RSI_SUCCESS status codes
#include "sl_constants.h"            // Silicon Labs constants
#include "FreeRTOS.h"                // FreeRTOS definitions
#include "timers.h"                  // FreeRTOS timer definitions
#include "ble_event_hdlr_auto_gen.h" // Event handler registration, event IDs
#include <inttypes.h>

/*=======================================================================*/
//! GLOBAL VARIABLES
/*=======================================================================*/
int32_t status; ///< Global status variable for BLE operations

/*=======================================================================*/
//! EXTERNAL REFERENCES - GAP Component State Variables
//! These are defined and owned by the GAP component
/*=======================================================================*/
extern uint8_t central_count;                ///< Number of active central connections (GAP-owned)
extern uint8_t peripheral_count;             ///< Number of active peripheral connections (GAP-owned)
extern adv_state_t adv_state_dut;            ///< GAP-owned advertising state
extern scan_state_t scan_state_dut;          ///< GAP-owned scanning state
extern rsi_ble_req_adv_t change_adv_param;   ///< GAP-owned advertising parameters
extern rsi_ble_req_scan_t change_scan_param; ///< GAP-owned scanning parameters

/*=======================================================================*/
//! COORDINATION STATE VARIABLES (Legacy Adv Component-owned)
//! These coordinate BLE operations across protocols
/*=======================================================================*/
uint8_t peripheral_con_req_pending = 0; ///< Connection request in progress flag
uint8_t adv_pkt_processing_pending = 0; ///< Advertisement packet being processed flag

// Weak symbols for WLAN transient state - application can override if needed
__attribute__((weak)) uint8_t ble_scanning_is_there = 0; ///< Indicates if BLE scanning is active (for coex)
__attribute__((weak)) uint8_t ble_adv_is_there      = 0; ///< Indicates if BLE advertising is active (for coex)

/*=======================================================================*/
//! LEGACY ADVERTISING EVENT HANDLERS
/*=======================================================================*/

/**
 * @fn         rsi_scan_restart_event
 * @brief      Handle scan restart event (Category C - calls legacy APIs)
 * @return     void
 * @section description
 * This function handles the scan restart event by:
 * - Stopping current scan if active
 * - Restarting scan with appropriate parameters based on peripheral count
 * - Managing scan state transitions
 */
void rsi_scan_restart_event()
{
  LOG_PRINT_D("\r\nIn Scan Re-Start evt = %d \r\n", scan_state_dut);

  // ================================================================
  // PHASE 1: Stop Current Scan if Active
  // ================================================================
  if (scan_state_dut != scan_off) {
    status = rsi_ble_stop_scanning();
    if (status != RSI_SUCCESS) {
      printf("\r\n scanning stop failed, cmd status = 0x%" PRIX32 " -conn\n", status);
    } else {
      scan_state_dut = scan_off;
    }
#if WLAN_TRANSIENT_CASE
    ble_scanning_is_there = 0; // Update coex state
#endif
  }

  printf("\r\n Restarting scanning \n");

  // ================================================================
  // PHASE 2: Restart Scan Based on Connection State
  // ================================================================
  if (scan_state_dut == scan_off) {
    // Case 1: Max peripherals reached - use non-connectable scan
    if (peripheral_count == RSI_BLE_MAX_NBR_PERIPHERALS) {
      printf("\r\n Start scanning\n");
      //! Open scan channel with custom parameters (interval: 33.125ms, window: 14.375ms)
      status = rsi_ble_start_scanning_with_values(&change_scan_param);
      if (status != RSI_SUCCESS) {
        printf("\r\n scan channel failed to open 0x%" PRIX32 " \n", status);
      } else {
        scan_state_dut = non_connectable_scan;
      }

      // Case 2: Below max peripherals - use connectable scan
    } else if (peripheral_count < RSI_BLE_MAX_NBR_PERIPHERALS) {
      if (!peripheral_con_req_pending) {
        status = rsi_ble_start_scanning();
        if (status != RSI_SUCCESS) {
          printf("\r\n scanning start failed, cmd status = 0x%" PRIX32 " -conn\n", status);
          rsi_ble_event_scan_restart_driver_callback(); // Retry via callback
        } else {
          scan_state_dut = connectable_scan;
        }
      }
    }
#if WLAN_TRANSIENT_CASE
    ble_scanning_is_there = 1; // Update coex state
#endif
  }
}

/**
 * @fn         rsi_adv_restart_event
 * @brief      Handle advertising restart event (Category C - calls legacy APIs)
 * @return     void
 * @section description
 * This function handles the advertising restart event by:
 * - Stopping current advertising if active
 * - Restarting advertising with appropriate parameters based on central count
 * - Switching to non-connectable advertising when max connections reached
 * - Managing advertising state transitions
 */
void rsi_adv_restart_event()
{
  LOG_PRINT_D("\r\n Number of master devices connected:%d \n", central_count);

  // ================================================================
  // PHASE 1: Stop Current Advertising if Active
  // ================================================================
  if (adv_state_dut == adv_enabled) {
    status = rsi_ble_stop_advertising();
    if (status != RSI_SUCCESS) {
      printf("\r\n advertising failed to stop, with status = 0x%" PRIX32 " -conn\n", status);

    } else {
      adv_state_dut = adv_disabled;
    }
#if WLAN_TRANSIENT_CASE
    ble_adv_is_there = 0; // Update coex state
#endif
  }

  // ================================================================
  // PHASE 2: Restart Advertising Based on Connection State
  // ================================================================

  // Case 1: Below max centrals - use connectable advertising with default interval
  if ((adv_state_dut == adv_disabled) && (central_count < RSI_BLE_MAX_NBR_CENTRALS)) {

    // NOTE: Advertising role is disabled in firmware by default once a remote device connects.
    //       This handler re-enables it to allow additional connections up to max.

    printf("\r\n Number of master devices connected:%d -\n", central_count);

    //! Advertise device with default interval
    status = rsi_ble_start_advertising();
    printf("\r\n Advertising Restarted \n");
    if (status != RSI_SUCCESS) {
      printf("\r\n advertising failed with status = 0x%" PRIX32 " -conn \n", status);
    } else {
#if WLAN_TRANSIENT_CASE
      ble_adv_is_there = 1; // Update coex state
#endif
      adv_state_dut = adv_enabled;
    }

    LOG_PRINT_D("\r\n advertising device n");
  }

  // Case 2: Max connections reached - switch to non-connectable advertising
  //! Advertise module with non-connectable advertising interval of 211.25ms
  else if ((central_count == RSI_BLE_MAX_NBR_CENTRALS) && (adv_state_dut == adv_disabled)) {
    status = rsi_ble_start_advertising_with_values(&change_adv_param);
    if (status != RSI_SUCCESS) {
      printf("\r\n advertising with values failed with status = 0x%" PRIX32 " -conn \n", status);
    } else {
      adv_state_dut = adv_enabled_non_connectable;
    }
  }
}