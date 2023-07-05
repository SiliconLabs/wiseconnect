/***************************************************************************/ /**
 * @file
 * @brief HTTP Client Post Data Application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_tls.h"
#include "sl_http_client.h"
#include <string.h>

//! Include index html page
#include "index.html.h"
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
#define HTTPS_ENABLE 0

//! Set 1 - Enable and 0 - Disable
#define EXTENDED_HEADER_ENABLE 0

#if HTTPS_ENABLE
//! Set TLS version
#define TLS_VERSION SL_TLS_V_1_2

#define CERTIFICATE_INDEX SL_HTTPS_CLIENT_CERTIFICATE_INDEX_1

//! Include SSL CA certificate
#include "cacert.pem.h"

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

//! End of data indications
// No data pending from host
#define HTTP_END_OF_DATA 1

// HTTP Post success response
#define HTTP_POST_RSP 2

// More data pending from host
#define HTTP_POST_DATA_RSP 4

// More data pending from module to host
#define HTTP_POST_RCV_RSP 8

// End of HTTP Data from module to host
#define HTTP_POST_RCV_DONE 9

//! Application buffer length
#define APP_BUFFER_LENGTH 3000

#define HTTP_SUCCESS_RESPONSE 1
#define HTTP_FAILURE_RESPONSE 2

#define HTTP_SYNC_RESPONSE  0
#define HTTP_ASYNC_RESPONSE 1

/******************************************************
 *               Variable Definitions
 ******************************************************/
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

static const sl_wifi_device_configuration_t http_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode                  = SL_SI91X_CLIENT_MODE,
                   .coex_mode                  = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map            = (SL_SI91X_FEAT_SECURITY_OPEN),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_HTTP_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL),
                   .custom_feature_bit_map     = SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_XTAL_CLK_ENABLE(2) |
#ifndef RSI_M4_INTERFACE
                                                  RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                                                  RAM_LEVEL_NWP_ADV_MCU_BASIC
#endif
                                                  ),
                   .bt_feature_bit_map      = 0,
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 }
};

//! Application buffer
uint8_t app_buffer[APP_BUFFER_LENGTH] = { 0 };

//! Application buffer index
uint32_t app_buff_index = 0;

volatile uint8_t http_rsp_received    = 0;
volatile uint8_t http_post_write_done = 0;
volatile uint8_t end_of_data          = 0;
volatile uint8_t http_recv_done       = 0;
sl_status_t callback_status           = SL_STATUS_OK;

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
sl_status_t http_client_post_data_application(void);
sl_status_t http_response_status(volatile uint8_t *response);
sl_status_t http_post_response_callback_handler(const sl_http_client_t *client,
                                                sl_http_client_event_t event,
                                                void *data,
                                                void *request_context);
sl_status_t http_get_response_callback_handler(const sl_http_client_t *client,
                                               sl_http_client_event_t event,
                                               void *data,
                                               void *request_context);

/******************************************************
 *               Function Definitions
 ******************************************************/
void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;

  status = sl_net_init(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, &http_client_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Init Success\r\n");

  status = sl_net_up(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE, 0);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("\r\nWi-Fi Client Connected\r\n");

#if HTTPS_ENABLE
  if (LOAD_CERTIFICATE) {
    sl_tls_store_configuration_t tls_configuration = { 0 };

    tls_configuration.cacert             = (uint8_t *)cacert;
    tls_configuration.cacert_length      = (sizeof(cacert) - 1);
    tls_configuration.cacert_type        = SL_TLS_SSL_CA_CERTIFICATE;
    tls_configuration.use_secure_element = false;

    if (CERTIFICATE_INDEX) {
      tls_configuration.cert_identifier = CERTIFICATE_INDEX;

      // Erase certificate at given cert index
      status = sl_tls_clear_ca_store(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE,
                                     SL_NET_APPLICATION_PROTOCOL_HTTP_HTTPS,
                                     CERTIFICATE_INDEX);
      if (status != SL_STATUS_OK) {
        printf("\r\nErasing certificate Failed, Error Code : 0x%lX\r\n", status);
        return;
      }

      // Load SSL CA certificate
      status = sl_tls_set_ca_store(SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE,
                                   SL_NET_APPLICATION_PROTOCOL_HTTP_HTTPS,
                                   tls_configuration);
      if (status != SL_STATUS_OK) {
        printf("\r\nLoading SSL CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
        return;
      }
      printf("\r\nLoad SSL CA certificate at index %d Success\r\n", CERTIFICATE_INDEX);
    } else {
      // Erase certificate buffer
      status = sl_tls_clear_global_ca_store();
      if (status != SL_STATUS_OK) {
        printf("\r\nErasing certificate Failed, Error Code : 0x%lX\r\n", status);
        return;
      }

      // Load SSL CA certificate
      status = sl_tls_set_global_ca_store(tls_configuration);
      if (status != SL_STATUS_OK) {
        printf("\r\nLoading SSL CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
        return;
      }
      printf("\r\nLoad SSL CA certificate at Default index Success\r\n");
    }
  }
#endif

  status = http_client_post_data_application();
  if (status != SL_STATUS_OK) {
    printf("\r\nUnexpected error while HTTP Post Data operation: 0x%lX\r\n", status);
    return;
  }

  printf("\r\nApplication Demonstration Completed Successfully!\r\n");
}

sl_status_t http_client_post_data_application(void)
{
  sl_status_t status                                  = SL_STATUS_OK;
  sl_http_client_t client_handle                      = 0;
  sl_http_client_configuration_t client_configuration = { 0 };
  sl_http_client_request_t client_request             = { 0 };
  int32_t total_post_data_len                         = sizeof(sl_index) - 1;
  int32_t offset                                      = 0;
  int32_t chunk_length                                = 0;

  //! Set HTTP Client credentials
  uint16_t username_length = strlen(HTTP_CLIENT_USERNAME);
  uint16_t password_length = strlen(HTTP_CLIENT_PASSWORD);
  uint32_t credential_size = sizeof(sl_http_client_credentials_t) + username_length + password_length;

  sl_http_client_credentials_t *client_credentials = (sl_http_client_credentials_t *)malloc(credential_size);
  SL_VERIFY_POINTER_OR_RETURN(client_credentials, SL_STATUS_ALLOCATION_FAILED);

  client_credentials->username_length = username_length;
  client_credentials->password_length = password_length;

  memcpy(&client_credentials->data[0], HTTP_CLIENT_USERNAME, username_length);
  memcpy(&client_credentials->data[username_length], HTTP_CLIENT_PASSWORD, password_length);

  status = sl_net_set_credential(SL_NET_HTTP_CLIENT_CREDENTIAL_ID,
                                 SL_NET_HTTP_CLIENT_CREDENTIALS,
                                 client_credentials,
                                 credential_size);
  if (status != SL_STATUS_OK) {
    free(client_credentials);
    return status;
  }

  //! Fill HTTP client_configurations
  client_configuration.network_interface = SL_NET_DEFAULT_WIFI_CLIENT_INTERFACE;
  client_configuration.ip_version        = IP_VERSION;
  client_configuration.http_version      = HTTP_VERSION;
#if HTTPS_ENABLE
  client_configuration.https_enable      = true;
  client_configuration.tls_version       = TLS_VERSION;
  client_configuration.certificate_index = CERTIFICATE_INDEX;
#endif

  //! Fill HTTP client_request configurations
  client_request.ip_address      = (uint8_t *)HTTP_SERVER_IP;
  client_request.port            = HTTP_PORT;
  client_request.resource        = (uint8_t *)HTTP_URL;
  client_request.extended_header = NULL;

  status = sl_http_client_init(&client_configuration, &client_handle);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nHTTP Client init success\r\n");

  //! Configure HTTP POST request
  client_request.http_method_type = SL_HTTP_POST;
  client_request.body             = NULL;
  client_request.body_length      = total_post_data_len;

  //! Initialize callback method for HTTP POST request
  status = sl_http_client_request_init(&client_request, http_post_response_callback_handler, "This is HTTP client");
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  printf("\r\nHTTP Post request init success\r\n");

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

  //! Send HTTP POST request
  status = sl_http_client_send_request(&client_handle, &client_request);
  if (status == SL_STATUS_IN_PROGRESS) {
    status = http_response_status(&http_rsp_received);
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);
  } else {
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  }

  //! Write HTTP POST data
  while (!end_of_data) {
    //! Get the current length that you want to send
    chunk_length = ((total_post_data_len - offset) > SL_MAX_HTTP_CLIENT_WRITE_BUFFER_LENGTH)
                     ? SL_MAX_HTTP_CLIENT_WRITE_BUFFER_LENGTH
                     : (total_post_data_len - offset);

    status = sl_http_client_write_chunked_data(&client_handle, (uint8_t *)(sl_index + offset), chunk_length, 0);

    if (status == SL_STATUS_IN_PROGRESS) {
      status = http_response_status(&http_post_write_done);
      CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);

      offset += chunk_length;
    } else {
      CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
    }
  }

  status = http_response_status(&http_recv_done);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);

  printf("\r\nHTTP POST request Success\r\n");

  //! Reset buffer and offset
  memset(app_buffer, 0, sizeof(app_buffer));
  app_buff_index = 0;
  http_recv_done = 0;

  //! Configure HTTP GET request
  client_request.http_method_type = SL_HTTP_GET;

  //! Initialize callback method for HTTP GET request
  status = sl_http_client_request_init(&client_request, http_get_response_callback_handler, "This is HTTP client");
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  printf("\r\nHTTP Get request init success\r\n");

  //! Send HTTP GET request
  status = sl_http_client_send_request(&client_handle, &client_request);
  if (status == SL_STATUS_IN_PROGRESS) {
    status = http_response_status(&http_rsp_received);
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_ASYNC_RESPONSE);
  } else {
    CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
  }

  SL_DEBUG_LOG("\r\nGET Data response:\n%s \nOffset: %ld\r\n", app_buffer, app_buff_index);

  printf("\r\nHTTP GET request Success\r\n");

#if EXTENDED_HEADER_ENABLE
  //! Delete extended headers
  status = sl_http_client_delete_all_headers(&client_request);
  CLEAN_HTTP_CLIENT_IF_FAILED(status, &client_handle, HTTP_SYNC_RESPONSE);
#endif

  status = sl_http_client_deinit(&client_handle);
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nHTTP Client deinit success\r\n");
  free(client_credentials);

  return status;
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

  SL_DEBUG_LOG("\r\n===========HTTP POST RESPONSE START===========\r\n");
  SL_DEBUG_LOG(
    "\r\n> Status: 0x%X\n> POST response: %u\n> End of data: %lu\n> Data Length: %u\n> Request Context: %s\r\n",
    post_response->status,
    post_response->http_response_code,
    post_response->end_of_data,
    post_response->data_length,
    (char *)request_context);

  if (post_response->status != SL_STATUS_OK
      || (post_response->http_response_code >= 400 && post_response->http_response_code <= 599
          && post_response->http_response_code != 0)) {
    http_rsp_received    = HTTP_FAILURE_RESPONSE;
    http_post_write_done = HTTP_FAILURE_RESPONSE;
    http_recv_done       = HTTP_FAILURE_RESPONSE;
    return post_response->status;
  }

  if (post_response->end_of_data & HTTP_POST_RSP) {
    http_rsp_received = 1;
    return SL_STATUS_OK;
  }

  if (post_response->end_of_data & HTTP_POST_DATA_RSP) {
    http_post_write_done = 1;
    end_of_data          = (post_response->end_of_data & HTTP_END_OF_DATA) ? 1 : 0;
  } else if (post_response->end_of_data & HTTP_POST_RCV_DONE) {
    memcpy(app_buffer + app_buff_index, post_response->data_buffer, post_response->data_length);
    app_buff_index += post_response->data_length;
    http_recv_done = (post_response->end_of_data & HTTP_END_OF_DATA) ? HTTP_SUCCESS_RESPONSE : http_recv_done;
  } else {
    http_post_write_done = HTTP_FAILURE_RESPONSE;
    http_recv_done       = HTTP_FAILURE_RESPONSE;
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

  sl_http_client_response_t *get_response = (sl_http_client_response_t *)data;
  callback_status                         = get_response->status;

  SL_DEBUG_LOG("\r\n===========HTTP GET RESPONSE START===========\r\n");
  SL_DEBUG_LOG(
    "\r\n> Status: 0x%X\n> GET response: %u\n> End of data: %lu\n> Data Length: %u\n> Request Context: %s\r\n",
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
