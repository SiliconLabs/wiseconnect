/***************************************************************************/ /**
* @file sl_log_common_config.h
* @brief SL DEBUG LOGGER Config.
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_LOG_COMMON_CONFIG_H
#define SL_LOG_COMMON_CONFIG_H

/** @brief Debug level logging */
#define SL_LOG_CONFIG_LEVEL_DEBUG 1
/** @brief Information level logging */
#define SL_LOG_CONFIG_LEVEL_INFO 2
/** @brief Warning level logging */
#define SL_LOG_CONFIG_LEVEL_WARN 3
/** @brief Error level logging */
#define SL_LOG_CONFIG_LEVEL_ERROR 4
/** @brief Crash level logging - most critical messages */
#define SL_LOG_CONFIG_LEVEL_CRASH 5
/** @brief No logging - all log messages are disabled,
 * once this level is set during at compile time,
 * all logging macros will be replaced with void
 * operations to completely eliminate logging overhead.
 */
#define SL_LOG_CONFIG_LEVEL_NONE 6

/** @brief No arguments supported */
#define SL_LOG_CONFIG_ARG0 0
/** @brief Up to 1 argument supported */
#define SL_LOG_CONFIG_ARG1 1
/** @brief Up to 2 arguments supported */
#define SL_LOG_CONFIG_ARG2 2
/** @brief Up to 3 arguments supported */
#define SL_LOG_CONFIG_ARG3 3
/** @brief Up to 4 arguments supported */
#define SL_LOG_CONFIG_ARG4 4
/** @brief Up to 5 arguments supported */
#define SL_LOG_CONFIG_ARG5 5
/** @brief Up to 6 arguments supported */
#define SL_LOG_CONFIG_ARG6 6
/** @brief Up to 7 arguments supported */
#define SL_LOG_CONFIG_ARG7 7
/** @brief Up to 8 arguments supported */
#define SL_LOG_CONFIG_ARG8 8
/** @brief Up to 9 arguments supported */
#define SL_LOG_CONFIG_ARG9 9
/** @brief Up to 10 arguments supported */
#define SL_LOG_CONFIG_ARG10 10

// <o SL_LOG_CONFIG_LEVEL_COMPILE_TIME> LOG_LEVEL
// <SL_LOG_CONFIG_LEVEL_NONE => NONE
// <SL_LOG_CONFIG_LEVEL_DEBUG => DEBUG
// <SL_LOG_CONFIG_LEVEL_INFO => INFO
// <SL_LOG_CONFIG_LEVEL_WARN => WARN
// <SL_LOG_CONFIG_LEVEL_ERROR => ERROR
// <i> Default: SL_LOG_CONFIG_LEVEL_ERROR
#define SL_LOG_CONFIG_LEVEL_COMPILE_TIME SL_LOG_CONFIG_LEVEL_INFO

// <o SL_LOG_CONFIG_ARG> CONFIG_MAX_ARGS
// <SL_LOG_CONFIG_ARG0 => 0
// <SL_LOG_CONFIG_ARG1 => 1
// <SL_LOG_CONFIG_ARG2 => 2
// <SL_LOG_CONFIG_ARG3 => 3
// <SL_LOG_CONFIG_ARG4 => 4
// <SL_LOG_CONFIG_ARG5 => 5
// <SL_LOG_CONFIG_ARG6 => 6
// <SL_LOG_CONFIG_ARG7 => 7
// <SL_LOG_CONFIG_ARG8 => 8
// <SL_LOG_CONFIG_ARG9 => 9
// <SL_LOG_CONFIG_ARG10 => 10
// <i> Default: 3
#define SL_LOG_CONFIG_ARG SL_LOG_CONFIG_ARG3

// <o SL_LOG_NUMBER_OF_EVENTS> No of Logs <1-255>
// <i> Default: 128
#define SL_LOG_NUMBER_OF_EVENTS 128 // Number of events

// <q SL_LOG_DEBUG_ASSERT_ENABLE> Enable Debug Assertions
// <i> When enabled, SL_DEBUG_ASSERT will check conditions and trigger asserts on failure.
// <i> When disabled, SL_DEBUG_ASSERT becomes a no-op to save code space in release builds.
// <i>during development to catch errors early
// <i> Default: 0 (disabled)
#define SL_LOG_DEBUG_ASSERT_ENABLE 0

#endif /* SL_LOG_COMMON_CONFIG_H */

// <<< end of configuration section >>>
