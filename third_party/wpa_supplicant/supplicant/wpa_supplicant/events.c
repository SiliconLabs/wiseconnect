/*
 * WPA Supplicant - Driver event processing
 * Copyright (c) 2003-2019, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "eapol_supp/eapol_supp_sm.h"
#include "rsn_supp/wpa.h"
#include "eloop.h"
#include "config.h"
#include "l2_packet/l2_packet.h"
#include "wpa_supplicant_i.h"
#include "driver_i.h"
#include "pcsc_funcs.h"
#ifdef RSI_ENABLE_CCX
#include "rsn_supp/wpa_i.h"
#endif
#include "rsn_supp/preauth.h"
#include "rsn_supp/pmksa_cache.h"
#include "common/wpa_ctrl.h"
#include "eap_peer/eap.h"
#include "ap/hostapd.h"
#include "p2p/p2p.h"
#ifdef CONFIG_FST
#include "fst/fst.h"
#endif
#include "wnm_sta.h"
#include "notify.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#ifdef CONFIG_GAS_SERVER
#include "common/gas_server.h"
#include "gas_query.h"
#endif
#ifdef CONFIG_DPP
#include "common/dpp.h"
#include "dpp_supplicant.h"
#endif
#include "crypto/random.h"
#include "blacklist.h"
#include "wpas_glue.h"
#include "wps_supplicant.h"
#include "ibss_rsn.h"
#include "sme.h"
#include "p2p_supplicant.h"
#include "bgscan.h"
#include "autoscan.h"
#include "ap.h"
#include "bss.h"
#include "scan.h"
#include "offchannel.h"
#ifdef CONFIG_INTERWORKING
#include "interworking.h"
#endif
#ifdef CONFIG_MESH
#include "mesh.h"
#include "mesh_mpm.h"
#endif
#include "wmm_ac.h"
#ifdef ENABLE_UMAC_ROM_PTR
#include  "supplicant_rom_image.h"
#endif /* ENABLE_UMAC_ROM_PTR */
#include "ap/sta_info.h"

extern void sme_clear_on_disassoc(struct wpa_supplicant *wpa_s);

#define MAX_OWE_TRANSITION_BSS_SELECT_COUNT 5
#ifndef MAX_CHANNELS_2_4GHZ
#define MAX_CHANNELS_2_4GHZ 14
#endif
#ifndef MAX_CHANNELS_5GHZ
#ifdef DISABLE_CH144
#define MAX_CHANNELS_5GHZ 	24
#else
#define MAX_CHANNELS_5GHZ 	25
#endif
#endif

#ifdef RSI_ENABLE_CCX
extern int wpa_install_cckm_newkeys(struct wpa_sm *sm, struct wpa_gtk_data *gd);
#endif

#ifndef CONFIG_NO_SCAN_PROCESSING
static int wpas_select_network_from_last_scan(struct wpa_supplicant *wpa_s,
					      int new_scan, int own_request);
#endif /* CONFIG_NO_SCAN_PROCESSING */

void handle_auth_sae(struct hostapd_data *hapd, struct sta_info *sta,
		const struct ieee80211_mgmt *mgmt, size_t len,
		u16 auth_transaction, u16 status_code);
int wpas_temp_disabled(struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid)
{
	struct os_reltime now;

	if (ssid == NULL || ssid->disabled_until.sec == 0)
		return 0;

	os_get_reltime(&now);
	if (ssid->disabled_until.sec > now.sec)
		return ssid->disabled_until.sec - now.sec;

	wpas_clear_temp_disabled(wpa_s, ssid, 0);

	return 0;
}


#ifndef CONFIG_NO_SCAN_PROCESSING
/**
 * wpas_reenabled_network_time - Time until first network is re-enabled
 * @wpa_s: Pointer to wpa_supplicant data
 * Returns: If all enabled networks are temporarily disabled, returns the time
 *	(in sec) until the first network is re-enabled. Otherwise returns 0.
 *
 * This function is used in case all enabled networks are temporarily disabled,
 * in which case it returns the time (in sec) that the first network will be
 * re-enabled. The function assumes that at least one network is enabled.
 */
#if UNUSED_FEAT_IN_SUPP_29
static int wpas_reenabled_network_time(struct wpa_supplicant *wpa_s)
{
	struct wpa_ssid *ssid;
	int disabled_for, res = 0;

#ifdef CONFIG_INTERWORKING
	if (wpa_s->conf->auto_interworking && wpa_s->conf->interworking &&
	    wpa_s->conf->cred)
		return 0;
#endif /* CONFIG_INTERWORKING */

	for (ssid = wpa_s->conf->ssid; ssid; ssid = ssid->next) {
		if (ssid->disabled)
			continue;

		disabled_for = wpas_temp_disabled(wpa_s, ssid);
		if (!disabled_for)
			return 0;

		if (!res || disabled_for < res)
			res = disabled_for;
	}

	return res;
}
#endif
#endif /* CONFIG_NO_SCAN_PROCESSING */


#if UNUSED_FEAT_IN_SUPP_29
void wpas_network_reenabled(void *eloop_ctx, void *timeout_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;

	if (wpa_s->disconnected || wpa_s->wpa_state != WPA_SCANNING)
		return;

	wpa_dbg(wpa_s, MSG_DEBUG,
		"Try to associate due to network getting re-enabled");
	if (wpa_supplicant_fast_associate(wpa_s) != 1) {
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_supplicant_req_scan(wpa_s, 0, 0);
	}
}
#endif

static struct wpa_bss * wpa_supplicant_get_new_bss(
	struct wpa_supplicant *wpa_s, const u8 *bssid)
{
	struct wpa_bss *bss = NULL;
	struct wpa_ssid *ssid = wpa_s->current_ssid;

	if (ssid->ssid_len > 0)
		bss = wpa_bss_get(wpa_s, bssid, ssid->ssid, ssid->ssid_len);
	if (!bss)
		bss = wpa_bss_get_bssid(wpa_s, bssid);

	return bss;
}


static void wpa_supplicant_update_current_bss(struct wpa_supplicant *wpa_s)
{
	struct wpa_bss *bss = wpa_supplicant_get_new_bss(wpa_s, wpa_s->bssid);

	if (!bss) {
		wpa_supplicant_update_scan_results(wpa_s);

		/* Get the BSS from the new scan results */
		bss = wpa_supplicant_get_new_bss(wpa_s, wpa_s->bssid);
	}

	if (bss)
		wpa_s->current_bss = bss;
}


static int wpa_supplicant_select_config(struct wpa_supplicant *wpa_s)
{
	struct wpa_ssid *ssid, *old_ssid;
	u8 drv_ssid[SSID_MAX_LEN];
	size_t drv_ssid_len;
	int res;

	if (wpa_s->conf->ap_scan == 1 && wpa_s->current_ssid) {
		wpa_supplicant_update_current_bss(wpa_s);

		if (wpa_s->current_ssid->ssid_len == 0)
			return 0; /* current profile still in use */
		res = wpa_drv_get_ssid(wpa_s, drv_ssid);
		if (res < 0) {
			wpa_msg(wpa_s, MSG_INFO,
				"Failed to read SSID from driver");
			return 0; /* try to use current profile */
		}
		drv_ssid_len = res;

		if (drv_ssid_len == wpa_s->current_ssid->ssid_len &&
		    os_memcmp(drv_ssid, wpa_s->current_ssid->ssid,
			      drv_ssid_len) == 0)
			return 0; /* current profile still in use */

		wpa_msg(wpa_s, MSG_DEBUG,
			"Driver-initiated BSS selection changed the SSID to %s",
			wpa_ssid_txt(drv_ssid, drv_ssid_len));
		/* continue selecting a new network profile */
	}

	wpa_dbg(wpa_s, MSG_DEBUG, "Select network based on association "
		"information");
	ssid = wpa_supplicant_get_ssid(wpa_s);
	if (ssid == NULL) {
		wpa_msg(wpa_s, MSG_INFO,
			"No network configuration found for the current AP");
		return -1;
	}

	if (wpas_network_disabled(wpa_s, ssid)) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Selected network is disabled");
		return -1;
	}

	if (disallowed_bssid(wpa_s, wpa_s->bssid) ||
	    disallowed_ssid(wpa_s, ssid->ssid, ssid->ssid_len)) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Selected BSS is disallowed");
		return -1;
	}

	res = wpas_temp_disabled(wpa_s, ssid);
	if (res > 0) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Selected network is temporarily "
			"disabled for %d second(s)", res);
		return -1;
	}

	wpa_dbg(wpa_s, MSG_DEBUG, "Network configuration found for the "
		"current AP");
	if (wpa_key_mgmt_wpa_any(ssid->key_mgmt)) {
		u8 wpa_ie[80];
		size_t wpa_ie_len = sizeof(wpa_ie);
		if (wpa_supplicant_set_suites(wpa_s, NULL, ssid,
					      wpa_ie, &wpa_ie_len) < 0)
			wpa_dbg(wpa_s, MSG_DEBUG, "Could not set WPA suites");
	} else {
		wpa_supplicant_set_non_wpa_policy(wpa_s, ssid);
	}

	if (wpa_s->current_ssid && wpa_s->current_ssid != ssid)
		eapol_sm_invalidate_cached_session(wpa_s->eapol);
	old_ssid = wpa_s->current_ssid;
	wpa_s->current_ssid = ssid;

	wpa_supplicant_update_current_bss(wpa_s);

	wpa_supplicant_rsn_supp_set_config(wpa_s, wpa_s->current_ssid);
	wpa_supplicant_initiate_eapol(wpa_s);
	if (old_ssid != wpa_s->current_ssid)
		wpas_notify_network_changed(wpa_s);

	return 0;
}

#ifdef RSI_ENABLE_CCX
    STATIC void
wpa_supplicant_event_cckm_reassoc_resp(struct wpa_supplicant *wpa_s,
		union wpa_event_data *data)
{
	//struct wpa_gtk_data gtk = {0}; 
	memset(&wpa_s->gtk, 0, sizeof(struct wpa_gtk_data));
	if (data == NULL)
		return; 
	/* Validate received MIC */
	wpa_printf(MSG_INFO, "Received validate MIC request");
	wpa_hexdump(MSG_ERROR, "CCKM Response IE:", (u8 *)&data->cckm_response, 80);
	wpa_hexdump(MSG_ERROR, "Encrypted GTK is:", data->cckm_response.gtk, *data->cckm_response.gkey_len);
	wpa_printf(MSG_DEBUG, "Key Id : %d\n",data->cckm_response.keyid);
	wpa_printf(MSG_DEBUG, "Re Key Number: %d\n",data->cckm_response.rekey_number);
	if (wpa_supplicant_verify_cckm_key_mic(wpa_s->wpa, 
				data->cckm_response.rekey_number,
				data->cckm_response.keyid,
				data->cckm_response.keyid_mcast,
				data->cckm_response.rsc,
				data->cckm_response.gkey_len,
				data->cckm_response.mic,
				data->cckm_response.gtk,
				(u8 *)&wpa_s->gtk
				) == 0) 
	{
		/* Install the keys now PTK followed by GTK */
		wpa_printf(MSG_INFO, "MIC validated, successfully derived keys");
		//wpa_install_cckm_newkeys(wpa_s->wpa, &gtk);
		wpa_s->wpa->wpa_cckm_completed = 1;
	}
	else
	{
		/* MIC check failed. So, ask station to deauthenticate 
		 * Check the reason code here
		 */	
		wpa_sm_deauthenticate(wpa_s->wpa, WLAN_REASON_UNSPECIFIED);
		wpa_s->wpa->wpa_cckm_completed = 0;
	}
	return;
}
#endif

#ifdef SUPPLICANT_NON_ROM
void wpa_supplicant_stop_countermeasures(void *eloop_ctx, void *sock_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;

	if (wpa_s->countermeasures) {
		wpa_s->countermeasures = 0;
		wpa_drv_set_countermeasures(wpa_s, 0);
		wpa_msg(wpa_s, MSG_INFO, "WPA: TKIP countermeasures stopped");

		/*
		 * It is possible that the device is sched scanning, which means
		 * that a connection attempt will be done only when we receive
		 * scan results. However, in this case, it would be preferable
		 * to scan and connect immediately, so cancel the sched_scan and
		 * issue a regular scan flow.
		 */
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_supplicant_req_scan(wpa_s, 0, 0);
	}
}


void wpa_supplicant_mark_disassoc(struct wpa_supplicant *wpa_s)
{
	int bssid_changed;

#ifdef CONFIG_WNM
	wnm_bss_keep_alive_deinit(wpa_s);
#endif

#ifdef CONFIG_IBSS_RSN
	ibss_rsn_deinit(wpa_s->ibss_rsn);
	wpa_s->ibss_rsn = NULL;
#endif /* CONFIG_IBSS_RSN */

#ifdef CONFIG_AP
	wpa_supplicant_ap_deinit(wpa_s);
#endif /* CONFIG_AP */

#ifdef CONFIG_HS20
	/* Clear possibly configured frame filters */
	wpa_drv_configure_frame_filters(wpa_s, 0);
#endif /* CONFIG_HS20 */

	if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED)
		return;

	if (os_reltime_initialized(&wpa_s->session_start)) {
		os_reltime_age(&wpa_s->session_start, &wpa_s->session_length);
		wpa_s->session_start.sec = 0;
		wpa_s->session_start.usec = 0;
		wpas_notify_session_length(wpa_s);
	}

	wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
	bssid_changed = !is_zero_ether_addr(wpa_s->bssid);
	os_memset(wpa_s->bssid, 0, ETH_ALEN);
	os_memset(wpa_s->pending_bssid, 0, ETH_ALEN);
	sme_clear_on_disassoc(wpa_s);
	wpa_s->current_bss = NULL;
	wpa_s->assoc_freq = 0;

	if (bssid_changed)
		wpas_notify_bssid_changed(wpa_s);

	eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
	eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
	if (wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_OWE ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_DPP)
		eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
	wpa_s->ap_ies_from_associnfo = 0;
	wpa_s->current_ssid = NULL;
	eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
	wpa_s->key_mgmt = 0;
#ifdef CHIP_9117
	wpas_rrm_reset(wpa_s);
#endif
	wpa_s->wnmsleep_used = 0;
	wnm_clear_coloc_intf_reporting(wpa_s);
    wpa_s->disable_mbo_oce = 0;

#ifdef CONFIG_TESTING_OPTIONS
	wpa_s->last_tk_alg = WPA_ALG_NONE;
	os_memset(wpa_s->last_tk, 0, sizeof(wpa_s->last_tk));
#endif /* CONFIG_TESTING_OPTIONS */
	wpa_s->ieee80211ac = 0;

	if (wpa_s->enabled_4addr_mode && wpa_drv_set_4addr_mode(wpa_s, 0) == 0)
		wpa_s->enabled_4addr_mode = 0;
}


STATIC void wpa_find_assoc_pmkid(struct wpa_supplicant *wpa_s)
{
	struct wpa_ie_data ie;
	int pmksa_set = -1;
	size_t i;
	struct rsn_pmksa_cache_entry *cur_pmksa;

	/* Start with assumption of no PMKSA cache entry match for cases other
	 * than SAE. In particular, this is needed to generate the PMKSA cache
	 * entries for Suite B cases with driver-based roaming indication. */
	cur_pmksa = pmksa_cache_get_current(wpa_s->wpa);
	if (cur_pmksa && !wpa_key_mgmt_sae(cur_pmksa->akmp))
		pmksa_cache_clear_current(wpa_s->wpa);

	if (wpa_sm_parse_own_wpa_ie(wpa_s->wpa, &ie) < 0 ||
	    ie.pmkid == NULL)
		return;

	for (i = 0; i < ie.num_pmkid; i++) {
		pmksa_set = pmksa_cache_set_current(wpa_s->wpa,
						    ie.pmkid + i * PMKID_LEN,
						    NULL, NULL, 0, NULL, 0);
		if (pmksa_set == 0) {
			eapol_sm_notify_pmkid_attempt(wpa_s->eapol);
			break;
		}
	}

	wpa_dbg(wpa_s, MSG_DEBUG, "RSN: PMKID from assoc IE %sfound from "
		"PMKSA cache", pmksa_set == 0 ? "" : "not ");
}

#ifndef CONFIG_OPTIMIZE_CODE
static void wpa_supplicant_event_pmkid_candidate(struct wpa_supplicant *wpa_s,
						 union wpa_event_data *data)
{
	if (data == NULL) {
		wpa_dbg(wpa_s, MSG_DEBUG, "RSN: No data in PMKID candidate "
			"event");
		return;
	}
	wpa_dbg(wpa_s, MSG_DEBUG, "RSN: PMKID candidate event - bssid=" MACSTR
		" index=%d preauth=%d",
		MAC2STR(data->pmkid_candidate.bssid),
		data->pmkid_candidate.index,
		data->pmkid_candidate.preauth);

	pmksa_candidate_add(wpa_s->wpa, data->pmkid_candidate.bssid,
			    data->pmkid_candidate.index,
			    data->pmkid_candidate.preauth);
}
#endif /*CONFIG_OPTIMIZE_CODE*/
#endif /*SUPPLICANT_NON_ROM*/
static int wpa_supplicant_dynamic_keys(struct wpa_supplicant *wpa_s);
static int wpa_supplicant_event_scan_results(struct wpa_supplicant *wpa_s,
					      union wpa_event_data *data);

STATIC int wpa_supplicant_dynamic_keys(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE)
		return 0;

#ifdef IEEE8021X_EAPOL
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA &&
	    wpa_s->current_ssid &&
	    !(wpa_s->current_ssid->eapol_flags &
	      (EAPOL_FLAG_REQUIRE_KEY_UNICAST |
	       EAPOL_FLAG_REQUIRE_KEY_BROADCAST))) {
		/* IEEE 802.1X, but not using dynamic WEP keys (i.e., either
		 * plaintext or static WEP keys). */
		return 0;
	}
#endif /* IEEE8021X_EAPOL */

	return 1;
}


/**
 * wpa_supplicant_scard_init - Initialize SIM/USIM access with PC/SC
 * @wpa_s: pointer to wpa_supplicant data
 * @ssid: Configuration data for the network
 * Returns: 0 on success, -1 on failure
 *
 * This function is called when starting authentication with a network that is
 * configured to use PC/SC for SIM/USIM access (EAP-SIM or EAP-AKA).
 */
int wpa_supplicant_scard_init(struct wpa_supplicant *wpa_s,
			      struct wpa_ssid *ssid)
{
#ifdef IEEE8021X_EAPOL
#ifdef PCSC_FUNCS
	int aka = 0, sim = 0;

	if ((ssid != NULL && ssid->eap.pcsc == NULL) ||
	    wpa_s->scard != NULL || wpa_s->conf->external_sim)
		return 0;

	if (ssid == NULL || ssid->eap.eap_methods == NULL) {
		sim = 1;
		aka = 1;
	} else {
		struct eap_method_type *eap = ssid->eap.eap_methods;
		while (eap->vendor != EAP_VENDOR_IETF ||
		       eap->method != EAP_TYPE_NONE) {
			if (eap->vendor == EAP_VENDOR_IETF) {
				if (eap->method == EAP_TYPE_SIM)
					sim = 1;
				else if (eap->method == EAP_TYPE_AKA ||
					 eap->method == EAP_TYPE_AKA_PRIME)
					aka = 1;
			}
			eap++;
		}
	}

	if (eap_peer_get_eap_method(EAP_VENDOR_IETF, EAP_TYPE_SIM) == NULL)
		sim = 0;
	if (eap_peer_get_eap_method(EAP_VENDOR_IETF, EAP_TYPE_AKA) == NULL &&
	    eap_peer_get_eap_method(EAP_VENDOR_IETF, EAP_TYPE_AKA_PRIME) ==
	    NULL)
		aka = 0;

	if (!sim && !aka) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Selected network is configured to "
			"use SIM, but neither EAP-SIM nor EAP-AKA are "
			"enabled");
		return 0;
	}

	wpa_dbg(wpa_s, MSG_DEBUG, "Selected network is configured to use SIM "
		"(sim=%d aka=%d) - initialize PCSC", sim, aka);

	wpa_s->scard = scard_init(wpa_s->conf->pcsc_reader);
	if (wpa_s->scard == NULL) {
		wpa_msg(wpa_s, MSG_WARNING, "Failed to initialize SIM "
			"(pcsc-lite)");
		return -1;
	}
	wpa_sm_set_scard_ctx(wpa_s->wpa, wpa_s->scard);
	eapol_sm_register_scard_ctx(wpa_s->eapol, wpa_s->scard);
#endif /* PCSC_FUNCS */
#endif /* IEEE8021X_EAPOL */

	return 0;
}


#ifndef CONFIG_NO_SCAN_PROCESSING

static int has_wep_key(struct wpa_ssid *ssid)
{
	int i;

	for (i = 0; i < NUM_WEP_KEYS; i++) {
		if (ssid->wep_key_len[i])
			return 1;
	}

	return 0;
}


static int wpa_supplicant_match_privacy(struct wpa_bss *bss,
					struct wpa_ssid *ssid)
{
	int privacy = 0;

	if (ssid->mixed_cell)
		return 1;

#ifdef CONFIG_WPS
	if (ssid->key_mgmt & WPA_KEY_MGMT_WPS)
		return 1;
#endif /* CONFIG_WPS */

#ifdef CONFIG_OWE
	if ((ssid->key_mgmt & WPA_KEY_MGMT_OWE) && !ssid->owe_only)
		return 1;
#endif /* CONFIG_OWE */

	if (has_wep_key(ssid))
		privacy = 1;

#ifdef IEEE8021X_EAPOL
	if ((ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA) &&
	    ssid->eapol_flags & (EAPOL_FLAG_REQUIRE_KEY_UNICAST |
				 EAPOL_FLAG_REQUIRE_KEY_BROADCAST))
		privacy = 1;
#endif /* IEEE8021X_EAPOL */

	if (wpa_key_mgmt_wpa(ssid->key_mgmt))
		privacy = 1;

	if (ssid->key_mgmt & WPA_KEY_MGMT_OSEN)
		privacy = 1;

	if (bss->caps & IEEE80211_CAP_PRIVACY)
		return privacy;
	return !privacy;
}


static int wpa_supplicant_ssid_bss_match(struct wpa_supplicant *wpa_s,
					 struct wpa_ssid *ssid,
					 struct wpa_bss *bss, int debug_print)
{
	struct wpa_ie_data ie;
	int proto_match = 0;
	const u8 *rsn_ie, *wpa_ie;
	int ret;
	int wep_ok;

	ret = wpas_wps_ssid_bss_match(wpa_s, ssid, bss);
	if (ret >= 0)
		return ret;

	/* Allow TSN if local configuration accepts WEP use without WPA/WPA2 */
	wep_ok = !wpa_key_mgmt_wpa(ssid->key_mgmt) &&
		(((ssid->key_mgmt & WPA_KEY_MGMT_NONE) &&
		  ssid->wep_key_len[ssid->wep_tx_keyidx] > 0) ||
		 (ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA));

	rsn_ie = wpa_bss_get_ie(bss, WLAN_EID_RSN);
	while ((ssid->proto & (WPA_PROTO_RSN | WPA_PROTO_OSEN)) && rsn_ie) {
		proto_match++;

		if (wpa_parse_wpa_ie(rsn_ie, 2 + rsn_ie[1], &ie)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - parse failed");
			break;
		}
		if (!ie.has_pairwise)
			ie.pairwise_cipher = wpa_default_rsn_cipher(bss->freq);
		if (!ie.has_group)
			ie.group_cipher = wpa_default_rsn_cipher(bss->freq);

		if (wep_ok &&
		    (ie.group_cipher & (WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)))
		{
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   selected based on TSN in RSN IE");
			return 1;
		}

		if (!(ie.proto & ssid->proto) &&
		    !(ssid->proto & WPA_PROTO_OSEN)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - proto mismatch");
			break;
		}

		if (!(ie.pairwise_cipher & ssid->pairwise_cipher)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - PTK cipher mismatch");
			break;
		}

		if (!(ie.group_cipher & ssid->group_cipher)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - GTK cipher mismatch");
			break;
		}

		if (ssid->group_mgmt_cipher &&
		    !(ie.mgmt_group_cipher & ssid->group_mgmt_cipher)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - group mgmt cipher mismatch");
			break;
		}

		if (!(ie.key_mgmt & ssid->key_mgmt)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - key mgmt mismatch");
			break;
		}

#ifdef CONFIG_IEEE80211W
		if (!(ie.capabilities & WPA_CAPABILITY_MFPC) &&
		    wpas_get_ssid_pmf(wpa_s, ssid) ==
		    MGMT_FRAME_PROTECTION_REQUIRED) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - no mgmt frame protection");
			break;
		}
#endif /* CONFIG_IEEE80211W */
		if ((ie.capabilities & WPA_CAPABILITY_MFPR) &&
		    wpas_get_ssid_pmf(wpa_s, ssid) ==
		    NO_MGMT_FRAME_PROTECTION) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip RSN IE - no mgmt frame protection enabled but AP requires it");
			break;
		}

		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   selected based on RSN IE");
		return 1;
	}

#ifdef CONFIG_IEEE80211W
	if (wpas_get_ssid_pmf(wpa_s, ssid) == MGMT_FRAME_PROTECTION_REQUIRED &&
	    (!(ssid->key_mgmt & WPA_KEY_MGMT_OWE) || ssid->owe_only)) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   skip - MFP Required but network not MFP Capable");
		return 0;
	}
#endif /* CONFIG_IEEE80211W */

	wpa_ie = wpa_bss_get_vendor_ie(bss, WPA_IE_VENDOR_TYPE);
	while ((ssid->proto & WPA_PROTO_WPA) && wpa_ie) {
		proto_match++;

		if (wpa_parse_wpa_ie(wpa_ie, 2 + wpa_ie[1], &ie)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip WPA IE - parse failed");
			break;
		}

		if (wep_ok &&
		    (ie.group_cipher & (WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)))
		{
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   selected based on TSN in WPA IE");
			return 1;
		}

		if (!(ie.proto & ssid->proto)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip WPA IE - proto mismatch");
			break;
		}

		if (!(ie.pairwise_cipher & ssid->pairwise_cipher)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip WPA IE - PTK cipher mismatch");
			break;
		}

		if (!(ie.group_cipher & ssid->group_cipher)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip WPA IE - GTK cipher mismatch");
			break;
		}

		if (!(ie.key_mgmt & ssid->key_mgmt)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip WPA IE - key mgmt mismatch");
			break;
		}

		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   selected based on WPA IE");
		return 1;
	}

	if ((ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA) && !wpa_ie &&
	    !rsn_ie) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   allow for non-WPA IEEE 802.1X");
		return 1;
	}

#ifdef CONFIG_OWE
	if ((ssid->key_mgmt & WPA_KEY_MGMT_OWE) && !ssid->owe_only &&
	    !wpa_ie && !rsn_ie) {
		if (wpa_s->owe_transition_select &&
		    wpa_bss_get_vendor_ie(bss, OWE_IE_VENDOR_TYPE) &&
		    ssid->owe_transition_bss_select_count + 1 <=
		    MAX_OWE_TRANSITION_BSS_SELECT_COUNT) {
			ssid->owe_transition_bss_select_count++;
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip OWE transition BSS (selection count %d does not exceed %d)",
					ssid->owe_transition_bss_select_count,
					MAX_OWE_TRANSITION_BSS_SELECT_COUNT);
			wpa_s->owe_transition_search = 1;
			return 0;
		}
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   allow in OWE transition mode");
		return 1;
	}
#endif /* CONFIG_OWE */

	if ((ssid->proto & (WPA_PROTO_WPA | WPA_PROTO_RSN)) &&
	    wpa_key_mgmt_wpa(ssid->key_mgmt) && proto_match == 0) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG,
				"   skip - no WPA/RSN proto match");
		return 0;
	}

	if ((ssid->key_mgmt & WPA_KEY_MGMT_OSEN) &&
	    wpa_bss_get_vendor_ie(bss, OSEN_IE_VENDOR_TYPE)) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG, "   allow in OSEN");
		return 1;
	}

	if (!wpa_key_mgmt_wpa(ssid->key_mgmt)) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG, "   allow in non-WPA/WPA2");
		return 1;
	}

	if (debug_print)
		wpa_dbg(wpa_s, MSG_DEBUG,
			"   reject due to mismatch with WPA/WPA2");

	return 0;
}


static int freq_allowed(int *freqs, int freq)
{
	int i;

	if (freqs == NULL)
		return 1;

	for (i = 0; freqs[i]; i++)
		if (freqs[i] == freq)
			return 1;
	return 0;
}

/*ROAMING Changes*/
int wpa_supplicant_translate_channel(int freq)
{
	/* Translate freq to channel . */
	int base_freq = 2412;
	int channel = 0;
	if (freq < 2484) {
		channel = ((freq - base_freq) / 5 ) + 1;
	}
	else if (freq == 2484) {
		channel = 14;
	}

	return channel;
}



static int rate_match(struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid,
		      struct wpa_bss *bss, int debug_print)
{
	const struct hostapd_hw_modes *mode = NULL, *modes;
	const u8 scan_ie[2] = { WLAN_EID_SUPP_RATES, WLAN_EID_EXT_SUPP_RATES };
	const u8 *rate_ie;
	int i, j, k;

	if (bss->freq == 0)
		return 1; /* Cannot do matching without knowing band */

	modes = wpa_s->hw.modes;
	if (modes == NULL) {
		/*
		 * The driver does not provide any additional information
		 * about the utilized hardware, so allow the connection attempt
		 * to continue.
		 */
		return 1;
	}

	for (i = 0; i < wpa_s->hw.num_modes; i++) {
		for (j = 0; j < modes[i].num_channels; j++) {
			int freq = modes[i].channels[j].freq;
			if (freq == bss->freq) {
				if (mode &&
				    mode->mode == HOSTAPD_MODE_IEEE80211G)
					break; /* do not allow 802.11b replace
						* 802.11g */
				mode = &modes[i];
				break;
			}
		}
	}

	if (mode == NULL)
		return 0;

	for (i = 0; i < (int) sizeof(scan_ie); i++) {
		rate_ie = wpa_bss_get_ie(bss, scan_ie[i]);
		if (rate_ie == NULL)
			continue;

		for (j = 2; j < rate_ie[1] + 2; j++) {
			int flagged = !!(rate_ie[j] & 0x80);
			int r = (rate_ie[j] & 0x7f) * 5;

			/*
			 * IEEE Std 802.11n-2009 7.3.2.2:
			 * The new BSS Membership selector value is encoded
			 * like a legacy basic rate, but it is not a rate and
			 * only indicates if the BSS members are required to
			 * support the mandatory features of Clause 20 [HT PHY]
			 * in order to join the BSS.
			 */
			if (flagged && ((rate_ie[j] & 0x7f) ==
					BSS_MEMBERSHIP_SELECTOR_HT_PHY)) {
				if (!ht_supported(mode)) {
					if (debug_print)
						wpa_dbg(wpa_s, MSG_DEBUG,
							"   hardware does not support HT PHY");
					return 0;
				}
				continue;
			}

			/* There's also a VHT selector for 802.11ac */
			if (flagged && ((rate_ie[j] & 0x7f) ==
					BSS_MEMBERSHIP_SELECTOR_VHT_PHY)) {
				if (!vht_supported(mode)) {
					if (debug_print)
						wpa_dbg(wpa_s, MSG_DEBUG,
							"   hardware does not support VHT PHY");
					return 0;
				}
				continue;
			}

#ifdef CONFIG_SAE
			if (flagged && ((rate_ie[j] & 0x7f) ==
					BSS_MEMBERSHIP_SELECTOR_SAE_H2E_ONLY)) {
				if (wpa_s->conf->sae_pwe == 0 &&
				    !ssid->sae_password_id &&
				    wpa_key_mgmt_sae(ssid->key_mgmt)) {
					if (debug_print)
						wpa_dbg(wpa_s, MSG_DEBUG,
							"   SAE H2E disabled");
					return 0;
				}
				continue;
			}
#endif /* CONFIG_SAE */

			if (!flagged)
				continue;

			/* check for legacy basic rates */
			for (k = 0; k < mode->num_rates; k++) {
				if (mode->rates[k] == r)
					break;
			}
			if (k == mode->num_rates) {
				/*
				 * IEEE Std 802.11-2007 7.3.2.2 demands that in
				 * order to join a BSS all required rates
				 * have to be supported by the hardware.
				 */
				if (debug_print)
					wpa_dbg(wpa_s, MSG_DEBUG,
						"   hardware does not support required rate %d.%d Mbps (freq=%d mode==%d num_rates=%d)",
						r / 10, r % 10,
						bss->freq, mode->mode, mode->num_rates);
				return 0;
			}
		}
	}

	return 1;
}


/*
 * Test whether BSS is in an ESS.
 * This is done differently in DMG (60 GHz) and non-DMG bands
 */
static int bss_is_ess(struct wpa_bss *bss)
{
	if (bss_is_dmg(bss)) {
		return (bss->caps & IEEE80211_CAP_DMG_MASK) ==
			IEEE80211_CAP_DMG_AP;
	}

	return ((bss->caps & (IEEE80211_CAP_ESS | IEEE80211_CAP_IBSS)) ==
		IEEE80211_CAP_ESS);
}


static int match_mac_mask(const u8 *addr_a, const u8 *addr_b, const u8 *mask)
{
	size_t i;

	for (i = 0; i < ETH_ALEN; i++) {
		if ((addr_a[i] & mask[i]) != (addr_b[i] & mask[i]))
			return 0;
	}
	return 1;
}


static int addr_in_list(const u8 *addr, const u8 *list, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++) {
		const u8 *a = list + i * ETH_ALEN * 2;
		const u8 *m = a + ETH_ALEN;

		if (match_mac_mask(a, addr, m))
			return 1;
	}
	return 0;
}


#ifdef CONFIG_OWE
static void owe_trans_ssid(struct wpa_supplicant *wpa_s, struct wpa_bss *bss,
			   const u8 **ret_ssid, size_t *ret_ssid_len)
{
	const u8 *owe, *pos, *end, *bssid;
	u8 ssid_len;
	struct wpa_bss *open_bss;

	owe = wpa_bss_get_vendor_ie(bss, OWE_IE_VENDOR_TYPE);
	if (!owe || !wpa_bss_get_ie(bss, WLAN_EID_RSN))
		return;

	pos = owe + 6;
	end = owe + 2 + owe[1];

	if (end - pos < ETH_ALEN + 1)
		return;
	bssid = pos;
	pos += ETH_ALEN;
	ssid_len = *pos++;
	if (end - pos < ssid_len || ssid_len > SSID_MAX_LEN)
		return;

	/* Match the profile SSID against the OWE transition mode SSID on the
	 * open network. */
	wpa_dbg(wpa_s, MSG_DEBUG, "OWE: transition mode BSSID: " MACSTR
		" SSID: %s", MAC2STR(bssid), wpa_ssid_txt(pos, ssid_len));
	*ret_ssid = pos;
	*ret_ssid_len = ssid_len;

	if (bss->ssid_len > 0)
		return;

	open_bss = wpa_bss_get_bssid_latest(wpa_s, bssid);
	if (!open_bss)
		return;
	if (ssid_len != open_bss->ssid_len ||
	    os_memcmp(pos, open_bss->ssid, ssid_len) != 0) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"OWE: transition mode SSID mismatch: %s",
			wpa_ssid_txt(open_bss->ssid, open_bss->ssid_len));
		return;
	}

	owe = wpa_bss_get_vendor_ie(open_bss, OWE_IE_VENDOR_TYPE);
	if (!owe || wpa_bss_get_ie(open_bss, WLAN_EID_RSN)) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"OWE: transition mode open BSS unexpected info");
		return;
	}

	pos = owe + 6;
	end = owe + 2 + owe[1];

	if (end - pos < ETH_ALEN + 1)
		return;
	if (os_memcmp(pos, bss->bssid, ETH_ALEN) != 0) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"OWE: transition mode BSSID mismatch: " MACSTR,
			MAC2STR(pos));
		return;
	}
	pos += ETH_ALEN;
	ssid_len = *pos++;
	if (end - pos < ssid_len || ssid_len > SSID_MAX_LEN)
		return;
	wpa_dbg(wpa_s, MSG_DEBUG, "OWE: learned transition mode OWE SSID: %s",
		wpa_ssid_txt(pos, ssid_len));
	os_memcpy(bss->ssid, pos, ssid_len);
	bss->ssid_len = ssid_len;
}
#endif /* CONFIG_OWE */


struct wpa_ssid * wpa_scan_res_match(struct wpa_supplicant *wpa_s,
				     int i, struct wpa_bss *bss,
				     struct wpa_ssid *group,
				     int only_first_ssid, int debug_print)
{
	u8 wpa_ie_len, rsn_ie_len;
	int wpa;
	int freq= bss->freq;
	struct wpa_blacklist *e;
	const u8 *ie;
	struct wpa_ssid *ssid;
	int osen, rsn_osen = 0;
#ifdef CONFIG_MBO_STA
	const u8 *assoc_disallow;
#endif /* CONFIG_MBO_STA */
	const u8 *match_ssid;
	size_t match_ssid_len;
	struct wpa_ie_data data;

	ie = wpa_bss_get_vendor_ie(bss, WPA_IE_VENDOR_TYPE);
	wpa_ie_len = ie ? ie[1] : 0;

	ie = wpa_bss_get_ie(bss, WLAN_EID_RSN);
	rsn_ie_len = ie ? ie[1] : 0;
	if (ie && wpa_parse_wpa_ie_rsn(ie, 2 + ie[1], &data) == 0 &&
	    (data.key_mgmt & WPA_KEY_MGMT_OSEN))
		rsn_osen = 1;

	ie = wpa_bss_get_vendor_ie(bss, OSEN_IE_VENDOR_TYPE);
	osen = ie != NULL;

	if (debug_print) {
		wpa_dbg(wpa_s, MSG_DEBUG, "%d: " MACSTR
			" ssid='%s' wpa_ie_len=%u rsn_ie_len=%u caps=0x%x level=%d freq=%d %s%s%s",
			i, MAC2STR(bss->bssid),
			wpa_ssid_txt(bss->ssid, bss->ssid_len),
			wpa_ie_len, rsn_ie_len, bss->caps, bss->level,
			bss->freq,
			wpa_bss_get_vendor_ie(bss, WPS_IE_VENDOR_TYPE) ?
			" wps" : "",
			(wpa_bss_get_vendor_ie(bss, P2P_IE_VENDOR_TYPE) ||
			 wpa_bss_get_vendor_ie_beacon(bss, P2P_IE_VENDOR_TYPE))
			? " p2p" : "",
			osen ? " osen=1" : "");
	}

	e = wpa_blacklist_get(wpa_s, bss->bssid);
	if (e) {
		int limit = 1;
		if (wpa_supplicant_enabled_networks(wpa_s) == 1) {
			/*
			 * When only a single network is enabled, we can
			 * trigger blacklisting on the first failure. This
			 * should not be done with multiple enabled networks to
			 * avoid getting forced to move into a worse ESS on
			 * single error if there are no other BSSes of the
			 * current ESS.
			 */
			limit = 0;
		}
		if (e->count > limit) {
			if (debug_print) {
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - blacklisted (count=%d limit=%d)",
					e->count, limit);
			}
			return NULL;
		}
	}

	match_ssid = bss->ssid;
	match_ssid_len = bss->ssid_len;
#ifdef CONFIG_OWE
	owe_trans_ssid(wpa_s, bss, &match_ssid, &match_ssid_len);
#endif

	if (match_ssid_len == 0) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG, "   skip - SSID not known");
		return NULL;
	}

	int mac_based_join_mismatch = 0;
	supp_mgmt_if_ctx.bssid = bss->bssid;
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_MAC_BASED_JOIN_MISMATCH, &mac_based_join_mismatch);

	if (mac_based_join_mismatch) {
		wpa_dbg(wpa_s, MSG_DEBUG, "   skip -Host Given BSSID mismatch");
		return NULL;
	}

	if (disallowed_bssid(wpa_s, bss->bssid)) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG, "   skip - BSSID disallowed");
		return NULL;
	}

	if (disallowed_ssid(wpa_s, match_ssid, match_ssid_len)) {
		if (debug_print)
			wpa_dbg(wpa_s, MSG_DEBUG, "   skip - SSID disallowed");
		return NULL;
	}

	wpa = wpa_ie_len > 0 || rsn_ie_len > 0;

	for (ssid = group; ssid; ssid = only_first_ssid ? NULL : ssid->pnext) {
		int check_ssid = wpa ? 1 : (ssid->ssid_len != 0);
		int res;

		if (wpas_network_disabled(wpa_s, ssid)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG, "   skip - disabled");
			continue;
		}

		res = wpas_temp_disabled(wpa_s, ssid);
		if (res > 0) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - disabled temporarily for %d second(s)",
					res);
			continue;
		}
/*ROAMING Change*/
		{
			int channel = 0;
			/* Translate freq to channel . If channel < Max_ then check this condition */
			channel = (wpa_supplicant_translate_channel(freq));

			if (channel != 0) {
				if (wpa_s->channel_status[channel - 1].status == CHANNEL_NOISY) {
					wpa_printf(MSG_DEBUG, "  skip - channel is noisy");
					continue;
				}
			}
		}

#ifdef CONFIG_WPS
		if ((ssid->key_mgmt & WPA_KEY_MGMT_WPS) && e && e->count > 0) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - blacklisted (WPS)");
			continue;
		}

		if (wpa && ssid->ssid_len == 0 &&
		    wpas_wps_ssid_wildcard_ok(wpa_s, ssid, bss))
			check_ssid = 0;

		if (!wpa && (ssid->key_mgmt & WPA_KEY_MGMT_WPS)) {
			/* Only allow wildcard SSID match if an AP
			 * advertises active WPS operation that matches
			 * with our mode. */
			check_ssid = 1;
			if (ssid->ssid_len == 0 &&
			    wpas_wps_ssid_wildcard_ok(wpa_s, ssid, bss))
				check_ssid = 0;
		}
#endif /* CONFIG_WPS */

		if (ssid->bssid_set && ssid->ssid_len == 0 &&
		    os_memcmp(bss->bssid, ssid->bssid, ETH_ALEN) == 0)
			check_ssid = 0;

		if (check_ssid &&
		    (match_ssid_len != ssid->ssid_len ||
		     os_memcmp(match_ssid, ssid->ssid, match_ssid_len) != 0)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - SSID mismatch");
			continue;
		}

		if (ssid->bssid_set &&
		    os_memcmp(bss->bssid, ssid->bssid, ETH_ALEN) != 0) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - BSSID mismatch");
			continue;
		}

		/* check blacklist */
		if (ssid->num_bssid_blacklist &&
		    addr_in_list(bss->bssid, ssid->bssid_blacklist,
				 ssid->num_bssid_blacklist)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - BSSID blacklisted");
			continue;
		}

		/* if there is a whitelist, only accept those APs */
		if (ssid->num_bssid_whitelist &&
		    !addr_in_list(bss->bssid, ssid->bssid_whitelist,
				  ssid->num_bssid_whitelist)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - BSSID not in whitelist");
			continue;
		}

		if (!wpa_supplicant_ssid_bss_match(wpa_s, ssid, bss,
						   debug_print))
			continue;

		if (!osen && !wpa &&
		    !(ssid->key_mgmt & WPA_KEY_MGMT_NONE) &&
		    !(ssid->key_mgmt & WPA_KEY_MGMT_WPS) &&
		    !(ssid->key_mgmt & WPA_KEY_MGMT_OWE) &&
		    !(ssid->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - non-WPA network not allowed");
			continue;
		}

		if (wpa && !wpa_key_mgmt_wpa(ssid->key_mgmt) &&
		    has_wep_key(ssid)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - ignore WPA/WPA2 AP for WEP network block");
			continue;
		}

		if ((ssid->key_mgmt & WPA_KEY_MGMT_OSEN) && !osen &&
		    !rsn_osen) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - non-OSEN network not allowed");
			continue;
		}

		if (!wpa_supplicant_match_privacy(bss, ssid)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - privacy mismatch");
			continue;
		}
		if (ssid->mode != WPAS_MODE_MESH && !bss_is_ess(bss) &&
		    !bss_is_pbss(bss)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - not ESS, PBSS, or MBSS");
			continue;
		}

		if (ssid->pbss != 2 && ssid->pbss != bss_is_pbss(bss)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - PBSS mismatch (ssid %d bss %d)",
					ssid->pbss, bss_is_pbss(bss));
			continue;
		}

		if (!freq_allowed(ssid->freq_list, bss->freq)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - frequency not allowed");
			continue;
		}

#ifdef CONFIG_MESH
		if (ssid->mode == WPAS_MODE_MESH && ssid->frequency > 0 &&
		    ssid->frequency != bss->freq) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - frequency not allowed (mesh)");
			continue;
		}
#endif /* CONFIG_MESH */

		if (!rate_match(wpa_s, ssid, bss, debug_print)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - rate sets do not match");
			continue;
		}

#ifndef CONFIG_IBSS_RSN
		if (ssid->mode == WPAS_MODE_IBSS &&
		    !(ssid->key_mgmt & (WPA_KEY_MGMT_NONE |
					WPA_KEY_MGMT_WPA_NONE))) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - IBSS RSN not supported in the build");
			continue;
		}
#endif /* !CONFIG_IBSS_RSN */

#ifdef CONFIG_P2P
		if (ssid->p2p_group &&
		    !wpa_bss_get_vendor_ie(bss, P2P_IE_VENDOR_TYPE) &&
		    !wpa_bss_get_vendor_ie_beacon(bss, P2P_IE_VENDOR_TYPE)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - no P2P IE seen");
			continue;
		}

		if (!is_zero_ether_addr(ssid->go_p2p_dev_addr)) {
			struct wpabuf *p2p_ie;
			u8 dev_addr[ETH_ALEN];

			ie = wpa_bss_get_vendor_ie(bss, P2P_IE_VENDOR_TYPE);
			if (ie == NULL) {
				if (debug_print)
					wpa_dbg(wpa_s, MSG_DEBUG,
						"   skip - no P2P element");
				continue;
			}
			p2p_ie = wpa_bss_get_vendor_ie_multi(
				bss, P2P_IE_VENDOR_TYPE);
			if (p2p_ie == NULL) {
				if (debug_print)
					wpa_dbg(wpa_s, MSG_DEBUG,
						"   skip - could not fetch P2P element");
				continue;
			}

			if (p2p_parse_dev_addr_in_p2p_ie(p2p_ie, dev_addr) < 0
			    || os_memcmp(dev_addr, ssid->go_p2p_dev_addr,
					 ETH_ALEN) != 0) {
				if (debug_print)
					wpa_dbg(wpa_s, MSG_DEBUG,
						"   skip - no matching GO P2P Device Address in P2P element");
				wpabuf_free(p2p_ie);
				continue;
			}
			wpabuf_free(p2p_ie);
		}

		/*
		 * TODO: skip the AP if its P2P IE has Group Formation
		 * bit set in the P2P Group Capability Bitmap and we
		 * are not in Group Formation with that device.
		 */
#endif /* CONFIG_P2P */

		if (os_reltime_before(&bss->last_update, &wpa_s->scan_min_time))
		{
			struct os_reltime diff;

			os_reltime_sub(&wpa_s->scan_min_time,
				       &bss->last_update, &diff);
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - scan result not recent enough (%u.%06u seconds too old)",
				(unsigned int) diff.sec,
				(unsigned int) diff.usec);
			continue;
		}
#ifdef CONFIG_MBO_STA
#ifdef CONFIG_TESTING_OPTIONS
		if (wpa_s->ignore_assoc_disallow)
			goto skip_assoc_disallow;
#endif /* CONFIG_TESTING_OPTIONS */
		assoc_disallow = wpas_mbo_get_bss_attr(
			bss, MBO_ATTR_ID_ASSOC_DISALLOW);
		if (assoc_disallow && assoc_disallow[1] >= 1) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - MBO association disallowed (reason %u)",
				assoc_disallow[2]);
			continue;
		}

		if (wpa_is_bss_tmp_disallowed(wpa_s, bss)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - AP temporarily disallowed");
			continue;
		}
#ifdef CONFIG_TESTING_OPTIONS
	skip_assoc_disallow:
#endif /* CONFIG_TESTING_OPTIONS */
#endif /* CONFIG_MBO_STA */

#ifdef CONFIG_DPP
		if ((ssid->key_mgmt & WPA_KEY_MGMT_DPP) &&
		    !wpa_sm_pmksa_exists(wpa_s->wpa, bss->bssid, ssid) &&
		    (!ssid->dpp_connector ||
		     !ssid->dpp_netaccesskey ||
		     !ssid->dpp_csign)) {
			if (debug_print)
				wpa_dbg(wpa_s, MSG_DEBUG,
					"   skip - no PMKSA entry for DPP");
			continue;
		}
#endif /* CONFIG_DPP */

		/* Matching configuration found */
		return ssid;
	}

	/* No matching configuration found */
	return NULL;
}

#ifdef ENABLE_MALLOC_PRINTS
uint32 debug_roam_count = 0;
uint32 debug_rejoin_count = 0;
extern long totalloc;
#endif

static struct wpa_bss *
wpa_supplicant_select_bss(struct wpa_supplicant *wpa_s,
			  struct wpa_ssid *group,
			  struct wpa_ssid **selected_ssid,
			  int only_first_ssid)
{
#ifndef SUPPLICANT_PORTING
	unsigned int i;

	if (wpa_s->current_ssid) {
		struct wpa_ssid *ssid;

		wpa_dbg(wpa_s, MSG_DEBUG,
			"Scan results matching the currently selected network");
		for (i = 0; i < wpa_s->last_scan_res_used; i++) {
			struct wpa_bss *bss = wpa_s->last_scan_res[i];

			ssid = wpa_scan_res_match(wpa_s, i, bss, group,
						  only_first_ssid, 0);
			if (ssid != wpa_s->current_ssid)
				continue;
			wpa_dbg(wpa_s, MSG_DEBUG, "%u: " MACSTR
				" freq=%d level=%d snr=%d est_throughput=%u",
				i, MAC2STR(bss->bssid), bss->freq, bss->level,
				bss->snr, bss->est_throughput);
		}
	}

	if (only_first_ssid)
		wpa_dbg(wpa_s, MSG_DEBUG, "Try to find BSS matching pre-selected network id=%d",
			group->id);
	else
		wpa_dbg(wpa_s, MSG_DEBUG, "Selecting BSS from priority group %d",
			group->priority);

	for (i = 0; i < wpa_s->last_scan_res_used; i++) {
		struct wpa_bss *bss = wpa_s->last_scan_res[i];

		wpa_s->owe_transition_select = 1;
		*selected_ssid = wpa_scan_res_match(wpa_s, i, bss, group,
						    only_first_ssid, 1);
		wpa_s->owe_transition_select = 0;
		if (!*selected_ssid)
			continue;
		wpa_dbg(wpa_s, MSG_DEBUG, "   selected BSS " MACSTR
			" ssid='%s'",
			MAC2STR(bss->bssid),
			wpa_ssid_txt(bss->ssid, bss->ssid_len));
		return bss;
	}

	return NULL;

#else
	unsigned int i;

/*ROAMING Changes*/
	struct wpa_bss *bss;
	u8 connected = 0, connectedAP_status = 0;
	int connectedAP_freq;
	int level = 0, index = 0;
	int freq = 0;

	/* By default we assume that current AP is best AP */
	supp_mgmt_if_ctx.async_notif_args.state_code = RSI_MODULE_STATE_CURRENT_AP_IS_BEST;
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_STATE_CODE, NULL);
	/* If we are already in the connected state then do this */
	if (wpa_s->wpa_state == WPA_COMPLETED) {
			connected = 1;
	}

  if (connected) {
    /* If in connected state, figure out the RSSI of the 
     * connected AP and its corresponding entry in the 
     * scan table 
     */
    for (i = 0; i < wpa_s->last_scan_res_used; i++) {
      if (os_memcmp(wpa_s->last_scan_res[i]->bssid, wpa_s->bssid, ETH_ALEN) == 0) {
        wpa_s->level = -(wpa_s->last_scan_res[i]->level); 

        supp_mgmt_if_ctx.async_notif_args.rssi        = (uint8) wpa_s->level;
        supp_mgmt_if_ctx.async_notif_args.reason_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
        supp_mgmt_if_ctx.async_notif_args.channel = get_channel_num((uint16)wpa_s->last_scan_res[i]->freq);
        sl_memcpy(supp_mgmt_if_ctx.async_notif_args.bssid, wpa_s->bssid, MAC_ADDR_LEN);

        sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_RSSI, NULL);
        sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_REASON_CODE, NULL);
        sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_CHANNEL, NULL);
        sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_BSSID, NULL);

        connectedAP_freq = wpa_s->last_scan_res[i]->freq;
        wpa_printf(MSG_DEBUG, "connected AP_freq %d", connectedAP_freq);
        connectedAP_status = wpa_s->channel_status[
          (wpa_supplicant_translate_channel(connectedAP_freq)) - 1].status;
        index = i;
        break;
      }
    }

    if (wpa_s->level < wpa_s->conf->roam_threshold) {
      wpa_printf(MSG_DEBUG, "Skipping searching for APs - current AP's RSSI %d is below "
          "the roam threshold: %d", wpa_s->level,
          wpa_s->conf->roam_threshold);
      bss = (struct wpa_bss *)wpa_s->last_scan_res[index];
      *selected_ssid = wpa_s->current_ssid;
      return wpa_bss_get(wpa_s, bss->bssid, wpa_s->current_ssid->ssid, wpa_s->current_ssid->ssid_len);
    }
  }
	
	wpa_dbg(wpa_s, MSG_DEBUG, "Selecting BSS from priority group %d",
		group->priority);

	for (i = 0; i < wpa_s->last_scan_res_used; i++) {
		struct wpa_bss *bss = wpa_s->last_scan_res[i];
		/*Roaming changes*/
		level = -(wpa_s->last_scan_res[i]->level); 
		freq = wpa_s->last_scan_res[i]->freq;
		*selected_ssid = wpa_scan_res_match(wpa_s, i, bss, group,0,0);

		if (!*selected_ssid)
			continue;
		/* Check here that if the connected AP's RSSI is better 
		 * by roam hysteresis,
		 * only then issue a connect request else continue,
		 * check in the connected state only.
		 */	

/*ROAMING Changes*/
		if (connected) {
			if (connectedAP_status == CHANNEL_GOOD) {
				if (wpa_s->level - level < wpa_s->conf->roam_hysteresis) {
					wpa_printf(MSG_DEBUG, "   skip - "
							"RSSI doesn't cross roam hystersis");
					continue;
				}
				/* Better AP found so update the other params too */
				supp_mgmt_if_ctx.async_notif_args.state_code = RSI_MODULE_STATE_BETTER_AP_FOUND;
				sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_STATE_CODE, NULL);
			}
		}

#ifdef ENABLE_MALLOC_PRINTS
		if(connected) {
		  ++debug_roam_count;
		}
		else {
		  ++debug_rejoin_count;
		}
#endif
		supp_mgmt_if_ctx.async_notif_args.rssi        = (uint8) level;
		supp_mgmt_if_ctx.async_notif_args.reason_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
		supp_mgmt_if_ctx.async_notif_args.channel = get_channel_num((uint16)wpa_s->last_scan_res[i]->freq);
		sl_memcpy(supp_mgmt_if_ctx.async_notif_args.bssid, wpa_s->last_scan_res[i]->bssid, MAC_ADDR_LEN);
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_RSSI, NULL);
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_REASON_CODE, NULL);
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_CHANNEL, NULL);
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_UPDATE_ASYNC_BSSID, NULL);
		wise_module_state(RSI_MODULE_STATE_SCAN_RESULT);

		wpa_dbg(wpa_s, MSG_DEBUG, "   selected BSS " MACSTR
			" ssid='%s'",
			MAC2STR(bss->bssid),
			wpa_ssid_txt(bss->ssid, bss->ssid_len));
		return bss;
	}
#ifdef ENABLE_PREAUTHENTICATION
	if (connected) {
		wise_module_state(RSI_MODULE_STATE_SCAN_RESULT);
		*selected_ssid = wpa_s->current_ssid;
		return wpa_bss_get(wpa_s, wpa_s->bssid, wpa_s->current_ssid->ssid, wpa_s->current_ssid->ssid_len);
	}
#endif
	return NULL;
#endif
}


struct wpa_bss * wpa_supplicant_pick_network(struct wpa_supplicant *wpa_s,
					     struct wpa_ssid **selected_ssid)
{
	struct wpa_bss *selected = NULL;
	int prio;

	if (wpa_s->last_scan_res == NULL ||
	    wpa_s->last_scan_res_used == 0)
		return NULL; /* no scan results from last update */

#if UNUSED_FEAT_IN_SUPP_29
	struct wpa_ssid *next_ssid = NULL;
	struct wpa_ssid *ssid;
	if (wpa_s->next_ssid) {
		/* check that next_ssid is still valid */
		for (ssid = wpa_s->conf->ssid; ssid; ssid = ssid->next) {
			if (ssid == wpa_s->next_ssid)
				break;
		}
		next_ssid = ssid;
		wpa_s->next_ssid = NULL;
	}
#endif

	while (selected == NULL) {
		for (prio = 0; prio < wpa_s->conf->num_prio; prio++) {

#if UNUSED_FEAT_IN_SUPP_29

			if (next_ssid && next_ssid->priority ==
			    wpa_s->conf->pssid[prio]->priority) {
				selected = wpa_supplicant_select_bss(
					wpa_s, next_ssid, selected_ssid, 1);
				if (selected)
					break;
			}
#endif

			selected = wpa_supplicant_select_bss(
				wpa_s, wpa_s->conf->pssid[prio],
				selected_ssid, 0);
			if (selected)
				break;
		}

		if (selected == NULL && wpa_s->blacklist &&
		    !wpa_s->countermeasures) {
			wpa_dbg(wpa_s, MSG_DEBUG, "No APs found - clear "
				"blacklist and try again");
			wpa_blacklist_clear(wpa_s);
			wpa_s->blacklist_cleared++;
		} else if (selected == NULL)
			break;
	}
#if UNUSED_FEAT_IN_SUPP_29

	ssid = *selected_ssid;
	if (selected && ssid && ssid->mem_only_psk && !ssid->psk_set &&
	    !ssid->passphrase && !ssid->ext_psk) {
		const char *field_name, *txt = NULL;

		wpa_dbg(wpa_s, MSG_DEBUG,
			"PSK/passphrase not yet available for the selected network");

		wpas_notify_network_request(wpa_s, ssid,
					    WPA_CTRL_REQ_PSK_PASSPHRASE, NULL);

		field_name = wpa_supplicant_ctrl_req_to_string(
			WPA_CTRL_REQ_PSK_PASSPHRASE, NULL, &txt);
		if (field_name == NULL)
			return NULL;

		wpas_send_ctrl_req(wpa_s, ssid, field_name, txt);

		selected = NULL;
	}
#endif

	return selected;
}


static void wpa_supplicant_req_new_scan(struct wpa_supplicant *wpa_s,
					int timeout_sec, int timeout_usec)
{
	if (!wpa_supplicant_enabled_networks(wpa_s)) {
		/*
		 * No networks are enabled; short-circuit request so
		 * we don't wait timeout seconds before transitioning
		 * to INACTIVE state.
		 */
		wpa_dbg(wpa_s, MSG_DEBUG, "Short-circuit new scan request "
			"since there are no enabled networks");
		wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
		return;
	}

	wpa_s->scan_for_connection = 1;
	wpa_supplicant_req_scan(wpa_s, timeout_sec, timeout_usec);
}


int wpa_supplicant_connect(struct wpa_supplicant *wpa_s,
			   struct wpa_bss *selected,
			   struct wpa_ssid *ssid)
{
	if (wpas_wps_scan_pbc_overlap(wpa_s, selected, ssid)) {
		wpa_msg(wpa_s, MSG_INFO, WPS_EVENT_OVERLAP
			"PBC session overlap");
		wpas_notify_wps_event_pbc_overlap(wpa_s);
#ifdef CONFIG_P2P
		if (wpa_s->p2p_group_interface == P2P_GROUP_INTERFACE_CLIENT ||
		    wpa_s->p2p_in_provisioning) {
			eloop_register_timeout(0, 0, wpas_p2p_pbc_overlap_cb,
					       wpa_s, NULL);
			return -1;
		}
#endif /* CONFIG_P2P */

#ifdef CONFIG_WPS
		wpas_wps_pbc_overlap(wpa_s);
		wpas_wps_cancel(wpa_s);
#endif /* CONFIG_WPS */
		return -1;
	}

	wpa_msg(wpa_s, MSG_DEBUG,
		"Considering connect request: reassociate: %d  selected: "
		MACSTR "  bssid: " MACSTR "  pending: " MACSTR
		"  wpa_state: %s  ssid=%p  current_ssid=%p",
		wpa_s->reassociate, MAC2STR(selected->bssid),
		MAC2STR(wpa_s->bssid), MAC2STR(wpa_s->pending_bssid),
		wpa_supplicant_state_txt(wpa_s->wpa_state),
		ssid, wpa_s->current_ssid);

	/*
	 * Do not trigger new association unless the BSSID has changed or if
	 * reassociation is requested. If we are in process of associating with
	 * the selected BSSID, do not trigger new attempt.
	 */
	if (wpa_s->reassociate ||
	    (os_memcmp(selected->bssid, wpa_s->bssid, ETH_ALEN) != 0 &&
	     ((wpa_s->wpa_state != WPA_ASSOCIATING &&
	       wpa_s->wpa_state != WPA_AUTHENTICATING) ||
	      (!is_zero_ether_addr(wpa_s->pending_bssid) &&
	       os_memcmp(selected->bssid, wpa_s->pending_bssid, ETH_ALEN) !=
	       0) ||
	      (is_zero_ether_addr(wpa_s->pending_bssid) &&
	       ssid != wpa_s->current_ssid)))) {
		if (wpa_supplicant_scard_init(wpa_s, ssid)) {
			wpa_supplicant_req_new_scan(wpa_s, 10, 0);
			return 0;
		}
		wpa_msg(wpa_s, MSG_DEBUG, "Request association with " MACSTR,
			MAC2STR(selected->bssid));
		wpa_supplicant_associate(wpa_s, selected, ssid);
	} else {
		wpa_dbg(wpa_s, MSG_DEBUG, "Already associated or trying to "
			"connect with the selected AP");
	}

	return 0;
}


static struct wpa_ssid *
wpa_supplicant_pick_new_network(struct wpa_supplicant *wpa_s)
{
	int prio;
	struct wpa_ssid *ssid;

	for (prio = 0; prio < wpa_s->conf->num_prio; prio++) {
		for (ssid = wpa_s->conf->pssid[prio]; ssid; ssid = ssid->pnext)
		{
			if (wpas_network_disabled(wpa_s, ssid))
				continue;
#ifndef CONFIG_IBSS_RSN
			if (ssid->mode == WPAS_MODE_IBSS &&
			    !(ssid->key_mgmt & (WPA_KEY_MGMT_NONE |
						WPA_KEY_MGMT_WPA_NONE))) {
				wpa_msg(wpa_s, MSG_INFO,
					"IBSS RSN not supported in the build - cannot use the profile for SSID '%s'",
					wpa_ssid_txt(ssid->ssid,
						     ssid->ssid_len));
				continue;
			}
#endif /* !CONFIG_IBSS_RSN */
			if (ssid->mode == WPAS_MODE_IBSS ||
			    ssid->mode == WPAS_MODE_AP ||
			    ssid->mode == WPAS_MODE_MESH)
				return ssid;
		}
	}
	return NULL;
}


/* TODO: move the rsn_preauth_scan_result*() to be called from notify.c based
 * on BSS added and BSS changed events */
static void wpa_supplicant_rsn_preauth_scan_results(
	struct wpa_supplicant *wpa_s)
{
	struct wpa_bss *bss;

	if (rsn_preauth_scan_results(wpa_s->wpa) < 0)
		return;

	dl_list_for_each(bss, &wpa_s->bss, struct wpa_bss, list) {
		const u8 *ssid, *rsn;

		ssid = wpa_bss_get_ie(bss, WLAN_EID_SSID);
		if (ssid == NULL)
			continue;

		rsn = wpa_bss_get_ie(bss, WLAN_EID_RSN);
		if (rsn == NULL)
			continue;

		rsn_preauth_scan_result(wpa_s->wpa, bss->bssid, ssid, rsn);
	}

}


static int wpa_supplicant_need_to_roam(struct wpa_supplicant *wpa_s,
				       struct wpa_bss *selected,
				       struct wpa_ssid *ssid)
{
#if DISABLED_IN_SUPP_20
	struct wpa_bss *current_bss = NULL;
#ifndef CONFIG_NO_ROAMING
	int min_diff, diff;
	int to_5ghz;
	int cur_est, sel_est;
#endif /* CONFIG_NO_ROAMING */

	if (wpa_s->reassociate)
		return 1; /* explicit request to reassociate */
	if (wpa_s->wpa_state < WPA_ASSOCIATED)
		return 1; /* we are not associated; continue */
	if (wpa_s->current_ssid == NULL)
		return 1; /* unknown current SSID */
	if (wpa_s->current_ssid != ssid)
		return 1; /* different network block */

	if (wpas_driver_bss_selection(wpa_s))
		return 0; /* Driver-based roaming */

	if (wpa_s->current_ssid->ssid)
		current_bss = wpa_bss_get(wpa_s, wpa_s->bssid,
					  wpa_s->current_ssid->ssid,
					  wpa_s->current_ssid->ssid_len);
	if (!current_bss)
		current_bss = wpa_bss_get_bssid(wpa_s, wpa_s->bssid);

	if (!current_bss)
		return 1; /* current BSS not seen in scan results */

	if (current_bss == selected)
		return 0;

	if (selected->last_update_idx > current_bss->last_update_idx)
		return 1; /* current BSS not seen in the last scan */
		
#ifndef CONFIG_NO_ROAMING
	wpa_dbg(wpa_s, MSG_DEBUG, "Considering within-ESS reassociation");
	wpa_dbg(wpa_s, MSG_DEBUG, "Current BSS: " MACSTR
		" freq=%d level=%d snr=%d est_throughput=%u",
		MAC2STR(current_bss->bssid),
		current_bss->freq, current_bss->level,
		current_bss->snr, current_bss->est_throughput);
	wpa_dbg(wpa_s, MSG_DEBUG, "Selected BSS: " MACSTR
		" freq=%d level=%d snr=%d est_throughput=%u",
		MAC2STR(selected->bssid), selected->freq, selected->level,
		selected->snr, selected->est_throughput);

	if (wpa_s->current_ssid->bssid_set &&
	    os_memcmp(selected->bssid, wpa_s->current_ssid->bssid, ETH_ALEN) ==
	    0) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Allow reassociation - selected BSS "
			"has preferred BSSID");
		return 1;
	}

	if (selected->est_throughput > current_bss->est_throughput + 5000) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"Allow reassociation - selected BSS has better estimated throughput");
		return 1;
	}

	to_5ghz = selected->freq > 4000 && current_bss->freq < 4000;

	if (current_bss->level < 0 &&
	    current_bss->level > selected->level + to_5ghz * 2) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Skip roam - Current BSS has better "
			"signal level");
		return 0;
	}

	if (current_bss->est_throughput > selected->est_throughput + 5000) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"Skip roam - Current BSS has better estimated throughput");
		return 0;
	}

	cur_est = current_bss->est_throughput;
	sel_est = selected->est_throughput;
	min_diff = 2;
	if (current_bss->level < 0) {
		if (current_bss->level < -85)
			min_diff = 1;
		else if (current_bss->level < -80)
			min_diff = 2;
		else if (current_bss->level < -75)
			min_diff = 3;
		else if (current_bss->level < -70)
			min_diff = 4;
		else
			min_diff = 5;
		if (cur_est > sel_est * 1.5)
			min_diff += 10;
		else if (cur_est > sel_est * 1.2)
			min_diff += 5;
		else if (cur_est > sel_est * 1.1)
			min_diff += 2;
		else if (cur_est > sel_est)
			min_diff++;
	}
	if (to_5ghz) {
		int reduce = 2;

		/* Make it easier to move to 5 GHz band */
		if (sel_est > cur_est * 1.5)
			reduce = 5;
		else if (sel_est > cur_est * 1.2)
			reduce = 4;
		else if (sel_est > cur_est * 1.1)
			reduce = 3;

		if (min_diff > reduce)
			min_diff -= reduce;
		else
			min_diff = 0;
	}
	diff = abs(current_bss->level - selected->level);
	if (diff < min_diff) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"Skip roam - too small difference in signal level (%d < %d)",
			diff, min_diff);
		return 0;
	}

	wpa_dbg(wpa_s, MSG_DEBUG,
		"Allow reassociation due to difference in signal level (%d >= %d)",
		diff, min_diff);
	return 1;
#else /* CONFIG_NO_ROAMING */
	return 0;
#endif /* CONFIG_NO_ROAMING */
#endif
#if 1

  int is_connected = 0;
  sl_wpa_driver_wrapper(wpa_s, WPA_DRV_CHK_CONNECTED_STATE, &is_connected);
  if (!is_connected) {
	supp_mgmt_if_ctx.roam_bssid_ind_update_input.roam_indication = 0;
	supp_mgmt_if_ctx.roam_bssid_ind_update_input.roam_bssid = NULL;
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_SET_ROAM_INDICATION_AND_BSSID, NULL);
    return 1;
  }
  /*ROAMING Change*/
  int prev_bssid_mismatch = 0;
  supp_mgmt_if_ctx.bssid = selected->bssid;
  sl_wpa_driver_wrapper(wpa_s, WPA_DRV_CHK_PREV_BSSID_MISMATCH, &prev_bssid_mismatch);
  if((prev_bssid_mismatch == 1) &&
      (os_memcmp(selected->ssid, wpa_s->current_ssid->ssid, wpa_s->current_ssid->ssid_len) == 0)){
		supp_mgmt_if_ctx.roam_bssid_ind_update_input.roam_indication = 1;
		supp_mgmt_if_ctx.roam_bssid_ind_update_input.roam_bssid= selected->bssid;
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_SET_ROAM_INDICATION_AND_BSSID, NULL);
    return 1;
  }

  return 0;
#endif
#if 0
  wsc->roam_ind = 0;
  if (((wpa_s->wpa_state == WPA_COMPLETED)&& (wpa_s->level < wpa_s->conf->roam_threshold))||
      ((wpa_s->wpa_state == WPA_COMPLETED)&&os_memcmp(selected->bssid, wpa_s->bssid, ETH_ALEN) == 0)){
    return 0; /*Need to skeep the Roaming*/
  }
  if((wpa_s->wpa_state == WPA_COMPLETED)&&os_memcmp(selected->bssid, wpa_s->bssid, ETH_ALEN) == 0){
    wsc->roam_ind = 1;
  }
  return 1;
#endif









}


/*
 * Return a negative value if no scan results could be fetched or if scan
 * results should not be shared with other virtual interfaces.
 * Return 0 if scan results were fetched and may be shared with other
 * interfaces.
 * Return 1 if scan results may be shared with other virtual interfaces but may
 * not trigger any operations.
 * Return 2 if the interface was removed and cannot be used.
 */
static int _wpa_supplicant_event_scan_results(struct wpa_supplicant *wpa_s,
					      union wpa_event_data *data,
					      int own_request, int update_only)
{
	struct wpa_scan_results *scan_res = NULL;
	int ret = 0;
	int ap = 0;
	int is_p2p_mode = 0;
#ifndef CONFIG_NO_RANDOM_POOL
	size_t i, num;
#endif /* CONFIG_NO_RANDOM_POOL */

#ifdef CONFIG_AP
	if (wpa_s->ap_iface)
		ap = 1;
#endif /* CONFIG_AP */

	wpa_supplicant_notify_scanning(wpa_s, 0);

#if 0 //def CONFIG_P2P /* This change is pulled from supp-20 orig, need to check is it required to pull */
	if (wpa_s->global->p2p_cb_on_scan_complete &&
	    !wpa_s->global->p2p_disabled &&
	    wpa_s->global->p2p != NULL && !wpa_s->sta_scan_pending &&
	    !wpa_s->scan_res_handler) {
		wpa_s->global->p2p_cb_on_scan_complete = 0;
		if (P2P_OTHER_SCAN_COMPLETED(wpa_s->global->p2p) == 1) {
			wpa_dbg(wpa_s, MSG_DEBUG, "P2P: Pending P2P operation "
				"stopped scan processing");
			wpa_s->sta_scan_pending = 1;
			WPA_SUPPLICANT_REQ_SCAN(wpa_s, 5, 0);
			return -1;
		}
	}
	wpa_s->sta_scan_pending = 0;
#endif 

	scan_res = wpa_supplicant_get_scan_results(wpa_s,
						   data ? &data->scan_info :
						   NULL, 1);
	if (scan_res == NULL) {
#if UNUSED_FEAT_IN_SUPP_29
		if (wpa_s->conf->ap_scan == 2 || ap ||
		    wpa_s->scan_res_handler == scan_only_handler)
#else
		if (wpa_s->conf->ap_scan == 2 || ap) 
#endif
			return -1;
		if (!own_request)
			return -1;
		if (data && data->scan_info.external_scan)
			return -1;
		wpa_dbg(wpa_s, MSG_DEBUG, "Failed to get scan results - try "
			"scanning again");
		wpa_supplicant_req_new_scan(wpa_s, 1, 0);
		ret = -1;
		goto scan_work_done;
	}

#ifndef CONFIG_NO_RANDOM_POOL
	num = scan_res->num;
	if (num > 10)
		num = 10;
	for (i = 0; i < num; i++) {
		u8 buf[5];
		struct wpa_scan_res *res = scan_res->res[i];
		buf[0] = res->bssid[5];
		buf[1] = res->qual & 0xff;
		buf[2] = res->noise & 0xff;
		buf[3] = res->level & 0xff;
		buf[4] = res->tsf & 0xff;
		random_add_randomness(buf, sizeof(buf));
	}
#endif /* CONFIG_NO_RANDOM_POOL */

	if (update_only) {
		ret = 1;
		goto scan_work_done;
	}
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_CHK_P2P_MODE, &is_p2p_mode);
	if (is_p2p_mode) {
	if (own_request && wpa_s->scan_res_handler &&
		!(data && data->scan_info.external_scan)) {
		void (*scan_res_handler)(struct wpa_supplicant *wpa_s,
					 struct wpa_scan_results *scan_res);

		scan_res_handler = wpa_s->scan_res_handler;
		wpa_s->scan_res_handler = NULL;
		scan_res_handler(wpa_s, scan_res);
#if UNUSED_FEAT_IN_SUPP_29	
		ret = 1;
		goto scan_work_done;
#else
		wpa_scan_results_free(scan_res);
		return -2;
#endif	
	}

	}

	if (ap) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Ignore scan results in AP mode");
#ifdef CONFIG_AP
		if (wpa_s->ap_iface->scan_cb)
			wpa_s->ap_iface->scan_cb(wpa_s->ap_iface);
#endif /* CONFIG_AP */
		goto scan_work_done;
	}

	wpa_dbg(wpa_s, MSG_DEBUG, "New scan results available (own=%u ext=%u)",
		wpa_s->own_scan_running,
		data ? data->scan_info.external_scan : 0);
	if (wpa_s->last_scan_req == MANUAL_SCAN_REQ &&
	    wpa_s->manual_scan_use_id && wpa_s->own_scan_running &&
	    own_request && !(data && data->scan_info.external_scan)) {
		wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_SCAN_RESULTS "id=%u",
			     wpa_s->manual_scan_id);
		wpa_s->manual_scan_use_id = 0;
	} else {
		wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_SCAN_RESULTS);
	}
	wpas_notify_scan_results(wpa_s);

	wpas_notify_scan_done(wpa_s, 1);

	if (data && data->scan_info.external_scan) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Do not use results from externally requested scan operation for network selection");
		wpa_scan_results_free(scan_res);
		return 0;
	}

	if (wnm_scan_process(wpa_s, 1) > 0)
		goto scan_work_done;

	if (sme_proc_obss_scan(wpa_s) > 0)
		goto scan_work_done;
#if UNUSED_FEAT_IN_SUPP_29
	if (own_request && data &&
	    wpas_beacon_rep_scan_process(wpa_s, scan_res, &data->scan_info) > 0)
		goto scan_work_done;
#endif
	if ((wpa_s->conf->ap_scan == 2 && !wpas_wps_searching(wpa_s)))
		goto scan_work_done;

	if (autoscan_notify_scan(wpa_s, scan_res))
		goto scan_work_done;

	if (wpa_s->disconnected) {
		wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
		goto scan_work_done;
	}

	if (!wpas_driver_bss_selection(wpa_s) &&
	    bgscan_notify_scan(wpa_s, scan_res) == 1)
		goto scan_work_done;
#if UNUSED_FEAT_IN_SUPP_29
	wpas_wps_update_ap_info(wpa_s, scan_res);

	if (wpa_s->wpa_state >= WPA_AUTHENTICATING &&
	    wpa_s->wpa_state < WPA_COMPLETED)
		goto scan_work_done;

	wpa_scan_results_free(scan_res);

	if (own_request && wpa_s->scan_work) {
		struct wpa_radio_work *work = wpa_s->scan_work;
		wpa_s->scan_work = NULL;
		radio_work_done(work);
	}
#else	
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_CHK_P2P_MODE, &is_p2p_mode);
	if (is_p2p_mode) {
		wpas_wps_update_ap_info(wpa_s, scan_res);
	}
	wpa_scan_results_free(scan_res);
#endif
	return wpas_select_network_from_last_scan(wpa_s, 1, own_request);

scan_work_done:
	wpa_scan_results_free(scan_res);
#if UNUSED_FEAT_IN_SUPP_29	
	if (own_request && wpa_s->scan_work) {
		struct wpa_radio_work *work = wpa_s->scan_work;
		wpa_s->scan_work = NULL;
		radio_work_done(work);		
	}
#endif	
	return ret;
}


static int wpas_select_network_from_last_scan(struct wpa_supplicant *wpa_s,
					      int new_scan, int own_request)
{
	struct wpa_bss *selected;
	struct wpa_ssid *ssid = NULL;

#if UNUSED_FEAT_IN_SUPP_29/* If this is not commented, rejoin is taking more time than expected */
	int time_to_reenable = wpas_reenabled_network_time(wpa_s);

	if (time_to_reenable > 0) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"Postpone network selection by %d seconds since all networks are disabled",
			time_to_reenable);
		eloop_cancel_timeout(wpas_network_reenabled, wpa_s, NULL);
		eloop_register_timeout(time_to_reenable, 0,
				       wpas_network_reenabled, wpa_s, NULL);
		return 0;	
	}

	if (wpa_s->p2p_mgmt)
		return 0; /* no normal connection on p2p_mgmt interface */

	wpa_s->owe_transition_search = 0;
#endif
	selected = wpa_supplicant_pick_network(wpa_s, &ssid);

#ifdef CONFIG_MESH
	if (wpa_s->ifmsh) {
		wpa_msg(wpa_s, MSG_INFO,
			"Avoiding join because we already joined a mesh group");
		return 0;
	}
#endif /* CONFIG_MESH */

	if (selected) {
		int skip;
		skip = !wpa_supplicant_need_to_roam(wpa_s, selected, ssid);
		if (skip) {
			if (new_scan)
				wpa_supplicant_rsn_preauth_scan_results(wpa_s);
			return 0;
		}

		if (ssid != wpa_s->current_ssid &&
		    wpa_s->wpa_state >= WPA_AUTHENTICATING) {
			wpa_s->own_disconnect_req = 1;
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_DEAUTH_LEAVING);
		}

		if (wpa_supplicant_connect(wpa_s, selected, ssid) < 0) {
			wpa_dbg(wpa_s, MSG_DEBUG, "Connect failed");
			return -1;
		}
		if (new_scan)
			wpa_supplicant_rsn_preauth_scan_results(wpa_s);
		/*
		 * Do not allow other virtual radios to trigger operations based
		 * on these scan results since we do not want them to start
		 * other associations at the same time.
		 */
		return 1;
	} else {
		wpa_dbg(wpa_s, MSG_DEBUG, "No suitable network found");

		int status = 0;
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_ASYNC_NOTIF_AP_NOT_FOUND, &status);

		if (status == -1) {
			return 0;
		}

		ssid = wpa_supplicant_pick_new_network(wpa_s);
		if (ssid) {
			wpa_dbg(wpa_s, MSG_DEBUG, "Setup a new network");
			wpa_supplicant_associate(wpa_s, NULL, ssid);
			if (new_scan)
				wpa_supplicant_rsn_preauth_scan_results(wpa_s);
		} 

		else if (own_request) {
			/*
			 * No SSID found. If SCAN results are as a result of
			 * own scan request and not due to a scan request on
			 * another shared interface, try another scan.
			 */
			int timeout_sec = wpa_s->scan_interval;
			int timeout_usec = 0;
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
			sl_wpa_driver_wrapper(wpa_s, WPA_DRV_GET_REJOIN_SCAN_INTERVAL, &timeout_sec);
#endif
#ifdef CONFIG_P2P
			int res;

			res = wpas_p2p_scan_no_go_seen(wpa_s);
			if (res == 2)
				return 2;
			if (res == 1)
				return 0;

			if (wpa_s->p2p_in_provisioning ||
			    wpa_s->show_group_started ||
			    wpa_s->p2p_in_invitation) {
				/*
				 * Use shorter wait during P2P Provisioning
				 * state and during P2P join-a-group operation
				 * to speed up group formation.
				 */
				timeout_sec = 0;
				timeout_usec = 250000;
				wpa_supplicant_req_new_scan(wpa_s, timeout_sec,
							    timeout_usec);
				return 0;
			}
#endif /* CONFIG_P2P */
#ifdef CONFIG_INTERWORKING
			if (wpa_s->conf->auto_interworking &&
			    wpa_s->conf->interworking &&
			    wpa_s->conf->cred) {
				wpa_dbg(wpa_s, MSG_DEBUG, "Interworking: "
					"start ANQP fetch since no matching "
					"networks found");
				wpa_s->network_select = 1;
				wpa_s->auto_network_select = 1;
				interworking_start_fetch_anqp(wpa_s);
				return 1;
			}
#endif /* CONFIG_INTERWORKING */
#ifdef CONFIG_WPS
			if (wpa_s->after_wps > 0 || wpas_wps_searching(wpa_s)) {
				wpa_dbg(wpa_s, MSG_DEBUG, "Use shorter wait during WPS processing");
				timeout_sec = 0;
				timeout_usec = 500000;
				wpa_supplicant_req_new_scan(wpa_s, timeout_sec,
							    timeout_usec);
				return 0;
			}
#endif /* CONFIG_WPS */
#ifdef CONFIG_OWE
			if (wpa_s->owe_transition_search) {
				wpa_dbg(wpa_s, MSG_DEBUG,
					"OWE: Use shorter wait during transition mode search");
				timeout_sec = 0;
				timeout_usec = 500000;
				wpa_supplicant_req_new_scan(wpa_s, timeout_sec,
							    timeout_usec);
				return 0;
			}
#endif /* CONFIG_OWE */
			if (wpa_supplicant_req_sched_scan(wpa_s))
				wpa_supplicant_req_new_scan(wpa_s, timeout_sec,
							    timeout_usec);

			wpa_msg_ctrl(wpa_s, MSG_INFO,
				     WPA_EVENT_NETWORK_NOT_FOUND);
		}
	}
	return 0;
}


STATIC int wpa_supplicant_event_scan_results(struct wpa_supplicant *wpa_s,
					     union wpa_event_data *data)
{
	struct wpa_supplicant *ifs;
	int res;
	const char *rn, *rn2;

	res = _wpa_supplicant_event_scan_results(wpa_s, data, 1, 0);
	if (res == 2) {
		/*
		 * Interface may have been removed, so must not dereference
		 * wpa_s after this.
		 */
		return 1;
	}

	if (res < 0) {
		/*
		 * If no scan results could be fetched, then no need to
		 * notify those interfaces that did not actually request
		 * this scan. Similarly, if scan results started a new operation on this
		 * interface, do not notify other interfaces to avoid concurrent
		 * operations during a connection attempt.
		 */
		return 0;
	}

	/*
	 * Check other interfaces to see if they share the same radio. If
	 * so, they get updated with this same scan info.
	 */
#if UNUSED_FEAT_IN_SUPP_29
	dl_list_for_each(ifs, &wpa_s->radio->ifaces, struct wpa_supplicant,
			 radio_list) {
		if (ifs != wpa_s) {
			wpa_printf(MSG_DEBUG, "%s: Updating scan results from "
				   "sibling", ifs->ifname);
			res = _wpa_supplicant_event_scan_results(ifs, data, 0,
								 res > 0);
			if (res < 0)
				return 0;
		}
	}
#endif
#if 1
	if (!wpa_s->driver->get_radio_name)
			return -1;
	
		rn = wpa_s->driver->get_radio_name(wpa_s->drv_priv);
		if (rn == NULL || rn[0] == '\0')
			return -1;
	
		wpa_dbg(wpa_s, MSG_DEBUG, "Checking for other virtual interfaces "
			"sharing same radio (%s) in event_scan_results", rn);
	
		for (ifs = wpa_s->global->ifaces; ifs; ifs = ifs->next) {
			if (ifs == wpa_s || !ifs->driver->get_radio_name)
				continue;
	
			rn2 = ifs->driver->get_radio_name(ifs->drv_priv);
			if (rn2 && os_strcmp(rn, rn2) == 0) {
				wpa_printf(MSG_DEBUG, "%s: Updating scan results from "
					   "sibling", ifs->ifname);
				res = _wpa_supplicant_event_scan_results(ifs, data, 0,
								 res > 0);
			if (res < 0)
				return 0;
			}
		}
#endif

	return 0;
}

#endif /* CONFIG_NO_SCAN_PROCESSING */


int wpa_supplicant_fast_associate(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_NO_SCAN_PROCESSING
	return -1;
#else /* CONFIG_NO_SCAN_PROCESSING */
	struct os_reltime now;

	wpa_s->ignore_post_flush_scan_res = 0;

	if (wpa_s->last_scan_res_used == 0)
		return -1;

	os_get_reltime(&now);
	if (os_reltime_expired(&now, &wpa_s->last_scan, 5)) {
		wpa_printf(MSG_DEBUG, "Fast associate: Old scan results");
		return -1;
	}

	return wpas_select_network_from_last_scan(wpa_s, 0, 1);
#endif /* CONFIG_NO_SCAN_PROCESSING */
}

#ifdef CONFIG_WNM

static void wnm_bss_keep_alive(void *eloop_ctx, void *sock_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;

	if (wpa_s->wpa_state < WPA_ASSOCIATED)
		return;

	if (!wpa_s->no_keep_alive) {
		wpa_printf(MSG_DEBUG, "WNM: Send keep-alive to AP " MACSTR,
			   MAC2STR(wpa_s->bssid));
		/* TODO: could skip this if normal data traffic has been sent */
		/* TODO: Consider using some more appropriate data frame for
		 * this */
		if (wpa_s->l2)
			l2_packet_send(wpa_s->l2, wpa_s->bssid, 0x0800,
				       (u8 *) "", 0);
	}

#ifdef CONFIG_SME
	if (wpa_s->sme.bss_max_idle_period) {
		unsigned int msec;
		msec = wpa_s->sme.bss_max_idle_period * 1024; /* times 1000 */
		if (msec > 100)
			msec -= 100;
		eloop_register_timeout(msec / 1000, msec % 1000 * 1000,
				       wnm_bss_keep_alive, wpa_s, NULL);
	}
#endif /* CONFIG_SME */
}


static void wnm_process_assoc_resp(struct wpa_supplicant *wpa_s,
				   const u8 *ies, size_t ies_len)
{
	struct ieee802_11_elems elems;

	if (ies == NULL)
		return;

	if (ieee802_11_parse_elems(ies, ies_len, &elems, 1) == ParseFailed)
		return;

#ifdef CONFIG_SME
	if (elems.bss_max_idle_period) {
		unsigned int msec;
		wpa_s->sme.bss_max_idle_period =
			WPA_GET_LE16(elems.bss_max_idle_period);
		wpa_printf(MSG_DEBUG, "WNM: BSS Max Idle Period: %u (* 1000 "
			   "TU)%s", wpa_s->sme.bss_max_idle_period,
			   (elems.bss_max_idle_period[2] & 0x01) ?
			   " (protected keep-live required)" : "");
		if (wpa_s->sme.bss_max_idle_period == 0)
			wpa_s->sme.bss_max_idle_period = 1;
		if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME) {
			eloop_cancel_timeout(wnm_bss_keep_alive, wpa_s, NULL);
			 /* msec times 1000 */
			msec = wpa_s->sme.bss_max_idle_period * 1024;
			if (msec > 100)
				msec -= 100;
			eloop_register_timeout(msec / 1000, msec % 1000 * 1000,
					       wnm_bss_keep_alive, wpa_s,
					       NULL);
		}
	}
#endif /* CONFIG_SME */
}



void wnm_bss_keep_alive_deinit(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_WNM
	eloop_cancel_timeout(wnm_bss_keep_alive, wpa_s, NULL);
#endif /* CONFIG_WNM */
}
#endif /* CONFIG_WNM */


#ifdef CONFIG_INTERWORKING

static int wpas_qos_map_set(struct wpa_supplicant *wpa_s, const u8 *qos_map,
			    size_t len)
{
	int res;

	wpa_hexdump(MSG_DEBUG, "Interworking: QoS Map Set", qos_map, len);
	res = wpa_drv_set_qos_map(wpa_s, qos_map, len);
	if (res) {
		wpa_printf(MSG_DEBUG, "Interworking: Failed to configure QoS Map Set to the driver");
	}

	return res;
}


static void interworking_process_assoc_resp(struct wpa_supplicant *wpa_s,
					    const u8 *ies, size_t ies_len)
{
	struct ieee802_11_elems elems;

	if (ies == NULL)
		return;

	if (ieee802_11_parse_elems(ies, ies_len, &elems, 1) == ParseFailed)
		return;

	if (elems.qos_map_set) {
		wpas_qos_map_set(wpa_s, elems.qos_map_set,
				 elems.qos_map_set_len);
	}
}

#endif /* CONFIG_INTERWORKING */


static void multi_ap_process_assoc_resp(struct wpa_supplicant *wpa_s,
					const u8 *ies, size_t ies_len)
{
	struct ieee802_11_elems elems;
	const u8 *map_sub_elem, *pos;
	size_t len;

	if (!wpa_s->current_ssid ||
	    !wpa_s->current_ssid->multi_ap_backhaul_sta ||
	    !ies ||
	    ieee802_11_parse_elems(ies, ies_len, &elems, 1) == ParseFailed)
		return;

	if (!elems.multi_ap || elems.multi_ap_len < 7) {
		wpa_printf(MSG_INFO, "AP doesn't support Multi-AP protocol");
		goto fail;
	}

	pos = elems.multi_ap + 4;
	len = elems.multi_ap_len - 4;

	map_sub_elem = get_ie(pos, len, MULTI_AP_SUB_ELEM_TYPE);
	if (!map_sub_elem || map_sub_elem[1] < 1) {
		wpa_printf(MSG_INFO, "invalid Multi-AP sub elem type");
		goto fail;
	}

	if (!(map_sub_elem[2] & MULTI_AP_BACKHAUL_BSS)) {
		if ((map_sub_elem[2] & MULTI_AP_FRONTHAUL_BSS) &&
		    wpa_s->current_ssid->key_mgmt & WPA_KEY_MGMT_WPS) {
			wpa_printf(MSG_INFO,
				   "WPS active, accepting fronthaul-only BSS");
			/* Don't set 4addr mode in this case, so just return */
			return;
		}
		wpa_printf(MSG_INFO, "AP doesn't support backhaul BSS");
		goto fail;
	}

	if (wpa_drv_set_4addr_mode(wpa_s, 1) < 0) {
		wpa_printf(MSG_ERROR, "Failed to set 4addr mode");
		goto fail;
	}
	wpa_s->enabled_4addr_mode = 1;
	return;

fail:
	wpa_supplicant_deauthenticate(wpa_s, WLAN_REASON_DEAUTH_LEAVING);
}


#ifdef CONFIG_FST
static int wpas_fst_update_mbie(struct wpa_supplicant *wpa_s,
				const u8 *ie, size_t ie_len)
{
	struct mb_ies_info mb_ies;

	if (!ie || !ie_len || !wpa_s->fst)
	    return -ENOENT;

	os_memset(&mb_ies, 0, sizeof(mb_ies));

	while (ie_len >= 2 && mb_ies.nof_ies < MAX_NOF_MB_IES_SUPPORTED) {
		size_t len;

		len = 2 + ie[1];
		if (len > ie_len) {
			wpa_hexdump(MSG_DEBUG, "FST: Truncated IE found",
				    ie, ie_len);
			break;
		}

		if (ie[0] == WLAN_EID_MULTI_BAND) {
			wpa_printf(MSG_DEBUG, "MB IE of %u bytes found",
				   (unsigned int) len);
			mb_ies.ies[mb_ies.nof_ies].ie = ie + 2;
			mb_ies.ies[mb_ies.nof_ies].ie_len = len - 2;
			mb_ies.nof_ies++;
		}

		ie_len -= len;
		ie += len;
	}

	if (mb_ies.nof_ies > 0) {
		wpabuf_free(wpa_s->received_mb_ies);
		wpa_s->received_mb_ies = mb_ies_by_info(&mb_ies);
		return 0;
	}

	return -ENOENT;
}
#endif /* CONFIG_FST */


STATIC int wpa_supplicant_event_associnfo(struct wpa_supplicant *wpa_s,
					  union wpa_event_data *data)
{
	int l, len, found = 0, found_x = 0, wpa_found, rsn_found;
	const u8 *p;
#if defined(CONFIG_IEEE80211R) || defined(CONFIG_OWE)
	u8 bssid[ETH_ALEN];
#endif /* CONFIG_IEEE80211R || CONFIG_OWE */

	wpa_dbg(wpa_s, MSG_DEBUG, "Association info event");
	if (data->assoc_info.req_ies)
		wpa_hexdump(MSG_DEBUG, "req_ies", data->assoc_info.req_ies,
			    data->assoc_info.req_ies_len);
	if (data->assoc_info.resp_ies) {
		wpa_hexdump(MSG_DEBUG, "resp_ies", data->assoc_info.resp_ies,
			    data->assoc_info.resp_ies_len);
#ifdef CONFIG_TDLS
		wpa_tdls_assoc_resp_ies(wpa_s->wpa, data->assoc_info.resp_ies,
					data->assoc_info.resp_ies_len);
#endif /* CONFIG_TDLS */
#ifdef CONFIG_WNM
		wnm_process_assoc_resp(wpa_s, data->assoc_info.resp_ies,
				       data->assoc_info.resp_ies_len);
#endif /* CONFIG_WNM */
#ifdef CONFIG_INTERWORKING
		interworking_process_assoc_resp(wpa_s, data->assoc_info.resp_ies,
						data->assoc_info.resp_ies_len);
#endif /* CONFIG_INTERWORKING */
		if (wpa_s->hw_capab == CAPAB_VHT &&
		    get_ie(data->assoc_info.resp_ies,
			   data->assoc_info.resp_ies_len, WLAN_EID_VHT_CAP))
			wpa_s->ieee80211ac = 1;

		multi_ap_process_assoc_resp(wpa_s, data->assoc_info.resp_ies,
					    data->assoc_info.resp_ies_len);
	}
	if (data->assoc_info.beacon_ies)
		wpa_hexdump(MSG_DEBUG, "beacon_ies",
			    data->assoc_info.beacon_ies,
			    data->assoc_info.beacon_ies_len);
	if (data->assoc_info.freq)
		wpa_dbg(wpa_s, MSG_DEBUG, "freq=%u MHz",
			data->assoc_info.freq);

	wpa_s->connection_set = 0;
	if (data->assoc_info.req_ies && data->assoc_info.resp_ies) {
		struct ieee802_11_elems req_elems, resp_elems;

		if (ieee802_11_parse_elems(data->assoc_info.req_ies,
					   data->assoc_info.req_ies_len,
					   &req_elems, 0) != ParseFailed &&
		    ieee802_11_parse_elems(data->assoc_info.resp_ies,
					   data->assoc_info.resp_ies_len,
					   &resp_elems, 0) != ParseFailed) {
			wpa_s->connection_set = 1;
			wpa_s->connection_ht = req_elems.ht_capabilities &&
				resp_elems.ht_capabilities;
			wpa_s->connection_vht = req_elems.vht_capabilities &&
				resp_elems.vht_capabilities;
			wpa_s->connection_he = req_elems.he_capabilities &&
				resp_elems.he_capabilities;
		}
	}

	p = data->assoc_info.req_ies;
	l = data->assoc_info.req_ies_len;

	/* Go through the IEs and make a copy of the WPA/RSN IE, if present. */
	while (p && l >= 2) {
		len = p[1] + 2;
		if (len > l) {
			wpa_hexdump(MSG_DEBUG, "Truncated IE in assoc_info",
				    p, l);
			break;
		}
		if ((p[0] == WLAN_EID_VENDOR_SPECIFIC && p[1] >= 6 &&
		     (os_memcmp(&p[2], "\x00\x50\xF2\x01\x01\x00", 6) == 0)) ||
		    (p[0] == WLAN_EID_VENDOR_SPECIFIC && p[1] >= 4 &&
		     (os_memcmp(&p[2], "\x50\x6F\x9A\x12", 4) == 0)) ||
		    (p[0] == WLAN_EID_RSN && p[1] >= 2)) {
			if (wpa_sm_set_assoc_wpa_ie(wpa_s->wpa, p, len))
				break;
			found = 1;
			wpa_find_assoc_pmkid(wpa_s);
		}
		if (!found_x && p[0] == WLAN_EID_RSNX) {
			if (wpa_sm_set_assoc_rsnxe(wpa_s->wpa, p, len))
				break;
			found_x = 1;
		}
		l -= len;
		p += len;
	}
	if (!found && data->assoc_info.req_ies)
		wpa_sm_set_assoc_wpa_ie(wpa_s->wpa, NULL, 0);
	if (!found_x && data->assoc_info.req_ies)
		wpa_sm_set_assoc_rsnxe(wpa_s->wpa, NULL, 0);

#ifdef CONFIG_FILS
#ifdef CONFIG_SME
	if ((wpa_s->sme.auth_alg == WPA_AUTH_ALG_FILS ||
	     wpa_s->sme.auth_alg == WPA_AUTH_ALG_FILS_SK_PFS) &&
	    (!data->assoc_info.resp_frame ||
	     fils_process_assoc_resp(wpa_s->wpa,
				     data->assoc_info.resp_frame,
				     data->assoc_info.resp_frame_len) < 0)) {
		wpa_supplicant_deauthenticate(wpa_s, WLAN_REASON_UNSPECIFIED);
		return -1;
	}
#endif /* CONFIG_SME */

	/* Additional processing for FILS when SME is in driver */
	if (wpa_s->auth_alg == WPA_AUTH_ALG_FILS &&
	    !(wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME))
		wpa_sm_set_reset_fils_completed(wpa_s->wpa, 1);
#endif /* CONFIG_FILS */

#ifdef CONFIG_OWE
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_OWE &&
	    (wpa_drv_get_bssid(wpa_s, bssid) < 0 ||
	     owe_process_assoc_resp(wpa_s->wpa, bssid,
				    data->assoc_info.resp_ies,
				    data->assoc_info.resp_ies_len) < 0)) {
		wpa_supplicant_deauthenticate(wpa_s, WLAN_REASON_UNSPECIFIED);
		return -1;
	}
#endif /* CONFIG_OWE */

#ifdef CONFIG_DPP2
	wpa_sm_set_dpp_z(wpa_s->wpa, NULL);
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_DPP && wpa_s->dpp_pfs) {
		struct ieee802_11_elems elems;

		if (ieee802_11_parse_elems(data->assoc_info.resp_ies,
					   data->assoc_info.resp_ies_len,
					   &elems, 0) == ParseFailed ||
		    !elems.owe_dh)
			goto no_pfs;
		if (dpp_pfs_process(wpa_s->dpp_pfs, elems.owe_dh,
				    elems.owe_dh_len) < 0) {
			wpa_supplicant_deauthenticate(wpa_s,
						      WLAN_REASON_UNSPECIFIED);
			return -1;
		}

		wpa_sm_set_dpp_z(wpa_s->wpa, wpa_s->dpp_pfs->secret);
	}
no_pfs:
#endif /* CONFIG_DPP2 */

#ifdef CONFIG_IEEE80211R
#ifdef CONFIG_SME
	if (wpa_s->sme.auth_alg == WPA_AUTH_ALG_FT) {
		if (wpa_drv_get_bssid(wpa_s, bssid) < 0 ||
		    wpa_ft_validate_reassoc_resp(wpa_s->wpa,
						 data->assoc_info.resp_ies,
						 data->assoc_info.resp_ies_len,
						 bssid) < 0) {
			wpa_dbg(wpa_s, MSG_DEBUG, "FT: Validation of "
				"Reassociation Response failed");
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_INVALID_IE);
			return -1;
		}
	}

	p = data->assoc_info.resp_ies;
	l = data->assoc_info.resp_ies_len;

#ifdef CONFIG_WPS_STRICT
	if (p && wpa_s->current_ssid &&
	    wpa_s->current_ssid->key_mgmt == WPA_KEY_MGMT_WPS) {
		struct wpabuf *wps;
		wps = ieee802_11_vendor_ie_concat(p, l, WPS_IE_VENDOR_TYPE);
		if (wps == NULL) {
			wpa_msg(wpa_s, MSG_INFO, "WPS-STRICT: AP did not "
				"include WPS IE in (Re)Association Response");
			return -1;
		}

		if (wps_validate_assoc_resp(wps) < 0) {
			wpabuf_free(wps);
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_INVALID_IE);
			return -1;
		}
		wpabuf_free(wps);
	}
#endif /* CONFIG_WPS_STRICT */

	/* Go through the IEs and make a copy of the MDIE, if present. */
	while (p && l >= 2) {
		len = p[1] + 2;
		if (len > l) {
			wpa_hexdump(MSG_DEBUG, "Truncated IE in assoc_info",
				    p, l);
			break;
		}
		if (p[0] == WLAN_EID_MOBILITY_DOMAIN &&
		    p[1] >= MOBILITY_DOMAIN_ID_LEN) {
			wpa_s->sme.ft_used = 1;
			os_memcpy(wpa_s->sme.mobility_domain, p + 2,
				  MOBILITY_DOMAIN_ID_LEN);
			break;
		}
		l -= len;
		p += len;
	}
#endif /* CONFIG_SME */

	/* Process FT when SME is in the driver */
	if (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME) &&
	    wpa_ft_is_completed(wpa_s->wpa)) {
		if (wpa_drv_get_bssid(wpa_s, bssid) < 0 ||
		    wpa_ft_validate_reassoc_resp(wpa_s->wpa,
						 data->assoc_info.resp_ies,
						 data->assoc_info.resp_ies_len,
						 bssid) < 0) {
			wpa_dbg(wpa_s, MSG_DEBUG, "FT: Validation of "
				"Reassociation Response failed");
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_INVALID_IE);
			return -1;
		}
		wpa_dbg(wpa_s, MSG_DEBUG, "FT: Reassociation Response done");
	}

	wpa_sm_set_ft_params(wpa_s->wpa, data->assoc_info.resp_ies,
			     data->assoc_info.resp_ies_len);
#endif /* CONFIG_IEEE80211R */

	/* WPA/RSN IE from Beacon/ProbeResp */
	p = data->assoc_info.beacon_ies;
	l = data->assoc_info.beacon_ies_len;

	/* Go through the IEs and make a copy of the WPA/RSN IEs, if present.
	 */
	wpa_found = rsn_found = 0;
	while (p && l >= 2) {
		len = p[1] + 2;
		if (len > l) {
			wpa_hexdump(MSG_DEBUG, "Truncated IE in beacon_ies",
				    p, l);
			break;
		}
		if (!wpa_found &&
		    p[0] == WLAN_EID_VENDOR_SPECIFIC && p[1] >= 6 &&
		    os_memcmp(&p[2], "\x00\x50\xF2\x01\x01\x00", 6) == 0) {
			wpa_found = 1;
			wpa_sm_set_ap_wpa_ie(wpa_s->wpa, p, len);
		}

		if (!rsn_found &&
		    p[0] == WLAN_EID_RSN && p[1] >= 2) {
			rsn_found = 1;
			wpa_sm_set_ap_rsn_ie(wpa_s->wpa, p, len);
		}

		if (p[0] == WLAN_EID_RSNX && p[1] >= 1)
			wpa_sm_set_ap_rsnxe(wpa_s->wpa, p, len);

		l -= len;
		p += len;
	}

	if (!wpa_found && data->assoc_info.beacon_ies)
		wpa_sm_set_ap_wpa_ie(wpa_s->wpa, NULL, 0);
	if (!rsn_found && data->assoc_info.beacon_ies) {
		wpa_sm_set_ap_rsn_ie(wpa_s->wpa, NULL, 0);
		wpa_sm_set_ap_rsnxe(wpa_s->wpa, NULL, 0);
	}
	if (wpa_found || rsn_found)
		wpa_s->ap_ies_from_associnfo = 1;

	if (wpa_s->assoc_freq && data->assoc_info.freq &&
	    wpa_s->assoc_freq != data->assoc_info.freq) {
		wpa_printf(MSG_DEBUG, "Operating frequency changed from "
			   "%u to %u MHz",
			   wpa_s->assoc_freq, data->assoc_info.freq);
		wpa_supplicant_update_scan_results(wpa_s);
	}

	wpa_s->assoc_freq = data->assoc_info.freq;

	return 0;
}


STATIC int wpa_supplicant_assoc_update_ie(struct wpa_supplicant *wpa_s)
{
	const u8 *bss_wpa = NULL, *bss_rsn = NULL, *bss_rsnx = NULL;

	if (!wpa_s->current_bss || !wpa_s->current_ssid)
		return -1;

	if (!wpa_key_mgmt_wpa_any(wpa_s->current_ssid->key_mgmt))
		return 0;

	bss_wpa = wpa_bss_get_vendor_ie(wpa_s->current_bss,
					WPA_IE_VENDOR_TYPE);
	bss_rsn = wpa_bss_get_ie(wpa_s->current_bss, WLAN_EID_RSN);
	bss_rsnx = wpa_bss_get_ie(wpa_s->current_bss, WLAN_EID_RSNX);

	if (wpa_sm_set_ap_wpa_ie(wpa_s->wpa, bss_wpa,
				 bss_wpa ? 2 + bss_wpa[1] : 0) ||
	    wpa_sm_set_ap_rsn_ie(wpa_s->wpa, bss_rsn,
				 bss_rsn ? 2 + bss_rsn[1] : 0) ||
	    wpa_sm_set_ap_rsnxe(wpa_s->wpa, bss_rsnx,
				 bss_rsnx ? 2 + bss_rsnx[1] : 0))
		return -1;

	return 0;
}


#ifdef CONFIG_FST
static void wpas_fst_update_mb_assoc(struct wpa_supplicant *wpa_s,
				     union wpa_event_data *data)
{
	struct assoc_info *ai = data ? &data->assoc_info : NULL;
	struct wpa_bss *bss = wpa_s->current_bss;
	const u8 *ieprb, *iebcn;

	wpabuf_free(wpa_s->received_mb_ies);
	wpa_s->received_mb_ies = NULL;

	if (ai &&
	    !wpas_fst_update_mbie(wpa_s, ai->resp_ies, ai->resp_ies_len)) {
		wpa_printf(MSG_DEBUG,
			   "FST: MB IEs updated from Association Response frame");
		return;
	}

	if (ai &&
	    !wpas_fst_update_mbie(wpa_s, ai->beacon_ies, ai->beacon_ies_len)) {
		wpa_printf(MSG_DEBUG,
			   "FST: MB IEs updated from association event Beacon IEs");
		return;
	}

	if (!bss)
		return;

	ieprb = (const u8 *) (bss + 1);
	iebcn = ieprb + bss->ie_len;

	if (!wpas_fst_update_mbie(wpa_s, ieprb, bss->ie_len))
		wpa_printf(MSG_DEBUG, "FST: MB IEs updated from bss IE");
	else if (!wpas_fst_update_mbie(wpa_s, iebcn, bss->beacon_ie_len))
		wpa_printf(MSG_DEBUG, "FST: MB IEs updated from bss beacon IE");
}
#endif /* CONFIG_FST */

static void  wpa_supplicant_event_assoc(struct wpa_supplicant *wpa_s,
				       union wpa_event_data *data)
{
	u8 bssid[ETH_ALEN];
	int ft_completed, already_authorized;
	int new_bss = 0;
#if defined(CONFIG_FILS) || defined(CONFIG_MBO)
          struct wpa_bss *bss;
#endif /*  CONFIG_FILS || CONFIG_MBO */
#ifdef RSI_ENABLE_CCX
	struct wpa_sm *wps = wpa_s->wpa;
	u8 cckm_completed = wps->wpa_cckm_completed;
#endif
#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
		if (!data)
			return;
		hostapd_notif_assoc(wpa_s->ap_iface->bss[0],
				    data->assoc_info.addr,
				    data->assoc_info.req_ies,
				    data->assoc_info.req_ies_len,
				    data->assoc_info.reassoc);
		return;
	}
#endif /* CONFIG_AP */
#if UNUSED_FEAT_IN_SUPP_29
	eloop_cancel_timeout(wpas_network_reenabled, wpa_s, NULL);
#endif
	ft_completed = wpa_ft_is_completed(wpa_s->wpa);
	if (data && wpa_supplicant_event_associnfo(wpa_s, data) < 0)
		return;
	/*
	 * FILS authentication can share the same mechanism to mark the
	 * connection fully authenticated, so set ft_completed also based on
	 * FILS result.
	 */
#ifdef CONFIG_FILS
	if (!ft_completed)
		ft_completed = wpa_fils_is_completed(wpa_s->wpa);
#endif

	if (wpa_drv_get_bssid(wpa_s, bssid) < 0) {
		wpa_dbg(wpa_s, MSG_ERROR, "Failed to get BSSID");
		wpa_supplicant_deauthenticate(
			wpa_s, WLAN_REASON_DEAUTH_LEAVING);
		return;
	}

	wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATED);
	if (os_memcmp(bssid, wpa_s->bssid, ETH_ALEN) != 0) {
		if (os_reltime_initialized(&wpa_s->session_start)) {
			os_reltime_age(&wpa_s->session_start,
				       &wpa_s->session_length);
			wpa_s->session_start.sec = 0;
			wpa_s->session_start.usec = 0;
			wpas_notify_session_length(wpa_s);
		} else {
			wpas_notify_auth_changed(wpa_s);
			os_get_reltime(&wpa_s->session_start);
		}
		wpa_dbg(wpa_s, MSG_DEBUG, "Associated to a new BSS: BSSID="
			MACSTR, MAC2STR(bssid));
		new_bss = 1;
		random_add_randomness(bssid, ETH_ALEN);
		os_memcpy(wpa_s->bssid, bssid, ETH_ALEN);
		os_memset(wpa_s->pending_bssid, 0, ETH_ALEN);
		wpas_notify_bssid_changed(wpa_s);

		if (wpa_supplicant_dynamic_keys(wpa_s) && !ft_completed
#ifdef RSI_ENABLE_CCX
				&& !cckm_completed
#endif
		   ) {
			wpa_clear_keys(wpa_s, bssid);
		}
		if (wpa_supplicant_select_config(wpa_s) < 0) {
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_DEAUTH_LEAVING);
			return;
		}
	}

	if (wpa_s->conf->ap_scan == 1 &&
	    wpa_s->drv_flags & WPA_DRIVER_FLAGS_BSS_SELECTION) {
		if (wpa_supplicant_assoc_update_ie(wpa_s) < 0 && new_bss)
			wpa_msg(wpa_s, MSG_WARNING,
				"WPA/RSN IEs not updated");
	}

#ifdef CONFIG_FST
	wpas_fst_update_mb_assoc(wpa_s, data);
#endif

#ifdef CONFIG_SME
	os_memcpy(wpa_s->sme.prev_bssid, bssid, ETH_ALEN);
	wpa_s->sme.prev_bssid_set = 1;
	wpa_s->sme.last_unprot_disconnect.sec = 0;
#endif /* CONFIG_SME */

	wpa_msg(wpa_s, MSG_INFO, "Associated with " MACSTR, MAC2STR(bssid));
	if (wpa_s->current_ssid) {
		/* When using scanning (ap_scan=1), SIM PC/SC interface can be
		 * initialized before association, but for other modes,
		 * initialize PC/SC here, if the current configuration needs
		 * smartcard or SIM/USIM. */
		wpa_supplicant_scard_init(wpa_s, wpa_s->current_ssid);
	}
	wpa_sm_notify_assoc(wpa_s->wpa, bssid);
	if (wpa_s->l2)
		l2_packet_notify_auth_start(wpa_s->l2);

	already_authorized = data && data->assoc_info.authorized;

	/*
	 * Set portEnabled first to FALSE in order to get EAP state machine out
	 * of the SUCCESS state and eapSuccess cleared. Without this, EAPOL PAE
	 * state machine may transit to AUTHENTICATING state based on obsolete
	 * eapSuccess and then trigger BE_AUTH to SUCCESS and PAE to
	 * AUTHENTICATED without ever giving chance to EAP state machine to
	 * reset the state.
	 */
#ifdef RSI_ENABLE_CCX
	        if (!ft_completed && !already_authorized && !cckm_completed) {
#else
	if (!ft_completed && !already_authorized) {
#endif		
		eapol_sm_notify_portEnabled(wpa_s->eapol, FALSE);
		eapol_sm_notify_portValid(wpa_s->eapol, FALSE);
	}
	if (wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt) ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_DPP ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_OWE || ft_completed ||
	    already_authorized)
		eapol_sm_notify_eap_success(wpa_s->eapol, FALSE);
	/* 802.1X::portControl = Auto */
	eapol_sm_notify_portEnabled(wpa_s->eapol, TRUE);
	wpa_s->eapol_received = 0;
	if (wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
	    wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE ||
	    (wpa_s->current_ssid &&
	     wpa_s->current_ssid->mode == WPAS_MODE_IBSS)) {
		if (wpa_s->current_ssid &&
		    wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE &&
		    (wpa_s->drv_flags &
		     WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC_DONE)) {
			/*
			 * Set the key after having received joined-IBSS event
			 * from the driver.
			 */
			wpa_supplicant_set_wpa_none_key(wpa_s,
							wpa_s->current_ssid);
		}
		wpa_supplicant_cancel_auth_timeout(wpa_s);
		wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
#ifdef RSI_ENABLE_CCX
		        } else if (!ft_completed && !cckm_completed) {
#else
	} else if (!ft_completed) {
#endif		
		/* Timeout for receiving the first EAPOL packet */
		wpa_supplicant_req_auth_timeout(wpa_s, 10, 0);
	}
	wpa_supplicant_cancel_scan(wpa_s);

	if (ft_completed) {
		/*
		 * FT protocol completed - make sure EAPOL state machine ends
		 * up in authenticated.
		 */
		wpa_supplicant_cancel_auth_timeout(wpa_s);
		wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
		eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
		eapol_sm_notify_eap_success(wpa_s->eapol, TRUE);
#ifdef RSI_ENABLE_CCX
	} else if (((wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE_8021X) &&
				wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt)) || cckm_completed) {
#else
	} else if ((wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE_PSK) &&
		   wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt)) {
#endif		
		/*
		 * We are done; the driver will take care of RSN 4-way
		 * handshake.
		 */
		wpa_supplicant_cancel_auth_timeout(wpa_s);
		wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
		eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
#ifdef RSI_ENABLE_CCX
		if (!cckm_completed)
#endif
		eapol_sm_notify_eap_success(wpa_s->eapol, TRUE);
	} else if ((wpa_s->drv_flags & WPA_DRIVER_FLAGS_4WAY_HANDSHAKE_8021X) &&
		   wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt)) {
		/*
		 * The driver will take care of RSN 4-way handshake, so we need
		 * to allow EAPOL supplicant to complete its work without
		 * waiting for WPA supplicant.
		 */
		eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
	}

	wpa_s->last_eapol_matches_bssid = 0;

	if (wpa_s->pending_eapol_rx) {
		struct os_reltime now, age;
		os_get_reltime(&now);
		os_reltime_sub(&now, &wpa_s->pending_eapol_rx_time, &age);
		if (age.sec == 0 && age.usec < 200000 &&
		    os_memcmp(wpa_s->pending_eapol_rx_src, bssid, ETH_ALEN) ==
		    0) {
			wpa_dbg(wpa_s, MSG_DEBUG, "Process pending EAPOL "
				"frame that was received just before "
				"association notification");
			wpa_supplicant_rx_eapol(
				wpa_s, wpa_s->pending_eapol_rx_src,
				wpabuf_head(wpa_s->pending_eapol_rx),
				wpabuf_len(wpa_s->pending_eapol_rx));
		}
		wpabuf_free(wpa_s->pending_eapol_rx);
		wpa_s->pending_eapol_rx = NULL;
	}

	if ((wpa_s->key_mgmt == WPA_KEY_MGMT_NONE ||
	     wpa_s->key_mgmt == WPA_KEY_MGMT_IEEE8021X_NO_WPA) &&
	    wpa_s->current_ssid &&
	    (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SET_KEYS_AFTER_ASSOC_DONE)) {
#ifndef CONFIG_NO_WEP	
		/* Set static WEP keys again */
		wpa_set_wep_keys(wpa_s, wpa_s->current_ssid);
#endif	
	}

#ifdef CONFIG_IBSS_RSN
	if (wpa_s->current_ssid &&
	    wpa_s->current_ssid->mode == WPAS_MODE_IBSS &&
	    wpa_s->key_mgmt != WPA_KEY_MGMT_NONE &&
	    wpa_s->key_mgmt != WPA_KEY_MGMT_WPA_NONE &&
	    wpa_s->ibss_rsn == NULL) {
		wpa_s->ibss_rsn = ibss_rsn_init(wpa_s, wpa_s->current_ssid);
		if (!wpa_s->ibss_rsn) {
			wpa_msg(wpa_s, MSG_INFO, "Failed to init IBSS RSN");
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_DEAUTH_LEAVING);
			return;
		}

		ibss_rsn_set_psk(wpa_s->ibss_rsn, wpa_s->current_ssid->psk);
	}
#endif /* CONFIG_IBSS_RSN */

	wpas_wps_notify_assoc(wpa_s, bssid);
#ifdef RSI_ENABLE_CCX
	if (cckm_completed)
	{
		wpa_install_cckm_newkeys(wpa_s->wpa, &wpa_s->gtk);
	}
#endif
#if  UNUSED_FEAT_IN_SUPP_29
	if (data) {
		wmm_ac_notify_assoc(wpa_s, data->assoc_info.resp_ies,
				    data->assoc_info.resp_ies_len,
				    &data->assoc_info.wmm_params);

		if (wpa_s->reassoc_same_bss)
			wmm_ac_restore_tspecs(wpa_s);
	}
#endif

#if defined(CONFIG_FILS) || defined(CONFIG_MBO)
		bss = wpa_bss_get_bssid(wpa_s, bssid);
#endif /*  CONFIG_FILS || CONFIG_MBO */

#ifdef CONFIG_FILS
	if (wpa_key_mgmt_fils(wpa_s->key_mgmt)) {
		const u8 *fils_cache_id = wpa_bss_get_fils_cache_id(bss);

		if (fils_cache_id)
			wpa_sm_set_fils_cache_id(wpa_s->wpa, fils_cache_id);
	}
#endif /* CONFIG_FILS */

#ifdef CONFIG_MBO
      wpas_mbo_check_pmf(wpa_s, bss, wpa_s->current_ssid);
#endif /*  CONFIG_MBO  */
}


STATIC int disconnect_reason_recoverable(u16 reason_code)
{
	return reason_code == WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY ||
		reason_code == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA ||
		reason_code == WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA;
}


STATIC void wpa_supplicant_event_disassoc(struct wpa_supplicant *wpa_s,
					  u16 reason_code,
					  int locally_generated)
{
	const u8 *bssid;

	if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
		/*
		 * At least Host AP driver and a Prism3 card seemed to be
		 * generating streams of disconnected events when configuring
		 * IBSS for WPA-None. Ignore them for now.
		 */
		return;
	}

	bssid = wpa_s->bssid;
	if (is_zero_ether_addr(bssid))
		bssid = wpa_s->pending_bssid;

	if (!is_zero_ether_addr(bssid) ||
	    wpa_s->wpa_state >= WPA_AUTHENTICATING) {
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_DISCONNECTED "bssid=" MACSTR
			" reason=%d%s",
			MAC2STR(bssid), reason_code,
			locally_generated ? " locally_generated=1" : "");
	}
}


static int could_be_psk_mismatch(struct wpa_supplicant *wpa_s, u16 reason_code,
				 int locally_generated)
{
	if (wpa_s->wpa_state != WPA_4WAY_HANDSHAKE ||
	    !wpa_key_mgmt_wpa_psk(wpa_s->key_mgmt))
		return 0; /* Not in 4-way handshake with PSK */

	/*
	 * It looks like connection was lost while trying to go through PSK
	 * 4-way handshake. Filter out known disconnection cases that are caused
	 * by something else than PSK mismatch to avoid confusing reports.
	 */

	if (locally_generated) {
		if (reason_code == WLAN_REASON_IE_IN_4WAY_DIFFERS)
			return 0;
	}

	return 1;
}


STATIC void wpa_supplicant_event_disassoc_finish(struct wpa_supplicant *wpa_s,
						 u16 reason_code,
						 int locally_generated)
{
	const u8 *bssid;
	int authenticating;
	u8 prev_pending_bssid[ETH_ALEN];
	struct wpa_bss *fast_reconnect = NULL;
	struct wpa_ssid *fast_reconnect_ssid = NULL;
	struct wpa_ssid *last_ssid;
	struct wpa_bss *curr = NULL;

	authenticating = wpa_s->wpa_state == WPA_AUTHENTICATING;
	os_memcpy(prev_pending_bssid, wpa_s->pending_bssid, ETH_ALEN);

	if (wpa_s->key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
		/*
		 * At least Host AP driver and a Prism3 card seemed to be
		 * generating streams of disconnected events when configuring
		 * IBSS for WPA-None. Ignore them for now.
		 */
		wpa_dbg(wpa_s, MSG_DEBUG, "Disconnect event - ignore in "
			"IBSS/WPA-None mode");
		return;
	}

	if (!wpa_s->disconnected && wpa_s->wpa_state >= WPA_AUTHENTICATING &&
	    reason_code == WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY &&
	    locally_generated)
		/*
		 * Remove the inactive AP (which is probably out of range) from
		 * the BSS list after marking disassociation. In particular
		 * mac80211-based drivers use the
		 * WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY reason code in
		 * locally generated disconnection events for cases where the
		 * AP does not reply anymore.
		 */
		curr = wpa_s->current_bss;

	if (could_be_psk_mismatch(wpa_s, reason_code, locally_generated)) {
		wpa_msg(wpa_s, MSG_INFO, "WPA: 4-Way Handshake failed - "
			"pre-shared key may be incorrect");
		if (wpas_p2p_4way_hs_failed(wpa_s) > 0)
			return; /* P2P group removed */
		wpas_auth_failed(wpa_s, "WRONG_KEY");
	}
	if (!wpa_s->disconnected &&
	    (!wpa_s->auto_reconnect_disabled ||
	     wpa_s->key_mgmt == WPA_KEY_MGMT_WPS ||
		 wpa_s->eap_expected_failure == 1 ||
	     wpas_wps_searching(wpa_s) ||
	     wpas_wps_reenable_networks_pending(wpa_s))) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Auto connect enabled: try to "
			"reconnect (wps=%d/%d wpa_state=%d)",
			wpa_s->key_mgmt == WPA_KEY_MGMT_WPS,
			wpas_wps_searching(wpa_s),
			wpa_s->wpa_state);
		if (wpa_s->wpa_state == WPA_COMPLETED &&
		    wpa_s->current_ssid &&
		    wpa_s->current_ssid->mode == WPAS_MODE_INFRA &&
		    !locally_generated &&
		    disconnect_reason_recoverable(reason_code)) {
			/*
			 * It looks like the AP has dropped association with
			 * us, but could allow us to get back in. Try to
			 * reconnect to the same BSS without full scan to save
			 * time for some common cases.
			 */
			fast_reconnect = wpa_s->current_bss;
			fast_reconnect_ssid = wpa_s->current_ssid;
		} else if (wpa_s->wpa_state >= WPA_ASSOCIATING)
			wpa_supplicant_req_scan(wpa_s, 0, 100000);
		else
			wpa_dbg(wpa_s, MSG_DEBUG, "Do not request new "
				"immediate scan");
	} else {
		wpa_dbg(wpa_s, MSG_DEBUG, "Auto connect disabled: do not "
			"try to re-connect");
		wpa_s->reassociate = 0;
		wpa_s->disconnected = 1;
		if (!wpa_s->pno)
			wpa_supplicant_cancel_sched_scan(wpa_s);
	}
	bssid = wpa_s->bssid;
	if (is_zero_ether_addr(bssid))
		bssid = wpa_s->pending_bssid;
	if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
		wpas_connection_failed(wpa_s, bssid);
	wpa_sm_notify_disassoc(wpa_s->wpa);
	if (locally_generated)
		wpa_s->disconnect_reason = -reason_code;
	else
		wpa_s->disconnect_reason = reason_code;
	wpas_notify_disconnect_reason(wpa_s);
	if (wpa_supplicant_dynamic_keys(wpa_s)) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Disconnect event - remove keys");
		wpa_clear_keys(wpa_s, wpa_s->bssid);
	}
	last_ssid = wpa_s->current_ssid;
	wpa_supplicant_mark_disassoc(wpa_s);

	if (curr)
		wpa_bss_remove(wpa_s, curr, "Connection to AP lost");

	if (authenticating && (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME)) {
		sme_disassoc_while_authenticating(wpa_s, prev_pending_bssid);
		wpa_s->current_ssid = last_ssid;
	}

	if (fast_reconnect &&
	    !wpas_network_disabled(wpa_s, fast_reconnect_ssid) &&
	    !disallowed_bssid(wpa_s, fast_reconnect->bssid) &&
	    !disallowed_ssid(wpa_s, fast_reconnect->ssid,
			     fast_reconnect->ssid_len) &&
	    !wpas_temp_disabled(wpa_s, fast_reconnect_ssid) &&
	    !wpa_is_bss_tmp_disallowed(wpa_s, fast_reconnect)) {
#ifndef CONFIG_NO_SCAN_PROCESSING
		wpa_dbg(wpa_s, MSG_DEBUG, "Try to reconnect to the same BSS");
		if (wpa_supplicant_connect(wpa_s, fast_reconnect,
					   fast_reconnect_ssid) < 0) {
			/* Recover through full scan */
			wpa_supplicant_req_scan(wpa_s, 0, 100000);
		}
#endif /* CONFIG_NO_SCAN_PROCESSING */
	} else if (fast_reconnect) {
		/*
		 * Could not reconnect to the same BSS due to network being
		 * disabled. Use a new scan to match the alternative behavior
		 * above, i.e., to continue automatic reconnection attempt in a
		 * way that enforces disabled network rules.
		 */
		wpa_supplicant_req_scan(wpa_s, 0, 100000);
	}
}


#ifdef CONFIG_DELAYED_MIC_ERROR_REPORT
void wpa_supplicant_delayed_mic_error_report(void *eloop_ctx, void *sock_ctx)
{
	struct wpa_supplicant *wpa_s = eloop_ctx;

	if (!wpa_s->pending_mic_error_report)
		return;

	wpa_dbg(wpa_s, MSG_DEBUG, "WPA: Sending pending MIC error report");
	wpa_sm_key_request(wpa_s->wpa, 1, wpa_s->pending_mic_error_pairwise);
	wpa_s->pending_mic_error_report = 0;
}
#endif /* CONFIG_DELAYED_MIC_ERROR_REPORT */


STATIC void
wpa_supplicant_event_michael_mic_failure(struct wpa_supplicant *wpa_s,
					 union wpa_event_data *data)
{
	int pairwise;
	struct os_reltime t;

	wpa_msg(wpa_s, MSG_WARNING, "Michael MIC failure detected");
	pairwise = (data && data->michael_mic_failure.unicast);
	os_get_reltime(&t);
	if ((wpa_s->last_michael_mic_error.sec &&
	     !os_reltime_expired(&t, &wpa_s->last_michael_mic_error, 60)) ||
	    wpa_s->pending_mic_error_report) {
		if (wpa_s->pending_mic_error_report) {
			/*
			 * Send the pending MIC error report immediately since
			 * we are going to start countermeasures and AP better
			 * do the same.
			 */
			wpa_sm_key_request(wpa_s->wpa, 1,
					   wpa_s->pending_mic_error_pairwise);
		}

		/* Send the new MIC error report immediately since we are going
		 * to start countermeasures and AP better do the same.
		 */
		wpa_sm_key_request(wpa_s->wpa, 1, pairwise);

		/* initialize countermeasures */
		wpa_s->countermeasures = 1;

		wpa_blacklist_add(wpa_s, wpa_s->bssid);

		wpa_msg(wpa_s, MSG_WARNING, "TKIP countermeasures started");

		/*
		 * Need to wait for completion of request frame. We do not get
		 * any callback for the message completion, so just wait a
		 * short while and hope for the best. */
		os_sleep(0, 10000);

		wpa_drv_set_countermeasures(wpa_s, 1);
		wpa_supplicant_deauthenticate(wpa_s,
					      WLAN_REASON_MICHAEL_MIC_FAILURE);
		eloop_cancel_timeout(wpa_supplicant_stop_countermeasures,
				     wpa_s, NULL);
		eloop_register_timeout(60, 0,
				       wpa_supplicant_stop_countermeasures,
				       wpa_s, NULL);
		/* TODO: mark the AP rejected for 60 second. STA is
		 * allowed to associate with another AP.. */
	} else {
#ifdef CONFIG_DELAYED_MIC_ERROR_REPORT
		if (wpa_s->mic_errors_seen) {
			/*
			 * Reduce the effectiveness of Michael MIC error
			 * reports as a means for attacking against TKIP if
			 * more than one MIC failure is noticed with the same
			 * PTK. We delay the transmission of the reports by a
			 * random time between 0 and 60 seconds in order to
			 * force the attacker wait 60 seconds before getting
			 * the information on whether a frame resulted in a MIC
			 * failure.
			 */
			u8 rval[4];
			int sec;

			if (os_get_random(rval, sizeof(rval)) < 0)
				sec = os_random() % 60;
			else
				sec = WPA_GET_BE32(rval) % 60;
			wpa_dbg(wpa_s, MSG_DEBUG, "WPA: Delay MIC error "
				"report %d seconds", sec);
			wpa_s->pending_mic_error_report = 1;
			wpa_s->pending_mic_error_pairwise = pairwise;
			eloop_cancel_timeout(
				wpa_supplicant_delayed_mic_error_report,
				wpa_s, NULL);
			eloop_register_timeout(
				sec, os_random() % 1000000,
				wpa_supplicant_delayed_mic_error_report,
				wpa_s, NULL);
		} else {
			wpa_sm_key_request(wpa_s->wpa, 1, pairwise);
		}
#else /* CONFIG_DELAYED_MIC_ERROR_REPORT */
		wpa_sm_key_request(wpa_s->wpa, 1, pairwise);
#endif /* CONFIG_DELAYED_MIC_ERROR_REPORT */
	}
	wpa_s->last_michael_mic_error = t;
	wpa_s->mic_errors_seen++;
}


#ifdef CONFIG_TERMINATE_ONLASTIF
static int any_interfaces(struct wpa_supplicant *head)
{
	struct wpa_supplicant *wpa_s;

	for (wpa_s = head; wpa_s != NULL; wpa_s = wpa_s->next)
		if (!wpa_s->interface_removed)
			return 1;
	return 0;
}
#endif /* CONFIG_TERMINATE_ONLASTIF */

#ifndef CONFIG_OPTIMIZE_CODE
static void
wpa_supplicant_event_interface_status(struct wpa_supplicant *wpa_s,
				      union wpa_event_data *data)
{
	if (os_strcmp(wpa_s->ifname, data->interface_status.ifname) != 0)
		return;

	switch (data->interface_status.ievent) {
	case EVENT_INTERFACE_ADDED:
		if (!wpa_s->interface_removed)
			break;
		wpa_s->interface_removed = 0;
		wpa_dbg(wpa_s, MSG_DEBUG, "Configured interface was added");
		if (wpa_supplicant_driver_init(wpa_s) < 0) {
			wpa_msg(wpa_s, MSG_INFO, "Failed to initialize the "
				"driver after interface was added");
		}

#ifdef CONFIG_P2P
		if (!wpa_s->global->p2p &&
		    !wpa_s->global->p2p_disabled &&
		    !wpa_s->conf->p2p_disabled &&
		    (wpa_s->drv_flags &
		     WPA_DRIVER_FLAGS_DEDICATED_P2P_DEVICE) &&
		    wpas_p2p_add_p2pdev_interface(
			    wpa_s, wpa_s->global->params.conf_p2p_dev) < 0) {
			wpa_printf(MSG_INFO,
				   "P2P: Failed to enable P2P Device interface");
			/* Try to continue without. P2P will be disabled. */
		}
#endif /* CONFIG_P2P */

		break;
	case EVENT_INTERFACE_REMOVED:
		wpa_dbg(wpa_s, MSG_DEBUG, "Configured interface was removed");
		wpa_s->interface_removed = 1;
		wpa_supplicant_mark_disassoc(wpa_s);
		wpa_supplicant_set_state(wpa_s, WPA_INTERFACE_DISABLED);
		l2_packet_deinit(wpa_s->l2);
		wpa_s->l2 = NULL;

#ifdef CONFIG_P2P
		if (wpa_s->global->p2p &&
		    wpa_s->global->p2p_init_wpa_s->parent == wpa_s &&
		    (wpa_s->drv_flags &
		     WPA_DRIVER_FLAGS_DEDICATED_P2P_DEVICE)) {
			wpa_dbg(wpa_s, MSG_DEBUG,
				"Removing P2P Device interface");
			wpa_supplicant_remove_iface(
				wpa_s->global, wpa_s->global->p2p_init_wpa_s,
				0);
			wpa_s->global->p2p_init_wpa_s = NULL;
		}
#endif /* CONFIG_P2P */

#ifdef CONFIG_MATCH_IFACE
		if (wpa_s->matched) {
			wpa_supplicant_remove_iface(wpa_s->global, wpa_s, 0);
			break;
		}
#endif /* CONFIG_MATCH_IFACE */

#ifdef CONFIG_TERMINATE_ONLASTIF
		/* check if last interface */
		if (!any_interfaces(wpa_s->global->ifaces))
			eloop_terminate();
#endif /* CONFIG_TERMINATE_ONLASTIF */
		break;
	}
}
#endif

#ifdef CONFIG_TDLS
static void wpa_supplicant_event_tdls(struct wpa_supplicant *wpa_s,
				      union wpa_event_data *data)
{
	if (data == NULL)
		return;
	switch (data->tdls.oper) {
	case TDLS_REQUEST_SETUP:
		wpa_tdls_remove(wpa_s->wpa, data->tdls.peer);
		if (wpa_tdls_is_external_setup(wpa_s->wpa))
			wpa_tdls_start(wpa_s->wpa, data->tdls.peer);
		else
			wpa_drv_tdls_oper(wpa_s, TDLS_SETUP, data->tdls.peer);
		break;
	case TDLS_REQUEST_TEARDOWN:
		if (wpa_tdls_is_external_setup(wpa_s->wpa))
			wpa_tdls_teardown_link(wpa_s->wpa, data->tdls.peer,
					       data->tdls.reason_code);
		else
			wpa_drv_tdls_oper(wpa_s, TDLS_TEARDOWN,
					  data->tdls.peer);
		break;
	case TDLS_REQUEST_DISCOVER:
			wpa_tdls_send_discovery_request(wpa_s->wpa,
							data->tdls.peer);
		break;
	}
}
#endif /* CONFIG_TDLS */


#ifdef CONFIG_WNM
static void wpa_supplicant_event_wnm(struct wpa_supplicant *wpa_s,
				     union wpa_event_data *data)
{
	if (data == NULL)
		return;
	switch (data->wnm.oper) {
	case WNM_OPER_SLEEP:
		wpa_printf(MSG_DEBUG, "Start sending WNM-Sleep Request "
			   "(action=%d, intval=%d)",
			   data->wnm.sleep_action, data->wnm.sleep_intval);
		ieee802_11_send_wnmsleep_req(wpa_s, data->wnm.sleep_action,
					     data->wnm.sleep_intval, NULL);
		break;
	}
}
#endif /* CONFIG_WNM */


#ifdef CONFIG_IEEE80211R
static void
wpa_supplicant_event_ft_response(struct wpa_supplicant *wpa_s,
				 union wpa_event_data *data)
{
	if (data == NULL)
		return;

	if (wpa_ft_process_response(wpa_s->wpa, data->ft_ies.ies,
				    data->ft_ies.ies_len,
				    data->ft_ies.ft_action,
				    data->ft_ies.target_ap,
				    data->ft_ies.ric_ies,
				    data->ft_ies.ric_ies_len) < 0) {
		/* TODO: prevent MLME/driver from trying to associate? */
	}
}
#endif /* CONFIG_IEEE80211R */


#ifdef CONFIG_IBSS_RSN
static void wpa_supplicant_event_ibss_rsn_start(struct wpa_supplicant *wpa_s,
						union wpa_event_data *data)
{
	struct wpa_ssid *ssid;
	if (wpa_s->wpa_state < WPA_ASSOCIATED)
		return;
	if (data == NULL)
		return;
	ssid = wpa_s->current_ssid;
	if (ssid == NULL)
		return;
	if (ssid->mode != WPAS_MODE_IBSS || !wpa_key_mgmt_wpa(ssid->key_mgmt))
		return;

	ibss_rsn_start(wpa_s->ibss_rsn, data->ibss_rsn_start.peer);
}


static void wpa_supplicant_event_ibss_auth(struct wpa_supplicant *wpa_s,
					   union wpa_event_data *data)
{
	struct wpa_ssid *ssid = wpa_s->current_ssid;

	if (ssid == NULL)
		return;

	/* check if the ssid is correctly configured as IBSS/RSN */
	if (ssid->mode != WPAS_MODE_IBSS || !wpa_key_mgmt_wpa(ssid->key_mgmt))
		return;

	ibss_rsn_handle_auth(wpa_s->ibss_rsn, data->rx_mgmt.frame,
			     data->rx_mgmt.frame_len);
}
#endif /* CONFIG_IBSS_RSN */


#ifdef CONFIG_IEEE80211R
void ft_rx_action(struct wpa_supplicant *wpa_s, const u8 *data,
			 size_t len)
{
	const u8 *sta_addr, *target_ap_addr;
	u16 status;

	wpa_hexdump(MSG_MSGDUMP, "FT: RX Action", data, len);
	if (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME))
		return; /* only SME case supported for now */
	if (len < 1 + 2 * ETH_ALEN + 2)
		return;
	if (data[0] != 2)
		return; /* Only FT Action Response is supported for now */
	sta_addr = data + 1;
	target_ap_addr = data + 1 + ETH_ALEN;
	status = WPA_GET_LE16(data + 1 + 2 * ETH_ALEN);
	wpa_dbg(wpa_s, MSG_DEBUG, "FT: Received FT Action Response: STA "
		MACSTR " TargetAP " MACSTR " status %u",
		MAC2STR(sta_addr), MAC2STR(target_ap_addr), status);

	if (os_memcmp(sta_addr, wpa_s->own_addr, ETH_ALEN) != 0) {
		wpa_dbg(wpa_s, MSG_DEBUG, "FT: Foreign STA Address " MACSTR
			" in FT Action Response", MAC2STR(sta_addr));
		return;
	}

	if (status) {
		wpa_dbg(wpa_s, MSG_DEBUG, "FT: FT Action Response indicates "
			"failure (status code %d)", status);
		/* TODO: report error to FT code(?) */
		return;
	}

	if (wpa_ft_process_response(wpa_s->wpa, data + 1 + 2 * ETH_ALEN + 2,
				    len - (1 + 2 * ETH_ALEN + 2), 1,
				    target_ap_addr, NULL, 0) < 0)
		return;

#ifdef CONFIG_SME
	{
		struct wpa_bss *bss;
		bss = wpa_bss_get_bssid(wpa_s, target_ap_addr);
		if (bss)
			wpa_s->sme.freq = bss->freq;
		wpa_s->sme.auth_alg = WPA_AUTH_ALG_FT;
		sme_associate(wpa_s, WPAS_MODE_INFRA, target_ap_addr,
			      WLAN_AUTH_FT);
	}
#endif /* CONFIG_SME */
}
#endif /* CONFIG_IEEE80211R */


STATIC void wpa_supplicant_event_unprot_deauth(struct wpa_supplicant *wpa_s,
					       struct unprot_deauth *e)
{
#ifdef CONFIG_IEEE80211W
	wpa_printf(MSG_DEBUG, "Unprotected Deauthentication frame "
		   "dropped: " MACSTR " -> " MACSTR
		   " (reason code %u)",
		   MAC2STR(e->sa), MAC2STR(e->da), e->reason_code);
	sme_event_unprot_disconnect(wpa_s, e->sa, e->da, e->reason_code);
#endif /* CONFIG_IEEE80211W */
}


STATIC void wpa_supplicant_event_unprot_disassoc(struct wpa_supplicant *wpa_s,
						 struct unprot_disassoc *e)
{
#ifdef CONFIG_IEEE80211W
	wpa_printf(MSG_DEBUG, "Unprotected Disassociation frame "
		   "dropped: " MACSTR " -> " MACSTR
		   " (reason code %u)",
		   MAC2STR(e->sa), MAC2STR(e->da), e->reason_code);
	sme_event_unprot_disconnect(wpa_s, e->sa, e->da, e->reason_code);
#endif /* CONFIG_IEEE80211W */
}


static void wpas_event_disconnect(struct wpa_supplicant *wpa_s, const u8 *addr,
				  u16 reason_code, int locally_generated,
				  const u8 *ie, size_t ie_len, int deauth)
{
#ifdef CONFIG_AP
	if (wpa_s->ap_iface && addr) {
		hostapd_notif_disassoc(wpa_s->ap_iface->bss[0], addr);
		return;
	}

	if (wpa_s->ap_iface) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Ignore deauth event in AP mode");
		return;
	}
#endif /* CONFIG_AP */

	if (!locally_generated)
		wpa_s->own_disconnect_req = 0;

	wpa_supplicant_event_disassoc(wpa_s, reason_code, locally_generated);

	if (((reason_code == WLAN_REASON_IEEE_802_1X_AUTH_FAILED ||
	      ((wpa_key_mgmt_wpa_ieee8021x(wpa_s->key_mgmt) ||
		(wpa_s->key_mgmt & WPA_KEY_MGMT_IEEE8021X_NO_WPA)) &&
	       eapol_sm_failed(wpa_s->eapol))) &&
	     !wpa_s->eap_expected_failure))
		wpas_auth_failed(wpa_s, "AUTH_FAILED");

#ifdef CONFIG_P2P
	if (deauth && reason_code > 0) {
		if (wpas_p2p_deauth_notif(wpa_s, addr, reason_code, ie, ie_len,
					  locally_generated) > 0) {
			/*
			 * The interface was removed, so cannot continue
			 * processing any additional operations after this.
			 */
			return;
		}
	}
#endif /* CONFIG_P2P */

	wpa_supplicant_event_disassoc_finish(wpa_s, reason_code,
					     locally_generated);
}


static void wpas_event_disassoc(struct wpa_supplicant *wpa_s,
				struct disassoc_info *info)
{
	u16 reason_code = 0;
	int locally_generated = 0;
	const u8 *addr = NULL;
	const u8 *ie = NULL;
	size_t ie_len = 0;

	wpa_dbg(wpa_s, MSG_DEBUG, "Disassociation notification");

	if (info) {
		addr = info->addr;
		ie = info->ie;
		ie_len = info->ie_len;
		reason_code = info->reason_code;
		locally_generated = info->locally_generated;
		wpa_dbg(wpa_s, MSG_DEBUG, " * reason %u (%s)%s", reason_code,
			reason2str(reason_code),
			locally_generated ? " locally_generated=1" : "");
		if (addr)
			wpa_dbg(wpa_s, MSG_DEBUG, " * address " MACSTR,
				MAC2STR(addr));
		wpa_hexdump(MSG_DEBUG, "Disassociation frame IE(s)",
			    ie, ie_len);
	}

#ifdef CONFIG_AP
	if (wpa_s->ap_iface && info && info->addr) {
		hostapd_notif_disassoc(wpa_s->ap_iface->bss[0], info->addr);
		return;
	}

	if (wpa_s->ap_iface) {
		wpa_dbg(wpa_s, MSG_DEBUG, "Ignore disassoc event in AP mode");
		return;
	}
#endif /* CONFIG_AP */

#ifdef CONFIG_P2P
	if (info) {
		wpas_p2p_disassoc_notif(
			wpa_s, info->addr, reason_code, info->ie, info->ie_len,
			locally_generated);
	}
#endif /* CONFIG_P2P */

	if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME)
		sme_event_disassoc(wpa_s, info);

	wpas_event_disconnect(wpa_s, addr, reason_code, locally_generated,
			      ie, ie_len, 0);
}

static void wpas_event_deauth(struct wpa_supplicant *wpa_s,
			      struct deauth_info *info)
{
	u16 reason_code = 0;
	int locally_generated = 0;
	const u8 *addr = NULL;
	const u8 *ie = NULL;
	size_t ie_len = 0;

	wpa_dbg(wpa_s, MSG_DEBUG, "Deauthentication notification");

	if (info) {
		addr = info->addr;
		ie = info->ie;
		ie_len = info->ie_len;
		reason_code = info->reason_code;
		locally_generated = info->locally_generated;
		wpa_dbg(wpa_s, MSG_DEBUG, " * reason %u (%s)%s",
			reason_code, reason2str(reason_code),
			locally_generated ? " locally_generated=1" : "");
		if (addr) {
			wpa_dbg(wpa_s, MSG_DEBUG, " * address " MACSTR,
				MAC2STR(addr));
		}
		wpa_hexdump(MSG_DEBUG, "Deauthentication frame IE(s)",
			    ie, ie_len);
	}

	wpa_reset_ft_completed(wpa_s->wpa);

	wpas_event_disconnect(wpa_s, addr, reason_code,
			      locally_generated, ie, ie_len, 1);
}

#if UNUSED_FEAT_IN_SUPP_29
static const char * reg_init_str(enum reg_change_initiator init)
{
	switch (init) {
	case REGDOM_SET_BY_CORE:
		return "CORE";
	case REGDOM_SET_BY_USER:
		return "USER";
	case REGDOM_SET_BY_DRIVER:
		return "DRIVER";
	case REGDOM_SET_BY_COUNTRY_IE:
		return "COUNTRY_IE";
	case REGDOM_BEACON_HINT:
		return "BEACON_HINT";
	}
	return "?";
}
#endif


#if UNUSED_FEAT_IN_SUPP_29
static const char * reg_type_str(enum reg_type type)
{
	switch (type) {
	case REGDOM_TYPE_UNKNOWN:
		return "UNKNOWN";
	case REGDOM_TYPE_COUNTRY:
		return "COUNTRY";
	case REGDOM_TYPE_WORLD:
		return "WORLD";
	case REGDOM_TYPE_CUSTOM_WORLD:
		return "CUSTOM_WORLD";
	case REGDOM_TYPE_INTERSECTION:
		return "INTERSECTION";
	}
	return "?";
}
#endif


#if UNUSED_FEAT_IN_SUPP_29
void wpa_supplicant_update_channel_list(struct wpa_supplicant *wpa_s,
					struct channel_list_changed *info)
{
	struct wpa_supplicant *ifs;
	u8 dfs_domain;

	/*
	 * To allow backwards compatibility with higher level layers that
	 * assumed the REGDOM_CHANGE event is sent over the initially added
	 * interface. Find the highest parent of this interface and use it to
	 * send the event.
	 */
	for (ifs = wpa_s; ifs->parent && ifs != ifs->parent; ifs = ifs->parent)
		;

	if (info) {
		wpa_msg(ifs, MSG_INFO,
			WPA_EVENT_REGDOM_CHANGE "init=%s type=%s%s%s",
			reg_init_str(info->initiator), reg_type_str(info->type),
			info->alpha2[0] ? " alpha2=" : "",
			info->alpha2[0] ? info->alpha2 : "");
	}

	if (wpa_s->drv_priv == NULL)
		return; /* Ignore event during drv initialization */

	dl_list_for_each(ifs, &wpa_s->radio->ifaces, struct wpa_supplicant,
			 radio_list) {
		wpa_printf(MSG_DEBUG, "%s: Updating hw mode",
			   ifs->ifname);
		free_hw_features(ifs);
		ifs->hw.modes = wpa_drv_get_hw_feature_data(
			ifs, &ifs->hw.num_modes, &ifs->hw.flags, &dfs_domain);

		/* Restart PNO/sched_scan with updated channel list */
		if (ifs->pno) {
			wpas_stop_pno(ifs);
			wpas_start_pno(ifs);
		} else if (ifs->sched_scanning && !ifs->pno_sched_pending) {
			wpa_dbg(ifs, MSG_DEBUG,
				"Channel list changed - restart sched_scan");
			wpas_scan_restart_sched_scan(ifs);
		}
	}

	wpas_p2p_update_channel_list(wpa_s, WPAS_P2P_CHANNEL_UPDATE_DRIVER);
}
#endif


static void wpas_event_rx_mgmt_action(struct wpa_supplicant *wpa_s,
				      const u8 *frame, size_t len, int freq,
				      int rssi)
{
	const struct ieee80211_mgmt *mgmt;
	const u8 *payload;
	size_t plen;
	u8 category;

	if (len < IEEE80211_HDRLEN + 2)
		return;

	mgmt = (const struct ieee80211_mgmt *) frame;
    payload = frame + IEEE80211_HDRLEN;
    plen = len - IEEE80211_HDRLEN - 1;
#ifdef SUPPLICANT_PORTING
    if(mgmt->frame_control & host_to_le16(WLAN_FC_ISWEP)){
    /* CCMP header remains intact in umac for packet number checks */
        payload += IEEE80211_CCMP_HDRLEN;
        plen -= IEEE80211_CCMP_HDRLEN;
    }
#endif
	category = *payload++;

	wpa_dbg(wpa_s, MSG_DEBUG, "Received Action frame: SA=" MACSTR
		" Category=%u DataLen=%d freq=%d MHz",
		MAC2STR(mgmt->sa), category, (int) plen, freq);
#if UNUSED_FEAT_IN_SUPP_29
	if (category == WLAN_ACTION_WMM) {
		wmm_ac_rx_action(wpa_s, mgmt->da, mgmt->sa, payload, plen);
		return;
	}
#endif
#ifdef CONFIG_IEEE80211R
	if (category == WLAN_ACTION_FT) {
		ft_rx_action(wpa_s, payload, plen);
		return;
	}
#endif /* CONFIG_IEEE80211R */

#ifdef CONFIG_IEEE80211W
#ifdef CONFIG_SME
	if (category == WLAN_ACTION_SA_QUERY) {
		sme_sa_query_rx(wpa_s, mgmt->sa, payload, plen);
		return;
	}
#endif /* CONFIG_SME */
#endif /* CONFIG_IEEE80211W */

#ifdef CONFIG_WNM
#ifdef SUPPLICANT_PORTING
	if (category == WLAN_ACTION_WNM)
#else
	if (mgmt->u.action.category == WLAN_ACTION_WNM)
#endif
    {
		ieee802_11_rx_wnm_action(wpa_s, mgmt, len);
		return;
	}
#endif /* CONFIG_WNM */

#ifdef CONFIG_GAS
	if ((mgmt->u.action.category == WLAN_ACTION_PUBLIC ||
	     mgmt->u.action.category == WLAN_ACTION_PROTECTED_DUAL) &&
	    gas_query_rx(wpa_s->gas, mgmt->da, mgmt->sa, mgmt->bssid,
			 mgmt->u.action.category,
			 payload, plen, freq) == 0)
		return;
#endif /* CONFIG_GAS */

#ifdef CONFIG_GAS_SERVER
	if ((mgmt->u.action.category == WLAN_ACTION_PUBLIC ||
	     mgmt->u.action.category == WLAN_ACTION_PROTECTED_DUAL) &&
	    gas_server_rx(wpa_s->gas_server, mgmt->da, mgmt->sa, mgmt->bssid,
			  mgmt->u.action.category,
			  payload, plen, freq) == 0)
		return;
#endif /* CONFIG_GAS_SERVER */

#ifdef CONFIG_TDLS
	if (category == WLAN_ACTION_PUBLIC && plen >= 4 &&
	    payload[0] == WLAN_TDLS_DISCOVERY_RESPONSE) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"TDLS: Received Discovery Response from " MACSTR,
			MAC2STR(mgmt->sa));
		return;
	}
#endif /* CONFIG_TDLS */

#ifdef CONFIG_INTERWORKING
	if (category == WLAN_ACTION_QOS && plen >= 1 &&
	    payload[0] == QOS_QOS_MAP_CONFIG) {
		const u8 *pos = payload + 1;
		size_t qlen = plen - 1;
		wpa_dbg(wpa_s, MSG_DEBUG, "Interworking: Received QoS Map Configure frame from "
			MACSTR, MAC2STR(mgmt->sa));
		if (os_memcmp(mgmt->sa, wpa_s->bssid, ETH_ALEN) == 0 &&
		    qlen > 2 && pos[0] == WLAN_EID_QOS_MAP_SET &&
		    pos[1] <= qlen - 2 && pos[1] >= 16)
			wpas_qos_map_set(wpa_s, pos + 2, pos[1]);
		return;
	}
#endif /* CONFIG_INTERWORKING */
#ifdef CHIP_9117
	if (category == WLAN_ACTION_RADIO_MEASUREMENT &&
	    payload[0] == WLAN_RRM_RADIO_MEASUREMENT_REQUEST) {
		wpas_rrm_handle_radio_measurement_request(wpa_s, mgmt->sa,
							  mgmt->da,
							  payload + 1,
							  plen - 1);
		return;
	}
#endif
#if UNUSED_FEAT_IN_SUPP_29
	if (category == WLAN_ACTION_RADIO_MEASUREMENT &&
	    payload[0] == WLAN_RRM_NEIGHBOR_REPORT_RESPONSE) {
		wpas_rrm_process_neighbor_rep(wpa_s, payload + 1, plen - 1);
		return;
	}

	if (category == WLAN_ACTION_RADIO_MEASUREMENT &&
	    payload[0] == WLAN_RRM_LINK_MEASUREMENT_REQUEST) {
		wpas_rrm_handle_link_measurement_request(wpa_s, mgmt->sa,
							 payload + 1, plen - 1,
							 rssi);
		return;
	}
#endif

#ifdef CONFIG_FST
	if (mgmt->u.action.category == WLAN_ACTION_FST && wpa_s->fst) {
		fst_rx_action(wpa_s->fst, mgmt, len);
		return;
	}
#endif /* CONFIG_FST */

#ifdef CONFIG_DPP
	if (category == WLAN_ACTION_PUBLIC && plen >= 5 &&
	    payload[0] == WLAN_PA_VENDOR_SPECIFIC &&
	    WPA_GET_BE24(&payload[1]) == OUI_WFA &&
	    payload[4] == DPP_OUI_TYPE) {
		payload++;
		plen--;
		wpas_dpp_rx_action(wpa_s, mgmt->sa, payload, plen, freq);
		return;
	}
#endif /* CONFIG_DPP */

	wpas_p2p_rx_action(wpa_s, mgmt->da, mgmt->sa, mgmt->bssid,
			   category, payload, plen, freq);
#ifdef CONFIG_MESH
	if (wpa_s->ifmsh)
		mesh_mpm_action_rx(wpa_s, mgmt, len);
#endif
}


#if UNUSED_FEAT_IN_SUPP_29
static void wpa_supplicant_notify_avoid_freq(struct wpa_supplicant *wpa_s,
					     union wpa_event_data *event)
{
	struct wpa_freq_range_list *list;
	char *str = NULL;

	list = &event->freq_range;

	if (list->num)
		str = freq_range_list_str(list);
	wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_AVOID_FREQ "ranges=%s",
		str ? str : "");

#ifdef CONFIG_P2P
	if (freq_range_list_parse(&wpa_s->global->p2p_go_avoid_freq, str)) {
		wpa_dbg(wpa_s, MSG_ERROR, "%s: Failed to parse freq range",
			__func__);
	} else {
		wpa_dbg(wpa_s, MSG_DEBUG, "P2P: Update channel list based on frequency avoid event");

		/*
		 * The update channel flow will also take care of moving a GO
		 * from the unsafe frequency if needed.
		 */
		wpas_p2p_update_channel_list(wpa_s,
					     WPAS_P2P_CHANNEL_UPDATE_AVOID);
	}
#endif /* CONFIG_P2P */

	os_free(str);
}
#endif


static void wpa_supplicant_event_port_authorized(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->wpa_state == WPA_ASSOCIATED) {
		wpa_supplicant_cancel_auth_timeout(wpa_s);
		wpa_supplicant_set_state(wpa_s, WPA_COMPLETED);
		eapol_sm_notify_portValid(wpa_s->eapol, TRUE);
		eapol_sm_notify_eap_success(wpa_s->eapol, TRUE);
	}
}


#if UNUSED_FEAT_IN_SUPP_29
static unsigned int wpas_event_cac_ms(const struct wpa_supplicant *wpa_s,
				      int freq)
{
	size_t i;
	int j;

	for (i = 0; i < wpa_s->hw.num_modes; i++) {
		const struct hostapd_hw_modes *mode = &wpa_s->hw.modes[i];

		for (j = 0; j < mode->num_channels; j++) {
			const struct hostapd_channel_data *chan;

			chan = &mode->channels[j];
			if (chan->freq == freq)
				return chan->dfs_cac_ms;
		}
	}

	return 0;
}


STATIC void wpas_event_dfs_cac_started(struct wpa_supplicant *wpa_s,
				       struct dfs_event *radar)
{
#if defined(NEED_AP_MLME) && defined(CONFIG_AP)
	if (wpa_s->ap_iface || wpa_s->ifmsh) {
		wpas_ap_event_dfs_cac_started(wpa_s, radar);
	} else
#endif /* NEED_AP_MLME && CONFIG_AP */
	{
		unsigned int cac_time = wpas_event_cac_ms(wpa_s, radar->freq);

		cac_time /= 1000; /* convert from ms to sec */
		if (!cac_time)
			cac_time = 10 * 60; /* max timeout: 10 minutes */

		/* Restart auth timeout: CAC time added to initial timeout */
		wpas_auth_timeout_restart(wpa_s, cac_time);
	}
}


STATIC void wpas_event_dfs_cac_finished(struct wpa_supplicant *wpa_s,
					struct dfs_event *radar)
{
#if defined(NEED_AP_MLME) && defined(CONFIG_AP)
	if (wpa_s->ap_iface || wpa_s->ifmsh) {
		wpas_ap_event_dfs_cac_finished(wpa_s, radar);
	} else
#endif /* NEED_AP_MLME && CONFIG_AP */
	{
		/* Restart auth timeout with original value after CAC is
		 * finished */
		wpas_auth_timeout_restart(wpa_s, 0);
	}
}


STATIC void wpas_event_dfs_cac_aborted(struct wpa_supplicant *wpa_s,
				       struct dfs_event *radar)
{
#if defined(NEED_AP_MLME) && defined(CONFIG_AP)
	if (wpa_s->ap_iface || wpa_s->ifmsh) {
		wpas_ap_event_dfs_cac_aborted(wpa_s, radar);
	} else
#endif /* NEED_AP_MLME && CONFIG_AP */
	{
		/* Restart auth timeout with original value after CAC is
		 * aborted */
		wpas_auth_timeout_restart(wpa_s, 0);
	}
}
#endif

#ifdef CONFIG_FILS
static void wpa_supplicant_event_assoc_auth(struct wpa_supplicant *wpa_s,
					    union wpa_event_data *data)
{
	wpa_dbg(wpa_s, MSG_DEBUG,
		"Connection authorized by device, previous state %d",
		wpa_s->wpa_state);

	wpa_supplicant_event_port_authorized(wpa_s);

	wpa_sm_set_rx_replay_ctr(wpa_s->wpa, data->assoc_info.key_replay_ctr);
	wpa_sm_set_ptk_kck_kek(wpa_s->wpa, data->assoc_info.ptk_kck,
			       data->assoc_info.ptk_kck_len,
			       data->assoc_info.ptk_kek,
			       data->assoc_info.ptk_kek_len);

	if (wpa_s->auth_alg == WPA_AUTH_ALG_FILS) {
		struct wpa_bss *bss = wpa_bss_get_bssid(wpa_s, wpa_s->bssid);
		const u8 *fils_cache_id = wpa_bss_get_fils_cache_id(bss);

		/* Update ERP next sequence number */
		eapol_sm_update_erp_next_seq_num(
			wpa_s->eapol, data->assoc_info.fils_erp_next_seq_num);

		if (data->assoc_info.fils_pmk && data->assoc_info.fils_pmkid) {
			/* Add the new PMK and PMKID to the PMKSA cache */
			wpa_sm_pmksa_cache_add(wpa_s->wpa,
					       data->assoc_info.fils_pmk,
					       data->assoc_info.fils_pmk_len,
					       data->assoc_info.fils_pmkid,
					       wpa_s->bssid, fils_cache_id);
		} else if (data->assoc_info.fils_pmkid) {
			/* Update the current PMKSA used for this connection */
			pmksa_cache_set_current(wpa_s->wpa,
						data->assoc_info.fils_pmkid,
						NULL, NULL, 0, NULL, 0);
		}
	}
}
#endif /* CONFIG_FILS */

static void wpas_event_assoc_reject(struct wpa_supplicant *wpa_s,
				    union wpa_event_data *data)
{
	const u8 *bssid = data->assoc_reject.bssid;
#ifdef CONFIG_MBO_STA
	struct wpa_bss *reject_bss;
#endif /* CONFIG_MBO_STA */

	if (!bssid || is_zero_ether_addr(bssid))
		bssid = wpa_s->pending_bssid;
#ifdef CONFIG_MBO_STA
	if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME)
		reject_bss = wpa_s->current_bss;
	else
		reject_bss = wpa_bss_get_bssid(wpa_s, bssid);
#endif /* CONFIG_MBO_STA */

	if (data->assoc_reject.bssid)
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_ASSOC_REJECT
			"bssid=" MACSTR	" status_code=%u%s%s%s",
			MAC2STR(data->assoc_reject.bssid),
			data->assoc_reject.status_code,
			data->assoc_reject.timed_out ? " timeout" : "",
			data->assoc_reject.timeout_reason ? "=" : "",
			data->assoc_reject.timeout_reason ?
			data->assoc_reject.timeout_reason : "");
	else
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_ASSOC_REJECT
			"status_code=%u%s%s%s",
			data->assoc_reject.status_code,
			data->assoc_reject.timed_out ? " timeout" : "",
			data->assoc_reject.timeout_reason ? "=" : "",
			data->assoc_reject.timeout_reason ?
			data->assoc_reject.timeout_reason : "");
	wpa_s->assoc_status_code = data->assoc_reject.status_code;
	wpas_notify_assoc_status_code(wpa_s);

#ifdef CONFIG_OWE
	if (data->assoc_reject.status_code ==
	    WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED &&
	    wpa_s->key_mgmt == WPA_KEY_MGMT_OWE &&
	    wpa_s->current_ssid &&
	    wpa_s->current_ssid->owe_group == 0 &&
	    wpa_s->last_owe_group != 21) {
		struct wpa_ssid *ssid = wpa_s->current_ssid;
		struct wpa_bss *bss = wpa_s->current_bss;

		if (!bss) {
			bss = wpa_supplicant_get_new_bss(wpa_s, bssid);
			if (!bss) {
				wpas_connection_failed(wpa_s, bssid);
				wpa_supplicant_mark_disassoc(wpa_s);
				return;
			}
		} 		 		
		wpa_printf(MSG_DEBUG, "OWE: Try next supported DH group");
		wpas_connect_work_done(wpa_s);
		wpa_supplicant_mark_disassoc(wpa_s);
		wpa_supplicant_connect(wpa_s, bss, ssid);
		return;
	}
#endif /* CONFIG_OWE */

#ifdef CONFIG_MBO_STA
	if (data->assoc_reject.status_code ==
	    WLAN_STATUS_DENIED_POOR_CHANNEL_CONDITIONS &&
	    reject_bss && data->assoc_reject.resp_ies) {
		const u8 *rssi_rej;

		rssi_rej = mbo_get_attr_from_ies(
			data->assoc_reject.resp_ies,
			data->assoc_reject.resp_ies_len,
			OCE_ATTR_ID_RSSI_BASED_ASSOC_REJECT);
		if (rssi_rej && rssi_rej[1] == 2) {
			wpa_printf(MSG_DEBUG,
				   "OCE: RSSI-based association rejection from "
				   MACSTR " (Delta RSSI: %u, Retry Delay: %u)",
				   MAC2STR(reject_bss->bssid),
				   rssi_rej[2], rssi_rej[3]);
			wpa_bss_tmp_disallow(wpa_s,
					     reject_bss->bssid,
					     rssi_rej[3],
					     rssi_rej[2] + reject_bss->level);
		}
	}
#endif /* CONFIG_MBO_STA */

	if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME) {
		sme_event_assoc_reject(wpa_s, data);
		return;
	}

	/* Driver-based SME cases */

#ifdef CONFIG_SAE
	if (wpa_s->current_ssid &&
	    wpa_key_mgmt_sae(wpa_s->current_ssid->key_mgmt) &&
	    !data->assoc_reject.timed_out) {
		wpa_dbg(wpa_s, MSG_DEBUG, "SAE: Drop PMKSA cache entry");
		wpa_sm_aborted_cached(wpa_s->wpa);
		wpa_sm_pmksa_cache_flush(wpa_s->wpa, wpa_s->current_ssid);
	}
#endif /* CONFIG_SAE */

#ifdef CONFIG_DPP
	if (wpa_s->current_ssid &&
	    wpa_s->current_ssid->key_mgmt == WPA_KEY_MGMT_DPP &&
	    !data->assoc_reject.timed_out) {
		wpa_dbg(wpa_s, MSG_DEBUG, "DPP: Drop PMKSA cache entry");
		wpa_sm_aborted_cached(wpa_s->wpa);
		wpa_sm_pmksa_cache_flush(wpa_s->wpa, wpa_s->current_ssid);
	}
#endif /* CONFIG_DPP */

#ifdef CONFIG_FILS
	/* Update ERP next sequence number */
	if (wpa_s->auth_alg == WPA_AUTH_ALG_FILS) {
		eapol_sm_update_erp_next_seq_num(
			wpa_s->eapol,
			data->assoc_reject.fils_erp_next_seq_num);
		fils_connection_failure(wpa_s);
	}
#endif /* CONFIG_FILS */

	wpas_connection_failed(wpa_s, bssid);
	wpa_supplicant_mark_disassoc(wpa_s);
}


void wpa_supplicant_event(void *ctx, enum wpa_event_type event,
			  union wpa_event_data *data)
{
#ifndef EXCLUDE_AP
	struct sta_info *sta_sae =NULL;
	u16 auth_transaction_sae1,status_code_sae1 ;
	struct ieee80211_mgmt *mgmt_sae1=NULL;
#endif

	struct wpa_supplicant *wpa_s = ctx;
#if UNUSED_FEAT_IN_SUPP_29
	int resched;
#endif
#ifndef CONFIG_NO_STDOUT_DEBUG
	int level = MSG_DEBUG;
#endif /* CONFIG_NO_STDOUT_DEBUG */

	if (!wpa_s)
		return;

	if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED &&
	    event != EVENT_INTERFACE_ENABLED &&
	    event != EVENT_INTERFACE_STATUS &&
	    event != EVENT_SCAN_RESULTS &&
	    event != EVENT_SCHED_SCAN_STOPPED) {
		wpa_dbg(wpa_s, MSG_DEBUG,
			"Ignore event %s (%d) while interface is disabled",
			event_to_string(event), event);
		return;
	}

#ifndef CONFIG_NO_STDOUT_DEBUG
	if (event == EVENT_RX_MGMT && data->rx_mgmt.frame_len >= 24) {
		const struct ieee80211_hdr *hdr;
		u16 fc;
		hdr = (const struct ieee80211_hdr *) data->rx_mgmt.frame;
		fc = le_to_host16(hdr->frame_control);
		if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
		    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_BEACON)
			level = MSG_EXCESSIVE;
	}

	wpa_dbg(wpa_s, level, "Event %s (%d) received",
		event_to_string(event), event);
#endif /* CONFIG_NO_STDOUT_DEBUG */

	switch (event) {
#ifndef CONFIG_OPTIMIZE_CODE	
	case EVENT_AUTH:
#ifdef CONFIG_FST
		if (!wpas_fst_update_mbie(wpa_s, data->auth.ies,
					  data->auth.ies_len))
			wpa_printf(MSG_DEBUG,
				   "FST: MB IEs updated from auth IE");
#endif /* CONFIG_FST */
		sme_event_auth(wpa_s, data);
		wpa_s->auth_status_code = data->auth.status_code;
		wpas_notify_auth_status_code(wpa_s);
		break;
#endif		
	case EVENT_ASSOC:
#ifdef CONFIG_TESTING_OPTIONS
		if (wpa_s->ignore_auth_resp) {
			wpa_printf(MSG_INFO,
				   "EVENT_ASSOC - ignore_auth_resp active!");
			break;
		}
		if (wpa_s->testing_resend_assoc) {
			wpa_printf(MSG_INFO,
				   "EVENT_DEAUTH - testing_resend_assoc");
			break;
		}
#endif /* CONFIG_TESTING_OPTIONS */
#ifdef CONFIG_IEEE80211W
      //os_memset(&sm_nonrom->igtk, 0, sizeof(sm_nonrom->igtk));
#endif /* CONFIG_IEEE80211W */
		wpa_supplicant_event_assoc(wpa_s, data);
		wpa_s->assoc_status_code = WLAN_STATUS_SUCCESS;
#ifdef CONFIG_FILS
		if (data &&
		    (data->assoc_info.authorized ||
		     (!(wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME) &&
		      wpa_fils_is_completed(wpa_s->wpa))))
			wpa_supplicant_event_assoc_auth(wpa_s, data);
#endif
		if (data) {
			wpa_msg(wpa_s, MSG_INFO,
				WPA_EVENT_SUBNET_STATUS_UPDATE "status=%u",
				data->assoc_info.subnet_status);
		}
		break;
	case EVENT_DISASSOC:
		wpas_event_disassoc(wpa_s,
				    data ? &data->disassoc_info : NULL);
		break;
	case EVENT_DEAUTH:
#ifdef CONFIG_TESTING_OPTIONS
		if (wpa_s->ignore_auth_resp) {
			wpa_printf(MSG_INFO,
				   "EVENT_DEAUTH - ignore_auth_resp active!");
			break;
		}
		if (wpa_s->testing_resend_assoc) {
			wpa_printf(MSG_INFO,
				   "EVENT_DEAUTH - testing_resend_assoc");
			break;
		}
#endif /* CONFIG_TESTING_OPTIONS */
		wpas_event_deauth(wpa_s,
				  data ? &data->deauth_info : NULL);
		break;
	case EVENT_MICHAEL_MIC_FAILURE:
		wpa_supplicant_event_michael_mic_failure(wpa_s, data);
		break;
#ifndef CONFIG_NO_SCAN_PROCESSING
	case EVENT_SCAN_STARTED:
		if (wpa_s->own_scan_requested ||
		    (data && !data->scan_info.external_scan)) {
			struct os_reltime diff;

			os_get_reltime(&wpa_s->scan_start_time);
			os_reltime_sub(&wpa_s->scan_start_time,
				       &wpa_s->scan_trigger_time, &diff);
			wpa_dbg(wpa_s, MSG_DEBUG, "Own scan request started a scan in %ld.%06ld seconds",
				diff.sec, diff.usec);
			wpa_s->own_scan_requested = 0;
			wpa_s->own_scan_running = 1;
			if (wpa_s->last_scan_req == MANUAL_SCAN_REQ &&
			    wpa_s->manual_scan_use_id) {
				wpa_msg_ctrl(wpa_s, MSG_INFO,
					     WPA_EVENT_SCAN_STARTED "id=%u",
					     wpa_s->manual_scan_id);
			} else {
				wpa_msg_ctrl(wpa_s, MSG_INFO,
					     WPA_EVENT_SCAN_STARTED);
			}
		} else {
			wpa_dbg(wpa_s, MSG_DEBUG, "External program started a scan");
			wpa_s->radio->external_scan_running = 1;
			wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_SCAN_STARTED);
		}
		break;
	case EVENT_SCAN_RESULTS:
#if UNUSED_FEAT_IN_SUPP_29

		if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED) {
			wpa_s->scan_res_handler = NULL;
			wpa_s->own_scan_running = 0;
			wpa_s->radio->external_scan_running = 0;
			wpa_s->last_scan_req = NORMAL_SCAN_REQ;
			break;
		}

		if (!(data && data->scan_info.external_scan) &&
		    os_reltime_initialized(&wpa_s->scan_start_time)) {
			struct os_reltime now, diff;
			os_get_reltime(&now);
			os_reltime_sub(&now, &wpa_s->scan_start_time, &diff);
			wpa_s->scan_start_time.sec = 0;
			wpa_s->scan_start_time.usec = 0;
			wpa_dbg(wpa_s, MSG_DEBUG, "Scan completed in %ld.%06ld seconds",
				diff.sec, diff.usec);
		}
		if (wpa_supplicant_event_scan_results(wpa_s, data))
			break; /* interface may have been removed */
		if (!(data && data->scan_info.external_scan))
		wpa_s->own_scan_running = 0;
		if (data && data->scan_info.nl_scan_event)
			wpa_s->radio->external_scan_running = 0;
		radio_work_check_next(wpa_s);
#else
	;
	int set_eap_okc = 0;
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_CHK_EAP_OKC, &set_eap_okc);

	// TODO: check if wpa_update_conf_okc could be used instead
	if (set_eap_okc) {
		wpa_s->conf->okc = 1;
	}
	wpa_supplicant_event_scan_results(wpa_s, data);

#if 0 //def CONFIG_P2P /* check the purpose of this code */
	if (wpa_s->global->p2p_cb_on_scan_complete && !wpa_s->global->p2p_disabled &&
	    wpa_s->global->p2p != NULL &&
	    wpa_s->wpa_state != WPA_AUTHENTICATING &&
	    wpa_s->wpa_state != WPA_ASSOCIATING) {
		wpa_s->global->p2p_cb_on_scan_complete = 0;
		
		if (P2P_OTHER_SCAN_COMPLETED(wpa_s->global->p2p) == 1) {
			wpa_dbg(wpa_s, MSG_DEBUG, "P2P: Pending P2P operation "
				"continued after scan result processing");
		}
	}
#endif /* CONFIG_P2P */
#endif	
		break;
#endif /* CONFIG_NO_SCAN_PROCESSING */
	case EVENT_ASSOCINFO:
		wpa_supplicant_event_associnfo(wpa_s, data);
		break;
#ifndef CONFIG_OPTIMIZE_CODE		
	case EVENT_INTERFACE_STATUS:
		wpa_supplicant_event_interface_status(wpa_s, data);
		break;
	case EVENT_PMKID_CANDIDATE:
		wpa_supplicant_event_pmkid_candidate(wpa_s, data);
		break;
#endif
#ifdef CONFIG_TDLS
	case EVENT_TDLS:
		wpa_supplicant_event_tdls(wpa_s, data);
		break;
#endif /* CONFIG_TDLS */
#ifdef CONFIG_WNM
	case EVENT_WNM:
		wpa_supplicant_event_wnm(wpa_s, data);
		break;
#endif /* CONFIG_WNM */
#ifdef CONFIG_IEEE80211R
	case EVENT_FT_RESPONSE:
		wpa_supplicant_event_ft_response(wpa_s, data);
		break;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IBSS_RSN
	case EVENT_IBSS_RSN_START:
		wpa_supplicant_event_ibss_rsn_start(wpa_s, data);
		break;
#endif /* CONFIG_IBSS_RSN */

	case EVENT_ASSOC_REJECT:
		wpas_event_assoc_reject(wpa_s, data);
		break;
#ifndef CONFIG_OPTIMIZE_CODE
	case EVENT_AUTH_TIMED_OUT:
		/* It is possible to get this event from earlier connection */
		if (wpa_s->current_ssid &&
		    wpa_s->current_ssid->mode == WPAS_MODE_MESH) {
			wpa_dbg(wpa_s, MSG_DEBUG,
				"Ignore AUTH_TIMED_OUT in mesh configuration");
			break;
		}
		if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME)
			sme_event_auth_timed_out(wpa_s, data);
		break;
	case EVENT_ASSOC_TIMED_OUT:
		/* It is possible to get this event from earlier connection */
		if (wpa_s->current_ssid &&
		    wpa_s->current_ssid->mode == WPAS_MODE_MESH) {
			wpa_dbg(wpa_s, MSG_DEBUG,
				"Ignore ASSOC_TIMED_OUT in mesh configuration");
			break;
		}
		if (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME)
			sme_event_assoc_timed_out(wpa_s, data);
		break;
#endif			
	case EVENT_TX_STATUS:
		wpa_dbg(wpa_s, MSG_DEBUG, "EVENT_TX_STATUS dst=" MACSTR
			" type=%d stype=%d",
			MAC2STR(data->tx_status.dst),
			data->tx_status.type, data->tx_status.stype);
#ifdef CONFIG_AP
		if (wpa_s->ap_iface == NULL) {
#ifdef CONFIG_OFFCHANNEL
			if (data->tx_status.type == WLAN_FC_TYPE_MGMT &&
			    data->tx_status.stype == WLAN_FC_STYPE_ACTION)
				offchannel_send_action_tx_status(
					wpa_s, data->tx_status.dst,
					data->tx_status.data,
					data->tx_status.data_len,
					data->tx_status.ack ?
					OFFCHANNEL_SEND_ACTION_SUCCESS :
					OFFCHANNEL_SEND_ACTION_NO_ACK);
#endif /* CONFIG_OFFCHANNEL */
			break;
		}
#endif /* CONFIG_AP */
#ifdef CONFIG_OFFCHANNEL
		wpa_dbg(wpa_s, MSG_DEBUG, "EVENT_TX_STATUS pending_dst="
			MACSTR, MAC2STR(wpa_s->p2pdev->pending_action_dst));
		/*
		 * Catch TX status events for Action frames we sent via group
		 * interface in GO mode, or via standalone AP interface.
		 * Note, wpa_s->p2pdev will be the same as wpa_s->parent,
		 * except when the primary interface is used as a GO interface
		 * (for drivers which do not have group interface concurrency)
		 */
		if (data->tx_status.type == WLAN_FC_TYPE_MGMT &&
		    data->tx_status.stype == WLAN_FC_STYPE_ACTION &&
		    os_memcmp(wpa_s->p2pdev->pending_action_dst,
			      data->tx_status.dst, ETH_ALEN) == 0) {
			offchannel_send_action_tx_status(
				wpa_s->p2pdev, data->tx_status.dst,
				data->tx_status.data,
				data->tx_status.data_len,
				data->tx_status.ack ?
				OFFCHANNEL_SEND_ACTION_SUCCESS :
				OFFCHANNEL_SEND_ACTION_NO_ACK);
			break;
		}
#endif /* CONFIG_OFFCHANNEL */
#ifdef CONFIG_AP
		switch (data->tx_status.type) {
		case WLAN_FC_TYPE_MGMT:
			ap_mgmt_tx_cb(wpa_s, data->tx_status.data,
				      data->tx_status.data_len,
				      data->tx_status.stype,
				      data->tx_status.ack);
			break;
		case WLAN_FC_TYPE_DATA:
			ap_tx_status(wpa_s, data->tx_status.dst,
				     data->tx_status.data,
				     data->tx_status.data_len,
				     data->tx_status.ack);
			break;
		}
#endif /* CONFIG_AP */
		break;
#ifdef CONFIG_AP
	case EVENT_EAPOL_TX_STATUS:
		ap_eapol_tx_status(wpa_s, data->eapol_tx_status.dst,
				   data->eapol_tx_status.data,
				   data->eapol_tx_status.data_len,
				   data->eapol_tx_status.ack);
		break;
	case EVENT_DRIVER_CLIENT_POLL_OK:
		ap_client_poll_ok(wpa_s, data->client_poll.addr);
		break;
	case EVENT_RX_FROM_UNKNOWN:
		if (wpa_s->ap_iface == NULL)
			break;
		ap_rx_from_unknown_sta(wpa_s, data->rx_from_unknown.addr,
				       data->rx_from_unknown.wds);
		break;
#endif /* CONFIG_AP */
#ifdef NEED_AP_MLME
	case EVENT_CH_SWITCH_STARTED:
	case EVENT_CH_SWITCH:
		if (!data || !wpa_s->current_ssid)
			break;

		wpa_msg(wpa_s, MSG_INFO,
			"%sfreq=%d ht_enabled=%d ch_offset=%d ch_width=%s cf1=%d cf2=%d",
			event == EVENT_CH_SWITCH ? WPA_EVENT_CHANNEL_SWITCH :
			WPA_EVENT_CHANNEL_SWITCH_STARTED,
			data->ch_switch.freq,
			data->ch_switch.ht_enabled,
			data->ch_switch.ch_offset,
			channel_width_to_string(data->ch_switch.ch_width),
			data->ch_switch.cf1,
			data->ch_switch.cf2);
		if (event == EVENT_CH_SWITCH_STARTED)
			break;

		wpa_s->assoc_freq = data->ch_switch.freq;
		wpa_s->current_ssid->frequency = data->ch_switch.freq;
#endif

#ifdef CONFIG_AP
		if (wpa_s->current_ssid->mode == WPAS_MODE_AP ||
		    wpa_s->current_ssid->mode == WPAS_MODE_P2P_GO ||
		    wpa_s->current_ssid->mode == WPAS_MODE_MESH ||
		    wpa_s->current_ssid->mode ==
		    WPAS_MODE_P2P_GROUP_FORMATION) {
			wpas_ap_ch_switch(wpa_s, data->ch_switch.freq,
					  data->ch_switch.ht_enabled,
					  data->ch_switch.ch_offset,
					  data->ch_switch.ch_width,
					  data->ch_switch.cf1,
					  data->ch_switch.cf2,
					  1);
		}
#endif /* CONFIG_AP */

#ifdef CONFIG_IEEE80211W
		sme_event_ch_switch(wpa_s);
#endif /* CONFIG_IEEE80211W */
		wpas_p2p_update_channel_list(wpa_s, WPAS_P2P_CHANNEL_UPDATE_CS);
		wnm_clear_coloc_intf_reporting(wpa_s);
		break;
#ifdef CONFIG_AP
#ifdef NEED_AP_MLME
	case EVENT_DFS_RADAR_DETECTED:
		if (data)
			wpas_ap_event_dfs_radar_detected(wpa_s,
							 &data->dfs_event);
		break;
	case EVENT_DFS_NOP_FINISHED:
		if (data)
			wpas_ap_event_dfs_cac_nop_finished(wpa_s,
							   &data->dfs_event);
		break;
#endif /* NEED_AP_MLME */
#endif /* CONFIG_AP */
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_DFS_CAC_STARTED:
		if (data)
			wpas_event_dfs_cac_started(wpa_s, &data->dfs_event);
		break;
	case EVENT_DFS_CAC_FINISHED:
		if (data)
			wpas_event_dfs_cac_finished(wpa_s, &data->dfs_event);
		break;
	case EVENT_DFS_CAC_ABORTED:
		if (data)
			wpas_event_dfs_cac_aborted(wpa_s, &data->dfs_event);
		break;
#endif
	case EVENT_RX_MGMT: {
		u16 fc, stype;
		const struct ieee80211_mgmt *mgmt;

#ifdef CONFIG_TESTING_OPTIONS
		if (wpa_s->ext_mgmt_frame_handling) {
			struct rx_mgmt *rx = &data->rx_mgmt;
			size_t hex_len = 2 * rx->frame_len + 1;
			char *hex = os_malloc(hex_len);
			if (hex) {
				wpa_snprintf_hex(hex, hex_len,
						 rx->frame, rx->frame_len);
				wpa_msg(wpa_s, MSG_INFO, "MGMT-RX freq=%d datarate=%u ssi_signal=%d %s",
					rx->freq, rx->datarate, rx->ssi_signal,
					hex);
				os_free(hex);
			}
			break;
		}
#endif /* CONFIG_TESTING_OPTIONS */

		mgmt = (const struct ieee80211_mgmt *)
			data->rx_mgmt.frame;
		fc = le_to_host16(mgmt->frame_control);
		stype = WLAN_FC_GET_STYPE(fc);

#ifdef CONFIG_AP
		if (wpa_s->ap_iface == NULL) {
#endif /* CONFIG_AP */
#ifdef CONFIG_P2P
			if (stype == WLAN_FC_STYPE_PROBE_REQ &&
			    data->rx_mgmt.frame_len > IEEE80211_HDRLEN) {
				const u8 *src = mgmt->sa;
				const u8 *ie;
				size_t ie_len;

				ie = data->rx_mgmt.frame + IEEE80211_HDRLEN;
				ie_len = data->rx_mgmt.frame_len -
					IEEE80211_HDRLEN;
				wpas_p2p_probe_req_rx(
					wpa_s, src, mgmt->da,
					mgmt->bssid, ie, ie_len,
					data->rx_mgmt.freq,
					data->rx_mgmt.ssi_signal);
				break;
			}
#endif /* CONFIG_P2P */
#ifdef CONFIG_IBSS_RSN
			if (wpa_s->current_ssid &&
			    wpa_s->current_ssid->mode == WPAS_MODE_IBSS &&
			    stype == WLAN_FC_STYPE_AUTH &&
			    data->rx_mgmt.frame_len >= 30) {
				wpa_supplicant_event_ibss_auth(wpa_s, data);
				break;
			}
#endif /* CONFIG_IBSS_RSN */

			if (stype == WLAN_FC_STYPE_ACTION) {
				wpas_event_rx_mgmt_action(
					wpa_s, data->rx_mgmt.frame,
					data->rx_mgmt.frame_len,
					data->rx_mgmt.freq,
					data->rx_mgmt.ssi_signal);
				break;
			}

#ifdef CONFIG_MESH
			if (wpa_s->ifmsh) {
				mesh_mpm_mgmt_rx(wpa_s, &data->rx_mgmt);
				break;
			}
#endif

#ifdef CONFIG_SAE
			if (stype == WLAN_FC_STYPE_AUTH &&
			    !(wpa_s->drv_flags & WPA_DRIVER_FLAGS_SME) &&
			    (wpa_s->drv_flags & WPA_DRIVER_FLAGS_SAE)) {
				sme_external_auth_mgmt_rx(
					wpa_s, data->rx_mgmt.frame,
					data->rx_mgmt.frame_len);
				break;
			}
#endif /* CONFIG_SAE */
			wpa_dbg(wpa_s, MSG_DEBUG, "AP: ignore received "
				"management frame in non-AP mode");
			break;
#ifdef CONFIG_AP
		}

		if (stype == WLAN_FC_STYPE_PROBE_REQ &&
		    data->rx_mgmt.frame_len > IEEE80211_HDRLEN) {
			const u8 *ie;
			size_t ie_len;

			ie = data->rx_mgmt.frame + IEEE80211_HDRLEN;
			ie_len = data->rx_mgmt.frame_len - IEEE80211_HDRLEN;

			wpas_notify_preq(wpa_s, mgmt->sa, mgmt->da,
					 mgmt->bssid, ie, ie_len,
					 data->rx_mgmt.ssi_signal);
		}

		ap_mgmt_rx(wpa_s, &data->rx_mgmt);
#endif /* CONFIG_AP */
		break;
		}
	case EVENT_RX_PROBE_REQ:
		if (data->rx_probe_req.sa == NULL ||
		    data->rx_probe_req.ie == NULL)
			break;
#ifdef CONFIG_AP
		if (wpa_s->ap_iface) {
			hostapd_probe_req_rx(wpa_s->ap_iface->bss[0],
					     data->rx_probe_req.sa,
					     data->rx_probe_req.da,
					     data->rx_probe_req.bssid,
					     data->rx_probe_req.ie,
					     data->rx_probe_req.ie_len,
					     data->rx_probe_req.ssi_signal);
			break;
		}
#endif /* CONFIG_AP */
		wpas_p2p_probe_req_rx(wpa_s, data->rx_probe_req.sa,
				      data->rx_probe_req.da,
				      data->rx_probe_req.bssid,
				      data->rx_probe_req.ie,
				      data->rx_probe_req.ie_len,
				      0,
				      data->rx_probe_req.ssi_signal);
		break;
	case EVENT_REMAIN_ON_CHANNEL:
#ifdef CONFIG_OFFCHANNEL
		offchannel_remain_on_channel_cb(
			wpa_s, data->remain_on_channel.freq,
			data->remain_on_channel.duration);
#endif /* CONFIG_OFFCHANNEL */
		wpas_p2p_remain_on_channel_cb(
			wpa_s, data->remain_on_channel.freq,
			data->remain_on_channel.duration);
		break;
	case EVENT_CANCEL_REMAIN_ON_CHANNEL:
#ifdef CONFIG_OFFCHANNEL
		offchannel_cancel_remain_on_channel_cb(
			wpa_s, data->remain_on_channel.freq);
#endif /* CONFIG_OFFCHANNEL */
		wpas_p2p_cancel_remain_on_channel_cb(
			wpa_s, data->remain_on_channel.freq);
#ifdef CONFIG_DPP
		wpas_dpp_cancel_remain_on_channel_cb(
			wpa_s, data->remain_on_channel.freq);
#endif /* CONFIG_DPP */
		break;
	case EVENT_EAPOL_RX:
		wpa_supplicant_rx_eapol(wpa_s, data->eapol_rx.src,
					data->eapol_rx.data,
					data->eapol_rx.data_len);
		break;
	case EVENT_SIGNAL_CHANGE:
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_SIGNAL_CHANGE
			"above=%d signal=%d noise=%d txrate=%d",
			data->signal_change.above_threshold,
			data->signal_change.current_signal,
			data->signal_change.current_noise,
			data->signal_change.current_txrate);
		wpa_bss_update_level(wpa_s->current_bss,
				     data->signal_change.current_signal);
		bgscan_notify_signal_change(
			wpa_s, data->signal_change.above_threshold,
			data->signal_change.current_signal,
			data->signal_change.current_noise,
			data->signal_change.current_txrate);
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_INTERFACE_MAC_CHANGED:
		wpa_supplicant_update_mac_addr(wpa_s);
		break;
	case EVENT_INTERFACE_ENABLED:
		wpa_dbg(wpa_s, MSG_DEBUG, "Interface was enabled");
		if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED) {
			wpa_supplicant_update_mac_addr(wpa_s);
			wpa_supplicant_set_default_scan_ies(wpa_s);
			if (wpa_s->p2p_mgmt) {
				wpa_supplicant_set_state(wpa_s,
							 WPA_DISCONNECTED);
				break;
			}
#ifdef CONFIG_AP
			if (!wpa_s->ap_iface) {
				wpa_supplicant_set_state(wpa_s,
							 WPA_DISCONNECTED);
				wpa_s->scan_req = NORMAL_SCAN_REQ;
				wpa_supplicant_req_scan(wpa_s, 0, 0);
			} else
				wpa_supplicant_set_state(wpa_s,
							 WPA_COMPLETED);
#else /* CONFIG_AP */
			wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
			wpa_supplicant_req_scan(wpa_s, 0, 0);
#endif /* CONFIG_AP */
		}
		break;
#endif
	case EVENT_INTERFACE_DISABLED:
		wpa_dbg(wpa_s, MSG_DEBUG, "Interface was disabled");
#ifdef CONFIG_P2P
		if (wpa_s->p2p_group_interface == P2P_GROUP_INTERFACE_GO ||
		    (wpa_s->current_ssid && wpa_s->current_ssid->p2p_group &&
		     wpa_s->current_ssid->mode == WPAS_MODE_P2P_GO)) {
			/*
			 * Mark interface disabled if this happens to end up not
			 * being removed as a separate P2P group interface.
			 */
			wpa_supplicant_set_state(wpa_s, WPA_INTERFACE_DISABLED);
			/*
			 * The interface was externally disabled. Remove
			 * it assuming an external entity will start a
			 * new session if needed.
			 */
			if (wpa_s->current_ssid &&
			    wpa_s->current_ssid->p2p_group)
				wpas_p2p_interface_unavailable(wpa_s);
			else
				wpas_p2p_disconnect(wpa_s);
			/*
			 * wpa_s instance may have been freed, so must not use
			 * it here anymore.
			 */
			break;
		}
		if (wpa_s->p2p_scan_work && wpa_s->global->p2p &&
		    p2p_in_progress(wpa_s->global->p2p) > 1) {
			/* This radio work will be cancelled, so clear P2P
			 * state as well.
			 */
			p2p_stop_find(wpa_s->global->p2p);
		}
#endif /* CONFIG_P2P */

		if (wpa_s->wpa_state >= WPA_AUTHENTICATING) {
			/*
			 * Indicate disconnection to keep ctrl_iface events
			 * consistent.
			 */
			wpa_supplicant_event_disassoc(
				wpa_s, WLAN_REASON_DEAUTH_LEAVING, 1);
		}
		wpa_supplicant_mark_disassoc(wpa_s);
		wpa_bss_flush(wpa_s);
#ifdef ENABLE_RRM
		radio_remove_works(wpa_s, NULL, 0);
#endif

		wpa_supplicant_set_state(wpa_s, WPA_INTERFACE_DISABLED);
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_CHANNEL_LIST_CHANGED:
		wpa_supplicant_update_channel_list(
			wpa_s, &data->channel_list_changed);
		break;
#endif
	case EVENT_INTERFACE_UNAVAILABLE:
		wpas_p2p_interface_unavailable(wpa_s);
		break;
	case EVENT_BEST_CHANNEL:
		wpa_dbg(wpa_s, MSG_DEBUG, "Best channel event received "
			"(%d %d %d)",
			data->best_chan.freq_24, data->best_chan.freq_5,
			data->best_chan.freq_overall);
		wpa_s->best_24_freq = data->best_chan.freq_24;
		wpa_s->best_5_freq = data->best_chan.freq_5;
		wpa_s->best_overall_freq = data->best_chan.freq_overall;
		wpas_p2p_update_best_channels(wpa_s, data->best_chan.freq_24,
					      data->best_chan.freq_5,
					      data->best_chan.freq_overall);
		break;
	case EVENT_UNPROT_DEAUTH:
		wpa_supplicant_event_unprot_deauth(wpa_s,
						   &data->unprot_deauth);
		break;
	case EVENT_UNPROT_DISASSOC:
		wpa_supplicant_event_unprot_disassoc(wpa_s,
						     &data->unprot_disassoc);
		break;
	case EVENT_STATION_LOW_ACK:
#ifdef CONFIG_AP
		if (wpa_s->ap_iface && data)
			hostapd_event_sta_low_ack(wpa_s->ap_iface->bss[0],
						  data->low_ack.addr);
#endif /* CONFIG_AP */
#ifdef CONFIG_TDLS
		if (data)
			wpa_tdls_disable_unreachable_link(wpa_s->wpa,
							  data->low_ack.addr);
#endif /* CONFIG_TDLS */
		break;
	case EVENT_IBSS_PEER_LOST:
#ifdef CONFIG_IBSS_RSN
		ibss_rsn_stop(wpa_s->ibss_rsn, data->ibss_peer_lost.peer);
#endif /* CONFIG_IBSS_RSN */
		break;
	case EVENT_DRIVER_GTK_REKEY:
		if (os_memcmp(data->driver_gtk_rekey.bssid,
			      wpa_s->bssid, ETH_ALEN))
			break;
		if (!wpa_s->wpa)
			break;
		wpa_sm_update_replay_ctr(wpa_s->wpa,
					 data->driver_gtk_rekey.replay_ctr);
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_SCHED_SCAN_STOPPED:
		wpa_s->sched_scanning = 0;
		resched = wpa_s->scanning && wpas_scan_scheduled(wpa_s);
		wpa_supplicant_notify_scanning(wpa_s, 0);

		if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED)
			break;

		/*
		 * If the driver stopped scanning without being requested to,
		 * request a new scan to continue scanning for networks.
		 */
		if (!wpa_s->sched_scan_stop_req &&
		    wpa_s->wpa_state == WPA_SCANNING) {
			wpa_dbg(wpa_s, MSG_DEBUG,
				"Restart scanning after unexpected sched_scan stop event");
			wpa_supplicant_req_scan(wpa_s, 1, 0);
			break;
		}

		wpa_s->sched_scan_stop_req = 0;

		/*
		 * Start a new sched scan to continue searching for more SSIDs
		 * either if timed out or PNO schedule scan is pending.
		 */
		if (wpa_s->sched_scan_timed_out) {
			wpa_supplicant_req_sched_scan(wpa_s);
		} else if (wpa_s->pno_sched_pending) {
			wpa_s->pno_sched_pending = 0;
			wpas_start_pno(wpa_s);
		} else if (resched) {
			wpa_supplicant_req_scan(wpa_s, 0, 0);
		}

		break;
#endif
	case EVENT_WPS_BUTTON_PUSHED:
#ifdef CONFIG_WPS
		wpas_wps_start_pbc(wpa_s, NULL, 0, 0);
#endif /* CONFIG_WPS */
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_AVOID_FREQUENCIES:
		wpa_supplicant_notify_avoid_freq(wpa_s, data);
		break;
	case EVENT_CONNECT_FAILED_REASON:
#ifdef CONFIG_AP
		if (!wpa_s->ap_iface || !data)
			break;
		hostapd_event_connect_failed_reason(
			wpa_s->ap_iface->bss[0],
			data->connect_failed_reason.addr,
			data->connect_failed_reason.code);
#endif /* CONFIG_AP */
		break;
#endif
	case EVENT_NEW_PEER_CANDIDATE:
#ifdef CONFIG_MESH
		if (!wpa_s->ifmsh || !data)
			break;
		wpa_mesh_notify_peer(wpa_s, data->mesh_peer.peer,
				     data->mesh_peer.ies,
				     data->mesh_peer.ie_len);
#endif /* CONFIG_MESH */
#ifdef SUPPLICANT_PORTING
#ifndef EXCLUDE_AP
		mgmt_sae1= (struct ieee80211_mgmt *)(data->rx_mgmt.frame) ;
		auth_transaction_sae1 = le_to_host16(mgmt_sae1->u.auth.auth_transaction);
		status_code_sae1 = le_to_host16(mgmt_sae1->u.auth.status_code);
		if(mgmt_sae1->u.auth.auth_transaction == 1){
			sta_sae = ap_sta_add((struct hostapd_data *)(wpa_s->ap_iface->bss[0]), mgmt_sae1->sa);
			sta_sae->added_unassoc = 1;
		}
		else if(mgmt_sae1->u.auth.auth_transaction == 2){
			sta_sae = ap_get_sta((struct hostapd_data *)(wpa_s->ap_iface->bss[0]), mgmt_sae1->sa);
		}
		handle_auth_sae((struct hostapd_data *)(wpa_s->ap_iface->bss[0]), sta_sae, mgmt_sae1, data->rx_mgmt.frame_len, auth_transaction_sae1,
				status_code_sae1);
#endif
#endif
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_SURVEY:
#ifdef CONFIG_AP
		if (!wpa_s->ap_iface)
			break;
		hostapd_event_get_survey(wpa_s->ap_iface,
					 &data->survey_results);
#endif /* CONFIG_AP */
		break;
#endif
	case EVENT_ACS_CHANNEL_SELECTED:
#ifdef CONFIG_AP
#ifdef CONFIG_ACS
		if (!wpa_s->ap_iface)
			break;
		hostapd_acs_channel_selected(wpa_s->ap_iface->bss[0],
					     &data->acs_selected_channels);
#endif /* CONFIG_ACS */
#endif /* CONFIG_AP */
		break;
	case EVENT_P2P_LO_STOP:
#ifdef CONFIG_P2P
		wpa_s->p2p_lo_started = 0;
		wpa_msg(wpa_s, MSG_INFO, P2P_EVENT_LISTEN_OFFLOAD_STOP
			P2P_LISTEN_OFFLOAD_STOP_REASON "reason=%d",
			data->p2p_lo_stop.reason_code);
#endif /* CONFIG_P2P */
		break;
	case EVENT_BEACON_LOSS:
		if (!wpa_s->current_bss || !wpa_s->current_ssid)
			break;
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_BEACON_LOSS);
		bgscan_notify_beacon_loss(wpa_s);
		break;
	case EVENT_EXTERNAL_AUTH:
#ifdef CONFIG_SAE
		if (!wpa_s->current_ssid) {
			wpa_printf(MSG_DEBUG, "SAE: current_ssid is NULL");
			break;
		}
		sme_external_auth_trigger(wpa_s, data);
#endif /* CONFIG_SAE */
		break;
	case EVENT_PORT_AUTHORIZED:
		wpa_supplicant_event_port_authorized(wpa_s);
		break;
#if UNUSED_FEAT_IN_SUPP_29
	case EVENT_STATION_OPMODE_CHANGED:
#ifdef CONFIG_AP
		if (!wpa_s->ap_iface || !data)
			break;

		hostapd_event_sta_opmode_changed(wpa_s->ap_iface->bss[0],
						 data->sta_opmode.addr,
						 data->sta_opmode.smps_mode,
						 data->sta_opmode.chan_width,
						 data->sta_opmode.rx_nss);
#endif /* CONFIG_AP */
		break;
#endif
#ifdef RSI_ENABLE_CCX
	case EVENT_CCKM_REASSOC_RESP:
		wpa_printf(MSG_DEBUG, "EVENT CCKMREASSOC RESP Recvd\n");
		wpa_supplicant_event_cckm_reassoc_resp(wpa_s, data);
		break;
#endif
	default:
		wpa_msg(wpa_s, MSG_INFO, "Unknown event %d", event);
		break;
	}
}

#if UNUSED_FEAT_IN_SUPP_29
void wpa_supplicant_event_global(void *ctx, enum wpa_event_type event,
				 union wpa_event_data *data)
{
	struct wpa_supplicant *wpa_s;

	if (event != EVENT_INTERFACE_STATUS)
		return;

	wpa_s = wpa_supplicant_get_iface(ctx, data->interface_status.ifname);
	if (wpa_s && wpa_s->driver->get_ifindex) {
		unsigned int ifindex;

		ifindex = wpa_s->driver->get_ifindex(wpa_s->drv_priv);
		if (ifindex != data->interface_status.ifindex) {
			wpa_dbg(wpa_s, MSG_DEBUG,
				"interface status ifindex %d mismatch (%d)",
				ifindex, data->interface_status.ifindex);
			return;
		}
	}
#ifdef CONFIG_MATCH_IFACE
	else if (data->interface_status.ievent == EVENT_INTERFACE_ADDED) {
		struct wpa_interface *wpa_i;

		wpa_i = wpa_supplicant_match_iface(
			ctx, data->interface_status.ifname);
		if (!wpa_i)
			return;
		wpa_s = wpa_supplicant_add_iface(ctx, wpa_i, NULL);
		os_free(wpa_i);
		if (wpa_s)
			wpa_s->matched = 1;
	}
#endif /* CONFIG_MATCH_IFACE */

	if (wpa_s)
		wpa_supplicant_event(wpa_s, event, data);
}
#endif 

/*Function to change roam_threshold and roam_hysterisis : DRAEGER_COMPILATION - ROAMING*/
void rsi_update_roam_params(void *wpa_s, uint8 roam_threshold, uint8 roam_hysterisis) {
	struct wpa_config *conf = (struct wpa_config *)((struct wpa_supplicant *)wpa_s)->conf;
	conf->roam_threshold = (int) roam_threshold;
	conf->roam_hysteresis = (int) roam_hysterisis;
}
