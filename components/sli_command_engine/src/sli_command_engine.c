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
#include <string.h>
#include "cmsis_os2.h"
#include "sl_constants.h"
#include "sl_core.h"
#include "sli_command_engine.h"
#include "sli_queue_manager.h"
#include "sli_routing_utility.h"

/******************************************************
  *               Macro Definitions
  ******************************************************/
// Event flag for thread termination notification
#define SLI_COMMAND_ENGINE_THREAD_TERMINATE_EVENT (1 << 23)

// Event flag for thread termination acknowledgment
#define SLI_COMMAND_ENGINE_THREAD_TERMINATE_ACK_EVENT (1 << 22)

// Event flag for packet TX acknowledgment
#define SLI_COMMAND_ENGINE_PACKET_TX_ACK_EVENT (1 << 21)

// Event flag for packet RX
#define SLI_COMMAND_ENGINE_PACKET_RX_EVENT (1 << 20)

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
static uint32_t sli_command_engine_wait_for_event(osEventFlagsId_t command_engine_eventId,
                                                  uint32_t event_mask,
                                                  uint32_t timeout)
{
  uint32_t result = osEventFlagsWait(command_engine_eventId, event_mask, osFlagsWaitAny, timeout);

  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return 0;
  }
  return result;
}

// Set a specific event flag
static void sli_command_engine_set_event(osEventFlagsId_t command_engine_eventId, uint32_t event_mask)
{
  osEventFlagsSet(command_engine_eventId, event_mask);
  return;
}

static sl_status_t sli_command_engine_send_error_event(sli_command_engine_t *instance,
                                                       sl_command_engine_error_status_t error)
{
  sl_status_t status                             = SL_STATUS_OK;
  sl_command_engine_error_status_t *error_status = NULL;

  if (NULL == instance->config.error_event_handler) {
    return SL_STATUS_OK;
  }

  status = sli_buffer_manager_allocate_buffer(instance->config.error_buffer_pool_type,
                                              SLI_BUFFER_MANAGER_ALLOCATION_TYPE_HYBRID,
                                              1000,
                                              (sli_buffer_t *)&error_status);
  if (SL_STATUS_OK != status) {
    error_status           = instance->error_buffer;
    instance->error_buffer = NULL;
    *error_status          = SLI_COMMAND_ENGINE_STATUS_MEMORY_ERROR;
    status                 = SL_STATUS_ALLOCATION_FAILED;
  }

  *error_status = error;
  sli_async_event_handler_send_event(instance->config.error_event_handler,
                                     instance->config.error_event,
                                     (void *)error_status);
  return status;
}

static bool rx_packet_identity_handler(sli_queue_t *handle, void *data, void *node_match_data)
{
  UNUSED_PARAMETER(handle);
  sli_command_engine_metadata_t *rx_metadata = (sli_command_engine_metadata_t *)node_match_data;
  sli_command_engine_metadata_t *metadata    = (sli_command_engine_metadata_t *)data;

  if ((rx_metadata->tx_info.packet_type == metadata->tx_info.packet_type)
      && (rx_metadata->tx_info.frame_id == metadata->tx_info.frame_id)) {
    return true;
  }
  return false;
}

// Command engine thread function
static void sli_command_engine_thread(void *args)
{
  sli_command_engine_t *instance            = (sli_command_engine_t *)args;
  uint32_t events_to_wait_on                = 0;
  uint32_t events_received                  = 0;
  uint32_t tx_events_mask                   = 0;
  sl_status_t status                        = SL_STATUS_FAIL;
  sli_command_engine_metadata_t rx_metadata = { 0 };
  sli_command_engine_metadata_t *metadata   = NULL;
  uint16_t packet_type                      = 0;
  void *data                                = NULL;

  SL_DEBUG_LOG("%s task started \n", instance->config.name);

  // Combine all TX events to wait on
  for (uint16_t i = 0; i < instance->config.packet_type_count; i++) {
    events_to_wait_on |= instance->queue_info[i].event;
  }
  events_to_wait_on |= (SLI_COMMAND_ENGINE_THREAD_TERMINATE_EVENT | SLI_COMMAND_ENGINE_PACKET_TX_ACK_EVENT
                        | SLI_COMMAND_ENGINE_PACKET_RX_EVENT);

  while (1) {
    // Wait for any of the events
    events_received |=
      sli_command_engine_wait_for_event(instance->command_engine_eventId, events_to_wait_on, osWaitForever);

    if (events_received & SLI_COMMAND_ENGINE_THREAD_TERMINATE_EVENT) {
      // Clear the termination event flag
      events_received &= ~SLI_COMMAND_ENGINE_THREAD_TERMINATE_EVENT;

      // Acknowledge the termination request
      sli_command_engine_set_event(instance->command_engine_eventId, SLI_COMMAND_ENGINE_THREAD_TERMINATE_ACK_EVENT);

      // Terminate the current thread
      osThreadTerminate(osThreadGetId());
    }

    if (SLI_COMMAND_ENGINE_PACKET_RX_EVENT == (events_received & SLI_COMMAND_ENGINE_PACKET_RX_EVENT)) {
      // Handle RX event
      status = sli_queue_manager_dequeue(&(instance->rx_packet_queue), (void **)&data);
      if (SL_STATUS_OK != status) {
        // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
        status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
        if (SL_STATUS_OK != status) {
          break;
        }
        continue;
      }

      if (SLI_QUEUE_MANAGER_IS_QUEUE_EMPTY(&(instance->rx_packet_queue))) {
        events_received &= ~(SLI_COMMAND_ENGINE_PACKET_RX_EVENT);
      }

      rx_metadata.tx_info.data_packet = data;
      status                          = instance->config.get_packet_metadata(instance, data, &rx_metadata);
      if (SL_STATUS_OK != status) {
        sli_buffer_manager_free_buffer(data);
        continue;
      }
      packet_type = rx_metadata.tx_info.packet_type;

      if (instance->queue_info[rx_metadata.tx_info.packet_type].in_flight_command_count > 0) {
        instance->queue_info[rx_metadata.tx_info.packet_type].in_flight_command_count--;
      }

      // Handle RX event
      if (NULL != instance->config.packet_type_configuration[packet_type].rx_event_handler) {
        status =
          instance->config.packet_type_configuration[packet_type].rx_event_handler(instance, packet_type, (void *)data);
        if (SL_STATUS_OK != status) {
          // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
          status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
          if (SL_STATUS_OK != status) {
            break;
          }
          sli_buffer_manager_free_buffer(data);
          continue;
        }
      }

      status = sli_queue_manager_remove_node_from_queue(
        &(instance->queue_info[rx_metadata.tx_info.packet_type].inflight_packet_queue),
        rx_packet_identity_handler,
        (void *)&rx_metadata,
        (void **)&metadata);
      if ((SL_STATUS_NOT_FOUND == status) | (SL_STATUS_EMPTY == status)) {
        SL_DEBUG_LOG("Sending data pointer : 0x%X to async event handler\n", (unsigned int)data);
        status = sli_async_event_handler_send_event(
          instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].async_response_event_handler,
          instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].async_response_event,
          data);
        if (SL_STATUS_OK != status) {
          // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
          status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
          if (SL_STATUS_OK != status) {
            break;
          }
          continue;
        }
      } else if (SL_STATUS_OK != status) {
        // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
        status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
        if (SL_STATUS_OK != status) {
          break;
        }
        continue;
      } else {
        SL_DEBUG_LOG("Found meta data : 0x%X\n", (unsigned int)metadata);
        metadata->tx_info.data_packet        = data;
        metadata->tx_info.data_packet_length = rx_metadata.tx_info.data_packet_length;

        SL_DEBUG_LOG("Adding %u command packet to queue 0x%X\n",
                     rx_metadata.tx_info.packet_type,
                     (unsigned int)instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type]
                       .sync_response_queue);

        // Enqueue the data packet into the appropriate queue
        status = sli_queue_manager_enqueue(
          instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].sync_response_queue,
          (void *)metadata);
        if (SL_STATUS_OK != status) {
          // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
          status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
          if (SL_STATUS_OK != status) {
            break;
          }
          continue;
        }
        SL_DEBUG_LOG(
          "Sending Event: 0x%X on event id : 0x%X for queue 0x%X\n",
          (unsigned int)instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].sync_response_event,
          (unsigned int)*(
            instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].sync_response_event_id),
          (unsigned int)instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type]
            .sync_response_queue);

        // Signal the thread to handle the packet
        sli_command_engine_set_event(
          *(instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].sync_response_event_id),
          instance->config.packet_type_configuration[rx_metadata.tx_info.packet_type].sync_response_event);

        metadata = NULL;
      }

      memset((void *)&rx_metadata, 0, sizeof(sli_command_engine_metadata_t));
    }

    for (uint8_t i = 0; i < instance->config.packet_type_count; i++) {
      if ((instance->queue_info[i].event == (events_received & (~tx_events_mask) & instance->queue_info[i].event))
          && (instance->queue_info[i].in_flight_command_count
              < instance->config.packet_type_configuration[i].max_in_flight_command_count)) {
        status = sli_queue_manager_dequeue(&(instance->queue_info[i].packet_queue), (void **)&metadata);
        if (SL_STATUS_OK != status) {
          // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
          status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
          if (SL_STATUS_OK != status) {
            break;
          }
          continue;
        }

        // Handle TX event
        if (NULL != instance->config.packet_type_configuration[i].pre_tx_handler) {
          status = instance->config.packet_type_configuration[i].pre_tx_handler(instance, i, (void *)metadata);
          if (SL_STATUS_OK != status) {
            // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
            status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
            if (SL_STATUS_OK != status) {
              break;
            }
            continue;
          }
        }

        status = sli_routing_utility_route_packet(instance->config.routing_table,
                                                  instance->config.packet_type_configuration[i].route_packet_type,
                                                  metadata->tx_info.data_packet,
                                                  metadata->tx_info.data_packet_length,
                                                  metadata);
        if (SL_STATUS_IN_PROGRESS == status) {
          // Packet is in progress, Pause sending packet until TX is completed
          continue;
        } else if (SL_STATUS_OK != status) {
          // Trigger SLI_COMMAND_ENGINE_STATUS_COMMAND_TX_FAILED event to error event id
          status = sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_COMMAND_TX_FAILED);
          if (SL_STATUS_OK != status) {
            break;
          }
          continue;
        } else {
          sli_buffer_manager_free_buffer(metadata->tx_info.data_packet);
        }

        if ((SLI_COMMAND_ENGINE_COMMAND_PACKET == (SLI_COMMAND_ENGINE_COMMAND_PACKET & metadata->tx_info.flags))
            && ((SLI_COMMAND_ENGINE_SYNC_RESPONSE_STATUS_PACKET
                 == (SLI_COMMAND_ENGINE_SYNC_RESPONSE_STATUS_PACKET & metadata->tx_info.flags))
                || (SLI_COMMAND_ENGINE_SYNC_RESPONSE_DATA_PACKET
                    == (SLI_COMMAND_ENGINE_SYNC_RESPONSE_DATA_PACKET & metadata->tx_info.flags)))) {
          metadata->tx_info.data_packet        = NULL;
          metadata->tx_info.data_packet_length = 0;
          SL_DEBUG_LOG("Adding meta data : 0x%X\n", (unsigned int)metadata);
          status = sli_queue_manager_enqueue(&(instance->queue_info[i].inflight_packet_queue), (void *)metadata);
          if (SL_STATUS_OK != status) {
            // Trigger SLI_COMMAND_ENGINE_FATAL_ERROR event to error event id
            sli_command_engine_send_error_event(instance, SLI_COMMAND_ENGINE_STATUS_FATAL_ERROR);
            break;
          }

          metadata = NULL;

          instance->queue_info[i].in_flight_command_count++;
          if (SLI_QUEUE_MANAGER_IS_QUEUE_EMPTY(&(instance->queue_info[i].packet_queue))) {
            events_received &= ~(instance->queue_info[i].event);
          }
        } else {
          sli_buffer_manager_free_buffer(metadata);
          metadata = NULL;
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
sl_status_t sli_command_engine_init(sli_command_engine_t *instance,
                                    const sli_command_engine_configuration_t *command_config)
{
  sl_status_t status = SL_STATUS_FAIL;

  // Store the configuration in the instance
  instance->config = *command_config;

  // Allocate memory for the queue information based on the packet type count
  instance->queue_info = (sli_command_engine_queue_info_t *)malloc(sizeof(sli_command_engine_queue_info_t)
                                                                   * command_config->packet_type_count);

  // Check if memory allocation was successful
  if (NULL == (instance->queue_info)) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  // Initialize each queue in the queue information
  for (uint16_t i = 0; i < instance->config.packet_type_count; i++) {
    memset(&(instance->queue_info[i]), 0, sizeof(sli_command_engine_queue_info_t));
    status = sli_queue_manager_init(&instance->queue_info[i].packet_queue, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
    VERIFY_STATUS_AND_RETURN(status);
    status = sli_queue_manager_init(&instance->queue_info[i].inflight_packet_queue, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
    VERIFY_STATUS_AND_RETURN(status);
    instance->queue_info[i].event = (1 << i);
  }

  status = sli_queue_manager_init(&instance->rx_packet_queue, SLI_BUFFER_MANAGER_QUEUE_NODE_POOL);
  VERIFY_STATUS_AND_RETURN(status);
  // Create event flags for the command engine
  instance->command_engine_eventId = osEventFlagsNew(NULL);

  // Thread attributes
  const osThreadAttr_t attr = {
    .name       = command_config->name,
    .priority   = command_config->priority,
    .stack_mem  = 0,
    .stack_size = command_config->stack_size,
    .cb_mem     = 0,
    .cb_size    = 0,
    .attr_bits  = 0u,
    .tz_module  = 0u,
  };

  // Create a new thread for the command engine
  instance->command_engine_threadId = osThreadNew(&sli_command_engine_thread, (void *)instance, &attr);
  if (NULL == instance->command_engine_threadId) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

// Deinitialize the command engine
sl_status_t sli_command_engine_deinit(sli_command_engine_t *instance)
{
  uint32_t events_received = 0;

  if (NULL == instance) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == instance->command_engine_eventId) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Signal the thread to terminate
  sli_command_engine_set_event(instance->command_engine_eventId, SLI_COMMAND_ENGINE_THREAD_TERMINATE_EVENT);

  // Wait for thread termination acknowledgment
  events_received = sli_command_engine_wait_for_event(instance->command_engine_eventId,
                                                      SLI_COMMAND_ENGINE_THREAD_TERMINATE_ACK_EVENT,
                                                      5000);
  if (0 == events_received) {
    // Return timeout if acknowledgment is not received
    return SL_STATUS_TIMEOUT;
  }

  instance->command_engine_threadId = NULL;

  return SL_STATUS_OK;
}

// Send a packet through the command engine
sl_status_t sli_command_engine_send_packet(sli_command_engine_t *instance, sli_command_engine_tx_info_t *tx_info)
{
  sl_status_t status                      = SL_STATUS_OK;
  sli_command_engine_metadata_t *metadata = NULL;

  // Check if the instance or data is NULL
  if ((NULL == instance) || (NULL == tx_info)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the packet type is within the valid range
  if (tx_info->packet_type >= instance->config.packet_type_count) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Allocate memory for the packet
  status = sli_buffer_manager_allocate_buffer(instance->config.metadata_buffer_pool_type,
                                              SLI_BUFFER_MANAGER_ALLOCATION_TYPE_HYBRID,
                                              1000,
                                              (sli_buffer_t *)&metadata);
  VERIFY_STATUS_AND_RETURN(status);

  CORE_irqState_t state = CORE_EnterAtomic();
  tx_info->packet_id    = (instance->queue_info[tx_info->packet_type].packet_id)++;
  CORE_ExitAtomic(state);

  metadata->instance               = instance;
  metadata->tx_info.packet_type    = tx_info->packet_type;
  metadata->tx_info.packet_id      = tx_info->packet_id;
  metadata->tx_info                = *tx_info;
  metadata->packet_start_tickcount = osKernelGetTickCount();

  // Enqueue the data packet into the appropriate queue
  status =
    sli_queue_manager_enqueue(&(instance->queue_info[tx_info->packet_type].packet_queue), (sl_slist_node_t *)metadata);
  VERIFY_STATUS_AND_RETURN(status);

  // Signal the thread to handle the packet
  sli_command_engine_set_event(instance->command_engine_eventId, instance->queue_info[tx_info->packet_type].event);

  // Return success status
  return SL_STATUS_OK;
}

// Send a packet through the command engine
sl_status_t sli_command_engine_receive_packet(sli_command_engine_t *instance, void *data)
{
  sl_status_t status = SL_STATUS_OK;

  // Check if the instance or data is NULL
  if ((NULL == instance) || (NULL == data)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Enqueue the data packet into the appropriate queue
  status = sli_queue_manager_enqueue(&(instance->rx_packet_queue), data);
  VERIFY_STATUS_AND_RETURN(status);

  // Signal the thread to handle the packet
  sli_command_engine_set_event(instance->command_engine_eventId, SLI_COMMAND_ENGINE_PACKET_RX_EVENT);

  // Return success status
  return SL_STATUS_OK;
}

void sli_command_engine_send_packet_tx_status(uint16_t packet_type, sl_status_t status, void *context)
{
  UNUSED_PARAMETER(packet_type);
  sli_command_engine_metadata_t *metadata = (sli_command_engine_metadata_t *)context;
  sli_command_engine_t *instance          = metadata->instance;

  metadata->packet_status = status;

  status =
    sli_queue_manager_enqueue(&(instance->queue_info[metadata->tx_info.packet_type].packet_queue), (void *)metadata);
  if (SL_STATUS_OK != status) {
    return;
  }

  // Set the event flag for the packet type
  sli_command_engine_set_event(instance->command_engine_eventId,
                               instance->queue_info[metadata->tx_info.packet_type].event);

  return;
}
