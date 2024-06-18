/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#ifndef _SL_WIFI_CONSTANTS_H_
#define _SL_WIFI_CONSTANTS_H_

#include <stdint.h>

/** \addtogroup SL_WIFI_CONSTANTS Constants
  * @{ */

/// Max number of Access points that can be scanned
#define SL_WIFI_MAX_SCANNED_AP 11

/// Max number of stations when module is running in access point mode.
#define SL_WIFI_MAX_CLIENT_COUNT 16

/// Max Length of Wi-Fi PSK credential
#define SL_WIFI_MAX_PSK_LENGTH 32

/// Max Length of Wi-Fi PMK credential
#define SL_WIFI_MAX_PMK_LENGTH 64

/// Max length of Key in WEP security.
#define SL_WIFI_WEP_KEY_LENGTH 32

/// Max number of keys for WEP security.
#define SL_WIFI_WEP_KEY_COUNT 4

/// Max Length of User Name in enterprise security.
#define SL_WIFI_EAP_USER_NAME_LENGTH 64

/// Max Length of password in enterprise security.
#define SL_WIFI_EAP_PASSWORD_LENGTH 128

/// Max Length of certificate key in enterprise security.
#define SL_WIFI_EAP_CERTIFICATE_KEY_LENGTH 80

/// Select Internal Antenna for Wi-Fi.
#define SL_WIFI_SELECT_INTERNAL_ANTENNA 0

/// Select External Antenna for Wi-Fi.
#define SL_WIFI_SELECT_EXTERNAL_ANTENNA 1

/// Default Wi-Fi interface macro
#define SL_WIFI_DEFAULT_INTERFACE sl_wifi_get_default_interface()

/// Max Wi-Fi roaming trigger interval
#define SL_WIFI_NEVER_ROAM 0x7FFFFFFF

/// Wi-Fi security.
/// @note WPA3 Transition mode not currently supported while running as an Access Point.
typedef enum {
  SL_WIFI_OPEN            = 0, ///< Wi-Fi Open security type
  SL_WIFI_WPA             = 1, ///< Wi-Fi WPA security type
  SL_WIFI_WPA2            = 2, ///< Wi-Fi WPA2 security type
  SL_WIFI_WEP             = 3, ///< Wi-Fi WEP security type
  SL_WIFI_WPA_ENTERPRISE  = 4, ///< Wi-Fi WPA Enterprise security type
  SL_WIFI_WPA2_ENTERPRISE = 5, ///< Wi-Fi WPA2 Enterprise security type
  SL_WIFI_WPA_WPA2_MIXED  = 6, ///< Wi-Fi WPA/WPA2 Mixed security type
  SL_WIFI_WPA3            = 7, ///< Wi-Fi WPA3 security type
  SL_WIFI_WPA3_TRANSITION = 8, ///< Wi-Fi WPA3 Transition security type (not currently supported in AP mode)

  SL_WIFI_SECURITY_UNKNOWN = 0xFFFF, ///< Wi-Fi Unknown Security type
} sl_wifi_security_t;

/// Wi-Fi encryption method.
/// @note Some encryption types not currently supported in station (STA) mode.
typedef enum {
  SL_WIFI_DEFAULT_ENCRYPTION,       ///< Wi-Fi with Default Encryption
  SL_WIFI_NO_ENCRYPTION,            ///< Wi-Fi with No Encryption (not currently supported in STA mode)
  SL_WIFI_WEP_ENCRYPTION,           ///< Wi-Fi with WEP Encryption (not currently supported in STA mode)
  SL_WIFI_TKIP_ENCRYPTION,          ///< Wi-Fi with TKIP Encryption (not currently supported in STA mode)
  SL_WIFI_CCMP_ENCRYPTION,          ///< Wi-Fi with CCMP Encryption
  SL_WIFI_EAP_TLS_ENCRYPTION,       ///< Wi-Fi with Enterprise TLS Encryption
  SL_WIFI_EAP_TTLS_ENCRYPTION,      ///< Wi-Fi with Enterprise TTLS Encryption
  SL_WIFI_EAP_FAST_ENCRYPTION,      ///< Wi-Fi with Enterprise FAST Encryption
  SL_WIFI_PEAP_MSCHAPV2_ENCRYPTION, ///< Wi-Fi with Enterprise PEAP Encryption
  SL_WIFI_EAP_LEAP_ENCRYPTION       ///< Wi-Fi with Enterprise LEAP Encryption
} sl_wifi_encryption_t;

/// Wi-Fi Transition Disable Indication (TDI). Supported only in WPA3(Personal or Personal Transition) security in AP mode .
typedef enum {
  SL_WIFI_TDI_NONE = 0, ///< Allows stations to do transition in AP network.
  SL_WIFI_TDI_SAE =
    (1
     << 4), ///< Disallow stations to do transition in AP network and allow to connect with only WPA3-SAE security mode.
} sl_wifi_tdi_t;

/// Wi-Fi Credential
typedef enum {
  SL_WIFI_PSK_CREDENTIAL = 0, ///< Wi-Fi Personal Credential
  SL_WIFI_PMK_CREDENTIAL,     ///< Wi-Fi Pairwise master key
  SL_WIFI_WEP_CREDENTIAL,     ///< Wi-Fi WEP Credential
  SL_WIFI_EAP_CREDENTIAL      ///< Wi-Fi Enterprise client Credential
} sl_wifi_credential_type_t;

/// Wi-Fi antenna selections
/// @note Only internal antenna currently supported.
typedef enum {
  SL_WIFI_ANTENNA_1,        ///< Wi-Fi Radio Antenna 1 (not currently supported)
  SL_WIFI_ANTENNA_2,        ///< Wi-Fi Radio Antenna 2 (not currently supported)
  SL_WIFI_ANTENNA_AUTO,     ///< Wi-Fi Radio Antenna Auto Selection (not currently supported)
  SL_WIFI_ANTENNA_EXTERNAL, ///< Wi-Fi Radio External Antenna (not currently supported)
  SL_WIFI_ANTENNA_INTERNAL, ///< Wi-Fi Radio Internal Antenna
} sl_wifi_antenna_t;
/// Wi-Fi interface index enumeration.
/// @note 5 GHz interfaces not currently supported.
typedef enum {
  SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX = 0, ///< Wi-Fi client on 2.4GHz interface
  SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX,         ///< Wi-Fi access point on 2.4GHz interface
  SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX,       ///< Wi-Fi client on 5GHz interface (not currently supported)
  SL_WIFI_AP_5GHZ_INTERFACE_INDEX,           ///< Wi-Fi access point on 5GHz interface (not currently supported)
  SL_WIFI_TRANSCEIVER_INTERFACE_INDEX,       ///< Wi-Fi Transceiver Mode
  SL_WIFI_MAX_INTERFACE_INDEX                ///< Used for internally by SDK
} sl_wifi_interface_index_t;

/// Wi-Fi interface enumeration
typedef enum {
  SL_WIFI_INVALID_INTERFACE = 0, ///< Invalid interface

  SL_WIFI_CLIENT_INTERFACE = (1 << 0), ///< Wi-Fi client interface
  SL_WIFI_AP_INTERFACE     = (1 << 1), ///< Wi-Fi access point interface

  SL_WIFI_2_4GHZ_INTERFACE = (1 << 2), ///<  2.4GHz radio interface
  SL_WIFI_5GHZ_INTERFACE   = (1 << 3), ///< 5GHz radio interface

  //BIT(4), BIT(5) - Reserved for 6GHz and Sub-GHz

  SL_WIFI_TRANSCEIVER_INTERFACE = (1 << 7), ///< Wi-Fi Transceiver mode interface

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
/// @note Australia and France regions not currently supported.
typedef enum {
  SL_WIFI_REGION_AUSTRALIA,      ///< Wi-Fi Region Australia (not currently supported)
  SL_WIFI_REGION_FRANCE,         ///< Wi-Fi Region France (not currently supported)
  SL_WIFI_REGION_EUROPEAN_UNION, ///< Wi-Fi Region European Union
  SL_WIFI_REGION_JAPAN,          ///< Wi-Fi Region Japan
  SL_WIFI_REGION_UNITED_STATES,  ///< Wi-Fi Region United States
} sl_wifi_regulatory_region_t;

/**
 * @brief Wi-Fi rate protocols
 * @note Recommended value for default behavior is SL_WIFI_RATE_PROTOCOL_AUTO
 * @note 802.11ac not currently supported.
 */
typedef enum {
  SL_WIFI_RATE_PROTOCOL_B_ONLY,  ///< 802.11b rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_G_ONLY,  ///< 802.11g rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_N_ONLY,  ///< 802.11n rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_AC_ONLY, ///< 802.11ac rates only (rates go here) (not currently supported)
  SL_WIFI_RATE_PROTOCOL_AX_ONLY, ///< 802.11ax rates only (rates go here)
  SL_WIFI_RATE_PROTOCOL_AUTO,    ///< Automatic rate selection
} sl_wifi_rate_protocol_t;

/// Wi-Fi scan types
typedef enum {
  SL_WIFI_SCAN_TYPE_ACTIVE = 0x00, ///< Active scan. Transmit probe requests and listen for responses
  SL_WIFI_SCAN_TYPE_PASSIVE =
    0x01, ///< Passive scan. No active transmissions, listen for AP beacons and probe responses
  SL_WIFI_SCAN_TYPE_PROHIBITED_CHANNELS = 0x04, ///< Scan channels prohibited by regulatory region
  SL_WIFI_SCAN_TYPE_ADV_SCAN            = 0X08 ///< Advance scanning of Access Points, when module is in connected state
} sl_wifi_scan_type_t;

/// Wi-Fi transfer rates
typedef enum {
  SL_WIFI_AUTO_RATE = 0, ///< Wi-Fi Auto transfer rate

  SL_WIFI_RATE_11B_1,                         ///< Wi-Fi 1 Mbps transfer rate for 802.11b
  SL_WIFI_RATE_11B_MIN = SL_WIFI_RATE_11B_1,  ///< Wi-Fi Minimum transfer rate for 802.11b
  SL_WIFI_RATE_11B_2,                         ///< Wi-Fi 2 Mbps transfer rate for 802.11b
  SL_WIFI_RATE_11B_5_5,                       ///< Wi-Fi 5.5 Mbps transfer rate for 802.11b
  SL_WIFI_RATE_11B_11,                        ///< Wi-Fi 11 Mbps transfer rate for 802.11b
  SL_WIFI_RATE_11B_MAX = SL_WIFI_RATE_11B_11, ///< Wi-Fi Maximum transfer rate for 802.11b

  SL_WIFI_RATE_11G_6,                         ///< Wi-Fi 6 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_MIN = SL_WIFI_RATE_11G_6,  ///< Wi-Fi Minimum transfer rate for 802.11g
  SL_WIFI_RATE_11G_9,                         ///< Wi-Fi 9 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_12,                        ///< Wi-Fi 12 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_18,                        ///< Wi-Fi 18 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_24,                        ///< Wi-Fi 24 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_36,                        ///< Wi-Fi 36 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_48,                        ///< Wi-Fi 48 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_54,                        ///< Wi-Fi 54 Mbps transfer rate for 802.11g
  SL_WIFI_RATE_11G_MAX = SL_WIFI_RATE_11G_54, ///< Wi-Fi Maximum transfer rate for 802.11g

  SL_WIFI_RATE_11N_MCS0,                        ///< Wi-Fi MCS index 0 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MIN = SL_WIFI_RATE_11N_MCS0, ///< Wi-Fi Minimum transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS1,                        ///< Wi-Fi MCS index 1 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS2,                        ///< Wi-Fi MCS index 2 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS3,                        ///< Wi-Fi MCS index 3 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS4,                        ///< Wi-Fi MCS index 4 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS5,                        ///< Wi-Fi MCS index 5 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS6,                        ///< Wi-Fi MCS index 6 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MCS7,                        ///< Wi-Fi MCS index 7 transfer rate for 802.11n
  SL_WIFI_RATE_11N_MAX = SL_WIFI_RATE_11N_MCS7, ///< Wi-Fi Maximum transfer rate for 802.11n

  SL_WIFI_RATE_11AX_MCS0,                         ///< Wi-Fi MCS index 0 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MIN = SL_WIFI_RATE_11AX_MCS0, ///< Wi-Fi Minimum transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS1,                         ///< Wi-Fi MCS index 1 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS2,                         ///< Wi-Fi MCS index 2 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS3,                         ///< Wi-Fi MCS index 3 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS4,                         ///< Wi-Fi MCS index 4 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS5,                         ///< Wi-Fi MCS index 5 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS6,                         ///< Wi-Fi MCS index 6 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MCS7,                         ///< Wi-Fi MCS index 7 transfer rate for 802.11ax
  SL_WIFI_RATE_11AX_MAX = SL_WIFI_RATE_11AX_MCS7, ///< Wi-Fi Maximum transfer rate for 802.11ax

  SL_WIFI_RATE_INVALID = 0xFF ///< Wi-Fi Invalid transfer rate
} sl_wifi_rate_t;

/// Wi-Fi BSS type
typedef enum {
  SL_WIFI_BSS_TYPE_INFRASTRUCTURE = 0,   ///< Wi-Fi BSS Type Infrastructure
  SL_WIFI_BSS_TYPE_ADHOC          = 1,   ///< Wi-Fi BSS Type ADHOC
  SL_WIFI_BSS_TYPE_ANY            = 2,   ///< Wi-Fi BSS Type ANY
  SL_WIFI_BSS_TYPE_UNKNOWN        = 0xFF ///< Wi-Fi BSS Type Unknown
} sl_wifi_bss_type_t;

/// Wi-Fi radio band.
/// @note Only 2.4 GHz currently supported.
typedef enum {
  SL_WIFI_AUTO_BAND   = 0, ///< Wi-Fi Band Auto
  SL_WIFI_BAND_900MHZ = 1, ///< Wi-Fi Band 900Mhz (not currently supported)
  SL_WIFI_BAND_2_4GHZ = 2, ///< Wi-Fi Band 2.4Ghz
  SL_WIFI_BAND_5GHZ   = 3, ///< Wi-Fi Band 5Ghz (not currently supported)
  SL_WIFI_BAND_6GHZ   = 4, ///< Wi-Fi Band 6Ghz (not currently supported)
  SL_WIFI_BAND_60GHZ  = 5, ///< Wi-Fi Band 60Ghz (not currently supported)
} sl_wifi_band_t;

/// @note Only 20 MHz currently supported.
typedef enum {
  SL_WIFI_AUTO_BANDWIDTH   = 0, ///< Wi-Fi Bandwidth Auto
  SL_WIFI_BANDWIDTH_10MHz  = 0, ///< Wi-Fi Bandwidth 10Mhz (not currently supported)
  SL_WIFI_BANDWIDTH_20MHz  = 1, ///< Wi-Fi Bandwidth 20Mhz
  SL_WIFI_BANDWIDTH_40MHz  = 2, ///< Wi-Fi Bandwidth 40Mhz (not currently supported)
  SL_WIFI_BANDWIDTH_80MHz  = 3, ///< Wi-Fi Bandwidth 80Mhz (not currently supported)
  SL_WIFI_BANDWIDTH_160MHz = 4, ///< Wi-Fi Bandwidth 160Mhz (not currently supported)
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
  SL_WIFI_SCAN_RESULT_EVENTS, ///< Event group for Wi-Fi scan results
  SL_WIFI_JOIN_EVENTS,        ///< Event group for Wi-Fi join status
  SL_WIFI_RX_PACKET_EVENTS, ///< Event group for Wi-Fi received packet. This feature is not supported in current release
  SL_WIFI_COMMAND_RESPONSE_EVENTS, ///< Event group for Wi-Fi command response. This feature is not supported in current release
  SL_WIFI_STATS_RESPONSE_EVENTS,     ///< Event group for Wi-Fi statistics response
  SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS, ///< Event group for Wi-Fi OTA firmware update status via HTTP
  SL_WIFI_NETWORK_DOWN_EVENTS, ///< Event group for Wi-Fi network down. This feature is not supported in current release
  SL_WIFI_NETWORK_UP_EVENTS,   ///< Event group for Wi-Fi network up. This feature is not supported in current release
  SL_WIFI_CLIENT_CONNECTED_EVENTS,    ///< Event group for Wi-Fi client connected status
  SL_WIFI_TWT_RESPONSE_EVENTS,        ///< Event group for Wi-Fi TWT response
  SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ///< Event group for Wi-Fi client disconnection status
  SL_WIFI_TRANSCEIVER_EVENTS,         ///< Event group for Wi-Fi transceiver events
  SL_WIFI_EVENT_GROUP_COUNT,          ///< Event group for Wi-Fi maximum default group count. Used internally by SDK
  SL_WIFI_EVENT_FAIL_INDICATION_EVENTS = (1 << 31), ///< Event group for Wi-Fi fail indication
} sl_wifi_event_group_t;

/// Wi-Fi events.
/// @note Each event group has a matching event.
/// @note Each event group may be a source of multiple different events.
typedef enum {
  SL_WIFI_SCAN_RESULT_EVENT =
    SL_WIFI_SCAN_RESULT_EVENTS, ///< Event for Wi-Fi scan result. Data would be of type of @ref sl_wifi_scan_result_t
  SL_WIFI_JOIN_EVENT = SL_WIFI_JOIN_EVENTS, ///< Event for Wi-Fi join status. Data would be of type string
  SL_WIFI_RX_PACKET_EVENT =
    SL_WIFI_RX_PACKET_EVENTS, ///< Event for Wi-Fi received packet. This feature is not supported in current release
  SL_WIFI_COMMAND_RESPONSE_EVENT =
    SL_WIFI_COMMAND_RESPONSE_EVENTS, ///< Event for Wi-Fi command response. This feature is not supported in current release
  SL_WIFI_STATS_RESPONSE_EVENT =
    SL_WIFI_STATS_RESPONSE_EVENTS, ///< Event for Wi-Fi statistics response. Data would be NULL
  SL_WIFI_HTTP_OTA_FW_UPDATE_EVENT =
    SL_WIFI_HTTP_OTA_FW_UPDATE_EVENTS, ///< Event for Wi-Fi OTA firmware update status via HTTP. Data would be NULL
  SL_WIFI_NETWORK_DOWN_EVENT =
    SL_WIFI_NETWORK_DOWN_EVENTS, ///< Event for Wi-Fi network down. This feature is not supported in current release
  SL_WIFI_NETWORK_UP_EVENT =
    SL_WIFI_NETWORK_UP_EVENTS, ///< Event for Wi-Fi network up. This feature is not supported in current release
  SL_WIFI_CLIENT_CONNECTED_EVENT =
    SL_WIFI_CLIENT_CONNECTED_EVENTS, ///< Event for Wi-Fi client connected status. Data would be of type @ref sl_mac_address_t
  SL_WIFI_TWT_RESPONSE_EVENT = SL_WIFI_TWT_RESPONSE_EVENTS, ///< Event for Wi-Fi TWT response. Data would be NULL
  SL_WIFI_CLIENT_DISCONNECTED_EVENT =
    SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ///< Event for Wi-Fi client disconnection status. Data would of type @ref sl_mac_address_t
  SL_WIFI_TRANSCEIVER_EVENT = SL_WIFI_TRANSCEIVER_EVENTS, ///< Event for Wi-Fi transceiver TX/RX events.
  // TWT specific events
  SL_WIFI_TWT_UNSOLICITED_SESSION_SUCCESS_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (1 << 16), ///< Event for TWT unsolicited session success. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_AP_REJECTED_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (4 << 16), ///< Event for TWT AP rejection. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_OUT_OF_TOLERANCE_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (5 << 16), ///< Event for TWT out of tolerance. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_RESPONSE_NOT_MATCHED_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (6 << 16), ///<  Event for TWT response not matched. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_UNSUPPORTED_RESPONSE_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (10 << 16), ///< Event for TWT unsupported response. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_TEARDOWN_SUCCESS_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (11 << 16), ///< Event for TWT teardown success. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_AP_TEARDOWN_SUCCESS_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (12 << 16), ///< Event for TWT AP teardown success. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_FAIL_MAX_RETRIES_REACHED_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (15 << 16), ///< Event for TWT maximum retries reached. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_INACTIVE_DUE_TO_ROAMING_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (16 << 16), ///< Event for TWT inactive due to roaming. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_INACTIVE_DUE_TO_DISCONNECT_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (17 << 16), ///< Event for TWT inactive due to disconnect. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_INACTIVE_NO_AP_SUPPORT_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (18 << 16), ///< Event for TWT inactive due to no AP support. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_RESCHEDULE_TWT_SUCCESS_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (19 << 16), ///<  Event for TWT suspend resume success. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_INFO_FRAME_EXCHANGE_FAILED_EVENT =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (20 << 16), ///< Event for TWT info frame exchange failure. Data would be of type @ref sl_si91x_twt_response_t.
  SL_WIFI_TWT_EVENTS_END =
    SL_WIFI_TWT_RESPONSE_EVENTS
    | (21 << 16), ///< Event for TWT event end.  Data would be of type @ref sl_si91x_twt_response_t.

  // Stats specific events
  SL_WIFI_STATS_EVENT = SL_WIFI_STATS_RESPONSE_EVENTS
                        | (1 << 16), ///< Event for Wi-Fi statistics. This feature is not supported in current release
  SL_WIFI_STATS_AYSNC_EVENT =
    SL_WIFI_STATS_RESPONSE_EVENTS
    | (2
       << 16), ///< Event for Wi-Fi asynchronous statistics. Data would be of type [sl_si91x_async_stats_response_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-async-stats-response-t)
  SL_WIFI_STATS_ADVANCE_EVENT =
    SL_WIFI_STATS_RESPONSE_EVENTS
    | (3
       << 16), ///< Event for Wi-Fi advance statistics. Data would be of type [sl_si91x_advance_stats_response_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-advance-stats-response-t)
  SL_WIFI_STATS_TEST_MODE_EVENT =
    SL_WIFI_STATS_RESPONSE_EVENTS
    | (4 << 16), ///< Event for Wi-Fi test mode statistics. This feature is not supported in current release
  SL_WIFI_STATS_MODULE_STATE_EVENT =
    SL_WIFI_STATS_RESPONSE_EVENTS
    | (5
       << 16), ///< Event for Wi-Fi module state statistics. Data would be of type [sl_si91x_module_state_stats_response_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91x-module-state-stats-response-t)

  SL_WIFI_TRANSCEIVER_RX_DATA_RECEIVE_CB = SL_WIFI_TRANSCEIVER_EVENTS | (1 << 16),
  SL_WIFI_TRANSCEIVER_TX_DATA_STATUS_CB  = SL_WIFI_TRANSCEIVER_EVENTS | (2 << 16),

  // Single bit to indicate relevant event is related to a failure condition
  SL_WIFI_EVENT_FAIL_INDICATION = (1 << 31),  ///< Event for Wi-Fi event failure indication
  SL_WIFI_INVALID_EVENT         = 0xFFFFFFFF, ///< Invalid Wi-Fi event.  Data would be NULL
} sl_wifi_event_t;

/// Enumeration defining actions related to Target Wake Time (TWT).
typedef enum {
  SL_WIFI_SUSPEND_INDEFINITELY, ///< Indicates the suspension of TWT for an indefinite period, effectively disabling TWT functionality until explicitly resumed.
  SL_WIFI_SUSPEND_FOR_DURATION, ///< Specifies that TWT should be suspended for a specified duration of time, after which it can automatically resume.
  SL_WIFI_RESUME_IMMEDIATELY ///< Signifies an immediate resumption of TWT, allowing devices to continue adhering to TWT schedules.
} sl_wifi_reschedule_twt_action_t;

typedef enum {
  SL_WIFI_DATA_RATE_1       = 0,   ///< Wi-Fi 1 Mbps transfer rate
  SL_WIFI_DATA_RATE_2       = 2,   ///< Wi-Fi 2 Mbps transfer rate
  SL_WIFI_DATA_RATE_5_5     = 4,   ///< Wi-Fi 5.5 Mbps transfer rate
  SL_WIFI_DATA_RATE_11      = 6,   ///< Wi-Fi 11 Mbps transfer rate
  SL_WIFI_DATA_RATE_6       = 139, ///< Wi-Fi 6 Mbps transfer rate
  SL_WIFI_DATA_RATE_9       = 143, ///< Wi-Fi 9 Mbps transfer rate
  SL_WIFI_DATA_RATE_12      = 138, ///< Wi-Fi 12 Mbps transfer rate
  SL_WIFI_DATA_RATE_18      = 142, ///< Wi-Fi 18 Mbps transfer rate
  SL_WIFI_DATA_RATE_24      = 137, ///< Wi-Fi 24 Mbps transfer rate
  SL_WIFI_DATA_RATE_36      = 141, ///< Wi-Fi 36 Mbps transfer rate
  SL_WIFI_DATA_RATE_48      = 136, ///< Wif-Fi 48 Mbps transfer rate
  SL_WIFI_DATA_RATE_54      = 140, ///< Wi-Fi 54 Mbps transfer rate
  SL_WIFI_DATA_RATE_MCS0    = 256, ///< Wi-Fi MCS index 0 transfer rate
  SL_WIFI_DATA_RATE_MCS1    = 257, ///< Wi-Fi MCS index 1 transfer rate
  SL_WIFI_DATA_RATE_MCS2    = 258, ///< Wi-Fi MCS index 2 transfer rate
  SL_WIFI_DATA_RATE_MCS3    = 259, ///< Wi-Fi MCS index 3 transfer rate
  SL_WIFI_DATA_RATE_MCS4    = 260, ///< Wi-Fi MCS index 4 transfer rate
  SL_WIFI_DATA_RATE_MCS5    = 261, ///< Wi-Fi MCS index 5 transfer rate
  SL_WIFI_DATA_RATE_MCS6    = 262, ///< Wi-Fi MCS index 6 transfer rate
  SL_WIFI_DATA_RATE_MCS7    = 263, ///< Wi-Fi MCS index 7 transfer rate
  SL_WIFI_DATA_RATE_MCS7_SG = 775,
} sl_wifi_data_rate_t;

/// tx test mode
typedef enum {
  SL_WIFI_TEST_BURST_MODE                          = 0, ///< Burst Mode
  SL_WIFI_TEST_CONTINOUS_MODE                      = 1, ///< Continuous Mode
  SL_WIFI_TEST_CONTINOUS_WAVE_MODE                 = 2, ///< CW Mode
  SL_WIFI_TEST_CONTINOUS_WAVE_MODE_OFF_CENTER_LOW  = 3, ///< CW Mode center frequency - 2.5MHZ Mode
  SL_WIFI_TEST_CONTINOUS_WAVE_MODE_OFF_CENTER_HIGH = 4, ///< CW Mode center frequency + 5MHZ Mode
} sl_wifi_tx_test_mode_t;

/// HT caps supported
typedef enum {
  SL_WIFI_HT_CAPS_NUM_RX_STBC      = (1 << 8),
  SL_WIFI_HT_CAPS_SHORT_GI_20MHZ   = (1 << 5),
  SL_WIFI_HT_CAPS_GREENFIELD_EN    = (1 << 4),
  SL_WIFI_HT_CAPS_SUPPORT_CH_WIDTH = (1 << 1),
} sl_wifi_high_throughput_capability_types_t;

/// Multicast filter command types
typedef enum {
  SL_WIFI_MULTICAST_MAC_ADD_BIT   = 0, ///< To set particular bit in multicast bitmap
  SL_WIFI_MULTICAST_MAC_CLEAR_BIT = 1, ///< To reset particular bit in multicast bitmap
  SL_WIFI_MULTICAST_MAC_CLEAR_ALL = 2, ///< To clear all the bits in multicast bitmap
  SL_WIFI_MULTICAST_MAC_SET_ALL   = 3, ///< To set all the bits in multicast bitmap
} sl_wifi_multicast_filter_command_t;

/// Option flags for EAP client interfaces
typedef enum {
  SL_WIFI_EAP_ENABLE_OKC = (1 << 0), ///< Wi-Fi EAP Client flag to enable opportunistic key caching (OKC)
  SL_WIFI_EAP_ENABLE_PEAP_CA =
    (1 << 1), ///< Wi-Fi EAP Client flag to enable CA certificate requirement for PEAP connection
  SL_WIFI_EAP_DHE_RSA_AES256_SHA256 =
    (1 << 2), ///< Wi-Fi EAP Client flag to use DHE-RSA-AES256-SHA256 Cipher for EAP connection
  SL_WIFI_EAP_DHE_RSA_AES128_SHA256 =
    (1 << 3), ///< Wi-Fi EAP Client flag to use DHE-RSA-AES128-SHA256 Cipher for EAP connection
  SL_WIFI_EAP_DHE_RSA_AES256_SHA =
    (1 << 4), ///< Wi-Fi EAP Client flag to use DHE-RSA-AES256-SHA Cipher for EAP connection
  SL_WIFI_EAP_DHE_RSA_AES128_SHA =
    (1 << 5),                            ///< Wi-Fi EAP Client flag to use DHE-RSA-AES128-SHA Cipher for EAP connection
  SL_WIFI_EAP_AES256_SHA256 = (1 << 6),  ///< Wi-Fi EAP Client flag to use AES256-SHA256 Cipher for EAP connection
  SL_WIFI_EAP_AES128_SHA256 = (1 << 7),  ///< Wi-Fi EAP Client flag to use AES128-SHA256 Cipher for EAP connection
  SL_WIFI_EAP_AES256_SHA    = (1 << 8),  ///< Wi-Fi EAP Client flag to use AES256-SHA Cipher for EAP connection
  SL_WIFI_EAP_AES128_SHA    = (1 << 9),  ///< Wi-Fi EAP Client flag to use AES128-SHA Cipher for EAP connection
  SL_WIFI_EAP_RC4_SHA       = (1 << 10), ///< Wi-Fi EAP Client flag to use RC4-SHA Cipher for EAP connection
  SL_WIFI_EAP_DES_CBC3_SHA  = (1 << 11), ///< Wi-Fi EAP Client flag to use DES-CBC3-SHA Cipher for EAP connection
  SL_WIFI_EAP_RC4_MD5       = (1 << 12)  ///< Wi-Fi EAP Client flag to use RC4-MD5 Cipher for EAP connection
} sl_wifi_eap_client_flag_t;

/// Auto detect channel
/*
**Note!**
- The channel in which the AP operates. A value of zero enables the Auto Channel Selection (ACS) feature to automatically determine the operating channel.
- If ACS is enabled, AP start may take ~9 seconds as the device scans all channels to select the best channel.
*/
#define SL_WIFI_AUTO_CHANNEL 0

/// API input checks
#define SL_WIFI_ARGS_CHECK_NULL_POINTER(ptr) \
  {                                          \
    if (ptr == NULL) {                       \
      return SL_STATUS_NULL_POINTER;         \
    }                                        \
  }

/// Interface input checks
#define SL_WIFI_ARGS_CHECK_INVALID_INTERFACE(interface)                                         \
  {                                                                                             \
    if (!((interface == SL_WIFI_CLIENT_INTERFACE) || (interface == SL_WIFI_CLIENT_INTERFACE)    \
          || (interface == SL_WIFI_AP_INTERFACE) || (interface == SL_WIFI_2_4GHZ_INTERFACE))) { \
      return SL_STATUS_WIFI_UNKNOWN_INTERFACE;                                                  \
    }                                                                                           \
  }

/** @} */
#endif // _SL_WIFI_CONSTANTS_H_
