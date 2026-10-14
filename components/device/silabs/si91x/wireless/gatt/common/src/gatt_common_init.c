/*******************************************************************************
* @file  gatt_common_init.c
* @brief GATT Common Component - Initialization
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
#include "gatt_common.h"
#include "sl_status.h"

extern void sl_btc_ble_gatt_common_register_callbacks_wrapper(void);

int32_t ble_gatt_common_init(void)
{
  printf("\r\n Initializing GATT Common...\r\n");

  // Register GATT common callbacks (event_gatt_write dispatcher)
  sl_btc_ble_gatt_common_register_callbacks_wrapper();

  printf("\r\n GATT Common initialized successfully\r\n");
  return SL_STATUS_OK;
}
