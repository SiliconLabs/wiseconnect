/*******************************************************************************
* @file  gatt_client.h
* @brief GATT Client Component - Main Header
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

#ifndef GATT_CLIENT_H
#define GATT_CLIENT_H

#include <stdint.h>
#include "gap.h"          // For rsi_ble_conn_info_t, rsi_ble_conn_config_t definitions
#include "rsi_ble_apis.h" // For sli_profile_dummy_data_t (used by BLE event handler auto-gen and GATT client)

/*=======================================================================*/
//! GATT Client Error Codes
/*=======================================================================*/
#define RSI_ERROR_IN_BUFFER_ALLOCATION   0x4e65 // Error code for buffer allocation failure
#define RSI_END_OF_PROFILE_QUERY         0x4a0a // Error code indicating end of profile query
#define RSI_TO_CHECK_ALL_CMD_IN_PROGRESS 0      // Check flag for commands in progress

/*=======================================================================*/
//! Multi-protocol variables (GAP owns - use extern)
/*=======================================================================*/
extern rsi_ble_conn_info_t rsi_ble_conn_info[];

// GATT Client initialization and configuration functions
int32_t ble_gatt_client_init(void);
void rsi_ble_gatt_client_default_init(void);
int8_t rsi_ble_gatt_client_initialize_conn_config(rsi_ble_conn_config_t *ble_conn_spec_conf);
void rsi_assign_remote_data_serv_and_char(void);

#endif // GATT_CLIENT_H
