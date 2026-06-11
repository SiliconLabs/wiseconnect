/*******************************************************************************
* @file   mgmt_if_tx.h
* @brief  This file contains functions to transmit commands to UMAC
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

#ifndef MGMT_IF_TX_H
#define MGMT_IF_TX_H

#include "wlan_scan.h"
#include "sl_pkt_desc_utils.h"

int16 mgmt_hm_switch_band(uint16 *band);
int16 mgmt_hm_join(uint8 *uJoinFrame, uint16 join_frame_len);
int16 mgmt_hm_load_ap_caps(uint32 ap_caps);
int16 mgmt_hm_set_ListenMode(listen_mode_t *listen_mode);
int16 mgmt_hm_set_channel(set_channel_t *set_channel);
int16 mgmt_hm_scan(uint8 *scan);
int16 mgmt_hm_send_bgscan_cmd(rsi_uBgscan_t *bg_scan);
int16 mgmt_hm_send_disconnect(disconnect_wlan_t *disconnect_wlan,
                              uint8 disconnect_from_supp,
                              uint8 disconnect_from_host);
#ifdef ENABLE_11R
int16 mgmt_hm_send_ft_action(const uint8 *ies, size_t ies_len, ft_action_req_frm_supp_t *ft_params);
int16 mgmt_hm_update_ft_ies(const uint8 *ies, size_t ies_len);
#endif /* ENABLE_11R */
int16 mgmt_hm_send_target_ap_info(struct scanConfirm_s *scancon);
#ifdef GTK_OFFLOAD
int16 mgmt_hm_set_rekey_info(gtk_rekey_info_t *gtk_rekey);
#endif
int16 rsi_rejoin_fail_to_lmac();
int16 rsi_load_ie(uint8 *buf, uint16 ie_len, uint8 pkt_type);
int16 rsi_load_roam_ind(uint8 *sme_notify);
int32 rsi_send_deauth_frm_host();
int32 rsi_send_null_and_block_unblock(uint8 block_unblock_enable);
int16 rsi_drv_send_action(const uint8 *buf,
                          uint16 size_param,
                          const uint8 *dst,
                          const uint8 *src,
                          const uint8 *bssid,
                          unsigned int freq,
                          int no_cck,
                          uint8 is_11w_enabled);
int16 send_sae_success_indication(void);
int32 wpa3_send_sae_handshake_packet(const uint8 *payload, size_t frame_length);
sl_status_t rsi_send_eapol_cmd(uint8 *data, size_t data_len, uint8 *msg, uint8 client_mode);
sl_status_t rsi_set_keys_frame_cmd(set_keys_t *tmp_setkey, uint8 ap_iface, uint16 *keys_configured);
int16 mgmt_hm_indicate_sae_commit_state(uint8 is_sae_commited);
uint8 mgmt_hm_send_pmkid_validation_status(uint8 *buf, uint16 buf_len);
#endif /* MGMT_IF_TX_H */
