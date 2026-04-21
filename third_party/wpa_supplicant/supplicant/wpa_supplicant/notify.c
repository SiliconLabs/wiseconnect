/*
 * wpa_supplicant - Event notifications
 * Copyright (c) 2009-2010, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/wpa_ctrl.h"
#include "config.h"
#include "wpa_supplicant_i.h"
#include "wps_supplicant.h"
#ifdef CONFIG_BINDER
#include "binder/binder.h"
#endif
#ifdef CONFIG_DBUS
#include "dbus/dbus_common.h"
#include "dbus/dbus_new.h"
#endif
#include "rsn_supp/wpa.h"
#ifdef CONFIG_FST
#include "fst/fst.h"
#endif
#include "crypto/tls.h"
#include "driver_i.h"
#include "scan.h"
#include "sme.h"
#include "notify.h"
#ifdef CONFIG_P2P
#include "p2p/p2p_i.h"
#include "p2p_supplicant.h"
#endif

int wpas_notify_supplicant_initialized(struct wpa_global *global)
{
#ifdef CONFIG_CTRL_IFACE_DBUS_NEW
	if (global->params.dbus_ctrl_interface) {
		global->dbus = wpas_dbus_init(global);
		if (global->dbus == NULL)
			return -1;
	}
#endif /* CONFIG_CTRL_IFACE_DBUS_NEW */

#ifdef CONFIG_BINDER
	global->binder = wpas_binder_init(global);
	if (!global->binder)
		return -1;
#endif /* CONFIG_BINDER */

	return 0;
}


void wpas_notify_supplicant_deinitialized(struct wpa_global *global)
{
#ifdef CONFIG_CTRL_IFACE_DBUS_NEW
	if (global->dbus)
		wpas_dbus_deinit(global->dbus);
#endif /* CONFIG_CTRL_IFACE_DBUS_NEW */

#ifdef CONFIG_BINDER
	if (global->binder)
		wpas_binder_deinit(global->binder);
#endif /* CONFIG_BINDER */
}


int wpas_notify_iface_added(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_DBUS
	if (wpa_s->p2p_mgmt)
		return 0;

	if (wpas_dbus_register_interface(wpa_s))
		return -1;
#endif

	return 0;
}


void wpas_notify_iface_removed(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_DBUS
	if (wpa_s->p2p_mgmt)
		return;

	/* unregister interface in new DBus ctrl iface */
	wpas_dbus_unregister_interface(wpa_s);
#endif
}


void wpas_notify_state_changed(struct wpa_supplicant *wpa_s,
			       enum wpa_states new_state,
			       enum wpa_states old_state)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	/* notify the new DBus API */
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_STATE);
#endif

#ifdef CONFIG_FST
	if (wpa_s->fst && !is_zero_ether_addr(wpa_s->bssid)) {
		if (new_state == WPA_COMPLETED)
			fst_notify_peer_connected(wpa_s->fst, wpa_s->bssid);
		else if (old_state >= WPA_ASSOCIATED &&
			 new_state < WPA_ASSOCIATED)
			fst_notify_peer_disconnected(wpa_s->fst, wpa_s->bssid);
	}
#endif /* CONFIG_FST */

#ifdef CONFIG_P2P
	if (new_state == WPA_COMPLETED)
		wpas_p2p_notif_connected(wpa_s);
	else if (old_state >= WPA_ASSOCIATED && new_state < WPA_ASSOCIATED)
		wpas_p2p_notif_disconnected(wpa_s);
#endif

	sme_state_changed(wpa_s);

#ifdef ANDROID
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_STATE_CHANGE
		     "id=%d state=%d BSSID=" MACSTR " SSID=%s",
		     wpa_s->current_ssid ? wpa_s->current_ssid->id : -1,
		     new_state,
		     MAC2STR(wpa_s->bssid),
		     wpa_s->current_ssid && wpa_s->current_ssid->ssid ?
		     wpa_ssid_txt(wpa_s->current_ssid->ssid,
				  wpa_s->current_ssid->ssid_len) : "");
#endif /* ANDROID */
}


void wpas_notify_disconnect_reason(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_DISCONNECT_REASON);
#endif
}

#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_auth_status_code(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_AUTH_STATUS_CODE);
}
#endif
void wpas_notify_assoc_status_code(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;
#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ASSOC_STATUS_CODE);
#endif
}


#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_roam_time(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ROAM_TIME);
}


void wpas_notify_roam_complete(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_ROAM_COMPLETE);
}

#endif 

void wpas_notify_session_length(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_SESSION_LENGTH);
#endif
}

#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_bss_tm_status(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_BSS_TM_STATUS);
}
#endif 

void wpas_notify_network_changed(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_NETWORK);
#endif
}


void wpas_notify_ap_scan_changed(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_AP_SCAN);
#endif
}


void wpas_notify_bssid_changed(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_BSS);
#endif
}


void wpas_notify_auth_changed(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_CURRENT_AUTH_MODE);
#endif
}


void wpas_notify_network_enabled_changed(struct wpa_supplicant *wpa_s,
					 struct wpa_ssid *ssid)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_network_enabled_changed(wpa_s, ssid);
#endif
}


#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_network_selected(struct wpa_supplicant *wpa_s,
				  struct wpa_ssid *ssid)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_network_selected(wpa_s, ssid->id);
}
#endif


void wpas_notify_network_request(struct wpa_supplicant *wpa_s,
				 struct wpa_ssid *ssid,
				 enum wpa_ctrl_req_type rtype,
				 const char *default_txt)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_network_request(wpa_s, ssid, rtype, default_txt);
#endif
}


void wpas_notify_scanning(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	/* notify the new DBus API */
	wpas_dbus_signal_prop_changed(wpa_s, WPAS_DBUS_PROP_SCANNING);
#endif

}


void wpas_notify_scan_done(struct wpa_supplicant *wpa_s, int success)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_signal_scan_done(wpa_s, success);
#endif
}


void wpas_notify_scan_results(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_wps_notify_scan_results(wpa_s);
}


void wpas_notify_wps_credential(struct wpa_supplicant *wpa_s,
				const struct wps_credential *cred)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	/* notify the new DBus API */
	wpas_dbus_signal_wps_cred(wpa_s, cred);
#endif
#endif /* CONFIG_WPS */
}


void wpas_notify_wps_event_m2d(struct wpa_supplicant *wpa_s,
			       struct wps_event_m2d *m2d)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	wpas_dbus_signal_wps_event_m2d(wpa_s, m2d);
#endif
#endif /* CONFIG_WPS */
}


void wpas_notify_wps_event_fail(struct wpa_supplicant *wpa_s,
				struct wps_event_fail *fail)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	wpas_dbus_signal_wps_event_fail(wpa_s, fail);
#endif
#endif /* CONFIG_WPS */
}

void wpas_notify_wps_event_success(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	wpas_dbus_signal_wps_event_success(wpa_s);
#endif
#endif /* CONFIG_WPS */
}

void wpas_notify_wps_event_pbc_overlap(struct wpa_supplicant *wpa_s)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	wpas_dbus_signal_wps_event_pbc_overlap(wpa_s);
#endif
#endif /* CONFIG_WPS */
}


void wpas_notify_network_added(struct wpa_supplicant *wpa_s,
			       struct wpa_ssid *ssid)
{
	if (wpa_s->p2p_mgmt)
		return;

	/*
	 * Networks objects created during any P2P activities should not be
	 * exposed out. They might/will confuse certain non-P2P aware
	 * applications since these network objects won't behave like
	 * regular ones.
	 */
	
#ifdef CONFIG_DBUS
	if (!ssid->p2p_group && wpa_s->global->p2p_group_formation != wpa_s)
		wpas_dbus_register_network(wpa_s, ssid);
#endif
}


#ifdef CONFIG_P2P
void wpas_notify_persistent_group_added(struct wpa_supplicant *wpa_s,
					struct wpa_ssid *ssid)
{
#ifdef CONFIG_DBUS
	wpas_dbus_register_persistent_group(wpa_s, ssid);
#endif
}


void wpas_notify_persistent_group_removed(struct wpa_supplicant *wpa_s,
					  struct wpa_ssid *ssid)
{
#ifdef CONFIG_DBUS
	wpas_dbus_unregister_persistent_group(wpa_s, ssid->id);
#endif
}
#endif /* CONFIG_P2P */


void wpas_notify_network_removed(struct wpa_supplicant *wpa_s,
				 struct wpa_ssid *ssid)
{
	if (wpa_s->next_ssid == ssid)
		wpa_s->next_ssid = NULL;
	if (wpa_s->wpa)
		wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);
	if (!ssid->p2p_group && wpa_s->global->p2p_group_formation != wpa_s &&
	    !wpa_s->p2p_mgmt)
	{
#ifdef CONFIG_DBUS
		wpas_dbus_unregister_network(wpa_s, ssid->id);
#endif
	}
#ifdef CONFIG_P2P
	if (network_is_persistent_group(ssid))
		wpas_notify_persistent_group_removed(wpa_s, ssid);

	wpas_p2p_network_removed(wpa_s, ssid);
#endif
}


void wpas_notify_bss_added(struct wpa_supplicant *wpa_s,
			   u8 bssid[], unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_register_bss(wpa_s, bssid, id);
#endif
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_BSS_ADDED "%u " MACSTR,
		     id, MAC2STR(bssid));
}

void wpas_notify_bss_removed(struct wpa_supplicant *wpa_s,u8 bssid[], unsigned int id) _OBE_LOW_POWER_RAM_
void wpas_notify_bss_removed(struct wpa_supplicant *wpa_s,
			     u8 bssid[], unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_unregister_bss(wpa_s, bssid, id);
#endif
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_BSS_REMOVED "%u " MACSTR,
		     id, MAC2STR(bssid));
}


void wpas_notify_bss_freq_changed(struct wpa_supplicant *wpa_s,
				  unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_FREQ, id);
#endif
}


void wpas_notify_bss_signal_changed(struct wpa_supplicant *wpa_s,
				    unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_SIGNAL,
					  id);
#endif
}


void wpas_notify_bss_privacy_changed(struct wpa_supplicant *wpa_s,
				     unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_PRIVACY,
					  id);
#endif
}


void wpas_notify_bss_mode_changed(struct wpa_supplicant *wpa_s,
				  unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_MODE, id);
#endif
}


void wpas_notify_bss_wpaie_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_WPA, id);
#endif
}


void wpas_notify_bss_rsnie_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_RSN, id);
#endif
}


void wpas_notify_bss_wps_changed(struct wpa_supplicant *wpa_s,
				 unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_WPS
#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_WPS, id);
#endif
#endif /* CONFIG_WPS */
}


void wpas_notify_bss_ies_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_IES, id);
#endif
}


void wpas_notify_bss_rates_changed(struct wpa_supplicant *wpa_s,
				   unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_RATES, id);
#endif
}

void wpas_notify_bss_seen(struct wpa_supplicant *wpa_s, unsigned int id)
{
	if (wpa_s->p2p_mgmt)
		return;

#ifdef CONFIG_DBUS
	wpas_dbus_bss_signal_prop_changed(wpa_s, WPAS_DBUS_BSS_PROP_AGE, id);
#endif
}

#ifdef CONFIG_BLOB
void wpas_notify_blob_added(struct wpa_supplicant *wpa_s, const char *name)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_blob_added(wpa_s, name);
}


void wpas_notify_blob_removed(struct wpa_supplicant *wpa_s, const char *name)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_blob_removed(wpa_s, name);
}
#endif


#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_debug_level_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_level_changed(global);
}


void wpas_notify_debug_timestamp_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_timestamp_changed(global);
}


void wpas_notify_debug_show_keys_changed(struct wpa_global *global)
{
	wpas_dbus_signal_debug_show_keys_changed(global);
}
#endif


#ifdef CTRL_IFACE
void wpas_notify_suspend(struct wpa_global *global)
{
	struct wpa_supplicant *wpa_s;

	os_get_time(&global->suspend_time);
	wpa_printf(MSG_DEBUG, "System suspend notification");
	for (wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next)
		wpa_drv_suspend(wpa_s);
}


void wpas_notify_resume(struct wpa_global *global)
{
	struct os_time now;
	int slept;
	struct wpa_supplicant *wpa_s;

	if (global->suspend_time.sec == 0)
		slept = -1;
	else {
		os_get_time(&now);
		slept = now.sec - global->suspend_time.sec;
	}
	wpa_printf(MSG_DEBUG, "System resume notification (slept %d seconds)",
		   slept);

	for (wpa_s = global->ifaces; wpa_s; wpa_s = wpa_s->next) {
		wpa_drv_resume(wpa_s);
		if (wpa_s->wpa_state == WPA_DISCONNECTED)
			wpa_supplicant_req_scan(wpa_s, 0, 100000);
	}
}
#endif


#ifdef CONFIG_P2P
void wpas_notify_p2p_find_stopped(struct wpa_supplicant *wpa_s)
{
#ifdef CONFIG_DBUS
	/* Notify P2P find has stopped */
	wpas_dbus_signal_p2p_find_stopped(wpa_s);
#endif
}

void wpas_notify_p2p_device_found(struct wpa_supplicant *wpa_s,
				  const u8 *dev_addr, int new_device)
{
#ifdef CONFIG_DBUS
	if (new_device) {
		/* Create the new peer object */
		wpas_dbus_register_peer(wpa_s, dev_addr);
	}

	/* Notify a new peer has been detected*/
	wpas_dbus_signal_peer_device_found(wpa_s, dev_addr);
#endif
}


void wpas_notify_p2p_device_lost(struct wpa_supplicant *wpa_s,
				 const u8 *dev_addr)
{
#ifdef CONFIG_DBUS
	wpas_dbus_unregister_peer(wpa_s, dev_addr);

	/* Create signal on interface object*/
	wpas_dbus_signal_peer_device_lost(wpa_s, dev_addr);
#endif
}


void wpas_notify_p2p_group_removed(struct wpa_supplicant *wpa_s,
				   const struct wpa_ssid *ssid,
				   const char *role)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_group_removed(wpa_s, role);

	wpas_dbus_unregister_p2p_group(wpa_s, ssid);
#endif
}


void wpas_notify_p2p_go_neg_req(struct wpa_supplicant *wpa_s,
				const u8 *src, u16 dev_passwd_id, u8 go_intent)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_go_neg_req(wpa_s, src, dev_passwd_id, go_intent);
#endif
}


void wpas_notify_p2p_go_neg_completed(struct wpa_supplicant *wpa_s,
				      struct p2p_go_neg_results *res)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_go_neg_resp(wpa_s, res);
#endif

	if (res->status)
		sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_P2P_GO_NEG_FAILED , NULL);
}


void wpas_notify_p2p_invitation_result(struct wpa_supplicant *wpa_s,
				       int status, const u8 *bssid)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_invitation_result(wpa_s, status, bssid);
#endif
}


void wpas_notify_p2p_sd_request(struct wpa_supplicant *wpa_s,
				int freq, const u8 *sa, u8 dialog_token,
				u16 update_indic, const u8 *tlvs,
				size_t tlvs_len)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_sd_request(wpa_s, freq, sa, dialog_token,
					update_indic, tlvs, tlvs_len);
#endif
}


void wpas_notify_p2p_sd_response(struct wpa_supplicant *wpa_s,
				 const u8 *sa, u16 update_indic,
				 const u8 *tlvs, size_t tlvs_len)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_sd_response(wpa_s, sa, update_indic,
					 tlvs, tlvs_len);
#endif
}


/**
 * wpas_notify_p2p_provision_discovery - Notification of provision discovery
 * @dev_addr: Who sent the request or responded to our request.
 * @request: Will be 1 if request, 0 for response.
 * @status: Valid only in case of response (0 in case of success)
 * @config_methods: WPS config methods
 * @generated_pin: PIN to be displayed in case of WPS_CONFIG_DISPLAY method
 *
 * This can be used to notify:
 * - Requests or responses
	 * - Various config methods
 * - Failure condition in case of response
 */
void wpas_notify_p2p_provision_discovery(struct wpa_supplicant *wpa_s,
					 const u8 *dev_addr, int request,
					 enum p2p_prov_disc_status status,
					 u16 config_methods,
					 unsigned int generated_pin)
{
	struct p2p_device * p2p_dev = NULL;
	u8 *dev_name = NULL;
	u8 *join_p2p_dev = NULL;
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_provision_discovery(wpa_s, dev_addr, request,
						 status, config_methods,
						 generated_pin);
#endif
	p2p_dev = p2p_get_device(wpa_s->global->p2p, dev_addr);

	if (!request && supp_mgmt_if_ctx.waiting_for_pd_resp)
	{
		supp_mgmt_if_ctx.waiting_for_pd_resp = 0;
		if (status == P2P_PROV_DISC_SUCCESS) {
			wise_p2p_join(wpa_s, supp_mgmt_if_ctx.join_p2p_dev);
		}
		else {
			/* Send join response failure here */
			//sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 1, WISE_P2P_PROV_DISC_FAILED, &rsi_wsc_cb.client);
		}
	}
	else if ((request) && p2p_dev &&
			(!(p2p_dev->flags & P2P_DEV_PD_REQUEST_RECEIVED)) &&
			(!(p2p_dev->flags & P2P_DEV_PD_FOR_JOIN) ))
	{

		dev_name = (u8 *)p2p_dev->info.device_name;
		p2p_dev->flags |= P2P_DEV_PD_REQUEST_RECEIVED;
		sl_mgmt_indicate_to_host(RSI_RSP_P2P_CONNREQ, 32 /* All the 32 bytes are valid */, 0, (unsigned char *)dev_name);
	}

}


void wpas_notify_p2p_group_started(struct wpa_supplicant *wpa_s,
				   struct wpa_ssid *ssid, int persistent,
				   int client, const u8 *ip)
{
#ifdef CONFIG_DBUS
	/* Notify a group has been started */
	wpas_dbus_register_p2p_group(wpa_s, ssid);

	wpas_dbus_signal_p2p_group_started(wpa_s, client, persistent, ip);
#endif
	supp_mgmt_if_ctx.client_mode = client;
	sl_wpa_driver_wrapper(wpa_s, WPA_DRV_SET_CLIENT, NULL);

	if (!client) /* If it is AP */
	{
		if (wpa_s->ap_configured_cb) {
			wpa_s->ap_configured_cb ((void *)wpa_s, NULL);
		}
	} 
	else 
	{
		supp_mgmt_if_ctx.supp_join_done_cmd_input.client = client;
		sl_wpa_driver_wrapper(wpa_s, WPA_DRV_SUPP_JOIN_DONE, NULL);
	}
}


void wpas_notify_p2p_group_formation_failure(struct wpa_supplicant *wpa_s,
					     const char *reason)
{
#ifdef CONFIG_DBUS
	/* Notify a group formation failed */
	wpas_dbus_signal_p2p_group_formation_failure(wpa_s, reason);
#endif
}


void wpas_notify_p2p_wps_failed(struct wpa_supplicant *wpa_s,
				struct wps_event_fail *fail)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_p2p_wps_failed(wpa_s, fail);
#endif
	sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_P2P_WPS_FAILED , NULL);
}

#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_p2p_invitation_received(struct wpa_supplicant *wpa_s,
					 const u8 *sa, const u8 *go_dev_addr,
					 const u8 *bssid, int id, int op_freq)
{
	/* Notify a P2P Invitation Request */
	wpas_dbus_signal_p2p_invitation_received(wpa_s, sa, go_dev_addr, bssid,
						 id, op_freq);
}
#endif 
#endif /* CONFIG_P2P */


static void wpas_notify_ap_sta_authorized(struct wpa_supplicant *wpa_s,
					  const u8 *sta,
					  const u8 *p2p_dev_addr)
{
#ifdef CONFIG_P2P
	wpas_p2p_notify_ap_sta_authorized(wpa_s, p2p_dev_addr);

	/*
	 * Create 'peer-joined' signal on group object -- will also
	 * check P2P itself.
	 */
	if (p2p_dev_addr)
	{
#ifdef CONFIG_DBUS
		wpas_dbus_signal_p2p_peer_joined(wpa_s, p2p_dev_addr);
#endif
	}
#endif /* CONFIG_P2P */
#ifdef CONFIG_DBUS

	/* Register the station */
	wpas_dbus_register_sta(wpa_s, sta);

	/* Notify listeners a new station has been authorized */
	wpas_dbus_signal_sta_authorized(wpa_s, sta);
#endif
}


static void wpas_notify_ap_sta_deauthorized(struct wpa_supplicant *wpa_s,
					    const u8 *sta,
					    const u8 *p2p_dev_addr)
{
#ifdef CONFIG_P2P
	/*
	 * Create 'peer-disconnected' signal on group object if this
	 * is a P2P group.
	 */
	if (p2p_dev_addr)
	{
#ifdef CONFIG_DBUS
		wpas_dbus_signal_p2p_peer_disconnected(wpa_s, p2p_dev_addr);
#endif
	}
#endif /* CONFIG_P2P */
#ifdef CONFIG_DBUS

	/* Notify listeners a station has been deauthorized */
	wpas_dbus_signal_sta_deauthorized(wpa_s, sta);

	/* Unregister the station */
	wpas_dbus_unregister_sta(wpa_s, sta);
#endif
}


void wpas_notify_sta_authorized(struct wpa_supplicant *wpa_s,
				const u8 *mac_addr, int authorized,
				const u8 *p2p_dev_addr)
{
	if (authorized)
		wpas_notify_ap_sta_authorized(wpa_s, mac_addr, p2p_dev_addr);
	else
		wpas_notify_ap_sta_deauthorized(wpa_s, mac_addr, p2p_dev_addr);
}


void wpas_notify_certification(struct wpa_supplicant *wpa_s,
			       struct tls_cert_data *cert,
			       const char *cert_hash)
{
	wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_EAP_PEER_CERT
		"depth=%d subject='%s'%s%s%s",
		cert->depth, cert->subject, cert_hash ? " hash=" : "",
		cert_hash ? cert_hash : "",
		cert->tod ? " tod=1" : "");
#ifndef CONFIG_OPTIMIZE_CODE
	if (cert->cert) {
		char *cert_hex;
		size_t len = wpabuf_len(cert->cert) * 2 + 1;
		cert_hex = os_malloc(len);
		if (cert_hex) {
			wpa_snprintf_hex(cert_hex, len, wpabuf_head(cert->cert),
					 wpabuf_len(cert->cert));
			wpa_msg_ctrl(wpa_s, MSG_INFO,
				     WPA_EVENT_EAP_PEER_CERT
				     "depth=%d subject='%s' cert=%s",
				     cert->depth, cert->subject, cert_hex);
			os_free(cert_hex);
		}
	}

	for (int i = 0; i < cert->num_altsubject; i++)
		wpa_msg(wpa_s, MSG_INFO, WPA_EVENT_EAP_PEER_ALT
			"depth=%d %s", cert->depth, cert->altsubject[i]);

	/* notify the new DBus API */
	wpas_dbus_signal_certification(wpa_s, cert->depth, cert->subject,
				       cert->altsubject, cert->num_altsubject,
				       cert_hash, cert->cert);
#endif
}


void wpas_notify_preq(struct wpa_supplicant *wpa_s,
		      const u8 *addr, const u8 *dst, const u8 *bssid,
		      const u8 *ie, size_t ie_len, u32 ssi_signal)
{
#ifdef CONFIG_AP
#ifdef CONFIG_DBUS
	wpas_dbus_signal_preq(wpa_s, addr, dst, bssid, ie, ie_len, ssi_signal);
#endif
#endif /* CONFIG_AP */
}


void wpas_notify_eap_status(struct wpa_supplicant *wpa_s, const char *status,
			    const char *parameter)
{
#ifdef CONFIG_DBUS
	wpas_dbus_signal_eap_status(wpa_s, status, parameter);
#endif
	wpa_msg_ctrl(wpa_s, MSG_INFO, WPA_EVENT_EAP_STATUS
		     "status='%s' parameter='%s'",
		     status, parameter);
}


void wpas_notify_eap_error(struct wpa_supplicant *wpa_s, int error_code)
{
	wpa_msg(wpa_s, MSG_ERROR, WPA_EVENT_EAP_ERROR_CODE "%d", error_code);
}

#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_network_bssid_set_changed(struct wpa_supplicant *wpa_s,
					   struct wpa_ssid *ssid)
{
	if (wpa_s->current_ssid != ssid)
		return;

#if UNUSED_FEAT_IN_SUPP_29
	wpa_dbg(wpa_s, MSG_DEBUG,
		"Network bssid config changed for the current network - within-ESS roaming %s",
		ssid->bssid_set ? "disabled" : "enabled");
#endif
	SL_PRINTF(WLAN_SUPP_NETWORK_BSSID_CONFIG_CHANGED, WLAN_UMAC, LOG_INFO);
	wpa_drv_roaming(wpa_s, !ssid->bssid_set,
			ssid->bssid_set ? ssid->bssid : NULL);
}

void wpas_notify_network_type_changed(struct wpa_supplicant *wpa_s,
				      struct wpa_ssid *ssid)
{
#ifdef CONFIG_P2P
	if (ssid->disabled == 2) {
		/* Changed from normal network profile to persistent group */
		ssid->disabled = 0;
		wpas_dbus_unregister_network(wpa_s, ssid->id);
		ssid->disabled = 2;
		ssid->p2p_persistent_group = 1;
		wpas_dbus_register_persistent_group(wpa_s, ssid);
	} else {
		/* Changed from persistent group to normal network profile */
		wpas_dbus_unregister_persistent_group(wpa_s, ssid->id);
		ssid->p2p_persistent_group = 0;
		wpas_dbus_register_network(wpa_s, ssid);
	}
#endif /* CONFIG_P2P */
}
#endif 

#ifdef CONFIG_MESH

#if UNUSED_FEAT_IN_SUPP_29
void wpas_notify_mesh_group_started(struct wpa_supplicant *wpa_s,
				    struct wpa_ssid *ssid)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_mesh_group_started(wpa_s, ssid);
}
#endif


void wpas_notify_mesh_group_removed(struct wpa_supplicant *wpa_s,
				    const u8 *meshid, u8 meshid_len,
				    u16 reason_code)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_mesh_group_removed(wpa_s, meshid, meshid_len,
					    reason_code);
}


void wpas_notify_mesh_peer_connected(struct wpa_supplicant *wpa_s,
				     const u8 *peer_addr)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_mesh_peer_connected(wpa_s, peer_addr);
}


void wpas_notify_mesh_peer_disconnected(struct wpa_supplicant *wpa_s,
					const u8 *peer_addr, u16 reason_code)
{
	if (wpa_s->p2p_mgmt)
		return;

	wpas_dbus_signal_mesh_peer_disconnected(wpa_s, peer_addr, reason_code);
}

#endif /* CONFIG_MESH */
