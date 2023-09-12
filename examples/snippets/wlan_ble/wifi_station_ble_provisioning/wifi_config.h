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
#ifndef RSI_CONFIG_H
#define RSI_CONFIG_H

#define RSI_APP_BUF_SIZE 1600

// Enumeration for states in application
typedef enum sl_wifi_app_state_e {
  SL_WIFI_INITIAL_STATE             = 0,
  SL_WIFI_UNCONNECTED_STATE         = 1,
  SL_WIFI_CONNECTED_STATE           = 2,
  SL_WIFI_IPCONFIG_DONE_STATE       = 3,
  SL_WIFI_SCAN_STATE                = 4,
  SL_WIFI_JOIN_STATE                = 5,
  SL_WIFI_SOCKET_RECEIVE_STATE      = 6,
  SL_WIFI_MQTT_INIT_DONE_STATE      = 7,
  SL_WIFI_MQTT_SUBSCRIBE_DONE_STATE = 8,
  SL_BLE_GATT_WRITE_EVENT           = 9,
  SL_WIFI_DISCONNECTED_STATE        = 10,
  SL_WIFI_DISCONN_NOTIFY_STATE      = 11,
  SL_WIFI_ERROR_STATE               = 12,
  SL_WIFI_FLASH_STATE               = 13,
  SL_WIFI_DATA_RECEIVE_STATE        = 14,
  SL_SD_WRITE_STATE                 = 15,
  SL_WIFI_DEMO_COMPLETE_STATE       = 16
} sl_wifi_app_state_t;

typedef enum rsi_app_cmd_e {
  SL_DATA                      = 0,
  SL_WIFI_SCAN_RESP            = 1,
  SL_WIFI_CONNECTION_STATUS    = 2,
  SL_WIFI_DISCONNECTION_STATUS = 3,
  SL_WIFI_DISCONNECTION_NOTIFY = 4,
  SL_WIFI_TIMEOUT_NOTIFY       = 5
} rsi_app_cmd_t;

#endif
