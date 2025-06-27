/*******************************************************************************
* @file  mgmt_if_core.h
* @brief This file contains management/supplicant interface core
*        functionalities
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
#ifndef MGMT_IF_CORE_H
#define MGMT_IF_CORE_H

#include "sli_wlan_command_interface.h"
#include "internal_mgmt_command_if.h"
#include "sl_wlan_global.h"
#include "mgmt_if_init.h"
#include "supplicant_mgmt_if.h"
#include "wlan_cmn_utils.h"

typedef struct dbg_host_intf_s {
  uint16 send_eapol_count;
  uint16 disconnect_wlan_count;
  uint16 sta_disassoc_count;
  uint16 set_key_count;
  uint16 channel_number;
  uint16 eap_rx_pkt_cnt;
} dbg_host_intf_t;

typedef struct mgmt_if_adapter_s {
  uint32 opermode; // Opermode given by Host
  uint32 feature_bit_map;
  uint32 custom_feature_bit_map;
  uint32 ext_custom_feature_bit_map;
  uint32 config_feature_bit_map;
  uint8 operating_mode; // Internal mapped opermode value
  enum wise_state_e state;
  enum wise_state_e prev_state;
  uint8 client;
  uint8 connected;
  uint8 module_type;
  uint8 execute_bg_scan_once;
  uint8 set_region_given;
  uint8 rsi_region_code_from_host;
  uint8 ap_started;
  uint8 delete_ap;
  uint8 max_station_supported;
  uint8 dyn_sta_ap_switch_enable;
  uint8 is_11n_ap;
  uint8 sta_encryption_mode;
  uint8 mac_first_if[6];
  uint8 mac_second_if[6];
  uint8 sta_connection_secMode; // can be deleted later if not needed
  uint8 wlan_radio_deinit_from_host;
  uint8 wlan_btr_mode_en;
  uint8 supplicant_triggered_bg_scan;
  uint8 is_11w_enabled;
  uint8 scan_feature_bitmap;
  uint8 join_feature_bitmap;
  uint8 join_feature_bitmap_concurrent_ap;
  uint8 scan_ssid_len;
  uint32 scan_channel_bitmap_2_4_GHz;
  uint32 scan_channel_bitmap_5_GHz;
  uint8 join_bssid[6];
  uint8 region_code;
  uint8 band;
  uint8 dual_band;
  uint16 current_freq_band;
  uint8 csec_mode;
  uint8 bgscan_enabled;
  uint8 instant_bg_flag;
  uint16 bgscan_enable;
  uint16 bgscan_threshold;
  uint16 rssi_tolerance_threshold;
  uint16 bgscan_periodicity;
  uint16 active_scan_duration;
  uint16 passive_scan_duration;
  uint32 listen_interval;
  uint8 multi_probe;
  uint16 host_cmd;
  uint64 prev_pn;
  uint8 scan_ssid[SL_SSID_LEN];
  uint8 join_ssid[SL_SSID_LEN];
  uint8 bgscan_ssid[SL_SSID_LEN];
  uint8 pmk_stored;
  uint8 psk[SL_PSK_LEN];
  uint8 pmk[SL_PMK_LEN];
  uint8 eap_method[32];
  uint8 inner_method[32];
  uint8 user_identity[64];
  uint8 private_key_passwd[82];
  uint8 passwd[128];
  int32 okc;
  uint8 go_intent[2];
  uint8 rejoin_going_on;
  uint16 rejoin_count;           //! current rejoin count
  uint32 max_rejoin_retry;       //! Maximum rejoin retries allowed
  int32 rejoin_scan_interval;    //! Rejoin scan interval
  int32 first_time_retry_enable; //! connection is retried if the very first connection attempt fails
  void *supplicant_priv;
  void *ap_supplicant_priv;
  uint8 own_mac_type;
  uint16 con_operating_mode;
  uint16 p2p_curr_state;
  uint16 keys_configured;
  uint16 ap_mode_set;
  uint16 sta_count;
  uint32 sec_type_enable;
  uint16 join_data_rate;
  uint16 join_power_level;
  uint16 join_channel;
  uint8 join_from_host;
  uint8 cfm_required;
  uint8 wps_session;
  uint8 wps_station;
  uint8 ccx_capable;
  uint16 rejoin_magic_code;
  uint8 switch_band;
  wep_key_ds wep_key;
  apconfig_st_t apconfig;
  uint8 scan_req_from_host;
  uint8 wps_pin[SL_WPS_PIN_LEN];
  rsi_roam_params_t roam_params;
  uint8 roam_ind;
  uint8 roam_bssid[MAC_ADDR_LEN];
  uint16 bgscan_ssid_len;
  uint8 fips_key_status_flag;
  enum wise_state_con_ap_e concurrent_mode_state;
  void *scan_res;
  uint8 bssid[MAC_ADDR_LEN];
  uint8 ssid[SL_SSID_LEN - 1];
  uint16 ssid_len;
  uint8 async_state_code;
  uint8 async_reason_code;
  uint8 async_channel;
  uint8 async_rssi;
  uint8 async_bssid[6];
  uint8 async_reason_code_bkp;
  uint8 supplicant_thread_started;
  dbg_host_intf_t *dbg_host_intf; // For debug stats
  uint32_t passive_scan_bitmap;
} mgmt_if_adapter_t;

extern struct mgmt_if_adapter_s mgmt_if_adapter;

#define START 1
#define STOP  0

#define MAX_REJOIN_RETRIES 20

#define KEY_TYPE_PTK 1
#define KEY_TYPE_GTK 2
#define INSTALL_KEY  BIT(7)

#define CCX_RADIO_CAP_OUI_IE 1
#define CCX_VERSION_OUI_IE   3

#define WISE_AUTONOMOUS_GO 16

typedef struct scanConfirm_nonroms {
  scanConfirm_t scan_confirm;
  mobility_dom_ie_t mdie;
  coun_Ie Country_info;
#ifdef ENABLE_11AX
  sl_mbssid_params_t mbssid_params;
  uint8 ap_rrm_capable;
#endif
} __attribute__((packed)) scanConfirm_nonrom_t;

/* Load IE command payload information */
typedef struct {
  uint16 ie_type; /* standard type information element */
  uint16 ie_len;  /* length of the data in the information element */
  uint8 *ie_data; /* content of the information element */
} ie_info;

typedef struct {
  uint8 frame_type; /* frame types */
  uint8 ie_count;   /* no. of information elements encoded */
  ie_info ie;
} load_ie_t;

struct rsi_scan_resp_s {
  uint8 uChannelNumber;
  uint8 uSecMode;
  uint8 rssiVal;
  uint8 network_type;
  uint8 aSSID[34];
  uint8 BSSID[6];
  uint8 SNR;
  uint8 reserved;
} __PACKED__;

struct rsi_host_ScanRsp {
  uint32 uScanCount;
  uint32 uErrorcode;
  struct rsi_scan_resp_s stScanInfo[32];
};

/* Gets the current BSSID and copies into @bssid */
int rsi_get_bssid(void *priv, uint8 *bssid);

/*Set the channel for the device to remain awake on*/
int rsi_set_channel(void *priv, unsigned int freq, unsigned int duration);

int16 is_client_mode(void);

int is_rejoin_happenig(void);

void rsi_sta_connected_notification_to_host(uint8 *mac_addr, uint8 con_event_type);

void wsc_supp_join_done(uint8 client);

void wise_update_psk_in_nwparams(uint8 *key, uint16 key_len);

void wise_notify_p2p_dev_found(uint8 new_device, uint8 *device_name, uint8 *p2p_device_addr, uint8 *pri_dev_type);

int rsi_set_ListenMode(void *priv, int report);

int16 trigger_bg_scan(uint16 channel_bitmask_24ghz, uint32 channel_bitmask_5ghz, uint8 cmd_enable);

int16 wise_send_go_params(void);

const uint8 *driver_get_mac_addr();

uint8 *rsi_get_scan_results(void *priv);

uint16 configure_wps_method(uint8 *txPkt);

int rsi_disconnect_wlan(void *priv, const uint8 *mac_addr, int reason_code);

uint16 set_wps_key(uint8 *buf);

// contains calls to various supplicant-interface functions required by
// supplicant
void sl_wpa_driver(void *ctx, unsigned int cmd, int *ret_val);

void wise_wps_ap_configured_cb(void *ctx, void *data);

int rsi_drv_send_action_wrapper(void *priv,
                                unsigned int freq,
                                unsigned int wait,
                                const uint8 *dst,
                                const uint8 *src,
                                const uint8 *bssid,
                                const uint8 *data,
                                size_t data_len,
                                int no_cck);

void wpa3_scan_result_sec_update_wrapper(const uint8 *ie,
                                         uint16 pairwise_cipher_count,
                                         void *result,
                                         uint8 update_scan_confirm_struct);

int wpas_mbo_update_non_pref_chan_wrapper(void *wpa_s, rsi_set_non_pref_chan_info_t *non_pref_chan_config);

void wise_reset_ap(uint8 vap_id);

int8 get_scan_results(uint8 scan_flag);

int wpa3_send_sae_handshake_packet_wrapper(void *priv,
                                           const uint8 *data,
                                           size_t data_len,
                                           int noack,
                                           unsigned int freq,
                                           const uint16 *csa_offs,
                                           size_t csa_offs_len);

int mgmtif_async_notify_ap_not_found();

#ifdef ENABLE_11R
void parse_beacon(struct scanConfirm_s *scanCon,
                  uint8 *beacon,
                  int16 pktlen,
                  uint16 scan_token_rcvd,
                  scanConfirm_nonrom_t *scanCon_nonrom);
#else
void parse_beacon(struct scanConfirm_s *scanCon, uint8 *beacon, int16 pktlen, uint16 scan_token_rcvd);
#endif /*ENABLE_11R*/

int rsi_wsc_send_eapol(void *priv, const uint8 *dest, uint16 proto, const uint8 *data, size_t data_len);

#ifdef SUPPLICANT_NON_ROM
int rsi_sta_disassoc(void *priv, const uint8 *own_addr, const uint8 *addr, uint16 reason);

int sl_ap_remove_sta(void *priv, const uint8 *addr);

int rsi_hapd_send_eapol(void *priv,
                        const uint8 *addr,
                        const uint8 *data,
                        size_t data_len,
                        int encrypt,
                        const uint8 *own_addr,
                        uint32 flags);
#endif /*SUPPLICANT_NON_ROM*/

int rsi_get_ssid(void *priv, uint8 *ssid);

int rsi_set_operstate(void *priv, int state);

#ifdef ENABLE_11R
int rsi_update_ft_ies_to_sme(void *wpa_s, const uint8 *md, const uint8 *ies, size_t ies_len);
#endif

#ifdef GTK_OFFLOAD
void rsi_set_rekey_info(void *priv,
                        const uint8 *kek,
                        size_t kek_len,
                        const uint8 *kck,
                        size_t kck_len,
                        const uint8 *replay_ctr);
#endif
int wpa_driver_rsi_join(void *priv, void *params);

void *get_wpa_supp_conf_ssid(void *wpa_s);

void wpa_s_conf_init_for_rejoin(void *ctx, void *connect_without_scan);

void wpa_update_conf_pmf(void *ctx, uint8 pmf);

void wpa_update_conf_sae_pwe(void *ctx, uint8 sae_pwe);

void wpa_update_conf_okc(void *ctx, int okc);

int16 is_ap_rrm_capable(struct scanConfirm_s *scanCon);

void trigger_rom2_event_disassoc_wrapper(void *wpa_s);

void trigger_rom2_event_deauth_wrapper(void *wpa_s);

void set_wpa_disconnected(void *wpa_s, uint8 value);

void set_wpa_auto_reconnect_disabled(void *wpa_s, uint8 value);

int get_wpa_auto_reconnect_disabled(void *wpa_s);

int16 rsi_scan(rsi_uScan *uScanFrame, uint16 band);

uint8 *sl_get_scan_results(void *supp_priv);

void wise_update_psk_in_nwparams_wrapper();

void wsc_supp_join_done_wrapper(void);

void wise_notify_p2p_dev_found_wrapper(void);

void rsi_sta_connected_notification_to_host_wrapper(void);

/**scan_results.c functions**/
#ifdef ENABLE_11AX
int16 process_tx_bss_scan_result(const uint8 *buf, uint16 ies_len, uint8 *result);

int16 process_mbss_scan_results(uint8 *buf, uint16 ies_len, uint8 *result);

uint16 merge_nt_profile(const uint8 *ies_list,
                        uint16 ies_len,
                        const struct element_s *mb_ie,
                        const struct element_s *sub_ie,
                        uint8 *complete_profile);

uint16 generate_nt_bss(const uint8 *ie, uint16 ie_len, const uint8 *sub_elem, uint16 subie_len, uint8 *new_bss);

void derive_nt_bssid(uint8 *t_bssid, uint8 max_bssid, uint8 mbssid_inx, uint8 *nt_bssid);
#endif /* ENABLE_11AX */
/**End of scan_results.c functions**/

/****RTLS_TAG_MODES RELATED FUNCTIONS****/
#ifdef RTLS_TAG_MODES
void wise_check_state_rtls_cmds(uint16 cmd, int16 *ret_val);

uint16 process_scan_result(uint8 *buffer, uint8 rssi);
#endif /*RTLS_TAG_MODES*/
/****END OF RTLS_TAG_MODES RELATED FUNCTIONS****/

void update_wpa_config_read_data(enum wpa_config_read_type type, uint8 *data, uint16 len);

void wise_reset_to_band(uint8 vap_id);

void sli_p2p_fsm(uint16 event);
#endif /* MGMT_IF_CORE_H */
