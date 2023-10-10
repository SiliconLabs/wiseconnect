/*******************************************************************************
* @file  rsi_nvic_priorities_config.c
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

/*
 * rsi_nvic_priorities_config.c
 *
 *  Created on: Aug 1, 2023
 *      Author: surondla
 */

#include "rsi_nvic_priorities_config.h"

#ifdef RSI_WITH_OS
void sl_si91x_device_init_nvic()
{
  for (IRQn_Type i = TIMER0_IRQn; i < I2C1_IRQn; i++) {
    NVIC_SetPriority(i, DEFAULT_PRIORITY);
  }
#ifdef CHIP_917B0
  NVIC_SetPriority(MVP_IRQn, DEFAULT_PRIORITY);
  NVIC_SetPriority(MVP_WAKEUP_IRQn, DEFAULT_PRIORITY);
#endif
  NVIC_SetPriority(I2S0_IRQn, DEFAULT_PRIORITY);
  NVIC_SetPriority(PLL_CLOCK_IRQn, DEFAULT_PRIORITY);
  NVIC_SetPriority(TASS_P2P_IRQn, DEFAULT_PRIORITY);
}
#endif
