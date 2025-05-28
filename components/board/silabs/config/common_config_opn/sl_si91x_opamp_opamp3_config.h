/***************************************************************************/ /**
* @file sl_si91x_opamp_opamp3_config.h
* @brief OPAMP opamp3 configuration file.
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

#ifndef SL_SI91X_OPAMP_OPAMP3_CONFIG_H
#define SL_SI91X_OPAMP_OPAMP3_CONFIG_H

#include "sl_si91x_opamp.h"
/******************** OPAMP Configuration ******************************/

#define SL_OPAMP_OPAMP3 SL_OPAMP_3

// <<< Use Configuration Wizard in Context Menu >>>
// <h>OPAMP3 Input/Output Configuration Selection

// OPAMP3 VinP
// <o SL_OPAMP3_P_PIN> VinP Mux Selection
//   <10=> P0 ULP_GPIO_10
//   <29=> P1 GPIO_29
//   <1=>  P2 DAC
//   <3=>  P3 Resistor_tap
//   <9=>  P5 OPAMP2_Out
//   <6=>  P6 OPAMP2_Resistor_tap
// <i> Selection of OPAMP3 VinP
#define SL_OPAMP3_P_PIN 10
#ifndef SL_OPAMP3_P_PORT
#if SL_OPAMP3_P_PIN == 29
#define SL_OPAMP3_P_PORT HP
#else
#define SL_OPAMP3_P_PORT ULP
#endif
#endif

#if SL_OPAMP3_P_PIN == 10
#define SL_OPAMP3_VINP_SEL 0
#elif SL_OPAMP3_P_PIN == 29
#define SL_OPAMP3_VINP_SEL 1
#elif SL_OPAMP3_P_PIN == 1
#define SL_OPAMP3_VINP_SEL 2
#elif SL_OPAMP3_P_PIN == 3
#define SL_OPAMP3_VINP_SEL 3
#elif SL_OPAMP3_P_PIN == 9
#define SL_OPAMP3_VINP_SEL 5
#elif SL_OPAMP3_P_PIN == 6
#define SL_OPAMP3_VINP_SEL 6
#else
#warning "OPAMP3 VinP input pin is not selected. Please select a valid pin."
#endif

// OPAMP3 VinN
// <o SL_OPAMP3_N_PIN> VinN Mux Selection
//   <10=> N0 ULP_GPIO_10
//   <1=>  N1 DAC
//   <2=>  N2 Resistor_tap
//   <3=>  N3 Out
// <i> Selection of OPAMP3 VinN
#define SL_OPAMP3_N_PIN 3
#ifndef SL_OPAMP3_N_PORT
#define SL_OPAMP3_N_PORT ULP
#endif

#if SL_OPAMP3_N_PIN == 10
#define SL_OPAMP3_VINN_SEL 0
#elif SL_OPAMP3_N_PIN == 1
#define SL_OPAMP3_VINN_SEL 1
#elif SL_OPAMP3_N_PIN == 2
#define SL_OPAMP3_VINN_SEL 2
#elif SL_OPAMP3_N_PIN == 3
#define SL_OPAMP3_VINN_SEL 3
#else
#warning "OPAMP3 VinN input pin is not selected. Please select a valid pin."
#endif

// OPAMP3 N
// <o SL_OPAMP3_RES_PIN> Resistor Mux Selection
//   <0=>  None
//   <10=> Res0 ULP_GPIO_10
//   <29=> Res1 GPIO_29
//   <1=>  Res2 DAC
//   <9=>  Res4 OPAMP2_OUT
// <i> Selection of OPAMP3 N
#define SL_OPAMP3_RES_PIN 0
#ifndef SL_OPAMP3_RES_PORT
#if (SL_OPAMP3_RES_PIN == 29)
#define SL_OPAMP3_RES_PORT HP
#else
#define SL_OPAMP3_RES_PORT ULP
#endif
#endif

#if SL_OPAMP3_RES_PIN == 10
#define SL_OPAMP3_VREF_SEL 0
#elif SL_OPAMP3_RES_PIN == 29
#define SL_OPAMP3_VREF_SEL 1
#elif SL_OPAMP3_RES_PIN == 1
#define SL_OPAMP3_VREF_SEL 2
#elif SL_OPAMP3_RES_PIN == 9
#define SL_OPAMP3_VREF_SEL 4
#elif SL_OPAMP3_RES_PIN == 0
#define SL_OPAMP3_VREF_SEL 5
#else
#warning "OPAMP3 Resistor input pin is not selected. Please select a valid pin."
#endif

#if (SL_OPAMP3_P_PIN == SL_OPAMP3_N_PIN) || (SL_OPAMP3_P_PIN == SL_OPAMP3_RES_PIN)
#warning "OPAMP3 both input pins should not be same"
#endif

// OPAMP3 OUT (Output)
// <o SL_OPAMP3_OUT_PIN> OUT Selection
//   <27=> GPIO_27
// <i> Selection of OPAMP3 OUT
#define SL_OPAMP3_OUT_PIN 27
#ifndef SL_OPAMP3_OUT_PORT
#define SL_OPAMP3_OUT_PORT HP
#endif

// OPAMP3 R1 Resistor
// <o SL_OPAMP3_R1_SELECT> R1 Resistor
//   <0=> None
//   <1=> 1_20Kohm
//   <2=> 2_60Kohm
//   <3=> 3_140Kohm
// <i> Selection of OPAMP3 R1
#define SL_OPAMP3_R1_SELECT 0

// OPAMP3 R2 Resistor
// <o SL_OPAMP3_R2_SELECT> R2 Resistor
//   <8=> None
//   <0=> 0_20Kohm
//   <1=> 1_30Kohm
//   <2=> 2_40Kohm
//   <3=> 3_60Kohm
//   <4=> 4_120Kohm
//   <5=> 5_250Kohm
//   <6=> 6_500Kohm
//   <7=> 7_1000Kohm
// <i> Selection of OPAMP3 R2
#define SL_OPAMP3_R2_SELECT 8

// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP3_CONFIG_H
