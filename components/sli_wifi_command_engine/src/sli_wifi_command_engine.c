/***************************************************************************/ /**
 * @file sli_wifi_command_engine.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdint.h>
#include "sli_wifi_command_engine.h"
#include "sl_constants.h"
#include "sli_wifi_event_handler.h"
#include "cmsis_os2.h"

/******************************************************
 *               Macro Definitions
 ******************************************************/
#define SLI_WLAN_TERMINATE_THREAD_EVENT     (1 << 23)
#define SLI_WLAN_TERMINATE_THREAD_ACK_EVENT (1 << 24)

/******************************************************
 *               Variable Definitions
 ******************************************************/
static osThreadId_t command_engine_ID = 0;

/******************************************************
 *               Function Declarations
******************************************************/

void sli_wifi_command_engine(void *args);

/******************************************************
 *               Function Definitions
******************************************************/

void sli_wifi_command_engine_init(void)
{
  // Call the init function of Command Engine Event Handler.
  sli_wifi_event_handler_init();

  // Create and start command engine thread
  if (NULL == command_engine_ID) {
    const osThreadAttr_t attr = {
      .name       = "sli_wifi_command_engine",
      .priority   = SL_WLAN_COMMAND_ENGINE_THREAD_PRIORITY,
      .stack_mem  = 0,
      .stack_size = 1636,
      .cb_mem     = 0,
      .cb_size    = 0,
      .attr_bits  = 0u,
      .tz_module  = 0u,
    };
    command_engine_ID = osThreadNew(&sli_wifi_command_engine, NULL, &attr);
  }
}

void sli_wifi_command_engine_deinit(void)
{
  uint32_t events_received = 0;
  sli_wifi_event_handler_deinit();

  if (NULL != command_engine_ID) {
    // Signal the thread to terminate
    sli_wifi_command_engine_set_event(SLI_WLAN_TERMINATE_THREAD_EVENT);

    // Wait for thread termination acknowledgment
    events_received = sli_wifi_command_engine_wait_for_event(SLI_WLAN_TERMINATE_THREAD_ACK_EVENT, 5000);
    if (0 == events_received) {
      // Return timeout if acknowledgment is not received
      //return SL_STATUS_TIMEOUT;
      return;
    }

    command_engine_ID = NULL;
  }

  return;
}

void sli_wifi_command_engine(void *args)
{
  UNUSED_PARAMETER(args);

  uint32_t events_to_wait_on = sli_wifi_event_handler_get_flags_to_wait_on();
  uint32_t events_received   = 0;

  events_to_wait_on |= SLI_WLAN_TERMINATE_THREAD_EVENT;

  while (1) {
    uint32_t wait_time = sli_wifi_event_handler_get_wait_time(&events_received);

    events_received |= sli_wifi_command_engine_wait_for_event(events_to_wait_on, wait_time);

    if (events_received & SLI_WLAN_TERMINATE_THREAD_EVENT) {
      // Clear the termination event flag
      events_received &= ~SLI_WLAN_TERMINATE_THREAD_EVENT;

      // Acknowledge the termination request
      sli_wifi_command_engine_set_event(SLI_WLAN_TERMINATE_THREAD_ACK_EVENT);

      // Terminate the current thread
      osThreadTerminate(osThreadGetId());
    }

    sli_wifi_handle_event(&events_received);
  }

  return;
}
