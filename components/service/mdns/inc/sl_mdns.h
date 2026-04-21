/***************************************************************************/ /**
 * @file  sl_mdns.h
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
#ifndef SL_MDNS_INCLUDED_H
#define SL_MDNS_INCLUDED_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_wifi_host_interface.h"
#include "sl_ip_types.h"
#include "sl_net_constants.h"

/**
 *  @addtogroup SERVICE_MDNS 
 *  @{
 */

/// mDNS events
typedef enum {
  SL_MDNS_SERVICE_DISCOVERED_EVENT = 0 ///< Event for mDNS service discovery. (Currently not supported.)
} sl_mdns_event_t;

/// mDNS protocol
typedef enum {
  SL_MDNS_PROTO_PHY = 0, ///< Run mDNS using ethernet frames. (Not supported in offload mode.)
  SL_MDNS_PROTO_UDP      ///< Run mDNS using UDP.
} sl_mdns_protocol_t;

/******************************************************
 *                   Type Definitions
 ******************************************************/
/// mDNS service query
typedef struct {
  const char *instance_name;    ///< mDNS instance service name
  const char *service_type;     ///< mDNS service type
  const char *service_sub_type; ///< mDNS service sub type
  sl_net_interface_t interface; ///< Target interface
  uint16_t timeout;             ///< Time out for query
} sl_mdns_service_query_t;

/// mDNS service
typedef struct {
  const char *instance_name;   ///< mDNS instance service name
  const char *service_type;    ///< mDNS service type
  const char *service_message; ///< mDNS service message
  uint16_t port;               ///< Service port number
  uint16_t ttl;                ///< Service TTL

  /* NOTE: The following parameters are not used in offload mode. */
  sl_ipv4_address_t
    ipv4; ///< IPv4 address of service. (In case of offload mode, it is provided by embedded network stack.)
  sl_ipv6_address_t
    ipv6; ///< IPv6 address of service. (In case of offload mode, it is provided by embedded network stack.)
} sl_mdns_service_t;

/// mDNS service discovery request
typedef struct {
  char *service_name; ///< Target service name for discovery
  sl_net_interface_t
    interface; ///< Interface on which to discover the service from [sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t).
  uint16_t timeout; ///< timeout for the discovery
} sl_mdns_discover_request_t;

/// MDNS instance configuration
typedef struct {
  sl_mdns_protocol_t protocol; ///< Protocol to use for mDNS from @ref sl_mdns_protocol_t.
  sl_ip_version_t
    type; ///< IP version to use for mDNS from [sl_ip_version_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-ip-address-t).
  char host_name
    [32]; ///< Host Name to use for the mDNS Instance. The host name Should contain dot(.) at the end (for example, "wiseconnect.local."). The string length should not exceed 32 including NULL terminator.
} sl_mdns_configuration_t;

/// mDNS interface
typedef struct {
  sl_net_interface_t
    interface; ///< Network interface of type [sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t).
  sl_wifi_buffer_t *
    service_list; ///< Pointer to the service list of type [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t).
} sl_mdns_interface_t;

// Forward declaration of the structure to allow usage in the sl_mdns_event_handler_t definition.
/// mDNS instance handle
typedef struct sl_mdns_s sl_mdns_t;

/**
 * @typedef sl_mdns_event_handler_t
 * @brief mDNS event handler
 * @param[in] mdns  mDNS instance handle of type @ref sl_mdns_t.
 * @param[in] event mDNS event of type @ref sl_mdns_event_t.
 * @param[in] data  Data pointer containing data structure of corresponding to corresponding event of type @ref sl_mdns_event_t.
 */
typedef void (*sl_mdns_event_handler_t)(sl_mdns_t *mdns, sl_mdns_event_t event, void *data);

/// mDNS instance handle
struct sl_mdns_s {
  sl_mdns_configuration_t configuration; ///< mDNS configuration of type @ref sl_mdns_configuration_t
  sl_mdns_event_handler_t event_handler; ///< mDNS event handler of type @ref sl_mdns_event_handler_t
  sl_wifi_buffer_t *
    interface_list; ///< Pointer to interface list of type [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t).
  uint8_t service_count; ///< Count of the total number of services being registered on all interfaces.
};

/**
 * @brief
 * Initialize mDNS instance.
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t. This cannot be modified by the application after this API call.
 * @param[in] config		Valid pointer to mDNS configuration structure of type @ref sl_mdns_configuration_t. This value cannot be null.
 * @param[in] event_handler Event handler of type @ref sl_mdns_event_handler_t for receiving asynchronous events.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note
 *   This API needs to be called before calling any other mDNS API
 * @note
 *   @ref sl_mdns_event_handler_t currently not supported. User should pass NULL for event_handler.
 */
sl_status_t sl_mdns_init(sl_mdns_t *mdns, const sl_mdns_configuration_t *config, sl_mdns_event_handler_t event_handler);

/**
 * @brief
 * De-initialize mDNS instance.
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_deinit(sl_mdns_t *mdns);

/**
 * @brief
 * Add network interface to mDNS instance.
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t.
 * @param[in] interface     Network interface of type [sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t) that needs to be added to mDNS instance.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_add_interface(sl_mdns_t *mdns, sl_net_interface_t interface);

/**
 * @brief
 * Remove network interface to mDNS instance. (Currently not supported.) 
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t.
 * @param[in] interface     Network interface of type [sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t) that needs to be removed from mDNS instance.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_remove_interface(const sl_mdns_t *mdns, sl_net_interface_t interface);

/**
 * @brief
 * Register a service in mDNS instance.
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t.
 * @param[in] interface     Network interface of type [sl_net_interface_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-interface-t) to which service needed to be added to mDNS instance.
 * @param[in] service		Valid pointer to mDNS service configuration structure of type @ref sl_mdns_service_t . This value cannot be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_register_service(sl_mdns_t *mdns, sl_net_interface_t interface, const sl_mdns_service_t *service);

/**
 * @brief
 * Unregister a service from mDNS instance. (Currently not supported.) 
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t
 * @param[in] service_query Valid pointer to mDNS service query structure of type @ref sl_mdns_service_query_t . This value cannot be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_unregister_service(const sl_mdns_t *mdns, const sl_mdns_service_query_t *service_query);

/**
 * @brief
 * Update service message of a service in mDNS instance. (Currently not supported.)
 * @param[in] mdns              mDNS instance handle of type @ref sl_mdns_t.
 * @param[in] service_query     Valid pointer to mDNS service query structure of type @ref sl_mdns_service_query_t. This value cannot be null.
 * @param[in] message           Valid pointer to a buffer containing service message string . This value cannot be null.
 * @param[in] message_length    Length of the message buffer.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_update_service_message(const sl_mdns_t *mdns,
                                           const sl_mdns_service_query_t *service_query,
                                           const char *message,
                                           uint32_t message_length);

/**
 * @brief
 * Discover services using mDNS instance. (Currently not supported.)
 * @param[in] mdns          mDNS instance handle of type @ref sl_mdns_t
 * @param[in] service_query Valid pointer to mDNS service query structure of type @ref sl_mdns_service_query_t . This value cannot be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sl_mdns_discover_service(const sl_mdns_t *mdns, const sl_mdns_service_query_t *service_query);

/** @} */

#endif //SL_MDNS_INCLUDED_H
