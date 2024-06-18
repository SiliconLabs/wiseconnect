/*******************************************************************************
* @file  sl_wifi_callback_framework.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#pragma once

#include "sl_wifi_host_interface.h"
#include "sl_wifi_device.h" // To access the device specific structs
#include <stdint.h>

/** \addtogroup WIFI_CALLBACK_FRAMEWORK Callback Framework
  * \ingroup SL_WIFI
  * @{ */

/// Generic macro for callback functions to check if the event has Failed.
#define SL_WIFI_CHECK_IF_EVENT_FAILED(event) ((event & SL_WIFI_EVENT_FAIL_INDICATION) ? true : false)

/**
 * @typedef sl_wifi_callback_function_t
 * @brief Generic callback for Wi-Fi group event of type @ref sl_wifi_event_group_t
 * @param event
 * Wi-Fi event of type @ref sl_wifi_event_t
 * @param data
 * Data received
 * @param data_length
 * Data length
 * @param optional_arg
 * Optional user provided argument passed in @ref sl_wifi_set_callback
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
 */
typedef sl_status_t (*sl_wifi_callback_function_t)(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

/**
 * @typedef sl_wifi_scan_callback_t
 * @brief Callback for SL_WIFI_SCAN_RESULT_EVENTS group event
 * @param event
 * Wi-Fi event of type @ref sl_wifi_event_t
 * @param data
 * Scan results of type @ref sl_wifi_scan_result_t
 * @param data_length
 * Data length
 * @param optional_arg
 * Optional user provided argument passed in [sl_wifi_set_scan_callback](../wiseconnect-api-reference-guide-wi-fi/wifi-callback-framework#sl-wifi-set-scan-callback)
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
 */
typedef sl_status_t (*sl_wifi_scan_callback_t)(sl_wifi_event_t event,
                                               sl_wifi_scan_result_t *data,
                                               uint32_t data_length,
                                               void *optional_arg);

/**
 * @typedef sl_wifi_stats_callback_t
 * @brief Callback for SL_WIFI_STATS_RESPONSE_EVENTS group events
 * @param event
 * 	Wi-Fi event of type @ref sl_wifi_event_t
 *  Individual Wi-Fi events related to SL_WIFI_STATS_RESPONSE_EVENTS is as follows.
 * | @ref sl_wifi_event_t                 | DataType                                    |
 * |:-------------------------------------|:--------------------------------------------|
 * | SL_WIFI_STATS_EVENT                  | Not supported in current release            |
 * | SL_WIFI_STATS_AYSNC_EVENT            | [sl_si91x_async_stats_response_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-async-stats-response-t)        |
 * | SL_WIFI_STATS_ADVANCE_EVENT          | Not supported in current release            |
 * | SL_WIFI_STATS_TEST_MODE_EVENT        | Not supported in current release            |
 * | SL_WIFI_STATS_MODULE_STATE_EVENT     | [sl_si91x_module_state_stats_response_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-module-state-stats-response-t) |
 * @param data
 * 	Data received.
 * @param data_length
 * 	Data length
 * @param optional_arg
 * Optional user provided argument passed in [sl_wifi_set_stats_callback](../wiseconnect-api-reference-guide-wi-fi/wifi-callback-framework#sl-wifi-set-stats-callback)
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored.
 */
typedef sl_status_t (*sl_wifi_stats_callback_t)(sl_wifi_event_t event,
                                                void *data,
                                                uint32_t data_length,
                                                void *optional_arg);

/**
 * @typedef sl_wifi_join_callback_t
 * @brief Callback for SL_WIFI_JOIN_EVENTS group events
 * @param event
 * 	Wi-Fi event of type @ref sl_wifi_event_t
 * @param data
 * 	Data received in string.
 * | Data received   | Description                |
 * |:----------------|:---------------------------|
 * | C               | Module connection success  |
 * | F               | Module connection failed   |
 * @param data_length
 * 	Data length
 * @param optional_arg
 * Optional user provided argument passed in [sl_wifi_set_join_callback](../wiseconnect-api-reference-guide-wi-fi/wifi-callback-framework#sl-wifi-set-join-callback)
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
 */
typedef sl_status_t (*sl_wifi_join_callback_t)(sl_wifi_event_t event, char *data, uint32_t data_length, void *arg);

/**
 * @typedef sl_wifi_twt_config_callback_t
 * @brief Callback for SL_WIFI_TWT_RESPONSE_EVENTS group events
 * @param event
 * 	Wi-Fi event of type @ref sl_wifi_event_t
 *  Individual Wi-Fi events related to SL_WIFI_TWT_RESPONSE_EVENTS is as follows.
 * | @ref sl_wifi_event_t                          |
 * |:----------------------------------------------|
 * | SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT |
 * | SL_WIFI_TWT_AP_REJECTED_EVENT                 |
 * | SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT            |
 * | SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT        |
 * | SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT        |
 * | SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT            |
 * | SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT         |
 * | SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT    |
 * | SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT     |
 * | SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT  |
 * | SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT      |
 * | SL_WIFI_RESCHEDULE_TWT_SUCCESS_EVENT          |
 * | SL_WIFI_TWT_INFO_FRAME_EXCHANGE_FAILED_EVENT  |
 * @param data
 * 	Data received of type @ref sl_si91x_twt_response_t.
 * @param data_length
 * 	Data length
 * @param optional_arg
 * Optional user provided argument passed in [sl_wifi_set_twt_config_callback](../wiseconnect-api-reference-guide-wi-fi/wifi-callback-framework#sl-wifi-set-twt-config-callback)
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
 */
typedef sl_status_t (*sl_wifi_twt_config_callback_t)(sl_wifi_event_t event,
                                                     sl_si91x_twt_response_t *data,
                                                     uint32_t data_length,
                                                     void *arg);

/**
 * @typedef sl_wifi_transceiver_callback_t
 * @brief Callback for SL_WIFI_TRANSCEIVER_EVENTS group events
 * @param[out] event
 *  Wi-Fi event of type @ref sl_wifi_event_t. Individual Wi-Fi events related to SL_WIFI_TRANSCEIVER_EVENTS are as follows.
 * | @ref sl_wifi_event_t                   |
 * |:---------------------------------------|
 * | SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB |
 * | SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB  |
 * @param[out] data
 *  - Data received is of type @ref sl_wifi_transceiver_rx_data_t for SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB event.
 *  - Data received is of type @ref sl_wifi_transceiver_tx_data_confirmation_t for SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB event.
 * @param[out] data_length
 *   Reserved
 * @param[out] optional_arg
 *   Optional user provided argument passed in [sl_wifi_set_transceiver_callback](../pages/wifi-callback-framework#sl-wifi-set-transceiver-callback)
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors).
 * @note This API is only supported in Wi-Fi Transceiver opermode (7).
 * @note
 *   In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored.
 */
typedef sl_status_t (*sl_wifi_transceiver_callback_t)(sl_wifi_event_t event,
                                                      void *data,
                                                      uint32_t data_length,
                                                      void *arg);

/***************************************************************************/ /**
 * @brief
 *   Register a callback for selected event group. All the individual Wi-Fi events related to specific group will be triggered via this group callback
 * @param[in] group
 *   group id of the event. See @ref sl_wifi_event_group_t
 * @param[in] function
 *   Function pointer to callback of type @ref sl_wifi_callback_function_t
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler of type @ref sl_wifi_callback_function_t
 * @pre Pre-conditions:
 * - Pre-conditions:
 * - Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   Callbacks can be set only for @ref sl_wifi_event_group_t, not for individual events (@ref sl_wifi_event_t)
 ******************************************************************************/
sl_status_t sl_wifi_set_callback(sl_wifi_event_group_t group, sl_wifi_callback_function_t function, void *optional_arg);

/***************************************************************************/ /**
 * @brief
 * 	Default Wi-Fi event handler to passed to @ref sl_wifi_init. This function will dispatch Wi-Fi events and invokes respective Wi-Fi group event callback
 * @param[in] event
 * 	Wi-Fi event of type of @ref sl_wifi_event_t
 * @param[in] buffer
 *  Buffer containing raw data from TA firmware
 * @return
 *  sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *  Passing this event handler is optional. User can implement his own event dispatching framework.
 ******************************************************************************/
extern sl_status_t sl_wifi_default_event_handler(sl_wifi_event_t event, sl_wifi_buffer_t *buffer);

/***************************************************************************/ /**
 * @brief
 *   Register callback for SL_WIFI_SCAN_RESULT_EVENTS group event from @ref sl_wifi_event_group_t.
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler of type @ref sl_wifi_scan_callback_t
 * @pre Pre-conditions:
 * - Pre-conditions:
 * - Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * All the individual Wi-Fi events related to this group will be triggered via this callback
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_scan_callback(sl_wifi_scan_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_SCAN_RESULT_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register callback for SL_WIFI_JOIN_EVENTS group event from @ref sl_wifi_event_group_t.
 * @param[in] function
 *   Function pointer to callback. This will be passed back to callback handler of type @ref sl_wifi_join_callback_t
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre Pre-conditions:
 * - Pre-conditions:
 * - Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * All the individual Wi-Fi events related to this group will be triggered via this callback
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_join_callback(sl_wifi_join_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_JOIN_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register callback for SL_WIFI_TWT_RESPONSE_EVENTS group event from @ref sl_wifi_event_group_t.
 * @param[in] function
 *   Function pointer to callback. This will be passed back to callback handler of type @ref sl_wifi_twt_config_callback_t
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre Pre-conditions:
 * - Pre-conditions:
 * - Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   All the individual Wi-Fi events related to this group will be triggered via this callback.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_twt_config_callback(sl_wifi_twt_config_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_TWT_RESPONSE_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register callback for SL_WIFI_STATS_RESPONSE_EVENTS group event from @ref sl_wifi_event_group_t.
 * @param[in] function
 *   Function pointer to callback. This will be passed back to callback handler of type @ref sl_wifi_stats_callback_t
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre Pre-conditions:
 * - Pre-conditions:
 * - Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   All the individual Wi-Fi events related to this group will be triggered via this callback.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_stats_callback(sl_wifi_stats_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_STATS_RESPONSE_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/***************************************************************************/ /**
 * @brief
 *   Register callback for SL_WIFI_TRANSCEIVER_EVENTS group event from @ref sl_wifi_event_group_t.
 * @param[in] function
 *   Function pointer to callback. This will be passed back to callback handler of type @ref sl_wifi_transceiver_callback_t
 * @param[in] optional_arg
 *   Optional user provided argument. This will be passed back to callback handler.
 * @pre Pre-conditions:
 * - @ref sl_wifi_init should be called before this API.
 * @return
 *   sl_status_t. See [Status Codes](../../wiseconnect-api-reference-guide-err-codes/pages/sl-additional-status-errors).
 * @note
 *   All the individual Wi-Fi events related to this group will be triggered via this callback.
 ******************************************************************************/
static inline sl_status_t sl_wifi_set_transceiver_callback(sl_wifi_transceiver_callback_t function, void *optional_arg)
{
  return sl_wifi_set_callback(SL_WIFI_TRANSCEIVER_EVENTS, (sl_wifi_callback_function_t)function, optional_arg);
}

/** @} */
