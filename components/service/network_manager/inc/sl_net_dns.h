/*******************************************************************************
* @file  sl_net_dns.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#pragma once

#include "sl_ip_types.h"
#include "sl_status.h"
#include "sl_net_constants.h"

/** 
 * \addtogroup NET_INTERFACE_FUNCTIONS Network Interface
 * \ingroup SL_NET_FUNCTIONS
 * @{ */

/**
 * Resolve given host name to IP address.
 * @pre Pre-conditions:
 * -
 *   Before calling this API, [SL_SI91X_TCP_IP_FEAT_DNS_CLIENT](../wiseconnect-api-reference-guide-si91x-driver/si91-x-tcp-ip-feature-bitmap#sl-si91-x-tcp-ip-feat-dns-client) bit should be enabled in TCP/IP feature bitmap.
 * @param[in] host_name 			Host name which needs to be resolved.
 * @param[in] timeout 				timeout in millisecs.
 * @param[in] dns_resolution_ip 	DNS resolution by IP of type @ref sl_net_dns_resolution_ip_type_t
 * @param[out] ip_address 			IP address object to store resolved IP address of type [sl_ip_address_t](../wiseconnect-api-reference-guide-common/ip-addresses#sl-ip-address-t)
 * @return
 *  sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note If timeout value is greater than zero, caller would be blocked till timeout milliseconds to get the response.
 * 		 If the values is equal's to zero, response would sent through @ref sl_net_event_handler_t.
 */
sl_status_t sl_net_host_get_by_name(const char *host_name,
                                    const uint32_t timeout,
                                    const sl_net_dns_resolution_ip_type_t dns_resolution_ip,
                                    sl_ip_address_t *ip_address);

/** @} */
