/***************************************************************************/ /**
 * @file
 * @brief Network types
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_ip_types.h"
#include "sl_net_constants.h"
#include "sl_net_ip_types.h"
#include "sl_constants.h"
#include "sl_status.h"
#include <stdint.h>

/** \addtogroup SL_NET_TYPES Types
 * @{ */

/// SL-Net event handler
typedef sl_status_t (*sl_net_event_handler_t)(uint32_t event, sl_status_t status, void *data, uint32_t data_length);

/// SL Net abstract profile
typedef void sl_net_profile_t;

/// Ping Response structure
typedef struct {
  uint16_t ip_version; ///< IP version
  uint16_t ping_size;  ///< ping size
  union {
    uint8_t ipv4_address[4];  ///< IPv4 address
    uint8_t ipv6_address[16]; ///< IPv6 address
  } ping_address;             ///< Pinged IP Address
} sl_si91x_ping_response_t;

/** @} */
