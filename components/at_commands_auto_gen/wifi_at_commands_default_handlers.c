/***************************************************************************/ /**
 * @file wifi_at_commands_default_handlers.c
 * @brief Wi-Fi AT commands default handlers
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

#include "console_types.h"
#include "sl_status.h"
#include "sl_utility.h"
#include "cmsis_compiler.h"
#include "sl_constants.h"

sl_status_t wifi_peripherals_cnf_axsiga_command_handler(console_args_t *arguments);
sl_status_t wifi_peripherals_cnf_axsigb_command_handler(console_args_t *arguments);
sl_status_t wifi_peripherals_stop_command_handler(console_args_t *arguments);
sl_status_t wifi_statistics_stop_command_handler(console_args_t *arguments);
sl_status_t wifi_statistics_start_command_handler(console_args_t *arguments);
sl_status_t wifi_peripherals_cnf_gen_command_handler(console_args_t *arguments);
sl_status_t wifi_peripherals_start_command_handler(console_args_t *arguments);
sl_status_t wifi_peripherals_cnf_axppdu_command_handler(console_args_t *arguments);
sl_status_t net_info_command_handler(console_args_t *arguments);

__WEAK sl_status_t wifi_per_cnf_axsiga_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_per_cnf_axsigb_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_per_stop_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_statistics_stop_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_statistics_start_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_per_cnf_gen_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_per_start_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_per_cnf_axppdu_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}

__WEAK sl_status_t wifi_peripherals_cnf_axsiga_command_handler(console_args_t *arguments)
{
  return wifi_per_cnf_axsiga_command_handler(arguments);
}

__WEAK sl_status_t wifi_peripherals_cnf_axsigb_command_handler(console_args_t *arguments)
{
  return wifi_per_cnf_axsigb_command_handler(arguments);
}

__WEAK sl_status_t wifi_peripherals_stop_command_handler(console_args_t *arguments)
{
  return wifi_per_stop_command_handler(arguments);
}

__WEAK sl_status_t wifi_peripherals_cnf_gen_command_handler(console_args_t *arguments)
{
  return wifi_per_cnf_gen_command_handler(arguments);
}

__WEAK sl_status_t wifi_peripherals_start_command_handler(console_args_t *arguments)
{
  return wifi_per_start_command_handler(arguments);
}

__WEAK sl_status_t wifi_peripherals_cnf_axppdu_command_handler(console_args_t *arguments)
{
  return wifi_per_cnf_axppdu_command_handler(arguments);
}

__WEAK sl_status_t net_info_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return SL_STATUS_NOT_SUPPORTED;
}
