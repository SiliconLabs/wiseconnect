/*******************************************************************************
* @file  wifi_config.h
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
 * @file         wifi_config.h
 * @version      0.1
 *
 *  @brief : This file contains user configurable details to configure the device
 *
 *  @section Description  This file contains user configurable details to configure the device
 *
 *
 */
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#define RSI_APP_BUF_SIZE 1600

//! Enumeration for states in application
typedef enum sl_wifi_app_state_e {
  SL_WIFI_INITIAL_STATE          = 0,
  SL_WIFI_UNCONNECTED_STATE      = 1,
  SL_WIFI_CONNECTED_STATE        = 2,
  SL_WIFI_IPCONFIG_DONE_STATE    = 3,
  SL_WIFI_SOCKET_CONNECTED_STATE = 4,
  SL_WIFI_SCAN_STATE             = 5,
  SL_WIFI_JOIN_STATE             = 6,
  SL_WIFI_SOCKET_RECEIVE_STATE   = 7,

  SL_WIFI_MQTT_INIT_STATE               = 8,
  SL_WIFI_MQTT_CONNECT_STATE            = 9,
  SL_WIFI_MQTT_AUTO_RECONNECT_SET_STATE = 18,
  SL_WIFI_MQTT_SUBSCRIBE_STATE          = 19,
  SL_WIFI_MQTT_PUBLISH_STATE            = 20,
  SL_IDLE_STATE                         = 23,
  SL_WLAN_MQTT_DISCONNECT               = 24,
  SEND_DATA                             = 25,
  RECEIVE_DATA                          = 26,
  /*
  SL_WIFI_MQTT_INIT_DONE_STATE      = 8,
  SL_WIFI_MQTT_SUBSCRIBE_DONE_STATE = 9,
  */
  SL_BLE_GATT_WRITE_EVENT      = 10,
  SL_WIFI_DISCONNECTED_STATE   = 11,
  SL_WIFI_DISCONN_NOTIFY_STATE = 12,
  SL_WIFI_ERROR_STATE          = 21,
  SL_WIFI_FLASH_STATE          = 22,
  SL_WIFI_DATA_RECEIVE_STATE   = 15,
  SL_SD_WRITE_STATE            = 16,
  SL_WIFI_DEMO_COMPLETE_STATE  = 17,
  RSI_AWS_SELECT_CONNECT_STATE = 13,
  RSI_SLEEP_STATE              = 14
} sl_wifi_app_state_t;

/*typedef struct sl_wlan_app_cb_s {
  //! wlan application state
  volatile sl_wifi_app_state_t state;

  //! length of buffer to copy
  uint32_t length;

  //! application buffer
  uint8_t buffer[RSI_APP_BUF_SIZE];

  //! to check application buffer availability
  uint8_t buf_in_use;

  //! application events bit map
  uint32_t event_map;

} sl_wlan_app_cb_t;*/
//sl_wlan_app_cb_t sl_wlan_app_cb; //! application control block

typedef enum rsi_app_cmd_e {
  SL_DATA                      = 0,
  SL_WIFI_SCAN_RESP            = 1,
  SL_WIFI_CONNECTION_STATUS    = 2,
  SL_WIFI_DISCONNECTION_STATUS = 3,
  SL_WIFI_DISCONNECTION_NOTIFY = 4,
  SL_WIFI_TIMEOUT_NOTIFY       = 5
} rsi_app_cmd_t;

#endif
