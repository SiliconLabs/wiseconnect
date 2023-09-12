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
#include "sl_constants.h"
#include "sl_si91x_types.h"
#include "sl_si91x_driver.h"
#include "sl_net_constants.h"
#include "sl_net_rsi_utility.h"
#ifdef SI91X_SOCKET_FEATURE
#include "sl_si91x_socket_callback_framework.h"
#endif
#include "sl_si91x_core_utilities.h"

#ifdef MQTT_CLIENT_FEATURE
#include "si91x_mqtt_client_callback_framework.h"
#include "si91x_mqtt_client_utility.h"
#endif

#ifdef SNTP_CLIENT_FEATURE
#include "si91x_sntp_client_callback_framework.h"
#endif

#ifdef HTTP_CLIENT_FEATURE
#include "sl_si91x_http_client_callback_framework.h"
#endif

#ifdef MQTT_CLIENT_FEATURE
/**
 * A internal function to handle to asynchronous mqtt client events.
 */
static void handle_mqtt_client_asynch_events(sl_si91x_queue_packet_t *mqtt_asyn_packet)
{
  sl_si91x_packet_t *raw_rx_packet = sl_si91x_host_get_buffer_data(mqtt_asyn_packet->host_packet, 0, NULL);
  sl_mqtt_client_t *mqtt_client;

  // Since these responses are unsolicited, We need to create a context for them.
  if (raw_rx_packet->command == RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE
      || raw_rx_packet->command == RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT) {

    sli_si91x_get_mqtt_client(&mqtt_client);

    if (mqtt_client == NULL) {
      // client state is either connecting or disconnected;
      SL_DEBUG_LOG("Dropping mqtt client event, Si91x Event: %hu", raw_rx_packet->command);
      return;
    }

    sli_si91x_build_mqtt_sdk_context_if_async(raw_rx_packet->command == RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE
                                                ? SL_MQTT_CLIENT_DISCONNECTED_EVENT
                                                : SL_MQTT_CLIENT_MESSAGED_RECEIVED_EVENT,
                                              mqtt_client,
                                              NULL,
                                              NULL,
                                              0,
                                              (sl_si91x_mqtt_client_context_t **)&mqtt_asyn_packet->sdk_context);
  }

  sl_si91x_mqtt_client_context_t *sdk_context = (sl_si91x_mqtt_client_context_t *)mqtt_asyn_packet->sdk_context;

  if (sdk_context == NULL) {
    return;
  }

  SL_DEBUG_LOG("handle_mqtt_client_asynch_events: event %x", sdk_context->event);

  uint16_t si91x_event_status = get_si91x_frame_status(raw_rx_packet);
  sl_status_t event_status    = convert_firmware_status(si91x_event_status);

  si91x_mqtt_event_handler(event_status, sdk_context, raw_rx_packet);
}
#endif

void sl_net_si91x_event_dispatch_handler(sl_si91x_queue_packet_t *data, sl_si91x_packet_t *packet)
{
  sl_status_t status;
  sl_net_event_t service_event;
#ifdef HTTP_CLIENT_FEATURE
  sl_http_client_event_t http_event;
#endif

#ifdef MQTT_CLIENT_FEATURE
  if (packet->command == RSI_WLAN_REQ_EMB_MQTT_CLIENT || packet->command == RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT
      || packet->command == RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE) {
    handle_mqtt_client_asynch_events(data);

    return;
  }
#endif

#ifdef SNTP_CLIENT_FEATURE
  if (packet->command & RSI_WLAN_RSP_SNTP_CLIENT) {
    si91x_sntp_event_handler(data);
    return;
  }
#endif

#ifdef SI91X_SOCKET_FEATURE
  if (packet->command == RSI_WLAN_RSP_CONN_ESTABLISH || packet->command == RSI_WLAN_RSP_REMOTE_TERMINATE
      || packet->command == RSI_RECEIVE_RAW_DATA || packet->command == RSI_WLAN_RSP_TCP_ACK_INDICATION
      || packet->command == RSI_WLAN_RSP_SELECT_REQUEST) {
    sl_si91x_packet_t *raw_rx_packet = packet;
    uint16_t si91x_event_status      = get_si91x_frame_status(raw_rx_packet);

    sl_status_t event_status = convert_firmware_status(si91x_event_status);
    si91x_socket_event_handler(event_status, (sl_si91x_socket_context_t *)data->sdk_context, raw_rx_packet);
  }
#endif

  status = convert_si91x_event_to_sl_net_event(&packet->command, &service_event);
  if (status == SL_STATUS_OK) {
    SL_DEBUG_LOG("><<<< Got net event : %u\n", service_event);
    sl_si91x_default_handler(service_event, data->host_packet);
  }
#ifdef HTTP_CLIENT_FEATURE
  // Check for sl_http_client_event_t
  else if (convert_si91x_event_to_sl_http_client_event(&packet->command, &http_event) == SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\n>>> HTTP Event received: %u <<<\r\n", http_event);
    sl_http_client_default_event_handler(http_event, data->host_packet, data->sdk_context);
  }
#endif
}
