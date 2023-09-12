#pragma once

#include "sl_status.h"
#include "sl_si91x_types.h"

typedef struct {
  void *user_context;
  void *socket_context;
} sl_si91x_socket_context_t;

/**
 * This is internal event handler to handle all events of sockets[Including BSD]
 * @param status 	  sl_status of rx_packet frame status
 * @param sdk_context sl_si91x_socket_context_t that was sent at the time sending the request.
 * @param rx_packet	  Firmware response.
 * @return
 */
sl_status_t si91x_socket_event_handler(sl_status_t status,
                                       sl_si91x_socket_context_t *sdk_context,
                                       sl_si91x_packet_t *rx_packet);
