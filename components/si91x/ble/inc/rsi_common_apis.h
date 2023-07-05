/*******************************************************************************
* @file  rsi_common_apis.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef RSI_COMMON_APIS_H
#define RSI_COMMON_APIS_H

#if defined(RSI_DEBUG_PRINTS)
#include <stdio.h>
#endif
#ifdef RSI_DEBUG_PRINTS
#include "rsi_common.h"
#endif
/******************************************************
 * *                      Macros
 * ******************************************************/
#if !(defined(FRDM_K28F) || defined(MXRT_595s))
#if defined(RSI_DEBUG_PRINTS)
#define LOG_PRINT(...)                                                       \
  {                                                                          \
    osMutexAcquire(rsi_driver_cb_non_rom->debug_prints_mutex, 0xFFFFFFFFUL); \
    printf(__VA_ARGS__);                                                     \
    osMutexRelease(rsi_driver_cb_non_rom->debug_prints_mutex);               \
  }
#elif defined(DEBUG_UART)
#define LOG_PRINT(...) DEBUGOUT(__VA_ARGS__)
#else
#define LOG_PRINT(...)
#endif
#endif

// success return value
#define RSI_SUCCESS 0

// failure return value
#define RSI_FAILURE -1

//Load Image types
#define LOAD_NWP_FW                    '1'
#define LOAD_DEFAULT_NWP_FW_ACTIVE_LOW 0x71

// Upgrade images
#define BURN_NWP_FW 'B'

// Upgrade images
#define RSI_JUMP_TO_PC 'J'

#define RSI_ACTIVE_LOW_INTR  0x2
#define RSI_ACTIVE_HIGH_INTR 0x0
#define RSI_RX_BUFFER_CHECK  0x15
// TX buffer full macro
#define RSI_TX_BUFFER_FULL -2
#define RSI_SOFT_RESET     0
#define RSI_HARD_RESET     1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
// enumerations for power save profile modes
typedef enum rsi_power_save_profile_mode_e {
  RSI_ACTIVE        = 0,
  RSI_SLEEP_MODE_1  = 1,
  RSI_SLEEP_MODE_2  = 2,
  RSI_SLEEP_MODE_8  = 8,
  RSI_SLEEP_MODE_10 = 10,
} rsi_power_save_profile_mode_t;

// enumerations for power save profile types
typedef enum rsi_power_save_profile_type_e {
  RSI_MAX_PSP  = 0,
  RSI_FAST_PSP = 1,
  RSI_UAPSD    = 2
} rsi_power_save_profile_type_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#include <stdint.h>
extern int32_t rsi_ble_driver_init(uint8_t *buffer, uint32_t length);
extern int32_t rsi_ble_driver_deinit(void);
extern int32_t rsi_get_fw_version(uint8_t *response, uint16_t length);
extern int32_t rsi_get_module_type(uint8_t *response);
extern int32_t rsi_common_debug_log(int32_t assertion_type, int32_t assertion_level);
extern int32_t rsi_get_ram_log(uint32_t addr, uint32_t length);
extern int32_t rsi_driver_version(uint8_t *request);
#endif
