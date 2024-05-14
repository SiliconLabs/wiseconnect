/*******************************************************************************
 * # License
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
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
 *  @addtogroup SERVICE_MDNS_CONSTANTS 
 *  @{
 */

/// MDNS Asynchronous Events
typedef enum {
  SL_MDNS_SERVICE_DISCOVERED_EVENT = 0 ///< Event for MDNS Service Discovery
} sl_mdns_event_t;

/// MDNS
typedef enum {
  SL_MDNS_PROTO_PHY = 0, ///< Run MDNS using Ethernet Frames (Not supported in offload mode)
  SL_MDNS_PROTO_UDP      ///< Run MDNS Using UDP
} sl_mdns_protocol_t;

/** @} */

/******************************************************
 *                   Type Definitions
 ******************************************************/

/**
 * @addtogroup SERVICE_MDNS_TYPES
 * @{
 */
/// MDNS service query
typedef struct {
  const char *instance_name;    ///< MDNS Instance service name
  const char *service_type;     ///< MDNS service type
  const char *service_sub_type; ///< MDNS service sub type
  sl_net_interface_t interface; ///< Target Interface
  uint16_t timeout;             ///< Time out for query
} sl_mdns_service_query_t;

/// MDNS service
typedef struct sl_mdns_service_configuration_s {
  const char *instance_name;   ///< MDNS Instance service name
  const char *service_type;    ///< MDNS service type
  const char *service_message; ///< MDNS service message
  uint16_t port;               ///< Service port number
  uint16_t ttl;                ///< Service TTL

  /* NOTE: Below parameters are not used in offload mode. */
  sl_ipv4_address_t
    ipv4; ///< IPv4 address of service (In case of offload mode, it is provided by embedded network stack)
  sl_ipv6_address_t
    ipv6; ///< IPv6 address of service (In case of offload mode, it is provided by embedded network stack)
} sl_mdns_service_t;

/// MDNS service discovery request
typedef struct {
  char *service_name;           ///< Target Service name for discovery
  sl_net_interface_t interface; ///< Interface on which to discover the service from @ref sl_net_interface_t
  uint16_t timeout;             ///< timeout for the discovery
} sl_mdns_discover_request_t;

/// MDNS instance configuration
typedef struct {
  sl_mdns_protocol_t protocol; ///< Protocol to use for MDNS from @ref sl_mdns_protocol_t
  sl_ip_version_t type;        ///< IP version to use for MDNS from @ref sl_ip_version_t
  char host_name
    [32]; ///< Host Name to use for the MDNS Instance. The host name Should contain dot(.) at the end, For example "wiseconnect.local.". The string length should not exceed 32 including NULL terminator.
} sl_mdns_configuration_t;

/// MDNS interface
typedef struct {
  sl_net_interface_t interface;   ///< Network Interface of type @ref sl_net_interface_t
  sl_wifi_buffer_t *service_list; ///< Pointer to service list of Type @ref sl_wifi_buffer_t
} sl_mdns_interface_t;

/// MDNS instance handle
typedef struct sl_mdns_s sl_mdns_t;

/**
 * @typedef sl_mdns_event_handler_t
 * @brief MDNS event handler
 * @param[in] mdns  MDNS instance handle of type @ref sl_mdns_t
 * @param[in] event MDNS event of type @ref sl_mdns_event_t.
 * @param[in] data  Data pointer containing data structure of corresponding to corresponding event of type @ref sl_mdns_event_t
 */
typedef void (*sl_mdns_event_handler_t)(sl_mdns_t *mdns, sl_mdns_event_t event, void *data);

/// MDNS instance handle
struct sl_mdns_s {
  sl_mdns_configuration_t configuration; ///< MDNS configuration of type @ref sl_mdns_configuration_t
  sl_mdns_event_handler_t event_handler; ///< MDNS event handler of type @ref sl_mdns_event_handler_t
  sl_wifi_buffer_t *interface_list;      ///< Pointer to interface list of Type @ref sl_wifi_buffer_t
  uint8_t service_count;                 ///< Count of total number of services being registered on all interfaces
};

/** @} */

/**
 * @addtogroup SERVICE_MDNS_FUNCTIONS
 * @{
 */

/**
 * @brief
 * Initialize MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t. This Shall not be modified by application after this API call.
 * @param[in] config		Valid pointer to mdns configuration structure of type @ref sl_mdns_configuration_t. This shall not be null.
 * @param[in] event_handler Event handler of type @ref sl_mdns_event_handler_t for receiving asynchronous events.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API needs to be called before calling any other MDNS API
 */
sl_status_t sl_mdns_init(sl_mdns_t *mdns, const sl_mdns_configuration_t *config, sl_mdns_event_handler_t event_handler);

/**
 * @brief
 * De-initialize MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_deinit(sl_mdns_t *mdns);

/**
 * @brief
 * Add network interface to MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @param[in] interface     Network interface of type @ref sl_net_interface_t that needs to be added to MDNS instance
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_add_interface(sl_mdns_t *mdns, sl_net_interface_t interface);

/**
 * @brief
 * Remove network interface to MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @param[in] interface     Network interface of type @ref sl_net_interface_t that needs to be removed from MDNS instance
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_remove_interface(sl_mdns_t *mdns, sl_net_interface_t interface);

/**
 * @brief
 * Register a service in MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @param[in] interface     Network interface of type @ref sl_net_interface_t to which service needed to be added to MDNS instance
 * @param[in] service		Valid pointer to mdns service configuration structure of type @ref sl_mdns_service_t . This shall not be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_register_service(sl_mdns_t *mdns, sl_net_interface_t interface, sl_mdns_service_t *service);

/**
 * @brief
 * Unregister a service from MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @param[in] service_query Valid pointer to mdns service query structure of type @ref sl_mdns_service_query_t . This shall not be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_unregister_service(sl_mdns_t *mdns, sl_mdns_service_query_t *service_query);

/**
 * @brief
 * Update service message of a service in MDNS Instance.
 * @param[in] mdns              MDNS instance handle of type @ref sl_mdns_t
 * @param[in] service_query     Valid pointer to mdns service query structure of type @ref sl_mdns_service_query_t . This shall not be null.
 * @param[in] message           Valid pointer to a buffer containing service message string . This shall not be null.
 * @param[in] message_length    Length of the message buffer.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_update_service_message(sl_mdns_t *mdns,
                                           sl_mdns_service_query_t *query,
                                           const char *message,
                                           uint32_t message_length);

/**
 * @brief
 * Discover services using MDNS Instance.
 * @param[in] mdns          MDNS instance handle of type @ref sl_mdns_t
 * @param[in] service_query Valid pointer to mdns service query structure of type @ref sl_mdns_service_query_t . This shall not be null.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 */
sl_status_t sl_mdns_discover_service(sl_mdns_t *mdns, sl_mdns_service_query_t *query);

/** @} */

#endif //SL_MDNS_INCLUDED_H