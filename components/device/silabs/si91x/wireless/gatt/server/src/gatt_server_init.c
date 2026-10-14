/*******************************************************************************
* @file  gatt_server_init.c
* @brief GATT Server Component - Initialization
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
#include <gatt_server_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gatt_server.h"
#include "gap.h"
#include "sl_si91x_status.h"

extern rsi_parsed_conf_t rsi_parsed_conf;

extern void sl_btc_ble_gatt_server_register_callbacks_wrapper(void);
extern void sl_btc_ble_gatt_extended_register_callbacks_wrapper(void);

int32_t ble_gatt_server_init(void)
{
  int32_t status;

  printf("\r\n Initializing GATT Server...\r\n");

  // Step 1: Register GATT server callbacks
  sl_btc_ble_gatt_server_register_callbacks_wrapper();
  sl_btc_ble_gatt_extended_register_callbacks_wrapper();

  // Step 2: Initialize GATT Server-specific connection state (from gatt_server_utilities.c)
  rsi_ble_gatt_server_default_init();

  // Step 3: Initialize GATT Server-specific connection buffer configuration (from gatt_server_utilities.c)
  status = rsi_ble_gatt_server_initialize_conn_config(
    (rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to initialize GATT Server conn config\r\n");
    return status;
  }

  // Step 4: Add GATT services (extracted from ble_unified_app.c)
  status = rsi_ble_add_simple_chat_serv();
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to add simple chat service\r\n");
    return status;
  }

  status = rsi_ble_add_simple_chat_serv2();
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to add simple chat service 2\r\n");
    return status;
  }

  status = rsi_ble_add_custom_service_serv();
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to add custom service\r\n");
    return status;
  }

  printf("\r\n GATT Server initialized successfully\r\n");
  return SL_STATUS_OK;
}
