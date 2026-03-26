/*******************************************************************************
* @file  mgmt_if_utils.h
* @brief This file acts as interface between management interface and
*        common utilities
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
#ifndef MGMT_IF_UTILS_H
#define MGMT_IF_UTILS_H
#include "sl_wlan_data_types.h"
#include "sli_wlan_command_parser_thread.h"

/** RSN Element related Params ***********************/
#define RSN_AKM_OFFSET 12

/** Beacon Elements offset***************************/
#define BEACON_IES_START_OFFSET 36

unsigned int get_akm_suites_info(const uint16 akmcnt, const uint8 *ie_data);
char *wise_get_pmk(void);
uint16 is_pmk_present(void);
uint8 is_wpa3_mode(void);
void wise_store_pmk(char *pmk);
#ifdef RSI_ENABLE_CCX
int isccxoui(const uint8 *frm);
#endif
void supplicant_stub(void);
uint8 is_join_cmd_from_host(void);
void set_join_cmd_from_host_flag(uint8 join_from_host);
uint8 is_japan_region(void);
uint8 is_scan_supplicant_bypass(void);
int16 is_wps_client(void);
uint8 is_dyn_sta_ap_switch_enabled(void);
void set_supplicant_trigger_bg_scan(uint8 supplicant_triggered_bg_scan);
uint8 is_band_24ghz(void);
uint8 is_blacklist_feature_enabled(void);
uint8 is_rsa_4096_supported(void);
void supp_set_roam_indication_and_bssid(void);
void supplicant_set_client(void);
uint8 mac_based_join_mismatch_check(uint8 *bssid);
uint8 prev_bssid_mismatch_check(uint8 *bssid);
uint8 *get_bssid_from_mgmt_intf_adapter(void);
int32 get_rejoin_scan_interval(void);
uint8 is_dual_band(void);
uint16 get_current_freq_band(void);
uint8 is_instant_bg_enabled(void);
uint8 is_bgscan_enabled(void);
uint8 is_p2p_mode(void);
void clear_instant_bg_flag(void);
uint8 is_connected_state(void);
uint8 is_eap_okc_enabled(void);
uint8 get_operating_mode(void);
uint8 mgmtif_is_tcp_bypass_enabled(void);
bool check_ssid_match(uint8_t *);
#endif /* MGMT_IF_UTILS_H */
