/*******************************************************************************
* @file  mgmt_if_utils.c
* @brief This contains utility functions for Management interface.
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
#include "mgmt_if_utils.h"
#include "mgmt_if_core.h"
#include "mgmt_supplicant_if.h"
#include "mgmt_if_rsi_wsc.h"

uint16 is_pmk_present(void)
{
  uint8 pmk_null[32];

  sl_memset(pmk_null, 0, 32);

  return (
    ((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION)
     || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (mgmt_if_adapter.state < WISE_STATE_CONNECTED)))
    && (mgmt_if_adapter.pmk_stored) && (sl_memcmp(mgmt_if_adapter.pmk, pmk_null, 32)));
}

uint8 is_wpa3_mode(void)
{
  return (mgmt_if_adapter.csec_mode == SEC_MODE_WPA3);
}

void wise_store_pmk(char *pmk)
{
  mgmt_if_adapter.pmk_stored = SL_TRUE;
  sl_memcpy(mgmt_if_adapter.pmk, pmk, 32 /* PMK_LEN */);
}

char *wise_get_pmk(void)
{
  char *pmk = NULL;
  pmk       = (char *)mgmt_if_adapter.pmk;
  return pmk;
}

#ifdef RSI_ENABLE_CCX
/* check for CCX Elemnet IE */
int isccxoui(const uint8 *frm)
{
  return ((frm[2] == 0x00) && (frm[3] == 0x40) && (frm[4] == 0x96));
}
#endif

/* Need to removed if no dependency in supplicant_rom_image.c*/
/* Dummy function */
void supplicant_stub(void)
{ /* Intentionally left blank */
}

void wise_update_state(uint8 state)
{
  mgmt_if_adapter.state = state;
}

/*==============================================*/
/**
 * @fn
 * @brief
 * @param[in]   , 
 * @param[out]
 * @return
 *
 *
 * @section description
 * This 
 *
 *
 */
/*Isolate from Netx and move to mgmt_if_utils*/
uint8 sl_get_concurrent_mode_state(void)
{
  return mgmt_if_adapter.concurrent_mode_state;
}

void wise_update_concurrent_state(uint8 state)
{
  mgmt_if_adapter.concurrent_mode_state = state;
}

void wise_update_prev_state(uint8 state)
{
  mgmt_if_adapter.prev_state = state;
}

void set_join_cmd_from_host_flag(uint8 join_from_host)
{
  mgmt_if_adapter.join_from_host = join_from_host;
}

uint8 is_join_cmd_from_host(void)
{
  return mgmt_if_adapter.join_from_host;
}

uint8 is_japan_region(void)
{
  return ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
          && (mgmt_if_adapter.region_code == REGION_JAP_NUM));
}

uint8 is_scan_supplicant_bypass(void)
{
  return (mgmt_if_adapter.scan_feature_bitmap & FEATURE_SCAN_SUPPLICANT_BYPASS) != 0;
}

int16 is_wps_client(void)
{
  return mgmt_if_adapter.wps_station;
}

void sl_set_wps_client(uint8 wps_client_flag)
{
  mgmt_if_adapter.wps_station = wps_client_flag;
}

uint8 *sl_get_ssid_from_mgmt_if_adapter(void)
{
  return (uint8 *)mgmt_if_adapter.ssid;
}

void set_supplicant_trigger_bg_scan(uint8 supplicant_triggered_bg_scan)
{
  mgmt_if_adapter.supplicant_triggered_bg_scan = supplicant_triggered_bg_scan;
}

uint8 is_dyn_sta_ap_switch_enabled(void)
{
  return (mgmt_if_adapter.dyn_sta_ap_switch_enable ? SL_TRUE : SL_FALSE);
}

uint8 is_band_24ghz(void)
{
  if ((supp_mgmt_if_ctx.wpa_config_read_data.module_type == 0)
      || ((mgmt_if_adapter.band == SL_BAND_2P4GHZ) && (supp_mgmt_if_ctx.wpa_config_read_data.module_type == 1))) {
    return 1;
  } else {
    return 0;
  }
}

uint8 is_blacklist_feature_enabled(void)
{
  return (mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_DISABLE_SUPP_BLACKLIST ? SL_TRUE : SL_FALSE);
}

uint8 is_rsa_4096_supported(void)
{
  return (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT ? SL_TRUE : SL_FALSE);
}

void supp_set_roam_indication_and_bssid(void)
{
  struct supp_roam_bssid_update_input_arg *roam_bssid_and_ind_update = &supp_mgmt_if_ctx.roam_bssid_ind_update_input;
#ifdef PS_DEPENDENT_VARIABLES
  set_roam_ind(roam_bssid_and_ind_update->roam_indication);
#endif /* PS_DEPENDENT_VARIABLES */
  mgmt_if_adapter.roam_ind = roam_bssid_and_ind_update->roam_indication;
  if (roam_bssid_and_ind_update->roam_bssid != NULL) {
    sl_memcpy((void *)mgmt_if_adapter.roam_bssid, (void *)roam_bssid_and_ind_update->roam_bssid, MAC_ADDR_LEN);
  } else {
    sl_memset((void *)mgmt_if_adapter.roam_bssid, 0, MAC_ADDR_LEN);
  }
}

void supplicant_set_client(void)
{
#ifdef RSI_GLOBAL_VARIABLES
  set_client_mode(supp_mgmt_if_ctx.client_mode);
#endif /* RSI_GLOBAL_VARIABLES */
  mgmt_if_adapter.client = supp_mgmt_if_ctx.client_mode;
}

uint8 mac_based_join_mismatch_check(uint8 *bssid)
{
  if (mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_MAC_BASED_JOIN) {
    //! Check for mac address given by host and selected by supplicant
    //! If not matching then return from here with 0 - as error
    if (sl_memcmp(bssid, mgmt_if_adapter.join_bssid, MAC_ADDR_LEN) != 0) {
      return 1;
    }
  }
  return 0;
}

uint8 prev_bssid_mismatch_check(uint8 *bssid)
{
  if (sl_memcmp(bssid, get_bssid_from_mgmt_intf_adapter(), MAC_ADDR_LEN) != 0) {
    return 1;
  }
  return 0;
}

uint16 get_current_freq_band(void)
{
  return mgmt_if_adapter.current_freq_band;
}

uint8 *get_bssid_from_mgmt_intf_adapter(void)
{
  return mgmt_if_adapter.bssid;
}

int32 get_rejoin_scan_interval(void)
{
  if (mgmt_if_adapter.rejoin_going_on == SL_TRUE) {
    if (mgmt_if_adapter.rejoin_scan_interval)
      return mgmt_if_adapter.rejoin_scan_interval;
  }

  return 0;
}

int32 get_first_time_retry_enable(void)
{
  return mgmt_if_adapter.first_time_retry_enable;
}
uint8 is_dual_band()
{
  return mgmt_if_adapter.dual_band == SL_BAND_DUAL;
}

uint8 is_instant_bg_enabled(void)
{
  return mgmt_if_adapter.instant_bg_flag;
}

void clear_instant_bg_flag(void)
{
  mgmt_if_adapter.instant_bg_flag = SL_FALSE;
}

uint8 is_bgscan_enabled(void)
{
  return mgmt_if_adapter.bgscan_enabled;
}

uint8 is_p2p_mode(void)
{
  return mgmt_if_adapter.operating_mode == WISE_MODE_P2P;
}

uint8 is_connected_state(void)
{
  return mgmt_if_adapter.state >= WISE_STATE_CONNECTED;
}

uint8 is_eap_okc_enabled(void)
{
  return (mgmt_if_adapter.operating_mode == WISE_MODE_EAP) && ((mgmt_if_adapter.okc & BIT(0)) == 1);
}

uint8 get_operating_mode(void)
{
  return mgmt_if_adapter.operating_mode;
}

uint8 mgmtif_is_tcp_bypass_enabled(void)
{
  if (mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_TCP_IP_BYPASS_ENABLE_SME)
    return 1;
  else
    return 0;
}

void send_ap_join_fail_response(void)
{
  uint8 go = 'G';
  /* Send join response here */
  sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 1, WISE_ERROR_UNABLE_TO_FORM_AUTOGO, &go);
  SL_PRINTF(WLAN_UMAC_MAIN_WISE_ERROR_UNABLE_TO_FORM_AUTOGO, WLAN_UMAC, LOG_ERROR);
}

bool check_ssid_match(uint8_t *buf)
{
  // check for ssid length
  if (buf[0] == 0) {
    return false;
  }
  if (mgmt_if_adapter.operating_mode == WISE_MODE_AP) {
    if (!mgmt_if_adapter.bgscan_ssid_len) {
      return true;
    }
    if ((mgmt_if_adapter.bgscan_ssid_len == buf[0])
        && (os_memcmp(&buf[1], (uint8_t *)mgmt_if_adapter.bgscan_ssid, mgmt_if_adapter.bgscan_ssid_len) == 0)) {
      return true;
    }
  } else {
    if ((mgmt_if_adapter.state >= WISE_STATE_CONNECTED) && (mgmt_if_adapter.multi_probe == 1)) {
      return true;
    }
    if (!mgmt_if_adapter.ssid_len) {
      return true;
    }
    if ((mgmt_if_adapter.ssid_len == buf[0])
        && (os_memcmp(&buf[1], (uint8_t *)mgmt_if_adapter.ssid, mgmt_if_adapter.ssid_len) == 0)) {
      return true;
    }
  }
  return false;
}