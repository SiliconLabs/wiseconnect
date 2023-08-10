/***************************************************************************/ /**
 * @file
 * @brief Networking constants
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

#include <stdint.h>
#include <limits.h>
#include "sl_wifi_constants.h"

/// Network Interface Type mask
#define NETWORK_INTERFACE_TYPE_MASK 0xFFF8

/// Interface Type Selection
#define SL_NET_INTERFACE_TYPE(x) (x & NETWORK_INTERFACE_TYPE_MASK)

/** \addtogroup SL_NET_CONSTANTS Constants
 * @{ */

/// Enumeration of network interfaces.
typedef enum {
  SL_NET_WIFI_CLIENT_INTERFACE = (1 << 3), ///< WiFi Client Interface
  SL_NET_WIFI_AP_INTERFACE     = (2 << 3), ///< WiFi Access Point Interface
  SL_NET_ETHERNET_INTERFACE    = (3 << 3), ///< Ethernet Interface
  SL_NET_THREAD_INTERFACE      = (4 << 3), ///< Thread Interface
  SL_NET_BLUETOOTH_INTERFACE   = (5 << 3), ///< Bluetooth Interface
  SL_NET_ZWAVE_INTERFACE       = (6 << 3), ///< Z-Wave Interface
} sl_net_interface_t;

/** @} */

/// WiFi Client Interface
#define SL_NET_WIFI_CLIENT_INTERFACE (1 << 3)

/// WiFi Access Point Interface
#define SL_NET_WIFI_AP_INTERFACE (2 << 3)

/// Ethernet Interface
#define SL_NET_ETHERNET_INTERFACE (3 << 3)

/// Thread Interface
#define SL_NET_THREAD_INTERFACE (4 << 3)

/// Bluetooth Interface
#define SL_NET_BLUETOOTH_INTERFACE (5 << 3)

/// Zwave Interface
#define SL_NET_ZWAVE_INTERFACE (6 << 3)

/** \addtogroup SL_NET_CONSTANTS Constants
 * @{ */

/// Enumeration of network packet types.
typedef enum {
  SL_NET_UDP_PACKET,       /// UDP Packet
  SL_NET_TCP_PACKET,       /// TCP Packet
  SL_NET_ETHERNET_PACKET,  /// Ethernet Packet
  SL_NET_TLS_PACKET,       /// TLS Packet
  SL_NET_DTLS_PACKET,      /// DTLS Packet
  SL_NET_THREAD_PACKET,    /// Thread Packet
  SL_NET_BLUETOOTH_PACKET, /// Bluetooth Packet
  SL_NET_ZWAVE_PACKET,     /// Zwave Packet
  SL_NET_ZIGBEE_PACKET,    /// Zigbee Packet
  SL_NET_6LOWPAN_PACKET,   /// LOWPAN Packet
  SL_NET_RAW_PACKET,       /// RAW Packet
} sl_net_packet_type_t;

/// Enumeration of IP address resolution methods.
typedef enum {
  SL_NET_AUTOMATIC_ADDRESS_RESOLUTION,
  SL_NET_STATIC_ADDRESS_RESOLUTION,
  SL_NET_DHCP_ADDRESS_RESOLUTION = SL_NET_AUTOMATIC_ADDRESS_RESOLUTION,
  SL_NET_LINK_LOCAL_ADDRESS_RESOLUTION,
} sl_net_address_resolution_t;

/// Enumeration of DNS resolution IP type.
typedef enum {
  SL_NET_DNS_TYPE_IPV4, ///< IPV4 DNS Address resolution
  SL_NET_DNS_TYPE_IPV6  ///< IPV6 DNS Address resolution
} sl_net_dns_resolution_ip_type_t;

/// Enumeration of SL-Net Event.
typedef enum {
  SL_NET_INTERFACE_UP_EVENT,             ///< Interface Up Event
  SL_NET_INTERFACE_DOWN_EVENT,           ///< Interface Down Event
  SL_NET_IP_UP_EVENT,                    ///< IP Up Event
  SL_NET_IP_DOWN_EVENT,                  ///< IP Down Event
  SL_NET_IP_EXPIRED_EVENT,               ///< IP Expired Event
  SL_NET_PING_RESPONSE_EVENT,            ///< Ping Response Event
  SL_NET_WIFI_CLIENT_CONNECTED_EVENT,    ///< Client Connected Event
  SL_NET_WIFI_CLIENT_DISCONNECTED_EVENT, ///< Client Disconnected Event
  SL_NET_DNS_RESOLVE_EVENT,              ///< DNS Address resolution Event
  SL_NET_OTA_FW_UPDATE_EVENT,            ///< OTA Firmware Update Event
  SL_NET_EVENT_COUNT                     ///< Maximum event count
} sl_net_event_t;

/// SL Network profile ID.
typedef enum {
  SL_NET_PROFILE_ID_0  = 0,  ///< Profile Id 0
  SL_NET_PROFILE_ID_1  = 1,  ///< Profile Id 1
  SL_NET_PROFILE_ID_2  = 2,  ///< Profile Id 2
  SL_NET_PROFILE_ID_3  = 3,  ///< Profile Id 3
  SL_NET_PROFILE_ID_4  = 4,  ///< Profile Id 4
  SL_NET_PROFILE_ID_5  = 5,  ///< Profile Id 5
  SL_NET_PROFILE_ID_6  = 6,  ///< Profile Id 6
  SL_NET_PROFILE_ID_7  = 7,  ///< Profile Id 7
  SL_NET_PROFILE_ID_8  = 8,  ///< Profile Id 8
  SL_NET_PROFILE_ID_9  = 9,  ///< Profile Id 9
  SL_NET_PROFILE_ID_10 = 10, ///< Profile Id 10

  SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID = SL_NET_PROFILE_ID_0, ///< WiFi Client Default Profile
  SL_NET_DEFAULT_WIFI_AP_PROFILE_ID     = SL_NET_PROFILE_ID_0, ///< WiFi Access Point Default Profile
  SL_NET_DEFAULT_ETHERNET_PROFILE_ID    = SL_NET_PROFILE_ID_0, ///< Ethernet Default Profile
  SL_NET_DEFAULT_THREAD_PROFILE_ID      = SL_NET_PROFILE_ID_0, ///< Thread Default Profile
  SL_NET_DEFAULT_ZWAVE_PROFILE_ID       = SL_NET_PROFILE_ID_0, ///< Zwave Default Profile
} sl_net_profile_id_t;

/// Enumeration of network credential types.
typedef enum {
  SL_NET_INVALID_CREDENTIAL_TYPE, ///< Invalid Credential Type
  SL_NET_WIFI_PSK,                ///< WiFi PSk Credential
  SL_NET_WIFI_PMK,                ///< WiFi PMK Credential
  SL_NET_WIFI_WEP,                ///< WiFi WEP Credential
  SL_NET_WIFI_EAP,                ///< WiFi EAP Credential
  SL_NET_WIFI_PEAP,               ///< WiFi PEAP Credential
  SL_NET_MQTT_CLIENT_CREDENTIALS, ///< MQTT Client Credential
  SL_NET_HTTP_CLIENT_CREDENTIALS, ///< HTTP Client Credential
  SL_NET_TLS_SERVER_CERTIFICATE,  ///< TLS Server Certificate
  SL_NET_TLS_SERVER_KEY,          ///< TLS Server Key
  SL_NET_TLS_CLIENT_CERTIFICATE,  ///< TLS Client Certificate
  SL_NET_TLS_CLIENT_KEY,          ///< TLS Client Key
  SL_NET_TLS_ROOT_CERTIFICATE,    ///< TLS Root Certificate
} sl_net_credential_type_t;

/// Enumeration of network credential identifiers.
typedef enum {
  SL_NET_INVALID_CREDENTIAL_ID = 0,           ///< Invalid Credential Id
  SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,   ///< WiFi Client Credential Id
  SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,       ///< WiFi Access Point Credential Id
  SL_NET_MQTT_SERVER_CERTIFICATE_ID,          ///< MQTT Server Certificate Id
  SL_NET_MQTT_SERVER_KEY_ID,                  ///< MQTT Server Key Id
  SL_NET_MQTT_CLIENT_CREDENTIALS_ID,          ///< MQTT Client Credential Id
  SL_NET_HTTP_CLIENT_CREDENTIAL_ID,           ///< HTTP Client Credential Id
  SL_NET_DEFAULT_PMK_CREDENTIAL_ID,           ///< Default PMK Credential Id
  SL_NET_USER_CREDENTIAL_ID = (int)(1u << 7), ///< User Credential Id
} sl_net_credential_id_t;

typedef uint32_t sl_net_certificate_id_t; ///< Unique Certificate store Id.

/** @} */
