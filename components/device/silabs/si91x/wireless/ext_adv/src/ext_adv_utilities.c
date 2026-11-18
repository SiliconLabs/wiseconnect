/*******************************************************************************
* @file  ext_adv_utilities.c
* @brief Extended Advertising Component Utility Functions
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

/**
 * Include files
 */
#include <stdio.h>
#include <string.h>
#include "rsi_ble.h"
#include "rsi_ble_apis.h"
#include <ext_adv_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "ext_adv.h"
#include "rsi_ble_common_config.h"
#include "rsi_utils.h"

/*=======================================================================*/
//! External variable references (defined in ext_adv_init.c)
/*=======================================================================*/
extern uint8_t adv[BLE_AE_ADV_DATA_LEN];
extern uint32_t rsi_app_resp_max_no_of_supp_adv_sets;
extern uint32_t rsi_app_resp_max_adv_data_len;
extern int8_t rsi_app_resp_tx_power;

/*=======================================================================*/
//! Utility Functions
/*=======================================================================*/

/**
 * @fn         ble_ae_set_1_parameters
 * @brief      Configure advertising parameters for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 750-777
 */
int32_t ble_ae_set_1_parameters(void)
{
  int32_t status;
  rsi_ble_ae_adv_params_t ble_ae_params = { 0 };
  uint8_t peer_addr[RSI_DEV_ADDR_LEN]   = { 0 };

  ble_ae_params.adv_handle                = BLE_AE_ADV_HNDL_SET_1;
  ble_ae_params.adv_event_prop            = BLE_AE_ADV_EVNT_PROP_SET_1;
  ble_ae_params.primary_adv_intterval_min = BLE_AE_ADV_INT_MIN_SET_1;
  ble_ae_params.primary_adv_intterval_max = BLE_AE_ADV_INT_MAX_SET_1;
  ble_ae_params.primary_adv_chnl_map      = BLE_AE_ADV_CHANNEL_MAP_SET_1;
  ble_ae_params.own_addr_type             = BLE_AE_OWN_ADDR_TYPE_SET_1;
  ble_ae_params.peer_addr_type            = BLE_AE_PEER_ADDR_TYPE_SET_1;
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)peer_addr, (int8_t *)BLE_AE_PEER_DEV_ADDR_SET_1);
  memcpy(ble_ae_params.peer_dev_addr, peer_addr, RSI_DEV_ADDR_LEN);
  ble_ae_params.adv_filter_policy      = BLE_AE_ADV_FILTER_POLICY_SET_1;
  ble_ae_params.adv_tx_power           = BLE_AE_ADV_TX_PWR_SET_1;
  ble_ae_params.primary_adv_phy        = BLE_AE_PRIMARY_ADV_PHY_SET_1;
  ble_ae_params.sec_adv_max_skip       = BLE_AE_SEC_ADV_MAX_SKIP_SET_1;
  ble_ae_params.sec_adv_phy            = BLE_AE_SECONDARY_ADV_PHY_SET_1;
  ble_ae_params.adv_sid                = BLE_AE_ADV_SID_SET_1;
  ble_ae_params.scan_req_notify_enable = BLE_AE_SCAN_REQ_NOTIF_EN_SET_1;

  // SAPI function call for setting ae parameters
  status = rsi_ble_set_ae_params(&ble_ae_params, &rsi_app_resp_tx_power);
  return status;
}

/**
 * @fn         ble_ae_set_2_parameters
 * @brief      Configure advertising parameters for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 790-817
 */
int32_t ble_ae_set_2_parameters(void)
{
  int32_t status;
  rsi_ble_ae_adv_params_t ble_ae_params = { 0 };
  uint8_t peer_addr[RSI_DEV_ADDR_LEN]   = { 0 };

  ble_ae_params.adv_handle                = BLE_AE_ADV_HNDL_SET_2;
  ble_ae_params.adv_event_prop            = BLE_AE_ADV_EVNT_PROP_SET_2;
  ble_ae_params.primary_adv_intterval_min = BLE_AE_ADV_INT_MIN_SET_2;
  ble_ae_params.primary_adv_intterval_max = BLE_AE_ADV_INT_MAX_SET_2;
  ble_ae_params.primary_adv_chnl_map      = BLE_AE_ADV_CHANNEL_MAP_SET_2;
  ble_ae_params.own_addr_type             = BLE_AE_OWN_ADDR_TYPE_SET_2;
  ble_ae_params.peer_addr_type            = BLE_AE_PEER_ADDR_TYPE_SET_2;
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)peer_addr, (int8_t *)BLE_AE_PEER_DEV_ADDR_SET_2);
  memcpy(ble_ae_params.peer_dev_addr, peer_addr, RSI_DEV_ADDR_LEN);
  ble_ae_params.adv_filter_policy      = BLE_AE_ADV_FILTER_POLICY_SET_2;
  ble_ae_params.adv_tx_power           = BLE_AE_ADV_TX_PWR_SET_2;
  ble_ae_params.primary_adv_phy        = BLE_AE_PRIMARY_ADV_PHY_SET_2;
  ble_ae_params.sec_adv_max_skip       = BLE_AE_SEC_ADV_MAX_SKIP_SET_2;
  ble_ae_params.sec_adv_phy            = BLE_AE_SECONDARY_ADV_PHY_SET_2;
  ble_ae_params.adv_sid                = BLE_AE_ADV_SID_SET_2;
  ble_ae_params.scan_req_notify_enable = BLE_AE_SCAN_REQ_NOTIF_EN_SET_2;

  // SAPI function call for setting ae parameters
  status = rsi_ble_set_ae_params(&ble_ae_params, &rsi_app_resp_tx_power);
  return status;
}

/**
 * @fn         ble_ae_set_1_adv_data
 * @brief      Set advertising data for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 939-955
 */
int32_t ble_ae_set_1_adv_data(void)
{
  int32_t status;
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_ADV_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_AE_ADV_HNDL_SET_1;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, adv, ble_ae_data.data_len);

  // SAPI function call for setting ae adv data of set 1
  status = rsi_ble_set_ae_data(&ble_ae_data);
  return status;
}

/**
 * @fn         ble_ae_set_2_adv_data
 * @brief      Set advertising data for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 968-984
 */
int32_t ble_ae_set_2_adv_data(void)
{
  int32_t status;
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_ADV_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_AE_ADV_HNDL_SET_2;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, adv, ble_ae_data.data_len);

  // SAPI function call for setting ae adv data of set 2
  status = rsi_ble_set_ae_data(&ble_ae_data);
  return status;
}

/**
 * @fn         ble_ae_set_1_scan_resp_data
 * @brief      Set scan response data for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 997-1013
 */
int32_t ble_ae_set_1_scan_resp_data(void)
{
  int32_t status;
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_SCAN_RSP_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_AE_ADV_HNDL_SET_1;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, adv, ble_ae_data.data_len);

  // SAPI function call for setting ae scan response data of set 1
  status = rsi_ble_set_ae_data(&ble_ae_data);
  return status;
}

/**
 * @fn         ble_ae_set_2_scan_resp_data
 * @brief      Set scan response data for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 1025-1041
 */
int32_t ble_ae_set_2_scan_resp_data(void)
{
  int32_t status;
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_SCAN_RSP_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_AE_ADV_HNDL_SET_2;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, adv, ble_ae_data.data_len);

  // SAPI function call for setting ae scan response data of set 2
  status = rsi_ble_set_ae_data(&ble_ae_data);
  return status;
}

/**
 * @fn         ble_ae_set_periodic_parameters
 * @brief      Set periodic advertising parameters
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 830-842
 */
int32_t ble_ae_set_periodic_parameters(void)
{
  int32_t status;
  rsi_ble_ae_periodic_adv_params_t ae_periodic_param = { 0 };

  ae_periodic_param.max_interval = BLE_AE_PER_ADV_INT_MAX;
  ae_periodic_param.min_interval = BLE_AE_PER_ADV_INT_MIN;
  ae_periodic_param.properties   = BLE_AE_PER_ADV_PROP;
  ae_periodic_param.adv_handle   = BLE_AE_ADV_HNDL_SET_1;

  // SAPI function call for setting periodic advertising parameters
  status = rsi_ble_app_set_periodic_ae_params(&ae_periodic_param);
  return status;
}

/**
 * @fn         ble_ae_set_periodic_data
 * @brief      Set periodic advertising data
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 911-926
 */
int32_t ble_ae_set_periodic_data(void)
{
  int32_t status;
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_PERIODIC_ADV_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_AE_ADV_HNDL_SET_1;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, adv, ble_ae_data.data_len);

  // SAPI function call for setting periodic adv data
  status = rsi_ble_set_ae_data(&ble_ae_data);
  return status;
}

/**
 * @fn         ble_ae_set_1_advertising_enable
 * @brief      Enable/disable advertising for Set 1
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 855-870
 */
int32_t ble_ae_set_1_advertising_enable(void)
{
  int32_t status;
  rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };

  ble_ae_adv.enable        = RSI_BLE_START_ADV;
  ble_ae_adv.no_of_sets    = 1;
  ble_ae_adv.adv_handle    = BLE_AE_ADV_HNDL_SET_1;
  ble_ae_adv.duration      = BLE_AE_ADV_DUR_SET_1;
  ble_ae_adv.max_ae_events = BLE_AE_ADV_MAX_AE_EVENTS_SET_1;

  // SAPI function call for enabling ae adv for set 1
  status = rsi_ble_start_ae_advertising(&ble_ae_adv);
  return status;
}

/**
 * @fn         ble_ae_set_2_advertising_enable
 * @brief      Enable/disable advertising for Set 2
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 883-898
 */
int32_t ble_ae_set_2_advertising_enable(void)
{
  int32_t status;
  rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };

  ble_ae_adv.enable        = RSI_BLE_START_ADV;
  ble_ae_adv.no_of_sets    = 1;
  ble_ae_adv.adv_handle    = BLE_AE_ADV_HNDL_SET_2;
  ble_ae_adv.duration      = BLE_AE_ADV_DUR_SET_2;
  ble_ae_adv.max_ae_events = BLE_AE_ADV_MAX_AE_EVENTS_SET_2;

  // SAPI function call for enabling ae adv for set 2
  status = rsi_ble_start_ae_advertising(&ble_ae_adv);
  return status;
}

/**
 * @fn         ble_ext_scan_params
 * @brief      Set extended scan parameters
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * NOTE: Renamed to ble_ext_scan_params to match reference app
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 1053-1075
 */
int32_t ble_ext_scan_params(void)
{
  int32_t status;

#define SET2_ENABLE 1
  // AE set scan params
  rsi_ble_ae_set_scan_params_t ae_set_scan_params = { 0 };

  ae_set_scan_params.own_addr_type              = BLE_AE_SCAN_OWN_ADDR_TYPE;
  ae_set_scan_params.scanning_filter_policy     = BLE_AE_SCAN_FILTER_TYPE;
  ae_set_scan_params.scanning_phys              = (PHY_1M | PHY_LE_CODED);
  ae_set_scan_params.ScanParams[0].ScanType     = PRI_PHY_BLE_AE_SCAN_TYPE;
  ae_set_scan_params.ScanParams[0].ScanInterval = PRI_PHY_LE_AE_SCAN_INTERVAL;
  ae_set_scan_params.ScanParams[0].ScanWindow   = PRI_PHY_LE_AE_SCAN_WINDOW;
#if SET2_ENABLE
  ae_set_scan_params.ScanParams[1].ScanType     = SEC_PHY_BLE_AE_SCAN_TYPE;
  ae_set_scan_params.ScanParams[1].ScanInterval = SEC_PHY_LE_AE_SCAN_INTERVAL;
  ae_set_scan_params.ScanParams[1].ScanWindow   = SEC_PHY_LE_AE_SCAN_WINDOW;
#endif

  // SAPI function call for setting the scan parameters
  status = rsi_ble_ae_set_scan_params(&ae_set_scan_params);
  return status;
}

/**
 * @fn         ble_ext_scan_enable
 * @brief      Enable/disable extended scanning
 * @return     int32_t - RSI_SUCCESS on success, error code otherwise
 * 
 * NOTE: Renamed to ble_ext_scan_enable to match reference app
 * EXTRACTED FROM: ble_unified_ae_coex_app/app.c lines 1088-1102
 */
int32_t ble_ext_scan_enable(void)
{
  int32_t status;

  // AE scan enable
  rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };

  ae_set_scan_enable.enable            = RSI_BLE_START_SCAN;
  ae_set_scan_enable.filter_duplicates = BLE_AE_SCAN_ENABLE_FILTER_DUP;
  ae_set_scan_enable.duration          = BLE_AE_SCAN_DUR;
  ae_set_scan_enable.period            = BLE_AE_SCAN_PERIOD;

  // SAPI Function call to enable extended scanning
  status = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
  return status;
}
