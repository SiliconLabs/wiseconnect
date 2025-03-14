/*******************************************************************************
* @file  sl_sntp.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_slist.h"
#include "sl_si91x_driver.h"
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_si91x_core_utilities.h"
#include "cmsis_os2.h"

#include "sl_sntp.h"
#include "si91x_sntp_client_types.h"
#include <string.h>

typedef struct {
  uint8_t callback_event_type;
  uint8_t *data;
  uint16_t data_length;
} sl_internal_sntp_client_context_t;

static osMutexId_t sntp_mutex;
static sl_sntp_client_event_handler_t sntp_event_handler;

static sl_status_t sli_sntp_client_get_time_date(uint8_t *data,
                                                 uint16_t data_length,
                                                 uint32_t timeout,
                                                 uint16_t cmd_type)
{
  sl_status_t status                      = SL_STATUS_FAIL;
  si91x_sntp_client_t client_req          = { 0 };
  sl_si91x_wait_period_t wait_time        = 0;
  sl_wifi_buffer_t *buffer                = NULL;
  const sl_si91x_packet_t *packet         = NULL;
  sl_wifi_buffer_t *sdk_context           = NULL;
  sl_internal_sntp_client_context_t *node = NULL;
  uint16_t length                         = 0;
  uint16_t buffer_length                  = 0;

  if ((timeout > 0) && ((data == NULL) || (data_length == 0))) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  client_req.command_type = (uint8_t)cmd_type;

  if (timeout > 0) {
    wait_time = SL_SI91X_WAIT_FOR_RESPONSE(timeout);
  } else {
    status = sl_si91x_host_allocate_buffer(&sdk_context,
                                           SL_WIFI_CONTROL_BUFFER,
                                           sizeof(sl_internal_sntp_client_context_t),
                                           1000);
    VERIFY_STATUS_AND_RETURN(status);
    node = sl_si91x_host_get_buffer_data(sdk_context, 0, &buffer_length);
    if (cmd_type == SI91X_SNTP_CLIENT_GETTIME) {
      node->callback_event_type = SL_SNTP_CLIENT_GET_TIME;
    } else {
      node->callback_event_type = SL_SNTP_CLIENT_GET_TIME_DATE;
    }
    node->data        = data;
    node->data_length = data_length;
    wait_time         = SL_SI91X_RETURN_IMMEDIATELY;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SNTP_CLIENT,
                                        SI91X_NETWORK_CMD,
                                        &client_req,
                                        sizeof(client_req),
                                        wait_time,
                                        (void *)sdk_context,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > data_length) {
    length = data_length;
  } else {
    length = packet->length;
  }
  memcpy(data, packet->data, length);
  status = SL_STATUS_OK;

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sli_si91x_sntp_event_handler(sli_si91x_queue_packet_t *data)
{
  uint16_t status                         = 0;
  sl_sntp_client_response_t response      = { 0 };
  sl_si91x_packet_t *raw_rx_packet        = sl_si91x_host_get_buffer_data(data->host_packet, 0, NULL);
  sl_wifi_buffer_t *sdk_context           = (sl_wifi_buffer_t *)data->sdk_context;
  sl_internal_sntp_client_context_t *node = NULL;
  uint16_t buffer_length                  = 0;

  node = (sl_internal_sntp_client_context_t *)sl_si91x_host_get_buffer_data(sdk_context, 0, &buffer_length);

  osMutexAcquire(sntp_mutex, 0xFFFFFFFFUL);
  if (NULL != sntp_event_handler) {
    status               = get_si91x_frame_status(raw_rx_packet);
    response.event_type  = node->callback_event_type;
    response.status      = convert_and_save_firmware_status(status);
    response.data        = raw_rx_packet->data;
    response.data_length = raw_rx_packet->length;

    sntp_event_handler(&response, node->data, node->data_length);
  }
  osMutexRelease(sntp_mutex);

  sl_si91x_host_free_buffer(sdk_context);
  return SL_STATUS_OK;
}

sl_status_t sl_sntp_client_start(sl_sntp_client_config_t *config, uint32_t timeout)
{
  sl_status_t status                      = SL_STATUS_FAIL;
  si91x_sntp_client_t client_req          = { 0 };
  sl_si91x_wait_period_t wait_time        = 0;
  sl_wifi_buffer_t *sdk_context           = NULL;
  sl_internal_sntp_client_context_t *node = NULL;
  uint16_t buffer_length                  = 0;

  if (NULL == config) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == config->server_host_name) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((SL_SNTP_BROADCAST_MODE != config->sntp_method) && (SL_SNTP_UNICAST_MODE != config->sntp_method)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (NULL == sntp_mutex) {
    sntp_mutex = osMutexNew(NULL);
  }

  osMutexAcquire(sntp_mutex, 0xFFFFFFFFUL);
  sntp_event_handler = config->event_handler;
  osMutexRelease(sntp_mutex);

  client_req.command_type                = SI91X_SNTP_CLIENT_CREATE;
  client_req.sntp_method                 = config->sntp_method;
  *((uint16_t *)client_req.sntp_timeout) = config->sntp_timeout;

  // Check for IP version
  if (config->flags & 0x01) {
    client_req.ip_version = 6;
    memcpy(client_req.server_ip_address.ipv6_address, config->server_host_name, 16);
  } else {
    client_req.ip_version = 4;
    memcpy(client_req.server_ip_address.ipv4_address, config->server_host_name, 4);
  }

  if (timeout > 0) {
    wait_time = SL_SI91X_WAIT_FOR_RESPONSE(timeout);
  } else {
    status = sl_si91x_host_allocate_buffer(&sdk_context,
                                           SL_WIFI_CONTROL_BUFFER,
                                           sizeof(sl_internal_sntp_client_context_t),
                                           1000);
    VERIFY_STATUS_AND_RETURN(status);
    node                      = sl_si91x_host_get_buffer_data(sdk_context, 0, &buffer_length);
    node->callback_event_type = SL_SNTP_CLIENT_START;
    node->data                = NULL;
    node->data_length         = 0;
    wait_time                 = SL_SI91X_RETURN_IMMEDIATELY;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SNTP_CLIENT,
                                        SI91X_NETWORK_CMD,
                                        &client_req,
                                        sizeof(client_req),
                                        wait_time,
                                        (void *)sdk_context,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_sntp_client_get_time(uint8_t *data, uint16_t data_length, uint32_t timeout)
{
  return sli_sntp_client_get_time_date(data, data_length, timeout, SI91X_SNTP_CLIENT_GETTIME);
}

sl_status_t sl_sntp_client_get_time_date(uint8_t *data, uint16_t data_length, uint32_t timeout)
{
  return sli_sntp_client_get_time_date(data, data_length, timeout, SI91X_SNTP_CLIENT_GETTIME_DATE);
}

sl_status_t sl_sntp_client_get_server_info(sl_sntp_server_info_t *data, uint32_t timeout)
{
  sl_status_t status                      = SL_STATUS_FAIL;
  si91x_sntp_client_t client_req          = { 0 };
  sl_si91x_wait_period_t wait_time        = 0;
  sl_wifi_buffer_t *buffer                = NULL;
  const sl_si91x_packet_t *packet         = NULL;
  uint16_t length                         = 0;
  sl_wifi_buffer_t *sdk_context           = NULL;
  sl_internal_sntp_client_context_t *node = NULL;
  uint16_t buffer_length                  = 0;

  if ((timeout > 0) && (data == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  client_req.command_type = SI91X_SNTP_CLIENT_GET_SERVER_INFO;

  if (timeout > 0) {
    wait_time = SL_SI91X_WAIT_FOR_RESPONSE(timeout);
  } else {
    status = sl_si91x_host_allocate_buffer(&sdk_context,
                                           SL_WIFI_CONTROL_BUFFER,
                                           sizeof(sl_internal_sntp_client_context_t),
                                           1000);
    VERIFY_STATUS_AND_RETURN(status);
    node                      = sl_si91x_host_get_buffer_data(sdk_context, 0, &buffer_length);
    node->callback_event_type = SL_SNTP_CLIENT_GET_SERVER_INFO;
    node->data                = (uint8_t *)data;
    node->data_length         = sizeof(sl_sntp_server_info_t);
    wait_time                 = SL_SI91X_RETURN_IMMEDIATELY;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SNTP_CLIENT,
                                        SI91X_NETWORK_CMD,
                                        &client_req,
                                        sizeof(client_req),
                                        wait_time,
                                        (void *)sdk_context,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);
  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > sizeof(sl_sntp_server_info_t)) {
    length = sizeof(sl_sntp_server_info_t);
  } else {
    length = packet->length;
  }
  memcpy(data, packet->data, length);
  status = SL_STATUS_OK;

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_sntp_client_stop(uint32_t timeout)
{
  sl_status_t status                      = SL_STATUS_FAIL;
  sl_si91x_wait_period_t wait_time        = 0;
  si91x_sntp_client_t client_req          = { 0 };
  sl_wifi_buffer_t *sdk_context           = NULL;
  sl_internal_sntp_client_context_t *node = NULL;
  uint16_t buffer_length                  = 0;

  client_req.command_type = SI91X_SNTP_CLIENT_DELETE;

  if (timeout > 0) {
    wait_time = SL_SI91X_WAIT_FOR_RESPONSE(timeout);
  } else {
    status = sl_si91x_host_allocate_buffer(&sdk_context,
                                           SL_WIFI_CONTROL_BUFFER,
                                           sizeof(sl_internal_sntp_client_context_t),
                                           1000);
    VERIFY_STATUS_AND_RETURN(status);
    node                      = sl_si91x_host_get_buffer_data(sdk_context, 0, &buffer_length);
    node->callback_event_type = SL_SNTP_CLIENT_STOP;
    node->data                = NULL;
    node->data_length         = 0;
    wait_time                 = SL_SI91X_RETURN_IMMEDIATELY;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SNTP_CLIENT,
                                        SI91X_NETWORK_CMD,
                                        &client_req,
                                        sizeof(client_req),
                                        wait_time,
                                        (void *)sdk_context,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
