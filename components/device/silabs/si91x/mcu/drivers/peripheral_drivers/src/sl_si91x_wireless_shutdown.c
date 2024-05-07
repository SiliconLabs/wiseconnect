/***************************************************************************/ /**
* @file sl_si91x_wireless_shutdown.c
* @brief  wireless shutdown
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licenser of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
#include "sl_si91x_wireless_shutdown.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"

// -----------------------------------------------------------------------------
// Prototypes
/**
 * @fn         sl_si91x_wireless_shutdown
 * @brief      Configure power management settings for SI91X device
 * @param[in]  none
 * @return    none.
 * @section description
 * This API disables the ULP reference clocks, changes the TASS
 * Reference, and shuts down the power supply.
 */
void sl_si91x_wireless_shutdown(void)
{
  RSI_ULPSS_DisableRefClks(MCU_ULP_40MHZ_CLK_EN); /* Disabling 40MHz Clocks */
  // This API is used to change the TASS reference clock to 32MHz
  RSI_ChangeTassRefClock();
  // This API is used to shutdown the power supply to wireless functionalities
  ps_wireless_shutdown();
}