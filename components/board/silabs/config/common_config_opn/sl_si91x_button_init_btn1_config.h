/***************************************************************************/ /**
 * @file sl_si91x_button_init_btn1_config.h
 * @brief Button Driver Configuration
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
#ifndef SL_SI91X_BUTTON_INIT_BTN1_CONFIG_H
#define SL_SI91X_BUTTON_INIT_BTN1_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_BUTTON_CONFIG_INSTANCE_INTR> Button Interrupt Configuration
// <HIGH_LEVEL_INTERRUPT => High level interrupt
// <LOW_LEVEL_INTERRUPT => Low level interrupt
// <HIGH_LEVEL_AND_LOW_LEVEL_INTERRUPT => High level and low level interrupt
// <RISE_EDGE_INTERRUPT => Rise edge interrupt
// <FALL_EDGE_INTERRUPT => Fall edge interrupt
// <RISE_EDGE_AND_FALL_EDGE_INTERRUPT => Rise edge and fall edge interrupt
// <i> Default: RISE_EDGE_AND_FALL_EDGE_INTERRUPT
#define SL_BUTTON_CONFIG_INSTANCE_INTR RISE_EDGE_AND_FALL_EDGE_INTERRUPT

// <<< end of configuration section >>>
#if USER_CONFIGURATION_ENABLE
// <<< sl:start pin_tool >>>
// <gpio> SL_SI91X_BUTTON_1
// $[GPIO_SL_SI91X_BUTTON_1]
#warning "Button is not configured. Please configure the Button pins according to the board connections."
#ifndef SL_SI91X_BUTTON_1_PORT
#define SL_SI91X_BUTTON_1_PORT HP
#endif
#ifndef SL_SI91X_BUTTON_1_PIN
#define SL_SI91X_BUTTON_1_PIN 11
#endif
// [GPIO_SL_SI91X_BUTTON_1]$
// <<< sl:end pin_tool >>>

#define SL_BUTTON_BTN1_PIN    SL_SI91X_BUTTON_1_PIN
#define SL_BUTTON_BTN1_PORT   SL_SI91X_BUTTON_1_PORT
#define SL_BUTTON_BTN1_NUMBER RTE_BUTTON1_NUMBER
#define SL_BUTTON_BTN1_PAD    RTE_BUTTON1_PAD
// This else part is only to resolve build errors in macro define. When USER_CONFIGURATION_ENABLE is enabled else part is neglected
#else
#define SL_SI91X_BUTTON_1_PORT HP
#define SL_BUTTON_BTN1_PIN     11
#define SL_BUTTON_BTN1_PORT    RTE_BUTTON1_PORT
#define SL_BUTTON_BTN1_NUMBER  RTE_BUTTON1_NUMBER
#define SL_BUTTON_BTN1_PAD     RTE_BUTTON1_PAD
#endif
#endif // SL_SI91X_BUTTON_INIT_BTN1_CONFIG_H
