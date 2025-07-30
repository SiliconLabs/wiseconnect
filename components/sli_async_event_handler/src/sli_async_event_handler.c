/***************************************************************************/ /**
 * @file sli_command_engine.c
 * @brief
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

#include <stdint.h>
#include "sli_async_event_handler.h"
#include "sli_queue_manager.h"
#include "sl_constants.h"
#include "cmsis_os2.h"

/******************************************************
   *               Macro Definitions
   ******************************************************/
// Event flag for thread termination notification
#define SLI_EVENT_HANDLER_THREAD_TERMINATE_EVENT (1 << 23)

// Event flag for thread termination acknowledgment
#define SLI_EVENT_HANDLER_THREAD_TERMINATE_ACK_EVENT (1 << 22)

/******************************************************
   *               Local Type Definitions
  ******************************************************/

/******************************************************
  *               Variable Definitions
  ******************************************************/

/******************************************************
  *              Static Function Declarations
  ******************************************************/
// Wait for a specific event or timeout
static uint32_t sli_event_handler_wait_for_event(osEventFlagsId_t eventId, uint32_t event_mask, uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(eventId, event_mask, osFlagsWaitAny, timeout);

  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

// Set a specific event flag
static void sli_event_handler_set_event(osEventFlagsId_t eventId, uint32_t event_mask)
{
  osEventFlagsSet(eventId, event_mask);
  return;
}

// Event Handler thread function
static void sli_event_handler_thread(void *args)
{
  sli_async_event_handler_t *instance = (sli_async_event_handler_t *)args;
  uint32_t events_to_wait_on          = 0;
  uint32_t events_received            = 0;
  sl_status_t status                  = SL_STATUS_FAIL;
  uint32_t event                      = 0;
  void *data                          = NULL;

  SL_DEBUG_LOG("%s task started \n", instance->config.name);

  // Combine all TX events to wait on
  for (uint16_t i = 0; i < instance->config.event_type_count; i++) {
    events_to_wait_on |= (1 << i);
  }
  events_to_wait_on |= SLI_EVENT_HANDLER_THREAD_TERMINATE_EVENT;

  while (1) {
    // Wait for any of the events
    events_received |= sli_event_handler_wait_for_event(instance->eventId, events_to_wait_on, osWaitForever);

    if (events_received & SLI_EVENT_HANDLER_THREAD_TERMINATE_EVENT) {
      // Clear the termination event flag
      events_received &= ~SLI_EVENT_HANDLER_THREAD_TERMINATE_EVENT;

      // Acknowledge the termination request
      sli_event_handler_set_event(instance->eventId, SLI_EVENT_HANDLER_THREAD_TERMINATE_ACK_EVENT);

      // Terminate the current thread
      osThreadTerminate(osThreadGetId());
    }

    for (uint8_t i = 0; i < instance->config.event_type_count; i++) {
      event = (1 << i);
      while (events_received & event) {
        status = sli_queue_manager_dequeue(&(instance->event_queue[i]), &data);
        if (SL_STATUS_OK != status) {
          // ToDo: Treate the error as fatal and handle it appropriately
          continue;
        }

        // Handle the event
        if (NULL != instance->config.event_handlers[i]) {
          instance->config.event_handlers[i](instance, event, data);
        }

        if (SLI_QUEUE_MANAGER_IS_QUEUE_EMPTY(&(instance->event_queue[i]))) {
          events_received &= ~event;
        }
      }
    }
  }

  return;
}

/******************************************************
  *             Internal Function Definitions
  ******************************************************/
// Initialize the command engine
sl_status_t sli_async_event_handler_init(sli_async_event_handler_t *instance,
                                         const sli_async_event_handler_configuration_t *event_config)
{
  sl_status_t status = SL_STATUS_FAIL;

  // Store the configuration in the instance
  instance->config = *event_config;

  // Create event flags for the command engine event handler
  instance->eventId = osEventFlagsNew(NULL);

  // Allocate memory for the queue information based on the packet type count
  instance->event_queue = (sli_queue_t *)malloc(sizeof(sli_queue_t) * event_config->event_type_count);

  // Check if memory allocation was successful
  if (NULL == (instance->event_queue)) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  // Initialize each queue in the event handler queue
  for (uint16_t i = 0; i < event_config->event_type_count; i++) {
    status = sli_queue_manager_init(&instance->event_queue[i], SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
    VERIFY_STATUS_AND_RETURN(status);
  }

  // Thread attributes
  const osThreadAttr_t attr = {
    .name       = event_config->name,
    .priority   = event_config->priority,
    .stack_mem  = 0,
    .stack_size = event_config->stack_size,
    .cb_mem     = 0,
    .cb_size    = 0,
    .attr_bits  = 0u,
    .tz_module  = 0u,
  };

  // Create a new thread for the command engine event handler
  instance->threadId = osThreadNew(&sli_event_handler_thread, (void *)instance, &attr);
  if (NULL == instance->threadId) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

// Deinitialize the command engine
sl_status_t sli_async_event_handler_deinit(sli_async_event_handler_t *instance)
{
  uint32_t events_received = 0;

  if (NULL == instance) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Signal the thread to terminate
  sli_event_handler_set_event(instance->eventId, SLI_EVENT_HANDLER_THREAD_TERMINATE_EVENT);

  // Wait for thread termination acknowledgment
  events_received =
    sli_event_handler_wait_for_event(instance->eventId, SLI_EVENT_HANDLER_THREAD_TERMINATE_ACK_EVENT, 5000);
  if (0 == events_received) {
    // Return timeout if acknowledgment is not received
    return SL_STATUS_TIMEOUT;
  }
  instance->threadId = NULL;

  return SL_STATUS_OK;
}

// Send an asynchronous event to the event handler
sl_status_t sli_async_event_handler_send_event(sli_async_event_handler_t *instance, uint32_t event, void *data)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (NULL == instance) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Enqueue the event data
  status = sli_queue_manager_enqueue(&(instance->event_queue[event]), data);
  if (SL_STATUS_OK != status) {
    return status;
  }

  // Set the event flag for the event handler thread
  sli_event_handler_set_event(instance->eventId, (1 << event));

  return SL_STATUS_OK;
}
