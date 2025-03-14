/***************************************************************************/ /**
 * @file
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
#include "sl_si91x_host_interface.h"
#include "sl_si91x_types.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include "sl_wifi_constants.h"
#include "sl_wifi_types.h"
#include "sl_rsi_utility.h"
#include "cmsis_os2.h"
#include "cmsis_compiler.h"
#include "sl_si91x_core_utilities.h"
#include <string.h>
#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_m4.h"
#endif
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#include "sl_si91x_socket_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_net_si91x_integration_handler.h"
#else
// This macro defines a handler for dispatching network events.
// It is used to handle events related to the SI91x module
#define SL_NET_EVENT_DISPATCH_HANDLER(data, packet) \
  {                                                 \
    UNUSED_PARAMETER(data);                         \
    UNUSED_PARAMETER(packet);                       \
  }
#endif

#ifdef SLI_SI91X_ENABLE_BLE
#include "rsi_bt_common.h"
#endif

#define BUS_THREAD_EVENTS                                                                                          \
  (SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS | SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT | SL_SI91X_NCP_HOST_BUS_RX_EVENT \
   | SL_SI91X_SOCKET_COMMAND_TX_PENDING_EVENT | SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT                             \
   | SL_SI91X_TA_BUFFER_FULL_CLEAR_EVENT | SL_SI91X_TERMINATE_BUS_THREAD_EVENT)

/**
 * All flags used with async events
 */
#define SL_SI91X_NOTIFICATION_FLAG(x) (1 << (x))

// Indicates asynchronous RX response received for WLAN command type
#define NCP_HOST_WLAN_NOTIFICATION_EVENT SL_SI91X_NOTIFICATION_FLAG(SI91X_WLAN_CMD)

// Indicates asynchronous RX response received for NETWORK command type
#define NCP_HOST_NETWORK_NOTIFICATION_EVENT SL_SI91X_NOTIFICATION_FLAG(SI91X_NETWORK_CMD)

// Indicates asynchronous RX response received for SOCKET command type
#define NCP_HOST_SOCKET_NOTIFICATION_EVENT SL_SI91X_NOTIFICATION_FLAG(SI91X_SOCKET_CMD)

// Indicates asynchronous RX response received for SOCKET data type
#define NCP_HOST_SOCKET_DATA_NOTIFICATION_EVENT SL_SI91X_NOTIFICATION_FLAG(SI91X_CMD_MAX)

/******************************************************
 *               Variable Definitions
 ******************************************************/

extern osEventFlagsId_t si91x_async_events;
extern osEventFlagsId_t si91x_events;

volatile uint32_t tx_command_queues_status             = 0;
volatile uint32_t tx_socket_command_queues_status      = 0;
volatile uint32_t tx_socket_data_queues_status         = 0;
volatile uint32_t tx_generic_socket_data_queues_status = 0;

volatile uint32_t tx_command_queues_command_in_flight_status       = 0;
volatile uint8_t tx_socket_command_command_in_flight_queues_status = 0;

#if SLI_SI91X_MCU_INTERFACE
extern sl_si91x_buffer_queue_t sli_ahb_bus_rx_queue;
#endif

#if SL_NCP_UART_INTERFACE
extern sl_si91x_buffer_queue_t sli_uart_bus_rx_queue;
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/

void si91x_event_handler_thread(const void *args);

static void set_async_event(uint32_t event_mask);

// Declaration of a global flag to indicate if background mode is enabled
extern bool bg_enabled;

// Declaration of the SI91x event handler function
extern sl_wifi_event_handler_t si91x_event_handler;

extern sli_si91x_command_queue_t cmd_queues[SI91X_CMD_MAX];
extern sl_si91x_buffer_queue_t sli_tx_data_queue;

void si91x_event_handler_thread(const void *args);

extern sl_status_t sl_create_generic_rx_packet_from_params(sli_si91x_queue_packet_t **queue_packet,
                                                           sl_wifi_buffer_t **packet_buffer,
                                                           uint16_t packet_id,
                                                           uint8_t flags,
                                                           void *sdk_context,
                                                           uint16_t frame_status);

void sli_submit_rx_buffer(void);
void si91x_bus_thread(const void *args);
void sli_handle_dhcp_and_rejoin_failure(void *sdk_context, sl_wifi_buffer_t *temp_buffer, uint16_t frame_status);
void si91x_event_handler_thread(const void *args);
#ifdef SLI_SI91X_MCU_INTERFACE
// External declaration of a function to configure M4 DMA descriptors on reset
extern void sli_si91x_config_m4_dma_desc_on_reset(void);
void unmask_ta_interrupt(uint32_t interrupt_no);
#endif

static sl_status_t bus_write_data_frame(sl_si91x_buffer_queue_t *queue);

static sl_status_t bus_write_frame(sli_si91x_command_queue_t *queue,
                                   sl_si91x_command_type_t command_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   bool *global_queue_block);

sl_status_t si91x_req_wakeup(void);
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
static sli_si91x_socket_t *get_socket_from_packet(sl_si91x_packet_t *socket_packet);
#endif
bool sli_si91x_is_bus_ready(bool global_queue_block);

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t sl_create_generic_rx_packet_from_params(sli_si91x_queue_packet_t **queue_packet,
                                                    sl_wifi_buffer_t **packet_buffer,
                                                    uint16_t packet_id,
                                                    uint8_t flags,
                                                    void *sdk_context,
                                                    uint16_t frame_status)
{
  sli_si91x_queue_packet_t *packet = NULL;
  sl_wifi_buffer_t *buffer         = NULL;
  sl_status_t status               = SL_STATUS_OK;
  uint16_t temp                    = 0;

  status = sl_si91x_host_allocate_buffer(&buffer,
                                         SL_WIFI_RX_FRAME_BUFFER,
                                         sizeof(sli_si91x_queue_packet_t),
                                         SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
    BREAKPOINT();
  }

  *packet_buffer = buffer;

  packet = sl_si91x_host_get_buffer_data(buffer, 0, &temp);
  if (packet == NULL) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  // Fill dummy packet with details passed
  buffer->id                = packet_id;
  packet->sdk_context       = sdk_context;
  packet->flags             = flags;
  packet->frame_status      = frame_status;
  packet->firmware_queue_id = RSI_WLAN_MGMT_Q;

  *queue_packet = packet;
  return SL_STATUS_OK;
}

void sli_handle_dhcp_and_rejoin_failure(void *sdk_context, sl_wifi_buffer_t *response_buffer, uint16_t frame_status)
{
  sl_status_t status;
  sl_wifi_buffer_t *temp_buffer = NULL;
  sli_si91x_queue_packet_t *node;

  // Retrieve the packet data from the response buffer
  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(response_buffer, 0, NULL);

  // Create a generic RX packet from the parameters
  status = sl_create_generic_rx_packet_from_params(&node, &temp_buffer, 0, 0, NULL, frame_status);
  if (status != SL_STATUS_OK) {
    return; // Exit if packet creation fails
  }

  sl_wifi_buffer_t *dummy_packet_buffer = NULL;

  // Allocate buffer for the dummy packet
  status = sl_si91x_host_allocate_buffer(&dummy_packet_buffer,
                                         SL_WIFI_RX_FRAME_BUFFER,
                                         (sizeof(packet->desc) + packet->length),
                                         SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  if (status != SL_STATUS_OK) {
    sl_si91x_host_free_buffer(temp_buffer); // Free temp buffer on failure
    return;                                 // Exit if buffer allocation fails
  }

  // Get the dummy packet data from the allocated buffer
  sl_si91x_packet_t *dummy_packet = sl_si91x_host_get_buffer_data(dummy_packet_buffer, 0, NULL);
  node->host_packet               = dummy_packet_buffer; // Link dummy packet to the node
  node->sdk_context               = sdk_context;

  // Copy the original packet data to the dummy packet
  memcpy(dummy_packet, packet, (sizeof(packet->desc) + packet->length));

  // Enqueue the node packet and trigger the async event
  sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, temp_buffer);
  set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
}

// Weak implementation of the function to process data frames received from the SI91x module
__WEAK sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(buffer);
  return SL_STATUS_OK;
}

/// Thread which handles the notification events.
void si91x_event_handler_thread(const void *args)
{
  UNUSED_PARAMETER(args);
  sl_wifi_event_t wifi_event     = 0;
  uint32_t event                 = 0;
  sl_wifi_buffer_t *buffer       = NULL;
  sl_si91x_packet_t *packet      = NULL;
  sli_si91x_queue_packet_t *data = NULL;
  uint16_t frame_status          = 0;
  const uint32_t event_mask      = (NCP_HOST_WLAN_NOTIFICATION_EVENT | NCP_HOST_NETWORK_NOTIFICATION_EVENT
                               | NCP_HOST_SOCKET_NOTIFICATION_EVENT | NCP_HOST_SOCKET_DATA_NOTIFICATION_EVENT);

  while (1) {
    event = osEventFlagsWait(si91x_async_events, event_mask, osFlagsWaitAny, osWaitForever);
    //    event = si91x_host_wait_for_async_event(event_mask, osWaitForever);

    if ((event & NCP_HOST_WLAN_NOTIFICATION_EVENT) != 0) {
      // Process WLAN notification events
      while (sl_si91x_host_queue_status(&cmd_queues[SI91X_WLAN_CMD].event_queue) != 0) {
        if (sli_si91x_remove_from_queue(&cmd_queues[SI91X_WLAN_CMD].event_queue, &buffer) == SL_STATUS_OK) {
          packet       = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          frame_status = get_si91x_frame_status(packet);

          // Call event handler
          if (si91x_event_handler != NULL) {
            wifi_event = convert_si91x_event_to_sl_wifi_event(packet->command, frame_status);

            if (RSI_WLAN_RSP_SCAN_RESULTS == packet->command) {
              sli_handle_wifi_beacon(packet);
            }

            if (wifi_event != SL_WIFI_INVALID_EVENT) {
              si91x_event_handler(wifi_event, buffer);
            }
          } else {
            // TODO: error handling
          }
          sl_si91x_host_free_buffer(buffer);
        }
      }
    }

    // If firmware sends network events even if the sl_net component is not include
    // (probably firmware issue), Free the resources of the packet to avoid memory leak
    if ((event & NCP_HOST_NETWORK_NOTIFICATION_EVENT) != 0) {
      while (sl_si91x_host_queue_status(&cmd_queues[SI91X_NETWORK_CMD].event_queue) != 0) {
        if (sli_si91x_remove_from_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, &buffer) == SL_STATUS_OK) {
          data   = (sli_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(data->host_packet, 0, NULL);

          SL_NET_EVENT_DISPATCH_HANDLER(data, packet);

          // Free the resources associated with the packet.
          sl_si91x_host_free_buffer(data->host_packet);
          sl_si91x_host_free_buffer(buffer);
        } else {
          // TODO: error handling
        }
      }
    }

    // Process socket notification events
    if ((event & NCP_HOST_SOCKET_NOTIFICATION_EVENT) != 0) {
      while (0 != sl_si91x_host_queue_status(&cmd_queues[SI91X_SOCKET_CMD].event_queue)) {
        if (sli_si91x_remove_from_queue(&cmd_queues[SI91X_SOCKET_CMD].event_queue, &buffer) == SL_STATUS_OK) {
          data   = (sli_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(data->host_packet, 0, NULL);

          SL_NET_EVENT_DISPATCH_HANDLER(data, packet);

          // Free the resources associated with the packet
          sl_si91x_host_free_buffer(data->host_packet);
          sl_si91x_host_free_buffer(buffer);
        } else {
          // TODO: error handling
        }
      }
    }

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
    // Process socket data events
    if (event & NCP_HOST_SOCKET_DATA_NOTIFICATION_EVENT) {
      for (int i = 0; i < NUMBER_OF_SOCKETS; ++i) {
        if (sli_si91x_sockets[i] != NULL) {
          if (sli_si91x_remove_from_queue(&sli_si91x_sockets[i]->rx_data_queue, &buffer) == SL_STATUS_OK) {
            if (sli_si91x_sockets[i]->recv_data_callback != NULL) {
              packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
              sl_si91x_socket_metadata_t *receive_data = (sl_si91x_socket_metadata_t *)packet->data;
              sli_si91x_sockets[i]->recv_data_callback(i,
                                                       &packet->data[receive_data->offset],
                                                       receive_data->length,
                                                       receive_data);
            }
            sl_si91x_host_free_buffer(buffer);
          }
        }
      }
    }
#endif
  }
}

// Thread which handles the TX and RX events.
void si91x_bus_thread(const void *args)
{
  UNUSED_PARAMETER(args);
  sl_status_t status;
  uint16_t temp;
  sli_si91x_queue_packet_t *node       = NULL;
  sli_si91x_queue_packet_t *error_node = NULL;
  sl_wifi_buffer_t *packet;
  sl_wifi_buffer_t *error_packet = NULL;
  sl_wifi_buffer_t *buffer;
  uint8_t tx_queues_empty = 0;
  uint32_t event          = 0;
  uint8_t *data;
  uint16_t length;
  uint8_t queue_id                                         = 0;
  uint16_t frame_type                                      = 0;
  uint16_t frame_status                                    = 0;
  bool global_queue_block                                  = false;
  uint16_t interrupt_status                                = 0;
  sl_wifi_performance_profile_t current_power_profile_mode = { 0 };
  uint32_t bus_wait_time                                   = 0;

  // Array to track the status of commands in flight
  cmd_queues[SI91X_COMMON_CMD].sequential  = true;
  cmd_queues[SI91X_WLAN_CMD].sequential    = true;
  cmd_queues[SI91X_NETWORK_CMD].sequential = true;
  cmd_queues[SI91X_BT_CMD].sequential      = true;
  cmd_queues[SI91X_SOCKET_CMD].sequential  = true;

  while (1) {
    // 0 - give up bus thread
    // 1 - need to process some tx commands/data packets
    tx_queues_empty = (
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
      (tx_socket_data_queues_status
       || (tx_socket_command_queues_status & (~(tx_socket_command_command_in_flight_queues_status))))
      ||
#endif
      (tx_command_queues_status & (~(tx_command_queues_command_in_flight_status)))
      || tx_generic_socket_data_queues_status);
    // TODO: Add checking ALL socket command queues

    // Set wait time:
    // If there might be a buffer to receive, do that immediately,
    // If the last interrupt_status check indicates buffer full, wait up to 10ms,
    // If there is queue not empty or some event to be processed do that immediately,
    // Otherwise there is nothing left to do and thus wait for an event
    if (event & SL_SI91X_NCP_HOST_BUS_RX_EVENT) {
      bus_wait_time = 0;
    } else if (interrupt_status & RSI_BUFFER_FULL) {
      bus_wait_time = 10;
    } else if (tx_queues_empty == 0) {
      bus_wait_time = osWaitForever;
    } else {
      bus_wait_time = 0;
    }

    event |= sli_si91x_wait_for_event(BUS_THREAD_EVENTS, bus_wait_time);

#ifndef SLI_SI91X_MCU_INTERFACE
    // Wake device, if needed
    if ((current_performance_profile != HIGH_PERFORMANCE)) {
      while (si91x_req_wakeup() != SL_STATUS_OK) {
        osDelay(1);
      }
    }

    // Read the interrupt status
    if (sl_si91x_bus_read_interrupt_status(&interrupt_status) != SL_STATUS_OK) {
      continue;
    }

    // Check if there is no RX packet pending or the bus RX event is not set
    if (!((interrupt_status & RSI_RX_PKT_PENDING) || (event & SL_SI91X_NCP_HOST_BUS_RX_EVENT))) {
      if (current_performance_profile != HIGH_PERFORMANCE) {
        // Clear the sleep indicator if the device is not in high-performance mode
        sl_si91x_host_clear_sleep_indicator();
      }
    }
#endif

    // Check if there is an RX packet pending or bus RX event is set
    if ((event & SL_SI91X_NCP_HOST_BUS_RX_EVENT)
#ifndef SLI_SI91X_MCU_INTERFACE
        && (interrupt_status & RSI_RX_PKT_PENDING)
#endif
        && (sl_si91x_bus_read_frame(&buffer) == SL_STATUS_OK)) { // Allocation from RX buffer type!

      if (current_performance_profile != HIGH_PERFORMANCE) {
        sl_si91x_host_clear_sleep_indicator();
      }

      // Check if the rx queue is empty
#ifdef SLI_SI91X_MCU_INTERFACE
      if (sli_si91x_buffer_queue_empty(&sli_ahb_bus_rx_queue)) {
#else
#ifdef SL_NCP_UART_INTERFACE
      if (sli_si91x_buffer_queue_empty(&sli_uart_bus_rx_queue)) {
#else
      {
#endif
#endif
        event &= ~SL_SI91X_NCP_HOST_BUS_RX_EVENT; // Reset the event flag
      }

      data = (uint8_t *)sl_si91x_host_get_buffer_data(buffer, 0, &length);
      // Process the frame
      queue_id     = ((data[1] & 0xF0) >> 4);                // Extract the queue ID
      frame_type   = (uint16_t)(data[2] + (data[3] << 8));   // Extract the frame type
      frame_status = (uint16_t)(data[12] + (data[13] << 8)); // Extract the frame status
#ifdef SLI_SI91X_MCU_INTERFACE
      if ((frame_type == RSI_COMMON_RSP_TA_M4_COMMANDS) || (frame_type == RSI_WLAN_REQ_SET_CERTIFICATE)) {
        // clear flag
        sli_si91x_update_flash_command_status(false);
      }
#endif

      const sl_si91x_packet_t *response = (const sl_si91x_packet_t *)data;
      SL_DEBUG_LOG("><<<< Rx -> queueId : %u, frameId : 0x%x, frameStatus: 0x%x, length : %u\n",
                   queue_id,
                   frame_type,
                   frame_status,
                   (response->length & (~(0xF000))));

      switch (queue_id) {
        case RSI_WLAN_MGMT_Q: {
          // Erase queue ID as it overlays with the length field which is only 24-bit
          data[1] &= 0xF;
          switch (frame_type) {
            // Handle different frame types within the WLAN management queue
            case RSI_COMMON_RSP_OPERMODE:
            case RSI_COMMON_RSP_SOFT_RESET:
            case RSI_COMMON_RSP_PWRMODE: {
              if (frame_type == cmd_queues[SI91X_COMMON_CMD].frame_type) {
                if ((RSI_COMMON_RSP_PWRMODE == frame_type) && (frame_status == SL_STATUS_OK)) {
                  get_wifi_current_performance_profile(&current_power_profile_mode);
                  current_performance_profile = current_power_profile_mode.profile;
                }
                if (current_performance_profile != HIGH_PERFORMANCE) {
                  // Clear the sleep indicator if the device is not in high-performance mode
                  sl_si91x_host_clear_sleep_indicator();
                }
                global_queue_block = false;
              }
            }
              // intentional fallthrough
              __attribute__((fallthrough));
            case RSI_COMMON_RSP_GET_EFUSE_DATA:
            case RSI_COMMON_RSP_GET_RAM_DUMP:
            case RSI_COMMON_RSP_ANTENNA_SELECT:
            case RSI_COMMON_RSP_ENCRYPT_CRYPTO:
#ifdef SLI_PUF_ENABLE
            case RSI_COMMON_RSP_PUF_ENROLL:
            case RSI_COMMON_RSP_PUF_DIS_ENROLL:
            case RSI_COMMON_RSP_PUF_START:
            case RSI_COMMON_RSP_PUF_SET_KEY:
            case RSI_COMMON_RSP_PUF_DIS_SET_KEY:
            case RSI_COMMON_RSP_PUF_GET_KEY:
            case RSI_COMMON_RSP_PUF_DIS_GET_KEY:
            case RSI_COMMON_RSP_PUF_LOAD_KEY:
            case RSI_COMMON_RSP_AES_ENCRYPT:
            case RSI_COMMON_RSP_AES_DECRYPT:
            case RSI_COMMON_RSP_AES_MAC:
            case RSI_COMMON_RSP_PUF_INTR_KEY:
#endif
            case RSI_COMMON_RSP_SET_RTC_TIMER:
            case RSI_COMMON_RSP_GET_RTC_TIMER:
            case RSI_COMMON_RSP_TA_M4_COMMANDS:
            case RSI_COMMON_RSP_SET_CONFIG:
            case RSI_COMMON_RSP_GET_CONFIG:
            case RSI_COMMON_RSP_DEBUG_LOG:
            case RSI_COMMON_RSP_FEATURE_FRAME: {
              ++cmd_queues[SI91X_COMMON_CMD].rx_counter; // Increment the received counter for common commands

              // Check if this command is expected to have a response status
              if (((cmd_queues[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (cmd_queues[SI91X_COMMON_CMD].frame_type == frame_type)) {
                // Allocate a packet to store the response
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Depending on the configuration, attach the original buffer or not
                if (SI91X_PACKET_RESPONSE_PACKET
                    == (cmd_queues[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Populate packet metadata
                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_COMMON_CMD;
                node->sdk_context       = cmd_queues[SI91X_COMMON_CMD].sdk_context;
                node->flags             = cmd_queues[SI91X_COMMON_CMD].flags;
                //                node->packet_id         = cmd_queues[SI91X_COMMON_CMD].packet_id;
                buffer->id = cmd_queues[SI91X_COMMON_CMD].packet_id;
                packet->id = cmd_queues[SI91X_COMMON_CMD].packet_id;

                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_COMMON_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_COMMON_CMD].command_timeout)) {
                  // Add the response packet to the common response queue
                  sli_si91x_append_to_buffer_queue(&cmd_queues[SI91X_COMMON_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet
                  sl_si91x_host_free_buffer(packet);
                  if ((cmd_queues[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)
                      == SI91X_PACKET_RESPONSE_PACKET) {
                    sl_si91x_host_free_buffer(buffer);
                  }
                }
                cmd_queues[SI91X_COMMON_CMD].command_tickcount = 0;
                cmd_queues[SI91X_COMMON_CMD].command_timeout   = 0;
              } else {
                sl_si91x_host_free_buffer(buffer);
              }

              // Marking a received frame as not in flight when it matches the expected type.
              if (frame_type == cmd_queues[SI91X_COMMON_CMD].frame_type) {
                cmd_queues[SI91X_COMMON_CMD].command_in_flight = false;
                cmd_queues[SI91X_COMMON_CMD].frame_type        = 0;
              }
              break;
            }
            case RSI_WLAN_RSP_BAND:
            case RSI_WLAN_RSP_INIT:
            case RSI_WLAN_RSP_RADIO:
            case RSI_WLAN_RSP_EAP_CONFIG:
            case RSI_WLAN_RSP_SET_CERTIFICATE:
            case RSI_WLAN_RSP_HOST_PSK:
            case RSI_WLAN_RSP_JOIN:
            case RSI_WLAN_RSP_SCAN:
            case RSI_WLAN_RSP_SCAN_RESULTS:
            case RSI_WLAN_RSP_FW_VERSION:
            case RSI_WLAN_RSP_FULL_FW_VERSION:
            case RSI_WLAN_RSP_FWUP:
            case RSI_WLAN_RSP_DISCONNECT:
            case RSI_WLAN_RSP_AP_STOP:
            case RSI_WLAN_RSP_RSSI:
            case RSI_WLAN_RSP_TSF:
            case RSI_WLAN_RSP_AP_CONFIGURATION:
            case RSI_WLAN_RSP_WPS_METHOD:
            case RSI_WLAN_RSP_QUERY_NETWORK_PARAMS:
            case RSI_WLAN_RSP_SET_MAC_ADDRESS:
            case RSI_WLAN_RSP_SET_REGION:
            case RSI_WLAN_RSP_SET_REGION_AP:
            case RSI_WLAN_RSP_MAC_ADDRESS:
            case RSI_WLAN_RSP_EXT_STATS:
            case RSI_WLAN_RSP_GET_STATS:
            case RSI_WLAN_RSP_RX_STATS:
            case RSI_WLAN_RSP_MODULE_STATE:
            case RSI_WLAN_RSP_QUERY_GO_PARAMS:
            case RSI_WLAN_RSP_ROAM_PARAMS:
            case RSI_WLAN_RSP_HTTP_OTAF:
            case RSI_WLAN_RSP_CLIENT_CONNECTED:
            case RSI_WLAN_RSP_CLIENT_DISCONNECTED:
            case RSI_WLAN_RSP_CALIB_WRITE:
            case RSI_WLAN_RSP_GET_DPD_DATA:
            case RSI_WLAN_RSP_CALIB_READ:
            case RSI_WLAN_RSP_FREQ_OFFSET:
            case RSI_WLAN_RSP_EVM_OFFSET:
            case RSI_WLAN_RSP_EVM_WRITE:
            case RSI_WLAN_RSP_EFUSE_READ:
            case RSI_WLAN_RSP_FILTER_BCAST_PACKETS:
            case RSI_WLAN_RSP_TWT_PARAMS:
            case RSI_WLAN_RSP_TWT_ASYNC:
            case RSI_WLAN_RSP_TWT_AUTO_CONFIG:
            case RSI_WLAN_RSP_11AX_PARAMS:
            case SL_WIFI_RSP_RESCHEDULE_TWT:
            case RSI_WLAN_RSP_REJOIN_PARAMS:
            case RSI_WLAN_RSP_GAIN_TABLE:
            case RSI_WLAN_RSP_TX_TEST_MODE:
            case RSI_WLAN_RSP_TIMEOUT:
            case RSI_WLAN_RSP_BEACON_STOP:
            case RSI_WLAN_RSP_DYNAMIC_POOL:
            case RSI_WLAN_RSP_TRANSCEIVER_SET_CHANNEL:
            case RSI_WLAN_RSP_TRANSCEIVER_CONFIG_PARAMS:
            case RSI_WLAN_RSP_TRANSCEIVER_PEER_LIST_UPDATE:
            case RSI_WLAN_RSP_TRANSCEIVER_SET_MCAST_FILTER:
            case RSI_WLAN_RSP_TRANSCEIVER_FLUSH_DATA_Q:
            case RSI_WLAN_RSP_TRANSCEIVER_TX_DATA_STATUS:
            case RSI_WLAN_RSP_HT_CAPABILITIES:
            case RSI_WLAN_RSP_SET_MULTICAST_FILTER: {
              ++cmd_queues[SI91X_WLAN_CMD].rx_counter;

              // Marking a received frame as not in flight when it matches the expected type
              if (frame_type == cmd_queues[SI91X_WLAN_CMD].frame_type) {
                cmd_queues[SI91X_WLAN_CMD].command_in_flight = false;
              }

              // Check for the following scenarios:
              // 1. If an asynchronous RX JOIN command fails (frame status not OK).
              // 2. If the station (STA) disconnects from a third-party Access Point (AP).
              // 3. If a third-party station disconnects from the DUT's Access Point (AP).
              // 4. If the DUT's Access Point (AP) stops operating.
              if (((RSI_WLAN_RSP_JOIN == frame_type && cmd_queues[SI91X_WLAN_CMD].frame_type != frame_type)
                   && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_DISCONNECT == frame_type) || (RSI_WLAN_RSP_CLIENT_DISCONNECTED == frame_type)
                  || (RSI_WLAN_RSP_AP_STOP == frame_type)) {

                // create dummy packets for respective queues to be cleared
                sli_handle_dhcp_and_rejoin_failure(cmd_queues[SI91X_WLAN_CMD].sdk_context, buffer, frame_status);
              }
              if (((RSI_WLAN_RSP_JOIN == frame_type) && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_DISCONNECT == frame_type)) {
                // Reset current performance profile and set it to high performance
                reset_coex_current_performance_profile();
                current_performance_profile = HIGH_PERFORMANCE;
              }

              // check if the frame type is valid
              if (((cmd_queues[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (cmd_queues[SI91X_WLAN_CMD].frame_type == frame_type)) {

                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }

                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Check if the packet response mode is set, and associate the host packet accordingly
                if (SI91X_PACKET_RESPONSE_PACKET == (cmd_queues[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Populate the response packet information
                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = cmd_queues[SI91X_WLAN_CMD].sdk_context;
                node->flags             = cmd_queues[SI91X_WLAN_CMD].flags;
                //                node->packet_id         = cmd_queues[SI91X_WLAN_CMD].packet_id;
                packet->id = cmd_queues[SI91X_WLAN_CMD].packet_id;
                buffer->id = cmd_queues[SI91X_WLAN_CMD].packet_id;

                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_WLAN_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_WLAN_CMD].command_timeout)) {
                  // Add the response packet to the WLAN response queue and set the WLAN response event
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet
                  sl_si91x_host_free_buffer(packet);
                  if ((cmd_queues[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)
                      == SI91X_PACKET_RESPONSE_PACKET) {
                    sl_si91x_host_free_buffer(buffer);
                  }
                }
                cmd_queues[SI91X_WLAN_CMD].command_tickcount = 0;
                cmd_queues[SI91X_WLAN_CMD].command_timeout   = 0;
              } else {
                // The received frame does not match the expected response status and frame type,
                // so add it to the WLAN event queue and set the WLAN notification event
                sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].event_queue, buffer);
                set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }

              // Resetting the frame_type in cmd_queues when it matches the expected frame_type
              if (frame_type == cmd_queues[SI91X_WLAN_CMD].frame_type) {
                cmd_queues[SI91X_WLAN_CMD].frame_type = 0;
              }

              break;
            }
            // Handle WLAN response frame type for background scan
            case RSI_WLAN_RSP_BG_SCAN: {
              ++cmd_queues[SI91X_WLAN_CMD].rx_counter;

              // Check if the received frame matches the expected response status and frame type
              if (((cmd_queues[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (cmd_queues[SI91X_WLAN_CMD].frame_type == frame_type)) {
                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = cmd_queues[SI91X_WLAN_CMD].sdk_context;
                node->flags             = cmd_queues[SI91X_WLAN_CMD].flags;
                //                node->packet_id         = cmd_queues[SI91X_WLAN_CMD].packet_id;
                buffer->id        = cmd_queues[SI91X_WLAN_CMD].packet_id;
                packet->id        = cmd_queues[SI91X_WLAN_CMD].packet_id;
                node->host_packet = NULL;

                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_WLAN_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_WLAN_CMD].command_timeout)) {
                  // Add the response packet to the WLAN response queue and set the WLAN response event
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet and buffer
                  sl_si91x_host_free_buffer(packet);
                }
                cmd_queues[SI91X_WLAN_CMD].command_tickcount = 0;
                cmd_queues[SI91X_WLAN_CMD].command_timeout   = 0;
                sl_si91x_host_free_buffer(buffer);
              } else {
                // If frame status is OK, set bg_enabled flag
                if (frame_status == SL_STATUS_OK) {
                  bg_enabled = true;
                }
                // Add the received frame to the WLAN event queue and set the WLAN notification event
                sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].event_queue, buffer);
                set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }
              // Marking a received frame as not in flight when it matches the expected type
              if (frame_type == cmd_queues[SI91X_WLAN_CMD].frame_type) {
                cmd_queues[SI91X_WLAN_CMD].command_in_flight = false;
                cmd_queues[SI91X_WLAN_CMD].frame_type        = 0;
              }
              break;
            }
            case RSI_WLAN_RSP_CONFIG: {
              ++cmd_queues[SI91X_WLAN_CMD].rx_counter;

              // Check if the received frame matches the expected response status and frame type
              if (((cmd_queues[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (cmd_queues[SI91X_WLAN_CMD].frame_type == frame_type)) {
                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Check if the frame status indicates an invalid configuration type
                if ((SL_STATUS_SI91X_INVALID_CONFIG_TYPE & 0xFF) == frame_status) {
                  node->frame_status = 0;
                } else {
                  node->frame_status = frame_status;
                }
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = cmd_queues[SI91X_WLAN_CMD].sdk_context;
                node->flags             = cmd_queues[SI91X_WLAN_CMD].flags;
                //                node->packet_id         = cmd_queues[SI91X_WLAN_CMD].packet_id;
                buffer->id        = cmd_queues[SI91X_WLAN_CMD].packet_id;
                packet->id        = cmd_queues[SI91X_WLAN_CMD].packet_id;
                node->host_packet = NULL;

                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_WLAN_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_WLAN_CMD].command_timeout)) {
                  // Add the response packet to response queue and set the WLAN response event
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet
                  sl_si91x_host_free_buffer(packet);
                }
                cmd_queues[SI91X_WLAN_CMD].command_tickcount = 0;
                cmd_queues[SI91X_WLAN_CMD].command_timeout   = 0;
              }

              // check if the frame type is valid.
              if (frame_type == cmd_queues[SI91X_WLAN_CMD].frame_type) {
                cmd_queues[SI91X_WLAN_CMD].command_in_flight = false;
                cmd_queues[SI91X_WLAN_CMD].frame_type        = 0;
              }

              sl_si91x_host_free_buffer(buffer);
              break;
            }
            case RSI_COMMON_RSP_ULP_NO_RAM_RETENTION: {
              //This frame will come, when the M4 is waken in without ram retention. This frame is equivalent to RSI_COMMON_RSP_CARDREADY
              sl_si91x_host_clear_sleep_indicator();
            }
              // intentional fallthrough
              __attribute__((fallthrough));
            case RSI_COMMON_RSP_CARDREADY: {
              ++cmd_queues[SI91X_COMMON_CMD].rx_counter;

              // Check if the frame type is valid
              if (frame_type == cmd_queues[SI91X_COMMON_CMD].frame_type) {
                // Mark the common command as not in flight
                cmd_queues[SI91X_COMMON_CMD].command_in_flight = false;
                cmd_queues[SI91X_COMMON_CMD].frame_type        = 0;
                cmd_queues[SI91X_COMMON_CMD].command_tickcount = 0;
                cmd_queues[SI91X_COMMON_CMD].command_timeout   = 0;
              }

              sl_si91x_host_free_buffer(buffer);
              sli_si91x_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
              break;
            }
            case RSI_WLAN_RSP_IPCONFV4:
            case RSI_WLAN_RSP_IPCONFV6:
            case RSI_WLAN_RSP_IPV4_CHANGE:
            case RSI_WLAN_RSP_OTA_FWUP:
            case RSI_WLAN_RSP_DNS_QUERY:
            case RSI_WLAN_RSP_DNS_SERVER_ADD:
            case RSI_WLAN_RSP_SET_SNI_EMBEDDED:
            case RSI_WLAN_RSP_MULTICAST:
            case RSI_WLAN_RSP_PING_PACKET:
            case RSI_WLAN_RSP_SNTP_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT:
            case RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE:
            case RSI_WLAN_RSP_MDNSD: {
              // Increment the received frame counter for network commands
              ++cmd_queues[SI91X_NETWORK_CMD].rx_counter;

              // Allocate a buffer for the response packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sli_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Set frame status, queue ID, and command type for the network response
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;

              // Check if the frame type is valid
              if ((frame_type == cmd_queues[SI91X_NETWORK_CMD].frame_type)
                  || (frame_type == RSI_WLAN_RSP_IPCONFV6
                      && cmd_queues[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6)) {
                node->sdk_context = cmd_queues[SI91X_NETWORK_CMD].sdk_context;
                node->flags       = cmd_queues[SI91X_NETWORK_CMD].flags;
                //                node->packet_id   = cmd_queues[SI91X_NETWORK_CMD].packet_id;
                packet->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;
                buffer->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                //                node->packet_id   = 0;
                packet->id = 0;
                buffer->id = 0;
              }
              // Set the host packet to the received buffer
              node->host_packet = buffer;

              if (((frame_type == RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE)
                   || (frame_type == RSI_WLAN_RSP_EMB_MQTT_CLIENT
                       && frame_status == (SL_STATUS_SI91X_MQTT_KEEP_ALIVE_TERMINATE_ERROR & ~BIT(16))))
                  && (cmd_queues[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_RSP_EMB_MQTT_CLIENT)) {
                cmd_queues[SI91X_NETWORK_CMD].command_in_flight = false;
                cmd_queues[SI91X_NETWORK_CMD].frame_type        = 0;

                if (cmd_queues[SI91X_NETWORK_CMD].flags == 0) {
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, packet);
                  set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
                  break;
                }

                // if command present in cmd_queues, an error RX packet is enqueued to network queue and command in flight is set to false.

                // Allocate a buffer for the error packet
                status = sl_si91x_host_allocate_buffer(&error_packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                error_node               = sl_si91x_host_get_buffer_data(error_packet, 0, NULL);
                error_node->frame_status = (SL_STATUS_SI91X_MQTT_REMOTE_TERMINATE_ERROR
                                            & 0xFFFF); // error given by firmware for remote terminate
                error_node->host_packet  = NULL;
                error_node->flags        = cmd_queues[SI91X_NETWORK_CMD].flags;
                //                error_node->packet_id    = cmd_queues[SI91X_NETWORK_CMD].packet_id;
                error_packet->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;

                sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].rx_queue, error_packet);
                sli_si91x_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
                cmd_queues[SI91X_NETWORK_CMD].command_tickcount = 0;
                cmd_queues[SI91X_NETWORK_CMD].command_timeout   = 0;
                break;
              }

              // Check if it's a response packet, and handle accordingly
              if (((cmd_queues[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && ((cmd_queues[SI91X_NETWORK_CMD].frame_type == frame_type)
                      || (cmd_queues[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6
                          && frame_type == RSI_WLAN_RSP_IPCONFV6))) {

                // Check if the response packet should be free or not
                if (SI91X_PACKET_RESPONSE_PACKET
                    != (cmd_queues[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }
                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_NETWORK_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_NETWORK_CMD].command_timeout)) {
                  // Add the response packet to response queue and set the network response event
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet
                  sl_si91x_host_free_buffer(packet);
                  if ((cmd_queues[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)
                      == SI91X_PACKET_RESPONSE_PACKET) {
                    sl_si91x_host_free_buffer(buffer);
                  }
                }
                cmd_queues[SI91X_NETWORK_CMD].command_tickcount = 0;
                cmd_queues[SI91X_NETWORK_CMD].command_timeout   = 0;
              } else {
                // Add the packet to event queue and set the async network event
                sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, packet);
                set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }

              // Check If the frame type is valid.
              if ((frame_type == cmd_queues[SI91X_NETWORK_CMD].frame_type)
                  || (frame_type == RSI_WLAN_RSP_IPCONFV6
                      && cmd_queues[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6)) {
                cmd_queues[SI91X_NETWORK_CMD].command_in_flight = false;
                cmd_queues[SI91X_NETWORK_CMD].frame_type        = 0;
              }

              // Check for the following scenarios:
              // 1. If an IPv4 configuration response (IPCONFV4) fails (frame status not OK).
              // 2. If there is an IPv4 address change event (IPV4_CHANGE).
              if (((RSI_WLAN_RSP_IPCONFV4 == frame_type && frame_type != cmd_queues[SI91X_NETWORK_CMD].frame_type)
                   && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_IPV4_CHANGE == frame_type)) {
                // Reset current performance profile and set it to high performance
                reset_coex_current_performance_profile();
                current_performance_profile = HIGH_PERFORMANCE;
                // check for command in flight and create dummy packets for respective queues to be cleared
                sli_handle_dhcp_and_rejoin_failure(cmd_queues[SI91X_NETWORK_CMD].sdk_context, buffer, frame_status);
                sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, packet);
                set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }
              break;
            }
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
            case RSI_WLAN_RSP_CONN_ESTABLISH:
              frame_type = RSI_WLAN_RSP_SOCKET_ACCEPT;
              // fall-through
            case RSI_WLAN_RSP_SOCKET_CONFIG:
            case RSI_WLAN_RSP_SOCKET_CREATE:
            case RSI_WLAN_RSP_SOCKET_CLOSE:
            case RSI_WLAN_RSP_SELECT_REQUEST:
            case RSI_WLAN_RSP_SOCKET_READ_DATA: // Socket read data response only is expected incase of failure
            case RSI_WLAN_RSP_SOCKET_ACCEPT:
            case RSI_WLAN_RSP_REMOTE_TERMINATE: {
              // Find relevant socket
              sl_si91x_packet_t *socket_packet = (sl_si91x_packet_t *)data;
              sli_si91x_socket_t *socket       = get_socket_from_packet(socket_packet);
              sli_si91x_command_queue_t *socket_command_queue;

              if ((socket == NULL) || (frame_type == RSI_WLAN_RSP_SELECT_REQUEST)
                  || (frame_type == RSI_WLAN_RSP_SOCKET_CREATE) || (frame_type == RSI_WLAN_RSP_SOCKET_CONFIG)) {
                socket_command_queue = &cmd_queues[SI91X_SOCKET_CMD];
              } else {
                socket_command_queue = &socket->command_queue;
              }

              if (frame_type == RSI_WLAN_RSP_SELECT_REQUEST) {
                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sli_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Set frame status, queue ID, and command type for the network response
                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_SOCKET_CMD;
                node->host_packet       = buffer;
                node->sdk_context       = NULL;
                // and set asynchronous socket notification event
                sli_si91x_add_to_queue(&cmd_queues[SI91X_SOCKET_CMD].event_queue, packet);
                set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
                break;
              }
              ++socket_command_queue->rx_counter;

              if ((socket_command_queue->command_timeout != 0)
                  && (sl_si91x_host_elapsed_time(socket_command_queue->command_tickcount)
                      > (socket_command_queue->command_timeout))) {
                sl_si91x_host_free_buffer(buffer);
                socket_command_queue->command_tickcount = 0;
                socket_command_queue->command_timeout   = 0;
                break;
              }

              if (frame_type == RSI_WLAN_RSP_SOCKET_READ_DATA && socket_command_queue->frame_type == frame_type) {
                buffer->id                              = (uint8_t)(socket->command_queue.packet_id);
                socket_command_queue->command_in_flight = false;
                socket_command_queue->frame_type        = 0;
                sli_si91x_add_to_queue(&socket->rx_data_queue, buffer);
                sli_si91x_set_socket_event(1 << socket->index);
                break;
              }

              // Allocate a buffer for the response packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sli_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Set frame status, queue ID, and command type for the socket response
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_SOCKET_CMD;
              node->host_packet       = buffer;

              // Check if the frame type is valid
              if (socket_command_queue->frame_type == frame_type) {
                node->sdk_context = socket_command_queue->sdk_context;
                node->flags       = socket_command_queue->flags;
                //                node->packet_id   = socket_command_queue->packet_id;
                buffer->id = socket_command_queue->packet_id;
                packet->id = socket_command_queue->packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                //                node->packet_id   = 0;
                buffer->id = 0;
                packet->id = 0;
              }

              // Check if it's a response packet, and handle accordingly
              if ((socket_command_queue->flags & SI91X_PACKET_RESPONSE_STATUS)
                  && (socket_command_queue->frame_type == frame_type)) {

                // Check if the response packet should be free or not
                if ((socket_command_queue->flags & SI91X_PACKET_RESPONSE_PACKET) == 0) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Add the response packet to the socket response queue and set the socket response event
                sli_si91x_add_to_queue(&socket_command_queue->rx_queue, packet);

                if (socket_command_queue == &cmd_queues[SI91X_SOCKET_CMD]) {
                  sli_si91x_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);
                } else {
                  sli_si91x_set_socket_event(1 << socket->index);
                }
                socket_command_queue->command_tickcount = 0;
                socket_command_queue->command_timeout   = 0;

              } else {
                // and set asynchronous socket notification event
                sli_si91x_add_to_queue(&cmd_queues[SI91X_SOCKET_CMD].event_queue, packet);
                set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
              }

              // If the frame_type matches the expected frame_type for the socket command
              // mark the command as not in flight and clear the frame_type
              if (frame_type == socket_command_queue->frame_type) {
                socket_command_queue->command_in_flight = false;
                socket_command_queue->frame_type        = 0;
              }

              break;
            }
#endif

            // Handle TCP ACK Indication response
            case RSI_WLAN_RSP_TCP_ACK_INDICATION: {
              // Allocate memory for a new packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sli_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Populate the packet's information
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_SOCKET_CMD;
              node->host_packet       = buffer;

              node->sdk_context = NULL;
              node->flags       = 0;
              //              node->packet_id   = 0;
              buffer->id = 0;
              packet->id = 0;

              // Add the packet to the socket event queue and set the asynchronous socket notification event
              sli_si91x_add_to_queue(&cmd_queues[SI91X_SOCKET_CMD].event_queue, packet);
              set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
              break;
            }
#ifdef SLI_SI91X_INTERNAL_HTTP_CLIENT
            case RSI_WLAN_RSP_HTTP_CLIENT_GET:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
              ++cmd_queues[SI91X_NETWORK_CMD].rx_counter;

              if (cmd_queues[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_RSP_HTTP_CLIENT_GET) {
                // If it's an HTTP GET response, check if the frame_status is not OK or if end_of_data is set to 1.
                sl_si91x_packet_t *get_response_packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
                const uint16_t *end_of_data            = (uint16_t *)&get_response_packet->data;

                if (frame_status != SL_STATUS_OK || *end_of_data == 1) {
                  // Mark the command as not in flight and clear the frame_type
                  cmd_queues[SI91X_NETWORK_CMD].command_in_flight = false;
                  cmd_queues[SI91X_NETWORK_CMD].frame_type        = 0;
                }
              } else {
                // For other HTTP responses, mark the command as not in flight and clear the frame_type
                cmd_queues[SI91X_NETWORK_CMD].command_in_flight = false;
                cmd_queues[SI91X_NETWORK_CMD].frame_type        = 0;
              }

              // Allocate memory for a new packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sli_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Populate the packet's information
              node->host_packet       = buffer;
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;
              node->sdk_context       = cmd_queues[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = cmd_queues[SI91X_NETWORK_CMD].flags;
              //              node->packet_id         = cmd_queues[SI91X_NETWORK_CMD].packet_id;
              buffer->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;
              packet->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;

              // Add the packet to the network event queue and set the asynchronous network notification event
              sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, packet);
              set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              break;
            }
            case RSI_WLAN_RSP_HTTP_ABORT:
            case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
              ++cmd_queues[SI91X_NETWORK_CMD].rx_counter;

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sli_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Populate the packet's information
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;
              node->sdk_context       = cmd_queues[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = cmd_queues[SI91X_NETWORK_CMD].flags;
              //    node->packet_id         = command_trace[SI91X_NETWORK_CMD].packet_id;
              buffer->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;
              packet->id = cmd_queues[SI91X_NETWORK_CMD].packet_id;

              if (((cmd_queues[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (cmd_queues[SI91X_NETWORK_CMD].frame_type == frame_type)) {
                // If it's a response status and the frame_type matches, set host_packet to NULL and free the buffer
                node->host_packet = NULL;
                sl_si91x_host_free_buffer(buffer);

                if (sl_si91x_host_elapsed_time(cmd_queues[SI91X_NETWORK_CMD].command_tickcount)
                    <= (cmd_queues[SI91X_NETWORK_CMD].command_timeout)) {
                  // Add the packet to the network response queue and set the network response event
                  sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].rx_queue, packet);
                  sli_si91x_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
                } else {
                  // no user thread is waiting for the response so flush the packet and buffer
                  sl_si91x_host_free_buffer(packet);
                  if ((cmd_queues[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                      != SI91X_PACKET_RESPONSE_STATUS) {
                    sl_si91x_host_free_buffer(buffer);
                  }
                }
              } else {
                // For other cases, set host_packet to buffer and add it to the network event queue
                node->host_packet = buffer;

                sli_si91x_add_to_queue(&cmd_queues[SI91X_NETWORK_CMD].event_queue, packet);
                set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }

              if (frame_type == cmd_queues[SI91X_NETWORK_CMD].frame_type) {
                // mark the command as not in flight and clear the frame_type
                cmd_queues[SI91X_NETWORK_CMD].command_in_flight = false;
                cmd_queues[SI91X_NETWORK_CMD].frame_type        = 0;
                cmd_queues[SI91X_NETWORK_CMD].command_tickcount = 0;
                cmd_queues[SI91X_NETWORK_CMD].command_timeout   = 0;
              }
              break;
            }
#endif
            default: {
              // frame_type doesn't match any known cases
              if (PRINT_ERROR_LOGS) {
                PRINT_ERROR_STATUS(INFO_TAG, frame_type);
              }
              // Free the buffer
              sl_si91x_host_free_buffer(buffer);
              break;
            }
          }
          break;
        }

        case RSI_WLAN_DATA_Q: {
          // Erase queue ID as it overlays with the length field which is only 24-bit
          data[1] &= 0xF;
          if (frame_type == RSI_RECEIVE_RAW_DATA) {
            // If the frame type is raw data reception

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
            SL_DEBUG_LOG("Raw Data\n");
            sl_si91x_packet_t *socket_packet = (sl_si91x_packet_t *)data;
            sli_si91x_socket_t *socket       = NULL;
            int socket_id                    = sli_si91x_get_socket_id(socket_packet);
            socket                           = sli_si91x_get_socket_from_id(socket_id, LISTEN, -1);

            // Check if we found a matching socket
            if (socket != NULL) {
              buffer->id = (uint8_t)(socket->command_queue.packet_id);
              // Check if command has timed out
              if (socket->command_queue.command_tickcount == 0
                  || (sl_si91x_host_elapsed_time(socket->command_queue.command_tickcount)
                      <= (socket->command_queue.command_timeout))) {
                if (((socket->command_queue.frame_type == RSI_WLAN_RSP_SOCKET_READ_DATA)
                     || (socket->command_queue.frame_type == socket_packet->command))
                    && socket->command_queue.command_in_flight
                    && socket->command_queue.flags & SI91X_PACKET_RESPONSE_PACKET) {
                  socket->command_queue.command_in_flight = false;
                  sli_si91x_add_to_queue(&socket->rx_data_queue, buffer);
                  sli_si91x_set_socket_event(1 << socket->index);
                } else {
                  sli_si91x_add_to_queue(&socket->rx_data_queue, buffer);
                  set_async_event(NCP_HOST_SOCKET_DATA_NOTIFICATION_EVENT);
                }
              } else {
                sli_si91x_add_to_queue(&socket->rx_data_queue, buffer);
                set_async_event(NCP_HOST_SOCKET_DATA_NOTIFICATION_EVENT);
                //                sl_si91x_host_free_buffer(buffer);
              }
              socket->command_queue.command_tickcount = 0;
              socket->command_queue.command_timeout   = 0;
            }
#else
            // If SLI_SI91X_OFFLOAD_NETWORK_STACK is not defined, process the data frame and free the buffer.
            sl_si91x_host_process_data_frame(SL_WIFI_CLIENT_INTERFACE, buffer);
            sl_si91x_host_free_buffer(buffer);
#endif
          } else if (frame_type == RSI_NET_DUAL_STACK_RX_RAW_DATA_FRAME) {

            // If network dual stack mode is enabled, process the received data frame of type 0x1 and free the buffer.
            sl_si91x_host_process_data_frame(SL_WIFI_CLIENT_INTERFACE, buffer);
            sl_si91x_host_free_buffer(buffer);
          } else if (frame_type == SL_SI91X_WIFI_RX_DOT11_DATA) {
            ++cmd_queues[SI91X_WLAN_CMD].rx_counter;

            // Marking a received frame as not in flight when it matches the expected type
            if (frame_type == cmd_queues[SI91X_WLAN_CMD].frame_type) {
              cmd_queues[SI91X_WLAN_CMD].command_in_flight = false;
              cmd_queues[SI91X_WLAN_CMD].command_tickcount = 0;
              cmd_queues[SI91X_WLAN_CMD].command_timeout   = 0;
            }

            // Add it to the WLAN event queue and set the WLAN notification event
            sli_si91x_add_to_queue(&cmd_queues[SI91X_WLAN_CMD].event_queue, buffer);
            set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
          }
          break;
        }

#ifdef SLI_SI91X_ENABLE_BLE
        case RSI_BT_Q: {
          SL_DEBUG_LOG("Received BLE packet\n");
          // Increment the receive counter for the Bluetooth command
          ++cmd_queues[SI91X_BT_CMD].rx_counter;

          if (frame_type == cmd_queues[SI91X_BT_CMD].frame_type) {
            // Mark the command as not in flight and clear the frame_type
            cmd_queues[SI91X_BT_CMD].command_in_flight = false;
            cmd_queues[SI91X_BT_CMD].frame_type        = 0;
          }

          // Process the Bluetooth response data and free the buffer
          rsi_driver_process_bt_resp_handler(data);
          sl_si91x_host_free_buffer(buffer);
          break;
        }
#endif
        default: {
          // If the frame_type doesn't match any known cases
          // Free the buffer
          sl_si91x_host_free_buffer(buffer);
          break;
        }
      }
      sli_submit_rx_buffer();
    } else {
      event &= ~SL_SI91X_NCP_HOST_BUS_RX_EVENT; // Reset the event flag
    }

    if (event & SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS) {
      // This condition is checked before writing frames to the bus
      for (int i = 0; i < SI91X_CMD_MAX; i++) {
        if (!(event & (SL_SI91X_TX_PENDING_FLAG(i)))) {
          continue;
        }
        if (cmd_queues[i].command_in_flight == true) {
          tx_command_queues_command_in_flight_status |= SL_SI91X_TX_PENDING_FLAG(i);
          continue;
        } else {
          tx_command_queues_command_in_flight_status &= ~SL_SI91X_TX_PENDING_FLAG(i);
        }
        // Check if the bus is ready for a packet
        if (!sli_si91x_is_bus_ready(global_queue_block)) {
          break;
        }

        bus_write_frame(&cmd_queues[i], i, SL_WIFI_CONTROL_BUFFER, &global_queue_block);
        if (sli_si91x_buffer_queue_empty(&cmd_queues[i].tx_queue)) {
          event &= ~SL_SI91X_TX_PENDING_FLAG(i);
          tx_command_queues_status &= ~SL_SI91X_TX_PENDING_FLAG(i);
        }
      }
    }

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
    if (tx_socket_command_queues_status & (~(tx_command_queues_command_in_flight_status))) {
      for (int i = 0; i < NUMBER_OF_SOCKETS; i++) {
        if (sli_si91x_sockets[i] != NULL
            && !sli_si91x_buffer_queue_empty(&sli_si91x_sockets[i]->command_queue.tx_queue)) {
          if (sli_si91x_sockets[i]->command_queue.command_in_flight == true) {
            tx_socket_command_command_in_flight_queues_status |= (1 << i);
            continue;
          } else {
            tx_socket_command_command_in_flight_queues_status &= ~(1 << i);
          }
          // Check if the bus is ready for a packet
          if (!sli_si91x_is_bus_ready(global_queue_block)) {
            break;
          }

          // Send the socket specific command
          bus_write_frame(&sli_si91x_sockets[i]->command_queue,
                          SI91X_SOCKET_CMD,
                          SL_WIFI_CONTROL_BUFFER,
                          &global_queue_block);
          if (sli_si91x_buffer_queue_empty(&sli_si91x_sockets[i]->command_queue.tx_queue)) {
            tx_socket_command_queues_status &= ~(1 << i);
          }
        }
      }
      // Clear the event flag for socket queues if there are no commands left
      if (tx_socket_command_queues_status == 0) {
        event &= ~SL_SI91X_SOCKET_COMMAND_TX_PENDING_EVENT;
      }
    }

    if (event & SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT) {
      bool all_socket_data_sent = true;
      // Check each socket if it has something to send
      for (int i = 0; i < NUMBER_OF_SOCKETS; ++i) {
        if (sli_si91x_sockets[i] != NULL && !sli_si91x_buffer_queue_empty(&sli_si91x_sockets[i]->tx_data_queue)) {
          all_socket_data_sent = false;

          // Check if the bus is ready for a packet
          if (!sli_si91x_is_bus_ready(global_queue_block)) {
            break;
          }

          sl_status_t status = bus_write_data_frame(&sli_si91x_sockets[i]->tx_data_queue);
          if (status == SL_STATUS_OK) {
            --sli_si91x_sockets[i]->data_buffer_count;
          }
          if (sli_si91x_buffer_queue_empty(&sli_si91x_sockets[i]->tx_data_queue)) {
            tx_socket_data_queues_status &= ~(1 << i);
          }
        }
      }

      // Clear event bit if we confirmed no more packets to send
      if (all_socket_data_sent) {
        event &= ~SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT;
      }
    }
#endif
    if (event & SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT) {
      // Check if the bus is ready for a packet
      sl_si91x_bus_read_interrupt_status(&interrupt_status);
      if (!(interrupt_status & RSI_BUFFER_FULL)) {
        bus_write_data_frame(&sli_tx_data_queue);
        if (sli_si91x_buffer_queue_empty(&sli_tx_data_queue)) {
          event &= ~SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT;
          tx_generic_socket_data_queues_status &= ~(SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT);
        }
      }
#ifdef SLI_SI91X_MCU_INTERFACE
      else {
        unmask_ta_interrupt(TA_RSI_BUFFER_FULL_CLEAR_EVENT);
      }
#endif
    }
    if (event & SL_SI91X_TERMINATE_BUS_THREAD_EVENT) {
      // Clear the termination event flag
      event &= ~SL_SI91X_TERMINATE_BUS_THREAD_EVENT;

      // Acknowledge the termination request
      osEventFlagsSet(si91x_events, SL_SI91X_TERMINATE_BUS_THREAD_EVENT_ACK);

      // Terminate the current thread
      osThreadTerminate(osThreadGetId());
    }
  }
  //To suppress warning unused parameter, no code effect
  UNUSED_PARAMETER(event);
}

static sl_status_t bus_write_frame(sli_si91x_command_queue_t *queue,
                                   sl_si91x_command_type_t command_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   bool *global_queue_block)
{
  UNUSED_PARAMETER(buffer_type);
  UNUSED_PARAMETER(command_type);
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sli_si91x_queue_packet_t *node = NULL;

  if ((current_performance_profile != HIGH_PERFORMANCE) && (si91x_req_wakeup() != SL_STATUS_OK)) {
    return SL_STATUS_TIMEOUT;
  }

  status = sli_si91x_remove_from_queue(&queue->tx_queue, &buffer);
  if (status != SL_STATUS_OK) {
    if (current_performance_profile != HIGH_PERFORMANCE) {
      sl_si91x_host_clear_sleep_indicator();
    }
    VERIFY_STATUS_AND_RETURN(status);
  }

  node            = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  packet          = sl_si91x_host_get_buffer_data(node->host_packet, 0, NULL);
  uint16_t length = packet->length;

  // Modify the packet's descriptor to include the firmware queue ID in the length field
  packet->desc[1] |= (node->firmware_queue_id << 4);

  if (packet->command) {
    // Set the global_queue_block flag if it is present in the packet's flags
    if (SI91X_PACKET_GLOBAL_QUEUE_BLOCK & node->flags) {
      *global_queue_block = true;
    }

    if (SI91X_PACKET_WITH_ASYNC_RESPONSE != (node->flags & SI91X_PACKET_WITH_ASYNC_RESPONSE)) {
      // Update trace information with packet details
      // If the packet doesn't have an async response, mark the command as in flight
      queue->command_in_flight = true;
      queue->packet_id         = node->host_packet->id;
      queue->firmware_queue_id = node->firmware_queue_id;
      queue->frame_type        = packet->command;
      queue->flags             = node->flags;
      queue->command_timeout   = node->command_timeout;
      queue->command_tickcount = node->command_tickcount;
      queue->sdk_context       = node->sdk_context;
    }
  }
#ifdef SLI_SI91X_MCU_INTERFACE
  if ((packet->command == RSI_COMMON_RSP_TA_M4_COMMANDS) || (packet->command == RSI_WLAN_REQ_SET_CERTIFICATE)) {
    // set flag
    sli_si91x_update_flash_command_status(true);
  }
#endif
  // Write the frame to the bus using packet data and length
  status = sl_si91x_bus_write_frame(packet, packet->data, length);

#ifdef SLI_SI91X_MCU_INTERFACE
  if (packet->desc[2] == RSI_COMMON_REQ_SOFT_RESET) {
    sli_si91x_config_m4_dma_desc_on_reset();
  }
#endif

  // Handle errors during frame writing
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n BUS_WRITE_ERROR \r\n");
    BREAKPOINT();
  }

  SL_DEBUG_LOG("<>>>> Tx -> queueId : %u, frameId : 0x%x, length : %u\n",
               node->firmware_queue_id,
               packet->command,
               length);

  if (current_performance_profile != HIGH_PERFORMANCE) {
    sl_si91x_host_clear_sleep_indicator();
  }

#ifdef SLI_SI91X_ENABLE_BLE
  if (command_type == SI91X_BT_CMD) {
    rsi_bt_common_tx_done(packet);
  }
#endif

  sl_si91x_host_free_buffer(node->host_packet);
  sl_si91x_host_free_buffer(buffer);

  queue->tx_counter++;
  return SL_STATUS_OK;
}

// This function is called for writing data
static sl_status_t bus_write_data_frame(sl_si91x_buffer_queue_t *queue)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if ((current_performance_profile != HIGH_PERFORMANCE) && (si91x_req_wakeup() != SL_STATUS_OK)) {
    return SL_STATUS_TIMEOUT;
  }

  status = sli_si91x_remove_from_queue(queue, &buffer);
  if (status != SL_STATUS_OK) {
    if (current_performance_profile != HIGH_PERFORMANCE) {
      sl_si91x_host_clear_sleep_indicator();
    }
    VERIFY_STATUS_AND_RETURN(status);
  }

  packet          = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  uint16_t length = packet->length;

  // Modify the packet's descriptor to include the firmware queue ID in the length field
  packet->desc[1] |= (5 << 4);

  // Write the frame to the bus using packet data and length
  status = sl_si91x_bus_write_frame(packet, packet->data, length);

  // Handle errors during frame writing
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n BUS_WRITE_ERROR \r\n");
    BREAKPOINT();
  }

  SL_DEBUG_LOG("<>>>> Tx -> queueId : %u, frameId : 0x%x, length : %u\n", 5, 0, length);

  if (current_performance_profile != HIGH_PERFORMANCE) {
    sl_si91x_host_clear_sleep_indicator();
  }

  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

static void set_async_event(uint32_t event_mask)
{
  osEventFlagsSet(si91x_async_events, event_mask);
}

// Function to check if the bus is ready for writing
bool sli_si91x_is_bus_ready(bool global_queue_block)
{
  if (global_queue_block) {
    return false;
  }
  uint16_t interrupt_status = 0;
  sl_si91x_bus_read_interrupt_status(&interrupt_status);
  if (interrupt_status & RSI_BUFFER_FULL) {
#ifdef SLI_SI91X_MCU_INTERFACE
    unmask_ta_interrupt(TA_RSI_BUFFER_FULL_CLEAR_EVENT);
#endif
    return false;
  }
  return true;
}

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
static sli_si91x_socket_t *get_socket_from_packet(sl_si91x_packet_t *socket_packet)
{
  int socket_id = sli_si91x_get_socket_id(socket_packet);

  if (socket_packet->command == RSI_WLAN_RSP_CONN_ESTABLISH) {
    socket_packet->command = RSI_WLAN_RSP_SOCKET_ACCEPT;
  }

  if (socket_packet->command == RSI_WLAN_RSP_SOCKET_CREATE) {
    sl_si91x_socket_create_response_t *socket_create_response =
      ((sl_si91x_socket_create_response_t *)socket_packet->data);
    return sli_si91x_get_socket_from_id(
      -1,
      RESET,
      (int16_t)(socket_create_response->socket_type[0] | (socket_create_response->socket_type[1] << 8)));
  } else if (socket_packet->command == RSI_WLAN_RSP_SOCKET_ACCEPT) {
    const sli_si91x_socket_t *si91x_socket = sli_si91x_get_socket_from_id(socket_id, RESET, -1);
    return get_si91x_socket(si91x_socket->client_id);
  } else if (socket_packet->command == RSI_WLAN_RSP_SOCKET_CLOSE) {
    return sli_si91x_get_socket_from_id(socket_id, RESET, -1);
  } else {
    return sli_si91x_get_socket_from_id(socket_id, LISTEN, -1);
  }
}
#endif
