/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include <stdint.h>

/** \addtogroup SL_WIFI_CONSTANTS Constants
  * @{ */

/// Max number of Access points that can be scanned
#define SL_MAX_SCANNED_AP 11

/// Max number of stations when module is running in access point mode.
#define SL_MAX_CLIENT_COUNT 16

/// Max Length of Wi-Fi PSK credential
#define SL_WIFI_MAX_PSK_LENGTH 32

/// Max Length of Wi-Fi PMK credential
#define SL_WIFI_MAX_PMK_LENGTH 64

/// Max length of Key in WEP security.
#define SL_WEP_KEY_LENGTH 32

/// Max number of keys for WEP security.
#define SL_WEP_KEY_COUNT 4

/// Max Length of User Name in enterprise security.
#define SL_EAP_USER_NAME_LENGTH 64

/// Max Length of password in enterprise security.
#define SL_EAP_PASSWORD_LENGTH 128

/// Select Internal Antenna for Wi-Fi.
#define SL_WIFI_SELECT_INTERNAL_ANTENNA 0

/// Select External Antenna for Wi-Fi.
#define SL_WIFI_SELECT_EXTERNAL_ANTENNA 1

/// Default Wi-Fi interface macro
#define SL_WIFI_DEFAULT_INTERFACE sl_wifi_get_default_interface()

/// Max Wi-Fi roaming trigger interval
#define SL_WIFI_NEVER_ROAM 0x7FFFFFFF

/// Low Transmit Power Threshold for Wi-Fi.
#define SI91X_LOW_TRANSMIT_POWER_THRESHOLD 6

/// Medium Transmit Power Threshold for Wi-Fi.
#define SI91X_MEDIUM_TRANSMIT_POWER_THRESHOLD 4

/// Wi-Fi security
typedef enum {
  SL_WIFI_OPEN            = 0, ///< Wi-Fi Open security type
  SL_WIFI_WPA             = 1, ///< Wi-Fi WPA security type
  SL_WIFI_WPA2            = 2, ///< Wi-Fi WPA2 security type
  SL_WIFI_WEP             = 3, ///< Wi-Fi WEP security type
  SL_WIFI_WPA_ENTERPRISE  = 4, ///< Wi-Fi WPA Enterprise security type
  SL_WIFI_WPA2_ENTERPRISE = 5, ///< Wi-Fi WPA2 Enterprise security type
  SL_WIFI_WPA_WPA2_MIXED  = 6, ///< Wi-Fi WPA/WPA2 Mixed security type
  SL_WIFI_WPA3            = 7, ///< Wi-Fi WPA3 security type
  SL_WIFI_WPA3_TRANSITION = 8, ///< Wi-Fi WPA3 Transition security type

  SL_WIFI_SECURITY_UNKNOWN = 0xFFFF, ///< WiFi Unknown Security type
} sl_wifi_security_t;

/// Wi-Fi encryption method
typedef enum {
  SL_WIFI_NO_ENCRYPTION,            ///< Wi-Fi with No Encryption
  SL_WIFI_WEP_ENCRYPTION,           ///< Wi-Fi with WEP Encryption
  SL_WIFI_TKIP_ENCRYPTION,          ///< Wi-Fi with TKIP Encryption
  SL_WIFI_CCMP_ENCRYPTION,          ///< Wi-Fi with CCMP Encryption
  SL_WIFI_EAP_TLS_ENCRYPTION,       ///< Wi-Fi with Enterprise TLS Encryption
  SL_WIFI_EAP_TTLS_ENCRYPTION,      ///< Wi-Fi with Enterprise TTLS Encryption
  SL_WIFI_EAP_FAST_ENCRYPTION,      ///< Wi-Fi with Enterprise FAST Encryption
  SL_WIFI_PEAP_MSCHAPV2_ENCRYPTION, ///< Wi-Fi with Enterprise PEAP Encryption
} sl_wifi_encryption_t;

/// Wi-Fi Credential
typedef enum {
  SL_WIFI_CRED_PSK = 0, ///< Wi-Fi Personal Credential
  SL_WIFI_CRED_PMK,     ///< Wi-Fi Pairwise master key
  SL_WIFI_CRED_WEP,     ///< Wi-Fi WEP Credential
  SL_WIFI_CRED_EAP      ///< Wi-Fi Enterprise client Credential
} sl_wifi_credential_type_t;

/// Wi-Fi antenna selections
typedef enum {
  SL_WIFI_ANTENNA_1,        ///< Wi-Fi Radio Antenna 1
  SL_WIFI_ANTENNA_2,        ///< Wi-Fi Radio Antenna 2
  SL_WIFI_ANTENNA_AUTO,     ///< Wi-Fi Radio Antenna Auto Selection
  SL_WIFI_ANTENNA_EXTERNAL, ///< Wi-Fi Radio External Antenna
  SL_WIFI_ANTENNA_INTERNAL, ///< Wi-Fi Radio Internal Antenna
} sl_wifi_antenna_t;

/// Wi-Fi interface index enumeration
typedef enum {
  SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX = 0, ///< Wi-Fi client on 2.4GHz interface
  SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX,         ///< Wi-Fi access point on 2.4GHz interface
  SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX,       ///< Wi-Fi client on 5GHz interface
  SL_WIFI_AP_5GHZ_INTERFACE_INDEX,           ///< Wi-Fi access point on 5GHz interface
  SL_WIFI_MAX_INTERFACE_INDEX
} sl_wifi_interface_index_t;

/// Wi-Fi interface enumeration
typedef enum {
  SL_WIFI_INVALID_INTERFACE = 0,        ///< Invalid interface
  SL_WIFI_CLIENT_INTERFACE  = (1 << 0), ///< (1 << 0) Wi-Fi client interface
  SL_WIFI_AP_INTERFACE      = (1 << 1), ///< (1 << 1) Wi-Fi access point interface

  SL_WIFI_2_4GHZ_INTERFACE = (1 << 2), ///< (1 << 2) 2.4GHz radio interface
  SL_WIFI_5GHZ_INTERFACE   = (1 << 3), ///< (1 << 3) 5GHz radio interface

  SL_WIFI_CLIENT_2_4GHZ_INTERFACE = SL_WIFI_CLIENT_INTERFACE
                                    | SL_WIFI_2_4GHZ_INTERFACE, ///< Wi-Fi client interface on 2.4GHz radio
  SL_WIFI_AP_2_4GHZ_INTERFACE = SL_WIFI_AP_INTERFACE
                                | SL_WIFI_2_4GHZ_INTERFACE, ///< Wi-Fi access point interface on 2.4GHz radio

  SL_WIFI_CLIENT_5GHZ_INTERFACE = SL_WIFI_CLIENT_INTERFACE
                                  | SL_WIFI_5GHZ_INTERFACE, ///< Wi-Fi client interface on 5GHz radio
  SL_WIFI_AP_5GHZ_INTERFACE = SL_WIFI_AP_INTERFACE
                              | SL_WIFI_5GHZ_INTERFACE, ///< Wi-Fi access point interface on 5GHz radio

  SL_WIFI_ALL_INTERFACES = SL_WIFI_CLIENT_INTERFACE | SL_WIFI_AP_INTERFACE | SL_WIFI_2_4GHZ_INTERFACE
                           | SL_WIFI_5GHZ_INTERFACE, ///< All available Wi-Fi interfaces
} sl_wifi_interface_t;

/// Enumeration of de-authentication reasons from an access point
typedef enum {
  SL_WIFI_DEAUTH,             ///< De-Authentication from radius server
  SL_WIFI_DEAUTH_UNSPECIFIED, ///< Unspecified de-authentication reason
} sl_wifi_deauth_reason_t;

/// W-Fi regulatory region
typedef enum {
  SL_WIFI_REGION_AUSTRALIA,      ///< Wi-Fi Region Australia
  SL_WIFI_REGION_FRANCE,         ///< Wi-Fi Region France
  SL_WIFI_REGION_EUROPEAN_UNION, ///< Wi-Fi Region European Union
  SL_WIFI_REGION_JAPAN,          ///< Wi-Fi Region Japan
  SL_WIFI_REGION_UNITED_STATES,  ///< Wi-Fi Region United States
} sl_wifi_regulatory_region_t;

/**
 * @brief Wi-Fi rate protocols
 * @note Recommended value for default behavior is SL_WIFI_RATE_PROTOCOL_AUTO
 */
typedef enum {
  SL_WIFI_RATE_PROTOCOL_B_ONLY,  ///< 802.11b rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_G_ONLY,  ///< 802.11g rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_N_ONLY,  ///< 802.11n rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_AC_ONLY, ///< 802.11ac rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_AX_ONLY, ///< 802.11ax rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_AUTO,    ///< Automatic rate selection
} sl_wifi_rate_protocol_t;

/// Wi-Fi scan types
typedef enum {
  SL_WIFI_SCAN_TYPE_ACTIVE = 0x00, ///< Active scan. Transmit probe requests and listen for responses
  SL_WIFI_SCAN_TYPE_PASSIVE =
    0x01, ///< Passive scan. No active transmissions, listen for AP beacons and probe responses
  SL_WIFI_SCAN_TYPE_PROHIBITED_CHANNELS = 0x04, ///< Scan channels prohibited by regulatory region
  SL_WIFI_SCAN_TYPE_ADV_SCAN = 0X08 ///< Adv_scan is for scan Access Points, when module is in connected state
} sl_wifi_scan_type_t;

/// Wi-Fi transfer rates
typedef enum {
  SL_WIFI_AUTO_RATE = 0, ///< Wi-Fi Auto transfer rate

  SL_WIFI_RATE_11B_1,                         ///< Wi-Fi 1 Mbps transfer rate
  SL_WIFI_RATE_11B_MIN = SL_WIFI_RATE_11B_1,  ///< Wi-Fi 11B Minimum transfer rate
  SL_WIFI_RATE_11B_2,                         ///< Wi-Fi 2 Mbps transfer rate
  SL_WIFI_RATE_11B_5_5,                       ///< Wi-Fi 5.5 Mbps transfer rate
  SL_WIFI_RATE_11B_11,                        ///< Wi-Fi 11 Mbps transfer rate
  SL_WIFI_RATE_11B_MAX = SL_WIFI_RATE_11B_11, ///< Wi-Fi 11B Maximum transfer rate

  SL_WIFI_RATE_11G_6,                         ///< Wi-Fi 6 Mbps transfer rate
  SL_WIFI_RATE_11G_MIN = SL_WIFI_RATE_11G_6,  ///< Wi-Fi 11G Minimum transfer rate
  SL_WIFI_RATE_11G_9,                         ///< Wi-Fi 9 Mbps transfer rate
  SL_WIFI_RATE_11G_12,                        ///< Wi-Fi 12 Mbps transfer rate
  SL_WIFI_RATE_11G_18,                        ///< Wi-Fi 18 Mbps transfer rate
  SL_WIFI_RATE_11G_24,                        ///< Wi-Fi 24 Mbps transfer rate
  SL_WIFI_RATE_11G_36,                        ///< Wi-Fi 36 Mbps transfer rate
  SL_WIFI_RATE_11G_48,                        ///< Wi-Fi 48 Mbps transfer rate
  SL_WIFI_RATE_11G_54,                        ///< Wi-Fi 54 Mbps transfer rate
  SL_WIFI_RATE_11G_MAX = SL_WIFI_RATE_11G_54, ///< Wi-Fi 11G Maximum transfer rate

  SL_WIFI_RATE_11N_MCS0,                        ///< Wi-Fi 11N MCS index 0 transfer rate
  SL_WIFI_RATE_11N_MIN = SL_WIFI_RATE_11N_MCS0, ///< Wi-Fi 11N Minimum transfer rate
  SL_WIFI_RATE_11N_MCS1,                        ///< Wi-Fi 11N MCS index 1 transfer rate
  SL_WIFI_RATE_11N_MCS2,                        ///< Wi-Fi 11N MCS index 2 transfer rate
  SL_WIFI_RATE_11N_MCS3,                        ///< Wi-Fi 11N MCS index 3 transfer rate
  SL_WIFI_RATE_11N_MCS4,                        ///< Wi-Fi 11N MCS index 4 transfer rate
  SL_WIFI_RATE_11N_MCS5,                        ///< Wi-Fi 11N MCS index 5 transfer rate
  SL_WIFI_RATE_11N_MCS6,                        ///< Wi-Fi 11N MCS index 6 transfer rate
  SL_WIFI_RATE_11N_MCS7,                        ///< Wi-Fi 11N MCS index 7 transfer rate
  SL_WIFI_RATE_11N_MAX = SL_WIFI_RATE_11N_MCS7, ///< Wi-Fi 11N Maximum transfer rate

  SL_WIFI_RATE_11AX_MCS0,                         ///< Wi-Fi 11AX MCS index 0 transfer rate
  SL_WIFI_RATE_11AX_MIN = SL_WIFI_RATE_11AX_MCS0, ///< Wi-Fi 11AX Minimum transfer rate
  SL_WIFI_RATE_11AX_MCS1,                         ///< Wi-Fi 11AX MCS index 1 transfer rate
  SL_WIFI_RATE_11AX_MCS2,                         ///< Wi-Fi 11AX MCS index 2 transfer rate
  SL_WIFI_RATE_11AX_MCS3,                         ///< Wi-Fi 11AX MCS index 3 transfer rate
  SL_WIFI_RATE_11AX_MCS4,                         ///< Wi-Fi 11AX MCS index 4 transfer rate
  SL_WIFI_RATE_11AX_MCS5,                         ///< Wi-Fi 11AX MCS index 5 transfer rate
  SL_WIFI_RATE_11AX_MCS6,                         ///< Wi-Fi 11AX MCS index 6 transfer rate
  SL_WIFI_RATE_11AX_MCS7,                         ///< Wi-Fi 11AX MCS index 7 transfer rate
  SL_WIFI_RATE_11AX_MAX = SL_WIFI_RATE_11AX_MCS7, ///< Wi-Fi 11AX Maximum transfer rate

  SL_WIFI_RATE_INVALID = 0xFF ///< Wi-Fi Invalid transfer rate
} sl_wifi_rate_t;

/// Wi-Fi BSS type
typedef enum {
  SL_WIFI_BSS_TYPE_INFRASTRUCTURE = 0,   ///< Wi-Fi BSS Type Infrastructure
  SL_WIFI_BSS_TYPE_ADHOC          = 1,   ///< Wi-Fi BSS Type ADHOC
  SL_WIFI_BSS_TYPE_ANY            = 2,   ///< Wi-Fi BSS Type ANY
  SL_WIFI_BSS_TYPE_UNKNOWN        = 0xFF ///< Wi-Fi BSS Type Unknown
} sl_wifi_bss_type_t;

/// Wi-Fi radio band
typedef enum {
  SL_WIFI_AUTO_BAND   = 0, ///< Wi-Fi Band Auto
  SL_WIFI_BAND_900MHZ = 1, ///< Wi-Fi Band 900Mhz
  SL_WIFI_BAND_2_4GHZ = 2, ///< Wi-Fi Band 2.4Ghz
  SL_WIFI_BAND_5GHZ   = 3, ///< Wi-Fi Band 5Ghz
  SL_WIFI_BAND_6GHZ   = 4, ///< Wi-Fi Band 6Ghz
  SL_WIFI_BAND_60GHZ  = 5, ///< Wi-Fi Band 60Ghz
} sl_wifi_band_t;

typedef enum {
  SL_WIFI_AUTO_BANDWIDTH   = 0, ///< Wi-Fi Bandwidth Auto
  SL_WIFI_BANDWIDTH_10MHz  = 0, ///< Wi-Fi Bandwidth 10Mhz
  SL_WIFI_BANDWIDTH_20MHz  = 1, ///< Wi-Fi Bandwidth 20Mhz
  SL_WIFI_BANDWIDTH_40MHz  = 2, ///< Wi-Fi Bandwidth 40Mhz
  SL_WIFI_BANDWIDTH_80MHz  = 3, ///< Wi-Fi Bandwidth 80Mhz
  SL_WIFI_BANDWIDTH_160MHz = 4, ///< Wi-Fi Bandwidth 160Mhz
} sl_wifi_bandwidth_t;

/// Option flags for client interfaces
typedef enum {
  SL_WIFI_NO_JOIN_OPTION   = 0,        ///< Wi-Fi Client Join with no flags
  SL_WIFI_JOIN_WITH_NO_CSA = (1 << 0), ///< Wi-Fi Client Join with no CSA
  SL_WIFI_JOIN_WITH_SCAN   = (1 << 1)  ///< Wi-Fi Client Join with Scan
} sl_wifi_client_flag_t;

/// Option flags for AP interfaces
typedef enum {
  SL_WIFI_HIDDEN_SSID = (1 << 0), ///< Hide SSID of the AP
} sl_wifi_ap_flag_t;

/// Listen interval time units
typedef enum {
  SL_WIFI_LISTEN_INTERVAL_TIME_UNIT_BEACON, ///< Time units specified in beacon periods
  SL_WIFI_LISTEN_INTERVAL_TIME_UNIT_DTIM    ///< Time units specified in DTIM periods
} sl_wifi_listen_interval_time_unit_t;

/// Wi-Fi WPS mode
typedef enum {
  SL_WIFI_WPS_PIN_MODE,         ///< WPS pin mode
  SL_WIFI_WPS_PUSH_BUTTON_MODE, ///< WPS push button mode
} sl_wifi_wps_mode_t;

/// Wi-Fi event group
typedef enum {
  SL_WIFI_SCAN_RESULT_EVENTS,                       ///< SL_WIFI_SCAN_RESULT_EVENTS
  SL_WIFI_JOIN_EVENTS,                              ///< SL_WIFI_JOIN_EVENTS
  SL_WIFI_RX_PACKET_EVENTS,                         ///< SL_WIFI_RX_PACKET_EVENTS
  SL_WIFI_COMMAND_RESPONSE_EVENTS,                  ///< SL_WIFI_COMMAND_RESPONSE_EVENTS
  SL_WIFI_STATS_RESPONSE_EVENTS,                    ///< SL_WIFI_STATS_RESPONSE_EVENTS
  SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS,                ///< SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS
  SL_WIFI_NETWORK_DOWN_EVENTS,                      ///< SL_WIFI_NETWORK_DOWN_EVENTS
  SL_WIFI_NETWORK_UP_EVENTS,                        ///< SL_WIFI_NETWORK_UP_EVENTS
  SL_WIFI_CLIENT_CONNECTED_EVENTS,                  ///< SL_WIFI_CLIENT_CONNECTED_EVENTS
  SL_WIFI_TWT_RESPONSE_EVENTS,                      ///< SL_WIFI_TWT_RESPONSE_EVENTS
  SL_WIFI_CLIENT_DISCONNECTED_EVENTS,               ///< SL_WIFI_CLIENT_DISCONNECTED_EVENTS
  SL_WIFI_EVENT_GROUP_COUNT,                        ///< SL_WIFI_EVENT_GROUP_COUNT
  SL_WIFI_EVENT_FAIL_INDICATION_EVENTS = (1 << 31), ///< SL_WIFI_EVENT_FAIL_INDICATION_EVENTS
} sl_wifi_event_group_t;

/// Wi-Fi events
/// Note that each event group has a matching event
/// Each event group may be a source of multiple different events
typedef enum {
  SL_WIFI_SCAN_RESULT_EVENT         = SL_WIFI_SCAN_RESULT_EVENTS,         ///< SL_WIFI_SCAN_RESULT_EVENT
  SL_WIFI_JOIN_EVENT                = SL_WIFI_JOIN_EVENTS,                ///< SL_WIFI_JOIN_EVENT
  SL_WIFI_RX_PACKET_EVENT           = SL_WIFI_RX_PACKET_EVENTS,           ///< SL_WIFI_RX_PACKET_EVENT
  SL_WIFI_COMMAND_RESPONSE_EVENT    = SL_WIFI_COMMAND_RESPONSE_EVENTS,    ///< SL_WIFI_COMMAND_RESPONSE_EVENT
  SL_WIFI_STATS_RESPONSE_EVENT      = SL_WIFI_STATS_RESPONSE_EVENTS,      ///< SL_WIFI_STATS_RESPONSE_EVENT
  SL_WIFI_HTTP_OTA_FW_UPDATE_EVENT  = SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS,  ///< SL_WIFI_HTTP_OTA_FW_UPDATE_EVENT
  SL_WIFI_NETWORK_DOWN_EVENT        = SL_WIFI_NETWORK_DOWN_EVENTS,        ///< SL_WIFI_NETWORK_DOWN_EVENT
  SL_WIFI_NETWORK_UP_EVENT          = SL_WIFI_NETWORK_UP_EVENTS,          ///< SL_WIFI_NETWORK_UP_EVENT
  SL_WIFI_CLIENT_CONNECTED_EVENT    = SL_WIFI_CLIENT_CONNECTED_EVENTS,    ///< SL_WIFI_CLIENT_CONNECTED_EVENT
  SL_WIFI_TWT_RESPONSE_EVENT        = SL_WIFI_TWT_RESPONSE_EVENTS,        ///< SL_WIFI_TWT_RESPONSE_EVENT
  SL_WIFI_CLIENT_DISCONNECTED_EVENT = SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ///< SL_WIFI_CLIENT_DISCONNECTED_EVENT

  // TWT specific events
  SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                                  | (1 << 16), ///< SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT
  SL_WIFI_TWT_AP_REJECTED_EVENT      = SL_WIFI_TWT_RESPONSE_EVENTS | (4 << 16), ///< SL_WIFI_TWT_AP_REJECTED_EVENT
  SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS | (5 << 16), ///< SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT
  SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                           | (6 << 16), ///< SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT
  SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                           | (10 << 16), ///< SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT
  SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS | (11 << 16), ///< SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT
  SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                          | (12 << 16), ///< SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT
  SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                               | (15 << 16), ///< SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT
  SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                              | (16 << 16), ///< SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT
  SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                                 | (17 << 16), ///< SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT
  SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS
                                             | (18 << 16), ///< SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT

  // Stats specific events
  SL_WIFI_STATS_EVENT              = SL_WIFI_STATS_RESPONSE_EVENTS | (1 << 16), ///< SL_WIFI_STATS_EVENT
  SL_WIFI_STATS_AYSNC_EVENT        = SL_WIFI_STATS_RESPONSE_EVENTS | (2 << 16), ///< SL_WIFI_STATS_AYSNC_EVENT
  SL_WIFI_STATS_ADVANCE_EVENT      = SL_WIFI_STATS_RESPONSE_EVENTS | (3 << 16), ///< SL_WIFI_STATS_ADVANCE_EVENT
  SL_WIFI_STATS_TEST_MODE_EVENT    = SL_WIFI_STATS_RESPONSE_EVENTS | (4 << 16), ///< SL_WIFI_STATS_TEST_MODE_EVENT
  SL_WIFI_STATS_MODULE_STATE_EVENT = SL_WIFI_STATS_RESPONSE_EVENTS | (5 << 16), ///< SL_WIFI_STATS_MODULE_STATE_EVENT

  // Single bit to indicate relevant event is related to a failure condition
  SL_WIFI_EVENT_FAIL_INDICATION = (1 << 31), ///< SL_WIFI_EVENT_FAIL_INDICATION
  SL_WIFI_INVALID_EVENT         = 0xFFFFFFFF,
} sl_wifi_event_t;

typedef enum {
  CLIENT_MODE, ///< WiFi Client mode
  AP_MODE,     ///< WiFi Access point mode
} sl_wifi_operational_mode_t;

/// Auto detect channel
#define SL_WIFI_AUTO_CHANNEL 0

/// API input checks
#define ARGS_CHECK_NULL_POINTER(ptr) \
  {                                  \
    if (ptr == NULL) {               \
      return SL_STATUS_NULL_POINTER; \
    }                                \
  }
#define ARGS_CHECK_INVALID_INTERFACE(interface)                                                 \
  {                                                                                             \
    if (!((interface == SL_WIFI_CLIENT_INTERFACE) || (interface == SL_WIFI_CLIENT_INTERFACE)    \
          || (interface == SL_WIFI_AP_INTERFACE) || (interface == SL_WIFI_2_4GHZ_INTERFACE))) { \
      return SL_STATUS_WIFI_UNKNOWN_INTERFACE;                                                  \
    }                                                                                           \
  }
/** @} */
