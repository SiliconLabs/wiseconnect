/*******************************************************************************
* @file  gatt_client_init.c
* @brief GATT Client Component - Initialization
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
#include <gatt_client_config.h> // resolved via -I: config/ble_config/ (user override) first, then component inc/
#include "gatt_client.h"
#include "gap.h"
#include "sl_si91x_status.h"

extern rsi_parsed_conf_t rsi_parsed_conf;
extern void sl_btc_ble_gatt_client_register_callbacks_wrapper(void);

int32_t ble_gatt_client_init(void)
{
  int32_t status;

  printf("\r\n Initializing GATT Client...\r\n");

  // Step 1: Register GATT client callbacks
  sl_btc_ble_gatt_client_register_callbacks_wrapper();

  // Step 2: Initialize GATT Client-specific connection state
  rsi_ble_gatt_client_default_init();

  // Step 3: Initialize GATT Client-specific connection buffer configuration
  status = rsi_ble_gatt_client_initialize_conn_config(
    (rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  if (status != RSI_SUCCESS) {
    printf("\r\n Failed to initialize GATT Client conn config\r\n");
    return status;
  }

  // Step 4: Assign remote service and characteristic UUIDs
  rsi_assign_remote_data_serv_and_char();

  printf("\r\n GATT Client initialized successfully\r\n");
  return SL_STATUS_OK;
}
