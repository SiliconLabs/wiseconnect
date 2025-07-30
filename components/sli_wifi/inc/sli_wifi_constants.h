/***************************************************************************/ /**
 * @file    sli_wifi_constants.h
 * @brief
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
#ifndef SLI_WIFI_CONSTANTS_H
#define SLI_WIFI_CONSTANTS_H
/// Timeout value for Wi-Fi join command
#define SLI_WIFI_CONNECT_TIMEOUT 120000 // in ms, i.e., 120s

typedef enum { SLI_WIFI_NO_ENCRYPTION, SLI_WIFI_TKIP_ENCRYPTION, SLI_WIFI_CCMP_ENCRYPTION } sli_wifi_encryption_t;
typedef enum {
  SLI_WIFI_REQ_INIT                         = 0x12, ///< Wi-Fi Initialization
  SLI_WIFI_REQ_OPERMODE                     = 0x10, ///< Wi-Fi Operation Mode
  SLI_WIFI_REQ_BAND                         = 0x11, ///< Wi-Fi Band
  SLI_WIFI_REQ_SCAN                         = 0x13, ///< Wi-Fi Scan
  SLI_WIFI_REQ_JOIN                         = 0x14, ///< Wi-Fi Join
  SLI_WIFI_REQ_PWRMODE                      = 0x15, ///< Power Mode
  SLI_WIFI_REQ_MAC_ADDRESS                  = 0x4A, ///< Set MAC Address
  SLI_WIFI_REQ_DISCONNECT                   = 0x19, ///< Wi-Fi Disconnect
  SLI_WIFI_REQ_AP_STOP                      = 0xAE, ///< Stop Access Point
  SLI_WIFI_REQ_QUERY_NETWORK_PARAMS         = 0x18, ///< Query Network Parameters
  SLI_WIFI_REQ_ANTENNA_SELECT               = 0x1B, ///< Antenna Selection
  SLI_WIFI_REQ_HT_CAPABILITIES              = 0x6D, ///< HT Capabilities
  SLI_WIFI_REQ_BG_SCAN                      = 0x6A, ///< Background Scan
  SLI_WIFI_REQ_EAP_CONFIG                   = 0x4C, ///< EAP Configuration
  SLI_WIFI_REQ_HOST_PSK                     = 0xA5, ///< Host PSK
  SLI_WIFI_REQ_AP_CONFIGURATION             = 0x24, ///< Access Point Configuration
  SLI_WIFI_REQ_BEACON_STOP                  = 0x63, ///< Stop Beacon
  SLI_WIFI_REQ_REJOIN_PARAMS                = 0x6F, ///< Rejoin Parameters
  SLI_WIFI_REQ_RSSI                         = 0x3A, ///< RSSI
  SLI_WIFI_REQ_SET_MAC_ADDRESS              = 0x17, ///< Set MAC Address
  SLI_WIFI_REQ_QUERY_GO_PARAMS              = 0x4E, ///< Query GO Parameters
  SLI_WIFI_REQ_EXT_STATS                    = 0x68, ///< Extended Statistics  // Neither part 22q2 nor alpha 2
  SLI_WIFI_REQ_GET_STATS                    = 0xF1, ///< Get Statistics
  SLI_WIFI_REQ_RX_STATS                     = 0xA2, ///< RX Statistics
  SLI_WIFI_REQ_ROAM_PARAMS                  = 0x7B, ///< Roam Parameters
  SLI_WIFI_REQ_TWT_AUTO_CONFIG              = 0x2E, ///< TWT Auto Configuration
  SLI_WIFI_REQ_FILTER_BCAST_PACKETS         = 0xC9, ///< Filter Broadcast Packets
  SLI_WIFI_REQ_WPS_METHOD                   = 0x72, ///< WPS Method
  SLI_WIFI_REQ_GAIN_TABLE                   = 0x47, ///< Gain Table
  SLI_WIFI_REQ_11AX_PARAMS                  = 0xFF, ///< 11AX Parameters
  SLI_WIFI_REQ_TWT_PARAMS                   = 0x2F, ///< TWT Parameters
  SLI_WIFI_REQ_RESCHEDULE_TWT               = 0x3F, ///< Reschedule TWT
  SLI_WIFI_REQ_SET_TRANSCEIVER_CHANNEL      = 0x7A, ///< Set Transceiver Channel
  SLI_WIFI_REQ_TRANSCEIVER_CONFIG_PARAMS    = 0x8C, ///< Transceiver Configuration Parameters
  SLI_WIFI_REQ_TRANSCEIVER_PEER_LIST_UPDATE = 0x8B, ///< Update Transceiver Peer List
  SLI_WIFI_REQ_SET_TRANSCEIVER_MCAST_FILTER = 0x8D, ///< Set Transceiver Multicast Filter
  SLI_WIFI_REQ_TRANSCEIVER_FLUSH_DATA_Q     = 0x8E, ///< Flush Transceiver Data Queue
  SLI_WIFI_REQ_SET_MULTICAST_FILTER         = 0x40, ///< Set Multicast Filter
  SLI_WIFI_REQ_CARDREADY                    = 0x89, ///< Card Ready
  SLI_WLAN_REQ_SCAN_RESULTS                 = 0xAF, ///< Scan Results
  SLI_WIFI_REQ_TSF                          = 0x65  ///< TSF
} sli_wifi_request_commands_t;

typedef enum {
  SLI_WIFI_RSP_OPERMODE                     = 0x10, ///< Wi-Fi Operation Mode
  SLI_WIFI_RSP_INIT                         = 0x12, ///< Wi-Fi Initialization
  SLI_WIFI_RSP_BAND                         = 0x11, ///< Wi-Fi
  SLI_WIFI_RSP_SCAN                         = 0x13, ///< Wi-Fi Scan
  SLI_WIFI_RSP_JOIN                         = 0x14, ///< Wi-Fi Join
  SLI_WIFI_RSP_PWRMODE                      = 0x15, ///< Power Mode
  SLI_WIFI_RSP_MAC_ADDRESS                  = 0x4A, ///< Set MAC Address
  SLI_WIFI_RSP_DISCONNECT                   = 0x19, ///< Wi-Fi Disconnect
  SLI_WIFI_RSP_AP_STOP                      = 0xAE, ///< Stop Access Point
  SLI_WIFI_RSP_QUERY_NETWORK_PARAMS         = 0x18, ///< Query Network Parameters
  SLI_WIFI_RSP_ANTENNA_SELECT               = 0x1B, ///< Antenna Selection
  SLI_WIFI_RSP_HT_CAPABILITIES              = 0x6D, ///< HT Capabilities
  SLI_WIFI_RSP_BG_SCAN                      = 0x6A, ///< Background Scan
  SLI_WIFI_RSP_EAP_CONFIG                   = 0x4C, ///< EAP Configuration
  SLI_WIFI_RSP_HOST_PSK                     = 0xA5, ///< Host PSK
  SLI_WIFI_RSP_AP_CONFIGURATION             = 0x24, ///< Access Point Configuration
  SLI_WIFI_RSP_BEACON_STOP                  = 0x63, ///< Stop Beacon
  SLI_WIFI_RSP_REJOIN_PARAMS                = 0x6F, ///< Rejoin Parameters
  SLI_WIFI_RSP_RSSI                         = 0x3A, ///< RSSI
  SLI_WIFI_RSP_SET_MAC_ADDRESS              = 0x17, ///< Set MAC Address
  SLI_WIFI_RSP_QUERY_GO_PARAMS              = 0x4E, ///< Query GO Parameters
  SLI_WIFI_RSP_EXT_STATS                    = 0x68, ///< Extended Statistics  // Neither part 22q2 nor alpha 2
  SLI_WIFI_RSP_GET_STATS                    = 0xF1, ///< Get Statistics
  SLI_WIFI_RSP_RX_STATS                     = 0xA2, ///< RX Statistics
  SLI_WIFI_RSP_ROAM_PARAMS                  = 0x7B, ///< Roam Parameters
  SLI_WIFI_RSP_TWT_AUTO_CONFIG              = 0x2E, ///< TWT Auto Configuration
  SLI_WIFI_RSP_FILTER_BCAST_PACKETS         = 0xC9, ///< Filter Broadcast Packets
  SLI_WIFI_RSP_WPS_METHOD                   = 0x72, ///< WPS Method
  SLI_WIFI_RSP_GAIN_TABLE                   = 0x47, ///< Gain Table
  SLI_WIFI_RSP_11AX_PARAMS                  = 0xFF, ///< 11AX Parameters
  SLI_WIFI_RSP_TWT_PARAMS                   = 0x2F, ///< TWT Parameters
  SLI_WIFI_RSP_RESCHEDULE_TWT               = 0x3F, ///< Reschedule TWT
  SLI_WIFI_RSP_TRANSCEIVER_SET_CHANNEL      = 0x7A, ///< Set Transceiver Channel
  SLI_WIFI_RSP_TRANSCEIVER_CONFIG_PARAMS    = 0x8C, ///< Transceiver Configuration Parameters
  SLI_WIFI_RSP_TRANSCEIVER_PEER_LIST_UPDATE = 0x8B, ///< Update Transceiver Peer List
  SLI_WIFI_RSP_SET_TRANSCEIVER_MCAST_FILTER = 0x8D, ///< Set Transceiver Multicast Filter
  SLI_WIFI_RSP_TRANSCEIVER_FLUSH_DATA_Q     = 0x8E, ///< Flush Transceiver Data Queue
  SLI_WIFI_RSP_SET_MULTICAST_FILTER         = 0x40, ///< Set Multicast Filter
  SLI_WIFI_RSP_CARDREADY                    = 0x89, ///< Card Ready
  SLI_WIFI_RSP_SCAN_RESULTS                 = 0xAF, ///< Scan Results
  SLI_WIFI_RSP_TSF                          = 0x65  ///< TSF
} sli_wifi_response_commands_t;
#endif

/// Timeout scaling factor for internal firmware operations
#ifndef SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF
#define SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF 1
#endif

/// Timeout scaling factor for over the air operations
#ifndef SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF
#define SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF 1
#endif

/// Timeout scaling factor for network operations
#ifndef SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF
#define SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF 1
#endif

/// Base timeout value for internal operations
#define SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE 1000

/// Base timeout value for Wi-Fi management operations
#define SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE 5000

/// Additional wait time(in ms) for command timeout calculations
#ifndef SL_TX_ADDITIONAL_WAIT_TIME
#define SL_TX_ADDITIONAL_WAIT_TIME 0
#endif

/// Default timeout value for commands
#define SLI_DEFAULT_TIMEOUT (30000 + SL_TX_ADDITIONAL_WAIT_TIME)

/// Internal commands timeout defines
/// Timeout value for waiting on operation mode response command
#define SLI_WIFI_RSP_OPERMODE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for waiting on band response command
#define SLI_WIFI_RSP_BAND_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for MAC address response command
#define SLI_WIFI_RSP_MAC_ADDRESS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for disconnect response command
#define SLI_WLAN_RSP_DISCONNECT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for rx stats response command
#define SLI_WIFI_RSP_RX_STATS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Antenna Select response command
#define SLI_WIFI_RSP_ANTENNA_SELECT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Debug Log response command
#define SLI_COMMON_RSP_DEBUG_LOG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Get RAM Dump response command
#define SLI_COMMON_RSP_GET_RAM_DUMP_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for waiting on feature frame response command
#define SLI_COMMON_RSP_FEATURE_FRAME_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Power Mode response command
#define SLI_WIFI_RSP_PWRMODE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF Start response command
#define SLI_COMMON_RSP_PUF_START_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF set key response command
#define SLI_COMMON_RSP_PUF_SET_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF get key response command
#define SLI_COMMON_RSP_PUF_GET_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF Dis set key response command
#define SLI_COMMON_RSP_PUF_DIS_SET_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF Dis get key response command
#define SLI_COMMON_RSP_PUF_DIS_GET_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF Load key response command
#define SLI_COMMON_RSP_PUF_LOAD_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for AES Encrypt response command
#define SLI_COMMON_RSP_AES_ENCRYPT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for AES Decrypt response command
#define SLI_COMMON_RSP_AES_DECRYPT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for AES MAC response command
#define SLI_COMMON_RSP_AES_MAC_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for PUF Intr key response command
#define SLI_COMMON_RSP_PUF_INTR_KEY_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Gain Table response command
#define SLI_WIFI_RSP_GAIN_TABLE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for EAP Config response command
#define SLI_WIFI_RSP_EAP_CONFIG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Set Region AP response command
#define SLI_WLAN_RSP_SET_REGION_AP_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Set Region response command
#define SLI_WLAN_RSP_SET_REGION_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Dynamic Pool response command
#define SLI_WLAN_RSP_DYNAMIC_POOL_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for set multicast filter response command
#define SLI_WIFI_RSP_SET_MULTICAST_FILTER_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Bcast Packets response command
#define SLI_WIFI_RSP_FILTER_BCAST_PACKETS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for initialize response command
#define SLI_WIFI_RSP_INIT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Set configuration response command
#define SLI_COMMON_RSP_SET_CONFIG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Get configuration response command
#define SLI_COMMON_RSP_GET_CONFIG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for configuration response command
#define SLI_WLAN_RSP_CONFIG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Query Network Parameters response command
#define SLI_WIFI_RSP_QUERY_NETWORK_PARAMS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for set Certificate response command
#define SLI_WLAN_RSP_SET_CERTIFICATE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for rejoin parameters response command
#define SLI_WIFI_RSP_REJOIN_PARAMS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for get stats response command
#define SLI_WIFI_RSP_GET_STATS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for ext stats response command
#define SLI_WIFI_RSP_EXT_STATS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for set rtc timer response command
#define SLI_COMMON_RSP_SET_RTC_TIMER_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for get rtc timer response command
#define SLI_COMMON_RSP_GET_RTC_TIMER_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for assert response command
#define SLI_COMMON_RSP_ASSERT_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for radio response command
#define SLI_WLAN_RSP_RADIO_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for get EFUSE data response command
#define SLI_COMMON_RSP_GET_EFUSE_DATA_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for EFUSE read response command
#define SLI_WLAN_RSP_EFUSE_READ_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for HT Capabilities response command
#define SLI_WIFI_RSP_HT_CAPABILITIES_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for AP Stop response command
#define SLI_WIFI_RSP_AP_STOP_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for encrypt crypto response command
#define SLI_COMMON_RSP_ENCRYPT_CRYPTO_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for TA M4 commands response command
#define SLI_COMMON_RSP_TA_M4_COMMANDS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for set transceiver multicast filter response command
#define SLI_WIFI_RSP_SET_TRANSCEIVER_MCAST_FILTER_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for roam parameters response command
#define SLI_WIFI_RSP_ROAM_PARAMS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for socket configuration response command
#define SLI_WLAN_RSP_SOCKET_CONFIG_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for TX test mode response command
#define SLI_WLAN_RSP_TX_TEST_MODE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for transceiver flush data queue response command
#define SLI_WIFI_RSP_TRANSCEIVER_FLUSH_DATA_Q_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for transceiver peer list update response command
#define SLI_WIFI_RSP_TRANSCEIVER_PEER_LIST_UPDATE_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for transceiver configuration parameters response command
#define SLI_WIFI_RSP_TRANSCEIVER_CONFIG_PARAMS_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for transceiver set channel response command
#define SLI_WIFI_RSP_SET_TRANSCEIVER_CHANNEL_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value to get Full Firmware Version response command
#define SLI_WLAN_RSP_FULL_FW_VERSION_WAIT_TIME \
  ((SLI_WIFI_INTERNAL_COMMANDS_BASE_VALUE * SL_WIFI_INTERNAL_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))

/// Wi-Fi commands wait time out defines
/// Timeout value for multicast response command
#define SLI_WLAN_RSP_MULTICAST_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Beacon Stop response command
#define SLI_WIFI_RSP_BEACON_STOP_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for mDNS service discovery response command
#define SLI_WLAN_RSP_MDNSD_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for frequency offset response command
#define SLI_WLAN_RSP_FREQ_OFFSET_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for EVM offset response command
#define SLI_RSP_EVM_OFFSET_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for EVM write response command
#define SLI_WLAN_RSP_EVM_WRITE_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for calibration write response command
#define SLI_WLAN_RSP_CALIB_WRITE_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for get DPD data response command
#define SLI_WLAN_RSP_GET_DPD_DATA_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for calibration read response command
#define SLI_WLAN_RSP_CALIB_READ_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for join response command
#define SLI_WIFI_RSP_JOIN_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for TWT response command
#define SLI_WIFI_RSP_TWT_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for 11ax parameters request command
#define SLI_WIFI_RSP_11AX_PARAMS_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for BG Scan response command
#define SLI_WIFI_RSP_BG_SCAN_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for WPS response command
#define SLI_WIFI_RSP_WPS_METHOD_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for firmware upgrade response command
#define SLI_WLAN_RSP_FWUP_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Host PSK response command
#define SLI_WIFI_RSP_HOST_PSK_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value to set Mac address response command
#define SLI_WIFI_RSP_SET_MAC_ADDRESS_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for RSSI response command
#define SLI_WIFI_RSP_RSSI_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for TSF response command
#define SLI_WIFI_RSP_TSF_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for query GO parameters response command
#define SLI_WIFI_RSP_QUERY_GO_PARAMS_WAIT_TIME \
  ((SLI_WIFI_MANAGEMENT_COMMANDS_BASE_VALUE * SL_WIFI_MANAGEMENT_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))

/// Network commands timeout defines
/// Timeout value for set SNI embedded response command
#define SLI_WLAN_RSP_SET_SNI_EMBEDDED_WAIT_TIME ((60000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for HTTP abort response command
#define SLI_WLAN_RSP_HTTP_ABORT_WAIT_TIME ((100000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for HTTP client PUT response command
#define SLI_WLAN_RSP_HTTP_CLIENT_PUT_WAIT_TIME ((100000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for embedded MQTT client response command
#define SLI_WLAN_RSP_EMB_MQTT_CLIENT_WAIT_TIME ((60000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for DNS server add response command
#define SLI_WLAN_RSP_DNS_SERVER_ADD_WAIT_TIME ((150000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
/// Timeout value for Socket create response command
#define SLI_WLAN_RSP_SOCKET_CREATE_WAIT_TIME ((100000 * SL_WIFI_NETWORK_COMMANDS_TIMEOUT_SF) + (SLI_DEFAULT_TIMEOUT))
