/***************************************************************************/ /**
* @file sl_si91x_opamp_opamp1_config.h
* @brief OPAMP opamp1 configuration file.
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

#ifndef SL_SI91X_OPAMP_OPAMP1_CONFIG_H
#define SL_SI91X_OPAMP_OPAMP1_CONFIG_H

#include "sl_si91x_opamp.h"
/******************** OPAMP Configuration ******************************/

#define SL_OPAMP_OPAMP1 SL_OPAMP_1

// <<< Use Configuration Wizard in Context Menu >>>
// <h>OPAMP1 Input/Output Configuration Selection

// OPAMP1 VinP
// <o SL_OPAMP1_P_PIN> VinP Mux Selection
//   <27=> P0 GPIO_27
//   <7=>  P1 ULP_GPIO_7
//   <0=>  P2 ULP_GPIO_0
//   <2=>  P3 ULP_GPIO_2
//   <6=>  P4 ULP_GPIO_6
//   <8=>  P5 ULP_GPIO_8
//   <1=>  P6 DAC
//   <3=>  P7 Resistor_tap
// <i> Selection of OPAMP1 VinP
#define SL_OPAMP1_P_PIN 6
#ifndef SL_OPAMP1_P_PORT
#if (SL_OPAMP1_P_PIN == 27)
#define SL_OPAMP1_P_PORT HP
#else
#define SL_OPAMP1_P_PORT ULP
#endif
#endif

#if SL_OPAMP1_P_PIN == 27
#define SL_OPAMP1_VINP_SEL 0
#elif SL_OPAMP1_P_PIN == 7
#define SL_OPAMP1_VINP_SEL 1
#elif SL_OPAMP1_P_PIN == 0
#define SL_OPAMP1_VINP_SEL 2
#elif SL_OPAMP1_P_PIN == 2
#define SL_OPAMP1_VINP_SEL 3
#elif SL_OPAMP1_P_PIN == 6
#define SL_OPAMP1_VINP_SEL 4
#elif SL_OPAMP1_P_PIN == 8
#define SL_OPAMP1_VINP_SEL 5
#elif SL_OPAMP1_P_PIN == 1
#define SL_OPAMP1_VINP_SEL 6
#elif SL_OPAMP1_P_PIN == 3
#define SL_OPAMP1_VINP_SEL 7
#else
#warning "OPAMP1 VinP input pin is not selected. Please select a valid pin."
#endif

// OPAMP1 VinN
// <o SL_OPAMP1_N_PIN> VinN Mux Selection
//   <27=> N0 GPIO_27
//   <7=>  N1 ULP_GPIO_7
//   <1=>  N2 DAC
//   <3=>  N3 Resistor_tap
//   <4=>  N4 Out
// <i> Selection of OPAMP1 VinN
#define SL_OPAMP1_N_PIN 4
#ifndef SL_OPAMP1_N_PORT
#if (SL_OPAMP1_N_PIN == 27)
#define SL_OPAMP1_N_PORT HP
#else
#define SL_OPAMP1_N_PORT ULP
#endif
#endif

#if SL_OPAMP1_N_PIN == 27
#define SL_OPAMP1_VINN_SEL 0
#elif SL_OPAMP1_N_PIN == 7
#define SL_OPAMP1_VINN_SEL 1
#elif SL_OPAMP1_N_PIN == 1
#define SL_OPAMP1_VINN_SEL 2
#elif SL_OPAMP1_N_PIN == 3
#define SL_OPAMP1_VINN_SEL 3
#elif SL_OPAMP1_N_PIN == 4
#define SL_OPAMP1_VINN_SEL 4
#else
#warning "OPAMP1 VinN input pin is not selected. Please select a valid pin."
#endif

// OPAMP1 Resistor
// <o SL_OPAMP1_RES_PIN> Resistor Mux Selection
//   <10=> None
//   <27=> Res0 GPIO_27
//   <7=>  Res1 ULP_GPIO_7
//   <0=>  Res2 ULP_GPIO_0
//   <2=>  Res3 ULP_GPIO_2
//   <6=>  Res4 ULP_GPIO_6
//   <8=>  Res5 ULP_GPIO_8
//   <1=>  Res6 DAC
// <i> Selection of OPAMP1 Resistor
#define SL_OPAMP1_RES_PIN 10
#ifndef SL_OPAMP1_RES_PORT
#if (SL_OPAMP1_RES_PIN == 27)
#define SL_OPAMP1_RES_PORT HP
#else
#define SL_OPAMP1_RES_PORT ULP
#endif
#endif

#if SL_OPAMP1_RES_PIN == 27
#define SL_OPAMP1_VREF_SEL 0
#elif SL_OPAMP1_RES_PIN == 7
#define SL_OPAMP1_VREF_SEL 1
#elif SL_OPAMP1_RES_PIN == 0
#define SL_OPAMP1_VREF_SEL 2
#elif SL_OPAMP1_RES_PIN == 2
#define SL_OPAMP1_VREF_SEL 3
#elif SL_OPAMP1_RES_PIN == 6
#define SL_OPAMP1_VREF_SEL 4
#elif SL_OPAMP1_RES_PIN == 8
#define SL_OPAMP1_VREF_SEL 5
#elif SL_OPAMP1_RES_PIN == 1
#define SL_OPAMP1_VREF_SEL 6
#elif SL_OPAMP1_RES_PIN == 10
#define SL_OPAMP1_VREF_SEL 7
#else
#warning "OPAMP1 Resistor input pin is not selected. Please select a valid pin."
#endif

#if (SL_OPAMP1_P_PIN == SL_OPAMP1_N_PIN) || (SL_OPAMP1_P_PIN == SL_OPAMP1_RES_PIN)
#warning "OPAMP1 both input pins should not be same"
#endif
// OPAMP1 OUT (Output)
// <o SL_OPAMP1_OUT_PIN> OUT Selection
//   <30=> Out0 GPIO_30
//   <4=>  Out1 ULP_GPIO_4
// <i> Selection of OPAMP1 OUT
#define SL_OPAMP1_OUT_PIN 30
#ifndef SL_OPAMP1_OUT_PORT
#if (SL_OPAMP1_OUT_PIN == 30)
#define SL_OPAMP1_OUT_PORT HP
#else
#define SL_OPAMP1_OUT_PORT ULP
#endif
#endif

// OPAMP1 R1 Resistor
// <o SL_OPAMP1_R1_SELECT> R1 Resistor
//   <0=> None
//   <1=> 1_20Kohm
//   <2=> 2_60Kohm
//   <3=> 3_140Kohm
// <i> Selection of OPAMP1 R1
#define SL_OPAMP1_R1_SELECT 0

// OPAMP1 R2 Resistor
// <o SL_OPAMP1_R2_SELECT> R2 Resistor
//   <8=> None
//   <0=> 0_20Kohm
//   <1=> 1_30Kohm
//   <2=> 2_40Kohm
//   <3=> 3_60Kohm
//   <4=> 4_120Kohm
//   <5=> 5_250Kohm
//   <6=> 6_500Kohm
//   <7=> 7_1000Kohm
// <i> Selection of OPAMP1 R2
#define SL_OPAMP1_R2_SELECT 8

// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP1_CONFIG_H
