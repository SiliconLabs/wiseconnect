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
#include "sl_common.h"
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

/**
 * @addtogroup SERVICE_SNTP_METHODS SNTP Methods
 * @{
 */
/// SNTP broadcast mode
#define SL_SNTP_BROADCAST_MODE 1

/// SNTP unicast mode
#define SL_SNTP_UNICAST_MODE 2
/** @} */

/**
 * @addtogroup SERVICE_SNTP_FLAGS SNTP Flags
 * @{
 */
/// SNTP enable IPv6.
/// @note By default IPv4 is enabled
#define SL_SNTP_ENABLE_IPV6 1
/** @} */

/******************************************************
 *                   Enumerations
 ******************************************************/

/// SNTP Client Events
typedef enum {
  SL_SNTP_CLIENT_START = 1,       ///< Event for SNTP client started
  SL_SNTP_CLIENT_GET_TIME,        ///< Event for SNTP get time
  SL_SNTP_CLIENT_GET_TIME_DATE,   ///< Event for SNTP get time date
  SL_SNTP_CLIENT_GET_SERVER_INFO, ///< Event for SNTP client get server info
  SL_SNTP_CLIENT_STOP             ///< Event for SNTP client stopped
} sl_sntp_client_events_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/

/**
 * @addtogroup SERVICE_SNTP_TYPES
 * @{
 */
/// SNTP Server Info response
typedef struct {
  uint8_t command_type; ///< command type
  uint8_t ip_version;   ///< Ip version of the SNTP server. 4 for IPV4 and 6 for IPV6
  union {
    uint8_t ipv4_address[4];       ///< Ipv4 address of the SNTP server
    unsigned long ipv6_address[4]; ///< Ipv6 address of the SNTP server
  } SL_ATTRIBUTE_PACKED server_ip_address;
  uint8_t sntp_method; ///< SNTP server method
} SL_ATTRIBUTE_PACKED sl_sntp_server_info_t;

/// SNTP Response
typedef struct {
  uint8_t event_type; ///< Type of event being received indicated by @ref sl_sntp_client_events_t
  sl_status_t
    status; ///< Status of the call which triggered this response. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
  uint8_t *
    data; ///< Pointer to response data. In case of SL_SNTP_CLIENT_GET_TIME, SL_SNTP_CLIENT_GET_TIME_DATE, it is a (uint8_t *) buffer and in case of SL_SNTP_CLIENT_GET_SERVER_INFO it is a pointer to @ref __attribute__ structure
  uint16_t data_length; ///< Length of response data
} sl_sntp_client_response_t;

/**
 * @typedef sl_sntp_client_event_handler_t
 * @brief SNTP response handler
 * @param[in] response         SNTP response of type @ref sl_sntp_client_response_t
 * @param[in] user_data        User data passed in sl_sntp_client_get_time(), sl_sntp_client_get_time_date(), sl_sntp_client_get_server_info() APIs.
 * @param[in] user_data_length User data length in sl_sntp_client_get_time(), sl_sntp_client_get_time_date(), sl_sntp_client_get_server_info() APIs
 */
typedef void (*sl_sntp_client_event_handler_t)(sl_sntp_client_response_t *response,
                                               uint8_t *user_data,
                                               uint16_t user_data_length);

/**
 * @typedef sl_sntp_set_time_sync_notification_handler_t
 * @brief SNTP time sync notification handler
 * @param[in] cmd_type command type
 * @param[in] status sl_status_t.See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @param[in] buffer Data buffer
 * @note This features is not supported currently in Si917 chipsets
 */
typedef void (*sl_sntp_set_time_sync_notification_handler_t)(const uint8_t cmd_type,
                                                             sl_status_t status,
                                                             const uint8_t *buffer);

/// SNTP configuration
typedef struct {
  uint8_t *server_host_name;                    ///< SNTP host name or address
  uint8_t sntp_method;                          ///< SNTP method. One of the defines from @ref SERVICE_SNTP_METHODS
  uint16_t sntp_timeout;                        ///< SNTP timeout.
  uint8_t flags;                                ///< SNTP flags. Combination of values from @ref SERVICE_SNTP_FLAGS.
                                                ///< @note Bits 1-7 are reserved. User must set them to NULL.
  sl_sntp_client_event_handler_t event_handler; ///< SNTP response handler of type @ref sl_sntp_client_event_handler_t
  sl_sntp_set_time_sync_notification_handler_t
    time_sync_notifiication_handler; ///< Set to true if smooth sync required of type @ref sl_sntp_set_time_sync_notification_handler_t
                                     ///< @note This feature is currently not supported in Si917 chipsets
  bool smooth_sync;                  ///< Set to true if smooth sync required.
                                     ///< @note This feature is currently not supported in Si917 chipsets
  bool server_from_dhcp;           ///< Set to true to request NTP server config from DHCP.
                                   ///< @note This feature is currently not supported in Si917 chipsets
  bool renew_servers_after_new_ip; ///< This is used to refresh server list if NTP provided by DHCP.
                                   ///< @note This feature is currently not supported in 917 chipsets
} sl_sntp_client_config_t;
/** @} */

/**
 * @addtogroup SERVICE_SNTP_FUNCTIONS
 * @{
 */

/**
 * @brief
 * Start SNTP client.
 * @param[in] config		Valid pointer to client configuration structure of type @ref sl_sntp_client_config_t . This shall not be null.
 * @param[in] timeout       Timeout for starting SNTP client. This is blocking API for timeout > 0,  else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API needs to be called before calling any other SNTP API
 */
sl_status_t sl_sntp_client_start(sl_sntp_client_config_t *config, uint32_t timeout);

/**
 * @brief
 * Get the current NTP epoch time in seconds.
 * @param[in] data		    Valid pointer to data buffer which should be greater than or equal to 50 bytes. In synchronous case, time is returned in this buffer in string format. In async call, this parameter is given to user in user_data parameter of event handler.
 * @param[in] data_length   Data buffer length. In async call, this parameter is given to user in user_data_length parameter of event handler.
 * @param[in] timeout       Timeout for getting time. This is blocking API for timeout > 0, else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_get_time(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * @brief
 * Get time and date information from NTP.
 * @param[in] data		      Valid pointer to data buffer which should be greater than or equal to 50 bytes. In synchronous case, the date and time is returned in this buffer in string format. In async call, this parameter is given to user in user_data parameter of event handler.
 * @param[in] data_length   Data buffer length. In async call, this parameter is given to user in user_data_length parameter of event handler.
 * @param[in] timeout       Timeout for getting time and date. This is blocking API for timeout > 0, else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_get_time_date(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * @brief
 * Get NTP server information.
 * @param[in] data		      Valid pointer to data buffer of type @ref __attribute__. In async call, this parameter is given to user in user_data with its the structure size (i.e sizeof( @ref sl_sntp_client_event_handler_t )) in user_data_length parameter of event handler.
 * @param[in] timeout       Timeout for getting server information. This is blocking API for timeout > 0,  else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_get_server_info(sl_sntp_server_info_t *data, uint32_t timeout);

/**
 * @brief
 * Stop SNTP client.
 * @param[in] timeout          Timeout for stop SNTP client. This is blocking API for timeout > 0, else results are returned in @ref sl_sntp_client_event_handler_t callback
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_sntp_client_stop(uint32_t timeout);
/** @} */
