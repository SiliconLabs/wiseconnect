/*******************************************************************************
* @file  gatt_common.h
* @brief GATT Common - Shared types and constants
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

#ifndef GATT_COMMON_H
#define GATT_COMMON_H

// Shared GATT types and constants
// Error codes and CCCD descriptor values used by both server and client.
// Common code is automatically included when either server or client component is selected.

// Notify/Indication CCCD descriptor values (from Bluetooth Specification)
#define NOTIFY_DISABLE     0x00
#define NOTIFY_ENABLE      0x01
#define INDICATION_DISABLE 0x00
#define INDICATION_ENABLE  0x02

// Error code for buffer allocation (from ble_config.h)
#ifndef RSI_ERROR_IN_BUFFER_ALLOCATION
#define RSI_ERROR_IN_BUFFER_ALLOCATION 0x4e65
#endif

#include <stdint.h>

// GATT Common initialization function
int32_t ble_gatt_common_init(void);

// GATT Common event handlers (shared by server and client)
void rsi_conn_update_req_event(uint8_t conn_id);

#endif // GATT_COMMON_H
