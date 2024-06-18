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
#define SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT  0xFFFF
#define SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME  0xFFFF
#define SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT        0xFFFF
#define SL_WIFI_DEFAULT_PASSIVE_CHANNEL_SCAN_TIME 0

/// Wi-Fi transceiver mode configurations
#define MAX_PAYLOAD_LEN                     2020
#define MAC_INFO_ENABLE                     BIT(0)
#define BCAST_INDICATION                    BIT(1)
#define CONFIRM_REQUIRED_TO_HOST            BIT(2)
#define QOS_ENABLE                          BIT(4)
#define MAC80211_HDR_MIN_LEN                24
#define MAC80211_HDR_QOS_CTRL_LEN           2
#define MAC80211_HDR_ADDR4_LEN              6
#define WME_AC_BE                           0 /* best effort */
#define WME_AC_BK                           1 /* background */
#define WME_AC_VI                           2 /* video */
#define WME_AC_VO                           3 /* voice */
#define WME_AC_TO_TID(_ac)                  (((_ac) == WME_AC_VO) ? 6 : ((_ac) == WME_AC_VI) ? 5 : ((_ac) == WME_AC_BK) ? 1 : 0)
#define WME_AC_TO_QNUM(_ac)                 (((_ac) == WME_AC_BK) ? 0 : ((_ac) == WME_AC_BE) ? 1 : ((_ac) == WME_AC_VI) ? 2 : 3)
#define FC_TYPE_DATA                        BIT(3)
#define FC_SUBTYPE_QOS_DATA                 BIT(7)
#define FC_TO_DS                            BIT(8)
#define FC_FROM_DS                          BIT(9)
#define TX_DATA_CTRL_FLAG_QOS_BIT           BIT(1)
#define IS_QOS_PKT(ctrl_flags)              (ctrl_flags & TX_DATA_CTRL_FLAG_QOS_BIT)
#define IS_PEER_DS_SUPPORT_ENABLED(bitmap)  (bitmap & SL_SI91X_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT)
#define IS_4ADDR(ctrl_flags)                (ctrl_flags & BIT(0))
#define IS_FIXED_DATA_RATE(ctrl_flags)      (ctrl_flags & BIT(2))
#define IS_TODS(ctrl_flags)                 (ctrl_flags & BIT(3))
#define IS_FROMDS(ctrl_flags)               (ctrl_flags & BIT(4))
#define IS_CFM_TO_HOST_SET(ctrl_flags)      (ctrl_flags & BIT(5))
#define IS_BCAST_MCAST_MAC(addr)            (addr & BIT(0))
#define IS_MAC_ZERO(mac)                    (!(mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]))
#define MAX_RETRANSMIT_COUNT                15
#define MAX_CW_EXPN_COUNT                   15
#define MAX_AIFSN                           15
#define TRANSCEIVER_PEER_ADD_FLAG           BIT(0)
#define TRANSCEIVER_PEER_AUTO_RATE_FLAG     BIT(1)
#define TRANSCEIVER_MCAST_FILTER_EN         BIT(0)
#define TRANSCEIVER_MCAST_FILTER_ADDR_LIMIT 2
#define TRANSCEIVER_TX_DATA_EXT_DESC_SIZE   4
#define SL_STATUS_ACK_ERR                   0x1
#define SL_STATUS_CS_BUSY                   0x2
#define SL_STATUS_UNKNOWN_PEER              0x3
#define TRANSCEIVER_RX_PKT_TA_MATCH_BIT     BIT(20)
/** @addtogroup SL_WIFI_CONSTANTS
  * @{ */
#define SL_CHANNEL_NO            14  ///< Wi-Fi transceiver default channel
#define SL_TX_POWER              127 ///< Wi-Fi transceiver default TX power
#define DEFAULT_RETRANSMIT_COUNT 15  ///< Wi-Fi transceiver default retransmit count
#define DEFAULT_QOS_BE_CWMIN     4   ///< Wi-Fi transceiver default BE cwmin contention param value
#define DEFAULT_QOS_BE_CWMAX     6   ///< Wi-Fi transceiver default BE cwmax contention param value
#define DEFAULT_QOS_BE_AIFSN     3   ///< Wi-Fi transceiver default BE aifsn contention param value
#define DEFAULT_QOS_BK_CWMIN     4   ///< Wi-Fi transceiver default BK cwmin contention param value
#define DEFAULT_QOS_BK_CWMAX     10  ///< Wi-Fi transceiver default BK cwmax contention param value
#define DEFAULT_QOS_BK_AIFSN     7   ///< Wi-Fi transceiver default BK aifsn contention param value
#define DEFAULT_QOS_VI_CWMIN     3   ///< Wi-Fi transceiver default VI cwmin contention param value
#define DEFAULT_QOS_VI_CWMAX     4   ///< Wi-Fi transceiver default VI cwmax contention param value
#define DEFAULT_QOS_VI_AIFSN     1   ///< Wi-Fi transceiver default VI aifsn contention param value
#define DEFAULT_QOS_VO_CWMIN     2   ///< Wi-Fi transceiver default VO cwmin contention param value
#define DEFAULT_QOS_VO_CWMAX     3   ///< Wi-Fi transceiver default VO cwmax contention param value
#define DEFAULT_QOS_VO_AIFSN     1   ///< Wi-Fi transceiver default VO aifsn contention param value
/** @} */

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
  uint32_t flags;                  ///< Reserved for future use.
  uint32_t periodic_scan_interval; ///< Duration in milliseconds between periodic scans
  uint16_t channel_bitmap_2g4;     ///< Bitmap of selected 2.4GHz channels
  uint32_t channel_bitmap_5g[8];   ///< Bitmap of selected 5GHz channels
  uint8_t lp_mode;                 ///< Enable LP mode, 1 - Enable LP mode, 0 - Disable LP mode
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
  uint8_t beacon_stop;          ///< Stop beaconing when there are no associated clients
  sl_wifi_tdi_t tdi_flags; ///< To enable Transition Disable Indication (TDI). One of the values from @ref sl_wifi_tdi_t
  uint8_t is_11n_enabled;  ///< A flag to enable 11n.
} sl_wifi_ap_configuration_t;

/// Wi-Fi Access Point advanced configuration
typedef struct {
  uint8_t csa_announcement_delay; ///< In beacon periods
  uint32_t tbd;                   ///< Advanced configuration option to be added
} sl_wifi_advanced_ap_configuration_t;

/// Wi-Fi Access Point dynamic configuration
typedef struct {
  uint8_t beacon_stop; ///< Beaconing control when no clients are connected
} sl_si91x_ap_reconfiguration_t;

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
  uint32_t max_retry_attempts;      ///< Maximum number of retries before indicating join failure
  uint32_t scan_interval;           ///< Scan interval in seconds between each retry
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
  /**
	   * @note
	   * - BIT[0] of Opportunistic Key Caching (OKC) is used to enable or disable OKC: –0 – disable; –1 – enable. When this is enabled, module will use cached PMKID to get the Master Session Key (MSK), which is needed for generating PMK that is needed for 4-way handshake.
	   * - BIT[1] of OKC is used to enable or disable CA certification for PEAP connection: –0 – CA certificate is not required; –1 – CA certificate is required.
	   * - BIT[2-12] of OKC argument are used for cipher list selection for EAP connection. All possible ciphers are listed below.
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
	   * @note If a user sets BIT[1] and does not provide the CA certificate for PEAP connection, an error is thrown. If a user provides an invalid CA certificate, an error is also thrown. A user can set either one or multiple bits from BIT[2-12] to provide the cipher list. If a user does not provide any values in OKC BIT[2-12], all ciphers are selected by default.
	   */
  uint32_t eap_flags; ///< EAP Flags of type @ref sl_wifi_eap_client_flag_t
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
    average_tx_throughput; ///< This is the expected average Tx throughput in Kbps. Value ranges from 0 to 10Mbps, which is half of the default [device_average_throughput](https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-wi-fi/sl-wifi-twt-selection-t#device-average-throughput) (20Mbps by default).
  uint32_t
    tx_latency; ///< The allowed latency, in milliseconds, within which the given Tx operation is expected to be completed. If 0 is configured, maximum allowed Tx latency is same as rx_latency. Otherwise, valid values are in the range of [200ms - 6hrs].
  uint32_t
    rx_latency; ///< The maximum latency, in milliseconds, for receiving buffered packets from the AP. The device wakes up at least once for a TWT service period within the configured rx_latency if there are any pending packets destined for the device from the AP. If set to 0, the default latency of 2 seconds is used. Valid range is between 2 seconds to 6 hours. Recommended range is 2 seconds to 60 seconds to avoid connection failures with AP due to longer sleep time.
  uint16_t
    device_average_throughput; ///< Refers to the average Tx throughput that the device is capable of achieving in Kbps. The default value is 20Mbps. Internal SDK use only: do not use.
  uint8_t
    estimated_extra_wake_duration_percent; ///< The percentage by which wake duration is supposed to be overestimated to compensate for bss congestion. Recommended input range is 0 - 50%. The default value is 0. Internal SDK use only: do not use.
  uint8_t
    twt_tolerable_deviation; ///< The allowed deviation percentage of wake duration TWT response. Recommended input range is 0 - 50%. The default value is 10. Internal SDK use only: do not use.
  uint32_t
    default_wake_interval_ms; ///< Default minimum wake interval. Recommended Range: 512ms to 1024ms. The default value is 1024msec. Internal SDK use only: do not use.
  uint32_t
    default_minimum_wake_duration_ms; ///< Default minimum wake interval. Recommended Range: 8ms - 16ms. The default value is 8ms. Internal SDK use only: do not use.
  uint8_t
    beacon_wake_up_count_after_sp; ///< The number of beacons after the service period completion for which the module wakes up and listens for any pending RX. The default value is 2. Internal SDK use only: do not use.
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

/// Wi-Fi Operational Statistics
typedef struct {
  uint8_t operating_mode;       ///< Opermode mode
  uint8_t dtim_period;          ///< DTIM period
  uint8_t ideal_beacon_info[2]; ///< Ideal beacon info
  uint8_t busy_beacon_info[2];  ///< Busy beacon info
  uint8_t beacon_interval[2];   ///< Beacon Interval
} sl_wifi_operational_statistics_t;

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
/**
 * @note
 *   The effective transmit power is subject to regional and device limitations. If the specified transmit power exceeds the
 *   maximum supported value for that region or if the specified transmit power exceeds the maximum supported value of the device, the transmission will occur at the maximum supported transmit power.
*/

typedef struct {
  uint8_t scan_tx_power; ///< Transmit power during scan. Valid input range: 1dBm to 31dBm.
  uint8_t join_tx_power; ///< Transmit power during join. Valid input range: 1dBm to 31dBm.
} sl_wifi_max_tx_power_t;

/// Wi-Fi Multicast filter info
typedef struct {
  sl_wifi_multicast_filter_command_t command_type; ///< Command type for multicast filter operation.
  sl_mac_address_t mac_address;                    ///< MAC address to which the filter has to be applied.
} sl_wifi_multicast_filter_info_t;

/// Wi-Fi station TSF
typedef struct {
  uint32_t tsf_l; ///< Used to store LSB of TSF
  uint32_t tsf_m; ///< Used to store MSB of TSF
} sl_wifi_tsf64_t;

/// Control block structure used to hold meta data for the payload passed in @ref sl_wifi_send_transceiver_data
typedef struct {
  ///       | Bit position | ctrl_flags bit description                                                                                                                     |
  ///       |--------------|------------------------------------------------------------------------------------------------------------------------------------------------|
  ///       | 0            | Shall be set for 4-address packet or unset for 3-address packet. addr4 is ignored if set to 0.                                                 |
  ///       | 1            | Shall be set for QoS packet. QoS control field shall not be present in the MAC header for non-QoS packet. priority is ignored if set to 0.     |
  ///       | 2            | Shall be set to use the fixed data rate provided in the rate field. If set to 0, rate field is ignored and auto rate shall be used.            |
  ///       | 3            | Shall be set to enable ToDS bit in Frame Control. Valid only for 3-addr packet (bit 0 is unset).                                               |
  ///       | 4            | Shall be set to enable FromDS bit in Frame Control. Valid only for 3-addr packet (bit 0 is unset).                                             |
  ///       | 5            | Shall be set if host requires TX data status report. Token is used for synchronization between data packets sent and reports received.         |
  ///       | 6:7          | Reserved.                                                                                                                                      |
  /// @note If addr1 is multicast/broadcast, ctrl_flags bit 1 is ignored and the frame is sent as a non-QoS frame, i.e. QoS control field shall not be present in the MAC header.
  uint8_t ctrl_flags;
  uint8_t reserved1;
  uint8_t reserved2;
  /// Data Packets are queued to respective queue based on priority. Best Effort - 0, Background - 1, Video - 2, Voice - 3.
  uint8_t priority;
  /// Rates shall be provided as per @ref sl_wifi_data_rate_t. Only 11b/g rates shall be supported.
  sl_wifi_data_rate_t rate;
  /// Used for synchronization between data packets sent and reports received. Application shall provide token/identifier per PPDU. MAC layer shall send the same token/identifier in status report along with the status of the transmitted packet.
  uint32_t token;
  /// Receiver MAC address
  uint8_t addr1[6];
  /// Transmitter MAC address
  uint8_t addr2[6];
  /// Destination MAC address
  uint8_t addr3[6];
  /// Source MAC address. Initialization of addr4 is optional.
  uint8_t addr4[6];
} sl_wifi_transceiver_tx_data_control_t;

typedef struct {
  /// Min contention window size. Value is calculated from 2n - 1 where exponent shall be provided as the input. Valid values for exponent N are 0 - 15
  uint8_t cwmin;
  /// Max contention window size. Value is calculated from 2n - 1 where exponent shall be provided as the input. Valid values for exponent N are 0 - 15.
  uint8_t cwmax;
  /// AIFSN. Valid range is 0 to 15.
  uint8_t aifsn;
  uint8_t reserved;
} sl_wifi_transceiver_cw_config_t;

typedef struct {
  /// Shall be set to 1 to configure the transceiver config params in MAC layer. Shall be set to 0 to query the transceiver config params from MAC layer.
  uint8_t set;
  /// Retransmit count. Common across all peers and access categories and valid only for unicast data frames. Valid range is 1 to 15.
  uint8_t retransmit_count;
  /// Reserved
  uint16_t flags;
  /// CW params for respective queues. AC index: Best Effort - 0, Background - 1, Video - 2, Voice - 3
  sl_wifi_transceiver_cw_config_t cw_params[4];
} sl_wifi_transceiver_parameters_t;

typedef struct {
  /// Channel information
  sl_wifi_channel_t chan_info;
  /// TX power
  uint8_t tx_power;
} sl_wifi_transceiver_set_channel_t;

typedef struct {
  /// | Bit position | Flags bit description                                                                                              |
  /// |--------------|--------------------------------------------------------------------------------------------------------------------|
  /// | 0            | Shall be set to add the peer, else reset to 0 to delete the peer.                                                  |
  /// | 1            | Shall be set for auto-rate enable. To enable auto-rate, application needs to provide peer_supported_rate_bitmap    |
  uint8_t flags;
  /// MAC address of peer to be added or deleted.
  uint8_t peer_mac_address[6];
  /// Rate bitmap of peer station
  /// | peer_supported_rate_bitmap | Data rate  |
  /// | :--------------------------| :----------|
  /// | BIT(0)                     | 1 Mbps     |
  /// | BIT(1)                     | 2 Mbps     |
  /// | BIT(2)                     | 5.5 Mbps   |
  /// | BIT(3)                     | 11 Mbps    |
  /// | BIT(4)                     | 6 Mbps     |
  /// | BIT(5)                     | 9 Mbps     |
  /// | BIT(6)                     | 12 Mbps    |
  /// | BIT(7)                     | 18 Mbps    |
  /// | BIT(8)                     | 24 Mbps    |
  /// | BIT(9)                     | 36 Mbps    |
  /// | BIT(10)                    | 48 Mbps    |
  /// | BIT(11)                    | 54 Mbps    |
  /// | BIT(12:31)                 | Reserved   |
  uint32_t peer_supported_rate_bitmap;
} sl_wifi_transceiver_peer_update_t;

typedef struct {
  /// Bit 0 is set to 1 to enable filtering for the specified MAC addresses, else set to 0 to disable filtering.
  uint8_t flags;
  /// Number of multicast addresses. Valid values are 1, 2. This field is ignored when disabling filtering.
  uint8_t num_of_mcast_addr;
  /// List of multicast addresses. This field is ignored when disabling filtering.
  uint8_t mac[2][6];
} sl_wifi_transceiver_mcast_filter_t;

/// Wi-Fi transceiver interface configuration
typedef struct {
  sl_wifi_transceiver_set_channel_t channel;   ///< Channel info
  sl_mac_address_t dut_mac;                    ///< DUT MAC address
  sl_wifi_transceiver_parameters_t parameters; ///< Transceiver parameters
} sl_wifi_transceiver_configuration_t;

/// Wi-Fi transceiver TX data confirm callback structure
typedef struct {
  /// Status report for the data packet identified by token.
  /// | Status                       | Description                                                                                                                         |
  /// | :----------------------------| :-----------------------------------------------------------------------------------------------------------------------------------|
  /// | SL_STATUS_OK (0x0)           | Received Ack                                                                                                                        |
  /// | SL_STATUS_ACK_ERR (0x1)      | Ack error                                                                                                                           |
  /// | SL_STATUS_CS_BUSY (0x2)      | Carrier sense busy                                                                                                                  |
  /// | SL_STATUS_UNKNOWN_PEER (0x3) | If @ref sl_wifi_send_transceiver_data was called for a peer that was not added or was deleted before the data packet was sent out.  |
  sl_status_t status;
  /// Rate at which data packet has been sent. Rate is invalid if error is SL_STATUS_CS_BUSY or SL_STATUS_UNKNOWN_PEER.
  uint32_t rate;
  /// Priority used for the data packet from control->priority in the corresponding call to @ref sl_wifi_send_transceiver_data.
  uint8_t priority;
  /// Data packet identifier from control->token value passed in the corresponding call to @ref sl_wifi_send_transceiver_data.
  uint32_t token;
} sl_wifi_transceiver_tx_data_confirmation_t;

/// Wi-Fi transceiver RX Data callback structure
typedef struct {
  /// Status code for the received RX packet.
  /// | Status                       | Description                                                                                                                          |
  /// | :----------------------------| :------------------------------------------------------------------------------------------------------------------------------------|
  /// | SL_STATUS_OK (0x0)           | Success                                                                                                                              |
  /// | SL_STATUS_UNKNOWN_PEER (0x3) | If SL_SI91X_FEAT_TRANSCEIVER_MAC_PEER_DS_SUPPORT feature is enabled and data packet is received from a peer not present in MAC layer |
  sl_status_t status;
  /// RSSI of the received 802.11 frame. This field is valid only if status is set to success.
  int8_t rssi;
  /// Rate of the received 802.11 frame as per @ref sl_wifi_data_rate_t. This field is valid only if status is set to success.
  uint32_t rate;
  /// Length of the buffer.
  uint32_t length;
  /// IEEE 802.11 frame received from firmware. `buffer` points to the beginning of the MAC header. Contents are not valid once the function returns. If the contents need to be accessed after return, it needs to be copied to the application buffer. If the status is not successful, buffer is set to NULL and shall not be accessed.
  ///
  /// *Format of IEEE 802.11 frame received from firmware in buffer*
  /// | Field name | Frame Control  | Duration | Addr1 | Addr2 | Adddr3 | Seq Ctrl | Addr4                  | QoS ctrl              | Payload (LLC + Data)  |
  /// |:-----------|:---------------|:---------|:------|:------|:-------|:---------|:-----------------------|:----------------------|:----------------------|
  /// | Size(bytes)| 2              | 2        | 6     | 6     | 6      | 2        | 6 (Optionally present) | 2 (Optionally present)| Variable              |
  /// @note All unicast data frames received where Address 1 (RA) matches the device MAC address are passed to the host application.
  /// @note All broadcast data frames are sent to the host.
  /// @note All multicast data frames are sent to the host unless filtering is enabled using @ref sl_wifi_set_transceiver_multicast_filter.
  /// @note On chip duplicate detection is not supported and is expected to be handled by the application.
  /// @note On chip MAC level decryption is not supported.
  uint8_t *buffer;
} sl_wifi_transceiver_rx_data_t;

/** @} */
