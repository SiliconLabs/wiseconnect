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

/** \defgroup WIFI_HOST Host Interface Types
  * \ingroup SL_WIFI_TYPES
  * @{ */

/// Types of Wi-Fi buffers
typedef enum {
  SL_WIFI_TX_FRAME_BUFFER,
  SL_WIFI_RX_FRAME_BUFFER,
  SL_WIFI_CONTROL_BUFFER,
  SL_WIFI_SCAN_RESULT_BUFFER,
} sl_wifi_buffer_type_t;

typedef struct {
  sl_slist_node_t node;
  uint32_t length;
  uint8_t data[];
} sl_wifi_buffer_t;

/** @} */
