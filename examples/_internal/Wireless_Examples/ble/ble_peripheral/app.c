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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for BLE Simple Peripheral
 * @section Description :
 * This application demonstrates how to configure the device in simple peripheral
 * mode and how to get connected from the remote Central device.
 =================================================================================*/

/**
 * Include files
 * */

//! SL Wi-Fi SDK includes
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"

//! BLE include file to refer BLE APIs
#include "rsi_ble_apis.h"
#include "ble_config.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common_apis.h"

//! Common include file
#include "rsi_common_apis.h"
#include <string.h>

#define RSI_BLE_LOCAL_NAME (void *)"BLE_PERIPHERAL"

#define RSI_SEL_ANTENNA    RSI_SEL_INTERNAL_ANTENNA //RSI_SEL_EXTERNAL_ANTENNA
#define RSI_SCAN_RESP_DATA "BLE_PERIPHERAL"

//! application events list
#define RSI_APP_EVENT_CONNECTED    1
#define RSI_APP_EVENT_DISCONNECTED 2

//! Application global parameters.
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static uint8_t rsi_app_resp_device_state                        = 0;
static int8_t rsi_app_resp_rssi                                 = 0;
static volatile uint8_t rsi_app_async_event_map                 = 0;
static rsi_ble_event_conn_status_t rsi_app_connected_device     = { 0 };
static rsi_ble_event_disconnect_t rsi_app_disconnected_device   = { 0 };
static uint8_t remote_dev_addr[18]                              = { 0 };

static const sl_wifi_device_configuration_t config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_BLE_MODE,
#ifdef RSI_M4_INTERFACE
                   .feature_bit_map = (SL_SI91X_FEAT_WPS_DISABLE | RSI_FEATURE_BIT_MAP),
#else
                   .feature_bit_map        = RSI_FEATURE_BIT_MAP,
#endif
#if RSI_TCP_IP_BYPASS
                   .tcp_ip_feature_bit_map = RSI_TCP_IP_FEATURE_BIT_MAP,
#else
                   .tcp_ip_feature_bit_map = (RSI_TCP_IP_FEATURE_BIT_MAP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
#endif
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID | RSI_CUSTOM_FEATURE_BIT_MAP),
                   .ext_custom_feature_bit_map = (
#ifdef CHIP_917
                     (RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#else //defaults
#ifdef RSI_M4_INTERFACE
                     (SL_SI91X_EXT_FEAT_256K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#else
                     (SL_SI91X_EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#endif
#endif
                     | (SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE)
#if (defined A2DP_POWER_SAVE_ENABLE)
                     | SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(2)
#endif
                       ),
                   .bt_feature_bit_map = (RSI_BT_FEATURE_BITMAP
#if (RSI_BT_GATT_ON_CLASSIC)
                                          | SL_SI91X_BT_ATT_OVER_CLASSIC_ACL /* to support att over classic acl link */
#endif
                                          ),
#ifdef RSI_PROCESS_MAX_RX_DATA
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID
                                                  | SL_SI91X_EXT_TCP_MAX_RECV_LENGTH),
#else
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
#endif
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
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | RSI_CONFIG_FEATURE_BITMAP) }
};

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = 0,
  .tz_module  = 0,
  .reserved   = 0,
};

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  rsi_app_async_event_map = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  rsi_app_async_event_map |= BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  rsi_app_async_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (rsi_app_async_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_conn_status_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_conn_status_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_simple_peripheral_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         ble_peripheral
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
  * @return    none.
 * @section description
 * This function is used to test the BLE peripheral role and simple GAP API's.
 */
void ble_peripheral(void *argument)
{
  UNUSED_PARAMETER(argument);

  int32_t status         = 0;
  int32_t temp_event_map = 0;
  uint8_t adv[31]        = { 2, 1, 6 };

  //! Wi-Fi initialization
  status = sl_wifi_init(&config, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nWi-Fi Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  }

  LOG_PRINT("\r\nWireless Initialization Success\r\n");

  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(NULL,
                                 rsi_ble_simple_peripheral_on_conn_status_event,
                                 rsi_ble_simple_peripheral_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_simple_peripheral_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //! initialize the event map
  rsi_ble_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGet MAC Addr Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("Get local device address: %x:%x:%x:%x\n",
              rsi_app_resp_get_dev_addr[3],
              rsi_app_resp_get_dev_addr[2],
              rsi_app_resp_get_dev_addr[1],
              rsi_app_resp_get_dev_addr[0]);
  }

  //! set the local device name
  status = rsi_bt_set_local_name(RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet Device Name Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\nSet Device Name Success\r\n");
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGet Device Name Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\nGet Device Name Success\r\n");
  }

  //! select/set the antenna type(internal/external)
  /*	status = rsi_bt_set_antenna(RSI_SEL_ANTENNA);
	if(status != RSI_SUCCESS)
	{
		return;
	}
	 */
  //!preparing scan response data
  adv[3] = strlen(RSI_SCAN_RESP_DATA) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_SCAN_RESP_DATA);

  //!set scan response data
  rsi_ble_set_scan_response_data(adv, strlen(RSI_SCAN_RESP_DATA) + 5);

  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_LOCAL_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_LOCAL_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_LOCAL_NAME) + 5);

  //! start the advertising
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nBLE Start Advertising Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\nBLE Start Advertising Success\r\n");
  }

  while (1) {
    //! Application main loop

    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);
        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr, (uint8_t *)rsi_app_connected_device.dev_addr);
        LOG_PRINT("\r\n Module connected to address : %s \r\n", remote_dev_addr);

        //! converts the 6 byte MAC address to string format
        //rsi_6byte_dev_address_to_ascii ((int8_t *)remote_dev_addr, rsi_app_connected_device.dev_addr);

        //! get the RSSI value with connected remote device
        status = rsi_bt_get_rssi((uint8_t *)rsi_app_connected_device.dev_addr, &rsi_app_resp_rssi);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nGet RSSI Failed, Error Code : 0x%lX\r\n", status);
          return;
        }

        //! get the local device state.
        status = rsi_ble_get_device_state(&rsi_app_resp_device_state);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nGet Device State Failed, Error Code : 0x%lX\r\n", status);
          return;
        }
      } break;

      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device connected event

        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        LOG_PRINT("\r\nModule got Disconnected\r\n");

        //! get the local device state.
        status = rsi_ble_get_device_state(&rsi_app_resp_device_state);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nGet Device State Failed, Error Code : 0x%lX\r\n", status);
          return;
        }

//! set device in advertising mode.
adv:
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nStart Advertising Failed, Error Code : 0x%lX\r\n", status);
          goto adv;
        }
      } break;
      default: {
      }
    }
  }
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);

  osThreadNew((osThreadFunc_t)ble_peripheral, NULL, &thread_attributes);
}
