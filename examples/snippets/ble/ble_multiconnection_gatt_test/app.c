/*******************************************************************************
* @file  app.c
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
 * @file    app.c
 * @version 0.1
 * @date    19 Mar 2021
 *
 *  @brief : This file contains driver, module initialization and application execution
 *  @section Description
 *           This file contains driver, module initializations
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

//! BLE include file to refer BLE APIs
#include "ble_config.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common.h"
#include "rsi_bt_common_apis.h"
#include "rsi_common_apis.h"

//! SL Wi-Fi SDK includes
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"
#include "app.h"
#include <string.h>

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//! Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
#define PSP_MODE RSI_SLEEP_MODE_2
#define PSP_TYPE RSI_MAX_PSP

sl_wifi_performance_profile_t wifi_profile = { ASSOCIATED_POWER_SAVE, 0, 0, 1000 };
#endif

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! flag to check bt power save
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
osSemaphoreId_t ble_main_task_sem, ble_peripheral_conn_sem, ble_scan_sem;
bool powersave_cmd_given;

static const sl_wifi_device_configuration_t config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_BLE_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
                                       | SL_SI91X_FEAT_DEV_TO_HOST_ULP_GPIO_1),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
#ifdef CHIP_917
                      | RAM_LEVEL_NWP_ADV_MCU_BASIC | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#else //defaults
#ifdef RSI_M4_INTERFACE
                      | RAM_LEVEL_NWP_MEDIUM_MCU_MEDIUM
#else
                      | RAM_LEVEL_NWP_ALL_MCU_ZERO
#endif
#endif
                      | SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE),
                   .bt_feature_bit_map = (SL_SI91X_BT_RF_TYPE | SL_SI91X_ENABLE_BLE_PROTOCOL
#if (RSI_BT_GATT_ON_CLASSIC)
                                          | SL_SI91X_BT_ATT_OVER_CLASSIC_ACL /* to support att over classic acl link */
#endif
                                          ),
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   //!ENABLE_BLE_PROTOCOL in bt_feature_bit_map
                   .ble_feature_bit_map =
                     ((SL_SI91X_BLE_MAX_NBR_PERIPHERALS(RSI_BLE_MAX_NBR_PERIPHERALS)
                       | SL_SI91X_BLE_MAX_NBR_CENTRALS(RSI_BLE_MAX_NBR_CENTRALS)
                       | SL_SI91X_BLE_MAX_NBR_ATT_SERV(RSI_BLE_MAX_NBR_ATT_SERV)
                       | SL_SI91X_BLE_MAX_NBR_ATT_REC(RSI_BLE_MAX_NBR_ATT_REC))
                      | SL_SI91X_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_BLE_PWR_INX(RSI_BLE_PWR_INX)
                      | SL_SI91X_BLE_PWR_SAVE_OPTIONS(RSI_BLE_PWR_SAVE_OPTIONS)
                      | SL_SI91X_916_BLE_COMPATIBLE_FEAT_ENABLE
#if RSI_BLE_GATT_ASYNC_ENABLE
                      | SL_SI91X_BLE_GATT_ASYNC_ENABLE
#endif
                      ),
                   .ble_ext_feature_bit_map =
                     ((SL_SI91X_BLE_NUM_CONN_EVENTS(RSI_BLE_NUM_CONN_EVENTS)
                       | SL_SI91X_BLE_NUM_REC_BYTES(RSI_BLE_NUM_REC_BYTES))
#if RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
                      | SL_SI91X_BLE_INDICATE_CONFIRMATION_FROM_HOST //indication response from app
#endif
#if RSI_BLE_MTU_EXCHANGE_FROM_HOST
                      | SL_SI91X_BLE_MTU_EXCHANGE_FROM_HOST //MTU Exchange request initiation from app
#endif
#if RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST
                      | (SL_SI91X_BLE_SET_SCAN_RESP_DATA_FROM_HOST) //Set SCAN Resp Data from app
#endif
#if RSI_BLE_DISABLE_CODED_PHY_FROM_HOST
                      | (SL_SI91X_BLE_DISABLE_CODED_PHY_FROM_HOST) //Disable Coded PHY from app
#endif
#if BLE_SIMPLE_GATT
                      | SL_SI91X_BLE_GATT_INIT
#endif
                      ),
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP) }
};

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
  .reserved   = 0,
};

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

#if ENABLE_POWER_SAVE
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

  LOG_PRINT("\r\n Keep module into power save \r\n");
  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
    return status;
  }

  //! initiating power save in wlan mode
  status = sl_wifi_set_performance_profile(&wifi_profile);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\n Failed to initiate power save in Wi-Fi mode :%ld\r\n", status);
    return status;
  }

  LOG_PRINT("\r\n Module is in power save \r\n");
  return status;
}
#endif // #if ENABLE_POWER_SAVE

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
    if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
      //! Initialize peripheral1 configurations
      ble_conn_spec_conf[PERIPHERAL1].smp_enable        = SMP_ENABLE_P1;
      ble_conn_spec_conf[PERIPHERAL1].add_to_acceptlist = ADD_TO_ACCEPTLIST_P1;
      ble_conn_spec_conf[PERIPHERAL1].profile_discovery = PROFILE_QUERY_P1;
      ble_conn_spec_conf[PERIPHERAL1].data_transfer     = DATA_TRANSFER_P1;
      //ble_conn_spec_conf[PERIPHERAL1].bidir_datatransfer = SMP_ENABLE_P1;
      ble_conn_spec_conf[PERIPHERAL1].rx_notifications                 = RX_NOTIFICATIONS_FROM_P1;
      ble_conn_spec_conf[PERIPHERAL1].rx_indications                   = RX_INDICATIONS_FROM_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_notifications                 = TX_NOTIFICATIONS_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_write                         = TX_WRITES_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_indications                   = TX_INDICATIONS_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_int       = CONN_INTERVAL_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_latncy    = CONN_LATENCY_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P1;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
      //! Initialize peripheral2 configurations
      ble_conn_spec_conf[PERIPHERAL2].smp_enable        = SMP_ENABLE_P2;
      ble_conn_spec_conf[PERIPHERAL2].add_to_acceptlist = ADD_TO_ACCEPTLIST_P2;
      ble_conn_spec_conf[PERIPHERAL2].profile_discovery = PROFILE_QUERY_P2;
      ble_conn_spec_conf[PERIPHERAL2].data_transfer     = DATA_TRANSFER_P2;
      //ble_conn_spec_conf[PERIPHERAL2].bidir_datatransfer = SMP_ENABLE_P2;
      ble_conn_spec_conf[PERIPHERAL2].rx_notifications                 = RX_NOTIFICATIONS_FROM_P2;
      ble_conn_spec_conf[PERIPHERAL2].rx_indications                   = RX_INDICATIONS_FROM_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_notifications                 = TX_NOTIFICATIONS_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_write                         = TX_WRITES_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_indications                   = TX_INDICATIONS_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_int       = CONN_INTERVAL_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_latncy    = CONN_LATENCY_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P2;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
      //! Initialize peripheral3 configurations
      ble_conn_spec_conf[PERIPHERAL3].smp_enable        = SMP_ENABLE_P3;
      ble_conn_spec_conf[PERIPHERAL3].add_to_acceptlist = ADD_TO_ACCEPTLIST_P3;
      ble_conn_spec_conf[PERIPHERAL3].profile_discovery = PROFILE_QUERY_P3;
      ble_conn_spec_conf[PERIPHERAL3].data_transfer     = DATA_TRANSFER_P3;
      //ble_conn_spec_conf[PERIPHERAL3].bidir_datatransfer = SMP_ENABLE_P3;
      ble_conn_spec_conf[PERIPHERAL3].rx_notifications                 = RX_NOTIFICATIONS_FROM_P3;
      ble_conn_spec_conf[PERIPHERAL3].rx_indications                   = RX_INDICATIONS_FROM_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_notifications                 = TX_NOTIFICATIONS_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_write                         = TX_WRITES_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_indications                   = TX_INDICATIONS_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_int       = CONN_INTERVAL_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_latncy    = CONN_LATENCY_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P3;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
      //! Initialize central1 configurations
      ble_conn_spec_conf[CENTRAL1].smp_enable        = SMP_ENABLE_C1;
      ble_conn_spec_conf[CENTRAL1].add_to_acceptlist = ADD_TO_ACCEPTLIST_C1;
      ble_conn_spec_conf[CENTRAL1].profile_discovery = PROFILE_QUERY_C1;
      ble_conn_spec_conf[CENTRAL1].data_transfer     = DATA_TRANSFER_C1;
      //ble_conn_spec_conf[CENTRAL1].bidir_datatransfer = SMP_ENABLE_C1;
      ble_conn_spec_conf[CENTRAL1].rx_notifications                 = RX_NOTIFICATIONS_FROM_C1;
      ble_conn_spec_conf[CENTRAL1].rx_indications                   = RX_INDICATIONS_FROM_C1;
      ble_conn_spec_conf[CENTRAL1].tx_notifications                 = TX_NOTIFICATIONS_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_write                         = TX_WRITES_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_indications                   = TX_INDICATIONS_TO_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_int       = CONN_INTERVAL_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_latncy    = CONN_LATENCY_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C1;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
      //! Initialize central2 configurations
      ble_conn_spec_conf[CENTRAL2].smp_enable        = SMP_ENABLE_C2;
      ble_conn_spec_conf[CENTRAL2].add_to_acceptlist = ADD_TO_ACCEPTLIST_C2;
      ble_conn_spec_conf[CENTRAL2].profile_discovery = PROFILE_QUERY_C2;
      ble_conn_spec_conf[CENTRAL2].data_transfer     = DATA_TRANSFER_C2;
      //ble_conn_spec_conf[CENTRAL2].bidir_datatransfer = SMP_ENABLE_C2;
      ble_conn_spec_conf[CENTRAL2].rx_notifications                 = RX_NOTIFICATIONS_FROM_C2;
      ble_conn_spec_conf[CENTRAL2].rx_indications                   = RX_INDICATIONS_FROM_C2;
      ble_conn_spec_conf[CENTRAL2].tx_notifications                 = TX_NOTIFICATIONS_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_write                         = TX_WRITES_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_indications                   = TX_INDICATIONS_TO_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_int       = CONN_INTERVAL_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_latncy    = CONN_LATENCY_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C2;
    }
  } else {
    LOG_PRINT("\r\n Invalid buffer passed \r\n");
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
void rsi_common_app_task(void)
{
  uint32_t status                  = RSI_SUCCESS;
  powersave_cmd_given              = false;
  sl_wifi_version_string_t version = { 0 };

  status = sl_wifi_init(&config, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\n Wi-Fi Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\n Wi-Fi Initialization Success\n");
  }

  //! Firmware version Prints
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
  } else {
    LOG_PRINT("\r\nfirmware_version = %s\r\n", version.version);
  }

  //! fill the configurations in local structure based on compilation macros
  status = rsi_fill_user_config();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n failed to fill the configurations in local buffer \r\n");
    return;
  }

  //! create ble main task if ble protocol is selected
  ble_main_task_sem = osSemaphoreNew(1, 0, NULL);
  ble_scan_sem      = osSemaphoreNew(1, 0, NULL);

  if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
    ble_peripheral_conn_sem = osSemaphoreNew(1, 0, NULL);
  }

  rsi_ble_main_app_task();
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)rsi_common_app_task, NULL, &thread_attributes);
}
