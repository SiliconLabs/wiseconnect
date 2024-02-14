/** @file
 *
 * File Description
 *
 */
#include "console.h"
#include "sl_utility.h"
#include "sl_constants.h"
#include "sl_wifi_callback_framework.h"
#include "errno.h"
#include "sl_string.h"
#include "sl_ip_types.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "printf.h"
#include "sl_sntp.h"
#include <string.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define SNTP_METHOD        SL_SNTP_UNICAST_MODE
#define FLAGS              0
#define DATA_BUFFER_LENGTH 50
#define SNTP_TIMEOUT       50
#define SNTP_API_TIMEOUT   0
#define ASYNC_WAIT_TIMEOUT (SNTP_API_TIMEOUT + 2000)
#define DATA_BUFFER_LENGTH 50

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Extern Variable
 ******************************************************/

/******************************************************
 *               Global Variable
 ******************************************************/
static sl_status_t cb_status     = SL_STATUS_FAIL;
static char *event_type[]        = { [SL_SNTP_CLIENT_START]           = "SNTP Client Start",
                                     [SL_SNTP_CLIENT_GET_TIME_DATE]   = "SNTP Client Get Time and Date",
                                     [SL_SNTP_CLIENT_GET_SERVER_INFO] = "SNTP Client Get Server Info",
                                     [SL_SNTP_CLIENT_STOP]            = "SNTP Client Stop" };
static uint8_t callback_event    = 0;
static uint8_t exec_status       = 0;
uint8_t data[DATA_BUFFER_LENGTH] = { 0 };
/******************************************************
 *               Variable Definitions
 ******************************************************/

sl_sntp_client_config_t config   = { 0 };
sl_sntp_server_info_t serverInfo = { 0 };

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

static void sntp_client_event_handler(sl_sntp_client_response_t *response,
                                      uint8_t *user_data,
                                      uint16_t user_data_length)
{
  uint16_t length = 0;

  printf("\nReceived %s SNTP event with status %s\n",
         event_type[response->event_type],
         (0 == response->status) ? "Success" : "Failed");

  if (0 == response->status) {
    if (response->data_length > user_data_length) {
      length = user_data_length;
    } else {
      length = response->data_length;
    }

    memcpy(user_data, response->data, length);
  }

  callback_event = response->event_type;
  cb_status      = response->status;

  if (SL_SNTP_CLIENT_STOP == response->event_type) {
    exec_status = 1;
  }
  return;
}

sl_status_t sntp_client_start_command_handler(console_args_t *arguments)
{
  sl_status_t status             = SL_STATUS_OK;
  sl_ipv4_address_t ipv4_address = { 0 };
  convert_string_to_sl_ipv4_address((char *)arguments->arg[0], &ipv4_address);
  config.server_host_name = ipv4_address.bytes;
  config.sntp_method      = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SNTP_METHOD, uint8_t);
  config.sntp_timeout     = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SNTP_TIMEOUT, uint16_t);
  config.event_handler    = sntp_client_event_handler;
  config.flags            = GET_OPTIONAL_COMMAND_ARG(arguments, 3, FLAGS, uint8_t);

  uint32_t start = 0;
  cb_status      = SL_STATUS_FAIL;
  status         = sl_sntp_client_start(&config, SNTP_API_TIMEOUT);

  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_START != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to start Async SNTP client: 0x%lx", status);
      return cb_status;
    }
  } else {
    if (status == SL_STATUS_OK) {
      printf("SNTP Client started successfully\n");
    } else {
      printf("Failed to start SNTP client: 0x%lx", status);
      return status;
    }
  }
  return status;
}

sl_status_t sntp_client_get_time_data_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status = SL_STATUS_OK;
  cb_status          = SL_STATUS_FAIL;
  uint32_t start     = 0;
  status             = sl_sntp_client_get_time_date(data, DATA_BUFFER_LENGTH, SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_GET_TIME_DATE != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to get Async date and time from ntp server : 0x%lx", status);
      return cb_status;
    }
  } else {
    if (status == SL_STATUS_OK) {
      printf("SNTP Client got TIME and DATE successfully");
    } else {
      printf("Failed to get date and time from ntp server : 0x%lx", status);
      return status;
    }
  }
  printf("%s", data);
  return status;
}

sl_status_t sntp_client_get_server_info(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status = SL_STATUS_OK;
  cb_status          = SL_STATUS_FAIL;
  status             = sl_sntp_client_get_server_info(&serverInfo, SNTP_API_TIMEOUT);
  uint32_t start     = 0;
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_GET_SERVER_INFO != callback_event)
           && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to get async ntp server info : 0x%lx", status);
      return cb_status;
    }
  } else {
    if (status == SL_STATUS_OK) {
      printf("SNTP Client got server info successfully");
    } else {
      printf("Failed to get ntp server info : 0x%lx", status);
      return status;
    }
  }
  printf("Got Server IP version : %u\n", serverInfo.ip_version);
  if (4 == serverInfo.ip_version) {
    printf("IPv4 Address is : %u.%u.%u.%u\n",
           serverInfo.server_ip_address.ipv4_address[0],
           serverInfo.server_ip_address.ipv4_address[1],
           serverInfo.server_ip_address.ipv4_address[2],
           serverInfo.server_ip_address.ipv4_address[3]);
  } else {
    printf("IPv6 Address is : %lx:%lx:%lx:%lx\n",
           serverInfo.server_ip_address.ipv6_address[0],
           serverInfo.server_ip_address.ipv6_address[1],
           serverInfo.server_ip_address.ipv6_address[2],
           serverInfo.server_ip_address.ipv6_address[3]);
  }
  printf("SNTP Server Method : %u\n", serverInfo.sntp_method);
  return status;
}

sl_status_t sntp_client_stop_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status = SL_STATUS_OK;
  cb_status          = SL_STATUS_FAIL;
  uint32_t start     = 0;
  status             = sl_sntp_client_stop(SNTP_API_TIMEOUT);
  if ((SNTP_API_TIMEOUT == 0) && (SL_STATUS_IN_PROGRESS == status)) {
    start = osKernelGetTickCount();

    while ((SL_SNTP_CLIENT_STOP != callback_event) && (osKernelGetTickCount() - start) <= ASYNC_WAIT_TIMEOUT) {
      osThreadYield();
    }

    if (cb_status != SL_STATUS_OK) {
      printf("Failed to stop Async SNTP client: 0x%lx", status);
      return cb_status;
    }
  } else {
    if (status == SL_STATUS_OK) {
      printf("SNTP Client stopped successfully");
    } else {
      printf("Failed to stop SNTP client: 0x%lx", status);
      return status;
    }
    exec_status = 1;
  }
  return status;
}