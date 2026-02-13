/*******************************************************************************
* @file  rsi_store_config.h
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

/*
 * rsi_store_config.h
 *
 *  Created on: Aug 21, 2012
 *      Author: Swati
 */

#ifndef RSI_STORE_CONFIG_H_
#define RSI_STORE_CONFIG_H_

#include "rsi_defines.h"
#include "rsi_global.h"
//#include "netx_porting.h"

struct sc_params_s {
  UINT8 cfg_enable;
  uint32_t opermode;
  uint32_t feature_bit_map;
  uint32_t tcp_ip_feature_bit_map;
  uint32_t custom_feature_bit_map;
  uint8 band;
  uint8 scan_feature_bitmap;
  uint8 join_ssid[RSI_SSID_LEN];
  UINT8 uRate;
  UINT8 uTxPower;
  UINT8 FIPS_key_type_stored;
  UINT8 uchannel;
  uint8 scan_ssid_len;
  UINT8 FIPS_key_restore;
  UINT8 csec_mode;
  UINT8 psk[RSI_PSK_LEN];
  uint8 scan_ssid[RSI_SSID_LEN];
  UINT8 scan_cnum; /* Channel number */
  UINT8 dhcp_enable;
  UINT8 ip[IP_ADDRESS_SZ];      /* Module IP address */
  UINT8 sn_mask[IP_ADDRESS_SZ]; /* Sub-net mask */
  UINT8 dgw[IP_ADDRESS_SZ];     /* Default gateway */

  uint8 eap_method[32];
  uint8 inner_method[32];
  uint8 user_identity[64];
  uint8 passwd[128];

  UINT8 go_intent[2];
  UINT8 device_name[64];
  UINT8 operating_channel[2];
  UINT8 ssid_postfix[64];
  UINT8 psk_key[64];
#define WISE_PMK_LEN 32
  uint8 pmk[WISE_PMK_LEN];

  apconfig_st_t apconfig;
  uint8 module_mac[6];
  uint16 antenna_select;
  //! In FIPS bypass mode will be used for storing FIPS mode value
  uint16 fips_bypass_mode;
#ifdef WEP_CODE
  wep_key_ds wep_key;
#endif

  uint16_t dhcp6_enable;  /* DHCPv6 enable or disable */
  uint16_t prefix_length; /* Prefix length */
  uint32_t ip6[4];        /* Module IPv6 address */
  uint32_t dgw6[4];       /* Default router gateway */
  UINT8 tcp_stack_used;   /* BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(2)=4 - BOTH*/
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
#define DRAEGER_SC 55
  /*Bgscan information*/
  uint16_t bgscan_magic_code;
  uint16_t bgscan_enable;

  uint16_t bgscan_threshold;
  uint16_t rssi_tolerance_threshold;
  uint16_t bgscan_periodicity;
  uint16_t active_scan_duration;
  uint16_t passive_scan_duration;
  UINT8 multi_probe;
  /*Channel bitmap info*/
  uint16_t chan_bitmap_magic_code;
  uint32_t scan_chan_bitmap_stored_2_4_GHz;
  uint32_t scan_chan_bitmap_stored_5_GHz;
  /*Roaming params info*/
  uint16_t roam_magic_code;
  rsi_roam_params_t roam_params_stored;
  /*rejoin params info*/
  uint16_t rejoin_magic_code;
  rsi_rejoin_params_t rejoin_param_stored;
#else
#define DRAEGER_SC 0
#endif
  uint8 region_request_from_host;  // delete from here once isolated from connection_cmd_parse.c
  uint8 rsi_region_code_from_host; // delete from here once isolated from connection_cmd_parse.c
  uint8 region_code;

#ifdef TRANS_MODE_ENABLE
  uint16_t trans_mode_enable;
  rsi_req_trans_mode_params_t transparams;
#endif

#define TRANS_MODE_STRUCT_SIZE          (2 + sizeof(rsi_req_trans_mode_params_t))
#define MULTICAST_POWERMODE_STRUCT_SIZE 17

  //! This is the magic code for multicast,powermode
  uint16 multicast_magic_code;
  uint16 multicast_bitmap;

  uint16 powermode_magic_code;
  uint8 powermode;
  uint8 ulp_mode;

  uint16 wmm_ps_magic_code;
  uint8 wmm_ps_enable;
  uint8 wmm_ps_type;
  uint32 wmm_ps_wakeup_interval;
  uint8 wmm_ps_uapsd_bitmap;

#define SIZE_OF_NEW_SC_PARAMS (4 + 1 + 4 + 82 + 6 + 1 + 2 + sizeof(ap_ht_caps_cmd_t) + 2 + 49 + 4 + 2 + 2 + 1)
  uint32_t listen_interval;
  uint8 listen_interval_dtim;

  /* NOTE: Always add new parameters here only */

#define MAX_HTTP_SERVER_USERNAME 31
#define MAX_HTTP_SERVER_PASSWORD 31

  uint32_t ext_custom_feature_bit_map;
  uint8 private_key_passwd[82];
  UINT8 join_bssid[6];
  UINT8 join_feature_bitmap;
  ap_ht_caps_cmd_t ht_caps;
  uint16_t ht_caps_magic_word;
  uint8 fast_psp_enable;
  uint16 monitor_interval;
  uint16_t req_timeout_magic_word;
  uint16 timeout_value;
  uint32 timeout_bitmap;

  UINT8 dhcp_ap_enable;
  UINT8 ap_ip[IP_ADDRESS_SZ];      /* Module IP address */
  UINT8 ap_sn_mask[IP_ADDRESS_SZ]; /* Sub-net mask */
  UINT8 ap_dgw[IP_ADDRESS_SZ];     /* Default gateway */

  uint16_t dhcp6_ap_enable;  /* DHCPv6 enable or disable */
  uint16_t ap_prefix_length; /* Prefix length */
  uint32_t ap_ip6[4];        /* Module IPv6 address */
  uint32_t ap_dgw6[4];       /* Module IPv6 address */
  uint32_t ext_tcp_ip_feature_bit_map;
  /* HTTP/HTTPS Server credentials */
  UINT8 http_credentials_avail;
  UINT8 http_username[MAX_HTTP_SERVER_USERNAME];
  UINT8 http_password[MAX_HTTP_SERVER_PASSWORD];
#ifdef ENABLE_CONNECT_WITHOUT_SCAN
  /*Need to change this below macro whenever any changes made in connect without scan*/
#define CONNECT_WITHOUT_SCAN_PARAM_SIZE 942
  uint8 connect_without_scan[524]; //! change the size with macro;
  uint8 bss_beacon[400];           //!change the size with macro
  uint16 beacon_int;
  uint16 caps;
  int16 ie_len;
  int32 freq;
  UINT8 bssid[6];
  uint16_t rsi_con_without_scan_magic_code;
#endif
  /*NOTE: always modify this macro after adding the extra members in structure*/
#define SIZEOF_DRAEGER_SC_PARAMS                                                                        \
  (DRAEGER_SC + 3 + TRANS_MODE_STRUCT_SIZE + MULTICAST_POWERMODE_STRUCT_SIZE + MAX_HTTP_SERVER_USERNAME \
   + MAX_HTTP_SERVER_PASSWORD + SIZE_OF_NEW_SC_PARAMS)
} __PACKED__;

struct sc_host_params_s {
  UINT8 cfg_enable;
  uint32_t opermode;
  uint32_t feature_bit_map;
  uint32_t tcp_ip_feature_bit_map;
  uint32_t custom_feature_bit_map;
  uint8 band;
  uint8 ssid[RSI_SSID_LEN];
  UINT8 uRate;
  UINT8 uTxPower;
  UINT8 psk[RSI_PSK_LEN];
  UINT8 channel; /* Channel number */
  UINT8 dhcp_enable;

  UINT8 ip[IP_ADDRESS_SZ];      /* Module IP address */
  UINT8 sn_mask[IP_ADDRESS_SZ]; /* Sub-net mask */
  UINT8 dgw[IP_ADDRESS_SZ];     /* Default gateway */

#define WISE_EAP_METHOD_TLS  1
#define WISE_EAP_METHOD_TTLS 2
#define WISE_EAP_METHOD_PEAP 3
#define WISE_EAP_METHOD_FAST 4
  uint8 eap_method;
#define WISE_INNER_METHOD_MSCHPAV2 1
  uint8 inner_method;
  uint8 user_identity[64];
  uint8 passwd[128];

  /* AP Mode */
  uint8 sec_type;
  uint8 encryption_type;
  uint8 beacon_interval[2];
  uint8 dtim_period[2];
#ifdef IPV6_ENABLE
  uint16_t dhcp6_enable;  /* DHCPv6 enable or disable */
  uint16_t prefix_length; /* Prefix length */
  uint32_t ip6[4];        /* Module IPv6 address */
  uint32_t dgw6[4];       /* Default router gateway */
#endif
  UINT8 tcp_stack_used; /* BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(2)=4 - BOTH*/
} __PACKED__;
#define CONFIG_FLASH_ENABLE      1
#define RSI_CFG_ENABLE_MAGIC_NUM 0xAB
#define SC_TX_PKT                1

#define SC_SAVE_OPERMODE(wsc, m)                   wsc->sc_params.opermode = m
#define SC_SAVE_FEATURE_BIT_MAP(wsc, m)            wsc->sc_params.feature_bit_map = m
#define SC_SAVE_TCP_IP_FEATURE_BIT_MAP(wsc, m)     wsc->sc_params.tcp_ip_feature_bit_map = m
#define SC_SAVE_CUSTOM_FEATURE_BIT_MAP(wsc, m)     wsc->sc_params.custom_feature_bit_map = m
#define SC_SAVE_EXT_TCP_IP_FEATURE_BIT_MAP(wsc, m) wsc->sc_params.ext_tcp_ip_feature_bit_map = m
#define SC_SAVE_MODULE_MAC(wsc, m)                 memcpy(wsc->sc_params.module_mac, m, 6)

#define SC_SAVE_BAND(wsc, band_val)             wsc->sc_params.band = band_val
#define SC_SAVE_SCAN_CHANNEL(wsc, scan_channel) wsc->sc_params.scan_cnum = scan_channel
#define SC_SAVE_SCAN_SSID_LEN(wsc, ssid_len)    wsc->sc_params.scan_ssid_len = ssid_len

#define SC_SAVE_SCAN_SSID(wsc, ssid, len) memcpy(wsc->sc_params.scan_ssid, ssid, len)

#define SC_SAVE_JOIN_SSID(wsc, j_ssid)         sl_strcpy(wsc->sc_params.join_ssid, j_ssid)
#define SC_SAVE_JOIN_SSID_LEN(wsc, j_ssid_len) wsc->sc_params.join_ssid_len = j_ssid_len
#define SC_SAVE_CSEC_MODE(wsc, securityType)   wsc->sc_params.csec_mode = securityType
#define SC_SAVE_LISTEN_INTERVAL(wsc, l)        wsc->sc_params.listen_interval = l
#define SC_SAVE_PSK(wsc, p)                    memcpy(wsc->sc_params.psk, p, 64)

#define SC_SAVE_SEC_TYPE(wsc, securityType)            wsc->sc_params.secMode = securityType
#define SC_SAVE_JOIN_FEATURE_BITMAP(wsc, join_feature) wsc->sc_params.join_feature_bitmap = join_feature
#define SC_SAVE_SCAN_FEATURE_BITMAP(wsc, scan_feature) wsc->sc_params.scan_feature_bitmap = scan_feature
#define SC_SAVE_IBSS_CHANNEL(wsc, ibssChannel)         wsc->sc_params.uchannel = ibssChannel
#define SC_SAVE_ACTION(wsc, uAct)                      wsc->sc_params.uAction = uAct
#define SC_SAVE_DATA_RATE(wsc, join_data_rate)         wsc->sc_params.uRate = join_data_rate
#define SC_SAVE_POWER_LEVEL(wsc, join_power_level)     wsc->sc_params.uTxPower = join_power_level

#define SC_SAVE_EAP_METHOD(wsc, eapmethod)        rsi_strcpy(wsc->sc_params.eap_method, eapmethod)
#define SC_SAVE_INNER_METHOD(wsc, innermethod)    rsi_strcpy(wsc->sc_params.inner_method, innermethod)
#define SC_SAVE_USER_IDENTITY(wsc, identity)      rsi_strcpy(wsc->sc_params.user_identity, identity)
#define SC_SAVE_PASSWD(wsc, password)             rsi_strcpy(wsc->sc_params.passwd, password)
#define SC_SAVE_PRIVATE_KEY_PASSWD(wsc, password) rsi_strcpy(wsc->sc_params.private_key_passwd, password)

#define SC_SAVE_GO_INTENT(wsc, gointent)        memcpy(wsc->sc_params.go_intent, gointent, 2)
#define SC_SAVE_OPERATING_CHANNEL(wsc, channel) memcpy(wsc->sc_params.operating_channel, channel, 2)
#define SC_SAVE_DEVICE_NAME(wsc, device)        rsi_strcpy(wsc->sc_params.device_name, device)
#define SC_SAVE_SSID_POSTFIX(wsc, postfix)      rsi_strcpy(wsc->sc_params.ssid_postfix, postfix)
#define SC_SAVE_PSKKEY(wsc, pskkey)             rsi_strcpy(wsc->sc_params.psk_key, pskkey)

#define SC_SAVE_CERT_TOTAL_LEN(wsc, tlen)      memcpy(wsc->sc_params.cert_total_len, tlen, 2)
#define SC_SAVE_CERT_LEN(wsc, len)             memcpy(wsc->sc_params.cert_len, len, 2)
#define SC_SAVE_CERTIFICATE(wsc, cert, length) memcpy(wsc->sc_params.Certificate, cert, length)
#define SC_SAVE_CERT_MORE_CHUNKS(wsc, chunks)  wsc->sc_params.more_chunks = chunks
/*NOT using baud that is why replaced with antenna select*/
//#define SC_SAVE_UART_BAUD(wsc,baud) wsc->sc_params.baud = baud
#define SC_SAVE_ANTENNA_SELECT(wsc, antenna_select) wsc->sc_params.antenna_select = antenna_select
//#define SC_SAVE_UART_BRFA(wsc,brfa) wsc->sc_params.brfa = brfa

int sc_save_config(struct sc_params_s *sc_params);
#define SC_SAVE_APCONFIG(wsc, ap_conf)   memcpy(&wsc->sc_params.apconfig, (uint8 *)ap_conf, sizeof(apconfig_st_t))
#define SC_SAVE_AP_PROFILE(wsc, ap_conf) memcpy(&wsc->sc_params.apconfig, (uint8 *)ap_conf, sizeof(apconfig_st_t))

#define SC_SAVE_REQ_TIMEOUT_VALUE(wsc, value)   wsc->sc_params.timeout_value = value
#define SC_SAVE_REQ_TIMEOUT_BITMAP(wsc, bitmap) wsc->sc_params.timeout_bitmap = bitmap

#ifdef ENABLE_DRAEGER_CUSTOMIZATION
#define SC_SAVE_REJOIN_PARAMS(wsc, m)            memcpy(&wsc->sc_params.rejoin_param_stored, m, sizeof(rsi_rejoin_params_t))
#define SC_SAVE_SCAN_CHAN_BITMAP_2_4(wsc, m)     wsc->sc_params.scan_chan_bitmap_stored_2_4_GHz = m
#define SC_SAVE_SCAN_CHAN_BITMAP_5(wsc, m)       wsc->sc_params.scan_chan_bitmap_stored_5_GHz = m
#define SC_SAVE_BG_ENABLE(wsc, m)                wsc->sc_params.bgscan_enable = m
#define SC_SAVE_BG_THRSHOLD(wsc, m)              wsc->sc_params.bgscan_threshold = m
#define SC_SAVE_BG_TOLERANCE(wsc, m)             wsc->sc_params.rssi_tolerance_threshold = m
#define SC_SAVE_BG_PERIODICITY(wsc, m)           wsc->sc_params.bgscan_periodicity = m
#define SC_SAVE_BG_ACTIVE_SCAN_DURATION(wsc, m)  wsc->sc_params.active_scan_duration = m
#define SC_SAVE_BG_PASSIVE_SCAN_DURATION(wsc, m) wsc->sc_params.passive_scan_duration = m
#define SC_SAVE_BG_MULTI_PROBE(wsc, m)           wsc->sc_params.multi_probe = m
#define SC_SAVE_REJOIN_MAGIC_CODE(wsc, m)        wsc->sc_params.rejoin_magic_code = m
#define SC_SAVE_BGSCAN_MAGIC_CODE(wsc, m)        wsc->sc_params.bgscan_magic_code = m
#define SC_SAVE_ROAM_MAGIC_CODE(wsc, m)          wsc->sc_params.roam_magic_code = m
#define SC_SAVE_CHANEL_BITMAP_MAGIC_CODE(wsc, m) wsc->sc_params.chan_bitmap_magic_code = m

#define SC_SAVE_MULTICAST_MAGIC_CODE(wsc, m) wsc->sc_params.multicast_magic_code = m
#define SC_SAVE_MULTICAST_BITMAP(wsc, m)     wsc->sc_params.multicast_bitmap = m

#define SC_SAVE_POWERMODE_MAGIC_CODE(wsc, m) wsc->sc_params.powermode_magic_code = m
#define SC_SAVE_POWERMODE(wsc, m)            wsc->sc_params.powermode = m
#define SC_SAVE_ULP_MODE(wsc, m)             wsc->sc_params.ulp_mode = m
#define SC_SAVE_LISTEN_INTERVAL_DTIM(wsc, m) wsc->sc_params.listen_interval_dtim = m
#define SC_SAVE_FAST_PSP_ENABEL(wsc, m)      wsc->sc_params.fast_psp_enable = m
#define SC_SAVE_MONITOR_INTERVAL(wsc, m)     wsc->sc_params.monitor_interval = m

#define SC_SAVE_WMM_PS_MAGIC_CODE(wsc, m)      wsc->sc_params.wmm_ps_magic_code = m
#define SC_SAVE_WMM_PS_ENABLE(wsc, m)          wsc->sc_params.wmm_ps_enable = m
#define SC_SAVE_WMM_PS_TYPE(wsc, m)            wsc->sc_params.wmm_ps_type = m
#define SC_SAVE_WMM_PS_WAKEUP_INTERVAL(wsc, m) wsc->sc_params.wmm_ps_wakeup_interval = m
#define SC_SAVE_WMM_PS_UAPSD_BITMAP(wsc, m)    wsc->sc_params.wmm_ps_uapsd_bitmap = m
#endif
#define SC_SAVE_REGION_REQUEST_FROM_HOST(wsc, m) wsc->sc_params.region_request_from_host = m
#define SC_SAVE_REGION_CODE_FROM_HOST(wsc, m)    wsc->sc_params.rsi_region_code_from_host = m
#define SC_SAVE_REGION_CODE(wsc, m)              wsc->sc_params.region_code = m
#ifdef FIPS_CODE_ENABLE
#define SC_SAVE_FIPS_BYPASS_MODE(m) rsi_wsc_cb.sc_params.fips_bypass_mode = m
#endif /* FIPS_CODE_ENABLE */

#ifdef SPECTRAL_MASK
uint8 rsi_load_spectral_mask_configuration(uint16 PktLength, Spectral_Mask_Config_Req *ConfigReqPayload);
#endif

#endif /* RSI_STORE_CONFIG_H_ */
