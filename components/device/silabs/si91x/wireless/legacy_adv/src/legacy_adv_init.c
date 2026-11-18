/*******************************************************************************
* @file  legacy_adv_init.c
* @brief Legacy Advertising Component Initialization
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
 * @brief Legacy Advertising Component Initialization
 * 
 * This file contains the Legacy Advertising initialization function that
 * encapsulates ALL Legacy Advertising initialization including:
 * - Advertising data setup with device name
 * - Legacy advertising start (if enabled)
 * - Scanning start (if enabled)
 * 
 * EXTRACTION SOURCE: Task 8 specification (task_08_legacy_adv.md)
 * - Legacy advertising initialization template
 * 
 * NOTE: Legacy advertising uses GAP callbacks (already registered in gap_init).
 *       Event handlers are in legacy_adv_user_event_handler.c
 */

#include <stdio.h>
#include <string.h>
#include "sl_constants.h" // SL_STATUS_OK
#include "rsi_ble_apis.h" // BLE APIs: rsi_ble_set_advertise_data(), rsi_ble_start_advertising(), rsi_ble_start_scanning()
#include <legacy_adv_config.h> // LEGACY_ADV_DATA_MAX_LEN
#include <gap_config.h> // RSI_BT_LOCAL_NAME, RSI_BLE_MAX_NBR_CENTRALS, RSI_BLE_MAX_NBR_PERIPHERALS, ADV_ENABLED_DEFAULT, SCAN_ENABLED_DEFAULT
#include <inttypes.h>

/*=======================================================================*/
/**
 * @fn         ble_legacy_adv_init
 * @brief      Legacy Advertising component initialization function
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 * @section description
 * This function encapsulates ALL Legacy Advertising initialization including:
 * - Setting advertising data with device name from configuration
 * - Starting legacy advertising (if ADV_ENABLED_DEFAULT and centrals configured)
 * - Starting scanning (if SCAN_ENABLED_DEFAULT and peripherals configured)
 * 
 * NOTE: Legacy advertising uses default intervals from rsi_ble_common_config.h
 *       or overridden by legacy_adv_config.h via ble_config.h include chain.
 */
int32_t ble_legacy_adv_init(void)
{
  int32_t status;
  uint8_t adv[LEGACY_ADV_DATA_MAX_LEN] = {
    2,
    1,
    6
  }; // Flags AD: Type=0x01, Len=0x01, Data=0x06 (LE General Discoverable, BR/EDR Not Supported)

  printf("\r\n Initializing Legacy Advertising Component...\r\n");

  // NOTE: Legacy advertising uses GAP callbacks (already registered in gap_init)
  //       Event handlers in legacy_adv_user_event_handler.c handle restart events

  // ================================================================
  // PHASE 1: Configure and Start Legacy Advertising
  // ================================================================
#if ADV_ENABLED_DEFAULT
  if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
    // Step 1: Get device name from configuration
    const char *device_name = (const char *)RSI_BT_LOCAL_NAME;
    uint8_t name_len        = strlen(device_name);

    // Step 2: Build advertising data packet
    // Format: | Flags (3 bytes) | Complete Local Name (length + type + name) |
    // Byte 0-2: Flags AD structure (already set above)
    // Byte 3:   Length of Complete Local Name field (includes AD type byte)
    // Byte 4:   AD Type = Complete Local Name (0x09)
    // Byte 5+:  Device name string
    adv[3] = name_len + 1; // Length = AD type (1 byte) + name string length
    adv[4] = 9;            // AD Type: Complete Local Name (0x09)
    strcpy((char *)&adv[5], device_name);

    // Step 3: Set advertising data to firmware
    printf("\r\n Setting advertising data with device name: %s\r\n", device_name);
    status = rsi_ble_set_advertise_data(adv, name_len + 5); // Total length = Flags (3) + Length (1) + Type (1) + Name
    if (status != RSI_SUCCESS) {
      printf("\r\n Failed to set advertising data: 0x%" PRIX32 "\r\n", status);
      return status;
    }

    // Step 4: Start legacy advertising with default parameters
    // Uses RSI_BLE_ADV_INT_MIN/MAX from legacy_adv_config.h (via ble_config.h)
    printf("\r\n Starting legacy advertising...\r\n");
    status = rsi_ble_start_advertising();
    if (status != RSI_SUCCESS) {
      printf("\r\n Failed to start advertising: 0x%" PRIX32 "\r\n", status);
      return status;
    }
    printf("\r\n Legacy advertising started successfully\r\n");
  }
#endif

  // ================================================================
  // PHASE 2: Start Legacy Scanning (if configured)
  // ================================================================
#if SCAN_ENABLED_DEFAULT
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
    printf("\r\n Starting legacy scanning...\r\n");
    status = rsi_ble_start_scanning();
    if (status != RSI_SUCCESS) {
      printf("\r\n Failed to start scanning: 0x%" PRIX32 "\r\n", status);
      return status;
    }
    printf("\r\n Legacy scanning started successfully\r\n");
  }
#endif

  printf("\r\n Legacy Advertising Component initialized successfully\r\n");
  return SL_STATUS_OK;
}
