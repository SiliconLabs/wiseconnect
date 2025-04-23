/***************************************************************************/ /**
* @file sl_si91x_opamp_inst_config.h
* @brief OPAMP configuration file.
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

#ifndef SL_SI91X_OPAMP_INST_CONFIG_H
#define SL_SI91X_OPAMP_INST_CONFIG_H

#include "sl_si91x_opamp.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************** OPAMP Configuration ******************************/

// <<< Use Configuration Wizard in Context Menu >>>
// <h>OPAMP Configuration

#define SLI_OPAMP1 1
#define SLI_OPAMP2 2
#define SLI_OPAMP3 3

#define SLI_OPAMP_DEFAULT SLI_INSTANCE

#define SL_OPAMP_P_SEL_PIN   SL_INSTANCE_P_PIN
#define SL_OPAMP_N_SEL_PIN   SL_INSTANCE_N_PIN
#define SL_OPAMP_OUT_SEL_PIN SL_INSTANCE_OUT_PIN

// </h>
// <<< end of configuration section >>>

#ifdef __cplusplus
}
#endif // SL_OPAMP
#endif // SL_OPAMP_INST_CONFIG_H
