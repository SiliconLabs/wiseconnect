#pragma once

#include "sl_mqtt_client.h"
#include "sl_mqtt_client_types.h"
#include "sl_si91x_types.h"
#include "sl_status.h"

/**
 * @addtogroup SERVICE_MQTT_TYPES 
 * @{
 */
typedef struct {
  sl_mqtt_client_event_t event;
  sl_mqtt_client_t *client;
  void *user_context;
  void *sdk_data;
} sl_si91x_mqtt_client_context_t;

sl_status_t si91x_mqtt_event_handler(sl_status_t status,
                                     sl_si91x_mqtt_client_context_t *sdk_context,
                                     sl_si91x_packet_t *rx_packet);

/** @} */