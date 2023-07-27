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

#include "sl_status.h"
#include "sl_net_types.h"

/** \addtogroup SL_PING_FUNCTIONS Functions
 * \ingroup SL_PING_APIS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Send an ICMP ping request.
 * @pre
 *   @ref sl_net_init should be called before this API.
 * @param[in] ip_address
 *   Destination IP address (IPv4 or IPv6) and SL IP type identified by @ref sl_ip_address_t 
 * @param[in] ping_packet_size
 *   Size of a ping packet
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   ping_packet_size valid range: [0, 300]
 ******************************************************************************/
sl_status_t sl_si91x_send_ping(sl_ip_address_t ip_address, uint16_t ping_packet_size);

/** @} */
