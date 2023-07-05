
#pragma once

#include "si91x_mqtt_client_callback_framework.h"

/**
 * An internal helper function to build sl_mqtt_client_event_t in case an API is called in asynchronous mode.
 * @param event 						Event that needs to be stored in context.
 * @param client 						Client address that needs to be stored in context.
 * @param user_context					user_context provided by user while calling API.
 * @param timeout						timeout which determines if API needs to be executed in sync or async mode.
 * @param si91x_mqtt_client_context_t** A double pointer to client context.This function allocate memory and initializes members if timeout is greater than 0 else pointer is initialized with null
 * @return								return SL_STATUS_OK in case of success or appropriate status.
 */
sl_status_t sli_si91x_build_mqtt_sdk_context_if_async(sl_mqtt_client_event_t event,
                                                      sl_mqtt_client_t *client,
                                                      void *user_context,
                                                      void *sdk_data,
                                                      uint32_t timeout,
                                                      sl_si91x_mqtt_client_context_t **context);

/**
 * An internal helper function to get current mqtt client.
 * @param client current mqtt client
 *
 * Note: client would be NULL if this function is called before sl_mqtt_client_init or after sl_mqtt_client_deint
 */
void sli_si91x_get_mqtt_client(sl_mqtt_client_t **client);
