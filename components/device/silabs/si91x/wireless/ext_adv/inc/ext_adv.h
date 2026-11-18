/*******************************************************************************
* @file  ext_adv.h
* @brief Extended Advertising Component Public API Header
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

#ifndef EXT_ADV_H
#define EXT_ADV_H

#include <stdint.h>

/*=======================================================================*/
//! Public API Functions
/*=======================================================================*/

/**
 * @fn         ble_ext_adv_init
 * @brief      Initialize Extended Advertising component
 * @return     int32_t - SL_STATUS_OK on success, error code otherwise
 */
int32_t ble_ext_adv_init(void);

/**
 * @fn         ble_ae_set_1_parameters
 * @brief      Configure advertising parameters for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_1_parameters(void);

/**
 * @fn         ble_ae_set_2_parameters
 * @brief      Configure advertising parameters for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_2_parameters(void);

/**
 * @fn         ble_ae_set_1_adv_data
 * @brief      Set advertising data for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_1_adv_data(void);

/**
 * @fn         ble_ae_set_2_adv_data
 * @brief      Set advertising data for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_2_adv_data(void);

/**
 * @fn         ble_ae_set_1_scan_resp_data
 * @brief      Set scan response data for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_1_scan_resp_data(void);

/**
 * @fn         ble_ae_set_2_scan_resp_data
 * @brief      Set scan response data for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_2_scan_resp_data(void);

/**
 * @fn         ble_ae_set_1_advertising_enable
 * @brief      Enable/disable advertising for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_1_advertising_enable(void);

/**
 * @fn         ble_ae_set_2_advertising_enable
 * @brief      Enable/disable advertising for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_2_advertising_enable(void);

/**
 * @fn         ble_ae_set_periodic_parameters
 * @brief      Set periodic advertising parameters
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_periodic_parameters(void);

/**
 * @fn         ble_ae_set_periodic_data
 * @brief      Set periodic advertising data
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ae_set_periodic_data(void);

/**
 * @fn         ble_ext_scan_params
 * @brief      Set extended scan parameters
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ext_scan_params(void);

/**
 * @fn         ble_ext_scan_enable
 * @brief      Enable/disable extended scanning
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 */
int32_t ble_ext_scan_enable(void);

/*=======================================================================*/
//! External Variables (defined in ext_adv_init.c)
/*=======================================================================*/

extern uint8_t adv[BLE_AE_ADV_DATA_LEN];
extern uint32_t rsi_app_resp_max_no_of_supp_adv_sets;
extern uint32_t rsi_app_resp_max_adv_data_len;
extern int8_t rsi_app_resp_tx_power;

// Weak symbols for advertising/scanning state (can be overridden by application)
extern __attribute__((weak)) uint8_t ble_adv_is_there;
extern __attribute__((weak)) uint8_t ble_scanning_is_there;

#endif // EXT_ADV_H
