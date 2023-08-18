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

#include "sl_si91x_constants.h"
#include "sl_wifi_host_interface.h"
#include <stdint.h>

/** \ingroup SL_SI91X_CONSTANTS
 * \addtogroup SI91X_RESPONSE_HANDLING_FLAGS si91x_response_handling_flags
 * @{
 * */

/// Flag to indicate that the response status of the command is expected
#define SI91X_PACKET_RESPONSE_STATUS (1 << 0)

/// Flag to indicate that the response packet of the command is expected
#define SI91X_PACKET_RESPONSE_PACKET (1 << 1)

/// Flag to indicate that all the packet tx has to be suspended until the corresponding command response is received
#define SI91X_PACKET_GLOBAL_QUEUE_BLOCK (1 << 3)

/// Flag to indicate that host would receive the response from firmware in asynchronous manner.
#define SI91X_PACKET_WITH_ASYNC_RESPONSE (1 << 4)
/** @} */

/** \ingroup SL_SI91X_CONSTANTS
 * @{
 * */
/// Si91x command types
typedef enum {
  SI91X_COMMON_CMD  = 0, ///< SI91X Common Command
  SI91X_WLAN_CMD    = 1, ///< SI91X Wireless LAN Command
  SI91X_NETWORK_CMD = 2, ///< SI91X Network Command
  SI91X_SOCKET_CMD  = 3, ///< SI91X Socket Command
  SI91X_BT_CMD      = 4, ///< SI91X Bluetooth Command
  SI91X_CMD_MAX          ///< SI91X Maximum Command value
} sl_si91x_command_type_t;

/// Si91x queue types
typedef enum {
  // SI91X command queue types
  // Note: The values of the command queue type should be
  //       in sync with that of sl_si91x_command_type_t
  SI91X_COMMON_CMD_QUEUE  = 0, ///< SI91X common Command queue
  SI91X_WLAN_CMD_QUEUE    = 1, ///< SI91X Wireless LAN Command queue
  SI91X_NETWORK_CMD_QUEUE = 2, ///< SI91X Network Command queue
  SI91X_SOCKET_CMD_QUEUE  = 3, ///< SI91X Socket Command queue
  SI91X_BT_CMD_QUEUE      = 4, ///< SI91X Bluetooth Command queue

  // SI91X response queue types
  SI91X_COMMON_RESPONSE_QUEUE  = 5, ///< SI91X common Command response queue
  SI91X_WLAN_RESPONSE_QUEUE    = 6, ///< SI91X Wireless LAN Command response queue
  SI91X_NETWORK_RESPONSE_QUEUE = 7, ///< SI91X Network Command response queue
  SI91X_SOCKET_RESPONSE_QUEUE  = 8, ///< SI91X Socket Command response queue
  SI91X_BT_RESPONSE_QUEUE      = 9, ///< SI91X Bluetooth Command response queue

  // All SI91X WLAN Async Events use this queue
  SI91X_WLAN_EVENT_QUEUE = 10, ///< SI91X Wireless LAN Asynchronous response queue
  // All SI91X Network Async Events use this queue
  SI91X_NETWORK_EVENT_QUEUE = 11, ///< SI91X Network Asynchronous response queue

  SI91X_SOCKET_EVENT_QUEUE = 12, ///< SI91X Asynchronous Sockets events queue

  // ALL SOC rx packets use this queue
  CCP_M4_TA_RX_QUEUE = 13, ///< SI91X M4 Receive queue

  SI91X_QUEUE_MAX ///< SI91X Maximum queue type
} sl_si91x_queue_type_t;

typedef uint32_t sl_si91x_semaphore_handle_t;

/// Si91x band mode
typedef enum {
  SL_SI91X_WIFI_BAND_2_4GHZ = 0, ///< 2.4GHz WiFi band
  SL_SI91X_WIFI_BAND_5GHZ   = 1, ///< 5GHz WiFi band
  SL_SI91X_WIFI_DUAL_BAND   = 2  ///< both 2.4GHz and 5GHZ WiFi band
} sl_si91x_band_mode_t;

/// Si91x region code
typedef enum {
  DEFAULT_REGION, ///< Factory default region
  US,             ///< United States
  EU,             ///< European Union
  JP,             ///< Japan
  WORLD_DOMAIN,   ///< World wide domain
  KR,             ///< Korea
  SG              ///< Singapore
} sl_si91x_region_code_t;
/** @} */

/** \addtogroup SL_SI91X_TYPES Types
 * @{
 * */

// TA RSI_COMMON_REQ_OPERMODE command request structure
// Note: refer sl_wifi_device.h for complete bit map details
/// Si91x boot configuration structure
typedef struct {
  uint16_t oper_mode;                  ///< operation mode, one of the values from @ref sl_si91x_operation_mode_t.
  uint16_t coex_mode;                  ///< coex mode, one of the values from @ref sl_si91x_coex_mode_t.
  uint32_t feature_bit_map;            ///< Feature bit map, @ref SI91X_FEATURE_BITMAP
  uint32_t tcp_ip_feature_bit_map;     ///< TCP/IP feature bit map, @ref SI91X_TCP_IP_FEATURE_BITMAP
  uint32_t custom_feature_bit_map;     ///< Custom feature bit map, @ref SI91X_CUSTOM_FEATURE_BITMAP
  uint32_t ext_custom_feature_bit_map; ///< Extended custom feature bit map, @ref SI91X_EXTENDED_CUSTOM_FEATURE_BITMAP
  uint32_t bt_feature_bit_map;         ///< BT featured bit map, @ref SI91X_BT_FEATURE_BITMAP
  uint32_t ext_tcp_ip_feature_bit_map; ///< Extended tcp/ip feature bit map, @ref SI91X_EXTENDED_TCP_IP_FEATURE_BITMAP
  uint32_t ble_feature_bit_map;        ///< BLE feature bitmap, @ref SI91X_BLE_FEATURE_BITMAP
  uint32_t ble_ext_feature_bit_map;    ///< BLE extended feature bit map, @ref SI91X_EXTENDED_BLE_CUSTOM_FEATURE_BITMAP
  uint32_t config_feature_bit_map;     ///< Config feature bit map, @ref SI91X_CONFIG_FEATURE_BITMAP
} sl_si91x_boot_configuration_t;

// driver TX/RX packet structure
/// Si91x packet structure
typedef struct {
  union {
    struct {
      uint16_t length;  ///< Length of data
      uint16_t command; ///< Si91x command type
      uint8_t unused
        [12]; ///< Contains command status and other additional information.Unused for TX and only used for rx packets
    };
    uint8_t desc[16]; ///< Si91x packet header
  };                  ///< Command header

  uint8_t data[]; ///< Data to be transmitted or received
} sl_si91x_packet_t;

/// Si91x queue packet structure
typedef struct {
  sl_wifi_buffer_t *host_packet;        ///< Si91x host buffer
  uint8_t firmware_queue_id;            ///< Si91x firmware queue id
  sl_si91x_command_type_t command_type; ///< Si91x command type
  uint16_t packet_id;                   ///< Packet id, used internally to track packets
  uint8_t flags;                        ///< One of the values from @ref SI91X_RESPONSE_HANDLING_FLAGS
  uint16_t frame_status;                ///< Si91x command status
  void *sdk_context;                    ///< sdk context, unused internally to invoke user callbacks
} sl_si91x_queue_packet_t;

/** @} */
