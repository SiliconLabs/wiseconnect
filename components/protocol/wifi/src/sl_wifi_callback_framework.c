/***************************************************************************/ /**
 * @file  sl_wifi_callback_framework.c
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

#include "sl_wifi_callback_framework.h"
#include "sl_si91x_host_interface.h"
#include "sl_constants.h"
#include "sl_si91x_core_utilities.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"

/// Entry in the callback table
typedef struct {
  sl_wifi_callback_function_t function; /// User provided callback function pointer
  void *arg;                            /// User provided callback argument
} sl_wifi_callback_entry_t;

static sl_wifi_callback_entry_t *get_callback_entry(sl_wifi_event_group_t group);
static sl_wifi_event_group_t get_event_group_from_event(sl_wifi_event_t event);

sl_wifi_callback_entry_t registered_callbacks[SL_WIFI_EVENT_GROUP_COUNT];

sl_status_t sl_wifi_set_callback(sl_wifi_event_group_t group, sl_wifi_callback_function_t function, void *optional_arg)
{
  sl_wifi_callback_entry_t *entry = get_callback_entry(group);
  if (entry != NULL) {
    entry->function = function;
    entry->arg      = optional_arg;
    return SL_STATUS_OK;
  }
  return SL_STATUS_FAIL;
}

sl_status_t sl_wifi_default_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer)
{
  sl_wifi_callback_entry_t *entry = get_callback_entry((sl_wifi_event_group_t)event);

  // Verify there is a callback registered, if not return immediately
  if (entry == NULL || entry->function == NULL) {
    return SL_STATUS_OK;
  }

  // Start processing the event
  sl_wifi_system_packet_t *packet = (sl_wifi_system_packet_t *)sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    sl_status_t status = sli_convert_and_save_firmware_status(sli_get_si91x_frame_status(packet));
    if (packet->command == SLI_WLAN_RSP_JOIN) {
      sl_status_t temp_status = sli_si91x_driver_send_command(SLI_WLAN_REQ_INIT,
                                                              SLI_SI91X_WLAN_CMD,
                                                              NULL,
                                                              0,
                                                              SLI_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                              NULL,
                                                              NULL);
      VERIFY_STATUS_AND_RETURN(temp_status);
    }

    return entry->function(event, &status, 0, entry->arg);
  }

  if (event == SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB) {
    sl_wifi_transceiver_tx_data_confirmation_t tx_cfm_cb_data = { 0 };
    tx_cfm_cb_data.status                                     = packet->desc[15];
    tx_cfm_cb_data.rate     = packet->data[0];          //Extended descriptor in data[] for rate
    tx_cfm_cb_data.priority = packet->data[4];          //Extended descriptor in data[] for priority
    memcpy(&tx_cfm_cb_data.token, &packet->data[8], 4); //Extended descriptor in data[] for token

    return entry->function(event, &tx_cfm_cb_data, 0, entry->arg);
  } else if (event == SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB) {
    sl_wifi_transceiver_rx_data_t rx_cb_data = { 0 };
    uint16_t payload_offset                  = packet->desc[4];
    uint16_t payload_length                  = packet->length & 0xFFF;
    uint32_t status                          = *(uint32_t *)(&packet->data[12]);

    rx_cb_data.length = payload_length - payload_offset;
    rx_cb_data.buffer = packet->data + payload_offset;
    rx_cb_data.rssi   = *(int8_t *)(&packet->data[0]);   //Extended descriptor in data[] for rssi
    rx_cb_data.rate   = *(uint16_t *)(&packet->data[2]); //Extended descriptor in data[] for rate

    /*
     * SL_STATUS_UNKNOWN_PEER - If SL_WIFI_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT is enabled but
     * data packet is received from a peer not present in MAC layer.
     */
    if ((status & TRANSCEIVER_RX_PKT_TA_MATCH_BIT) || IS_BCAST_MCAST_MAC(rx_cb_data.buffer[4]))
      rx_cb_data.status = SL_STATUS_OK;
    else
      rx_cb_data.status = SL_STATUS_UNKNOWN_PEER;

    return entry->function(event, &rx_cb_data, 0, entry->arg);
  }

  if (packet->length) {
    return entry->function(event, packet->data, packet->length, entry->arg);
  } else {
    return entry->function(event, NULL, packet->length, entry->arg);
  }
}

static sl_wifi_callback_entry_t *get_callback_entry(sl_wifi_event_group_t group)
{
  if (group > SL_WIFI_EVENT_GROUP_COUNT) {
    group = get_event_group_from_event((sl_wifi_event_t)group);
  }
  group = (group & ~SL_WIFI_EVENT_FAIL_INDICATION);
  if (group < 0 || group >= SL_WIFI_EVENT_GROUP_COUNT) {
    return NULL; // Return NULL if the group index is out of bounds
  }
  return &registered_callbacks[group];
}

static sl_wifi_event_group_t get_event_group_from_event(sl_wifi_event_t event)
{
  //For TWT Events
  if (event == SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT || event == SL_WIFI_TWT_AP_REJECTED_EVENT
      || event == SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT || event == SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT
      || event == SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT || event == SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT
      || event == SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT || event == SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT
      || event == SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT || event == SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT
      || event == SL_WIFI_RESCHEDULE_TWT_SUCCESS_EVENT || event == SL_WIFI_TWT_INFO_FRAME_EXCHANGE_FAILED_EVENT
      || event == SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT) {
    return SL_WIFI_TWT_RESPONSE_EVENTS;
  }
  // for STATS Events
  else if (event == SL_WIFI_STATS_EVENT || event == SL_WIFI_STATS_ASYNC_EVENT || event == SL_WIFI_STATS_ADVANCE_EVENT
           || event == SL_WIFI_STATS_TEST_MODE_EVENT || event == SL_WIFI_STATS_MODULE_STATE_EVENT) {
    return SL_WIFI_STATS_RESPONSE_EVENTS;
  } else if (event == SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB || event == SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB) {
    return SL_WIFI_TRANSCEIVER_EVENTS;
  } else {
    event = SL_WIFI_INVALID_EVENT;
  }
  return (sl_wifi_event_group_t)event;
}
