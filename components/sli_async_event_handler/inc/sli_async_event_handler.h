/***************************************************************************/ /**
 * @file sli_async_event_handler.h
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

#ifndef SLI_ASYNC_EVENT_HANDLER_H
#define SLI_ASYNC_EVENT_HANDLER_H

#include <stdint.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_slist.h"
#include "sli_queue_manager.h"

/**
   * @brief Defines the maximum length for the event handler thread name.
   *
   * This macro sets the maximum length for the name of the event handler thread.
   * The default value is set to 32 characters.
   *
   * @note
   * - Ensure that the thread name does not exceed this length to avoid buffer overflows.
   */
#ifndef SLI_ASYNC_EVENT_HANDLER_THREAD_NAME_LENGTH
#define SLI_ASYNC_EVENT_HANDLER_THREAD_NAME_LENGTH 32
#endif

// Forward Declaration of event handler handle for dependencies in callback signatures
typedef struct sli_async_event_handler_s sli_async_event_handler_t;

typedef void (*sli_async_event_handler_callback_t)(sli_async_event_handler_t *instance, uint32_t event, void *data);

// Command engine configuration structure
typedef struct {
  char name[SLI_ASYNC_EVENT_HANDLER_THREAD_NAME_LENGTH]; ///< Event Handler thread name
  uint8_t
    event_type_count; ///< Number of event types for event handler to handle. Note: This count cannot be greater than 10
  sli_async_event_handler_callback_t *event_handlers; ///< Event Handler packet handlers list.
  osPriority_t priority;                              ///< Event Handler thread Priority
  uint32_t stack_size;                                ///< Event Handler thread stack size
} sli_async_event_handler_configuration_t;

// Command engine instance structure
struct sli_async_event_handler_s {
  sli_async_event_handler_configuration_t config; // Event handler Configuration
  osThreadId_t threadId;                          // Event Handler Thread ID
  osEventFlagsId_t eventId;                       // Event ID for the event handler
  sli_queue_t *event_queue;                       // Event Handlers Queue
  void *lock;                                     // Event Handler instance lock to protect contents of instance handle
};

/**
 * @brief Initializes the event handler instance with the specified configuration.
 *
 * This function sets up the event handler instance by applying the provided
 * configuration. It prepares the instance for handling events by creating
 * event handler thread, event handler event id and queues as defined in the
 * configuration.
 *
 * @param[in,out] instance       Pointer to the event handler instance to initialize.
 *                               This must be a valid, uninitialized instance. 
 *                               This must not be NULL.
 * @param[in]     event_config   Pointer to the configuration structure that defines
 *                               the event handling behavior. This must not be NULL.
 *
 * @return sl_status_t
 *         - SL_STATUS_OK if the initialization was successful.
 *         - SL_STATUS_INVALID_PARAMETER if any of the input parameters are invalid.
 *         - SL_STATUS_FAIL if the initialization failed due to other reasons.
 */
sl_status_t sli_async_event_handler_init(sli_async_event_handler_t *instance,
                                         const sli_async_event_handler_configuration_t *event_config);

/**
   * @brief Deinitialize the event handler instance.
   *
   * @param[in] instance Pointer to the event handler instance to deinitialize.
   *
   * @return Status of the deinitialization operation.
   *         - SL_STATUS_OK: Deinitialization successful.
   *         - SL_STATUS_FAIL: Deinitialization failed.
   */
sl_status_t sli_async_event_handler_deinit(sli_async_event_handler_t *instance);

/**
 * @brief Sends an asynchronous event to the event handler.
 *
 * This function allows you to send an event to the specified asynchronous
 * event handler instance. The event is queued for processing, and any
 * associated data can be passed along with it.
 *
 * @param[in] instance Pointer to the asynchronous event handler instance.
 *                     This must be a valid instance initialized prior to calling this function.
 * @param[in] event    The event identifier to be sent. This is typically
 *                     an application-defined value representing the event type.
 * @param[in] data     Pointer to the data associated with the event. This can
 *                     be NULL if no additional data is required for the event.
 *
 * @return SL_STATUS_OK if the event was successfully queued for processing.
 *         An appropriate error code otherwise.
 *
 * @note The caller is responsible for ensuring that the data pointer (if provided)
 *       remains valid until the event is processed.
 */
sl_status_t sli_async_event_handler_send_event(sli_async_event_handler_t *instance, uint32_t event, void *data);

#endif // SLI_ASYNC_EVENT_HANDLER_H