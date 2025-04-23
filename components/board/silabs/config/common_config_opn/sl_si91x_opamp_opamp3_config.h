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

// OPAMP3 P
// <o SL_OPAMP3_P_PIN> OPAMP3 P input Selection
//   <10=> ULP_GPIO_10
// <i> Selection of OPAMP3 P
#define SL_OPAMP3_P_PIN 10
#ifndef SL_OPAMP3_P_PORT
#define SL_OPAMP3_P_PORT ULP
#endif

#if SL_OPAMP3_P_PIN == 10
#define SL_OPAMP3_VIN_SEL 0
#elif SL_OPAMP3_P_PIN == 29
#define SL_OPAMP3_VIN_SEL 1
#else
#warning "OPAMP3 P pin is not selected. Please select a valid pin."
#endif

// OPAMP3 N
// <o SL_OPAMP3_N_PIN> OPAMP3 N input Selection
//   <1=> None
//   <10=> N0 ULP_GPIO_10
//   <29=> N1 GPIO_29
// <i> Selection of OPAMP3 N
#define SL_OPAMP3_N_PIN 1
#ifndef SL_OPAMP3_N_PORT
#if (SL_OPAMP3_N_PIN == 29)
#define SL_OPAMP3_N_PORT HP
#else
#define SL_OPAMP3_N_PORT ULP
#endif
#endif

#if (SL_OPAMP3_N_PIN == 10) || (SL_OPAMP3_N_PIN == 1)
#define SL_OPAMP3_VREF_SEL 0
#elif SL_OPAMP3_N_PIN == 29
#define SL_OPAMP3_VREF_SEL 1
#else
#warning "OPAMP3 N pin is not selected. Please select a valid pin."
#endif

// OPAMP3 OUT (Output)
// <o SL_OPAMP3_OUT_PIN> OPAMP3 OUT Selection
//   <27=> GPIO_27
// <i> Selection of OPAMP3 OUT
#define SL_OPAMP3_OUT_PIN 27
#ifndef SL_OPAMP3_OUT_PORT
#define SL_OPAMP3_OUT_PORT HP
#endif
// </h>OPAMP
// <<< end of configuration section >>>

#endif // SL_SI91X_OPAMP_OPAMP3_CONFIG_H
