/*******************************************************************************
* @file  supplicant_mgmt_if.h
* @brief This file contains structures, macros and/or function prototype
* declarations needed for supplicant interface functions that are used by
* the supplicant.
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
#ifndef SUPPLICANT_MGMT_IF_H
#define SUPPLICANT_MGMT_IF_H

#include "mgmt_platform_if.h"
#include "mgmt_if_utils.h"
#include "nn.h"
#include "sli_supp_event_loop_thread.h"

enum supplicant_mgmt_if_enum {
  WPA_DRV_CHK_CLIENT = 1,
  WPA_DRV_CHK_REJOIN,
  WPA_DRV_CHK_WPS_CLIENT,
  WPA_DRV_CHK_BGSCAN,
  WPA_DRV_CHK_P2P_MODE,
  WPA_DRV_CHK_CONNECTED_STATE,
  WPA_DRV_CONN_NOTIFY_TO_HOST,
  WPA_DRV_NOTIFY_P2P_DEV_FOUND,
  WPA_DRV_UPDATE_PSK,
  WPA_DRV_SUPP_JOIN_DONE,
  WPA_DRV_INDICATE_SAE_SUCC,
  WPA_DRV_INDICATE_SAE_COMMITED_STATE,
  WPA_DRV_TRIGGER_BG_SCAN,
  WPA_DRV_CHK_DYN_STA_AP_SWITCH,
  WPA_DRV_CHK_BAND_2_4GHZ,
  WPA_DRV_CHK_FEAT_BLACKLIST,
  WPA_DRV_CHK_RSA_4096_SUPPORT,
  WPA_DRV_SET_ROAM_INDICATION_AND_BSSID,
  WPA_DRV_SET_CLIENT,
  WPA_DRV_MAC_BASED_JOIN_MISMATCH,
  WPA_DRV_CHK_PREV_BSSID_MISMATCH,
  WPA_DRV_GET_REJOIN_SCAN_INTERVAL,
  WPA_DRV_CHK_EAP_OKC,
  WPA_DRV_UPDATE_ASYNC_STATE_CODE,
  WPA_DRV_UPDATE_ASYNC_REASON_CODE,
  WPA_DRV_UPDATE_ASYNC_CHANNEL,
  WPA_DRV_UPDATE_ASYNC_RSSI,
  WPA_DRV_UPDATE_ASYNC_BSSID,
  WPA_DRV_ASYNC_NOTIF_AP_NOT_FOUND,
  WPA_DRV_TIMEOUT_SEND_ERROR_TO_HOST,
};
// TODO : These are not originally supplicant functions. But wrappers defined wrongly in supplicant and meant to be used by supplicant. Isolate this.
#define SM_KEY_RX_NO_KEY_RECEIVE_ENTER sm_KEY_RX_NO_KEY_RECEIVE_Enter
#define SM_KEY_RX_KEY_RECEIVE_ENTER    sm_KEY_RX_KEY_RECEIVE_Enter
#define SM_SUPP_PAE_CONNECTING_ENTER   sm_SUPP_PAE_CONNECTING_Enter
#define SM_SUPP_BE_RESPONSE_ENTER      sm_SUPP_BE_RESPONSE_Enter
#define SM_SUPP_BE_RECEIVE_ENTER       sm_SUPP_BE_RECEIVE_Enter
#define SM_SUPP_BE_REQUEST_ENTER       sm_SUPP_BE_REQUEST_Enter

#define RSI_MODULE_STATE_START_UP_OR_NOT_AVAVILABLE 0x00
#define RSI_MODULE_STATE_BEACON_LOSS                0x10
#define RSI_MODULE_STATE_DEAUTH_FROM_AP             0x20
#define RSI_MODULE_STATE_AUTH_FAILED                0x01
#define RSI_MODULE_STATE_ASSOC_FAILED               0x02
#define RSI_MODULE_STATE_DEAUTH_FRM_HOST            0x06
#define RSI_MODULE_STATE_PSK_NOT_CONF               0x07
#define RSI_MODULE_STATE_SUPP_DEAUTH                0x08
#define RSI_MODULE_STATE_ROAMING_NOT_ENABLED        0x09

#define RSI_MODULE_STATE_CURRENT_AP_IS_BEST        0x50
#define RSI_MODULE_STATE_BETTER_AP_FOUND           0x60
#define RSI_MODULE_STATE_NO_AP_FOUND               0x70
#define RSI_MODULE_STATE_ASSOCIATED                0x80
#define RSI_MODULE_STATE_UNASSOCIATED              0x90
#define RSI_MODULE_STATE_CONFIGURED_AP_NOT_PRESENT 0x03
#define RSI_MODULE_STATE_EAP_DENAIL                0x04
#define RSI_MODULE_STATE_FOUR_WAY_HANDSHAKE_FAIL   0x05
#define RSI_MODULE_STATE_RSSI_NOT_AVAILABLE        0x64

#define RSI_MODULE_STATE_SCANNING        1
#define RSI_MODULE_STATE_SCAN_RESULT     2
#define RSI_MODULE_STATE_FINAL_CONNECTON 3

enum p2p_sm_state {
  P2P_SM_INIT = 0,
  P2P_SM_SCANNING,
  P2P_SM_FIND,
};

enum p2p_sm_event {
  P2P_EVENT_INIT = 0,
  P2P_EVENT_START_FULL_SCAN,
  P2P_EVENT_SCAN_DONE,
};

struct conn_notify_to_host_input_arg {
  uint8 *mac_addr;
  uint8 con_event_type;
};

struct notify_p2p_dev_found_input_arg {
  uint8 new_device;
  uint8 *device_name;
  uint8 *p2p_device_addr;
  uint8 *pri_dev_type;
};

struct update_psk_in_nwparams_input_arg {
  uint8 *key;
  uint16 key_len;
};

struct supp_join_done_input_arg {
  uint8 client;
};

struct supp_roam_bssid_update_input_arg {
  uint8 roam_indication;
  uint8 *roam_bssid;
};

struct wpa_config_read_data_s {
  uint8 device_name[64];
  uint8 go_intent[2];
  uint8 ssid_postfix[64];
  uint8 operating_channel[2];
  uint8 module_type;
  uint8 dual_band;
  uint8 switch_band;
};

struct async_notifications_args_s {
  uint8 state_code;
  uint8 reason_code;
  uint8 channel;
  uint8 rssi;
  uint8 bssid[6];
};

enum wpa_config_read_type {
  WPA_CONFIG_READ_DEVICE_NAME = 0,
  WPA_CONFIG_READ_GO_INTENT,
  WPA_CONFIG_READ_SSID_POSTFIX,
  WPA_CONFIG_READ_OPERATING_CHANNEL,
  WPA_CONFIG_READ_MODULE_TYPE,
  WPA_CONFIG_READ_DUAL_BAND,
  WPA_CONFIG_READ_SWITCH_BAND
};

typedef struct supplicant_mgmt_if_ctx_s {
  struct conn_notify_to_host_input_arg conn_notify_to_host;
  struct notify_p2p_dev_found_input_arg notify_p2p_dev_input;
  struct update_psk_in_nwparams_input_arg update_psk_input;
  struct supp_join_done_input_arg supp_join_done_cmd_input;
  struct supp_roam_bssid_update_input_arg roam_bssid_ind_update_input;
  uint8 is_sae_commited;
  uint8 trigger_bg_scan;
  void *join_p2p_dev;
  uint8 *bssid;
  uint8 waiting_for_pd_resp;
  int client_mode;
  uint16 wps_method;
  struct wpa_config_read_data_s wpa_config_read_data;
  struct async_notifications_args_s async_notif_args;
  uint16 is_eap;
  uint8 tls_version;
} supplicant_mgmt_if_ctx_t;

extern supplicant_mgmt_if_ctx_t supp_mgmt_if_ctx;
#endif /*SUPPLICANT_MGMT_IF_H*/
