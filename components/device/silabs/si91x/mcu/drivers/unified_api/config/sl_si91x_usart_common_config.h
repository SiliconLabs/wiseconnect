/***************************************************************************/ /**
 * @file sl_si91x_usart_common_config.h
 * @brief SL SI91X USART Common Config.
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

#ifndef SL_SI91X_USART_COMMON_CONFIG_H
#define SL_SI91X_USART_COMMON_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>USART0 Configuration

// <q SL_USART0_DMA_CONFIG_ENABLE> USART0 DMA
// <i> Default: 1
#define SL_USART0_DMA_CONFIG_ENABLE 1

// <q SL_USART0_SYNC_MODE_EN> USART0 SYNC Mode
// <i> Default: 0
#define SL_USART0_SYNC_MODE_EN 0

// <o SL_USART0_CLOCK_SRC> USART0 Clock Source
// <USART_ULPREFCLK=> ULP REF CLK
// <USART_SOCPLLCLK=> SOC PLL CLK
// <USART_INTFPLLCLK=> INTF PLL CLK
// <i> Default: USART_ULPREFCLK
#define SL_USART0_CLOCK_SRC USART_ULPREFCLK

// <o SL_USART0_FRAC_DIV_SEL> USART0 SCLK fractional divider mode
// <USART_FRACTIONAL_DIVIDER=> Fractional (clk_in / (div + 0.5))
// <USART_CLOCK_SWALLOW=> Clock swallow (clk_in / div)
// <i> See USART_SCLK_FRAC_SEL_T in rsi_pll.h
// <i> Default: USART_FRACTIONAL_DIVIDER (matches typical RTE_Device_917.h; use USART_CLOCK_SWALLOW in UC when needed)
#define SL_USART0_FRAC_DIV_SEL USART_FRACTIONAL_DIVIDER

// </h>
// <<< end of configuration section >>>

/* RTE_USART0_FRAC_DIV_SEL follows UC (SL_USART0_FRAC_DIV_SEL); RTE_Device value is not used after this header. */
#ifdef RTE_USART0_FRAC_DIV_SEL
#undef RTE_USART0_FRAC_DIV_SEL
#endif
#define RTE_USART0_FRAC_DIV_SEL SL_USART0_FRAC_DIV_SEL

// <<< sl:start pin_tool >>>
// <usart0 signal=(CLK),TX,RX,(CTS),(RTS)> SL_USART0
// $[USART0_SL_USART0]
#ifndef SL_USART0_PERIPHERAL
#define SL_USART0_PERIPHERAL USART0
#endif

// USART0 TX on GPIO_30
#ifndef SL_USART0_TX_PORT
#define SL_USART0_TX_PORT HP
#endif
#ifndef SL_USART0_TX_PIN
#define SL_USART0_TX_PIN 30
#endif
#ifndef SL_USART0_TX_LOC
#define SL_USART0_TX_LOC 5
#endif

// USART0 RX on GPIO_29
#ifndef SL_USART0_RX_PORT
#define SL_USART0_RX_PORT HP
#endif
#ifndef SL_USART0_RX_PIN
#define SL_USART0_RX_PIN 29
#endif
#ifndef SL_USART0_RX_LOC
#define SL_USART0_RX_LOC 10
#endif

// [USART0_SL_USART0]$
// <<< sl:end pin_tool >>>
#endif // SL_SI91X_USART_COMMON_CONFIG_H
