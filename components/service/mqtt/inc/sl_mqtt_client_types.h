/*******************************************************************************
* @file  sl_mqtt_client_types.h
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

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sl_slist.h"
#include "sl_net_constants.h"
#include "sl_ip_types.h"

typedef enum MQTTStatus {
  MQTTSuccess = 0,     /**< Function completed successfully. */
  MQTTBadParameter,    /**< At least one parameter was invalid. */
  MQTTNoMemory,        /**< A provided buffer was too small. */
  MQTTSendFailed,      /**< The transport send function failed. */
  MQTTRecvFailed,      /**< The transport receive function failed. */
  MQTTBadResponse,     /**< An invalid packet was received from the server. */
  MQTTServerRefused,   /**< The server refused a CONNECT or SUBSCRIBE. */
  MQTTNoDataAvailable, /**< No data available from the transport interface. */
  MQTTIllegalState,    /**< An illegal state in the state record. */
  MQTTStateCollision,  /**< A collision with an existing state record entry. */
  MQTTKeepAliveTimeout /**< Timeout while waiting for PINGRESP. */
} MQTTStatus_t;

/**
 * @addtogroup SERVICE_MQTT_CONSTANTS 
 * @{
 */

/**
 * @brief MQTT quality of service (QoS) levels
 * @note  Quality of service (QoS) level 2 not currently supported.
*/
typedef enum {
  SL_MQTT_QOS_LEVEL_0, ///< MQTT QoS level 0
  SL_MQTT_QOS_LEVEL_1, ///< MQTT QoS level 1
  SL_MQTT_QOS_LEVEL_2  ///< MQTT QoS level 2 (not currently supported)
} sl_mqtt_qos_t;

/// MQTT Client connection states
typedef enum {
  SL_MQTT_CLIENT_DISCONNECTED,      ///< Initial state.
  SL_MQTT_CLIENT_TA_INIT,           ///< Attains this state when TA MQTT is initialized successfully.
  SL_MQTT_CLIENT_CONNECTION_FAILED, ///< Attains this state when the connection to MQTT broker failed.
  SL_MQTT_CLIENT_CONNECTED,         ///< Connection established with MQTT broker.
  SL_MQTT_CLIENT_REMOTE_TERMINATE,  ///< Attains this state when the connection is terminated by the broker.
  SL_MQTT_CLIENT_TA_DISCONNECTED // Attains this state when TA is disconnected from broker but TA deinit is not yet called.
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
  SL_MQTT_CLIENT_ERROR_EVENT              ///< MQTT client error event.
} sl_mqtt_client_event_t;

/// MQTT Client error status
typedef enum {
  SL_MQTT_CLIENT_CONNECT_FAILED,      ///< MQTT client connect failed status.
  SL_MQTT_CLIENT_PUBLISH_FAILED,      ///< MQTT client publish failed status.
  SL_MQTT_CLIENT_SUBSCRIBE_FAILED,    ///< MQTT client subscribe failed status.
  SL_MQTT_CLIENT_UNSUBSCRIBED_FAILED, ///< MQTT client unsubscribe failed status.
  SL_MQTT_CLIENT_DISCONNECT_FAILED,   ///< MQTT client disconnect failed status.
  SL_MQTT_CLIENT_UNKNKOWN_ERROR       ///< MQTT client unknown error status.
} sl_mqtt_client_error_status_t;

/// MQTT Client disconnection reason
typedef enum {
  SL_MQTT_CLIENT_REMOTE_TERMINATE_DISCONNECTION, ///< Disconnection due to remote termination.
  SL_MQTT_CLIENT_WLAN_DISCONNECTION,             ///< Disconnection due to WLAN disconnection.
  SL_MQTT_CLIENT_USER_INITIATED_DISCONNECTION    ///< User initiated disconnection.
} sl_mqtt_client_disconnection_reason_t;

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
  uint16_t
    will_topic_length;   ///< Length of topic. Length should not exceed 62 bytes including NULL termination character.
  uint8_t *will_message; ///< Pointer to will message.
  uint32_t
    will_message_length; ///<  Length of the will message. Length should not exceed 62 bytes including NULL termination character.
} sl_mqtt_client_last_will_message_t;

/// MQTT Client Message structure
typedef struct {
  sl_mqtt_qos_t qos_level;    ///< Quality of subscription.
  uint16_t packet_identifier; ///< Packed id of the received message.
  bool is_retained;           ///< Retained flag of message as sent by the broker.
  bool is_duplicate_message;  ///< Whether this is a duplicate message.
  uint8_t *topic;             ///< Pointer to topic name.
  uint16_t
    topic_length;   ///< Length of the topic. Length should not exceed 202 bytes including NULL termination character.
  uint8_t *content; ///< Pointer to content.
  uint32_t content_length; ///< Length of the content.
} sl_mqtt_client_message_t;

/// MQTT Client broker information structure
typedef struct {
  sl_ip_address_t ip;           ///< IP address of broker.
  uint16_t port;                ///< Port number of broker.
  bool is_connection_encrypted; ///< Whether to use MQTT or MQTTS.
  uint16_t connect_timeout;     ///< MQTT Connection timeout.
  uint16_t keep_alive_interval; ///< Keep alive timeout of MQTT connection.
  uint16_t keep_alive_retries;  ///< MQTT ping retries.
} sl_mqtt_broker_t;

/// MQTT Client credentials structure
typedef struct {
  uint16_t
    username_length; ///< Length of the username. Length should not exceed 122 bytes including NULL termination character.
  uint16_t
    password_length; ///< Length of the password. Length should not exceed 62 bytes including NULL termination character.
  uint8_t data[];    ///< A flexible array to store both username and password.
} sl_mqtt_client_credentials_t;

/// MQTT Client Configuration structure
typedef struct {
  bool auto_reconnect;                  ///< Whether to automatically connect back to broker in case of disconnection.
  uint8_t retry_count;                  ///< Maximum retry count of auto reconnect.
  uint16_t minimum_back_off_time;       ///< Minimum back off time between two successive reconnect attempts.
  uint16_t maximum_back_off_time;       ///< Maximum back off time between two successive reconnect attempts.
  bool is_clean_session;                ///< Clean session flag to send to broker in connect request.
  sl_mqtt_version_t mqt_version;        ///< Version of client MQTT.
  uint16_t client_port;                 ///< Port number of client.
  sl_net_credential_id_t credential_id; ///< Credential id for username and password of MQTT connect request.
  uint8_t *client_id;                   ///< Pointer to MQTT client id.
  uint8_t
    client_id_length; ///< Length of client id. Length should not exceed 62 bytes including NULL termination character.
} sl_mqtt_client_configuration_t;

/**
 * @typedef sl_mqtt_client_event_handler_t
 * @brief
 *    Handler for MQTT client Events.
 * @param client		
 *    Client on which the event occurred. 
 * @param event			
 *    Event occurred of type @ref sl_mqtt_client_event_t
 * @param event_data	
 *    Event data for the data. This parameter would be non-null only for MQTT_CLIENT_MESSAGED_RECEIVED and MQTT_CLIENT_ERROR.
 * @param context		
 *    Context provided by user at the time of API call. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 */
typedef void (*sl_mqtt_client_event_handler_t)(void *client,
                                               sl_mqtt_client_event_t event,
                                               void *event_data,
                                               void *context);

/**
 * @typedef sl_mqtt_client_message_received_t
 * @brief
 *    Handler for MQTT client RX message.
 * @param client	
 *    Client which has received message.
 * @param message_to_be_published	
 *    Message received of type @ref sl_mqtt_client_message_t
 * @param context	
 *    Context provided by user at the time of Subscribe API call. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 * @note
 * 	  Due to the constraints from the firmware, is_retained, qos_level, packet_identifier, duplicate_message of sl_mqtt_client_message_t are not populated and shall be ignored while processing the message.
 */
typedef void (*sl_mqtt_client_message_received_t)(void *client,
                                                  sl_mqtt_client_message_t *message_to_be_published,
                                                  void *context);

/// MQTT Client Topic Subscription Info structure
typedef struct {
  sl_slist_node_t next_subscription;                       ///< Next node in the linked list.
  sl_mqtt_client_message_received_t topic_message_handler; ///< A function pointer to message handler.
  sl_mqtt_qos_t qos_of_subscription;                       ///< Quality of subscription.
  uint16_t
    topic_length; ///< Length of the subscribed topic. Length should not exceed 202 bytes including NULL termination character.
  uint8_t topic[]; ///< A flexible array to store the topic.
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
  sl_mqtt_client_event_handler_t
    client_event_handler; ///< Function pointer to event handler given at the time of @ref sl_mqtt_client_init.
} sl_mqtt_client_t;

/** @} */
