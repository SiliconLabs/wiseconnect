/*******************************************************************************
* @file  rsi_wlan_config.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file         rsi_wlan_config.h
 * @version      0.1
 * @date         25 Feb 2021
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device
 *
 *  @section Description  This file contains user configurable details to configure the device
 *
 *
 */
#pragma once

#include "rsi_wlan_defines.h"

// global variables for various feature settings, referenced
// in #defines below and set via rsi_device_init_command
extern uint32_t g_rsi_feature_bit_map;
extern uint32_t g_rsi_tcp_ip_feature_bit_map;
extern uint32_t g_rsi_custom_feature_bit_map;
extern uint32_t g_rsi_ext_custom_feature_bit_map;
extern uint32_t g_rsi_bt_feature_bit_map;
extern uint32_t g_rsi_ext_tcp_ip_feature_bit_map;
extern uint32_t g_rsi_ble_feature_bit_map;
extern uint32_t g_rsi_ble_custom_ext_feature_bit_map;
extern uint32_t g_rsi_config_feature_bit_map;
extern uint8_t g_rsi_band;

//globals for scan feature settings, referenced in #defines below
//and set via rsi_wlan_scan_features_command
extern uint16_t g_rsi_scan_channel_bitmap_2_4;
extern uint32_t g_rsi_scan_channel_bitmap_5_0;
extern uint8_t g_rsi_scan_add_feature;

#define EAP_METHOD_LEN       32
#define EAP_INNER_METHOD_LEN 32
#define EAP_PASSWORD_LEN     82

//globals for eap settings, referenced in #defines below
//and set via rsi_wlan_eap_method
extern uint8_t g_rsi_eap_method[EAP_METHOD_LEN];
extern uint8_t g_rsi_eap_inner_method[EAP_INNER_METHOD_LEN];
extern uint8_t g_rsi_private_key_password[EAP_PASSWORD_LEN];
extern uint32_t g_rsi_okc_value;

//globals for set region, referenced in #defines below

extern uint8_t g_rsi_set_region;
extern uint8_t g_rsi_region_code;
extern uint16_t g_rsi_module_type;

//globals for rejoin params, referenced in #defines below

extern uint32_t g_rsi_rejoin_max_retry;
extern uint32_t g_rsi_rejoin_scan_interval;
extern uint32_t g_rsi_rejoin_beacon_missed_count;
extern uint32_t g_rsi_rejoin_first_time_retry;

//globals for join api, referenced in #defines below
//and set via rsi_wlan_connect method
extern uint8_t g_rsi_power_level;
extern uint8_t g_rsi_join_feat_bit_map;
extern uint32_t g_rsi_listen_interval;
extern uint8_t g_rsi_data_rate;

//globals for bg scan api, referenced in #defines below
extern uint16_t g_rsi_bg_scan_enable;
//instant BG scan
extern uint16_t g_rsi_instant_bg;
//BG scan threshold value
extern uint16_t g_rsi_bg_scan_threshold;
//RSSI tolerance Threshold
extern uint16_t g_rsi_rssi_tolerance_threshold;
//BG scan periodicity
extern uint16_t g_rsi_bg_scan_periodicity;
// Active scan duration
extern uint16_t g_rsi_active_scan_duration;
//Passive scan duration
extern uint16_t g_rsi_passive_scan_duration;
// Multi probe
extern uint8_t g_rsi_multiprobe;

//globals for roaming api, referenced in #defines below
extern uint32_t g_rsi_roaming_threshold;
extern uint32_t g_rsi_roaming_hysterisis;

extern uint32_t g_rsi_ssl_ciphers;

//globals for timeout api, referenced in #defines below
extern uint32_t g_rsi_timeout_bit_map;
extern uint16_t g_rsi_timeout_value;

//globals for socket limit, referenced in #defines below
extern uint8_t g_rsi_ltcp_sockets;
extern uint8_t g_rsi_client_sockets;

//globals for WMM tos, referenced in #defines below
extern uint8_t g_rsi_tos;

//globals for AP mode set region, referenced in #defines below
extern uint8_t g_region_config;
extern uint8_t g_region_code[4];

//globals for power save parameters, referenced in #defines below
extern uint8_t g_dtim_aligned_type;
extern uint16_t g_monitor_interval;
extern uint8_t g_num_of_dtim_skip;

extern uint32_t g_tx_wait_time;

extern uint16_t g_rsi_ht_caps_bitmap;

//globals for rsi_send_feature_frame parameters, referenced in #defines below
extern uint8_t g_pll_mode;
extern uint8_t g_rf_type;
extern uint8_t g_wireless_mode;
extern uint8_t g_enable_ppp;
extern uint8_t g_afe_type;
extern uint32_t g_features_enable;
extern uint32_t g_rsi_app_config_feature_bitmap;

extern uint8_t g_common_log_level;
extern uint8_t g_cm_pm_log_level;
extern uint8_t g_wlan_lmac_log_level;
extern uint8_t g_wlan_umac_log_level;
extern uint8_t g_wlan_netstack_log_level;
extern uint8_t g_bt_ble_ctrl_log_level;
extern uint8_t g_bt_ble_stack_log_level;

//globals for rsi_socket_config() apis, referenced in #defined below
extern uint8_t g_total_sockets;
extern uint8_t g_total_tcp_sockets;
extern uint8_t g_total_udp_sockets;
extern uint8_t g_tcp_tx_only_sockets;
extern uint8_t g_tcp_rx_only_sockets;
extern uint8_t g_udp_tx_only_sockets;
extern uint8_t g_udp_rx_only_sockets;
extern uint8_t g_tcp_rx_high_perfornamce_sockets;
extern uint8_t g_tcp_rx_window_size_cap;
extern uint8_t g_tcp_rx_window_div_factor;

//Parameter to set Config Threshold
extern uint16_t config_threshold;
//Parameter to set RTS Threshold
extern uint16_t rts_threshold;

// Enable feature
#define RSI_ENABLE 1
// Disable feature
#define RSI_DISABLE 0

// opermode command parameters
/*=======================================================================*/
// To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP (g_rsi_feature_bit_map)

// TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS RSI_DISABLE

// TCP/IP feature select bitmap for selecting TCP/IP features
#define RSI_TCP_IP_FEATURE_BIT_MAP (g_rsi_tcp_ip_feature_bit_map)

// To set custom feature select bit map ;
#define RSI_CUSTOM_FEATURE_BIT_MAP (g_rsi_custom_feature_bit_map)

// To set Extended custom feature select bit map
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (g_rsi_ext_custom_feature_bit_map)

#define RSI_EXT_TCPIP_FEATURE_BITMAP (g_rsi_ext_tcp_ip_feature_bit_map)

#define RSI_BT_FEATURE_BITMAP (g_rsi_bt_feature_bit_map)

#define RSI_BLE_FEATURE_BITMAP (g_rsi_ble_feature_bit_map)

#define RSI_BLE_CUSTOM_EXT_FEATURE_BITMAP (g_rsi_ble_custom_ext_feature_bit_map)

#define RSI_CONFIG_FEATURE_BITMAP (g_rsi_config_feature_bit_map)

/*=======================================================================*/

// Feature frame parameters
/*=======================================================================*/
//#define PLL_MODE      0
//#define RF_TYPE       1  // 0 - External RF 1- Internal RF
//#define WIRELESS_MODE 12 // HP mode - 0 ;LP mode - 12
//#define ENABLE_PPP    0
//#define AFE_TYPE      1
//#define FEATURE_ENABLES  (RSI_FEAT_FRAME_PREAMBLE_DUTY_CYCLE | RSI_FEAT_FRAME_LP_CHAIN | RSI_FEAT_FRAME_IN_PACKET_DUTY_CYCLE)

#define PLL_MODE        g_pll_mode
#define RF_TYPE         g_rf_type
#define WIRELESS_MODE   g_wireless_mode
#define ENABLE_PPP      g_enable_ppp
#define AFE_TYPE        g_afe_type
#define FEATURE_ENABLES g_features_enable

// Band command parameters
/*=======================================================================*/

// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_BAND (g_rsi_band)

/*=======================================================================*/

// set region command parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Set region support
#define RSI_SET_REGION_SUPPORT RSI_ENABLE //@ RSI_ENABLE or RSI_DISABLE set region

// If 1:region configurations taken from user ;0:region configurations taken from beacon
#define RSI_SET_REGION_FROM_USER_OR_BEACON (g_rsi_set_region)

// 0-Default Region domain ,1-US, 2-EUROPE, 3-JAPAN
#define RSI_REGION_CODE (g_rsi_region_code)

// 0- Without On Board Antenna , 1- With On Board Antenna
#define RSI_MODULE_TYPE (g_rsi_module_type)

/*=======================================================================*/

// set region AP command parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Set region AP support
#define RSI_SET_REGION_AP_SUPPORT RSI_ENABLE

// If 1:region configurations taken from user ;0:region configurations taken from firmware
#define RSI_SET_REGION_AP_FROM_USER (g_region_config)

// "US" or "EU" or "JP" or other region codes
#define RSI_COUNTRY_CODE (g_region_code)

/*=======================================================================*/

// Rejoin parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE rejoin parameters
#define RSI_REJOIN_PARAMS_SUPPORT RSI_ENABLE

// Rejoin retry count. If 0 retries infinity times
#define RSI_REJOIN_MAX_RETRY (g_rsi_rejoin_max_retry)

// Periodicity of rejoin attempt
#define RSI_REJOIN_SCAN_INTERVAL (g_rsi_rejoin_scan_interval)

// Beacon missed count
#define RSI_REJOIN_BEACON_MISSED_COUNT (g_rsi_rejoin_beacon_missed_count)

// RSI_ENABLE or RSI_DISABLE retry for first time join failure
#define RSI_REJOIN_FIRST_TIME_RETRY (g_rsi_rejoin_first_time_retry)

/*=======================================================================*/

//BG scan command parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE BG Scan support
//#define RSI_BG_SCAN_SUPPORT RSI_DISABLE

#define RSI_BG_SCAN_SUPPORT RSI_ENABLE

#define RSI_BG_SCAN_ENABLE (g_rsi_bg_scan_enable)

// RSI_ENABLE or RSI_DISABLE instant BG scan
#define RSI_INSTANT_BG (g_rsi_instant_bg)

// BG scan threshold value
#define RSI_BG_SCAN_THRESHOLD (g_rsi_bg_scan_threshold)

// RSSI tolerance Threshold
#define RSI_RSSI_TOLERANCE_THRESHOLD g_rsi_rssi_tolerance_threshold

// BG scan periodicity
#define RSI_BG_SCAN_PERIODICITY g_rsi_bg_scan_periodicity

// Active scan duration
#define RSI_ACTIVE_SCAN_DURATION g_rsi_active_scan_duration

// Passive scan duration
#define RSI_PASSIVE_SCAN_DURATION g_rsi_passive_scan_duration

// Multi probe
#define RSI_MULTIPROBE g_rsi_multiprobe
/*=======================================================================*/

//Roaming parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Roaming support
#define RSI_ROAMING_SUPPORT RSI_ENABLE

// roaming threshold value
#define RSI_ROAMING_THRESHOLD g_rsi_roaming_threshold

// roaming hysteresis value
#define RSI_ROAMING_HYSTERISIS g_rsi_roaming_hysterisis

/*=======================================================================*/

// High Throughput capabilities related information
/*=======================================================================*/
//// HT caps supported
#define RSI_HT_CAPS_NUM_RX_STBC      (1 << 8)
#define RSI_HT_CAPS_SHORT_GI_20MHZ   BIT(5)
#define RSI_HT_CAPS_GREENFIELD_EN    BIT(4)
#define RSI_HT_CAPS_SUPPORT_CH_WIDTH BIT(1)
//
//// HT caps bit map.
//#define RSI_HT_CAPS_BIT_MAP  (RSI_HT_CAPS_NUM_RX_STBC | RSI_HT_CAPS_SHORT_GI_20MHZ | RSI_HT_CAPS_GREENFIELD_EN)

#define RSI_HT_CAPS_BIT_MAP g_rsi_ht_caps_bitmap
/*=======================================================================*/

// Scan command parameters
/*=======================================================================*/

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_2_4 (g_rsi_scan_channel_bitmap_2_4)

// scan channel bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_5 (g_rsi_scan_channel_bitmap_5_0)

// scan_feature_bitmap ,valid only if specific channel to scan and ssid are given
#define RSI_SCAN_FEAT_BITMAP 0

/*=======================================================================*/

// Enterprise configuration command parameters
/*=======================================================================*/

#define RSI_EAP_METHOD (&g_rsi_eap_method)
// This parameter is used to configure the module in Enterprise security mode
#define RSI_EAP_INNER_METHOD (&g_rsi_eap_inner_method)
// Private Key Password is required for encrypted private key, format is like "\"12345678\""
#define RSI_PRIVATE_KEY_PASSWORD (&g_rsi_private_key_password)
#define OKC_VALUE                g_rsi_okc_value

#if 0
// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#define RSI_EAP_METHOD           "TLS"
// This parameter is used to configure the module in Enterprise security mode
#define RSI_EAP_INNER_METHOD     "\"auth=MSCHAPV2\""
// Private Key Password is required for encrypted private key, format is like "\"12345678\""
#define RSI_PRIVATE_KEY_PASSWORD ""

#endif
/*=======================================================================*/

// AP configuration command parameters
/*=======================================================================*/

// This Macro is used to enable AP keep alive functionality
#define RSI_AP_KEEP_ALIVE_ENABLE RSI_ENABLE

// This parameter is used to configure keep alive type
#define RSI_AP_KEEP_ALIVE_TYPE RSI_NULL_BASED_KEEP_ALIVE

// This parameter is used to configure keep alive period
#define RSI_AP_KEEP_ALIVE_PERIOD 100

// This parameter is used to configure maximum stations supported
#define RSI_MAX_STATIONS_SUPPORT 4
/*=======================================================================*/

// Join command parameters
/*=======================================================================*/

// Tx power level
#define RSI_POWER_LEVEL g_rsi_power_level
//To enable specific join features
#define RSI_JOIN_FEAT_BIT_MAP g_rsi_join_feat_bit_map
//listen interval
#define RSI_LISTEN_INTERVAL g_rsi_listen_interval
//data rate
#define RSI_DATA_RATE g_rsi_data_rate

/*=======================================================================*/

// Ipconf command parameters
/*=======================================================================*/

// DHCP client host name
#define RSI_DHCP_HOST_NAME "dhcp_client"

// Transmit test command parameters
/*=======================================================================*/
// TX TEST rate flags
#define RSI_TX_TEST_RATE_FLAGS 0

// TX TEST per channel bandwidth
#define RSI_TX_TEST_PER_CH_BW 0

// TX TEST aggregation enable or disable
#define RSI_TX_TEST_AGGR_ENABLE RSI_DISABLE

// TX TEST delay
#define RSI_TX_TEST_DELAY 0

/*=======================================================================*/

// ssl parameters
/*=======================================================================*/
// ssl version
#define RSI_SSL_VERSION PROTOCOL_TLS_1_2

// ssl ciphers
#define RSI_SSL_CIPHERS (g_rsi_ssl_ciphers)

// Enable TCP over SSL with TLS version depends on remote side
#define PROTOCOL_DFLT_VERSION BIT(0)

// Enable TCP over SSL with TLS version 1.0
#define PROTOCOL_TLS_1_0 (BIT(0) | BIT(13))

// Enable TCP over SSL with TLS version 1.1
#define PROTOCOL_TLS_1_1 (BIT(0) | BIT(14))

// Enable TCP over SSL with TLS version 1.2
#define PROTOCOL_TLS_1_2 (BIT(0) | BIT(15))
/*=======================================================================*/

// Power save command parameters
/*=======================================================================*/
// set handshake type of power mode
//#define RSI_HAND_SHAKE_TYPE GPIO_BASED

/*
// 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
#define RSI_SELECT_LP_OR_ULP_MODE RSI_ULP_WITH_RAM_RET

// set DTIM aligment required
// 0 - module wakes up at beacon which is just before or equal to listen_interval
// 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#define RSI_DTIM_ALIGNED_TYPE 0

// Monitor interval for the FAST PSP mode
// default is 50 ms, and this parameter is valid for FAST PSP only
#define RSI_MONITOR_INTERVAL 50

// Number of DTIMs to skip during powersave
#define RSI_NUM_OF_DTIM_SKIP 0

//WMM PS parameters
// set WMM enable or disable
#define RSI_WMM_PS_ENABLE RSI_DISABLE

// set WMM enable or disable
// 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_TYPE 0

// set WMM wake up interval
#define RSI_WMM_PS_WAKE_INTERVAL 20

// set WMM UAPSD bitmap
#define RSI_WMM_PS_UAPSD_BITMAP 15
*/

/*=======================================================================*/

// Socket configuration
/*=======================================================================*/
// RSI_ENABLE or RSI_DISABLE High performance socket
#define HIGH_PERFORMANCE_ENABLE RSI_ENABLE //@ RSI_ENABLE or RSI_DISABLE High performance socket

#define TOTAL_SOCKETS                   1  //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#define TOTAL_TCP_SOCKETS               1  //@ Total TCP sockets. TCP TX + TCP RX
#define TOTAL_UDP_SOCKETS               0  //@ Total UDP sockets. UDP TX + UDP RX
#define TCP_TX_ONLY_SOCKETS             0  //@ Total TCP TX only sockets. TCP TX
#define TCP_RX_ONLY_SOCKETS             1  //@ Total TCP RX only sockets. TCP RX
#define UDP_TX_ONLY_SOCKETS             0  //@ Total UDP TX only sockets. UDP TX
#define UDP_RX_ONLY_SOCKETS             0  //@ Total UDP RX only sockets. UDP RX
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1  //@ Total TCP RX High Performance sockets
#define TCP_RX_WINDOW_SIZE_CAP          10 //@ TCP RX Window size
#define TCP_RX_WINDOW_DIV_FACTOR        10 //@ TCP RX Window division factor
/*=======================================================================*/

// Socket Create parameters
/*=======================================================================*/

// Initial timeout for Socket
#define RSI_SOCKET_KEEPALIVE_TIMEOUT 1200

// VAP ID for Concurrent mode
#define RSI_VAP_ID 0

// Timeout for join or scan
/*=======================================================================*/

//RSI_ENABLE or RSI_DISABLE Timeout support
#define RSI_TIMEOUT_SUPPORT RSI_ENABLE

#define RSI_TIMEOUT_BIT_MAP g_rsi_timeout_bit_map
#define RSI_TIMEOUT_VALUE   g_rsi_timeout_value

// Timeout for ping request
/*=======================================================================*/

//Timeout for PING_REQUEST
#define RSI_PING_REQ_TIMEOUT_MS 1000

// Store Config Profile parameters
/*=======================================================================*/

// Client profile
#define RSI_WLAN_PROFILE_CLIENT 0
// P2P profile
#define RSI_WLAN_PROFILE_P2P 1
// EAP profile
#define RSI_WLAN_PROFILE_EAP 2
// AP profile
#define RSI_WLAN_PROFILE_AP 6
// All profiles
#define RSI_WLAN_PROFILE_ALL 0xFF

// AP Config Profile Parameters
/*==============================================================================*/

// Transmission data rate. Physical rate at which data has to be transmitted.
#define RSI_CONFIG_AP_DATA_RATE RSI_DATA_RATE_AUTO
// To set wlan feature select bit map
#define RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP (FEAT_SECURITY_PSK)
// TCP/IP feature select bitmap for selecting TCP/IP features
#define RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP (TCP_IP_FEAT_DHCPV4_SERVER)
// To set custom feature select bit map
#define RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP 0
// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_AP_BAND RSI_BAND_2P4GHZ
// Tx power level
#define RSI_CONFIG_AP_TX_POWER RSI_POWER_LEVEL_HIGH
// AP SSID
#define RSI_CONFIG_AP_SSID "SILABS_AP"
// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_AP_BAND RSI_BAND_2P4GHZ
// To configure AP channel number
#define RSI_CONFIG_AP_CHANNEL 6
// To configure security type
#define RSI_CONFIG_AP_SECURITY_TYPE RSI_WPA
// To configure encryption type
#define RSI_CONFIG_AP_ENCRYPTION_TYPE 1
// To configure PSK
#define RSI_CONFIG_AP_PSK "1234567890"
// To configure beacon interval
#define RSI_CONFIG_AP_BEACON_INTERVAL 100
// To configure DTIM period
#define RSI_CONFIG_AP_DTIM 2
// This parameter is used to configure keep alive type
#define RSI_CONFIG_AP_KEEP_ALIVE_TYPE 0 // RSI_NULL_BASED_KEEP_ALIVE

#define RSI_CONFIG_AP_KEEP_ALIVE_COUNTER 0 // 100
// This parameter is used to configure keep alive period
#define RSI_CONFIG_AP_KEEP_ALIVE_PERIOD 100
// This parameter is used to configure maximum stations supported
#define RSI_CONFIG_AP_MAX_STATIONS_COUNT 4
// P2P Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_AP_TCP_STACK_USED BIT(0)
// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_AP_IP_ADDRESS 0x0A0AA8C0
// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_AP_SN_MASK_ADDRESS 0x00FFFFFF
// IP address of Gateway
// E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_AP_GATEWAY_ADDRESS 0x0A0AA8C0

// Client Profile Parameters
/* ===================================================================================== */

// To configure data rate
#define RSI_CONFIG_CLIENT_DATA_RATE RSI_DATA_RATE_AUTO
// To configure wlan feature bitmap
#define RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP 0
// To configure tcp/ip feature bitmap
#define RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP BIT(2)
// To configure custom feature bit map
#define RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP 0
// To configure TX power
#define RSI_CONFIG_CLIENT_TX_POWER RSI_POWER_LEVEL_HIGH
// To configure listen interval
#define RSI_CONFIG_CLIENT_LISTEN_INTERVAL 0
// To configure SSID
#define RSI_CONFIG_CLIENT_SSID "SILABS_AP"
// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_CLIENT_BAND RSI_BAND_2P4GHZ
// To configure channel number
#define RSI_CONFIG_CLIENT_CHANNEL 0
// To configure security type
#define RSI_CONFIG_CLIENT_SECURITY_TYPE RSI_WPA
// To configure encryption type
#define RSI_CONFIG_CLIENT_ENCRYPTION_TYPE 0
// To configure PSK
#define RSI_CONFIG_CLIENT_PSK "1234567890"
// To configure PMK
#define RSI_CONFIG_CLIENT_PMK ""
// Client Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_CLIENT_TCP_STACK_USED BIT(0)
// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#define RSI_CONFIG_CLIENT_DHCP_MODE RSI_DHCP
// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_CLIENT_IP_ADDRESS 0x0A0AA8C0
// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_CLIENT_SN_MASK_ADDRESS 0x00FFFFFF
// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_CLIENT_GATEWAY_ADDRESS 0x010AA8C0
// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP 0
// Scan channel magic code
#define RSI_CONFIG_CLIENT_MAGIC_CODE 0x4321
// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ 0
// scan channel bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ 0

// EAP Client Profile Parameters
/* =========================================================================== */

// To configure data rate
#define RSI_CONFIG_EAP_DATA_RATE RSI_DATA_RATE_AUTO
// To configure wlan feature bitmap
#define RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP 0
// To configure tcp/ip feature bitmap
#define RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP BIT(2)
// To configure custom feature bit map
#define RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP 0
// To configure EAP TX power
#define RSI_CONFIG_EAP_TX_POWER RSI_POWER_LEVEL_HIGH
// To Configure scan channel feature bitmap
#define RSI_CONFIG_EAP_SCAN_FEAT_BITMAP 0
// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_CHAN_MAGIC_CODE 0 //0x4321
// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ 0
// scan channel bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ 0
// To configure SSID
#define RSI_CONFIG_EAP_SSID "SILABS_AP"
// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_EAP_BAND RSI_BAND_2P4GHZ
// To set security type
#define RSI_CONFIG_EAP_SECURITY_TYPE RSI_WPA2_EAP
// To set encryption type
#define RSI_CONFIG_EAP_ENCRYPTION_TYPE 0
// To set channel number
#define RSI_CONFIG_EAP_CHANNEL 0
// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#define RSI_CONFIG_EAP_METHOD "TLS"
// This parameter is used to configure the module in Enterprise security mode
#define RSI_CONFIG_EAP_INNER_METHOD "\"auth=MSCHAPV2\""
// To configure EAP user identity
#define RSI_CONFIG_EAP_USER_IDENTITY "\"user1\""
// TO configure EAP password
#define RSI_CONFIG_EAP_PASSWORD "\"test123\""
// EAP Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_EAP_TCP_STACK_USED BIT(0)
// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#define RSI_CONFIG_EAP_DHCP_MODE RSI_DHCP
// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_EAP_IP_ADDRESS 0x0A0AA8C0
// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_EAP_SN_MASK_ADDRESS 0x00FFFFFF
// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_EAP_GATEWAY_ADDRESS 0x010AA8C0

// P2P Profile parameters
/* ================================================================================= */

// To configure data rate
#define RSI_CONFIG_P2P_DATA_RATE RSI_DATA_RATE_AUTO
// To configure wlan feature bitmap
#define RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP 0
// To configure P2P tcp/ip feature  bitmap
#define RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP BIT(2)
// To configure P2P custom feature bitmap
#define RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP 0
// TO configure P2P tx power level
#define RSI_CONFIG_P2P_TX_POWER RSI_POWER_LEVEL_HIGH
// Set P2P go intent
#define RSI_CONFIG_P2P_GO_INTNET 16 // Support only Autonomous GO mode
// Set device name
#define RSI_CONFIG_P2P_DEVICE_NAME "WSC1.1"
// Set device operating channel
#define RSI_CONFIG_P2P_OPERATING_CHANNEL 11
// Set SSID postfix
#define RSI_CONFIG_P2P_SSID_POSTFIX "WSC_1_0_0"
// Set P2P join SSID
#define RSI_CONFIG_P2P_JOIN_SSID "SILABS_AP"
// Set psk key
#define RSI_CONFIG_P2P_PSK_KEY "12345678"
// P2P Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_P2P_TCP_STACK_USED BIT(0)
// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#define RSI_CONFIG_P2P_DHCP_MODE 1
// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_P2P_IP_ADDRESS 0x0A0AA8C0
// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_P2P_SN_MASK_ADDRESS 0x00FFFFFF
// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_P2P_GATEWAY_ADDRESS 0x010AA8C0

//! Provide HTTP/HTTPS response status code indication to application e.g 200, 404 etc
/*=======================================================================*/
//! Enable or Diable feature
#define RSI_HTTP_STATUS_INDICATION_EN RSI_DISABLE

#include "rsi_wlan_common_config.h"
#include "rsi_wlan_apis.h"

//! No of ltcp socktes
#undef RSI_NUMBER_OF_LTCP_SOCKETS
#define RSI_NUMBER_OF_LTCP_SOCKETS (g_rsi_ltcp_sockets)
//! Default number of sockets supported,max 10 sockets are supported
#undef RSI_NUMBER_OF_SOCKETS
#define RSI_NUMBER_OF_SOCKETS (g_rsi_client_sockets + g_rsi_ltcp_sockets)

#undef RSI_TOS
#define RSI_TOS (g_rsi_tos)

//! 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
//#define RSI_SELECT_LP_OR_ULP_MODE RSI_ULP_WITH_RAM_RET

//! set DTIM aligment required
//! 0 - module wakes up at beacon which is just before or equal to listen_interval
//! 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#undef RSI_DTIM_ALIGNED_TYPE
#define RSI_DTIM_ALIGNED_TYPE g_dtim_aligned_type

//! Monitor interval for the FAST PSP mode
//! default is 50 ms, and this parameter is valid for FAST PSP only
#undef RSI_MONITOR_INTERVAL
#define RSI_MONITOR_INTERVAL (g_monitor_interval)

//! Number of DTIMs to skip during powersave
#undef RSI_NUM_OF_DTIM_SKIP
#define RSI_NUM_OF_DTIM_SKIP (g_num_of_dtim_skip)

//!WMM PS parameters
//! set wmm enable or disable
#define RSI_WMM_PS_ENABLE RSI_DISABLE

//! set wmm enable or disable
//! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_TYPE 0

//! set wmm wake up interval
#define RSI_WMM_PS_WAKE_INTERVAL 20

//! set wmm UAPSD bitmap
#define RSI_WMM_PS_UAPSD_BITMAP 15

#undef TX_WAIT_TIME
#define TX_WAIT_TIME g_tx_wait_time

#undef TOTAL_SOCKETS
#define TOTAL_SOCKETS g_total_sockets
#undef TOTAL_TCP_SOCKETS
#define TOTAL_TCP_SOCKETS g_total_tcp_sockets
#undef TOTAL_UDP_SOCKETS
#define TOTAL_UDP_SOCKETS g_total_udp_sockets
#undef TCP_TX_ONLY_SOCKETS
#define TCP_TX_ONLY_SOCKETS g_tcp_tx_only_sockets
#undef TCP_RX_ONLY_SOCKETS
#define TCP_RX_ONLY_SOCKETS g_tcp_rx_only_sockets
#undef UDP_TX_ONLY_SOCKETS
#define UDP_TX_ONLY_SOCKETS g_udp_tx_only_sockets
#undef UDP_RX_ONLY_SOCKETS
#define UDP_RX_ONLY_SOCKETS g_udp_rx_only_sockets
#undef TCP_RX_HIGH_PERFORMANCE_SOCKETS
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS g_tcp_rx_high_perfornamce_sockets
#undef TCP_RX_WINDOW_SIZE_CAP
#define TCP_RX_WINDOW_SIZE_CAP g_tcp_rx_window_size_cap
#undef TCP_RX_WINDOW_DIV_FACTOR
#define TCP_RX_WINDOW_DIV_FACTOR g_tcp_rx_window_div_factor

#undef CONFIG_RTSTHRESHOLD
#define CONFIG_RTSTHRESHOLD config_threshold

#undef RSI_RTS_THRESHOLD
#define RSI_RTS_THRESHOLD rts_threshold

#undef RSI_APP_CONFIG_FEATURE_BITMAP

#define RSI_APP_CONFIG_FEATURE_BITMAP g_rsi_app_config_feature_bitmap

#define PROCESS_SCAN_RESULTS_AT_HOST 1

#define COMMON_LOG_LEVEL        g_common_log_level
#define CM_PM_LOG_LEVEL         g_cm_pm_log_level
#define WLAN_LMAC_LOG_LEVEL     g_wlan_lmac_log_level
#define WLAN_UMAC_LOG_LEVEL     g_wlan_umac_log_level
#define WLAN_NETSTACK_LOG_LEVEL g_wlan_netstack_log_level
#define BT_BLE_CTRL_LOG_LEVEL   g_bt_ble_ctrl_log_level
#define BT_BLE_STACK_LOG_LEVEL  g_bt_ble_stack_log_level
//! Min Value = 2048 bytes; Max Value = 4096 bytes; Value should be in multiples of 512 bytes
#define FW_LOG_BUFFER_SIZE 2048
//! Set queue size for firmware log messages
#define FW_LOG_QUEUE_SIZE 2
