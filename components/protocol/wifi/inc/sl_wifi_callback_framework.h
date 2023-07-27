#pragma once

#include "sl_wifi_host_interface.h"
#include "sl_wifi_device.h" // To access the device specific structs
#include <stdint.h>

/** \addtogroup WIFI_CALLBACK_FRAMEWORK Callback Framework
  * \ingroup SL_WIFI
  * @{ */

/// Generic macro for callback functions to check if the event has Failed.
#define CHECK_IF_EVENT_FAILED(event) ((event & SL_WIFI_EVENT_FAIL_INDICATION) ? true : false)

/// Generic Wi-Fi callback function type
typedef sl_status_t (*sl_wifi_callback_function_t)(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

/// Wi-Fi scan result callback type.
/// In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
typedef sl_status_t (*sl_wifi_scan_callback_t)(sl_wifi_event_t event,
                                               sl_wifi_scan_result_t *data,
                                               uint32_t data_length,
                                               void *arg);

/// Wi-Fi stats callback type.
/// In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
/// @note structure of data to be expected
/// @note SL_WIFI_STATS_EVENT -> Coming soon
/// @note: SL_WIFI_STATS_AYSNC_EVENT -> sl_si91x_async_stats_response_t
/// @note: SL_WIFI_STATS_ADVANCE_EVENT -> sl_si91x_advance_stats_response_t
/// @note: SL_WIFI_STATS_TEST_MODE_EVENT -> Coming soon
/// @note: SL_WIFI_STATS_MODULE_STATE_EVENT -> sl_si91x_module_state_stats_response_t
typedef sl_status_t (*sl_wifi_stats_callback_t)(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

/// Wi-Fi join callback type.
/// In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
typedef sl_status_t (*sl_wifi_join_callback_t)(sl_wifi_event_t event, char *data, uint32_t data_length, void *arg);

/// TWT Response callback type.
/// In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
typedef sl_status_t (*sl_wifi_twt_config_callback_t)(sl_wifi_event_t event,
                                                     sl_si91x_twt_response_t *data,
                                                     uint32_t data_length,
                                                     void *arg);

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for a selected callback
 * @param[in] event
 *   ID of the event. See \ref sl_wifi_event_t
 * @param[in] function
 *   Function pointer to callback
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   Callbacks can be set only for sl_wifi_event_group_t, not for individual events (sl_wifi_event_t)
 ******************************************************************************/
sl_status_t sl_wifi_set_callback(sl_wifi_event_group_t group, sl_wifi_callback_function_t function, void *optional_arg);

/// Default Wi-Fi event handler
extern sl_status_t default_wifi_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer);

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for scan results callback
 * @param[in] function
 *   Function pointer to callback
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_scan_callback(sl_wifi_scan_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_SCAN_RESULT_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for join status callback
 * @param[in] function
 *   Function pointer to callback
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_join_callback(sl_wifi_join_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_JOIN_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for TWT response callback
 * @param[in] function
 *   Function pointer to callback
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_twt_config_callback(sl_wifi_twt_config_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_TWT_RESPONSE_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for statistic report callback
 * @param[in] function
 *   Function pointer to callback
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_stats_callback(sl_wifi_stats_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_STATS_RESPONSE_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/** @} */
