/*******************************************************************************
* @file  rsi_board.h
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

#ifndef __RSI_BOARD_H__
#define __RSI_BOARD_H__
/**
 * @defgroup  Board_Specific_Driver   Board specific drivers and support functions
 * @{
 */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @defgroup  Board_Drivers BOARD: Common board components used with board drivers
 * @{
 */
#include "rsi_ccp_user_config.h"
#include "stdint.h"
#include "stdio.h"
#define DEBUG_ENABLE

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

void RSI_Board_LED_Set(int x, int y);
void RSI_Board_Init(void);
void RSI_Board_LED_Toggle(int);

#else
#define DEBUGINIT()
#define DEBUGOUT(...)
#define DEBUGSTR(str)
#define DEBUGIN()
#endif

#define DEBUG_ENABLE

#ifdef __cplusplus
}
#endif

#endif /*__RSI_BOARD_H__*/

/* @} end of Board_Drivers */
/* @} end of Board_Specific_Driver */
