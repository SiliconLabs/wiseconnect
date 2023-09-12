/***************************************************************************/ /**
 * @file sl_si91x_gspi_config.h
 * @brief GSPI API configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_SI91X_GSPI_CONFIG_H
#define SL_SI91X_GSPI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_gspi.h"

// <<< Use Configuration Wizard in Context Menu >>>

//<h>GSPI Configuration

// <o SL_GSPI_CLOCK_MODE> Mode
//   <SL_GSPI_MODE_0=> Mode 0
//   <SL_GSPI_MODE_3=> Mode 3
// <i> Selection of the GSPI Mode.
#define SL_GSPI_CLOCK_MODE SL_GSPI_MODE_0

// <o SL_GSPI_BITRATE> Bit Rate (Bits/Second) <1-40000000>
// <i> Default: 10000000
#define SL_GSPI_BITRATE 10000000

// <o SL_GSPI_BIT_WIDTH> Data Width <1-15>
// <i> Default: 8
#define SL_GSPI_BIT_WIDTH 8

// <o SL_GSPI_SLAVE_SELECT_MODE> Manual Chip Select
//   <SL_GSPI_MASTER_SW=>  Enable
//   <SL_GSPI_MASTER_HW_OUTPUT=> Disable
// <i> Selection of the SSM Master Control.
#define SL_GSPI_SLAVE_SELECT_MODE SL_GSPI_MASTER_HW_OUTPUT

// <q SL_GSPI_SWAP_READ_DATA> Byte-wise Swapping Read
// <i> Default: 1
#define SL_GSPI_SWAP_READ_DATA 1

// <q SL_GSPI_SWAP_WRITE_DATA> Byte-wise Swapping Write
// <i> Default: 0
#define SL_GSPI_SWAP_WRITE_DATA 0

// </h>
// <<< end of configuration section >>>

#if defined(SL_GSPI_CLOCK_MODE)
sl_gspi_control_config_t gspi_configuration = {
  .bit_width         = SL_GSPI_BIT_WIDTH,
  .clock_mode        = SL_GSPI_CLOCK_MODE,
  .slave_select_mode = SL_GSPI_SLAVE_SELECT_MODE,
  .bitrate           = SL_GSPI_BITRATE,
  .swap_read         = SL_GSPI_SWAP_READ_DATA,
  .swap_write        = SL_GSPI_SWAP_WRITE_DATA,
};
#endif

#ifdef __cplusplus
}
#endif // SL_GSPI_CLOCK_MODE
#endif // SL_SI91X_GSPI_CONFIG_H
