/** @file
 *
 * File Description
 *
 */
#include "console.h"
#include "sl_utility.h"
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_http_client.h"
#include "sl_wifi_device.h"
#include "sl_wifi_callback_framework.h"
#include "errno.h"
#include "sl_string.h"
#include "sl_ip_types.h"
#include "cmsis_os2.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "firmware_upgradation.h"
#include "printf.h"
//! Include index html page
#include "index.html.h"
#include "utilities.h"
#include "command_identifiers.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/******************************************************
 *                      Macros
 ******************************************************/
#define CLEAN_HTTP_CLIENT_IF_FAILED(status, client_handle, is_sync) \
  {                                                                 \
    if (status != SL_STATUS_OK) {                                   \
      sl_http_client_deinit(client_handle);                         \
      return ((is_sync == 0) ? status : callback_status);           \
    }                                                               \
  }

/******************************************************
 *                    Constants
 ******************************************************/

//! HTTP Client Configurations
#define HTTP_CLIENT_USERNAME "admin"
#define HTTP_CLIENT_PASSWORD "admin"

#define IP_VERSION   SL_IPV4
#define HTTP_VERSION SL_HTTP_V_1_1

//! Set 1 - Enable and 0 - Disable
#define HTTPS_ENABLE 1

//! Set 1 - Enable and 0 - Disable
#define EXTENDED_HEADER_ENABLE 0

#if HTTPS_ENABLE
//! Set TLS version
#define TLS_VERSION SL_TLS_V_1_2

#define CERTIFICATE_INDEX SL_HTTPS_CLIENT_CERTIFICATE_INDEX_1

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1
#endif

//! HTTP request configurations
//! Set HTTP Server IP Address
#define HTTP_SERVER_IP "192.168.0.100"

//! Server port number
#if HTTPS_ENABLE
//! Set default HTTPS port
#define HTTP_PORT 443
#else
//! Set default HTTP port
#define HTTP_PORT 80
#endif

//! HTTP resource name
#define HTTP_URL "/index.html"

//! HTTP post data
#define HTTP_DATA "employee_name=MR.REDDY&employee_id=RSXYZ123&designation=Engineer&company=SILABS&location=Hyderabad"

#if EXTENDED_HEADER_ENABLE
//! Extended headers
#define KEY1 "Content-Type"
#define VAL1 "text/html; charset=utf-8"

#define KEY2 "Content-Encoding"
#define VAL2 "br"

#define KEY3 "Content-Language"
#define VAL3 "de-DE"

#define KEY4 "Content-Location"
#define VAL4 "/index.html"
#endif

//! Application buffer length
#define APP_BUFFER_LENGTH 2000

#define HTTP_SUCCESS_RESPONSE 1
#define HTTP_FAILURE_RESPONSE 2

//! End of data indications
// No data pending from host
#define HTTP_END_OF_DATA 1

#define HTTP_SYNC_RESPONSE  0
#define HTTP_ASYNC_RESPONSE 1
//HTTP OTAF Parameters
#define HTTP_OTAF    2
#define OTAF_TIMEOUT 600000

/******************************************************
 *               Variable Definitions
 ******************************************************/

//! Application buffer
uint8_t app_buffer[APP_BUFFER_LENGTH] = { 0 };

//! Application buffer index
uint32_t app_buff_index = 0;

volatile uint8_t http_rsp_received = 0;
volatile uint8_t end_of_file       = 0;
static sl_status_t callback_status = SL_STATUS_OK;

//otaf response
volatile bool response = false;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Extern Variable
 ******************************************************/

/******************************************************
 *               Global Variable
 ******************************************************/

int32_t total_put_data_len                          = sizeof(sl_index) - 1;
int32_t offset                                      = 0;
int32_t chunk_length                                = 0;
sl_http_client_request_t client_request             = { 0 };
sl_http_client_t client_handle                      = 0;
sl_http_client_configuration_t client_configuration = { 0 };
sl_http_client_credentials_t *client_credentials    = { 0 };

/******************************************************
 *               Function Declarations
 ******************************************************/

sl_status_t http_response_status(volatile uint8_t *response);
sl_status_t http_put_response_callback_handler(const sl_http_client_t *client,
                                               sl_http_client_event_t event,
                                               void *data,
                                               void *request_context);
sl_status_t http_get_response_callback_handler(const sl_http_client_t *client,
                                               sl_http_client_event_t event,
                                               void *data,
                                               void *request_context);
sl_status_t http_post_response_callback_handler(const sl_http_client_t *client,
                                                sl_http_client_event_t event,
                                                void *data,
                                                void *request_context);
static void reset_http_handles(void);

sl_status_t http_fw_update_response_handler(sl_wifi_event_t event, uint16_t *data, uint32_t data_length, void *arg);

/******************************************************
 *               Function Definitions
 ******************************************************/

static void reset_http_handles(void)
{
  app_buff_index = 0;
  end_of_file    = 0;
}

sl_status_t http_fw_update_response_handler(sl_wifi_event_t event, uint16_t *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    response = false;
    return SL_STATUS_FAIL;
  }
  response = true;
  return SL_STATUS_OK;
}

sl_status_t http_put_response_callback_handler(const sl_http_client_t *client,
                                               sl_http_client_event_t event,
                                               void *data,
                                               void *request_context)
{
  UNUSED_PARAMETER(client);
  UNUSED_PARAMETER(event);

  sl_http_client_response_t *put_response = (sl_http_client_response_t *)data;
  callback_status                         = put_response->status;

  SL_DEBUG_LOG("===========HTTP PUT RESPONSE START===========");
  SL_DEBUG_LOG("> Status: 0x%X\n> PUT response: %u\n> End of data: %lu\n> Data Length: %u\n> Request Context: %s",
               put_response->status,
               put_response->http_response_code,
               put_response->end_of_data,
               put_response->data_length,
               (char *)request_context);

  if (put_response->status != SL_STATUS_OK) {
    http_rsp_received = 2;
    return put_response->status;
  }

  if (put_response->end_of_data & HTTP_END_OF_DATA) {
    if (put_response->data_length) {
      memcpy(app_buffer + app_buff_index, put_response->data_buffer, put_response->data_length);
      app_buff_index += put_response->data_length;
    }
    http_rsp_received = HTTP_SUCCESS_RESPONSE;
    end_of_file       = HTTP_SUCCESS_RESPONSE;
  } else {
    memcpy(app_buffer + app_buff_index, put_response->data_buffer, put_response->data_length);
    app_buff_index += put_response->data_length;
    http_rsp_received = HTTP_SUCCESS_RESPONSE;
  }

  return SL_STATUS_OK;
}

sl_status_t http_get_response_callback_handler(const sl_http_client_t *client,
                                               sl_http_client_event_t event,
                                               void *data,
                                               void *request_context)
{
  UNUSED_PARAMETER(client);
  UNUSED_PARAMETER(event);
  PRINT_EVENT_START(event, WIFI)
  sl_http_client_response_t *get_response = (sl_http_client_response_t *)data;
  callback_status                         = get_response->status;

  SL_DEBUG_LOG("===========HTTP GET RESPONSE START===========");
  SL_DEBUG_LOG("> Status: 0x%X\n> GET response: %u\n> End of data: %lu\n> Data Length: %u\n> Request Context: %s",
               get_response->status,
               get_response->http_response_code,
               get_response->end_of_data,
               get_response->data_length,
               (char *)request_context);

  if (get_response->status != SL_STATUS_OK
      || (get_response->http_response_code >= 400 && get_response->http_response_code <= 599
          && get_response->http_response_code != 0)) {
    http_rsp_received = HTTP_FAILURE_RESPONSE;
    return get_response->status;
  }

  if (!get_response->end_of_data) {
    memcpy(app_buffer + app_buff_index, get_response->data_buffer, get_response->data_length);
    app_buff_index += get_response->data_length;
  } else {
    if (get_response->data_length) {
      memcpy(app_buffer + app_buff_index, get_response->data_buffer, get_response->data_length);
      app_buff_index += get_response->data_length;
    }
    http_rsp_received = HTTP_SUCCESS_RESPONSE;
  }
  char *b64data = base64_encode(app_buffer, app_buff_index);
  printf("GET Data response:%s, Offset: %ld", b64data, app_buff_index);
  PRINT_EVENT_END();
  free(b64data);
  return SL_STATUS_OK;
}

sl_status_t http_post_response_callback_handler(const sl_http_client_t *client,
                                                sl_http_client_event_t event,
                                                void *data,
                                                void *request_context)
{
  UNUSED_PARAMETER(client);
  UNUSED_PARAMETER(event);

  sl_http_client_response_t *post_response = (sl_http_client_response_t *)data;
  callback_status                          = post_response->status;

  SL_DEBUG_LOG("===========HTTP POST RESPONSE START===========");
  SL_DEBUG_LOG("> Status: 0x%X\n> POST response: %u\n> End of data: %lu\n> Data Length: %u\n> Request Context: %s",
               post_response->status,
               post_response->http_response_code,
               post_response->end_of_data,
               post_response->data_length,
               (char *)request_context);

  if (post_response->status != SL_STATUS_OK
      || (post_response->http_response_code >= 400 && post_response->http_response_code <= 599
          && post_response->http_response_code != 0)) {
    http_rsp_received = HTTP_FAILURE_RESPONSE;
    return post_response->status;
  }

  if (post_response->end_of_data) {
    http_rsp_received = 1;
  }

  return SL_STATUS_OK;
}

sl_status_t http_response_status(volatile uint8_t *response)
{
  while (!(*response)) {
    /* Wait till response arrives */
  }

  if (*response != HTTP_SUCCESS_RESPONSE) {
    return SL_STATUS_FAIL;
  }

  // Reset response
  *response = 0;

  return SL_STATUS_OK;
}

sl_status_t http_client_init_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  //! Set HTTP Client credentials
  char *username           = GET_OPTIONAL_COMMAND_ARG(arguments, 0, HTTP_CLIENT_USERNAME, char *);
  char *password           = GET_OPTIONAL_COMMAND_ARG(arguments, 1, HTTP_CLIENT_PASSWORD, char *);
  uint16_t username_length = strlen(HTTP_CLIENT_USERNAME);
  uint16_t password_length = strlen(HTTP_CLIENT_PASSWORD);
  uint32_t credential_size = sizeof(sl_http_client_credentials_t) + username_length + password_length;

  client_credentials = (sl_http_client_credentials_t *)malloc(credential_size);
  SL_VERIFY_POINTER_OR_RETURN(client_credentials, SL_STATUS_ALLOCATION_FAILED);
  memset(client_credentials, 0, credential_size);
  client_credentials->username_length = username_length;
  client_credentials->password_length = password_length;

  memcpy(&client_credentials->data[0], username, username_length);
  memcpy(&client_credentials->data[username_length], password, password_length);

  status = sl_net_set_credential(SL_NET_HTTP_CLIENT_CREDENTIAL_ID(0),
                                 SL_NET_HTTP_CLIENT_CREDENTIAL,
                                 client_credentials,
                                 credential_size);
  if (status != SL_STATUS_OK) {
    free(client_credentials);
    return status;
  }

  //! Fill HTTP client_configurations
  client_configuration.network_interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_WIFI_CLIENT_INTERFACE, sl_net_interface_t);
  client_configuration.ip_version   = GET_OPTIONAL_COMMAND_ARG(arguments, 3, IP_VERSION, sl_ip_address_type_t);
  client_configuration.http_version = GET_OPTIONAL_COMMAND_ARG(arguments, 4, HTTP_VERSION, sl_http_client_version_t);
  bool https_enable                 = GET_OPTIONAL_COMMAND_ARG(arguments, 5, false, bool);
  if (https_enable) {

    client_configuration.https_enable = true;
    client_configuration.tls_version =
      GET_OPTIONAL_COMMAND_ARG(arguments, 6, TLS_VERSION, sl_http_client_tls_version_t);
    client_configuration.certificate_index = GET_OPTIONAL_COMMAND_ARG(arguments, 7, CERTIFICATE_INDEX, uint8_t);
  }

  status = sl_http_client_init(&client_configuration, &client_handle);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t http_client_put_start_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  //! Fill HTTP client_request configurations
  client_request.ip_address      = GET_OPTIONAL_COMMAND_ARG(arguments, 0, HTTP_SERVER_IP, uint8_t *);
  client_request.port            = GET_OPTIONAL_COMMAND_ARG(arguments, 1, HTTP_PORT, uint16_t);
  client_request.resource        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, HTTP_URL, uint8_t *);
  client_request.extended_header = NULL;
  //! Configure HTTP PUT request
  client_request.http_method_type = SL_HTTP_PUT;
  client_request.body             = NULL;
  client_request.body_length      = GET_OPTIONAL_COMMAND_ARG(arguments, 3, total_put_data_len, uint32_t);

  //! Initialize callback method for HTTP PUT request
  status = sl_http_client_request_init(&client_request, http_put_response_callback_handler, "This is HTTP client");
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

#if EXTENDED_HEADER_ENABLE
  //! Add extended headers
  status = sl_http_client_add_header(&client_request, KEY1, VAL1);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  status = sl_http_client_add_header(&client_request, KEY2, VAL2);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  status = sl_http_client_add_header(&client_request, KEY3, VAL3);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  status = sl_http_client_add_header(&client_request, KEY4, VAL4);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
#endif

  //! Send HTTP PUT request
  status = sl_http_client_send_request(&client_handle, &client_request);
  if (status == SL_STATUS_IN_PROGRESS) {
    status = http_response_status(&http_rsp_received);
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);
  } else {
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  }

  //! Write HTTP PUT data
  while (!end_of_file) {
    //! Get the current length that you want to send
    chunk_length = ((total_put_data_len - offset) > SL_HTTP_CLIENT_MAX_WRITE_BUFFER_LENGTH)
                     ? SL_HTTP_CLIENT_MAX_WRITE_BUFFER_LENGTH
                     : (total_put_data_len - offset);

    status = sl_http_client_write_chunked_data(&client_handle, (uint8_t *)(sl_index + offset), chunk_length, 0);

    if (status == SL_STATUS_IN_PROGRESS) {
      status = http_response_status(&http_rsp_received);
      CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);

      offset += chunk_length;
    } else {
      CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
    }
  }

  reset_http_handles();
  return status;
}

sl_status_t http_client_get_start_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  //! Fill HTTP client_request configurations
  client_request.ip_address      = GET_OPTIONAL_COMMAND_ARG(arguments, 0, HTTP_SERVER_IP, uint8_t *);
  client_request.port            = GET_OPTIONAL_COMMAND_ARG(arguments, 1, HTTP_PORT, uint16_t);
  client_request.resource        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, HTTP_URL, uint8_t *);
  client_request.extended_header = NULL;
  //! Configure HTTP GET request
  client_request.http_method_type = SL_HTTP_GET;

  //! Initialize callback method for HTTP GET request
  status = sl_http_client_request_init(&client_request, http_get_response_callback_handler, "This is HTTP client");
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  //! Send HTTP GET request
  status = sl_http_client_send_request(&client_handle, &client_request);
  if (status == SL_STATUS_IN_PROGRESS) {
    status = http_response_status(&http_rsp_received);
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);
  } else {
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  }
  reset_http_handles();
  return status;
}

sl_status_t http_client_post_start_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;
  //! Fill HTTP client_request configurations
  client_request.ip_address      = GET_OPTIONAL_COMMAND_ARG(arguments, 0, HTTP_SERVER_IP, uint8_t *);
  client_request.port            = GET_OPTIONAL_COMMAND_ARG(arguments, 1, HTTP_PORT, uint16_t);
  client_request.resource        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, HTTP_URL, uint8_t *);
  client_request.extended_header = NULL;
  //! Configure HTTP POST request
  client_request.http_method_type = SL_HTTP_POST;
  client_request.body             = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 3, HTTP_DATA, char *);
  client_request.body_length      = GET_OPTIONAL_COMMAND_ARG(arguments, 4, strlen(HTTP_DATA), uint32_t);

  //! Initialize callback method for HTTP POST request
  status = sl_http_client_request_init(&client_request, http_post_response_callback_handler, "This is HTTP client");
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  //! Send HTTP POST request
  status = sl_http_client_send_request(&client_handle, &client_request);
  if (status == SL_STATUS_IN_PROGRESS) {
    status = http_response_status(&http_rsp_received);
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);
  } else {
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  }

#if EXTENDED_HEADER_ENABLE
  status = sl_http_client_delete_all_headers(&client_request);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
#endif

  reset_http_handles();
  return status;
}

sl_status_t http_client_deinit_command_handler(console_args_t *arguments)
{
  sl_status_t status = SL_STATUS_OK;
  status             = sl_http_client_deinit(&client_handle);
  VERIFY_STATUS_AND_RETURN(status);
  free(client_credentials);
  return status;
}

sl_status_t http_otaf_upgrade_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  // Set Callback for HTTP OTA FW UPDATES
  sl_wifi_set_callback(SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS,
                       (sl_wifi_callback_function_t)&http_fw_update_response_handler,
                       NULL);

  uint8_t flags             = sl_cli_get_argument_uint8(arguments, 0);
  char *server_ip           = sl_cli_get_argument_string(arguments, 1);
  uint16_t port             = sl_cli_get_argument_uint16(arguments, 2);
  char *resource            = sl_cli_get_argument_string(arguments, 3);
  char *host_name           = sl_cli_get_argument_string(arguments, 4);
  char *extended_header     = sl_cli_get_argument_string(arguments, 5);
  char *username            = sl_cli_get_argument_string(arguments, 6);
  char *password            = sl_cli_get_argument_string(arguments, 7);
  char *post_data           = sl_cli_get_argument_string(arguments, 8);
  uint32_t post_data_length = sl_cli_get_argument_uint32(arguments, 9);

  status = sl_si91x_http_otaf(HTTP_OTAF,
                              (uint8_t)flags,
                              (uint8_t *)server_ip,
                              (uint16_t)port,
                              (uint8_t *)resource,
                              (uint8_t *)host_name,
                              (uint8_t *)extended_header,
                              (uint8_t *)username,
                              (uint8_t *)password,
                              (uint8_t *)post_data,
                              (uint32_t)post_data_length);

  return status;
}

sl_status_t http_download_command_handler(console_args_t *arguments)
{
  //! Fill HTTP client_request configurations
  char httpreq[100]                          = "GET /";
  char str1[6][50]                           = { " ",
                                                 " HTTP/1.1\r\n",
                                                 " HTTPS/1.1\r\n",
                                                 "Host: ",
                                                 " ",
                                                 "\r\n"
                                                                           "User-Agent: silabs/1.0.4a\r\n"
                                                                           "Accept: */*\r\n" };
  const char http_req_str_end[]              = "\r\n";
  const char http_req_str_connection_close[] = "Connection: close\r\n";

  //! arguments
  uint8_t client_socket = sl_cli_get_argument_uint8(arguments, 0);
  strcpy(str1[0], sl_cli_get_argument_string(arguments, 1));
  strcpy(str1[4], sl_cli_get_argument_string(arguments, 2));
  uint8_t mode = sl_cli_get_argument_uint8(arguments, 3);

  sl_status_t status          = SL_STATUS_OK;
  volatile uint8_t data_recvd = 0;
  int read_bytes              = 0;
  uint32_t total_read_bytes   = 0;
  uint8_t bytes_cnt           = 0;
  uint8_t download_inprogress = 0;
  int8_t recv_buffer2[1500];

  for (int i = 0; i < 6; i++) {
    if (mode == 0 && i == 2) {
      continue;
    } else if (mode == 1 && i == 1) {
      continue;
    }
    strcat(httpreq, str1[i]);
    strcat(httpreq, " ");
  }

  /* Send first set of HTTP/HTTPS headers to server */
  while (bytes_cnt != strlen(httpreq)) {
    status = send(client_socket, (const int8_t *)(httpreq + bytes_cnt), (strlen(httpreq) - bytes_cnt), 0);
    if (status < 0) {
      close(client_socket);
      printf("\r\n send failed\n");
      break;
    }
    bytes_cnt += status;
  }

  /* Send connection close headers to server */
  bytes_cnt = 0;
  while (bytes_cnt != strlen(http_req_str_connection_close)) {
    status = send(client_socket,
                  (const int8_t *)(http_req_str_connection_close + bytes_cnt),
                  (strlen(http_req_str_connection_close) - bytes_cnt),
                  0);
    if (status < 0) {
      close(client_socket);
      printf("\r\n send failed\r\n");
      break;
    }
    bytes_cnt += status;
  }
  printf("\r\n Send connection close headers to server\r\n");
  /* Send last set of HTTP headers to server */
#if !HTTPS_DOWNLOAD
  bytes_cnt = 0;
  while (bytes_cnt != strlen(http_req_str_end)) {
    status =
      send(client_socket, (const int8_t *)(http_req_str_end + bytes_cnt), (strlen(http_req_str_end) - bytes_cnt), 0);
    if (status < 0) {
      close(client_socket);
      printf("send failed\n");
      break;
    }
    bytes_cnt += status;
  }
#endif
  printf("\r\n Download started \r\n");

  while (1) {
    read_bytes = recv(client_socket, recv_buffer2, sizeof(recv_buffer2), 0);
    if (read_bytes < 0) {
      if (errno == 0) {
        printf("\r\nrecv failed with BSD error = %d and status = 0x%lu\r\n", errno, status);
      } else if ((errno == ENOTCONN) && (download_inprogress)) {
        data_recvd          = 1;
        download_inprogress = 0;
      } else {
        printf("\r\nrecv failed with BSD error = %d\r\n", errno);
      }
      break;
    }
    if (!download_inprogress) {
      download_inprogress = 1;
    }
    total_read_bytes += read_bytes;
  }
  if (data_recvd) {
    data_recvd = 0;
    printf("\r\nDownload completed, total received bytes: %lu\r\n", total_read_bytes);
    status = SL_STATUS_OK;
  }
  return status;
}