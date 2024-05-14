/***************************************************************************/ /**
 * @file
 * @brief SL SI91X Common Config.
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

#ifndef SL_SI91X_PWM_COMMON_CONFIG_H
#define SL_SI91X_PWM_COMMON_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h>PWM Common Configuration

// <o SL_PWM_COMMON_TIMER_SELECTION> Base Timer Selection
//   <SL_BASE_TIMER_EACH_CHANNEL=> Timer (Each Channel)
//   <SL_BASE_TIMER_ALL_CHANNEL=> Timer (All Channels)
// <i> Default: SL_BASE_TIMER_ALL_CHANNEL
#define SL_PWM_COMMON_TIMER_SELECTION SL_BASE_TIMER_ALL_CHANNEL

// </h>
// <<< end of configuration section >>>
#endif // SL_SI91X_PWM_COMMON_CONFIG_H
