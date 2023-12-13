/*******************************************************************************
* @file  clock_update.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// Includes Files

#include "base_types.h"

#ifndef RSI_CLOCK_UPDATE_H
#define RSI_CLOCK_UPDATE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PERI_CLKS {
  M4_USART0,  // Enables or Disables USART1 Peripheral clock when it is passed
  M4_UART1,   // Enables or Disables USART2 Master Peripheral clock when it is passed
  M4_SSI_MST, // Enables or Disables SSI Master Peripheral clock when it is passed
  M4_CT,      // Enables or Disables CT Peripheral clock when it is passed
#ifndef SLI_SI917
  M4_SD_MEM, // Enables or Disables SD_MEM Peripheral clock when it is passed
  M4_CCI,    // Enables or Disables CCI Peripheral clock when it is passed
#endif
  M4_QSPI, // Enables or Disables QSPI Peripheral clock when it is passed
  M4_QSPI2,
  M4_GSPI,     // Enables or Disables GSPI Peripheral clock when it is passed
  M4_ETHERNET, // Enables or Disables ETHERNET Peripheral clock when it is passed
  M4_I2SM,     // Enables or Disables I2SM Peripheral clock when it is passed
  ULPSS_SSI,
  ULPSS_I2S,
  ULPSS_UART,
  ULPSS_TIMER,
  ULPSS_AUX,
  //ULPSS_VAD,
  ULPSS_TOUCH,
} PERI_CLKS_T;

uint32_t RSI_CLK_GetBaseClock(PERI_CLKS_T peri_src);
uint32_t GetTickCount(void);
void SysTick_Handler(void);

#ifdef __SYSTICK
void rsi_delay_ms(uint32_t val);
#endif

#ifdef __cplusplus
}
#endif

#endif // RSI_CLOCK_UPDATE_H
