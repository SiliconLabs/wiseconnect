/***************************************************************************/ /**
 * @file  si91x_mqtt_client_callback_framework.h
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
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
                                         sl_wifi_system_packet_t *rx_packet);

/** @} */