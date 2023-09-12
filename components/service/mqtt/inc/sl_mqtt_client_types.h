#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sl_slist.h"
#include "sl_net_constants.h"
#include "sl_ip_types.h"

/**
 * @addtogroup SERVICE_MQTT_CONSTANTS 
 * @{
 */

/// MQTT QOS levels
typedef enum {
  SL_MQTT_QOS_LEVEL_0, ///< MQTT QOS level 0
  SL_MQTT_QOS_LEVEL_1, ///< MQTT QOS level 1
  SL_MQTT_QOS_LEVEL_2, ///< MQTT QOS level 2
} sl_mqtt_qos_t;

/// MQTT Client connection states
typedef enum {
  SL_MQTT_CLIENT_DISCONNECTED, ///< Initial state.
  SL_MQTT_CLIENT_CONNECTING,   ///< Attains this state when the connect is called and await for results.
  SL_MQTT_CLIENT_CONNECTED,    ///< Connection established with MQTT broker
  SL_MQTT_CLIENT_DISCONNECTING ///< Moves to this state when the disconnect is called and await for results.
} sl_mqtt_client_connection_state_t;

/// MQTT Protocol version
typedef enum {
  SL_MQTT_VERSION_3,  ///< MQTT Version 3.0
  SL_MQTT_VERSION_3_1 ///< MQTT Version 3.1
} sl_mqtt_version_t;

/// MQTT Client Events
typedef enum {
  SL_MQTT_CLIENT_CONNECTED_EVENT,         ///< MQTT client connected event.
  SL_MQTT_CLIENT_DISCONNECTED_EVENT,      ///< MQTT client disconnected event.
  SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT, ///< MQTT client message published event.
  SL_MQTT_CLIENT_MESSAGED_RECEIVED_EVENT, ///< MQTT client message received event.
  SL_MQTT_CLIENT_SUBSCRIBED_EVENT,        ///< MQTT client subscribed event.
  SL_MQTT_CLIENT_UNSUBSCRIBED_EVENT,      ///< MQTT client unsubscribed event.
  SL_MQTT_CLIENT_ERROR_EVENT,             ///< MQTT client error event.
} sl_mqtt_client_event_t;

/// MQTT Client error status
typedef enum {
  SL_MQTT_CLIENT_CONNECT_FAILED,      ///< MQTT client connect failed status.
  SL_MQTT_CLIENT_PUBLISH_FAILED,      ///< MQTT client publish failed status.
  SL_MQTT_CLIENT_SUBSCRIBE_FAILED,    ///< MQTT client subscribe failed status.
  SL_MQTT_CLIENT_UNSUBSCRIBED_FAILED, ///< MQTT client unsubscribe failed status.
  SL_MQTT_CLIENT_DISCONNECT_FAILED,   ///< MQTT client disconnect failed status.
  SL_MQTT_CLIENT_UNKNKOWN_ERROR,      ///< MQTT client unknown error status.
} sl_mqtt_client_error_status_t;

/** @} */

/**
 * @addtogroup SERVICE_MQTT_TYPES 
 * @{
 */

/// MQTT Client last will message
typedef struct {
  bool is_retained;             ///< Flag whether to retain the will message or not.
  sl_mqtt_qos_t will_qos_level; ///< Quality of subscription.
  uint8_t *will_topic;          ///< Name of will topic.
  uint16_t will_topic_length;   ///< Length of topic.
  uint8_t *will_message;        ///< Pointer to will message.
  uint32_t will_message_length; ///<  Length of the will message.
} sl_mqtt_client_last_will_message_t;

/// MQTT Client Message structure
typedef struct {
  sl_mqtt_qos_t qos_level;   ///< Quality of subscription.
  uint16_t pakcet_identifer; ///< Packed id of the received message.
  bool is_retained;          ///< Retained flag of message as sent by the broker.
  bool is_duplicate_message; ///< Whether this is a duplicate message.
  uint8_t *topic;            ///< Pointer to topic name.
  uint16_t topic_length;     ///< Length of the topic.
  uint8_t *content;          ///< Pointer to content.
  uint32_t content_length;   ///< Length of the content.
} sl_mqtt_client_message_t;

/// MQTT Client broker information structure
typedef struct {
  sl_ip_address_t ip;           ///< IP address of broker.
  uint16_t port;                ///< Port number of broker.
  bool is_connection_encrypted; ///< Whether to use MQTT or MQTTS.
  uint16_t connect_timeout;     ///< MQTT Connection timeout.
  uint16_t keep_alive_interval; ///< Keep alive timeout of MQTT connection.
} sl_mqtt_broker_t;

/// MQTT Client credentials structure
typedef struct {
  uint16_t username_length; ///< Length of the username.
  uint16_t password_length; ///< Length of the password.
  uint8_t data[];           ///< A flexible array to store both username and password.
} sl_mqtt_client_credentials_t;

/// MQTT Client Configuration structure
typedef struct {
  bool auto_reconnect;                  ///< Whether to automatically connect back to broker in case of disconnection.
  uint8_t retry_count;                  ///< Maximum retry count of auto reconnect.
  uint16_t minimum_back_off_time;       ///< Minimum back off time between two successive reconnect attempts.
  uint16_t maximun_back_off_time;       ///< Maximum back off time between two successive reconnect attempts.
  bool is_clean_session;                ///< Clean session flag to send to broker in connect request.
  sl_mqtt_version_t mqt_version;        ///< Version of client MQTT.
  uint16_t client_port;                 ///< Port number of client.
  sl_net_credential_id_t credential_id; ///< Credential id for username and password of MQTT connect request.
  uint8_t *client_id;                   ///< Pointer to MQTT client id.
  uint8_t client_id_length;             ///< Length of client id.
} sl_mqtt_client_configuration_t;

/**
 * @brief
 *    Handler for MQTT client Events.
 * @param[out] client		
 *    Client on which the event occurred.
 * @param[out] event			
 *    Event occurred.
 * @param[out] event_data	
 *    Event data for the data. This parameter would be non-null only for MQTT_CLIENT_MESSAGED_RECEIVED and MQTT_CLIENT_ERROR.
 * @param[out] context		
 *    Context provided by user at the time of API call. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 */
typedef void (*sl_mqtt_client_event_handler)(void *client,
                                             sl_mqtt_client_event_t event,
                                             void *event_data,
                                             void *context);

/**
 * @brief
 *    Handler for MQTT client RX message.
 * @param[out] client	
 *    Client which has received message.
 * @param[out] message	
 *    Message received.
 * @param[out] context	
 *    Context provided by user at the time of Subscribe API call. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 *
 * @note
 * 	  Due to the constraints from the firmware, is_retained, qos_level, packet_identifer, duplicate_message of sl_mqtt_client_message_t are not populated and shall be ignored while processing the message.
 */
typedef void (*sl_mqtt_client_message_received)(void *client,
                                                sl_mqtt_client_message_t *message_to_be_published,
                                                void *context);

/// MQTT Client Topic Subscription Info structure
typedef struct {
  sl_slist_node_t next_subscription;                     ///< Next node in the linked list.
  sl_mqtt_client_message_received topic_message_handler; ///< A function pointer to message handler.
  sl_mqtt_qos_t qos_of_subscription;                     ///< Quality of subscription.
  uint16_t topic_length;                                 ///< Length of the subscribed topic.
  uint8_t topic[];                                       ///< A flexible array to store the topic.
} sl_mqtt_client_topic_subscription_info_t;

/// MQTT Client Handle structure
typedef struct {
  sl_mqtt_client_connection_state_t state; ///< Current state of MQTT client.
  const sl_mqtt_broker_t *broker;          ///< Pointer to broker configuration, given at the time of connect() API.
  const sl_mqtt_client_last_will_message_t
    *last_will_message; ///< Pointer to last will message configuration, given at the time of connect() API.
  const sl_mqtt_client_configuration_t
    *client_configuration; ///< Pointer to client configuration, given at the time of connect() API.
  sl_mqtt_client_topic_subscription_info_t
    *subscription_list_head; ///< Pointer to the head of the subscription linked list.
  sl_mqtt_client_event_handler
    client_event_handler; ///< Function pointer to event handler given at the time of @ref sl_mqtt_client_init.
} sl_mqtt_client_t;

/** @} */
