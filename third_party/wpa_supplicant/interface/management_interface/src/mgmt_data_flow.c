/*******************************************************************************
* @file  mgmt_data_flow.c
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
#ifdef DATA_PATH_UMAC_ENABLE
#include "rsi_wsc_non_rom.h"
#include "umac_events.h"
#include "mgmt_platform_if.h"
#include "mgmt_if_utils.h"
#include "wrappers_wlan.h"
#include "rsi_controller_apme.h"
#if (defined(NETX_ENABLE) && defined(IPV6_ENABLE))
#include "netx_porting_non_rom.h"
#endif /* (defined(NETX_ENABLE) && defined(IPV6_ENABLE)) */

extern uint8 glbl_max_station_supported;
#ifdef SSL_16K_RECORD
extern struct ssl_buffer_available_s ssl_buffer_available;
#endif
extern TCPIPEvt_t common_event_resp;
extern TX_EVENT_FLAGS_GROUP event_flags_1;

int16 is_rom_sl_ip_pkt(uint8 *rxPkt, uint8 vap_id);
int16 sl_process_rx_data_pkt(uint8 *rx_pkt, uint8 vap_id);

#ifdef ROM_IMAGE_20
void set_tls_version_1()
{
  rsi_wsc_cb_non_rom.sc_params_non_rom.tls_version = 1;
}
#endif

int16 process_network_params_rsp(uint8 *rx_pkt)
{
  struct app_info_s *app_info                 = &glbl_app_info;
  NET_PARAMS_FRM_MODULE *net_param_frm_module = NULL;
  TCP_EVT_NET_PARAMS *stNetParams;
  uint8 *uCmdRspBuf;

  uCmdRspBuf           = (uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);
  uint8 *linear_buffer = NULL;

  if (SL_PKT_GET_RXPKT_SCATTER_COUNT(rx_pkt) == 1) {
    net_param_frm_module = (NET_PARAMS_FRM_MODULE *)((rx_pkt)
                                                     + (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) + HOST_DESC_LENGTH
                                                        + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt)));
  } else {
    linear_buffer = sli_form_linear_buffer(rx_pkt);
    if (linear_buffer == NULL) {
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_NETWORK_PARAMS, 0, MEM_ALLOC_FAILED, NULL);
      return WLAN_STATUS_FAILURE;
    }
    net_param_frm_module = (NET_PARAMS_FRM_MODULE *)linear_buffer;
  }
#if (defined ENABLE_DNS && defined ENABLE_FEAT_SEL)
  interface_ptr = MK_INTERFACE_PTR(my_interface_no);
#endif

  stNetParams = &common_event_resp.EvtBody.stNetParams;
  sl_memset((uint8 *)stNetParams, 0, sizeof(TCP_EVT_NET_PARAMS));

  stNetParams->wlan_state      = net_param_frm_module->wlan_state;
  stNetParams->connection_type = net_param_frm_module->connection_type;

  if (net_param_frm_module->wlan_state) {
    sl_memcpy(stNetParams->ssid, (uint8 *)net_param_frm_module->ssid, 32);
  }

  if (net_param_frm_module->wlan_state) {
    stNetParams->sec_type = net_param_frm_module->sec_type;
    stNetParams->Chn_no   = net_param_frm_module->Chn_no;

    sl_memzero(stNetParams->psk, SL_PSK_LEN);

    if (net_param_frm_module->sec_type) {
#ifdef WPA3_PMK_DEBUG
      struct wpa_supplicant *wpa_s = rsi_wsc_cb.supplicant_priv;
#endif
      if ((rsi_wsc_cb.sc_params.feature_bit_map & FEAT_HIDE_PSK_CREDENTIALS)
          // Below security type check is to handle WPA3 transition with WPA3 authentication
          // Here the pmk is already filled with the WPA2 PMK
          // To avoid publishing the wrong PMK of WPA3 connection, clearing the psk.
          || (net_param_frm_module->sec_type == SEC_MODE_WPA3)
#ifdef FIPS_CODE_ENABLE
          || rsi_wsc_cb.fips.fips_mode_enable
#endif
      ) {
        /* Clear sensitive data before giving network params to host */
        sl_memzero(stNetParams->psk, SL_PSK_LEN);
      } else {
        sl_memcpy(stNetParams->psk, wise_get_pmk(), SL_PMK_LEN);
      }
#ifdef WPA3_PMK_DEBUG
      sl_memcpy(stNetParams->psk, wpa_s->wpa->pmk, SL_PMK_LEN);
#endif
    }
  }
  sl_memcpy(stNetParams->mac_addr, app_info->PermanentAddress, MAC_ADDR_LEN);
  stNetParams->num_open_socks = 0;
  if (!rsi_wsc_cb.tcp_bypass_mode) {
    //! Fill in non TCP/IP bypass mode
    fill_nw_params(net_param_frm_module->wlan_state, stNetParams);
  }

  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_NETWORK_PARAMS,
                           sizeof(TCP_EVT_NET_PARAMS),
                           NO_ERROR,
                           (uint8 *)&common_event_resp);

  if (linear_buffer) {
    sl_free(linear_buffer);
  }

  return WLAN_STATUS_SUCCESS;
}

int16 is_rom_sl_ip_pkt(uint8 *rxPkt, uint8 vap_id)
{
  return is_ip_pkt((wlan_rx_pkt_t *)rxPkt, vap_id);
}

int16 is_sl_ip_pkt(uint8 *rxPkt, uint8 vap_id)
{
  return is_ip_pkt((wlan_rx_pkt_t *)rxPkt, vap_id);
}

void sl_handle_tcp_packet(uint8 *pkt)
{
  handle_tcp_packet((wlan_rx_pkt_t *)pkt);
}

int16 sl_process_rx_data_pkt(uint8 *rx_pkt, uint8 vap_id)
{
  return rsi_process_rx_data_pkt(rx_pkt, vap_id);
}

void sl_free_netx_malloc_mem(uint8 vap_id)
{
  free_netx_malloc_mem(vap_id);
}

void get_go_ip_address_info(void *go_params_p, void *sta_p)
{
  rsi_GOParams *go_params = (rsi_GOParams *)go_params_p;
  struct sta_info_s *sta  = (struct sta_info_s *)sta_p;
  struct go_sta_info_s *sta_info;
  uint16 ii;
  uint8 got_ip = 0;
  uint8 ip[20] = { 0 };
  if (tcp_ip_cb.tcp_stack_used & BIT(TCP_IP_STACK_V4)) {
    if (tcp_ip_cb.operating_mode == WISE_CONCURRENT_MODE) {
      get_eth_src_addr(go_params->ipv4_address, 1);
    } else {
      get_eth_src_addr(go_params->ipv4_address, 0);
    }
  }
#ifdef IPV6_ENABLE
  if (tcp_ip_cb.tcp_stack_used & BIT(TCP_IP_STACK_V6)) {
    get_eth_src_addr6((ULONG *)go_params->ipv6_address);
  }
#endif
  sta_info = go_params->sta_info;
  for (ii = 0; ii < glbl_max_station_supported; ii++, sta++) {
    if (sta->state & BIT(0)) {
      sl_memcpy(sta_info->mac, sta->mac, 6);
#ifdef DHCPV4_SERVER_ENABLE
      got_ip = get_ip_addr(sta_info->mac, ip);
#endif
      if (got_ip != 0) {
        sta_info->ip_version = NX_IP_VERSION_V4;
        sl_memcpy(sta_info->ip_address.ipv4_address, ip, 4);
      }
#ifdef NETX_ENABLE
#ifdef IPV6_ENABLE
      else {
#ifdef DHCPV6_SERVER_ENABLE
        got_ip = get_ip_addr6(sta_info->mac, ip);
        if (got_ip != 0) {
          sta_info->ip_version = NX_IP_VERSION_V6;
          sl_memcpy(sta_info->ip_address.ipv6_address, ip, IPV6_ADDRESS_SIZE);
        }
#endif
      }
#endif
#endif
      sta_info++;
    }
  }
}
void dhcp_release_wrapper(uint8 *buf)
{
  if (tcp_ip_cb.tcp_stack_used & BIT(TCP_IP_STACK_V4)) {
    dhcp_release(buf);
  }
#ifdef NETX_ENABLE
#ifdef IPV6_ENABLE
  if (tcp_ip_cb.tcp_stack_used & BIT(TCP_IP_STACK_V6)) {
    dhcpv6_release(buf);
  }
#endif
#endif
}

#if ((defined ENABLE_KRACK_PATCH) || (defined KRACK_PN_ENABLE))

/* [KRACK] PN (IV) NUMBER FIX */

static inline int32 ieee80211_compare_pn(uint8 *pn1, uint8 *pn2, int32 len)
{
  int32 i;
  const uint8 *s1 = (const uint8 *)pn1;
  const uint8 *s2 = (const uint8 *)pn2;

  /* pn is little endian */
  for (i = len - 1; i >= 0; i--) {
    if (s1[i] < s2[i]) {
      return -1;
    } else if (s1[i] > s2[i]) {
      return 1;
    }
  }
  return 0;
}
#endif

int16 is_ip_pkt_ram(uint8 *rxPkt, uint8 vap_id)
{
#if ((defined ENABLE_KRACK_PATCH) || (defined KRACK_PN_ENABLE))
  /* [KRACK] PN (IV) NUMBER FIX */
  uint8 *buf = (uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0);
  int32 rc;
  if (IS_PN_VALID(SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt)) && (rsi_wsc_cb_non_rom.sta_connection_secMode == SEC_MODE_WPA2)) {
    /* broadcast/multicast address */
    if (buf[0] & 1) {
      uint8 *msg            = (uint8 *)&(SL_PKT_RX_HDESC_GET_DW2(rxPkt));
      rsi_wsc_cb_non_rom.pn = sl_read_6(msg[0], msg[1], msg[2], msg[3], msg[4], msg[5]);
      rc = ieee80211_compare_pn((uint8 *)&rsi_wsc_cb_non_rom.pn, (uint8 *)&rsi_wsc_cb_non_rom.prev_pn, 6);
      if (rc <= 0) {
        SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);
        return 1;
      } else {
        rsi_wsc_cb_non_rom.prev_pn = rsi_wsc_cb_non_rom.pn;
      }
    }
  }
#endif
  if ((tcp_ip_cb.tcp_ip_feature_bit_map & TCP_IP_FEAT_EXTENTION_VALID)
      && ((tcp_ip_cb.ext_tcp_ip_feature_bit_map & EXT_TCP_IP_DUAL_MODE)
          || (tcp_ip_cb.ext_tcp_ip_feature_bit_map & EXT_TCP_IP_ETH_BRIDGE_MODE))) {
    return sl_process_rx_data_pkt((uint8 *)rxPkt, vap_id);
  } else
    return is_rom_sl_ip_pkt((uint8 *)rxPkt, vap_id);
}

#endif /* DATA_PATH_UMAC_ENABLE */
