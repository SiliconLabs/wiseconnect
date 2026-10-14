/*******************************************************************************
* @file  ble_subtasks.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file    ble_subtasks.c
 * @version 0.1
 * @date    19 Mar 2021

 *  @brief : This file contains example application for BLE DUAL role.
 *  @section Description  This application connects as a Central/Peripheral with l2cap connection.
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/
#include "ble_config.h"
#include "ble_device_info.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common.h"
#include "rsi_bt_common_apis.h"
#include "rsi_common_apis.h"
#include "sl_si91x_status.h"
#include <rsi_ble.h>
#include "sl_constants.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
volatile uint64_t rsi_current_state[TOTAL_CONNECTIONS];

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern volatile uint8_t num_of_conn_centrals, num_of_conn_peripherals;
extern volatile uint16_t rsi_ble_att1_val_hndl;
extern volatile uint16_t rsi_ble_att2_val_hndl;
extern volatile uint16_t rsi_ble_att3_val_hndl;
extern volatile uint32_t ble_app_event_task_map[];
extern volatile uint32_t ble_app_event_task_map1[];
extern volatile uint16_t rsi_disconnect_reason[TOTAL_CONNECTIONS];
extern volatile uint32_t ble_temp_event_map[TOTAL_CONNECTIONS];
extern rsi_ble_conn_info_t rsi_ble_conn_info[];
extern rsi_ble_req_adv_t change_adv_param;
extern rsi_ble_req_scan_t change_scan_param;
extern uint8_t peripheral_connection_in_prgs, peripheral_con_req_pending;
extern uint16_t rsi_scan_in_progress;
extern osThreadId_t ble_app_task_handle[TOTAL_CONNECTIONS];
extern uint8_t central_task_instances, peripheral_task_instances;
extern rsi_ble_t att_list;
extern rsi_ble_t ta_att_list;
extern osSemaphoreId_t ble_conn_sem[TOTAL_CONNECTIONS];
extern osSemaphoreId_t ble_peripheral_conn_sem;

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/
extern int32_t rsi_ble_start_advertising_with_values(const void *rsi_ble_adv);
extern rsi_ble_att_list_t *rsi_gatt_get_attribute_from_list(rsi_ble_t *p_val, uint16_t handle);
extern void rsi_ble_app_set_task_event(uint8_t conn_id, uint32_t event_num);
extern int32_t rsi_ble_gatt_prepare_write_response(uint8_t *dev_addr,
                                                   uint16_t handle,
                                                   uint16_t offset,
                                                   uint16_t length,
                                                   const uint8_t *data);

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

/*==============================================*/
/**
 * @fn         rsi_ble_set_event_based_on_conn
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_set_event_based_on_conn(uint8_t conn_id, uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_task_map[conn_id] |= BIT(event_num);
  } else {
    ble_app_event_task_map1[conn_id] |= BIT((event_num - 32));
  }

  osSemaphoreRelease(ble_conn_sem[conn_id]);
}

/*==============================================*/
/**
 * @fn         rsi_ble_clear_event_based_on_conn
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_clear_event_based_on_conn(uint8_t conn_id, uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_task_map[conn_id] &= ~BIT(event_num);
  } else {
    ble_app_event_task_map1[conn_id] &= ~BIT((event_num - 32));
  }
  return;
}

//! True when the handle is registered in ta_att_list (TA-maintained). The host still receives events and
//! may inspect the written data for application-specific use, but must not send ATT write responses for it.
static bool rsi_ble_is_ta_maintained_attribute(uint16_t handle)
{
  return (rsi_gatt_get_attribute_from_list(&ta_att_list, handle) != NULL);
}

/*==============================================*/
/**
 * @fn         rsi_ble_get_event_based_on_conn
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
int32_t rsi_ble_get_event_based_on_conn(uint8_t conn_id)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_task_map[conn_id] & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_task_map1[conn_id] & (1 << (ix - 32))) {
        return ix;
      }
    }
  }
  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_task_on_conn
 * @brief      This task process each ble connection events
 * @param[in]  parameters - connection specific configurations which are selected at compile/run time
 * @return     none
 * @section description
 * This function process configured ble peripheral and central connections
 */

void rsi_ble_task_on_conn(void *parameters)
{
  bool buff_config_done = false, conn_param_req_given = false;
  bool done_profiles_query          = false;
  bool prof_resp_recvd              = false;
  bool char_resp_recvd              = false;
  bool char_desc_resp_recvd         = false;
  bool write_handle_found           = false;
  bool smp_pairing_initated         = false;
  bool smp_pairing_request_received = false;
  int16_t write_handle              = 0;
  uint16_t write_cnt                = 0;
  bool write_wwr_handle_found       = false;
  int16_t write_wwr_handle          = 0;
  uint16_t wwr_count                = 0;
  bool notify_handle_found          = false;
  uint16_t notify_handle            = 0;
  bool indication_handle_found      = false;
  uint16_t indication_handle        = 0;
  uint16_t notfy_cnt                = 0;
  uint16_t indication_cnt           = 0;
  uint8_t max_data_length           = 0;
  uint8_t no_of_profiles = 0, total_remote_profiles = 0;
  uint8_t l_num_of_services = 0, l_char_property = 0;
  uint8_t profs_evt_cnt = 0, prof_evt_cnt = 0, char_for_serv_cnt = 0, char_desc_cnt = 0;
  uint8_t i = 0, profile_index_for_char_query = 0, temp1 = 0, temp2 = 0;
  (void)char_desc_cnt;
  uint8_t temp_prepare_write_value[250];
  uint16_t temp_prepare_write_value_len            = 0;
  uint8_t prep_write_err                           = 0;
  int32_t status                                   = 0;
  int32_t event_id                                 = 0;
  uint16_t profiles_endhandle                      = 0;
  uuid_t search_serv                               = { 0 };
  uint8_t rsi_connected_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
  uint8_t str_remote_address[RSI_REM_DEV_ADDR_LEN] = { '\0' };
  uint16_t offset                                  = 0;
  uint16_t handle                                  = 0;
  uint8_t type                                     = 0;
  uint8_t read_data1[230]                          = {
                             0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
                             29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
                             58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 72, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
                             87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
                             16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
                             45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 72,
                             74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99
  };
  uint8_t first_connect                                      = 0;
  rsi_bt_event_encryption_enabled_t l_rsi_encryption_enabled = { 0 };
  uint8_t smp_done = 0, mtu_exchange_done = 0;
  uint8_t l_conn_id                    = 0xff;
  rsi_ble_conn_config_t *ble_conn_conf = NULL;
  bool rsi_rx_from_rem_dev             = false;
  bool rsi_tx_to_rem_dev               = false;
  //! flag indicating whether notifications from remote device received or not
  bool notification_received      = false;
  bool profile_mem_init           = false;
  bool service_char_mem_init      = false;
  uint16_t notification_tx_handle = 0;
  bool notifications_enabled      = false;
  bool indications_enabled        = false;

  rsi_ble_profile_list_by_conn_t rsi_ble_profile_list_by_conn;
  //! initialize the structure member variables to NULL
  rsi_ble_profile_list_by_conn.profile_desc      = NULL;
  rsi_ble_profile_list_by_conn.profile_info_uuid = NULL;
  rsi_ble_profile_list_by_conn.profile_char_info = NULL;

  ble_conn_conf   = (rsi_ble_conn_config_t *)parameters;
  max_data_length = ble_conn_conf->buff_mode_sel.max_data_length;
  //! store connection identifier in local variable
  l_conn_id = ble_conn_conf->conn_id;

  //! connection identifier should always be less than the total no. of connections requested
  if (l_conn_id >= TOTAL_CONNECTIONS) {
    SL_DEBUG_LOG_V2(ERROR, "Wrong connection identifier\r\n");
    //! check why this state has occurred
    return;
  }

  //! create semaphore for each connection
  ble_conn_sem[l_conn_id] = osSemaphoreNew(1, 0, NULL);
  if (ble_conn_sem[l_conn_id] == NULL) {
    SL_DEBUG_LOG_V2(ERROR, "Semaphore creation failed for peripheral%d\r\n", l_conn_id);
    return;
  }

  //! if connection is from remote peripheral devices, set advertising event for processing further activities in this connection
  if (rsi_ble_conn_info[l_conn_id].remote_device_role == PERIPHERAL_ROLE) {
    rsi_ble_app_set_task_event(l_conn_id, RSI_APP_EVENT_ADV_REPORT);
  }
  //! if connection is from remote central devices, set connection event for processing further activities in this connection
  else if (rsi_ble_conn_info[l_conn_id].remote_device_role == CENTRAL_ROLE) {
    if (rsi_ble_conn_info[l_conn_id].is_enhanced_conn) {
      rsi_ble_app_set_task_event(l_conn_id, RSI_BLE_ENHC_CONN_EVENT);
    } else {
      rsi_ble_app_set_task_event(l_conn_id, RSI_BLE_CONN_EVENT);
    }
  } else {
    //! nothing to do
  }

  while (1) {
    //! checking for events list
    event_id = rsi_ble_get_event_based_on_conn(l_conn_id);
    if (event_id == -1) {
      osSemaphoreAcquire(ble_conn_sem[l_conn_id], osWaitForever);
      continue;
    }

    //! Check if the event id is requested when not in connected state and clear it
    if (!((event_id == RSI_APP_EVENT_ADV_REPORT) || (event_id == RSI_BLE_CONN_EVENT)
          || (event_id == RSI_BLE_ENHC_CONN_EVENT))) {
      if (rsi_ble_conn_info[l_conn_id].conn_status != 1) {
        //! Check if connection is done or not;
        rsi_ble_clear_event_based_on_conn(l_conn_id, event_id);
      }
    }
    ble_temp_event_map[l_conn_id] |= BIT(event_id);
    switch (event_id) {
      case RSI_APP_EVENT_ADV_REPORT: {
        SL_DEBUG_LOG_V2(INFO, "Advertise report received- conn%d \r\n", l_conn_id);
        //! clear the advertise report event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_ADV_REPORT);

        SL_DEBUG_LOG_V2(INFO, "Connect command - conn%d \r\n", l_conn_id);

        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "Scan stopping failed with status : %ld", status);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
        }
        peripheral_con_req_pending    = 0;
        peripheral_connection_in_prgs = 1;
        status = rsi_ble_connect_with_params(rsi_ble_conn_info[l_conn_id].rsi_app_adv_reports_to_app.dev_addr_type,
                                             (int8_t *)rsi_ble_conn_info[l_conn_id].rsi_app_adv_reports_to_app.dev_addr,
                                             LE_SCAN_INTERVAL_CONN,
                                             LE_SCAN_WINDOW_CONN,
                                             C2P12_CONNECTION_INTERVAL_MAX,
                                             C2P12_CONNECTION_INTERVAL_MIN,
                                             C2P12_CONNECTION_LATENCY,
                                             C2P12_SUPERVISION_TIMEOUT);
        SL_DEBUG_LOG_V2(INFO,
                        "Connecting to device :  %s",
                        (uintptr_t)((int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_addr));
        SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "Connecting failed with status : 0x%lx", status);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          //return status;
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
        }

        osSemaphoreAcquire(ble_peripheral_conn_sem, 10000);

        int32_t temp_event_id = rsi_ble_get_event_based_on_conn(l_conn_id);

        if ((temp_event_id == -1) || (!(temp_event_id & (RSI_BLE_CONN_EVENT | RSI_BLE_ENHC_CONN_EVENT)))) {
          SL_DEBUG_LOG_V2(INFO, "Initiating connect cancel command in -conn%d\r\n", l_conn_id);
          status = rsi_ble_connect_cancel((int8_t *)rsi_ble_conn_info[l_conn_id].rsi_app_adv_reports_to_app.dev_addr);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(INFO, "BLE connect cancel cmd status = %lx\r\n", status);
          } else {
            num_of_conn_peripherals++;
            rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_DISCONN_EVENT);
          }
          peripheral_connection_in_prgs = 0;
        }

      } break;
      case RSI_BLE_CONN_EVENT: {
#if RSI_DEBUG_EN
        //! event invokes when connection was completed
        SL_DEBUG_LOG_V2(INFO, "In on conn evt - task%d\r\n", l_conn_id);
#endif
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_CONN_EVENT);
        rsi_ble_conn_info[l_conn_id].conn_status = 1;
        //! copy connected remote device address in to local buffer
        memcpy(rsi_connected_dev_addr, rsi_ble_conn_info[l_conn_id].conn_event_to_app.dev_addr, RSI_DEV_ADDR_LEN);
        //! start SDP query if profile discovery is enabled in connection specific buffer
        if (ble_conn_conf->profile_discovery) {
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
        }
        //! add device to acceptlist
        if (ble_conn_conf->add_to_acceptlist) {
          //! clear the acceptlist completely
          status = rsi_ble_clear_acceptlist();
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to clear the accept list, error: 0x%lx", status);
            SL_DEBUG_LOG_V2(ERROR, "-conn%d", l_conn_id);
            break;
          }
          status = rsi_ble_addto_acceptlist((int8_t *)rsi_connected_dev_addr, LE_RANDOM_ADDRESS);
          rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to add the device:%s", (uintptr_t)(str_remote_address));
            SL_DEBUG_LOG_V2(ERROR, "to accept list -conn%d", l_conn_id);
            break;
          }
          SL_DEBUG_LOG_V2(INFO, "Device %s", (uintptr_t)(str_remote_address));
          SL_DEBUG_LOG_V2(INFO, "added to accept list - conn%d", l_conn_id);
        }

        //! check if connection is from remote central device
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == CENTRAL_ROLE) {

          SL_DEBUG_LOG_V2(INFO, "Number of central devices connected: %d", num_of_conn_centrals);
          SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);

          if (num_of_conn_centrals < RSI_BLE_MAX_NBR_CENTRALS) {
            //! advertise device with default interval
            status = rsi_ble_start_advertising();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising failed with status = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
              continue;
            }
          }

          //! if max connections reached, advertise module with non connectable advertising interval of 211.25ms
          else if (num_of_conn_centrals == RSI_BLE_MAX_NBR_CENTRALS) {
            status = rsi_ble_start_advertising_with_values(&change_adv_param);
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising with values failed with status = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
              continue;
            }
          }
          SL_DEBUG_LOG_V2(INFO, "Advertising device - conn%d", l_conn_id);
        } else {
          //! assuming that connection is from remote peripheral device

          SL_DEBUG_LOG_V2(INFO, "Number of peripheral devices connected: %d", num_of_conn_peripherals);

          if (rsi_ble_conn_info[l_conn_id].remote_device_role == PERIPHERAL_ROLE) {
            SL_DEBUG_LOG_V2(INFO, "Start scanning - conn%d\r\n", l_conn_id);
            status = rsi_ble_start_scanning();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Scan channel failed to open 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            rsi_scan_in_progress = 1;
          }

          //! do background scan with interval of 33.125ms and window 14.375ms
          else if (num_of_conn_peripherals == RSI_BLE_MAX_NBR_PERIPHERALS) {
            SL_DEBUG_LOG_V2(INFO, "Start scanning - conn%d\r\n", l_conn_id);
            //! open scan channel with interval of 33.125ms, window 14.375ms
            status = rsi_ble_start_scanning_with_values(&change_scan_param);
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Scan channel failed to open 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            rsi_scan_in_progress = 1;
          }
        }

        //! perform MTU exchange with remote device
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr,
                                              (int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_addr);

          status = rsi_ble_mtu_exchange_event(rsi_connected_dev_addr, MAX_MTU_SIZE);
          if (status != 0) {
            SL_DEBUG_LOG_V2(ERROR, "MTU Exchange request failed - conn%d\r\n", l_conn_id);
          }
        } else {
          mtu_exchange_done = 1;
        }
      } break;
      case RSI_BLE_ENHC_CONN_EVENT: {

        SL_DEBUG_LOG_V2(INFO, "In on_enhance_conn evt - conn%d\r\n", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_ENHC_CONN_EVENT);
        rsi_ble_conn_info[l_conn_id].conn_status = 1;
        //! copy connected remote device address in to local buffer
        memcpy(rsi_connected_dev_addr, rsi_ble_conn_info[l_conn_id].rsi_enhc_conn_status.dev_addr, RSI_DEV_ADDR_LEN);

        //! start SDP query if profile discovery is enabled in connection specific buffer
        if (ble_conn_conf->profile_discovery) {
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
        }
        //! add device to acceptlist
        if (ble_conn_conf->add_to_acceptlist) {
          //! clear the acceptlist completely
          status = rsi_ble_clear_acceptlist();
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to clear the accept list, error: 0x%lx", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          }
          status = rsi_ble_addto_acceptlist((int8_t *)rsi_connected_dev_addr, LE_RANDOM_ADDRESS);
          rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to add the device:%s", (uintptr_t)(str_remote_address));
            SL_DEBUG_LOG_V2(ERROR, "to acceptlist - conn%d", l_conn_id);
          }
          SL_DEBUG_LOG_V2(INFO, "Device %s", (uintptr_t)(str_remote_address));
          SL_DEBUG_LOG_V2(INFO, "added to acceptlist -conn%d ", l_conn_id);
        }

        //! check if connection is from remote central device
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == CENTRAL_ROLE) {

          SL_DEBUG_LOG_V2(INFO, "Number of central devices connected: %d", num_of_conn_centrals);
          SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);

          //! advertise device with default interval
          status = rsi_ble_stop_advertising();
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Advertising failed to stop, with status = 0x%lx", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            //continue;
          }

          if (num_of_conn_centrals < RSI_BLE_MAX_NBR_CENTRALS) {
            //! advertise device with default interval
            status = rsi_ble_start_advertising();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising failed with status = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              //continue;
            }
          }

          //! if max connections reached, advertise module with non connectable advertising interval of 211.25ms
          else if (num_of_conn_centrals == RSI_BLE_MAX_NBR_CENTRALS) {
            status = rsi_ble_start_advertising_with_values(&change_adv_param);
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising with values failed with status = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
              //continue ;
            }
          }
          SL_DEBUG_LOG_V2(INFO, "Advertising device - conn%d", l_conn_id);
        } else {
          //! assuming that connection is from remote peripheral device

          SL_DEBUG_LOG_V2(INFO, "Number of peripheral devices connected: %d", num_of_conn_peripherals);

          if (rsi_ble_conn_info[l_conn_id].remote_device_role == PERIPHERAL_ROLE) {
            SL_DEBUG_LOG_V2(INFO, "Start scanning - conn%d\r\n", l_conn_id);
            status = rsi_ble_start_scanning();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Scan channel failed to open 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            rsi_scan_in_progress = 1;
          }

          //! do background scan with interval of 33.125ms and window 14.375ms
          else if (num_of_conn_peripherals == RSI_BLE_MAX_NBR_PERIPHERALS) {
            SL_DEBUG_LOG_V2(INFO, "Start scanning - conn%d\r\n", l_conn_id);
            //! open scan channel with interval of 33.125ms, window 14.375ms
            status = rsi_ble_start_scanning_with_values(&change_scan_param);
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Scan channel failed to open 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            rsi_scan_in_progress = 1;
          }
        }

        //! perform MTU exchange with remote device
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr,
                                              (int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_addr);

          status = rsi_ble_mtu_exchange_event(rsi_connected_dev_addr, MAX_MTU_SIZE);
          if (status != 0) {
            SL_DEBUG_LOG_V2(ERROR, "MTU Exchange request failed - conn%d\r\n", l_conn_id);
          }
        }
      } break;
      case RSI_BLE_MTU_EXCHANGE_INFORMATION: {
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MTU_EXCHANGE_INFORMATION);
        SL_DEBUG_LOG_V2(INFO, "MTU EXCHANGE INFORMATION - in subtask - conn%d\r\n", l_conn_id);
        if ((rsi_ble_conn_info[l_conn_id].mtu_exchange_info.initiated_role == PEER_DEVICE_INITATED_MTU_EXCHANGE)
            && (RSI_BLE_MTU_EXCHANGE_FROM_HOST)) {
          status = rsi_ble_mtu_exchange_resp(rsi_connected_dev_addr, LOCAL_MTU_SIZE);
          //! check for procedure already in progress error
          if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
            rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_MTU_EXCHANGE_INFORMATION);
            SL_DEBUG_LOG_V2(INFO,
                            "rsi_ble_mtu_exchange_resp procedure is already in progress - conn%d \r\n",
                            l_conn_id);
            break;
          }
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "MTU EXCHANGE RESP Failed status : 0x%lx\r\n", status);
          } else {
            SL_DEBUG_LOG_V2(INFO, "MTU EXCHANGE RESP SUCCESS status : 0x%lx\r\n", status);
          }
        }
      } break;

      case RSI_BLE_MORE_DATA_REQ_EVT: {

        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MORE_DATA_REQ_EVT);
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "More data request -conn%d \r\n", l_conn_id);
#endif

        if (rsi_current_state[l_conn_id] & BIT64(RSI_DATA_TRANSMIT_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_DATA_TRANSMIT_EVENT);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_REQ_GATT_PROFILE)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_REQ_GATT_PROFILE);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_PROFILES)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_PROFILES);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_PROFILE)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_PROFILE);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_CHAR_SERVICES)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_CHAR_SERVICES);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_READ_REQ_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_READ_REQ_EVENT);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_READ_REQ_EVENT);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_BUFF_CONF_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_BUFF_CONF_EVENT);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
        }
      } break;
      case RSI_BLE_REQ_GATT_PROFILE: {
        //! Discover profile after mtu exchange
        if (mtu_exchange_done) {
          //! continue profile query only after pairing and that too if smp is selected in configuration
          if (ble_conn_conf->smp_enable) {
            if (!smp_done) {
              break;
            }
          }
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
          //! get remote device profiles
          SL_DEBUG_LOG_V2(INFO, "Remote device profile discovery started - conn%d\r\n", l_conn_id);
          status = rsi_ble_get_profiles_async(rsi_connected_dev_addr, 1, 0xffff, NULL);
          if (status != RSI_SUCCESS) {
            //! check for procedure already in progress error
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_REQ_GATT_PROFILE);
              SL_DEBUG_LOG_V2(INFO,
                              "rsi_ble_get_profiles_async procedure is already in progress - conn%d\r\n",
                              l_conn_id);
              break;
            }
            //! check for buffer full error, which is not expected for this procedure
            else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              SL_DEBUG_LOG_V2(ERROR,
                              "rsi_ble_get_profiles_async failed with buffer full error - conn%d\r\n",
                              l_conn_id);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "get profile async call failed with error code :%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          }
        }
      } break;
      case RSI_BLE_GATT_PROFILES: {
#if RSI_DEBUG_EN
        //! prof_resp_recvd is set to false for every profile query response
        SL_DEBUG_LOG_V2(INFO, "In GATT profiles - conn%d\r\n", l_conn_id);
#endif
        if (!prof_resp_recvd) {
          //! check until completion of first level query
          if (!done_profiles_query) {
#if RSI_DEBUG_EN
            SL_DEBUG_LOG_V2(INFO, "In profile query - conn %d\r\n", l_conn_id);
#endif
            rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
            no_of_profiles = rsi_ble_conn_info[l_conn_id].get_allprofiles.number_of_profiles;
            //! copy the end of handle of last searched profile
            profiles_endhandle =
              *(uint16_t *)(rsi_ble_conn_info[l_conn_id].get_allprofiles.profile_desc[no_of_profiles - 1].end_handle);
#if RSI_DEBUG_EN
            SL_DEBUG_LOG_V2(INFO, "Profiles end handle = 0x%x", profiles_endhandle);
            SL_DEBUG_LOG_V2(INFO, "- conn%d ", l_conn_id);
#endif
            //! allocate memory
            if (rsi_ble_profile_list_by_conn.profile_desc == NULL) {
              rsi_ble_profile_list_by_conn.profile_desc =
                (profile_descriptors_t *)malloc(sizeof(profile_descriptors_t) * no_of_profiles);
              if (rsi_ble_profile_list_by_conn.profile_desc == NULL)
                return;
              memset(rsi_ble_profile_list_by_conn.profile_desc, 0, sizeof(profile_descriptors_t) * no_of_profiles);
            } else {
              void *temp = NULL;
              //rsi_ble_profile_list_by_conn.profile_desc = (profile_descriptors_t *)realloc(rsi_ble_profile_list_by_conn.profile_desc, sizeof(profile_descriptors_t) * (total_remote_profiles + no_of_profiles));
              temp = (void *)malloc(sizeof(profile_descriptors_t) * (total_remote_profiles + no_of_profiles));
              if (temp == NULL) {
                SL_DEBUG_LOG_V2(
                  ERROR,
                  "Failed to allocate memory for rsi_ble_profile_list_by_conn.profile_desc\r\n, conn_id:%d",
                  l_conn_id);
                return;
              }
              //! fill the allocated buffer with '0'
              memset(temp, 0, sizeof(profile_descriptors_t) * (total_remote_profiles + no_of_profiles));
              //! copy the already stored data in to this buffer
              memcpy(temp,
                     rsi_ble_profile_list_by_conn.profile_desc,
                     (sizeof(profile_descriptors_t) * total_remote_profiles));
              //! free the old buffer which holds data
              free(rsi_ble_profile_list_by_conn.profile_desc);
              //! assign the new buffer to old buffer
              rsi_ble_profile_list_by_conn.profile_desc = temp;
            }
            if (rsi_ble_profile_list_by_conn.profile_desc == NULL) {
              SL_DEBUG_LOG_V2(ERROR,
                              "Failed to allocate memory for rsi_ble_profile_list_by_conn.profile_desc\r\n, conn_id:%d",
                              l_conn_id);
              return;
            }
            //! copy retrieved profiles in local central buffer
            for (i = 0; i < no_of_profiles; i++) {
              memcpy(&rsi_ble_profile_list_by_conn.profile_desc[i + total_remote_profiles],
                     &rsi_ble_conn_info[l_conn_id].get_allprofiles.profile_desc[i],
                     sizeof(profile_descriptors_t));
            }
            total_remote_profiles += no_of_profiles;

            //! check for end of profiles
            if (profiles_endhandle != 0xffff) {
              status = rsi_ble_get_profiles_async(rsi_connected_dev_addr, profiles_endhandle + 1, 0xffff, NULL);
              if (status != RSI_SUCCESS) {
                //! check for procedure already in progress error
                if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
                  rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_GATT_PROFILES);
#if RSI_DEBUG_EN
                  SL_DEBUG_LOG_V2(INFO,
                                  "rsi_ble_get_profiles_async procedure is already in progress - conn%d ",
                                  l_conn_id);
#endif
                  break;
                }
                //! check for buffer full error, which is not expected for this procedure
                else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
                  SL_DEBUG_LOG_V2(ERROR,
                                  "rsi_ble_get_profiles_async failed with buffer full error - conn%d\r\n",
                                  l_conn_id);
                  break;
                } else {
                  SL_DEBUG_LOG_V2(ERROR, "Get profile async call failed with error code :%lx", status);
                  SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
                }
              }
            } else {
              //! first level profile query completed
              done_profiles_query = true;
              //set event to start second level profile query
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
            }
          } else {
            prof_resp_recvd = true;
            //! check until completion of second level profiles query
            if (profs_evt_cnt < total_remote_profiles) {
              //! search handles for all retrieved profiles
              search_serv.size = rsi_ble_profile_list_by_conn.profile_desc[profs_evt_cnt].profile_uuid.size;
              if (search_serv.size == 2) //! check for 16 bit(2 bytes) UUID value
              {
                search_serv.val.val16 = rsi_ble_profile_list_by_conn.profile_desc[profs_evt_cnt].profile_uuid.val.val16;
#if RSI_DEBUG_EN
                SL_DEBUG_LOG_V2(INFO, "Search for profile :0x%x", search_serv.val.val16);
                SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
#endif
              } else if (search_serv.size == 4) {
                search_serv.val.val32 = rsi_ble_profile_list_by_conn.profile_desc[profs_evt_cnt].profile_uuid.val.val32;
#if RSI_DEBUG_EN
                SL_DEBUG_LOG_V2(INFO, "Search for profile :0x%x", search_serv.val.val32);
                SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
#endif
              } else if (search_serv.size == 16) //! 128 bit(16 byte) UUID value
              {
                search_serv.val.val128 =
                  rsi_ble_profile_list_by_conn.profile_desc[profs_evt_cnt].profile_uuid.val.val128;
#if RSI_DEBUG_EN
                SL_DEBUG_LOG_V2(INFO, "Search for profile :0x%x", search_serv.val.val128);
                SL_DEBUG_LOG_V2(INFO, "- conn%d ", l_conn_id);
#endif
              }
              status = rsi_ble_get_profile_async(rsi_connected_dev_addr, search_serv, NULL);
              if (status != RSI_SUCCESS) {
                //! check for procedure already in progress error
                if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
                  prof_resp_recvd = false;
                  rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_GATT_PROFILES);
#if RSI_DEBUG_EN
                  SL_DEBUG_LOG_V2(INFO,
                                  "rsi_ble_get_profile_async procedure is already in progress - conn%d",
                                  l_conn_id);
#endif
                  break;
                }
                //! check for buffer full error, which is not expected for this procedure
                else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
                  SL_DEBUG_LOG_V2(ERROR,
                                  "rsi_ble_get_profiles_async failed with buffer full error - conn%d\r\n",
                                  l_conn_id);
                  break;
                } else {
                  SL_DEBUG_LOG_V2(ERROR, "Get profile async call failed with error code :%lx", status);
                  SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
                }
              } else {
                profs_evt_cnt++;
              }
            } else {
              //! second level of profile query completed
              rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
            }
          }
        }

      } break;
      case RSI_BLE_GATT_PROFILE: {
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In gatt profile - conn%d\r\n", l_conn_id);
#endif
        if (!profile_mem_init) {
          if (rsi_ble_profile_list_by_conn.profile_info_uuid == NULL) {
            rsi_ble_profile_list_by_conn.profile_info_uuid = (rsi_ble_event_profile_by_uuid_t *)malloc(
              sizeof(rsi_ble_event_profile_by_uuid_t) * total_remote_profiles);
          }
          if (rsi_ble_profile_list_by_conn.profile_info_uuid == NULL) {
            SL_DEBUG_LOG_V2(ERROR,
                            "Failed to allocate memory for rsi_ble_profile_list_by_conn[%d].profile_info_uuid",
                            l_conn_id);
            return;
          }
          memset(rsi_ble_profile_list_by_conn.profile_info_uuid,
                 0,
                 sizeof(rsi_ble_event_profile_by_uuid_t) * total_remote_profiles);
          profile_mem_init = true;
        }
        //copy total searched profiles in local buffer
        if (prof_evt_cnt < total_remote_profiles) {
          //! clear the served event
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
          //! copy to central buffer
          memcpy(&rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt],
                 &rsi_ble_conn_info[l_conn_id].get_profile,
                 sizeof(rsi_ble_event_profile_by_uuid_t));
          prof_resp_recvd = false;
#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO,
                          "GATT profile:\nStart handle: 0x%x",
                          *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt].start_handle);
          SL_DEBUG_LOG_V2(INFO,
                          "\nEnd handle:0x%x ",
                          *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt].end_handle);
          SL_DEBUG_LOG_V2(INFO, "- conn%d ", l_conn_id);
#endif
          prof_evt_cnt++;
        } else {
#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "Completed GATT profile - conn%d \r\n", l_conn_id);
#endif
          if (!char_resp_recvd) {
            if (profile_index_for_char_query < total_remote_profiles) {
              char_resp_recvd = true;
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(
                INFO,
                "Search for profile characteristics :0x%x",
                rsi_ble_profile_list_by_conn.profile_desc[profile_index_for_char_query].profile_uuid.val.val16);
              SL_DEBUG_LOG_V2(INFO, "- conn%d ", l_conn_id);
#endif
              uint16_t start_handle =
                *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[profile_index_for_char_query].start_handle;
              uint16_t end_handle =
                *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[profile_index_for_char_query].end_handle;

              //! Get characteristic services of searched profile
              status = rsi_ble_get_char_services_async(rsi_connected_dev_addr, start_handle, end_handle, NULL);
              if (status != RSI_SUCCESS) {
                //! check for procedure already in progress error
                if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
                  char_resp_recvd = false;
                  rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_GATT_PROFILE);
#if RSI_DEBUG_EN
                  SL_DEBUG_LOG_V2(INFO,
                                  "rsi_ble_get_char_services_async procedure is already in progress - conn%d ",
                                  l_conn_id);
#endif
                  break;
                }
                //! check for buffer full error, which is not expected for this procedure
                else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
                  SL_DEBUG_LOG_V2(ERROR,
                                  "rsi_ble_get_char_services_async failed with buffer full error - conn%d ",
                                  l_conn_id);
                  break;
                } else {
                  SL_DEBUG_LOG_V2(
                    ERROR,
                    "Failed to get service characteristics of the remote GATT server UUID:0x%x",
                    rsi_ble_profile_list_by_conn.profile_desc[profile_index_for_char_query].profile_uuid.val.val16);
                  SL_DEBUG_LOG_V2(ERROR, ",error:0x%lx ", status);
                  SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
                }
              }
              profile_index_for_char_query++;
            } else {
              //! discovery of complete characteristics in each profile is completed
              rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
            }
          }
        }
      } break;
      case RSI_BLE_GATT_CHAR_SERVICES: {
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In GATT test:RSI_BLE_GATT_CHAR_SERVICES - conn%d \r\n", l_conn_id);
#endif
        if (!service_char_mem_init) {
          if (rsi_ble_profile_list_by_conn.profile_char_info == NULL) {
            rsi_ble_profile_list_by_conn.profile_char_info =
              (rsi_ble_event_read_by_type1_t *)malloc(sizeof(rsi_ble_event_read_by_type1_t) * total_remote_profiles);
          }
          if (rsi_ble_profile_list_by_conn.profile_char_info == NULL) {
            SL_DEBUG_LOG_V2(ERROR,
                            "Failed to allocate memory for rsi_ble_profile_list_by_conn[%d].profile_char_info ",
                            l_conn_id);
            return;
          }
          memset(rsi_ble_profile_list_by_conn.profile_char_info,
                 0,
                 sizeof(rsi_ble_event_read_by_type1_t) * total_remote_profiles);
          service_char_mem_init = true;
        }

        if (char_for_serv_cnt < total_remote_profiles) {
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
          memcpy(&rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt],
                 &rsi_ble_conn_info[l_conn_id].get_char_services,
                 sizeof(rsi_ble_event_read_by_type1_t));
          char_for_serv_cnt++;
          char_resp_recvd = false;
        } else {
          if (!char_desc_resp_recvd) {
            char_desc_resp_recvd = true;
            //! search for all characteristics descriptor in all profiles
            if (temp1 < total_remote_profiles) {
              l_num_of_services = rsi_ble_profile_list_by_conn.profile_char_info[temp1].num_of_services;
              ;
              //! search for all characteristics descriptor in each profile
              if (temp2 < l_num_of_services) {
                l_char_property =
                  rsi_ble_profile_list_by_conn.profile_char_info[temp1].char_services[temp2].char_data.char_property;
                if ((l_char_property == RSI_BLE_ATT_PROPERTY_INDICATE)
                    || (l_char_property == RSI_BLE_ATT_PROPERTY_NOTIFY)) {
                  //SL_DEBUG_LOG_V2(INFO, "query for profile service1 %d -conn%d \r\n", temp1);
                  status = rsi_ble_get_att_value_async(
                    rsi_connected_dev_addr,
                    rsi_ble_profile_list_by_conn.profile_char_info[temp1].char_services[temp2].handle,
                    NULL);

                  if (status != RSI_SUCCESS) {
                    //! check for procedure already in progress error
                    if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
                      char_desc_resp_recvd = false;
                      rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_GATT_CHAR_SERVICES);
#if RSI_DEBUG_EN
                      SL_DEBUG_LOG_V2(INFO,
                                      "rsi_ble_get_att_value_async procedure is already in progress - conn%d",
                                      l_conn_id);
#endif
                      break;
                    }
                    //! check for buffer full error, which is not expected for this procedure
                    else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
                      SL_DEBUG_LOG_V2(ERROR,
                                      "rsi_ble_get_att_value_async failed with buffer full error - conn%d",
                                      l_conn_id);
                      break;
                    } else {
                      SL_DEBUG_LOG_V2(
                        ERROR,
                        "Failed to get characteristics descriptor of the remote GATT server with error:0x%lx",
                        status);
                      SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
                    }
                  }
                } else {
                  temp2++;
                  char_desc_resp_recvd = false;
                }
              }
              //! completed characteristic descriptor discovery in required profile
              else if (temp2 == l_num_of_services) {
                temp2                = 0; //!  to start searching from starting of next profile
                char_desc_resp_recvd = false;
                temp1++; //! look for next profile, after completion of searching all characteristic descriptors in one profile
              }
            }
          }
          //! discovering completed for all profiles
          else if (temp1 == total_remote_profiles) {
            rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
            rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_DESC_SERVICES);
          }
        }
      } break;

      case RSI_BLE_GATT_DESC_SERVICES: {
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In GATT test:RSI_BLE_GATT_DESC_SERVICES : temp1 %d", temp1);
        SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
#endif
        if (temp1 < total_remote_profiles) {
          temp2++;
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_DESC_SERVICES);
          char_desc_cnt++;
          char_desc_resp_recvd = false;
        } else {
          SL_DEBUG_LOG_V2(INFO, "Remote device profiles discovery completed - conn%d\r\n", l_conn_id);
          for (i = 0; i < total_remote_profiles; i++) {
            SL_DEBUG_LOG_V2(INFO,
                            "Profile descriptor UUID :0x%x",
                            rsi_ble_profile_list_by_conn.profile_desc[i].profile_uuid.val.val16);
            for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[i].num_of_services; ix++) {
              SL_DEBUG_LOG_V2(
                INFO,
                "Service characteristics UUID 0x%x",
                rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_uuid.val.val16);
            }
          }
          //SL_DEBUG_LOG_V2(INFO, "free bytes remaining after connection1 - %ld ", xPortGetFreeHeapSize());
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_DESC_SERVICES);
          //! if data transfer is configured
          if (ble_conn_conf->data_transfer) {
            rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SELECT_DATATRANSFER);
          }
        }
      } break;
      case RSI_BLE_SELECT_DATATRANSFER: {
        //! clear the event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SELECT_DATATRANSFER);

        //! if receive 'write' is configured, then check for client characteristic handle having Gatt 'write' property
        if (ble_conn_conf->tx_write) {
          //! check for required client service and characteristic UUID
          for (i = 0; i < total_remote_profiles; i++) {
            //! check for configured client service UUID
            if (rsi_ble_profile_list_by_conn.profile_desc[i].profile_uuid.val.val16
                == ble_conn_conf->tx_write_clientservice_uuid) {
              for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[i].num_of_services; ix++) {
                //! check for configured client characteristic UUID
                if (rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_uuid.val.val16
                    == ble_conn_conf->tx_write_client_char_uuid) {
                  //! check for 'write' characteristic property
                  if ((!write_handle_found)
                      && ((rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_property)
                          & RSI_BLE_ATT_PROPERTY_WRITE)) {
                    SL_DEBUG_LOG_V2(INFO, "Write handle found - conn%d \r\n", l_conn_id);
                    write_handle_found = true;
                    write_handle =
                      rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_handle;
                    rsi_tx_to_rem_dev = true;
                    //! configure the buffer configuration mode
                    rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
                    break;
                  }
                }
              }
            }
          }
          if (!write_handle_found) {
            SL_DEBUG_LOG_V2(ERROR, "Client GATT write service not found - conn%d \r\n", l_conn_id);
          }
        }

        //! if receive 'write no response' is configured, then check for client characteristic handle having Gatt 'write No response' property
        else if (ble_conn_conf->tx_write_no_response) {
          //! check for required client service and characteristic UUID
          for (i = 0; i < total_remote_profiles; i++) {
            //! check for configured client service UUID
            if (rsi_ble_profile_list_by_conn.profile_desc[i].profile_uuid.val.val16
                == ble_conn_conf->tx_wnr_client_service_uuid) {
              //! check for configured client service UUID
              for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[i].num_of_services; ix++) {
                //! check for configured client characteristic UUID
                if (rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_uuid.val.val16
                    == ble_conn_conf->tx_wnr_client_char_uuid) {
                  //! check for 'write no response' characteristic property
                  if ((!write_wwr_handle_found)
                      && (rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_property
                          & RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE)) {
#if RSI_DEBUG_EN
                    SL_DEBUG_LOG_V2(INFO, "Write without response handle found - conn%d\r\n", l_conn_id);
#endif
                    write_wwr_handle_found = true; //! write handle found
                    write_wwr_handle =
                      rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_handle;
                    rsi_tx_to_rem_dev = true;
                    rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
                    break;
                  }
                }
              }
            }
          }
          if (!write_wwr_handle_found) {
            SL_DEBUG_LOG_V2(ERROR, "Client GATT write no response service not found - conn%d\r\n", l_conn_id);
          }
        }

        //! if receive 'Indications' is configured, then check for client characteristic handle having Gatt 'Indication' property
        if (ble_conn_conf->rx_indications) {
          //! check for required client service and characteristic UUID
          for (i = 0; i < total_remote_profiles; i++) {
            //! check for configured client service UUID
            if (rsi_ble_profile_list_by_conn.profile_desc[i].profile_uuid.val.val16
                == ble_conn_conf->rx_indi_client_service_uuid) {
              for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[i].num_of_services; ix++) {
                //! check for configured client characteristic UUID
                if (rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_uuid.val.val16
                    == ble_conn_conf->rx_indi_client_char_uuid) {
                  //! check for 'Indication' characteristic property
                  if ((!indication_handle_found)
                      && ((rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_property)
                          & RSI_BLE_ATT_PROPERTY_INDICATE)) {
                    SL_DEBUG_LOG_V2(INFO, "Indicate handle found - conn%d\r\n", l_conn_id);
                    indication_handle_found = true;
                    indication_handle =
                      rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_handle;
                    rsi_rx_from_rem_dev = true;
                    rsi_ble_set_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
                    break;
                  }
                }
              }
            }
          }
          if (!indication_handle_found) {
            SL_DEBUG_LOG_V2(ERROR, "Client GATT Indication service not found - conn%d\r\n", l_conn_id);
          }
        }

        //! if receive 'Notifications' is configured, then check for client characteristic handle having Gatt 'Notification' property
        else if (ble_conn_conf->rx_notifications) {
          //! check for required client service and characteristic UUID
          for (i = 0; i < total_remote_profiles; i++) {
            if (rsi_ble_profile_list_by_conn.profile_desc[i].profile_uuid.val.val16
                == ble_conn_conf->rx_notif_client_service_uuid) {
              //! check for configured client service UUID
              for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[i].num_of_services; ix++) {
                //! check for configured client characteristic UUID
                if (rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_uuid.val.val16
                    == ble_conn_conf->rx_notif_client_char_uuid) {
                  //! check for 'Notification' characteristic property
                  if ((!notify_handle_found)
                      && ((rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_property)
                          & RSI_BLE_ATT_PROPERTY_NOTIFY)) {
                    notify_handle_found = true;
                    notify_handle =
                      rsi_ble_profile_list_by_conn.profile_char_info[i].char_services[ix].char_data.char_handle;
                    rsi_rx_from_rem_dev = true;
                    rsi_ble_set_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
                    break;
                  }
                }
              }
            }
          }
          if (!notify_handle_found) {
            SL_DEBUG_LOG_V2(ERROR, "Client GATT Notification service not found\r\n");
          }
        }

      } break;
      case RSI_BLE_BUFF_CONF_EVENT: {
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In GATT RSI_BLE_BUFF_CONF_EVENT - conn%d\r\n", l_conn_id);
#endif
        if (!buff_config_done) {
          status = rsi_ble_set_wo_resp_notify_buf_info(rsi_connected_dev_addr,
                                                       ble_conn_conf->buff_mode_sel.buffer_mode,
                                                       ble_conn_conf->buff_mode_sel.buffer_cnt);
          if (status != RSI_SUCCESS) {
#if RSI_DEBUG_EN
            SL_DEBUG_LOG_V2(ERROR, "Failed to set the buffer configuration mode, error:0x%x", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
#endif
            break;
          } else {
            buff_config_done = true;
          }
        }
        //! set RSI_DATA_TRANSMIT_EVENT to transmit data to remote device
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
        rsi_ble_set_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
      } break;
      case RSI_CONN_UPDATE_REQ_EVENT: {
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In connection update req event - conn%d\r\n", l_conn_id);
#endif
        if (!conn_param_req_given) {
          //! update connection interval to 45ms, latency 0, supervision timeout 4s
          status = rsi_ble_conn_params_update(rsi_connected_dev_addr,
                                              ble_conn_conf->conn_param_update.conn_int,
                                              ble_conn_conf->conn_param_update.conn_int,
                                              ble_conn_conf->conn_param_update.conn_latncy,
                                              ble_conn_conf->conn_param_update.supervision_to);
          if (status != RSI_SUCCESS) {
            //! check for procedure already in progress error
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              SL_DEBUG_LOG_V2(INFO,
                              "rsi_ble_conn_params_update procedure is already in progress - conn%d\r\n",
                              l_conn_id);
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Failed to update connection parameters error:0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
            break;
          } else {
            SL_DEBUG_LOG_V2(INFO, "Connection params request was successful - conn%d", l_conn_id);
            conn_param_req_given = true;
          }
        }
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
        //! set RSI_DATA_RECEIVE_EVENT to receive data from remote device
        if (rsi_rx_from_rem_dev) {
          rsi_rx_from_rem_dev = false;
          SL_DEBUG_LOG_V2(INFO, "Start receiving data from - conn%d\r\n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
        }
        //! set RSI_DATA_TRANSMIT_EVENT to transmit data to remote device
        if (rsi_tx_to_rem_dev) {
          rsi_tx_to_rem_dev = false;
          SL_DEBUG_LOG_V2(INFO, "Start transmitting data to - conn%d\r\n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
        }
      } break;
      case RSI_BLE_RECEIVE_REMOTE_FEATURES: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_RECEIVE_REMOTE_FEATURES);
        if (ble_conn_conf->smp_enable) {
          if (mtu_exchange_done) {
            if ((!smp_pairing_initated) && (!smp_pairing_request_received)) {
              //! initiating the SMP pairing process
              status = rsi_ble_smp_pair_request(rsi_connected_dev_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_ENABLE);
              if (status != RSI_SUCCESS) {
                SL_DEBUG_LOG_V2(ERROR, "Start of SMP pairing process failed with error code %lx", status);
                SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              } else {
                smp_pairing_initated = true;
                rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
                SL_DEBUG_LOG_V2(INFO, "SMP pairing request initiated to %s", (uintptr_t)(str_remote_address));
                SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
              }
            }
          }
        }
      } break;
      case RSI_BLE_CONN_UPDATE_COMPLETE_EVENT: {
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_CONN_UPDATE_COMPLETE_EVENT);
        SL_DEBUG_LOG_V2(ERROR,
                        "Conn updated device address : %s",
                        (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));
        SL_DEBUG_LOG_V2(ERROR, "conn_interval:%d", rsi_ble_conn_info[l_conn_id].conn_update_resp.conn_interval);
        SL_DEBUG_LOG_V2(ERROR, "supervision timeout:%d", rsi_ble_conn_info[l_conn_id].conn_update_resp.timeout);
        SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
#if UPDATE_CONN_PARAMETERS
        status = rsi_conn_update_request();
#endif
      } break;
      case RSI_BLE_DISCONN_EVENT: {

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_DISCONN_EVENT);
        ble_app_event_task_map[l_conn_id]  = 0;
        ble_app_event_task_map1[l_conn_id] = 0;
        rsi_current_state[l_conn_id]       = 0;
        //! clear the profile data
        free(rsi_ble_profile_list_by_conn.profile_desc);
        free(rsi_ble_profile_list_by_conn.profile_info_uuid);
        free(rsi_ble_profile_list_by_conn.profile_char_info);
        memset(rsi_connected_dev_addr, 0, RSI_DEV_ADDR_LEN);

        //! check whether disconnection is from central
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == CENTRAL_ROLE) {
          SL_DEBUG_LOG_V2(INFO, "Central is disconnected, reason : 0x%x", rsi_disconnect_reason[l_conn_id]);
          SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
          //! decrement the task count
          central_task_instances--;

          if (num_of_conn_centrals < RSI_BLE_MAX_NBR_CENTRALS) {
            status = rsi_ble_stop_advertising();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising failed to stop = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            SL_DEBUG_LOG_V2(INFO, "In dis-conn evt, Start Adv - conn%d", l_conn_id);
            //! Advertising interval set to ~211.25ms with full advertising data payload
            status = rsi_ble_start_advertising();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Advertising failed to start = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
              continue;
            }
            SL_DEBUG_LOG_V2(INFO, "Advertising started - conn%d", l_conn_id);
#if ENABLE_NWP_POWER_SAVE
            SL_DEBUG_LOG_V2(INFO, "Initiate module in to power save ");
            status = rsi_initiate_power_save();
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Failed to keep module in power save ");
            }
#endif
          }
        } else {
          SL_DEBUG_LOG_V2(INFO, "Peripheral is disconnected, reason : 0x%x", rsi_disconnect_reason[l_conn_id]);
          SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
          peripheral_task_instances--;

          //rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
          status = rsi_ble_stop_scanning();
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Scanning stop failed, cmd status = %lx", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          } else {
            rsi_scan_in_progress = 0;
          }
          SL_DEBUG_LOG_V2(INFO, "Restarting scanning ");
          status = rsi_ble_start_scanning();
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Scanning start failed, cmd status = %lx", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          } else {
            rsi_scan_in_progress = 1;
          }
        }
        rsi_disconnect_reason[l_conn_id] = 0;

        SL_DEBUG_LOG_V2(INFO, "Number of connected central devices: %d", num_of_conn_centrals);
        SL_DEBUG_LOG_V2(INFO, "Number of connected peripheral devices: %d", num_of_conn_peripherals);

        memset(&rsi_ble_conn_info[l_conn_id], 0, sizeof(rsi_ble_conn_info_t));

        //! delete the semaphore
        if (ble_conn_sem[l_conn_id]) {
          osSemaphoreDelete(ble_conn_sem[l_conn_id]);
        }
        SL_DEBUG_LOG_V2(INFO, "Delete task%d resources\r\n", l_conn_id);

        //! delete the task
        // Deallocate all threads, mutexes and event handlers
        if (NULL != ble_app_task_handle[l_conn_id]) {
          osThreadTerminate(ble_app_task_handle[l_conn_id]);
          ble_temp_event_map[l_conn_id]  = 0;
          ble_app_task_handle[l_conn_id] = NULL;
        }
        l_conn_id = 0xff;
      } break;
      case RSI_BLE_GATT_WRITE_EVENT: {
        //! event invokes when write/notification events received
        uint16_t write_handle = *(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle);
        //! TA-maintained: ATT completion is handled in the TA. On the host you may read the payload from
        //! app_ble_write_event and use it as needed for your use case; do not call rsi_ble_gatt_write_response
        //! or rsi_ble_att_error_response for this handle.
        //! Host-maintained: mirror into att_list, validate properties, send write response or error, and run
        //! CCC / notify-indicate handling below.
        bool attr_maintained_in_ta = rsi_ble_is_ta_maintained_attribute(write_handle);

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_WRITE_EVENT);

        if (!attr_maintained_in_ta) {
          //! Host-maintained path only from here.
          //! process the received 'write response' data packet
          if (write_handle == rsi_ble_att1_val_hndl) {
            rsi_ble_att_list_t *attribute = NULL;
            uint8_t opcode = 0x12, err = 0x00;
            attribute = rsi_gatt_get_attribute_from_list(&att_list, write_handle);

            //! Check if value has write properties
            if ((attribute != NULL) && (attribute->value != NULL)) {
              if (!(attribute->char_val_prop & 0x08)) //! If no write property, send error response
              {
                err = 0x03; //! Error - Write not permitted
              }
            } else {
              //!Error = No such handle exists
              err = 0x01;
            }

            //! Update the value based on the offset and length of the value
            if ((err == 0) && ((rsi_ble_conn_info[l_conn_id].app_ble_write_event.length) <= attribute->max_value_len)) {
              memset(attribute->value, 0, attribute->max_value_len);

              //! Check if value exists for the handle. If so, maximum length of the value.
              memcpy(attribute->value,
                     rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value,
                     rsi_ble_conn_info[l_conn_id].app_ble_write_event.length);

              //! Update value length
              attribute->value_len = rsi_ble_conn_info[l_conn_id].app_ble_write_event.length;

              SL_DEBUG_LOG_V2(INFO,
                              "Received data from remote device: %s\r\n",
                              (uintptr_t)((uint8_t *)attribute->value));

              status = rsi_ble_gatt_write_response(rsi_connected_dev_addr, 0);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG_V2(ERROR, "Error while sending GATT Write Response : %lx", status);
              } else {
                SL_DEBUG_LOG_V2(INFO, "GATT Write Response sent successfully");
              }
            } else {
              //! Error : 0x07 - Invalid request,  0x0D - Invalid attribute value length
              err = 0x07;
            }

            if (err) {
              //! Send error response
              status = rsi_ble_att_error_response(rsi_connected_dev_addr,
                                                  *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle,
                                                  opcode,
                                                  err);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG_V2(ERROR, "Error while sending ATT Error Response : %lx", status);
              } else {
                SL_DEBUG_LOG_V2(ERROR, "ATT Error Response sent successfully");
              }
            }
          }

          //! print the received 'write no response' data packet
          if (write_handle == rsi_ble_att2_val_hndl) {
            SL_DEBUG_LOG_V2(INFO,
                            "Received data from remote device: %s ",
                            (uintptr_t)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value));
          }

          //! when remote device enabled the notifications
          if ((write_handle - 1) == rsi_ble_att1_val_hndl) {
            notification_tx_handle = write_handle - 1;
            if (ble_conn_conf->tx_notifications) {
              //check for valid notifications
              if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == NOTIFY_ENABLE) {
                SL_DEBUG_LOG_V2(INFO, "Remote device enabled the notification - conn%d", l_conn_id);
                rsi_tx_to_rem_dev     = true;
                notifications_enabled = true;
                //! configure the buffer configuration mode
                rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
              } else if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == NOTIFY_DISABLE) {
                notifications_enabled = false;
                SL_DEBUG_LOG_V2(INFO, "Remote device disabled the notification - conn%d", l_conn_id);
                //rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
              }
            }
          }

          else if ((write_handle - 1) == rsi_ble_att3_val_hndl) {
            if (ble_conn_conf->tx_indications) {
              //check for valid indications
              if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == INDICATION_ENABLE) {
                SL_DEBUG_LOG_V2(INFO, "Remote device enabled the indications - conn%d", l_conn_id);
                rsi_tx_to_rem_dev   = true;
                indications_enabled = true;
                //! configure the buffer configuration mode
                rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
              } else if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == INDICATION_DISABLE) {
                indications_enabled = false;
                SL_DEBUG_LOG_V2(INFO, "Remote device disabled the indications - conn%d", l_conn_id);
                //rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
              }
            }
          }
        }

        //! When the attribute is TA-maintained, the host block above is skipped (no mirror, no write/att error
        //! response). You may still use app_ble_write_event for optional logging or side effects only.

        //! code to handle remote device indications
        //! send acknowledgement to the received indication packet
        if (write_handle == indication_handle) {
          if (ble_conn_conf->rx_indications) {
            SL_DEBUG_LOG_V2(INFO,
                            "Received indication packet from remote device, data= %s",
                            (uintptr_t)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value));
#if RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
            //! Send indication acknowledgment to remote device
            status = rsi_ble_indicate_confirm(rsi_connected_dev_addr);
            if (status != RSI_SUCCESS) {
              SL_DEBUG_LOG_V2(ERROR, "Indication confirm failed \t reason = %lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            } else {
              SL_DEBUG_LOG_V2(INFO, "Indication confirm response sent - conn%d", l_conn_id);
            }
#endif
          }
        }

        //! code to handle remote device notifications
        else if (write_handle == notify_handle) {
          if ((!notification_received) && (ble_conn_conf->rx_notifications)) {
            //! stop printing the logs after receiving first notification
            notification_received = true;
            SL_DEBUG_LOG_V2(INFO, "Receiving notifications from remote device - conn%d", l_conn_id);
          } else {
            //! do nothing as received notifications not required to print
          }
        }
      } break;

      case RSI_BLE_GATT_PREPARE_WRITE_EVENT: {
        SL_DEBUG_LOG_V2(INFO, "PWE\r\n");
        uint16_t prepare_write_handle = *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle;
        //! TA-maintained: TA drives the prepare-write procedure; you may still inspect att_value/offset for app
        //! logic, but do not call rsi_ble_gatt_prepare_write_response or rsi_ble_att_error_response from here.
        //! Host-maintained: validate, accumulate into temp_prepare_write_value, send prepare-write response.
        uint8_t err = 0;
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PREPARE_WRITE_EVENT);
        if (!rsi_ble_is_ta_maintained_attribute(prepare_write_handle)) {
          //! Host path: prepare-write for att1 when value is host-maintained.
          if (prepare_write_handle == rsi_ble_att1_val_hndl) {
            rsi_ble_att_list_t *attribute = NULL;
            uint8_t opcode                = 0x16;
            attribute                     = rsi_gatt_get_attribute_from_list(
              &att_list,
              *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle);

            //! Check if value has write properties
            if ((attribute != NULL) && (attribute->value != NULL)) {
              if (!(attribute->char_val_prop & 0x08)) //! If no write property, send error response
              {
                err = 0x03; //! Error - Write not permitted
              }
            } else {
              //!Error = No such handle exists
              err = 0x01;
            }

            if (err) {
              //! Send error response
              rsi_ble_att_error_response(rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.dev_addr,
                                         *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle,
                                         opcode,
                                         err);
              break;
            }

            //! Update the value based on the offset and length of the value
            if ((err == 0)
                && ((*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.offset)
                    <= attribute->max_value_len)) {
              //SL_DEBUG_LOG_V2(INFO, "PWE - offset : %d\r\n", (*(uint16_t *)app_ble_prepared_write_event.offset));
              //! Hold the value to update it
              memcpy(&temp_prepare_write_value[temp_prepare_write_value_len],
                     rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.att_value,
                     rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.length);
              temp_prepare_write_value_len += rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.length;
            } else {
              //! Error : 0x07 - Invalid offset,  0x0D - Invalid attribute value length
              prep_write_err = 0x07;
            }
            //! Send gatt write response
            rsi_ble_gatt_prepare_write_response(
              rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.dev_addr,
              *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle,
              (*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.offset),
              rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.length,
              rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.att_value);
          }
        }
      } break;
      case RSI_BLE_GATT_EXECUTE_WRITE_EVENT: {
        SL_DEBUG_LOG_V2(INFO, "EWE\r\n");
        uint16_t execute_write_handle = *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle;
        //! TA-maintained: TA completes execute write; optional host-side use of event data only—no
        //! rsi_ble_gatt_write_response or rsi_ble_att_error_response for this handle.
        //! Host-maintained: commit temp_prepare_write_value to att_list, send response or error, clear temp state.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_EXECUTE_WRITE_EVENT);
        if (!rsi_ble_is_ta_maintained_attribute(execute_write_handle)) {
          if (execute_write_handle == rsi_ble_att1_val_hndl) {
            rsi_ble_att_list_t *attribute = NULL;
            uint8_t opcode = 0x18, err = 0x00;
            attribute = rsi_gatt_get_attribute_from_list(
              &att_list,
              *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle);

            //! Check if value has write properties
            if ((attribute != NULL) && (attribute->value != NULL)) {
              if (!(attribute->char_val_prop & 0x08)) //! If no write property, send error response
              {
                err = 0x03; //! Error - Write not permitted
              }
            } else {
              //!Error = No such handle exists
              err = 0x01;
            }

            //! Update the value based on the offset and length of the value
            if ((!err) && (rsi_ble_conn_info[l_conn_id].app_ble_execute_write_event.exeflag == 0x1)
                && (temp_prepare_write_value_len <= attribute->max_value_len) && !prep_write_err) {
              //! Hold the value to update it
              memcpy((uint8_t *)attribute->value, temp_prepare_write_value, temp_prepare_write_value_len);
              attribute->value_len = temp_prepare_write_value_len;

              //! Send gatt write response
              rsi_ble_gatt_write_response(rsi_ble_conn_info[l_conn_id].app_ble_execute_write_event.dev_addr, 1);
            } else {
              err = 0x0D; //Invalid attribute value length
            }
            if (prep_write_err) {
              //! Send error response
              rsi_ble_att_error_response(rsi_ble_conn_info[l_conn_id].app_ble_execute_write_event.dev_addr,
                                         *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle,
                                         opcode,
                                         prep_write_err);
            } else if (err) {
              //! Send error response
              rsi_ble_att_error_response(rsi_ble_conn_info[l_conn_id].app_ble_execute_write_event.dev_addr,
                                         *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle,
                                         opcode,
                                         err);
            }
            prep_write_err = 0;
            err            = 0;
            memset(temp_prepare_write_value, 0, temp_prepare_write_value_len);
            temp_prepare_write_value_len = 0;
          }
        }
      } break;

      case RSI_DATA_TRANSMIT_EVENT: {

        //! Perform Gatt write to remote device continuously
        if (write_handle_found && ble_conn_conf->tx_write) {
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "In write with response event - conn%d\r\n", l_conn_id);
#endif
          read_data1[0] = write_cnt;
          status = rsi_ble_set_att_value_async(rsi_connected_dev_addr, write_handle, max_data_length, read_data1);
          if (status != RSI_SUCCESS) {
            //! check for procedure already in progress error
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(INFO,
                              "rsi_ble_set_att_value_async procedure is already in progress - conn%d\r\n",
                              l_conn_id);
#endif
              break;
            } else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              SL_DEBUG_LOG_V2(ERROR, "Write with response failed with buffer error - conn%d\r\n", l_conn_id);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Write with response failed with status = %lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          } else {
            write_cnt++;
          }
        }

        //! Gatt write no response to remote device continuously
        else if (write_wwr_handle_found && ble_conn_conf->tx_write_no_response) {
          read_data1[0] = wwr_count;

#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "In write without response event - conn%d\r\n", l_conn_id);
#endif
          status =
            rsi_ble_set_att_cmd_async(rsi_connected_dev_addr, write_wwr_handle, max_data_length, (uint8_t *)read_data1);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(ERROR, "Write without response failed with buffer error - conn%d\r\n", l_conn_id);
#endif
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
              break;
            } else if (status == RSI_ERROR_IN_BUFFER_ALLOCATION) //! TO-DO, add proper error code
            {
              SL_DEBUG_LOG_V2(ERROR, "Cannot transmit %d", max_data_length);
              SL_DEBUG_LOG_V2(ERROR, "bytes in small buffer configuration mode - conn%d", l_conn_id);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_DISCONN_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Write without response failed with status = 0x%lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d ", l_conn_id);
            }
          } else {
            wwr_count++;
          }
        }

        //! indicate to remote device continuously
        else if (ble_conn_conf->tx_indications && (indications_enabled == true)) {
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
          //! prepare the data to set as local attribute value.
          read_data1[0] = indication_cnt;
          read_data1[1] = indication_cnt >> 8;
          status        = rsi_ble_indicate_value(rsi_connected_dev_addr,
                                          rsi_ble_att3_val_hndl,
                                          max_data_length,
                                          (uint8_t *)read_data1);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(INFO,
                              "rsi_ble_set_att_value_async procedure is already in progress - conn%d\r\n",
                              l_conn_id);
#endif
              break;
            }
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(ERROR, "Indicate %d", indication_cnt);
              SL_DEBUG_LOG_V2(ERROR, "failed with buffer full error - conn%d", l_conn_id);
#endif
              rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
              break;
            } else if (status == RSI_ERROR_IN_BUFFER_ALLOCATION) //! TO-DO, add proper error code
            {
              SL_DEBUG_LOG_V2(ERROR, "Cannot transmit %d", max_data_length);
              SL_DEBUG_LOG_V2(ERROR, "bytes in small buffer configuration mode - conn%d", l_conn_id);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_DISCONN_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Indication %d", indication_cnt);
              SL_DEBUG_LOG_V2(ERROR, "failed with error code %lx ", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          } else {
            indication_cnt++;
          }
        }

        //! Notify to remote device continuously
        else if (ble_conn_conf->tx_notifications && (notifications_enabled == true)) {
          //! prepare the data to set as local attribute value.
          read_data1[0] = notfy_cnt;
          read_data1[1] = notfy_cnt >> 8;

#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "Sending notify : %d\r\n", notfy_cnt);
#endif
          //! set the local attribute value.
          status = rsi_ble_notify_value(rsi_connected_dev_addr,
                                        notification_tx_handle,
                                        max_data_length,
                                        (uint8_t *)read_data1);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
#if RSI_DEBUG_EN
              SL_DEBUG_LOG_V2(ERROR, "Notify %d", notfy_cnt);
              SL_DEBUG_LOG_V2(ERROR, "failed with buffer full error - conn%d", l_conn_id);
#endif
              rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_TRANSMIT_EVENT);
              break;
            } else if (status == RSI_ERROR_IN_BUFFER_ALLOCATION) //! TO-DO, add proper error code
            {
              SL_DEBUG_LOG_V2(ERROR, "Cannot transmit %d", max_data_length);
              SL_DEBUG_LOG_V2(ERROR, "bytes in small buffer configuration mode - conn%d", l_conn_id);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_DISCONN_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Notify %d", notfy_cnt);
              SL_DEBUG_LOG_V2(ERROR, "failed with error code %lx ", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          } else {
#if RSI_DEBUG_EN
            SL_DEBUG_LOG_V2(INFO, "Notify %d", notfy_cnt);
            SL_DEBUG_LOG_V2(INFO, "success - conn%d", l_conn_id);
#endif
            notfy_cnt++;
          }
        }
      } break;
      case RSI_DATA_RECEIVE_EVENT: {
        //! To receive notifications from remote device, set the attribute value
        if (notify_handle_found && ble_conn_conf->rx_notifications) {
          uint8_t notify_data[2] = { 1, 0 };
          SL_DEBUG_LOG_V2(INFO, "In receive notification event - conn%d\r\n", l_conn_id);
          //! clear the served event
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
          status = rsi_ble_set_att_value_async(rsi_connected_dev_addr, //enable the notifications
                                               notify_handle + 1,
                                               2,
                                               notify_data);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              SL_DEBUG_LOG_V2(ERROR, "Notify failed with buffer error - conn%d\r\n", l_conn_id);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_RECEIVE_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Notify value failed with status = %lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          }
        }
        //! Transmit Gatt indications to remote device
        else if (indication_handle_found && ble_conn_conf->rx_indications) {
          uint8_t indicate_data[2] = { 2, 0 };
          SL_DEBUG_LOG_V2(INFO, "In receive indication event - conn%d", l_conn_id);
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
          status = rsi_ble_set_att_value_async(rsi_connected_dev_addr, //enable the indications
                                               indication_handle + 1,
                                               2,
                                               indicate_data);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              SL_DEBUG_LOG_V2(ERROR, "Indication failed with buffer error - conn%d\r\n", l_conn_id);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_RECEIVE_EVENT);
              break;
            } else {
              SL_DEBUG_LOG_V2(ERROR, "Indication failed with status = %lx", status);
              SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
            }
          }
        }
      } break;
      case RSI_BLE_WRITE_EVENT_RESP: {
        //! event invokes when write response received
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_WRITE_EVENT_RESP);
        if (ble_conn_conf->tx_write) {
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "Write response received - conn%d\r\n", l_conn_id);
#endif
        }
      } break;
      case RSI_BLE_GATT_INDICATION_CONFIRMATION: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_INDICATION_CONFIRMATION);
        if (ble_conn_conf->tx_indications && (indications_enabled == true)) {
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
#if RSI_DEBUG_EN
          SL_DEBUG_LOG_V2(INFO, "In rsi_ble_on_event_indication_confirmation event\r\n");
#endif
        }
      } break;

      case RSI_BLE_READ_REQ_EVENT: {
        //! event invokes when write/notification events received

        SL_DEBUG_LOG_V2(INFO, "Read request initiated by remote device - conn%d\r\n", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_READ_REQ_EVENT);

        type   = rsi_ble_conn_info[l_conn_id].app_ble_read_event.type;
        handle = rsi_ble_conn_info[l_conn_id].app_ble_read_event.handle;
        offset = rsi_ble_conn_info[l_conn_id].app_ble_read_event.offset;

        if (type == 1) {
          status = rsi_ble_gatt_read_response(rsi_connected_dev_addr,
                                              1,
                                              handle,
                                              offset,
                                              (sizeof(read_data1) - offset),
                                              &(read_data1[offset]));
          offset = 0;
        } else {

          status = rsi_ble_gatt_read_response(rsi_connected_dev_addr, 0, handle, 0, (sizeof(read_data1)), read_data1);
        }
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "Read response failed, error:0x%lx", status);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
        } else {
          SL_DEBUG_LOG_V2(INFO,
                          "Response to read request initiated by remote device was successful - conn%d",
                          l_conn_id);
        }

      } break;
      case RSI_BLE_MTU_EVENT: {
        //! event invokes when write/notification events received

        rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_ble_conn_info[l_conn_id].app_ble_mtu_event.dev_addr);
        SL_DEBUG_LOG_V2(INFO, "MTU size from remote device(%s", (uintptr_t)(str_remote_address));
        SL_DEBUG_LOG_V2(INFO, "), %d ", rsi_ble_conn_info[l_conn_id].app_ble_mtu_event.mtu_size);
        SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MTU_EVENT);
        mtu_exchange_done = 1;
        if (ble_conn_conf->smp_enable) {
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_RECEIVE_REMOTE_FEATURES);
        }
      } break;
      case RSI_BLE_SCAN_RESTART_EVENT: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
        SL_DEBUG_LOG_V2(INFO, "In Scan Re-Start evt \r\n");
        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "Scanning stop failed, cmd status = %lx", status);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
        }
        SL_DEBUG_LOG_V2(INFO, "Restarting scanning ");
        status = rsi_ble_start_scanning();
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(ERROR, "Scanning start failed, cmd status = %lx", status);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
        }
      } break;
      case RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ: {

        //!default ACCEPT the remote conn params request (0-ACCEPT, 1-REJECT)
        status = rsi_ble_conn_param_resp(rsi_ble_conn_info[l_conn_id].rsi_app_remote_device_conn_params.dev_addr, 0);
        if (status != RSI_SUCCESS) {
          SL_DEBUG_LOG_V2(INFO, "Conn param resp status: 0x%lX\r\n", status);
        }
        //! remote device conn params request
        //! clear the conn params request event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);

      } break;
      case RSI_APP_EVENT_DATA_LENGTH_CHANGE: {
        //! clear the disconnected event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_DATA_LENGTH_CHANGE);

        if (rsi_ble_conn_info[l_conn_id].remote_dev_feature.remote_features[1] & 0x01) {
          status = rsi_ble_setphy((int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_feature.dev_addr,
                                  TX_PHY_RATE,
                                  RX_PHY_RATE,
                                  CODDED_PHY_RATE);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to cancel the connection request: 0x%lx", status);
            SL_DEBUG_LOG_V2(ERROR, "\r\n - conn%d", l_conn_id);
          }
        }
      } break;

      case RSI_APP_EVENT_PHY_UPDATE_COMPLETE: {
        //! phy update complete event

        //! clear the phy updare complete event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
      } break;

      case RSI_BLE_SMP_REQ_EVENT: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_REQ_EVENT);

        SL_DEBUG_LOG_V2(INFO, "In SMP request \r\n - conn%d\r\n", l_conn_id);
        if (ble_conn_conf->smp_enable) {
          smp_pairing_request_received = true;

          if (first_connect == 0) {
            if (!smp_pairing_initated) {
              //! initiating the SMP pairing process
              status = rsi_ble_smp_pair_request(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_req.dev_addr,
                                                RSI_BLE_SMP_IO_CAPABILITY,
                                                MITM_ENABLE);
              if (status != RSI_SUCCESS) {
                SL_DEBUG_LOG_V2(ERROR,
                                "RSI_BLE_SMP_REQ_EVENT: failed to initiate the SMP pairing process: 0x%lx",
                                status);
                SL_DEBUG_LOG_V2(ERROR, "\r\n - conn%d", l_conn_id);
              } else {
                smp_pairing_initated = true;
                //rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
                SL_DEBUG_LOG_V2(INFO, "SMP pairing request initiated to %s", (uintptr_t)(str_remote_address));
                SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
              }
            }

          } else {
            status = rsi_ble_start_encryption(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_req.dev_addr,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localediv,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localrand,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localltk);
          }
        }
      } break;

      case RSI_BLE_SMP_RESP_EVENT: {

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_RESP_EVENT);

        SL_DEBUG_LOG_V2(INFO, "In SMP response - conn%d\r\n", l_conn_id);
        if (ble_conn_conf->smp_enable) {
          //! initiating the SMP pairing process
          status = rsi_ble_smp_pair_response(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.dev_addr,
                                             RSI_BLE_SMP_IO_CAPABILITY,
                                             MITM_ENABLE);
        }
      } break;

      case RSI_BLE_SMP_PASSKEY_EVENT: {

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_PASSKEY_EVENT);

#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In SMP passkey - conn%d\r\n", l_conn_id);
#endif

        SL_DEBUG_LOG_V2(INFO,
                        "In smp_passkey - str_remote_address : %s",
                        (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));

        //! initiating the SMP pairing process
        status =
          rsi_ble_smp_passkey(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_passkey.dev_addr, RSI_BLE_APP_SMP_PASSKEY);
      } break;
      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
#if RSI_DEBUG_EN
        SL_DEBUG_LOG_V2(INFO, "In SMP pass key display - conn%d\r\n", l_conn_id);
#endif

        SL_DEBUG_LOG_V2(INFO, "Remote addr: %s", (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));
        SL_DEBUG_LOG_V2(INFO,
                        ", passkey: %s",
                        (uintptr_t)(rsi_ble_conn_info[l_conn_id].rsi_ble_smp_passkey_display.passkey));
      } break;
      case RSI_BLE_LTK_REQ_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_LTK_REQ_EVENT);

        SL_DEBUG_LOG_V2(INFO, "In LTK  request - conn%d \r\n", l_conn_id);

        if (0) /* ((rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.localediv == l_rsi_encryption_enabled.localediv)
					&& !((memcmp(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.localrand,
							l_rsi_encryption_enabled.localrand, 8))))*/
        {
          SL_DEBUG_LOG_V2(INFO, "Positive reply\r\n");
          //! give le ltk req reply cmd with positive reply
          status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.dev_addr,
                                         1,
                                         l_rsi_encryption_enabled.localltk);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to restart smp pairing with status: 0x%lx", status);
            SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
          }
        } else {
          SL_DEBUG_LOG_V2(INFO, "Negative reply");
          //! give le ltk req reply cmd with negative reply
          status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.dev_addr, 0, NULL);
          if (status != RSI_SUCCESS) {
            SL_DEBUG_LOG_V2(ERROR, "Failed to restart smp pairing with status: 0x%lx ", status);
          }
        }
      } break;

      case RSI_BLE_SC_PASSKEY_EVENT: {
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SC_PASSKEY_EVENT);

        if (((RSI_BLE_SMP_IO_CAPABILITY == 0x04 && rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.io_cap == 0x00))
            || (RSI_BLE_SMP_IO_CAPABILITY == 0x02
                && (rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.io_cap == 0x00
                    || rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.io_cap == 0x01
                    || rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.io_cap == 0x02
                    || rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.io_cap == 0x04))) {
          SL_DEBUG_LOG_V2(INFO, "In SMP sc passkey event - conn%d", l_conn_id);
          SL_DEBUG_LOG_V2(INFO, "remote addr: %s ", (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));
          rsi_ble_smp_passkey(rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.dev_addr,
                              rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.passkey);

        } else {
          SL_DEBUG_LOG_V2(INFO,
                          "In SMP sc passkey display event, remote addr: %s",
                          (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));
          SL_DEBUG_LOG_V2(INFO, ", passkey: %06ld ", rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.passkey);
          SL_DEBUG_LOG_V2(INFO, "- conn%d", l_conn_id);
          rsi_ble_smp_passkey(rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.dev_addr,
                              rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.passkey);
        }
      } break;

      case RSI_BLE_SECURITY_KEYS_EVENT: {
        //! event invokes when security keys are received
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SECURITY_KEYS_EVENT);

        SL_DEBUG_LOG_V2(INFO, "In SMP security keys event  - conn%d\r\n", l_conn_id);

      } break;

      case RSI_BLE_SMP_FAILED_EVENT: {
        //! initiate SMP protocol as a Central

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_FAILED_EVENT);

        SL_DEBUG_LOG_V2(ERROR,
                        "In SMP failed remote address: %s",
                        (uintptr_t)(rsi_ble_conn_info[l_conn_id].remote_dev_addr));
        SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
      }

      break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT: {
        //! start the encrypt event

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_ENCRYPT_STARTED_EVENT);
        //! copy to local buffer
        memcpy(&l_rsi_encryption_enabled,
               &rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled,
               sizeof(rsi_bt_event_encryption_enabled_t));
        SL_DEBUG_LOG_V2(INFO, "In SMP encrypt event - conn%d\r\n", l_conn_id);
        smp_done = 1;
      } break;
      case RSI_BLE_GATT_ERROR: {
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_ERROR);
        if ((*(uint16_t *)rsi_ble_conn_info[l_conn_id].rsi_ble_gatt_err_resp.error) == RSI_END_OF_PROFILE_QUERY) {
          if (total_remote_profiles != 0) //! If any profiles exists
          {
            if ((profile_index_for_char_query - 1) < total_remote_profiles) //! Error received for any profile
            {
              char_resp_recvd = false;
              char_for_serv_cnt++;
              //! set event
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
            } else //! Error received for last profile
            {
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
            }
          } else //! Check for profiles pending, else done profile querying
          {
            //! first level profile query completed
            done_profiles_query = true;

            //set event to start second level profile query
            rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
          }
        } else {
          SL_DEBUG_LOG_V2(ERROR,
                          "GATT ERROR REASON:0x%x",
                          *(uint16_t *)rsi_ble_conn_info[l_conn_id].rsi_ble_gatt_err_resp.error);
          SL_DEBUG_LOG_V2(ERROR, "- conn%d", l_conn_id);
        }

      } break;
      default:
        break;
    }
    ble_temp_event_map[l_conn_id] &= ~(BIT(event_id));
  }
}