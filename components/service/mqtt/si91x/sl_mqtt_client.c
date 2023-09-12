#include "sl_net.h"
#include "sl_slist.h"
#include "sl_si91x_driver.h"
#include "stdint.h"
#include <stdbool.h>
#include <string.h>
#include "sl_mqtt_client.h"
#include "sl_mqtt_client_types.h"
#include "si91x_mqtt_client_types.h"
#include "si91x_mqtt_client_utility.h"
#include "sl_status.h"

/**
 * MQTT CLIENT STATE MACHINE
=======================================================|
	STATE			EVENT					NEXT_STATE |
=======================================================|
	CONNECTED	  |	CONNECT				  |	UNKNOWN
	CONNECTED	  |	RECONNECT			  |	UNKNOWN
	CONNECTED	  |	DISCONNECT			  |	DISCONNECTING
	CONNECTED	  | PUBLISH			      | CONNECTED
	CONNECTED	  |	SUBSCRIBE			  |	CONNECTED
	CONNECTED	  |	UNSUBSCRIBE			  |	CONNECTED
	CONNECTED	  |	ON_DISCONNECT_RECV^   |	DISCONNECTED
                  |                       |
	DISCONNECTED  |	CONNECT				  |	CONNECTING
	DISCONNECTED  |	RECONNECT			  |	was_connect_called_previously ? CONNECTING: UNKNOWN
	DISCONNECTED  |	ANY_OTHER_EVENT		  |	UNKNOWN
                  |                       |
	CONNECTING 	  |	ON_CONNECT_SUCCESS^   |	CONNECTED
	CONNECTING	  |	ON_CONNECT_FAILED^	  |	DISCONNECTED
	CONNECTING 	  |	DISCONNECT			  |	DISCONNECTED
	CONNECTING	  |	ANY_OTHER_EVENT		  |	UNKNOWN

	DISCONNECTING | DISCONNECT			  | DISCONNECTING
	DISCONNECTING | ON_DISCONNECT_SUCCESS^| DISCONNECTED
	DISCONNECTING | ON_DISCONNECT_FAILED^ | CONNECTED
	DISCONNECTING | ANY_OTHER_EVENT		  | UNKNOWN
============================================================

	*UNKOWN -> Throw Error
	^ -> firmware events
**/

#define SI91X_MQTT_CLIENT_INIT_TIMEOUT       5000
#define SI91X_MQTT_CLIENT_DISCONNECT_TIMEOUT 5000

#define VERIFY_AND_RETURN_ERROR_IF_FLASE(condition, status) \
  {                                                         \
    do {                                                    \
      if (!(condition)) {                                   \
        PRINT_STATUS(INFO_TAG, status)                      \
        return status;                                      \
      }                                                     \
    } while (0);                                            \
  }

static sl_mqtt_client_t *mqtt_client;

/**
 * A internal helper function to get node of list which matches the given topic.
 * @param client 		Pointer to client object whose subscription list needs to be searched.
 * @param topic			Topic which needs to be searched in list.
 * @param topic_length	Length of the topic that needs to be searched.
 * @param required_node	A double pointer to node pointer.
 */
static void get_subscription(const sl_mqtt_client_t *client,
                             const uint8_t *topic,
                             const uint16_t topic_length,
                             sl_mqtt_client_topic_subscription_info_t **required_subscription)
{
  sl_mqtt_client_topic_subscription_info_t *subscription = client->subscription_list_head;

  *required_subscription = NULL;

  while (subscription != NULL) {
    if (topic_length == subscription->topic_length
        && memcmp(subscription->topic, topic, subscription->topic_length) == 0) {
      *required_subscription = subscription;
      return;
    }

    subscription = (sl_mqtt_client_topic_subscription_info_t *)subscription->next_subscription.node;
  }
}

static void remove_and_free_all_subscriptions(sl_mqtt_client_t *client)
{
  // Free subscription list.
  sl_mqtt_client_topic_subscription_info_t *node_to_be_freed;
  while ((node_to_be_freed = (sl_mqtt_client_topic_subscription_info_t *)(sl_slist_pop(
            (sl_slist_node_t **)&client->subscription_list_head)))
         != NULL) {
    free(node_to_be_freed);
  }
}
static inline bool is_connect_previously_called(sl_mqtt_client_t *client)
{
  return (NULL != client->client_configuration);
}

sl_status_t sli_si91x_build_mqtt_sdk_context_if_async(sl_mqtt_client_event_t event,
                                                      sl_mqtt_client_t *client,
                                                      void *user_context,
                                                      void *sdk_data,
                                                      uint32_t timeout,
                                                      sl_si91x_mqtt_client_context_t **context)
{
  *context = NULL;
  if (timeout > 0) {
    return SL_STATUS_OK;
  }

  sl_si91x_mqtt_client_context_t *mqtt_client_sdk_context = calloc(sizeof(sl_si91x_mqtt_client_context_t), 1);

  mqtt_client_sdk_context->client       = client;
  mqtt_client_sdk_context->event        = event;
  mqtt_client_sdk_context->sdk_data     = sdk_data;
  mqtt_client_sdk_context->user_context = user_context;

  *context = mqtt_client_sdk_context;

  return SL_STATUS_OK;
}

void sli_si91x_get_mqtt_client(sl_mqtt_client_t **client)
{
  *client = mqtt_client;
}

/**
 * A internal function to map the event to its appropriate mqtt error state.
 * @param event	Event whose sl_mqtt_client_event_t value is required.
 * @return sl_mqtt_client_error_status_t for the Event.
 */
sl_mqtt_client_error_status_t sli_si91x_get_event_error_status(sl_mqtt_client_event_t event)
{
  switch (event) {
    case SL_MQTT_CLIENT_CONNECTED_EVENT: {
      return SL_MQTT_CLIENT_CONNECT_FAILED;
    }

    case SL_MQTT_CLIENT_DISCONNECTED_EVENT: {
      return SL_MQTT_CLIENT_DISCONNECT_FAILED;
    }

    case SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT: {
      return SL_MQTT_CLIENT_PUBLISH_FAILED;
    }

    case SL_MQTT_CLIENT_SUBSCRIBED_EVENT: {
      return SL_MQTT_CLIENT_SUBSCRIBE_FAILED;
    }

    case SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT: {
      return SL_MQTT_CLIENT_UNSUBSCRIBED_FAILED;
    }

    default: {
      return SL_MQTT_CLIENT_UNKNKOWN_ERROR;
    }
  }
}

sl_status_t sl_mqtt_client_init(sl_mqtt_client_t *client, sl_mqtt_client_event_handler event_handler)
{
  SL_VERIFY_POINTER_OR_RETURN(event_handler, SL_STATUS_WIFI_NULL_PTR_ARG);

  client->client_event_handler = event_handler;
  sl_slist_init((sl_slist_node_t **)&client->subscription_list_head);

  mqtt_client = client;
  return SL_STATUS_OK;
}

sl_status_t sl_mqtt_client_deinit(sl_mqtt_client_t *client)
{

  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_DISCONNECTED, SL_STATUS_INVALID_STATE);
  memset(client, 0, sizeof(sl_mqtt_client_t));

  mqtt_client = NULL;
  return SL_STATUS_OK;
}

sl_status_t sl_mqtt_client_connect(sl_mqtt_client_t *client,
                                   const sl_mqtt_broker_t *broker,
                                   const sl_mqtt_client_last_will_message_t *last_will,
                                   const sl_mqtt_client_configuration_t *configuration,
                                   uint32_t connect_timeout)
{
  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_DISCONNECTED, SL_STATUS_INVALID_STATE);

  // check if this the first time connect() call being made,
  // In subsequent calls it not mandatory to pass parameters as we store them in client structure
  if (!is_connect_previously_called(client)) {
    SL_VERIFY_POINTER_OR_RETURN(client, SL_STATUS_WIFI_NULL_PTR_ARG);
    SL_VERIFY_POINTER_OR_RETURN(broker, SL_STATUS_WIFI_NULL_PTR_ARG);
    SL_VERIFY_POINTER_OR_RETURN(configuration, SL_STATUS_WIFI_NULL_PTR_ARG);
  }

  sl_status_t status;
  si91x_mqtt_client_init_request_t si91x_init_request       = { 0 };
  si91x_mqtt_client_connect_request_t si91x_connect_request = { 0 };
  sl_si91x_mqtt_client_context_t *sdk_context               = NULL;

  // If user provides valid values in subsequent calls, we store the new values else we keep referring to structures provided in first connect call.
  client->broker               = broker == NULL ? client->broker : broker;
  client->client_configuration = configuration == NULL ? client->client_configuration : configuration;

  // Special case for last_will, as NULL can be a legitimate value if client wouldn't want to provide a last will.
  client->last_will_message = last_will;

  memcpy(&si91x_init_request.server_ip.server_ip_address,
         &client->broker->ip.ip,
         client->broker->ip.type == SL_IPV4 ? SL_IPV4_ADDRESS_LENGTH : SL_IPV6_ADDRESS_LENGTH);

  si91x_init_request.command_type = SI91X_MQTT_CLIENT_INIT_COMMAND;

  si91x_init_request.server_port = client->broker->port;
  si91x_init_request.clean       = client->client_configuration->is_clean_session;
  si91x_init_request.encrypt     = client->broker->is_connection_encrypted;
  si91x_init_request.client_port = client->client_configuration->client_port;

  si91x_init_request.client_id_len = client->client_configuration->client_id_length;
  memcpy(si91x_init_request.client_id,
         client->client_configuration->client_id,
         client->client_configuration->client_id_length);

  si91x_init_request.keep_alive_interval = client->broker->keep_alive_interval;

  if (client->client_configuration->credential_id != (sl_net_credential_id_t)SL_NET_INVALID_CREDENTIAL_TYPE) {

    uint32_t maximum_credential_size = sizeof(sl_mqtt_client_credentials_t) + SI91X_MQTT_CLIENT_USERNAME_MAXIMUM_LENGTH
                                       + SI91X_MQTT_CLIENT_PASSWORD_MAXIMUM_LENGTH;

    sl_mqtt_client_credentials_t *credentials = malloc(maximum_credential_size);

    if (credentials == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }

    sl_net_credential_type_t type;

    sl_status_t status =
      sl_net_get_credential(client->client_configuration->credential_id, &type, credentials, &maximum_credential_size);

    if (status != SL_STATUS_OK || type != SL_NET_MQTT_CLIENT_CREDENTIALS) {
      free(credentials);
      return status != SL_STATUS_OK ? status : SL_STATUS_INVALID_CREDENTIALS;
    }

    memcpy(si91x_init_request.user_name, &credentials->data[0], credentials->username_length);

    // credentails.username_length is being used as offset as we store both user_name, password in same array.
    memcpy(si91x_init_request.password, &credentials->data[credentials->username_length], credentials->password_length);

    si91x_init_request.username_len = credentials->username_length;
    si91x_init_request.password_len = credentials->password_length;

    si91x_connect_request.is_password_present = 1;
    si91x_connect_request.is_username_present = 1;

    free(credentials);
  }

  client->state = SL_MQTT_CLIENT_CONNECTING;

  // Host connect() call maps to two commands in firmware, init() and connect()
  // since we can't send(At least with current design) two command in aysnc mode,
  // We send init command in sync mode, whereas, connect will be sent as async
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &si91x_init_request,
                                        sizeof(si91x_init_request),
                                        SL_SI91X_WAIT_FOR(SI91X_MQTT_CLIENT_INIT_TIMEOUT),
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_GOTO(status, revert_client_state_and_return_error);

  si91x_connect_request.command_type = SI91X_MQTT_CLIENT_CONNECT_COMMAND;

  if (client->last_will_message != NULL) {
    si91x_connect_request.will_retain      = client->last_will_message->is_retained;
    si91x_connect_request.will_qos         = client->last_will_message->will_qos_level;
    si91x_connect_request.will_topic_len   = client->last_will_message->will_topic_length;   // Narrowing of variable
    si91x_connect_request.will_message_len = client->last_will_message->will_message_length; // Narrowing of variable

    memcpy(si91x_connect_request.will_topic,
           client->last_will_message->will_topic,
           client->last_will_message->will_topic_length);
    memcpy(si91x_connect_request.will_msg,
           client->last_will_message->will_message,
           client->last_will_message->will_message_length);

    si91x_connect_request.will_flag = 1;
  }

  status = sli_si91x_build_mqtt_sdk_context_if_async(SL_MQTT_CLIENT_CONNECTED_EVENT,
                                                     client,
                                                     NULL,
                                                     NULL,
                                                     connect_timeout,
                                                     &sdk_context);
  VERIFY_STATUS_AND_GOTO(status, revert_client_state_and_return_error);

  status = sl_si91x_driver_send_command(
    RSI_WLAN_REQ_EMB_MQTT_CLIENT,
    SI91X_NETWORK_CMD_QUEUE,
    &si91x_connect_request,
    sizeof(si91x_connect_request),
    connect_timeout == 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR(connect_timeout),
    sdk_context,
    NULL);

  if (status == SL_STATUS_IN_PROGRESS) {
    return status;
  } else if (status != SL_STATUS_OK) {
    client->state = SL_MQTT_CLIENT_DISCONNECTED;

    SL_CLEANUP_MALLOC(sdk_context);
    return status;
  }

  client->state = SL_MQTT_CLIENT_CONNECTED;
  return SL_STATUS_OK;

revert_client_state_and_return_error:
  client->state = SL_MQTT_CLIENT_DISCONNECTED;
  return status;
}

sl_status_t sl_mqtt_client_disconnect(sl_mqtt_client_t *client, uint32_t timeout)
{

  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_CONNECTED, SL_STATUS_INVALID_STATE);
  SL_VERIFY_POINTER_OR_RETURN(client, SL_STATUS_WIFI_NULL_PTR_ARG);

  client->state = SL_MQTT_CLIENT_DISCONNECTING;

  sl_status_t status;
  sl_si91x_mqtt_client_context_t *sdk_context = NULL;

  // As in connect, disconnect() call maps to disconnect and deint in firmware
  si91x_mqtt_client_command_request_t si91x_deint_request      = { .command_type = SI91X_MQTT_CLIENT_DEINIT_COMMAND };
  si91x_mqtt_client_command_request_t si91x_disconnect_request = { .command_type =
                                                                     SI91X_MQTT_CLIENT_DISCONNECT_COMMMAND };

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &si91x_disconnect_request,
                                        sizeof(si91x_disconnect_request),
                                        SL_SI91X_WAIT_FOR(SI91X_MQTT_CLIENT_DISCONNECT_TIMEOUT),
                                        sdk_context,
                                        NULL);

  VERIFY_STATUS_AND_GOTO(status, revert_client_state_and_return_error);

  status = sli_si91x_build_mqtt_sdk_context_if_async(SL_MQTT_CLIENT_DISCONNECTED_EVENT,
                                                     client,
                                                     NULL,
                                                     NULL,
                                                     timeout,
                                                     &sdk_context);
  VERIFY_STATUS_AND_GOTO(status, revert_client_state_and_return_error);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &si91x_deint_request,
                                        sizeof(si91x_deint_request),
                                        timeout <= 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR(timeout),
                                        sdk_context,
                                        NULL);

  if (status == SL_STATUS_IN_PROGRESS) {
    return status;
  } else if (status != SL_STATUS_OK) {

    client->state = SL_MQTT_CLIENT_CONNECTED;
    SL_CLEANUP_MALLOC(sdk_context);
    return status;
  }

  client->state = SL_MQTT_CLIENT_DISCONNECTED;
  remove_and_free_all_subscriptions(client);

  return status;

revert_client_state_and_return_error:
  client->state = SL_MQTT_CLIENT_CONNECTED;
  return status;
}

sl_status_t sl_mqtt_client_publish(sl_mqtt_client_t *client,
                                   const sl_mqtt_client_message_t *message,
                                   uint32_t timeout,
                                   void *context)
{
  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_CONNECTED, SL_STATUS_INVALID_STATE);

  SL_VERIFY_POINTER_OR_RETURN(client, SL_STATUS_WIFI_NULL_PTR_ARG);

  if (message->topic_length > SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status;
  sl_si91x_mqtt_client_context_t *sdk_context = NULL;
  uint32_t publish_request_size               = sizeof(si91x_mqtt_client_publish_request_t) + message->content_length;

  si91x_mqtt_client_publish_request_t *si91x_publish_request = calloc(publish_request_size, 1);
  status = sli_si91x_build_mqtt_sdk_context_if_async(SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT,
                                                     client,
                                                     context,
                                                     NULL,
                                                     timeout,
                                                     &sdk_context);

  if (si91x_publish_request == NULL || status != SL_STATUS_OK) {
    SL_CLEANUP_MALLOC(si91x_publish_request);
    SL_CLEANUP_MALLOC(sdk_context);

    return SL_STATUS_ALLOCATION_FAILED;
  }

  si91x_publish_request->command_type = SI91X_MQTT_CLIENT_PUBLISH_COMMMAND;

  si91x_publish_request->dup      = message->is_duplicate_message;
  si91x_publish_request->qos      = message->qos_level;
  si91x_publish_request->retained = message->is_retained;

  si91x_publish_request->topic_len = message->topic_length;   // Narrowing of variable
  si91x_publish_request->msg_len   = message->content_length; // Narrowing of variable

  si91x_publish_request->msg = (int8_t *)si91x_publish_request + sizeof(si91x_mqtt_client_publish_request_t);
  memcpy(si91x_publish_request->topic, message->topic, message->topic_length);
  memcpy(si91x_publish_request->msg, message->content, message->content_length);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        si91x_publish_request,
                                        publish_request_size,
                                        timeout <= 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR(timeout),
                                        sdk_context,
                                        NULL);
  free(si91x_publish_request);

  if (status == SL_STATUS_IN_PROGRESS) {
    return status;
  }

  SL_CLEANUP_MALLOC(sdk_context);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sl_mqtt_client_subscribe(sl_mqtt_client_t *client,
                                     const uint8_t *topic,
                                     uint16_t topic_length,
                                     sl_mqtt_qos_t qos_level,
                                     uint32_t timeout,
                                     sl_mqtt_client_message_received message_handler,
                                     void *context)
{
  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_CONNECTED, SL_STATUS_INVALID_STATE);

  SL_VERIFY_POINTER_OR_RETURN(client, SL_STATUS_WIFI_NULL_PTR_ARG);
  SL_VERIFY_POINTER_OR_RETURN(topic, SL_STATUS_WIFI_NULL_PTR_ARG);
  SL_VERIFY_POINTER_OR_RETURN(message_handler, SL_STATUS_WIFI_NULL_PTR_ARG);

  if (topic_length > SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status;
  si91x_mqtt_client_subscribe_t si91x_subscribe_request = { 0 };
  sl_si91x_mqtt_client_context_t *sdk_context           = NULL;

  sl_mqtt_client_topic_subscription_info_t *subscripton =
    calloc(sizeof(sl_mqtt_client_topic_subscription_info_t) + topic_length, 1);

  status = sli_si91x_build_mqtt_sdk_context_if_async(SL_MQTT_CLIENT_SUBSCRIBED_EVENT,
                                                     client,
                                                     context,
                                                     subscripton,
                                                     timeout,
                                                     &sdk_context);

  if (subscripton == NULL || status != SL_STATUS_OK) {
    SL_CLEANUP_MALLOC(subscripton);
    SL_CLEANUP_MALLOC(sdk_context);

    return SL_STATUS_ALLOCATION_FAILED;
  }

  si91x_subscribe_request.command_type = SI91X_MQTT_CLIENT_SUBSCRIBE_COMMMAND;

  si91x_subscribe_request.topic_len = topic_length;
  si91x_subscribe_request.qos       = qos_level;

  subscripton->topic_length          = topic_length;
  subscripton->topic_message_handler = message_handler;

  memcpy(si91x_subscribe_request.topic, topic, topic_length);
  memcpy(subscripton->topic, topic, topic_length);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &si91x_subscribe_request,
                                        sizeof(si91x_subscribe_request),
                                        timeout <= 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR(timeout),
                                        sdk_context,
                                        NULL);

  if (status == SL_STATUS_IN_PROGRESS) {
    return status;
  } else if (status != SL_STATUS_OK) {

    free(subscripton);
    SL_CLEANUP_MALLOC(sdk_context);
    return status;
  }

  sl_slist_push((sl_slist_node_t **)&client->subscription_list_head, (sl_slist_node_t *)subscripton);
  return status;
}

sl_status_t sl_mqtt_client_unsubscribe(sl_mqtt_client_t *client,
                                       const uint8_t *topic,
                                       uint16_t topic_length,
                                       uint32_t timeout,
                                       void *context)
{
  VERIFY_AND_RETURN_ERROR_IF_FLASE(client->state == SL_MQTT_CLIENT_CONNECTED, SL_STATUS_INVALID_STATE);

  SL_VERIFY_POINTER_OR_RETURN(client, SL_STATUS_WIFI_NULL_PTR_ARG);
  SL_VERIFY_POINTER_OR_RETURN(topic, SL_STATUS_WIFI_NULL_PTR_ARG);

  if (topic_length > SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status;
  sl_si91x_mqtt_client_context_t *sdk_contex                        = NULL;
  si91x_mqtt_client_unsubscribe_request_t si91x_unsubscribe_request = { 0 };
  sl_mqtt_client_topic_subscription_info_t *subscription;

  get_subscription(client, topic, topic_length, &subscription);

  status = sli_si91x_build_mqtt_sdk_context_if_async(SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT,
                                                     client,
                                                     context,
                                                     subscription,
                                                     timeout,
                                                     &sdk_contex);

  VERIFY_STATUS_AND_RETURN(status);
  si91x_unsubscribe_request.command_type = SI91X_MQTT_CLIENT_UNSUBSCRIBE_COMMMAND;
  si91x_unsubscribe_request.topic_len    = topic_length;
  memcpy(si91x_unsubscribe_request.topic, topic, topic_length);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EMB_MQTT_CLIENT,
                                        SI91X_NETWORK_CMD_QUEUE,
                                        &si91x_unsubscribe_request,
                                        sizeof(si91x_unsubscribe_request),
                                        timeout <= 0 ? SL_SI91X_RETURN_IMMEDIATELY : SL_SI91X_WAIT_FOR(timeout),
                                        sdk_contex,
                                        NULL);

  if (status == SL_STATUS_IN_PROGRESS) {
    return status;
  } else if (status != SL_STATUS_OK) {

    SL_CLEANUP_MALLOC(sdk_contex);
    return status;
  }

  if (subscription != NULL) {
    sl_slist_remove((sl_slist_node_t **)&client->subscription_list_head, (sl_slist_node_t *)subscription);
    free(subscription);
  }

  return status;
}

sl_status_t si91x_mqtt_event_handler(sl_status_t status,
                                     sl_si91x_mqtt_client_context_t *sdk_context,
                                     sl_si91x_packet_t *rx_packet)
{
  sl_mqtt_client_error_status_t error_status = sli_si91x_get_event_error_status(sdk_context->event);

  switch (sdk_context->event) {
    case SL_MQTT_CLIENT_CONNECTED_EVENT: {
      sdk_context->client->state = (status == SL_STATUS_OK) ? SL_MQTT_CLIENT_CONNECTED : SL_MQTT_CLIENT_DISCONNECTED;
      break;
    }

    case SL_MQTT_CLIENT_SUBSCRIBED_EVENT: {
      if (status != SL_STATUS_OK) {
        // Free subscription passed in subscribe() call if subscription call failed.
        free(sdk_context->sdk_data);
        break;
      }

      // As subscription is success, add the subscription to list.
      sl_slist_push((sl_slist_node_t **)&sdk_context->client->subscription_list_head,
                    (sl_slist_node_t *)sdk_context->sdk_data);
      break;
    }

    case SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT: {
      if (status != SL_STATUS_OK) {
        break;
      }

      // Free subscription if the unsubscription API call is successful.
      sl_slist_remove((sl_slist_node_t **)&sdk_context->client->subscription_list_head,
                      (sl_slist_node_t *)sdk_context->sdk_data);
      free(sdk_context->sdk_data);
      break;
    }

    case SL_MQTT_CLIENT_MESSAGED_RECEIVED_EVENT: {
      // Extract the MQTT message from payload and create sl_mqtt_message
      sl_mqtt_client_message_t received_message;
      sl_mqtt_client_topic_subscription_info_t *subscription;

      si91x_mqtt_client_received_message *si91x_message = (si91x_mqtt_client_received_message *)rx_packet->data;

      received_message.topic        = si91x_message->data;
      received_message.topic_length = si91x_message->topic_length;

      received_message.content_length = si91x_message->current_chunk_length;
      received_message.content        = (uint8_t *)&si91x_message->data[si91x_message->topic_length];

      get_subscription(sdk_context->client, received_message.topic, received_message.topic_length, &subscription);

      if (subscription == NULL) {
        SL_DEBUG_LOG("Unable to find subscription: Dropping MQTT message handling");
      } else {
        subscription->topic_message_handler(sdk_context->client, &received_message, sdk_context->user_context);
      }

      free(sdk_context);
      return SL_STATUS_OK;
    }

    case SL_MQTT_CLIENT_DISCONNECTED_EVENT: {
      sdk_context->client->state = status == SL_STATUS_OK ? SL_MQTT_CLIENT_DISCONNECTED : SL_MQTT_CLIENT_CONNECTED;

      // Free all subscriptions as we have disconnected from mqtt broker
      if (status == SL_STATUS_OK) {
        remove_and_free_all_subscriptions(sdk_context->client);
      }

      break;
    }

    default:
      break;
  }

  sdk_context->client->client_event_handler(sdk_context->client,
                                            status != SL_STATUS_OK ? SL_MQTT_CLIENT_ERROR_EVENT : sdk_context->event,
                                            status != SL_STATUS_OK ? &error_status : NULL,
                                            sdk_context->user_context);

  // Free the sdk_context after event handler is triggered.
  free(sdk_context);
  return SL_STATUS_OK;
}
