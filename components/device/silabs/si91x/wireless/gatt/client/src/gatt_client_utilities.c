/*******************************************************************************
* @file  gatt_client_utilities.c
* @brief GATT Client Component - Utility Functions
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

#include <stdio.h>
#include <string.h>
#include <gatt_client_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gatt_client.h"
#include "gap.h"
#include "rsi_ble.h"
#include "rsi_ble_apis.h"
#include "rsi_common_apis.h"

extern rsi_parsed_conf_t rsi_parsed_conf;

/*=======================================================================*/
//! Category 1: Connection State Initialization
/*=======================================================================*/

void rsi_ble_gatt_client_default_init(void)
{
  uint8_t iter;
  for (iter = 0; iter < TOTAL_CONNECTIONS; iter++) {
    rsi_ble_conn_info[iter].mtu_exchange_done            = 0;
    rsi_ble_conn_info[iter].char_resp_recvd              = false;
    rsi_ble_conn_info[iter].char_desc_resp_recvd         = false;
    rsi_ble_conn_info[iter].prof_resp_recvd              = false;
    rsi_ble_conn_info[iter].write_handle_found           = false;
    rsi_ble_conn_info[iter].write_cnt                    = 0;
    rsi_ble_conn_info[iter].write_wwr_handle_found       = false;
    rsi_ble_conn_info[iter].write_handle                 = 0;
    rsi_ble_conn_info[iter].wwr_count                    = 0;
    rsi_ble_conn_info[iter].write_wwr_handle             = 0;
    rsi_ble_conn_info[iter].notify_handle_found          = false;
    rsi_ble_conn_info[iter].notify_handle                = 0;
    rsi_ble_conn_info[iter].indication_handle_found      = false;
    rsi_ble_conn_info[iter].indication_handle            = 0;
    rsi_ble_conn_info[iter].notification_received        = false;
    rsi_ble_conn_info[iter].indication_cnt               = 0;
    rsi_ble_conn_info[iter].notfy_cnt                    = 0;
    rsi_ble_conn_info[iter].l_num_of_services            = 0;
    rsi_ble_conn_info[iter].l_char_property              = 0;
    rsi_ble_conn_info[iter].char_for_serv_cnt            = 0;
    rsi_ble_conn_info[iter].char_desc_cnt                = 0;
    rsi_ble_conn_info[iter].profile_index_for_char_query = 0;
    rsi_ble_conn_info[iter].done_profiles_query          = false;
    rsi_ble_conn_info[iter].no_of_profiles               = 0;
    rsi_ble_conn_info[iter].total_remote_profiles        = 0;
    rsi_ble_conn_info[iter].profs_evt_cnt                = 0;
    rsi_ble_conn_info[iter].prof_evt_cnt                 = 0;
    rsi_ble_conn_info[iter].profiles_endhandle           = 0;
    rsi_ble_conn_info[iter].prof_cnt                     = 0;
    rsi_ble_conn_info[iter].char_cnt                     = 0;
    rsi_ble_conn_info[iter].profile_mem_init             = false;
    rsi_ble_conn_info[iter].service_char_mem_init        = false;
    rsi_ble_conn_info[iter].skip_end_profile             = 0;
    rsi_ble_conn_info[iter].temp1                        = 0;
    rsi_ble_conn_info[iter].temp2                        = 0;
  }
}

/*=======================================================================*/
//! Category 2: Connection Buffer Configuration
/*=======================================================================*/

int8_t rsi_ble_gatt_client_initialize_conn_config(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  if (ble_conn_spec_conf == NULL) {
    return RSI_FAILURE;
  }

  if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
    ble_conn_spec_conf[PERIPHERAL1].profile_discovery    = PROFILE_QUERY_P1;
    ble_conn_spec_conf[PERIPHERAL1].data_transfer        = DATA_TRANSFER_P1;
    ble_conn_spec_conf[PERIPHERAL1].rx_notifications     = RX_NOTIFICATIONS_FROM_P1;
    ble_conn_spec_conf[PERIPHERAL1].rx_indications       = RX_INDICATIONS_FROM_P1;
    ble_conn_spec_conf[PERIPHERAL1].tx_write             = TX_WRITES_TO_P1;
    ble_conn_spec_conf[PERIPHERAL1].tx_write_no_response = TX_WRITES_NO_RESP_TO_P1;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
    ble_conn_spec_conf[PERIPHERAL2].profile_discovery    = PROFILE_QUERY_P2;
    ble_conn_spec_conf[PERIPHERAL2].data_transfer        = DATA_TRANSFER_P2;
    ble_conn_spec_conf[PERIPHERAL2].rx_notifications     = RX_NOTIFICATIONS_FROM_P2;
    ble_conn_spec_conf[PERIPHERAL2].rx_indications       = RX_INDICATIONS_FROM_P2;
    ble_conn_spec_conf[PERIPHERAL2].tx_write             = TX_WRITES_TO_P2;
    ble_conn_spec_conf[PERIPHERAL2].tx_write_no_response = TX_WRITES_NO_RESP_TO_P2;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
    ble_conn_spec_conf[PERIPHERAL3].profile_discovery    = PROFILE_QUERY_P3;
    ble_conn_spec_conf[PERIPHERAL3].data_transfer        = DATA_TRANSFER_P3;
    ble_conn_spec_conf[PERIPHERAL3].rx_notifications     = RX_NOTIFICATIONS_FROM_P3;
    ble_conn_spec_conf[PERIPHERAL3].rx_indications       = RX_INDICATIONS_FROM_P3;
    ble_conn_spec_conf[PERIPHERAL3].tx_write             = TX_WRITES_TO_P3;
    ble_conn_spec_conf[PERIPHERAL3].tx_write_no_response = TX_WRITES_NO_RESP_TO_P3;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 3) {
    ble_conn_spec_conf[PERIPHERAL4].profile_discovery    = PROFILE_QUERY_P4;
    ble_conn_spec_conf[PERIPHERAL4].data_transfer        = DATA_TRANSFER_P4;
    ble_conn_spec_conf[PERIPHERAL4].rx_notifications     = RX_NOTIFICATIONS_FROM_P4;
    ble_conn_spec_conf[PERIPHERAL4].rx_indications       = RX_INDICATIONS_FROM_P4;
    ble_conn_spec_conf[PERIPHERAL4].tx_write             = TX_WRITES_TO_P4;
    ble_conn_spec_conf[PERIPHERAL4].tx_write_no_response = TX_WRITES_NO_RESP_TO_P4;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 4) {
    ble_conn_spec_conf[PERIPHERAL5].profile_discovery    = PROFILE_QUERY_P5;
    ble_conn_spec_conf[PERIPHERAL5].data_transfer        = DATA_TRANSFER_P5;
    ble_conn_spec_conf[PERIPHERAL5].rx_notifications     = RX_NOTIFICATIONS_FROM_P5;
    ble_conn_spec_conf[PERIPHERAL5].rx_indications       = RX_INDICATIONS_FROM_P5;
    ble_conn_spec_conf[PERIPHERAL5].tx_write             = TX_WRITES_TO_P5;
    ble_conn_spec_conf[PERIPHERAL5].tx_write_no_response = TX_WRITES_NO_RESP_TO_P5;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 5) {
    ble_conn_spec_conf[PERIPHERAL6].profile_discovery    = PROFILE_QUERY_P6;
    ble_conn_spec_conf[PERIPHERAL6].data_transfer        = DATA_TRANSFER_P6;
    ble_conn_spec_conf[PERIPHERAL6].rx_notifications     = RX_NOTIFICATIONS_FROM_P6;
    ble_conn_spec_conf[PERIPHERAL6].rx_indications       = RX_INDICATIONS_FROM_P6;
    ble_conn_spec_conf[PERIPHERAL6].tx_write             = TX_WRITES_TO_P6;
    ble_conn_spec_conf[PERIPHERAL6].tx_write_no_response = TX_WRITES_NO_RESP_TO_P6;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 6) {
    ble_conn_spec_conf[PERIPHERAL7].profile_discovery    = PROFILE_QUERY_P7;
    ble_conn_spec_conf[PERIPHERAL7].data_transfer        = DATA_TRANSFER_P7;
    ble_conn_spec_conf[PERIPHERAL7].rx_notifications     = RX_NOTIFICATIONS_FROM_P7;
    ble_conn_spec_conf[PERIPHERAL7].rx_indications       = RX_INDICATIONS_FROM_P7;
    ble_conn_spec_conf[PERIPHERAL7].tx_write             = TX_WRITES_TO_P7;
    ble_conn_spec_conf[PERIPHERAL7].tx_write_no_response = TX_WRITES_NO_RESP_TO_P7;
  }
  if (RSI_BLE_MAX_NBR_PERIPHERALS > 7) {
    ble_conn_spec_conf[PERIPHERAL8].profile_discovery    = PROFILE_QUERY_P8;
    ble_conn_spec_conf[PERIPHERAL8].data_transfer        = DATA_TRANSFER_P8;
    ble_conn_spec_conf[PERIPHERAL8].rx_notifications     = RX_NOTIFICATIONS_FROM_P8;
    ble_conn_spec_conf[PERIPHERAL8].rx_indications       = RX_INDICATIONS_FROM_P8;
    ble_conn_spec_conf[PERIPHERAL8].tx_write             = TX_WRITES_TO_P8;
    ble_conn_spec_conf[PERIPHERAL8].tx_write_no_response = TX_WRITES_NO_RESP_TO_P8;
  }

  if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
    ble_conn_spec_conf[CENTRAL1].profile_discovery    = PROFILE_QUERY_C1;
    ble_conn_spec_conf[CENTRAL1].data_transfer        = DATA_TRANSFER_C1;
    ble_conn_spec_conf[CENTRAL1].rx_notifications     = RX_NOTIFICATIONS_FROM_C1;
    ble_conn_spec_conf[CENTRAL1].rx_indications       = RX_INDICATIONS_FROM_C1;
    ble_conn_spec_conf[CENTRAL1].tx_write             = TX_WRITES_TO_C1;
    ble_conn_spec_conf[CENTRAL1].tx_write_no_response = TX_WRITES_NO_RESP_TO_C1;
  }
  if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
    ble_conn_spec_conf[CENTRAL2].profile_discovery    = PROFILE_QUERY_C2;
    ble_conn_spec_conf[CENTRAL2].data_transfer        = DATA_TRANSFER_C2;
    ble_conn_spec_conf[CENTRAL2].rx_notifications     = RX_NOTIFICATIONS_FROM_C2;
    ble_conn_spec_conf[CENTRAL2].rx_indications       = RX_INDICATIONS_FROM_C2;
    ble_conn_spec_conf[CENTRAL2].tx_write             = TX_WRITES_TO_C2;
    ble_conn_spec_conf[CENTRAL2].tx_write_no_response = TX_WRITES_NO_RESP_TO_C2;
  }

  return RSI_SUCCESS;
}

/*=======================================================================*/
//! Category 3: Application-Level Utilities
/*=======================================================================*/

void rsi_assign_remote_data_serv_and_char(void)
{
  uint8_t i;
  for (i = 0; i < TOTAL_CONNECTIONS; i++) {
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_clientservice_uuid =
      RSI_BLE_CLIENT_WRITE_SERVICE_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_write_client_char_uuid = RSI_BLE_CLIENT_WRITE_CHAR_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_service_uuid =
      RSI_BLE_CLIENT_WRITE_NO_RESP_SERVICE_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].tx_wnr_client_char_uuid =
      RSI_BLE_CLIENT_WRITE_NO_RESP_CHAR_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_service_uuid =
      RSI_BLE_CLIENT_INIDCATIONS_SERVICE_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_indi_client_char_uuid =
      RSI_BLE_CLIENT_INIDCATIONS_CHAR_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_service_uuid =
      RSI_BLE_CLIENT_NOTIFICATIONS_SERVICE_UUID;
    rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config[i].rx_notif_client_char_uuid =
      RSI_BLE_CLIENT_NOTIFICATIONS_CHAR_UUID;

    rsi_ble_conn_info[i].rsi_ble_profile_list_by_conn.profile_desc      = NULL;
    rsi_ble_conn_info[i].rsi_ble_profile_list_by_conn.profile_info_uuid = NULL;
    rsi_ble_conn_info[i].rsi_ble_profile_list_by_conn.profile_char_info = NULL;
  }
}
