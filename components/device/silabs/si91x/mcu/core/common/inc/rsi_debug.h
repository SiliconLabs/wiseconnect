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

#ifdef __cplusplus
extern "C" {
#endif

/*Sample print prototype*/
void Board_Debug_Init(void);
void Board_UARTPutSTR(uint8_t *ptr);
uint8_t Board_UARTGetChar(void);
void Board_UARTPutChar(uint8_t ch);

#ifdef DEBUG_UART
#define DEBUGINIT()   Board_Debug_Init()
#define DEBUGOUT(...) printf(__VA_ARGS__)
#define DEBUGSTR(str) Board_UARTPutSTR(str)
#define DEBUGIN()     Board_UARTGetChar()
#else
#define DEBUGINIT()
#define DEBUGOUT(...)
#define DEBUGSTR(str)
#define DEBUGIN()
#endif

#ifdef __cplusplus
}
#endif

#endif // __RSI_DEBUG_H__