/**
 * @file sl_wifi_host_interface.h
 * @brief This file defines the host interface for Wi-Fi operations.
 *
 * This file contains the definitions and structures used for managing Wi-Fi buffers and their configurations.
 * It includes enumerations for buffer types and structures for buffer management.
 *
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 *
 * (c) Copyright 2024, Silicon Laboratories Inc. All rights reserved.
 */

#pragma once

#include "sl_status.h"
#include "sl_slist.h"
#include <stdbool.h>
#include <stdint.h>

/** \addtogroup SL_WIFI_TYPES
  * @{ */

/**
 * @enum sl_wifi_buffer_type_t
 * @brief Enumeration for types of Wi-Fi buffers.
 */
typedef enum {
  SL_WIFI_TX_FRAME_BUFFER,    ///< Buffer for sending socket data to NWP
  SL_WIFI_RX_FRAME_BUFFER,    ///< Buffer for storing response from NWP
  SL_WIFI_CONTROL_BUFFER,     ///< Buffer for sending command to NWP
  SL_WIFI_SCAN_RESULT_BUFFER, ///< Buffer for storing scan results
} sl_wifi_buffer_type_t;

/**
 * @struct sl_wifi_buffer_t
 * @brief Structure representing a Wi-Fi buffer.
 */
typedef struct {
  sl_slist_node_t node; ///< Pointer to the node of the list of which the buffer is part of
  uint32_t length;      ///< Size of the buffer in bytes
  uint8_t
    type; ///< Indicates the buffer type (SL_WIFI_TX_FRAME_BUFFER, SL_WIFI_RX_FRAME_BUFFER, and so on.) corresponding to the buffer.
  uint8_t data[]; ///< Stores the data (header + payload) to be send to NWP
} sl_wifi_buffer_t;

/**
 * @struct sl_wifi_buffer_configuration_t
 * @brief Structure representing the Wi-Fi buffer configuration.
 */
typedef struct {
  uint8_t tx_buffer_quota;      ///< Indicates the limit on buffers used for sending the data to NWP
  uint8_t rx_buffer_quota;      ///< Indicates the limit on buffers used for storing the response from NWP
  uint8_t control_buffer_quota; ///< Indicates the limit on buffers used for sending the command to NWP
  uint32_t block_size;          ///< Indicates the block size in bytes
  void *buffer_memory;          ///< Pointer to the chunk of memory allocated on the first invocation of malloc
} sl_wifi_buffer_configuration_t;

/** @} */
