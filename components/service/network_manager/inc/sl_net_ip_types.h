/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include <stdint.h>
#include <limits.h>
#include "sl_ip_types.h"

/** \addtogroup SL_NET_TYPES Types
 * @{ */

/// IPv4 address settings for a network interface
typedef struct {
  sl_ipv4_address_t ip_address; ///< IPv4 address
  sl_ipv4_address_t gateway;    ///< IPv4 gateway address
  sl_ipv4_address_t netmask;    ///< IPv4 netmask
} sl_net_ipv4_setting_t;

/// IPv6 address settings for a network interface
typedef struct {
  sl_ipv6_address_t link_local_address; ///< IPv6 link local address
  sl_ipv6_address_t global_address;     ///< IPv6 global address
  sl_ipv6_address_t gateway;            ///< IPv6 gateway address
} sl_net_ipv6_setting_t;

typedef struct {
  sl_ip_management_t mode;   ///< IP Assignment Type
  sl_ip_address_type_t type; ///< IP Address Type
  char *host_name;           ///< Host name visible on network
  union {
    sl_net_ipv4_setting_t v4; ///< IPv4 setting to be used in case of static IP address assignment
    sl_net_ipv6_setting_t v6; ///< IPv6 setting to be used in case of static IP address assignment
  } ip;                       ///< IP setting to be used for static IP address assignment
} sl_net_ip_configuration_t;

/// Wi-Fi specific IPv4 address settings
typedef struct {
  sl_ipv4_address_t ip_address; ///< IPv4 address
  sl_ipv4_address_t gateway;    ///< IPv4 gateway address
  sl_ipv4_address_t netmask;    ///< IPv4 netmask
} sl_wifi_ipv4_setting_t;

/// Wi-Fi specific IPv6 address settings
typedef struct {
  sl_ipv6_address_t link_local_address; ///< IPv6 link local address
  sl_ipv6_address_t global_address;     ///< IPv6 global address
  sl_ipv6_address_t gateway;            ///< IPv6 gateway address
} sl_wifi_ipv6_setting_t;

/// Ethernet specific IPv4 address settings
typedef struct {
  sl_ipv4_address_t ip_address; ///< IPv4 address
  sl_ipv4_address_t gateway;    ///< IPv4 gateway address
  sl_ipv4_address_t netmask;    ///< IPv4 netmask
} sl_ethernet_ipv4_setting_t;

/// Ethernet specific IPv6 address settings
typedef struct {
  sl_ipv6_address_t link_local_address; ///< IPv6 link local address
  sl_ipv6_address_t global_address;     ///< IPv6 global address
  sl_ipv6_address_t gateway;            ///< IPv6 gateway address
} sl_ethernet_ipv6_setting_t;

/// Thread specific IPv6 address settings
typedef struct {
  sl_ipv6_address_t link_local_address; ///< IPv6 link local address
  sl_ipv6_address_t mesh_local_address; ///< IPv6 mesh local address
  sl_ipv6_address_t global_address;     ///< IPv6 global address
} sl_thread_ipv6_setting_t;

/** @} */
