/***************************************************************************/ /**
 * @file
 * @brief SL Network API
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

#include "sl_status.h"
#include "sl_net_types.h"

/** 
 * \addtogroup NET_INTERFACE_FUNCTIONS Network Interface
 * 
 * @note Stack overflows may occur when invoking functions or using your own variables or data structures while handling callbacks.
 *       Configure the stack size by modifying the pre-processor macro `SL_SI91X_EVENT_HANDLER_STACK_SIZE` as 
 *       per your application's requirements. See [here](https://docs.silabs.com/wiseconnect/latest/wiseconnect-developers-guide-prog-preprocessor-build-settings/list-of-preprocessor-build-settings) 
 *       for the instructions in modifying a pre-processor macro.
 * @note Event/Callback handlers must not contain function calls or code which can block or delay the execution of 
 *       the event/callback handler as it will cause all the other events to queue up and delay the execution of 
 *       other events since all the events are invoked and handled from a single thread.
 * @note Do not call any synchronous SDK APIs from within the Event/Callback handlers.
 * 
 * \ingroup SL_NET_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Initialize a network interface. This API should be called before calling any sl_net API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] configuration
 *   Configuration object specific to network interface of type [sl_wifi_device_configuration_t](../wiseconnect-api-reference-guide-si91x-driver/sl-wifi-device-configuration-t).
 *   If configuration = NULL, then following configuration is used internally by SDK
 * | sl_net_interface_t           | Default configuration                 |
 * |:-----------------------------|:--------------------------------------|
 * | SL_NET_WIFI_CLIENT_INTERFACE | sl_wifi_default_client_configuration  |
 * | SL_NET_WIFI_AP_INTERFACE     | sl_wifi_default_ap_configuration      |
 * @param[in] network_context
 *   Runtime context specific to network interface.
 * @param[in] event_handler
 *   Processes all network events related to network interface as identified by @ref sl_net_event_handler_t
 * 
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.4
 * @note 
 *   For Wi-Fi events, sl_net uses the wifi callback framework. Register the corresponding Wi-Fi event handlers using [sl_wifi_set_callback](../wiseconnect-api-reference-guide-wi-fi/wifi-callback-framework#sl-wifi-set-callback) API.
 * @note
 * 	 Parameter \p network_context is only being used when module is acting as station in external stack mode[lwIP].
 * 	 In such case, \p network_context is supposed to refer a valid @ref sl_net_wifi_lwip_context_t variable.
 *
 ******************************************************************************/
sl_status_t sl_net_init(sl_net_interface_t interface,
                        const void *configuration,
                        void *network_context,
                        sl_net_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief
 *   De-initialize a network interface.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_net_init should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_deinit(sl_net_interface_t interface);

/***************************************************************************/ /**
 * @brief
 *   Bring a network interface up.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_net_init should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] profile_id
 *   Network profile identifier for specific interface of type @ref sl_net_profile_id_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
   @note
 *   By default, profile and credential configurations in sl_net_defaults.h are used by SDK. 
 *   User can define their profile and credential configurations for an interface, by calling @ref sl_net_set_profile and @ref sl_net_set_credentials APIs before calling @ref sl_net_up API".
 * ******************************************************************************/
sl_status_t sl_net_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id);

/***************************************************************************/ /**
 * @brief
 *   Bring a network interface down.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_net_up should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_down(sl_net_interface_t interface);

/** @} */

/** 
 * \addtogroup NET_PROFILE_FUNCTIONS Network Profiles
 * \ingroup SL_NET_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Store a network profile for a given interface.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] index
 *   Profile storage index / identifier of type @ref sl_net_profile_id_t
 * @param[in] profile
 *   Pointer to profile data of type @ref sl_net_profile_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_set_profile(sl_net_interface_t interface, sl_net_profile_id_t id, const sl_net_profile_t *profile);

/***************************************************************************/ /**
 * @brief
 *   Retrieve a stored network profile for a given interface.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] index
 *   Profile storage index / identifier of type @ref sl_net_profile_id_t
 * @param[out] profile
 *   Pointer to @ref sl_net_profile_t object that stores data.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_get_profile(sl_net_interface_t interface, sl_net_profile_id_t id, sl_net_profile_t *profile);

/***************************************************************************/ /**
 * @brief
 *   Delete a stored network profile for a given interface.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] index
 *   Profile storage index / identifier of type @ref sl_net_profile_id_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_delete_profile(sl_net_interface_t interface, sl_net_profile_id_t id);

/** @} */

/** 
 * \addtogroup NET_CREDENTIAL_FUNCTIONS Network Credential
 * \ingroup SL_NET_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Set a network credential including client credentials, certificates, and keys.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] id
 *   Network credential identifier as identified by @ref sl_net_credential_id_t
 * @param[in] type
 *   Network credential type as identified by @ref sl_net_credential_type_t
 * @param[in] credential
 *   Pointer to the credential data object.
 * @param[in] credential_length
 *   Length of the credential data object.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_set_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t type,
                                  const void *credential,
                                  uint32_t credential_length);

/***************************************************************************/ /**
 * @brief
 *   Retrieve a stored network credential.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] id
 *   Network credential identifier as identified by @ref sl_net_credential_id_t
 * @param[out] type
 *   Network credential type as identified by @ref sl_net_credential_type_t
 * @param[out] credential
 *   Pointer to location where credential data is stored.
 * @param[in,out] credential_length
 *   in: Number of bytes available at credential,
 *   out: Number of bytes written.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_get_credential(sl_net_credential_id_t id,
                                  sl_net_credential_type_t *type,
                                  void *credential,
                                  uint32_t *credential_length);

/***************************************************************************/ /**
 * @brief
 *   Delete a stored network credential.
 * @pre Pre-conditions:
 * -
 *   @ref sl_net_init should be called before this API.
 * @param[in] id
 *   Network credential identifier as identified by @ref sl_net_credential_id_t
 * @param[out] type
 *   Network credential type as identified by @ref sl_net_credential_type_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_delete_credential(sl_net_credential_id_t id, sl_net_credential_type_t type);

/** @} */

// /** \addtogroup NET_CERTIFICATE_FUNCTIONS Certificate API
//  * \ingroup SL_NET_FUNCTIONS
//  * @{ */

/***************************************************************************/ /**
 * @brief 
 * @param[in] id
 * @param[in] data
 * @param[in] data_length
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_net_set_certificate(sl_net_certificate_id_t id, const void *data, uint32_t data_length);

/***************************************************************************/ /**
 * @brief 
 * @param[in]  id
 * @param[out] data
 * @param[in]  data_length
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_net_get_certificate(sl_net_certificate_id_t id, void *data, uint32_t data_length);

/***************************************************************************/ /**
 * @note
 *   This API is not yet implemented.
 ******************************************************************************/
sl_status_t sl_net_verify_certificate();

// /** @} */

/***************************************************************************/ /**
 * @brief
 *   Convert an IPv4 address in string of from a.b.c.d to a binary uint32_t value
 * @param[in] addr
 *   IPV4 address.
 * @param[out] value
 *   Binary value of the given IP address.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_inet_addr(const char *addr, uint32_t *value);

/** 
 * \addtogroup NET_MULTICAST_FUNCTIONS Multicast
 * \ingroup SL_NET_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Enable multicast for the given IP address.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_net_up should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] ip_address
 *   Multicast IP address of type [sl_ip_address_t](../wiseconnect-api-reference-guide-common/ip-addresses#sl-ip-address-t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_join_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address);

/***************************************************************************/ /**
 * @brief
 *   Disable multicast for the given IP address.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_net_up should be called before this API.
 * @param[in] interface
 *   Interface identified by @ref sl_net_interface_t
 * @param[in] ip_address
 *   Multicast IP address of type [sl_ip_address_t](../wiseconnect-api-reference-guide-common/ip-addresses#sl-ip-address-t)
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_net_leave_multicast_address(sl_net_interface_t interface, const sl_ip_address_t *ip_address);

/** @} */

// Helper functions
sl_status_t sl_net_wifi_client_init(sl_net_interface_t interface,
                                    const void *configuration,
                                    void *context,
                                    sl_net_event_handler_t event_handler);
sl_status_t sl_net_wifi_client_deinit(sl_net_interface_t interface);
sl_status_t sl_net_wifi_client_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id);
sl_status_t sl_net_wifi_client_down(sl_net_interface_t interface);

sl_status_t sl_net_wifi_ap_init(sl_net_interface_t interface,
                                const void *configuration,
                                void *context,
                                sl_net_event_handler_t event_handler);
sl_status_t sl_net_wifi_ap_deinit(sl_net_interface_t interface);
sl_status_t sl_net_wifi_ap_up(sl_net_interface_t interface, sl_net_profile_id_t profile_id);
sl_status_t sl_net_wifi_ap_down(sl_net_interface_t interface);
