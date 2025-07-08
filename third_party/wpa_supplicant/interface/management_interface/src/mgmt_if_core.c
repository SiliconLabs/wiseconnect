/*******************************************************************************
* @file  mgmt_if_core.c
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
#include "mgmt_if_core.h"
#include "mgmt_if_header.h"
#include "mgmt_if_utils.h"
#include "mgmt_supplicant_if.h"
#include "sli_wlan_command_interface.h"
#include "sl_pkt_utils.h"
#include "mgmt_if_tx.h"
#include "mgmt_if_rsi_wsc.h"

struct mgmt_if_adapter_s mgmt_if_adapter;

#ifdef CONFIG_P2P
void wsc_connect()
{
  if (mgmt_if_adapter.supplicant_priv == NULL)
    return;

  if ((*(uint16 *)mgmt_if_adapter.go_intent) < WISE_AUTONOMOUS_GO) {
    mgmt_if_adapter.p2p_curr_state = P2P_SM_INIT;
    sli_p2p_fsm(P2P_EVENT_START_FULL_SCAN);
  }
}
#endif /*CONFIG_P2P*/

int16 mgmtif_tx_status_indication(uint32 sta_id, uint32 token)
{
  if ((sta_id == EAPOL4_CONFIRM) && (token == 1)) {
    /*EAPOL 4-way handshake success*/
    if (mgmt_if_adapter.cfm_required == SL_TRUE) {
      wsc_supp_join_done(1);
    }
  } else if ((sta_id == EAPOL4_CONFIRM) && (token == 0)) {
    /*EAPOL 4-way handshake failure*/
    if (mgmt_if_adapter.state < WISE_STATE_CONNECTED)
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, EAPOL_4_WAY_HANDSHAKE_FAIL, NULL);
  }

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_reset_ap_ht_params()
{
  mgmt_if_adapter.is_11n_ap = 0;
  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_radio_deinit_rsp()
{
  if (mgmt_if_adapter.wlan_radio_deinit_from_host == 2 && mgmt_if_adapter.state == WISE_STATE_SCAN_DONE) {
    wise_update_state(WISE_STATE_INIT_DONE);
  }

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_disconnect(uint8 *rxPkt, uint32 vap_id)
{
  int16 retval              = WLAN_STATUS_SUCCESS;
  mgmt_if_adapter.delete_ap = *(uint8 *)((uint8 *)rxPkt + SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rxPkt) + HOST_DESC_LENGTH);

  if (mgmt_if_adapter.host_cmd != SLI_MGMT_REQ_DISCONNECT) {
    if ((((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 0)) || !mgmt_if_adapter.ap_mode_set)
        && mgmt_if_adapter.rejoin_going_on && (mgmt_if_adapter.rejoin_count++ >= MAX_REJOIN_RETRIES)) {
      /* Re-initialize the Supplicant */
      wise_reset_to_band(0);
    }
    return retval;
  }
  mgmt_if_adapter.host_cmd = 0;

  if (!mgmt_if_adapter.ap_mode_set || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 0))) {
    /* Re-initialize the Supplicant */
    wise_reset_to_band(0);
  }

  if (mgmt_if_adapter.dyn_sta_ap_switch_enable && mgmt_if_adapter.ap_mode_set
      && ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 1) && mgmt_if_adapter.delete_ap)) {
    wise_reset_ap(1);
  }

  // Dword3 Queue ID bit is used to indicate whether the pkt should be freed.
  // Setting it to zero so that pkt is not freed until sending to host is complete.
  SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 0);
  retval = sli_send_host_rx_pkt((uint8 *)rxPkt);
  return retval;
}

int16 mgmtif_process_scan_results()
{
  //! Send bgscan result as scan result in case of AP mode scan
  if (mgmt_if_adapter.operating_mode == WISE_MODE_AP) {
    get_scan_results(0);
  } else {
    get_scan_results(1);
  }

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_on_air_mgmt_pkt(uint8 *rxPkt,
                                     uint8 *buf,
                                     int16 msg_len,
                                     int rcv_freq,
                                     int16 rcv_rssi,
                                     uint16 rcv_channel)
{
  int16 retval         = WLAN_STATUS_SUCCESS;
  uint8 *linear_buffer = 0;

  if (buf == NULL || rxPkt == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (SL_PKT_GET_RXPKT_SCATTER_COUNT(rxPkt) == 1) {
    retval = sli_process_on_air_mgmt(buf, msg_len, rcv_freq, rcv_rssi, rcv_channel);
  } else {
    linear_buffer = sli_form_linear_buffer(rxPkt);
    if (linear_buffer != NULL) {
      retval = sli_process_on_air_mgmt(linear_buffer, msg_len, rcv_freq, rcv_rssi, rcv_channel);
    }
  }

  if (linear_buffer) {
    sl_free(linear_buffer);
  }

  return retval;
}

void rsi_sta_connected_notification_to_host_wrapper(void)
{
  struct conn_notify_to_host_input_arg *conn_notify_input = &supp_mgmt_if_ctx.conn_notify_to_host;
  if (conn_notify_input == NULL) {
    return;
  }

  rsi_sta_connected_notification_to_host(conn_notify_input->mac_addr, conn_notify_input->con_event_type);
}

void wise_notify_p2p_dev_found_wrapper(void)
{
  const struct notify_p2p_dev_found_input_arg *notify_p2p_dev_input = &supp_mgmt_if_ctx.notify_p2p_dev_input;
  if (notify_p2p_dev_input == NULL) {
    return;
  }

  wise_notify_p2p_dev_found(notify_p2p_dev_input->new_device,
                            notify_p2p_dev_input->device_name,
                            notify_p2p_dev_input->p2p_device_addr,
                            notify_p2p_dev_input->pri_dev_type);
}

void wsc_supp_join_done_wrapper(void)
{
  wsc_supp_join_done(supp_mgmt_if_ctx.supp_join_done_cmd_input.client);
}

void wise_update_psk_in_nwparams_wrapper()
{
  struct update_psk_in_nwparams_input_arg *update_psk_input = &supp_mgmt_if_ctx.update_psk_input;
  if (update_psk_input == NULL) {
    return;
  }

  wise_update_psk_in_nwparams(update_psk_input->key, update_psk_input->key_len);
}

void wise_wps_ap_configured_cb(void *ctx, void *data)
{
#ifdef RSI_GLOBAL_VARIABLES
  set_client_mode(0);
#endif /* RSI_GLOBAL_VARIABLES */
  wsc_supp_join_done(0);
}

int rsi_drv_send_action_wrapper(void *priv,
                                unsigned int freq,
                                unsigned int wait,
                                const uint8 *dst,
                                const uint8 *src,
                                const uint8 *bssid,
                                const uint8 *data,
                                size_t data_len,
                                int no_cck)
{
  uint8 is_11w_enabled = 0;

#ifdef CONFIG_IEEE_80211W
  if (mgmt_if_adapter.is_11w_enabled) {
    is_11w_enabled = 1;
  }
#endif

  return rsi_drv_send_action(data, data_len, dst, src, bssid, freq, no_cck, is_11w_enabled);
}

int wpa3_send_sae_handshake_packet_wrapper(void *priv,
                                           const uint8 *data,
                                           size_t data_len,
                                           int noack,
                                           unsigned int freq,
                                           const uint16 *csa_offs,
                                           size_t csa_offs_len)
{
  return wpa3_send_sae_handshake_packet(data, data_len);
}

int mgmtif_async_notify_ap_not_found()
{
  /* This is the case where configured AP is not found */
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
  if (mgmt_if_adapter.state >= WISE_STATE_CONNECTED) { /* During bgscan */
    mgmt_if_adapter.async_state_code = RSI_MODULE_STATE_CURRENT_AP_IS_BEST;
  } else { /* During rejoin */
    mgmt_if_adapter.async_state_code = RSI_MODULE_STATE_NO_AP_FOUND | RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT;
    mgmt_if_adapter.async_rssi       = RSI_MODULE_STATE_RSSI_NOT_AVAILABLE;
    mgmt_if_adapter.async_channel    = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
    if ((mgmt_if_adapter.async_reason_code & RSI_MODULE_STATE_BEACON_LOSS)
        || (mgmt_if_adapter.async_reason_code & RSI_MODULE_STATE_DEAUTH_FROM_AP)) {
      mgmt_if_adapter.async_reason_code_bkp = mgmt_if_adapter.async_reason_code;
    }
    mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT;
    sl_memzero(mgmt_if_adapter.async_bssid, MAC_ADDR_LEN);
  }
  wise_module_state(RSI_MODULE_STATE_SCAN_RESULT);

  /* Reset the state to scanning state */
  if (mgmt_if_adapter.async_reason_code == RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT) {
    mgmt_if_adapter.async_state_code = mgmt_if_adapter.async_reason_code_bkp;
  } else {
    mgmt_if_adapter.async_state_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
  }
  mgmt_if_adapter.async_rssi    = RSI_MODULE_STATE_RSSI_NOT_AVAILABLE;
  mgmt_if_adapter.async_channel = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
#endif

  if (is_client_mode() && !is_rejoin_happenig() && !is_wps_client()) {
    /* No AP Found */
    /* Send join response from here ERROR message */
    if (((mgmt_if_adapter.state >= WISE_STATE_CONNECTED) && (!is_bgscan_enabled()))
        || (mgmt_if_adapter.state < WISE_STATE_CONNECTED)) {
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
      /* This is the case where configured AP is not found */
      mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_UNASSOCIATED | RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT;
      mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT;
      mgmt_if_adapter.async_rssi        = RSI_MODULE_STATE_RSSI_NOT_AVAILABLE;
      mgmt_if_adapter.async_channel     = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
      sl_memzero(mgmt_if_adapter.async_bssid, MAC_ADDR_LEN);
      wise_module_state(RSI_MODULE_STATE_FINAL_CONNECTON);
#endif
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, P2P_DEV_NOT_FOUND, NULL);
      return -1;
    }
  }
  return 0;
}
