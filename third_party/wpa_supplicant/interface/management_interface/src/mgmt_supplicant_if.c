/*******************************************************************************
* @file  mgmt_supplicant_if.c
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
#include "mgmt_supplicant_if.h"
#include "common.h"
#include "wpa_supplicant_i.h"
#include "sl_supplicant.h"
#include "config.h"
#include "notify.h"
#include "scan.h"
#include "bss.h"
#include "ap.h"
#include "wps_supplicant.h"
#include "p2p_supplicant.h"
#include "eloop.h"
#include "crypto/crypto.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "crypto/random.h"
#include "mgmt_if_core.h"
#include "mgmt_if_tx.h"

#define STATION_RSN_IE_PREAUTH_BIT_ENABLE 1

extern void wpas_wps_timeout(void *eloop_ctx, void *timeout_ctx);
extern void rsn_preauth_receive(void *ctx, const uint8 *src_addr, const uint8 *buf, size_t len);
extern int wpas_mbo_validate_non_pref_chan(uint8 oper_class, uint8 chan, uint8 reason);
extern void wpas_mbo_non_pref_chan_changed(struct wpa_supplicant *wpa_s);
extern int wpa_non_pref_chan_cmp(const void *_a, const void *_b);
extern int wpa_non_pref_chan_is_eq(struct wpa_mbo_non_pref_channel *a, struct wpa_mbo_non_pref_channel *b);

void supp_wps_mem_init(uint32 size)
{
  wps_mem_init(size);
}

void *wpa_config_add_network_wrapper(void *ctx)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  return (void *)wpa_config_add_network(wpa_s->conf);
}

void wpas_notify_network_added_wrapper(void *wpa_s, void *ssid)
{
  wpas_notify_network_added((struct wpa_supplicant *)wpa_s, (struct wpa_ssid *)ssid);
}

void wpas_notify_network_removed_wrapper(void *wpa_s, void *ssid)
{
  wpas_notify_network_removed((struct wpa_supplicant *)wpa_s, (struct wpa_ssid *)ssid);
}

void wpa_config_set_network_defaults_wrapper(void *ssid)
{
  wpa_config_set_network_defaults((struct wpa_ssid *)ssid);
}

int wpa_config_set_wrapper(void *ssid, const char *var, const char *value, int line)
{
  return wpa_config_set((struct wpa_ssid *)ssid, var, value, line);
}

#ifndef EXCLUDE_WPS
int wpas_wps_start_pin_wrapper(void *wpa_s, const uint8 *bssid, const char *pin, int p2p_group, uint16 dev_pw_id)
{
  return wpas_wps_start_pin((struct wpa_supplicant *)wpa_s, bssid, pin, p2p_group, dev_pw_id);
}

int wpas_wps_start_pbc_wrapper(void *wpa_s, const uint8 *bssid, int p2p_group, int multi_ap_backhaul_sta)
{
  return wpas_wps_start_pbc((struct wpa_supplicant *)wpa_s, bssid, p2p_group, multi_ap_backhaul_sta);
}
#endif

int wpa_config_remove_network_wrapper(void *ctx, int id)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  return wpa_config_remove_network(wpa_s->conf, id);
}

void *wpa_supplicant_add_iface_wrapper(void *global, void *iface, void *parent)
{
  return wpa_supplicant_add_iface((struct wpa_global *)global,
                                  (struct wpa_interface *)iface,
                                  (struct wpa_supplicant *)parent);
}

void *wpa_supplicant_init_wrapper(void *params)
{
  return wpa_supplicant_init((struct wpa_params *)params);
}

void wpa_supplicant_req_scan_wrapper(void *wpa_s, int sec, int usec)
{
  wpa_supplicant_req_scan((struct wpa_supplicant *)wpa_s, sec, usec);
}

int eloop_cancel_timeout_wrapper(void (*handler)(void *eloop_ctx, void *sock_ctx), void *eloop_data, void *user_data)
{
  return eloop_cancel_timeout(handler, eloop_data, user_data);
}

int eloop_is_timeout_registered_wrapper(void (*handler)(void *eloop_ctx, void *timeout_ctx),
                                        void *eloop_data,
                                        void *user_data)
{
  return eloop_is_timeout_registered(handler, eloop_data, user_data);
}

int eloop_register_timeout_wrapper(unsigned int secs,
                                   unsigned int usecs,
                                   void (*handler)(void *eloop_ctx, void *timeout_ctx),
                                   void *eloop_data,
                                   void *user_data)
{
  return eloop_register_timeout(secs, usecs, handler, eloop_data, user_data);
}

const uint8 *get_ie_wrapper(const uint8 *ies, size_t len, uint8 eid)
{
  return get_ie(ies, len, eid);
}

const uint8 *get_ie_ext_wrapper(const uint8 *ies, size_t len, uint8 ext)
{
  return get_ie_ext(ies, len, ext);
}

void rsn_preauth_receive_wrapper(void *supp_priv, const uint8 *src_addr, const uint8 *buf, size_t len)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)supp_priv;
  rsn_preauth_receive(wpa_s->wpa, src_addr, buf, len);
}

void sme_external_auth_mgmt_rx_wrapper(void *wpa_s, const uint8 *auth_frame, size_t len)
{
  sme_external_auth_mgmt_rx((struct wpa_supplicant *)wpa_s, auth_frame, len);
}

void *wpa_bss_get_wrapper(void *wpa_s, const uint8 *bssid, const uint8 *ssid, size_t ssid_len)
{
  return wpa_bss_get((struct wpa_supplicant *)wpa_s, bssid, ssid, ssid_len);
}

void *wpa_bss_get_bssid_wrapper(void *wpa_s, const uint8 *bssid)
{
  return wpa_bss_get_bssid((struct wpa_supplicant *)wpa_s, bssid);
}

int wpa_ft_start_over_ds_wrapper(void *sm, const uint8 *target_ap, const uint8 *mdie)
{
  return wpa_ft_start_over_ds(sm, target_ap, mdie);
}

int wpa_ft_validate_reassoc_resp_wrapper(void *sm, const uint8 *ies, size_t ies_len, const uint8 *src_addr)
{
  return wpa_ft_validate_reassoc_resp(sm, ies, ies_len, src_addr);
}

void wpa_scan_results_free_wrapper(void *res)
{
  wpa_scan_results_free(res);
}

#ifndef EXCLUDE_AP
int wpa_supplicant_ap_wps_pbc_wrapper(void *wpa_s, const uint8 *bssid, const uint8 *p2p_dev_addr)
{
  return wpa_supplicant_ap_wps_pbc(wpa_s, bssid, p2p_dev_addr);
}
#endif

void wpa_supplicant_deinit_wrapper(void *wpa_s)
{
  wpa_supplicant_deinit(((struct wpa_supplicant *)wpa_s)->global);
}

void wpa_supplicant_event_wrapper(void *ctx, uint8 event, void *data)
{
  wpa_supplicant_event(ctx, event, data);
}

int wpa_supplicant_remove_iface_wrapper(void *wpa_s, int terminate)
{
  return wpa_supplicant_remove_iface(((struct wpa_supplicant *)wpa_s)->global, wpa_s, terminate);
}

void wpas_wps_timeout_wrapper(void *eloop_ctx, void *timeout_ctx)
{
#ifndef EXCLUDE_WPS
  wpas_wps_timeout(eloop_ctx, timeout_ctx);
#endif
}

void rsi_update_roam_params_wrapper(void *wpa_s, uint8 roam_threshold, uint8 roam_hysterisis)
{
  rsi_update_roam_params(wpa_s, roam_threshold, roam_hysterisis);
}

unsigned long os_random_wrapper(void)
{
  return os_random();
}

int os_get_time_wrapper(void *t)
{
  return os_get_time((struct os_time *)t);
}

void generate_pmk_wrapper(uint8 *passphrase, uint8 *ssid, uint16 ssid_len, uint8 *pmk)
{
  generate_pmk(passphrase, ssid, ssid_len, pmk);
}

int sha1_vector_wrapper(size_t num_elem, const uint8 *addr[], const size_t *len, uint8 *mac)
{
  return sha1_vector(num_elem, addr, len, mac);
}

int random_get_bytes_wrapper(void *buf, size_t len)
{
  return random_get_bytes(buf, len);
}

void *os_realloc_array_wrapper(void *ptr, size_t nmemb, size_t size)
{
  return os_realloc_array(ptr, nmemb, size);
}

void wpas_p2p_rx_action_wrapper(void *wpa_s,
                                const uint8 *da,
                                const uint8 *sa,
                                const uint8 *bssid,
                                uint8 category,
                                const uint8 *data,
                                size_t len,
                                int freq)
{
  wpas_p2p_rx_action(wpa_s, da, sa, bssid, category, data, len, freq);
}

int wpas_mbo_validate_non_pref_chan_wrapper(uint8 oper_class, uint8 chan, uint8 reason)
{
  return wpas_mbo_validate_non_pref_chan(oper_class, chan, reason);
}

void wpas_mbo_non_pref_chan_changed_wrapper(void *wpa_s)
{
  wpas_mbo_non_pref_chan_changed(wpa_s);
}

int wpa_non_pref_chan_cmp_wrapper(const void *_a, const void *_b)
{
  return wpa_non_pref_chan_cmp(_a, _b);
}

int wpa_non_pref_chan_is_eq_wrapper(void *a, void *b)
{
  return wpa_non_pref_chan_is_eq(a, b);
}

uint32 wpa_get_bss_max_cnt()
{
  return DEFAULT_BSS_MAX_COUNT;
}

void wpa_notify_remove_all_networks(void *ctx)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  if (wpa_s->conf != NULL) {
    struct wpa_ssid *ssid;
    for (ssid = wpa_s->conf->ssid; ssid; ssid = ssid->next)
      wpas_notify_network_removed_wrapper(wpa_s, ssid);
  }
}

void eloop_terminate_wrapper(void)
{
  eloop_terminate();
}