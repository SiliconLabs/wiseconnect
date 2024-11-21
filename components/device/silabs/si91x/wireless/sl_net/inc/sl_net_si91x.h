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

#include "sl_net_types.h"
#include "sl_net_ip_types.h"
#include "sl_net_constants.h"
#include "sl_wifi_host_interface.h"

/** \addtogroup SI91X_NETWORK_FUNCTIONS 
 * \ingroup SL_SI91X_API
 * @{ */
/***************************************************************************/
/**
 * @brief
 *   Configure the IP address for the specified virtual AP.
 * 
 * @details
 *   This function configures the IP address for the specified virtual Access Point (AP) on the Si91x device. The IP address details are provided in the `address` parameter.
 * 
 *   Virtual AP ID is used to differentiate between client and AP in concurrent mode.
 * 
 * @param[in] address
 *   Pointer to an [sl_net_ip_configuration_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-ip-configuration-t) structure where the assigned IP address details will be stored.
 * 
 * @param[in] virtual_ap_id
 *   Virtual AP ID. One of the values from @ref sl_si91x_wifi_vap_id_t.
 * 
 * @return
 *   sl_status_t. See [Status Codes](https://docs.silabs.com/gecko-platform/latest/platform-common/status) and [Additional Status Codes](../wiseconnect-api-reference-guide-err-codes/sl-additional-status-errors) for details.
 */
sl_status_t sl_si91x_configure_ip_address(sl_net_ip_configuration_t *address, uint8_t virtual_ap_id);
/** @} */

/***************************************************************************/ /**
 * @brief
 *   Register a event handler for network events.
 * @param[in] function
 *   Function pointer to callback.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_si91x_register_event_handler(sl_net_event_handler_t function);

/***************************************************************************/ /**
 * @brief
 *   Default event handler for all events.
 * @param[in] event
 *   [sl_net_event_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-event-t) Asynchronous event received.
 * @param[in] buffer
 *   [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) Buffer containing data related to asynchronous event.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 ******************************************************************************/
sl_status_t sl_si91x_default_handler(sl_net_event_t event, sl_wifi_buffer_t *buffer);

//! @cond Doxygen_Suppress
/// Enumerate multicast address command types
typedef enum {
  SL_WIFI_MULTICAST_LEAVE = 0, ///<  Wi-Fi multicast leave
  SL_WIFI_MULTICAST_JOIN  = 1, ///< Wi-Fi mulitcast join
} sl_si91x_multicast_address_command_type_t;

/// Enumerate IP configuration modes for SI91X module
typedef enum {
  SL_SI91X_STATIC = 0,    ///< Static
  SL_SI91X_DHCP,          ///< DHCP
  SL_SI91X_DHCP_RESERVED, ///< DHCP Rseserved
  SL_SI91X_DHCP_HOSTNAME, ///< DHCP HostName
  SL_SI91X_DHCP_OPTION81, ///< DHCP option 81
  SL_SI91X_DHCP_OPTION77  ///< DHCP option 77
} sl_si91x_ip_config_mode_t;

sl_status_t sl_si91x_set_credential(sl_net_credential_id_t id,
                                    sl_net_credential_type_t type,
                                    const void *credential,
                                    uint32_t credential_length);

sl_status_t sl_si91x_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);
//! @endcond
