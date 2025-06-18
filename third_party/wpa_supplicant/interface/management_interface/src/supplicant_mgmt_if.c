/*******************************************************************************
* @file  supplicant_mgmt_if.c
* @brief This contains supplicant wrapper functions that call
*        Supplicant-interface functions.
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
#include "supplicant_mgmt_if.h"
#include "mgmt_if_core.h"
#ifdef DATA_PATH_UMAC_ENABLE
#include "rsi_wsc_non_rom.h"
#endif /* DATA_PATH_UMAC_ENABLE */
#include "mgmt_if_tx.h"

#include "common.h"
#include "wpa_supplicant_i.h" // TODO: Needs dependency removal. This is temporary

supplicant_mgmt_if_ctx_t supp_mgmt_if_ctx;

void sl_wpa_driver(void *ctx, unsigned int cmd, int *ret_val)
{
  int ret_val_bkp = 0;

  switch (cmd) {
    case WPA_DRV_CHK_CLIENT: {
      if (ret_val) {
        *ret_val = (int)is_client_mode();
      }
    } break;
    case WPA_DRV_CHK_REJOIN: {
      if (ret_val) {
        *ret_val = is_rejoin_happenig();
      }
    } break;
    case WPA_DRV_CHK_WPS_CLIENT: {
      if (ret_val) {
        *ret_val = (int)is_wps_client();
      }
    } break;
    case WPA_DRV_CHK_BGSCAN: {
      if (ret_val) {
        *ret_val = (int)is_bgscan_enabled();
      }
    } break;
    case WPA_DRV_CHK_P2P_MODE: {
      if (ret_val) {
        *ret_val = (int)is_p2p_mode();
      }
    } break;
    case WPA_DRV_CHK_CONNECTED_STATE: {
      if (ret_val) {
        *ret_val = (int)is_connected_state();
      }
    } break;
    case WPA_DRV_CONN_NOTIFY_TO_HOST: {
      rsi_sta_connected_notification_to_host_wrapper();
    } break;
    case WPA_DRV_NOTIFY_P2P_DEV_FOUND: {
      wise_notify_p2p_dev_found_wrapper();
    } break;
    case WPA_DRV_UPDATE_PSK: {
      wise_update_psk_in_nwparams_wrapper();
    } break;
    case WPA_DRV_SUPP_JOIN_DONE: {
      wsc_supp_join_done_wrapper();
    } break;
    case WPA_DRV_INDICATE_SAE_SUCC: {
      ret_val_bkp = (int)send_sae_success_indication();
      if (ret_val) {
        *ret_val = ret_val_bkp;
      }
    } break;
    case WPA_DRV_INDICATE_SAE_COMMITED_STATE: {
      ret_val_bkp = (int)mgmt_hm_indicate_sae_commit_state(supp_mgmt_if_ctx.is_sae_commited);
      if (ret_val) {
        *ret_val = ret_val_bkp;
      }
    } break;
    case WPA_DRV_TRIGGER_BG_SCAN: {
      set_supplicant_trigger_bg_scan(supp_mgmt_if_ctx.trigger_bg_scan);
    } break;
    case WPA_DRV_CHK_DYN_STA_AP_SWITCH: {
      if (ret_val) {
        *ret_val = (int)is_dyn_sta_ap_switch_enabled();
      }
    } break;
    case WPA_DRV_CHK_BAND_2_4GHZ: {
      if (ret_val) {
        *ret_val = (int)is_band_24ghz();
      }
    } break;
    case WPA_DRV_CHK_FEAT_BLACKLIST: {
      if (ret_val) {
        *ret_val = (int)is_blacklist_feature_enabled();
      }
    } break;
    case WPA_DRV_CHK_RSA_4096_SUPPORT: {
      if (ret_val) {
        *ret_val = (int)is_rsa_4096_supported();
      }
    } break;
    case WPA_DRV_SET_ROAM_INDICATION_AND_BSSID: {
      supp_set_roam_indication_and_bssid();
    } break;
    case WPA_DRV_SET_CLIENT: {
      supplicant_set_client();
    } break;
    case WPA_DRV_MAC_BASED_JOIN_MISMATCH: {
      if (ret_val) {
        *ret_val = (int)mac_based_join_mismatch_check(supp_mgmt_if_ctx.bssid);
      }
    } break;
    case WPA_DRV_CHK_PREV_BSSID_MISMATCH: {
      if (ret_val) {
        *ret_val = (int)prev_bssid_mismatch_check(supp_mgmt_if_ctx.bssid);
      }
    } break;
    case WPA_DRV_GET_REJOIN_SCAN_INTERVAL: {
      if (ret_val) {
        ret_val_bkp = (int)get_rejoin_scan_interval();
        if (ret_val_bkp != 0) {
          *ret_val = ret_val_bkp;
        }
      }
    } break;
    case WPA_DRV_CHK_EAP_OKC: {
      if (ret_val) {
        *ret_val = (int)is_eap_okc_enabled();
      }
    } break;
    case WPA_DRV_UPDATE_ASYNC_STATE_CODE: {
      mgmt_if_adapter.async_state_code = supp_mgmt_if_ctx.async_notif_args.state_code;
    } break;
    case WPA_DRV_UPDATE_ASYNC_REASON_CODE: {
      mgmt_if_adapter.async_reason_code = supp_mgmt_if_ctx.async_notif_args.reason_code;
    } break;
    case WPA_DRV_UPDATE_ASYNC_CHANNEL: {
      mgmt_if_adapter.async_channel = supp_mgmt_if_ctx.async_notif_args.channel;
    } break;
    case WPA_DRV_UPDATE_ASYNC_RSSI: {
      mgmt_if_adapter.async_rssi = supp_mgmt_if_ctx.async_notif_args.rssi;
    } break;
    case WPA_DRV_UPDATE_ASYNC_BSSID: {
      sl_memcpy(mgmt_if_adapter.async_bssid, supp_mgmt_if_ctx.async_notif_args.bssid, MAC_ADDR_LEN);
    } break;
    case WPA_DRV_ASYNC_NOTIF_AP_NOT_FOUND: {
      if (ret_val) {
        *ret_val = mgmtif_async_notify_ap_not_found();
      }
    }
    case WPA_DRV_TIMEOUT_SEND_ERROR_TO_HOST: {
      sl_mgmt_indicate_to_host(SLI_WLAN_RSP_JOIN, 0, WISE_P2P_WPS_FAILED, NULL);
    }
    default:
      break;
  }
  return;
}

// TODO: added for compilation
//This function is used by supplicant. It is taken from missing_func.c of 9117 Iso code
void ieee802_11_set_beacon(void *ptr)
{
}
