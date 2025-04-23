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

// OPAMP2 P
// <o SL_OPAMP2_P_PIN> OPAMP2 P input Selection
//   <11=> P0 ULP_GPIO_11
//   <5=> P1 ULP_GPIO_5
// <i> Selection of OPAMP2 P
#define SL_OPAMP2_P_PIN 5
#ifndef SL_OPAMP2_P_PORT
#define SL_OPAMP2_P_PORT ULP
#endif

#if SL_OPAMP2_P_PIN == 11
#define SL_OPAMP2_VIN_SEL 0
#elif SL_OPAMP2_P_PIN == 5
#define SL_OPAMP2_VIN_SEL 1
#else
#warning "OPAMP2 P pin is not selected. Please select a valid pin."
#endif

// OPAMP2 N
// <o SL_OPAMP2_N_PIN> OPAMP2 N input Selection
//   <1=> None
//   <11=> N0 ULP_GPIO_11
//   <5=> N1 ULP_GPIO_5
// <i> Selection of OPAMP2 N
#define SL_OPAMP2_N_PIN 1
#ifndef SL_OPAMP2_N_PORT
#define SL_OPAMP2_N_PORT ULP
#endif

#if (SL_OPAMP2_N_PIN == 11) || (SL_OPAMP2_N_PIN == 1)
#define SL_OPAMP2_VREF_SEL 0
#elif SL_OPAMP2_N_PIN == 5
#define SL_OPAMP2_VREF_SEL 1
#else
#warning "OPAMP2 N pin is not selected. Please select a valid pin."
#endif

// OPAMP2 OUT (Output)
// <o SL_OPAMP2_OUT_PIN> OPAMP2 OUT Selection
//   <9=> ULP_GPIO_9
// <i> Selection of OPAMP2 OUT
#define SL_OPAMP2_OUT_PIN 9
#ifndef SL_OPAMP2_OUT_PORT
#define SL_OPAMP2_OUT_PORT ULP
#endif
// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP2_CONFIG_H
