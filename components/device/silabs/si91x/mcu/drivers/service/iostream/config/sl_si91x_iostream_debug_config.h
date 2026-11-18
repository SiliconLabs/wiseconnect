/***************************************************************************/ /**
 * @file
 * @brief IO Stream Debug configuration for SiWx91x.
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_SI91X_IOSTREAM_DEBUG_CONFIG_H
#define SL_SI91X_IOSTREAM_DEBUG_CONFIG_H

#include "sl_si91x_iostream_swo_itm_8.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h>IO Stream Debug settings

// <o SL_SI91X_IOSTREAM_DEBUG_MSG_TYPE> Debug message type
// <SI91X_DEBUG_PRINTF=> Debug printf
// <SI91X_DEBUG_VIRTUAL_UART_TX=> Virtual UART TX
// <SI91X_DEBUG_ML_PROFILER=> ML Profiler
// <i> Selects the SWO ITM stimulus 8 message type used by the IO Stream Debug
// <i> (DCH) transport. This value is applied automatically inside
// <i> sl_si91x_iostream_debug_init().
// <i> Default: SI91X_DEBUG_PRINTF
#define SL_SI91X_IOSTREAM_DEBUG_MSG_TYPE SI91X_DEBUG_PRINTF

// </h>
// <<< end of configuration section >>>

#endif /* SL_SI91X_IOSTREAM_DEBUG_CONFIG_H */
