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

#include <stdint.h>
#include "sl_ieee802_types.h"
#include "sl_ip_types.h"
#include "sl_wifi_constants.h"

// below defines and structure for CFG_GET: Getting user store configuration.
#define IP_ADDRESS_SZ            4
#define RSI_SSID_LEN             34
#define WISE_PMK_LEN             32
#define MAX_HTTP_SERVER_USERNAME 31
#define MAX_HTTP_SERVER_PASSWORD 31
#define RSI_PSK_LEN              64
#define RSI_MAC_ADDR_LEN         6

// Maximum Acccess points that can be scanned
#define RSI_AP_SCANNED_MAX 11

// Maximum number of stations associated when running as an AP
#define SI91X_MAX_STATIONS 16

// Quick reference: Bit 3 -> DHCPv6 client, Bit 5 -> DHCPv6 server, Bit 17 -> TCP_IP_FEAT_IPV6
// Details: https://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-sapi-reference/opermode#rsi-tcp-ip-feature-bit-map
#define SI91X_IPV6_MODE    (1 << 3) | (1 << 5) | (1 << 17)
#define RSI_IP_ADDRESS_LEN 4
// Maximum payload size
#define RSI_MAX_PAYLOAD_SIZE 1500

#define SL_MAX_FWUP_CHUNK_SIZE 1024
#define SL_RPS_HEADER_SIZE     64
// Firmware upgrade packet types
#define SL_FWUP_RPS_HEADER  1
#define SL_FWUP_RPS_CONTENT 0

// Websocket max url length
#define RSI_WEBS_MAX_URL_LENGTH 51

// Websocket max host length
#define RSI_WEBS_MAX_HOST_LENGTH 51

#ifdef CHIP_917
#define SI91X_MAX_SIZE_OF_EXTENSION_DATA 256
#else
#define SI91X_MAX_SIZE_OF_EXTENSION_DATA 64
#endif

#define SI91X_DNS_REQUEST_MAX_URL_LEN 90

#define SI91X_DNS_RESPONSE_MAX_ENTRIES 10

#define SI91X_MAX_CERT_SEND_SIZE 1400

#define SI91X_HTTP_BUFFER_LEN 1200

/** NOTE: For power save related info
 * https://confluence.silabs.com/pages/viewpage.action?spaceKey=RPD&title=Master+++Power+Save+modes
 * https://confluence.silabs.com/display/RPD/Master+WLAN+APIs#MasterWLANAPIs-rsi_wlan_power_save_profile
 * https://docs.silabs.com/rs9116/wiseconnect/rs9116w-wifi-at-command-prm/latest/wlan-commands#rsi-pwmode----power-mode
 * NOTE: Wake up procedure provided in Confluence is outdated, Please refer AT PRM for it.
 * ****************************** POWER RELATED DEFINES START *******************************/
#define SL_POWER_MODE_DISABLE      0
#define SL_CONNECTED_SLEEP_PS      1
#define SL_CONNECTED_GPIO_BASED_PS 2
#define SL_CONNECTED_MSG_BASED_PS  3

#ifdef RSI_M4_INTERFACE
#define SL_CONNECTED_M4_BASED_PS 4
#endif

#define SL_GPIO_BASED_DEEP_SLEEP 8
#define SL_MSG_BASED_DEEP_SLEEP  9

#ifdef RSI_M4_INTERFACE
#define SL_M4_BASED_DEEP_SLEEP 10
#endif

#define DEFAULT_MONITOR_INTERVAL 50
#define DEFAULT_DTIM_SKIP        0
#define DEFAULT_DTIM_ALIGNMENT   0

#define SL_LP_MODE                       0
#define SL_ULP_WITH_RAM_RETENTION        1
#define SL_ULP_WITHOUT_RAM_RET_RETENTION 2

#define SL_MAX_PSP  0
#define SL_FAST_PSP 1
#define SL_UAPSD    2

/****************************** POWER RELATED DEFINES END ***********************************/

// **************************** Macros for BG scan **************************************/
#define SI91X_BG_SCAN_DISABLE      0
#define SI91X_BG_SCAN_ENABLE       1
#define SI91X_INSTANT_SCAN_DISABLE 0
#define SI91X_INSTANT_SCAN_ENABLE  1

//**************************** Macros for BG end *********************************/
#define NUMBER_OF_BSD_SOCKETS 10

//**************************** Macros for WPS Method request START *********************************/

#define SI91X_SET_WPS_METHOD_PUSH_BUTTON 0
#define SI91X_SET_WPS_METHOD_PIN         1
#define Si91X_SET_WPS_VALIDATE_PIN       0
#define SI91X_SET_WPS_GENERATE_PIN       1
#define SI91X_WPS_PIN_LEN                8

//**************************** Macros for WPS Method request END ***********************************/

//**************************** Macros for JOIN Method request START *********************************/
// To enable b/g only mode in station mode
#define SI91X_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE (1 << 0)

// To take listen interval from join command.
#define SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID (1 << 1)

// To enable quick join feature
#define SI91X_JOIN_FEAT_QUICK_JOIN (1 << 2)

//To enable CCXV2 feature
#define SI91X_JOIN_FEAT_CCXV2_FEATURE (1 << 3)

//To connect to AP based on BSSID together with configured SSID
#define SI91X_JOIN_FEAT_BSSID_BASED (1 << 4)

// MFP Type
#define SI91X_JOIN_FEAT_MFP_CAPABLE_ONLY     (1 << 5)
#define SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED ((1 << 5) | (1 << 6))

// listen interval from power save command
#define SI91X_JOIN_FEAT_PS_CMD_LISTEN_INTERVAL_VALID (1 << 7)

// To take listen interval from join command.
#define SI91X_JOIN_FEAT_BIT_MAP SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID
#define SI91X_LISTEN_INTERVAL   1000

//**************************** Macros for JOIN Method request END ***********************************/

//**************************** Macros for FEATURE frame Method request START *********************************/
#define SI91X_FEAT_FRAME_PREAMBLE_DUTY_CYCLE  (1 << 0)
#define SI91X_FEAT_FRAME_LP_CHAIN             (1 << 4)
#define SI91X_FEAT_FRAME_IN_PACKET_DUTY_CYCLE (1 << 5)

#define PLL_MODE      0
#define RF_TYPE       1 // 0 - External RF 1- Internal RF
#define WIRELESS_MODE 0
#define ENABLE_PPP    0
#define AFE_TYPE      1
#ifndef FEATURE_ENABLES
#define FEATURE_ENABLES \
  (SI91X_FEAT_FRAME_PREAMBLE_DUTY_CYCLE | SI91X_FEAT_FRAME_LP_CHAIN | SI91X_FEAT_FRAME_IN_PACKET_DUTY_CYCLE)
#endif

//**************************** Macros for FEATURE frame Method request END *********************************/

//**************************** Macros for HTTP Client START *********************************/

#define SI91X_HTTP_BUFFER_LEN 1200

#define SI91X_HTTP_CLIENT_MAX_WRITE_BUFFER_LENGTH 900
#define SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH   900
#define SI91X_HTTP_CLIENT_POST_MAX_BUFFER_LENGTH  900

// HTTP client PUT create command
#define SI91X_HTTP_CLIENT_PUT_CREATE 1

// HTTP client PUT START command
#define SI91X_HTTP_CLIENT_PUT_START 2

// HTTP client PUT PKT command
#define SI91X_HTTP_CLIENT_PUT_PKT 3

// HTTP client PUT DELETE command
#define SI91X_HTTP_CLIENT_PUT_DELETE 4

// HTTP client PUT PKT OFFSET command
#define SI91X_HTTP_CLIENT_PUT_OFFSET_PKT 5

//***************************** Macros for HTTP Client End **********************************/

#define SI91X_COUNTRY_CODE_LENGTH  3
#define SI91X_MAX_POSSIBLE_CHANNEL 24

typedef enum { RSI_NONE, RSI_TKIP, RSI_CCMP } sl_si91x_encryption_mode_t;

// Set region command request structure
typedef struct {
  // Enable or disable set region from user: 1-take from user configuration,0-Take from Beacons
  uint8_t set_region_code_from_user_cmd;

  // region code(1-US,2-EU,3-JP,4-World Domain,5-KR)*/
  uint8_t region_code;

  // module type (0- Without on board antenna, 1- With on board antenna)
  uint16_t module_type;
} sl_si91x_set_region_request_t;

// Set region in AP mode command request structure
typedef struct {
  // Enable or disable set region from user: 1-take from user configuration, 0-Take US or EU or JP
  uint8_t set_region_code_from_user_cmd;

  // region code(1-US,2-EU,3-JP)
  uint8_t country_code[SI91X_COUNTRY_CODE_LENGTH];

  uint32_t no_of_rules;

  struct {
    uint8_t first_channel;
    uint8_t no_of_channels;
    uint8_t max_tx_power;
  } channel_info[SI91X_MAX_POSSIBLE_CHANNEL];
} sl_si91x_set_region_ap_request_t;

// Scan command request structure
// channel: RF channel to scan, 0=All, 1-14 for 2.5GHz channels 1-14
typedef struct {
  uint8_t channel[4];
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t _reserved[5];
  uint8_t scan_feature_bitmap;
  uint8_t channel_bit_map_2_4[2];
  uint8_t channel_bit_map_5[4];
} sl_si91x_req_scan_t;

// bg scan command request structure
typedef struct {
  // enable or disable BG scan
  uint16_t bgscan_enable;

  // Is it instant bgscan or normal bgscan
  uint16_t enable_instant_bgscan;

  // bg scan threshold value
  uint16_t bgscan_threshold;

  // tolerance threshold
  uint16_t rssi_tolerance_threshold;

  // periodicity
  uint16_t bgscan_periodicity;

  // active scan duration
  uint16_t active_scan_duration;

  // passive scan duration
  uint16_t passive_scan_duration;

  // multi probe
  uint8_t multi_probe;
} sl_si91x_req_bg_scan_t;

// Scan information response structure
// rf_channel: channel number of the scanned AP
// security_mode: security mode of the scanned AP
// rssi_val: rssi value of the scanned AP
// network_type: network type of the scanned AP
// ssid: SSID of the scanned AP
// bssid: BSSID of the scanned AP
typedef struct {
  uint8_t rf_channel;
  uint8_t security_mode;
  uint8_t rssi_val;
  uint8_t network_type;
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t bssid[RSI_MAC_ADDR_LEN];
  uint8_t reserved[2];
} sl_si91x_scan_info_t;

// Scan command response structure
// scan_count: number of access points scanned
// scan_info: scanned access points information
typedef struct {
  uint8_t scan_count[4];
  uint8_t reserved[4];
  sl_si91x_scan_info_t scan_info[RSI_AP_SCANNED_MAX];
} sl_si91x_rsp_scan_t;

// Antenna select command request structure
typedef struct {
  // Antenna value to set
  uint8_t antenna_value;
  // Antenna 2G gain value
  uint8_t gain_2g;
  // Antenna 5G gain value
  uint8_t gain_5g;
} sl_si91x_antenna_select_t;

// PSK command request  structure
typedef struct {
  // psk type , 1-psk alone, 2-pmk, 3-generate pmk from psk
  uint8_t type;

  // psk or pmk
  uint8_t psk_or_pmk[RSI_PSK_LEN];

  // access point ssid: used for generation pmk
  uint8_t ap_ssid[RSI_SSID_LEN];
} sl_si91x_req_psk_t;

// Enterprise configuration command request structure
typedef struct {
  // EAP method
  uint8_t eap_method[32];

  // Inner method
  uint8_t inner_method[32];

  // Username
  uint8_t user_identity[64];

  // Password
  uint8_t password[128];

  // Opportunistic key caching enable
  int8_t okc_enable[4];

  // Private key password for encrypted private keys
  uint8_t private_key_password[82];
} sl_si91x_req_eap_config_t;

// Set certificate information structure
typedef struct {
  uint16_t total_len;          // total length of the certificate
  uint8_t certificate_type;    // type of certificate
  uint8_t more_chunks;         // more chunks flag
  uint16_t certificate_length; // length of the current segment
  uint8_t certificate_inx;     // index of certificate
  uint8_t key_password[127];   // reserved
} sl_si91x_cert_info_t;

typedef struct {
  uint16_t socket_id;       // Socket ID
  volatile uint16_t status; // certificate valid status
} sl_si91x_req_cert_valid_t;

// Set certificate command request structure
typedef struct {
  sl_si91x_cert_info_t cert_info;                // certificate information structure
  uint8_t certificate[SI91X_MAX_CERT_SEND_SIZE]; // certificate
} sl_si91x_req_set_certificate_t;

// join command request  structure
typedef struct {
  // reserved bytes:Can be used for security Type
  uint8_t reserved1;

  // 0- Open, 1-WPA, 2-WPA2,6-MIXED_MODE
  uint8_t security_type;

  // data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
  uint8_t data_rate;

  // transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
  uint8_t power_level;

  // pre-shared key, 63-byte string , last charecter is NULL
  uint8_t psk[RSI_PSK_LEN];

  // ssid of access point to join to, 34-byte string
  uint8_t ssid[RSI_SSID_LEN];

  // feature bitmap for join
  uint8_t join_feature_bitmap;

  // reserved bytes
  uint8_t reserved2[2];

  // length of ssid given
  uint8_t ssid_len;

  // listen interval
  uint8_t listen_interval[4];

  // vap id, 0 - station mode, 1 - AP mode
  uint8_t vap_id;

  // join bssid for mac based join
  uint8_t join_bssid[6];
} sl_si91x_join_request_t;

// IPV4 ipconfig command request  structure
typedef struct {
  // 0=Manual, 1=Use DHCP
  uint8_t dhcp_mode;

  // IP address of this module if in manual mode
  uint8_t ipaddress[4];

  // Netmask used if in manual mode
  uint8_t netmask[4];

  // IP address of default gateway if in manual mode
  uint8_t gateway[4];

  // DHCP client host name
  uint8_t hostname[31];

  // vap id, 0 - station and 1 - AP
  uint8_t vap_id;
} sl_si91x_req_ipv4_params_t;

// IPV4 ipconfig command response  structure
typedef struct {
  // MAC address of this module
  uint8_t macAddr[6];

  // Configured IP address
  uint8_t ipaddr[4];

  // Configured netmask
  uint8_t netmask[4];

  // Configured default gateway
  uint8_t gateway[4];
} sl_si91x_rsp_ipv4_params_t;

// IPV6 ipconfig command request  structure
typedef struct {
  // 0=Manual, 1=Use DHCP
  uint8_t mode[2];

  // prefix length
  uint8_t prefixLength[2];

  // IPV6 address of the module
  uint8_t ipaddr6[16];

  // address of gateway
  uint8_t gateway6[16];
} sl_si91x_req_ipv6_params_t;

// IPV6 ipconfig command response structure
typedef struct {
  // prefix length
  uint8_t prefixLength[2];

  // Configured IPv address
  uint8_t ipaddr6[16];

  // Router IPv6 address
  uint8_t defaultgw6[16];
} sl_si91x_rsp_ipv6_params_t;

// Structure for firmware upgradation
typedef struct {
  // Type of the packet
  uint16_t type;

  // Length of the packet
  uint16_t length;

  // RPS content
  uint8_t content[SL_MAX_FWUP_CHUNK_SIZE];
} sl_si91x_req_fwup_t;

// wireless information
typedef struct {
  // wlan state: connected or disconnected in station mode
  // wlan state: no of stations connected in AP mode
  uint16_t wlan_state;

  // channel number of connected AP
  uint16_t channel_number;

  // uint8[32], SSID of connected access point
  uint8_t ssid[RSI_SSID_LEN];

  // Mac address
  uint8_t mac_address[6];

  // security type
  uint8_t sec_type;

  // PSK
  uint8_t psk[64];

  // uint8[4], Module IP Address
  uint8_t ipv4_address[4];

  // uint8[16], Module IPv6 Address
  uint8_t ipv6_address[16];

  // reserved1
  uint8_t reserved1[2];

  // reserved2
  uint8_t reserved2[2];
} sl_si91x_rsp_wireless_info_t;

// socket create command request structure
#pragma pack(1)
typedef struct {
  // ip version4 or 6
  uint16_t ip_version;

  // 0=TCP Client, 1=UDP Client, 2=TCP Server (Listening TCP)
  uint16_t socket_type;

  // Our local module port number
  uint16_t local_port;

  // Port number of what we are connecting to
  uint16_t remote_port;

  union {
    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];
  } dest_ip_addr;

  // maximum no of LTCP sockets on same port
  uint16_t max_count;

// type of service
#ifdef CHIP_917
  uint16_t tos;
#else
  uint32_t tos;
#endif

// ssl version select bit map
#ifdef CHIP_917
  uint32_t ssl_bitmap;
#else
  uint8_t ssl_bitmap;

  // ssl ciphers bitmap
  uint8_t ssl_ciphers;
#endif
  // web socket resource name
  uint8_t webs_resource_name[RSI_WEBS_MAX_URL_LENGTH];

  // web socket host name
  uint8_t webs_host_name[RSI_WEBS_MAX_HOST_LENGTH];

  // TCP retries
  uint8_t max_tcp_retries_count;

  // Socket bitmap
  uint8_t socket_bitmap;

  // RX window size
  uint8_t rx_window_size;

  // TCP keepalive initial timeout
  uint16_t tcp_keepalive_initial_time;

  // VAPID
  uint8_t vap_id;

  //socket cert inx
  uint8_t socket_cert_inx;

  //ssl ciphers bitmap
  uint32_t ssl_ciphers_bitmap;

#ifdef CHIP_917
  // ssl extended ciphers bitmap
  uint32_t ssl_ext_ciphers_bitmap;

  // max retransmission timeout value
  uint8_t max_retransmission_timeout_value;
#endif

  // tcp retry transmission timer
  uint8_t tcp_retry_transmit_timer;

  // TCP MSS
  uint16_t tcp_mss;

  uint16_t no_of_tls_extensions;
  uint16_t total_extension_length;
  uint8_t tls_extension_data[SI91X_MAX_SIZE_OF_EXTENSION_DATA];

#ifdef CHIP_917
  uint16_t recv_buff_len;
#endif

} sl_si91x_socket_create_request_t;
#pragma pack()

// socket create command response structure
typedef struct {
  // ip version 4 or 6
  uint8_t ip_version[2];

  // 2 bytes, type of socket created
  uint8_t socket_type[2];

  // 2 bytes socket descriptor, like a file handle, usually 0x00
  uint8_t socket_id[2];

  // 2 bytes, Port number of our local socket
  uint8_t module_port[2];

  uint8_t dst_port[2];
  union {

    // 4 bytes, Our (module) IPv4 Address
    uint8_t ipv4_addr[4];

    // 4 bytes, Our (module) IPv6 Address
    uint8_t ipv6_addr[16];
  } module_ip_addr;

  union {
    // 4 bytes, Our (module) IPv4 Address
    uint8_t ipv4_addr[4];
    // 4 bytes, Our (module) IPv6 Address
    uint8_t ipv6_addr[16];
  } dest_ip_addr;

  // 2 bytes, Remote peer MSS size
  uint8_t mss[2];

  // 4 bytes, Remote peer Window size
  uint8_t window_size[4];
} sl_si91x_socket_create_response_t;

// Socket close command request structure
#pragma pack(1)
typedef struct {
  // 2 bytes, socket that was closed
  uint16_t socket_id;

  // 4 bytes, port number
  uint16_t port_number;
} sl_si91x_socket_close_request_t;
#pragma pack()

// Socket close command response structure
#pragma pack(1)
typedef struct {
  // 2 bytes, socket that was closed
  uint16_t socket_id;

  // 4 bytes, sent bytes count
  uint32_t sent_bytes_count;

  // 2 bytes, port number
  uint16_t port_number;
} sl_si91x_socket_close_response_t;
#pragma pack()

#pragma pack(1)
// send data on socket request structure
typedef struct {
  // ip version 4 or 6
  uint16_t ip_version;

  // socket descriptor of the already opened socket connection
  uint16_t socket_id;

  // length of the data to be sent
  uint32_t length;

  // Data Offset
  uint16_t data_offset;

  // destination port
  uint16_t dest_port;

  union {
    // 4 bytes, IPv4 Address of the remote device
    uint8_t ipv4_address[RSI_IP_ADDRESS_LEN];

    // 4 bytes, IPv6 Address of the remote device
    uint8_t ipv6_address[RSI_IP_ADDRESS_LEN * 4];
  } dest_ip_addr;

  // data buffer to send
  uint8_t send_buffer[];
} sl_si91x_socket_send_request_t;
#pragma pack()

// socket accept request structure
#pragma pack(1)
typedef struct {
  // Socket ID
  uint8_t socket_id;

  // Local port number
  uint16_t source_port;
} sl_si91x_socket_accept_request_t;
#pragma pack()

// LTCP socket establish request structure
#pragma pack(1)
typedef struct {
  uint16_t ip_version;

  // 2 bytes, socket handle
  uint16_t socket_id;

  // 2 bytes, remote port number
  uint16_t dest_port;

  union {

    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];
  } dest_ip_addr;

  // 2 bytes, remote peer MSS size
  uint16_t mss;

  // 4 bytes, remote peer Window size
  uint32_t window_size;

  // source port number
  uint16_t src_port_num;
} sl_si91x_rsp_ltcp_est_t;
#pragma pack()

// disassociate command request structure
#pragma pack(1)
typedef struct {
  // FIXME: Enumerate
  // 0- Module in Client mode, 1- AP mode
  uint16_t mode_flag;

  // client MAC address, Ignored/Reserved in case of client mode
  sl_mac_address_t client_mac_address;
} sl_si91x_disassociation_request_t;
#pragma pack()

// Access point configuration parameters
#pragma pack(1)
typedef struct {
  // channel number of the access point
  uint16_t channel;

  // ssid of the AP to be created
  uint8_t ssid[RSI_SSID_LEN];

  // security type of the Access point
  uint8_t security_type;

  // encryption mode
  uint8_t encryption_mode;

  // password in case of security mode
  uint8_t psk[SL_WIFI_MAX_PMK_LENGTH];

  // Beacon interval of the access point in milliseconds. Allowed values are integers in the range of 100 to 1000 in multiples of 100.
  uint16_t beacon_interval;

  // DTIM period of the access point
  uint16_t dtim_period;

  // This is the bitmap to enable AP keep alive functionality and to select the keep alive type.
  uint8_t ap_keepalive_type;

  // Keep alive time after which AP will disconnect the station if there are no wireless exchanges from station to AP.
  uint8_t ap_keepalive_period;

  // Number of clients supported
  uint16_t max_sta_support;
} sl_si91x_ap_config_request;
#pragma pack()

// wireless information
typedef struct {
  // wlan state: connected or disconnected in station mode
  // wlan state: no of stations connected in AP mode
  uint16_t wlan_state;

  // channel number of connected AP
  uint16_t channel_number;

  // uint8[32], SSID of connected access point
  uint8_t ssid[RSI_SSID_LEN];

  // MAC address
  uint8_t mac_address[6];

  // security type
  uint8_t sec_type;

  // PSK
  uint8_t psk[64];

  // uint8[4], Module IP Address
  uint8_t ipv4_address[4];

  // uint8[16], Module IPv6 Address
  uint8_t ipv6_address[16];

  // reserved1
  uint8_t reserved1[2];

  // reserved2
  uint8_t reserved2[2];
} sl_si91x_wireless_info_t;

// socket information structure
typedef struct {
  // 2 bytes, socket id
  uint8_t sock_id[2];

  // 2 bytes, socket type
  uint8_t sock_type[2];

  // 2 bytes, source port number
  uint8_t source_port[2];

  // 2 bytes, remote port number
  uint8_t dest_port[2];

  union {
    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];
  } dest_ip_address;
} sl_si91x_sock_info_query_t;

// Network params command response structure
typedef struct {
  // uint8, 0=NOT Connected, 1=Connected
  uint8_t wlan_state;

  // channel number of connected AP
  uint8_t channel_number;

  // PSK
  uint8_t psk[64];

  // Mac address
  uint8_t mac_address[6];

  // uint8[32], SSID of connected access point
  uint8_t ssid[RSI_SSID_LEN];

  // 2 bytes, 0=AdHoc, 1=Infrastructure
  uint8_t connType[2];

  // security type
  uint8_t sec_type;

  // uint8, 0=Manual IP Configuration,1=DHCP
  uint8_t dhcpMode;

  // uint8[4], Module IP Address
  uint8_t ipv4_address[4];

  // uint8[4], Module Subnet Mask
  uint8_t subnetMask[4];

  // uint8[4], Gateway address for the Module
  uint8_t gateway[4];

  // number of sockets opened
  uint8_t num_open_socks[2];

  // prefix length for ipv6 address
  uint8_t prefix_length[2];

  // modules ipv6 address
  uint8_t ipv6_address[16];

  // router ipv6 address
  uint8_t defaultgw6[16];

  // BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(0) & BIT(1)=3 - BOTH
  uint8_t tcp_stack_used;

  //sockets information array
  sl_si91x_sock_info_query_t socket_info[10];
} sl_si91x_network_params_response_t;

typedef struct {
  // IP version if the connected client
  uint8_t ip_version[2];

  // Mac Address of the connected client
  uint8_t mac[6];
  union {
    // IPv4 Address of the Connected client
    uint8_t ipv4_address[4];

    // IPv6 Address of the Connected client
    uint8_t ipv6_address[16];
  } ip_address;
} sl_si91x_station_info_t;

// go paramas response structure
typedef struct {
  // SSID of the P2p GO
  uint8_t ssid[RSI_SSID_LEN];

  // BSSID of the P2p GO
  uint8_t mac_address[6];

  // Operating channel of the GO
  uint8_t channel_number[2];

  // PSK of the GO
  uint8_t psk[64];

  // IPv4 Address of the GO
  uint8_t ipv4_address[4];

  // IPv6 Address of the GO
  uint8_t ipv6_address[16];

  // Number of stations Connected to GO
  uint8_t sta_count[2];

  sl_si91x_station_info_t sta_info[SI91X_MAX_STATIONS];
} sl_si91x_client_info_response;

typedef enum {
  START_STATISTICS_REPORT,
  STOP_STATISTICS_REPORT,
} sl_wifi_statistics_report_t;

// per stats command request structure
typedef struct {
  // 0 - start , 1 -stop
  uint8_t start[2];

  // channel number
  uint8_t channel[2];
} sl_si91x_req_rx_stats_t;

//! wlan per stats structure
typedef struct {
  uint8_t tx_pkts[2];
  uint8_t reserved_1[2];
  uint8_t tx_retries[2];
  uint16_t crc_pass;
  uint16_t crc_fail;
  uint8_t cca_stk[2];
  uint8_t cca_not_stk[2];
  uint8_t pkt_abort[2];
  uint8_t fls_rx_start[2];
  uint8_t cca_idle[2];
  uint8_t reserved_2[26];
  uint8_t rx_retries[2];
  uint8_t reserved_3[2];
  uint16_t cal_rssi;
  uint8_t reserved_4[4];
  uint8_t xretries[2];
  uint8_t max_cons_pkts_dropped[2];
  uint8_t reserved_5[2];
  uint8_t bss_broadcast_pkts[2];
  uint8_t bss_multicast_pkts[2];
  uint8_t bss_filter_matched_multicast_pkts[2];
} sl_si91x_per_stats_rsp_t;

// Sending data on specific socket structure
// sock_fd : Socket identifier
// length  : Length of data
// data    : Data
typedef struct {
  uint8_t sock_fd;
  uint32_t length;
  // Need to discuss and increase data size
  uint8_t data[50];
} sl_si91x_socket_send_data_parameters_t;

// Receiving data on specific socket structure
// sock_fd : Socket identifier
// length  : Length of data
// data    : Data
typedef struct {
  uint8_t new_sock_fd;
  uint32_t length;
  uint8_t data[1460];
} sl_si91x_socket_receive_data_parameters_t;

// Socket creation and connection parameters
// domain        : This parameter selects the address family (format of addresses within a domain) that is used. The families supported are 2(AF_INET), 0(AF_INET6)
// type          : Socket protocol types (TCP/UDP/RAW) :: 1(SOCK_STREAM), 2(SOCK_DGRAM), 3(SOCK_RAW)
// protocol      : protocol default : 0(IPPROTO_IP)
typedef struct {
  uint8_t domain;
  uint8_t type;
  uint8_t protocol;
} sl_si91x_socket_create_parameters_t;

// Socket connection parameters
// domain        : This parameter selects the address family (format of addresses within a domain) that is used. The families supported are 2(AF_INET), 0(AF_INET6)
// sock_fd       : socket descriptor
// port          : Port number
// remote_ip_addr: Remote IP address
typedef struct {
  uint8_t domain;
  int8_t sock_fd;
  uint32_t port;
  uint32_t remote_ip_addr;
} sl_si91x_socket_connect_or_listen_parameters_t;

typedef struct {
  uint8_t pll_mode;
  uint8_t rf_type;
  uint8_t wireless_mode;
  uint8_t enable_ppp;
  uint8_t afe_type;
  uint32_t feature_enables;
} sl_si91x_feature_frame_request;

// structure for power save request
typedef struct {
  // power mode to set
  uint8_t power_mode;

  // set LP/ULP/ULP-without RAM retention
  uint8_t ulp_mode_enable;

  // set DTIM aligment required
  // 0 - module wakes up at beacon which is just before or equal to listen_interval
  // 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
  uint8_t dtim_aligned_type;

  // Set PSP type, 0-Max PSP, 1- FAST PSP, 2-APSD
  uint8_t psp_type;

  // Monitor interval for the FAST PSP mode
  // default is 50 ms, and this parameter is valid for FAST PSP only
  uint16_t monitor_interval;
  uint8_t num_of_dtim_skip;
  uint16_t listen_interval;
} sl_si91x_power_save_request_t;

typedef struct {
  //! Ip version value
  uint8_t ip_version[2];

  //! URL name
  uint8_t url_name[SI91X_DNS_REQUEST_MAX_URL_LEN];

  //! DNS servers count
  uint8_t dns_server_number[2];
} sl_si91x_dns_query_request_t;

typedef struct {
  //! Ip version of the DNS server
  uint8_t ip_version[2];

  //! DNS response count
  uint8_t ip_count[2];

  //! DNS address responses
  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } ip_address[SI91X_DNS_RESPONSE_MAX_ENTRIES];
} sl_si91x_dns_response_t;
// Structure for TCP ACK indication
typedef struct {
  // Socket ID
  uint8_t socket_id;

  // Length
  uint8_t length[2];

} sl_si91x_rsp_tcp_ack_t;

// Config command request structure
typedef struct {
  // config type
  uint16_t config_type;

  // value to set
  uint16_t value;
} sl_si91x_config_request_t;

// read bytes coming on socket request structure
typedef struct {
  // socket id
  uint8_t socket_id;

  // requested bytes
  uint8_t requested_bytes[4];

  // Timeout for read
  uint8_t read_timeout[2];
} sl_si91x_req_socket_read_t;

typedef struct {
  // 2 bytes, the ip version of the ip address , 4 or 6
  uint16_t ip_version;

  // 2 bytes, the socket number associated with this read event
  uint16_t socket_id;

  // 4 bytes, length of data received
  uint32_t length;

  // 2 bytes, offset of data from start of buffer
  uint16_t offset;

  // 2 bytes, port number of the device sending the data to us
  uint16_t dest_port;

  union {
    // 4 bytes, IPv4 Address of the device sending the data to us
    uint8_t ipv4_address[4];

    // 4 bytes, IPv6 Address of the device sending the data to us
    uint8_t ipv6_address[16];
  } dest_ip_addr;
} si91x_rsp_socket_recv_t;

typedef struct {
  //! TCP TX + TCP RX + UDP TX + UDP RX
  uint8_t total_sockets;

  //! TCP TX + TCP RX
  uint8_t total_tcp_sockets;

  //! UDP TX + UDP RX
  uint8_t total_udp_sockets;

  //! TCP TX
  uint8_t tcp_tx_only_sockets;

  //! TCP RX
  uint8_t tcp_rx_only_sockets;

  //! UDP TX
  uint8_t udp_tx_only_sockets;

  //! UDP RX
  uint8_t udp_rx_only_sockets;

  //! TCP RX High Performance
  uint8_t tcp_rx_high_performance_sockets;

  //! TCP RX WIndows cap
  uint8_t tcp_rx_window_size_cap;

  //! TCP RX WIndows cap
  uint8_t tcp_rx_window_div_factor;
} sl_si91x_socket_config_t;

typedef struct {
  uint32_t tv_sec;  /* Seconds      */
  uint32_t tv_usec; /* Microseconds */
} sl_si91x_time_value;

/* The select socket array manager.  */
typedef struct {
  uint32_t fd_array[(NUMBER_OF_BSD_SOCKETS + 31) / 32]; /* Bit map of SOCKET Descriptors.*/
  int32_t fd_count;                                     /* How many are SET? */
} sl_si91x_fd_set_t;

typedef struct {
  // Socket ID
  uint8_t num_fd;
  uint8_t select_id;
  sl_si91x_fd_set_t read_fds;
  sl_si91x_fd_set_t write_fds;
  sl_si91x_time_value select_timeout;
  uint8_t no_timeout;
} sl_si91x_socket_select_req_t;

typedef struct {
  // select id
  uint8_t select_id;

  // readfd struct to store select info
  sl_si91x_fd_set_t read_fds;

  // writefd struct to store select info
  sl_si91x_fd_set_t write_fds;

  uint32_t socket_terminate_bitmap;
} sl_si91x_socket_select_rsp_t;

// Structure for OTA firmware upgradation
typedef struct {
  // Type of the packet
  uint8_t ip_version;

  union {
    // 4 bytes, IPv4 Address of the server
    uint8_t ipv4_address[4];

    // 16 bytes, IPv6 Address of the server
    uint8_t ipv6_address[16];

  } server_ip_address;

  // server port
  uint8_t server_port[4];

  // Chunk number
  uint8_t chunk_number[2];

  // Timeout
  uint8_t timeout[2];

  // TCP retry count
  uint8_t retry_count[2];

} sl_si91x_ota_firmware_update_request_t;

// Multicast request structure
typedef struct {
  // IP version
  uint8_t ip_version[2];

  // command type
  uint8_t type[2];

  union {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } multicast_address;
} si91x_req_multicast_t;

typedef struct {
  // wps method: 0 - push button, 1 - pin method
  uint16_t wps_method;

  // If 0 - validate given pin, 1 - generate new pin
  uint16_t generate_pin;

  // wps pin for validation
  uint8_t wps_pin[SI91X_WPS_PIN_LEN];
} sl_si91x_wps_method_request_t;

typedef struct {
  uint32_t roam_enable;     // Enable or disable roaming
  uint32_t roam_threshold;  // roaming threshold
  uint32_t roam_hysteresis; // roaming hysteresis
} sl_si91x_req_roam_params_t;

// Ping Request Frame
typedef struct {
  // ip version
  uint16_t ip_version;

  // ping size
  uint16_t ping_size;

  union {
    // ipv4 address
    uint8_t ipv4_address[4];

    // ipv6 address
    uint8_t ipv6_address[16];

  } ping_address;

  // ping request timeout
  uint16_t timeout;
} sl_si91x_ping_request_t;

//! HTTP Get/Post request structure
typedef struct {
  //! ip version
  uint16_t ip_version;

  //! https enable
  uint16_t https_enable;

  //! port number
  uint16_t port_number;

  //! buffer
  uint8_t buffer[SI91X_HTTP_BUFFER_LEN];
} sl_si91x_http_client_request_t;

#ifdef HTTP_CLIENT_FEATURE
//! HTTP client PUT START create structure
typedef struct {
  //! HTTP server ip version
  uint8_t ip_version;

  //! HTTPS bit map
  uint16_t https_enable;

  //! HTTP server port number
  uint32_t port_number;

  //! HTTP Content Length
  uint32_t content_length;
} __attribute__((__packed__)) sl_si91x_http_client_put_start_t;

typedef struct {
  //! Current chunk length
  uint16_t current_length;
} __attribute__((__packed__)) sl_si91x_http_client_put_data_request_t;

//! HTTP client PUT request structure
typedef struct {
  //! Command type
  uint8_t command_type;

  union {
    //! HTTP PUT START command structure
    sl_si91x_http_client_put_start_t http_client_put_start;

    //! HTTP PUT PACKET command structure
    sl_si91x_http_client_put_data_request_t http_client_put_data_req;

  } http_client_put_struct;

  //! HTTP PUT buffer
  uint8_t http_put_buffer[SI91X_HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH];
} __attribute__((__packed__)) sl_si91x_http_client_put_request_t;

//! HTTP Client POST DATA PKT request structure
typedef struct {
  //! Current http data chunk length
  uint16_t current_length;

  //! HTTP POST buffer
  uint8_t http_post_data_buffer[SI91X_HTTP_CLIENT_POST_MAX_BUFFER_LENGTH];
} sl_si91x_http_client_post_data_request_t;

//! HTTP Client PUT PKT response structure
typedef struct {
  //! Receive HTTP client PUT command type
  uint8_t command_type;

  //! End of resource content file
  uint8_t end_of_file;
} sl_si91x_http_client_put_pkt_rsp_t;

//! HTTP Client PUT pkt server response structure
typedef struct {
  uint32_t command_type;
  uint32_t more;
  uint32_t offset;
  uint32_t data_len;
} sl_si91x_http_put_pkt_server_rsp_t;
#endif

typedef struct {
  uint8_t wake_duration;
  uint8_t wake_duration_unit;
  uint8_t wake_int_exp;
  uint8_t negotiation_type;
  uint16_t wake_int_mantissa;
  uint8_t implicit_twt;
  uint8_t un_announced_twt;
  uint8_t triggered_twt;
  uint8_t twt_channel;
  uint8_t twt_protection;
  uint8_t twt_flow_id;
} sl_si91x_twt_response_t;

typedef struct {
  uint8_t beacon_drop_threshold[2];
  uint8_t filter_bcast_in_tim;
  uint8_t filter_bcast_tim_till_next_cmd;
} sl_si91x_request_wlan_filter_broadcast_t;

//! user configurable gain table structure
typedef struct {
  uint8_t band;      ///< band value
  uint8_t bandwidth; ///< bandwidth value
  uint16_t size;     ///< payload size
  uint32_t reserved;
  uint8_t gain_table[]; ///< payload
} sl_si91x_gain_table_info_t;

typedef struct {
  uint8_t guard_interval; ///< Period of time inserted between two packets in wireless transmission. Range : 0 - 3
  uint8_t nominal_pe;     ///< Nominal Packet extension Range: 0 - 2
  uint8_t dcm_enable;     ///< Enable or disable dual carrier modulation (DCM). 0 - Disable DCM, 1 - Enable DCM
  uint8_t ldpc_enable;    ///< Enable or disable low-density parity-check (LDPC). 0 - Disable LDPC, 1 - Enable LDPC
  uint8_t
    ng_cb_enable; ///< Enable or disable non-contiguous channel bonding (NG CB). 0 - Disable NG CB, 1 - Enable NG CB
  uint8_t ng_cb_values; ///< Values of non-contiguous channel bonding (NG CB). Range: 0x00 - 0x11
  uint8_t
    uora_enable; ///< Enable or disable uplink orthogonal frequency division multiple random access (UORA). 0 - Disable uora, 1 - Enable uora
  uint8_t
    trigger_rsp_ind; ///< Trigger_Response_Indication. BIT(0) ? Trigger Response For BE, BIT(1) ? Trigger Response For BK, BIT(2) ? Trigger Response For VI, BIT(3) ? Trigger Response For VO
  uint8_t ipps_valid_value;
  uint8_t tx_only_on_ap_trig; ///< Reserved for future use
  uint8_t twt_support;        ///< Enable or Disable TWT. 0 - Disable TWT, 1 - Enable TWT
  uint8_t
    config_er_su; ///< Extended Range Single User. 0 - NO ER_SU support, 1 - Use ER_SU rates along with Non_ER_SU rates, 2 - Use ER_SU rates only
} sl_si91x_11ax_config_params_t;

typedef struct {
  uint32_t address;
  uint32_t length;
} sl_si91x_ram_dump_t;

#ifdef RSI_M4_INTERFACE
typedef enum {
  SI91X_TAKE_M4_64K                 = 1,
  SI91X_GET_IPMU_PROGRAMMING_VALUES = 2,
  SI91X_READ_TA_REGISTER            = 3,
  SI91X_WRITE_TA_REGISTER           = 4,
  // This enum varibale added for M4 has to give indication to TA, for Configure the Clock switching between 1.3V to 3.3 .For more details check Jira Ticket RSC-3802.
  SI91X_ENABLE_XTAL = 5,
#ifdef CHIP_917
  SI91X_WRITE_TO_COMMON_FLASH = 6,
#endif
} ta_m4_commands_t;

//  M4 and TA secure handshake request structure.
typedef struct {
  // sub_cmd form the  enum ta_m4_commands_e(Main command type is RSI_COMMON_REQ_TA_M4_COMMANDS)
  ta_m4_commands_t sub_cmd;
  // length of input_data
  uint8_t input_data_size;
  // Input data. In this input data first byte is reserved for enable(1) or Disable(0) sub_cmd of this structure.
  uint8_t input_data[];
} ta_m4_handshake_parameters_t;

#ifdef CHIP_917
#define MAX_CHUNK_SIZE 1400
// TA2M4 handshake request structure.
typedef struct {
  // sub_cmd
  uint8_t sub_cmd;

  // TA flash location
  uint32_t addr;

  // total length of input data
  uint16_t input_buffer_length;

  // erases multiples of 4kbytes
  uint8_t flash_sector_erase_enable;

  //data
  uint8_t input_data[MAX_CHUNK_SIZE];
} __attribute__((__packed__)) sl_si91x_request_ta2m4_t;
#endif // CHIP_917

#endif // RSI_M4_INTERFACE

// tx test mode command request structure
typedef struct {
  uint16_t enable;      ///< enable/disable tx test mode
  uint16_t power;       ///< tx test mode power
  uint32_t rate;        ///< tx test mode rate
  uint16_t length;      ///< tx test mode length
  uint16_t mode;        ///< tx test mode mode
  uint16_t channel;     ///< tx test mode channel
  uint16_t rate_flags;  ///< tx test mode rate_flags
  uint16_t channel_bw;  ///< tx test mode tx test_ch_bw
  uint16_t aggr_enable; ///< tx test mode aggr_enable
  uint16_t reserved;    ///< tx test mode reserved
  uint16_t no_of_pkts;  ///< tx test mode no_of_pkts
  uint32_t delay;       ///< tx test mode delay
#ifdef CHIP_917
  uint8_t enable_11ax;
  uint8_t coding_type;
  uint8_t nominal_pe;
  uint8_t UL_DL;
  uint8_t he_ppdu_type;
  uint8_t beam_change;
  uint8_t BW;
  uint8_t STBC;
  uint8_t Tx_BF;
  uint8_t GI_LTF;
  uint8_t DCM;
  uint8_t NSTS_MIDAMBLE;
  uint8_t spatial_reuse;
  uint8_t BSS_color;
  uint16_t HE_SIGA2_RESERVED;
  uint8_t RU_ALLOCATION;
  uint8_t N_HELTF_TOT;
  uint8_t SIGB_DCM;
  uint8_t SIGB_MCS;
  uint16_t USER_STA_ID;
  uint8_t USER_IDX;
  uint8_t SIGB_COMPRESSION_FIELD;
#endif
} sl_si91x_request_tx_test_info_t;

typedef struct {
  uint32_t max_retry_attempts;      ///< Maximum number of retries before indicating join failure
  uint32_t scan_interval;           ///< Scan interval between each retry
  uint32_t beacon_missed_count;     ///< Number of missed beacons that will trigger rejoin
  uint32_t first_time_retry_enable; ///< Retry enable or disable for first time joining
} sl_si91x_rejoin_params_t;

/// Wi-Fi per stats structure
typedef struct {
  uint16_t tx_pkts;
  uint8_t reserved_1[2];
  uint16_t tx_retries;
  uint16_t crc_pass;
  uint16_t crc_fail;
  uint16_t cca_stk;
  uint16_t cca_not_stk;
  uint16_t pkt_abort;
  uint16_t fls_rx_start;
  uint16_t cca_idle;
  uint8_t reserved_2[26];
  uint16_t rx_retries;
  uint8_t reserved_3[2];
  uint16_t cal_rssi;
  uint8_t reserved_4[4];
  uint16_t xretries;
  uint16_t max_cons_pkts_dropped;
  uint8_t reserved_5[2];
  uint16_t bss_broadcast_pkts;
  uint16_t bss_multicast_pkts;
  uint16_t bss_filter_matched_multicast_pkts;
} sl_si91x_async_stats_response_t;

/// Wi-Fi module state stats stucture
typedef struct {
  uint8_t time_stamp[4];
  uint8_t state_code;
  uint8_t reason_code;
  uint8_t channel;
  uint8_t rssi;
  uint8_t bssid[6];
} sl_si91x_module_state_stats_response_t;

typedef struct {
  uint32_t beacon_lost_count; ///< Number of missed beacons
  uint32_t beacon_rx_count;   ///< Number of received beacons
  uint32_t mcast_rx_count;    ///< Multicast packets received
  uint32_t mcast_tx_count;    ///< Multicast packets transmitted
  uint32_t ucast_rx_count;    ///< Unicast packets received
  uint32_t ucast_tx_count;    ///< Unicast packets transmitted
  uint32_t
    overrun_count; ///< Number of packets dropped either at ingress or egress, due to lack of buffer memory to retain all packets.
} sl_si91x_advance_stats_response_t;
