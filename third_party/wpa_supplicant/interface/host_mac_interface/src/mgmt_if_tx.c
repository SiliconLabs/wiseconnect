/*******************************************************************************
* @file   mgmt_if_tx.c
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

#include "internal_mgmt_command_if.h"
#include "sli_wlan_command_interface.h"
#include "mgmt_platform_if.h"
#include "host_mac_if_tx.h"
#include "sl_pkt_desc_utils.h"
#include "sl_status.h"
#include "sli_nhcp_memory_mgmt.h"
#include "sli_nhcp_pkt_struct.h"
#include "sli_nhcp_driver_thread.h"

int16 mgmt_hm_switch_band(uint16 *band)
{
  return send_msg_to_nwp_2((uint8 *)band, sizeof(*band), NULL, 0, SLI_WLAN_REQ_SWITCH_BAND, 1, 0);
}

int16 mgmt_hm_join(uint8 *uJoinFrame, uint16 join_frame_len)
{
  return send_msg_to_nwp_2(uJoinFrame, join_frame_len, NULL, 0, SLI_WLAN_REQ_JOIN, 1, 0);
}

int16 mgmt_hm_load_ap_caps(uint32 ap_caps)
{
  load_ap_caps_t load_ap_caps;

  load_ap_caps.ap_capabilites = ap_caps;

  return send_msg_to_nwp_2((uint8 *)&load_ap_caps, sizeof(load_ap_caps), NULL, 0, MGMT_LOAD_AP_CAPS, 1, 0);
}

int16 mgmt_hm_set_ListenMode(listen_mode_t *listen_mode)
{
  return send_msg_to_nwp_2((uint8 *)listen_mode, sizeof(*listen_mode), NULL, 0, MGMT_SET_LISTEN_MODE, 1, 0);
}

int16 mgmt_hm_set_channel(set_channel_t *set_channel)
{
  return send_msg_to_nwp_2((uint8 *)set_channel, sizeof(*set_channel), NULL, 0, MGMT_SET_CHANNEL, 1, 0);
}

int16 mgmt_hm_scan(uint8 *scan)
{
  return send_msg_to_nwp_2(scan, sizeof(rsi_uScan_non_rom_t), NULL, 0, SLI_WLAN_REQ_SCAN, 1, 0);
}

int16 mgmt_hm_send_bgscan_cmd(rsi_uBgscan_t *bg_scan)
{
  return send_msg_to_nwp((uint8 *)bg_scan, sizeof(*bg_scan), SLI_WLAN_BGSCAN);
}

int16 mgmt_hm_send_disconnect(disconnect_wlan_t *disconnect_wlan,
                              uint8 disconnect_from_supp,
                              uint8 disconnect_from_host)
{
  uint8 flag = 0;

  if (disconnect_from_supp)
    flag = DISCONNECT_FROM_SUPP_IN_WPS;

  if (disconnect_from_host)
    flag = DISCONNECT_FROM_HOST;

  return send_msg_to_nwp_2((uint8 *)disconnect_wlan,
                           sizeof(*disconnect_wlan),
                           NULL,
                           0,
                           SLI_MGMT_REQ_DISCONNECT,
                           1,
                           flag);
}

#ifdef ENABLE_11R
int16 mgmt_hm_send_ft_action(const uint8 *ies, size_t ies_len, ft_action_req_frm_supp_t *ft_params)
{
  return send_msg_to_nwp_2((uint8 *)ies, ies_len, (uint8 *)ft_params, sizeof(*ft_params), LOAD_FT_ACTION_IE, 1, 0);
}

int16 mgmt_hm_update_ft_ies(const uint8 *ies, size_t ies_len)
{
  return send_msg_to_nwp((uint8 *)ies, ies_len, UPDATE_FT_IES_FROM_SUPP);
}
#endif /* ENABLE_11R */

int16 mgmt_hm_send_target_ap_info(struct scanConfirm_s *scancon)
{
  return send_msg_to_nwp((uint8 *)scancon, sizeof(*scancon), SLI_SEND_TARGET_AP_INFO_TO_SME);
}

#ifdef GTK_OFFLOAD
int16 mgmt_hm_set_rekey_info(gtk_rekey_info_t *gtk_rekey)
{
  return send_msg_to_nwp((uint8 *)gtk_rekey, sizeof(*gtk_rekey), GTK_REKEY_DATA);
}
#endif

int16 rsi_rejoin_fail_to_lmac()
{
  return send_msg_to_nwp(NULL, 0, MGMT_REJOIN_FAIL);
}

int16 send_sae_success_indication(void)
{
  return send_msg_to_nwp(NULL, 0, WPA3_SAE_SUCC_ASSOC_REQ);
}

int16 mgmt_hm_indicate_sae_commit_state(uint8 is_sae_commited)
{
  return send_msg_to_nwp(&is_sae_commited, sizeof(uint8), MGMT_INDICATE_SAE_COMMIT_STATE);
}

int16 rsi_load_ie(uint8 *buf, uint16 ie_len, uint8 pkt_type)
{
  uint16 data[2] = { 0 };
  data[0]        = pkt_type;
  data[1]        = ie_len;

  return send_msg_to_nwp_2(buf, ie_len, (uint8 *)&data[0], 4, MGMT_LOAD_IE_INFO, 1, 0);
}

int16 rsi_load_roam_ind(uint8 *sme_notify)
{
  return send_msg_to_nwp(sme_notify, sizeof(*sme_notify), ROAM_INDICATION);
}

int32 rsi_send_deauth_frm_host()
{
  rsi_disassoc_t data; /*Sending 2 to represent cause of deauth is internal due to Roam*/
  data.remove_client = 0;
  sl_memset(data.mac_addr, 0, 6);

  return send_msg_to_nwp((uint8 *)&data, sizeof(data), DEAUTH_FROM_ROAM_PROCESS);
}

int32 rsi_send_null_and_block_unblock(uint8 block_unblock_enable)
{
  return send_msg_to_nwp(&block_unblock_enable, sizeof(block_unblock_enable), NULL_AND_BLK_UNBLK);
}

int16 rsi_drv_send_action(const uint8 *buf,
                          uint16 size_param,
                          const uint8 *dst,
                          const uint8 *src,
                          const uint8 *bssid,
                          unsigned int freq,
                          int no_cck,
                          uint8 is_11w_enabled)
{
  uint8 mac_header[MAC_HEADER_LENGTH] = { 0 };
  uint8 offset                        = MAC_HEADER_LENGTH;

  if (is_11w_enabled) {
    offset += CCMP_HEADER_SIZE;
  }

  sl_memcpy(&mac_header[4], (uint8 *)dst, MAC_ADDR_LEN);
  sl_memcpy(&mac_header[10], (uint8 *)src, MAC_ADDR_LEN);
  sl_memcpy(&mac_header[16], (uint8 *)bssid, MAC_ADDR_LEN);

  send_msg_to_nwp_2((uint8 *)buf, size_param, &mac_header[0], offset, MGMT_P2P_ACTION_FRAME, 1, 0);

  return 1; /* We should give non zero here to allow P2P FSM continue */
}

int32 wpa3_send_sae_handshake_packet(const uint8 *payload, size_t frame_length)
{
  return send_msg_to_nwp_2((uint8 *)payload, frame_length, NULL, 0, TX_DOT11_MGMT_FRAME, 0, 0);
}

sl_status_t rsi_send_eapol_cmd(uint8 *data, size_t data_len, uint8 *msg, uint8 client_mode)
{
  uint8 *txPkt                    = NULL;
  uint8 extended_desc_size        = 0;
  uint8 *scatter_buf0_addr        = NULL;
  uint16 offset_len               = SL_ETH_HDR_LEN;
  sl_status_t status              = SL_STATUS_OK;
  extended_desc_t *xtend_desc_pkt = NULL;

  if ((data_len == SL_EAPOL_4_OF_4_PKT_SIZE) && client_mode)
    offset_len += sizeof(extended_desc_t);

  txPkt = (uint8 *)sl_mgmt_prepare_tx_pkt(data, data_len, NULL, offset_len, 1, NHCP_HIGH_PRIORITY);

  if (txPkt == NULL) {
    return SL_STATUS_FAIL;
  }

  extended_desc_size = SL_PKT_TX_HDESC_GET_DW1_EXT_DESC_SIZE(txPkt);
  SL_PKT_TX_HDESC_SET_DW0_LENGTH(txPkt, (SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0) - HOST_DESC_LENGTH));
  SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_MGMT_Q);
  SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(txPkt, TX_DOT11_MGMT_FRAME);

  if ((data_len == SL_EAPOL_4_OF_4_PKT_SIZE) && client_mode) {
    SL_PKT_TX_HDESC_SET_DW0_MISC_FLAGS(txPkt, SL_PKT_TX_HDESC_GET_DW0_MISC_FLAGS(txPkt) | SL_CONFIRM_REQUIRED_TO_HOST);
    xtend_desc_pkt = (extended_desc_t *)(SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt) + HOST_DESC_LENGTH);
    sl_memset(xtend_desc_pkt, 0, EXTENDED_DESC_LENGTH);
    xtend_desc_pkt->confirm_frame_type = EAPOL4_CONFIRM;
    extended_desc_size                 = EXTENDED_DESC_LENGTH;
    SL_PKT_TX_HDESC_SET_DW1_EXT_DESC_SIZE(txPkt, EXTENDED_DESC_LENGTH);
  }

  scatter_buf0_addr = (((uint8 *)SL_PKT_GET_TXPKT_HDESC_ADDR(txPkt)) + HOST_DESC_LENGTH + extended_desc_size);

  /* Copy the msg TODO Try to avoid sl_memcpy here */
  sl_memcpy(scatter_buf0_addr, msg, SL_ETH_HDR_LEN);

  status = sli_hal_nhcp_send_pkt_without_copy((uint8_t *)(&((sli_nhcp_tx_pkt_t *)txPkt)->host_desc));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to send TX packet to NWP: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }

  return SL_STATUS_OK;
}

sl_status_t rsi_set_keys_frame_cmd(set_keys_t *tmp_setkey, uint8 ap_iface, uint16 *keys_configured)
{
  sl_status_t status = SL_STATUS_OK;
  uint8 *txPkt       = NULL;

  txPkt = (uint8 *)sl_mgmt_prepare_tx_pkt((uint8 *)tmp_setkey, sizeof(*tmp_setkey), NULL, 0, 1, NHCP_HIGH_PRIORITY);
  if (txPkt == NULL) {
    return SL_STATUS_FAIL;
  }

  SL_PKT_TX_HDESC_SET_DW0_FRAME_TYPE(txPkt, MGMT_SET_KEYS_FRAME);

  if (ap_iface) {
    /*This is ap interface so set vap id 1*/
    SL_PKT_TX_HDESC_SET_DW1_B3(txPkt, 1);
  }

  SL_PKT_TX_HDESC_SET_DW0_LENGTH(txPkt, (SL_PKT_GET_TXPKT_SCATTER_BUFF_LEN(txPkt, 0) - HOST_DESC_LENGTH));
  SL_PKT_TX_HDESC_SET_DW0_QNUM(txPkt, SL_WLAN_MGMT_Q);
  *keys_configured = 1;

  status = sli_hal_nhcp_send_pkt_without_copy((uint8_t *)(&((sli_nhcp_tx_pkt_t *)txPkt)->host_desc));
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nFailed to send TX packet to NWP: 0x%lX\r\n", status);
    SL_MGMT_ASSERT(0);
  }

  return SL_STATUS_OK;
}

/*==============================================*/
/**
 * @fn          send_pmkid_validation_status
 * @brief       This function is used to send pmkid validation to SME layer. 
 * @param[in]   buf, input buffer which has STA mac, islegacy, assoc_type and ies
 * @param[in]   buf_len, length of the IEs 
 * @param[in]   pmkid_status, status of pmkid processing
 * @section description
 * This function create txpkt that has PMKID validation status and sending it to SME.
 */
uint8 mgmt_hm_send_pmkid_validation_status(uint8 *buf, uint16 buf_len)
{
  return send_msg_to_nwp(buf, buf_len, SEND_ASSOC_REASSOC_RESP);
}
