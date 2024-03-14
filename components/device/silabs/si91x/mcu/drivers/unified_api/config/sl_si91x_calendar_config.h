/***************************************************************************/ /**
 * @file sl_si91x_calendar_config.h
 * @brief Calendar configuration file.
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

#ifndef SL_SI91X_CALENDAR_CONFIG_H
#define SL_SI91X_CALENDAR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h>Clock Configuration

#define CALENDAR_RO_CLOCK   1
#define CALENDAR_RC_CLOCK   2
#define CALENDAR_XTAL_CLOCK 4

// <e>CALENDAR UC Configuration
//  <i> Enable: Peripheral configuration is taken straight from the configuration set in the universal configuration (UC).
//  <i> Disable: If the application demands it to be modified during runtime, use the sl_si91x_calendar_set_configuration API to modify the peripheral configuration.
//  <i> Default: 1
#define CALENDAR_UC 1

// <o CALENDAR_CLOCK_TYPE> Clock Source
//   <CALENDAR_RC_CLOCK=> RC Clock
//   <CALENDAR_RO_CLOCK=> RO Clock
//   <CALENDAR_XTAL_CLOCK=> XTAL Clock
// <i> Selection of the Calendar CLK Type.
#define CALENDAR_CLOCK_TYPE CALENDAR_RC_CLOCK

// </e>

// </h>
// <<< end of configuration section >>>

// Structure to get value of clock config from user
typedef struct config {
  uint8_t calendar_clock_type;
} config_t;

#if (CALENDAR_CLOCK_TYPE)
config_t configuration = {
  .calendar_clock_type = CALENDAR_CLOCK_TYPE,
};
#endif

#endif /* SL_SI91X_CALENDAR_CONFIG_H */
