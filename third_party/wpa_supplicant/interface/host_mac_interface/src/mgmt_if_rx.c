/*******************************************************************************
* @file   mgmt_if_rx.c
* @brief  This file contains functions to receive commands from UMAC
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

#include "mgmt_if_rx.h"
#include "sli_wlan_command_interface.h"
#include "internal_mgmt_command_if.h"
#include "mgmt_if_header.h"
#include "ieee_80211_def.h"
#include "mgmt_platform_if.h"
#include "mgmt_if_utils.h"
#include "mgmt_data_flow.h"
#include "mgmt_event_interface.h"
#include "mgmt_if_core.h"
#include "wlan_cmn_utils.h"

/*************** Function declarations ***************/
int16 mgmtif_rx_process_on_air_mgmt(uint8 *rxPkt);
#ifdef RTLS_TAG_MODES
int16 mgmtif_rx_process_rtls_tag_pkt();
#endif /* RTLS_TAG_MODES */
int16 mgmtif_rx_indicate_fast_psp(uint32 sta_id);
int16 mgmtif_rx_process_bgscan(uint8 *rxPkt);
int16 mgmtif_rx_process_add_sta_ind(uint8 *rxPkt);
int16 mgmtif_rx_process_remove_sta_ind(uint8 *rxPkt);
static int16 decode_mgmt_cmd_response(uint8 *rx_pkt);

/*************** Function definitions ***************/
int16 mgmtif_rx_process_on_air_mgmt(uint8 *rxPkt)
{
  uint8 *rx_buff                = NULL;
  ieee80211_macframe_t *hdr_mac = NULL;
  struct auth_s *st_auth        = NULL;
  uint8 *sae_auth_frame         = NULL;
  size_t sae_auth_frame_len;
  uint8 vap_id = 0;

  if (get_operating_mode() == WISE_CONCURRENT_MODE) {
    /*Keeping vap_id here for the responses*/
    vap_id = SL_PKT_RX_HDESC_GET_DW1_B3(rxPkt);
  }

  rx_buff =
    (((uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt)) + HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt));
  hdr_mac = (ieee80211_macframe_t *)rx_buff;

  if ((hdr_mac->fc[0] == IEEE80211_FC0_SUBTYPE_AUTH || hdr_mac->fc[0] == IEEE80211_FC0_SUBTYPE_PROBE_RESP
       || hdr_mac->fc[0] == IEEE80211_FC0_SUBTYPE_BEACON)) {
    if (hdr_mac->fc[0] == IEEE80211_FC0_SUBTYPE_PROBE_RESP || hdr_mac->fc[0] == IEEE80211_FC0_SUBTYPE_BEACON) {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);
      mgmtif_send_ext_auth();
      return WLAN_STATUS_SUCCESS;
    }

    rx_buff += sizeof(ieee80211_macframe_t);
    st_auth = (struct auth_s *)rx_buff;
    if (st_auth->authAlgoID == WLAN_AUTH_SAE) {
      sae_auth_frame     = sli_form_linear_buffer((uint8 *)rxPkt);
      sae_auth_frame_len = SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt) - SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt);
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);
      mgmtif_send_sae_auth_frame(sae_auth_frame, sae_auth_frame_len, st_auth->authSeqNum, hdr_mac, vap_id);
      sl_free(sae_auth_frame);
      return WLAN_STATUS_SUCCESS;
    }
  } else {
    return sli_send_host_rx_pkt(rxPkt);
  }

  return WLAN_STATUS_SUCCESS;
}

#ifdef RTLS_TAG_MODES
int16 mgmtif_rx_process_rtls_tag_pkt()
{
  common_info_ext_t *glbl_common_info_ext_p = (common_info_ext_t *)glbl_common_info.reserved_ptr;
  glbl_tag_packet_info_t *tag_pkt_info_p    = (glbl_tag_packet_info_t *)glbl_common_info_ext_p->tag_pkt_info_p;

  //! Check if memory is not allocated to hold tag scan results
  if ((tag_pkt_info_p->tag_scan_info == NULL)
      || ((tag_pkt_info_p->tag_scan_info != NULL) && (tag_pkt_info_p->tag_scan_info->scan_count == 0)))
    sl_mgmt_indicate_to_host(SLI_WLAN_TAG_PKT_RSP, 0, tag_pkt_info_p->tag_scan_info, NULL);
  else {
    //! Save connected AP BSSID
    sl_memcpy(tag_pkt_info_p->tag_scan_info->connected_ap_bssid, get_bssid_from_mgmt_intf_adapter(), 6);

    //! Send tag scan results to host
    sl_mgmt_indicate_to_host(SLI_WLAN_TAG_PKT_RSP, sizeof(tag_scan_result_t), 0, tag_pkt_info_p->tag_scan_info);
  }

  if (tag_pkt_info_p->tag_scan_info != NULL) {
    //! Free tag scan results pointer
    sl_free(tag_pkt_info_p->tag_scan_info);
    tag_pkt_info_p->tag_scan_info = NULL;
  }

  return WLAN_STATUS_SUCCESS;
}
#endif /* RTLS_TAG_MODES */

int16 mgmtif_rx_indicate_fast_psp(uint32 sta_id)
{
#ifdef PS_DEPENDENT_VARIABLES
  rx_fast_psp_indication(sta_id);
#endif /* PS_DEPENDENT_VARIABLES */

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_rx_process_bgscan(uint8 *rxPkt)
{
  if (rxPkt == NULL)
    return WLAN_STATUS_FAILURE;

  SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);

  if (is_instant_bg_enabled()) {
    clear_instant_bg_flag();
    //! SME will sends (no_of_scan_results*sizeof(scan_res) + 8)
    //! So if scan fails 8 bytes will be sent
    if (SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt) == 8) {
      /*Indicating AP not found error to host */
      SL_PKT_RX_HDESC_SET_DW3_TOKEN(rxPkt, 3);
    }

    mgmtif_process_scan_results();
  }

  mgmtif_process_bgscan();
  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_rx_process_add_sta_ind(uint8 *rxPkt)
{
  uint8 *buf        = NULL;
  uint16 msg_len    = 0;
  uint16 ie_len     = 0;
  uint8 *uCmdRspBuf = NULL;

  if (rxPkt == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  /* check whether xtended_desc_len 8 bytes is taken care or not */
  buf = (uint8 *)((uint8 *)(rxPkt)
                  + (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + HOST_DESC_LENGTH
                     + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt)));

  msg_len = *(uint16 *)&uCmdRspBuf[0] & 0xfff;
  if (msg_len >= MAC_ADDR_LEN)
    msg_len -= MAC_ADDR_LEN; /* MAC address */

  if (msg_len > 0) {
    ie_len = (*(uint16 *)&uCmdRspBuf[0] & 0xfff) - 6;
  }

  mgmtif_process_add_sta_ind(buf, ie_len);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_rx_process_remove_sta_ind(uint8 *rxPkt)
{
  uint8 *buf = NULL;

  if (rxPkt == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  buf =
    ((uint8 *)(rxPkt)
     + (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt)));
  mgmtif_process_remove_sta(buf);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_rx_process_on_air_mgmt_pkt(uint8 *rxPkt, uint8 *send_to_host)
{
  int16 msg_len = 0;
  uint16 rcv_channel, band = 0;
  int rcv_freq;
  uint8 *buf                                       = NULL;
  uint8 subtype                                    = 0;
  int16 rcv_rssi                                   = 0;
  uint8 *uCmdRspBuf                                = NULL;
  rx_onair_extended_desc_t *rx_onair_extended_desc = NULL;

  if (rxPkt == NULL)
    return WLAN_STATUS_FAILURE;

  uCmdRspBuf  = (uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt);
  msg_len     = ((SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt) & 0xfff) - (SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt)));
  rcv_channel = uCmdRspBuf[8];
  /* supplicant will sort scan results based on this signal level.
  * signal level is negative value. We are getting absolute value of signal
  * level from TA. So, making it negative here. This makes supplicant to sort
  * properly.
  */
  if (SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt)) {
    // rssi value need to negate and passed to supplicant
    rx_onair_extended_desc =
      (rx_onair_extended_desc_t *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt) + HOST_DESC_LENGTH);
    rcv_rssi = -rx_onair_extended_desc->rssi;
  }

  /*WFD 5GHz and Dual band change */
  if (get_operating_mode() == WISE_MODE_P2P || is_dual_band()) {
    if ((rcv_channel > 14))
      band = SL_BAND_5GHZ;
    else
      band = SL_BAND_2P4GHZ;
  } else {
    band = get_current_freq_band();
  }

  rcv_freq = get_frequency(band, rcv_channel, is_japan_region());

  buf =
    (((uint8 *)rxPkt)
     + (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + HOST_DESC_LENGTH + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rxPkt)));
  subtype = buf[0] & IEEE80211_FC0_SUBTYPE_MASK;

  if (!is_connected_state()
      && (is_scan_supplicant_bypass()
          && ((subtype == IEEE80211_FC0_SUBTYPE_PROBE_RESP) || (subtype == IEEE80211_FC0_SUBTYPE_BEACON)))) {
    SL_PKT_RX_HDESC_SET_DW0_FRAME_TYPE(rxPkt, SLI_WLAN_RSP_SCAN_RESULTS);
    SL_PKT_RX_HDESC_SET_DW2_B2(rxPkt, (rcv_rssi & 0xFF));
    SL_PKT_RX_HDESC_SET_DW2_B0(rxPkt, rcv_freq & 0xFF);
    SL_PKT_RX_HDESC_SET_DW2_B1(rxPkt, (rcv_freq >> 8) & 0xFF);
    SL_PKT_RX_HDESC_SET_DW2_B3(rxPkt, rcv_channel);

    SL_PKT_RX_HDESC_SET_DW3_TOKEN(rxPkt, 0x0);
    *send_to_host = 1;
    SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, UMAC_DATA_QUEUE0);

    return WLAN_STATUS_SUCCESS;
  } else
    SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, UMAC_DATA_QUEUE1);

  return mgmtif_process_on_air_mgmt_pkt((uint8 *)rxPkt, buf, msg_len, rcv_freq, rcv_rssi, rcv_channel);
}

static int16 decode_mgmt_cmd_response(uint8 *rx_pkt)
{
  uint16 *rx_frame_desc;
  uint8 default_case = 0;
  uint8 send_to_host = 0;
  int16 retval       = WLAN_STATUS_SUCCESS;
  uint8 *uCmdRspBuf;

  if (rx_pkt == NULL)
    return WLAN_STATUS_FAILURE;

  rx_frame_desc = (uint16 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);
  uCmdRspBuf    = (uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);

  switch (SL_PKT_RX_HDESC_GET_DW0_FRAME_TYPE(rx_pkt)) {

    case ONAIR_MGMT: {
      mgmtif_rx_process_on_air_mgmt(rx_pkt);
    } break;

    case DECRYPT_ERROR_INDICATION: {
      if (rx_frame_desc[6] & 0x4) {
        mgmtif_send_mic_failure();
      }
    } break;

    case TX_STATUS_INDICATION: {
      mgmtif_tx_status_indication(SL_PKT_RX_HDESC_GET_DW3_STA_ID(rx_pkt), SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt));
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SLI_WLAN_CARD_READY_INDICATION_FROM_NWP: {
      // Todo: Process the contents of card ready response coming from NWP
      sl_mgmt_indicate_to_host(SLI_WLAN_CARD_READY_INDICATION_TO_HOST, 0, 0, NULL);
      send_to_host = 0;
    } break;

    case SLI_WLAN_RSP_AP_HT_CAPS: {
      if (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt) != 0)
        mgmtif_reset_ap_ht_params();

      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_AP_STOP:
    case SLI_WLAN_RSP_BEACON_STOP: {
      wlan_mgmt_if_event_handler(rx_pkt);
    } break;

#ifdef ENABLE_WLAN_RADIO_REQ
    case SLI_WLAN_RADIO_RSP_DEINIT: {
      mgmtif_radio_deinit_rsp();
      send_to_host = 1;
    } break;
#endif

#ifdef ENABLE_11R
    case SLI_AUTH_FT_IE_RESP: {
      uint16 allocation_size = (SL_PKT_RX_HDESC_GET_DW0_LENGTH(rx_pkt) - 1);
      uint32 offset          = 16 + 1;
      uint8 *ft_ies;

      //! First Byte contains FTIE response type. It may come through Auth
      //! response when Roaming mode is OTA. And it may come from ft action
      //! resonse from connected AP in ODS mode of roaming.
      if (*(uint8 *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) + 16) == 0) {
      } else if (*(uint8 *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) + 16) == 1) {
      } else {
        break;
      }

      ft_ies = sl_zalloc(allocation_size);
      if (ft_ies) {
        sl_memcpy(ft_ies, ((uint32 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) + offset), allocation_size);
        mgmtif_auth_ft_ie_rsp(ft_ies, allocation_size, *(uint8 *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt) + 16));
        sl_free(ft_ies);
        break;
      }
    } break;
#endif

    case FAST_PSP_TIM_INDICATION: {
      mgmtif_rx_indicate_fast_psp(SL_PKT_RX_HDESC_GET_DW3_STA_ID(rx_pkt));
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SLI_WLAN_RSP_PER_STATS: {
      send_to_host = 1;
    } break;

#ifdef ENABLE_11AX
    case SLI_WLAN_RSP_TWT_PARAMS: {
      send_to_host = 1;
    } break;
#endif

#ifdef TAG_ENABLE
    case TAG_DATA_PKT_SENT: {
      if (IS_TAG_MODE_ENABLE)
        send_to_host = 1;
    } break;
#endif
#ifdef PER_MODE_ENABLE
    case SLI_WLAN_RSP_PER_PARAMS: {
      send_to_host = 1;
    } break;
#endif
    case SLI_WLAN_RSP_CFG_SAVE: {
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_CONFIG: {
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_TIMEOUTS: {
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_RF_POWER_LEVEL: {
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_PING_CMD: {
      send_to_host = 1;
    } break;
#ifdef TSF_SYNC
    case SLI_WLAN_REQ_HOST_TSF_QUERY: {
      send_to_host = 1;
    } break;
#endif

    case LOAD_IE_RSP: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SET_KEY_PACKET_RSP: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case LOAD_AP_CAPABILITIES_RSP: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

#ifndef EXCLUDE_AP
      /*Validates PMKID in Assoc req in AP mode*/
    case VALIDATE_ASSOC_REQ_PMKID: {
      process_assoc_req_pmkid_req(rx_pkt);
    } break;
#endif /* EXCLUDE_AP */

    case LISTEN_CHANNEL_CMD_RESP: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SLI_WLAN_RSP_P2P_ENABLE: {
      wise_update_state(WISE_STATE_OPERMODE_SET);
      wise_update_concurrent_state(WISE_CON_STATE_OPERMODE_SET);
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_BAND: {
      if (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt) == 0) {
        wise_update_state(WISE_STATE_BAND_DONE);
        wise_update_concurrent_state(WISE_CON_STATE_BAND_DONE);
      }
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_SET_MAC: {
      wise_update_state(WISE_STATE_MAC_DONE);
      send_to_host = 1;
    } break;

    case SLI_WLAN_RSP_NETWORK_PARAMS: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
#ifdef DATA_PATH_UMAC_ENABLE
      process_network_params_rsp(rx_pkt);
#endif
      return 0;
    } break;

    case STATION_ADD_INDICATION: {
      mgmtif_rx_process_add_sta_ind(rx_pkt);
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case STATION_REMOVE_INDICATION: {
      mgmtif_rx_process_remove_sta_ind(rx_pkt);
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SLI_WLAN_RSP_DISCONNECT: {
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
      mgmtif_process_disconnect((uint8 *)rx_pkt, SL_PKT_RX_HDESC_GET_DW1_B3(rx_pkt));
      return 0;
    } break;

    case SLI_WLAN_RSP_SCAN: {
      mgmtif_process_scan_resp(&send_to_host, SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt));
    } break;

    case SLI_WLAN_RSP_BGSCAN: {
      send_to_host = 0; /*Assuming roaming enable*/
      mgmtif_rx_process_bgscan(rx_pkt);
    } break;

    case SET_CHANNEL_CMD_RSP: {
      mgmtif_process_channel_cmd_resp(*(uint16 *)&uCmdRspBuf[16]);
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
    } break;

    case SLI_WLAN_RSP_APCONF: {
      mgmtif_process_apconfig_resp(rx_pkt, SL_PKT_RX_HDESC_GET_DW3_TOKEN(rx_pkt));
      send_to_host = 1;
    } break;

    case ON_AIR_MGMT_PKT: {
      mgmtif_rx_process_on_air_mgmt_pkt(rx_pkt, &send_to_host);
    } break;

    case SLI_WLAN_RSP_JOIN: {
      mgmtif_process_join_resp(rx_pkt, &send_to_host, &default_case);
    } break;

    case SLI_WLAN_RSP_INIT: {
      mgmtif_process_init_rsp(rx_pkt, &send_to_host);
    } break;
    case SLI_WLAN_RSP_SET_FEATURE:
    case SLI_WLAN_RSP_SET_REGION:
    case SLI_WLAN_RSP_RSSI:
    case SLI_WLAN_RSP_WLAN_EXT_STATS:
    case SLI_WLAN_RSP_GET_WLAN_STATS:
    case SLI_WLAN_RSP_FILTER_BROADCAST:
    // The above case are ment to fall through
    default: {
      send_to_host = 1;
    } break;
  }

  if (send_to_host) {
    // Dword3 Queue ID bit is used to indicate whether the pkt should be freed.
    // Setting it to zero so that pkt is not freed until sending to host is complete.
    SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 0);
    retval = sli_send_host_rx_pkt(rx_pkt);
  } else if (!default_case) {
    SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rx_pkt, 1);
  }

  return retval;
}

int16 mgmtif_rx_process_mgmt_frame(uint8 *rxPkt)
{
  decode_mgmt_cmd_response(rxPkt);

  if (IS_FREE_RX_PKT(SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt))) {
#if (defined TRACE_PKB_SRC && !(defined ENABLE_DYNAMIC_POOL))
    if (detect_mem_leak[(((uint32)rxPkt - (uint32)WLAN_HEAP_START) / PKT_CHUNK_SIZE)].dealloc_done == 1) {
      SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_MEMORY_FREE_DETECTED_2_IN_PROCESS_RX_PKT_FROM_LMAC);
    }
#endif
    sl_free_rx_pkt(rxPkt);
  }

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_rx_process_data_frame(uint8 *rxPkt, uint32 vap_id)
{
  uint16 rx_packet_free = 0;

  if (get_operating_mode() != WISE_CONCURRENT_MODE)
    vap_id = 0;

  /* add eapol_rx here */
  if (!is_eapol_pkt(rxPkt, vap_id)) {
#ifdef DATA_PATH_UMAC_ENABLE
    rx_packet_free = is_sl_ip_pkt(rxPkt, vap_id);
#endif
  } else {
    return WLAN_STATUS_SUCCESS;
  }

  if (rx_packet_free) {
    sl_free_rx_pkt(rxPkt);
  }

  return WLAN_STATUS_SUCCESS;
}
