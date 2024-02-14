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
#include "sl_wifi_device.h"
#include "sl_wifi_callback_framework.h"
#include "errno.h"
#include "sl_string.h"
#include "sl_ip_types.h"
#include "cmsis_os2.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "printf.h"
#include "sl_mqtt_client.h"
#include "sl_mqtt_client_types.h"
#include "command_identifiers.h"
#include "sl_si91x_core_utilities.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define MQTT_BROKER_IP   "192.168.1.27"
#define MQTT_BROKER_PORT 8886

#define CLIENT_PORT 1

#define CLIENT_ID "WIFI-SDK-MQTT-CLIENT"

#define TOPIC_TO_BE_SUBSCRIBED "THERMOSTAT-DATA\0"
#define QOS_OF_SUBSCRIPTION    SL_MQTT_QOS_LEVEL_1

#define PUBLISH_TOPIC          "WiFiSDK_TOPIC"
#define PUBLISH_MESSAGE        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do"
#define QOS_OF_PUBLISH_MESSAGE 0

#define IS_DUPLICATE_MESSAGE 0
#define IS_MESSAGE_RETAINED  1
#define IS_CLEAN_SESSION     1

#define LAST_WILL_TOPIC       "WiFiSDK-MQTT-CLIENT-LAST-WILL"
#define LAST_WILL_MESSAGE     "WiFiSDK-MQTT-CLIENT has been disconnect from network"
#define QOS_OF_LAST_WILL      1
#define IS_LAST_WILL_RETAINED 1

#define ENCRYPT_CONNECTION   0
#define KEEP_ALIVE_INTERVAL  2000
#define MQTT_CONNECT_TIMEOUT 5000

#define SEND_CREDENTIALS 0

#define USERNAME "WIFISDK"
#define PASSWORD "password"

/******************************************************
 *               Static Function Declarations
 ******************************************************/

/******************************************************
 *               Extern Variable
 ******************************************************/

/******************************************************
 *               Global Variable
 ******************************************************/

/******************************************************
 *               Variable Definitions
 ******************************************************/

sl_mqtt_client_t client = { 0 };

uint8_t is_execution_completed = 0;

sl_mqtt_client_credentials_t *client_credentails = NULL;

sl_mqtt_client_configuration_t mqtt_client_configuration = { .is_clean_session = IS_CLEAN_SESSION,
                                                             .client_id        = (uint8_t *)CLIENT_ID,
                                                             .client_id_length = strlen(CLIENT_ID),
                                                             .client_port      = CLIENT_PORT };

sl_mqtt_broker_t mqtt_broker_configuration = {
  .port                    = MQTT_BROKER_PORT,
  .is_connection_encrypted = ENCRYPT_CONNECTION,
  .connect_timeout         = MQTT_CONNECT_TIMEOUT,
  .keep_alive_interval     = KEEP_ALIVE_INTERVAL,
};

sl_mqtt_client_message_t message_to_be_published = {
  .qos_level            = QOS_OF_PUBLISH_MESSAGE,
  .is_retained          = IS_MESSAGE_RETAINED,
  .is_duplicate_message = IS_DUPLICATE_MESSAGE,
  .topic                = (uint8_t *)PUBLISH_TOPIC,
  .topic_length         = strlen(PUBLISH_TOPIC),
  .content              = (uint8_t *)PUBLISH_MESSAGE,
  .content_length       = strlen(PUBLISH_MESSAGE),
};

sl_mqtt_client_last_will_message_t last_will_message = {
  .is_retained         = IS_LAST_WILL_RETAINED,
  .will_qos_level      = QOS_OF_LAST_WILL,
  .will_topic          = (uint8_t *)LAST_WILL_TOPIC,
  .will_topic_length   = strlen(LAST_WILL_TOPIC),
  .will_message        = (uint8_t *)LAST_WILL_MESSAGE,
  .will_message_length = strlen(LAST_WILL_MESSAGE),
};

/******************************************************
 *               Function Declarations
 ******************************************************/

void mqtt_client_message_handler(void *client, sl_mqtt_client_message_t *message, void *context);
void mqtt_client_event_handler(void *client, sl_mqtt_client_event_t event, void *event_data, void *context);
void mqtt_client_error_event_handler(void *client, sl_mqtt_client_error_status_t *error);
void mqtt_client_cleanup();
void print_char_buffer(char *buffer, uint32_t buffer_length);

/******************************************************
 *               Function Definitions
 ******************************************************/

void mqtt_client_cleanup()
{
  SL_CLEANUP_MALLOC(client_credentails);
}

void print_char_buffer(char *buffer, uint32_t buffer_length)
{
  for (uint32_t index = 0; index < buffer_length; index++) {
    printf("%c", buffer[index]);
  }
  printf("\n"); //\n to separate the topic and content data
}

void mqtt_client_message_handler(void *client, sl_mqtt_client_message_t *message, void *context)
{
  UNUSED_PARAMETER(context);
  printf("\\"); //Preamble for capturing received messages on HostSerialConnectionAsync

  print_char_buffer((char *)message->topic, message->topic_length);
  print_char_buffer((char *)message->content, message->content_length);
}

void mqtt_client_event_handler(void *client, sl_mqtt_client_event_t event, void *event_data, void *context)
{
  PRINT_EVENT_START(event, WIFI)
  switch (event) {
    case SL_MQTT_CLIENT_CONNECTED_EVENT: {

      printf("SL_MQTT_CLIENT_CONNECTED_EVENT");
      break;
    }

    case SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT: {
      //      sl_mqtt_client_message_t *published_message = (sl_mqtt_client_message_t *)context;

      printf("SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT");
      break;
    }

    case SL_MQTT_CLIENT_SUBSCRIBED_EVENT: {
      printf("SL_MQTT_CLIENT_SUBSCRIBED_EVENT");
      break;
    }

    case SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT: {
      printf("SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT");

      break;
    }

    case SL_MQTT_CLIENT_DISCONNECTED_EVENT: {
      printf("SL_MQTT_CLIENT_DISCONNECTED_EVENT");
      break;
    }

    case SL_MQTT_CLIENT_ERROR_EVENT: {
      printf("SL_MQTT_CLIENT_ERROR_EVENT");
      mqtt_client_error_event_handler(client, (sl_mqtt_client_error_status_t *)event_data);
      break;
    }
    default:
      break;
  }
  PRINT_EVENT_END();
}

void mqtt_client_error_event_handler(void *client, sl_mqtt_client_error_status_t *error)
{
  UNUSED_PARAMETER(client);
  printf("\nFirmware Status: 0x%lx\n", sl_si91x_get_saved_firmware_status());
  printf("Terminating program, Error: %d", *error);
  mqtt_client_cleanup();
}

sl_status_t sl_wifi_set_advanced_mqtt_configuration_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  mqtt_broker_configuration.port = GET_OPTIONAL_COMMAND_ARG(arguments, 0, MQTT_BROKER_PORT, uint16_t);
  mqtt_broker_configuration.is_connection_encrypted = GET_OPTIONAL_COMMAND_ARG(arguments, 1, ENCRYPT_CONNECTION, bool);
  mqtt_broker_configuration.connect_timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 2, MQTT_CONNECT_TIMEOUT, uint16_t);
  mqtt_broker_configuration.keep_alive_interval = GET_OPTIONAL_COMMAND_ARG(arguments, 3, KEEP_ALIVE_INTERVAL, uint16_t);

  mqtt_client_configuration.is_clean_session = GET_OPTIONAL_COMMAND_ARG(arguments, 4, IS_CLEAN_SESSION, bool);
  mqtt_client_configuration.client_id        = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 5, CLIENT_ID, char *);
  mqtt_client_configuration.client_id_length = GET_OPTIONAL_COMMAND_ARG(arguments, 6, strlen(CLIENT_ID), uint8_t);
  mqtt_client_configuration.client_port      = GET_OPTIONAL_COMMAND_ARG(arguments, 7, CLIENT_PORT, uint16_t);

  last_will_message.is_retained         = GET_OPTIONAL_COMMAND_ARG(arguments, 8, IS_LAST_WILL_RETAINED, bool);
  last_will_message.will_qos_level      = GET_OPTIONAL_COMMAND_ARG(arguments, 9, QOS_OF_LAST_WILL, sl_mqtt_qos_t);
  last_will_message.will_topic          = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 10, LAST_WILL_TOPIC, char *);
  last_will_message.will_topic_length   = GET_OPTIONAL_COMMAND_ARG(arguments, 11, strlen(LAST_WILL_TOPIC), uint16_t);
  last_will_message.will_message        = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 12, LAST_WILL_MESSAGE, char *);
  last_will_message.will_message_length = GET_OPTIONAL_COMMAND_ARG(arguments, 13, strlen(LAST_WILL_MESSAGE), uint32_t);

  return status;
}

sl_status_t mqtt_client_connect_to_server_command_handler(console_args_t *arguments)
{

  sl_status_t status          = SL_STATUS_OK;
  status                      = sl_net_inet_addr((char *)arguments->arg[0], &mqtt_broker_configuration.ip.ip.v4.value);
  const bool send_credentials = GET_OPTIONAL_COMMAND_ARG(arguments, 1, false, const bool);
  const char *username        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, USERNAME, const char *);
  const char *password        = GET_OPTIONAL_COMMAND_ARG(arguments, 3, PASSWORD, const char *);
  uint32_t timeout_ms         = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t);

  if (send_credentials) {

    uint16_t username_length, password_length;

    username_length = strlen(username);
    password_length = strlen(password);

    uint32_t malloc_size = sizeof(sl_mqtt_client_credentials_t) + username_length + password_length;

    client_credentails = malloc(malloc_size);

    client_credentails->username_length = username_length;
    client_credentails->password_length = password_length;

    memcpy(&client_credentails->data[0], username, username_length);
    memcpy(&client_credentails->data[username_length], password, password_length);

    status = sl_net_set_credential(SL_NET_MQTT_CLIENT_CREDENTIAL_ID(0),
                                   SL_NET_MQTT_CLIENT_CREDENTIAL,
                                   client_credentails,
                                   malloc_size);

    if (status != SL_STATUS_OK) {
      mqtt_client_cleanup();
      printf("Failed to set credentials: 0x%lx", status);

      return status;
    }
    free(client_credentails);
    mqtt_client_configuration.credential_id = SL_NET_MQTT_CLIENT_CREDENTIAL_ID(0);
  }
  //Init MQTT client
  status = sl_mqtt_client_init(&client, mqtt_client_event_handler);

  if (status != SL_STATUS_OK) {
    printf("Failed to init mqtt client: 0x%lx", status);
    mqtt_client_cleanup();
  }

  //Connection to MQTT Broker
  status = sl_mqtt_client_connect(&client,
                                  &mqtt_broker_configuration,
                                  &last_will_message,
                                  &mqtt_client_configuration,
                                  timeout_ms);

  if ((timeout_ms != 0 && status != SL_STATUS_OK) || (timeout_ms == 0 && status != SL_STATUS_IN_PROGRESS)) {
    printf("Failed to connect to mqtt broker: 0x%lx", status);
    mqtt_client_cleanup();
    return status;
  }

  return status;
}

sl_status_t mqtt_client_subscribe_command_handler(console_args_t *arguments)
{

  sl_status_t status      = SL_STATUS_OK;
  char *topic             = GET_OPTIONAL_COMMAND_ARG(arguments, 0, TOPIC_TO_BE_SUBSCRIBED, char *);
  uint16_t topic_length   = GET_OPTIONAL_COMMAND_ARG(arguments, 1, strlen(TOPIC_TO_BE_SUBSCRIBED), uint16_t);
  sl_mqtt_qos_t qos_level = GET_OPTIONAL_COMMAND_ARG(arguments, 2, QOS_OF_SUBSCRIPTION, sl_mqtt_qos_t);
  uint32_t timeout_ms     = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint32_t);
  //Subscribe to the MQTT service on topic
  status = sl_mqtt_client_subscribe(&client,
                                    (uint8_t *)topic,
                                    topic_length,
                                    qos_level,
                                    timeout_ms,
                                    mqtt_client_message_handler,
                                    topic);
  if ((timeout_ms != 0 && status != SL_STATUS_OK) || (timeout_ms == 0 && status != SL_STATUS_IN_PROGRESS)) {
    printf("Failed to subscribe : 0x%lx", status);
    mqtt_client_cleanup();
    return status;
  }
  return status;
}

sl_status_t mqtt_client_publish_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  message_to_be_published.qos_level   = GET_OPTIONAL_COMMAND_ARG(arguments, 0, QOS_OF_SUBSCRIPTION, sl_mqtt_qos_t);
  message_to_be_published.is_retained = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 1, bool);
  message_to_be_published.is_duplicate_message = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, bool);
  message_to_be_published.topic          = (uint8_t *)GET_OPTIONAL_COMMAND_ARG(arguments, 3, PUBLISH_TOPIC, char *);
  message_to_be_published.topic_length   = GET_OPTIONAL_COMMAND_ARG(arguments, 4, strlen(PUBLISH_TOPIC), uint16_t);
  message_to_be_published.content        = (void *)GET_OPTIONAL_COMMAND_ARG(arguments, 5, PUBLISH_MESSAGE, char *);
  message_to_be_published.content_length = GET_OPTIONAL_COMMAND_ARG(arguments, 6, strlen(PUBLISH_MESSAGE), uint16_t);

  uint32_t timeout_ms = GET_OPTIONAL_COMMAND_ARG(arguments, 7, 0, uint32_t);
  status              = sl_mqtt_client_publish(&client, &message_to_be_published, timeout_ms, &message_to_be_published);
  if ((timeout_ms != 0 && status != SL_STATUS_OK) || (timeout_ms == 0 && status != SL_STATUS_IN_PROGRESS)) {
    printf("Failed to publish message: 0x%lx", status);
    mqtt_client_cleanup();
    return status;
  }
  return status;
}

sl_status_t mqtt_client_unsubscribe_command_handler(console_args_t *arguments)
{

  sl_status_t status  = SL_STATUS_OK;
  char *topic         = GET_OPTIONAL_COMMAND_ARG(arguments, 0, TOPIC_TO_BE_SUBSCRIBED, char *);
  uint16_t length     = GET_OPTIONAL_COMMAND_ARG(arguments, 1, strlen(TOPIC_TO_BE_SUBSCRIBED), uint16_t);
  uint32_t timeout_ms = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint32_t);
  status              = sl_mqtt_client_unsubscribe(&client, (uint8_t *)topic, length, timeout_ms, topic);
  if ((timeout_ms != 0 && status != SL_STATUS_OK) || (timeout_ms == 0 && status != SL_STATUS_IN_PROGRESS)) {
    printf("Failed to unsubscribe : 0x%lx", status);
    mqtt_client_cleanup();
    return status;
  }
  return status;
}

sl_status_t mqtt_client_deinit_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;
  status             = sl_mqtt_client_deinit(&client);
  if (status != SL_STATUS_OK) {
    printf("Failed to deinit : 0x%lx", status);
    return status;
  }
  return status;
}

sl_status_t mqtt_client_disconnect_command_handler(console_args_t *arguments)
{

  sl_status_t status       = SL_STATUS_OK;
  uint32_t connect_timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, uint32_t);
  status                   = sl_mqtt_client_disconnect(&client, connect_timeout);
  if ((connect_timeout != 0 && status != SL_STATUS_OK) || (connect_timeout == 0 && status != SL_STATUS_IN_PROGRESS)) {
    printf("Failed to disconnect: 0x%lx", status);
    return status;
  }
  return status;
}
