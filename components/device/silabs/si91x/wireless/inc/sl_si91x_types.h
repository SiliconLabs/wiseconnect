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

/// Flag to indicate that the response status of the command is expected.
#define SI91X_PACKET_RESPONSE_STATUS (1 << 0)

/// Flag to indicate that the response packet of the command is expected.
#define SI91X_PACKET_RESPONSE_PACKET (1 << 1)

/// Flag to indicate that all the packet tx has to be suspended until the corresponding command response is received.
#define SI91X_PACKET_GLOBAL_QUEUE_BLOCK (1 << 3)

/// Flag to indicate that host would receive the response from firmware in asynchronous manner.
#define SI91X_PACKET_WITH_ASYNC_RESPONSE (1 << 4)

/// Si91x specific command type
typedef enum {
  SI91X_COMMON_CMD  = 0, ///< SI91X Common Command
  SI91X_WLAN_CMD    = 1, ///< SI91X Wireless LAN Command
  SI91X_NETWORK_CMD = 2, ///< SI91X Network Command
  SI91X_BT_CMD      = 3, ///< SI91X Bluetooth Command
  SI91X_SOCKET_CMD  = 4, ///< SI91X Socket Command
  SI91X_CMD_MAX,         ///< SI91X Maximum Command value
} sl_si91x_command_type_t;

/** \addtogroup SL_SI91X_CONSTANTS 
 * @{
 * */
/// Si91x band mode.
/// @note Only 2.4 GHz is currently supported.
typedef enum {
  SL_SI91X_WIFI_BAND_2_4GHZ = 0, ///< 2.4 GHz Wi-Fi band
  SL_SI91X_WIFI_BAND_5GHZ   = 1, ///< 5 GHz Wi-Fi band (not currently supported)
  SL_SI91X_WIFI_DUAL_BAND   = 2  ///< Both 2.4 GHz and 5 GHZ WiFi band (not currently supported)
} sl_si91x_band_mode_t;

/// Si91x region code.
/// @note Singapore region not currently supported.
typedef enum {
  DEFAULT_REGION, ///< Factory default region
  US,             ///< United States
  EU,             ///< European Union
  JP,             ///< Japan
  WORLD_DOMAIN,   ///< Worldwide domain
  KR,             ///< Korea
  SG,             ///< Singapore (not currently supported)
  CN,             ///< China
  IGNORE_REGION   ///< Do not update region code during initialization
} sl_si91x_region_code_t;

/// Si91x Timeout types
typedef enum {
  SL_SI91X_AUTHENTICATION_ASSOCIATION_TIMEOUT =
    0, ///< Used for setting association and authentication timeout request in milliseconds
  SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT, ///< Used for setting dwell time per channel in milliseconds during active scan
  SL_SI91X_KEEP_ALIVE_TIMEOUT,          ///< Used for setting WLAN keep alive time in seconds
  SL_SI91X_CHANNEL_PASSIVE_SCAN_TIMEOUT ///< Used for setting dwell time per channel in milliseconds during passive scan
} sl_si91x_timeout_type_t;

/// Si91x Wi-Fi VAP ID
typedef enum {
  SL_SI91X_WIFI_CLIENT_VAP_ID, ///< Wi-Fi Client VAP ID
  SL_SI91X_WIFI_AP_VAP_ID,     ///< Wi-Fi Access Point VAP ID
} sl_si91x_wifi_vap_id_t;
/** @} */

/** \addtogroup SL_SI91X_TYPES 
 * @{
 * */
// NWP RSI_COMMON_REQ_OPERMODE command request structure
// Note: refer sl_wifi_device.h for complete bit map details
/// Si91x boot configuration structure
typedef struct {
  uint16_t oper_mode;                  ///< Operation mode, one of the values from @ref sl_si91x_operation_mode_t.
  uint16_t coex_mode;                  ///< Coexistence mode, one of the values from @ref sl_si91x_coex_mode_t.
  uint32_t feature_bit_map;            ///< Feature bit map, @ref SI91X_FEATURE_BITMAP
  uint32_t tcp_ip_feature_bit_map;     ///< TCP/IP feature bit map, @ref SI91X_TCP_IP_FEATURE_BITMAP
  uint32_t custom_feature_bit_map;     ///< Custom feature bit map, @ref SI91X_CUSTOM_FEATURE_BITMAP
  uint32_t ext_custom_feature_bit_map; ///< Extended custom feature bit map, @ref SI91X_EXTENDED_CUSTOM_FEATURE_BITMAP
  uint32_t bt_feature_bit_map;         ///< BT featured bit map, @ref SI91X_BT_FEATURE_BITMAP
  uint32_t ext_tcp_ip_feature_bit_map; ///< Extended TCP/IP feature bit map, @ref SI91X_EXTENDED_TCP_IP_FEATURE_BITMAP
  uint32_t ble_feature_bit_map;        ///< BLE feature bitmap, @ref SI91X_BLE_FEATURE_BITMAP
  uint32_t ble_ext_feature_bit_map;    ///< BLE extended feature bitmap, @ref SI91X_EXTENDED_BLE_CUSTOM_FEATURE_BITMAP
  uint32_t config_feature_bit_map;     ///< Config feature bitmap, @ref SI91X_CONFIG_FEATURE_BITMAP
} sl_si91x_boot_configuration_t;

/// Timeout Configuration Structure
typedef struct {
  uint16_t
    active_chan_scan_timeout_value; ///< Time spent on each channel when performing active scan (milliseconds). Default value of 100 millisecs is used when SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME is passed.
  uint16_t
    auth_assoc_timeout_value; ///< Authentication and association timeout value. Default value of 300 millisecs is used when SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT is passed.
  uint16_t
    keep_alive_timeout_value; ///< Keep Alive Timeout value. Default value of 30 seconds is used when SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT is passed.
  uint16_t
    passive_scan_timeout_value; ///< Time spent on each channel when performing passive scan (milliseconds). The minimum passive_scan_timeout_value is 5 millisecs, and maximum is 1000 milliseconds. Default value of 400 milliseconds is used when SL_WIFI_DEFAULT_PASSIVE_CHANNEL_SCAN_TIME is passed.
} sl_si91x_timeout_t;

/// Si917 specific Wi-Fi module state statistics
#pragma pack(1)
typedef struct {
  uint32_t
    timestamp; ///< Timestamp. This is value of counter at the time of message. This counter is continuously incremented by one per 100ms time.
  uint8_t
    state_code; ///< State code. This field indicates state of the module. state code contain two parts (upper and lower nibbles). Upper nibble represent the state of rejoin process and StateCode represented by the lower nibble of state code.
  uint8_t reason_code; ///< Reason code. This is used to get the reason code from firmware point of view.
  uint8_t
    channel; ///< Channel number. If value of channel is 0, it means channel information is not available. In State-I, channel of association or Invalid if it is startup. In State-II, channel of next association if module finds better AP in bgscan result. In State-III, Channel at the time of association.
  uint8_t
    rssi; ///< RSSI VALUE. If value of rssi is 100, it means RSSI information is not available. In State-I it is RSSI of AP at the time of trigger. In State-II it is RSSI of next association. In State-III it is RSSI at the time of final association.
  uint8_t bssid
    [6]; ///< BSSID of AP. If the value of AP BSSID is 00:00:00:00:00:00,it means MAC information is not available. In State-I it is MAC of AP at the time of scan trigger. In State-II it is MAC of next association. In State-III it is MAC at the time of association.
} sl_si91x_module_state_stats_response_t;
#pragma pack()

/// Firmware version information
typedef struct {
  uint8_t build_num;        ///< Build number of the firmware
  uint8_t security_version; ///< Security version indicating if security is enabled or disabled
  uint8_t minor;            ///< Minor version number of the firmware
  uint8_t major;            ///< Major version number of the firmware
} sl_si91x_fw_version_info_t;

/// Firmware version extended information
typedef struct {
  uint8_t patch_num;                  ///< Patch number of the firmware
  uint8_t customer_id : 4;            ///< Customer ID
  uint8_t build_number_extension : 4; ///< Build number extension
  uint8_t rom_id;                     ///< ROM ID of the firmware
  uint8_t chip_id;                    ///< Chip ID of the device
} sl_si91x_fw_version_ext_info_t;

/// Firmware header information
typedef struct {
  uint16_t control_flags;                     ///< Control flags for the firmware image.
  uint16_t sha_type;                          ///< SHA type used for the firmware image.
  uint32_t magic_no;                          ///< Magic number identifying the firmware image.
  uint32_t image_size;                        ///< Size of the firmware image in bytes.
  sl_si91x_fw_version_info_t fw_version_info; ///< Firmware version information.
  uint32_t flash_location;                    ///< Address location in flash memory where the firmware image is stored.
  uint32_t crc;                               ///< Cyclic Redundancy Check (CRC) value of the firmware image.
  uint32_t mic[4];                            ///< Message Integrity Code (MIC) of the firmware image.
  uint32_t reserved;                          ///< Reserved fields for future use.
  sl_si91x_fw_version_ext_info_t fw_version_ext_info; ///< Firmware version extended information.
  uint32_t reserved1[4];                              ///< Reserved fields for future use.
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
        [12]; ///< Contains command status and other additional information. Unused for TX and only used for rx packets.
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
  //  uint16_t packet_id;                   ///< Packet id, used internally to track packets
  uint8_t flags;              ///< One of the values from Si91x packet response flags
  uint16_t frame_status;      ///< Si91x command status
  void *sdk_context;          ///< SDK context, unused internally to invoke user callbacks
  uint32_t command_timeout;   ///< Si91x command timeout
  uint32_t command_tickcount; ///< command_tickcount stores the tickcount when the command is given to the bus thread.
} sli_si91x_queue_packet_t;

/// Si91x specific buffer queue structure
typedef struct {
  sl_wifi_buffer_t *head; ///< Head
  sl_wifi_buffer_t *tail; ///< Tail
} sl_si91x_buffer_queue_t;

/// NWP buffer allocation command parameters
/// The summation of all three ratios should max 10 and the ratio should be in decimal value.
typedef struct {
  uint8_t tx_ratio_in_buffer_pool;     ///< tx ratio
  uint8_t rx_ratio_in_buffer_pool;     ///< rx ratio
  uint8_t global_ratio_in_buffer_pool; ///< global ratio
} sl_si91x_dynamic_pool;

/// Structure to represent a command queue
typedef struct {
  sl_si91x_buffer_queue_t tx_queue;    ///< TX queue
  sl_si91x_buffer_queue_t rx_queue;    ///< RX queue
  sl_si91x_buffer_queue_t event_queue; ///< Event queue
  void *mutex;                         ///< Pointer to mutex
  uint32_t flag;                       ///< Flags
  bool sequential;                     ///< Indicates if the commands are sequential
  bool command_in_flight;              ///< Indicates if a command is currently being processed
  uint16_t frame_type;                 ///< Type of the frame associated with the command
  uint8_t firmware_queue_id;           ///< ID of the firmware queue for the command
  uint32_t rx_counter;                 ///< Counter for received packets
  uint32_t tx_counter;                 ///< Counter for transmitted packets
  uint16_t packet_id;                  ///< ID of the packet associated with the command
  uint8_t flags;                       ///< Flags associated with the command
  uint32_t command_tickcount;          ///< Command tick count
  uint32_t command_timeout;            ///< Command timeout
  void *sdk_context;                   ///< Context data associated with the command
} sli_si91x_command_queue_t;