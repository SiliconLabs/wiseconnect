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

// OPAMP1 P
// <o SL_OPAMP1_P_PIN> OPAMP1 P input Selection
//   <7=> P1 ULP_GPIO_7
//   <0=> P2 ULP_GPIO_0
//   <6=> P4 ULP_GPIO_6
//   <8=> P5 ULP_GPIO_8
// <i> Selection of OPAMP1 P
#define SL_OPAMP1_P_PIN 7
#ifndef SL_OPAMP1_P_PORT
#if (SL_OPAMP1_P_PIN == 27)
#define SL_OPAMP1_P_PORT HP
#else
#define SL_OPAMP1_P_PORT ULP
#endif
#endif

#if SL_OPAMP1_P_PIN == 27
#define SL_OPAMP1_VIN_SEL 0
#elif SL_OPAMP1_P_PIN == 7
#define SL_OPAMP1_VIN_SEL 1
#elif SL_OPAMP1_P_PIN == 0
#define SL_OPAMP1_VIN_SEL 2
#elif SL_OPAMP1_P_PIN == 2
#define SL_OPAMP1_VIN_SEL 3
#elif SL_OPAMP1_P_PIN == 6
#define SL_OPAMP1_VIN_SEL 4
#elif SL_OPAMP1_P_PIN == 8
#define SL_OPAMP1_VIN_SEL 5
#else
#warning "OPAMP1 P pin is not selected. Please select a valid pin."
#endif

// OPAMP1 N
// <o SL_OPAMP1_N_PIN> OPAMP1 N input Selection
//   <1=> None
//   <7=> N1 ULP_GPIO_7
//   <0=> N2 ULP_GPIO_0
//   <6=> N4 ULP_GPIO_6
//   <8=> N5 ULP_GPIO_8
// <i> Selection of OPAMP1 N
#define SL_OPAMP1_N_PIN 1
#ifndef SL_OPAMP1_N_PORT
#if (SL_OPAMP1_N_PIN == 27)
#define SL_OPAMP1_N_PORT HP
#else
#define SL_OPAMP1_N_PORT ULP
#endif
#endif

#if (SL_OPAMP1_N_PIN == 27) || (SL_OPAMP1_N_PIN == 1)
#define SL_OPAMP1_VREF_SEL 0
#elif SL_OPAMP1_N_PIN == 7
#define SL_OPAMP1_VREF_SEL 1
#elif SL_OPAMP1_N_PIN == 0
#define SL_OPAMP1_VREF_SEL 2
#elif SL_OPAMP1_N_PIN == 2
#define SL_OPAMP1_VREF_SEL 3
#elif SL_OPAMP1_N_PIN == 6
#define SL_OPAMP1_VREF_SEL 4
#elif SL_OPAMP1_N_PIN == 8
#define SL_OPAMP1_VREF_SEL 5
#else
#warning "OPAMP1 N pin is not selected. Please select a valid pin."
#endif

// OPAMP1 OUT (Output)
// <o SL_OPAMP1_OUT_PIN> OPAMP1 OUT Selection
//   <30=> GPIO_30
//   <4=> ULP_GPIO_4
// <i> Selection of OPAMP1 OUT
#define SL_OPAMP1_OUT_PIN 30
#ifndef SL_OPAMP1_OUT_PORT
#if (SL_OPAMP1_OUT_PIN == 30)
#define SL_OPAMP1_OUT_PORT HP
#else
#define SL_OPAMP1_OUT_PORT ULP
#endif
#endif

// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP1_CONFIG_H
