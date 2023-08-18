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
