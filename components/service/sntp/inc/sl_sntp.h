#pragma once
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>

//SNTP Method types
#define SL_SNTP_BROADCAST_MODE 1
#define SL_SNTP_UNICAST_MODE   2

typedef enum {
  SL_SNTP_CLIENT_START = 1,
  SL_SNTP_CLIENT_GET_TIME_DATE,
  SL_SNTP_CLIENT_GET_SERVER_INFO,
  SL_SNTP_CLIENT_STOP
} sl_sntp_client_events_t;

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
/// SNTP time sync notification handler. Coming soon
typedef void (*sl_sntp_set_time_sync_notification_handler_t)(const uint8_t cmd_type,
                                                             sl_status_t status,
                                                             const uint8_t *buffer);

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

/**
 * Start SNTP client. This API needs to be called before calling any other SNTP API.
 * @param config		Valid pointer to client configuration structure. This shall not be null.
 * @param timeout       Timeout for starting SNTP client.
 * @return				status of the API call.
 */
sl_status_t sl_sntp_client_start(sl_sntp_client_config_t *config, uint32_t timeout);

/**
 * Get time and date information from NTP.
 * @param data		    Valid pointer to data buffer.
 * @param data_length   Data buffer length.
 * @param timeout       Timeout for starting SNTP client.
 * @return				status of the API call.
 */
sl_status_t sl_sntp_client_get_time_date(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * Get NTP server information.
 * @param data		    Valid pointer to data buffer.
 * @param data_length   Data buffer length.
 * @param timeout       Timeout for starting SNTP client.
 * @return				status of the API call.
 */
sl_status_t sl_sntp_client_get_server_info(uint8_t *data, uint16_t data_length, uint32_t timeout);

/**
 * Stop SNTP client.
 * @param None          No input parameters
 * @return				status of the API call.
 */
sl_status_t sl_sntp_client_stop(uint32_t timeout);