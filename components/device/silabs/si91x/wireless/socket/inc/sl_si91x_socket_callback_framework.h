/*******************************************************************************
* @file  sl_si91x_socket_callback_framework.h
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

#include "sl_status.h"
#include "sl_si91x_types.h"

typedef struct {
  void *user_context;
  void *socket_context;
  int32_t socket_id; //socket_id to update in command trace of bus thread.
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
