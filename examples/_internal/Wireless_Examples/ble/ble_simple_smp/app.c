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
 * Include files
 * */

//! SL Wi-Fi SDK includes
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"

//! local device IO capability
#define RSI_BLE_SMP_IO_CAPABILITY 0x03
#define RSI_BLE_SMP_PASSKEY       0

//! BLE include file to refer BLE APIs
#include "rsi_ble_apis.h"
#include "ble_config.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common_apis.h"

//! Common include file
#include "rsi_common_apis.h"
#include <string.h>

#define CENTRAL_ROLE    1
#define PERIPHERAL_ROLE 0

//! local device name
#define RSI_BLE_DEVICE_NAME (void *)"BLE_SIMPLE_SMP"
#define RSI_SCAN_RESP_DATA  "SILABS"
#define ROLE                PERIPHERAL_ROLE

//! application event list
#define RSI_APP_EVENT_ADV_REPORT          0x00
#define RSI_BLE_CONN_EVENT                0x01
#define RSI_BLE_DISCONN_EVENT             0x02
#define RSI_BLE_SMP_REQ_EVENT             0x03
#define RSI_BLE_SMP_RESP_EVENT            0x04
#define RSI_BLE_SMP_PASSKEY_EVENT         0x05
#define RSI_BLE_SMP_FAILED_EVENT          0x06
#define RSI_BLE_ENCRYPT_STARTED_EVENT     0x07
#define RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT 0x08
#define RSI_BLE_LTK_REQ_EVENT             0x09
#define RSI_BLE_MTU_EVENT                 0x0A

#if (ROLE == CENTRAL_ROLE)
//! Address type of the device to connect
#define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS

//! Address of the device to connect
#define RSI_BLE_DEV_ADDR "00:23:A7:80:70:B9"

//! Remote Device Name to connect
#define RSI_REMOTE_DEVICE_NAME "SILABS_DEV"

#endif

#define MITM_REQ 0x01
//! global parameters list
static volatile uint32_t ble_app_event_map;
static uint32_t ble_app_event_mask;
static uint8_t str_remote_address[18];
static uint8_t remote_addr_type = 0;
static uint8_t remote_name[31];
static uint8_t remote_dev_addr[18]   = { 0 };
static uint8_t remote_dev_bd_addr[6] = { 0 };
static uint8_t device_found          = 0;
rsi_bt_event_encryption_enabled_t glbl_enc_enabled;
rsi_bt_event_le_ltk_request_t temp_le_ltk_req;
rsi_ble_event_mtu_t app_ble_mtu_event;
static rsi_bt_event_smp_passkey_display_t smp_passkey_display_event;

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
  ble_app_event_map  = 0;
  ble_app_event_mask = 0xFFFFFFFF;
  ble_app_event_mask = ble_app_event_mask; //To suppress warning while compiling
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  ble_app_event_map |= BIT(event_num);
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
  ble_app_event_map &= ~BIT(event_num);
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
    if (ble_app_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}
/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{
  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
#if (ROLE == CENTRAL_ROLE)
  if ((device_found == 0) && ((strcmp(remote_name, RSI_REMOTE_DEVICE_NAME)) == 0)
      || ((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE) && ((strcmp(remote_dev_addr, RSI_BLE_DEV_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
#endif
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  memcpy((int8_t *)remote_dev_bd_addr, resp_enh_conn->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, resp_enh_conn->dev_addr);

  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy((int8_t *)remote_dev_bd_addr, resp_conn->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, resp_conn->dev_addr);

  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy((int8_t *)remote_dev_bd_addr, resp_disconnect->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, resp_disconnect->dev_addr);

  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_request 
 * @brief      its invoked when smp request event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP request events is received(we are in Central mode)
 * Note: peripheral requested to start SMP request, we have to send SMP request command
 */
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_dev_address)
{
  memcpy((int8_t *)remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr);

  rsi_ble_app_set_event(RSI_BLE_SMP_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_response 
 * @brief      its invoked when smp response event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP response events is received(we are in peripheral mode) 
 * Note: Central initiated SMP protocol, we have to send SMP response command
 */
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_dev_address)
{
  memcpy((int8_t *)remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr);

  rsi_ble_app_set_event(RSI_BLE_SMP_RESP_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey 
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *remote_dev_address)
{
  memcpy((int8_t *)remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr);
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey_display 
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey_display(rsi_bt_event_smp_passkey_display_t *smp_passkey_display)
{
  memcpy(&smp_passkey_display_event, smp_passkey_display, sizeof(rsi_bt_event_smp_passkey_display_t));

  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_failed 
 * @brief      its invoked when smp failed event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP failed events is received
 */
void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *remote_dev_address)
{
  UNUSED_PARAMETER(status);

  memcpy((int8_t *)remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  LOG_PRINT("smp_failed \r\n");
  rsi_ble_app_set_event(RSI_BLE_SMP_FAILED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_encrypt_started 
 * @brief      its invoked when encryption started event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when encryption started events is received
 */
void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&glbl_enc_enabled, enc_enabled, sizeof(rsi_bt_event_encryption_enabled_t));
  rsi_ble_app_set_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_ltk_req_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  LOG_PRINT(" \r\n rsi_ble_on_le_ltk_req_event \r\n");
  memcpy(&temp_le_ltk_req, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));
  rsi_ble_app_set_event(RSI_BLE_LTK_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_mtu_event
 * @brief      its invoked when write/notify/indication events are received.
 * @param[in]  event_id, it indicates write/notification event id.
 * @param[in]  rsi_ble_write, write event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when write/notify/indication events are received
 */
static void rsi_ble_on_mtu_event(rsi_ble_event_mtu_t *rsi_ble_mtu)
{
  memcpy(&app_ble_mtu_event, rsi_ble_mtu, sizeof(rsi_ble_event_mtu_t));
  rsi_ble_app_set_event(RSI_BLE_MTU_EVENT);
}

/*==============================================*/
/**
 * @fn         ble_smp_test_app
 * @brief      This application is used for testing the SMP protocol.
 * @param[in]  none. 
 * @return     none.
 * @section description
 * This function is used to test the SMP Protocol.
 */
void ble_smp_test_app(void *argument)
{
  UNUSED_PARAMETER(argument);

  int32_t status = 0;
  int32_t event_id;
  uint8_t adv[31] = { 2, 1, 6 };
#if (ROLE == CENTRAL_ROLE)
  uint8_t already_paired = 0;
#endif

  status = sl_wifi_init(&config, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nWi-Fi Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\n Wi-Fi Initialization Success\n");
  }

  //! registering the GAP callback functions
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //! registering the SMP callback functions
  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request,
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 rsi_ble_on_smp_passkey_display,
                                 NULL,
                                 rsi_ble_on_le_ltk_req_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //! registering the GATT callback functions
  rsi_ble_gatt_register_callbacks(NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  rsi_ble_on_mtu_event,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL);

  //!  initializing the application events map
  rsi_ble_app_init_events();

  //! Set local name
  rsi_bt_set_local_name(RSI_BLE_DEVICE_NAME);

#if (ROLE == CENTRAL_ROLE)
  //! start scanning
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Start scanning failed, error status : %lx \r\n", status);
    return;
  }
  LOG_PRINT("\r\n Scanning started \r\n");
#else
  //!preparing scan response data
  adv[3] = strlen(RSI_SCAN_RESP_DATA) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_SCAN_RESP_DATA);

  //!set scan response data
  rsi_ble_set_scan_response_data(adv, strlen(RSI_SCAN_RESP_DATA) + 5);

  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_DEVICE_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_DEVICE_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_DEVICE_NAME) + 5);

  //! start the advertising
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Start advertising failed, error status : %lx \r\n", status);
    return;
  }
  LOG_PRINT("\r\n Advertising started \r\n");
#endif

  //! waiting for events from controller.
  while (1) {

    //! checking for events list
    event_id = rsi_ble_app_get_event();
    if (event_id == -1) {
      continue;
    }

    switch (event_id) {
#if (ROLE == CENTRAL_ROLE)
      case RSI_APP_EVENT_ADV_REPORT: {
        //! advertise report event.
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);

        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%X\r\n", status);
        }
      } break;
#endif
      case RSI_BLE_CONN_EVENT: {
        //! event invokes when connection was completed
        LOG_PRINT("\r\n Module connected to address : %s \r\n", str_remote_address);

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);

      } break;

      case RSI_BLE_DISCONN_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
        LOG_PRINT("\r\nModule got Disconnected\r\n");
#if (ROLE == PERIPHERAL_ROLE)
        //! start the advertising
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Start advertising failed, error status : %lx \r\n", status);
          return;
        }
        LOG_PRINT("\r\n Advertising started \r\n");
#endif
#if (ROLE == CENTRAL_ROLE)
        //! start scanning
        device_found = 0;
        status       = rsi_ble_start_scanning();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("start_scanning status: 0x%X\r\n", status);
        }
        LOG_PRINT("\r\n scanning started \r\n");
#endif
      } break;
      case RSI_BLE_MTU_EVENT: {
        //! event invokes when write/notification events received

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_MTU_EVENT);
#if (ROLE == CENTRAL_ROLE)
        if (!already_paired) {
          //! If not paired even once
          //! initiating the SMP pairing process
          status = rsi_ble_smp_pair_request(app_ble_mtu_event.dev_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
        }
#endif
      } break;
      case RSI_BLE_LTK_REQ_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_LTK_REQ_EVENT);
        //rsi_6byte_dev_address_to_ascii (remote_dev_bd_addr,temp_le_ltk_req.dev_addr);

#if (ROLE == PERIPHERAL_ROLE)
        if ((temp_le_ltk_req.localediv == glbl_enc_enabled.localediv)
            && !((memcmp(temp_le_ltk_req.localrand, glbl_enc_enabled.localrand, 8)))) {
          LOG_PRINT("\n positive reply\n");
          //! give le ltk req reply cmd with positive reply
          status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr, 1, glbl_enc_enabled.localltk);
        } else {
          LOG_PRINT("\n negative reply\n");
          //! give le ltk req reply cmd with negative reply
          status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr, 0, NULL);
        }
#endif
      } break;

      case RSI_BLE_SMP_REQ_EVENT: {
        //! initiate SMP protocol as a Central
        LOG_PRINT("\n\n smp_req - str_remote_address : %s\r\n", str_remote_address);

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_REQ_EVENT);

#if (ROLE == CENTRAL_ROLE)
        if (already_paired)
          //! If once paired
          //! initiating the SMP pairing process
          status = rsi_ble_smp_pair_request(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
#endif
      } break;

      case RSI_BLE_SMP_RESP_EVENT: {
        //! initiate SMP protocol as a Central
        LOG_PRINT("smp_resp - str_remote_address : %s\r\n", str_remote_address);

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_RESP_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_pair_response(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
      } break;

      case RSI_BLE_SMP_PASSKEY_EVENT: {
        //! initiate SMP protocol as a Central
        LOG_PRINT("smp_passkey - str_remote_address : %s\r\n", str_remote_address);

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_passkey(remote_dev_bd_addr, RSI_BLE_SMP_PASSKEY);
      } break;
      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT: {
        //! clear the served event

        uint8_t passkey[BLE_PASSKEY_SIZE];

        memset(passkey, 0, BLE_PASSKEY_SIZE);

        rsi_6byte_dev_address_to_ascii(str_remote_address, smp_passkey_display_event.dev_addr);
        memcpy(passkey, smp_passkey_display_event.passkey, BLE_PASSKEY_SIZE);

        LOG_PRINT("Pass_key display event remote addr: %s, passkey: %s \r\n", str_remote_address, (char *)passkey);
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
      } break;

      case RSI_BLE_SMP_FAILED_EVENT: {
        //! initiate SMP protocol as a Central

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_FAILED_EVENT);
      } break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT: {
        //! start the encrypt event

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
#if (ROLE == CENTRAL_ROLE)
        already_paired = 1;
#endif
      } break;
      default: {
      }
    }
  }
}

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);

  osThreadNew((osThreadFunc_t)ble_smp_test_app, NULL, &thread_attributes);
}
