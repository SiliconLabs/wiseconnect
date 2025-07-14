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

// <<< sl:start pin_tool >>>
// <opamp3 signal=(P),(N),(RES),(OUT)> SL_OPAMP3
// $[OPAMP3_SL_OPAMP3]
#ifndef SL_OPAMP3_PERIPHERAL
#define SL_OPAMP3_PERIPHERAL OPAMP3
#endif
#ifndef SL_OPAMP3_PERIPHERAL_NO
#define SL_OPAMP3_PERIPHERAL_NO 3
#endif

// [OPAMP3_SL_OPAMP3]$
// <<< sl:end pin_tool >>>

// <<< Use Configuration Wizard in Context Menu >>>
// <h>OPAMP3 Input/Output Configuration Selection

// OPAMP3 VinP
// <o SL_OPAMP3_VINP_SEL> VinP Mux Selection
//   <0=>  P0 ULP_GPIO_10
//   <1=>  P1 GPIO_29
//   <2=>  P2 DAC
//   <3=>  P3 Resistor_tap
//   <5=>  P5 OPAMP2_Out
//   <6=>  P6 OPAMP2_Resistor_tap
// <i> Selection of OPAMP3 VinP
#define SL_OPAMP3_VINP_SEL 0

// OPAMP3 VinN
// <o SL_OPAMP3_VINN_SEL> VinN Mux Selection
//   <0=>  N0 ULP_GPIO_10
//   <1=>  N1 DAC
//   <2=>  N2 Resistor_tap
//   <3=>  N3 Out
// <i> Selection of OPAMP3 VinN
#define SL_OPAMP3_VINN_SEL 3

// OPAMP3 N
// <o SL_OPAMP3_VREF_SEL> Resistor Mux Selection
//   <10=> None
//   <0=>  Res0 ULP_GPIO_10
//   <1=>  Res1 GPIO_29
//   <2=>  Res2 DAC
//   <4=>  Res4 OPAMP2_OUT
// <i> Selection of OPAMP3 N
#define SL_OPAMP3_VREF_SEL 10

// <o SL_OPAMP_LP_MODE> OPAMP Power mode
//   <0=> HP mode
//   <1=> LP mode
// <i> Selection of OPAMP Power Mode
#define SL_OPAMP_LP_MODE 0

// OPAMP3 R1 Resistor
// <o SL_OPAMP3_R1_SELECT> R1 Resistor (kΩ)
//   <0=> None
//   <1=> 20
//   <2=> 60
//   <3=> 140
// <i> Selection of OPAMP3 R1
#define SL_OPAMP3_R1_SELECT 0

// OPAMP3 R2 Resistor
// <o SL_OPAMP3_R2_SELECT> R2 Resistor (kΩ)
//   <8=> None
//   <0=> 20
//   <1=> 30
//   <2=> 40
//   <3=> 60
//   <4=> 120
//   <5=> 250
//   <6=> 500
//   <7=> 1000
// <i> Selection of OPAMP3 R2
#define SL_OPAMP3_R2_SELECT 8

// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP3_CONFIG_H