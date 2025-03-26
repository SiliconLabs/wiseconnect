/*******************************************************************************
* @file  rsi_debug.h
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

#ifndef __RSI_DEBUG_H__
#define __RSI_DEBUG_H__

#include <stdio.h>
#include <stdint.h>
#include "rsi_ccp_user_config.h"
#include "sl_component_catalog.h"
#ifdef DEBUG_UART_UC
#ifndef SL_SI91X_ULP_UART
#include "sl_si91x_debug_uc_1_config.h"
#else
#include "sl_si91x_debug_uc_2_config.h"
#endif
#endif
#ifdef SL_CATALOG_SI91X_IOSTREAM_PRINTS_PRESENT
#include "sl_iostream.h"
#include "sl_si91x_iostream_log.h"
#include "sl_si91x_iostream_log_config.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/*Sample print prototype*/
void Board_Debug_Init(void);
void Board_UARTPutSTR(const uint8_t *ptr);
uint8_t Board_UARTGetChar(void);
void Board_UARTPutChar(uint8_t ch);
void dummy_printf(const char *fmt, ...);

// Define debug macros based on configuration
#if defined(IOSTREAM_USART) || SL_SI91X_IOSTREAM_LOG_PRINTS_ENABLE
// SI91X IOStream prints configuration
#if SL_SI91X_IOSTREAM_LOG_PRINTS_ENABLE
#define DEBUGINIT() sl_si91x_iostream_log_init()
#else
#define DEBUGINIT()
#endif
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) (void)str
#define DEBUGIN()

#elif defined(DEBUG_UART)
// Debug over UART configuration
#define DEBUGINIT()   Board_Debug_Init()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) Board_UARTPutSTR(str)
#define DEBUGIN()     Board_UARTGetChar()

#else
// No debug output configuration
#define DEBUGINIT()
#define DEBUGOUT(...) dummy_printf(__VA_ARGS__)
#define DEBUGSTR(str) (void)str
#define DEBUGIN()
#endif

#endif // __RSI_DEBUG_H__
