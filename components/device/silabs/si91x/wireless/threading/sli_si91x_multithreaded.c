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
#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
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

#define BUS_THREAD_EVENTS \
  (SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS | SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT | SL_SI91X_NCP_HOST_BUS_RX_EVENT)

/******************************************************
 *               Function Declarations
 ******************************************************/

void si91x_event_handler_thread(void *args);
/*
  Structure to track the status and info of all the commands in flight 
  for a particular command of type sl_si91x_command_type_t
*/
typedef struct {
  const bool sequential;     // Indicates if the commands are sequential
  bool command_in_flight;    // Indicates if a command is currently being processed
  uint16_t frame_type;       // Type of the frame associated with the command
  uint8_t firmware_queue_id; // ID of the firmware queue for the command
  uint32_t rx_counter;       // Counter for received packets
  uint32_t tx_counter;       // Counter for transmitted packets
  uint16_t packet_id;        // ID of the packet associated with the command
  uint8_t flags;             // Flags associated with the command
  void *sdk_context;         // Context data associated with the command
  int32_t
    sl_si91x_socket_id; // socket_id, used only for SI91X_SOCKET_CMD queue to update socket_id in command trace of bus thread.
} sl_si91x_command_trace_t;

// Declaration of a global flag to indicate if background mode is enabled
extern bool bg_enabled;

// Declaration of the SI91x event handler function
extern sl_wifi_event_handler_t si91x_event_handler;

extern si91x_packet_queue_t cmd_queues[SI91X_QUEUE_MAX];

#ifndef SLI_SI91X_MCU_INTERFACE
// Declaration of a semaphore handle used for command locking
extern osSemaphoreId_t cmd_lock;
#endif
/******************************************************
 *               Function Declarations
 ******************************************************/

void si91x_event_handler_thread(void *args);

extern sl_status_t sl_create_generic_rx_packet_from_params(sl_si91x_queue_packet_t **queue_packet,
                                                           sl_wifi_buffer_t **packet_buffer,
                                                           uint16_t packet_id,
                                                           uint8_t flags,
                                                           void *sdk_context,
                                                           sl_si91x_command_type_t command_type);

void sli_submit_rx_buffer(void);
void si91x_bus_thread(void *args);
void handle_dhcp_and_rejoin_failure(sl_si91x_queue_packet_t *node,
                                    sl_wifi_buffer_t *temp_buffer,
                                    sl_si91x_command_trace_t *command_trace);
void si91x_event_handler_thread(void *args);
#ifdef SLI_SI91X_MCU_INTERFACE
// External declaration of a function to configure M4 DMA descriptors on reset
extern void sli_si91x_config_m4_dma_desc_on_reset(void);
#endif
static sl_status_t bus_write_frame(sl_si91x_queue_type_t queue_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   sl_si91x_command_trace_t *trace,
                                   bool *global_queue_block);

static sl_status_t bus_write_data_frame(sl_si91x_queue_type_t queue_type,
                                        sl_wifi_buffer_type_t buffer_type,
                                        bool *global_queue_block);

sl_status_t si91x_req_wakeup(void);

sl_status_t sl_create_generic_rx_packet_from_params(sl_si91x_queue_packet_t **queue_packet,
                                                    sl_wifi_buffer_t **packet_buffer,
                                                    uint16_t packet_id,
                                                    uint8_t flags,
                                                    void *sdk_context,
                                                    sl_si91x_command_type_t command_type)
{
  sl_si91x_queue_packet_t *packet = NULL;
  sl_wifi_buffer_t *buffer        = NULL;
  sl_status_t status              = SL_STATUS_OK;
  uint16_t temp                   = 0;

  status = sl_si91x_host_allocate_buffer(&buffer,
                                         SL_WIFI_RX_FRAME_BUFFER,
                                         sizeof(sl_si91x_queue_packet_t),
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
  packet->packet_id         = packet_id;
  packet->sdk_context       = sdk_context;
  packet->flags             = flags;
  packet->frame_status      = SL_STATUS_FAIL;
  packet->command_type      = command_type;
  packet->firmware_queue_id = RSI_WLAN_MGMT_Q;

  *queue_packet = packet;
  return SL_STATUS_OK;
}

void handle_dhcp_and_rejoin_failure(sl_si91x_queue_packet_t *node,
                                    sl_wifi_buffer_t *temp_buffer,
                                    sl_si91x_command_trace_t *command_trace)
{
  sl_status_t status      = SL_STATUS_OK;
  uint32_t response_event = 0;
  uint32_t response_queue = 0;

  for (int queue_id = 0; queue_id < SI91X_BT_CMD; queue_id++) {
    if (command_trace[queue_id].command_in_flight != true) {
      continue;
    }

    status = sl_create_generic_rx_packet_from_params(&node,
                                                     &temp_buffer,
                                                     command_trace[queue_id].packet_id,
                                                     command_trace[queue_id].flags,
                                                     command_trace[queue_id].sdk_context,
                                                     queue_id);
    if (status != SL_STATUS_OK) {
      break;
    }

    // reset command trace
    command_trace[queue_id].command_in_flight = false;
    command_trace[queue_id].frame_type        = 0;

    if (queue_id == SI91X_COMMON_CMD) {
      response_event = NCP_HOST_COMMON_RESPONSE_EVENT;
      response_queue = SI91X_COMMON_RESPONSE_QUEUE;
    } else if (queue_id == SI91X_WLAN_CMD) {
      response_event = NCP_HOST_WLAN_RESPONSE_EVENT;
      response_queue = SI91X_WLAN_RESPONSE_QUEUE;
    } else if (queue_id == SI91X_NETWORK_CMD) {
      response_event = NCP_HOST_NETWORK_RESPONSE_EVENT;
      response_queue = SI91X_NETWORK_RESPONSE_QUEUE;
    } else if (queue_id == SI91X_SOCKET_CMD) {
      response_event = NCP_HOST_SOCKET_RESPONSE_EVENT;
      response_queue = SI91X_SOCKET_RESPONSE_QUEUE;
    }

    sl_si91x_host_add_to_queue(response_queue, temp_buffer);
    sl_si91x_host_set_event(response_event); // TODO: additonal checks for async queue, async event
  }

  sl_wifi_buffer_t *mqtt_remote_terminate_packet_buffer = NULL;
  sl_si91x_packet_t *mqtt_remote_terminate_packet       = NULL;

  // generate dummy sl_si91x_queue_packet_t for MQTT remote terminate.
  status = sl_create_generic_rx_packet_from_params(&node, &temp_buffer, 0, 0, NULL, SI91X_NETWORK_CMD);
  if (status != SL_STATUS_OK) {
    return;
  }

  // generate dummy MQTT remote terminate packet.
  status = sl_si91x_host_allocate_buffer(&mqtt_remote_terminate_packet_buffer,
                                         SL_WIFI_RX_FRAME_BUFFER,
                                         sizeof(sl_si91x_packet_t),
                                         SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  if (status != SL_STATUS_OK) {
    sl_si91x_host_free_buffer(temp_buffer);
    return;
  }

  mqtt_remote_terminate_packet = sl_si91x_host_get_buffer_data(mqtt_remote_terminate_packet_buffer, 0, NULL);

  memset(mqtt_remote_terminate_packet->desc, 0, sizeof(mqtt_remote_terminate_packet->desc));

  node->frame_status = SL_STATUS_OK;
  node->host_packet  = mqtt_remote_terminate_packet_buffer;

  mqtt_remote_terminate_packet->command = RSI_WLAN_RSP_JOIN;

  // Enqueue the packet and set the async event.
  sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, temp_buffer);
  sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
}

// Weak implementation of the function to process data frames received from the SI91x module
__WEAK sl_status_t sl_si91x_host_process_data_frame(sl_wifi_interface_t interface, sl_wifi_buffer_t *buffer)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(buffer);
  return SL_STATUS_OK;
}

/// Thread which handles the notification events.
void si91x_event_handler_thread(void *args)
{
  UNUSED_PARAMETER(args);
  sl_wifi_event_t wifi_event    = 0;
  uint32_t event                = 0;
  sl_wifi_buffer_t *buffer      = NULL;
  sl_si91x_packet_t *packet     = NULL;
  sl_si91x_queue_packet_t *data = NULL;
  uint16_t frame_status         = 0;
  const uint32_t event_mask =
    (NCP_HOST_WLAN_NOTIFICATION_EVENT | NCP_HOST_NETWORK_NOTIFICATION_EVENT | NCP_HOST_SOCKET_NOTIFICATION_EVENT);

  while (1) {
    event = si91x_host_wait_for_async_event(event_mask, osWaitForever);

    if ((event & NCP_HOST_WLAN_NOTIFICATION_EVENT) != 0) {
      // Process WLAN notification events
      while (sl_si91x_host_queue_status(SI91X_WLAN_EVENT_QUEUE) != 0) {
        if (sl_si91x_host_remove_from_queue(SI91X_WLAN_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          packet       = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          frame_status = get_si91x_frame_status(packet);

          // Call event handler
          if (si91x_event_handler != NULL) {
            wifi_event = convert_si91x_event_to_sl_wifi_event(packet->command, frame_status);
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
      while (sl_si91x_host_queue_status(SI91X_NETWORK_EVENT_QUEUE) != 0) {
        if (sl_si91x_host_remove_from_queue(SI91X_NETWORK_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          data   = (sl_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
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
      while (0 != sl_si91x_host_queue_status(SI91X_SOCKET_EVENT_QUEUE)) {
        if (sl_si91x_host_remove_from_queue(SI91X_SOCKET_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          data   = (sl_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
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
  }
}

// Thread which handles the TX and RX events.
void si91x_bus_thread(void *args)
{
  UNUSED_PARAMETER(args);
  sl_status_t status;
  uint16_t interrupt_status;
  uint16_t temp;
  sl_si91x_queue_packet_t *node = NULL, *error_node = NULL;
  sl_wifi_buffer_t *packet, *error_packet           = NULL;
  sl_wifi_buffer_t *temp_buffer = NULL;
  sl_wifi_buffer_t *buffer;
  uint8_t tx_queues_empty = 0;
  uint32_t event          = 0;
  uint8_t *data;
  uint16_t length;
  uint8_t queue_id                                         = 0;
  uint16_t frame_type                                      = 0;
  uint16_t frame_status                                    = 0;
  bool global_queue_block                                  = false;
  sl_wifi_performance_profile_t current_power_profile_mode = { 0 };
  int i;

  // Array to track the status of commands in flight
  sl_si91x_command_trace_t command_trace[SI91X_CMD_MAX] = {
    [SI91X_COMMON_CMD] = { .sequential = true },  [SI91X_WLAN_CMD] = { .sequential = true },
    [SI91X_NETWORK_CMD] = { .sequential = true }, [SI91X_SOCKET_CMD] = { .sequential = true },
    [SI91X_BT_CMD] = { .sequential = true },      [SI91X_SOCKET_DATA] = { .sequential = true }
  };

  while (1) {
    tx_queues_empty = ((cmd_queues[((sl_si91x_queue_type_t)SI91X_SOCKET_DATA)].queued_packet_count)
                       || (cmd_queues[((sl_si91x_queue_type_t)SI91X_SOCKET_CMD)].queued_packet_count
                           && (false == command_trace[SI91X_SOCKET_CMD].command_in_flight))
                       || (cmd_queues[((sl_si91x_queue_type_t)SI91X_WLAN_CMD)].queued_packet_count
                           && (false == command_trace[SI91X_WLAN_CMD].command_in_flight))
                       || (cmd_queues[((sl_si91x_queue_type_t)SI91X_COMMON_CMD)].queued_packet_count
                           && (false == command_trace[SI91X_COMMON_CMD].command_in_flight))
                       || (cmd_queues[((sl_si91x_queue_type_t)SI91X_BT_CMD)].queued_packet_count
                           && (false == command_trace[SI91X_BT_CMD].command_in_flight))
                       || (cmd_queues[((sl_si91x_queue_type_t)SI91X_NETWORK_CMD)].queued_packet_count
                           && (false == command_trace[SI91X_NETWORK_CMD].command_in_flight)));

    // Check for an already set event
    event |= si91x_host_wait_for_bus_event(BUS_THREAD_EVENTS, 0);
    // If there are no TX packets to be processed and no RX packets pending, then waitforever
    if (((tx_queues_empty == 0) && !(event & SL_SI91X_NCP_HOST_BUS_RX_EVENT))) {
      // Wait for an event related to data TX or RX on the bus with an infinite timeout.
      event |= si91x_host_wait_for_bus_event(BUS_THREAD_EVENTS, osWaitForever);
    }

#ifndef SLI_SI91X_MCU_INTERFACE
    // Wake device, if needed
    if ((current_performance_profile != HIGH_PERFORMANCE)) {
      while (si91x_req_wakeup() != SL_STATUS_OK) {
        osDelay(1);
      }
    }

    // Read the interrupt status
    interrupt_status = 0;
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
    if (((event & SL_SI91X_NCP_HOST_BUS_RX_EVENT)
#ifndef SLI_SI91X_MCU_INTERFACE
         && (interrupt_status & RSI_RX_PKT_PENDING)
#endif
           )
        && (sl_si91x_bus_read_frame(&buffer) == SL_STATUS_OK)) { // Allocation from RX buffer type!

      // Check if the rx queue is empty
      if (0 == cmd_queues[((sl_si91x_queue_type_t)CCP_M4_TA_RX_QUEUE)].queued_packet_count) {
        event &= ~SL_SI91X_NCP_HOST_BUS_RX_EVENT; // Reset the event flag
      }

      if (current_performance_profile != HIGH_PERFORMANCE) {
        sl_si91x_host_clear_sleep_indicator();
      }

      data = (uint8_t *)sl_si91x_host_get_buffer_data(buffer, 0, &length);
      // Process the frame
      queue_id     = ((data[1] & 0xF0) >> 4);    // Extract the queue ID
      frame_type   = data[2] + (data[3] << 8);   // Extract the frame type
      frame_status = data[12] + (data[13] << 8); // Extract the frame status
#ifdef SLI_SI91X_MCU_INTERFACE
      if ((frame_type == RSI_COMMON_RSP_TA_M4_COMMANDS) || (frame_type == RSI_WLAN_REQ_SET_CERTIFICATE)) {
        // clear flag
        sli_si91x_update_flash_command_status(false);
      }
#endif

      SL_DEBUG_LOG("><<<< Rx -> queueId : %u, frameId : 0x%x, frameStatus: 0x%x, length : %u\n",
                   queue_id,
                   frame_type,
                   frame_status,
                   length);

      switch (queue_id) {
        case RSI_WLAN_MGMT_Q: {
          // Erase queue ID as it overlays with the length field which is only 24-bit
          data[1] &= 0xF;
          switch (frame_type) {
            // Handle different frame types within the WLAN management queue
            case RSI_COMMON_RSP_OPERMODE:
            case RSI_COMMON_RSP_SOFT_RESET:
            case RSI_COMMON_RSP_PWRMODE: {
              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                if ((RSI_COMMON_RSP_PWRMODE == frame_type) && (frame_status == SL_STATUS_OK)) {
                  get_wifi_current_performance_profile(&current_power_profile_mode);
                  current_performance_profile = current_power_profile_mode.profile;
                }
                global_queue_block = false;
              }
            }
            // intentional fallthrough
            case RSI_COMMON_RSP_GET_EFUSE_DATA:
            case RSI_COMMON_RSP_GET_RAM_DUMP:
            case RSI_COMMON_RSP_ANTENNA_SELECT:
            case RSI_COMMON_RSP_ENCRYPT_CRYPTO:
            case RSI_COMMON_RSP_SET_RTC_TIMER:
            case RSI_COMMON_RSP_GET_RTC_TIMER:
            case RSI_COMMON_RSP_TA_M4_COMMANDS:
            case RSI_COMMON_RSP_FEATURE_FRAME: {
              ++command_trace[SI91X_COMMON_CMD].rx_counter; // Increment the received counter for common commands

              // Check if this command is expected to have a response status
              if (((command_trace[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_COMMON_CMD].frame_type == frame_type)) {
                // Allocate a packet to store the response
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sl_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Depending on the configuration, attach the original buffer or not
                if (SI91X_PACKET_RESPONSE_PACKET
                    == (command_trace[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Populate packet metadata
                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_COMMON_CMD;
                node->sdk_context       = command_trace[SI91X_COMMON_CMD].sdk_context;
                node->flags             = command_trace[SI91X_COMMON_CMD].flags;
                node->packet_id         = command_trace[SI91X_COMMON_CMD].packet_id;

                // Add the response packet to the common response queue
                sl_si91x_host_add_to_queue(SI91X_COMMON_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
              } else {
                sl_si91x_host_free_buffer(buffer);
              }

              // Marking a received frame as not in flight when it matches the expected type.
              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                command_trace[SI91X_COMMON_CMD].command_in_flight = false;
                command_trace[SI91X_COMMON_CMD].frame_type        = 0;
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
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              // Marking a received frame as not in flight when it matches the expected type
              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
              }

              // Check if the frame type indicates a failed join operation or a disconnect
              if (((RSI_WLAN_RSP_JOIN == frame_type) && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_DISCONNECT == frame_type)) {
                // Reset current performance profile and set it to high performance
                reset_coex_current_performance_profile();
                current_performance_profile = HIGH_PERFORMANCE;
                // check for command in flight and create dummy packets for respective queues to be cleared
                handle_dhcp_and_rejoin_failure(node, temp_buffer, command_trace);
              }

              // check if the frame type is valid
              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {

                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sl_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }

                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                // Check if the packet response mode is set, and associate the host packet accordingly
                if (SI91X_PACKET_RESPONSE_PACKET
                    == (command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Populate the response packet information
                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = command_trace[SI91X_WLAN_CMD].sdk_context;
                node->flags             = command_trace[SI91X_WLAN_CMD].flags;
                node->packet_id         = command_trace[SI91X_WLAN_CMD].packet_id;

                // Add the response packet to the WLAN response queue and set the WLAN response event
                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
              } else {
                // The received frame does not match the expected response status and frame type,
                // so add it to the WLAN event queue and set the WLAN notification event
                sl_si91x_host_add_to_queue(SI91X_WLAN_EVENT_QUEUE, buffer);
                sl_si91x_host_set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }

              // Resetting the frame_type in command_trace when it matches the expected frame_type
              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].frame_type = 0;
              }

              break;
            }
            // Handle WLAN response frame type for background scan
            case RSI_WLAN_RSP_BG_SCAN: {
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              // Check if the received frame matches the expected response status and frame type
              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {
                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sl_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = command_trace[SI91X_WLAN_CMD].sdk_context;
                node->flags             = command_trace[SI91X_WLAN_CMD].flags;
                node->packet_id         = command_trace[SI91X_WLAN_CMD].packet_id;
                node->host_packet       = NULL;

                // Add the response packet to the WLAN response queue and set the WLAN response event
                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
                sl_si91x_host_free_buffer(buffer);
              } else {
                // If frame status is OK, set bg_enabled flag
                if (frame_status == SL_STATUS_OK) {
                  bg_enabled = true;
                }
                // Add the received frame to the WLAN event queue and set the WLAN notification event
                sl_si91x_host_add_to_queue(SI91X_WLAN_EVENT_QUEUE, buffer);
                sl_si91x_host_set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }
              // Marking a received frame as not in flight when it matches the expected type
              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
                command_trace[SI91X_WLAN_CMD].frame_type        = 0;
              }
              break;
            }
            case RSI_WLAN_RSP_CONFIG: {
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              // Check if the received frame matches the expected response status and frame type
              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {
                // Allocate a buffer for the response packet
                status = sl_si91x_host_allocate_buffer(&packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sl_si91x_queue_packet_t),
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
                node->sdk_context       = command_trace[SI91X_WLAN_CMD].sdk_context;
                node->flags             = command_trace[SI91X_WLAN_CMD].flags;
                node->packet_id         = command_trace[SI91X_WLAN_CMD].packet_id;
                node->host_packet       = NULL;

                // Add the response packet to response queue and set the WLAN response event
                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
              }

              // check if the frame type is valid.
              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
                command_trace[SI91X_WLAN_CMD].frame_type        = 0;
              }

              sl_si91x_host_free_buffer(buffer);
              break;
            }
            case RSI_COMMON_RSP_ULP_NO_RAM_RETENTION: {
              //This frame will come, when the M4 is waken in without ram retention. This frame is equivalent to RSI_COMMON_RSP_CARDREADY
              sl_si91x_host_clear_sleep_indicator();
            }
            // intentional fallthrough
            case RSI_COMMON_RSP_CARDREADY: {
              ++command_trace[SI91X_COMMON_CMD].rx_counter;

              // Check if the frame type is valid
              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                // Mark the common command as not in flight
                command_trace[SI91X_COMMON_CMD].command_in_flight = false;
                command_trace[SI91X_COMMON_CMD].frame_type        = 0;
              }

              sl_si91x_host_free_buffer(buffer);
              sl_si91x_host_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
              break;
            }
            case RSI_WLAN_RSP_IPCONFV4:
            case RSI_WLAN_RSP_IPCONFV6:
            case RSI_WLAN_RSP_IPV4_CHANGE:
            case RSI_WLAN_RSP_OTA_FWUP:
            case RSI_WLAN_RSP_DNS_QUERY:
            case RSI_WLAN_RSP_SET_SNI_EMBEDDED:
            case RSI_WLAN_RSP_MULTICAST:
            case RSI_WLAN_RSP_PING_PACKET:
            case RSI_WLAN_RSP_SNTP_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT:
            case RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE:
            case RSI_WLAN_RSP_MDNSD: {
              // Increment the received frame counter for network commands
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              // Allocate a buffer for the response packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
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
              if ((frame_type == command_trace[SI91X_NETWORK_CMD].frame_type)
                  || (frame_type == RSI_WLAN_RSP_IPCONFV6
                      && command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6)) {
                node->sdk_context = command_trace[SI91X_NETWORK_CMD].sdk_context;
                node->flags       = command_trace[SI91X_NETWORK_CMD].flags;
                node->packet_id   = command_trace[SI91X_NETWORK_CMD].packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                node->packet_id   = 0;
              }
              // Set the host packet to the received buffer
              node->host_packet = buffer;

              if (((frame_type == RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE)
                   || (frame_type == RSI_WLAN_RSP_EMB_MQTT_CLIENT
                       && frame_status == (SL_STATUS_SI91X_MQTT_KEEP_ALIVE_TERMINATE_ERROR & ~BIT(16))))
                  && (command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_RSP_EMB_MQTT_CLIENT)) {
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                command_trace[SI91X_NETWORK_CMD].frame_type        = 0;

                if (command_trace[SI91X_NETWORK_CMD].flags == 0) {
                  sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
                  sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
                  break;
                }

                // if command present in command_trace, an error RX packet is enqueued to network queue and command in flight is set to false.

                // Allocate a buffer for the error packet
                status = sl_si91x_host_allocate_buffer(&error_packet,
                                                       SL_WIFI_RX_FRAME_BUFFER,
                                                       sizeof(sl_si91x_queue_packet_t),
                                                       1000);
                if (status != SL_STATUS_OK) {
                  SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                  BREAKPOINT();
                }
                error_node               = sl_si91x_host_get_buffer_data(error_packet, 0, NULL);
                error_node->frame_status = (SL_STATUS_SI91X_MQTT_REMOTE_TERMINATE_ERROR
                                            & 0xFFFF); // error given by firmware for remote terminate
                error_node->host_packet  = NULL;
                error_node->flags        = command_trace[SI91X_NETWORK_CMD].flags;
                error_node->packet_id    = command_trace[SI91X_NETWORK_CMD].packet_id;

                sl_si91x_host_add_to_queue(SI91X_NETWORK_RESPONSE_QUEUE, error_packet);
                sl_si91x_host_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
                break;
              }

              // Check if it's a response packet, and handle accordingly
              if (((command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && ((command_trace[SI91X_NETWORK_CMD].frame_type == frame_type)
                      || (command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6
                          && frame_type == RSI_WLAN_RSP_IPCONFV6))) {

                // Check if the response packet should be free or not
                if (SI91X_PACKET_RESPONSE_PACKET
                    != (command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Add the response packet to response queue and set the network response event
                sl_si91x_host_add_to_queue(SI91X_NETWORK_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
              } else {
                // Add the packet to event queue and set the async network event
                sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
                sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }

              // Check If the frame type is valid.
              if ((frame_type == command_trace[SI91X_NETWORK_CMD].frame_type)
                  || (frame_type == RSI_WLAN_RSP_IPCONFV6
                      && command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6)) {
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                command_trace[SI91X_NETWORK_CMD].frame_type        = 0;
              }

              // Check if the frame type indicates a failed join operation or a disconnect
              if (((RSI_WLAN_RSP_IPCONFV4 == frame_type) && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_IPV4_CHANGE == frame_type)) {
                // Reset current performance profile and set it to high performance
                reset_coex_current_performance_profile();
                current_performance_profile = HIGH_PERFORMANCE;
                // check for command in flight and create dummy packets for respective queues to be cleared
                handle_dhcp_and_rejoin_failure(node, temp_buffer, command_trace);
                SL_NET_EVENT_DISPATCH_HANDLER(node, (sl_si91x_packet_t *)data);
              }
              break;
            }
            case RSI_WLAN_RSP_SOCKET_CONFIG:
            case RSI_WLAN_RSP_SOCKET_CREATE:
            case RSI_WLAN_RSP_SOCKET_CLOSE:
            case RSI_WLAN_RSP_SELECT_REQUEST:
            case RSI_WLAN_RSP_CONN_ESTABLISH:
            case RSI_WLAN_RSP_SOCKET_READ_DATA: // Socket read data response only is expected incase of failure
            case RSI_WLAN_RSP_SOCKET_ACCEPT:
            case RSI_WLAN_RSP_REMOTE_TERMINATE: {
              ++command_trace[SI91X_SOCKET_CMD].rx_counter;

              if (RSI_WLAN_RSP_CONN_ESTABLISH == frame_type) {
                frame_type = RSI_WLAN_RSP_SOCKET_ACCEPT;
              }
              // Allocate a buffer for the response packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
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
              if ((command_trace[SI91X_SOCKET_CMD].frame_type == frame_type)) {
                node->sdk_context = command_trace[SI91X_SOCKET_CMD].sdk_context;
                node->flags       = command_trace[SI91X_SOCKET_CMD].flags;
                node->packet_id   = command_trace[SI91X_SOCKET_CMD].packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                node->packet_id   = 0;
              }

              // Check if it's a response packet, and handle accordingly
              if (((command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_SOCKET_CMD].frame_type == frame_type)) {

                // Check if the response packet should be free or not
                if (SI91X_PACKET_RESPONSE_PACKET
                    != (command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer);
                }

                // Add the response packet to the socket response queue and set the socket response event
                sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);

              } else {
                // Handle the default case when the frame_type doesn't match any known cases
                if (frame_type == RSI_WLAN_RSP_REMOTE_TERMINATE) {
                  sl_si91x_socket_close_response_t *remote_socket_closure =
                    (sl_si91x_socket_close_response_t *)(((sl_si91x_packet_t *)data)->data);
                  // if command present in command_trace belongs to terminated socket, an error RX packet is enqueued to socket queue and command in flight is set to false.
                  if ((command_trace[SI91X_SOCKET_CMD].sl_si91x_socket_id == remote_socket_closure->socket_id)
                      && (command_trace[SI91X_SOCKET_CMD].command_in_flight == true)) {
                    // Allocate a buffer for the error packet
                    status = sl_si91x_host_allocate_buffer(&error_packet,
                                                           SL_WIFI_RX_FRAME_BUFFER,
                                                           sizeof(sl_si91x_queue_packet_t),
                                                           1000);
                    if (status != SL_STATUS_OK) {
                      SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                      BREAKPOINT();
                    }
                    error_node = sl_si91x_host_get_buffer_data(error_packet, 0, NULL);
                    error_node->frame_status =
                      (SL_STATUS_SI91X_SOCKET_CLOSED & 0xFFFF); // error given by firmware for remote terminate
                    error_node->host_packet = NULL;
                    error_node->flags       = command_trace[SI91X_SOCKET_CMD].flags;
                    error_node->packet_id   = command_trace[SI91X_SOCKET_CMD].packet_id;
                    sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, error_packet);
                    sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);
                    command_trace[SI91X_SOCKET_CMD].command_in_flight = false;
                  }
                  SL_NET_EVENT_DISPATCH_HANDLER(node, (sl_si91x_packet_t *)data);

                  sl_si91x_host_free_buffer(packet);
                  sl_si91x_host_free_buffer(buffer);
                } else {
                  // If it's not remote termination, add the packet to the socket event queue
                  // and set asynchronous socket notification event
                  sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
                  sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
                }
              }

              // If the frame_type matches the expected frame_type for the socket command
              // mark the command as not in flight and clear the frame_type
              if (frame_type == command_trace[SI91X_SOCKET_CMD].frame_type) {
                command_trace[SI91X_SOCKET_CMD].command_in_flight = false;
                command_trace[SI91X_SOCKET_CMD].frame_type        = 0;
              }

              break;
            }

            // Handle TCP ACK Indication response
            case RSI_WLAN_RSP_TCP_ACK_INDICATION: {
              // Allocate memory for a new packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
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
              node->packet_id   = 0;

              // Add the packet to the socket event queue and set the asynchronous socket notification event
              sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
              sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
              break;
            }
#ifdef SLI_SI91X_INTERNAL_HTTP_CLIENT
            case RSI_WLAN_RSP_HTTP_CLIENT_GET:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              if (command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_RSP_HTTP_CLIENT_GET) {
                // If it's an HTTP GET response, check if the frame_status is not OK or if end_of_data is set to 1.
                sl_si91x_packet_t *get_response_packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
                uint16_t *end_of_data                  = (uint16_t *)&get_response_packet->data;

                if (frame_status != SL_STATUS_OK || *end_of_data == 1) {
                  // Mark the command as not in flight and clear the frame_type
                  command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                  command_trace[SI91X_NETWORK_CMD].frame_type        = 0;
                }
              } else {
                // For other HTTP responses, mark the command as not in flight and clear the frame_type
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                command_trace[SI91X_NETWORK_CMD].frame_type        = 0;
              }

              // Allocate memory for a new packet
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
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
              node->sdk_context       = command_trace[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = command_trace[SI91X_NETWORK_CMD].flags;
              node->packet_id         = command_trace[SI91X_NETWORK_CMD].packet_id;

              // Add the packet to the network event queue and set the asynchronous network notification event
              sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
              sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              break;
            }
            case RSI_WLAN_RSP_HTTP_ABORT:
            case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                SL_DEBUG_LOG("\r\n HEAP EXHAUSTED DURING ALLOCATION \r\n");
                BREAKPOINT();
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              // Populate the packet's information
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;
              node->sdk_context       = command_trace[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = command_trace[SI91X_NETWORK_CMD].flags;
              node->packet_id         = command_trace[SI91X_NETWORK_CMD].packet_id;

              if (((command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_NETWORK_CMD].frame_type == frame_type)) {
                // If it's a response status and the frame_type matches, set host_packet to NULL and free the buffer
                node->host_packet = NULL;
                sl_si91x_host_free_buffer(buffer);

                // Add the packet to the network response queue and set the network response event
                sl_si91x_host_add_to_queue(SI91X_NETWORK_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
              } else {
                // For other cases, set host_packet to buffer and add it to the network event queue
                node->host_packet = buffer;

                sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
                sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }

              if (frame_type == command_trace[SI91X_NETWORK_CMD].frame_type) {
                // mark the command as not in flight and clear the frame_type
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                command_trace[SI91X_NETWORK_CMD].frame_type        = 0;
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
            // Increment the receive counter for the socket command
            ++command_trace[SI91X_SOCKET_CMD].rx_counter;

            // Allocate memory for a new packet from SL_WIFI_RX_FRAME_BUFFER
            status =
              sl_si91x_host_allocate_buffer(&packet, SL_WIFI_RX_FRAME_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
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

            if ((command_trace[SI91X_SOCKET_CMD].frame_type == RSI_WLAN_REQ_SOCKET_READ_DATA)) {
              // If it's a socket read data request, copy additional information
              node->sdk_context = command_trace[SI91X_SOCKET_CMD].sdk_context;
              node->flags       = command_trace[SI91X_SOCKET_CMD].flags;
              node->packet_id   = command_trace[SI91X_SOCKET_CMD].packet_id;
            } else {
              // Otherwise, set additional information to defaults
              node->sdk_context = NULL;
              node->flags       = 0;
              node->packet_id   = 0;
            }

            if (((command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                && (command_trace[SI91X_SOCKET_CMD].frame_type == RSI_WLAN_REQ_SOCKET_READ_DATA)) {
              // If it's a response status and the frame_type matches, add the packet to the socket response queue.
              sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, packet);
              sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);
            } else {
              // sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
              // sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);

              // For other cases, process the raw packet and free the host_packet and packet
              sl_si91x_packet_t *raw_packet =
                (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(node->host_packet, 0, NULL);
              SL_NET_EVENT_DISPATCH_HANDLER(node, raw_packet);

              sl_si91x_host_free_buffer(node->host_packet);
              sl_si91x_host_free_buffer(packet);
            }

            if (RSI_WLAN_RSP_SOCKET_READ_DATA == command_trace[SI91X_SOCKET_CMD].frame_type) {
              // Mark the command as not in flight and clear the frame_type
              command_trace[SI91X_SOCKET_CMD].command_in_flight = false;
              command_trace[SI91X_SOCKET_CMD].frame_type        = 0;
            }

#else
            // If SLI_SI91X_OFFLOAD_NETWORK_STACK is not defined, process the data frame and free the buffer.
            sl_si91x_host_process_data_frame(SL_WIFI_CLIENT_INTERFACE, buffer);
            sl_si91x_host_free_buffer(buffer);
#endif
          } else if (frame_type == SL_SI91X_WIFI_RX_DOT11_DATA) {
            ++command_trace[SI91X_WLAN_CMD].rx_counter;

            // Marking a received frame as not in flight when it matches the expected type
            if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
              command_trace[SI91X_WLAN_CMD].command_in_flight = false;
            }

            // Add it to the WLAN event queue and set the WLAN notification event
            sl_si91x_host_add_to_queue(SI91X_WLAN_EVENT_QUEUE, buffer);
            sl_si91x_host_set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
          }

          break;
        }
#ifdef SLI_SI91X_ENABLE_BLE
        case RSI_BT_Q: {
          SL_DEBUG_LOG("Received BLE packet\n");
          // Increment the receive counter for the Bluetooth command
          ++command_trace[SI91X_BT_CMD].rx_counter;

          if (frame_type == command_trace[SI91X_BT_CMD].frame_type) {
            // Mark the command as not in flight and clear the frame_type
            command_trace[SI91X_BT_CMD].command_in_flight = false;
            command_trace[SI91X_BT_CMD].frame_type        = 0;
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
    }

    if (event & SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS) {
      // This condition is checked before writing frames to the bus
      for (i = 0; i < SI91X_SOCKET_DATA; i++) {
        // Check if the current command queue is empty
        if ((cmd_queues[((sl_si91x_queue_type_t)i)].queued_packet_count)
            && (command_trace[i].command_in_flight != true)) {
          // Read the interrupt status
          sl_si91x_bus_read_interrupt_status(&interrupt_status);
          if ((0 == (interrupt_status & RSI_BUFFER_FULL))) {
            if (bus_write_frame(i, SL_WIFI_CONTROL_BUFFER, &(command_trace[i]), &global_queue_block) == SL_STATUS_OK) {
              // Check if there are more packets in the queue
              if (0 == cmd_queues[((sl_si91x_queue_type_t)i)].queued_packet_count) {
                // No more packets, clear the SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS for the processed queue
                event &= ~SL_SI91X_TX_PENDING_FLAG(i);
              }
            }
          } else {
            break;
          }
        } else {
          // Check if the current command queue is empty
          if (0 == cmd_queues[((sl_si91x_queue_type_t)i)].queued_packet_count) {
            // No more packets, clear the SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS for the processed queue
            event &= ~SL_SI91X_TX_PENDING_FLAG(i);
          }
        }
      }
    }

    if (event & SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT) {
      if (cmd_queues[((sl_si91x_queue_type_t)SI91X_SOCKET_DATA)].queued_packet_count) {
        // Read the interrupt status
        sl_si91x_bus_read_interrupt_status(&interrupt_status);
        if ((0 == (interrupt_status & RSI_BUFFER_FULL))) {
          if (bus_write_data_frame(SI91X_SOCKET_DATA_QUEUE, SL_WIFI_TX_FRAME_BUFFER, &global_queue_block)
              != SL_STATUS_OK) {
            continue;
          }
        } else {
          continue;
        }
      } else {
        event &= ~SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT;
      }
    }
  }
  //To suppress warning unused parameter, no code effect
  UNUSED_PARAMETER(event);
}

static sl_status_t bus_write_frame(sl_si91x_queue_type_t queue_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   sl_si91x_command_trace_t *trace,
                                   bool *global_queue_block)
{
  UNUSED_PARAMETER(buffer_type);
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_si91x_queue_packet_t *node = NULL;

  if (true == *global_queue_block) {
    return SL_STATUS_BUSY;
  }

  if (current_performance_profile != HIGH_PERFORMANCE) {
    if (si91x_req_wakeup() != SL_STATUS_OK) {
      return SL_STATUS_TIMEOUT;
    }
  }

  status = sl_si91x_host_remove_from_queue(queue_type, &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  node            = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  packet          = sl_si91x_host_get_buffer_data(node->host_packet, 0, NULL);
  uint16_t length = packet->length;

  // Modify the packet's descriptor to include the firmware queue ID in the length field
  packet->desc[1] |= (node->firmware_queue_id << 4);

  if (packet->command) {
    // If the packet doesn't have an async response, mark the command as in flight
    if (SI91X_PACKET_WITH_ASYNC_RESPONSE != (node->flags & SI91X_PACKET_WITH_ASYNC_RESPONSE)) {
      trace->command_in_flight = true;
    }

    // Set the global_queue_block flag if it is present in the packet's flags
    if (SI91X_PACKET_GLOBAL_QUEUE_BLOCK == (SI91X_PACKET_GLOBAL_QUEUE_BLOCK & node->flags)) {
      *global_queue_block = true;
    }

    // Update trace information with packet details
    trace->packet_id         = node->packet_id;
    trace->firmware_queue_id = node->firmware_queue_id;
    trace->frame_type        = packet->command;
    trace->flags             = node->flags;

    // copy the socket_id of sl_si91x_queue_packet_t structure to sl_si91x_command_trace_t structure.
    if (queue_type == (sl_si91x_queue_type_t)SI91X_SOCKET_CMD) {
      trace->sl_si91x_socket_id = node->sl_si91x_socket_id;
    }

    trace->sdk_context = node->sdk_context;
  }
#ifdef SLI_SI91X_MCU_INTERFACE
  if ((trace->frame_type == RSI_COMMON_RSP_TA_M4_COMMANDS) || (trace->frame_type == RSI_WLAN_REQ_SET_CERTIFICATE)) {
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
               trace->firmware_queue_id,
               trace->frame_type,
               length);

  if (current_performance_profile != HIGH_PERFORMANCE) {
    sl_si91x_host_clear_sleep_indicator();
  }

#ifdef SLI_SI91X_ENABLE_BLE
  if (SI91X_BT_CMD_QUEUE == queue_type) {
    rsi_bt_common_tx_done(packet);
  }
#endif

  sl_si91x_host_free_buffer(node->host_packet);
  sl_si91x_host_free_buffer(buffer);

  trace->tx_counter++;
  return SL_STATUS_OK;
}

static sl_status_t bus_write_data_frame(sl_si91x_queue_type_t queue_type, // This function is called for writing data
                                        sl_wifi_buffer_type_t buffer_type,
                                        bool *global_queue_block)
{
  UNUSED_PARAMETER(buffer_type);
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if (true == *global_queue_block) {
    return SL_STATUS_BUSY;
  }

  if (current_performance_profile != HIGH_PERFORMANCE) {
    if (si91x_req_wakeup() != SL_STATUS_OK) {
      return SL_STATUS_TIMEOUT;
    }
  }

  status = sl_si91x_host_remove_from_queue(queue_type, &buffer);
  VERIFY_STATUS_AND_RETURN(status);
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

#ifndef SLI_SI91X_MCU_INTERFACE
  // If WLAN data queue is involved, release semaphore
  osSemaphoreRelease(cmd_lock);
#endif

  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}
