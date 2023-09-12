/***************************************************************************/ /**
 * @file sl_si91x_hal_soft_reset.c
 * @brief SoC soft reset API
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "rsi_wwdt.h"

/*
 *
 * @brief  WDT interrupt handler
 * @param  None
 * @return None
 */
void WDT_IRQHandler(void)
{
  /*Clear interrupt */
  RSI_WWDT_IntrClear();
}

/*
 *
 * @brief  This API is used to config WDT
 * @param  None
 * @return None
 */
void sl_si91x_soc_soft_reset(void)
{
  RSI_PS_NpssPeriPowerUp(SLPSS_PWRGATE_ULP_MCUWDT);
  osDelay(2);
  /*Un mask WDT interrupt */
  RSI_WWDT_IntrUnMask();
  /*configure the WDT reset and interrupt counters */
  RSI_WWDT_ConfigIntrTimer(MCU_WDT, WDT_INTERRUPT_TIMER);
  /*Configure the WDT reset value*/
  RSI_WWDT_ConfigSysRstTimer(MCU_WDT, WDT_SYSTEM_RESET_TIMER);
  /*NVIC interrupt enables*/
  NVIC_EnableIRQ(NVIC_WDT);
  /*Start WDT */
  RSI_WWDT_Start(MCU_WDT);
  while (1)
    ;
}
