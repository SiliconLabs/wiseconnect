/*******************************************************************************
 * # License
 * Copyright 2019 Silicon Laboratories Inc. www.silabs.com
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

#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/**
 *  @addtogroup SERVICE_SNTP_CONSTANTS 
 *  @{
 */
//SNTP Method types
#define SL_SNTP_BROADCAST_MODE 1
#define SL_SNTP_UNICAST_MODE   2

/******************************************************
 *                   Enumerations
 ******************************************************/

/// SNTP Client Events
typedef enum {
  SL_SNTP_CLIENT_START = 1,
  SL_SNTP_CLIENT_GET_TIME_DATE,
  SL_SNTP_CLIENT_GET_SERVER_INFO,
  SL_SNTP_CLIENT_STOP
} sl_sntp_client_events_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/

/**
 * @addtogroup SERVICE_SNTP_TYPES
 * @{
 */

/// SNTP Response
typedef struct {
  uint8_t event_type;   ///< Type of event being received indicated by sl_sntp_client_events_t
  sl_status_t status;   ///< Status of the call which triggered this response
  uint8_t *data;        ///< Pointer to response data
  uint16_t data_length; ///< Length of response data
} sl_sntp_client_response_t;

/// SNTP response handler
typedef void (*sl_sntp_client_event_handler_t)(sl_sntp_client_response_t *response,
                                               uint8_t *user_data,
                                               uint16_t user_data_length);
/// SNTP time sync notification handler (Coming soon)
typedef void (*sl_sntp_set_time_sync_notification_handler_t)(const uint8_t cmd_type,
                                                             sl_status_t status,
                                                             const uint8_t *buffer);
/// SNTP configuration
typedef struct {
  uint8_t *server_host_name;                    ///< SNTP host name or address
  uint8_t sntp_method;                          ///< SNTP method
  uint16_t sntp_timeout;                        ///< SNTP timeout
  uint8_t flags;                                ///< Flags to define functionality
  sl_sntp_client_event_handler_t event_handler; ///< SNTP response handler
  sl_sntp_set_time_sync_notification_handler_t
    time_sync_notifiication_handler; ///< Set to true if smooth sync required. Coming Soon
  bool smooth_sync;                  ///< Set to true if smooth sync required. Coming Soon
  bool server_from_dhcp;             ///< Set to true to request NTP server config from DHCP. Coming Soon
  bool renew_servers_after_new_IP;   ///< This is used to refresh server list if NTP provided by DHCP. Coming Soon
} sl_sntp_client_config_t;
/** @} */

/**
 * @addtogroup SERVICE_SNTP_FUNCTIONS
 * @{
 */

/**
 * @brief
 * Start SNTP client.
 * @param[in] config		Valid pointer to client configuration structure. This shall not be null.
 * @param[in] timeout       Timeout for starting SNTP client. This is blocking API for timeout > 0,  else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API needs to be called before calling any other SNTP API
 */
sl_status_t sl_sntp_client_start(sl_sntp_client_config_t *config, uint32_t timeout);

/**
 * @brief
 * Get time and date information from NTP.
 * @param[in] data		    Valid pointer to data buffer.
 * @param[in] data_length   Data buffer length.
 * @param[in] timeout       Timeout for getting time and date. This is blocking API for timeout > 0, else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_get_time_date(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * @brief
 * Get NTP server information.
 * @param[in] data		    Valid pointer to data buffer.
 * @param[in] data_length   Data buffer length.
 * @param[in] timeout       Timeout for getting server information. This is blocking API for timeout > 0,  else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_get_server_info(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * @brief
 * Stop SNTP client.
 * @param[in] timeout          Timeout for stop SNTP client. This is blocking API for timeout > 0, else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_stop(uint32_t timeout);
/** @} */
