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
#include "sl_wifi_types.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "cmsis_os2.h"
#include "sl_si91x_socket_types.h"
#include <stdbool.h>

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/**
 *  @addtogroup SERVICE_HTTP_CLIENT_CONSTANTS 
 *  @{
 */

/// MAX buffer length for write data.
#define SL_HTTP_CLIENT_MAX_WRITE_BUFFER_LENGTH 900

/// HTTPS client Certificate Index constants.
#define SL_HTTPS_CLIENT_DEFAULT_CERTIFICATE_INDEX 0

/// HTTPS CLIENT 1st Certificate Index
#define SL_HTTPS_CLIENT_CERTIFICATE_INDEX_1 1

/// HTTPS CLIENT 2nd Certificate Index
#define SL_HTTPS_CLIENT_CERTIFICATE_INDEX_2 2

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * @brief HTTP client methods.
 * @note 917 does not support SL_HTTP_HEAD and SL_HTTP_DELETE in current release.
 */
typedef enum {
  SL_HTTP_GET    = 0, ///< HTTP Request Method type GET
  SL_HTTP_POST   = 1, ///< HTTP Request Method type POST
  SL_HTTP_HEAD   = 2, ///< HTTP Request Method type HEAD
  SL_HTTP_PUT    = 3, ///< HTTP Request Method type PUT
  SL_HTTP_DELETE = 4  ///< HTTP Request Method type DELETE
} sl_http_client_method_type_t;

/**
 * @brief HTTPS client TLS versions
 */
typedef enum {
  SL_TLS_V_1_0 = 0, ///< Use TLS Version 1.0 for HTTP Client
  SL_TLS_V_1_1 = 1, ///< Use TLS Version 1.1 for HTTP Client
  SL_TLS_V_1_2 = 2, ///< Use TLS Version 1.2 for HTTP Client
  SL_TLS_V_1_3 = 3  ///< Use TLS Version 1.3 for HTTP Client
} sl_http_client_tls_version_t;

/// HTTP versions.
typedef enum {
  SL_HTTP_V_1_0 = 0, ///< Use HTTP Protocol version 1.0
  SL_HTTP_V_1_1 = 1  ///< Use HTTP Protocol version 1.1
} sl_http_client_version_t;

/// HTTP client events.
typedef enum {
  SL_HTTP_CLIENT_GET_RESPONSE_EVENT = 0, ///< HTTP Client GET response Event
  SL_HTTP_CLIENT_POST_RESPONSE_EVENT,    ///< HTTP Client POST response Event
  SL_HTTP_CLIENT_PUT_RESPONSE_EVENT,     ///< HTTP Client PUT response Event
  SL_HTTP_CLIENT_MAX_EVENT               ///< HTTP Client Maximum number of events
} sl_http_client_event_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/

/**
 * @addtogroup SERVICE_HTTP_CLIENT_TYPES
 * @{
 */

/// HTTP Client handle
typedef uint32_t sl_http_client_t;

/**
 * @typedef sl_http_client_event_handler_t
 * @brief
 *    Callback invoked when an HTTP response is received.
 * @param[out] client
 *    HTTP client handle of type @ref sl_http_client_t
 * @param[out] event
 *    HTTP event which has occurred of type @ref sl_http_client_event_t
 * @param[out] data
 *    HTTP response data.
 * @param[out] request_context
 *    User defined context pointer.
 * @return
 *    sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
typedef sl_status_t (*sl_http_client_event_handler_t)(const sl_http_client_t *client,
                                                      sl_http_client_event_t event,
                                                      void *data,
                                                      void *request_context);

/******************************************************
 *                    Structures
 ******************************************************/
/// HTTP client credentials
typedef struct {
  uint16_t username_length; ///< Length of username. Maximum supported length of username is 139 bytes
  uint16_t password_length; ///< Length of password. Maximum supported length of password is 139 bytes
  uint8_t data[];           ///< A flexible array to store both username and password.
} sl_http_client_credentials_t;

/// HTTP client configurations
typedef struct {
  uint8_t certificate_index;                ///< HTTPS client Certificate index.
  sl_http_client_tls_version_t tls_version; ///< HTTP client TLS version. @ref sl_http_client_tls_version_t
  sl_http_client_version_t http_version;    ///< HTTP version. @ref sl_http_client_version_t
  bool https_enable;                        ///< HTTP security option.
  bool https_use_sni;                       ///< HTTPs SNI extension
  sl_ip_address_type_t ip_version;          ///< HTTP client IP version. @ref sl_ip_address_type_t
  sl_net_interface_t
    network_interface; ///< HTTP client network interface.[sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t)
} sl_http_client_configuration_t;

/// Structure of HTTP client extended header node.
typedef struct sl_http_client_header_s {
  struct sl_http_client_header_s *next; ///< Link to the next client header.
  char *key;                            ///< Header key name.
  char *value;                          ///< Header value.
} sl_http_client_header_t;

/// HTTP client request configurations
typedef struct {
  sl_http_client_method_type_t http_method_type; ///< HTTP request method. @ref sl_http_client_method_type_t
  uint8_t *ip_address;                           ///< HTTP server IP address.
  uint8_t *
    resource; ///< URL string for requested resource. The maximum supported HTTP URL is 2048 bytes, when the SL_SI91X_FEAT_LONG_HTTP_URL Bit is enabled in the feature_bit_map. If the SL_SI91X_FEAT_LONG_HTTP_URL Bit is disabled then the maximum supported length for HTTP URL is
  ///< (872-(length of User_name + length of Password) - length of hostname - length of IP address) bytes excluding delimiters.
  uint16_t port;                                   ///< HTTP server port number.
  si91x_socket_type_length_value_t *sni_extension; ///< SNI extension address. @ref si91x_socket_type_length_value_t
  uint8_t
    *body; ///< HTTP body to be sent to the server. Setting this to null will process the request in chunked encoding.
  uint32_t
    body_length; ///< Body length of data to be posted. In case of chunked request, body length should be equal to total content length.
  sl_http_client_header_t *
    extended_header; ///< User-defined extended header. If null, default extended header will be added internally. @ref sl_http_client_header_t
  uint16_t timeout_ms; ///< HTTP request timeout period. (Si91x chipsets does not support this feature).
  uint16_t
    retry_count; ///< HTTP request maximum retry count after timeout. (Si91x chipsets does not support this feature).
  uint16_t
    retry_period_ms; ///< Retry period after max retry count reach. (Si91x chipsets does not support this feature).
  bool
    tcp_connection_reuse; ///< Flag to use same TCP socket for connection. (Si91x chipsets does not support this feature).
  void *context;          ///< User defined context.
  sl_http_client_event_handler_t event_handler; ///< Callback method. @ref sl_http_client_event_handler_t
  uint8_t *
    host_name; ///< The hostname of the HTTP server, as specified in the request header. If the hostname is NULL, then the ip_address is used instead of the hostname.
} sl_http_client_request_t;

/// HTTP client response structure
typedef struct {
  uint32_t status;      ///< Request status.
  uint8_t *data_buffer; ///< Response data.
  uint16_t data_length; ///< Length of received data.
  uint32_t end_of_data; ///< End of data indication.
  uint16_t
    http_response_code; ///< HTTP server response. (Si91x chipsets does not support this feature for SL_HTTP_PUT).
  uint8_t version;      ///< HTTP version. (Si91x chipsets does not support this feature)
  sl_http_client_header_t *
    response_headers; ///< HTTP response headers. @ref sl_http_client_header_t  (Si91x chipsets does not support this feature).
} sl_http_client_response_t;

/** @} */

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
 * @addtogroup SERVICE_HTTP_CLIENT_FUNCTIONS
 * @{
 */

/***************************************************************************/ /**
 * @brief
 *   Initialize an HTTP client.
 * @param[in] configuration
 *   HTTP client configuration of type @ref sl_http_client_configuration_t
 * @param[out] client
 *   @ref sl_http_client_t object which will be initialized with client handle.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_http_client_init(const sl_http_client_configuration_t *configuration, sl_http_client_t *client);

/***************************************************************************/ /**
 * @brief
 *   Deinitialize an HTTP client and release resources used by HTTP client.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_http_client_init should be called before this API.
 * @param[in] client
 *   HTTP client's handle of type @ref sl_http_client_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   User must call this API to release the resources once the HTTP client is no longer needed. 
 *   This function deletes extended headers if any exists during deinitialization.
 ******************************************************************************/
sl_status_t sl_http_client_deinit(sl_http_client_t *client);

/***************************************************************************/ /**
 * @brief
 *   Initialize a callback function for the requested HTTP method.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_http_client_init should be called before this API.
 * @param[in] request
 *   HTTP client request configuration of type @ref sl_http_client_request_t
 * @param[in] event_handler
 *   HTTP client callback event handler of type @ref sl_http_client_event_handler_t
 * @param[in] request_context
 *   User defined context pointer. 
 *   Memory space for the user defined context must be valid till the response is received.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_http_client_request_init(sl_http_client_request_t *request,
                                        sl_http_client_event_handler_t event_handler,
                                        void *request_context);

/***************************************************************************/ /**
 * @brief
 *   Add extended header with key and value in client request.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_http_client_init should be called before this API.
 * @param[in] request
 *   HTTP client request configuration of type @ref sl_http_client_request_t
 * @param[in] key
 *   Header key.
 * @param[in] value
 *   Header value.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   If a request does not contain any extended header, this API allocates memory for it.
 ******************************************************************************/
sl_status_t sl_http_client_add_header(sl_http_client_request_t *request, const char *key, const char *value);

/***************************************************************************/ /**
 * @brief
 *   Delete a specified header field from the extended header based on key of an HTTP client request.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_http_client_add_header should be called before this API.
 * @param[in] request
 *   HTTP client request configuration of type @ref sl_http_client_request_t
 * @param[in] key
 *   Header key.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_http_client_delete_header(sl_http_client_request_t *request, const char *key);

/***************************************************************************/ /**
 * @brief
 *   Delete all the headers from the extended header of an HTTP client request.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_http_client_add_header should be called before this API.
 * @param[in] request
 *   HTTP client request configuration of type @ref sl_http_client_request_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   User must call this API to free the memory allocated to headers.
 ******************************************************************************/
sl_status_t sl_http_client_delete_all_headers(sl_http_client_request_t *request);

/***************************************************************************/ /**
 * @brief
 *   Send HTTP request.
 * @pre Pre-conditions:
 * -
 *   @ref sl_http_client_request_init should be called before this API.
 * @param[in] client
 *   HTTP client handle of type @ref sl_http_client_t
 * @param[in] request
 *   HTTP client request configuration of type @ref sl_http_client_request_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *   This status only indicates the library status.
 * @note
 *   1. HTTP HEAD and DELETE methods are not supported in Si91x specific chipsets.
 *   2. body_length header in request by default internally in Si91x specific chipsets.
 *   3. HTTP PUT does not support sending body through this API, it is mandatory to call sl_http_client_write_chunked_data() in Si91x specific chipsets.
 *   4. HTTP response status and response code e.g., 200, 201, 404, etc will be returned in the corresponding event_handler registered during sl_http_client_request_init().
 ******************************************************************************/
sl_status_t sl_http_client_send_request(const sl_http_client_t *client, const sl_http_client_request_t *request);

/***************************************************************************/ /**
 * @brief
 *   Send an HTTP POST and PUT chunked data.
 * @pre Pre-conditions:
 * -
 *   @ref sl_http_client_send_request should be called before this API.
 * @param[in] client
 *   HTTP client handle of type @ref sl_http_client_t
 * @param[in] data
 *   Buffer pointer of data to be written.
 * @param[in] data_length
 *    Length of data/chunk to be sent.
 * @param[in] flush_now
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note:
 * flush_now feature is not supported in Si91x specific chipsets.
 ******************************************************************************/
sl_status_t sl_http_client_write_chunked_data(const sl_http_client_t *client,
                                              uint8_t *data,
                                              uint32_t data_length,
                                              bool flush_now);
/** @} */
