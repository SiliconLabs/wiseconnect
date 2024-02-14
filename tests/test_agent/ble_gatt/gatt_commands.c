/*******************************************************************************
* @file  main.c
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
 * @file    main.c
 * @version 0.1
 * @date    19 Mar 2021
 *
 *
 *
 *  @brief : This file contains driver, module initialization and application execution
 *
 *  @section Description  This file contains driver, module initializations
 *
 *
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

//#include "sl_cli.h"
//#include "sl_cli_types.h"
//#include "sl_cli_arguments.h"
#include "console.h"
#include <stdio.h>
#include <string.h>
//#include "rsi_driver.h"
#include "rsi_common_apis.h"
#include "rsi_bt_common_apis.h"
#include "rsi_gatt_common_config.h"

#define RSI_PRINT_LOGS 0

#ifndef LOG_PRINT
#if RSI_PRINT_LOGS
#define LOG_PRINT(...) printf(__VA_ARGS__)
#else
#define LOG_PRINT(...)
#endif
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
#define RSI_DRIVER_TASK_PRIORITY   4         //! Wireless driver task priority
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2) //! Wireless driver task stack size
#define RSI_COMMON_TASK_PRIORITY   0         //! application task priority
#define RSI_COMMON_TASK_STACK_SIZE (512 * 2) //! application task size

#define GLOBAL_BUFF_LEN 15000 //! Memory length of driver

/*=======================================================================*/
//! Powersave configurations
/*=======================================================================*/
#define PSP_MODE RSI_SLEEP_MODE_2
#define PSP_TYPE RSI_MAX_PSP

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
osThreadId_t common_task_handle = NULL;
osThreadId_t driver_task_handle = NULL;
//! Memory to initialize driver
//uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
//! flag to check bt power save
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
osSemaphoreId_t ble_main_task_sem, ble_slave_conn_sem, ble_scan_sem;
osThreadId_t ble_main_app_task_handle;
uint8_t powersave_cmd_given;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
/*==============================================*/
/**
 * @fn         rsi_initiate_power_save
 * @brief      send power save command to RS9116 module
 *
 * @param[out] none
 * @return     status of commands, success-> 0, failure ->-1
 * @section description
 * This function sends command to keep module in power save
 */
int32_t rsi_initiate_power_save(void)
{
  int32_t status = RSI_SUCCESS;

  //! enable wlan radio
  //  status = rsi_wlan_radio_init();
  //  if (status != RSI_SUCCESS) {
  //    LOG_PRINT("\n radio init failed \n");
  //  }

  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("Failed to initiate power save in BLE mode \n");
    return status;
  }

  //! initiating power save in wlan mode
  //  status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
  //  if (status != RSI_SUCCESS) {
  //    LOG_PRINT("Failed to initiate power save in WLAN mode \n");
  //    return status;
  //  }

  return status;
}

/*==============================================*/
/**
 * @fn         rsi_ble_initialize_conn_buffer
 * @brief      this function initializes the configurations for each connection
 * @param[out] none
 * @param[out] none
 * @return     none
 * @section description
 */
int8_t rsi_ble_initialize_conn_buffer(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  int8_t status = RSI_SUCCESS;
  if (ble_conn_spec_conf != NULL) {
    if (RSI_BLE_MAX_NBR_SLAVES > 0) {
      //! Initialize slave1 configurations
      ble_conn_spec_conf[SLAVE1].smp_enable        = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].add_to_acceptlist = ADD_TO_ACCEPTLIST_S1;
      ble_conn_spec_conf[SLAVE1].profile_discovery = PROFILE_QUERY_S1;
      ble_conn_spec_conf[SLAVE1].data_transfer     = DATA_TRANSFER_S1;
      //ble_conn_spec_conf[SLAVE1].bidir_datatransfer = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].rx_notifications                 = RX_NOTIFICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].rx_indications                   = RX_INDICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].tx_notifications                 = TX_NOTIFICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write                         = TX_WRITES_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_indications                   = TX_INDICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_int       = CONN_INTERVAL_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_latncy    = CONN_LATENCY_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S1;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 1) {
      //! Initialize slave2 configurations
      ble_conn_spec_conf[SLAVE2].smp_enable        = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].add_to_acceptlist = ADD_TO_ACCEPTLIST_S2;
      ble_conn_spec_conf[SLAVE2].profile_discovery = PROFILE_QUERY_S2;
      ble_conn_spec_conf[SLAVE2].data_transfer     = DATA_TRANSFER_S2;
      //ble_conn_spec_conf[SLAVE2].bidir_datatransfer = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].rx_notifications                 = RX_NOTIFICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].rx_indications                   = RX_INDICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].tx_notifications                 = TX_NOTIFICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write                         = TX_WRITES_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_indications                   = TX_INDICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_int       = CONN_INTERVAL_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_latncy    = CONN_LATENCY_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S2;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 2) {
      //! Initialize SLAVE3 configurations
      ble_conn_spec_conf[SLAVE3].smp_enable        = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].add_to_acceptlist = ADD_TO_ACCEPTLIST_S3;
      ble_conn_spec_conf[SLAVE3].profile_discovery = PROFILE_QUERY_S3;
      ble_conn_spec_conf[SLAVE3].data_transfer     = DATA_TRANSFER_S3;
      //ble_conn_spec_conf[SLAVE3].bidir_datatransfer = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].rx_notifications                 = RX_NOTIFICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].rx_indications                   = RX_INDICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].tx_notifications                 = TX_NOTIFICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write                         = TX_WRITES_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_indications                   = TX_INDICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_int       = CONN_INTERVAL_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_latncy    = CONN_LATENCY_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S3;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 0) {
      //! Initialize master1 configurations
      ble_conn_spec_conf[MASTER1].smp_enable        = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].add_to_acceptlist = ADD_TO_ACCEPTLIST_M1;
      ble_conn_spec_conf[MASTER1].profile_discovery = PROFILE_QUERY_M1;
      ble_conn_spec_conf[MASTER1].data_transfer     = DATA_TRANSFER_M1;
      //ble_conn_spec_conf[MASTER1].bidir_datatransfer = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].rx_notifications                 = RX_NOTIFICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].rx_indications                   = RX_INDICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].tx_notifications                 = TX_NOTIFICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write                         = TX_WRITES_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_indications                   = TX_INDICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_int       = CONN_INTERVAL_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_latncy    = CONN_LATENCY_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M1;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 1) {
      //! Initialize master2 configurations
      ble_conn_spec_conf[MASTER2].smp_enable        = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].add_to_acceptlist = ADD_TO_ACCEPTLIST_M2;
      ble_conn_spec_conf[MASTER2].profile_discovery = PROFILE_QUERY_M2;
      ble_conn_spec_conf[MASTER2].data_transfer     = DATA_TRANSFER_M2;
      //ble_conn_spec_conf[MASTER2].bidir_datatransfer = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].rx_notifications                 = RX_NOTIFICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].rx_indications                   = RX_INDICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].tx_notifications                 = TX_NOTIFICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write                         = TX_WRITES_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_indications                   = TX_INDICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_int       = CONN_INTERVAL_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_latncy    = CONN_LATENCY_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M2;
    }
  } else {
    LOG_PRINT("Invalid buffer passed \n");
    status = RSI_FAILURE;
  }
  return status;
}

/*==============================================*/
/**
 * @fn         rsi_fill_user_config
 * @brief      this function fills the compile time user inputs to local buffer
 * @param[out] none
 * @return     none.
 * @section description
 * this function fills the compile time userinputs to local buffer
 */
int8_t rsi_fill_user_config()
{
  int8_t status = RSI_SUCCESS;
  //! copy protocol selection macros
  status = rsi_ble_initialize_conn_buffer((rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  return status;
}

/*==============================================*/
/**
 * @fn         rsi_common_app_task
 * @brief      This function creates the main tasks for selected protocol
 * @param[out] none
 * @return     none.
 * @section description
 * This function creates the main tasks for enabled protocols based on local buffer 'rsi_parsed_conf'
 */
sl_status_t init_ble_gatt_command(console_args_t *arguments)
{
  (void)arguments;
  int8_t status            = RSI_SUCCESS;
  powersave_cmd_given      = 0;
  ble_main_app_task_handle = NULL;
  g_smp                    = sl_cli_get_argument_uint8(arguments, 0);

  // ! fill the configurations in local structure based on compilation macros
  status = rsi_fill_user_config();
  LOG_EVENT("\n init_gatt status : %d \n", status);
  // print_status(status);
  // if (status != RSI_SUCCESS) {
  //   LOG_PRINT("failed to fill the configurations in local buffer \n");
  //   return;
  // }

  //! create ble main task if ble protocol is selected
  ble_main_task_sem = osSemaphoreNew(1, 0, 0);
  ble_scan_sem      = osSemaphoreNew(1, 0, 0);
  //  rsi_semaphore_create(&ble_main_task_sem, 0);
  //  rsi_semaphore_create(&ble_scan_sem, 0);
  if (RSI_BLE_MAX_NBR_SLAVES > 0) {
    ble_slave_conn_sem = osSemaphoreNew(1, 0, 0);
    //    rsi_semaphore_create(&ble_slave_conn_sem, 0);
  }

  const osThreadAttr_t thread_attributes = {
    .name       = "ble_main_task",
    .attr_bits  = 0,
    .cb_mem     = 0,
    .cb_size    = 0,
    .stack_mem  = 0,
    .stack_size = RSI_BLE_APP_MAIN_TASK_SIZE,
    .priority   = RSI_BLE_MAIN_TASK_PRIORITY,
    .tz_module  = 0,
    .reserved   = 0,
  };
  ble_main_app_task_handle = osThreadNew((osThreadFunc_t)rsi_ble_main_app_task, NULL, &thread_attributes);
  //  status = rsi_task_create((rsi_task_function_t)rsi_ble_main_app_task,
  //                           (uint8_t *)"ble_main_task",
  //                           RSI_BLE_APP_MAIN_TASK_SIZE,
  //                           NULL,
  //                           RSI_BLE_MAIN_TASK_PRIORITY,
  //                           &ble_main_app_task_handle);

  LOG_EVENT("\n rsi_task_create \n");
  if (ble_main_app_task_handle == 0) {
    LOG_EVENT("ble main task failed to create \n");
  }
  return status;
}

sl_status_t rsi_ble_get_long_att_value_async_command(console_args_t *arguments)
{
  char *remote_dev_addr     = sl_cli_get_argument_string(arguments, 0);
  uint8_t device_address[6] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev(device_address, (int8_t *)remote_dev_addr);
  uint16_t handle = sl_cli_get_argument_uint16(arguments, 1);
  uint16_t offset = sl_cli_get_argument_uint16(arguments, 2);
  return (rsi_ble_get_long_att_value_async(device_address, handle, offset, NULL));
}
