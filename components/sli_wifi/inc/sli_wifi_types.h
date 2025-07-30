/***************************************************************************/ /**
 * @file    sli_wifi_types.h
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
#ifndef SLI_WIFI_TYPES_H
#define SLI_WIFI_TYPES_H
#include "sl_wifi_device.h"
#include "sl_wifi_constants.h"

#define SLI_WIFI_PSK_LEN                 64
#define SLI_WIFI_SSID_LEN                34
#define SLI_WIFI_WPS_PIN_LEN             8
#define SLI_WIFI_MAX_PMK_LENGTH          64
#define SLI_WIFI_HARDWARE_ADDRESS_LENGTH 6 // Hardware Address Length

/// Efuse data information
typedef union {
  uint8_t mfg_sw_version; ///< Manufacturing PTE software version
  uint16_t pte_crc;       ///< PTE CRC value
} sli_wifi_efuse_data_t;

/// Antenna select command request structure
typedef struct {
  uint8_t antenna_value; ///< Antenna value to set

  uint8_t gain_2g; ///< Antenna 2G gain value

  uint8_t gain_5g; ///< Antenna 5G gain value

} sli_wifi_antenna_select_t;

//! user configurable gain table structure
typedef struct {
  uint8_t band;               ///< band value
  uint8_t bandwidth;          ///< bandwidth value
  uint16_t size;              ///< payload size
  uint8_t x_offset;           ///< X: bump up offset for 52 tone RU
  uint8_t y_offset;           ///< Y: bump up offset for 106 tone RU
  uint8_t gain_table_version; ///< 0: Old gain table, 1:New gain table
  uint8_t reserved;           ///< Reserved
  uint8_t gain_table[];       ///< payload
} sli_wifi_gain_table_info_t;

/// 11AX configuration parameters
typedef struct {
  uint8_t guard_interval; ///< Period of time inserted between two packets in wireless transmission. Range : 0 - 3
  uint8_t nominal_pe;     ///< Nominal Packet extension Range: 0 - 2
  uint8_t dcm_enable;     ///< Enable or disable dual carrier modulation (DCM). 0 - Disable DCM, 1 - Enable DCM
  uint8_t ldpc_enable;    ///< Enable or disable low-density parity-check (LDPC). 0 - Disable LDPC, 1 - Enable LDPC
  uint8_t
    ng_cb_enable; ///< Enable or disable non-contiguous channel bonding (NG CB). 0 - Disable NG CB, 1 - Enable NG CB
  uint8_t ng_cb_values; ///< Values of non-contiguous channel bonding (NG CB). Range: 0x00 - 0x11
  uint8_t
    uora_enable; ///< Enable or disable uplink orthogonal frequency division multiple random access (UORA). 0 - Disable uora, 1 - Enable uora.
  uint8_t
    trigger_rsp_ind; ///< Trigger_Response_Indication. BIT(0) ? Trigger Response For BE, BIT(1) ? Trigger Response For BK, BIT(2) ? Trigger Response For VI, BIT(3) ? Trigger Response For VO
  uint8_t ipps_valid_value;   ///< IPPS valid value
  uint8_t tx_only_on_ap_trig; ///< Reserved for future use
  uint8_t twt_support;        ///< Enable or Disable TWT. 0 - Disable TWT, 1 - Enable TWT.
  uint8_t
    config_er_su; ///< Extended Range Single User. 0 - NO ER_SU support, 1 - Use ER_SU rates along with Non_ER_SU rates, 2 - Use ER_SU rates only
  uint8_t beamformee_support; ///< Flag indicating Beamformee support.
                              /// *        0: Enabled, 1: Disable SU (Single User), 2 : Disable MU (Multi User).
} sli_wifi_11ax_config_params_t;

/// bg scan command request structure
typedef struct {
  /// enable or disable BG scan
  uint16_t bgscan_enable;

  /// Is it instant bgscan or normal bgscan
  uint16_t enable_instant_bgscan;

  /// bg scan threshold value
  uint16_t bgscan_threshold;

  /// tolerance threshold
  uint16_t rssi_tolerance_threshold;

  /// periodicity
  uint16_t bgscan_periodicity;

  /// active scan duration
  uint16_t active_scan_duration;

  /// passive scan duration
  uint16_t passive_scan_duration;

  /// multi probe
  uint8_t multi_probe;
} sli_wifi_request_bg_scan_t;

/// Scan command request structure
typedef struct {
  uint8_t channel[4];              ///< RF channel to scan, 0=All, 1-14 for 2.4 GHz channels 1-14
  uint8_t ssid[SLI_WIFI_SSID_LEN]; ///< SSID to scan, 0=All
  uint8_t pscan_bitmap[4];         ///< Pscan bitmap
  uint8_t _reserved;               ///< Reserved
  uint8_t scan_feature_bitmap;     ///< Scan feature bitmap
  uint8_t channel_bit_map_2_4[2];  ///< Channel bit map for 2.4GHz
  uint8_t channel_bit_map_5[4];    ///< Channel bit map for 5GHz
} sli_wifi_request_scan_t;

/// Enterprise configuration command request structure
typedef struct {
  uint8_t eap_method[32]; ///< EAP method

  uint8_t inner_method[32]; ///< Inner method

  uint8_t user_identity[64]; ///< Username

  uint8_t password[128]; ///< Password

  int8_t okc_enable[4]; ///< Opportunistic key caching enable

  uint8_t private_key_password[82]; ///< Private key password for encrypted private keys

} sli_wifi_request_eap_config_t;

/// join command request  structure
#pragma pack(1)
typedef struct {
  /// reserved bytes:Can be used for security Type
  uint8_t reserved1;

  /// 0- Open, 1-WPA, 2-WPA2,6-MIXED_MODE, 7-WPA3, 8-WP3_Transition
  uint8_t security_type;

  /// data rate, 0=auto, 1=1 Mbps, 2=2 Mbps, 3=5.5Mbps, 4=11 Mbps, 12=54 Mbps
  uint8_t data_rate;

  /// transmit power level, 0=low (6-9 dBm), 1=medium (10-14 dBm, 2=high (15-17 dBm)
  uint8_t power_level;

  /// pre-shared key, 63-byte string , last character is NULL
  uint8_t psk[SLI_WIFI_PSK_LEN];

  /// ssid of access point to join to, 34-byte string
  uint8_t ssid[SLI_WIFI_SSID_LEN];

  /// feature bitmap for join
  uint8_t join_feature_bitmap;

  /// reserved bytes
  uint8_t reserved2[1];

  /// Multiply the listen interval by the configured value in the association request. Default is 1, max recommended is 10. Higher values may cause interoperability issues.
  uint8_t listen_interval_multiplier;

  /// length of ssid given
  uint8_t ssid_len;

  /// listen interval
  uint32_t listen_interval;

  /// vap id, 0 - station mode, 1 - AP mode
  uint8_t vap_id;

  /// join bssid for mac based join
  uint8_t join_bssid[6];
} sli_wifi_join_request_t;
#pragma pack()

/// Roam parameters request
typedef struct {
  uint32_t roam_enable;           ///< Enable or disable roaming
  uint32_t roam_threshold;        ///< roaming threshold
  uint32_t roam_hysteresis;       ///< roaming hysteresis
} sli_wifi_request_roam_params_t; /// roam parameters request

/// Rejoin parameters
typedef struct {
  uint32_t max_retry_attempts; ///< Maximum number of retries before indicating join failure.
  uint32_t scan_interval;      ///< Scan interval between each retry.
  uint32_t
    beacon_missed_count; ///< Number of missed beacons that will trigger rejoin. Minimum value of beacon_missed_count is 40.
  uint32_t first_time_retry_enable; ///< Retry enable or disable for first time joining.
} sli_wifi_rejoin_params_t;

/// WLAN filter broadcast request
typedef struct {
  uint8_t beacon_drop_threshold[2];       ///< Beacon drop threshold
  uint8_t filter_bcast_in_tim;            ///< Filter broadcast in TIM
  uint8_t filter_bcast_tim_till_next_cmd; ///< Filter broadcast TIM till next command
} sli_wifi_request_wlan_filter_broadcast_t;

/// PSK command request  structure
typedef struct {
  uint8_t type;                         ///< psk type , 1-psk alone, 2-pmk, 3-generate pmk from psk
  uint8_t psk_or_pmk[SLI_WIFI_PSK_LEN]; ///< psk or pmk
  uint8_t ap_ssid[SLI_WIFI_SSID_LEN];   ///< access point ssid: used for generation pmk
} sli_wifi_request_psk_t;

/// Access point configuration parameters
#pragma pack(1)
typedef struct {
  /// channel number of the access point
  uint16_t channel;

  /// ssid of the AP to be created
  uint8_t ssid[SLI_WIFI_SSID_LEN];

  /// security type of the Access point
  uint8_t security_type;

  /// encryption mode
  uint8_t encryption_mode;

  /// password in case of security mode
  uint8_t psk[SLI_WIFI_MAX_PMK_LENGTH];

  /// Beacon interval of the access point in milliseconds. Allowed values are integers in the range of 100 to 1000 in multiples of 100.
  uint16_t beacon_interval;

  /// DTIM period of the access point
  uint16_t dtim_period;

  /// This is the bitmap to enable AP keep alive functionality and to select the keep alive type.
  uint8_t ap_keepalive_type;

  /// Keep alive time after which AP will disconnect the station if there are no wireless exchanges from station to AP.
  uint8_t ap_keepalive_period;

  /// Number of clients supported
  uint16_t max_sta_support;
} sli_wifi_ap_config_request;
#pragma pack()

/// High throughputs enable command
typedef struct {
  uint16_t mode_11n_enable; ///< 11n mode enable
  uint16_t ht_caps_bitmap;  ///< HT caps bitmap
} sli_wifi_request_ap_high_throughput_capability_t;

/// disassociate command request structure
#pragma pack(1)
typedef struct {
  /// 0- Module in Client mode, 1- AP mode
  uint16_t mode_flag;

  /// client MAC address, Ignored/Reserved in case of client mode
  sl_mac_address_t client_mac_address;
} sli_wifi_disassociation_request_t;
#pragma pack()

/// WPS method request
typedef struct {
  /// wps method: 0 - push button, 1 - pin method
  uint16_t wps_method;

  /// If 0 - validate given pin, 1 - generate new pin
  uint16_t generate_pin;

  /// wps pin for validation
  uint8_t wps_pin[SLI_WIFI_WPS_PIN_LEN];
} sli_wifi_wps_method_request_t;

/// per stats command request structure
typedef struct {
  /// 0 - start , 1 -stop
  uint8_t start[2];

  /// channel number
  uint8_t channel[2];
} sli_wifi_request_rx_stats_t;

typedef struct {
  sl_wifi_performance_profile_v2_t wifi_performance_profile;
  sl_bt_performance_profile_t bt_performance_profile;
  sl_wifi_system_coex_mode_t coex_mode;
} sli_wifi_performance_profile_t;

/// Si91x specific buffer queue structure
typedef struct {
  sl_wifi_buffer_t *head; ///< Head
  sl_wifi_buffer_t *tail; ///< Tail
} sli_wifi_buffer_queue_t;
/// Si91x specific command type
typedef enum {
  SI91X_COMMON_CMD      = 0, ///< SI91X Common Command
  SLI_SI91X_WLAN_CMD    = 1, ///< SI91X Wireless LAN Command
  SLI_SI91X_NETWORK_CMD = 2, ///< SI91X Network Command
  SLI_SI91X_BT_CMD      = 3, ///< SI91X Bluetooth Command
  SLI_SI91X_SOCKET_CMD  = 4, ///< SI91X Socket Command
  SI91X_CMD_MAX         = 5  ///< SI91X Maximum Command value
} sli_wifi_command_type_t;

/// Si91x queue packet structure
typedef struct {
  sl_wifi_buffer_t *host_packet;        ///< Si91x host buffer
  uint8_t firmware_queue_id;            ///< Si91x firmware queue id
  sli_wifi_command_type_t command_type; ///< Si91x command type
  //  uint16_t packet_id;                   ///< Packet id, used internally to track packets
  uint8_t flags;              ///< One of the values from Si91x packet response flags
  uint16_t frame_status;      ///< Si91x command status
  void *sdk_context;          ///< SDK context, unused internally to invoke user callbacks
  uint32_t command_timeout;   ///< Si91x command timeout
  uint32_t command_tickcount; ///< Command_tickcount stores the tickcount when the command is given to the bus thread
  uint32_t event_mask;        ///< Bitmask to notify handler threads when a packet is added to the rx_queue.
} sli_si91x_queue_packet_t;

/// Structure to represent a command queue
typedef struct {
  sli_wifi_buffer_queue_t tx_queue;    ///< TX queue
  sli_wifi_buffer_queue_t rx_queue;    ///< RX queue
  sli_wifi_buffer_queue_t event_queue; ///< Event queue
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
  bool is_queue_initialiazed;          ///< indicates queue is initialiazed or not.
  uint32_t event_mask;                 ///< Bitmask to notify handler threads when a packet is added to the rx_queue.
} sli_wifi_command_queue_t;

// Scan Information
typedef struct sli_scan_info_s {
  struct sli_scan_info_s *next;
  uint8_t channel;                                 ///< Channel number of the AP
  uint8_t security_mode;                           ///< Security mode of the AP
  uint8_t rssi;                                    ///< RSSI value of the AP
  uint8_t network_type;                            ///< AP network type
  uint8_t ssid[34];                                ///< SSID of the AP
  uint8_t bssid[SLI_WIFI_HARDWARE_ADDRESS_LENGTH]; ///< BSSID of the AP
} sli_scan_info_t;

#endif