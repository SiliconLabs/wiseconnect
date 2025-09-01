/*******************************************************************************
 * @file  mqtt_commands.c
 * @brief MQTT AT Commands
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "console.h"
#include "sl_net.h"
#include "sl_mqtt_client.h"
#include "si91x_mqtt_client_types.h"
#include "at_utility.h"
#include "at_command_data_mode.h"
#include <string.h>
#include <inttypes.h>
#include "sl_string.h"

/******************************************************
 *                    Constants
 ******************************************************/
#define MQTT_BROKER_PORT 1883
#define MQTT_CLIENT_PORT 1

#define MQTT_CLIENT_INVALID_ID 0xFF
#define MQTT_CLIENT_DEFAULT_ID 0

#define MQTT_KEEPALIVE_INTERVAL 60 // seconds
#define MQTT_CONNECT_TIMEOUT    5000
#define MQTT_DISCONNECT_TIMEOUT 5000
#define MQTT_KEEPALIVE_RETRIES  3

#define MQTT_SUBSCRIBE_TIMEOUT   5000
#define MQTT_UNSUBSCRIBE_TIMEOUT 5000
#define MQTT_PUBLISH_TIMEOUT     5000

#define MQTT_USERNAME_DEFAULT ""
#define MQTT_PASSWORD_DEFAULT ""

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 1024
#endif

/******************************************************
 *               Variable Definitions
 ******************************************************/
extern sl_wifi_device_configuration_t si91x_init_configuration;

// MQTT Client
static uint8_t mqtt_client_inst_id  = MQTT_CLIENT_INVALID_ID;
static sl_mqtt_client_t mqtt_client = { 0 };

// MQTT Client Configuration
static uint8_t mqtt_client_id[SLI_SI91X_MQTT_CLIENT_ID_MAXIMUM_LENGTH];
static sl_mqtt_client_configuration_t mqtt_client_configuration = { 0 };

// MQTT Broker Configuration
static sl_mqtt_broker_t mqtt_broker_configuration = { .ip.type = SL_IPV4 };

// SIGN CERT CREDENTIAL
static sl_net_credential_id_t signcert_cred_id = SL_NET_INVALID_CREDENTIAL_ID;

// MQTT Message
static uint8_t message_topic[SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH];
static uint32_t message_timeout         = 0;
static sl_mqtt_client_message_t message = { 0 };

// MQTT Last Will Message
static bool mqtt_config_last_will_message = false;
static uint8_t mqtt_will_topic[SI91X_MQTT_CLIENT_WILL_TOPIC_MAXIMUM_LENGTH];
static uint8_t mqtt_will_message[SLI_SI91X_MQTT_CLIENT_MESSAGE_MAXIMUM_LENGTH];
static sl_mqtt_client_last_will_message_t mqtt_last_will_message = { 0 };

static void print_char_buffer(char *buffer, uint32_t buffer_length)
{
  for (uint32_t index = 0; index < buffer_length; index++) {
    printf("%c", buffer[index]);
  }
}

static sl_status_t check_client_instance_id(console_args_t *arguments, uint8_t index)
{
  if (mqtt_client_inst_id == MQTT_CLIENT_INVALID_ID) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  uint8_t client_inst_id = GET_OPTIONAL_COMMAND_ARG(arguments, index, MQTT_CLIENT_INVALID_ID, uint8_t);
  if (client_inst_id != mqtt_client_inst_id) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

static void mqtt_client_event_handler(void *client, sl_mqtt_client_event_t event, void *event_data, void *context)
{
  UNUSED_PARAMETER(context);

  sl_mqtt_client_t *p_client = (sl_mqtt_client_t *)client;
  if ((p_client == NULL) || (mqtt_client_inst_id == MQTT_CLIENT_INVALID_ID)) {
    return;
  }

  switch (event) {
    case SL_MQTT_CLIENT_CONNECTED_EVENT:
      printf("at+MQTT_CLIENT_CONNECTED=%d\r\n>\r\n", mqtt_client_inst_id);
      break;

    case SL_MQTT_CLIENT_DISCONNECTED_EVENT:
      printf("at+MQTT_CLIENT_DISCONNECTED=%d\r\n>\r\n", mqtt_client_inst_id);
      break;

    case SL_MQTT_CLIENT_SUBSCRIBED_EVENT:
      printf("at+MQTT_CLIENT_SUBSCRIBED=%d\r\n>\r\n", mqtt_client_inst_id);
      break;

    case SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT:
      printf("at+MQTT_CLIENT_UNSUBSCRIBED=%d\r\n>\r\n", mqtt_client_inst_id);
      break;

    case SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT:
      printf("at+MQTT_CLIENT_MESSAGE_PUBLISHED=%d\r\n>\r\n", mqtt_client_inst_id);
      break;

    case SL_MQTT_CLIENT_ERROR_EVENT: {
      sl_mqtt_client_error_status_t *error = (sl_mqtt_client_error_status_t *)event_data;
      printf("at+MQTT_CLIENT_ERROR=%d,%d\r\n>\r\n", mqtt_client_inst_id, *error);
    } break;

    default:
      break;
  }
}

static void mqtt_client_message_handler(void *client, sl_mqtt_client_message_t *message, void *context)
{
  UNUSED_PARAMETER(client);
  UNUSED_PARAMETER(context);

  if (message == NULL)
    return;
  printf("at+MQTT_CLIENT_MESSAGE_RECEIVED=%d,\"", mqtt_client_inst_id);
  print_char_buffer((char *)message->topic, message->topic_length);
  printf("\",%" PRIu32 ",", message->content_length);
  print_char_buffer((char *)message->content, message->content_length);
  printf("\r\n>\r\n");
}

static sl_status_t net_cred_signcert_send_buffer_handler(uint8_t *buffer, uint32_t length, void *user_data)
{
  UNUSED_PARAMETER(user_data);

  sl_status_t status = sl_net_set_credential(signcert_cred_id, SL_NET_SIGNING_CERTIFICATE, buffer, length);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;
  return status; // SL_STATUS_OK
}

static sl_status_t mqtt_client_pub_send_buffer_handler(uint8_t *buffer, uint32_t length, void *user_data)
{
  UNUSED_PARAMETER(user_data);

  message.content        = buffer;
  message.content_length = length;

  sl_status_t status = sl_mqtt_client_publish(&mqtt_client, &message, message_timeout, NULL);

  if ((SL_STATUS_IN_PROGRESS == status) || (SL_STATUS_OK == status)) {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  if (SL_STATUS_TIMEOUT == status) {
    printf("TIMEOUT");
    return SL_STATUS_OK;
  }

  return status;
}

static sl_status_t mqtt_client_lwt_send_buffer_handler(uint8_t *buffer, uint32_t length, void *user_data)
{
  UNUSED_PARAMETER(user_data);

  memcpy(mqtt_will_message, buffer, length);

  mqtt_last_will_message.will_message        = (uint8_t *)mqtt_will_message;
  mqtt_last_will_message.will_message_length = length;
  mqtt_config_last_will_message              = true;

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+enable-nwp-mqtt=<enable>
sl_status_t enable_nwp_mqtt_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  bool enable = GET_OPTIONAL_COMMAND_ARG(arguments, 0, false, bool);
  si91x_init_configuration.boot_config.tcp_ip_feature_bit_map |= SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID;
  if (enable) {
    si91x_init_configuration.boot_config.ext_tcp_ip_feature_bit_map |= SL_SI91X_EXT_EMB_MQTT_ENABLE;
  } else {
    si91x_init_configuration.boot_config.ext_tcp_ip_feature_bit_map &= ~SL_SI91X_EXT_EMB_MQTT_ENABLE;
  }

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+net-cred-mqttclient=<credential-id>,<username>,<password>
sl_status_t net_cred_mqttclient_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_net_credential_id_t mqtt_cred_id =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_INVALID_CREDENTIAL_ID, sl_net_credential_id_t);
  const char *username = GET_OPTIONAL_COMMAND_ARG(arguments, 1, MQTT_USERNAME_DEFAULT, const char *);
  const char *password = GET_OPTIONAL_COMMAND_ARG(arguments, 2, MQTT_PASSWORD_DEFAULT, const char *);

  if (mqtt_cred_id == SL_NET_INVALID_CREDENTIAL_ID) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint32_t malloc_size = sizeof(sl_mqtt_client_credentials_t) + strlen(username) + strlen(password);
  sl_mqtt_client_credentials_t *mqtt_client_credentials = (sl_mqtt_client_credentials_t *)malloc(malloc_size);
  SL_VERIFY_POINTER_OR_RETURN(mqtt_client_credentials, SL_STATUS_ALLOCATION_FAILED);

  memset(mqtt_client_credentials, 0, malloc_size);

  mqtt_client_credentials->username_length = strlen(username);
  mqtt_client_credentials->password_length = strlen(password);

  memcpy(&mqtt_client_credentials->data[0], username, strlen(username));
  memcpy(&mqtt_client_credentials->data[strlen(username)], password, strlen(password));

  sl_status_t status =
    sl_net_set_credential(mqtt_cred_id, SL_NET_MQTT_CLIENT_CREDENTIAL, mqtt_client_credentials, malloc_size);
  if (status != SL_STATUS_OK) {
    SL_CLEANUP_MALLOC(mqtt_client_credentials);
    return status;
  }

  SL_CLEANUP_MALLOC(mqtt_client_credentials);
  mqtt_client_configuration.credential_id = mqtt_cred_id;

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+net-cred-signcert=<credential-id>,<ca-cert-len>
sl_status_t net_cred_signcert_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);

  signcert_cred_id   = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_INVALID_CREDENTIAL_ID, sl_net_credential_id_t);
  uint32_t ca_length = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint32_t);

  if ((signcert_cred_id == SL_NET_INVALID_CREDENTIAL_ID) || (ca_length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status = at_command_goto_data_mode(net_cred_signcert_send_buffer_handler, ca_length, NULL);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+mqtt-client-init
sl_status_t mqtt_client_init_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);

  if (mqtt_client_inst_id == MQTT_CLIENT_DEFAULT_ID) {
    return SL_STATUS_ALREADY_INITIALIZED;
  }

  sl_status_t status = sl_mqtt_client_init(&mqtt_client, mqtt_client_event_handler);
  VERIFY_STATUS_AND_RETURN(status);

  mqtt_client_inst_id = MQTT_CLIENT_DEFAULT_ID;

  PRINT_AT_CMD_SUCCESS;
  printf("%d\r\n", mqtt_client_inst_id);

  return SL_STATUS_OK;
}

// at+mqtt-client-deinit=<client-instance-id>
sl_status_t mqtt_client_deinit_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_mqtt_client_deinit(&mqtt_client);
  VERIFY_STATUS_AND_RETURN(status);

  mqtt_client_inst_id = MQTT_CLIENT_INVALID_ID;

  PRINT_AT_CMD_SUCCESS;
  return status;
}

// at+mqtt-client-conncfg=<client-instance-id>,<ip-addr-type>,<ip-addr>,<port>,
//                        <connect-timeout>,<keep-alive-interval>,<keep-alive-retries>
sl_status_t mqtt_client_conncfg_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x3F);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  sl_ip_address_type_t ip_version = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_INVALID_IP, sl_ip_address_type_t);
  const char *ip_addr             = GET_OPTIONAL_COMMAND_ARG(arguments, 2, NULL, const char *);
  if (ip_addr == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (ip_version == SL_IPV4) {
    status = sl_net_inet_addr(ip_addr, &mqtt_broker_configuration.ip.ip.v4.value);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
#ifdef SLI_SI91X_ENABLE_IPV6
  else if ((ip_version == SL_IPV6) || (ip_version == SL_IPV6_LINK_LOCAL) || (ip_version == SL_IPV6_SITE_LOCAL)
           || (ip_version == SL_IPV6_GLOBAL)) {
    unsigned char hex_addr[SL_IPV6_ADDRESS_LENGTH] = { 0 };
    int ret                                        = sl_inet_pton6(ip_addr,
                            ip_addr + strlen(ip_addr),
                            hex_addr,
                            (unsigned int *)mqtt_broker_configuration.ip.ip.v6.value);
    if (ret != 0x1) {
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
#endif
  else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint16_t port                = GET_OPTIONAL_COMMAND_ARG(arguments, 3, MQTT_BROKER_PORT, uint16_t);
  uint16_t timeout             = GET_OPTIONAL_COMMAND_ARG(arguments, 4, MQTT_CONNECT_TIMEOUT, uint16_t);
  uint16_t keep_alive_interval = GET_OPTIONAL_COMMAND_ARG(arguments, 5, MQTT_KEEPALIVE_INTERVAL, uint16_t);
  uint16_t keep_alive_retries  = GET_OPTIONAL_COMMAND_ARG(arguments, 6, MQTT_KEEPALIVE_RETRIES, uint16_t);

  mqtt_broker_configuration.ip.type             = ip_version;
  mqtt_broker_configuration.port                = port;
  mqtt_broker_configuration.connect_timeout     = timeout;
  mqtt_broker_configuration.keep_alive_interval = keep_alive_interval;
  mqtt_broker_configuration.keep_alive_retries  = keep_alive_retries;

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+mqtt-client-opt=<client-instance-id>,<auto-reconnect>,<retry-count>,
//                    <min-back-off-time>,<max-back-off-time>,<clean-session>,
//                    <mqtt-version>,<port>,<credential-id>,<client-id>,<tls-flags>
sl_status_t mqtt_client_opt_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x7FF);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  bool auto_reconnect            = GET_OPTIONAL_COMMAND_ARG(arguments, 1, false, bool);
  uint8_t retry                  = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint8_t);
  uint16_t minimum_back_off_time = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint16_t);
  uint16_t maximum_back_off_time = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint16_t);

  if (minimum_back_off_time > maximum_back_off_time) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  bool clean_session             = GET_OPTIONAL_COMMAND_ARG(arguments, 5, true, bool);
  sl_mqtt_version_t mqtt_version = GET_OPTIONAL_COMMAND_ARG(arguments, 6, SL_MQTT_VERSION_3, sl_mqtt_version_t);
  uint16_t client_port           = GET_OPTIONAL_COMMAND_ARG(arguments, 7, MQTT_CLIENT_PORT, uint16_t);
  sl_net_credential_id_t credential_id =
    GET_OPTIONAL_COMMAND_ARG(arguments, 8, SL_NET_INVALID_CREDENTIAL_ID, sl_net_credential_id_t);
  const char *client_id = GET_OPTIONAL_COMMAND_ARG(arguments, 9, NULL, const char *);

  if ((credential_id == SL_NET_INVALID_CREDENTIAL_ID) || (client_id == NULL)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (strlen(client_id) >= SLI_SI91X_MQTT_CLIENT_ID_MAXIMUM_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_mqtt_tls_flag_t tls_flags = GET_OPTIONAL_COMMAND_ARG(arguments, 10, 0, sl_mqtt_tls_flag_t);

  mqtt_client_configuration.auto_reconnect        = auto_reconnect;
  mqtt_client_configuration.retry_count           = retry;
  mqtt_client_configuration.minimum_back_off_time = minimum_back_off_time;
  mqtt_client_configuration.maximum_back_off_time = maximum_back_off_time;
  mqtt_client_configuration.is_clean_session      = clean_session;
  mqtt_client_configuration.mqt_version           = mqtt_version;
  mqtt_client_configuration.client_port           = client_port;
  mqtt_client_configuration.credential_id         = credential_id;
  memcpy(mqtt_client_id, client_id, strlen(client_id));
  mqtt_client_configuration.client_id        = (uint8_t *)mqtt_client_id;
  mqtt_client_configuration.client_id_length = (uint8_t)strlen(client_id);
  mqtt_client_configuration.tls_flags        = tls_flags;

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+mqtt-client-lwt=<client-instance-id>,<retained>,<qos>,<topic>,<content-length>
sl_status_t mqtt_client_lwt_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  // If only <client-instance-id> is provided
  if (arguments->bitmap == 0x01) {
    mqtt_config_last_will_message = false;

    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  bool retained                = GET_OPTIONAL_COMMAND_ARG(arguments, 1, false, bool);
  sl_mqtt_qos_t will_qos_level = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_MQTT_QOS_LEVEL_0, sl_mqtt_qos_t);
  const char *will_topic       = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, const char *);
  uint32_t will_message_length = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t);

  mqtt_last_will_message.is_retained    = retained;
  mqtt_last_will_message.will_qos_level = will_qos_level;

  if (will_topic != NULL) {
    if (strlen(will_topic) >= SI91X_MQTT_CLIENT_WILL_TOPIC_MAXIMUM_LENGTH) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    memcpy(mqtt_will_topic, will_topic, strlen(will_topic));

    mqtt_last_will_message.will_topic        = (uint8_t *)mqtt_will_topic;
    mqtt_last_will_message.will_topic_length = (uint16_t)strlen(will_topic);
  } else {
    mqtt_last_will_message.will_topic        = NULL;
    mqtt_last_will_message.will_topic_length = 0;
  }

  if ((will_message_length > SLI_SI91X_MQTT_CLIENT_MESSAGE_MAXIMUM_LENGTH) || (will_message_length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = at_command_goto_data_mode(mqtt_client_lwt_send_buffer_handler, will_message_length, NULL);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+mqtt-client-conn=<client-instance-id>,<timeout>
sl_status_t mqtt_client_conn_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  uint32_t connect_timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 1, MQTT_CONNECT_TIMEOUT, uint32_t);
  status                   = sl_mqtt_client_connect(&mqtt_client,
                                  &mqtt_broker_configuration,
                                  mqtt_config_last_will_message ? &mqtt_last_will_message : NULL,
                                  &mqtt_client_configuration,
                                  connect_timeout);

  if ((SL_STATUS_IN_PROGRESS == status) || (SL_STATUS_OK == status)) {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  if (SL_STATUS_TIMEOUT == status) {
    printf("TIMEOUT");
    return SL_STATUS_OK;
  }

  return status;
}

// at+mqtt-client-discon=<client-instance-id>,<timeout>
sl_status_t mqtt_client_discon_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x03);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  uint32_t timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 1, MQTT_DISCONNECT_TIMEOUT, uint32_t);

  status = sl_mqtt_client_disconnect(&mqtt_client, timeout);

  if ((SL_STATUS_IN_PROGRESS == status) || (SL_STATUS_OK == status)) {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  if (SL_STATUS_TIMEOUT == status) {
    printf("TIMEOUT");
    return SL_STATUS_OK;
  }

  return status;
}

// at+mqtt-client-sub=<client-instance-id>,<qos>,<topic>,<timeout>
sl_status_t mqtt_client_sub_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x0F);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  sl_mqtt_qos_t qos_level = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_MQTT_QOS_LEVEL_0, sl_mqtt_qos_t);
  const char *topic       = GET_OPTIONAL_COMMAND_ARG(arguments, 2, NULL, const char *);
  uint32_t timeout        = GET_OPTIONAL_COMMAND_ARG(arguments, 3, MQTT_SUBSCRIBE_TIMEOUT, uint32_t);

  status = sl_mqtt_client_subscribe(&mqtt_client,
                                    (uint8_t *)topic,
                                    strlen(topic),
                                    qos_level,
                                    timeout,
                                    mqtt_client_message_handler,
                                    NULL);

  if ((SL_STATUS_IN_PROGRESS == status) || (SL_STATUS_OK == status)) {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  if (SL_STATUS_TIMEOUT == status) {
    printf("TIMEOUT");
    return SL_STATUS_OK;
  }

  return status;
}

// at+mqtt-client-unsub=<client-instance-id>,<topic>,<timeout>
sl_status_t mqtt_client_unsub_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x07);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  const char *topic = GET_OPTIONAL_COMMAND_ARG(arguments, 1, NULL, const char *);
  uint32_t timeout  = GET_OPTIONAL_COMMAND_ARG(arguments, 2, MQTT_UNSUBSCRIBE_TIMEOUT, uint32_t);

  status = sl_mqtt_client_unsubscribe(&mqtt_client, (uint8_t *)topic, strlen(topic), timeout, NULL);

  if ((SL_STATUS_IN_PROGRESS == status) || (SL_STATUS_OK == status)) {
    PRINT_AT_CMD_SUCCESS;
    return SL_STATUS_OK;
  }

  if (SL_STATUS_TIMEOUT == status) {
    printf("TIMEOUT");
    return SL_STATUS_OK;
  }

  return status;
}

// at+mqtt-client-pub=<client-instance-id>,<qos>,<id>,<retained>,<duplicate>,
//                    <topic>,<timeout>,<content-length>
sl_status_t mqtt_client_pub_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0xFF);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  sl_mqtt_qos_t qos_level = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_MQTT_QOS_LEVEL_0, sl_mqtt_qos_t);
  uint16_t packet_id      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint16_t);
  bool retained           = GET_OPTIONAL_COMMAND_ARG(arguments, 3, false, bool);
  bool duplicate          = GET_OPTIONAL_COMMAND_ARG(arguments, 4, false, bool);
  const char *topic       = GET_OPTIONAL_COMMAND_ARG(arguments, 5, NULL, const char *);
  uint32_t timeout        = GET_OPTIONAL_COMMAND_ARG(arguments, 6, MQTT_PUBLISH_TIMEOUT, uint32_t);
  uint32_t content_length = GET_OPTIONAL_COMMAND_ARG(arguments, 7, 0, uint32_t);

  if ((topic == NULL) || (content_length == 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (strlen(topic) >= SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  message.qos_level            = qos_level;
  message.packet_identifier    = packet_id;
  message.is_retained          = retained;
  message.is_duplicate_message = duplicate;
  memcpy(message_topic, topic, strlen(topic));
  message.topic          = (uint8_t *)message_topic;
  message.topic_length   = (uint16_t)strlen(topic);
  message.content_length = content_length;
  message_timeout        = timeout;

  status = at_command_goto_data_mode(mqtt_client_pub_send_buffer_handler, content_length, NULL);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;
  return SL_STATUS_OK;
}

// at+mqtt-client-conninfo=<client-instance-id>
sl_status_t mqtt_client_conninfo_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;

  // <connection-state>
  switch (mqtt_client.state) {
    case SL_MQTT_CLIENT_DISCONNECTED:
      printf("DISCONNECTED ");
      break;
    case SL_MQTT_CLIENT_CONNECTED:
      printf("CONNECTED ");
      break;
    case SL_MQTT_CLIENT_CONNECTION_FAILED:
      printf("CONNECTION_FAILED ");
      break;
    case SL_MQTT_CLIENT_TA_INIT:
      printf("NWP_INIT ");
      break;
    case SL_MQTT_CLIENT_TA_DISCONNECTED:
      printf("NWP_DISCONNECTED ");
      break;
    default:
      break;
  }

  // <broker-ip>
  print_sl_ip_address(&mqtt_broker_configuration.ip);

  // <broker-port> <connect-timeout> <keep-alive-interval> <keep-alive-retries>
  printf(" %d %d %d %d\r\n",
         mqtt_broker_configuration.port,
         mqtt_broker_configuration.connect_timeout,
         mqtt_broker_configuration.keep_alive_interval,
         mqtt_broker_configuration.keep_alive_retries);

  return SL_STATUS_OK;
}

// at+mqtt-client-optinfo=<client-instance-id>
sl_status_t mqtt_client_optinfo_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  PRINT_AT_CMD_SUCCESS;

  // <auto-reconnect> <connect-retries> <min-back-off-time> <max-back-off-time>
  // <clean-session> <mqtt-version> <port> <credential-id> <client-id> <tls-flags>
  printf("%d %d %d %d %d %d %d %d %s %d\r\n",
         mqtt_client_configuration.auto_reconnect,
         mqtt_client_configuration.retry_count,
         mqtt_client_configuration.minimum_back_off_time,
         mqtt_client_configuration.maximum_back_off_time,
         mqtt_client_configuration.is_clean_session,
         mqtt_client_configuration.mqt_version,
         mqtt_client_configuration.client_port,
         mqtt_client_configuration.credential_id,
         mqtt_client_configuration.client_id,
         mqtt_client_configuration.tls_flags);

  return SL_STATUS_OK;
}

// at+mqtt-client-subinfo=<client-instance-id>
sl_status_t mqtt_client_subinfo_command_handler(console_args_t *arguments)
{
  CHECK_ARGUMENT_BITMAP(arguments, 0x01);

  sl_status_t status = check_client_instance_id(arguments, 0);
  VERIFY_STATUS_AND_RETURN(status);

  // <subscription-count> <topic1> <qos1> <topic2> <qos2> ... <topicN> <qosN>
  // Count subscriptions and build output string
  uint32_t sub_count              = 0;
  char output[OUTPUT_BUFFER_SIZE] = { 0 }; // Buffer to store topic and QoS info
  char *buffer                    = output;
  int remaining_size              = OUTPUT_BUFFER_SIZE;

  sl_mqtt_client_topic_subscription_info_t *subscription = mqtt_client.subscription_list_head;
  while (subscription != NULL) {
    ++sub_count;

    // Add space before each topic-qos pair
    int written = snprintf(buffer, remaining_size, " ");
    if (written > 0 && written < remaining_size) {
      buffer += written;
      remaining_size -= written;
    } else {
      return SL_STATUS_WOULD_OVERFLOW;
    }

    // Add topic
    if (subscription->topic_length < remaining_size) {
      memcpy(buffer, subscription->topic, subscription->topic_length);
      buffer += subscription->topic_length;
      remaining_size -= subscription->topic_length;
    } else {
      return SL_STATUS_WOULD_OVERFLOW;
    }

    // Add QoS
    written = snprintf(buffer, remaining_size, " %d", subscription->qos_of_subscription);
    if (written > 0 && written < remaining_size) {
      buffer += written;
      remaining_size -= written;
    } else {
      return SL_STATUS_WOULD_OVERFLOW;
    }

    subscription = (sl_mqtt_client_topic_subscription_info_t *)subscription->next_subscription.node;
  }

  // Print subscription count and all topics with QoS
  PRINT_AT_CMD_SUCCESS;
  printf("%" PRIu32 "%s\r\n", sub_count, output);

  return SL_STATUS_OK;
}