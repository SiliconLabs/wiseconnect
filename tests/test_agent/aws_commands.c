#include <string.h>
#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_net_wifi_types.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "console.h"
#include "sl_utility.h"
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi_callback_framework.h"
#include "errno.h"
#include "sl_string.h"
#include "sl_ip_types.h"
#include "printf.h"
#include "command_identifiers.h"
#include "sl_si91x_socket.h"
#include <string.h>

//! AWS files
#include "aws_iot_error.h"
#include "aws_iot_config.h"
#include "aws_iot_shadow_interface.h"

/******************************************************
*                    Constants
******************************************************/
#define JSON_DOC_BUFFER_LENGTH       300
#define ROOM_TEMPERATURE_UPPER_LIMIT 32
#define ROOM_TEMPERATURE_LOWER_LIMIT 25
#define STARTING_ROOM_TEMPERATURE    ROOM_TEMPERATURE_LOWER_LIMIT
#define ENABLE_POWER_SAVE            0

/******************************************************
 *          Default values for AWS IOT variables
 ******************************************************/
#undef AWS_IOT_MQTT_TX_BUF_LEN
#undef AWS_IOT_MQTT_RX_BUF_LEN
#define AWS_IOT_MQTT_TX_BUF_LEN 1400
#define AWS_IOT_MQTT_RX_BUF_LEN 1400

char json_document_buffer[JSON_DOC_BUFFER_LENGTH];
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"

/******************************************************
 *               Function Variable
 ******************************************************/
static IoT_Error_t rc                               = -1;
AWS_IoT_Client mqtt_client                          = { 0 };
ShadowInitParameters_t shadow_init_parameters       = { 0 };
ShadowConnectParameters_t shadow_connect_parameters = { 0 };
static bool window_open_state                       = false;
jsonStruct_t window_actuator                        = { 0 };
static float temperature                            = 0.0;
jsonStruct_t temperature_handler                    = { 0 };
size_t size_of_json_document_buffer                 = sizeof(json_document_buffer) / sizeof(json_document_buffer[0]);

IoT_Client_Init_Params mqtt_init_params       = { 0 };
IoT_Client_Connect_Params mqtt_connect_params = { 0 };
IoT_Publish_Message_Params publish_QOS0       = { 0 };
IoT_Publish_Message_Params publish_iot_msg    = { 0 };

int32_t status                       = RSI_SUCCESS;
fd_set read_fds                      = { 0 };
volatile uint8_t check_for_recv_data = 0, is_select_given = 0, recv_data = 1;

char *topic  = NULL;
char *topic1 = NULL;

/******************************************************
 *               Function Declarations
 ******************************************************/
sl_status_t aws_iot_shadow_init_command(console_args_t *arguments);
sl_status_t aws_iot_shadow_connect_command();
sl_status_t aws_iot_shadow_register_delta_command();
sl_status_t aws_iot_shadow_yield_command();
sl_status_t aws_iot_shadow_init_json_document_command();
sl_status_t aws_iot_shadow_add_reported_command();
sl_status_t aws_iot_finalize_json_document_command();
sl_status_t aws_iot_shadow_update_command();
IoT_Error_t aws_iot_mqtt_init_command(console_args_t *arguments);
IoT_Error_t aws_iot_mqtt_connect_command();
IoT_Error_t aws_iot_mqtt_autoreconnect_set_status_command();
IoT_Error_t aws_iot_mqtt_subscribe_command(console_args_t *arguments);
IoT_Error_t aws_iot_mqtt_publish_command(console_args_t *arguments);
IoT_Error_t aws_iot_mqtt_publish_command(console_args_t *arguments);
IoT_Error_t aws_iot_mqtt_disconnect_command();
IoT_Error_t aws_iot_mqtt_unsubscribe_command(console_args_t *arguments);
IoT_Error_t aws_select_socket_command();

static void simulate_room_temperature_command(float *p_room_temperature, bool *p_window_open);
static void window_actuate_callback_command(const char *p_json_string,
                                            uint32_t json_string_data_len,
                                            jsonStruct_t *p_context);
static void shadow_update_status_callback_command(const char *p_thing_name,
                                                  ShadowActions_t action,
                                                  Shadow_Ack_Status_t status,
                                                  const char *p_received_json_document,
                                                  void *p_context_data);
void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data);
void subscribe_handler(struct _Client *pClient,
                       char *pTopicName,
                       short unsigned int topicNameLen,
                       IoT_Publish_Message_Params *pParams,
                       void *data);
void async_socket_select(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status);

/******************************************************
 *               Function Definitions
 ******************************************************/

sl_status_t aws_iot_shadow_init_command(console_args_t *arguments)
{

  shadow_init_parameters    = ShadowInitParametersDefault;
  shadow_connect_parameters = ShadowConnectParametersDefault;

  shadow_init_parameters.pHost              = (char *)arguments->arg[0];
  shadow_init_parameters.port               = (uint16_t)arguments->arg[1];
  shadow_connect_parameters.pMyThingName    = (char *)arguments->arg[2];
  shadow_connect_parameters.pMqttClientId   = (char *)arguments->arg[3];
  shadow_connect_parameters.mqttClientIdLen = (uint16_t)strlen(shadow_connect_parameters.pMqttClientId);

  shadow_init_parameters.pRootCA             = (char *)""; //aws_starfield_ca;
  shadow_init_parameters.pClientCRT          = (char *)""; //aws_client_certificate;
  shadow_init_parameters.pClientKey          = (char *)""; //aws_client_private_key;
  shadow_init_parameters.enableAutoReconnect = true;

  rc = aws_iot_shadow_init(&mqtt_client, &shadow_init_parameters);
  if (rc < 0) {
    printf("\nShadow Initialization failed with aws error: %d\n", rc);
  } else {
    printf("\nShadow Initialization Success\n");
  }
  return rc;
}

sl_status_t aws_iot_shadow_connect_command()
{

  rc = aws_iot_shadow_connect(&mqtt_client, &shadow_connect_parameters);
  if (rc < 0) {
    printf("\nShadow Connection failed with aws error: %d\n", rc);
  } else {
    printf("\nShadow Connection Success\n");
  }
  return rc;
}

sl_status_t aws_iot_shadow_register_delta_command()
{

  window_actuator.pKey       = "Window Open";
  window_actuator.pData      = &window_open_state;
  window_actuator.dataLength = sizeof(bool);
  window_actuator.type       = SHADOW_JSON_BOOL;
  window_actuator.cb         = window_actuate_callback_command;

  temperature_handler.pKey       = "Temperature";
  temperature_handler.pData      = &temperature;
  temperature_handler.dataLength = sizeof(float);
  temperature_handler.type       = SHADOW_JSON_FLOAT;
  temperature_handler.cb         = NULL;

  temperature = STARTING_ROOM_TEMPERATURE;

  rc = aws_iot_shadow_register_delta(&mqtt_client, &window_actuator);
  if (rc != SUCCESS) {
    printf("\nShadow Register Delta failed with aws error: %d\n", rc);
    return rc;
  } else {
    printf("\nShadow Register Delta\n");
  }
  return rc;
}

sl_status_t aws_iot_shadow_yield_command()
{

  if ((NETWORK_ATTEMPTING_RECONNECT == rc) || (NETWORK_RECONNECTED == rc) || (SUCCESS == rc)) {
    rc = aws_iot_shadow_yield(&mqtt_client, 2000);
    if (rc == NETWORK_ATTEMPTING_RECONNECT) {
      printf("\naws_iot_shadow_yield :0x%x\n", rc);
    }
  }
  return 0;
}

sl_status_t aws_iot_shadow_init_json_document_command()
{

  simulate_room_temperature_command(&temperature, &window_open_state);

  rc = aws_iot_shadow_init_json_document(json_document_buffer, size_of_json_document_buffer);
  if (rc != SUCCESS) {
    printf("\nFailed to initialize JSON buffer with error:0x%x\n", rc);
  }
  return rc;
}

sl_status_t aws_iot_shadow_add_reported_command()
{

  rc = aws_iot_shadow_add_reported(json_document_buffer,
                                   size_of_json_document_buffer,
                                   2,
                                   &temperature_handler,
                                   &window_actuator);
  if (rc != SUCCESS) {
    printf("\nFailed to add reported value in JSON buffer with error:0x%x\n", rc);
    return rc;
  }
  printf("\nAdding reported value in JSON buffer success\n");
  return rc;
}
sl_status_t aws_iot_finalize_json_document_command()
{

  rc = aws_iot_finalize_json_document(json_document_buffer, size_of_json_document_buffer);
  if (rc != SUCCESS) {
    printf("\nFailed to finalize JSON buffer with error:0x%x\n", rc);
    return rc;
  }
  printf("\nJSON finalization buffer Success\n");
  printf("\nUpdate Shadow: %s\n", json_document_buffer);
  return rc;
}
sl_status_t aws_iot_shadow_update_command()
{

  rc = aws_iot_shadow_update(&mqtt_client,
                             AWS_IOT_MY_THING_NAME,
                             json_document_buffer,
                             shadow_update_status_callback_command,
                             NULL,
                             40,
                             true);
  if (rc != SUCCESS) {
    printf("\nFailed to update JSON buffer with error:0x%x\n", rc);
  } else {
    printf("\nSuccess to update JSON buffer\n");
  }
  return rc;
}

IoT_Error_t aws_iot_mqtt_init_command(console_args_t *arguments)
{

  mqtt_init_params    = iotClientInitParamsDefault;
  mqtt_connect_params = iotClientConnectParamsDefault;

  mqtt_init_params.enableAutoReconnect       = true;
  mqtt_init_params.pHostURL                  = (char *)arguments->arg[0];
  mqtt_init_params.port                      = (uint16_t)arguments->arg[1];
  mqtt_init_params.pRootCALocation           = (char *)""; //aws_starfield_ca;
  mqtt_init_params.pDeviceCertLocation       = (char *)""; //aws_client_certificate;
  mqtt_init_params.pDevicePrivateKeyLocation = (char *)""; //aws_client_private_key;
  mqtt_init_params.mqttCommandTimeout_ms     = 20000;
  mqtt_init_params.tlsHandshakeTimeout_ms    = 5000;
  mqtt_init_params.isSSLHostnameVerify       = true;
  mqtt_init_params.disconnectHandler         = disconnect_notify_handler;
  mqtt_init_params.disconnectHandlerData     = NULL;

  mqtt_connect_params.keepAliveIntervalInSec = (uint16_t)arguments->arg[3];
  mqtt_connect_params.isCleanSession         = (bool)arguments->arg[4];
  mqtt_connect_params.MQTTVersion            = MQTT_3_1_1;
  mqtt_connect_params.pClientID              = (char *)arguments->arg[2];
  mqtt_connect_params.clientIDLen            = (uint16_t)strlen((char *)arguments->arg[2]);
  mqtt_connect_params.isWillMsgPresent       = false;
  mqtt_connect_params.pUsername              = MQTT_USERNAME;
  mqtt_connect_params.usernameLen            = strlen(mqtt_connect_params.pUsername);
  mqtt_connect_params.pPassword              = MQTT_PASSWORD;
  mqtt_connect_params.passwordLen            = strlen(mqtt_connect_params.pPassword);

  rc = aws_iot_mqtt_init(&mqtt_client, &mqtt_init_params);
  if (SUCCESS != rc) {
    printf("\nMqtt Init failed with error: 0x%x\n", rc);
  } else {
    printf("\nInitialization Success\n");
  }
  return rc;
}

IoT_Error_t aws_iot_mqtt_connect_command()
{

  rc = aws_iot_mqtt_connect(&mqtt_client, &mqtt_connect_params);
  if (SUCCESS != rc) {
    if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
      printf("Network is already connected\n");
    } else {
      printf("\r\nMqtt Connect failed with error: 0x%x\n", rc);
    }
    rc = aws_iot_mqtt_init(&mqtt_client, &mqtt_init_params);
  } else {
    printf("Connect Success\n");
  }
  return rc;
}

IoT_Error_t aws_iot_mqtt_autoreconnect_set_status_command()
{

  rc = aws_iot_mqtt_autoreconnect_set_status(&mqtt_client, false);
  ;
  if (SUCCESS != rc) {
    if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
      printf("Network is already connected\n");
    } else {
      printf("\r\nMqtt autoreconnect set status failed with error: 0x%x\n", rc);
    }
  } else {
    printf("autoreconnect_set_status Success\n");
  }
  return rc;
}

IoT_Error_t aws_iot_mqtt_subscribe_command(console_args_t *arguments)
{
  QoS qos = (QoS)arguments->arg[0];
  topic   = (char *)arguments->arg[1];
  topic1  = (char *)malloc(strlen(topic));
  strcpy(topic1, topic);

  rc = aws_iot_mqtt_subscribe(&mqtt_client, (char *)topic1, strlen((char *)topic1), qos, subscribe_handler, NULL);

  if (SUCCESS != rc) {
    if (NETWORK_DISCONNECTED_ERROR == rc) {
      printf("\r\nSubscription failed with error: 0x%x\n", rc);
    } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
      printf("\r\nNETWORK_ATTEMPTING_RECONNECT");
    }
  } else {
    printf("\rSubscribed to topic %s successfully\n", topic1);
  }
  return rc;
}

IoT_Error_t aws_iot_mqtt_publish_command(console_args_t *arguments)
{

  char *pTopicName        = (char *)arguments->arg[0];
  uint16_t topicNameLen   = (uint16_t)strlen((char *)arguments->arg[0]);
  publish_QOS0.qos        = (QoS)arguments->arg[1];
  publish_QOS0.payload    = (char *)arguments->arg[2];
  publish_QOS0.isRetained = 0;
  publish_QOS0.payloadLen = strlen((char *)arguments->arg[2]);

  // mqtt publish with QOS0
  rc = aws_iot_mqtt_publish(&mqtt_client, pTopicName, topicNameLen, &publish_QOS0);

  if (rc != SUCCESS) {
    printf("\r\nMqtt Publish for QOS0 failed with error: 0x%x\n", rc);
  }
  if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
    printf("QOS0 publish ack not received.\n");
  }
  printf("QOS0 publish success\n");
  return rc;
}

IoT_Error_t aws_iot_mqtt_disconnect_command()
{

  // mqtt publish with QOS0
  rc = aws_iot_mqtt_disconnect(&mqtt_client);
  if (rc != SUCCESS) {
    printf("\r\nDisconnect failed with error: 0x%x\n", rc);
  }
  printf("Disconnect success\n");
  free(topic1);
  return rc;
}

IoT_Error_t aws_iot_mqtt_unsubscribe_command(console_args_t *arguments)
{

  char *topic       = (char *)arguments->arg[0];
  uint16_t topiclen = (uint16_t)strlen((char *)arguments->arg[0]);

  rc = aws_iot_mqtt_unsubscribe(&mqtt_client, topic, topiclen);

  if (SUCCESS != rc) {
    printf("\r\nUnsubscribe error: 0x%x\n", rc);
  } else {
    printf("Unsubscribe Success\n");
  }
  free(topic1);
  return rc;
}

IoT_Error_t aws_select_socket_command()
{

  memset(&read_fds, 0, sizeof(fd_set));

  FD_SET(mqtt_client.networkStack.socket_id, &read_fds);
  printf("\rSocket ID: %d\n", mqtt_client.networkStack.socket_id);

  status = sl_si91x_select(mqtt_client.networkStack.socket_id + 1, &read_fds, NULL, NULL, NULL, async_socket_select);
  printf("\rSelect status: %ld", status);

  rc = aws_iot_shadow_yield(&mqtt_client, 1);
  if (SUCCESS == rc) {
    printf("\ryield status: 0x%x\n", rc);
  }

  return status;
}

static void simulate_room_temperature_command(float *p_room_temperature, bool *p_window_open)
{
  static float delta_change;

  if (*p_room_temperature >= ROOM_TEMPERATURE_UPPER_LIMIT) {
    delta_change = -0.5f;
  } else if (*p_room_temperature <= ROOM_TEMPERATURE_LOWER_LIMIT) {
    delta_change = +0.5f;
  }

  *p_room_temperature += delta_change;
  *p_window_open = false;

  if (*p_room_temperature >= (ROOM_TEMPERATURE_UPPER_LIMIT + ROOM_TEMPERATURE_LOWER_LIMIT) / 2) {
    *p_window_open = true;
  }
}

static void shadow_update_status_callback_command(const char *p_thing_name,
                                                  ShadowActions_t action,
                                                  Shadow_Ack_Status_t status,
                                                  const char *p_received_json_document,
                                                  void *p_context_data)
{
  UNUSED_PARAMETER(p_thing_name);
  UNUSED_PARAMETER(action);
  UNUSED_PARAMETER(p_received_json_document);
  UNUSED_PARAMETER(p_context_data);

  if (SHADOW_ACK_TIMEOUT == status) {
    printf("\r\nUpdate Timed out!!\n");
  } else if (SHADOW_ACK_REJECTED == status) {
    printf("\r\nUpdate Rejected!!\n");
  } else if (SHADOW_ACK_ACCEPTED == status) {
    printf("\r\nUpdate Accepted!!\n");
  }
}

static void window_actuate_callback_command(const char *p_json_string,
                                            uint32_t json_string_data_len,
                                            jsonStruct_t *p_context)
{
  UNUSED_PARAMETER(p_json_string);
  UNUSED_PARAMETER(json_string_data_len);

  if (p_context != NULL) {
    printf("Delta - Window state changed to %d", *(bool *)(p_context->pData));
  }
}

void disconnect_notify_handler(AWS_IoT_Client *pClient, void *data)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(data);
  printf("\r\nMqtt disconnected abruptly and pClient state is: %d\n", pClient->clientStatus.clientState);
}

void subscribe_handler(struct _Client *pClient,
                       char *pTopicName,
                       short unsigned int topicNameLen,
                       IoT_Publish_Message_Params *pParams,
                       void *data)
{
  UNUSED_PARAMETER(pClient);
  UNUSED_PARAMETER(pTopicName);
  UNUSED_PARAMETER(topicNameLen);
  UNUSED_PARAMETER(data);
  printf("\nData received on the Subscribed Topic is %.*s \n", pParams->payloadLen, (char *)pParams->payload);
}

void async_socket_select(fd_set *fd_read, fd_set *fd_write, fd_set *fd_except, int32_t status)
{
  UNUSED_PARAMETER(fd_except);
  UNUSED_PARAMETER(fd_write);
  UNUSED_PARAMETER(status);

  //!Check the data pending on this particular socket descriptor
  if (FD_ISSET(mqtt_client.networkStack.socket_id, fd_read)) {
    check_for_recv_data = 1;
  }
}
