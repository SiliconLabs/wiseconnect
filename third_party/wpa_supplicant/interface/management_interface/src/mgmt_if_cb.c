/*******************************************************************************
* @file  mgmt_if_cb.c
* @brief This contains interface functions that require supplicant defined
*        structures and assigns callback functions for driver.
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
#include "mgmt_if_utils.h"
#include "mgmt_if_tx.h"
#include "mgmt_if_header.h"
#include "mgmt_data_flow.h"
#include "ieee_80211_utils.h"
#include "common.h"
#include "wpa_supplicant_i.h"
#include "bss.h"
#include "rsn_supp/wpa_i.h"
#include "config.h"
#include "includes.h"
#include "ap/hostapd.h"
#include "ap/sta_info.h"
#include "ap/ieee802_11.h"
#include "mgmt_if_rsi_wsc.h"
#include "rsi_controller_apme.h"
#include "sli_supp_event_loop_thread.h"

extern struct ap_cb_s ap_cb;

#define STATION_RSN_IE_PREAUTH_BIT_ENABLE 1

#define IS_AUTH_WEP_MODE(m) (m == SEC_MODE_WEP)

#define IS_AUTH_SEC_MODE(m)                                                                          \
  ((m == SEC_MODE_WPA) || (m == SEC_MODE_WPA2) || (m == SEC_MODE_WPA3) || (m == SEC_MODE_MIXED_MODE) \
   || (m == SEC_MODE_WPA_ENTERPRISE) || (m == SEC_MODE_WPA2_ENTERPRISE) || (m == SEC_MODE_WPA3_TRANSITION))

#define IS_AUTH_OPEN_MODE(m) (m == SEC_MODE_OPEN)

const char glbl_ifname[10] = "wlan0";

const char glbl_confname[10] = "wsc_conf";

const char glbl_ifname_ap[10] = "ap0";

const char glbl_confname_ap[11] = "ap_conf";

struct wpa_interface global_iface;

#ifdef DATA_PATH_UMAC_ENABLE
extern INT tcp_ip_stack_init();
#endif

static uint8 send_pmkid_validation_status_to_sme(uint8 *buf, uint16 msg_len, uint16 pmkid_status);

extern int wpa_scan_result_compar(const void *a, const void *b);

int16 is_ap_rrm_capable(struct scanConfirm_s *scanCon)
{
  return ((scanCon->CapabilityInfo & IEEE80211_CAP_RRM) ? 1 : 0);
}

int wpa_driver_rsi_scan(void *priv, struct wpa_driver_scan_params *params)
{
  rsi_uScan_non_rom_t Drv_uScanFrame;

  if (!priv) {
    return -1;
  }
  /* Scan already sent; waiting for results */

  sl_memset((char *)&Drv_uScanFrame, 0, sizeof(rsi_uScan_non_rom_t));

  if (params) {
    if (params->extra_ies != NULL && params->extra_ies_len) {
      if (rsi_load_ie((uint8 *)params->extra_ies, params->extra_ies_len, IE_PROBE_REQ)) {
        return WLAN_STATUS_RSI_FAIL;
      }
    }

    /* Get the bitmap for channels to be scanned */
    if (
      params
        ->freqs /*&& freq_to_bitmap_convert(params->freqs, (uint16 *)Drv_uScanFrame.scanFrameSnd.channel_bit_mask) == 1 */) {
      if (freq_to_bitmap_convert(params->freqs, Drv_uScanFrame.scanFrameSnd.channel_bit_mask) == 1) {
        Drv_uScanFrame.scanFrameSnd.scan_channel = get_channel_num((uint16)params->freqs[0]);
      } else {
        Drv_uScanFrame.scanFrameSnd.scan_channel = 0;
      }
    } else {
      Drv_uScanFrame.scanFrameSnd.channel_bit_mask[0] = 0xFFFF;
      Drv_uScanFrame.scanFrameSnd.channel_bit_mask[1] = 0xFFFF;
      Drv_uScanFrame.scanFrameSnd.channel_bit_mask[2] = 0xFFFF;
    }

    // printf("Scan chan:%d\n", Drv_uScanFrame.scanFrameSnd.scan_channel);
    if ((mgmt_if_adapter.operating_mode == WISE_MODE_P2P) && params->freqs
        && (mgmt_if_adapter.current_freq_band != GET_FREQ_BAND(*params->freqs))) {
      /* Save channel frequency */
      mgmt_if_adapter.current_freq_band = GET_FREQ_BAND(*params->freqs);
      mgmt_hm_switch_band(&mgmt_if_adapter.current_freq_band);
    }

    if ((mgmt_if_adapter.ssid_len > 0) && (mgmt_if_adapter.operating_mode != WISE_MODE_P2P)) {
      sl_memcpy((char *)Drv_uScanFrame.scanFrameSnd.ssid,
                (char *)sl_get_ssid_from_mgmt_if_adapter(),
                mgmt_if_adapter.ssid_len);
    } else {
      Drv_uScanFrame.scanFrameSnd.ssid_len = params->ssids[0].ssid_len;
      if (Drv_uScanFrame.scanFrameSnd.ssid_len > 0) {
        sl_strcpy((uint8 *)Drv_uScanFrame.scanFrameSnd.ssid, (uint8 *)params->ssids[0].ssid);
      }
    }
  }
  Drv_uScanFrame.scanFrameSnd.pscan_bitmap        = mgmt_if_adapter.passive_scan_bitmap;
  Drv_uScanFrame.scanFrameSnd.scan_feature_bitmap = mgmt_if_adapter.scan_feature_bitmap;
  if (mgmt_if_adapter.scan_feature_bitmap & FEATURE_BSSID_BASED_SCAN) {
    sl_memcpy(Drv_uScanFrame.scanFrameSnd.bssid, get_bssid_from_mgmt_intf_adapter(), BSSID_LENGTH);
  }

  return rsi_scan((rsi_uScan *)&Drv_uScanFrame, mgmt_if_adapter.current_freq_band);
}

uint8 *sl_get_scan_results(void *supp_priv)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)supp_priv;
  return rsi_get_scan_results(wpa_s->drv_priv);
}

/*==============================================*/
/**
 *
 * @fn         get_akm_suites_info
 * @brief      This function lists the AKM suites supported by AP.
 * @param[in]  akmcnt - AKM count.
 * @param[in]  ie_data -  pointer to IE buffer.
 * @return     key_mgmt value.
 *
 * @section description
 * This function returns list of AKM suites supported by AP.
 *
 */

unsigned int get_akm_suites_info(const uint16 akmcnt, const uint8 *ie_data)
{
  int i;
  unsigned int key_mgmt = 0;
  uint32 oui_type, oui_type_endian_swapped;
  if (!ie_data) {
    return 0;
  }

  for (i = 0; i < akmcnt; i++) {
    oui_type                = (*(uint32 *)&ie_data[(RSN_SELECTOR_LEN * i)]);
    oui_type_endian_swapped = ((oui_type & 0x000000FF) << 24) | ((oui_type & 0x0000FF00) << 8)
                              | ((oui_type & 0x00FF0000) >> 8) | ((oui_type & 0xFF000000) >> 24);

    switch (oui_type_endian_swapped) {
      case RSN_AUTH_KEY_MGMT_UNSPEC_802_1X:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA | WPA_DRIVER_CAPA_KEY_MGMT_WPA2;
        break;
      case RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK | WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK;
        break;
      case RSN_AUTH_KEY_MGMT_802_1X_SHA256:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_802_1X_SHA256;
        break;
      case RSN_AUTH_KEY_MGMT_PSK_SHA256:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_PSK_SHA256;
        break;
      case RSN_AUTH_KEY_MGMT_SAE:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_SAE;
        break;
      case RSN_AUTH_KEY_MGMT_FT_SAE:
        key_mgmt |= WPA_DRIVER_CAPA_KEY_MGMT_FT_SAE;
        break;
    }
  }
  return key_mgmt;
}

void wpa3_scan_result_sec_update_wrapper(const uint8 *ie,
                                         uint16 pairwise_cipher_count,
                                         void *result,
                                         uint8 update_scan_confirm_struct)
{
  uint16 akmcnt         = *(uint16 *)&ie[8 + 2 + 4 * pairwise_cipher_count];
  unsigned int key_mgmt = get_akm_suites_info(akmcnt, &ie[RSN_AKM_OFFSET + (RSN_SELECTOR_LEN * pairwise_cipher_count)]);
  if (key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_SAE) /* WPA3 psk */
  {
    if (update_scan_confirm_struct == 1) {
      ((struct scanConfirm_s *)result)->sec_mode = SEC_MODE_WPA3;
    } else {
      if (akmcnt == 1) {
        ((struct wpa_scan_res *)result)->security_mode = SEC_MODE_WPA3;
      } else {
        if ((key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_PSK_SHA256) || (key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK)) {
          ((struct wpa_scan_res *)result)->security_mode = SEC_MODE_WPA3_TRANSITION;
        }
      }
    }
  }
}

/*==============================================*/
/**
 *
 * @fn         get_cipher_suites_info
 * @brief      This function lists the RSN cipher suites supported by AP.
 * @param[in]  cipher_cnt - cipher count.
 * @param[in]  ie_data -  pointer to IE buffer.
 * @return     cipher_suite_type.
 *
 * @section description
 * This function returns list of RSN cipher suites used for unicast or multicast encryption supported by AP.
 *
 */

static unsigned int get_cipher_suites_info(const uint16 cipher_cnt, const uint8 *ie_data)
{
  int i;
  unsigned int cipher_suite_type = 0;
  uint32 oui_type;

  if (!ie_data) {
    return 0;
  }

  for (i = 0; i < cipher_cnt; i++) {
    oui_type = (*(uint32 *)&ie_data[(RSN_SELECTOR_LEN * i)]);

    switch (oui_type) {
      case RSN_CIPHER_SUITE_WEP40:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_WEP40;
        break;

      case RSN_CIPHER_SUITE_TKIP:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_TKIP;
        break;

      case RSN_CIPHER_SUITE_CCMP:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_CCMP;
        break;

      case RSN_CIPHER_SUITE_WEP104:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_WEP104;
        break;

      case RSN_CIPHER_SUITE_AES_128_CMAC:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_BIP;
        break;

      case RSN_CIPHER_SUITE_GCMP:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_GCMP;
        break;

      case RSN_CIPHER_SUITE_GCMP_256:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_GCMP_256;
        break;

      case RSN_CIPHER_SUITE_CCMP_256:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_CCMP_256;
        break;

      case RSN_CIPHER_SUITE_BIP_GMAC_128:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_BIP_GMAC_128;
        break;

      case RSN_CIPHER_SUITE_BIP_GMAC_256:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_BIP_GMAC_256;
        break;

      case RSN_CIPHER_SUITE_BIP_CMAC_256:
        cipher_suite_type |= WPA_DRIVER_CAPA_ENC_BIP_CMAC_256;
        break;

      default:
        break;
    }
  }
  return cipher_suite_type;
}

/*==============================================*/
/* @fn         set_security_type
 * @brief      This function updates security type in scan results
 * @param[in]  ies -      pointer to new bss (Information Elements(IEs) from beacon/probe resp).
 * @param[in]  ies_len -  length of IE buffer
 * @param[out] result -   pointer to wpa_scan_res structure which contains specifics of the BSS.
 * @return     void
 */
void set_security_type(const uint8 *ies, uint16 ies_len, struct wpa_scan_res *result, uint8 update_scan_confirm_struct)
{
  uint16 ii                      = 0;
  uint32 eId                     = 0;
  uint32 eIdLength               = 0;
  uint16 pairwise_cipher_count   = 0;
  uint16 auth_count              = 0;
  unsigned int key_mgmt          = 0;
  unsigned int cipher_suite_type = 0;
  const uint8 *ie                = NULL;

#ifdef RSI_ENABLE_CCX
  while (ii < ies_len) {
    eId       = ies[ii];
    eIdLength = ies[ii + 1];

    if (eId == IEEE80211_ELEMID_VENDOR) {
      if (isccxoui(&ies[ii])) {
        result->ccx_capable = 1;
      } else if ((!memcmp(&ies[ii + 2], WLAN_WIFI_OUI, 3)) && ies[ii + 2 + 3] == 0x01) {
        result->security_mode = SEC_MODE_WPA;
        pairwise_cipher_count = *(uint16 *)&ies[ii + 12];
        auth_count            = *(uint16 *)&ies[ii + 12 + 2 + pairwise_cipher_count * 4];
        if (ies[ii + 12 + 2 + pairwise_cipher_count * 4 + 2 + auth_count * 4 - 1] == 1) {
          result->security_mode = SEC_MODE_WPA_ENTERPRISE;
        }
      }
    }
    ii += (eIdLength + 2);
  }
#else
  ie = get_ie_wrapper((uint8 *)(result + 1), ies_len, (uint8)WLAN_EID_VENDOR_SPECIFIC);

  if (ie != NULL) {
    if ((!memcmp(&ie[2], WLAN_WIFI_OUI, 3)) && ie[2 + 3] == 0x01) {
      result->security_mode = SEC_MODE_WPA;
      pairwise_cipher_count = *(uint16 *)&ie[12];
      if (ie[19 + pairwise_cipher_count * 4] == 1) {
        result->security_mode = SEC_MODE_WPA_ENTERPRISE;
      }
    }
  }
#endif /* RSI_ENABLE_CCX */
  ie = get_ie_wrapper((uint8 *)(result + 1), ies_len, (uint8)WLAN_EID_RSN);

  if (ie != NULL) {
    result->security_mode = SEC_MODE_WPA2_ENTERPRISE;
    pairwise_cipher_count = *(uint16 *)&ie[8];
    auth_count            = *(uint16 *)&ie[10 + pairwise_cipher_count * 4];
    key_mgmt = get_akm_suites_info(auth_count, &ie[RSN_AKM_OFFSET + (RSN_SELECTOR_LEN * pairwise_cipher_count)]);
    cipher_suite_type = get_cipher_suites_info(pairwise_cipher_count, &ie[10]);

    if (key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK) {
      result->security_mode = SEC_MODE_WPA2;
      if ((cipher_suite_type & WPA_DRIVER_CAPA_ENC_TKIP) && (cipher_suite_type & WPA_DRIVER_CAPA_ENC_CCMP))
        result->security_mode = SEC_MODE_MIXED_MODE;
    }

    if (key_mgmt & WPA_DRIVER_CAPA_KEY_MGMT_WPA2) {
      result->security_mode = SEC_MODE_WPA2_ENTERPRISE;
    }

    wpa3_scan_result_sec_update_wrapper(ie, pairwise_cipher_count, result, update_scan_confirm_struct);
  }
}

/*mgm_if_init functions that are dependent on supplicant structures*/

void wpas_start_wps_apconf(void *wpa_ctx)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)wpa_ctx;
  struct wpa_ssid *ssid;
  uint16 ssid_len;
  apconfig_st_t *ap_conf = &mgmt_if_adapter.apconfig;
  ssid_len               = sl_strlen((char *)ap_conf->ssid);
  ssid                   = (struct wpa_ssid *)wpa_config_add_network_wrapper(wpa_s);

  if (ssid == NULL) {
    return;
  }

  wpa_config_set_network_defaults_wrapper(ssid);
  ssid->temporary = 1;
  ssid->mode      = WPAS_MODE_AP;
  ssid->frequency = get_frequency(mgmt_if_adapter.current_freq_band,
                                  ap_conf->channel_no,
                                  ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
                                   && (mgmt_if_adapter.region_code == REGION_JAP_NUM)));
  ssid->ssid      = sl_zalloc(ssid_len + 1);
  if (ssid->ssid) {
    sl_memcpy(ssid->ssid, ap_conf->ssid, ssid_len);
    ssid->ssid_len = ssid_len;
  }
  if (ap_conf->security_type == SEC_MODE_WPA3) {
    ssid->auth_alg = WPA_AUTH_ALG_SAE;
  } else {
    ssid->auth_alg = WPA_AUTH_ALG_OPEN;
  }

  if (ap_conf->security_type == 0) {
    ssid->key_mgmt = WPA_KEY_MGMT_NONE;
  } else if (ap_conf->security_type == SEC_MODE_WPA3) {
    ssid->key_mgmt = WPA_KEY_MGMT_SAE;
  } else if (ap_conf->security_type == SEC_MODE_WPA3_TRANSITION) {
    ssid->key_mgmt = WPA_KEY_MGMT_SAE | WPA_KEY_MGMT_PSK;
  } else {
    ssid->key_mgmt = WPA_KEY_MGMT_PSK;
  }

  /* In WPA2/3 or WPA3 AP mode, TDI is extacted from bit[7-4] of encryp_mode byte */
  if (ap_conf->security_type == SEC_MODE_WPA3_TRANSITION || ap_conf->security_type == SEC_MODE_WPA3) {
    ssid->transition_disable = (ap_conf->encryp_mode >> 4) & 0x1;
  }

#ifdef CONFIG_IEEE_80211W
  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    ssid->ieee80211w = (mgmt_if_adapter.join_feature_bitmap_concurrent_ap & 0x60)
                         ? (mgmt_if_adapter.join_feature_bitmap_concurrent_ap & 0x40 ? MGMT_FRAME_PROTECTION_REQUIRED
                                                                                     : MGMT_FRAME_PROTECTION_OPTIONAL)
                         : 0;
  } else {
    ssid->ieee80211w =
      (mgmt_if_adapter.join_feature_bitmap & 0x60)
        ? (mgmt_if_adapter.join_feature_bitmap & 0x40 ? MGMT_FRAME_PROTECTION_REQUIRED : MGMT_FRAME_PROTECTION_OPTIONAL)
        : 0;
  }
  if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211W && (ap_conf->security_type == SEC_MODE_WPA2)) {
    if (ssid->ieee80211w == MGMT_FRAME_PROTECTION_OPTIONAL) {
      ssid->key_mgmt |= WPA_KEY_MGMT_PSK;
    } else if (ssid->ieee80211w == MGMT_FRAME_PROTECTION_REQUIRED) {
      ssid->key_mgmt |= WPA_KEY_MGMT_PSK_SHA256;
    }
  }
#endif

  if (ap_conf->security_type == SEC_MODE_WPA) {
    ssid->proto = WPA_PROTO_WPA;
  } else if ((ap_conf->security_type == SEC_MODE_WPA2) || (ap_conf->security_type == SEC_MODE_WPA3)
             || (ap_conf->security_type == SEC_MODE_WPA3_TRANSITION)) {
    ssid->proto = WPA_PROTO_RSN;
  }

  /*----------------WPA_CIPHER_TKIP = 2 ,WPA_CIPHER_CCMP =1--------------------*/

  if ((ap_conf->encryp_mode & 0xF) == 1)
    ssid->pairwise_cipher = WPA_CIPHER_TKIP;
  else if ((ap_conf->encryp_mode & 0xF) == 2)
    ssid->pairwise_cipher = WPA_CIPHER_CCMP;

  ssid->passphrase = (char *)sl_malloc(sizeof(ap_conf->psk));
  if (ssid->passphrase == NULL)
    return;

  sl_strcpy((uint8 *)ssid->passphrase, (uint8 *)ap_conf->psk);

#ifndef EXCLUDE_AP
  wpa_s->ap_configured_cb      = wise_wps_ap_configured_cb;
  wpa_s->ap_configured_cb_ctx  = wpa_s;
  wpa_s->ap_configured_cb_data = NULL;
#endif
  wpa_s->connect_without_scan = ssid;
  wpa_s->reassociate          = 1;
  wpa_s->disconnected         = 0;
  wpa_supplicant_req_scan_wrapper(wpa_s, 0, 0);
}

void wsc_supp_join_done(uint8 client)
{
  uint8 go;
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;

  if (NULL == wpa_s) {
    return;
  }

  set_wpa_auto_reconnect_disabled(wpa_s, 0);

  mgmt_if_adapter.wps_session  = SL_FALSE;
  mgmt_if_adapter.cfm_required = SL_FALSE;

  if ((mgmt_if_adapter.operating_mode != WISE_CONCURRENT_MODE && !mgmt_if_adapter.roam_ind)
      || (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE && !mgmt_if_adapter.roam_ind && client))
    wise_update_state(WISE_STATE_CONNECTED);

#ifdef PS_DEPENDENT_VARIABLES
  if (umac_ps_cb.sleep_type != DISABLE_PS) {
    update_umac_pmmode_cmd_info();
  }
#endif /* PS_DEPENDENT_VARIABLES */

#ifndef ENABLE_DRAEGER_CUSTOMIZATION
  if (rsi_wsc_cb.connected == 0) {
    rsi_wsc_cb.rejoin_going_on = 0;
  }
#endif
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
  mgmt_if_adapter.async_reason_code_bkp = 0;
  /* This is the case where configured AP is not found */
  mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_ASSOCIATED;
  mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
  wise_module_state(RSI_MODULE_STATE_FINAL_CONNECTON);
#endif
  /*Reconfigure these value because it is reseted with default value during supplicant initialisation*/
  rsi_update_roam_params_wrapper(mgmt_if_adapter.supplicant_priv,
                                 mgmt_if_adapter.roam_params.roam_threshold,
                                 mgmt_if_adapter.roam_params.roam_hysteresis);

  if (mgmt_if_adapter.rejoin_going_on == SL_TRUE) {
    if (mgmt_if_adapter.prev_state == WISE_STATE_IP_CONFIG_DONE) {
      wise_update_state(WISE_STATE_IP_CONFIG_DONE);
    }
    if (mgmt_if_adapter.prev_state == WISE_STATE_IPV6_CONFIG_DONE) {
      wise_update_state(WISE_STATE_IPV6_CONFIG_DONE);
    }

    mgmt_if_adapter.rejoin_count    = 0;
    mgmt_if_adapter.rejoin_going_on = SL_FALSE;
#ifdef PS_DEPENDENT_VARIABLES
    set_rejoin_going_on(0);
    if (umac_ps_cb.sleep_type == PM1 || umac_ps_cb.sleep_type == PM2 || umac_ps_cb.sleep_type == PM2_MESSAGE
        || umac_ps_cb.sleep_type == PM2_M4_BASED) {
      process_pwmode_req(umac_ps_cb.sleep_type, umac_ps_cb.ulp_mode_enable, umac_ps_cb.listen_interval_dtim, 1);
    }
#endif /*PS_DEPENDENT_VARIABLES*/

#ifdef ENABLE_DRAEGER_CUSTOMIZATION
    if (!mgmt_if_adapter.join_from_host)
#endif
      return;
  }
  mgmt_if_adapter.connected = SL_TRUE;

  go = client ? 'C' : 'G';

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE && !client && !mgmt_if_adapter.ap_started) {
    mgmt_if_adapter.ap_started = SL_TRUE;
    wise_update_concurrent_state(WISE_CON_STATE_CONNECTED);
  }

#ifdef ENABLE_CONNECT_WITHOUT_SCAN
  struct wpa_bss *bss = NULL;
  bss                 = (struct wpa_bss *)
    wpa_bss_get_wrapper(wpa_s, wpa_s->bssid, mgmt_if_adapter.join_ssid, mgmt_if_adapter.scan_ssid_len);
#endif
  if ((!mgmt_if_adapter.roam_ind)) {
    /* Send join response here */
    sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 1, WLAN_STATUS_SUCCESS, &go);
    SL_PRINTF(WLAN_UMAC_MAIN_ROAM_IND_SUCCESS, WLAN_UMAC, LOG_INFO);
  } else {
#ifdef PS_DEPENDENT_VARIABLES
    /*Re-enable powersave if module is already in sleep*/
    if (umac_ps_cb.sleep_type == PM1 || umac_ps_cb.sleep_type == PM2 || umac_ps_cb.sleep_type == PM2_MESSAGE
        || umac_ps_cb.sleep_type == PM2_M4_BASED) {
      process_pwmode_req(umac_ps_cb.sleep_type, umac_ps_cb.ulp_mode_enable, umac_ps_cb.listen_interval_dtim, 1);
    }
    set_roam_ind(0); /*Do not give response of Roaming to host*/
#endif               /*PS_DEPENDENT_VARIABLES*/

    mgmt_if_adapter.roam_ind = 0;
    SL_PRINTF(WLAN_UMAC_MAIN_ROAM_IND_FAILURE, WLAN_UMAC, LOG_INFO);
  }

  //! Reset command request
  mgmt_if_adapter.host_cmd = 0;

#ifdef DATA_PATH_UMAC_ENABLE
  /* AP case */
  if (!client) {
    tcp_ip_stack_init();
  }
#endif
}

int wpa_driver_rsi_join(void *priv, void *params_p)
{
  struct wpa_driver_associate_params *params = (struct wpa_driver_associate_params *)params_p;
#ifdef ENABLE_PREAUTHENTICATION
  uint8 *rsn_ie;
#endif
  rsi_uJoin *uJoinFrame;
  uint8 *join_frame            = NULL;
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.supplicant_priv;
  struct wpa_bss *bss          = NULL;
  struct wpa_bss *bss_beacon;
  uint16 join_frame_len  = 0;
  scanConfirm_t *scanCon = NULL;
  uint8 channel;

  if (!params || !priv) {
#ifdef WISE_DEBUG_ENABLE
    SLI_WLAN_MGMT_ASSERT(RSI_JOIN_ERROR);
#endif
  }

  /*ROAMING Change*/
  if (
#ifdef ENABLE_11R
    !(mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_ENABLE_11R_ODS) &&
#endif
    (mgmt_if_adapter.state >= WISE_STATE_CONNECTED) && (mgmt_if_adapter.roam_ind == 1)) {
    rsi_load_roam_ind(&mgmt_if_adapter.roam_ind);

    if ((mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_ROAM_MODE_WITH_DEAUTH)) {
      /*Set ROAM mode thruogh custom feature bitmap 0x2000: if set then roam
       * through DEAUTH, default is Roam through NULL.
       * */
      rsi_send_null_and_block_unblock(1);
    } else {
      rsi_send_deauth_frm_host();
    }
  }

  /* Store bssid */
  if (params->bssid)
    sl_memcpy(mgmt_if_adapter.bssid, (void *)(params->bssid), ETH_ALEN);

  if (((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (params->mode != WPAS_MODE_AP))
      || (mgmt_if_adapter.operating_mode != WISE_CONCURRENT_MODE)) {
    /* Store ssid */
    sl_memcpy(mgmt_if_adapter.ssid, (void *)(params->ssid), params->ssid_len);
    mgmt_if_adapter.ssid_len = params->ssid_len;
  }

  if (params->wpa_ie != NULL /*&& params->wpa_ie_len*/) {
    // asm("debug");
#ifdef ENABLE_PREAUTHENTICATION
    bss    = (struct wpa_bss *)wpa_bss_get_wrapper(wpa_s, params->bssid, params->ssid, params->ssid_len);
    rsn_ie = (uint8 *)wpa_bss_get_ie(bss, WLAN_EID_RSN);

    if ((*(uint16 *)(rsn_ie + 20) & STATION_RSN_IE_PREAUTH_BIT_ENABLE)
        && (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_PREAUTH_SUPPORT_ENABLE)) {
      *((uint16 *)(((uint8 *)params->wpa_ie) + 20)) |= STATION_RSN_IE_PREAUTH_BIT_ENABLE;

      struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)(mgmt_if_adapter.supplicant_priv);
      *((uint16 *)(((uint8 *)wpa_s->wpa->assoc_wpa_ie) + 20)) |= STATION_RSN_IE_PREAUTH_BIT_ENABLE;
    }
#endif

    if (rsi_load_ie((uint8 *)params->wpa_ie, params->wpa_ie_len, IE_ASSOC_REQ)) {
      return WLAN_STATUS_RSI_FAIL;
    }
  }
#ifdef ENABLE_11R

  join_frame = os_malloc(sizeof(rsi_uJoin) + sizeof(scanConfirm_nonrom_t));

#else

  join_frame = os_malloc(sizeof(rsi_uJoin) + sizeof(scanConfirm_t));
#endif
  if (join_frame == NULL) {
    //! Return error
    //wpa_printf(1, "Malloc failed: Heap memory for supplicant over during join command");
    return -1;
  }
#ifdef ENABLE_11R
  sl_memset(join_frame, 0, (sizeof(rsi_uJoin) + sizeof(scanConfirm_nonrom_t)));
#else
  sl_memset(join_frame, 0, (sizeof(rsi_uJoin) + sizeof(scanConfirm_t)));
#endif
  uJoinFrame = (rsi_uJoin *)join_frame;

  uJoinFrame->joinFrameSnd.sae_pmksa_caching = wpa_s->sme.sae_pmksa_caching;
  uJoinFrame->joinFrameSnd.listen_interval   = mgmt_if_adapter.listen_interval;
  uJoinFrame->joinFrameSnd.ibssChannel       = get_channel_num((uint16)params->freq.freq);
  channel                                    = uJoinFrame->joinFrameSnd.ibssChannel;

  if (params->freq.freq) {
    if (GET_FREQ_BAND(params->freq.freq)) {
#ifdef CONFIG_IEEE_80211J
      if ((!(mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J))
          || ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
              && ((mgmt_if_adapter.region_code != REGION_JAP_NUM))))
#endif
        channel = translate_channel(uJoinFrame->joinFrameSnd.ibssChannel);
    }
  }
#ifdef ENABLE_BG_ONLY_MODE
  uJoinFrame->joinFrameSnd.join_feature_bitmap = mgmt_if_adapter.join_feature_bitmap;
#endif

  mgmt_if_adapter.join_channel          = uJoinFrame->joinFrameSnd.ibssChannel;
  uJoinFrame->joinFrameSnd.dataRate     = mgmt_if_adapter.join_data_rate;
  uJoinFrame->joinFrameSnd.powerLevel   = mgmt_if_adapter.join_power_level;
  uJoinFrame->joinFrameSnd.ssid_len     = params->ssid_len;
  uJoinFrame->joinFrameSnd.securityType = mgmt_if_adapter.sec_type_enable;
  sl_memcpy(uJoinFrame->joinFrameSnd.ssid, (void *)(params->ssid), params->ssid_len);
  uJoinFrame->joinFrameSnd.wps_session = mgmt_if_adapter.wps_session;

  /* BSSID */
  if (params->bssid)
    sl_memcpy(uJoinFrame->joinFrameSnd.bssid, (void *)(params->bssid), ETH_ALEN);

  uJoinFrame->joinFrameSnd.nwType = LMAC_OP_STA; /* 0- IBSS, 1-STA mode, 2-IBSS Sec, 3- AP*/
  if ((params->mode == WPAS_MODE_P2P_GO) || (params->mode == WPAS_MODE_AP)) {
    uJoinFrame->joinFrameSnd.nwType = LMAC_OP_AP;
    mgmt_if_adapter.ap_mode_set     = 1;

    if (mgmt_if_adapter.sec_type_enable == SEC_MODE_WPA) {
      mgmt_hm_load_ap_caps(IEEE80211_F_PRIVACY | IEEE80211_F_WPA1);
    } else if ((mgmt_if_adapter.sec_type_enable != SEC_MODE_OPEN)) {
      mgmt_hm_load_ap_caps(IEEE80211_F_PRIVACY | IEEE80211_F_WPA2);
    } else {
      mgmt_hm_load_ap_caps(0);
    }
  }

  if (params->mode == WPAS_MODE_INFRA) {

#ifdef ENABLE_11R
    scanConfirm_nonrom_t *scanCon_nonrom;
    scanCon_nonrom = (scanConfirm_nonrom_t *)(join_frame + sizeof(rsi_uJoin));
    scanCon        = &scanCon_nonrom->scan_confirm;
#else
    scanCon = (scanConfirm_t *)(join_frame + sizeof(rsi_uJoin));
#endif
    scanCon->rssiVal = params->rssi_val;
    {
      bss        = (struct wpa_bss *)wpa_bss_get_wrapper(wpa_s, params->bssid, params->ssid, params->ssid_len);
      bss_beacon = bss + 1;
      sl_memcpy((uint8 *)&scanCon->TimeStamp, (uint8 *)&bss->tsf, 8);
      sl_memcpy((uint8 *)scanCon->bssid, (uint8 *)params->bssid, 6);
      *(uint16 *)&scanCon->Beacon         = bss->beacon_int;
      *(uint16 *)&scanCon->CapabilityInfo = bss->caps;
#ifdef ENABLE_11AX
      sl_memcpy((uint8 *)&scanCon_nonrom->mbssid_params, (uint8 *)&bss->mbssid_params, sizeof(sl_mbssid_params_t));
#endif
#ifdef RSI_ENABLE_CCX
      // Reset all CCX variables
      scanCon->ccx_ver_ie = 0; // CCX version Present
      scanCon->tsf        = params->tsf;
      scanCon->mic        = params->mic;
      scanCon->rn         = params->rn;

      mgmt_if_adapter.ccx_capable = SL_FALSE;
#endif
#ifdef ENABLE_11R
      parse_beacon(scanCon, (uint8 *)bss_beacon, bss->ie_len, channel, scanCon_nonrom);
#else
      parse_beacon(scanCon, (uint8 *)bss_beacon, bss->ie_len, channel);
#endif
    }
#ifdef CHIP_9117
    if (scanCon_nonrom->ap_rrm_capable) {
      wpa_s->rrm.rrm_used = 1;
    }
#endif

#ifdef ENABLE_11R
    join_frame_len = sizeof(rsi_uJoin) + sizeof(scanConfirm_nonrom_t);
#else
    join_frame_len = sizeof(rsi_uJoin) + sizeof(scanConfirm_t);
#endif
  } else {
    join_frame_len = sizeof(rsi_uJoin);
  }

  if (params->freq.freq) {

    if (mgmt_if_adapter.current_freq_band != GET_FREQ_BAND(params->freq.freq)) {
      /* Save channel frequency */
      mgmt_if_adapter.current_freq_band = GET_FREQ_BAND(params->freq.freq); /*WFD 5GHz change*/

      if (mgmt_if_adapter.operating_mode == WISE_MODE_P2P) {
        mgmt_hm_switch_band(&mgmt_if_adapter.current_freq_band);
      }

      if (mgmt_if_adapter.current_freq_band == SL_BAND_5GHZ
          && (mgmt_if_adapter.operating_mode == WISE_MODE_P2P || mgmt_if_adapter.dual_band == SL_BAND_DUAL)) {
        uJoinFrame->joinFrameSnd.ibssChannel = ((uJoinFrame->joinFrameSnd.ibssChannel >= 36)
                                                  ? uJoinFrame->joinFrameSnd.ibssChannel
                                                  : translate_channel(uJoinFrame->joinFrameSnd.ibssChannel));
        mgmt_if_adapter.join_channel         = uJoinFrame->joinFrameSnd.ibssChannel;
      }
    }

    //! Switch band if dual band, sme should be sink with UMAC
    if (mgmt_if_adapter.dual_band == SL_BAND_DUAL) {
      mgmt_hm_switch_band(&mgmt_if_adapter.current_freq_band);
    }
#ifdef ENABLE_11R
    if ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_ENABLE_11R_ODS) && (mgmt_if_adapter.roam_ind == 1)) {
      // Send Target AP detail to the sme
      mgmt_hm_send_target_ap_info(scanCon);
      //! Free the malloc memory as it is no longer required
      sl_free(join_frame);

      uint8 target_ap[ETH_ALEN];
      struct wpa_bss *bss;
      const uint8 *mdie;
      sl_memcpy(target_ap, (uint8 *)params->bssid, 6);

      //wpa_printf(MSG_DEBUG, "CTRL_IFACE FT_DS " MACSTR, MAC2STR(target_ap));

      bss = wpa_bss_get_bssid_wrapper(wpa_s, target_ap);
      if (bss)
        mdie = wpa_bss_get_ie(bss, WLAN_EID_MOBILITY_DOMAIN);
      else
        mdie = NULL;

      return wpa_ft_start_over_ds_wrapper(wpa_s->wpa, target_ap, mdie);
    } else
#endif
    {
      //! Send join request frame
      mgmt_hm_join(join_frame, join_frame_len);
    }
  }

  sl_free(join_frame);
  return 0;
}

void *get_wpa_supp_conf_ssid(void *wpa_s)
{
  return (void *)(((struct wpa_supplicant *)wpa_s)->conf->ssid);
}

void wpa_s_conf_init_for_rejoin(void *ctx, void *connect_without_scan)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  wpa_s->conf->num_prio        = 1;
  wpa_s->conf->pssid[0]        = (struct wpa_ssid *)connect_without_scan;
  sl_memzero(&wpa_s->conf->ssid->disabled_until, sizeof(struct os_time));
}

void wpa_update_conf_pmf(void *ctx, uint8 pmf)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  wpa_s->conf->pmf             = pmf;
}

void wpa_update_conf_sae_pwe(void *ctx, uint8 sae_pwe)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  wpa_s->conf->sae_pwe         = sae_pwe;
}

void wpa_update_conf_okc(void *ctx, int okc)
{
  struct wpa_supplicant *wpa_s = (struct wpa_supplicant *)ctx;
  wpa_s->conf->okc             = okc;
}

void trigger_rom2_event_deauth_wrapper(void *wpa_s)
{
  union wpa_event_data data;
  wpa_supplicant_event((struct wpa_supplicant *)wpa_s, EVENT_DEAUTH, &data);
}

void trigger_rom2_event_disassoc_wrapper(void *wpa_s)
{
  union wpa_event_data data;
  wpa_supplicant_event((struct wpa_supplicant *)wpa_s, EVENT_DISASSOC, &data);
}

void set_wpa_disconnected(void *wpa_s, uint8 value)
{
  ((struct wpa_supplicant *)wpa_s)->disconnected = value;
}

void set_wpa_auto_reconnect_disabled(void *wpa_s, uint8 value)
{
  ((struct wpa_supplicant *)wpa_s)->auto_reconnect_disabled = value;
}

int get_wpa_auto_reconnect_disabled(void *wpa_s)
{
  return (((struct wpa_supplicant *)wpa_s)->auto_reconnect_disabled);
}

int16 mgmtif_send_ext_auth()
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s = NULL;

  wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL)
    return WLAN_STATUS_FAILURE;

  data.external_auth.action         = EXT_AUTH_START;
  data.external_auth.key_mgmt_suite = RSN_AUTH_KEY_MGMT_SAE;
  data.external_auth.ssid           = sl_get_ssid_from_mgmt_if_adapter();
  data.external_auth.bssid          = get_bssid_from_mgmt_intf_adapter();
  data.external_auth.ssid_len       = mgmt_if_adapter.ssid_len;
  wpa_supplicant_event_wrapper(wpa_s, EVENT_EXTERNAL_AUTH, &data);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_send_sae_auth_frame(uint8 *sae_auth_frame,
                                 size_t sae_auth_frame_len,
                                 uint16 authSeqNum,
                                 ieee80211_macframe_t *hdr_mac,
                                 uint8 vap_id)
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s = NULL;
  struct ap_cb_s *ap           = (struct ap_cb_s *)&ap_cb;
  struct sta_info_s *sta       = &ap->sta_cb[0];
  uint8 ii                     = 0;

  if (sae_auth_frame == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  /*Triggers supplicant event to handle WPA3 AUTH in case of AP alone mode and AP in concurrent mode*/
  if (mgmt_if_adapter.operating_mode == WISE_MODE_AP
      || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 1))) {
    if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 1)) {
      /*Updates AP supplicant interface in concurrent mode */
      wpa_s = mgmt_if_adapter.ap_supplicant_priv;
    }
    /*Adds new WPA3 STA in ap_cb*/
    if (authSeqNum == 1) {
      for (ii = 0; ii < mgmt_if_adapter.max_station_supported; ii++) {
        if (!(sta->state & BIT(0))) {
          sl_memset((void *)sta, 0, sizeof(struct sta_info_s));
          sl_memcpy(sta->mac, hdr_mac->addr2, 6);

          sta->state |= BIT(0);
          mgmt_if_adapter.sta_count++;

          break;
        }

        sta++;
      }
    }

    data.rx_mgmt.frame     = sae_auth_frame;
    data.rx_mgmt.frame_len = sae_auth_frame_len;

    wpa_supplicant_event_wrapper(wpa_s, EVENT_NEW_PEER_CANDIDATE, &data);
  } else if ((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION)
             || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id == 0))) {
    sme_external_auth_mgmt_rx_wrapper(wpa_s, sae_auth_frame, sae_auth_frame_len);
  }

  return WLAN_STATUS_SUCCESS;
}

/*==============================================*/
/**
 *
 * @fn         wpas_mbo_update_non_pref_chan_wrapper
 * @brief      This function parses non-preferred channels
 * @param[in]  wpa_supplicant info structure
 * @param[in]  non-preferred channel parameter structure
 * @return     0               - Success
 *             Non-Zero  Value - Failure
 * @section description
 * This function parses non-preferred channels configured by user, does
 * validation and updates them.
 */
int wpas_mbo_update_non_pref_chan_wrapper(void *wpa_ctx, rsi_set_non_pref_chan_info_t *non_pref_chan_config)
{
  struct wpa_supplicant *wpa_s           = (struct wpa_supplicant *)wpa_ctx;
  struct wpa_mbo_non_pref_channel *chans = NULL, *tmp_chans = NULL;
  size_t num = 0, size = 0;
  unsigned i;

  uint8 channel_num = non_pref_chan_config->channel_num;
  while (num < channel_num) {
    struct wpa_mbo_non_pref_channel *chan;
    if (num == size) {
      size      = size ? size * 2 : 1;
      tmp_chans = os_realloc_array(chans, size, sizeof(*chans));
      if (!tmp_chans) {
        goto fail;
      }
      chans = tmp_chans;
    }
    chan             = &chans[num];
    chan->oper_class = non_pref_chan_config->mbo_non_pref_chan[num].operating_class;
    chan->chan       = non_pref_chan_config->mbo_non_pref_chan[num].channel;
    chan->reason     = non_pref_chan_config->mbo_non_pref_chan[num].reason;
    chan->preference = non_pref_chan_config->mbo_non_pref_chan[num].preference;

    if (wpas_mbo_validate_non_pref_chan_wrapper(chan->oper_class, chan->chan, chan->reason)) {
      goto fail;
    }
    for (i = 0; i < num; i++)
      if (wpa_non_pref_chan_is_eq_wrapper(chan, &chans[i]))
        break;
    if (i != num) {
      goto fail;
    }
    num++;
  }
  if (chans) {
    qsort(chans, num, sizeof(struct wpa_mbo_non_pref_channel), wpa_non_pref_chan_cmp_wrapper);
  }
  sl_free(wpa_s->non_pref_chan);
  wpa_s->non_pref_chan     = chans;
  wpa_s->non_pref_chan_num = num;
  wpas_mbo_non_pref_chan_changed_wrapper(wpa_s);
  return 0;
fail:
  sl_free(chans);
  return -1;
}

int16 mgmtif_send_mic_failure()
{
  if (mgmt_if_adapter.supplicant_priv == NULL)
    return WLAN_STATUS_FAILURE;

  //wpa_printf(1, "OBE-MIC_FAIL: Starting MIC failure investigation");
  wpa_supplicant_event_wrapper(mgmt_if_adapter.supplicant_priv, EVENT_MICHAEL_MIC_FAILURE, NULL);
  /*Add assertion of MIC failure to send msg on UART2*/
  SLI_WLAN_MGMT_ASSERT(UMAC_ASSERT_MIC_FAILURE);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_apconfig_resp(uint8 *rxPkt, uint32 token)
{
  struct wpa_supplicant *wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    if (mgmt_if_adapter.dyn_sta_ap_switch_enable && (mgmt_if_adapter.ap_supplicant_priv == NULL)) {
      ap_supp_start(wpa_s->global);
    }
    wise_update_concurrent_state(WISE_CON_STATE_APCONF_DONE);
  } else {
    wise_update_state(WISE_STATE_APCONF_DONE);
  }

#ifdef ENABLE_ACS
  if (token == 0 && mgmt_if_adapter.apconfig.channel_no == 0) {
    mgmt_if_adapter.apconfig.channel_no = *(uint16 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0);
  }
#endif

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_channel_cmd_resp(uint16 rcv_channel)
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s = NULL;
  int rcv_freq;

  wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL)
    return WLAN_STATUS_FAILURE;

  if (mgmt_if_adapter.operating_mode == WISE_MODE_P2P && mgmt_if_adapter.current_freq_band && rcv_channel < 36) {
    rcv_channel = translate_channel(rcv_channel);
  }

  rcv_freq = get_frequency(mgmt_if_adapter.current_freq_band,
                           rcv_channel,
                           ((mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211J)
                            && (mgmt_if_adapter.region_code == REGION_JAP_NUM)));

  sl_memset((uint8 *)&data, 0, sizeof(union wpa_event_data));
  data.remain_on_channel.freq     = rcv_freq;
  data.remain_on_channel.duration = 0;
  wpa_supplicant_event_wrapper(wpa_s, EVENT_REMAIN_ON_CHANNEL, &data);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_scan_resp(uint8 *send_to_host, uint32 token)
{
  struct wpa_supplicant *wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (((mgmt_if_adapter.operating_mode == WISE_MODE_CONNECTION) || (mgmt_if_adapter.operating_mode == WISE_MODE_EAP)
       || (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE))
      && !mgmt_if_adapter.rejoin_going_on) {
    mgmt_if_adapter.passive_scan_bitmap = 0;
    *send_to_host                       = mgmt_if_adapter.scan_req_from_host;

    if ((mgmt_if_adapter.state < WISE_STATE_CONNECTED) && (mgmt_if_adapter.state > WISE_STATE_BAND_DONE)) {
      wise_update_state(WISE_STATE_SCAN_DONE);
    }

    if (*send_to_host && !token) {
      get_scan_results(0);
      *send_to_host = 0;
    }
    mgmt_if_adapter.scan_req_from_host = 0;

    if ((is_wps_client() == 1)
        || (mgmt_if_adapter.operating_mode == WISE_MODE_EAP && (wpa_s->eap_expected_failure == 1))) {
      *send_to_host = 0;
      wpa_supplicant_event_wrapper(wpa_s, EVENT_SCAN_RESULTS, NULL);
    }
  } else { /* P2P */
    /* NULL has to be reviewed - when to indicate the new results */
    wpa_supplicant_event_wrapper(wpa_s, EVENT_SCAN_RESULTS, NULL);
  }

  return WLAN_STATUS_SUCCESS;
}

#ifdef ENABLE_11R
int16 mgmtif_auth_ft_ie_rsp(uint8 *ft_ies, uint16 allocation_size, uint32 ft_action)
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s  = mgmt_if_adapter.supplicant_priv;
  union wpa_event_data *ft_data = &data;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  sl_memzero(ft_data, sizeof(*ft_data));
  ft_data->ft_ies.ies       = ft_ies;
  ft_data->ft_ies.ies_len   = allocation_size;
  ft_data->ft_ies.ft_action = ft_action;
  sl_memcpy(ft_data->ft_ies.target_ap, mgmt_if_adapter.roam_bssid, BSSID_LENGTH);
  wpa_supplicant_event_wrapper(wpa_s, EVENT_FT_RESPONSE, ft_data);

  return WLAN_STATUS_SUCCESS;
}
#endif /* ENABLE_11R */

int16 mgmtif_process_bgscan()
{
  struct wpa_supplicant *wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (mgmt_if_adapter.roam_params.roam_enable != 1 && !mgmt_if_adapter.supplicant_triggered_bg_scan) {
    mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_CURRENT_AP_IS_BEST;
    mgmt_if_adapter.async_reason_code = RSI_MODULE_STATE_ROAMING_NOT_ENABLED;
    sl_memcpy(mgmt_if_adapter.async_bssid, get_bssid_from_mgmt_intf_adapter(), BSSID_LENGTH);
    wise_module_state(RSI_MODULE_STATE_SCAN_RESULT);
    return WLAN_STATUS_SUCCESS;
  }

  if (mgmt_if_adapter.execute_bg_scan_once) {
    trigger_bg_scan(0, 0, STOP);
    mgmt_if_adapter.execute_bg_scan_once         = 0;
    mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
  }
  wpa_supplicant_event_wrapper(wpa_s, EVENT_SCAN_RESULTS, NULL);

  return WLAN_STATUS_SUCCESS;
}

int16 mgmtif_process_remove_sta(uint8 *buf)
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s = NULL;
  uint8 ii                     = 0;
  struct sta_info_s *sta       = &ap_cb.sta_cb[0];

  if (buf == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.ap_supplicant_priv;
  }

  sl_memzero((uint8 *)&data, sizeof(union wpa_event_data));
  data.disassoc_info.addr = buf;
  for (ii = 0; ii < mgmt_if_adapter.max_station_supported; ii++) {
    if ((sta->state & BIT(0)) && !sl_memcmp(sta->mac, buf, 6)) {
      sta->state &= ~BIT(0);
      mgmt_if_adapter.sta_count--;
      if (buf[6] == IEEE80211_FC0_SUBTYPE_DISASSOC) {
        wpa_supplicant_event_wrapper(wpa_s, EVENT_DISASSOC, &data);
      } else {
        wpa_supplicant_event_wrapper(wpa_s, EVENT_DEAUTH, &data);
      }
#ifdef DATA_PATH_UMAC_ENABLE
#ifdef DHCPV4_SERVER_ENABLE
      rsi_dhcp_release(buf);
#endif /*DHCPV4_SERVER_ENABLE*/
#endif /*DATA_PATH_UMAC_ENABLE*/
#ifdef SOCKET_CLOSE_IN_AP_MODE
      free_sockets(mgmt_if_adapter.ap_mode_set, buf, 1);
#endif
      break;
    }
    sta++;
  }

  return WLAN_STATUS_SUCCESS;
}

// TODO: Need to refactor as a part of modularization
int16 mgmtif_process_join_resp(uint8 *rxPkt, uint8 *send_to_host, uint8 *default_case)
{
  int16 retval                 = 0;
  struct wpa_supplicant *wpa_s = mgmt_if_adapter.supplicant_priv;
  uint8 vap_id                 = 0;
  union wpa_event_data data;

  if (rxPkt == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    /*Keeping vap_id here for the responses*/
    vap_id = SL_PKT_RX_HDESC_GET_DW1_B3(rxPkt);
  }

  sl_memzero((uint8 *)&data, sizeof(union wpa_event_data));
  if (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == SAE_TRIGGER) {
    SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);
    mgmtif_send_ext_auth();
    return WLAN_STATUS_SUCCESS;
  }

  if (((mgmt_if_adapter.operating_mode != WISE_CONCURRENT_MODE) && (mgmt_if_adapter.ap_mode_set || (wpa_s == NULL)))
      || ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && (vap_id || (wpa_s == NULL)))) {
    if ((vap_id != 0) && (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == CONCURRENT_AP_AND_STA_CHANNEL_MISMATCH)) {
      *send_to_host = 1;
      SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);
    }
    return WLAN_STATUS_SUCCESS;
  }
  if (is_wps_client() && (wpa_s->wps_success == 1)
      && (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == DEAUTH_FROM_AP
          || SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == SUPP_DEAUTH)) {
    mgmt_if_adapter.wps_session                  = SL_FALSE;
    mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
    mgmt_if_adapter.is_11w_enabled               = 0;

    sl_strcpy(mgmt_if_adapter.join_ssid, wpa_s->conf->ssid->ssid);
    mgmt_if_adapter.scan_ssid_len = wpa_s->conf->ssid->ssid_len;
    sl_memcpy(mgmt_if_adapter.scan_ssid, wpa_s->conf->ssid->ssid, mgmt_if_adapter.scan_ssid_len);

    sl_memcpy(mgmt_if_adapter.pmk, wpa_s->conf->ssid->psk, SL_PMK_LEN);

    if (wpa_s->conf->ssid->key_mgmt == WPA_KEY_MGMT_NONE) {
      mgmt_if_adapter.csec_mode = SEC_MODE_OPEN;
    } else if (wpa_s->conf->ssid->key_mgmt == WPA_KEY_MGMT_PSK) {
      mgmt_if_adapter.csec_mode = SEC_MODE_WPA2;
    }

    wpa_supplicant_event_wrapper(wpa_s, EVENT_DEAUTH, &data);
    return WLAN_STATUS_SUCCESS;
  }

  if (mgmt_if_adapter.operating_mode == WISE_MODE_EAP && (wpa_s->eap_expected_failure == 1)
      && (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) != 0)) {
    mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
    mgmt_if_adapter.is_11w_enabled               = 0;
    wpa_supplicant_event_wrapper(wpa_s, EVENT_DEAUTH, &data);
    return WLAN_STATUS_SUCCESS;
  }

  if (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == 0) {
#ifdef RSI_ENABLE_CCX
    if (SL_PKT_RX_HDESC_GET_DW1_B2(rxPkt) == 1) {
      sl_memcpy(&data,
                (uint8 *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt) + 16),
                SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
      wpa_supplicant_event_wrapper(wpa_s, EVENT_CCKM_REASSOC_RESP, &data);
    }
#endif
#ifdef ENABLE_11R
    else if (SL_PKT_RX_HDESC_GET_DW1_B2(rxPkt) == 2) {
      union wpa_event_data *temp = &data;
      uint8 *assoc_resp_ie       = sl_zalloc(SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
      if (assoc_resp_ie) {
        sl_memzero(temp, sizeof(union wpa_event_data));
        sl_memcpy(assoc_resp_ie,
                  (uint8 *)((uint32)SL_PKT_GET_RXPKT_HDESC_ADDR(rxPkt) + 16),
                  SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
        temp->assoc_info.resp_ies     = assoc_resp_ie;
        temp->assoc_info.resp_ies_len = (SL_PKT_RX_HDESC_GET_DW0_LENGTH(rxPkt));
        //wpa_printf(1, "OBE_DEBUG:Calling Assoc Event");
        if (mgmt_if_adapter.roam_ind) {
          //wpa_printf(1, "OBE_DEBUG:Calling validate reassoc");
          if (wpa_ft_validate_reassoc_resp_wrapper(wpa_s->wpa,
                                                   temp->assoc_info.resp_ies,
                                                   temp->assoc_info.resp_ies_len,
                                                   mgmt_if_adapter.roam_bssid)
              < 0) {
            SL_PRINTF(WLAN_SUPP_FT_VALIDATION_OF_REASSOCIATION_RESPONSE_FAILED, WLAN_UMAC, LOG_INFO);
          }
        }
        wpa_supplicant_event_wrapper(wpa_s, EVENT_ASSOC, temp);

        sl_free(assoc_resp_ie);
        return WLAN_STATUS_SUCCESS;
      }
    }
#endif
    sl_memset((uint8 *)&data, 0, sizeof(union wpa_event_data));
    wpa_supplicant_event_wrapper(wpa_s, EVENT_ASSOC, &data);
  } else {
    if (SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt) == SLI_WLAN_INVALID_PMKID) {
      if (wpa_s->current_ssid && wpa_key_mgmt_sae(wpa_s->current_ssid->key_mgmt)) {
        wpa_sm_aborted_cached(wpa_s->wpa);
        wpa_sm_pmksa_cache_flush(wpa_s->wpa, wpa_s->current_ssid);
      }
    }

    if (mgmt_if_adapter.rejoin_going_on == SL_FALSE) {
      /* Preserve the previous WSC state */
      mgmt_if_adapter.prev_state = mgmt_if_adapter.state;
    }

    wise_update_state(WISE_STATE_INIT_DONE);

    /*TODO Give buffer full to host */
    if (mgmt_if_adapter.operating_mode != WISE_MODE_P2P) {
      mgmt_if_adapter.roam_ind = 0;

      if (!get_first_time_retry_enable() && mgmt_if_adapter.join_from_host) {
        wpa_s->auto_reconnect_disabled = 1;
      } else {
        wpa_s->auto_reconnect_disabled = 0; /*Start rejoin process*/
      }
      mgmt_if_adapter.async_state_code  = RSI_MODULE_STATE_UNASSOCIATED;
      mgmt_if_adapter.async_reason_code = rsi_find_reason_code(SL_PKT_RX_HDESC_GET_DW3_TOKEN(rxPkt));
      mgmt_if_adapter.async_state_code |= (mgmt_if_adapter.async_reason_code & 0xF);
      if (mgmt_if_adapter.async_reason_code == RSI_MODULE_STATE_DEAUTH_FROM_AP) {
        mgmt_if_adapter.async_reason_code = (*(uint8 *)SL_PKT_GET_RXPKT_SCATTER_BUFF_ADDR(rxPkt, 0));
      }
      sl_memcpy(mgmt_if_adapter.async_bssid, get_bssid_from_mgmt_intf_adapter(), MAC_ADDR_LEN);
      if (mgmt_if_adapter.custom_feature_bit_map & FEAT_SEL_ENABLE_ASYNC_MESSAGE) {
        wise_module_state(RSI_MODULE_STATE_FINAL_CONNECTON);
      }
      /* Now rejoin will start so update the status for rejoin/retry */
      mgmt_if_adapter.async_state_code = RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE;
      if ((mgmt_if_adapter.async_reason_code & RSI_MODULE_STATE_BEACON_LOSS)
          || (mgmt_if_adapter.async_reason_code & RSI_MODULE_STATE_DEAUTH_FROM_AP)) {
        mgmt_if_adapter.async_state_code = mgmt_if_adapter.async_reason_code;
      }
    } else {
      if (mgmt_if_adapter.operating_mode == WISE_MODE_P2P) {
        /*Rejoin is not aplicable in P2P*/
        wpa_s->auto_reconnect_disabled = 1;
      }
    }

    if (!is_rejoin_happenig()) {
      wise_update_state(WISE_STATE_BAND_DONE);
      rsi_rejoin_fail_to_lmac();
      *send_to_host = 1;
    } else {
#ifdef ENABLE_FAST_PSP
      /*Keep backup of Fast psp stuff if enable*/
      if (rsiFastPsp.fast_psp_enable == RSI_FAST_PSP_MODE) {
        rsiFastPsp.sleep_type           = 0;
        rsiFastPsp.ulp_mode_enable      = 0;
        rsiFastPsp.listen_interval_dtim = 0;
        rsiFastPsp.state                = FAST_PSP_IDLE;
      }
#endif
      if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
        mgmt_if_adapter.con_operating_mode = WISE_MODE_CONNECTION;
      }

      mgmt_if_adapter.supplicant_triggered_bg_scan = 0;
      mgmt_if_adapter.is_11w_enabled               = 0;
      wpa_supplicant_event_wrapper(wpa_s, EVENT_DEAUTH, &data);
    }
  }
  SL_PKT_RX_HDESC_SET_DW3_QUEUE_ID(rxPkt, 1);

  return retval;
}

/*==============================================*/
/**
 * @fn          send_pmkid_validation_status_to_sme
 * @brief       This function is used to send pmkid validation to SME layer. 
 * @param[in]   buf, input buffer which has STA mac, islegacy, assoc_type and ies
 * @param[in]   msg_len, length of the IEs 
 * @param[in]   pmkid_status, status of pmkid processing
 * @section description
 * This function create txpkt that has PMKID validation status and sending it to SME.
 */
static uint8 send_pmkid_validation_status_to_sme(uint8 *buf, uint16 msg_len, uint16 pmkid_status)
{
  /* Buffer to store data */
  sme_assoc_resp_t resp_data;

  sl_memzero(&resp_data, sizeof(resp_data) + msg_len);

  resp_data.pmkid_status       = pmkid_status;
  resp_data.assoc_reassoc_type = buf[7];
  resp_data.is_legacy          = buf[6];

  sl_memcpy(resp_data.mac_addr, buf, MAC_ADDR_LEN);

  resp_data.ies_len = msg_len;

  sl_memcpy(resp_data.ie, buf + 8, msg_len);
  mgmt_hm_send_pmkid_validation_status((uint8 *)&resp_data, sizeof(resp_data) + msg_len);

  return 0;
}

void process_assoc_req_pmkid_req(uint8 *rx_pkt)
{
  uint16 pmkid_resp            = 0;
  struct sta_info *supp_sta    = NULL;
  struct hostapd_data *hapd    = NULL;
  struct wpa_supplicant *wpa_s = NULL;
  uint8 *buf                   = NULL;
  int16 msg_len                = 0;
  uint8 *uCmdRspBuf            = (uint8 *)SL_PKT_GET_RXPKT_HDESC_ADDR(rx_pkt);
  struct ap_cb_s *ap           = (struct ap_cb_s *)&ap_cb;

  struct sta_info_s *sta = &ap->sta_cb[0];
  uint8 ii               = 0;

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.ap_supplicant_priv;
  }

  hapd = (struct hostapd_data *)(wpa_s->ap_iface->bss[0]);
  /*buf has info in this order STA mac(6 bytes), islegacy(1 byte), type(1 byte) and ies(vaiable len) */
  buf     = (uint8 *)((uint8 *)(rx_pkt)
                  + (SL_PKT_GET_RXPKT_HOST_DESC_OFFSET(rx_pkt) + HOST_DESC_LENGTH
                     + SL_PKT_RX_HDESC_GET_DW1_EXT_DESC_SIZE(rx_pkt)));
  msg_len = *(uint16 *)&uCmdRspBuf[0] & 0xfff;
  /*loop for the respective STA*/
  for (ii = 0; ii < mgmt_if_adapter.max_station_supported; ii++) {
    if (!(sta->state & BIT(0))) {
      sl_memset((void *)sta, 0, sizeof(struct sta_info_s));
      sl_memcpy(sta->mac, buf, 6);
      sta->state |= BIT(0);
      mgmt_if_adapter.sta_count++;
      supp_sta = ap_sta_add(hapd, sta->mac);
      /* Validates PMKID */
      pmkid_resp = check_assoc_ies(hapd, supp_sta, buf + 8, msg_len - 8, 0);
      send_pmkid_validation_status_to_sme(buf, msg_len - 8, pmkid_resp);
      break;
    }
    sta++;
  }
}

int16 mgmtif_process_add_sta_ind(uint8 *buf, uint16 ie_len)
{
  union wpa_event_data data;
  struct wpa_supplicant *wpa_s = NULL;
  uint8 ii                     = 0;
  struct sta_info_s *sta       = &ap_cb.sta_cb[0];

  if (buf == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  wpa_s = mgmt_if_adapter.supplicant_priv;

  if (wpa_s == NULL) {
    return WLAN_STATUS_FAILURE;
  }

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    wpa_s = (struct wpa_supplicant *)mgmt_if_adapter.ap_supplicant_priv;
  }

  sl_memset((uint8 *)&data, 0, sizeof(data));
  data.assoc_info.addr = (uint8 *)buf;

  if (ie_len > 0) {
    data.assoc_info.req_ies     = buf + MAC_ADDR_LEN;
    data.assoc_info.req_ies_len = ie_len;
  }

  for (ii = 0; ii < mgmt_if_adapter.max_station_supported; ii++) {
    /* If it is already there */
    if ((sta->state & BIT(0)) && !memcmp(sta->mac, buf, 6)) {
      sl_memcpy(sta->mac, buf, 6);
      wpa_supplicant_event_wrapper(wpa_s, EVENT_ASSOC, &data);
      return WLAN_STATUS_SUCCESS;
    }
    sta++;
  }

  /* Not found */
  if (ii == mgmt_if_adapter.max_station_supported) {
    sta = sta - mgmt_if_adapter.max_station_supported;
    for (ii = 0; ii < mgmt_if_adapter.max_station_supported; ii++) {
      if (!(sta->state & BIT(0))) {
        sl_memset((void *)sta, 0, sizeof(struct sta_info_s));
        sl_memcpy(sta->mac, buf, 6);
        sta->state |= BIT(0);
        mgmt_if_adapter.sta_count++;
        wpa_supplicant_event_wrapper(wpa_s, EVENT_ASSOC, &data);
        return WLAN_STATUS_SUCCESS;
      }
      sta++;
    }
  }

  return WLAN_STATUS_SUCCESS;
}

// This function contains various initialization of station mode
int16 start_sta(void *ctx, uint8 *psk, uint8 *ssid_str, uint8 auth_mode)
{
  struct wpa_supplicant *wpa_s = ctx;
  struct wpa_ssid *ssid;
  unsigned int length;
  uint8 tmp_psk[70];
  uint8 tmp_ssid[40];
  uint8 wep_key[20];
  uint8 wep_key_id[10];

  ssid = (struct wpa_ssid *)wpa_config_add_network_wrapper(wpa_s);

  if (ssid == NULL)
    return MEM_ALLOC_FAILED; /* Memory allocation failed ERROR_CODE*/

  wpas_notify_network_added_wrapper(wpa_s, ssid);

  wpa_config_set_network_defaults_wrapper(ssid);

  ssid->temporary = 1;
  tmp_ssid[0]     = '\"';
  length          = sl_strlen((char *)ssid_str);
  sl_memcpy(&tmp_ssid[1], ssid_str, length);
  tmp_ssid[1 + length]     = '\"';
  tmp_ssid[1 + 1 + length] = '\0';

  if (wpa_config_set_wrapper(ssid, "ssid", (char *)tmp_ssid, 0) < 0) {
    goto remove;
  }

  if (IS_AUTH_WEP_MODE(auth_mode)) { /* For WEP */
    sl_memset(tmp_psk, 0, 65);
    sl_strcpy(tmp_psk, mgmt_if_adapter.wep_key.key[mgmt_if_adapter.wep_key.index]);
    sl_strcpy(wep_key, (uint8 *)"wep_key");
    wep_key[7]    = mgmt_if_adapter.wep_key.index + '0';
    wep_key[8]    = '\0';
    wep_key_id[0] = mgmt_if_adapter.wep_key.index + '0';
    wep_key_id[1] = '\0';

    if ((wpa_config_set_wrapper(ssid, (char *)wep_key, (char *)tmp_psk, 0) < 0)
        || (wpa_config_set_wrapper(ssid, "key_mgmt", "NONE", 0) < 0)
        || (wpa_config_set_wrapper(ssid, "wep_tx_keyidx", (char *)wep_key_id, 0) < 0)) {
      goto remove;
    }
  } else {
    tmp_psk[0]         = '\"';
    uint8 key_mgmt[50] = { 0 };
    length             = sl_strlen((char *)psk);
    sl_memcpy(&tmp_psk[1], psk, length);
    tmp_psk[1 + length]     = '\"';
    tmp_psk[1 + 1 + length] = '\0';

    if (IS_AUTH_SEC_MODE(auth_mode) && ((sl_strlen((char *)psk) > 0) || mgmt_if_adapter.pmk_stored)) {

      //! Default key mgmt is WPA-PSK
      if (auth_mode != SEC_MODE_WPA3) {
        sl_memcpy(key_mgmt, "WPA-PSK", sizeof("WPA-PSK"));
      }

      if (mgmt_if_adapter.ext_custom_feature_bit_map & (EXT_FEAT_ENABLE_11R_ODS | EXT_FEAT_ENABLE_11R_OTA)) {
        strcat((char *)key_mgmt, " FT-PSK");
      }

      if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211W) {

        if (auth_mode == SEC_MODE_WPA3) {
          strcat((char *)key_mgmt, " SAE");
        } else if (auth_mode == SEC_MODE_WPA3_TRANSITION) {
          strcat((char *)key_mgmt, " WPA-PSK-SHA256 SAE");
        } else {
          strcat((char *)key_mgmt, " WPA-PSK-SHA256");
        }
      }

      if (auth_mode == SEC_MODE_WPA3) {
        strcat((char *)key_mgmt, " SAE");
      }

      if (wpa_config_set_wrapper(ssid, "key_mgmt", (const char *)key_mgmt, 0) < 0) {
        goto remove;
      }

      if ((wpa_config_set_wrapper(ssid, "psk", (char *)tmp_psk, 0) < 0)) {
        goto remove;
      }

      set_security_protocol(ssid, auth_mode);
    } else if (IS_AUTH_OPEN_MODE(auth_mode)) {

      if (wpa_config_set_wrapper(ssid, "key_mgmt", "NONE", 0) < 0) {
        goto remove;
      }
    }
  }

  // By default, both TKIP and CCMP ciphers are allowed (or set) in the
  // supplicant. If CCMP_ONLY is set, then this is overridden with CCMP only.
  if (mgmt_if_adapter.sta_encryption_mode == CCMP_ONLY) {
    if (wpa_config_set_wrapper(ssid, "pairwise", "CCMP", 0) < 0) {
      goto remove;
    }

    if (wpa_config_set_wrapper(ssid, "group", "CCMP", 0) < 0) {
      goto remove;
    }
  }

  return 0;

remove:
  wpas_notify_network_removed_wrapper(wpa_s, ssid);
  wpa_config_remove_network_wrapper(wpa_s, ssid->id);
  return JOIN_INFO_NOT_SENT; /*Join configuration failed ERROR_CODE*/
}

int ap_supp_start(void *wpa_ctx)
{
  int exitcode                 = 0;
  struct wpa_supplicant *wpa_s = wpa_ctx;
  /*Initializing supplicant for ap vaps*/
  global_iface.ifname          = glbl_ifname_ap;
  global_iface.confname        = glbl_confname_ap;
  mgmt_if_adapter.own_mac_type = 1; /*AP mac*/
  //wpa_printf(MSG_DEBUG, "Initializing supplicant VAP for AP mode");

  if (mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) {
    mgmt_if_adapter.ap_supplicant_priv = wpa_supplicant_add_iface_wrapper(wpa_s->global, &global_iface, NULL);

    if (mgmt_if_adapter.ap_supplicant_priv == NULL) {
      //wpa_printf(MSG_DEBUG, "Failed to initialize supplicant VAP for AP");
      exitcode = -1;
    }
  } else {
    mgmt_if_adapter.supplicant_priv = wpa_supplicant_add_iface_wrapper(wpa_s->global, &global_iface, NULL);

    if (mgmt_if_adapter.supplicant_priv == NULL) {
      //wpa_printf(MSG_DEBUG, "Failed to initialize supplicant VAP for AP");
      exitcode = -1;
    }
  }
  return exitcode;
}

int wsc_supp_start()
{
  struct wpa_interface iface;
  struct wpa_supplicant *wpa_s = NULL;
  int exitcode                 = 0;
  struct wpa_params params;
  struct wpa_global *global;
  uint32 status = SL_STATUS_OK;

  /* interface wrapper for Supplicant and driver */
  init_wsc_supp_interface();

  sl_memset(&params, 0, sizeof(params));
  params.wpa_debug_level = MSG_WARNING;

  global = wpa_supplicant_init_wrapper(&params);
  if (global == NULL)
    return -1;

  if (mgmt_if_adapter.supplicant_thread_started == SL_FALSE) {
    status = sli_supp_eloop_thread_init();
    if (status != SL_STATUS_OK) {
      SL_DEBUG_LOG("\r\nFailed to create supp thread : 0x%lX\r\n", status);
      mgmt_if_adapter.supplicant_thread_started = SL_FALSE;
      SL_MGMT_ASSERT(0);
    } else {
      mgmt_if_adapter.supplicant_thread_started = SL_TRUE;
    }
  }

  if (mgmt_if_adapter.dyn_sta_ap_switch_enable) {
    sl_memset(&global_iface, 0, sizeof(global_iface));
  } else {
    sl_memset(&iface, 0, sizeof(iface));
  }

  if (mgmt_if_adapter.dyn_sta_ap_switch_enable) {
    global_iface.ifname   = glbl_ifname;
    global_iface.confname = glbl_confname;
  } else {
    iface.ifname   = glbl_ifname;
    iface.confname = glbl_confname;
  }

  //wpa_printf(MSG_DEBUG, "Initializing supplicant VAP for station mode");
  if (mgmt_if_adapter.dyn_sta_ap_switch_enable) {
    mgmt_if_adapter.supplicant_priv = wpa_supplicant_add_iface_wrapper(global, &global_iface, NULL);
  } else {
    mgmt_if_adapter.supplicant_priv = wpa_supplicant_add_iface_wrapper(global, &iface, NULL);
  }
  if (mgmt_if_adapter.supplicant_priv == NULL) {
    //wpa_printf(MSG_DEBUG, "Failed to initializing supplicant VAP for station mode");
    exitcode = -1;
  }

  if (mgmt_if_adapter.dyn_sta_ap_switch_enable) {
    wpa_s = mgmt_if_adapter.supplicant_priv;
  }

  if ((mgmt_if_adapter.operating_mode == WISE_CONCURRENT_MODE) && !mgmt_if_adapter.ap_supplicant_priv) {
    if (mgmt_if_adapter.dyn_sta_ap_switch_enable) {
      exitcode = ap_supp_start(wpa_s->global);
    } else {
      /*Initializing supplicant for ap vaps*/
      iface.ifname                 = glbl_ifname_ap;
      iface.confname               = glbl_confname_ap;
      mgmt_if_adapter.own_mac_type = 1; /*AP mac*/
      //wpa_printf(MSG_DEBUG, "Initializing supplicant VAP for AP mode");
      mgmt_if_adapter.ap_supplicant_priv = wpa_supplicant_add_iface_wrapper(global, &iface, NULL);
      if (mgmt_if_adapter.ap_supplicant_priv == NULL) {
        //wpa_printf(MSG_DEBUG, "Failed to initialize supplicant VAP for AP");
        exitcode = -1;
      }
    }
  }
  return exitcode;
}

int16 start_enterprise_security(void *wpa_s, uint8 *bssid, uint8 *tmp_ssid, uint8 auth_mode)
{
  struct wpa_ssid *ssid;
  uint32 write_addr;
  uint8 key_passwd[30] = { 0 };
  ssid                 = (struct wpa_ssid *)wpa_config_add_network_wrapper(wpa_s);
  uint8_t phase1[100]  = { 0 };
  if (ssid == NULL)
    return MEM_ALLOC_FAILED;
  wpas_notify_network_added_wrapper(wpa_s, ssid);
  wpa_config_set_network_defaults_wrapper(ssid);
  ssid->temporary = 1;

#ifdef RSI_ENABLE_CCX
  uint8 key_mgmt[50] = { 0 };
  if (mgmt_if_adapter.join_feature_bitmap & JOIN_FEAT_KEY_MGMT_WPA_CCXV2) {
    sl_memcpy(key_mgmt, "WPA-CCKM", sizeof("WPA-CCKM"));
  } else {
    sl_memcpy(key_mgmt, "WPA-EAP", sizeof("WPA-EAP"));
    if (mgmt_if_adapter.ext_custom_feature_bit_map & (EXT_FEAT_ENABLE_11R_ODS | EXT_FEAT_ENABLE_11R_OTA)) {
      strcat((char *)key_mgmt, " FT-EAP");
    }

    if (mgmt_if_adapter.ext_custom_feature_bit_map & EXT_FEAT_IEEE_80211W) {
      strcat((char *)key_mgmt, " WPA-EAP-SHA256");
    }
  }
#endif
  sl_memcpy(phase1,
            "\"tls_disable_time_checks=1\" \"fast_provisioning=1\"",
            sizeof("\"tls_disable_time_checks=1\" \"fast_provisioning=1\""));
  if (sl_strcmp((int8_t *)mgmt_if_adapter.eap_method, "PEAPSAFE1") == 0) {
    sl_strcat((int8_t *)phase1, " \"phase2_auth=1\"");
    sl_memcpy((int8_t *)mgmt_if_adapter.eap_method, "PEAP", sizeof("PEAP"));
  } else if (sl_strcmp((int8_t *)mgmt_if_adapter.eap_method, "PEAPSAFE2") == 0) {
    sl_strcat((int8_t *)phase1, " \"phase2_auth=2\"");
    sl_memcpy((int8_t *)mgmt_if_adapter.eap_method, "PEAP", sizeof("PEAP"));
  } else if (sl_strcmp((int8_t *)mgmt_if_adapter.eap_method, "PEAP") == 0) {
    sl_strcat((int8_t *)phase1, " \"phase2_auth=0\"");
  }

  if (
#ifdef RSI_ENABLE_CCX
    wpa_config_set_wrapper(ssid, "key_mgmt", (const char *)key_mgmt, 0) < 0 ||
#else
    wpa_config_set_wrapper(ssid, "key_mgmt", "WPA-EAP", 0) < 0 ||
#endif
    wpa_config_set_wrapper(ssid, "ssid", (char *)tmp_ssid, 0) < 0
    || wpa_config_set_wrapper(ssid, "eap", (char *)mgmt_if_adapter.eap_method, 0) < 0
    || wpa_config_set_wrapper(ssid, "pac_file", "\"testuser.pac\"", 0) < 0
    || wpa_config_set_wrapper(ssid, "phase1", (const int8_t *)phase1, 0)
    || wpa_config_set_wrapper(ssid, "phase2", (char *)mgmt_if_adapter.inner_method, 0)
    || wpa_config_set_wrapper(ssid, "password", (char *)mgmt_if_adapter.passwd, 0) < 0
    || wpa_config_set_wrapper(ssid, "identity", (char *)mgmt_if_adapter.user_identity, 0) < 0) {
    wpas_notify_network_removed_wrapper(wpa_s, ssid);
    wpa_config_remove_network_wrapper(wpa_s, ssid->id);
    return EAP_CONFIG_FAIL; /* EAP Configuration failed ERROR_CODE */
  }

  write_addr = cpf_get_cert_flash_addr();

  if (*(uint32 *)write_addr != 0xFFFFFFFF) /* If certificate is loaded on to the flash */
  {
    if (sl_strcmp(mgmt_if_adapter.eap_method, (uint8 *)"TLS") == 0
        || sl_strcmp(mgmt_if_adapter.eap_method, (uint8 *)"TTLS") == 0
        || sl_strcmp(mgmt_if_adapter.eap_method, (uint8 *)"PEAP") == 0
        || sl_strcmp(mgmt_if_adapter.eap_method, (uint8 *)"LEAP") == 0) {
      if (wpa_config_set_wrapper(ssid, "ca_cert", "\"wifiuser.pem\"", 0) < 0) {
        wpas_notify_network_removed_wrapper(wpa_s, ssid);
        wpa_config_remove_network_wrapper(wpa_s, ssid->id);
        return EAP_CONFIG_FAIL; /* EAP Configuration failed ERROR_CODE */
      }
    }
    if (sl_strcmp(mgmt_if_adapter.eap_method, (uint8 *)"TLS") == 0) {
      if (wpa_config_set_wrapper(ssid, "client_cert", "\"wifiuser.pem\"", 0) < 0
          || wpa_config_set_wrapper(ssid, "private_key", "\"wifiuser.pem\"", 0) < 0
          || wpa_config_set_wrapper(ssid, "private_key_passwd", (char *)key_passwd, 0) < 0) {
        wpas_notify_network_removed_wrapper(wpa_s, ssid);
        wpa_config_remove_network_wrapper(wpa_s, ssid->id);
        return EAP_CONFIG_FAIL; /* EAP Configuration failed ERROR_CODE */
      }
    }

    if (sl_strlen((char *)(mgmt_if_adapter.private_key_passwd))
        && (wpa_config_set_wrapper(ssid, "private_key_passwd", (const char *)mgmt_if_adapter.private_key_passwd, 0)
            < 0)) {
      return EAP_CONFIG_FAIL; /* EAP Configuration failed ERROR_CODE */
    }
  }

  if (IS_AUTH_SEC_MODE(auth_mode)) {
    set_security_protocol(ssid, auth_mode);
  } else {
    wpas_notify_network_removed_wrapper(wpa_s, ssid);
    wpa_config_remove_network_wrapper(wpa_s, ssid->id);
    return EAP_CONFIG_FAIL; /* EAP Configuration failed ERROR_CODE */
  }
  if (bssid) {
    sl_memcpy(ssid->bssid, bssid, ETH_ALEN);
    ssid->bssid_set = 1;
  }

  return 0;
}

/**scan_results.c functions**/

/*==============================================*/
/* @fn        update_meta_data
 * @brief     This function updates scan result structure (to be copied to final
 * result in later stages) with given inputs.
 * @param[out] result, pointer to the structure to be filled.
 * @param[in] caps, pointer to the capabilities of the BSS
 * @param[in] bssid, MAC address of the AP
 * @param[in] new_bss, pointer to the information elements of BSS.
 * @param[in] ies_len, length of the total information elements.
 * @return    void
 */
static void update_meta_data(struct wpa_scan_res *result,
                             const uint8 *caps,
                             const uint8 *bssid,
                             const uint8 *new_bss,
                             uint16 ies_len,
                             uint8 *res_ssid_len)
{
  const uint8 *ie_ptr = NULL;
  const uint8 *ssid_  = NULL;
  uint8 ssid_len      = 0;
  if (result == NULL) {
    return;
  }
  sl_memzero(result->bssid, 6);
  sl_memcpy((uint8 *)&result->bssid, (uint8 *)bssid, ETH_ALEN);
  result->caps = *(uint16 *)&caps[0];
  if (((result->caps & BIT(0)) == 1) && ((result->caps & BIT(1)) == 0)) {
    result->uNetworkType = 1;
  } else {
    result->uNetworkType = 0;
  }
  if ((result->caps & BIT(4))) {
    result->security_mode = SEC_MODE_WEP;
  } else {
    result->security_mode = SEC_MODE_OPEN;
  }
  result->ie_len = ies_len;
  ie_ptr         = get_ie_wrapper(new_bss, ies_len, (uint8)WLAN_EID_SSID);
  if (ie_ptr != NULL) {
    ssid_ = ie_ptr ? ie_ptr + 2 : (const uint8 *)"";
    sl_memzero((uint8 *)&result->ssid, 34);
    if (res_ssid_len != NULL) {
      *res_ssid_len = ie_ptr ? ie_ptr[1] : 0;
      sl_memcpy((uint8 *)&result->ssid, (uint8 *)ssid_, *res_ssid_len);
    } else {
      ssid_len = ie_ptr ? ie_ptr[1] : 0;
      sl_memcpy((uint8 *)&result->ssid, (uint8 *)ssid_, ssid_len);
    }
  }
  return;
}

/*==============================================*/
/* @fn        update_bss_to_scan_results
 * @brief     This function adds a new bss or replaces existing bss with updated
 * information in the scan results.
 * @param[in] new_bss, pointer to new bss (Information Elements(IEs) from
 * beacon/probe resp)
 * @param[in] res, pointer to wpa_scan_res structure which contains specifics of
 * the BSS.
 * @return    -1 to indicate failure to update. Else success.
 */
static int16 update_bss_to_scan_results(const uint8 *new_bss, struct wpa_scan_res *res)
{
  const uint8 *ssid_ = NULL, *ie = NULL;
  ssid_ = ie                        = NULL;
  struct wpa_scan_res *bss          = NULL;
  uint16 ii                         = 0;
  uint8 ssid_len                    = 0;
  struct wpa_scan_results *scan_res = mgmt_if_adapter.scan_res;
  struct wpa_scan_res *result = NULL, **tmp = NULL, *temp_result = NULL;
  uint8 temp_val                            = 0;
  int (*compar)(const void *, const void *) = wpa_scan_result_compar;

  /* if memory is not allocated to hold scan results break */
  if ((scan_res == NULL) || (res == NULL)) {
    return -1;
  }
  /* Find if there is an entry already with same bssid */
  for (ii = 0; ii < scan_res->num; ii++) {
    bss      = scan_res->res[ii];
    ie       = get_ie_wrapper((uint8 *)(bss + 1), bss->ie_len, (uint8)WLAN_EID_SSID);
    ssid_    = ie ? ie + 2 : (const uint8 *)"";
    ssid_len = ie ? ie[1] : 0;

    if ((sl_memcmp(scan_res->res[ii]->bssid, res->bssid, ETH_ALEN) == 0)
        && (sl_strncmp((uint8 *)ssid_, (uint8 *)res->ssid, ssid_len) == 0)) {
      /* entry exist with same bssid, need to replace with new information*/
      result = scan_res->res[ii];
      break;
    }
  }

  if (result == NULL) {
    /* Try to get entry to hold new information from beacon & probe response */
    if (scan_res->num >= wpa_get_bss_max_cnt()) {
      /* if already maximum limit reached create space by deleting existing
       * entry with low rssi */
      qsort(scan_res->res, scan_res->num, sizeof(struct wpa_scan_res *), compar);
      if ((scan_res->res[scan_res->num - 1]->level < res->level)
          || ((mgmt_if_adapter.connected)
              && (sl_memcmp(res->bssid, get_bssid_from_mgmt_intf_adapter(), ETH_ALEN) == 0))) {
        if ((mgmt_if_adapter.connected)
            && (sl_memcmp(scan_res->res[scan_res->num - 1]->bssid, get_bssid_from_mgmt_intf_adapter(), ETH_ALEN)
                == 0)) {
          if (scan_res->res[scan_res->num - 2]->level > res->level) {
            return -1;
          }
          result   = scan_res->res[scan_res->num - 2];
          temp_val = scan_res->num - 2;
        } else {
          result   = scan_res->res[scan_res->num - 1];
          temp_val = scan_res->num - 1;
        }
      }
      temp_result = (struct wpa_scan_res *)sl_zalloc(sizeof(*result) + res->ie_len);
      if (temp_result == NULL) {
        return -1;
      }
      if (result != NULL) {
        sl_free(result);
      } else {
        sl_free(temp_result);
        return -1;
      }
      /* allocate new record to hold scan information */

      result = temp_result;
      /* place the new entry to the list */
      scan_res->res[temp_val] = result;
    } else {
      result = (struct wpa_scan_res *)sl_zalloc(sizeof(*result) + res->ie_len);
      if (result == NULL) {
        return -1;
      }
      tmp = sli_mgmt_realloc(scan_res->res, (scan_res->num + 1) * sizeof(struct wpa_scan_res *));
      if (tmp == NULL) {
        sl_free(result);
        return -1;
      }
      /* place the new entry to the list */
      tmp[scan_res->num] = result;
      scan_res->num++;
      scan_res->res = tmp;
    }
  } else {
    /*free the existing entry and alloc new entry to hold information from
     * received beacon or probe response. current beacon/probe reponse may have
     * more number of ies, so freeing existing current entry and allocting
     * memory for new entry.
     */
    temp_result = (struct wpa_scan_res *)sl_zalloc(sizeof(*result) + res->ie_len);
    if (temp_result == NULL) {
      return -1;
    }
    sl_free(result);
    result            = temp_result;
    scan_res->res[ii] = result;
  }

  if (res->freq == 0) {
    SLI_WLAN_MGMT_ASSERT(WSC_ASSERT_CHANNEL_NUM_ZERO);
  }
#ifdef ENABLE_DRAEGER_CUSTOMIZATION
  if (sl_memcmp(res->bssid, get_bssid_from_mgmt_intf_adapter(), MAC_ADDR_LEN) == 0) {
    mgmt_if_adapter.async_rssi    = res->rssiVal;
    mgmt_if_adapter.async_channel = res->channel_no;
  }
#endif
  /* copy res */
  sl_memcpy((uint8 *)result, res, sizeof(*res));
  sl_memcpy((uint8 *)(result + 1), (uint8 *)new_bss, res->ie_len);
  set_security_type(new_bss, res->ie_len, result, 0);
  qsort(scan_res->res, scan_res->num, sizeof(struct wpa_scan_res *), compar);
  return 1;
}

/*==============================================*/
/* @fn         process_tx_bss_scan_result
 * @brief      Fills wpa_scan_res structure with info from the beacon/probe resp
 * and calls update_bss_to_scan_results.
 * @param[in]  wsc_cb, pointer to global rsi_wsc_cb
 * @param[in]  buf, pointer to the start of the probe response frame.
 * @param[in]  ies_len, length of the all information elements
 * @param[out] result, pointer to wpa_scan_res structure to be filled.
 * @return     -1 to indicate failure
 */
int16 process_tx_bss_scan_result(const uint8 *buf, uint16 ies_len, uint8 *result_data)
{
  const uint8 *ie             = NULL;
  int16 ret_val               = 0;
  struct wpa_scan_res *result = (struct wpa_scan_res *)result_data;

  if (result == NULL) {
    return -1;
  }
#ifdef ENABLE_11AX
  if (!(mgmt_if_adapter.feature_bit_map & FEAT_DISABLE_11AX)) {
    result->mbssid_params.full_set_rx_per = 1;
    ie = get_ie_wrapper((uint8 *)&buf[BEACON_IES_START_OFFSET], ies_len, (uint8)WLAN_EID_MULTIPLE_BSSID);

    if (ie != NULL && ie[1] > 0) {
      result->mbssid_params.max_bssid_indicator = ie[2];
    } else {
      result->mbssid_params.max_bssid_indicator = 0;
    }

    result->mbssid_params.mbssid_index = 0;
  }
#endif /* ENABLE_11AX */
  update_meta_data(result, &buf[34], &buf[16], &buf[36], ies_len, NULL);
  ret_val = update_bss_to_scan_results(&buf[BEACON_IES_START_OFFSET], result);
  return ret_val;
}

/*==============================================*/
/* @fn         process_mbss_scan_results
 * @brief      Add multiple bss from beacon/probe response to the scan results.
 * @param[in]  buf, pointer to the start of the probe response mac header.
 * @param[in]  ie_len, length of all the IEs.
 * @param[in]  result, the wpa_scan_res structure to be filled with NT details.
 * Some of its members are prefilled.
 * @return     -1 to indicate failure
 */
int16 process_mbss_scan_results(uint8 *buf, uint16 ies_len, uint8 *result_data)
{
  const uint8 *mb_inx_ie = NULL, *ie_ptr = NULL;
  uint8 *new_bss = NULL, *nt_sub_profile = NULL;
  const struct element_s *info_elem = NULL, *sub_elem = NULL;
  uint8 nt_bssid[MAC_ADDR_LEN] = { 0 }, t_bssid[MAC_ADDR_LEN] = { 0 };
  uint16 nt_ies_len = 0, nt_profile_len = 0;
  uint64 parsed_nt_indices = 0;
  uint8 ret_val = 0, ssid_len = 0;
  struct wpa_scan_res *result = (struct wpa_scan_res *)result_data;

  if (result == NULL) {
    return -1;
  }
  // Check for MBSSID element and copy max_bssid_indicator
  ie_ptr = get_ie_wrapper((uint8 *)&buf[BEACON_IES_START_OFFSET], ies_len, (uint8)IEEE80211_ELEMID_MULTIPLE_BSSID);
  if (ie_ptr != NULL && ie_ptr[1] > 0) {
    result->mbssid_params.max_bssid_indicator = ie_ptr[2];
  } else {
    result->mbssid_params.max_bssid_indicator = 0;
    return -1;
  }
  // Get MBSSID configuration element
  ie_ptr =
    get_ie_ext_wrapper((uint8 *)&buf[BEACON_IES_START_OFFSET], ies_len, (uint8)IEEE80211_ELEMID_EXT_MBSSID_CONFIG);
  if (!ie_ptr && ie_ptr[1] == 3) {
    result->mbssid_params.full_set_rx_per = ie_ptr[4];
  } else {
    result->mbssid_params.full_set_rx_per = 1;
  }
  result->beacon_int *= result->mbssid_params.full_set_rx_per;

  new_bss = sl_zalloc(ies_len);
  if (new_bss == NULL) {
    return -1;
  }

  nt_sub_profile = sl_zalloc(ies_len);
  if (nt_sub_profile == NULL) {
    goto out;
  }

  sl_memcpy(t_bssid, &buf[16], 6);
  ie_ptr = &buf[BEACON_IES_START_OFFSET];
  ieee_for_each_element_id(info_elem, IEEE80211_ELEMID_MULTIPLE_BSSID, ie_ptr, ies_len)
  {
    if (info_elem->datalen < 4) {
      continue;
    }
    ieee_for_each_element(sub_elem, info_elem->data + 1, info_elem->datalen - 1)
    {
      // check for valid NT nt_sub_profile
      if ((sub_elem == NULL) || (sub_elem->id != 0) || (sub_elem->datalen < 4)
          || (sub_elem->data[0] != IEEE80211_ELEMID_NONTX_BSSID_CAP) || (sub_elem->data[1] != 2)) {
        continue;
      }

      sl_memzero((uint8 *)nt_sub_profile, ies_len);
      nt_profile_len = merge_nt_profile(ie_ptr, ies_len, info_elem, sub_elem, nt_sub_profile);
      // complete Nontransmitted BSSID Profile is obtained
      mb_inx_ie = get_ie_wrapper(nt_sub_profile, nt_profile_len, IEEE80211_ELEMID_MULTIPLE_BSSID_INDEX);
      // Check for valid multiple BSSID-Index element
      if ((mb_inx_ie == NULL) || (mb_inx_ie[1] < 1) || (mb_inx_ie[2] == 0)
          || (parsed_nt_indices & (1 << mb_inx_ie[2]))) {
        continue;
      }
      // mark parsed NT index to discard any duplicate index found
      parsed_nt_indices |= (1 << mb_inx_ie[2]);
      if (check_ssid_match(&nt_sub_profile[5])) {
        result->mbssid_params.mbssid_index = mb_inx_ie[2];
        sl_memzero(new_bss, ies_len); // new_bss initial length
        // generate IEs for NT profile
        nt_ies_len = generate_nt_bss(ie_ptr, ies_len, nt_sub_profile, nt_profile_len, new_bss);
        if (nt_ies_len == 0) {
          continue;
        } else if (nt_ies_len == -1) {
          ret_val = -1;
          goto out;
        }
        // derive NT BSSID from T bssid, mbssid index and max bssid indicator.
        derive_nt_bssid(t_bssid,
                        result->mbssid_params.max_bssid_indicator,
                        result->mbssid_params.mbssid_index,
                        nt_bssid);
        // Update the result structure
        update_meta_data(result, &nt_sub_profile[2], nt_bssid, new_bss, nt_ies_len, &ssid_len);
        // update bss to scan results
        ret_val = update_bss_to_scan_results(new_bss, result);
        if (ret_val == -1) {
          goto out;
        }
      }
    }
  }

out:
  if (new_bss) {
    sl_free(new_bss);
  }
  if (nt_sub_profile) {
    sl_free(nt_sub_profile);
  }
  return ret_val;
}
/**End of scan_results.c functions**/

void update_wpa_config_read_data(enum wpa_config_read_type type, uint8 *data, uint16 len)
{
  switch (type) {
    case WPA_CONFIG_READ_DEVICE_NAME:
      sl_memcpy(supp_mgmt_if_ctx.wpa_config_read_data.device_name, data, len);
      break;
    case WPA_CONFIG_READ_GO_INTENT:
      sl_memcpy(supp_mgmt_if_ctx.wpa_config_read_data.go_intent, data, len);
      break;
    case WPA_CONFIG_READ_SSID_POSTFIX:
      sl_memcpy(supp_mgmt_if_ctx.wpa_config_read_data.ssid_postfix, data, len);
      break;
    case WPA_CONFIG_READ_OPERATING_CHANNEL:
      sl_memcpy(supp_mgmt_if_ctx.wpa_config_read_data.operating_channel, data, len);
      break;
    case WPA_CONFIG_READ_MODULE_TYPE:
      sl_memcpy(&supp_mgmt_if_ctx.wpa_config_read_data.module_type, data, len);
      break;
    case WPA_CONFIG_READ_DUAL_BAND:
      sl_memcpy(&supp_mgmt_if_ctx.wpa_config_read_data.dual_band, data, len);
      break;
    case WPA_CONFIG_READ_SWITCH_BAND:
      sl_memcpy(&supp_mgmt_if_ctx.wpa_config_read_data.switch_band, data, len);
      break;
    default:
      break;
  }
}
