/*******************************************************************************
* @file  wifi_config.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
 *  @section Description
 *  Wi-Fi/BLE example configuration, application state enum, and (when
 *  SL_BLE_DYNAMIC_ENABLE_DISABLE_DEMO is 1) 16k SSL demo server macros plus
 *  externs for ble_disable_done_queue, ble_enable_done_queue, and
 *  rsi_wlan_init_wifi. Full behavior is documented in readme.md under
 *  "Optional: Dynamic BLE enable/disable and 16k SSL demo".
 *
 */
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#define RSI_APP_BUF_SIZE 1600

/* 0: standard path — load AWS TLS creds in rsi_wlan_mqtt_certs_init(), MQTT after DHCP.
 * 1: after DHCP — BLE disable, 16k TLS demo (SSL_16K_DEMO_*), sl_wifi_disconnect,
 *    BLE enable, wifi_app_init_and_reconnect(), load_certificates_in_flash(), MQTT.
 * Failures skip MQTT; see readme "Optional: Dynamic BLE enable/disable and 16k SSL demo". */
#define SL_BLE_DYNAMIC_ENABLE_DISABLE_DEMO 0

//! Enumeration for states in application
typedef enum wifi_app_state_e {
  WIFI_APP_INITIAL_STATE          = 0,
  WIFI_APP_UNCONNECTED_STATE      = 1,
  WIFI_APP_CONNECTED_STATE        = 2,
  WIFI_APP_IPCONFIG_DONE_STATE    = 3,
  WIFI_APP_SOCKET_CONNECTED_STATE = 4,
  WIFI_APP_SCAN_STATE             = 5,
  WIFI_APP_JOIN_STATE             = 6,
  WIFI_APP_SOCKET_RECEIVE_STATE   = 7,

  WIFI_APP_MQTT_INIT_STATE               = 8,
  WIFI_APP_MQTT_CONNECT_STATE            = 9,
  WIFI_APP_MQTT_AUTO_RECONNECT_SET_STATE = 18,
  WIFI_APP_MQTT_SUBSCRIBE_STATE          = 19,
  WIFI_APP_MQTT_PUBLISH_STATE            = 20,
  WIFI_APP_IDLE_STATE                    = 23,
  WIFI_APP_MQTT_DISCONNECT               = 24,
  WIFI_APP_SEND_DATA                     = 25,
  WIFI_APP_RECEIVE_DATA                  = 26,

  WIFI_APP_DISCONNECTED_STATE       = 11,
  WIFI_APP_DISCONN_NOTIFY_STATE     = 12,
  WIFI_APP_ERROR_STATE              = 21,
  WIFI_APP_FLASH_STATE              = 22,
  WIFI_APP_DATA_RECEIVE_STATE       = 15,
  WIFI_APP_SD_WRITE_STATE           = 16,
  WIFI_APP_DEMO_COMPLETE_STATE      = 17,
  WIFI_APP_AWS_SELECT_CONNECT_STATE = 13,
  WIFI_APP_SLEEP_STATE              = 14
} wifi_app_state_t;

typedef enum rsi_app_cmd_e {
  WIFI_APP_DATA                 = 0,
  WIFI_APP_SCAN_RESP            = 1,
  WIFI_APP_CONNECTION_STATUS    = 2,
  WIFI_APP_DISCONNECTION_STATUS = 3,
  WIFI_APP_DISCONNECTION_NOTIFY = 4,
  WIFI_APP_TIMEOUT_NOTIFY       = 5,
#if SL_BLE_DYNAMIC_ENABLE_DISABLE_DEMO
  WIFI_APP_BLE_ENABLE_REQUEST = 6
#endif
} rsi_app_cmd_t;

#if SL_BLE_DYNAMIC_ENABLE_DISABLE_DEMO
/*
 * 16k-record TLS lab demo: two TLS 1.2 clients to the same host, different ports.
 * These values must match your test setup — they are not chosen for any network by default.
 *
 * - SSL_16K_DEMO_SERVER_IP: IPv4 of the machine running the OpenSSL servers (often your PC;
 *   use an address on the same subnet as the module after Wi-Fi connect).
 * - PORT_1 / PORT_2: Each must match one listening openssl s_server -accept <port> instance.
 * See readme.md "Optional: Dynamic BLE enable/disable and 16k SSL demo" and
 * "Lab TLS server (OpenSSL example)".
 */
#define SSL_16K_DEMO_SERVER_IP     "192.168.0.100"
#define SSL_16K_DEMO_SERVER_PORT_1 4443
#define SSL_16K_DEMO_SERVER_PORT_2 4444

#include "cmsis_os2.h"
extern osMessageQueueId_t ble_disable_done_queue;
extern osMessageQueueId_t ble_enable_done_queue;
extern void rsi_wlan_init_wifi(void);
#endif

#endif
