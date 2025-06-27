/***************************************************************************/ /**
 * @file sl_si91x_led_init_led0_config.h
 * @brief Led Driver Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
 *****************************************************************************/

#ifndef SL_SI91X_LED_INIT_LED0_CONFIG_H
#define SL_SI91X_LED_INIT_LED0_CONFIG_H
#if USER_CONFIGURATION_ENABLE
// <<< sl:start pin_tool >>>
// <gpio> SL_SI91X_LED_0
// $[GPIO_SL_SI91X_LED_0]
#warning "LED is not configured. Please configure the LED pins according to the board connections."
#ifndef SL_SI91X_LED_0_PORT
#define SL_SI91X_LED_0_PORT ULP
#endif
#ifndef SL_SI91X_LED_0_PIN
#define SL_SI91X_LED_0_PIN 2
#endif
// [GPIO_SL_SI91X_LED_0]$
// <<< sl:end pin_tool >>>

#define SL_LED_LED0_PIN    SL_SI91X_LED_0_PIN
#define SL_LED_LED0_PORT   RTE_LED0_PORT
#define SL_LED_LED0_NUMBER RTE_LED0_NUMBER
// This else part is only to resolve build errors in macro define. When USER_CONFIGURATION_ENABLE is enabled else part is neglected
#else
#define SL_SI91X_LED_0_PORT ULP
#define SL_LED_LED0_PIN     2
#define SL_LED_LED0_PORT    RTE_LED0_PORT
#define SL_LED_LED0_NUMBER  RTE_LED0_NUMBER
#endif
#endif // SL_SI91X_LED_INIT_LED0_CONFIG_H
