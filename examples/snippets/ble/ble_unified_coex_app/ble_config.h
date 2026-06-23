/*******************************************************************************
* @file  ble_config.h
* @brief BLE Application Configuration
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

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

/******************************************************
 * BLE Configuration Entry Point
 * 
 * This file is required by SPAI and serves as the
 * application-level configuration entry point.
 * It includes the BLE Unified component configuration.
 ******************************************************/

// Include BLE Unified App API and configuration first
// This ensures unified app configurations become the defaults
#include "ble_unified_app.h"

// Include BLE SAPI common configuration (from 'ble' component)
#include "rsi_ble_common_config.h"

/******************************************************
 * Application-Specific Configuration Overrides
 * 
 * Add any application-specific BLE configuration
 * overrides here if needed
 ******************************************************/

// Example: Override connection limits for this specific app
// #undef RSI_BLE_MAX_NBR_PERIPHERALS
// #define RSI_BLE_MAX_NBR_PERIPHERALS 2

// Example: Enable power save for this app
// #undef ENABLE_NWP_POWER_SAVE
// #define ENABLE_NWP_POWER_SAVE 1

#endif // BLE_CONFIG_H
