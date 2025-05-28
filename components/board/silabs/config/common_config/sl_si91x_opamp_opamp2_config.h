/***************************************************************************/ /**
* @file sl_si91x_opamp_opamp2_config.h
* @brief OPAMP opamp2 configuration file.
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_OPAMP_OPAMP2_CONFIG_H
#define SL_SI91X_OPAMP_OPAMP2_CONFIG_H

#include "sl_si91x_opamp.h"
/******************** OPAMP Configuration ******************************/

#define SL_OPAMP_OPAMP2 SL_OPAMP_2

// <<< Use Configuration Wizard in Context Menu >>>
// <h>OPAMP2 Input/Output Configuration Selection

// OPAMP2 VinP
// <o SL_OPAMP2_P_PIN> VinP Mux Selection
//   <11=> P0 ULP_GPIO_11
//   <5=>  P1 ULP_GPIO_5
//   <1=>  P3 DAC
//   <3=>  P4 Resistor_tap
//   <0=>  P6 OPAMP1_OUT
// <i> Selection of OPAMP2 VinP
#define SL_OPAMP2_P_PIN 11
#ifndef SL_OPAMP2_P_PORT
#define SL_OPAMP2_P_PORT ULP
#endif

#if SL_OPAMP2_P_PIN == 11
#define SL_OPAMP2_VINP_SEL 0
#elif SL_OPAMP2_P_PIN == 5
#define SL_OPAMP2_VINP_SEL 1
#elif SL_OPAMP2_P_PIN == 1
#define SL_OPAMP2_VINP_SEL 3
#elif SL_OPAMP2_P_PIN == 3
#define SL_OPAMP2_VINP_SEL 4
#elif SL_OPAMP2_P_PIN == 0
#define SL_OPAMP2_VINP_SEL 6
#else
#warning "OPAMP2 VinP input pin is not selected. Please select a valid pin."
#endif

// OPAMP2 VinN
// <o SL_OPAMP2_N_PIN> VinN Mux Selection
//   <11=> N0 ULP_GPIO_11
//   <1=>  N1 DAC
//   <2=>  N2 Resistor_tap
//   <4=>  N3 Out
// <i> Selection of OPAMP2 VinN
#define SL_OPAMP2_N_PIN 4
#ifndef SL_OPAMP2_N_PORT
#define SL_OPAMP2_N_PORT ULP
#endif

#if SL_OPAMP2_N_PIN == 11
#define SL_OPAMP2_VINN_SEL 0
#elif SL_OPAMP2_N_PIN == 1
#define SL_OPAMP2_VINN_SEL 1
#elif SL_OPAMP2_N_PIN == 2
#define SL_OPAMP2_VINN_SEL 2
#elif SL_OPAMP2_N_PIN == 4
#define SL_OPAMP2_VINN_SEL 3
#else
#warning "OPAMP2 VinN input pin is not selected. Please select a valid pin."
#endif

// OPAMP2 Resistor
// <o SL_OPAMP2_RES_PIN> Resistor Mux Selection
//   <10=> None
//   <11=> Res0 ULP_GPIO_11
//   <5=>  Res1 ULP_GPIO_5
//   <1=>  Res3 DAC
//   <30=> Res5 OPAMP1_OUT0
//   <4=>  Res5 OPAMP1_OUT1
// <i> Selection of OPAMP2 Resistor
#define SL_OPAMP2_RES_PIN 10
#ifndef SL_OPAMP2_RES_PORT
#if SL_OPAMP2_RES_PIN == 30
#define SL_OPAMP2_RES_PORT HP
#else
#define SL_OPAMP2_RES_PORT ULP
#endif
#endif

#if SL_OPAMP2_RES_PIN == 11
#define SL_OPAMP2_VREF_SEL 0
#elif SL_OPAMP2_RES_PIN == 5
#define SL_OPAMP2_VREF_SEL 1
#elif SL_OPAMP2_RES_PIN == 1
#define SL_OPAMP2_VREF_SEL 3
#elif SL_OPAMP2_RES_PIN == (30 || 4)
#define SL_OPAMP2_VREF_SEL 5
#elif SL_OPAMP2_RES_PIN == 10
#define SL_OPAMP2_VREF_SEL 6
#else
#warning "OPAMP2 Resistor input pin is not selected. Please select a valid pin."
#endif

#if (SL_OPAMP2_P_PIN == SL_OPAMP2_N_PIN) || (SL_OPAMP2_P_PIN == SL_OPAMP2_RES_PIN)
#warning "OPAMP2 both input pins should not be same"
#endif

// OPAMP2 OUT (Output)
// <o SL_OPAMP2_OUT_PIN> OUT Selection
//   <9=> ULP_GPIO_9
// <i> Selection of OPAMP2 OUT
#define SL_OPAMP2_OUT_PIN 9
#ifndef SL_OPAMP2_OUT_PORT
#define SL_OPAMP2_OUT_PORT ULP
#endif

// OPAMP2 R1 Resistor
// <o SL_OPAMP2_R1_SELECT> R1 Resistor
//   <0=> None
//   <1=> 1_20Kohm
//   <2=> 2_60Kohm
//   <3=> 3_140Kohm
// <i> Selection of OPAMP2 R1
#define SL_OPAMP2_R1_SELECT 0

// OPAMP2 R2 Resistor
// <o SL_OPAMP2_R2_SELECT> R2 Resistor
//   <8=> None
//   <0=> 0_20Kohm
//   <1=> 1_30Kohm
//   <2=> 2_40Kohm
//   <3=> 3_60Kohm
//   <4=> 4_120Kohm
//   <5=> 5_250Kohm
//   <6=> 6_500Kohm
//   <7=> 7_1000Kohm
// <i> Selection of OPAMP2 R2
#define SL_OPAMP2_R2_SELECT 8

// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP2_CONFIG_H
