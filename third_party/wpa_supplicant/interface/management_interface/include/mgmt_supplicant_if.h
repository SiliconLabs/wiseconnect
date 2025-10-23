/*******************************************************************************
* @file  mgmt_supplicant_if.h
* @brief This contains interface functions to call Supplicant functions.
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
#ifndef MGMT_SUPPLICANT_IF_H
#define MGMT_SUPPLICANT_IF_H

#include "mgmt_platform_if.h"

void supp_wps_mem_init(uint32 size);

void *wpa_config_add_network_wrapper(void *ctx);

void wpas_notify_network_added_wrapper(void *wpa_s, void *ssid);

void wpas_notify_network_removed_wrapper(void *wpa_s, void *ssid);

void wpa_config_set_network_defaults_wrapper(void *ssid);

int wpa_config_set_wrapper(void *ssid, const char *var, const char *value, int line);

int wpas_wps_start_pin_wrapper(void *wpa_s, const uint8 *bssid, const char *pin, int p2p_group, uint16 dev_pw_id);

int wpas_wps_start_pbc_wrapper(void *wpa_s, const uint8 *bssid, int p2p_group, int multi_ap_backhaul_sta);

int wpa_config_remove_network_wrapper(void *ctx, int id);

void *wpa_supplicant_add_iface_wrapper(void *global, void *iface, void *parent);

void *wpa_supplicant_init_wrapper(void *params);

void wpa_supplicant_req_scan_wrapper(void *wpa_s, int sec, int usec);

int eloop_cancel_timeout_wrapper(void (*handler)(void *eloop_ctx, void *sock_ctx), void *eloop_data, void *user_data);

int eloop_is_timeout_registered_wrapper(void (*handler)(void *eloop_ctx, void *timeout_ctx),
                                        void *eloop_data,
                                        void *user_data);

int eloop_register_timeout_wrapper(unsigned int secs,
                                   unsigned int usecs,
                                   void (*handler)(void *eloop_ctx, void *timeout_ctx),
                                   void *eloop_data,
                                   void *user_data);

const uint8 *get_ie_wrapper(const uint8 *ies, size_t len, uint8 eid);

const uint8 *get_ie_ext_wrapper(const uint8 *ies, size_t len, uint8 ext);

void rsn_preauth_receive_wrapper(void *ctx, const uint8 *src_addr, const uint8 *buf, size_t len);

void sme_external_auth_mgmt_rx_wrapper(void *wpa_s, const uint8 *auth_frame, size_t len);

void *wpa_bss_get_wrapper(void *wpa_s, const uint8 *bssid, const uint8 *ssid, size_t ssid_len);

void *wpa_bss_get_bssid_wrapper(void *wpa_s, const uint8 *bssid);

int wpa_ft_start_over_ds_wrapper(void *sm, const uint8 *target_ap, const uint8 *mdie);

int wpa_ft_validate_reassoc_resp_wrapper(void *sm, const uint8 *ies, size_t ies_len, const uint8 *src_addr);

void wpa_scan_results_free_wrapper(void *res);

void wpa_supplicant_deinit_wrapper(void *global);

void wpa_supplicant_event_wrapper(void *ctx, uint8 event, void *data);

int wpa_supplicant_remove_iface_wrapper(void *wpa_s, int terminate);

void wpas_wps_timeout_wrapper(void *eloop_ctx, void *timeout_ctx);

int wpa_supplicant_ap_wps_pbc_wrapper(void *wpa_s, const uint8 *bssid, const uint8 *p2p_dev_addr);

#ifdef SUPPLICANT_NON_ROM
void rsi_update_roam_params_wrapper(void *wpa_s, uint8 roam_threshold, uint8 roam_hysterisis);
#endif /* SUPPLICANT_NON_ROM */

unsigned long os_random_wrapper(void);

int os_get_time_wrapper(void *t);

void generate_pmk_wrapper(uint8 *passphrase, uint8 *ssid, uint16 ssid_len, uint8 *pmk);

int sha1_vector_wrapper(size_t num_elem, const uint8 *addr[], const size_t *len, uint8 *mac);

int random_get_bytes_wrapper(void *buf, size_t len);

void *os_realloc_array_wrapper(void *ptr, size_t nmemb, size_t size);

void wpas_p2p_rx_action_wrapper(void *wpa_s,
                                const uint8 *da,
                                const uint8 *sa,
                                const uint8 *bssid,
                                uint8 category,
                                const uint8 *data,
                                size_t len,
                                int freq);

int wpas_mbo_validate_non_pref_chan_wrapper(uint8 oper_class, uint8 chan, uint8 reason);

void wpas_mbo_non_pref_chan_changed_wrapper(void *wpa_s);

int wpa_non_pref_chan_cmp_wrapper(const void *_a, const void *_b);

int wpa_non_pref_chan_is_eq_wrapper(void *a, void *b);

uint32 wpa_get_bss_max_cnt();

void wpa_notify_remove_all_networks(void *ctx);

void eloop_terminate_wrapper(void);
#endif /* MGMT_SUPPLICANT_IF_H */
