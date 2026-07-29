/*******************************************************************************
* @file  gatt_common_config.h
* @brief GATT Common Configuration - Shared by Server and Client
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

#ifndef GATT_COMMON_CONFIG_H
#define GATT_COMMON_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> GATT Common Configuration
// <i> Shared by GATT Server and Client components

// <q RSI_BLE_MTU_EXCHANGE_FROM_HOST> Host initiates MTU exchange
// <i> 1 = Host initiates MTU exchange, 0 = Wait for peer. Default: 1
#ifndef RSI_BLE_MTU_EXCHANGE_FROM_HOST
#define RSI_BLE_MTU_EXCHANGE_FROM_HOST 1
#endif

// <q RSI_BLE_GATT_ASYNC_ENABLE> GATT async mode
// <i> 1 = Async mode enabled, 0 = Sync mode. Default: 1
#ifndef RSI_BLE_GATT_ASYNC_ENABLE
#define RSI_BLE_GATT_ASYNC_ENABLE 1
#endif

// <q BLE_SIMPLE_GATT> Simple GATT mode
// <i> 1 = Simple GATT init, 0 = Full GATT mode. Default: 0
#ifndef BLE_SIMPLE_GATT
#define BLE_SIMPLE_GATT 0
#endif

// </h>
// <<< end of configuration section >>>

#endif // GATT_COMMON_CONFIG_H
