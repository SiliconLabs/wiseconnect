/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "cmsis_os2.h"
#include "sl_net_types.h"
#include "sl_si91x_constants.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_types.h"
#include "sl_wifi_types.h"
#include <stdint.h>

#pragma pack(1)
typedef struct {
  sl_wifi_buffer_t *buffer_header;
  sl_si91x_packet_t packet_header;
  sl_si91x_socket_send_request_t request_header;
  uint8_t data[];
} si91x_data_buffer_t;
#pragma pack()

typedef struct {
  //  si91x_data_stream_t receive_data_stream;
  osMutexId_t mutex;
  sl_wifi_buffer_t *receive_buffer_queue;
  si91x_data_buffer_t *read_buffer;
  uint8_t *read_data;
  si91x_data_buffer_t *write_buffer;
  uint8_t *write_data;
  sl_net_interface_t interface;
  sl_ip_address_t remote_ip;
  //  sl_socket_listen_handler_t listen_handler;
  void *arg;
  uint16_t remote_port;
  uint16_t local_port;
  uint16_t id;
} sl_si91x_socket_t;
