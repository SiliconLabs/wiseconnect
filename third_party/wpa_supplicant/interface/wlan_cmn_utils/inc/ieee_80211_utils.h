/*******************************************************************************
* @file  ieee_80211_utils.h
* @brief This file includes the utility functions that are defined on top
*        of IEEE 802.11 definitions.
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
#ifndef IEEE_80211_UTILS_H
#define IEEE_80211_UTILS_H

#include "ieee_80211_def.h"
//#include "umac_platform_if.h"
#include "sl_lib_utils_rtos.h"

#ifdef CONFIG_IEEE_80211J
extern const uint16 BAND_4_9GHZ_CHANNEL_MAP[];
#endif

extern const uint8 channel_map_5GHz[];

/* Macro functions for mac address compare and copy */
#define IEEE80211_ADDR_EQ(a1, a2)     (sl_memcmp(a1, a2, MAC_ADDR_LEN) == 0)
#define IEEE80211_ADDR_COPY(dst, src) sl_memcpy(dst, src, MAC_ADDR_LEN)
#define IS_MBCAST_ADDR(mac_0b)        (mac_0b & 0x01)

#define ieee_for_each_element(_elem, _data, _datalen)                                                         \
  for (_elem = (const element_t *)(_data);                                                                    \
       (const uint8 *)(_data) + (_datalen) - (const uint8 *)_elem >= (int)sizeof(*_elem)                      \
       && (const uint8 *)(_data) + (_datalen) - (const uint8 *)_elem >= (int)sizeof(*_elem) + _elem->datalen; \
       _elem = (const element_t *)(_elem->data + _elem->datalen))

#define ieee_for_each_element_id(element, _id, data, datalen) \
  ieee_for_each_element(element, data, datalen) if (element->id == (_id))

uint16 iswpaoui(const uint8 *frm);
int iswmeinfo(const uint8 *frm);
uint8 check_mac_empty(uint8 *mac_addr);
uint8 *ieee80211_add_cfparms(uint8 *frm);
void add_wme_param(uint8 *frm);
void add_wpa1_ie(uint8 *frm);
uint8 verify_element(uint8 *element, uint16 max_len);
void add_rsn_ie(uint8 *frm);
uint8 is11bclient(const uint8 *rates, const uint8 *xrates);
uint8 islegacystation(uint8 *vendor);

#ifdef RSI_ENABLE_CCX
uint8 *ieee80211_add_ccx_info(uint8 *frm);
uint8 *ieee80211_add_aironet_ie(uint8 *frm, struct ieee80211_aironet_ie *ni_ies);
uint8 *ieee80211_add_cckm_info(uint8 *frm, uint64 tsf, uint32 rn, uint64 mic);
uint8 *ieee80211_add_ccx_radio_cap_ie(uint8 *frm, struct ccx_radio_cap_ie *ni_ccx_radio_cap_ie);
uint8 *ieee80211_add_aironet_ap_ip_ie(uint8 *frm);
#endif /* RSI_ENABLE_CCX */

#ifdef ENABLE_11AX
void prepare_fill_ext_cap_ie(uint8 *buff, uint8 ie_len);
void fill_ext_cap_ie(uint8 *buff, uint8 bit);
uint8 *add_rrm_ie(uint8 *buff);
uint8 *add_twt_constraint_params(uint8 *buff);
#ifdef CONFIG_IEEE_80211W
void encap_ccmp_header(uint8 *frm, uint8 keyid);
#endif /* CONFIG_IEEE_80211W */
uint8 *add_he_cap_ie(uint8 *buff, uint8 twt_support);
#endif /* ENABLE_11AX */

void encap_mac_hdr(uint8 *frm, uint8 type, uint8 *da, uint8 *sa, uint8 *bssid, uint16 seq_no, uint16 duration);
int32 fill_p2p_action_frame_mac_hdr(ieee80211_macframe_t *mac_hdr, uint8 seq_num);
uint8 parse_rsn_ie(const uint8 *rsn_ie, size_t rsn_ie_len);

#endif /* IEEE_80211_UTILS_H */
