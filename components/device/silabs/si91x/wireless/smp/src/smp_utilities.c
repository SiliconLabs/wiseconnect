/*******************************************************************************
* @file  smp_utilities.c
* @brief SMP (Security Manager Protocol) Utility Functions
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// Standard C library
#include <stdio.h>   // printf() - debug messages
#include <stdbool.h> // bool type
#include <string.h>  // memcpy(), memcmp() - memory operations

// SMP Component headers
#include <smp_config.h> // SMP configuration - resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "smp.h"        // SMP types and API (includes gap.h)

// BLE SDK headers
#include "rsi_ble.h"            // BLE API types
#include "rsi_bt_common_apis.h" // RSI_FAILURE, RSI_SUCCESS

// BLE SDK Configuration and utilities
#include "rsi_ble_common_config.h" // BLE common configurations
#include "rsi_common_utils.h"      // LOG_PRINT_D, event/queue utilities

/*=======================================================================*/
//! Category 1: Connection State Initialization
//! EXTRACTED FROM: gap_utilities.c:ble_private_default_init()
/*=======================================================================*/

/**
 * @brief Initialize SMP-specific fields in rsi_ble_conn_info[]
 * @details Sets SMP pairing state flags for all connections
 * 
 * EXTRACTED FROM: gap_utilities.c:ble_private_default_init()
 * - Line 548: smp_pairing_initated
 * - Line 549: smp_pairing_request_received
 * - Line 574: smp_done
 */
void rsi_ble_smp_default_init(void)
{
  uint8_t iter;
  for (iter = 0; iter < TOTAL_CONNECTIONS; iter++) {
    rsi_ble_conn_info[iter].smp_pairing_initated         = false;
    rsi_ble_conn_info[iter].smp_pairing_request_received = false;
    rsi_ble_conn_info[iter].smp_done                     = 0;
  }
}

/*=======================================================================*/
//! Category 2: Connection Buffer Configuration
//! EXTRACTED FROM: gap_utilities.c:rsi_ble_initialize_conn_buffer()
/*=======================================================================*/

/**
 * @brief Initialize SMP-specific connection buffer configuration
 * @details Sets SMP enable flags for all connections
 * 
 * EXTRACTED FROM: gap_utilities.c:rsi_ble_initialize_conn_buffer()
 * - Line 291: smp_enable for PERIPHERAL1
 * - Line 311: smp_enable for PERIPHERAL2
 * - ... (for all peripherals/centrals)
 */
int8_t rsi_ble_smp_initialize_conn_config(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  if (ble_conn_spec_conf == NULL) {
    return RSI_FAILURE;
  }

  // Initialize Peripheral SMP enable flags
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
    ble_conn_spec_conf[PERIPHERAL1].smp_enable = SMP_ENABLE_P1;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
    ble_conn_spec_conf[PERIPHERAL2].smp_enable = SMP_ENABLE_P2;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
    ble_conn_spec_conf[PERIPHERAL3].smp_enable = SMP_ENABLE_P3;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 3) {
    ble_conn_spec_conf[PERIPHERAL4].smp_enable = SMP_ENABLE_P4;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 4) {
    ble_conn_spec_conf[PERIPHERAL5].smp_enable = SMP_ENABLE_P5;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 5) {
    ble_conn_spec_conf[PERIPHERAL6].smp_enable = SMP_ENABLE_P6;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 6) {
    ble_conn_spec_conf[PERIPHERAL7].smp_enable = SMP_ENABLE_P7;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 7) {
    ble_conn_spec_conf[PERIPHERAL8].smp_enable = SMP_ENABLE_P8;
  }

  // Initialize Central SMP enable flags
  if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
    ble_conn_spec_conf[CENTRAL1].smp_enable = SMP_ENABLE_C1;
  }
  if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
    ble_conn_spec_conf[CENTRAL2].smp_enable = SMP_ENABLE_C2;
  }

  return RSI_SUCCESS;
}

/*=======================================================================*/
//! Category 3: LTK List Management Functions
//! EXTRACTED FROM: smp_user_event_hdlrs.c
/*=======================================================================*/

/*==============================================*/
/**
 * @fn         add_security_keys_to_device_list
 * @brief      this function will add device to resolvlist with updated IRKs 
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in]  le_sec_keys - Security keys event data
 * @return     int8_t - 0 on success, -1 if device not found
 * @section description
 * add device to resolvlist with updated irks 
 * */
int8_t add_security_keys_to_device_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                        rsi_bt_event_le_security_keys_t le_sec_keys)
{
  uint8_t status = RSI_SUCCESS;
  uint8_t ix;
  for (ix = 0; ix < TOTAL_CONNECTIONS; ix++) {

    if ((ble_dev_ltk_list[ix].used == 1) && (!memcmp(ble_dev_ltk_list[ix].remote_dev_addr, le_sec_keys.dev_addr, 6))
        && (ble_dev_ltk_list[ix].remote_dev_addr_type == le_sec_keys.dev_addr_type)) {
      memcpy(ble_dev_ltk_list[ix].local_irk, le_sec_keys.local_irk, 16);
      memcpy(ble_dev_ltk_list[ix].peer_irk, le_sec_keys.remote_irk, 16);
      memcpy(ble_dev_ltk_list[ix].remote_rand, le_sec_keys.remote_rand, 8);
      memcpy(ble_dev_ltk_list[ix].remote_ltk, le_sec_keys.remote_ltk, 16);
      memcpy(ble_dev_ltk_list[ix].Identity_addr, le_sec_keys.Identity_addr, 6);
      ble_dev_ltk_list[ix].remote_ediv        = le_sec_keys.remote_ediv;
      ble_dev_ltk_list[ix].Identity_addr_type = le_sec_keys.Identity_addr_type;
      break;
    }
  }
  if (ix >= TOTAL_CONNECTIONS) {
    return -1;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         add_device_to_ltk_key_list
 * @brief      this function will add device to the ltk key list
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in]  enc_enabled - Encryption enabled event data
 * @return     int8_t - 0 on success, -1 on failure
 * @section description
 * add device to ltk key list
 * */
int8_t add_device_to_ltk_key_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                  rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  int8_t status = 0;
  uint8_t ix;
  uint8_t index_found = 0;

  for (ix = 0; ix < TOTAL_CONNECTIONS; ix++) {

    if (ble_dev_ltk_list[ix].used == 1) {
      if ((enc_enabled->dev_addr_type > 1) && (!memcmp(enc_enabled->dev_addr, ble_dev_ltk_list[ix].Identity_addr, 6))) {
        index_found = 1;
        printf("index found = %d\n", ix);
        break;
      }

      if ((enc_enabled->dev_addr_type <= 1)
          && (!memcmp(enc_enabled->dev_addr, ble_dev_ltk_list[ix].remote_dev_addr, 6))) {
        index_found = 1;
        printf("index found = %d\n", ix);
        break;
      }
    }

    if (ble_dev_ltk_list[ix].used == 0) {
      ble_dev_ltk_list[ix].used       = 1;
      ble_dev_ltk_list[ix].enc_enable = enc_enabled->enabled;
      ble_dev_ltk_list[ix].sc_enable  = enc_enabled->sc_enable;
      memcpy(ble_dev_ltk_list[ix].remote_dev_addr, enc_enabled->dev_addr, 6);
      memcpy(ble_dev_ltk_list[ix].localltk, enc_enabled->localltk, 16);
      memcpy(ble_dev_ltk_list[ix].localrand, enc_enabled->localrand, 8);
      ble_dev_ltk_list[ix].local_ediv = enc_enabled->localediv;
      break;
    }
  }
  if (index_found == 1) {
    LOG_PRINT_D("\n Add device to LTK list and its index = %d \n", status);
    ble_dev_ltk_list[ix].enc_enable = enc_enabled->enabled;
    ble_dev_ltk_list[ix].sc_enable  = enc_enabled->sc_enable;
    memcpy(ble_dev_ltk_list[ix].remote_dev_addr, enc_enabled->dev_addr, 6);
    memcpy(ble_dev_ltk_list[ix].localltk, enc_enabled->localltk, 16);
    memcpy(ble_dev_ltk_list[ix].localrand, enc_enabled->localrand, 8);
    ble_dev_ltk_list[ix].local_ediv = enc_enabled->localediv;
    index_found                     = 0;
  }
  if (ix >= TOTAL_CONNECTIONS) {
    return -1;
  }
  return status;
}

/*==============================================*/
/**
 * @fn         add_derived_key_to_ltk_list
 * @brief      invoked when security keys event is received
 * @param[in,out] ble_dev_ltk_list - LTK device list pointer
 * @param[in]  ble_ctkd - Cross-Transport Key Derivation event data
 * @return     int32_t - 0 on success, -1 if device not found
 * @section description
 * This callback function adds the derived linkkey to the list based on address
 */
int32_t add_derived_key_to_ltk_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list, rsi_ble_event_ctkd_t *ble_ctkd)
{
  int8_t status = 0;
  int ix;
  for (ix = 0; ix < TOTAL_CONNECTIONS; ix++) {
    if (!(memcmp(ble_dev_ltk_list[ix].remote_dev_addr, ble_ctkd->dev_addr, 6))) {
      memcpy(ble_dev_ltk_list[ix].derived_linkkey, ble_ctkd->key, 16);
      printf("\n bd addr mapped  with inx = %x \n", ix);
      break;
    }
  }
  if (ix == TOTAL_CONNECTIONS) {

    status = -1;
  }
  return status;
}
