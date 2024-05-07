/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "sl_status.h"
#include "sl_slist.h"
#include <stdbool.h>
#include <stdint.h>

/** \addtogroup SL_WIFI_TYPES
  * @{ */
/// Types of Wi-Fi buffers
typedef enum {
  SL_WIFI_TX_FRAME_BUFFER,    ///< Buffer for sending socket data to TA
  SL_WIFI_RX_FRAME_BUFFER,    ///< Buffer for storing response from TA
  SL_WIFI_CONTROL_BUFFER,     ///< Buffer for sending command to TA
  SL_WIFI_SCAN_RESULT_BUFFER, ///< Buffer for storing scan results
} sl_wifi_buffer_type_t;

/// Wi-Fi buffer
typedef struct {
  sl_slist_node_t node; ///< Points to the node of the list of which the buffer is part of
  uint32_t length;      ///< Indicates the buffer size
  uint8_t
    type; ///< Indicates the buffer type (SL_WIFI_TX_FRAME_BUFFER,SL_WIFI_RX_FRAME_BUFFER,etc..) corresponding to the buffer.
  uint8_t data[]; ///< Stores the data (header + payload) to be send to TA
} sl_wifi_buffer_t;

/// Wi-Fi buffer configuration
typedef struct {
  uint8_t tx_buffer_quota;      ///< Indicates the limit on buffers used for sending data to TA
  uint8_t rx_buffer_quota;      ///< Indicates the limit on buffers used for storing response from TA
  uint8_t control_buffer_quota; ///< Indicates the limit on buffers used for sending command to TA
  uint32_t block_size;          ///< Indicates the block size
  void *buffer_memory;          ///< This field points to the chunk of memory allocated on first invocation of malloc
} sl_wifi_buffer_configuration_t;

/** @} */
