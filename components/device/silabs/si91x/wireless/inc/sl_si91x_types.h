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

/// Flag to indicate that the response status of the command is expected
#define SI91X_PACKET_RESPONSE_STATUS (1 << 0)

/// Flag to indicate that the response packet of the command is expected
#define SI91X_PACKET_RESPONSE_PACKET (1 << 1)

/// Flag to indicate that all the packet tx has to be suspended until the corresponding command response is received
#define SI91X_PACKET_GLOBAL_QUEUE_BLOCK (1 << 3)

/// Flag to indicate that host would receive the response from firmware in asynchronous manner.
#define SI91X_PACKET_WITH_ASYNC_RESPONSE (1 << 4)

typedef enum {
  SI91X_COMMON_CMD  = 0, ///< SI91X Common Command
  SI91X_WLAN_CMD    = 1, ///< SI91X Wireless LAN Command
  SI91X_NETWORK_CMD = 2, ///< SI91X Network Command
  SI91X_SOCKET_CMD  = 3, ///< SI91X Socket Command
  SI91X_BT_CMD      = 4, ///< SI91X Bluetooth Command
  SI91X_SOCKET_DATA = 5, ///< SI91X Socket Data Command
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
  SI91X_SOCKET_DATA_QUEUE = 5, ///< SI91X Socket Command queue

  // SI91X response queue types
  SI91X_COMMON_RESPONSE_QUEUE  = 6,  ///< SI91X common Command response queue
  SI91X_WLAN_RESPONSE_QUEUE    = 7,  ///< SI91X Wireless LAN Command response queue
  SI91X_NETWORK_RESPONSE_QUEUE = 8,  ///< SI91X Network Command response queue
  SI91X_SOCKET_RESPONSE_QUEUE  = 9,  ///< SI91X Socket Command response queue
  SI91X_BT_RESPONSE_QUEUE      = 10, ///< SI91X Bluetooth Command response queue

  // All SI91X WLAN Async Events use this queue
  SI91X_WLAN_EVENT_QUEUE = 11, ///< SI91X Wireless LAN Asynchronous response queue
  // All SI91X Network Async Events use this queue
  SI91X_NETWORK_EVENT_QUEUE = 12, ///< SI91X Network Asynchronous response queue

  SI91X_SOCKET_EVENT_QUEUE = 13, ///< SI91X Asynchronous Sockets events queue

  // ALL SOC rx packets use this queue
  CCP_M4_TA_RX_QUEUE = 14, ///< SI91X M4 Receive queue

  SI91X_QUEUE_MAX ///< SI91X Maximum queue type
} sl_si91x_queue_type_t;

/** \addtogroup SL_SI91X_TYPES 
 * @{
 * */
/// Si91x command types
/// Si91x band mode.
/// @note Only 2.4 GHz currently supported.
typedef enum {
  SL_SI91X_WIFI_BAND_2_4GHZ = 0, ///< 2.4GHz WiFi band
  SL_SI91X_WIFI_BAND_5GHZ   = 1, ///< 5GHz WiFi band (not currently supported)
  SL_SI91X_WIFI_DUAL_BAND   = 2  ///< both 2.4GHz and 5GHZ WiFi band (not currently supported)
} sl_si91x_band_mode_t;

/// Si91x region code.
/// @note Singapore region not currently supported.
typedef enum {
  DEFAULT_REGION, ///< Factory default region
  US,             ///< United States
  EU,             ///< European Union
  JP,             ///< Japan
  WORLD_DOMAIN,   ///< World wide domain
  KR,             ///< Korea
  SG              ///< Singapore (not currently supported)
} sl_si91x_region_code_t;

/// Si91x Timeout types
typedef enum {
  SL_SI91X_AUTHENTICATION_ASSOCIATION_TIMEOUT =
    0, ///< Used for setting association and authentication timeout request in millisecs
  SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT, ///< Used for setting dwell time per channel in milli seconds during active scan
  SL_SI91X_KEEP_ALIVE_TIMEOUT,          ///< Used for setting WLAN keep alive time in seconds
  SL_SI91X_CHANNEL_PASSIVE_SCAN_TIMEOUT ///< Used for setting dwell time per channel in milli seconds during passive scan
} sl_si91x_timeout_type_t;

/// Si91x Wi-Fi VAP ID
typedef enum {
  SL_SI91X_WIFI_CLIENT_VAP_ID, ///< Wi-Fi Client VAP ID
  SL_SI91X_WIFI_AP_VAP_ID,     ///< Wi-Fi Access point VAP ID
} sl_si91x_wifi_vap_id_t;

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

/// Timeout Configuration Structure
typedef struct {
  uint16_t
    active_chan_scan_timeout_value; ///< Time spent on each channel when performing active scan (milliseconds). Default value of 100 millisecs is used when SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME is passed.
  uint16_t
    auth_assoc_timeout_value; ///< Authentication and association timeout value. Default value of 300 millisecs is used when SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT is passed.
  uint16_t
    keep_alive_timeout_value; ///< Keep Alive Timeout value. Default value of 30 secs is used when SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT is passed
  uint16_t
    passive_scan_timeout_value; ///<Time spent on each channel when performing passive scan (milliseconds). The minimum passive_scan_timeout_value is 5 millisecs and maximum is 1000 millisecs. Default value of 400 millisecs is used when SL_WIFI_DEFAULT_PASSIVE_CHANNEL_SCAN_TIME is passed.
} sl_si91x_timeout_t;

/// Si917 specific Wi-Fi module state statistics
#pragma pack(1)
typedef struct {
  uint32_t timestamp;  ///< Timestamp
  uint8_t state_code;  ///< State code
  uint8_t reason_code; ///< Reason code
  uint8_t channel;     ///< Channel number
  uint8_t rssi;        ///< RSSI VALUE
  uint8_t bssid[6];    ///< BSSID
} sl_si91x_module_state_stats_response_t;
#pragma pack()

/// Firmware version information
typedef struct {
  uint8_t build_num;        ///< Build number
  uint8_t security_version; ///< Security enabled or disabled
  uint8_t minor;            ///< Minor version number
  uint8_t major;            ///< Major version number
} sl_si91x_fw_version_info_t;

/// Firmware version extended information
typedef struct {
  uint8_t patch_num;                  ///< Patch number
  uint8_t customer_id : 4;            ///< Customer ID
  uint8_t build_number_extension : 4; ///< Build number extension
  uint8_t rom_id;                     ///< ROM ID
  uint8_t chip_id;                    ///< Chip ID
} sl_si91x_fw_version_ext_info_t;

/// Firmware header information
typedef struct {
  uint16_t control_flags;                             ///< Control flags
  uint16_t sha_type;                                  ///< SHA type
  uint32_t magic_no;                                  ///< Firmware image magic byte
  uint32_t image_size;                                ///< Size of firmware image
  sl_si91x_fw_version_info_t fw_version_info;         ///< Firmware version information
  uint32_t flash_location;                            ///< Address location in flash to store the image
  uint32_t crc;                                       ///< CRC of the image
  uint32_t mic[4];                                    ///< MIC of the image
  uint32_t reserved;                                  ///< Reserved
  sl_si91x_fw_version_ext_info_t fw_version_ext_info; ///< Firmware version extended information
  uint32_t reserved1[4];                              ///< Reserved
} sl_si91x_firmware_header_t;

/** @} */

#if defined(__Keil)
#pragma anon_unions
#endif

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
  uint8_t flags;                        ///< One of the values from SI91X packet response flags
  uint16_t frame_status;                ///< Si91x command status
  void *sdk_context;                    ///< sdk context, unused internally to invoke user callbacks
  int32_t
    sl_si91x_socket_id; ///< socket_id, used only for SI91X_SOCKET_CMD queue to update socket_id in command trace of bus thread.
} sl_si91x_queue_packet_t;

typedef struct {
  sl_wifi_buffer_t *head;
  sl_wifi_buffer_t *tail;
} sl_si91x_buffer_queue_t;

/// TA buffer allocation command parameters
/// The summation of the all three ratios should max 10 and the ratio should be in decimal value
typedef struct {
  uint8_t tx_ratio_in_buffer_pool;     ///< tx ratio
  uint8_t rx_ratio_in_buffer_pool;     ///< rx ratio
  uint8_t global_ratio_in_buffer_pool; ///< global ratio
} sl_si91x_dynamic_pool;
