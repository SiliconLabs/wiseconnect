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
#include "sl_wifi_device.h"
#include <stdint.h>

/* NUMBER_OF_BSD_SOCKETS must be < 32 (sizeof(unsigned) * 8) */
#ifdef __ZEPHYR__
typedef struct sl_si91x_fdset_s {
  unsigned int __fds_bits;
} sl_si91x_fdset_t;
#endif

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
  SI91X_COMMON_CMD      = 0, ///< SI91X Common Command
  SLI_SI91X_WLAN_CMD    = 1, ///< SI91X Wireless LAN Command
  SLI_SI91X_NETWORK_CMD = 2, ///< SI91X Network Command
  SLI_SI91X_BT_CMD      = 3, ///< SI91X Bluetooth Command
  SLI_SI91X_SOCKET_CMD  = 4, ///< SI91X Socket Command
  SI91X_CMD_MAX         = 5  ///< SI91X Maximum Command value
} sli_si91x_command_type_t;

/** \addtogroup SL_SI91X_CONSTANTS 
 * @{
 * */
/// Si91x band mode.
#define SL_SI91X_WIFI_BAND_2_4GHZ SL_WIFI_BAND_MODE_2_4GHZ ///< 2.4 GHz Wi-Fi band
#define SL_SI91X_WIFI_BAND_5GHZ   SL_WIFI_BAND_MODE_5GHZ   ///< 5 GHz Wi-Fi band (not currently supported)
#define SL_SI91X_WIFI_DUAL_BAND   SL_WIFI_DUAL_BAND_MODE ///< Both 2.4 GHz and 5 GHZ WiFi band (not currently supported)

/// Si91x band mode.
/// @note Only 2.4 GHz is currently supported.
typedef sl_wifi_band_mode_t sl_si91x_band_mode_t;

/// Si91x region code.
/// @note Singapore region is not currently supported.
/// @note The suggested mapping for different countries is to be used only for guidance purpose. The customer is expected to verify corresponding country emission requirements and get certification.

/**
 * @details
 * Guidance for Region code Mapping for Different Countries
 * | Country         | Country Code                 |  Max power (Based on Regulatory domain)   | Frequency Range (Based on Regulatory Domain) | Suggested Region Code Mapping |
 * |:----------------|:-----------------------------|:------------------------------------------|:---------------------------------------------|:------------------------------|
 * | Korea           | KR                           | 23 dBm                                    | 2400 - 2483.5                                | SL_WIFI_REGION_KR             |
 * | Hong Kong       | HK                           | 36 dBm                                    | 2400 - 2483.5                                | SL_WIFI_REGION_EU             |
 * | Singapore       | SG                           | 200 mW (23 dBm)                           | 2400 - 2483.5                                | SL_WIFI_REGION_EU             |
 * | Malaysia        | MY                           | 500 mW (27 dBm)                           | 2402 - 2482                                  | SL_WIFI_REGION_EU             |
 * | Australia       | AU                           | 4000 mW (36 dBm)                          | 2400 - 2483.5                                | SL_WIFI_REGION_EU             |
 * | Taiwan          | TW                           | 30 dBm                                    | 2400 - 2483.5                                | SL_WIFI_REGION_EU             |
 * | Thailand        | TH                           | 20 dBm                                    | 2402 - 2482                                  | SL_WIFI_REGION_EU             |
 * | Mexico          | MX                           | 20 dBm                                    | 2402 - 2482                                  | SL_WIFI_REGION_EU             |
 * | Vietnam         | VN                           | 20 dBm                                    | 2402 - 2482                                  | SL_WIFI_REGION_EU             |
 * | Indonesia       | ID                           | 500mW (27 dBm)                            | 2400 - 2483.5                                | SL_WIFI_REGION_EU             |
 * | China           | CN                           | 20 dBm                                    | 2400 - 2483.5                                | CN                            |
 *
 **/
#define DEFAULT_REGION SL_WIFI_DEFAULT_REGION      ///< Factory default region
#define US             SL_WIFI_REGION_US           ///< United States
#define EU             SL_WIFI_REGION_EU           ///< European Union
#define JP             SL_WIFI_REGION_JP           ///< Japan
#define WORLD_DOMAIN   SL_WIFI_REGION_WORLD_DOMAIN ///< World domain
#define KR             SL_WIFI_REGION_KR           ///< Korea
#define SG             SL_WIFI_REGION_SG           ///< Singapore (not currently supported)
#define CN             SL_WIFI_REGION_CN           ///< China
#define IGNORE_REGION  SL_WIFI_IGNORE_REGION       ///< Do not update region code during initialization

/**
 * @typedef sl_si91x_region_code_t
 */
typedef sl_wifi_region_code_t sl_si91x_region_code_t; ///< sl_si91x_region_code_t

/// Si91x Wi-Fi VAP ID
#define SL_SI91X_WIFI_CLIENT_VAP_ID   SL_WIFI_CLIENT_VAP_ID   ///< Wi-Fi Client VAP ID
#define SL_SI91X_WIFI_AP_VAP_ID       SL_WIFI_AP_VAP_ID       ///< Wi-Fi Access point VAP ID
#define SL_SI91X_WIFI_CLIENT_VAP_ID_1 SL_WIFI_CLIENT_VAP_ID_1 ///< Wi-Fi Client 1 VAP ID
#define SL_SI91X_WIFI_AP_VAP_ID_1     SL_WIFI_AP_VAP_ID_1     ///< Wi-Fi Access point 1 VAP ID

/**
 * @typedef sl_si91x_vap_id_t
 */
typedef sl_wifi_vap_id_t sl_si91x_vap_id_t; ///< Si91x Wi-Fi VAP ID

/// Si91x Timeout types
typedef enum {
  SL_SI91X_AUTHENTICATION_ASSOCIATION_TIMEOUT =
    0, ///< Used for setting association and authentication timeout request in milliseconds
  SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT, ///< Used for setting dwell time per channel in milliseconds during active scan
  SL_SI91X_KEEP_ALIVE_TIMEOUT,          ///< Used for setting WLAN keep alive time in seconds
  SL_SI91X_CHANNEL_PASSIVE_SCAN_TIMEOUT ///< Used for setting dwell time per channel in milliseconds during passive scan
} sl_si91x_timeout_type_t;

/// Si91x read status types
typedef enum {
  SL_SI91X_READ_NWP_DEBUG_PORT_STATUS = 9, ///< Read NWP debug port status
  SL_SI91X_READ_MCU_DEBUG_PORT_STATUS = 10 ///< Read MCU debug port status
} sl_si91x_read_status_t;
/** @} */

/** \addtogroup SL_SI91X_TYPES 
 * @{
 * */

/**
  * @typedef sl_si91x_boot_configuration_t
  */
typedef sl_wifi_system_boot_configuration_t sl_si91x_boot_configuration_t; ///< Si91x Boot configuration structure

/**
 * @typedef sl_si91x_timeout_t
 */
typedef sl_wifi_timeout_t sl_si91x_timeout_t; ///< Si91x Timeout configuration structure

/**
 * @typedef sl_si91x_module_state_stats_response_t
 */
typedef sl_wifi_module_state_stats_response_t sl_si91x_module_state_stats_response_t; ///< Si91x module state statistics

/**
 * @typedef sl_si91x_fw_version_info_t
 */
typedef sl_wifi_system_fw_version_info_t sl_si91x_fw_version_info_t; ///< Fireware version information

/**
 * @typedef sl_si91x_fw_version_ext_info_t
 * Firmware version extended information
 */
typedef sl_wifi_fw_version_ext_info_t sl_si91x_fw_version_ext_info_t; ///< Firmware version extended information

/**
 * @typedef sl_si91x_firmware_header_t
 * 
 */
typedef sl_wifi_firmware_header_t sl_si91x_firmware_header_t; ///< Firmware header information for Si91x devices.

/** @} */

#if defined(__Keil)
#pragma anon_unions
#endif

/// driver TX/RX packet structure
typedef sl_wifi_system_packet_t sl_si91x_packet_t;

/// Si91x queue packet structure
typedef struct {
  sl_wifi_buffer_t *host_packet;         ///< Si91x host buffer
  uint8_t firmware_queue_id;             ///< Si91x firmware queue id
  sli_si91x_command_type_t command_type; ///< Si91x command type
  //  uint16_t packet_id;                   ///< Packet id, used internally to track packets
  uint8_t flags;              ///< One of the values from Si91x packet response flags
  uint16_t frame_status;      ///< Si91x command status
  void *sdk_context;          ///< SDK context, unused internally to invoke user callbacks
  uint32_t command_timeout;   ///< Si91x command timeout
  uint32_t command_tickcount; ///< Command_tickcount stores the tickcount when the command is given to the bus thread
} sli_si91x_queue_packet_t;

/// Si91x specific buffer queue structure
typedef struct {
  sl_wifi_buffer_t *head; ///< Head
  sl_wifi_buffer_t *tail; ///< Tail
} sli_si91x_buffer_queue_t;

/// NWP Buffer allocation command parameters
/// The summation of all three ratios should max 10 and the ratio should be in decimal value.
typedef sl_wifi_system_dynamic_pool_t sl_si91x_dynamic_pool;

/// Structure to represent a command queue
typedef struct {
  sli_si91x_buffer_queue_t tx_queue;    ///< TX queue
  sli_si91x_buffer_queue_t rx_queue;    ///< RX queue
  sli_si91x_buffer_queue_t event_queue; ///< Event queue
  void *mutex;                          ///< Pointer to mutex
  uint32_t flag;                        ///< Flags
  bool sequential;                      ///< Indicates if the commands are sequential
  bool command_in_flight;               ///< Indicates if a command is currently being processed
  uint16_t frame_type;                  ///< Type of the frame associated with the command
  uint8_t firmware_queue_id;            ///< ID of the firmware queue for the command
  uint32_t rx_counter;                  ///< Counter for received packets
  uint32_t tx_counter;                  ///< Counter for transmitted packets
  uint16_t packet_id;                   ///< ID of the packet associated with the command
  uint8_t flags;                        ///< Flags associated with the command
  uint32_t command_tickcount;           ///< Command tick count
  uint32_t command_timeout;             ///< Command timeout
  void *sdk_context;                    ///< Context data associated with the command
  bool is_queue_initialiazed;           ///< indicates queue is initialiazed or not.
} sli_si91x_command_queue_t;
