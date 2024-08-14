/*******************************************************************************
 * # License
 * Copyright 2024 Silicon Laboratories Inc. www.silabs.com
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
#ifndef SL_HTTP_SERVER_TYPES_H
#define SL_HTTP_SERVER_TYPES_H

#include "sl_status.h"
#include "sl_wifi_types.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "cmsis_os2.h"
#include "sl_si91x_socket_types.h"
#include <stdbool.h>
#include <stdint.h>

/******************************************************
 *                    Constants
 ******************************************************/

/**
 *  @addtogroup SERVICE_HTTP_SERVER_CONSTANTS 
 *  @{
 */

/// Content type for CSS files.
#define SL_HTTP_CONTENT_TYPE_TEXT_CSS "text/css"

/// Content type for CSV files.
#define SL_HTTP_CONTENT_TYPE_TEXT_CSV "text/csv"

/// Content type for HTML files.
#define SL_HTTP_CONTENT_TYPE_TEXT_HTML "text/html"

/// Content type for JavaScript files.
#define SL_HTTP_CONTENT_TYPE_TEXT_JAVASCRIPT "text/javascript"

/// Content type for plain text files.
#define SL_HTTP_CONTENT_TYPE_TEXT_PLAIN "text/plain"

/// Content type for XML files.
#define SL_HTTP_CONTENT_TYPE_TEXT_XML "text/xml"

/// Content type for PDF files.
#define SL_HTTP_CONTENT_TYPE_APPLICATION_PDF "application/pdf"

/// Content type for JSON files.
#define SL_HTTP_CONTENT_TYPE_APPLICATION_JSON "application/json"

/// Content type for XML files.
#define SL_HTTP_CONTENT_TYPE_APPLICATION_XML "application/xml"

/// Content type for PDF files.
#define SL_HTTP_CONTENT_TYPE_APPLICATION_PDF "application/pdf"

/// Maximum number of query parameters in a URI.
#define MAX_QUERY_PARAMETERS 4

/// Maximum length of the header buffer.
#define MAX_HEADER_BUFFER_LENGTH 1024

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * @brief HTTP server response types.
 * @note These are the standard HTTP/1.1 response status codes.
 */
typedef enum {
  SL_HTTP_RESPONSE_OK                         = 200, ///< Success response code
  SL_HTTP_RESPONSE_BAD_REQUEST                = 400, ///< Bad request response code
  SL_HTTP_RESPONSE_UNAUTHORIZED               = 401, ///< Unauthorized response code
  SL_HTTP_RESPONSE_FORBIDDEN                  = 403, ///< Forbidden response code
  SL_HTTP_RESPONSE_NOT_FOUND                  = 404, ///< Not found response code
  SL_HTTP_RESPONSE_METHOD_NOT_ALLOWED         = 405, ///< Method not allowed response code
  SL_HTTP_RESPONSE_INTERNAL_SERVER_ERROR      = 500, ///< Internal server error response code
  SL_HTTP_RESPONSE_NOT_IMPLEMENTED            = 501, ///< Not implemented response code
  SL_HTTP_RESPONSE_SERVICE_UNAVAILABLE        = 503, ///< Service unavailable response code
  SL_HTTP_RESPONSE_HTTP_VERSION_NOT_SUPPORTED = 505, ///< HTTP version not supported response code
} sl_http_response_code_t;

/**
 * @brief HTTP request versions.
 */
typedef enum {
  SL_HTTP_VERSION_1_0, ///< HTTP Version 1.0
  SL_HTTP_VERSION_1_1  ///< HTTP Version 1.1
} sl_http_request_version_t;

/**
 * @brief HTTP server request types.
 */
typedef enum {
  SL_HTTP_REQUEST_GET,    ///< GET request type
  SL_HTTP_REQUEST_POST,   ///< POST request type
  SL_HTTP_REQUEST_PUT,    ///< PUT request type
  SL_HTTP_REQUEST_DELETE, ///< DELETE request type
  SL_HTTP_REQUEST_HEAD    ///< HEAD request type
} sl_http_server_request_type_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/

// Forward declaration of the type
typedef struct sl_http_server_s sl_http_server_t;
typedef struct sl_http_server_request_s sl_http_server_request_t;

/**
 * @addtogroup SERVICE_HTTP_SERVER_TYPES
 * @{
 */

/**
 * @typedef sl_http_request_handler_t
 * @brief
 *    Callback invoked when an HTTP request is received.
 * @param[out] handle
 *    HTTP server handle of type @ref sl_http_server_t
 * @param[out] request
 *    HTTP request which has been received of type @ref sl_http_server_request_t
 * @return
 *    sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
typedef sl_status_t (*sl_http_request_handler_t)(sl_http_server_t *handle, sl_http_server_request_t *request);

/******************************************************
 *                    Structures
 ******************************************************/

/// HTTP header parameters in the form of key and value
typedef struct {
  char *key;   ///< Key of the header
  char *value; ///< Value of the header
} sl_http_header_t;

/// HTTP server response parameters
typedef struct {
  sl_http_response_code_t response_code; ///< HTTP response code
  char *content_type;                    ///< Content type of the response
  sl_http_header_t *headers;             ///< Pointer to an array of headers
  uint16_t header_count;                 ///< Length of header array
  uint8_t *data;                         ///< Pointer to the data buffer
  uint32_t current_data_length;          ///< Length of the data buffer
  uint32_t expected_data_length;         ///< Total length of the data to be sent in response
} sl_http_server_response_t;

/// HTTP server uri query parametrs
typedef struct {
  char *query; ///< URI path
  char *value; ///< HTTP protocol version
} sl_http_server_uri_query_parameter_t;

/// HTTP server request URI parameters
typedef struct {
  char *path;                                                                  ///< URI path
  sl_http_server_uri_query_parameter_t query_parameters[MAX_QUERY_PARAMETERS]; ///< Array of query parameters
  uint16_t query_parameter_count; ///< Number of query parameters received in URI
} sl_http_server_request_uri_t;

/// HTTP server request parameters
typedef struct sl_http_server_request_s {
  sl_http_server_request_uri_t uri;   ///< URI of the request
  sl_http_request_version_t version;  ///< HTTP protocol version
  uint16_t request_header_count;      ///< Number of request headers received
  sl_http_server_request_type_t type; ///< Type of the request (GET, POST, etc.)
  uint32_t request_data_length;       ///< Length of the request data
} sl_http_server_request_t;

/// HTTP server expected request handling parameters
typedef struct {
  char *uri;                         ///< URI pattern to match the request
  sl_http_request_handler_t handler; ///< Request handler function
} sl_http_server_handler_t;

/// HTTP server configuration
typedef struct {
  uint16_t port;                             ///< Port number of the HTTP server
  sl_http_server_handler_t *handlers_list;   ///< Array of request handlers
  uint16_t handlers_count;                   ///< Length of request handlers array
  sl_http_request_handler_t default_handler; ///< Default handler
  uint16_t client_idle_time;                 ///< Idle duration in seconds before the client is considered inactive.
} sl_http_server_config_t;

/// HTTP server handle used to manage all HTTP server functions
typedef struct sl_http_server_s {
  sl_http_server_config_t config;                ///< Configuration settings for the HTTP server.
  int server_socket;                             ///< Socket descriptor for the server.
  int client_socket;                             ///< Socket descriptor for the client currently being served.
  osEventFlagsId_t http_server_id;               ///< Event ID for the HTTP server, used for synchronization.
  sl_http_server_request_t request;              ///< Current HTTP request being processed.
  char request_buffer[MAX_HEADER_BUFFER_LENGTH]; ///< Buffer for storing the HTTP request.
  char *header;                                  ///< Pointer to string containing headers.
  uint8_t *req_data;                             ///< Pointer to the data of the HTTP request.
  uint32_t data_length;                          ///< Length of the request data.
  uint32_t rem_len;                              ///< Remaining length of data to be processed in the request.
  bool response_sent;       ///< Flag indicating whether the response has been sent for the current request.
  uint32_t rem_resp_length; ///< Remaining length of data to be sent in the response.
} sl_http_server_t;

/// HTTP read data parameters
typedef struct {
  sl_http_server_request_t *request; ///< Pointer to request structure received in the request handler
  uint8_t *buffer;                   ///< Pointer to user defined memory buffer
  uint32_t buffer_length;            ///< Length of user defined memory buffer given
  uint32_t received_data_length;     ///< Amount of data received in the read operation
} sl_http_recv_req_data_t;

/** @} */

#endif //SL_HTTP_SERVER_TYPES_H
