/*******************************************************************************
* @file  sl_mqtt_client.h
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
#include "sl_mqtt_client_types.h"
#include "sl_status.h"
#include <stdint.h>

/**
 * @addtogroup SERVICE_MQTT_FUNCTIONS 
 * @{
 */

/***************************************************************************/ /**
 * @brief               
 *   Initialize the mqtt_client and registers the event_handler.
 * @param[in] client    
 *   Valid pointer to the client structure of type @ref sl_mqtt_client_t. This shall not be null.
 * @param[in] event_handler 
 *   Event handler of type @ref sl_mqtt_client_event_handler_t which will be called for various events on client.
 * @return				
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_mqtt_client_init(sl_mqtt_client_t *client, sl_mqtt_client_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief 
 *   Deinitialize the MQTT client.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_init should be called before this API.
 * @param[in] client 
 *   Valid pointer to the client structure of type @ref sl_mqtt_client_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_mqtt_client_deinit(sl_mqtt_client_t *client);

/***************************************************************************/ /**
 * @brief                    
 *   Connect the client to MQTT broker.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_init should be called before this API.
 * @param[in] client
 *   @ref sl_mqtt_client_t client which would be connected to the broker.
 * @param[in] broker
 *   Broker configuration of type @ref sl_mqtt_broker_t
 * @param[in] last_will_message
 *   Last will message of the client of type @ref sl_mqtt_client_last_will_message_t
 * @param[in] configuration
 *   Client configuration of type @ref sl_mqtt_client_configuration_t
 * @param[in] timeout
 *   Timeout for the API. If the value is zero, the API will be asynchronous.
 * @return
 *   sl_status_t. If called asynchronously, SL_STATUS_IN_PROGRESS will be returned as status.
 * @note
 *   In subsequent calls to connect, it is optional to provide broker, last_will_message, and configuration parameters.
 * @note
 *   If broker and configuration parameters are given null, then value given in the first connect() call would be retained.
 * @note
 *   In case of last_will_message parameter, values given in each connect() call would be considered. If value of last_will_message parameter is given as null, then no will message would be sent to the broker parameter.
 * @note
 *   Only is_clean_session, credential_id, client_id, client_id_length of sl_mqtt_client_configuration_t are considered.
 *   Topic length of last_will_message should be less than SI91X_MQTT_CLIENT_WILL_TOPIC_MAXIMUM_LENGTH.
 *   Client ID length should be less than SI91X_MQTT_CLIENT_CLIENT_ID_MAXIMUM_LENGTH.
 *   Username length should be less than SI91X_MQTT_CLIENT_USERNAME_MAXIMUM_LENGTH.
 *   Password length should be less than SI91X_MQTT_CLIENT_PASSWORD_MAXIMUM_LENGTH.
 ******************************************************************************/
sl_status_t sl_mqtt_client_connect(sl_mqtt_client_t *client,
                                   const sl_mqtt_broker_t *broker,
                                   const sl_mqtt_client_last_will_message_t *last_will_message,
                                   const sl_mqtt_client_configuration_t *configuration,
                                   uint32_t timeout);

/***************************************************************************/ /**
 * @brief                   
 *   Disconnect the client from MQTT broker.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_connect should be called before this API.
 * @param[in] client			
 *   @ref sl_mqtt_client_t client which needs to be disconnected from the broker.
 * @param[in] connect_timeout 	
 *   Timeout for the API. If the value is zero, the API will be asynchronous.
 * @return					
 *   sl_status_t. If called asynchronously, SL_STATUS_IN_PROGRESS will be returned as status.
 ******************************************************************************/
sl_status_t sl_mqtt_client_disconnect(sl_mqtt_client_t *client, uint32_t timeout);

/***************************************************************************/ /**
 * @brief 
 *   Publish a message.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_connect should be called before this API.
 * @param[in] client	
 *   @ref sl_mqtt_client_t client which requires to publish the message.
 * @param[in] message	
 *   @ref sl_mqtt_client_message_t Message which needs to be published.
 * @param[in] timeout	
 *   Timeout for the API. If the value is zero, the API will be asynchronous.
 * @param[in] context   
 *   Context which would be returned in event handler if the API is called asynchronously. The caller must ensure that the lifecycle of context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 * @return			
 *   sl_status_t. If called asynchronously, SL_STATUS_IN_PROGRESS will be returned as status.
 * @note
 *  The maximum length of the topic should be less than SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH.
 ******************************************************************************/
sl_status_t sl_mqtt_client_publish(sl_mqtt_client_t *client,
                                   const sl_mqtt_client_message_t *message,
                                   uint32_t timeout,
                                   void *context);

/***************************************************************************/ /**
 * @brief
 *   Indicate which client can subscribe to a topic.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_connect should be called before this API.
 * @param[in] client			
 *   @ref sl_mqtt_client_t client which needs to subscribe to the topic.
 * @param[in] topic
 *   Topic of interest.
 * @param[in] topic_length		
 *   Length of the topic.
 * @param[in] qos_level			
 *   Quality of service of type @ref sl_mqtt_qos_t using which client subscribes.
 * @param[in] timeout			
 *   Timeout for API. If the value is zero, the API will be asynchronous.
 * @param[in] message_handler 	
 *   @ref sl_mqtt_client_message_received_t message handler which invokes for messages published on the topic.
 * @param[in] context
 *   Context that is sent back to the message handler. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 * @return					
 *   sl_status_t. If called asynchronously, SL_STATUS_IN_PROGRESS will be returned as status.
 * @note
 *   The maximum length of the topic should be less than SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH.
 ******************************************************************************/
sl_status_t sl_mqtt_client_subscribe(sl_mqtt_client_t *client,
                                     const uint8_t *topic,
                                     uint16_t topic_length,
                                     sl_mqtt_qos_t qos_level,
                                     uint32_t timeout,
                                     sl_mqtt_client_message_received_t message_handler,
                                     void *context);

/***************************************************************************/ /**
 * @brief    
 *   Indicate which client can unsubscribe a topic.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_mqtt_client_subscribe should be called before this API.
 * @param[in] client			
 *   @ref sl_mqtt_client_t client that unsubscribes the topic.
 * @param[in] topic				
 *   Topic from which client needs to unsubscribe.
 * @param[in] length			
 *   Length of the topic.
 * @param[in] connect_timeout	
 *   Timeout for the API. If the value is zero, the API will be asynchronous.
 * @param[in] context			
 *   Context that returns in event handler if the API is called asynchronously. The caller must ensure that the lifecycle of the context is retained until the callback is invoked. The deallocation of context is also the responsibility of the caller.
 * @return					
 *   sl_status_t. If called asynchronously, SL_STATUS_IN_PROGRESS will be returned as status.
 * @note
 *   The maximum length of the topic should be less than SI91X_MQTT_CLIENT_TOPIC_MAXIMUM_LENGTH
 ******************************************************************************/
sl_status_t sl_mqtt_client_unsubscribe(sl_mqtt_client_t *client,
                                       const uint8_t *topic,
                                       uint16_t length,
                                       uint32_t timeout,
                                       void *context);

/** @} */
