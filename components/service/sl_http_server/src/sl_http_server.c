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

#include "sl_http_server.h"
#include "sl_constants.h"
#include "sl_slist.h"
#include "sl_net.h"
#include "sl_net_constants.h"
#include "sl_wifi_constants.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket.h"
#include "socket.h"
#include "sl_net_rsi_utility.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BACK_LOG                      1 ///< As we are processing one request at a time, the backlog is set to one.
#define SL_HIGH_PERFORMANCE_SOCKET    BIT(7)
#define HTTP_MAX_HEADER_LENGTH        (MAX_HEADER_BUFFER_LENGTH - 1)
#define HTTP_CONNECTION_STATUS_HEADER "Connection: close\r\n"

#define HTTP_SERVER_START_SUCCESS   BIT(0)
#define HTTP_SERVER_START_FAILED    BIT(1)
#define HTTP_SERVER_STOP_CMD        BIT(2)
#define HTTP_SERVER_EXIT            BIT(3)
#define HTTP_SERVER_CONNECT_SUCCESS BIT(4)

/******************************************************
 *               Variable Definitions
 ******************************************************/
static int server_socket               = -1;
static int client_socket               = -1;
static sl_http_server_t *server_handle = NULL;
static osThreadId_t http_server_id     = NULL;

const osThreadAttr_t http_server_attributes = {
  .name       = "http_server",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 2048,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
  .reserved   = 0,
};

/******************************************************
 *               Static functions
 ******************************************************/
static sl_status_t unknown_request_handler(sl_http_server_t *handle, sl_http_server_request_t *request)
{
  UNUSED_PARAMETER(request);
  sl_http_server_response_t response = { 0 };
  char *response_data                = "Not Found!";
  uint32_t length                    = strlen(response_data);

  response.response_code        = SL_HTTP_RESPONSE_NOT_FOUND;
  response.content_type         = SL_HTTP_CONTENT_TYPE_TEXT_HTML;
  response.data                 = (uint8_t *)response_data;
  response.current_data_length  = length;
  response.expected_data_length = length;
  response.headers              = NULL;
  response.header_count         = 0;
  sl_http_server_send_response(handle, &response);

  return SL_STATUS_OK;
}

static sl_status_t sli_parse_http_headers(sl_http_server_t *handle, int length)
{
  uint16_t header_count   = 0;
  int string_length       = 0;
  char *headers           = NULL;
  char *sol               = NULL;
  char *target            = NULL;
  char content_length[32] = { 0 };

  handle->request.request_data_length       = 0;
  handle->request.request_header_count      = 0;
  handle->request.uri.path                  = NULL;
  handle->request.uri.query_parameter_count = 0;
  headers                                   = handle->request_buffer;
  sol                                       = headers;
  for (int i = 0; i < length; i++) {
    if ('\r' == headers[i] && '\n' == headers[i + 1]) {
      headers[i]     = 0;
      headers[i + 1] = 0;

      SL_DEBUG_LOG("Got request method : %s\n", sol);

      char *sep_pos = strchr(sol, ' '); // Find the space position
      sep_pos[0]    = 0;
      if (strcmp(sol, "GET") == 0) {
        handle->request.type = SL_HTTP_REQUEST_GET;
      } else if (strcmp(sol, "POST") == 0) {
        handle->request.type = SL_HTTP_REQUEST_POST;
      } else if (strcmp(sol, "PUT") == 0) {
        handle->request.type = SL_HTTP_REQUEST_PUT;
      } else if (strcmp(sol, "DELETE") == 0) {
        handle->request.type = SL_HTTP_REQUEST_DELETE;
      } else if (strcmp(sol, "HEAD") == 0) {
        handle->request.type = SL_HTTP_REQUEST_HEAD;
      } else {
        return SL_STATUS_FAIL;
      }
      handle->request.uri.path = &sep_pos[1];

      sep_pos = strchr(handle->request.uri.path, ' '); // Find the space position
      if (NULL != sep_pos) {
        sep_pos[0] = 0;
        if (0 == strcmp(&(sep_pos[1]), "HTTP/1.1")) {
          handle->request.version = SL_HTTP_VERSION_1_1;
        } else {
          handle->request.version = SL_HTTP_VERSION_1_0;
        }
        // ToDo: Store HTTP version
      } else {
        handle->request.version = SL_HTTP_VERSION_1_0;
      }

      sol = &(headers[i + 2]);
      break;
    }
  }

  handle->header = sol;
  length         = strlen(sol);
  headers        = sol;

  target = strchr(handle->request.uri.path, '?');
  if (NULL != target) {
    target[0] = 0;
    target += 1;
    handle->request.uri.query_parameter_count = 0;

    char *query = NULL;
    char *value = NULL;

    query = target;
    for (int i = 0; i < MAX_QUERY_PARAMETERS; i++) {
      handle->request.uri.query_parameters[i].query = query;
      query                                         = strchr(query, '&');
      if (NULL != query) {
        query[0] = 0;
        query += 1;
      }
      value = strchr(handle->request.uri.query_parameters[i].query, '=');
      if (NULL != value) {
        value[0] = 0;
        value += 1;
      }
      handle->request.uri.query_parameters[i].value = value;
      handle->request.uri.query_parameter_count++;

      if (NULL == query) {
        break;
      }
    }
  }

  for (int i = 0; i < length; i++) {
    // Identify number of headers
    if ('\r' == headers[i] && '\n' == headers[i + 1]) {
      // Found a new line
      header_count++;

      if (0 == strncmp("Content-Length", sol, strlen("Content-Length"))) {
        target = strchr(sol, ':');
        if (' ' == target[1]) {
          target += 2;
        } else {
          target += 1;
        }

        string_length = (&headers[i]) - target;
        if (string_length < 32) {
          strncpy(content_length, target, string_length);
          handle->request.request_data_length = atoi(content_length);
        }
      }

      sol = &(headers[i + 2]);
      i++;
    }
  }
  handle->request.request_header_count = header_count;

  return SL_STATUS_OK;
}

// Define the thread function for handling individual clients
static void sli_process_request(sl_http_server_t *handle, int client_socket)
{
  int recv_length                   = 0;
  int rem_length                    = 0;
  sl_http_server_request_t *request = NULL;
  char *recv_buffer                 = NULL;

  handle->request.request_data_length = 0;
  handle->response_sent               = false;
  rem_length                          = (MAX_HEADER_BUFFER_LENGTH - 1);
  recv_buffer                         = handle->request_buffer;

  // Loop until Complete headers are received
  while (rem_length > 0) {
    int length = recv(client_socket, recv_buffer, rem_length, 0);
    if (length < 0) {
      SL_DEBUG_LOG("\r\nSocket receive failed with bsd error: %d\r\n", errno);
      close(client_socket);
      return;
    }
    char *sol = &(recv_buffer[recv_length]);
    recv_length += length;
    rem_length -= length;
    recv_buffer[recv_length] = 0;
    recv_buffer += length;

    // Search for end of the header
    char *sep_pos = strstr(sol, "\r\n\r\n");
    SL_DEBUG_LOG("Got chunk: \n%s\n", sol);

    if (NULL != sep_pos) {
      sep_pos[2] = 0;
      sep_pos[3] = 0;
      rem_length = 0;

      if (SL_STATUS_OK != sli_parse_http_headers(handle, length)) {
        break;
      }
      SL_DEBUG_LOG("Got expected data length : %u\n", handle->request.request_data_length);

      if (handle->request.request_data_length > 0) {
        int header_length = (int)(sep_pos - handle->request_buffer);
        recv_buffer       = (char *)handle->req_data;
        handle->rem_len   = handle->request.request_data_length;

        SL_DEBUG_LOG("Got header length : %u\n", header_length);

        // Check if we recived data along with header
        if (recv_length > (header_length + 4)) {
          length              = recv_length - (header_length + 4);
          handle->data_length = length;
          SL_DEBUG_LOG("Got remaining data length : %u\n", handle->data_length);
          handle->req_data = (uint8_t *)(handle->request_buffer + header_length + 4);
        }
        recv_length = 0;
      }
    }
  }

  request = &(handle->request);

  // Check if the handler's list has the URI
  for (uint16_t i = 0; i < handle->config.handlers_count; i++) {
    if (strcmp(handle->config.handlers_list[i].uri, request->uri.path) == 0) {
      // If the handler is present, call the handler
      handle->config.handlers_list[i].handler(handle, request);
      break;
    }
  }

  if (false == handle->response_sent) {
    handle->config.default_handler(handle, &(handle->request));
  }
  return;
}

static void client_accept_callback(int32_t sock_id, struct sockaddr *addr, uint8_t ip_version)
{
  UNUSED_PARAMETER(addr);
  UNUSED_PARAMETER(ip_version);
  SL_DEBUG_LOG("\r\nAccepted socket ID : %ld\r\n", sock_id);
  client_socket = sock_id;
  osEventFlagsSet(server_handle->http_server_id, HTTP_SERVER_CONNECT_SUCCESS);
}

static void sli_http_server(const void *arg)
{
  uint32_t result                   = 0;
  server_handle                     = (sl_http_server_t *)arg;
  int socket_return_value           = 0;
  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);
  uint8_t high_performance_socket   = SL_HIGH_PERFORMANCE_SOCKET;

  // Indicate to HTTP Server start that http server thread started successfully.
  osEventFlagsSet(server_handle->http_server_id, HTTP_SERVER_START_SUCCESS);

  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    SL_DEBUG_LOG("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  SL_DEBUG_LOG("\r\nServer Socket ID : %d\r\n", server_socket);

  socket_return_value = sl_si91x_setsockopt_async(server_socket,
                                                  SOL_SOCKET,
                                                  SL_SI91X_SO_HIGH_PERFORMANCE_SOCKET,
                                                  &high_performance_socket,
                                                  sizeof(high_performance_socket));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  uint8_t ap_vap = 1;
  socket_return_value =
    sl_si91x_setsockopt_async(server_socket, SOL_SOCKET, SL_SI91X_SO_SOCK_VAP_ID, &ap_vap, sizeof(ap_vap));
  if (socket_return_value < 0) {
    SL_DEBUG_LOG("\r\nSet Socket option failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port   = server_handle->config.port;

  socket_return_value = bind(server_socket, (struct sockaddr *)&server_address, socket_length);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG("\r\nSocket bind failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }

  socket_return_value = listen(server_socket, BACK_LOG);
  if (socket_return_value < 0) {
    SL_DEBUG_LOG("\r\nSocket listen failed with bsd error: %d\r\n", errno);
    close(server_socket);
    return;
  }
  SL_DEBUG_LOG("\r\nListening on Local Port : %d\r\n", server_address.sin_port);

  while (1) {
    socket_return_value = sl_si91x_accept_async(server_socket, client_accept_callback);
    if (socket_return_value != SI91X_NO_ERROR) {
      SL_DEBUG_LOG("\r\nSocket accept failed with bsd error: %d\r\n", errno);
    }

    // Wait for small amount of time to check if HTTP server stop is called
    result = osEventFlagsWait(server_handle->http_server_id,
                              HTTP_SERVER_STOP_CMD | HTTP_SERVER_CONNECT_SUCCESS,
                              osFlagsWaitAny,
                              osWaitForever);
    if (result != (uint32_t)osErrorTimeout) {
      if (result & HTTP_SERVER_STOP_CMD) {
        // HTTP_SERVER_STOP_CMD flag is set
        server_handle->server_socket = -1;
        SL_DEBUG_LOG("\r\nIn http server thread: Got Stop Command\r\n");
        close(server_socket);
        osEventFlagsSet(server_handle->http_server_id, HTTP_SERVER_EXIT);
        break;
      }
      if (result & HTTP_SERVER_CONNECT_SUCCESS) {
        // HTTP_SERVER_CONNECT_SUCCESS flag is set
        osEventFlagsClear(server_handle->http_server_id, HTTP_SERVER_CONNECT_SUCCESS);
        SL_DEBUG_LOG("\r\nClient Socket:%d----------------------------", client_socket);
        server_handle->server_socket = server_socket;
        server_handle->client_socket = client_socket;
        sli_process_request(server_handle, client_socket);
      }
    }
    osDelay(100);
    close(client_socket);
  }

  while (1) {
    osDelay(1000); // Delay for 1 second
  }
}

static int sli_process_socket_buffered_data(int fd, char *data, size_t data_length, int window_size)
{
  size_t tx_length = 0;

  while (data_length > 0) {
    tx_length = ((size_t)window_size > data_length) ? data_length : (size_t)window_size;

    if (send(fd, data, tx_length, 0) == -1) {
      return -1;
    }

    data += tx_length;
    data_length -= tx_length;
  }

  return 0;
}

static int sli_send_response_headers(sl_http_server_t *server, sl_http_server_response_t *response, int window_size)
{
  char response_code[16]  = { 0 };
  char content_length[32] = { 0 };
  char *http_version      = NULL;

  // Convert the response code to a string
  sprintf(response_code, "%d", response->response_code);

  if (response->expected_data_length > 0) {
    sprintf(content_length, "%lu", response->expected_data_length);
  }

  if (SL_HTTP_VERSION_1_1 == server->request.version) {
    http_version = "HTTP/1.1 ";
  } else {
    http_version = "HTTP/1.0 ";
  }

  sli_process_socket_buffered_data(server->client_socket, http_version, strlen(http_version), window_size);
  sli_process_socket_buffered_data(server->client_socket, response_code, strlen(response_code), window_size);
  sli_process_socket_buffered_data(server->client_socket, "\r\n", 2, window_size);

  if (NULL != response->content_type) {
    sli_process_socket_buffered_data(server->client_socket, "Content-Type: ", strlen("Content-Type: "), window_size);
    sli_process_socket_buffered_data(server->client_socket,
                                     response->content_type,
                                     strlen(response->content_type),
                                     window_size);
    sli_process_socket_buffered_data(server->client_socket, "\r\n", 2, window_size);
  }

  if (response->expected_data_length > 0) {
    sli_process_socket_buffered_data(server->client_socket,
                                     "Content-Length: ",
                                     strlen("Content-Length: "),
                                     window_size);
    sli_process_socket_buffered_data(server->client_socket, content_length, strlen(content_length), window_size);
    sli_process_socket_buffered_data(server->client_socket, "\r\n", 2, window_size);
  }
  sli_process_socket_buffered_data(server->client_socket,
                                   HTTP_CONNECTION_STATUS_HEADER,
                                   strlen(HTTP_CONNECTION_STATUS_HEADER),
                                   window_size);

  // Append the headers to the response
  if ((response->header_count > 0) && (NULL != response->headers)) {
    for (int i = 0; i < response->header_count; i++) {
      sli_process_socket_buffered_data(server->client_socket,
                                       response->headers[i].key,
                                       strlen(response->headers[i].key),
                                       window_size);
      sli_process_socket_buffered_data(server->client_socket, ": ", 2, window_size);
      sli_process_socket_buffered_data(server->client_socket,
                                       response->headers[i].value,
                                       strlen(response->headers[i].value),
                                       window_size);
      sli_process_socket_buffered_data(server->client_socket, "\r\n", 2, window_size);
    }
  }

  sli_process_socket_buffered_data(server->client_socket, "\r\n", 2, window_size);
  return 0;
}

/******************************************************
 *               Function Definitions APIs
 ******************************************************/
sl_status_t sl_http_server_init(sl_http_server_t *handle, const sl_http_server_config_t *config)
{
  if (!handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Initialize the server fields
  handle->config.port = config->port;
  if (NULL != config->default_handler) {
    handle->config.default_handler = config->default_handler;
  } else {
    handle->config.default_handler = unknown_request_handler;
  }
  handle->config.handlers_list  = config->handlers_list;
  handle->config.handlers_count = config->handlers_count;

  memset(handle->request_buffer, 0, sizeof(MAX_HEADER_BUFFER_LENGTH));
  handle->http_server_id = osEventFlagsNew(NULL);

  memset(&(handle->request), 0, sizeof(sl_http_server_request_t));

  return SL_STATUS_OK;
}

/* Deinitializes the HTTP server frees all the resources related to HTTP server handle. After this 
   API is called the parameter "handle" should not be used in any other API without calling 
   sl_http_server_init() again */
sl_status_t sl_http_server_deinit(sl_http_server_t *handle)
{
  if (handle == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Close the server socket if it's open
  if (handle->server_socket != -1) {
    close(handle->server_socket);
    handle->server_socket = -1;
  }

  // Close event id
  osEventFlagsDelete(handle->http_server_id);

  // Reset the request handlers
  handle->config.handlers_list   = NULL;
  handle->config.handlers_count  = 0;
  handle->config.default_handler = NULL;
  return SL_STATUS_OK;
}

sl_status_t sl_http_server_start(sl_http_server_t *handle)
{
  http_server_id = NULL;

  http_server_id = osThreadNew((osThreadFunc_t)sli_http_server, handle, &http_server_attributes);
  if (http_server_id == NULL) {
    return SL_STATUS_FAIL;
  }

  uint32_t result = osEventFlagsWait(handle->http_server_id,
                                     (HTTP_SERVER_START_SUCCESS | HTTP_SERVER_START_FAILED),
                                     osFlagsWaitAny,
                                     osWaitForever);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    // TODO: Handle any required clean up
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/* This API waits for all the on going requests to complete and stops the HTTP server thread. */
sl_status_t sl_http_server_stop(sl_http_server_t *handle)
{
  osEventFlagsSet(handle->http_server_id, HTTP_SERVER_STOP_CMD);

  uint32_t result = osEventFlagsWait(handle->http_server_id, HTTP_SERVER_EXIT, osFlagsWaitAny, osWaitForever);
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    return SL_STATUS_FAIL;
  }

  osThreadTerminate(http_server_id);
  osThreadState_t state = osThreadGetState(http_server_id);
  if (state == osThreadTerminated) {
    SL_DEBUG_LOG("\r\n Server Thread Terminated \r\n");
  }
  SL_DEBUG_LOG("\r\nIn http server stop: Done\r\n");
  return SL_STATUS_OK;
}

sl_status_t sl_http_server_get_request_headers(sl_http_server_t *handle,
                                               sl_http_server_request_t *request,
                                               sl_http_header_t *headers,
                                               uint16_t header_count)
{

  uint16_t current_count = 0;
  char *header           = NULL;
  char *sol              = NULL;
  int length             = 0;

  if (NULL == handle) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == request) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == headers) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (0 == header_count) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sol    = handle->header;
  length = strlen(sol);
  header = sol;
  for (int i = 0; i < length; i++) {
    if ('\r' == header[i] && '\n' == header[i + 1]) {
      // Found a new line
      header[i]     = 0;
      header[i + 1] = 0;

      // Seggrigate key and value in current header line
      char *colon_pos = strchr(sol, ':'); // Find the colon position
      if (' ' == *(colon_pos - 1)) {
        *(colon_pos - 1) = 0;
      } else {
        *colon_pos = 0;
      }
      colon_pos += 2;

      headers[current_count].key   = sol;
      headers[current_count].value = colon_pos;

      sol = &(header[i + 2]);
      current_count++;
      if (current_count == header_count) {
        break;
      }
    }
  }
  return SL_STATUS_OK;
}

sl_status_t sl_http_server_read_request_data(sl_http_server_t *handle, sl_http_recv_req_data_t *recvData)
{
  uint32_t rem_len = 0;
  uint32_t length  = 0;
  uint32_t offset  = 0;

  if (NULL == recvData) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == recvData->buffer) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == recvData->request) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (0 == recvData->request->request_data_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (0 == recvData->buffer_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  recvData->received_data_length = 0;

  length  = 0;
  offset  = 0;
  rem_len = handle->data_length;
  if (rem_len > 0) {
    if (rem_len > recvData->buffer_length) {
      length = recvData->buffer_length;
    } else {
      length = rem_len;
    }

    memcpy(recvData->buffer, handle->req_data, length);
    handle->req_data = (handle->req_data + length);
    handle->rem_len -= length;
    handle->data_length -= length;
    offset += length;
    recvData->received_data_length = length;

    if ((length == recvData->buffer_length) || (0 == handle->rem_len)) {
      return SL_STATUS_OK;
    }

    rem_len = recvData->buffer_length - length;
  } else {
    if (handle->rem_len > recvData->buffer_length) {
      rem_len = recvData->buffer_length;
    } else {
      rem_len = handle->rem_len;
    }
  }

  while (0 != rem_len) {
    int receive_length = recv(handle->client_socket, &(recvData->buffer[offset]), rem_len, 0);
    if (receive_length < 0) {
      SL_DEBUG_LOG("\r\nSocket receive failed with bsd error: %d\r\n", errno);
      close(handle->client_socket);
      return SL_STATUS_FAIL;
    }
    length = (uint32_t)receive_length;
    offset += length;
    rem_len -= length;
    handle->rem_len -= length;
    recvData->received_data_length += length;
  }

  return SL_STATUS_OK;
}

sl_status_t sl_http_server_send_response(sl_http_server_t *handle, sl_http_server_response_t *response)
{
  int buffersize              = 0;
  socklen_t buffersize_length = sizeof(buffersize); // in/out parameter

  // Check if the response is not NULL
  if (response == NULL) {
    // If the response is NULL, return an error
    return SL_STATUS_FAIL;
  }

  // Check if the response is sent already
  if (true == handle->response_sent) {
    return SL_STATUS_FAIL;
  }

  getsockopt(handle->client_socket, SOL_SOCKET, SO_SNDBUF, (char *)&buffersize, &buffersize_length);

  sli_send_response_headers(handle, response, buffersize);

  if ((NULL != response->data) && (response->current_data_length > 0)) {
    if (sli_process_socket_buffered_data(handle->client_socket,
                                         (char *)response->data,
                                         response->current_data_length,
                                         buffersize)
        == -1) {
      SL_DEBUG_LOG("Failed to send buffer");
      // TODO: Free the response string
      return SL_STATUS_FAIL;
    }
  }
  handle->response_sent = true;

  return SL_STATUS_OK;
}

sl_status_t sl_http_server_write_data(sl_http_server_t *handle, uint8_t *data, uint32_t data_length)
{
  int buffersize              = 0;
  socklen_t buffersize_length = sizeof(buffersize); // in/out parameter

  if (handle == NULL) {
    return SL_STATUS_FAIL;
  }

  if (data == NULL) {
    return SL_STATUS_FAIL;
  }

  if (data_length == 0) {
    return SL_STATUS_FAIL;
  }

  // Check if the response is sent already
  if (true != handle->response_sent) {
    return SL_STATUS_FAIL;
  }

  getsockopt(handle->client_socket, SOL_SOCKET, SO_SNDBUF, (char *)&buffersize, &buffersize_length);
  if (sli_process_socket_buffered_data(handle->client_socket, (char *)data, data_length, buffersize) == -1) {
    SL_DEBUG_LOG("Failed to send buffer");
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}
