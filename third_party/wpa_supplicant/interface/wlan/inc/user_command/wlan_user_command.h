/*******************************************************************************
* @file  wlan_user_command.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef WLAN_USER_COMMAND_H
#define WLAN_USER_COMMAND_H

#include "ieee_80211_def.h"

#define LMAC_OP_AP         0x0
#define LMAC_OP_STA        0x1
#define LMAC_OP_P2P_GO     0x2
#define LMAC_OP_P2P_CLIENT 0x3
#define LMAC_OP_IBSS       0x4
#define LMAC_OP_PER        0x8
#define LMAC_OP_CON_CUR    0x9

/************* SLI_WLAN_REQ_P2P_ENABLE = 0x10 *************/

/*** Ext Feature Sel Bitmap ***/
/*** Ext Custom Feature Bitmap ***/
#define EXT_FEAT_ANTENNA_DIVERSITY              BIT(0)
#define EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT      BIT(1)
#define EXT_FEAT_TELEC_SUPPORT                  BIT(2)
#define EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT BIT(3)
#define EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM   BIT(4)
#define EXT_FEAT_PREAUTH_SUPPORT_ENABLE         BIT(5)
#define EXT_FEAT_40MHZ_SUPPORT_ENABLE           BIT(6)
#ifdef PUF_ENABLE
#define EXT_FEAT_PUF BIT(7)
#endif
#ifdef QSPI_CLK_80MHZ
#define EXT_FEAT_QSPI_80MHZ_CLK_ENABLE BIT(8)
#else
#define EXT_FEAT_SPECTRAL_MASK_NOKIA BIT(8)
#endif
#define EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER BIT(9)
#ifdef RSI_PUF_PRIVATE
#define EXT_FEAT_PUF_PRIVATE_KEY BIT(10)
#endif
#define EXT_FEAT_ENABLE_11R_OTA BIT(11)
#define EXT_FEAT_IEEE_80211J    BIT(12)
#ifdef CONFIG_IEEE_80211W
#define EXT_FEAT_IEEE_80211W BIT(13)
#endif
#define EXT_FEAT_SSL_VERSIONS_SUPPORT      BIT(14)
#define EXT_FEAT_16th_STATION_IN_AP_MODE   BIT(15)
#define EXT_FEAT_ENABLE_11R_ODS            BIT(16)
#define EXT_FEAT_WOWLAN_DISABLE            BIT(17)
#define EXT_FEAT_ENABLE_GTK_REKEY_DATA     BIT(17)
#define EXT_FEAT_LOW_POWER_ENABLE          BIT(19)
#define MEMORY_MODE_CONFIG                 (BIT(20) | BIT(21))
#define EXT_FEAT_448K_M4SS_256K            BIT(21)
#define EXT_FEAT_512K_M4SS_192K            BIT(20)
#define EXT_FEAT_704K_M4SS_0K              (BIT(21) | BIT(20))
#define EXT_FEAT_384K_M4SS_320K            0
#define EXT_FEAT_SLEEP_CLK_SELECTION       (BIT(23) | BIT(22))
#define EXT_FEAT_HOMEKIT_WAC_ENABLED       BIT(24)
#define EXT_FEAT_BATTERY_VOLTAGE_0         BIT(25)
#define EXT_FEAT_BATTERY_VOLTAGE_1         BIT(26)
#define EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS BIT(27)
#define EXT_FEAT_DISABLE_DEBUG_PRINTS      BIT(28)
#define EXT_FEAT_PIN_SELECT_BIT_LOCATION   29
#ifdef WAKEFI_NODE
#define EXT_FEAT_WAKEFI_WUTX_ENABLE BIT(29)
#define EXT_FEAT_WAKEFI_WURX_ENABLE BIT(30)
#endif

// Region codes for Gain Table Command
#define REGION_FCC        0
#define REGION_ETSI       1
#define REGION_TELEC      2
#define REGION_WORLD_WIDE 3
#define REGION_KCC        4
#define REGION_WORLD_SAFE 5
#define REGION_SRRC       6

/* Reg Domain */
#define REGION_US_NUM   1
#define REGION_EU_NUM   2
#define REGION_JAP_NUM  3
#define REGION_WRLD_NUM 4
#define REGION_KR_NUM   5
#ifdef REGION_CONFIG_DISABLE_FOR_ACX
#define REGION_US_MIX_NUM 6
#endif /* REGION_CONFIG_DISABLE_FOR_ACX */
#define REGION_CN_NUM 7

/* SET_REGION Command Errors */

#define ERROR_IN_SET_REGION_COMMAND                    0xC7
#define ERROR_IN_SET_REGION_AP_COMMAND                 0xCA
#define ERROR_IN_SET_REION_AP_PARAMETERS               0xCB
#define REGION_CODE_NOT_SUPPORTED                      0xCC
#define ERROR_IN_COUNTRY_REGIONS_EXTRACTED_FROM_BEACON 0xCD
#define MODULE_DOES_NOT_SUPPORT_SELECTED_REGION        0xCE

/*** config feature bitmaps **/
#define FEAT_SLEEP_GPIO_SEL                          BIT(0)
#define PROTOCOL_RADIO_SHARE_GPIO_DEBUG_ENABLE       BIT(1)
#define DYNAMIC_VOLTAGE_SEL                          (BIT(2) | BIT(3) | BIT(4) | BIT(5))
#define EXT_PMU_GOOD_TIME_SEL                        (BIT(6) | BIT(7) | BIT(8) | BIT(9))
#define EXTERNAL_LDO_SEL                             BIT(10)
#define EXTERNAL_LDO_1P0V_SEL                        BIT(11)
#define FEAT_SEL_ULP_GPIO_FOR_PWRSAVE                BIT(12)
#define FEAT_SEL_ULP_GPIO_5_FOR_PWRSAVE              (BIT(12) | BIT(13))
#define FEAT_SEL_ULP_GPIO_8_FOR_PWRSAVE              BIT(13)
#define FEAT_TLS_V1P0                                BIT(14)
#define FEAT_TLS_V1P2                                BIT(15)
#define FEAT_ENABLE_ACTIVE_HIGH_INTERRUPT_FOR_WOWLAN BIT(16)
#define FEAT_CONC_STA_AP_DYN_SWITCH_SEL              BIT(17) //! This bit is reserved for Medtronics
#define ULP_GPIO9_FOR_UART2_TX                       BIT(18)
#define FEAT_DISABLE_MCS_5_6_7_DATARATES             BIT(19)
#define FEAT_DISABLE_SHORT_GI                        BIT(20)
#ifdef PTA_3WIRE_FEAT
#define FEAT_PTA_CONFIG_ENABLE BIT(21)
#define FEAT_PTA_CONFIG_SEL    (BIT(22) | BIT(23))
#endif
#define ENABLE_DEBUG_BBP_TEST_PINS BIT(27) //! This bit is reserved for BBP test pins enable configuration.
#define FEAT_ENABLE_LMAC_MISC      (BIT(24) | BIT(25) | BIT(26) | BIT(27) | BIT(28) | BIT(29) | BIT(30) | BIT(31))

/*** feature sel bitmap ***/
/*** Custom feature bitmap ***/
#define FEAT_SEL_TCP_IP_BYPASS_ENABLE_SME BIT(0)
#define SNR_TO_HOST                       0x01
#define FEAT_SOC_CLK_CONFIG_160MHZ        BIT(4)
#define FEAT_SEL_HIDDEN_AP                0x05 /* 6th bit: (3-5 bits are reserved used in K60) */
#define FEAT_SEL_DFS_CHANNEL_SUPPORT      0x100
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
#define FEAT_SEL_ENABLE_ASYNC_MESSAGE 0x400
#endif
#define FEAT_HOST_WAKEUP_IRQ                 0x800
#define FEAT_SEL_DISABLE_SUPP_BLACKLIST      0x1000
#define EXTRACT_NUMBER_OF_STATIONS_SUPPORTED 0x1E000
#define FEAT_SEL_ROAM_MODE_WITH_DEAUTH       0x20000
#define FEAT_BTR_MAC_PEER_DS_SUPPORT         BIT(13)
#define FEAT_SEL_CETON_CUSTOMIATION          BIT(18)
#define FEAT_SEL_AGGR_ENABLE_SME             BIT(19)
#define FEAT_WAIT_ON_HOST                    BIT(20)
//! If this feature is set,will hold max 2 packets per station
#define FEAT_PER_STATION_PACKET_LIMIT_IN_AP BIT(22)
//! To configure the soc clk to 120Mhz default will be 80Mhz
#define FEAT_SOC_CLK_CONFIG_120MHZ       BIT(24)
#define FEAT_DUAL_BAND_REJOIN_ROAM       BIT(27)
#define FEAT_RTC_TIME_FRM_HOST           BIT(28)
#define FEAT_SEL_IAP_IN_BT               BIT(29)
#define FEAT_WIRELESS_PROVISIONING       BIT(30)
#define FEAT_CUSTOM_FEAT_EXTENTION_VALID BIT(31)

/*** feature bitmap ***/
#define FEAT_SECURITY_OPEN   BIT(0)
#define FEAT_SECURITY_PSK    BIT(1)
#define FEAT_SEL_AGGR_ENABLE BIT(2)
//! bits 3 to 6 are used for LP ULP power save
#define FEAT_WPS_DISABLE            BIT(7)
#define FEAT_EAP_LEAP               BIT(8)
#define FEAT_HIDE_PSK_CREDENTIALS   BIT(9)
#define FEAT_SSL_HIGH_STREAMING_BIT BIT(10)
#define FEAT_TLS_PATH_LENGTH_ENABLE BIT(11)
#define FEAT_DISABLE_11AX           BIT(15)

/*================================================================*/
/**
 * TCP/IP Feature bit maps 
 */

#define TCP_IP_FEAT_BYPASS        BIT(0)
#define TCP_IP_FEAT_HTTP_SERVER   BIT(1)
#define TCP_IP_FEAT_DHCPV4_CLIENT BIT(2)
#define TCP_IP_FEAT_DHCPV6_CLIENT BIT(3)
#define TCP_IP_FEAT_DHCPV4_SERVER BIT(4)
#define TCP_IP_FEAT_DHCPV6_SERVER BIT(5)
#define TCP_IP_FEAT_JSON          BIT(6)
#define TCP_IP_FEAT_HTTP_CLIENT   BIT(7)
#define TCP_IP_FEAT_DNS_CLIENT    BIT(8)
#define TCP_IP_FEAT_SNMP_AGENT    BIT(9)
#ifdef SSL_PORTING
#define TCP_IP_FEAT_SSL BIT(10)
#endif
#define TCP_IP_FEAT_ICMP_CLIENT BIT(11)
#ifdef HTTPS_PORTING
#define TCP_IP_FEAT_HTTPS_SERVER BIT(12)
#endif
#ifdef EVK_DEMO
#define TCP_IP_EVK_JSON_DATA_TO_HOST BIT(13)
#endif
#define TCP_IP_CONFIG_DATA_TO_HOST BIT(14)
#define TCP_IP_FEAT_FTP_CLIENT     BIT(15)
#define TCP_IP_FEAT_SNTP_CLIENT    BIT(16)
#define TCP_IP_FEAT_IPV6_MODE      BIT(17)
#define TCP_IP_FEAT_RAW_SOCKETS    BIT(18)
#ifdef MDNSD_ENABLE
#define TCP_IP_FEAT_MDNSD BIT(19)
#endif
#define TCP_IP_FEAT_SMTP_CLIENT BIT(20)
#define TCP_IP_FEAT_POP3_CLIENT BIT(29)

//! Note BITS 21 to 24 are used to select no of sockets
#define TCP_IP_FEAT_SSL_SINGLE_SOCKET   BIT(25)
#define TCP_IP_FEAT_SSL_PRIV_PUB_CERT   BIT(26)
#define TCP_IP_FEAT_SSL_CERT_TO_RAM     BIT(27)
#define TCP_IP_FEAT_SSL_ENABLE_TCP_DUMP BIT(28)
#ifdef OTA_FW_UP
#define TCP_IP_FEAT_OTAF BIT(30)
#endif
#define TCP_IP_FEAT_EXTENTION_VALID BIT(31)

//! Extended TCP-IP Feature bitmap
#define EXT_TCP_IP_FEAT_SSL_KEY_REVEAL BIT(0)
#ifdef RSI_DHCP_USER_CLASS
#define EXT_TCP_IP_DHCP_USER_CLASS BIT(1)
#endif
#define EXT_TCP_IP_HTTP_SERVER_BYPASS BIT(2)
#define EXT_TCP_IP_ACK_FIX            BIT(3)
#define EXT_TCP_IP_WINDOW_DIV         BIT(4)
#define EXT_TCP_IP_CERT_BYPASS        BIT(5)
#define EXT_TCP_IP_SSL_16K_RECORD     BIT(6)
#define EXT_TCP_IP_DNS_CLIENT_BYPASS  BIT(7)
#define EXT_TCP_IP_WINDOW_SCALING     BIT(8)
#define EXT_TCP_IP_DUAL_MODE          BIT(9)
#define EXT_TCP_IP_ETH_BRIDGE_MODE    BIT(10)
#define EXT_DYNAMIC_COEX_MEMORY       BIT(11)
/*NOTE: BIT(12:15) reserved for the select implementation*/
#define EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE BIT(16)
#define EXT_TCP_IP_FEAT_MQTT_ENABLE      BIT(17)
#ifdef SSL_3_BUFF_ARCH
#define EXT_TCP_IP_FEAT_SSL_HIGH_PERFORMANCE BIT(18)
#endif
#define EXT_TCP_IP_FEAT_HTTP_OTAF_ENABLE        BIT(18)
#define EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST BIT(19)
#define EXT_TCP_MAX_RECV_LENGTH                 BIT(20)
#define EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS       BIT(29)
#define EXT_TCP_IP_SSL_CLOUD_CONNECTION         BIT(30)
#define CONFIG_FEAT_EXTENTION_VALID             BIT(31)

/*** WLAN BTR MODE related macros ***/
#define BTR_MODE_EN BIT(0)
#define BTR_PEER_EN BIT(1)

#define BTR_MAX_PEER_SUPPORTED 101

typedef struct WiFi_CNFG_wsc_mode {
  uint32 wsc_modeVal;
  uint32 feature_bit_map;
  uint32 tcp_ip_feature_bit_map;
  uint32 custom_feature_bit_map;
  uint32 ext_custom_feature_bit_map;
  uint32 bt_custom_feature_bit_map;
  uint32 ext_tcp_ip_feature_bit_map;
  uint32 ble_custom_feature_bit_map;
  uint32 ble_custom_ext_feature_bit_map;
  uint32 config_feature_bit_map;
} WiFi_CNFG_wsc_mode_t;

typedef union {
  struct {
    uint16 modeVal;   // uint16, p2p mode value to set
    uint16 coex_mode; // uint16, coex mode valute to set

    uint32 feature_bit_map;
    uint32 tcp_ip_feature_bit_map;
    uint32 custom_feature_bit_map;
    uint32 ext_custom_feature_bit_map;

    //! BT feature bitmap
    // 0 - 1  BT LP Chain selection
    // 2      BT Pwr Control enable/disable
    // 3 - 7  BT Tx pwr index
    // 8      edr 3mbps disable
    // 9      edr 2mbps disable
    // 10     Disable 5 slot packets
    // 11     Disable 3 slot packets
    // 12     Enable Noise figure
    //13 - 14  reserved
    //15      bit for HFP profile enable.
    //16 - 19 Reserved
    //20 - 22 bits for bt number of slaves.
    //23      bit for bt a2dp profile enable.
    //24      bit for bt a2dp profile role: 0 -sink role, 1 - source role.
    //25      bit for Accelerated mode enabel 0 - disable , 1- enable.
    //26      bit for i2s mode in accelerated application. 0 - no i2s, 1- i2s.
    //25 - 29 Reserved
    //30      bit for rf type (internal - 1 / external - 0)
    //31      bit for reserved.
    uint32 bt_custom_feature_bit_map;
    uint32 ext_tcp_ip_feature_bit_map;
    //! BLE feature bitmap
    //0-7 - BLE nbr of attributes,
    // 8 - 11: BLE Nbr of GATT services
    // (12 - 15): BLE Nbr of slaves
    // (16 - 23) BLE tx powersave index,
    //(24 - 26)BLE powersave options
    //(27- 28) BLE Nbr of masters
    //29 - BT_GATT_ASYNC_ENABLE
    //30: Enable BLE new features.
    //31: Reserved

    uint32 ble_custom_feature_bit_map;

    //0-4 No of notifications
    //5-12 BLE No of GATT Record size in bytes
    uint32 ble_custom_ext_feature_bit_map;
    uint32 config_feature_bit_map;

  } P2PmodeFrameSnd;
  uint8 uP2PmodeBuf[40]; // byte format to send to the spi interface, 4 bytes
} rsi_uP2Pmode_ram;

typedef struct {
  uint16 modeVal;   // uint16, p2p mode value to set
  uint16 coex_mode; // uint16, coex mode valute to set

  uint32 feature_bit_map;
  uint32 tcp_ip_feature_bit_map;
  uint32 custom_feature_bit_map;
  uint32 ext_custom_feature_bit_map;

  uint32 bt_custom_feature_bit_map;
  uint32 ext_tcp_ip_feature_bit_map;

  uint32 ble_custom_feature_bit_map;

  uint32 ble_custom_ext_feature_bit_map;
  uint32 config_feature_bit_map;

} P2PmodeFrameSnd;

#define POWER_SAVE_OPT_FEAT_FRAME (BIT(1) | BIT(7) | BIT(30))

/************* SLI_WLAN_REQ_BAND = 0x11 *************/

#define SL_BAND_2P4GHZ 0x00 // uint8
#define SL_BAND_5GHZ   0x01 // uint8
#define SL_BAND_DUAL   0x02 // uint8

typedef struct WiFi_CNFG_Band {
  uint8 band_val;
} WiFi_CNFG_Band_t;

/************* SLI_WLAN_REQ_SCAN = 0x13 *************/
#define FEATURE_SELECT_QUICK_SCAN      BIT(0)
#define FEATURE_SCAN_SUPPLICANT_BYPASS BIT(1)
#define FEATURE_BSSID_BASED_SCAN       BIT(2)

typedef union {
  struct {
    uint32 scan_channel;     /* scan_channel */
    uint8 ssid[SL_SSID_LEN]; /* scan SSID */
    /* 2 bytes padding is getting added here */
    uint32 pscan_bitmap; /* passive scan bitmap */
    uint8 ssid_len;
    uint8 scan_feature_bitmap;
    /* 2 bytes padding is getting added here */
    uint16 channel_bit_mask[3];
    uint8 set_scan_channel_bitmask_from_user;
    uint8 bssid[6]; /* scan SSID */
  } __PACKED__ scanFrameSnd;
  uint8 uScanBuf[SL_SSID_LEN + 16 + 4 /* padding bytes */]; // byte format to send to the spi interface, 36 bytes
} rsi_uScan_non_rom_t;

typedef union {
  struct {
    uint32 scan_channel;     /* scan_channel */
    uint8 ssid[SL_SSID_LEN]; /* scan SSID */
    /* 2 bytes padding is getting added here */
    uint32 pscan_bitmap; /* passive scan bitmap */

    uint8 ssid_len;
    uint8 scan_feature_bitmap;
    /* 2 bytes padding is getting added here */
    uint16 channel_bit_mask[3];
  } __PACKED__ scanFrameSnd;
  uint8 uScanBuf[SL_SSID_LEN + 16 + 4 /* padding bytes */]; // byte format to send to the spi interface, 36 bytes
} rsi_uScan;

typedef struct wlan_cnfg_scan_s {
  uint32 ScanChannelNum;
  uint8 scan_ssid[SL_SSID_LEN];
  uint32 pscan_bitmap;
  uint8 ssid_len;
  uint8 scan_feature_bitmap;
  uint16 channel_bit_mask[3];
  uint8 set_scan_channel_bitmask_from_user;
} __PACKED__ wlan_cnfg_scan_t;

/************* SLI_WLAN_REQ_JOIN = 0x14 *************/

/* Power value related macros */

#define ABSOLUTE_JOIN_POWER_VALUE_MASK   0x7C
#define ABSOLUTE_JOIN_POWER_VALUE_TOGGLE 0x80

/* Join Feature Bitmap related macros */

#define JOIN_FEAT_BG_ONLY_MODE          BIT(0)
#define JOIN_FEAT_LISTEN_INTERVAL_VALID BIT(1)
#define FEATURE_SELECT_QUICK_JOIN       BIT(2)
#define JOIN_FEAT_KEY_MGMT_WPA_CCXV2    BIT(3)
#define JOIN_FEAT_MAC_BASED_JOIN        BIT(4)
#define JOIN_FEAT_ENABLE_11R_AND_11W    BIT(5)

/* Used for Security Type */

#define TKIP_AND_CCMP          0
#define CCMP_ONLY              1
#define ENCRYPT_MODE_CCMP_ONLY 0x80

typedef union {
  struct {
    uint8 nwType;              // network type, 0=Ad-Hoc (IBSS), 1=Infrastructure
    uint8 securityType;        // security type, 0=Open, 1=WPA1, 2=WPA2, 3=WEP
    uint8 dataRate;            // data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
    uint8 powerLevel;          // xmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
    uint8 psk[SL_PSK_LEN];     // pre-shared key, 32-byte string
    uint8 ssid[SL_SSID_LEN];   // ssid of access point to join to, 32-byte string
    uint8 join_feature_bitmap; // Ad-Hoc Mode (IBSS), 0=Joiner, 1=Creator
    uint8 ibssChannel;         // rf channel number for Ad-Hoc (IBSS) mode
    uint8 uAction;
    uint8 ssid_len; // data length sent should always be in multiples of 4 bytes
    uint32 listen_interval;
    uint8 vap_id; // 0 - for STA, 1 - AP1, 2 - AP2 (Currently only 1 AP allowed)
    uint8 join_bssid[6];
    uint8 bssid[6];
    uint8 wps_session;
    uint8 sae_pmksa_caching;
  } __attribute__((packed)) joinFrameSnd;
  uint8 uJoinBuf[SL_SSID_LEN + SL_PSK_LEN + 8]; // byte format to send to the spi interface, 72 (0x48) bytes
} __attribute__((packed)) rsi_uJoin;

typedef struct wlan_cnfg_join_s {
  uint8 oper_mode;
  uint8 secMode;
  uint8 uRate;
  uint8 uTxPower;
  uint8 psk[SL_PSK_LEN]; /* It is used to take bssid from K60 */
  uint8 join_ssid[SL_SSID_LEN];
  uint8 join_feature_bitmap;
  uint8 uchannel;
  uint8 uAction;
  uint8 ssid_len;
  uint32 listen_interval;
  uint8 vap_id; // reserved here
  uint8 join_bssid[6];
  uint8 bssid[6];
  uint8 wps_session;
  uint8 sae_pmksa_caching;
} __attribute__((packed)) wlan_cnfg_join_t;

/*********** SLI_WLAN_REQ_PWRMODE = 0x15 ************/

typedef union {
  struct {
    uint8 powerVal; // uint16, power value to set
    uint8 ulp_mode_enable;
    uint8 listen_interval_dtim;
#ifdef ENABLE_FAST_PSP
    uint8 fast_psp_enable;
    uint16 monitor_interval;
#endif
    uint8 num_of_dtim_skip;
    uint16 listen_interval;
  } powerFrameSnd;
  uint8 uPowerBuf[2]; // byte format to send to the spi interface, 4 bytes
} rsi_uPower_non_rom;

/*********** SLI_WLAN_REQ_NETWORK_PARAMS = 0x18 ************/

typedef struct TCP_EVT_NET_PARAMS_SME {
  uint16 wlan_state;
  uint8 ssid[SL_SSID_LEN];
  uint16 connection_type;
  uint8 sec_type;
  uint8 Chn_no;
  uint8 psk[SL_PSK_LEN];
} TCP_EVT_NET_PARAMS_SME;

/*********** SLI_MGMT_REQ_DISCONNECT = 0x19 ************/

typedef struct rsi_disassoc_s {
  uint16 remove_client; /* 1 - Remove the client (AP mode), 0 - Disassoc to AP */
  uint8 mac_addr[6];    /* MAC address if AP mode, Ignored in STA mode */
} rsi_disassoc_t;

/*********** SLI_WLAN_REQ_SELECT_ANTENNA = 0x1B ************/

typedef struct rsi_antenna_s {
  uint8 antenna_sel;
  uint8 gain_2g;
  uint8 gain_5g;
  uint8 antenna_path;
  uint8 antenna_type;
} __attribute__((packed)) rsi_antenna_t;

/*********** SLI_WLAN_REQ_SET_REGION = 0x1D ************/

/*11D paramters*/
typedef struct {
  uint8 setregion_code_from_user_cmd;
  /*Enable or disable set region from user:
    1-take from user configuration;
    0-Take from Beacons*/
  uint8 region_code;
  /*region code(1-US,2-EU,3-JP)*/
  //! Module type from host:
  //! To know antenna is present or not for manuf s/w below 3.1
  //! valid values are 0-if no ext antenna and 1-if ext antenna present
  uint16 module_type;
} rsi_setregion_t;

/*********** SLI_WLAN_REQ_FEATSEL = 0x1E ************/

typedef struct WiFi_CNFG_Feat_Sel {
  uint32 feat_sel_bitmap;
} WiFi_CNFG_Feat_Sel_t;

/*********** SLI_WLAN_REQ_SNR = 0x1F ************/

typedef struct TCP_EVT_SNR_t {
  uint8 snr_val;
} TCP_EVT_SNR_t;

/*********** SLI_WLAN_REQ_APCONF = 0x24 ************/

typedef struct {
  uint16 channel_no;
  uint8 ssid[SL_SSID_LEN];
  uint8 security_type;
  uint8 encryp_mode;
  uint8 psk[SL_PSK_LEN];
  uint16 beacon_interval;
  uint16 dtim_period;
  uint8 keep_alive_type;
  uint8 keep_alive_counter;
  uint16 max_no_sta;
} __PACKED__ apconfig_st_t;

typedef struct ap_conf_ta_s {
  uint16 dtim_period;
  uint16 beacon_interval;
  uint16 channel;
  uint16 max_no_sta;
  uint16 encryp_mode;
  uint8 keep_alive_type;
  uint8 keep_alive_counter;
} __PACKED__ ap_conf_ta_t;

/*********** SLI_WLAN_REQ_SET_WEPKEY = 0x25 ************/

typedef struct wep_key_ds_s {
  uint16 index;
  uint8 key[4][32];
} __PACKED__ wep_key_ds;

/*********** SLI_WLAN_REQ_DEBUG_DUMP = 0x26 ************/

typedef struct debug_info {
  uint32 assertion_type;
  uint32 assertion_level;
} debug_info_t;

/******** SLI_WLAN_REQ_TWT_AUTO_CONFIG = 0x2E ********/

typedef struct twt_selection_s {
  uint8 twt_enable;
  uint16 expected_tx_throughput;
  uint32 tx_latency;
  uint32 rx_latency;
  uint16 device_avg_throughput;
  uint8 estimated_extra_wake_duration_percent;
  uint8 twt_tolerable_deviation;
  uint32 default_wake_interval_ms;
  uint32 default_min_wake_duration_ms;
  uint8 beacon_wake_up_count_after_sp;
} twt_selection_t;

/******** SLI_WLAN_REQ_TWT_PARAMS = 0x2F ********/

typedef struct rsi_twt_config_s {
  uint8 wake_duration;
  uint8 wake_duration_tol;
  uint8 wake_int_exp;
  uint8 wake_int_exp_tol;
  uint16 wake_int_mantissa;
  uint16 wake_int_mantissa_tol;
  uint8 implicit_twt;
  uint8 un_announced_twt;
  uint8 triggered_twt;
  uint8 negotiation_type;
  uint8 twt_channel;
  uint8 twt_protection;
  uint8 twt_flow_id;
  uint8 restrict_tx_outside_tsp;
  uint8 twt_retry_limit;
  uint8 twt_retry_interval;
  uint8 req_type;
  uint8 twt_enable;
  uint8 twt_wake_duration_unit;
  uint32 rx_latency;
  uint8 beacon_wake_up_count_after_sp;
} __PACKED__ rsi_twt_config_t;

/************* SLI_WLAN_REQ_EVM_OFFSET = 0x36 *************/

#ifdef RAM_VER2
typedef struct rsi_evm_offset_s {
  int8 evm_offset_val;
  uint8 evm_index;
} rsi_evm_offset_t;
#endif

/************* MGMT_REQ_RSSI_QRY = 0x3A *************/

typedef struct TCP_EVT_RSSI_t {
  uint16 rssi_val;
} __PACKED__ TCP_EVT_RSSI_t;

/*********** SLI_WIFI_REQ_RESCHEDULE_TWT = 0x3F **********/

typedef enum {
  SUSPEND_INDEFINITELY,
  SUSPEND_FOR_DURATION,
  RESUME_IMMEDIATELY,
} sl_wifi_reschedule_twt_action_t;

typedef struct {
  uint8 flow_id;
  sl_wifi_reschedule_twt_action_t twt_action;
  uint16 reserved1;
  uint8 reserved2;
  uint64 suspend_duration;
} __PACKED__ sl_wifi_reschedule_twt_config_t;

/************* SLI_WLAN_REQ_SET_REGION_AP = 0xBD *************/
typedef struct {
  uint8 setregion_code_from_user_cmd;
  /*Enable or disable set region from user:
         1-take from user configuration;
         0-Take US or EU or JP*/
  uint8 country_code[COUNTRY_CODE_LEN];
  /*region code(1-US,2-EU,3-JP)*/
  uint32 no_of_rules;
  struct {
    uint8 first_channel;
    uint8 no_of_channels;
    uint8 max_tx_power;
  } __PACKED__ channel_info[MAX_POSSIBLE_CHANNELS_5G];
} __PACKED__ rsi_setregion_ap_t;

typedef struct rsi_twt_rsp_s {
  uint8 wake_duration;
  uint8 wake_duration_unit;
  uint8 wake_int_exp;
  uint8 negotiation_type;
  uint16 wake_int_mantissa;
  uint8 implicit_twt;
  uint8 un_announced_twt;
  uint8 triggered_twt;
  uint8 twt_channel;
  uint8 twt_protection;
  uint8 twt_flow_id;
} __PACKED__ rsi_twt_rsp_t;

#define MAX_STA_SUPPORT_IN_GO_COMMAND_RSP 16
struct go_sta_info_s {
  uint16 ip_version;
  uint8 mac[MAC_ADDR_LEN];
  union {
    uint8 ipv4_address[SL_IP_ADDR_LEN];
    uint32 ipv6_address[4];
  } ip_address;
} __PACKED__;

/************** SLI_WLAN_REQ_GAIN_TABLE = 0x47 ****************/

#define RSI_GAIN_TABLE_BAND_2P4_GHZ    0x01
#define RSI_GAIN_TABLE_BAND_5GHZ       0x02
#define RSI_MIN_GAIN_TABLE_PAYLOAD_LEN 0
#define RSI_MAX_GAIN_TABLE_PAYLOAD_LEN 128

typedef struct WLAN_USR_GAIN_TABLE_Req {
  uint8 band;
  uint8 bandwidth;
  uint16 size;
  uint32 reserved;
  uint8 gain_table[1];
} __PACKED__ WLAN_USR_GAIN_TABLE_Req;

/********** SLI_WLAN_REQ_CFG_P2P = 0x4B **********/

typedef struct WLAN_CFG_P2P_Req {
  uint8 GOIntent[2];
  uint8 DeviceName[64];
  uint8 operChannel[2];
  uint8 SSIDPostfix[64];
  uint8 psk[64];
} __PACKED__ WLAN_CFG_P2P_Req;

/************** SLI_WLAN_REQ_GAIN_TABLE = 0x4C ****************/

typedef struct WLAN_SET_EAP_CFG_Req {
  uint8 eapMethod[32];
  uint8 innerMethod[32];
  uint8 UserIdentity[64];
  uint8 password[128];
  int8 okc[4];
  uint8 private_key_passwd[82];
} __PACKED__ WLAN_SET_EAP_CFG_Req;

/************* SLI_WLAN_REQ_GO_PARAMS_QUERY = 0x4E *************/

typedef struct {
  uint8 ssid[SL_SSID_LEN];
  uint8 bssid[MAC_ADDR_LEN];
  uint16 channel_number;
  uint8 psk[SL_PSK_LEN];
  uint8 ipv4_address[SL_IP_ADDR_LEN];
  uint32 ipv6_address[SL_IP_ADDR_LEN];
  uint16 sta_count;
  struct go_sta_info_s sta_info[MAX_STA_SUPPORT_IN_GO_COMMAND_RSP];
} __PACKED__ rsi_GOParams;

/************* SLI_WLAN_REQ_SET_NON_PREF_CHAN = 0x5F *************/

#define RSI_MAX_SINGLE_BAND_CHANNEL 14

typedef struct rsi_set_non_pref_chan_info_s {
  uint8 channel_num;
  struct {
    uint8 reason;
    uint8 operating_class;
    uint8 channel;
    uint8 preference;
  } __PACKED__ mbo_non_pref_chan[RSI_MAX_SINGLE_BAND_CHANNEL];

} __PACKED__ rsi_set_non_pref_chan_info_t;

/************* SLI_MGMT_REQ_CONNECTED_AP_TSF = 0x65 *************/

#define RSI_STA_TSF_INVALID 121

/************* SLI_WLAN_REQ_WLAN_EXT_STATS = 0x68 *************/

/*Structure for module stats notification*/
typedef struct rsi_wlan_ext_stats_s {
  uint32 beacon_lost_count;
  uint32 beacon_rx_count;
  uint32 mcast_rx_count;
  uint32 mcast_tx_count;
  uint32 ucast_rx_count;
  uint32 ucast_tx_count;
  uint32 overrun_count;
} rsi_wlan_ext_stats_t;

/************* SLI_WLAN_BGSCAN = 0x6A *************/

#define MAX_BG_CHANNEL_SUPPORT (24 + 12)

typedef struct rsi_uBgscan_s {
  uint16 bgscan_enable;
  uint16 instant_bg_enable;
  uint16 bgscan_threshold;
  uint16 rssi_tolerance_threshold;
  uint16 bgscan_periodicity;
  uint16 active_scan_duration;
  uint16 passive_scan_duration;
  uint8 multi_probe;
  uint8 num_bg_channels;
  //! Max number of channels in dual band (5G + 2.4G)
  uint16 channels2scan[MAX_BG_CHANNEL_SUPPORT];
  uint8 ssid[SL_SSID_LEN]; // ssid of access point to scan
  uint16 ssid_len;
  uint32 channel_bitmap;
} __attribute__((packed)) rsi_uBgscan_t;

/************* SLI_WLAN_REQ_AP_HT_CAPS = 0x6D *************/

typedef struct ap_ht_caps_cmd_s {
  uint16 is_11n_ap;
  uint8 ht_caps[2];
} ap_ht_caps_cmd_t;

/************* SLI_WLAN_REQ_REJOIN_PARAMS = 0x6F *************/

typedef struct rsi_rejoin_params_s {
  uint32 rsi_max_try;
  int32 rsi_scan_interval;
  int32 rsi_beacon_missed_count;
  uint32 rsi_first_time_retry_enable;
} rsi_rejoin_params_t;

/************** SLI_WLAN_REQ_SET_CHANNEL = 0x7A ****************/

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

typedef struct {
  uint16 channel;                ///< Channel number
  sl_wifi_band_t band;           ///< Wi-Fi Radio Band
  sl_wifi_bandwidth_t bandwidth; ///< Channel bandwidth
} sl_wifi_channel_t;

typedef struct sl_wifi_btr_set_channel_s {
  sl_wifi_channel_t channel_info;
  uint8 tx_pwr;
} sl_wifi_btr_set_channel_t;

/*********** SLI_WLAN_REQ_ROAM_PARAMS = 0x7B *************/

typedef struct rsi_roam_params_s {
  int roam_enable;
  int roam_threshold;
  int roam_hysteresis;
} __PACKED__ rsi_roam_params_t;

/*********** SLI_WLAN_REQ_PER_PARAMS = 0x7C *************/

#ifdef PER_MODE_ENABLE
typedef struct umac_per_params_s {
  unsigned short enable;
  unsigned short power;
  unsigned int rate;
  unsigned short pkt_length;
  unsigned short mode;
  unsigned short chan_number;
  unsigned short rate_flags;
  unsigned short per_ch_bw;
  unsigned short aggr_enable;
  unsigned short aggr_count;
  unsigned short no_of_pkts;
  unsigned int delay;
} __PACKED__ per_params_t;
#endif /* PER_MODE_ENABLE */

#ifdef ENABLE_11AX
typedef struct he_per_conf_params {
  uint8 enable_11ax;
  uint8 coding_type;
  uint8 nominal_pe;
  uint8 UL_DL;
  uint8 he_ppdu_type;
  uint8 beam_change;
  uint8 BW;
  uint8 STBC;
  uint8 Tx_BF;
  uint8 GI_LTF;
  uint8 DCM;
  uint8 NSTS_MIDAMBLE;
  uint8 spatial_reuse;
  uint8 BSS_color;
  uint16 HE_SIGA2_RESERVED;
  uint8 RU_ALLOCATION;
  uint8 N_HELTF_TOT;
  uint8 SIGB_DCM;
  uint8 SIGB_MCS;
  uint16 USER_STA_ID;
  uint8 USER_IDX;
  uint8 SIGB_COMPRESSION_FIELD;
} __PACKED__ he_per_params_t;
#endif /* ENABLE_11AX */

/*********** SLI_WLAN_RADIO_REQ_DEINIT = 0x81 *************/

//! wlan radio req structure
typedef struct rsi_wlan_req_radio_s {
  uint8 radio_req;
} rsi_wlan_req_radio_t;

/*********** SLI_WLAN_REQ_GET_RAM_DUMP = 0x92 *************/

#ifdef CHIP_9117
#ifdef CHIP_9117B0
#define MAX_384K_RAM (352 * 1024)
#define MAX_448K_RAM (416 * 1024)
#define MAX_512K_RAM (480 * 1024)
#define MAX_704K_RAM (672 * 1024)
#else
#define MAX_384K_RAM (384 * 1024)
#define MAX_448K_RAM (448 * 1024)
#define MAX_512K_RAM (512 * 1024)
#define MAX_704K_RAM (704 * 1024)
#endif
#endif

/********** SLI_WLAN_REQ_GET_RAM_DUMP = 0x92 **********/

//! ram_dump structure
typedef struct rsi_ram_dump_s {
  uint32 addr;
  uint32 length;
} rsi_ram_dump_t;

/********** SLI_WLAN_RSP_WMM_PS = 0x97 **********/

typedef struct rsi_uwmm_ps_s {
  uint16 wmm_ps_enable;
  uint16 wmm_ps_type;
  uint32 wmm_ps_wakeup_interval;
  uint8 wmm_ps_uapsd_bitmap;
} rsi_uwmm_ps_t;

/********** SLI_WLAN_RSP_HOST_TSF_QUERY = 0x98 **********/

#ifdef TSF_SYNC
typedef struct rsi_tsf_sync_s {
  uint16 cmd_type;
  uint32 time_stamp[2];
} rsi_tsf_sync_t;

typedef struct rsi_tsf_sync_pkt_s {
  uint8 mac_addr[6];
} rsi_tsf_sync_pkt_t;
#endif

/********** SLI_WLAN_REQ_PER_STATS = 0xA2 **********/

#ifdef PER_MODE_ENABLE
typedef struct umac_per_stats_req_s {
  unsigned short enable;
  unsigned short chan_number;
} per_stats_req_t;
#endif

/********** SLI_WLAN_RSP_HOST_PSK = 0xA5 **********/

#define PSK_FRM_HOST                 1
#define PMK_FRM_HOST                 2
#define GENERATE_PMK                 3
#define LENGTH_BASED_PSK             4
#define GEN_PMK_FRM_LENGTH_BASED_PSK 5

typedef struct sl_set_psk_s {
  uint8 TYPE;
  /*
   * TYPE = 0  Only PSK is valid PMK will be generated in join.
   * TYPE = 1  PMK is valid only use the same PMK. We will not generate PMK.
   * TYPE = 2  PSK and SSID valid generate PMK. Use the generated generated PMK in join.
   */
  uint8 psk_or_pmk[SL_PSK_LEN]; /* PSK or PMK */
  uint8 ap_ssid[SL_SSID_LEN];   /* SSID of the access point we want to join. */
} sl_set_psk_t;

/********** SLI_WLAN_REQ_LOAD_MFI_IE = 0xB5 **********/

/* Structure of MFI IE in beacon */
typedef struct rsi_mfi_load_ie_request_s {
  uint8 ie_len; /* MFI IE length */
  uint8 ie[0];  /* Ie data */
} rsi_mfi_load_ie_request_t;

/*********** SLI_WLAN_REQ_SET_CONFIG = 0xBA *************/

typedef struct rsi_set_config_s {
#define XO_CTUNE_FROM_HOST BIT(0)
  uint32 code;
  union {
    uint8 xo_ctune;
  } values;
} rsi_set_config_t;

/*********** SLI_WLAN_REQ_CONFIG = 0xBE *************/

typedef union {
  struct {
    uint8 config_type[2]; // needs to be 4 bytes long
    uint8 value[2];       // uint16, rtsthreshold value to set
  } ConfigFrameSnd;
  uint8 uConfigBuf[4]; // byte format to send to the spi interface, 4 bytes
} rsi_uConfig;

/*********** SLI_WLAN_REQ_SET_AUTO_RATE = 0xC5 *************/

typedef union {
  struct {
    uint32 supported_rate_bitmap;
    uint16 mcs_rate_bitmap;
    uint16 number_of_retries;
  } setDataRateFrameSnd;
  uint8 uSetDataRateBuf[6];
} rsi_uSetDataRate;

/*********** SLI_WLAN_REQ_SET_FEATURE = 0xC8 *************/

typedef struct WiFi_FEATURE_FRAME {
  uint8 pll_mode;
  uint8 rf_type;
  uint8 wireless_mode;
  uint8 enable_ppp;
  uint8 afe_type;
  uint8 disable_programming;
  uint16 reserved;
  uint32 feature_enables;
} WiFi_FEATURE_FRAME_t;

/*********** SLI_WLAN_REQ_FILTER_BROADCAST = 0xC9 *************/

typedef struct wlan_9116_filter_bcast_s {
  uint16 full_beacon_timeout;
  uint8 filter_broadcast_data_pending;
  uint8 filter_broadcast_forever;
} wlan_9116_filter_bcast_t;

/*********** SLI_REQ_MEMRD = 0xDF *************/

typedef struct sl_memrd_s {
  uint32 addr;
  uint32 length;
} sl_memrd_t;

/*********** SLI_WLAN_REQ_GET_FULL_FW_VERSION = 0xE0 *************/

typedef struct {
  uint8 chip_id;
  uint8 rom_id;
  uint8 major;
  uint8 minor;
  uint8 security_version;
  uint8 patch_num;
  uint8 customer_id;
  uint16 build_num;
} __attribute__((packed)) full_fw_version_t;

/*********** SLI_WLAN_REQ_RTC_DATE_AND_TIME = 0xE9 *************/

typedef struct rsi_module_rtc_time_s {
  uint32 tm_sec;  /* seconds after the minute [0-60] */
  uint32 tm_min;  /* minutes after the hour [0-59] */
  uint32 tm_hour; /* hours since midnight [0-23] */
  uint32 tm_mday; /* day of the month [1-31] */
  uint32 tm_mon;  /* months since January [0-11] */
  uint32 tm_year; /* years since 1900 */
  uint32 tm_wday; /* Weekday from Sunday to Saturday [1-7] */
} rsi_module_rtc_time_t;

/*********** SLI_WLAN_REQ_TIMEOUTS = 0xEA *************/

typedef struct rsi_timeouts_s {
  uint32 timeout_bitmap;
  uint16 timeout_val;
} rsi_timeouts_t;

/*********** SLI_WLAN_REQ_GET_WLAN_STATS = 0xF1 *************/

/*Structure for module stats notification*/
typedef struct rsi_wlan_stats_s {
  uint8 operating_mode;
  uint8 dtim_period;
  uint16 ideal_beacon_info;
  uint16 busy_beacon_info;
  uint16 beacon_interval;
} rsi_wlan_stats_t;

/*********** SLI_WLAN_REQ_FREQ_OFFSET = 0xF3 *************/

typedef struct rsi_freq_offset_s {
  int32 freq_offset_in_khz;
} rsi_freq_offset_t;

/***************** SLI_WLAN_REQ_RE_KEY = 0xFB *****************/

typedef struct rsi_re_key_s {
  uint8 type;
  uint8 key_store;
  uint8 key[128];
} rsi_re_key_t;

/************* SLI_WLAN_REQ_AX_PARAMS = 0xFF *************/

#ifdef ENABLE_11AX
typedef struct wlan_11ax_config_params_s {
  uint8 guard_interval;
  uint8 nominal_pe;
  uint8 dcm_enable;
  uint8 ldpc_enable;
  uint8 ng_cb_enable;
  uint8 ng_cb_values;
  uint8 uora_enable;
  uint8 trigger_resp_ind;
  uint8 ipps_valid_value;
  uint8 tx_only_on_ap_trig;
  uint8 twt_support;
  uint8 config_er_su;
  uint8_t disable_su_beamformee_support;
} wlan_11ax_config_params_t;
#endif /* ENABLE_11AX */

/************* SLI_E_WLAN_WFD_DEV_RSP = 0x54 *************/
typedef struct {
  uint8 devState;            // If New device  1; Device left 0
  uint8 devName[32];         // Name the device found or left 33 bytes
  uint8 macAddress[6];       // Mac address of the device
  uint8 devtype[2];          // Type of the device 1st byte inidcates primary device type;
                             // 2nd byte indicates sub catagory
} __PACKED__ rsi_wfdDevInfo; /*made packed to remove extra bytes from wfd response*/

#endif /* WLAN_USER_COMMAND_H */
