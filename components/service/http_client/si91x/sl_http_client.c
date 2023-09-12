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

#include "sl_http_client.h"
#include "sl_constants.h"
#include "sl_slist.h"
#include "sl_net.h"
#include "sl_net_constants.h"
#include "sl_wifi_constants.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_protocol_types.h"
#include "sl_net_rsi_utility.h"
#include "sl_si91x_http_client_callback_framework.h"

/******************************************************
 *                      Macros
 ******************************************************/
// Macro to check if malloc failed
#define VERIFY_MALLOC_AND_RETURN(ptr)     \
  {                                       \
    if (ptr == NULL) {                    \
      return SL_STATUS_ALLOCATION_FAILED; \
    }                                     \
  }

#define VERIFY_MALLOC_AND_FREE_IF_FAIL(ptr, ptr1, ptr2) \
  {                                                     \
    if (ptr == NULL) {                                  \
      if (ptr1 != NULL)                                 \
        free(ptr1);                                     \
      if (ptr2 != NULL)                                 \
        free(ptr2);                                     \
      return SL_STATUS_ALLOCATION_FAILED;               \
    }                                                   \
  }

/******************************************************
 *                    Constants
 ******************************************************/
//! HTTP client state
typedef enum {
  HTTP_STATE_DEINITIALIZED = 0,        ///< HTTP client deinitialized state
  HTTP_STATE_INITIALIZED,              ///< HTTP client initialized state
  HTTP_STATE_GET_REQUEST_INITIALIZED,  ///< HTTP client state after GET request initialized
  HTTP_STATE_POST_REQUEST_INITIALIZED, ///< HTTP client state after POST request initialized
  HTTP_STATE_PUT_REQUEST_INITIALIZED,  ///< HTTP client state after PUT request initialized
  HTTP_STATE_REQUEST_SENT,             ///< HTTP client state after GET/POST/PUT request sent
  HTTP_STATE_CHUNKED_REQUEST_SENT      ///< HTTP client state after sending chunked request
} sl_http_client_state_t;

//! MAX supported length for Username and Password together
#define SI91X_MAX_SUPPORTED_HTTP_CREDENTIAL_LENGTH 278

/******************************************************
 *                    Structures
 ******************************************************/
//! HTTP client internal context
typedef struct {
  sl_http_client_credentials_t *client_credentials; ///< HTTP client credentials
  sl_http_client_configuration_t configuration;     ///< HTTP client configurations
  sl_http_client_request_t request;                 ///< HTTP client request configurations
  sl_http_client_state_t client_state;              ///< HTTP client state
} sl_http_client_internal_t;

/******************************************************
 *                 Global Variables
 ******************************************************/
static sl_http_client_internal_t http_client_handle = { 0 };

extern bool device_initialized;

/******************************************************
 *               Function Declarations
 ******************************************************/
// Validates HTTP clients request configurations
static sl_status_t validate_http_client_request(const sl_http_client_request_t *request);

// Copy extended headers into HTTP request buffer
static void load_extended_headers_into_request_buffer(uint8_t *buffer,
                                                      sl_http_client_header_t *extended_header,
                                                      uint16_t *http_buffer_offset);

// Sends GET/POST request
static sl_status_t send_http_client_request(sl_http_client_method_type_t send_request,
                                            sl_http_client_internal_t *client_internal,
                                            const sl_http_client_request_t *request);

static sl_status_t sl_si91x_http_client_abort(void);

/******************************************************
 *               Function Definitions
 ******************************************************/
sl_status_t sl_http_client_init(const sl_http_client_configuration_t *client_configuration, sl_http_client_t *client)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  ARGS_CHECK_NULL_POINTER(client_configuration);
  ARGS_CHECK_NULL_POINTER(client);

  // Only one http client supported in current release
  if (http_client_handle.client_state != HTTP_STATE_DEINITIALIZED) {
    return SL_STATUS_ALREADY_EXISTS;
  }

  // Check https configurations
  if (client_configuration->certificate_index > SL_HTTPS_CLIENT_CERTIFICATE_INDEX_2) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  // Check HTTP version
  if ((client_configuration->http_version != SL_HTTP_V_1_0) && (client_configuration->http_version != SL_HTTP_V_1_1)) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }

  // Check SSL/TLS version
  switch (client_configuration->tls_version) {
    case SL_TLS_V_1_0:
    case SL_TLS_V_1_1:
    case SL_TLS_V_1_2: {
      break;
    }
    case SL_TLS_V_1_3: {
#ifdef CHIP_917
      break;
#else
      return SL_STATUS_NOT_SUPPORTED;
#endif
    }
    default: {
      return SL_STATUS_INVALID_CONFIGURATION;
    }
  }

  // Check for network interface
  if ((client_configuration->network_interface != SL_NET_WIFI_CLIENT_INTERFACE)
      && (client_configuration->network_interface != SL_NET_WIFI_AP_INTERFACE)) {
    return SL_STATUS_INVALID_MODE;
  }

  // Store client configurations into internal configurations
  memcpy(&http_client_handle.configuration, client_configuration, sizeof(sl_http_client_configuration_t));

  sl_net_credential_type_t type;
  uint32_t max_credential_size = sizeof(sl_http_client_credentials_t) + SI91X_MAX_SUPPORTED_HTTP_CREDENTIAL_LENGTH;

  http_client_handle.client_credentials = (sl_http_client_credentials_t *)malloc(max_credential_size);
  VERIFY_MALLOC_AND_RETURN(http_client_handle.client_credentials);

  sl_status_t status = sl_net_get_credential(SL_NET_HTTP_CLIENT_CREDENTIAL_ID,
                                             &type,
                                             http_client_handle.client_credentials,
                                             &max_credential_size);

  if (status != SL_STATUS_OK || type != SL_NET_HTTP_CLIENT_CREDENTIALS) {
    return status != SL_STATUS_OK ? status : SL_STATUS_INVALID_CREDENTIALS;
  }

  // Set HTTP client state
  http_client_handle.client_state = HTTP_STATE_INITIALIZED;

  // Copy address of HTTP client internal handle
  *client = (sl_http_client_t)&http_client_handle;

  return SL_STATUS_OK;
}

sl_status_t sl_http_client_deinit(sl_http_client_t *client)
{
  ARGS_CHECK_NULL_POINTER(client);

  sl_status_t status = SL_STATUS_OK;

  if (http_client_handle.client_state == HTTP_STATE_DEINITIALIZED) {
    return SL_STATUS_INVALID_STATE;
  }

  if (*client != (sl_http_client_t)&http_client_handle) {
    return SL_STATUS_INVALID_HANDLE;
  }

  // Free HTTP client credentials
  if (http_client_handle.client_credentials != NULL) {
    free(http_client_handle.client_credentials);
  }

  // Free extended headers
  if (http_client_handle.request.extended_header != NULL) {
    status = sl_http_client_delete_all_headers(&http_client_handle.request);
    VERIFY_STATUS_AND_RETURN(status);
  }

  memset(&http_client_handle, 0, sizeof(sl_http_client_internal_t));

  status = sl_si91x_http_client_abort();

  return status;
}

static sl_status_t validate_http_client_request(const sl_http_client_request_t *request)
{
  // Validate host name
  ARGS_CHECK_NULL_POINTER(request->ip_address);

  // Validate resource
  ARGS_CHECK_NULL_POINTER(request->resource);

  // 917 does not support HTTP POST and PUT request with body_length as 0 in Alpha 3 release
  // Bug in TA, when SL_HTTP_POST is requested with body_length = 0, TA sends SL_HTTP_GET
  // Bug in TA, when SL_HTTP_PUT is requested with body_length = 0, TA doesn't send end of data to host so currently there is no way to handle put_delete()
  // Below check should be removed once the above TA bugs are fixed
  if (request->http_method_type != SL_HTTP_GET && request->body_length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Validate http request method
  switch (request->http_method_type) {
    case SL_HTTP_GET:
    case SL_HTTP_POST:
    case SL_HTTP_PUT: {
      break;
    }
    case SL_HTTP_HEAD:
    case SL_HTTP_DELETE: {
      // Not supported in current release
      return SL_STATUS_NOT_SUPPORTED;
    }
    default: {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  return SL_STATUS_OK;
}

sl_status_t sl_http_client_request_init(sl_http_client_request_t *request,
                                        sl_http_client_event_handler event_handler,
                                        void *request_context)
{
  ARGS_CHECK_NULL_POINTER(request);
  ARGS_CHECK_NULL_POINTER(event_handler);

  sl_status_t status = SL_STATUS_OK;

  // Validate HTTP client request
  status = validate_http_client_request(request);
  VERIFY_STATUS_AND_RETURN(status);

  request->event_handler = event_handler;
  request->context       = request_context;

  sl_http_client_event_t http_event;

  // Set HTTP client event for requested method
  switch (request->http_method_type) {
    case SL_HTTP_GET: {
      http_event                      = SL_HTTP_CLIENT_GET_RESPONSE_EVENT;
      http_client_handle.client_state = HTTP_STATE_GET_REQUEST_INITIALIZED;
      break;
    }
    case SL_HTTP_POST: {
      http_event                      = SL_HTTP_CLIENT_POST_RESPONSE_EVENT;
      http_client_handle.client_state = HTTP_STATE_POST_REQUEST_INITIALIZED;
      break;
    }
    case SL_HTTP_PUT: {
      http_event                      = SL_HTTP_CLIENT_PUT_RESPONSE_EVENT;
      http_client_handle.client_state = HTTP_STATE_PUT_REQUEST_INITIALIZED;
      break;
    }
    default: {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Register callback function for the http event
  status = sl_http_client_register_callback(http_event, (sl_http_client_t)&http_client_handle, event_handler);

  return status;
}

sl_status_t sl_http_client_add_header(sl_http_client_request_t *request, const char *key, const char *value)
{
  ARGS_CHECK_NULL_POINTER(request);
  ARGS_CHECK_NULL_POINTER(key);
  ARGS_CHECK_NULL_POINTER(value);

  // Allocate memory for the new header
  sl_http_client_header_t *new_header = (sl_http_client_header_t *)malloc(sizeof(sl_http_client_header_t));
  VERIFY_MALLOC_AND_RETURN(new_header);

  // Memset the heap chunk
  memset(new_header, 0, sizeof(sl_http_client_header_t));

  // Allocate memory for key
  new_header->key = (char *)malloc(strlen(key) + 1);
  VERIFY_MALLOC_AND_FREE_IF_FAIL(new_header->key, new_header, NULL);

  // Copy key in header
  snprintf(new_header->key, strlen(key) + 1, "%s", key);

  // Allocate memory for value
  new_header->value = (char *)malloc(strlen(value) + 1);
  VERIFY_MALLOC_AND_FREE_IF_FAIL(new_header->value, new_header, new_header->key);

  // Copy value in header
  snprintf(new_header->value, strlen(value) + 1, "%s", value);

  // Add new header at start of linked list
  sl_slist_push((sl_slist_node_t **)&request->extended_header, (sl_slist_node_t *)new_header);

  return SL_STATUS_OK;
}

sl_status_t sl_http_client_delete_header(sl_http_client_request_t *request, const char *key)
{
  ARGS_CHECK_NULL_POINTER(request);
  ARGS_CHECK_NULL_POINTER(key);

  sl_http_client_header_t *current_header = request->extended_header;

  // Check if linked list is empty
  if (request->extended_header == NULL) {
    return SL_STATUS_EMPTY;
  }

  // Search key in linked list
  while ((current_header != NULL) && (strcmp(current_header->key, key) != 0)) {
    current_header = current_header->next;
  }

  // If key not present in linked list
  if (current_header == request->extended_header) {
    return SL_STATUS_INVALID_KEY;
  }

  // Unlink node from the linked list
  sl_slist_remove((sl_slist_node_t **)&request->extended_header, (sl_slist_node_t *)current_header);

  free(current_header->key);
  free(current_header->value);
  free(current_header);

  return SL_STATUS_OK;
}

sl_status_t sl_http_client_delete_all_headers(sl_http_client_request_t *request)
{
  ARGS_CHECK_NULL_POINTER(request);

  sl_http_client_header_t *current_header = request->extended_header;
  sl_http_client_header_t *next_header    = NULL;

  // Check if linked list is empty
  if (request->extended_header == NULL) {
    return SL_STATUS_EMPTY;
  }

  while (current_header != NULL) {
    // Store next header link
    next_header = current_header->next;

    // Remove node from list
    sl_slist_remove((sl_slist_node_t **)&request->extended_header, (sl_slist_node_t *)current_header);

    // Free current node
    free(current_header->key);
    free(current_header->value);
    free(current_header);

    // Traverse to next node
    current_header = next_header;
  }

  // Set head node of a linked list to NULL
  request->extended_header                   = NULL;
  http_client_handle.request.extended_header = NULL;

  return SL_STATUS_OK;
}

static void load_extended_headers_into_request_buffer(uint8_t *buffer,
                                                      sl_http_client_header_t *extended_header,
                                                      uint16_t *http_buffer_offset)
{
  sl_http_client_header_t *current_header = extended_header;

  while (current_header != NULL) {
    // Copy header key
    *http_buffer_offset += snprintf((char *)(buffer + (*http_buffer_offset)),
                                    SI91X_HTTP_BUFFER_LEN - (*http_buffer_offset),
                                    "%s",
                                    current_header->key);

    buffer[(*http_buffer_offset)] = ':';
    (*http_buffer_offset)++;

    // Copy header value
    *http_buffer_offset += snprintf((char *)(buffer + (*http_buffer_offset)),
                                    SI91X_HTTP_BUFFER_LEN - (*http_buffer_offset),
                                    "%s",
                                    current_header->value);

    if (current_header->next == NULL) {
      buffer[(*http_buffer_offset)] = '\0';
      (*http_buffer_offset)++;
      break;
    } else {
      buffer[(*http_buffer_offset)] = '\r';
      (*http_buffer_offset)++;
      buffer[(*http_buffer_offset)] = '\n';
      (*http_buffer_offset)++;

      // Point to next header
      current_header = current_header->next;
    }
  }
}

static sl_status_t send_http_client_request(sl_http_client_method_type_t send_request,
                                            sl_http_client_internal_t *client_internal,
                                            const sl_http_client_request_t *request)
{
  sl_status_t status          = SL_STATUS_OK;
  uint32_t packet_length      = 0;
  uint16_t http_buffer_offset = 0;

  // Allocate memory for request structure
  sl_si91x_http_client_request_t *http_client_request =
    (sl_si91x_http_client_request_t *)malloc(sizeof(sl_si91x_http_client_request_t));
  VERIFY_MALLOC_AND_RETURN(http_client_request);

  memset(http_client_request, 0, sizeof(sl_si91x_http_client_request_t));

  // Fill IP version
  if (client_internal->configuration.ip_version == SL_IPV6) {
    http_client_request->ip_version = 6;
  } else {
    http_client_request->ip_version = 4;
  }

  // Set default by NULL delimiter
  http_client_request->https_enable |= SL_SI91X_ENABLE_NULL_DELIMETER;

  // Fill HTTPS feature
  if (client_internal->configuration.https_enable) {
    // Enable SSL/TLS
    http_client_request->https_enable |= SL_SI91X_ENABLE_TLS;

    // Fill SSL/TLS version
    switch (client_internal->configuration.tls_version) {
      case SL_TLS_V_1_0: {
        http_client_request->https_enable |= SL_SI91X_TLS_V_1_0;
        break;
      }
      case SL_TLS_V_1_1: {
        http_client_request->https_enable |= SL_SI91X_TLS_V_1_1;
        break;
      }
      case SL_TLS_V_1_2: {
        http_client_request->https_enable |= SL_SI91X_TLS_V_1_2;
        break;
      }
#ifdef CHIP_917
      case SL_TLS_V_1_3: {
        http_client_request->https_enable |= SL_SI91X_TLS_V_1_3;
        break;
      }
#endif
      default:
        return SL_STATUS_INVALID_CONFIGURATION;
    }

    // Fill HTTPS certificate index bitmap
    switch (client_internal->configuration.certificate_index) {
      case SL_HTTPS_CLIENT_CERTIFICATE_INDEX_1: {
        http_client_request->https_enable |= SL_SI91X_HTTPS_CERTIFICATE_INDEX_1;
        break;
      }
      case SL_HTTPS_CLIENT_CERTIFICATE_INDEX_2: {
        http_client_request->https_enable |= SL_SI91X_HTTPS_CERTIFICATE_INDEX_2;
        break;
      }
      case SL_HTTPS_CLIENT_DEFAULT_CERTIFICATE_INDEX:
      default:
        break;
    }
  }

  // Fill HTTP version
  if (client_internal->configuration.http_version == SL_HTTP_V_1_1) {
    http_client_request->https_enable |= SL_SI91X_HTTP_V_1_1;
  }

  // Fill port number
  http_client_request->port_number = request->port;

  // Fill username
  memcpy(http_client_request->buffer,
         &client_internal->client_credentials->data[0],
         client_internal->client_credentials->username_length);
  http_buffer_offset += client_internal->client_credentials->username_length;
  http_client_request->buffer[http_buffer_offset] = '\0';
  http_buffer_offset++;

  // Fill password
  memcpy(http_client_request->buffer + http_buffer_offset,
         &client_internal->client_credentials->data[client_internal->client_credentials->username_length],
         client_internal->client_credentials->password_length);
  http_buffer_offset += client_internal->client_credentials->password_length;
  http_client_request->buffer[http_buffer_offset] = '\0';
  http_buffer_offset++;

  // Fill hostname
  http_buffer_offset += snprintf((char *)(http_client_request->buffer + http_buffer_offset),
                                 SI91X_HTTP_BUFFER_LEN - http_buffer_offset,
                                 "%s",
                                 request->ip_address);
  http_buffer_offset++;

  // Fill IP address
  http_buffer_offset += snprintf((char *)(http_client_request->buffer + http_buffer_offset),
                                 SI91X_HTTP_BUFFER_LEN - http_buffer_offset,
                                 "%s",
                                 request->ip_address);
  http_buffer_offset++;

  // Fill URL resource
  http_buffer_offset += snprintf((char *)(http_client_request->buffer + http_buffer_offset),
                                 SI91X_HTTP_BUFFER_LEN - http_buffer_offset,
                                 "%s",
                                 request->resource);
  http_buffer_offset++;

  // Fill extended header
  if (request->extended_header != NULL) {
    // Enable user given content type in extended header
    http_client_request->https_enable |= SL_SI91X_HTTP_USER_DEFINED_CONTENT_TYPE;

    load_extended_headers_into_request_buffer(http_client_request->buffer,
                                              request->extended_header,
                                              &http_buffer_offset);
  } else {
    http_client_request->buffer[http_buffer_offset] = '\0';
    http_buffer_offset++;
  }

  if (send_request == SL_HTTP_POST) {
    // Check for HTTP post data feature
    if (request->body == NULL) {
      // Enable HTTP Post big data feature
      http_client_request->https_enable |= SL_SI91X_SUPPORT_HTTP_POST_DATA;

      // Copy total data length into buffer
      uint8_t temp_str[7] = { 0 };
      convert_itoa(request->body_length, temp_str);
      http_buffer_offset += snprintf((char *)(http_client_request->buffer + http_buffer_offset),
                                     SI91X_HTTP_BUFFER_LEN - http_buffer_offset,
                                     "%s",
                                     temp_str);
      http_buffer_offset++;
    } else {
      // Fill HTTP post data
      http_buffer_offset += snprintf((char *)(http_client_request->buffer + http_buffer_offset),
                                     SI91X_HTTP_BUFFER_LEN - http_buffer_offset,
                                     "%s",
                                     request->body);
      http_buffer_offset++;
    }
  }

  // Check if request buffer is overflowed
  if (http_buffer_offset > SI91X_HTTP_BUFFER_LEN) {
    free(http_client_request);
    return SL_STATUS_HAS_OVERFLOWED;
  }

  // Fill total packet length
  packet_length = sizeof(sl_si91x_http_client_request_t) - SI91X_HTTP_BUFFER_LEN + http_buffer_offset;

  // Send HTTP request
  if (send_request == SL_HTTP_POST) {
    // HTTP Post request
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_POST,
                                          SI91X_NETWORK_CMD_QUEUE,
                                          http_client_request,
                                          packet_length,
                                          SL_SI91X_RETURN_IMMEDIATELY,
                                          request->context,
                                          NULL);
  } else {
    // HTTP Get request
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_GET,
                                          SI91X_NETWORK_CMD_QUEUE,
                                          http_client_request,
                                          packet_length,
                                          SL_SI91X_RETURN_IMMEDIATELY,
                                          request->context,
                                          NULL);
  }

  // Free request structure memory
  free(http_client_request);

  return status;
}

sl_status_t sl_http_client_send_request(const sl_http_client_t *client, const sl_http_client_request_t *request)
{
  ARGS_CHECK_NULL_POINTER(client);
  ARGS_CHECK_NULL_POINTER(request);

  if (*client != (sl_http_client_t)&http_client_handle) {
    return SL_STATUS_INVALID_HANDLE;
  }

  // Validate HTTP client request
  sl_status_t status = validate_http_client_request(request);
  VERIFY_STATUS_AND_RETURN(status);

  uint32_t packet_length      = 0;
  uint16_t http_buffer_offset = 0;

  switch (request->http_method_type) {
    case SL_HTTP_GET: {
      // Validate HTTP client state
      if (http_client_handle.client_state != HTTP_STATE_GET_REQUEST_INITIALIZED) {
        return SL_STATUS_INVALID_STATE;
      }

      //! Send HTTP client GET request
      status = send_http_client_request(SL_HTTP_GET, &http_client_handle, request);
      break;
    }

    case SL_HTTP_POST: {
      // Validate HTTP client state
      if (http_client_handle.client_state != HTTP_STATE_POST_REQUEST_INITIALIZED) {
        return SL_STATUS_INVALID_STATE;
      }

      //! Send HTTP client POST request
      status = send_http_client_request(SL_HTTP_POST, &http_client_handle, request);
      break;
    }

    case SL_HTTP_PUT: {
      // Validate HTTP client state
      if (http_client_handle.client_state != HTTP_STATE_PUT_REQUEST_INITIALIZED) {
        return SL_STATUS_INVALID_STATE;
      }

      // 917 does not support this feature for PUT request in Alpha 3 release
      if (request->body != NULL) {
        return SL_STATUS_NOT_SUPPORTED;
      }

      // Allocate memory for request structure
      sl_si91x_http_client_put_request_t *http_put_request =
        (sl_si91x_http_client_put_request_t *)malloc(sizeof(sl_si91x_http_client_put_request_t));
      VERIFY_MALLOC_AND_RETURN(http_put_request);

      memset(http_put_request, 0, sizeof(sl_si91x_http_client_put_request_t));

      sl_si91x_http_client_put_start_t *http_put_start =
        &http_put_request->http_client_put_struct.http_client_put_start;

      //! Create HTTP PUT client
      // Fill command type
      http_put_request->command_type = SI91X_HTTP_CLIENT_PUT_CREATE;

      // Fill payload length
      // Since body is not sent in this request, we are subtracting max SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH
      packet_length = sizeof(sl_si91x_http_client_put_request_t) - SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH;

      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_PUT,
                                            SI91X_NETWORK_CMD_QUEUE,
                                            http_put_request,
                                            packet_length,
                                            SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                            NULL,
                                            NULL);
      if (status != SL_STATUS_OK) {
        free(http_put_request);
        return status;
      }

      //! Start HTTP client PUT process
      // Fill command type
      http_put_request->command_type = SI91X_HTTP_CLIENT_PUT_START;

      // Fill IP version
      if (http_client_handle.configuration.ip_version == SL_IPV6) {
        http_put_start->ip_version = 6;
      } else {
        http_put_start->ip_version = 4;
      }

      // Set default by NULL delimiter
      http_put_start->https_enable |= SL_SI91X_ENABLE_NULL_DELIMETER;

      // Fill HTTPS feature
      if (http_client_handle.configuration.https_enable) {
        http_put_start->https_enable = SL_SI91X_ENABLE_TLS;

        // Fill SSL/TLS version
        switch (http_client_handle.configuration.tls_version) {
          case SL_TLS_V_1_0: {
            http_put_start->https_enable |= SL_SI91X_TLS_V_1_0;
            break;
          }
          case SL_TLS_V_1_1: {
            http_put_start->https_enable |= SL_SI91X_TLS_V_1_1;
            break;
          }
          case SL_TLS_V_1_2: {
            http_put_start->https_enable |= SL_SI91X_TLS_V_1_2;
            break;
          }
#ifdef CHIP_917
          case SL_TLS_V_1_3: {
            http_put_start->https_enable |= SL_SI91X_TLS_V_1_3;
            break;
          }
#endif
          default:
            return SL_STATUS_INVALID_CONFIGURATION;
        }

        // Fill HTTPS certificate index bitmap
        switch (http_client_handle.configuration.certificate_index) {
          case SL_HTTPS_CLIENT_CERTIFICATE_INDEX_1: {
            http_put_start->https_enable |= SL_SI91X_HTTPS_CERTIFICATE_INDEX_1;
            break;
          }
          case SL_HTTPS_CLIENT_CERTIFICATE_INDEX_2: {
            http_put_start->https_enable |= SL_SI91X_HTTPS_CERTIFICATE_INDEX_2;
            break;
          }
          case SL_HTTPS_CLIENT_DEFAULT_CERTIFICATE_INDEX:
          default:
            break;
        }
      }

      // Fill HTTP version
      if (http_client_handle.configuration.http_version == SL_HTTP_V_1_1) {
        http_put_start->https_enable |= SL_SI91X_HTTP_V_1_1;
      }

      // Fill port number
      http_put_start->port_number = (uint32_t)request->port;

      // Fill Total resource content length
      http_put_start->content_length = request->body_length;

      // Fill username
      memcpy(http_put_request->http_put_buffer,
             &http_client_handle.client_credentials->data[0],
             http_client_handle.client_credentials->username_length);
      http_buffer_offset += http_client_handle.client_credentials->username_length;
      http_put_request->http_put_buffer[http_buffer_offset] = '\0';
      http_buffer_offset++;

      // Fill password
      memcpy(http_put_request->http_put_buffer + http_buffer_offset,
             &http_client_handle.client_credentials->data[http_client_handle.client_credentials->username_length],
             http_client_handle.client_credentials->password_length);
      http_buffer_offset += http_client_handle.client_credentials->password_length;
      http_put_request->http_put_buffer[http_buffer_offset] = '\0';
      http_buffer_offset++;

      // Fill hostname
      http_buffer_offset += snprintf((char *)(http_put_request->http_put_buffer + http_buffer_offset),
                                     SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH - http_buffer_offset,
                                     "%s",
                                     request->ip_address);
      http_buffer_offset++;

      // Fill IP address
      http_buffer_offset += snprintf((char *)(http_put_request->http_put_buffer + http_buffer_offset),
                                     SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH - http_buffer_offset,
                                     "%s",
                                     request->ip_address);
      http_buffer_offset++;

      // Fill URL resource
      http_buffer_offset += snprintf((char *)(http_put_request->http_put_buffer + http_buffer_offset),
                                     SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH - http_buffer_offset,
                                     "%s",
                                     request->resource);
      http_buffer_offset++;

      // Fill extended header
      if (request->extended_header != NULL) {
        // Enable user given content type in extended header
        http_put_start->https_enable |= SL_SI91X_HTTP_USER_DEFINED_CONTENT_TYPE;

        load_extended_headers_into_request_buffer(http_put_request->http_put_buffer,
                                                  request->extended_header,
                                                  &http_buffer_offset);
      } else {
        http_put_request->http_put_buffer[http_buffer_offset] = '\0';
      }

      // Check if request buffer is overflowed
      if (http_buffer_offset > SI91X_HTTP_BUFFER_LEN) {
        free(http_put_request);
        return SL_STATUS_HAS_OVERFLOWED;
      }

      // Fill data length
      packet_length =
        sizeof(sl_si91x_http_client_put_request_t) - SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH + http_buffer_offset;

      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_PUT,
                                            SI91X_NETWORK_CMD_QUEUE,
                                            http_put_request,
                                            packet_length,
                                            SL_SI91X_RETURN_IMMEDIATELY,
                                            request->context,
                                            NULL);

      // Free memory of request struture
      free(http_put_request);

      break;
    }

    default: {
      return SL_STATUS_FAIL;
    }
  }

  // Store request configurations into client_internal structure
  memcpy(&http_client_handle.request, request, sizeof(sl_http_client_request_t));

  // Set HTTP client state to requested sent state
  if (request->http_method_type != SL_HTTP_GET && request->body == NULL) {
    http_client_handle.client_state = HTTP_STATE_CHUNKED_REQUEST_SENT;
  } else {
    http_client_handle.client_state = HTTP_STATE_REQUEST_SENT;
  }

  return status;
}

sl_status_t sl_http_client_write_chunked_data(const sl_http_client_t *client,
                                              uint8_t *data,
                                              uint32_t data_length,
                                              bool flush_now)
{
  UNUSED_PARAMETER(flush_now);
  ARGS_CHECK_NULL_POINTER(client);
  ARGS_CHECK_NULL_POINTER(data);

  if (*client != (sl_http_client_t)&http_client_handle) {
    return SL_STATUS_INVALID_HANDLE;
  }

  // Check for HTTP client requested state
  if (http_client_handle.client_state != HTTP_STATE_CHUNKED_REQUEST_SENT) {
    return SL_STATUS_INVALID_STATE;
  }

  // Check for data length more than max buffer size
  if (data_length > SI91X_HTTP_CLIENT_MAX_WRITE_BUFFER_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check for invalid data length
  if ((data_length == 0) && (strlen((char *)data) == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status     = SL_STATUS_OK;
  uint16_t packet_length = 0;

  switch (http_client_handle.request.http_method_type) {
    case SL_HTTP_POST: {
      // Allocate memory for request structure
      sl_si91x_http_client_post_data_request_t *http_post_data =
        (sl_si91x_http_client_post_data_request_t *)malloc(sizeof(sl_si91x_http_client_post_data_request_t));
      VERIFY_MALLOC_AND_RETURN(http_post_data);

      // Fill HTTP Post data current chunk length
      http_post_data->current_length = data_length;

      // Fill HTTP Post data
      memcpy(http_post_data->http_post_data_buffer, data, data_length);

      // Fill total packet length
      packet_length =
        sizeof(sl_si91x_http_client_post_data_request_t) - SI91X_HTTP_CLIENT_POST_MAX_BUFFER_LENGTH + data_length;

      // Send HTTP Post Data request
      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA,
                                            SI91X_NETWORK_CMD_QUEUE,
                                            http_post_data,
                                            packet_length,
                                            SL_SI91X_RETURN_IMMEDIATELY,
                                            http_client_handle.request.context,
                                            NULL);

      // Free memory of request struture
      free(http_post_data);

      break;
    }

    case SL_HTTP_PUT: {
      // Allocate memory for request structure
      sl_si91x_http_client_put_request_t *http_put_pkt_request =
        (sl_si91x_http_client_put_request_t *)malloc(sizeof(sl_si91x_http_client_put_request_t));
      VERIFY_MALLOC_AND_RETURN(http_put_pkt_request);

      sl_si91x_http_client_put_data_request_t *http_put_data =
        &http_put_pkt_request->http_client_put_struct.http_client_put_data_req;

      // Fill command type
      http_put_pkt_request->command_type = SI91X_HTTP_CLIENT_PUT_PKT;

      // Fill HTTP Put packet current chunk length
      http_put_data->current_length = data_length;

      // Fill HTTP Put data
      memcpy(http_put_pkt_request->http_put_buffer, data, data_length);

      // Fill total packet length
      packet_length =
        sizeof(sl_si91x_http_client_put_request_t) - SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH + data_length;

      // Send HTTP Put Data request
      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_CLIENT_PUT,
                                            SI91X_NETWORK_CMD_QUEUE,
                                            http_put_pkt_request,
                                            packet_length,
                                            SL_SI91X_RETURN_IMMEDIATELY,
                                            http_client_handle.request.context,
                                            NULL);

      // Free memory of request struture
      free(http_put_pkt_request);

      break;
    }

    default:
      return SL_STATUS_FAIL;
  }

  return status;
}

static sl_status_t sl_si91x_http_client_abort(void)
{
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HTTP_ABORT,
                                                    SI91X_NETWORK_CMD_QUEUE,
                                                    NULL,
                                                    0,
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
