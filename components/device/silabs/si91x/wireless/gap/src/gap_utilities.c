/*******************************************************************************
* @file  gap_utilities.c
* @brief GAP Component Utility Functions
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
 * @brief GAP Component Utility Functions
 * 
 * This file contains GAP utility functions including:
 * - Multi-protocol connection management functions (~350 lines)
 * - Connection buffer & config management functions (~310 lines)
 * - Advertising and scan parameter setup
 * 
 * These functions are used by GAP, SMP, and GATT components.
 * 
 * EXTRACTION SOURCE: ble_unified_app.c
 * - Lines ~448-459:   rsi_get_remote_device_role()
 * - Lines ~505-608:   rsi_check_dev_list_driver()
 * - Lines ~611-636:   rsi_get_ble_conn_id()
 * - Lines ~639-664:   rsi_add_ble_conn_id()
 * - Lines ~1043-1272: rsi_ble_initialize_conn_buffer()
 * - Lines ~1274-1292: rsi_fill_ble_user_config()
 * - Lines ~1308-1385: ble_private_default_init()
 * - Lines ~1474-1498: rsi_change_ble_adv_and_scan_params()
 * - Lines ~474-503:   rsi_assign_remote_data_serv_and_char()
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rsi_common_utils.h"
#include <gap_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gap.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common_apis.h"

/*=======================================================================*/
//! GAP-Owned Multi-Protocol Variables (GAP DEFINES, others extern)
//! These are used by GAP, SMP, and GATT components
/*=======================================================================*/
rsi_ble_conn_info_t rsi_ble_conn_info[TOTAL_CONNECTIONS] = { 0 };
uint8_t RSI_NULL_BLE_ADDR[RSI_REM_DEV_ADDR_LEN]          = { 0 };
rsi_parsed_conf_t rsi_parsed_conf                        = { 0 };
rsi_ble_req_adv_t change_adv_param;
rsi_ble_req_scan_t change_scan_param;

/*=======================================================================*/
//! External variables from other GAP files
/*=======================================================================*/
extern more_data_state_beta_t more_data_state_beta[TOTAL_CONNECTIONS];

/*=======================================================================*/
//! Category 1: Multi-protocol Connection Management Functions (~350 lines)
//! These functions are used by GAP, SMP, and GATT components
/*=======================================================================*/

/**
 * @fn         rsi_get_remote_device_role
 * @brief      Get the role of the remote device (central or peripheral)
 * @param[in]  remote_dev_addr - Remote device address
 * @return     uint8_t - CENTRAL_ROLE or PERIPHERAL_RL
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~448-459
 */
uint8_t rsi_get_remote_device_role(uint8_t *remote_dev_addr)
{
  uint8_t role = CENTRAL_ROLE, i;

  // Loop all structures and if the device addr is matched for peripheral structure, then return peripheral role or else central role
  for (i = 0; i < (RSI_BLE_MAX_NBR_PERIPHERALS); i++) {
    if (memcmp(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN) == 0) {
      return rsi_ble_conn_info[i].remote_device_role;
    }
  }
  return role; // Returning role as central
}

/**
 * @fn         rsi_check_dev_list_driver
 * @brief      Check if a device is already connected or should be connected
 * @param[in]  remote_dev_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  adv_dev_addr - Remote device address
 * @return     uint8_t - PERIPHERAL_FOUND, PERIPHERAL_CONNECTED, or NO_PERIPHERAL_FOUND
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~505-608
 */
uint8_t rsi_check_dev_list_driver(uint8_t *remote_dev_name, uint8_t *adv_dev_addr)
{
  LOG_PRINT_D("\r\n inside rsi_check_dev_list_driver \n");
  uint8_t i                       = 0;
  uint8_t peripheral_device_found = NO_PERIPHERAL_FOUND;

  // These statements are added only to resolve compilation warning, value is unchanged
#if ((CONNECT_OPTION == CONN_BY_NAME) && RSI_DEBUG_EN)
  uint8_t *rem_dev_name        = remote_dev_name;
  uint8_t *advertised_dev_addr = adv_dev_addr;
#elif (CONNECT_OPTION == CONN_BY_NAME)
  UNUSED_PARAMETER(adv_dev_addr);
  uint8_t *rem_dev_name = remote_dev_name;
#else
  uint8_t *advertised_dev_addr = adv_dev_addr;
  UNUSED_PARAMETER(remote_dev_name);
#endif

#if (CONNECT_OPTION == CONN_BY_NAME)
  if ((strcmp((const char *)rem_dev_name, "")) == 0) {
    return peripheral_device_found;
  }

  if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME1)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME2)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME3)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME4)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME5)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME6)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME7)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((strcmp((const char *)rem_dev_name, RSI_REMOTE_DEVICE_NAME8)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else
    return peripheral_device_found;

  // Check if remote device already connected or advertise report received
  if (peripheral_device_found == PERIPHERAL_FOUND) {
    for (i = 0; i < (RSI_BLE_MAX_NBR_PERIPHERALS); i++) {
      if (rsi_ble_conn_info[i].rsi_remote_name != NULL) {
        if (!(strcmp((const char *)rsi_ble_conn_info[i].rsi_remote_name, (const char *)rem_dev_name))) {
          peripheral_device_found = PERIPHERAL_CONNECTED;
#if RSI_DEBUG_EN
          LOG_PRINT_D("\r\n Device %s already connected!!!\r\n", advertised_dev_addr);
#endif
          break;
        }
      }
    }
  }
#else
  if (!strcmp(RSI_BLE_DEV_1_ADDR, (char *)advertised_dev_addr)) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if (!strcmp(RSI_BLE_DEV_2_ADDR, (char *)advertised_dev_addr)) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if (!strcmp(RSI_BLE_DEV_3_ADDR, (char *)advertised_dev_addr)) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if (!strcmp(RSI_BLE_DEV_4_ADDR, (char *)advertised_dev_addr)) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((!strcmp(RSI_BLE_DEV_5_ADDR, (char *)advertised_dev_addr)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((!strcmp(RSI_BLE_DEV_6_ADDR, (char *)advertised_dev_addr)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((!strcmp(RSI_BLE_DEV_7_ADDR, (char *)advertised_dev_addr)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else if ((!strcmp(RSI_BLE_DEV_8_ADDR, (char *)advertised_dev_addr)) == 0) {
    peripheral_device_found = PERIPHERAL_FOUND;
  } else
    return peripheral_device_found;

  // Check if remote device already connected
  if (peripheral_device_found == PERIPHERAL_FOUND) {
    for (i = 0; i < (RSI_BLE_MAX_NBR_PERIPHERALS); i++) {
      if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, advertised_dev_addr, RSI_REM_DEV_ADDR_LEN)) {
        peripheral_device_found = PERIPHERAL_CONNECTED;
#if RSI_DEBUG_EN
        LOG_PRINT_D("\r\n Device %s already connected!!!\r\n", advertised_dev_addr);
#endif
        break;
      }
    }
  }

#endif
  if (i == RSI_BLE_MAX_NBR_PERIPHERALS) {
    peripheral_device_found = PERIPHERAL_NOT_CONNECTED;
  }

  return peripheral_device_found;
}

/**
 * @fn         rsi_get_ble_conn_id
 * @brief      Get connection ID for a remote device, add if not found
 * @param[in]  remote_dev_addr - Remote device address
 * @param[in]  remote_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  size - Size of remote name
 * @return     uint8_t - Connection ID
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~611-636
 */
#if (CONNECT_OPTION == CONN_BY_NAME)
uint8_t rsi_get_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size)
#else
uint8_t rsi_get_ble_conn_id(uint8_t *remote_dev_addr)
#endif
{
  uint8_t conn_id = 0xFF; // Max connections (0xFF -1)
  uint8_t i       = 0;

  for (i = 0; i < (TOTAL_CONNECTIONS); i++) {
    if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN)) {
      conn_id = i;
      break;
    }
  }

  // If bd_addr not found, add to the list
  if (conn_id == 0xFF) {
#if (CONNECT_OPTION == CONN_BY_NAME)
    conn_id = rsi_add_ble_conn_id(remote_dev_addr, remote_name, size);
#else
    conn_id = rsi_add_ble_conn_id(remote_dev_addr);
#endif
  }

  return conn_id;
}

/**
 * @fn         rsi_add_ble_conn_id
 * @brief      Add a new connection ID for a remote device
 * @param[in]  remote_dev_addr - Remote device address
 * @param[in]  remote_name - Remote device name (if CONNECT_OPTION == CONN_BY_NAME)
 * @param[in]  size - Size of remote name
 * @return     uint8_t - Connection ID
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~639-664
 */
#if (CONNECT_OPTION == CONN_BY_NAME)
uint8_t rsi_add_ble_conn_id(uint8_t *remote_dev_addr, uint8_t *remote_name, uint8_t size)
#else
uint8_t rsi_add_ble_conn_id(uint8_t *remote_dev_addr)
#endif
{
  uint8_t conn_id = 0xFF; // Max connections (0xFF -1)
  uint8_t i       = 0;

  for (i = 0; i < (RSI_BLE_MAX_NBR_PERIPHERALS); i++) {
    if (!memcmp(rsi_ble_conn_info[i].remote_dev_addr, RSI_NULL_BLE_ADDR, RSI_REM_DEV_ADDR_LEN)) {
      memcpy(rsi_ble_conn_info[i].remote_dev_addr, remote_dev_addr, RSI_REM_DEV_ADDR_LEN);
#if (CONNECT_OPTION == CONN_BY_NAME)
      rsi_ble_conn_info[i].rsi_remote_name = (uint8_t *)malloc((size + 1) * sizeof(uint8_t));
      memset(rsi_ble_conn_info[i].rsi_remote_name, 0, size + 1);
      memcpy(rsi_ble_conn_info[i].rsi_remote_name, remote_name, size);
#endif
      rsi_ble_conn_info[i].remote_device_role = PERIPHERAL_RL; // Remote device is peripheral
      conn_id                                 = i;
      break;
    }
  }

  rsi_ble_conn_info[conn_id].conn_id = conn_id;

  return conn_id;
}

/*=======================================================================*/
//! Category 2: Connection Buffer & Config Management Functions (~310 lines)
/*=======================================================================*/

/**
 * @fn         rsi_ble_initialize_conn_buffer
 * @brief      Initialize configurations for each connection
 * @param[in]  ble_conn_spec_conf - Connection configuration buffer
 * @return     int8_t - RSI_SUCCESS or RSI_FAILURE
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~1043-1272
 */
int8_t rsi_ble_initialize_conn_buffer(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  LOG_PRINT_D("\r\n init conn buffer function called \n");
  int8_t status = RSI_SUCCESS;
  if (ble_conn_spec_conf != NULL) {
    if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
      // Initialize peripheral1 configurations (GAP-owned only; GATT Server/Client set their fields via component init)
      ble_conn_spec_conf[PERIPHERAL1].add_to_acceptlist                = ADD_TO_ACCEPTLIST_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_int       = CONN_INTERVAL_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_latncy    = CONN_LATENCY_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P1;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
      ble_conn_spec_conf[PERIPHERAL2].add_to_acceptlist                = ADD_TO_ACCEPTLIST_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_int       = CONN_INTERVAL_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_latncy    = CONN_LATENCY_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P2;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
      ble_conn_spec_conf[PERIPHERAL3].add_to_acceptlist                = ADD_TO_ACCEPTLIST_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_int       = CONN_INTERVAL_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_latncy    = CONN_LATENCY_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P3;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 3) {
      ble_conn_spec_conf[PERIPHERAL4].add_to_acceptlist                = ADD_TO_WHITELIST_P4;
      ble_conn_spec_conf[PERIPHERAL4].conn_param_update.conn_int       = CONN_INTERVAL_P4;
      ble_conn_spec_conf[PERIPHERAL4].conn_param_update.conn_latncy    = CONN_LATENCY_P4;
      ble_conn_spec_conf[PERIPHERAL4].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P4;
      ble_conn_spec_conf[PERIPHERAL4].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P4;
      ble_conn_spec_conf[PERIPHERAL4].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P4;
      ble_conn_spec_conf[PERIPHERAL4].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P4;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 4) {
      ble_conn_spec_conf[PERIPHERAL5].add_to_acceptlist                = ADD_TO_WHITELIST_P5;
      ble_conn_spec_conf[PERIPHERAL5].conn_param_update.conn_int       = CONN_INTERVAL_P5;
      ble_conn_spec_conf[PERIPHERAL5].conn_param_update.conn_latncy    = CONN_LATENCY_P5;
      ble_conn_spec_conf[PERIPHERAL5].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P5;
      ble_conn_spec_conf[PERIPHERAL5].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P5;
      ble_conn_spec_conf[PERIPHERAL5].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P5;
      ble_conn_spec_conf[PERIPHERAL5].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P5;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 5) {
      ble_conn_spec_conf[PERIPHERAL6].add_to_acceptlist                = ADD_TO_WHITELIST_P6;
      ble_conn_spec_conf[PERIPHERAL6].conn_param_update.conn_int       = CONN_INTERVAL_P6;
      ble_conn_spec_conf[PERIPHERAL6].conn_param_update.conn_latncy    = CONN_LATENCY_P6;
      ble_conn_spec_conf[PERIPHERAL6].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P6;
      ble_conn_spec_conf[PERIPHERAL6].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P6;
      ble_conn_spec_conf[PERIPHERAL6].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P6;
      ble_conn_spec_conf[PERIPHERAL6].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P6;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 6) {
      ble_conn_spec_conf[PERIPHERAL7].add_to_acceptlist                = ADD_TO_WHITELIST_P7;
      ble_conn_spec_conf[PERIPHERAL7].conn_param_update.conn_int       = CONN_INTERVAL_P7;
      ble_conn_spec_conf[PERIPHERAL7].conn_param_update.conn_latncy    = CONN_LATENCY_P7;
      ble_conn_spec_conf[PERIPHERAL7].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P7;
      ble_conn_spec_conf[PERIPHERAL7].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P7;
      ble_conn_spec_conf[PERIPHERAL7].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P7;
      ble_conn_spec_conf[PERIPHERAL7].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P7;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 7) {
      ble_conn_spec_conf[PERIPHERAL8].add_to_acceptlist                = ADD_TO_WHITELIST_P8;
      ble_conn_spec_conf[PERIPHERAL8].conn_param_update.conn_int       = CONN_INTERVAL_P8;
      ble_conn_spec_conf[PERIPHERAL8].conn_param_update.conn_latncy    = CONN_LATENCY_P8;
      ble_conn_spec_conf[PERIPHERAL8].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P8;
      ble_conn_spec_conf[PERIPHERAL8].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P8;
      ble_conn_spec_conf[PERIPHERAL8].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P8;
      ble_conn_spec_conf[PERIPHERAL8].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P8;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
      ble_conn_spec_conf[CENTRAL1].add_to_acceptlist                = ADD_TO_ACCEPTLIST_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_int       = CONN_INTERVAL_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_latncy    = CONN_LATENCY_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C1;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
      ble_conn_spec_conf[CENTRAL2].add_to_acceptlist                = ADD_TO_ACCEPTLIST_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_int       = CONN_INTERVAL_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_latncy    = CONN_LATENCY_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C2;
    }

    /* Check the Total Number of Buffers allocated.*/
    if ((DLE_BUFFER_COUNT_P1 + DLE_BUFFER_COUNT_P2 + DLE_BUFFER_COUNT_P3 + DLE_BUFFER_COUNT_P4 + DLE_BUFFER_COUNT_P5
         + DLE_BUFFER_COUNT_P6 + DLE_BUFFER_COUNT_P7 + DLE_BUFFER_COUNT_P8 + DLE_BUFFER_COUNT_C1 + DLE_BUFFER_COUNT_C2)
        > RSI_BLE_NUM_CONN_EVENTS) {
      printf("\r\n Total number of per connection buffer count is more than the total number allocated \r\n");
      status = RSI_FAILURE;
    }
  } else {
    printf("\r\n Invalid buffer passed \r\n");
    status = RSI_FAILURE;
  }
  return status;
}

/**
 * @fn         rsi_fill_ble_user_config
 * @brief      Fill BLE user configuration
 * @return     int8_t - RSI_SUCCESS or RSI_FAILURE
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~1274-1292
 */
int8_t rsi_fill_ble_user_config()
{
  LOG_PRINT_D("\r\n fill user function called \n");
  int8_t status = RSI_SUCCESS;
  // Copy protocol selection macros
  rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled = BT_TRUE;

  // Copy ble connection specific configurations
  if ((RSI_BLE_MAX_NBR_CENTRALS > 2) || (RSI_BLE_MAX_NBR_PERIPHERALS > 8)) {
    printf("\r\n number of BLE CENTRALS or BLE PERIPHERALS Given wrong declaration\r\n");
    return RSI_FAILURE;
  }

  LOG_PRINT_D("\r\n inside status rsibleinitconnbuffer \n");
  status = rsi_ble_initialize_conn_buffer((rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);

  return status;
}

/**
 * @fn         ble_private_default_init
 * @brief      Initialize connection info structures to default values
 * @return     void
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~1308-1385
 */
void ble_private_default_init(void)
{
  printf("\r\n private default init\r\n");
  uint8_t iter;
  for (iter = 0; iter < TOTAL_CONNECTIONS; iter++) {
    // GAP-owned only; GATT Server/Client init their fields via rsi_ble_gatt_*_default_init()
    rsi_ble_conn_info[iter].buff_config_done     = false;
    rsi_ble_conn_info[iter].conn_param_req_given = false;
    rsi_ble_conn_info[iter].first_connect        = 0;
    rsi_ble_conn_info[iter].neg_rply             = 0;
    rsi_ble_conn_info[iter].offset               = 0;
    rsi_ble_conn_info[iter].handle               = 0;
    rsi_ble_conn_info[iter].type                 = 0;
    rsi_ble_conn_info[iter].max_data_length      = 0;
    rsi_ble_conn_info[iter].rsi_rx_from_rem_dev  = false;
    rsi_ble_conn_info[iter].rsi_tx_to_rem_dev    = false;

    memset(rsi_ble_conn_info[iter].rsi_connected_dev_addr, 0, sizeof(rsi_ble_conn_info[iter].rsi_connected_dev_addr));
#if (CONNECT_OPTION == CONN_BY_NAME)
    rsi_ble_conn_info[iter].rsi_remote_name = NULL;
#endif
    rsi_ble_conn_info[iter].switch_case_count = 0;
    rsi_ble_conn_info[iter].transmit          = false;
    more_data_state_beta[iter].data_transmit  = 0;
  }
}

/**
 * @fn         rsi_change_ble_adv_and_scan_params
 * @brief      Setup default advertising and scan parameters
 * @return     void
 * 
 * EXTRACTED FROM: ble_unified_app.c lines ~1474-1498
 */
void rsi_change_ble_adv_and_scan_params()
{
  // Update the new scan and advertise parameters
  memset(&change_adv_param, 0, sizeof(rsi_ble_req_adv_t));
  memset(&change_scan_param, 0, sizeof(rsi_ble_req_scan_t));

  // Advertise parameters
  change_adv_param.status           = RSI_BLE_START_ADV;
  change_adv_param.adv_type         = UNDIR_NON_CONN; // Non connectable advertising
  change_adv_param.filter_type      = RSI_BLE_ADV_FILTER_TYPE;
  change_adv_param.direct_addr_type = RSI_BLE_ADV_DIR_ADDR_TYPE;
  change_adv_param.adv_int_min      = RSI_BLE_ADV_INT_MIN; // Advertising interval - 211.25ms
  change_adv_param.adv_int_max      = RSI_BLE_ADV_INT_MAX;
  change_adv_param.own_addr_type    = LE_PUBLIC_ADDRESS;
  change_adv_param.adv_channel_map  = RSI_BLE_ADV_CHANNEL_MAP;
  rsi_ascii_dev_address_to_6bytes_rev(change_adv_param.direct_addr, (int8_t *)RSI_BLE_ADV_DIR_ADDR);

  // Scan parameters
  change_scan_param.status        = RSI_BLE_START_SCAN;
  change_scan_param.scan_type     = SCAN_TYPE_PASSIVE;
  change_scan_param.filter_type   = RSI_BLE_SCAN_FILTER_TYPE;
  change_scan_param.scan_int      = LE_SCAN_INTERVAL; // Scan interval 33.125ms
  change_scan_param.scan_win      = LE_SCAN_WINDOW;   // Scan window 13.375ms
  change_scan_param.own_addr_type = LE_PUBLIC_ADDRESS;
}

// TODO: GATT Client init code - will be moved to GATT client component
// /**
//  * @fn         rsi_assign_remote_data_serv_and_char
//  * @brief      Assign remote data transfer service and characteristic UUIDs
//  * @return     void
//  *
//  * EXTRACTED FROM: ble_unified_app.c lines ~474-503
//  */
// void rsi_assign_remote_data_serv_and_char()
// {
//     // TO-DO: this initialization should be taken care in parsing itself
//     // Assign the remote data transfer service and characteristic UUID's to local buffer
//     for (uint8_t i = 0; i < TOTAL_CONNECTIONS; i++) {
//         // Parsed Connection Init
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_clientservice_uuid =
//             RSI_BLE_CLIENT_WRITE_SERVICE_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_client_char_uuid = RSI_BLE_CLIENT_WRITE_CHAR_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_service_uuid =
//             RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_char_uuid =
//             RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_service_uuid =
//             RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_char_uuid =
//             RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_service_uuid =
//             RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID;
//         rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_char_uuid =
//             RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID;
//     }
// }
