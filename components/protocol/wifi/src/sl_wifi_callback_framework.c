#include "sl_wifi_callback_framework.h"
#include "sl_si91x_host_interface.h"
#include "sl_constants.h"
#include "sl_si91x_core_utilities.h"
#include "sl_si91x_driver.h"

/// Entry in the callback table
typedef struct {
  sl_wifi_callback_function_t function; /// User provided callback function pointer
  void *arg;                            /// User provided callback argument
} sl_wifi_callback_entry_t;

//#define EXECUTE_CALLBACK(id, packet) do { if (registered_callbacks[id].function) {return registered_callbacks[id].function(packet->command, packet->data, packet->length, registered_callbacks[id].arg); } } while(0)
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

sl_status_t default_wifi_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer)
{
  sl_wifi_callback_entry_t *entry = get_callback_entry((sl_wifi_event_group_t)event);

  // Verify there is a callback registered, if not return immediately
  if (entry == NULL || entry->function == NULL) {
    return SL_STATUS_OK;
  }

  // Start processing the event
  sl_si91x_packet_t *packet = (sl_si91x_packet_t *)sl_si91x_host_get_buffer_data((sl_wifi_buffer_t *)buffer, 0, NULL);
  if (CHECK_IF_EVENT_FAILED(event)) {
    sl_status_t status = convert_firmware_status(get_si91x_frame_status(packet));
    if (packet->command == RSI_WLAN_RSP_JOIN) {
      sl_status_t temp_status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                                             SI91X_WLAN_CMD_QUEUE,
                                                             NULL,
                                                             0,
                                                             SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                             NULL,
                                                             NULL);
      VERIFY_STATUS_AND_RETURN(temp_status);
    }

    return entry->function(event, &status, 0, entry->arg);
  }
  return entry->function(event, packet->data, packet->length, entry->arg);
}

static sl_wifi_callback_entry_t *get_callback_entry(sl_wifi_event_group_t group)
{
  if (group > SL_WIFI_EVENT_GROUP_COUNT) {
    group = get_event_group_from_event((sl_wifi_event_t)group);
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
      || event == SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT) {
    return SL_WIFI_TWT_RESPONSE_EVENTS;
  }
  return (sl_wifi_event_group_t)event;
}
