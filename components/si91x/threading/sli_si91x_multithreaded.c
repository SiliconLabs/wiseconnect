/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifdef SI91X_NETWORK_OFFLOAD_ENABLED
#include "sl_net_si91x_integration_handler.h"
#else
#define SL_NET_EVENT_DISPATCH_HANDLER(data, packet) \
  {                                                 \
    UNUSED_PARAMETER(data);                         \
    UNUSED_PARAMETER(packet);                       \
  }
#endif

#ifdef BLE_ENABLE
#include "rsi_bt_common.h"
#endif

#ifdef RSI_M4_INTERFACE
extern uint8_t rx_packet_pending_flag;
#endif

/*
  Structure to track the status and info of all the commands in flight 
  for a particular command of type sl_si91x_command_type_t
*/
typedef struct {
  const bool sequential;
  bool command_in_flight;
  uint16_t frame_type;
  uint8_t firmware_queue_id;
  uint32_t rx_counter;
  uint32_t tx_counter;
  uint16_t packet_id;
  uint8_t flags;
  void *sdk_context;
} sl_si91x_command_trace_t;
extern bool bg_enabled;
extern sl_wifi_event_handler_t si91x_event_handler;
#ifndef RSI_M4_INTERFACE
extern sl_si91x_semaphore_handle_t cmd_lock;
#endif

void sli_submit_rx_buffer(void);

#ifdef RSI_M4_INTERFACE
extern void sli_config_m4_dma_desc_on_reset(void);
#endif
static sl_status_t bus_write_frame(sl_si91x_queue_type_t queue_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   sl_si91x_command_trace_t *trace,
                                   bool *global_queue_block);
sl_status_t si91x_req_wakeup(void);

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

  while (1) {
    const uint32_t event_mask =
      (NCP_HOST_WLAN_NOTIFICATION_EVENT | NCP_HOST_NETWORK_NOTIFICATION_EVENT | NCP_HOST_SOCKET_NOTIFICATION_EVENT);
    uint32_t event = si91x_host_wait_for_async_event(event_mask, osWaitForever);

    if ((event & NCP_HOST_WLAN_NOTIFICATION_EVENT) != 0) {
      while (sl_si91x_host_queue_status(SI91X_WLAN_EVENT_QUEUE) != 0) {
        sl_wifi_buffer_t *buffer = NULL;
        if (sl_si91x_host_remove_from_queue(SI91X_WLAN_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          sl_si91x_packet_t *packet   = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          const uint16_t frame_status = get_si91x_frame_status(packet);

          // Call event handler
          if (si91x_event_handler != NULL) {
            sl_wifi_event_t wifi_event = convert_si91x_event_to_sl_wifi_event(packet->command, frame_status);
            if (wifi_event != SL_WIFI_INVALID_EVENT) {
              si91x_event_handler(wifi_event, buffer);
            }
          } else {
            // TODO: error handling
          }
          sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
        }
      }
    }

    // If firmware sends network events even if the sl_net component is not include
    // (probably firmware issue), Free the resources of the packet to avoid memory leak
    if ((event & NCP_HOST_NETWORK_NOTIFICATION_EVENT) != 0) {
      while (sl_si91x_host_queue_status(SI91X_NETWORK_EVENT_QUEUE) != 0) {
        sl_wifi_buffer_t *buffer = NULL;
        if (sl_si91x_host_remove_from_queue(SI91X_NETWORK_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          sl_si91x_queue_packet_t *data = (sl_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          sl_si91x_packet_t *packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(data->host_packet, 0, NULL);

          SL_NET_EVENT_DISPATCH_HANDLER(data, packet);

          sl_si91x_host_free_buffer(data->host_packet, SL_WIFI_RX_FRAME_BUFFER);
          sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
        } else {
          // TODO: error handling
        }
      }
    }

    if ((event & NCP_HOST_SOCKET_NOTIFICATION_EVENT) != 0) {
      while (0 != sl_si91x_host_queue_status(SI91X_SOCKET_EVENT_QUEUE)) {
        sl_wifi_buffer_t *buffer = NULL;
        if (sl_si91x_host_remove_from_queue(SI91X_SOCKET_EVENT_QUEUE, &buffer) == SL_STATUS_OK) {
          sl_si91x_queue_packet_t *data = (sl_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
          sl_si91x_packet_t *packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(data->host_packet, 0, NULL);

          SL_NET_EVENT_DISPATCH_HANDLER(data, packet);

          sl_si91x_host_free_buffer(data->host_packet, SL_WIFI_RX_FRAME_BUFFER);
          sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
        } else {
          // TODO: error handling
        }
      }
    }
  }
}

/// Thread which handles the TX and RX events.
void si91x_bus_thread(void *args)
{
  UNUSED_PARAMETER(args);
  sl_status_t status;
  uint16_t interrupt_status;
  uint16_t temp;
  sl_si91x_queue_packet_t *node = NULL;
  sl_wifi_buffer_t *packet;
  sl_wifi_buffer_t *buffer;
  uint8_t tx_queues_empty = 0;
  uint32_t event          = 0;
  uint8_t queue_mask      = 0;
  uint8_t *data;
  uint16_t length;
  uint8_t queue_id        = 0;
  uint16_t frame_type     = 0;
  uint16_t frame_status   = 0;
  bool global_queue_block = false;
  int i;

  sl_si91x_command_trace_t command_trace[SI91X_CMD_MAX] = { [SI91X_COMMON_CMD]  = { .sequential = true },
                                                            [SI91X_WLAN_CMD]    = { .sequential = true },
                                                            [SI91X_NETWORK_CMD] = { .sequential = true },
                                                            [SI91X_SOCKET_CMD]  = { .sequential = true },
                                                            [SI91X_BT_CMD]      = { .sequential = true } };

  while (1) {
    tx_queues_empty = (sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_COMMON_CMD)
                       | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_WLAN_CMD)
                       | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_NETWORK_CMD)
                       | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_SOCKET_CMD)
                       | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_BT_CMD));

    // Wait for an event
#ifdef RSI_M4_INTERFACE
    if ((0 == tx_queues_empty) || (rx_packet_pending_flag == 1)) {
#else
    if (0 == tx_queues_empty) {
#endif
      event           = si91x_host_wait_for_bus_event((NCP_HOST_DATA_TX_EVENT | NCP_HOST_BUS_RX_EVENT), osWaitForever);
      tx_queues_empty = (sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_COMMON_CMD)
                         | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_WLAN_CMD)
                         | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_NETWORK_CMD)
                         | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_SOCKET_CMD)
                         | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_BT_CMD));
    }

    // Wake device, if needed
    if ((current_performance_profile != HIGH_PERFORMANCE)) {
      if (si91x_req_wakeup() != SL_STATUS_OK) {
        // TBD: VERY IMPORTANT: Need to restore Rx event if it was set: if(event & NCP_HOST_BUS_RX_EVENT) then restore Rx event
        if ((event & NCP_HOST_DATA_TX_EVENT) || (tx_queues_empty != 0))
          goto process_data_tx_event;
        else
          continue;
      }
    }

    // Read the interrupt status
    interrupt_status = 0;
    if (sl_si91x_bus_read_interrupt_status(&interrupt_status) != SL_STATUS_OK) {
      continue;
    }

    if (!((interrupt_status & RSI_RX_PKT_PENDING) || (event & NCP_HOST_BUS_RX_EVENT))) {
      if (current_performance_profile != HIGH_PERFORMANCE) {
        sl_si91x_host_clear_sleep_indicator();
      }
    }

    if (((event & NCP_HOST_BUS_RX_EVENT) || (interrupt_status & RSI_RX_PKT_PENDING)
#ifdef RSI_M4_INTERFACE
         || (rx_packet_pending_flag == 1)
#endif
           )
        && (sl_si91x_bus_read_frame(&buffer) == SL_STATUS_OK)) {

#ifdef RSI_M4_INTERFACE
      rx_packet_pending_flag = 0x00;
#endif

      event = 0;
      // Allocate packet

      if (current_performance_profile != HIGH_PERFORMANCE) {
        sl_si91x_host_clear_sleep_indicator();
      }

      data = (uint8_t *)sl_si91x_host_get_buffer_data(buffer, 0, &length);
      // Process the frame
      queue_id     = ((data[1] & 0xF0) >> 4);
      frame_type   = data[2] + (data[3] << 8);
      frame_status = data[12] + (data[13] << 8);

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
            case RSI_COMMON_RSP_OPERMODE:
            case RSI_COMMON_RSP_SOFT_RESET:
            case RSI_COMMON_RSP_PWRMODE: {
              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                global_queue_block = false;
              }
            }
            // intentional fallthrough
            case RSI_COMMON_RSP_GET_RAM_DUMP:
            case RSI_COMMON_RSP_ANTENNA_SELECT:
            case RSI_COMMON_RSP_FEATURE_FRAME: {
              ++command_trace[SI91X_COMMON_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                command_trace[SI91X_COMMON_CMD].command_in_flight = false;
              }

              if (((command_trace[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_COMMON_CMD].frame_type == frame_type)) {
                status =
                  sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
                if (status != SL_STATUS_OK) {
                  break;
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                if (SI91X_PACKET_RESPONSE_PACKET
                    == (command_trace[SI91X_COMMON_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
                }

                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_COMMON_CMD;
                node->sdk_context       = command_trace[SI91X_COMMON_CMD].sdk_context;
                node->flags             = command_trace[SI91X_COMMON_CMD].flags;
                node->packet_id         = command_trace[SI91X_COMMON_CMD].packet_id;

                sl_si91x_host_add_to_queue(SI91X_COMMON_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
              } else {
                sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
              }
              break;
            }
            case RSI_WLAN_RSP_BAND:
            case RSI_WLAN_RSP_INIT:
            case RSI_WLAN_RSP_EAP_CONFIG:
            case RSI_WLAN_RSP_SET_CERTIFICATE:
            case RSI_WLAN_RSP_HOST_PSK:
            case RSI_WLAN_RSP_JOIN:
            case RSI_WLAN_RSP_SCAN:
            case RSI_WLAN_RSP_SCAN_RESULTS:
            case RSI_WLAN_RSP_FW_VERSION:
            case RSI_WLAN_RSP_FWUP:
            case RSI_WLAN_RSP_DISCONNECT:
            case RSI_WLAN_RSP_RSSI:
            case RSI_WLAN_RSP_AP_CONFIGURATION:
            case RSI_WLAN_RSP_WPS_METHOD:
            case RSI_WLAN_RSP_QUERY_NETWORK_PARAMS:
            case RSI_WLAN_RSP_SET_MAC_ADDRESS:
            case RSI_WLAN_RSP_SET_REGION:
            case RSI_WLAN_RSP_SET_REGION_AP:
            case RSI_WLAN_RSP_MAC_ADDRESS:
            case RSI_WLAN_RSP_EXT_STATS:
            case RSI_WLAN_RSP_RX_STATS:
            case RSI_WLAN_RSP_QUERY_GO_PARAMS:
            case RSI_WLAN_RSP_ROAM_PARAMS:
            case RSI_WLAN_RSP_HTTP_OTAF:
            case RSI_COMMON_RSP_TA_M4_COMMANDS:
            case RSI_WLAN_RSP_CLIENT_CONNECTED:
            case RSI_WLAN_RSP_CLIENT_DISCONNECTED:
            case RSI_WLAN_RSP_FILTER_BCAST_PACKETS:
            case RSI_WLAN_RSP_TWT_PARAMS:
            case RSI_WLAN_RSP_TWT_ASYNC:
            case RSI_WLAN_RSP_11AX_PARAMS:
            case RSI_WLAN_RSP_REJOIN_PARAMS:
            case RSI_WLAN_RSP_GAIN_TABLE:
            case RSI_WLAN_RSP_TX_TEST_MODE: {
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
              }

              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {
                status =
                  sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
                if (status != SL_STATUS_OK) {
                  break;
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                if (SI91X_PACKET_RESPONSE_PACKET
                    == (command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = buffer;
                } else {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
                }

                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = command_trace[SI91X_WLAN_CMD].sdk_context;
                node->flags             = command_trace[SI91X_WLAN_CMD].flags;
                node->packet_id         = command_trace[SI91X_WLAN_CMD].packet_id;

                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
              } else {
                sl_si91x_host_add_to_queue(SI91X_WLAN_EVENT_QUEUE, buffer);
                sl_si91x_host_set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }
              if (((RSI_WLAN_RSP_JOIN == frame_type) && (frame_status != SL_STATUS_OK))
                  || (RSI_WLAN_RSP_DISCONNECT == frame_type)) {
                reset_coex_current_performance_profile();
                current_performance_profile = HIGH_PERFORMANCE;
              }
              break;
            }
            case RSI_WLAN_RSP_BG_SCAN: {
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
              }

              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {
                status =
                  sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
                if (status != SL_STATUS_OK) {
                  break;
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

                node->frame_status      = frame_status;
                node->firmware_queue_id = RSI_WLAN_MGMT_Q;
                node->command_type      = SI91X_WLAN_CMD;
                node->sdk_context       = command_trace[SI91X_WLAN_CMD].sdk_context;
                node->flags             = command_trace[SI91X_WLAN_CMD].flags;
                node->packet_id         = command_trace[SI91X_WLAN_CMD].packet_id;
                node->host_packet       = NULL;

                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
                sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
              } else {
                if (frame_status == SL_STATUS_OK) {
                  bg_enabled = true;
                }
                sl_si91x_host_add_to_queue(SI91X_WLAN_EVENT_QUEUE, buffer);
                sl_si91x_host_set_async_event(NCP_HOST_WLAN_NOTIFICATION_EVENT);
              }
              break;
            }
            case RSI_WLAN_RSP_CONFIG: {
              ++command_trace[SI91X_WLAN_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_WLAN_CMD].frame_type) {
                command_trace[SI91X_WLAN_CMD].command_in_flight = false;
              }

              if (((command_trace[SI91X_WLAN_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_WLAN_CMD].frame_type == frame_type)) {
                status =
                  sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
                if (status != SL_STATUS_OK) {
                  break;
                }
                node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

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

                sl_si91x_host_add_to_queue(SI91X_WLAN_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_WLAN_RESPONSE_EVENT);
              }

              sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
              break;
            }
            case RSI_COMMON_RSP_ULP_NO_RAM_RETENTION: {
              //This frame will come, when the M4 is waken in without ram retention. This frame is equivalent to RSI_COMMON_RSP_CARDREADY
              sl_si91x_host_clear_sleep_indicator();
            }
            // intentional fallthrough
            case RSI_COMMON_RSP_CARDREADY: {
              ++command_trace[SI91X_COMMON_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_COMMON_CMD].frame_type) {
                command_trace[SI91X_COMMON_CMD].command_in_flight = false;
              }

              sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
              sl_si91x_host_set_event(NCP_HOST_COMMON_RESPONSE_EVENT);
              break;
            }
            case RSI_WLAN_RSP_IPCONFV4:
            case RSI_WLAN_RSP_IPCONFV6:
            case RSI_WLAN_RSP_OTA_FWUP:
            case RSI_WLAN_RSP_DNS_QUERY:
            case RSI_WLAN_RSP_MULTICAST:
            case RSI_WLAN_RSP_PING_PACKET:
            case RSI_WLAN_RSP_SNTP_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_CLIENT:
            case RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT:
            case RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE: {
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                break;
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;

              if ((frame_type == command_trace[SI91X_NETWORK_CMD].frame_type)
                  || (frame_type == RSI_WLAN_RSP_IPCONFV6
                      && command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6)) {
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                node->sdk_context                                  = command_trace[SI91X_NETWORK_CMD].sdk_context;
                node->flags                                        = command_trace[SI91X_NETWORK_CMD].flags;
                node->packet_id                                    = command_trace[SI91X_NETWORK_CMD].packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                node->packet_id   = 0;
              }
              node->host_packet = buffer;

              if (((command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && ((command_trace[SI91X_NETWORK_CMD].frame_type == frame_type)
                      || (command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_REQ_IPCONFV6
                          && frame_type == RSI_WLAN_RSP_IPCONFV6))) {

                if (SI91X_PACKET_RESPONSE_PACKET
                    != (command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
                }

                sl_si91x_host_add_to_queue(SI91X_NETWORK_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
              } else {
                sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
                sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
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

              if (frame_type == command_trace[SI91X_SOCKET_CMD].frame_type) {
                command_trace[SI91X_SOCKET_CMD].command_in_flight = false;
              }

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                break;
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_SOCKET_CMD;
              node->host_packet       = buffer;

              if ((command_trace[SI91X_SOCKET_CMD].frame_type == frame_type)) {

                node->sdk_context = command_trace[SI91X_SOCKET_CMD].sdk_context;
                node->flags       = command_trace[SI91X_SOCKET_CMD].flags;
                node->packet_id   = command_trace[SI91X_SOCKET_CMD].packet_id;
              } else {
                node->sdk_context = NULL;
                node->flags       = 0;
                node->packet_id   = 0;
              }

              if (((command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_SOCKET_CMD].frame_type == frame_type)) {

                if (SI91X_PACKET_RESPONSE_PACKET
                    != (command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_PACKET)) {
                  node->host_packet = NULL;
                  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
                }

                sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);

              } else {
                if (frame_type == RSI_WLAN_RSP_REMOTE_TERMINATE) {
                  SL_NET_EVENT_DISPATCH_HANDLER(node, (sl_si91x_packet_t *)data);

                  sl_si91x_host_free_buffer(packet, SL_WIFI_RX_FRAME_BUFFER);
                  sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
                } else {
                  sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
                  sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
                }
              }

              break;
            }

            case RSI_WLAN_RSP_TCP_ACK_INDICATION: {
              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                break;
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_SOCKET_CMD;
              node->host_packet       = buffer;

              node->sdk_context = NULL;
              node->flags       = 0;
              node->packet_id   = 0;

              sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
              sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);
              break;
            }
#ifdef HTTP_CLIENT_FEATURE
            case RSI_WLAN_RSP_HTTP_CLIENT_GET:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST:
            case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              if (command_trace[SI91X_NETWORK_CMD].frame_type == RSI_WLAN_RSP_HTTP_CLIENT_GET) {
                sl_si91x_packet_t *get_response_packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
                uint16_t *end_of_data                  = (uint16_t *)&get_response_packet->data;

                if (frame_status != SL_STATUS_OK || *end_of_data == 1) {
                  command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
                }
              } else {
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
              }

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                break;
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              node->host_packet       = buffer;
              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;
              node->sdk_context       = command_trace[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = command_trace[SI91X_NETWORK_CMD].flags;
              node->packet_id         = command_trace[SI91X_NETWORK_CMD].packet_id;

              sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
              sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              break;
            }
            case RSI_WLAN_RSP_HTTP_ABORT:
            case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
              ++command_trace[SI91X_NETWORK_CMD].rx_counter;

              if (frame_type == command_trace[SI91X_NETWORK_CMD].frame_type) {
                command_trace[SI91X_NETWORK_CMD].command_in_flight = false;
              }

              status =
                sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
              if (status != SL_STATUS_OK) {
                break;
              }
              node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

              node->frame_status      = frame_status;
              node->firmware_queue_id = RSI_WLAN_MGMT_Q;
              node->command_type      = SI91X_NETWORK_CMD;
              node->sdk_context       = command_trace[SI91X_NETWORK_CMD].sdk_context;
              node->flags             = command_trace[SI91X_NETWORK_CMD].flags;
              node->packet_id         = command_trace[SI91X_NETWORK_CMD].packet_id;

              if (((command_trace[SI91X_NETWORK_CMD].flags & SI91X_PACKET_RESPONSE_STATUS)
                   == SI91X_PACKET_RESPONSE_STATUS)
                  && (command_trace[SI91X_NETWORK_CMD].frame_type == frame_type)) {
                node->host_packet = NULL;
                sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);

                sl_si91x_host_add_to_queue(SI91X_NETWORK_RESPONSE_QUEUE, packet);
                sl_si91x_host_set_event(NCP_HOST_NETWORK_RESPONSE_EVENT);
              } else {
                node->host_packet = buffer;

                sl_si91x_host_add_to_queue(SI91X_NETWORK_EVENT_QUEUE, packet);
                sl_si91x_host_set_async_event(NCP_HOST_NETWORK_NOTIFICATION_EVENT);
              }
              break;
            }
#endif
            default: {
              if (PRINT_ERROR_LOGS) {
                PRINT_ERROR_STATUS(INFO_TAG, frame_type);
              }
              sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
              break;
            }
          }
          break;
        }

        case RSI_WLAN_DATA_Q: {
          // Erase queue ID as it overlays with the length field which is only 24-bit
          data[1] &= 0xF;
          if (frame_type == RSI_RECEIVE_RAW_DATA) {
#ifdef SI91X_NETWORK_OFFLOAD_ENABLED
            SL_DEBUG_LOG("Raw Data\n");
            ++command_trace[SI91X_SOCKET_CMD].rx_counter;

            if (RSI_WLAN_RSP_SOCKET_READ_DATA == command_trace[SI91X_SOCKET_CMD].frame_type) {
              command_trace[SI91X_SOCKET_CMD].command_in_flight = false;
            }

            status =
              sl_si91x_host_allocate_buffer(&packet, SL_WIFI_CONTROL_BUFFER, sizeof(sl_si91x_queue_packet_t), 1000);
            if (status != SL_STATUS_OK) {
              break;
            }
            node = sl_si91x_host_get_buffer_data(packet, 0, &temp);

            node->frame_status      = frame_status;
            node->firmware_queue_id = RSI_WLAN_MGMT_Q;
            node->command_type      = SI91X_SOCKET_CMD;
            node->host_packet       = buffer;

            if ((command_trace[SI91X_SOCKET_CMD].frame_type == RSI_WLAN_REQ_SOCKET_READ_DATA)) {
              node->sdk_context = command_trace[SI91X_SOCKET_CMD].sdk_context;
              node->flags       = command_trace[SI91X_SOCKET_CMD].flags;
              node->packet_id   = command_trace[SI91X_SOCKET_CMD].packet_id;
            } else {
              node->sdk_context = NULL;
              node->flags       = 0;
              node->packet_id   = 0;
            }

            if (((command_trace[SI91X_SOCKET_CMD].flags & SI91X_PACKET_RESPONSE_STATUS) == SI91X_PACKET_RESPONSE_STATUS)
                && (command_trace[SI91X_SOCKET_CMD].frame_type == RSI_WLAN_REQ_SOCKET_READ_DATA)) {
              sl_si91x_host_add_to_queue(SI91X_SOCKET_RESPONSE_QUEUE, packet);
              sl_si91x_host_set_event(NCP_HOST_SOCKET_RESPONSE_EVENT);
            } else {
              // sl_si91x_host_add_to_queue(SI91X_SOCKET_EVENT_QUEUE, packet);
              // sl_si91x_host_set_async_event(NCP_HOST_SOCKET_NOTIFICATION_EVENT);

              sl_si91x_packet_t *raw_packet =
                (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data(node->host_packet, 0, NULL);
              SL_NET_EVENT_DISPATCH_HANDLER(node, raw_packet);

              sl_si91x_host_free_buffer(node->host_packet, SL_WIFI_RX_FRAME_BUFFER);
              sl_si91x_host_free_buffer(packet, SL_WIFI_RX_FRAME_BUFFER);
            }

#else
            sl_si91x_host_process_data_frame(SL_WIFI_CLIENT_INTERFACE, buffer);
            sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
#endif
          }

          break;
        }
#ifdef BLE_ENABLE
        case RSI_BT_Q: {
          SL_DEBUG_LOG("Received BLE packet\n");
          ++command_trace[SI91X_BT_CMD].rx_counter;

          if (frame_type == command_trace[SI91X_BT_CMD].frame_type) {
            command_trace[SI91X_BT_CMD].command_in_flight = false;
          }
          rsi_driver_process_bt_resp_handler(data);
          sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
          break;
        }
#endif
        default: {
          sl_si91x_host_free_buffer(buffer, SL_WIFI_CONTROL_BUFFER);
          break;
        }
      }
      sli_submit_rx_buffer();
    }
process_data_tx_event:
    if ((0 == (interrupt_status & RSI_BUFFER_FULL)) && (false == global_queue_block)) {
      queue_mask = 1;

      for (i = 0; i < SI91X_CMD_MAX; i++) {
        if ((queue_mask & tx_queues_empty) > 0) {
          bus_write_frame(i, SL_WIFI_CONTROL_BUFFER, &(command_trace[i]), &global_queue_block);
        }

        queue_mask = (queue_mask << 1);
      }
    }
  }
}

static sl_status_t bus_write_frame(sl_si91x_queue_type_t queue_type,
                                   sl_wifi_buffer_type_t buffer_type,
                                   sl_si91x_command_trace_t *trace,
                                   bool *global_queue_block)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_si91x_queue_packet_t *node = NULL;

  if (true == trace->sequential) {
    if (true == trace->command_in_flight) {
      return SL_STATUS_BUSY;
    }
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
  packet->desc[1] |= (node->firmware_queue_id << 4); // This modifies packet->length

  if (SI91X_PACKET_WITH_ASYNC_RESPONSE != (node->flags & SI91X_PACKET_WITH_ASYNC_RESPONSE)) {
    trace->command_in_flight = true;
  }

  if (SI91X_PACKET_GLOBAL_QUEUE_BLOCK == (SI91X_PACKET_GLOBAL_QUEUE_BLOCK & node->flags)) {
    *global_queue_block = true;
  }

  trace->packet_id         = node->packet_id;
  trace->firmware_queue_id = node->firmware_queue_id;
  trace->frame_type        = packet->command;
  trace->flags             = node->flags;
  trace->sdk_context       = node->sdk_context;

  status = sl_si91x_bus_write_frame(packet, packet->data, length);
#ifdef RSI_M4_INTERFACE
  if (packet->desc[2] == RSI_COMMON_REQ_SOFT_RESET) {
    sli_config_m4_dma_desc_on_reset();
  }
#endif
  if (trace->frame_type == RSI_COMMON_REQ_PWRMODE) {
    // TBD : Power save Request expecting small delay while switching between Any power save mode to HIGH_PERFORMANCE and vise-versa. Maybe a small delay is required for configuring the power save parameter in the firmware.
    osDelay(3);
  }

  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n BUS_WRITE_ERROR \r\n");
    __asm__("bkpt");
  }

  SL_DEBUG_LOG("<>>>> Tx -> queueId : %u, frameId : 0x%x, length : %u\n",
               trace->firmware_queue_id,
               trace->frame_type,
               length);

  if (current_performance_profile != HIGH_PERFORMANCE) {
    sl_si91x_host_clear_sleep_indicator();
  }

#ifdef BLE_ENABLE
  if (SI91X_BT_CMD_QUEUE == queue_type) {
    rsi_bt_common_tx_done(packet);
  }
#endif

#ifndef RSI_M4_INTERFACE
  if (RSI_WLAN_DATA_Q == trace->firmware_queue_id) {
    sl_si91x_semaphore_post(&cmd_lock);
  }
#endif

  sl_si91x_host_free_buffer(node->host_packet, buffer_type);
  sl_si91x_host_free_buffer(buffer, buffer_type);

  trace->tx_counter++;
  return SL_STATUS_OK;
}
