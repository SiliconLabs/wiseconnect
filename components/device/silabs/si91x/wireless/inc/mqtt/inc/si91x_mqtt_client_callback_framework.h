/*******************************************************************************
* @file  si91x_mqtt_client_callback_framework.h
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

sl_status_t sli_si91x_mqtt_event_handler(sl_status_t status,
                                         sl_si91x_mqtt_client_context_t *sdk_context,
                                         sl_si91x_packet_t *rx_packet);

/** @} */