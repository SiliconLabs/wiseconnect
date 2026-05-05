/*******************************************************************************
* @file  wlan_cmn_utils.h
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
#ifndef WLAN_CMN_UTILS_H
#define WLAN_CMN_UTILS_H

#include "ieee_80211_def.h"
#include "sl_mgmt_if_thread.h"

#define GET_FREQ_BAND(f) (((f >= 2412) && (f <= 2484)) ? SL_BAND_2P4GHZ : SL_BAND_5GHZ)

enum oper_mode {
  HOST_OP_CLIENT_MODE            = 0,
  HOST_OP_P2P_MODE               = 1,
  HOST_OP_ENTERPRISE_CLIENT_MODE = 2,
  HOST_OP_ACCESS_POINT_MODE      = 6,
  HOST_OP_BTR_MODE               = 7,
  HOST_OP_PER_MODE               = 8,
  HOST_OP_CONCURRENT_MODE        = 9
};

enum wise_mode_e {
  WISE_MODE_CONNECTION = 0,
  WISE_MODE_P2P,
  WISE_MODE_EAP,
  WISE_MODE_AP,
  WISE_MODE_PER,
  WISE_CONCURRENT_MODE,
#if ((defined RAM_VER1) || (defined RAM_VER2))
  WLAN_BTR_MODE = 7,
  WISE_MODE_MAX
#endif
};

enum wise_state_e {
  WISE_STATE_INIT = 0,
  WISE_STATE_OPERMODE_SET,
  WISE_STATE_MAC_DONE,
  WISE_STATE_BAND_DONE,
  WISE_STATE_INIT_DONE,
  WISE_STATE_P2P_CFG_DONE,
  WISE_STATE_EAP_CFG_DONE,
  WISE_STATE_SCAN_DONE,
  WISE_STATE_CONNECTED,
  WISE_STATE_IP_CONFIG_DONE,
  WISE_STATE_IPV6_CONFIG_DONE,
  WISE_STATE_TARNS_MODE_DONE,
  WISE_STATE_APCONF_DONE
};

enum wise_state_con_ap_e {
  WISE_CON_STATE_INIT = 0,
  WISE_CON_STATE_OPERMODE_SET,
  WISE_CON_STATE_MAC_DONE,
  WISE_CON_STATE_BAND_DONE,
  WISE_CON_STATE_INIT_DONE,
  WISE_CON_STATE_EAP_CFG_DONE,
  WISE_CON_STATE_CONNECTED,
  WISE_CON_STATE_IP_CONFIG_DONE,
  WISE_CON_STATE_APCONF_DONE
};

uint8 set_operating_mode(uint8 oper_mode);
uint8 reverse_translate_channel(uint8 channelNum);
uint8 translate_channel(uint8 channelNum);
#ifdef CONFIG_IEEE_80211J
uint8 valid_80211J_channel(uint32 channel);
#endif /* CONFIG_IEEE_80211J */
uint16 get_frequency(uint16 band, uint16 rcv_channel, uint8 enabled_11j_japan_region);
int16 freq_to_bitmap_convert(int *freqs, uint16 *bitmap);
uint16 get_channel_num(uint16 freq);
uint8 reverse_translate_80211j_channel(uint32 channel);
int8 check_inheritance(const struct element_s *ie, const struct element_s *non_inherit_ie);
sl_status_t free_tx_host_q(void);
#endif /* WLAN_CMN_UTILS_H */
