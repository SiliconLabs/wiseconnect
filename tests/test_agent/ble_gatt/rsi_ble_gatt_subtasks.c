/*******************************************************************************
* @file  rsi_ble_subtasks.c
* @brief
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
/**
 * @file    rsi_ble_subtasks.c
 * @version 0.1
 * @date    19 Mar 2021
 *
 *
 *
 *  @brief : This file contains example application for BLE DUAL role.
 *
 *  @section Description  This application connects as a Central/Master with l2cap connection.
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/
//#include "rsi_driver.h"
#include "rsi_bt_common_apis.h"
#include <rsi_ble.h>
#include "rsi_ble_apis.h"
#include "rsi_ble_gatt_device_info.h"
#include "rsi_ble_gatt_config.h"
#include "rsi_gatt_common_config.h"
#include "sl_si91x_status.h"
#include "command_identifiers.h"

#define RSI_PRINT_LOGS 0
#define SMP_ENABLE     1

#if RSI_PRINT_LOGS
#define LOG_PRINT(...) printf(__VA_ARGS__)
#else
#define LOG_PRINT(...)
#endif

#define RSI_PRINT_EVENTS 1

#if RSI_PRINT_EVENTS
#define LOG_EVENT(...) printf(__VA_ARGS__)
#else
#define LOG_EVENT(...)
#endif
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
extern volatile uint8_t num_of_conn_masters, num_of_conn_slaves;
extern volatile uint16_t rsi_ble_att1_val_hndl;
extern volatile uint16_t rsi_ble_att2_val_hndl;
extern volatile uint16_t rsi_ble_att3_val_hndl;
extern volatile uint32_t ble_app_event_task_map[];
extern volatile uint32_t ble_app_event_task_map1[];
extern volatile uint16_t rsi_disconnect_reason[TOTAL_CONNECTIONS];
extern rsi_ble_conn_info_t rsi_ble_conn_info[];
extern rsi_ble_req_adv_t change_adv_param;
extern rsi_ble_req_scan_t change_scan_param;
extern uint8_t slave_connection_in_prgs, slave_con_req_pending;
extern uint16_t rsi_scan_in_progress;
extern osThreadId_t ble_app_task_handle[TOTAL_CONNECTIONS];
extern uint8_t master_task_instances, slave_task_instances;
extern rsi_ble_t att_list;
extern osSemaphoreId_t ble_conn_sem[TOTAL_CONNECTIONS];
extern osSemaphoreId_t ble_slave_conn_sem;

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/
extern int32_t rsi_ble_start_advertising_with_values(void *rsi_ble_adv);
extern rsi_ble_att_list_t *rsi_gatt_get_attribute_from_list(rsi_ble_t *p_val, uint16_t handle);
extern void rsi_ble_app_set_task_event(uint8_t conn_id, uint32_t event_num);
extern int32_t rsi_ble_gatt_prepare_write_response(uint8_t *dev_addr,
                                                   uint16_t handle,
                                                   uint16_t offset,
                                                   uint16_t length,
                                                   uint8_t *data);

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
 * This function process configured ble slave and master connections
 */
void rsi_ble_task_on_conn(void *parameters)
{
  LOG_PRINT("Entering rsi_ble_task_on_conn \n");
  uint8_t buff_config_done = 0, conn_param_req_given = 0;
  uint8_t done_profiles_query = 0;
  uint8_t prof_resp_recvd     = 0;
  //  uint8_t char_resp_recvd              = 0;
  //  uint8_t char_desc_resp_recvd         = 0;
  //  uint8_t write_handle_found           = 0;
  uint8_t smp_pairing_initated         = 0;
  uint8_t smp_pairing_request_received = 0;
  //  int16_t write_handle                 = 0;
  //  uint16_t write_cnt                   = 0;
  //  uint8_t write_wwr_handle_found       = 0;
  //  int16_t write_wwr_handle             = 0;
  //  uint16_t wwr_count                   = 0;
  uint8_t notify_handle_found     = 0;
  uint16_t notify_handle          = 0;
  uint8_t indication_handle_found = 0;
  uint16_t indication_handle      = 0;
  //  uint16_t notfy_cnt                   = 0;
  //  uint16_t indication_cnt              = 0;
  uint8_t no_of_profiles = 0, total_remote_profiles = 0;
  //  uint8_t l_num_of_services = 0, l_char_property = 0;
  //  uint8_t profs_evt_cnt = 0, char_desc_cnt = 0;
  uint8_t prof_evt_cnt      = 0;
  uint8_t char_for_serv_cnt = 0;
  //  uint8_t i = 0, profile_index_for_char_query = 0, temp1 = 0, temp2 = 0;
  uint8_t temp_prepare_write_value[250];
  uint16_t temp_prepare_write_value_len = 0;
  uint8_t prep_write_err                = 0;
  int32_t status                        = 0;
  int32_t event_id                      = 0;
  uint16_t profiles_endhandle           = 0;
  //  uuid_t search_serv                               = { 0 };
  uint8_t rsi_connected_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
  uint8_t str_remote_address[RSI_REM_DEV_ADDR_LEN] = { '\0' };
  uint16_t offset                                  = 0;
  uint16_t handle                                  = 0;
  uint8_t type                                     = 0;

  //TODO : review
  // uint8_t read_data1[230]                          = {
  //   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
  //   29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
  //   58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 72, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
  //   87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
  //   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
  //   45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 72,
  //   74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99
  // };
  uint8_t read_data1[1]                                      = { 90 };
  uint8_t first_connect                                      = 0;
  rsi_bt_event_encryption_enabled_t l_rsi_encryption_enabled = { 0 };
  uint8_t smp_done = 0, mtu_exchange_done = 0;
  uint8_t l_conn_id                    = 0xff;
  uint8_t max_data_length              = 0;
  rsi_ble_conn_config_t *ble_conn_conf = NULL;
  uint8_t rsi_rx_from_rem_dev          = 0;
  uint8_t rsi_tx_to_rem_dev            = 0;
  // //! flag indicating whether notifications from remote device received or not
  uint8_t notification_received   = 0;
  uint8_t profile_mem_init        = 0;
  uint8_t service_char_mem_init   = 0;
  uint16_t notification_tx_handle = 0;

  rsi_ble_profile_list_by_conn_t rsi_ble_profile_list_by_conn;
  // //! initialize the structure member variables to NULL
  rsi_ble_profile_list_by_conn.profile_desc      = NULL;
  rsi_ble_profile_list_by_conn.profile_info_uuid = NULL;
  rsi_ble_profile_list_by_conn.profile_char_info = NULL;

  ble_conn_conf   = (rsi_ble_conn_config_t *)parameters;
  max_data_length = ble_conn_conf->buff_mode_sel.max_data_length;
  // //! store connection identifier in local variable
  l_conn_id = ble_conn_conf->conn_id;

  //! connection identifier should always be less than the total no. of connections requested
  if (l_conn_id >= TOTAL_CONNECTIONS) {
    LOG_PRINT("wrong connection identifier \n");
    //! check why this state has occured
    return;
  }
  //! create semaphore for each connection
  ble_conn_sem[l_conn_id] = osSemaphoreNew(1, 1, 0);
  LOG_PRINT("rsi_semaphore_create(&ble_conn_sem[%d]) \n", l_conn_id);
  //  print_status(status);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("semaphore creation failed for slave %d \n", l_conn_id);
    return;
  }

  //! if connection is from remote slave devices, set advertising event for processing further activities in this connection
  if (rsi_ble_conn_info[l_conn_id].remote_device_role == SLAVE_ROLE) {
    rsi_ble_app_set_task_event(l_conn_id, RSI_APP_EVENT_ADV_REPORT);
    LOG_PRINT("rsi_ble_app_set_task_event(%d, RSI_APP_EVENT_ADV_REPORT) \n", l_conn_id);
  }
  //! if connection is from remote master devices, set connection event for processing further activities in this connection
  else if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
    if (rsi_ble_conn_info[l_conn_id].is_enhanced_conn) {
      rsi_ble_app_set_task_event(l_conn_id, RSI_BLE_ENHC_CONN_EVENT);
    } else {
      rsi_ble_app_set_task_event(l_conn_id, RSI_BLE_CONN_EVENT);
    }
  } else {
    LOG_PRINT("Nothing to do as the rsi_ble_conn_info[%d].remote_device_role is not obtained \n", l_conn_id);
  }

  while (1) {
    //! checking for events list
    event_id = rsi_ble_get_event_based_on_conn(l_conn_id);
    if (event_id == -1) {
      //! wait on connection specific semaphore
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

    switch (event_id) {
      case RSI_APP_EVENT_ADV_REPORT: {
        LOG_EVENT("\nRSI_APP_EVENT_ADV_REPORT\n");
        LOG_PRINT("Advertise report received- conn%d \n", l_conn_id);
        //! clear the advertise report event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_ADV_REPORT);
      } break;
      case RSI_BLE_CONN_EVENT: {
        LOG_EVENT("\nRSI_BLE_CONN_EVENT\n");
        //! event invokes when connection was completed
        LOG_PRINT("In on conn evt - task%d \n", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_CONN_EVENT);
        rsi_ble_conn_info[l_conn_id].conn_status = 1;
        //! copy connected remote device address in to local buffer
        memcpy(rsi_connected_dev_addr, rsi_ble_conn_info[l_conn_id].conn_event_to_app.dev_addr, RSI_DEV_ADDR_LEN);
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
          //! start SDP query if profile discovery is enabled in connection specific buffer and remote role is Master
          if (ble_conn_conf->profile_discovery) {
            rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
          }
        }
        //! add device to acceptlist
        if (ble_conn_conf->add_to_acceptlist) {
          //! clear the acceptlist completely
          status = rsi_ble_clear_acceptlist();
          LOG_PRINT("rsi_ble_clear_acceptlist\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("Failed to clear the accept list, error: 0x%lx -conn%d\n", status, l_conn_id);
            break;
          }
          status = rsi_ble_addto_acceptlist((int8_t *)rsi_connected_dev_addr, LE_RANDOM_ADDRESS);
          LOG_PRINT("rsi_ble_addto_acceptlist\n");
          //          print_status(status);
          rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("Failed to add the device:%s to acceptlist -conn%d\n", str_remote_address, l_conn_id);
            break;
          }
          LOG_PRINT("device %s added to acceptlist -conn%d \n", str_remote_address, l_conn_id);
        }

        //! check if connection is from remote master device
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
          num_of_conn_masters++;

          LOG_PRINT("Number of master devices connected:%d -conn%d\n", num_of_conn_masters, l_conn_id);
        } else {
          //! assuming that connection is from remote slave device
          num_of_conn_slaves++;
          LOG_EVENT("Number of slave devices connected:%d \n", num_of_conn_slaves);
        }

        //! perform MTU exchange with remote device
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr,
                                              (int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_addr);

          status = rsi_ble_mtu_exchange_event(rsi_connected_dev_addr, MAX_MTU_SIZE);
          LOG_EVENT("rsi_ble_mtu_exchange_event\n");
          //          print_status(status);
          if (status != 0) {
            LOG_EVENT("MTU Exchange request failed -conn%d\n", l_conn_id);
          }
        } else {
          mtu_exchange_done = 1;
        }
      } break;
      case RSI_BLE_ENHC_CONN_EVENT: {
        LOG_EVENT("\nRSI_BLE_ENHC_CONN_EVENT\n");

        LOG_PRINT("In on_enhance_conn evt - conn%d\n", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_ENHC_CONN_EVENT);
        rsi_ble_conn_info[l_conn_id].conn_status = 1;
        //! copy connected remote device address in to local buffer
        memcpy(rsi_connected_dev_addr, rsi_ble_conn_info[l_conn_id].rsi_enhc_conn_status.dev_addr, RSI_DEV_ADDR_LEN);
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
          //! start SDP query if profile discovery is enabled in connection specific buffer and remote role is Master
          // if (ble_conn_conf->profile_discovery) {
          //   LOG_EVENT("RSI_BLE_ENHC_CONN_EVENT ->  RSI_BLE_REQ_GATT_PROFILE -conn%d \n", l_conn_id);
          //   rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
          // }
        }
        //! add device to acceptlist
        if (ble_conn_conf->add_to_acceptlist) {
          //! clear the acceptlist completely
          status = rsi_ble_clear_acceptlist();
          LOG_PRINT("rsi_ble_clear_acceptlist\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("Failed to clear the accept list, error: 0x%lx -conn%d\n", status, l_conn_id);
          }
          status = rsi_ble_addto_acceptlist((int8_t *)rsi_connected_dev_addr, LE_RANDOM_ADDRESS);
          LOG_PRINT("rsi_ble_addto_acceptlist\n");
          //          print_status(status);
          rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("Failed to add the device:%s to acceptlist -conn%d\n", str_remote_address, l_conn_id);
          }
          LOG_PRINT("device %s added to acceptlist -conn%d \n", str_remote_address, l_conn_id);
        }

        //! check if connection is from remote master device
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
          num_of_conn_masters++;

          LOG_PRINT("Number of master devices connected:%d -conn%d\n", num_of_conn_masters, l_conn_id);
        } else {
          //! assuming that connection is from remote slave device
          num_of_conn_slaves++;
          LOG_PRINT("Number of slave devices connected:%d \n", num_of_conn_slaves);
        }

        //! perform MTU exchange with remote device
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          rsi_ascii_dev_address_to_6bytes_rev(rsi_connected_dev_addr,
                                              (int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_addr);

          status = rsi_ble_mtu_exchange_event(rsi_connected_dev_addr, MAX_MTU_SIZE);
          LOG_PRINT("rsi_ble_mtu_exchange_event\n");
          //          print_status(status);
          if (status != 0) {
            LOG_PRINT("MTU Exchange request failed -conn%d\n", l_conn_id);
          }
        }
      } break;
      case RSI_BLE_MTU_EXCHANGE_INFORMATION: {
        LOG_EVENT("\nRSI_BLE_MTU_EXCHANGE_INFORMATION\n");
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MTU_EXCHANGE_INFORMATION);
        LOG_EVENT("MTU EXCHANGE INFORMATION - in subtask -conn%d \n", l_conn_id);
        if ((rsi_ble_conn_info[l_conn_id].mtu_exchange_info.initiated_role == PEER_DEVICE_INITATED_MTU_EXCHANGE)
            && (RSI_BLE_MTU_EXCHANGE_FROM_HOST)) {
          status = rsi_ble_mtu_exchange_resp(rsi_connected_dev_addr, LOCAL_MTU_SIZE);
          LOG_PRINT("rsi_ble_mtu_exchange_resp\n");
          //          print_status(status);
          //! check for procedure already in progress error
          if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
            rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_MTU_EXCHANGE_INFORMATION);
            LOG_PRINT("rsi_ble_mtu_exchange_resp procedure is already in progress -conn%d \n", l_conn_id);
            break;
          }
          if (status != RSI_SUCCESS) {
            LOG_PRINT("MTU EXCHANGE RESP Failed status : 0x%x \n", status);
          } else {
            LOG_PRINT("MTU EXCHANGE RESP SUCCESS status : 0x%x \n", status);
          }
        }
      } break;

      case RSI_BLE_MORE_DATA_REQ_EVT: {
        LOG_EVENT("\nRSI_BLE_MORE_DATA_REQ_EVT\n");

        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MORE_DATA_REQ_EVT);
        LOG_PRINT("more data request -conn%d \n", l_conn_id);

        if (rsi_current_state[l_conn_id] & BIT64(RSI_DATA_TRANSMIT_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_DATA_TRANSMIT_EVENT);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_DATA_TRANSMIT_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_REQ_GATT_PROFILE)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_REQ_GATT_PROFILE);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_REQ_GATT_PROFILE -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_REQ_GATT_PROFILE);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_PROFILES)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_PROFILES);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_GATT_PROFILES -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_PROFILE)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_PROFILE);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_GATT_PROFILE -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_GATT_CHAR_SERVICES)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_GATT_CHAR_SERVICES);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_GATT_CHAR_SERVICES -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_READ_REQ_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_READ_REQ_EVENT);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_READ_REQ_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_READ_REQ_EVENT);
        }
        if (rsi_current_state[l_conn_id] & BIT64(RSI_BLE_BUFF_CONF_EVENT)) {
          rsi_current_state[l_conn_id] &= ~BIT64(RSI_BLE_BUFF_CONF_EVENT);
          LOG_PRINT("RSI_BLE_MORE_DATA_REQ_EVT ->  RSI_BLE_BUFF_CONF_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
        }
      } break;
      case RSI_BLE_REQ_GATT_PROFILE: {
        //TODO : This is where we can trigger the profile enquiry. Convert this from event to sqa testa gent command
        //LOG_EVENT("\nRSI_BLE_REQ_GATT_PROFILE\n");
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
          LOG_PRINT("\r\n remote device profile discovery started -conn%d \r\n", l_conn_id);
          status = rsi_ble_get_profiles_async(rsi_connected_dev_addr, 1, 0xffff, NULL);
          if (status != RSI_SUCCESS) {
            //! check for procedure already in progress error
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_REQ_GATT_PROFILE);
              LOG_PRINT("\r\n rsi_ble_get_profiles_async procedure is already in progress -conn%d \r\n", l_conn_id);
              break;
            }
            //! check for buffer full error, which is not expected for this procedure
            else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              LOG_PRINT("\r\n rsi_ble_get_profiles_async failed with buffer full error -conn%d \r\n", l_conn_id);
              break;
            } else {
              LOG_PRINT("\r\n get profile async call failed with error code :%lx -conn%d \r\n", status, l_conn_id);
            }
          }
        }
      } break;
      case RSI_BLE_GATT_PROFILES: {
        PRINT_EVENT_START(GATT_PROFILES, BLE);
        LOG_EVENT("RSI_BLE_GATT_PROFILES\n");
        //! prof_resp_recvd is set to 0 for every profile query response
        LOG_EVENT("in gatt profiles -conn%d \n", l_conn_id);
        if (!prof_resp_recvd) {
          //! check until completion of first level query
          LOG_PRINT("done_profiles_query %d\n", done_profiles_query);
          if (!done_profiles_query) {
            LOG_EVENT("in profile query -conn%d \n", l_conn_id);
            rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
            no_of_profiles = rsi_ble_conn_info[l_conn_id].get_allprofiles.number_of_profiles;
            //! copy the end of handle of last searched profile
            profiles_endhandle =
              *(uint16_t *)(rsi_ble_conn_info[l_conn_id].get_allprofiles.profile_desc[no_of_profiles - 1].end_handle);
            LOG_EVENT("profiles end handle = 0x%x -conn%d \n", profiles_endhandle, l_conn_id);
            //! allocate memory
            if (rsi_ble_profile_list_by_conn.profile_desc == NULL) {
              rsi_ble_profile_list_by_conn.profile_desc =
                (profile_descriptors_t *)malloc(sizeof(profile_descriptors_t) * no_of_profiles);
              if (rsi_ble_profile_list_by_conn.profile_desc == NULL)
                return;
              memset(rsi_ble_profile_list_by_conn.profile_desc, 0, sizeof(profile_descriptors_t) * no_of_profiles);
            } else {
              void *temp                                = NULL;
              rsi_ble_profile_list_by_conn.profile_desc = (profile_descriptors_t *)realloc(
                rsi_ble_profile_list_by_conn.profile_desc,
                sizeof(profile_descriptors_t) * (total_remote_profiles + no_of_profiles));
              temp = (void *)malloc(sizeof(profile_descriptors_t) * (total_remote_profiles + no_of_profiles));
              if (temp == NULL) {
                LOG_EVENT("failed to allocate memory for rsi_ble_profile_list_by_conn.profile_desc for conn_id:%d of "
                          "size size : %d\n",
                          l_conn_id,
                          sizeof(profile_descriptors_t) * no_of_profiles);
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
              LOG_EVENT("failed to allocate memory for rsi_ble_profile_list_by_conn.profile_desc for conn_id:%d of "
                        "size size : %d\n",
                        l_conn_id,
                        sizeof(profile_descriptors_t) * total_remote_profiles);
              return;
            }
            //! copy retrieved profiles in local master buffer
            for (uint8_t i = 0; i < no_of_profiles; i++) {
              memcpy(&rsi_ble_profile_list_by_conn.profile_desc[i + total_remote_profiles],
                     &rsi_ble_conn_info[l_conn_id].get_allprofiles.profile_desc[i],
                     sizeof(profile_descriptors_t));
            }
            total_remote_profiles += no_of_profiles;

            //! check for end of profiles
            if (profiles_endhandle != 0xffff) {
              status = rsi_ble_get_profiles_async(rsi_connected_dev_addr, profiles_endhandle + 1, 0xffff, NULL);
              LOG_EVENT("rsi_ble_get_profiles_async\n");
              if (status != RSI_SUCCESS) {
                //! check for procedure already in progress error
                if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
                  rsi_current_state[l_conn_id] |= BIT64(RSI_BLE_GATT_PROFILES);
                  LOG_EVENT("rsi_ble_get_profiles_async procedure is already in progress -conn%d \n", l_conn_id);
                  break;
                }
                //! check for buffer full error, which is not expected for this procedure
                else if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
                  LOG_EVENT("rsi_ble_get_profiles_async failed with buffer full error -conn%d \n", l_conn_id);
                  break;
                } else {
                  LOG_EVENT("get profile async call failed with error code :%lx -conn%d \n", status, l_conn_id);
                }
              }
            } else {
              //! first level profile query completed
              done_profiles_query = 1;
              //set event to start second level profile query
              // LOG_EVENT("RSI_BLE_GATT_PROFILES ->  RSI_BLE_GATT_PROFILES -conn%d \n", l_conn_id);
              // rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
            }
          } else {
            LOG_EVENT("prof_resp_recvd %d\n", prof_resp_recvd);
            prof_resp_recvd = 1;

            //TODO : Not calling rsi_ble_get_profile_async here
            LOG_EVENT("RSI_BLE_GATT_PROFILES - total_remote_profiles:%d -conn%d\n", total_remote_profiles, l_conn_id);
            uint8_t pix;
            for (pix = 0; pix < total_remote_profiles; pix++) {
              uint8_t size = rsi_ble_profile_list_by_conn.profile_desc[pix].profile_uuid.size;
              if (size == 2) //! check for 16 bit(2 bytes) UUID value
              {
                uuid16_t val16 = rsi_ble_profile_list_by_conn.profile_desc[pix].profile_uuid.val.val16;
                LOG_EVENT("RSI_BLE_GATT_PROFILES - index:%d, uuid:0x%x, uuid_size:%d -conn%d\n",
                          pix,
                          val16,
                          size,
                          l_conn_id);
              } else if (size == 4) {
                uuid32_t val32 = rsi_ble_profile_list_by_conn.profile_desc[pix].profile_uuid.val.val32;
                LOG_EVENT("RSI_BLE_GATT_PROFILES - index:%d, uuid:0x%lx, uuid_size:%d -conn%d\n",
                          pix,
                          val32,
                          size,
                          l_conn_id);
              } else if (size == 16) //! 128 bit(16 byte) UUID value
              {
                uuid128_t val128 = rsi_ble_profile_list_by_conn.profile_desc[pix].profile_uuid.val.val128;
                LOG_EVENT("RSI_BLE_GATT_PROFILES - index:%d, uuid:0x%lx, uuid_size:%d -conn%d\n",
                          pix,
                          val128.data1,
                          size,
                          l_conn_id);
              }
            }
            LOG_EVENT("RSI_BLE_GATT_PROFILES - resolved, -conn%d\n", l_conn_id);
            rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
          }
        }
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
        PRINT_EVENT_END();
      } break;

      case RSI_BLE_GATT_PROFILE: {
        PRINT_EVENT_START(GATT_PROFILE, BLE);
        LOG_EVENT("RSI_BLE_GATT_PROFILE\n");
        LOG_PRINT("in gatt profile -conn%d \n", l_conn_id);
        LOG_PRINT("profile_mem_init %d \n", profile_mem_init);
        if (!profile_mem_init) {
          if (rsi_ble_profile_list_by_conn.profile_info_uuid == NULL) {
            rsi_ble_profile_list_by_conn.profile_info_uuid = (rsi_ble_event_profile_by_uuid_t *)malloc(
              sizeof(rsi_ble_event_profile_by_uuid_t) * total_remote_profiles);
          }
          if (rsi_ble_profile_list_by_conn.profile_info_uuid == NULL) {
            LOG_PRINT("failed to allocate memory for rsi_ble_profile_list_by_conn[%d].profile_info_uuid of size : %d\n",
                      l_conn_id,
                      sizeof(rsi_ble_event_profile_by_uuid_t) * total_remote_profiles);
            return;
          }
          memset(rsi_ble_profile_list_by_conn.profile_info_uuid,
                 0,
                 sizeof(rsi_ble_event_profile_by_uuid_t) * total_remote_profiles);
          profile_mem_init = 1;
        }
        //TODO: setting prof_evt_cnt to 0 and not soring all the profiles handles as we are dealing with individual profiles
        prof_evt_cnt = 0;
        //copy total searched profiles in local buffer
        //! copy to master buffer
        memcpy(&rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt],
               &rsi_ble_conn_info[l_conn_id].get_profile,
               sizeof(rsi_ble_event_profile_by_uuid_t));
        LOG_EVENT("RSI_BLE_GATT_PROFILE - start_handle:0x%x -conn%d \n",
                  *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt].start_handle,
                  l_conn_id);
        LOG_EVENT("RSI_BLE_GATT_PROFILE - end_handle:0x%x -conn%d \n",
                  *(uint16_t *)rsi_ble_profile_list_by_conn.profile_info_uuid[prof_evt_cnt].end_handle,
                  l_conn_id);
        prof_evt_cnt = 2;
        uint8_t size = rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.size;
        if (size == 2) //! check for 16 bit(2 bytes) UUID value
        {
          //          val16 = rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val16;
          LOG_PRINT("RSI_BLE_GATT_PROFILES - uuid:0x%x, uuid_size:%d -conn%d\n",
                    rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val16,
                    size,
                    l_conn_id);
        } else if (size == 4) {
          //          uuid32_t val32 = rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val32;
          LOG_PRINT("RSI_BLE_GATT_PROFILES - uuid:0x%lx, uuid_size:%d -conn%d\n",
                    rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val32,
                    size,
                    l_conn_id);
        } else if (size == 16) //! 128 bit(16 byte) UUID value
        {
          //          uuid128_t val128 = rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val128;
          LOG_PRINT("RSI_BLE_GATT_PROFILES - uuid:0x%lx, uuid_size:%d -conn%d\n",
                    rsi_ble_profile_list_by_conn.profile_desc[prof_evt_cnt].profile_uuid.val.val128.data1,
                    size,
                    l_conn_id);
        }
        LOG_EVENT("RSI_BLE_GATT_PROFILE - resolved, -conn%d\n", l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILE);
        PRINT_EVENT_END();
      } break;

      case RSI_BLE_GATT_CHAR_SERVICES: {
        LOG_EVENT("\nRSI_BLE_GATT_CHAR_SERVICES\n");
        LOG_PRINT("in gatt test:RSI_BLE_GATT_CHAR_SERVICES -conn%d \n", l_conn_id);
        if (!service_char_mem_init) {
          if (rsi_ble_profile_list_by_conn.profile_char_info == NULL) {
            rsi_ble_profile_list_by_conn.profile_char_info =
              (rsi_ble_event_read_by_type1_t *)malloc(sizeof(rsi_ble_event_read_by_type1_t) * total_remote_profiles);
            LOG_PRINT("malloc in gatt test:RSI_BLE_GATT_CHAR_SERVICES -conn%d \n", l_conn_id);
          }
          if (rsi_ble_profile_list_by_conn.profile_char_info == NULL) {
            LOG_PRINT("failed to allocate memory for rsi_ble_profile_list_by_conn[%d].profile_char_info of size : %d\n",
                      l_conn_id,
                      sizeof(rsi_ble_event_read_by_type1_t) * total_remote_profiles);
            return;
          }
          memset(rsi_ble_profile_list_by_conn.profile_char_info,
                 0,
                 sizeof(rsi_ble_event_read_by_type1_t) * total_remote_profiles);
          service_char_mem_init = 1;
        }

        //TODO: setting char_for_serv_cnt to 0 and not soring all the characteristics as we are dealing with individual characteristics
        char_for_serv_cnt = 0;
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
        memcpy(&rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt],
               &rsi_ble_conn_info[l_conn_id].get_char_services,
               sizeof(rsi_ble_event_read_by_type1_t));
        uint8_t count = rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt].num_of_services;
        uint8_t cix;
        LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - num_of_services:%d -conn%d \n", count, l_conn_id);
        for (cix = 0; cix < count; cix++) {
          char_serv_t char_service =
            rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt].char_services[cix];
          LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - index:%d, handle:0x%x -conn%d \n",
                    cix,
                    char_service.handle,
                    l_conn_id);
          LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - index:%d, uuid:0x%x -conn%d \n",
                    cix,
                    char_service.char_data.char_uuid.val.val16,
                    l_conn_id);
          LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - index:%d, write_handle:0x%x -conn%d \n",
                    cix,
                    char_service.char_data.char_handle,
                    l_conn_id);
          LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - index:%d, reserved:%d -conn%d \n",
                    cix,
                    char_service.char_data.reserved,
                    l_conn_id);
          LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - index:%d, char_property:%d -conn%d \n",
                    cix,
                    char_service.char_data.char_property,
                    l_conn_id);
          // char_property
          // #define RSI_BLE_ATT_PROPERTY_READ 0x02
          // #define RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE 0x04
          // #define RSI_BLE_ATT_PROPERTY_WRITE 0x08
          // #define RSI_BLE_ATT_PROPERTY_NOTIFY 0x10
          // #define RSI_BLE_ATT_PROPERTY_INDICATE 0x20
        }
        LOG_EVENT("RSI_BLE_GATT_CHAR_SERVICES - resolved, -conn%d\n", l_conn_id);
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_CHAR_SERVICES);
      } break;

      case RSI_BLE_GATT_DESC_SERVICES: {
        LOG_EVENT("\nRSI_BLE_GATT_DESC_SERVICES\n");
        // char_for_serv_cnt = 0;
        // // LOG_PRINT("in gatt test:RSI_BLE_GATT_DESC_SERVICES : temp1 %d -conn%d \n", temp1, l_conn_id);
        // // LOG_PRINT("remote device profiles discovery completed -conn%d \n", l_conn_id);
        // LOG_PRINT("profile descritor UUID :0x%x \n\n",
        //             rsi_ble_profile_list_by_conn.profile_desc[char_for_serv_cnt].profile_uuid.val.val16);
        // for (uint8_t ix = 0; ix < rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt].num_of_services; ix++) {
        //   LOG_PRINT(
        //     "service characteristics UUID 0x%x\n\n ",
        //     rsi_ble_profile_list_by_conn.profile_char_info[char_for_serv_cnt].char_services[ix].char_data.char_uuid.val.val16);
        // }
        // LOG_PRINT("\n");
        // LOG_EVENT("RSI_BLE_GATT_DESC_SERVICES - resolved, -conn%d\n", l_conn_id);
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_DESC_SERVICES);
      } break;
      case RSI_BLE_SELECT_DATATRANSFER: {
        LOG_EVENT("\nRSI_BLE_SELECT_DATATRANSFER\n");
        //! clear the event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SELECT_DATATRANSFER);
      } break;
      case RSI_BLE_BUFF_CONF_EVENT: {
        LOG_EVENT("\nRSI_BLE_BUFF_CONF_EVENT\n");
        LOG_PRINT("in gatt RSI_BLE_BUFF_CONF_EVENT -conn%d \n", l_conn_id);
        if (!buff_config_done) {
          status = rsi_ble_set_wo_resp_notify_buf_info(rsi_connected_dev_addr,
                                                       ble_conn_conf->buff_mode_sel.buffer_mode,
                                                       ble_conn_conf->buff_mode_sel.buffer_cnt);
          LOG_PRINT("rsi_ble_set_wo_resp_notify_buf_info\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("failed to set the buffer configuration mode, error:0x%x -conn%d \n", status, l_conn_id);
            break;
          } else {
            buff_config_done = 1;
          }
        }
        //! set RSI_DATA_TRANSMIT_EVENT to transmit data to remote device
        LOG_PRINT("start transmitting data to - conn%d \n", l_conn_id);
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
        LOG_PRINT("RSI_BLE_BUFF_CONF_EVENT ->  RSI_CONN_UPDATE_REQ_EVENT -conn%d \n", l_conn_id);
        rsi_ble_set_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
      } break;
      case RSI_CONN_UPDATE_REQ_EVENT: {
        LOG_EVENT("\nRSI_CONN_UPDATE_REQ_EVENT\n");
        LOG_PRINT("in connection update req event -conn%d \n", l_conn_id);
        if (!conn_param_req_given) {
          //! update connection interval to 45ms, latency 0, supervision timeout 4s
          status = rsi_ble_conn_params_update(rsi_connected_dev_addr,
                                              ble_conn_conf->conn_param_update.conn_int,
                                              ble_conn_conf->conn_param_update.conn_int,
                                              ble_conn_conf->conn_param_update.conn_latncy,
                                              ble_conn_conf->conn_param_update.supervision_to);
          LOG_PRINT("rsi_ble_conn_params_update\n");
          if (status != RSI_SUCCESS) {
            //! check for procedure already in progress error
            if (status == RSI_ERROR_BLE_ATT_CMD_IN_PROGRESS) {
              LOG_PRINT("rsi_ble_conn_params_update procedure is already in progress -conn%d \n", l_conn_id);
            } else {
              LOG_PRINT("failed to update connection paramaters error:0x%lx -conn%d \n", status, l_conn_id);
            }
            break;
          } else {
            LOG_PRINT("connection params request was successfull -conn%d \n", l_conn_id);
            conn_param_req_given = 1;
          }
        }
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_CONN_UPDATE_REQ_EVENT);
        //! set RSI_DATA_RECEIVE_EVENT to receive data from remote device
        if (rsi_rx_from_rem_dev) {
          rsi_rx_from_rem_dev = 0;
          LOG_PRINT("start receiving data from - conn%d\n", l_conn_id);
          LOG_PRINT("RSI_CONN_UPDATE_REQ_EVENT ->  RSI_DATA_RECEIVE_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
        }
        //! set RSI_DATA_TRANSMIT_EVENT to transmit data to remote device
        if (rsi_tx_to_rem_dev) {
          rsi_tx_to_rem_dev = 0;
          LOG_PRINT("start transmitting data to - conn%d\n", l_conn_id);
          LOG_PRINT("RSI_CONN_UPDATE_REQ_EVENT ->  RSI_DATA_TRANSMIT_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
        }
      } break;
      case RSI_BLE_RECEIVE_REMOTE_FEATURES: {
        LOG_EVENT("\nRSI_BLE_RECEIVE_REMOTE_FEATURES\n");
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_RECEIVE_REMOTE_FEATURES);
        if (g_smp == 1) {
          LOG_EVENT("\nRSI_BLE_GATT_SMP_ENABLED\n");
          if (ble_conn_conf->smp_enable) {
            if (mtu_exchange_done) {
              if ((!smp_pairing_initated) && (!smp_pairing_request_received)) {
                //! initiating the SMP pairing process
                status = rsi_ble_smp_pair_request(rsi_connected_dev_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_ENABLE);
                LOG_EVENT("rsi_ble_smp_pair_request\n");
                if (status != RSI_SUCCESS) {
                  LOG_EVENT("start of SMP pairing process failed with error code %lx -conn%d \n", status, l_conn_id);
                } else {
                  smp_pairing_initated = 1;
                  rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
                  LOG_EVENT("smp pairing request initiated to %s - conn%d \n", str_remote_address, l_conn_id);
                }
              }
            }
          }
        } else
          LOG_EVENT("\n=======RSI_BLE_NON_SMP_CONN\n");
      } break;
      case RSI_BLE_CONN_UPDATE_COMPLETE_EVENT: {
        LOG_EVENT("\nRSI_BLE_CONN_UPDATE_COMPLETE_EVENT\n");
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_CONN_UPDATE_COMPLETE_EVENT);
        LOG_EVENT("conn updated device address : %s\n conn_interval:%d\n supervision timeout:%d -conn%d\n",
                  rsi_ble_conn_info[l_conn_id].remote_dev_addr,
                  rsi_ble_conn_info[l_conn_id].conn_update_resp.conn_interval,
                  rsi_ble_conn_info[l_conn_id].conn_update_resp.timeout,
                  l_conn_id);
#if UPDATE_CONN_PARAMETERS
        status = rsi_conn_update_request();
#endif
      } break;
      case RSI_BLE_DISCONN_EVENT: {
        LOG_EVENT("\nRSI_BLE_DISCONN_EVENT\n");

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

        //! check whether disconnection is from master
        if (rsi_ble_conn_info[l_conn_id].remote_device_role == MASTER_ROLE) {
          LOG_PRINT("master is disconnected, reason : 0x%x -conn%d \n", rsi_disconnect_reason[l_conn_id], l_conn_id);
          //! decrement the task count
          master_task_instances--;
          num_of_conn_masters--;
          // if (num_of_conn_masters < RSI_BLE_MAX_NBR_MASTERS) {
          //   status = rsi_ble_stop_advertising();
          //   if (status != RSI_SUCCESS) {
          //     LOG_PRINT("advertising failed to stop = 0x%lx -conn%d \n", status, l_conn_id);
          //     continue;
          //   }
          //   LOG_PRINT("In dis-conn evt, Start Adv -conn%d \n", l_conn_id);
          //   //! Advertising interval set to ~211.25ms with full advertising data payload
          //   status = rsi_ble_start_advertising();
          //   if (status != RSI_SUCCESS) {
          //     LOG_PRINT("advertising failed to start = 0x%lx -conn%d \n", status, l_conn_id);
          //     continue;
          //   }
          //   LOG_PRINT("advertising started -conn%d \n", l_conn_id);
          // }
        } else {
          LOG_PRINT("slave is disconnected, reason : 0x%x -conn%d \n", rsi_disconnect_reason[l_conn_id], l_conn_id);
          slave_task_instances--;
          num_of_conn_slaves--;

          // LOG_PRINT("RSI_BLE_DISCONN_EVENT ->  RSI_BLE_SCAN_RESTART_EVENT -conn%d \n", l_conn_id);
          // //rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
          // status = rsi_ble_stop_scanning();
          // LOG_PRINT("rsi_ble_stop_scanning\n");
          // print_status(status);
          // if (status != RSI_SUCCESS) {
          //   LOG_PRINT("scanning stop failed, cmd status = %lx -conn%d\n", status, l_conn_id);
          // } else {
          //   rsi_scan_in_progress = 0;
          // }
          // LOG_PRINT("Restarting scanning \n");
          // status = rsi_ble_start_scanning();
          // LOG_PRINT("rsi_ble_start_scanning\n");
          // print_status(status);
          // if (status != RSI_SUCCESS) {
          //   LOG_PRINT("scanning start failed, cmd status = %lx -conn%d\n", status, l_conn_id);
          // } else {
          //   rsi_scan_in_progress = 1;
          // }
        }
        rsi_disconnect_reason[l_conn_id] = 0;

        LOG_PRINT("Number of connected master devices:%d\n", num_of_conn_masters);
        LOG_PRINT("Number of connected slave devices:%d\n", num_of_conn_slaves);

        memset(&rsi_ble_conn_info[l_conn_id], 0, sizeof(rsi_ble_conn_info_t));

        //! delete the semaphore
        status = osSemaphoreDelete(ble_conn_sem[l_conn_id]);
        //        status = rsi_semaphore_destroy(&ble_conn_sem[l_conn_id]);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("failed to destroy semaphore %d \n", l_conn_id);
        }

        LOG_PRINT("delete task%d resources \n", l_conn_id);
        //! delete the task
        osThreadTerminate(ble_app_task_handle[l_conn_id]);
        //        rsi_task_destroy(ble_app_task_handle[l_conn_id]);
        l_conn_id = 0xff;
      } break;
      case RSI_BLE_GATT_WRITE_EVENT: {
        LOG_EVENT("RSI_BLE_GATT_WRITE_EVENT\n");
        //! event invokes when write/notification events received

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_WRITE_EVENT);

        //! process the received 'write response' data packet
        if ((*(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle)) == rsi_ble_att1_val_hndl) {
          rsi_ble_att_list_t *attribute = NULL;
          uint8_t opcode = 0x12, err = 0x00;
          attribute =
            rsi_gatt_get_attribute_from_list(&att_list,
                                             (*(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle)));

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

          //! Update the value based6 on the offset and length of the value
          if ((err == 0) && ((rsi_ble_conn_info[l_conn_id].app_ble_write_event.length) <= attribute->max_value_len)) {
            memset(attribute->value, 0, attribute->max_value_len);

            //! Check if value exists for the handle. If so, maximum length of the value.
            memcpy(attribute->value,
                   rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value,
                   rsi_ble_conn_info[l_conn_id].app_ble_write_event.length);

            //! Update value length
            attribute->value_len = rsi_ble_conn_info[l_conn_id].app_ble_write_event.length;

            LOG_PRINT("received data from remote device: %s \n", (uint8_t *)attribute->value);

            //! Send gatt write response
            rsi_ble_gatt_write_response(rsi_connected_dev_addr, 0);
          } else {
            //! Error : 0x07 - Invalid request,  0x0D - Invalid attribute value length
            err = 0x07;
          }

          if (err) {
            //! Send error response
            rsi_ble_att_error_response(rsi_connected_dev_addr,
                                       *(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle,
                                       opcode,
                                       err);
          }
        }

        //! print the received 'write no response' data packet
        if ((*(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle)) == rsi_ble_att2_val_hndl) {
          LOG_PRINT("received data from remote device: %s \n",
                    rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value);
        }

        //! when remote device enabled the notifications
        if (((*(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle) - 1) == rsi_ble_att1_val_hndl)) {
          notification_tx_handle = *(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle) - 1;
          if (ble_conn_conf->tx_notifications) {
            //check for valid notifications
            if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == NOTIFY_ENABLE) {
              LOG_PRINT("Remote device enabled the notification -conn%d\n", l_conn_id);
              rsi_tx_to_rem_dev = 1;
              //! configure the buffer configuration mode
              LOG_PRINT("RSI_BLE_GATT_WRITE_EVENT ->  RSI_BLE_BUFF_CONF_EVENT -conn%d \n", l_conn_id);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
            } else if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == NOTIFY_DISABLE) {
              LOG_PRINT("Remote device disabled the notification -conn%d\n", l_conn_id);
              //rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
            }
          }
        }

        else if (((*(uint16_t *)(rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle) - 1)
                  == rsi_ble_att3_val_hndl)) {
          if (ble_conn_conf->tx_indications) {
            //check for valid indications
            if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == INDICATION_ENABLE) {
              LOG_PRINT("Remote device enabled the indications -conn%d\n", l_conn_id);
              rsi_tx_to_rem_dev = 1;
              //! configure the buffer configuration mode
              LOG_PRINT("RSI_BLE_GATT_WRITE_EVENT ->  RSI_BLE_BUFF_CONF_EVENT -conn%d \n", l_conn_id);
              rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_BUFF_CONF_EVENT);
            } else if (rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value[0] == INDICATION_DISABLE) {
              LOG_PRINT("Remote device disabled the indications -conn%d\n", l_conn_id);
              //rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
            }
          }
        }
        //! code to handle remote device indications
        //! send acknowledgement to the received indication packet
        if (*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle == (indication_handle)) {
          if (ble_conn_conf->rx_indications) {
            LOG_PRINT("received indication packet from remote device, data= %s \n",
                      rsi_ble_conn_info[l_conn_id].app_ble_write_event.att_value);
#if RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
            //! Send indication acknowledgement to remote device
            status = rsi_ble_indicate_confirm(rsi_connected_dev_addr);
            LOG_PRINT("rsi_ble_indicate_confirm\n");
            //            print_status(status);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("indication confirm failed \t reason = %lx -conn%d\n", status, l_conn_id);
            } else {
              LOG_PRINT("indication confirm response sent -conn%d\n", l_conn_id);
            }
#endif
          }
        }

        //! code to handle remote device notifications
        else if (*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_write_event.handle == (notify_handle)) {
          if ((!notification_received) && (ble_conn_conf->rx_notifications)) {
            //! stop printing the logs after receiving first notification
            notification_received = 1;
            LOG_PRINT("receiving notifications from remote device -conn%d\n", l_conn_id);
          } else {
            //! do nothing as received notifications not required to print
          }
        }
      } break;
      case RSI_BLE_GATT_PREPARE_WRITE_EVENT: {
        LOG_EVENT("RSI_BLE_GATT_PREPARE_WRITE_EVENT\n");
        LOG_PRINT("PWE\n");
        uint8_t err = 0;
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PREPARE_WRITE_EVENT);
        if (*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle == rsi_ble_att1_val_hndl) {
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

          //! Update the value based6 on the offset and length of the value
          if ((err == 0)
              && ((*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.offset)
                  <= attribute->max_value_len)) {
            //LOG_PRINT("PWE - offset : %d\n",(*(uint16_t *)app_ble_prepared_write_event.offset));
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
      } break;
      case RSI_BLE_GATT_EXECUTE_WRITE_EVENT: {
        LOG_EVENT("\nRSI_BLE_GATT_EXECUTE_WRITE_EVENT\n");
        LOG_PRINT("\nEWE\n");
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_EXECUTE_WRITE_EVENT);
        if (*(uint16_t *)rsi_ble_conn_info[l_conn_id].app_ble_prepared_write_event.handle == rsi_ble_att1_val_hndl) {
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
      } break;
      case RSI_DATA_TRANSMIT_EVENT: {
        LOG_EVENT("\nRSI_DATA_TRANSMIT_EVENT\n");
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
      } break;
      case RSI_DATA_RECEIVE_EVENT: {
        LOG_EVENT("\nRSI_DATA_RECEIVE_EVENT\n");
        // ! To receive notifications from remote device, set the attribute value
        if (notify_handle_found && ble_conn_conf->rx_notifications) {
          uint8_t notify_data[2] = { 1, 0 };
          LOG_PRINT("in receive notification event -conn%d \n", l_conn_id);
          //! clear the served event
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
          status = rsi_ble_set_att_value(rsi_connected_dev_addr, //enable the notifications
                                         notify_handle + 1,
                                         2,
                                         notify_data);
          LOG_PRINT("rsi_ble_set_att_value\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              LOG_PRINT("notify failed with buffer error -conn%d \n", l_conn_id);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_RECEIVE_EVENT);
              break;
            } else {
              LOG_PRINT("notify value failed with status = %lx -conn%d \n", status, l_conn_id);
            }
          }
        }
        //! Transmit Gatt indications to remote device
        else if (indication_handle_found && ble_conn_conf->rx_indications) {
          uint8_t indicate_data[2] = { 2, 0 };
          LOG_PRINT("in receive indication event -conn%d \n", l_conn_id);
          rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_DATA_RECEIVE_EVENT);
          status = rsi_ble_set_att_value_async(rsi_connected_dev_addr, //enable the indications
                                               indication_handle + 1,
                                               2,
                                               indicate_data);
          LOG_PRINT("rsi_ble_set_att_value_async\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            if (status == RSI_ERROR_BLE_DEV_BUF_FULL) {
              LOG_PRINT("indication failed with buffer error -conn%d \n", l_conn_id);
              rsi_current_state[l_conn_id] |= BIT64(RSI_DATA_RECEIVE_EVENT);
              break;
            } else {
              LOG_PRINT("indication failed with status = %lx -conn%d \n", status, l_conn_id);
            }
          }
        }
      } break;
      case RSI_BLE_WRITE_EVENT_RESP: {
        LOG_EVENT("RSI_BLE_WRITE_EVENT_RESP\n");
        //! event invokes when write response received
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_WRITE_EVENT_RESP);
        // if (ble_conn_conf->tx_write) {
        //   LOG_PRINT("RSI_BLE_WRITE_EVENT_RESP ->  RSI_DATA_TRANSMIT_EVENT -conn%d \n", l_conn_id);
        //   rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
        //   LOG_PRINT("write response received -conn%d\n", l_conn_id);
        // }
      } break;
      case RSI_BLE_GATT_INDICATION_CONFIRMATION: {
        LOG_EVENT("\nRSI_BLE_GATT_INDICATION_CONFIRMATION\n");
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_INDICATION_CONFIRMATION);
        if (ble_conn_conf->tx_indications) {
          LOG_PRINT("RSI_BLE_WRITE_EVENT_RESP ->  RSI_DATA_TRANSMIT_EVENT -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_DATA_TRANSMIT_EVENT);
          LOG_PRINT("In rsi_ble_on_event_indication_confirmation_gatt event\n");
        }
      } break;

      case RSI_BLE_READ_REQ_EVENT: {
        LOG_EVENT("RSI_BLE_READ_REQ_EVENT\n");
        //! event invokes when write/notification events received

        LOG_PRINT("Read request initiated by remote device -conn%d \n", l_conn_id);
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
          LOG_PRINT("rsi_ble_gatt_read_response\n");
          //          print_status(status);
          offset = 0;
        } else {

          status = rsi_ble_gatt_read_response(rsi_connected_dev_addr, 0, handle, 0, (sizeof(read_data1)), read_data1);
          LOG_PRINT("rsi_ble_gatt_read_response\n");
          //          print_status(status);
        }
        if (status != RSI_SUCCESS) {
          LOG_PRINT("read response failed, error:0x%lx -conn%d \n", status, l_conn_id);
        } else {
          LOG_PRINT("response to read request initiated by remote device was successfull -conn%d \n", l_conn_id);
        }

      } break;
      case RSI_BLE_MTU_EVENT: {
        LOG_EVENT("==RSI_BLE_MTU_EVENT\n");
        //! event invokes when write/notification events received
        if (RSI_BLE_MTU_EXCHANGE_FROM_HOST) {
          uint8_t status = rsi_ble_mtu_exchange_resp(rsi_connected_dev_addr, LOCAL_MTU_SIZE);
          if (status != 0) {
            LOG_EVENT("\n MTU Exchange response failed \n");
          }
        }
        rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_ble_conn_info[l_conn_id].app_ble_mtu_event.dev_addr);
        LOG_EVENT("MTU size from remote device(%s), %d - conn%d\n",
                  str_remote_address,
                  rsi_ble_conn_info[l_conn_id].app_ble_mtu_event.mtu_size,
                  l_conn_id);
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_MTU_EVENT);
        mtu_exchange_done = 1;
        if (ble_conn_conf->smp_enable) {
          LOG_EVENT("RSI_BLE_MTU_EVENT ->  RSI_BLE_RECEIVE_REMOTE_FEATURES -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_RECEIVE_REMOTE_FEATURES);
        }
      } break;
      case RSI_BLE_SCAN_RESTART_EVENT: {
        LOG_EVENT("\nRSI_BLE_SCAN_RESTART_EVENT\n");
        //! clear the served event
        // rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
        // LOG_PRINT("In Scan Re-Start evt \n");
        // status = rsi_ble_stop_scanning();
        //   LOG_PRINT("rsi_ble_stop_scanning\n");
        //   print_status(status);
        // if (status != RSI_SUCCESS) {
        //   LOG_PRINT("scanning stop failed, cmd status = %lx -conn%d\n", status, l_conn_id);
        // }
        // LOG_PRINT("Restarting scanning \n");
        // status = rsi_ble_start_scanning();
        // LOG_PRINT("rsi_ble_start_scanning\n");
        // print_status(status);
        // if (status != RSI_SUCCESS) {
        //   LOG_PRINT("scanning start failed, cmd status = %lx -conn%d\n", status, l_conn_id);
        //   LOG_PRINT("RSI_BLE_SCAN_RESTART_EVENT ->  RSI_BLE_SCAN_RESTART_EVENT -conn%d \n", l_conn_id);
        //   rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_SCAN_RESTART_EVENT);
        // }
      } break;
      case RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ: {
        LOG_EVENT("\nRSI_APP_EVENT_REMOTE_CONN_PARAM_REQ\n");

        //!default ACCEPT the remote conn params request (0-ACCEPT, 1-REJECT)
        status = rsi_ble_conn_param_resp(rsi_ble_conn_info[l_conn_id].rsi_app_remote_device_conn_params.dev_addr, 0);
        LOG_EVENT("rsi_ble_conn_param_resp\n");
        if (status != RSI_SUCCESS) {
          LOG_EVENT("conn param resp status: 0x%lX\n", status);
        }
        //! remote device conn params request
        //! clear the conn params request event.
        LOG_EVENT("RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ ->  RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ -conn%d \n", l_conn_id);
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);

      } break;
      case RSI_APP_EVENT_DATA_LENGTH_CHANGE: {
        LOG_EVENT("\nRSI_APP_EVENT_DATA_LENGTH_CHANGE\n");
        // //! clear the disconnected event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_DATA_LENGTH_CHANGE);
        if (rsi_ble_conn_info[l_conn_id].remote_dev_feature.remote_features[1] & 0x01) {
          status = rsi_ble_setphy((int8_t *)rsi_ble_conn_info[l_conn_id].remote_dev_feature.dev_addr,
                                  TX_PHY_RATE,
                                  RX_PHY_RATE,
                                  CODDED_PHY_RATE);
          LOG_PRINT("rsi_ble_setphy\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("failed to cancel the connection request: 0x%lx \n -conn%d", status, l_conn_id);
          }
        }
      } break;

      case RSI_APP_EVENT_PHY_UPDATE_COMPLETE: {
        LOG_EVENT("\nRSI_APP_EVENT_PHY_UPDATE_COMPLETE\n");
        //! phy update complete event

        //! clear the phy updare complete event.
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
      } break;

      case RSI_BLE_SMP_REQ_EVENT: {
        LOG_EVENT("\nRSI_BLE_SMP_REQ_EVENT\n");
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_REQ_EVENT);

        LOG_EVENT("in smp request \n -conn%d \n", l_conn_id);
        if (ble_conn_conf->smp_enable) {
          smp_pairing_request_received = 1;

          if (first_connect == 0) {
            if (!smp_pairing_initated) {
              //! initiating the SMP pairing process
              status = rsi_ble_smp_pair_request(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_req.dev_addr,
                                                RSI_BLE_SMP_IO_CAPABILITY,
                                                MITM_ENABLE);
              LOG_EVENT("rsi_ble_smp_pair_request\n");
              //              print_status(status);
              if (status != RSI_SUCCESS) {
                LOG_EVENT("RSI_BLE_SMP_REQ_EVENT: failed to initiate the SMP pairing process: 0x%lx \n -conn%d",
                          status,
                          l_conn_id);
              } else {
                smp_pairing_initated = 1;
                //rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_connected_dev_addr);
                LOG_EVENT("smp pairing request initiated to %s - conn%d \n", str_remote_address, l_conn_id);
              }
            }

          } else {
            status = rsi_ble_start_encryption(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_req.dev_addr,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localediv,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localrand,
                                              rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled.localltk);
            LOG_PRINT("rsi_ble_start_encryption\n");
            //            print_status(status);
          }
        }
      } break;

      case RSI_BLE_SMP_RESP_EVENT: {
        LOG_EVENT("\nRSI_BLE_SMP_RESP_EVENT\n");

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_RESP_EVENT);

        LOG_PRINT("in smp response -conn%d \n", l_conn_id);
        if (ble_conn_conf->smp_enable) {
          //! initiating the SMP pairing process
          status = rsi_ble_smp_pair_response(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_resp.dev_addr,
                                             RSI_BLE_SMP_IO_CAPABILITY,
                                             MITM_ENABLE);
          LOG_PRINT("rsi_ble_smp_pair_response\n");
          //          print_status(status);
        }
      } break;

      case RSI_BLE_SMP_PASSKEY_EVENT: {
        LOG_EVENT("\nRSI_BLE_SMP_PASSKEY_EVENT\n");

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_PASSKEY_EVENT);

        LOG_PRINT("in smp passkey -conn%d \n", l_conn_id);

        LOG_PRINT("in smp_passkey - str_remote_address : %s\n", rsi_ble_conn_info[l_conn_id].remote_dev_addr);

        //! initiating the SMP pairing process
        status =
          rsi_ble_smp_passkey(rsi_ble_conn_info[l_conn_id].rsi_ble_event_smp_passkey.dev_addr, RSI_BLE_APP_SMP_PASSKEY);
      } break;
      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT: {
        LOG_EVENT("\nRSI_BLE_SMP_PASSKEY_DISPLAY_EVENT\n");
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
        LOG_PRINT("in smp pass key display -conn%d \n", l_conn_id);

        LOG_PRINT("remote addr: %s, passkey: %s \n",
                  rsi_ble_conn_info[l_conn_id].remote_dev_addr,
                  rsi_ble_conn_info[l_conn_id].rsi_ble_smp_passkey_display.passkey);
      } break;
      case RSI_BLE_LTK_REQ_EVENT: {
        LOG_EVENT("\nRSI_BLE_LTK_REQ_EVENT\n");
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_LTK_REQ_EVENT);

        LOG_PRINT("in LTK  request -conn%d \n", l_conn_id);

        if (0) /* ((rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.localediv == l_rsi_encryption_enabled.localediv)
					&& !((memcmp(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.localrand,
							l_rsi_encryption_enabled.localrand, 8))))*/
        {
          LOG_PRINT("positive reply\n");
          //! give le ltk req reply cmd with positive reply
          status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.dev_addr,
                                         1,
                                         l_rsi_encryption_enabled.localltk);
          LOG_PRINT("rsi_ble_ltk_req_reply\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("failed to restart smp pairing with status: 0x%lx -conn%d\n", status, l_conn_id);
          }
        } else {
          LOG_PRINT("negative reply\n");
          //! give le ltk req reply cmd with negative reply
          status = rsi_ble_ltk_req_reply(rsi_ble_conn_info[l_conn_id].rsi_le_ltk_resp.dev_addr, 0, NULL);
          LOG_PRINT("rsi_ble_ltk_req_reply\n");
          //          print_status(status);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("failed to restart smp pairing with status: 0x%lx \n", status);
          }
        }
      } break;

      case RSI_BLE_SC_PASSKEY_EVENT: {
        LOG_EVENT("\nRSI_BLE_SC_PASSKEY_EVENT\n");
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SC_PASSKEY_EVENT);

        LOG_PRINT("in smp sc passkey event -conn%d \n", l_conn_id);

        LOG_PRINT("In passkey event, remote addr: %s, passkey: %06ld -conn%d \n",
                  rsi_ble_conn_info[l_conn_id].remote_dev_addr,
                  rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.passkey,
                  l_conn_id);

        rsi_ble_smp_passkey(rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.dev_addr,
                            rsi_ble_conn_info[l_conn_id].rsi_event_sc_passkey.passkey);
      } break;

      case RSI_BLE_SECURITY_KEYS_EVENT: {
        LOG_EVENT("\nRSI_BLE_SECURITY_KEYS_EVENT\n");
        //! event invokes when security keys are received
        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SECURITY_KEYS_EVENT);

        LOG_PRINT("in smp security keys event  -conn%d \n", l_conn_id);

      } break;

      case RSI_BLE_SMP_FAILED_EVENT: {
        LOG_EVENT("\nRSI_BLE_SMP_FAILED_EVENT\n");
        //! initiate SMP protocol as a Master

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_SMP_FAILED_EVENT);

        LOG_PRINT("in smp failed remote address: %s -conn%d \n",
                  rsi_ble_conn_info[l_conn_id].remote_dev_addr,
                  l_conn_id);
      } break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT: {
        LOG_EVENT("\nRSI_BLE_ENCRYPT_STARTED_EVENT\n");
        //! start the encrypt event

        //! clear the served event
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_ENCRYPT_STARTED_EVENT);
        //! copy to local buffer
        memcpy(&l_rsi_encryption_enabled,
               &rsi_ble_conn_info[l_conn_id].rsi_encryption_enabled,
               sizeof(rsi_bt_event_encryption_enabled_t));
        LOG_PRINT("in smp encrypt event -conn%d \n", l_conn_id);
        smp_done = 1;
      } break;
      case RSI_BLE_GATT_ERROR: {
        PRINT_EVENT_START(GATT_PROFILES, BLE);
        LOG_PRINT("\nRSI_BLE_GATT_ERROR\n");
        rsi_ble_clear_event_based_on_conn(l_conn_id, RSI_BLE_GATT_ERROR);
        if ((*(uint16_t *)rsi_ble_conn_info[l_conn_id].rsi_ble_gatt_err_resp.error) == RSI_END_OF_PROFILE_QUERY) {
          LOG_EVENT("RSI_BLE_GATT_PROFILES - total_remote_profiles %d\n", total_remote_profiles);
          done_profiles_query = 1;
          LOG_EVENT("RSI_BLE_GATT_PROFILES ->  RSI_BLE_GATT_PROFILES -conn%d \n", l_conn_id);
          rsi_ble_set_event_based_on_conn(l_conn_id, RSI_BLE_GATT_PROFILES);
          LOG_PRINT("GATT ERROR REASON:0x%x -conn%d \n",
                    *(uint16_t *)rsi_ble_conn_info[l_conn_id].rsi_ble_gatt_err_resp.error,
                    l_conn_id);
          // }
        } else {
          LOG_PRINT("GATT ERROR REASON:0x%x -conn%d \n",
                    *(uint16_t *)rsi_ble_conn_info[l_conn_id].rsi_ble_gatt_err_resp.error,
                    l_conn_id);
        }
        PRINT_EVENT_END();
      } break;
      default:
        break;
    }
  }
}
