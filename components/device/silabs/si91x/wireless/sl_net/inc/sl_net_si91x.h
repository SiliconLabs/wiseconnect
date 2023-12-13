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

#include "sl_net_ip_types.h"
#include "sl_net_constants.h"

/** \addtogroup SI91X_NETWORK_FUNCTIONS 
 * \ingroup SL_SI91X_API
 * @{ */

// Enumerate multicast address command types
typedef enum {
  SL_WIFI_MULTICAST_LEAVE = 0,
  SL_WIFI_MULTICAST_JOIN  = 1,
} sl_si91x_multicast_address_command_type_t;

// Enumerate IP configuration modes for SI91X module
typedef enum {
  SL_SI91X_STATIC = 0,
  SL_SI91X_DHCP,
  SL_SI91X_DHCP_RESERVED,
  SL_SI91X_DHCP_HOSTNAME,
  SL_SI91X_DHCP_OPTION81,
  SL_SI91X_DHCP_OPTION77
} sl_si91x_ip_config_mode_t;

/*==============================================*/
/**
 * @brief      Configure IP address.
 * @param[in]  address - Pointer to store assigned Ip address details.
 * @param[in]  virtual_ap_id  - Virtual AP ID. One of values from @ref sl_si91x_wifi_vap_id_t
 * @return       sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *
 */
sl_status_t sl_si91x_configure_ip_address(sl_net_ip_configuration_t *address, uint8_t virtual_ap_id);

/** @} */

sl_status_t sl_si91x_set_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t type,
                                    const void *credential,
                                    uint32_t credential_length);

sl_status_t sl_si91x_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);
