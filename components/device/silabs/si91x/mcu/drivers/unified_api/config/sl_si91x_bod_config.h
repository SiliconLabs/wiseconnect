/***************************************************************************/
/**
 * @file sl_si91x_bod_config.h
 * @brief BOD configuration file.
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
#ifndef SL_SI91X_BOD_CONFIG_H
#define SL_SI91X_BOD_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

// <<< Use Configuration Wizard in Context Menu >>>

/******************************* BOD Configuration **************************/

//#if USER_CONFIGURATION_ENABLE

//  <e>Enable BOD UC configuration
//  <i> Enable: BOD configuration enable
//  <i> Disable: BOD configuration disable
//  <i> Enable: Peripheral configuration is taken straight from the
//  configuration set in the universal configuration (UC). <i> Disable: If the
//  application demands it to be modified during runtime, use the
//  sl_si91x_bod_set_configuration API to modify the peripheral configuration.
//  <i> Default: 0

#define SL_BOD_UC_CONFIG_EN 1

// <h> BOD UC Configuration
// <o SLOT_VALUE> Slot Value <2-65535>
// <i> Range: <2-65535>
// <i> Default: 10
#define SLOT_VALUE 10

// </h>

//  <e>Enable Black-Out Detection
//  <i> Enable: BOD configuration enable
//  <i> Disable: BOD configuration disable
//  <i> Default: 0
#define BLACK_OUT_EN 0
// </e>
//  </e>
// <<< end of configuration section >>>

sl_bod_uc_param_t uc_config_params = { .slot_value = SLOT_VALUE, .blackout_en = BLACK_OUT_EN };
//#endif // USER_CONFIGURATION_ENABLE

#ifdef __cplusplus
}
#endif // SL_BOD

#endif /* SL_SI91X_BOD_COMMON_CONFIG_H */
