/*******************************************************************************
* @file  mgmt_if_header.h
* @brief This file acts as interface between management interface and
*        external modules
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
#ifndef MGMT_IF_HEADER_H
#define MGMT_IF_HEADER_H

#include "ieee_80211_def.h"
#include "sl_lib_utils_rtos.h"
#include "sl_pkt_desc_utils.h"
#include "mgmt_platform_if.h"

typedef struct mgmt_command_status_s {
  uint16 command;
  uint16 status;
  uint16 length;
  uint8 *message;
} mgmt_command_status_t;

#define MGMT_IF_FREE_CMD_PKT    0
#define MGMT_IF_FWD_CMD_TO_UMAC 1

#define VALID_STATE   0
#define INVALID_STATE 1

#define CHECK_REJOIN_STATE_AND_CMD(cmd)                                                    \
  ((mgmt_if_adapter.rejoin_going_on == 1)                                                  \
       && ((cmd != SLI_WLAN_REQ_GET_FULL_FW_VERSION) && (cmd != SLI_WLAN_REQ_GET_MAC_ADDR) \
           && (cmd != SLI_MGMT_REQ_DISCONNECT))                                            \
     ? (sl_mgmt_indicate_to_host(cmd, 0, WISE_ERROR_REJOIN_HAPPENING, NULL), 1)            \
     : 0)

void send_supplicant_command(uint8 *txPkt);
void wsc_init();
void timer_and_memory_pool_init(uint32 size);
void wlan_mgmt_if_cmd_handler(uint8 *txPkt);
#ifdef CONFIG_P2P
void wsc_connect();
#endif /*CONFIG_P2P*/
int16 mgmtif_send_ext_auth();
int16 mgmtif_send_sae_auth_frame(uint8 *sae_auth_frame,
                                 size_t sae_auth_frame_len,
                                 uint16 authSeqNum,
                                 ieee80211_macframe_t *hdr_mac,
                                 uint8 vap_id);
int16 mgmtif_send_mic_failure();
int16 mgmtif_tx_status_indication(uint32 sta_id, uint32 token);
int16 mgmtif_reset_ap_ht_params();
int16 mgmtif_radio_deinit_rsp();
int16 mgmtif_process_disconnect(uint8 *rxPkt, uint32 vap_id);
int16 mgmtif_process_apconfig_resp(uint8 *rxPkt, uint32 token);
int16 mgmtif_process_channel_cmd_resp(uint16 rcv_channel);
int16 mgmtif_process_scan_resp(uint8 *send_to_host, uint32 token);
#ifdef ENABLE_11R
int16 mgmtif_auth_ft_ie_rsp(uint8 *ft_ies, uint16 allocation_size, uint32 ft_action);
#endif /* ENABLE_11R */
int16 mgmtif_process_scan_results();
int16 mgmtif_process_bgscan();
int16 mgmtif_process_add_sta_ind(uint8 *buf, uint16 ie_len);
int16 mgmtif_process_remove_sta(uint8 *buf);
int16 mgmtif_process_on_air_mgmt_pkt(uint8 *rxPkt,
                                     uint8 *buf,
                                     int16 msg_len,
                                     int rcv_freq,
                                     int16 rcv_rssi,
                                     uint16 rcv_channel);
int16 mgmtif_process_join_resp(uint8 *rxPkt, uint8 *send_to_host, uint8 *default_case);
void process_assoc_req_pmkid_req(uint8 *rx_pkt);
int16 mgmtif_process_init_rsp(uint8 *rxPkt, uint8 *send_to_host);
void mgmt_if_update_max_sta_supported(uint8 max_station_supported);
void wise_update_state(uint8 state);
void wise_update_concurrent_state(uint8 state);
void wise_update_prev_state(uint8 state);
uint8 sl_get_concurrent_mode_state(void);
void supp_wps_mem_init(uint32 size);
void sl_set_wps_client(uint8 wps_client_flag);
#ifdef AP_JSON_SCAN_LIST
rsi_wpa_scan_results_t *rsi_get_scan_results_from_mgmt(void);
#endif /* AP_JSON_SCAN_LIST */
uint8 *sl_get_ssid_from_mgmt_if_adapter(void);
int32 get_first_time_retry_enable(void);
int16 is_eapol_pkt(uint8 *rx_pkt, uint8 vap_id);
#endif /* MGMT_IF_HEADER_H */
