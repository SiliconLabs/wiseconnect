/*******************************************************************************
* @file  legacy_adv.h
* @brief Legacy Advertising Component Public API Header
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

#ifndef LEGACY_ADV_H
#define LEGACY_ADV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=======================================================================*/
//! Public API Functions
/*=======================================================================*/

/**
 * @fn         ble_legacy_adv_init
 * @brief      Initialize Legacy Advertising component
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 * 
 * @section Description
 * This function initializes the legacy advertising component including:
 * - Setting up advertising data with device name from configuration
 * - Starting legacy advertising (if ADV_ENABLED_DEFAULT and centrals configured)
 * - Starting scanning (if SCAN_ENABLED_DEFAULT and peripherals configured)
 * 
 * @note This function is called from ble_unified_app.c when legacy advertising
 *       mode is selected (SL_SI91X_SI917_BLE_ADV_SCAN_TYPE != EXTENDED_ADV_SCAN).
 *       Legacy advertising uses GAP callbacks registered in gap_init().
 */
int32_t ble_legacy_adv_init(void);

#ifdef __cplusplus
}
#endif

#endif // LEGACY_ADV_H
