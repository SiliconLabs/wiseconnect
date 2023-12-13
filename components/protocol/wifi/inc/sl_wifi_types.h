/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "sl_wifi_constants.h"
#include "sl_wifi_host_interface.h"
#include "sl_ieee802_types.h"
#include "sl_status.h"
#include "sl_ip_types.h"
#include <stdint.h>

// Default Timeout Configuration
#define SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT 0xFFFF
#define SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME 0xFFFF
#define SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT       0xFFFF

/** @addtogroup SL_WIFI_TYPES Types
  * @{ */

/**
 * @typedef sl_wifi_event_handler_t
 * @brief Generic callback for Wi-Fi event
 * @param event
 * Wi-Fi event of type @ref sl_wifi_event_t
 * @param buffer
 * Wi-Fi buffer containing information related to the event of type @ref sl_wifi_buffer_t 
 * @return
 * sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 * In case of event failure, SL_WIFI_FAIL_EVENT_STATUS_INDICATION bit is set in event, data will be of type sl_status_t and data_length can be ignored
 */
typedef sl_status_t (*sl_wifi_event_handler_t)(sl_wifi_event_t event, sl_wifi_buffer_t *buffer);

/// Wi-Fi credential handle
typedef uint32_t sl_wifi_credential_id_t;

/// Wi-Fi channel
typedef struct {
  uint16_t channel;              ///< Channel number
  sl_wifi_band_t band;           ///< Wi-Fi Radio Band
  sl_wifi_bandwidth_t bandwidth; ///< Channel bandwidth
} sl_wifi_channel_t;

/// SSID data structure
typedef struct {
  uint8_t value[32]; ///< SSID value
  uint8_t length;    ///< Length of SSID
} sl_wifi_ssid_t;

/// Roam configuration structure
typedef struct {
  int32_t
    trigger_level; ///< RSSI level to trigger roam algorithm, Setting the value to SL_WIFI_NEVER_ROAM will disable roaming configuration
  uint32_t trigger_level_change; ///< RSSI level delta change to trigger roam algorithm
} sl_wifi_roam_configuration_t;

/// Wi-Fi firmware version information
typedef struct {
  uint8_t chip_id;          ///< Chip ID
  uint8_t rom_id;           ///< ROM ID
  uint8_t major;            ///< Major version number
  uint8_t minor;            ///< Minor version number
  uint8_t security_version; ///< Security enabled or disabled
  uint8_t patch_num;        ///< Patch number
  uint8_t customer_id;      ///< Customer ID
  uint16_t build_num;       ///< Build number
} sl_wifi_firmware_version_t;

/// Wi-Fi scan result
typedef struct {
  uint32_t scan_count; ///< Number of available scan results
  uint32_t reserved;   ///< Reserved
  struct {
    uint8_t rf_channel;    ///< Channel number of the AP
    uint8_t security_mode; ///< Security mode of the AP
    uint8_t rssi_val;      ///< RSSI value of the AP
    uint8_t network_type;  ///< AP network type
    uint8_t ssid[34];      ///< SSID of the AP
    uint8_t bssid[6];      ///< BSSID of the AP
    uint8_t reserved[2];   ///< Reserved bytes for future use
  } scan_info[];           ///< Array of scan result data
} sl_wifi_scan_result_t;

/** Wi-Fi scan configuration.
 *  @note channel_bitmap_2g4 uses the lower 14 bits to represent channels from 1 - 14 where channel 1 = (1 << 0), channel 2 = (1 << 1), etc
 */
typedef struct {
  sl_wifi_scan_type_t type;        ///< Scan type to be configured
  uint32_t flags;                  ///< Flags for the scan configuration
  uint32_t periodic_scan_interval; ///< Duration in milliseconds between periodic scans
  uint16_t channel_bitmap_2g4;     ///< Bitmap of selected 2.4GHz channels
  uint32_t channel_bitmap_5g[8];   ///< Bitmap of selected 5GHz channels
} sl_wifi_scan_configuration_t;

/// Wi-Fi advanced scan configuration options
typedef struct {
  int32_t trigger_level;         ///< RSSI level to trigger advanced scan.
  uint32_t trigger_level_change; ///< RSSI level to trigger advanced scan.
  uint16_t active_channel_time;  ///< Time spent on each channel when performing active scan (milliseconds).
  uint16_t passive_channel_time; ///< Time spent on each channel when performing passive scan (milliseconds).
  uint8_t enable_instant_scan;   ///< Flag to start advanced scan immediately.
  uint8_t
    enable_multi_probe; ///< Flag to indicate to send multiple probes to AP. If set. a probe request would be sent to all access points in addition to connected SSID.
} sl_wifi_advanced_scan_configuration_t;

/// Wi-Fi Access Point configuration
typedef struct {
  sl_wifi_ssid_t ssid;                   ///< SSID
  sl_wifi_security_t security;           ///< Security mode
  sl_wifi_encryption_t encryption;       ///< Encryption mode
  sl_wifi_channel_t channel;             ///< Channel
  sl_wifi_rate_protocol_t rate_protocol; ///< Rate protocol
  sl_wifi_ap_flag_t options;             ///< Optional flags for AP configuration
  sl_wifi_credential_id_t credential_id; ///< ID of secure credentials
  uint8_t
    keepalive_type; ///< Keep alive type of the access point. One of the values from [sl_si91x_ap_keepalive_type_t](../wiseconnect-api-reference-guide-si91x-driver/sl-si91-x-types#sl-si91x-ap-keepalive-type-t)
  uint16_t beacon_interval;     ///< Beacon interval of the access point in milliseconds
  uint32_t client_idle_timeout; ///< Duration in milliseconds to kick idle client
  uint16_t dtim_beacon_count;   ///< How many beacons per DTIM
  uint8_t maximum_clients;      ///< Maximum number of associated clients
} sl_wifi_ap_configuration_t;

/// Wi-Fi Access Point advanced configuration
typedef struct {
  uint8_t csa_announcement_delay; ///< In beacon periods
  uint32_t tbd;                   ///< Advanced configuration option to be added
} sl_wifi_advanced_ap_configuration_t;

/// Wi-Fi Client interface configuration
typedef struct {
  sl_wifi_ssid_t ssid;                   ///< SSID
  sl_wifi_channel_t channel;             ///< Channel
  sl_mac_address_t bssid;                ///< BSSID
  sl_wifi_bss_type_t bss_type;           ///< BSS type
  sl_wifi_security_t security;           ///< Security mode
  sl_wifi_encryption_t encryption;       ///< Encryption mode
  sl_wifi_client_flag_t client_options;  ///< Optional flags for client configuration
  sl_wifi_credential_id_t credential_id; ///< ID of secure credentials
} sl_wifi_client_configuration_t;

/// Wi-Fi Client interface advance configuration
typedef struct {
  /**
	 * @note
	 * - BIT[0] of OKC is used to enable or disable opportunistic key caching (OKC), –0 – disable –1 – enable – When this is enabled, module will use cached PMKID to get MSK(Master Session Key) which is need for generating PMK which is needed for 4-way handshake.
	 * - BIT[1] of OKC is used to enable or disable CA certification for PEAP connection. –0 – CA certificate is not required. –1 – CA certificate is required.
	 * - BIT[2-12] of OKC argument are used for Cipher list selection for EAP connection. All possible ciphers are listed below.
	 *       | BIT position| Cipher selected       |
	 *       |-------------|-----------------------|
	 *       | 2           | DHE-RSA-AES256-SHA256 |
	 *       | 3           | DHE-RSA-AES128-SHA256 |
	 *       | 4           | DHE-RSA-AES256-SHA    |
	 *       | 5           | DHE-RSA-AES128-SHA    |
	 *       | 6           | AES256-SHA256         |
	 *       | 7           | AES128-SHA256         |
	 *       | 8           | AES256-SHA            |
	 *       | 9           | AES128-SHA            |
	 *       | 10          | RC4-SHA               |
	 *       | 11          | DES-CBC3-SHA          |
	 *       | 12          | RC4-MD5               |
	 * - BIT[13-31] of OKC argument is reserved.
	 * @note When user sets BIT[1] and does not provide the CA certificate for PEAP connection then error is thrown. If user provides invalid CA certificate then also error is thrown. User can set either one or multiple bits from BIT[2-12] to provide the cipher's list. When user does not provide any value in OKC's BIT[2-12] then by default all the ciphers are selected.
	 */
  uint32_t eap_flags;               ///< EAP Flags
  uint32_t max_retry_attempts;      ///< Maximum number of retries before indicating join failure
  uint32_t scan_interval;           ///< Scan interval between each retry
  uint32_t beacon_missed_count;     ///< Number of missed beacons that will trigger rejoin
  uint32_t first_time_retry_enable; ///< Retry enable or disable for first time joining
} sl_wifi_advanced_client_configuration_t;

/// Wi-Fi PSK security credentials
typedef struct {
  uint8_t value[SL_WIFI_MAX_PSK_LENGTH]; ///< PSK buffer
} sl_wifi_psk_credential_t;

/// Wi-Fi PMK security credentials
typedef struct {
  uint8_t value[SL_WIFI_MAX_PMK_LENGTH]; ///< PMK buffer
} sl_wifi_pmk_credential_t;

/// Wi-Fi WEP security credentials
typedef struct {
  uint8_t index[2];                                           ///< Index
  uint8_t key[SL_WIFI_WEP_KEY_COUNT][SL_WIFI_WEP_KEY_LENGTH]; ///< WEP Keys
} sl_wifi_wep_credential_t;

/// Wi-Fi Enterprise security credentials
typedef struct {
  uint8_t username[SL_WIFI_EAP_USER_NAME_LENGTH];              ///< Enterprise User Name
  uint8_t password[SL_WIFI_EAP_PASSWORD_LENGTH];               ///< Enterprise password
  uint8_t certificate_key[SL_WIFI_EAP_CERTIFICATE_KEY_LENGTH]; ///< Certificate password
  uint32_t certificate_id;                                     ///< Certificate Id for Enterprise authentication
} sl_wifi_eap_credential_t;

#if defined(__Keil)
#pragma anon_unions
#endif

/// Wi-Fi security credentials
typedef struct {
  sl_wifi_credential_type_t type; ///< Credential type
  union {
    sl_wifi_psk_credential_t psk; ///< WiFi Personal credentials
    sl_wifi_pmk_credential_t pmk; ///< WiFi PMK credentials
    sl_wifi_wep_credential_t wep; ///< WEP keys
    sl_wifi_eap_credential_t eap; ///< Enterprise client credentials
  };                              ///< WiFi Credential structure
} sl_wifi_credential_t;

/// TWT request structure to configure a session
typedef struct {
  uint8_t wake_duration; ///< Nominal minimum wake duration. Range : 0 - 255.
  uint8_t
    wake_duration_tol; ///< Tolerance allowed for wake duration in case of suggest TWT. Received TWT wake duration from AP will be validated against tolerance limits and decided if TWT config received is in acceptable range. Range : 0 - 255.
  uint8_t wake_int_exp; ///< Wake interval exponent to the base 2. Range : 0 - 31.
  uint8_t
    wake_int_exp_tol; ///< Tolerance allowed for wake_int_exp in case of suggest TWT request. Received TWT wake interval exponent from AP will be validated against tolerance limits and decided if TWT config received is in acceptable range. Range : 0 - 31.
  uint16_t wake_int_mantissa; ///< Wake interval mantissa. Range : 0 - 65535.
  uint16_t
    wake_int_mantissa_tol; ///< Tolerance allowed for wake_int_mantissa in case of suggest TWT. Received TWT wake interval mantissa from AP will be validated against tolerance limits and decided if TWT config received is in acceptable range. Range : 0 - 65535.
  uint8_t
    implicit_twt; ///< If enabled (1), the TWT requesting STA calculates the next TWT by adding a fixed value to the current TWT value. Explicit TWT is currently not allowed.
  uint8_t
    un_announced_twt; ///< If enabled (1), the TWT requesting STA does not announce its wake up to AP through PS-POLLs or UAPSD Trigger frames. Values : 0 or 1.
  uint8_t
    triggered_twt; ///< If enabled(1), at least one trigger frame is included in the TWT Service Period(TSP). Values : 0 or 1.
  uint8_t negotiation_type; ///< Negotiation type : 0 - Individual TWT; 1 - Broadcast TWT.
  uint8_t twt_channel;      ///< Currently this configuration is not supported. Range : 0 - 7.
  uint8_t
    twt_protection; ///< If enabled (1), TSP is protected. This is negotiable with AP. Currently this is not supported. Values : 0 or 1.
  uint8_t twt_flow_id; ///< TWT session flow id. 0 - 7 valid. 0xFF to disable all active TWT sessions.
  uint8_t
    restrict_tx_outside_tsp;  ///< 1 - Any Tx outside the TSP is restricted. 0 - TX can happen outside the TSP also.
  uint8_t twt_retry_limit;    ///< TWT retry limit. Range : 0 - 15.
  uint8_t twt_retry_interval; ///< TWT retry interval in seconds between two twt requests. Range : 5 - 255.
  uint8_t req_type;           ///< TWT request type. 0 - Request TWT; 1 - Suggest TWT; 2 - Demand TWT.
  uint8_t twt_enable;         ///< TWT enable. 0 - TWT session teardown; 1 - TWT session setup.
  uint8_t wake_duration_unit; ///< Wake duration unit. 0 - 256 microseconds ; 1 - 1024 microseconds.
} sl_wifi_twt_request_t;

/// TWT request structure to auto select a session
typedef struct {
  uint8_t twt_enable; ///< TWT enable. 0 - TWT session teardown; 1 - TWT session setup.
  uint16_t
    average_tx_throughput; ///< The expected average Tx throughput in Kbps. The value configured should be between 0 and half of device average throughput.
  uint32_t
    tx_latency; ///< The allowed latency, in milliseconds, within which the given Tx operation is expected to be completed. If 0 is configured, maximum allowed Tx latency is same as rx_latency. Otherwise, valid values are in the range of [200ms - 6hrs].
  uint32_t
    rx_latency; ///< The maximum latency for receiving buffered packets from the AP. The device wakes up at least once for a TWT service period within the configured rx_latency if there are any pending packets from the AP. If set to 0, the default latency of 2 seconds is used. Valid range is between 2 seconds to 6 hours.
  uint16_t device_average_throughput; ///< Refers to the average Tx throughput that the device is capable of achieving.
  uint8_t
    estimated_extra_wake_duration_percent; ///< The percentage by which wake duration is supposed to be overestimated to compensate for bss congestion. Valid input range is 0 - 50%.
  uint8_t
    twt_tolerable_deviation; ///< The allowed deviation percentage of wake duration TWT response. Valid input range is 0 - 50%.
  uint32_t default_wake_interval_ms;         ///< Default minimum wake interval. Recommended Range: 512 to 1024ms
  uint32_t default_minimum_wake_duration_ms; ///< Default minimum wake interval. Recommended Range: 8 - 16ms
  uint8_t
    beacon_wake_up_count_after_sp; ///< The number of beacons after the service period completion for which the module wakes up and listens for any pending RX.
} sl_wifi_twt_selection_t;

/// TWT reschedule structure
typedef struct {
  uint8_t flow_id;                            ///< TWT session flow id.
  sl_wifi_reschedule_twt_action_t twt_action; ///< Suspension time for TWT
  uint16_t reserved1;                         ///< Reserved
  uint8_t reserved2;                          ///< Reserved
  uint64_t suspend_duration;                  ///< Duration to suspend respective TWT session
} sl_wifi_reschedule_twt_config_t;

/// Wi-Fi device status
typedef struct {
  uint8_t client_active : 1;       ///< WiFi Client active
  uint8_t ap_active : 1;           ///< WiFi Access point active
  uint8_t monitor_mode_active : 1; ///< WiFi promiscuous mode active
  uint8_t wfd_go_active : 1;       ///< Reserved Status bit
  uint8_t wfd_client_active : 1;   ///< Reserved Status bit
  uint8_t scan_active : 1;         ///< Scan in Progress
  uint8_t _reserved : 1;           ///< Reserved Status bit
  uint8_t _reserved2 : 1;          ///< Reserved Status bit
} sl_wifi_status_t;

/// Wi-Fi interface statistics
typedef struct {
  uint32_t beacon_lost_count; ///< Number of missed beacons
  uint32_t beacon_rx_count;   ///< Number of received beacons
  uint32_t mcast_rx_count;    ///< Multicast packets received
  uint32_t mcast_tx_count;    ///< Multicast packets transmitted
  uint32_t ucast_rx_count;    ///< Unicast packets received
  uint32_t ucast_tx_count;    ///< Unicast packets transmitted
  uint32_t
    overrun_count; ///< Number of packets dropped either at ingress or egress, due to lack of buffer memory to retain all packets.
} sl_wifi_statistics_t;

/// Wi-Fi Direct (P2P) configuration
typedef struct {
  uint16_t group_owner_intent; ///< Group owner intent
  const char *device_name;     ///< Device name
  sl_wifi_channel_t channel;   ///< Wi-Fi channel
  char ssid_suffix[6];         ///< SSID suffix
} sl_wifi_p2p_configuration_t;

/// Wi-Fi event data
typedef union {
  sl_wifi_scan_result_t scan_results; ///< Scan Result structure
  uint32_t join_status;               ///< Join status
} sl_wifi_event_data_t;

/// Wi-Fi WPS PIN object that is an 8 digit number
typedef struct {
  char digits[8]; ///< Array to store digits of WPS Pin
} sl_wifi_wps_pin_t;

/// Wi-Fi listen interval
typedef struct {
  uint32_t listen_interval; ///< Wi-Fi Listen interval in millisecs
} sl_wifi_listen_interval_t;

/// Wi-Fi client info
typedef struct {
  sl_mac_address_t mac_adddress; ///< MAC Address of the client
  sl_ip_address_t ip_address;    ///< IP address of client
} sl_wifi_client_info_t;

/// Wi-Fi client info response
typedef struct {
  uint8_t client_count;                                        ///< Client count
  sl_wifi_client_info_t client_info[SL_WIFI_MAX_CLIENT_COUNT]; ///< Array of client info
} sl_wifi_client_info_response_t;

/// Wi-Fi max transmit power
typedef struct {
  uint8_t join_tx_power;
} sl_wifi_max_tx_power_t;
/** @} */
