/*******************************************************************************
* @file  si91x_mqtt_client_types.h
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
#include "stdint.h"
#include "sl_common.h"

//Note: Please go through http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html

#define SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH      202 // This size is including the NULL terminating character.
#define SI91X_MQTT_CLIENT_WILL_TOPIC_MAXIMUM_LENGTH 202 // This size is including the NULL terminating character.
#define SI91X_MQTT_CLIENT_MESSAGE_MAXIMUM_LENGTH    100

#define SI91X_MQTT_CLIENT_ID_MAXIMUM_LENGTH       62  // This size is including the NULL terminating character.
#define SI91X_MQTT_CLIENT_USERNAME_MAXIMUM_LENGTH 122 // This size is including the NULL terminating character.
#define SI91X_MQTT_CLIENT_PASSWORD_MAXIMUM_LENGTH 62  // This size is including the NULL terminating character.

#define SI91X_MQTT_CLIENT_INIT_COMMAND        1
#define SI91X_MQTT_CLIENT_CONNECT_COMMAND     2
#define SI91X_MQTT_CLIENT_SUBSCRIBE_COMMAND   3
#define SI91X_MQTT_CLIENT_PUBLISH_COMMAND     4
#define SI91X_MQTT_CLIENT_UNSUBSCRIBE_COMMAND 5
#define SI91X_MQTT_CLIENT_DISCONNECT_COMMAND  8
#define SI91X_MQTT_CLIENT_DEINIT_COMMAND      9

#define SL_SI91X_MQTT_CLIENT_TOPIC_DELIMITER        "/"
#define SL_SI91X_MQTT_CLIENT_SINGLE_LEVEL_WILD_CARD "+"
#define SL_SI91X_MQTT_CLIENT_MULTI_LEVEL_WILD_CARD  "#"

typedef struct {
  // IP version
  uint32_t ip_version;
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } server_ip_address;
} si91x_mqtt_client_ip_address_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint32_t command_type;
  // MQTT server IP address
  si91x_mqtt_client_ip_address_t server_ip;
  // MQTT server port
  uint32_t server_port;
  // Client ID Length
  uint8_t client_id_len;
  // client ID, should be unique
  int8_t client_id[SI91X_MQTT_CLIENT_ID_MAXIMUM_LENGTH];
  // keep alive interval (s)
  uint16_t keep_alive_interval;
  // username Length
  uint8_t username_len;
  // user name
  uint8_t user_name[SI91X_MQTT_CLIENT_USERNAME_MAXIMUM_LENGTH];
  // password Length
  uint8_t password_len;
  // password
  uint8_t password[SI91X_MQTT_CLIENT_PASSWORD_MAXIMUM_LENGTH];
  // clean session(0-1)
  uint8_t clean;
  // 0 : TCP , 1 : SSl
  uint8_t encrypt;
  // MQTT  Client port
  uint32_t client_port;
#ifdef SLI_SI917
  //! Capping tcp retransmission timeout
  uint8_t tcp_max_retransmission_cap_for_emb_mqtt;
#endif
  //! MQTT ping retries.
  uint16_t keep_alive_retries;
} si91x_mqtt_client_init_request_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint32_t command_type;
  // whether to use username (0-1)
  uint8_t is_username_present;
  //  whether to use pwdFlag (0-1)
  uint8_t is_password_present;
  //  whether to set willmsg (0-1)
  uint8_t will_flag;
  //  retained flag(0-1)
  uint8_t will_retain;
  //  message Qos(0-2)
  uint8_t will_qos;
  // length of topic
  uint8_t will_topic_len;
  //  topic name of will
  uint8_t will_topic[SI91X_MQTT_CLIENT_WILL_TOPIC_MAXIMUM_LENGTH];
  // Length of Will message
  uint8_t will_message_len;
  //  message of will
  uint8_t will_msg[SI91X_MQTT_CLIENT_MESSAGE_MAXIMUM_LENGTH];

} si91x_mqtt_client_connect_request_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint32_t command_type;
  // length of TOPIC
  uint8_t topic_len;
  // topic of subscribe message
  int8_t topic[SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH];
  // message Qos, can be 0, 1, or 2
  int8_t qos;

} si91x_mqtt_client_subscribe_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint32_t command_type;
  // length of TOPIC
  uint8_t topic_len;
  // topic of unsubscribe message
  uint8_t topic[SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH];
  // message Qos, can be 0, 1, or 2
  uint8_t qos;
  // retained flag, can be 0 or 1
  uint8_t retained;
  // duplicate flag, can be 0 or 1
  uint8_t dup;
  // length of publish message(option), if set to 0 or
  //omitted, <message> will be parsed in text format, else
  //hexidecimai format
  uint16_t msg_len;
  // publish message
  int8_t *msg;

} si91x_mqtt_client_publish_request_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint32_t command_type;
  // length of TOPIC
  uint8_t topic_len;
  // topic of unsubscribe message
  uint8_t topic[SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH];

} si91x_mqtt_client_unsubscribe_request_t;

typedef struct {
  uint32_t command_type;

} si91x_mqtt_client_command_request_t;

typedef struct SL_ATTRIBUTE_PACKED {
  uint16_t mqtt_flags;
  uint16_t current_chunk_length;
  uint16_t topic_length;
  uint8_t data[];
} si91x_mqtt_client_received_message;
