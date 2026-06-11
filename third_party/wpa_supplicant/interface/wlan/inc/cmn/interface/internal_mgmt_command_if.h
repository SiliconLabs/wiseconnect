/*******************************************************************************
* @file  internal_mgmt_command_if.h
* @brief This file contains internal mgmt command lists and event lists
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
#ifndef INTERNAL_MGMT_COMMAND_IF_H
#define INTERNAL_MGMT_COMMAND_IF_H

#include "wlan_scan.h"

/************* Internal Mgmt Command Lists *************/
#define TX_DOT11_MGMT_FRAME            0x0
#define ONAIR_MGMT                     0x2
#define MGMT_SET_KEYS_FRAME            0x30
#define MGMT_LOAD_IE_INFO              0x31
#define MGMT_P2P_ACTION_FRAME          0x32
#define MGMT_LOAD_AP_CAPS              0x34
#define MGMT_SET_CHANNEL               0x35
#define MGMT_SET_LISTEN_MODE           0x36
#define MGMT_REJOIN_FAIL               0x37
#define GTK_REKEY_DATA                 0x39
#define MGMT_REQ_RSSI_QRY              0x3A
#define SEND_ASSOC_REASSOC_RESP        0x56
#define ROAM_INDICATION                0x6B
#define NULL_AND_BLK_UNBLK             0x6C
#define DEAUTH_FROM_ROAM_PROCESS       0x6E
#define UPDATE_FT_IES_FROM_SUPP        0x7D
#define LOAD_FT_ACTION_IE              0x7F
#define WPA3_SAE_SUCC_ASSOC_REQ        0xCA
#define MGMT_INDICATE_SAE_COMMIT_STATE 0xCB
#define MGMT_ENABLE_DYN_AP_STA_SWITCH  0xCC
#define NULL_DATA_CONFIRM              0xE0
#define EAPOL4_CONFIRM                 0xEE
#define TWT_INFO_FRAME_CONFIRM         0xEF

/**************** Internal RX/Event Lists ****************/

#define TX_STATUS_INDICATION      0x04
#define DECRYPT_ERROR_INDICATION  0x07
#define FAST_PSP_TIM_INDICATION   0x35
#define LOAD_IE_RSP               0x00A5 /* Status code in W1 of the descriptor, Non zero means Failure */
#define SET_KEY_PACKET_RSP        0x00A6 /* Status code in W1 of the descriptor, Non zero means Failure */
#define LOAD_AP_CAPABILITIES_RSP  0x00A7 /* Status code in W1 of the descriptor, Non zero means Failure */
#define STATION_ADD_INDICATION    0x00A8
#define STATION_REMOVE_INDICATION 0x00A9 /* Status code in W1 of the descriptor, Non zero means Failure */
#define ON_AIR_MGMT_PKT           0x00AA /* Status code in W1 of the descriptor, Non zero means Failure */
#define SET_CHANNEL_CMD_RSP       0x00AB /* Status code in W1 of the descriptor, Non zero means Failure */
#define LISTEN_CHANNEL_CMD_RESP   0x00AC /* Status code in W1 of the descriptor, Non zero means Failure */
#define VALIDATE_ASSOC_REQ_PMKID  0xF3

/************ Structures and Definitions **************/

/************ ONAIR_MGMT :: 0x2 ***************/

struct auth_s {
  uint16 authAlgoID;
  uint16 authSeqNum;
  uint16 statusCode;
};

/************ MGMT_SET_KEYS_FRAME :: 0x30 ***************/

typedef struct set_keys_s {
  uint8 type;           /* 1 for PTK and 2 for GTK */
  uint8 key_id;         /* BIT 0-1 is key ID and BIT 7 -0 for remove key and 1 for Install key to PPE */
  uint8 key[32];        /* key to be loaded to PPE */
  uint8 mac_address[6]; /* STA MAC address in AP mode, Station mode – 0x00) */
  uint8 key_len;
} set_keys_t;

/************ MGMT_LOAD_IE_INFO :: 0x31 ***************/

#define IE_ASSOC_REQ 0x00
#define IE_ASSOC_RES 0x01
#define IE_PROBE_REQ 0x02
#define IE_PROBE_RES 0x03
#define IE_BEACON    0x04

typedef struct disconnect_wlan_s {
  uint8 station_mac[6];
} disconnect_wlan_t;

typedef struct _coun_Ie {
  uint8 found_countryie; /*Is set if country ie is found in beacon*/
  uint8 eId;
  uint8 eid_len;
  uint8 country_code[COUNTRY_CODE_LEN];
  struct {
    uint8 firstchan;
    uint8 nrchan;
    int8 max_txpwr;
  } __PACKED__ chan_info[30];
} __PACKED__ coun_Ie;

typedef struct ft_action_req_frm_supp_s {
  uint8 action_code;
  uint8 ft_ies_len;
  uint8 target_mac[6];
} ft_action_req_frm_supp_t;

/************ MGMT_LOAD_AP_CAPS :: 0x34 ***************/

typedef struct {
  uint32 ap_capabilites;
} load_ap_caps_t;

/************ MGMT_SET_CHANNEL :: 0x35 ***************/

/* Set channel command payload information */
typedef struct {
  uint16 channel_number;
} set_channel_t; // payload size is 1 byte

/************ MGMT_SET_LISTEN_MODE :: 0x36 ***************/

typedef struct {
  uint16 probe_report;
} listen_mode_t;

/************ GTK_REKEY_DATA :: 0x39 ***************/

#ifdef GTK_OFFLOAD
#define KEK_LENGTH            16
#define KCK_LENGTH            16
#define REPLAY_COUNTER_LENGTH 8

typedef struct gtk_rekey_info_s {
  uint8 kek[KEK_LENGTH];                    // kek- Key encryption key
  uint8 kck[KCK_LENGTH];                    // kck -key confirmation key
  uint8 replay_cntr[REPLAY_COUNTER_LENGTH]; // Counter value
} gtk_rekey_info_t;
#endif

/************ SEND_ASSOC_REASSOC_RESP :: 0x56 ***************/

typedef struct sme_assoc_resp {
  uint16 pmkid_status;
  uint8 assoc_reassoc_type;
  uint8 is_legacy;
  uint8 mac_addr[MAC_ADDR_LEN];
  size_t ies_len;
  uint8 ie[0];
} __attribute__((packed)) sme_assoc_resp_t;

#endif /* INTERNAL_MGMT_COMMAND_IF_H */