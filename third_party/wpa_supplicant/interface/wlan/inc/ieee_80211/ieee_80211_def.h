/*******************************************************************************
* @file  ieee_80211_def.h
* @brief This file includes IEEE 802.11 definitions as defined in the IEEE
*        802.11 standards
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
#ifndef IEEE_80211_DEF_H
#define IEEE_80211_DEF_H

#include "wlan_header.h"

/**********************************************/
/**************** Frame Format ****************/

#define WLAN_FC_PVER     0x0003
#define WLAN_FC_TODS     0x0100
#define WLAN_FC_FROMDS   0x0200
#define WLAN_FC_MOREFRAG 0x0400
#define WLAN_FC_RETRY    0x0800
#define WLAN_FC_PWRMGT   0x1000
#define WLAN_FC_MOREDATA 0x2000
#define WLAN_FC_ISWEP    0x4000
#define WLAN_FC_ORDER    0x8000

/* Note: This macro is used in a bitiwise operation 
   with the 2nd byte of mac header frame */

/**********************************************/

/****** Format of individual frame types ******/

/********** Association Request Frame *********/

struct assoc_s {
  uint16 capabilityInfo;
  uint16 listenInterval;
  uint8 variable[1]; /*SSID and supported rates are of variable length*/
};

/********** Association Response Frame *********/

struct assoc_rsp_s {
  uint16 capabilityInfo;
  uint16 statusCode;
  uint8 association_id;
};

/**********************************************/

/* Management/Extension Frame Body Components */

/******************* Fields *******************/

/*** Authentication algorithm Number field ****/

#define WLAN_AUTH_OPEN        0
#define WLAN_AUTH_SHARED_KEY  1
#define WLAN_AUTH_FT          2
#define WLAN_AUTH_SAE         3
#define WLAN_AUTH_FILS_SK     4
#define WLAN_AUTH_FILS_SK_PFS 5
#define WLAN_AUTH_FILS_PK     6
#define WLAN_SME_WPA3         7

/*** Authentication Sequence Number field *****/

#define AUTH_SEQ_NUM 0x0001

/**** Capability Information field macros *****/

#define RSI_CAPABILITY_INFO     0x0021
#define RSI_CAP_SPECTRUM_MGMT   0x0100
#define RSI_CAP_GMODE_SLOT      0x0400
#define RADIO_MEASUREMENT_CAPAB BIT(12)

/******** Listen Interval Field Macros ********/

#define RSI_LISTEN_INTERVAL 0x0010

/**** Block Ack Parameter Set Field Macros ****/

#define BLOCK_ACK_IMMEDIATE 2
#define BLOCK_ACK_DELAYED   3

/**********************************************/

/***************** Element ID *****************/

typedef struct element_s {
  uint8 id;
  uint8 datalen;
  uint8 data[];
} __PACKED__ element_t;

enum ieee80211_element_id {
  IEEE80211_ELEMID_SSID      = 0,
  IEEE80211_ELEMID_RATES     = 1,
  IEEE80211_ELEMID_FHPARMS   = 2,
  IEEE80211_ELEMID_DSPARMS   = 3,
  IEEE80211_ELEMID_CFPARMS   = 4,
  IEEE80211_ELEMID_TIM       = 5,
  IEEE80211_ELEMID_IBSSPARMS = 6,
  IEEE80211_ELEMID_COUNTRY   = 7,
  IEEE80211_ELEMID_CHALLENGE = 16,
  /* 17-31 reserved for challenge text extension */
  IEEE80211_ELEMID_PWRCNSTR             = 32,
  IEEE80211_ELEMID_PWRCAP               = 33,
  IEEE80211_ELEMID_TPCREQ               = 34,
  IEEE80211_ELEMID_TPCREP               = 35,
  IEEE80211_ELEMID_SUPPCHAN             = 36,
  IEEE80211_ELEMID_CSA                  = 37,
  IEEE80211_ELEMID_MEASREQ              = 38,
  IEEE80211_ELEMID_MEASREP              = 39,
  IEEE80211_ELEMID_QUIET                = 40,
  IEEE80211_ELEMID_IBSSDFS              = 41,
  IEEE80211_ELEMID_ERP                  = 42,
  IEEE80211_ELEMID_HTCAP                = 45,
  IEEE80211_ELEMID_RSN                  = 48,
  IEEE80211_ELEMID_XRATES               = 50,
  IEEE80211_ELEMID_MOB_DOMAIN           = 54,
  IEEE80211_ELEMID_OPER_CLASS           = 59,
  IEEE80211_ELEMID_HTINFO               = 61,
  IEEE80211_ELEMID_RM_ENABLED_CAP       = 70,
  IEEE80211_ELEMID_MULTIPLE_BSSID       = 71,
  IEEE80211_ELEMID_MGMT_MIC             = 76,
  IEEE80211_ELEMID_NONTX_BSSID_CAP      = 83,
  IEEE80211_ELEMID_MULTIPLE_BSSID_INDEX = 85,
  IEEE80211_ELEMID_EXT_CAPAB            = 127,
  IEEE80211_ELEMID_TPC                  = 150,
  IEEE80211_ELEMID_CCKM                 = 156,
  IEEE80211_ELEMID_VHT_CAP              = 191,
  IEEE80211_ELEMID_VHT_OPER             = 192,
  IEEE80211_ELEMID_TWT                  = 216,
  IEEE80211_ELEMID_VENDOR               = 221, /* vendor private */
  IEEE80211_ELEMID_RSNXE                = 244,
#ifdef RSI_ENABLE_CCX
  IEEE80211_ELEMID_DDP         = 155,
  IEEE80211_ELEMID_AIRONET     = 133,
  IEEE80211_ELEMID_AIRONET_RSD = 0x96,
  IEEE80211_ELEMID_AIRONET_IP  = 0x95,
#endif
  IEEE80211_ELEMID_EXTENSION = 255,
};

enum ieee80211_element_id_extension {
  IEEE80211_ELEMID_EXT_HE_CAPABILITY     = 35,
  IEEE80211_ELEMID_EXT_HE_OPERATION      = 36,
  IEEE80211_ELEMID_EXT_UORA              = 37,
  IEEE80211_ELEMID_EXT_HE_MU_EDCA_PARAMS = 38,
  IEEE80211_ELEMID_EXT_SPATIAL_REUSE     = 39,
  IEEE80211_ELEMID_EXT_MBSSID_CONFIG     = 55,
  IEEE80211_ELEMID_EXT_NON_INHERITANCE   = 56,
};

/**
 * BEACON management packets
 *
 *	octet timestamp[8]
 *	octet beacon interval[2]
 *	octet capability information[2]
 *	information element
 *		octet elemid
 *		octet length
 *		octet information[length]
 */

#define IEEE80211_CAPINFO_ESS            0x0001
#define IEEE80211_CAPINFO_IBSS           0x0002
#define IEEE80211_CAPINFO_CF_POLLABLE    0x0004
#define IEEE80211_CAPINFO_CF_POLLREQ     0x0008
#define IEEE80211_CAPINFO_PRIVACY        0x0010
#define IEEE80211_CAPINFO_SHORT_PREAMBLE 0x0020
#define IEEE80211_CAPINFO_PBCC           0x0040
#define IEEE80211_CAPINFO_CHNL_AGILITY   0x0080
#define IEEE80211_CAPINFO_SPECTRUM_MGMT  0x0100
/* bit 9 is reserved */
#define IEEE80211_CAPINFO_SHORT_SLOTTIME 0x0400
#define IEEE80211_CAPINFO_RSN            0x0800
/* bit 12 is reserved */
#define IEEE80211_CAPINFO_DSSSOFDM           0x2000
#define IEEE80211_CAPINFO_IMMDIATE_BLOCK_ACK 0x8000 /* CONF: Dynamic WDS enabled */
/* bits 14-15 are reserved */

/* ic_flags/iv_flags */
#define IEEE80211_F_TURBOP 0x00000001 /* CONF: ATH Turbo enabled*/
#define IEEE80211_F_COMP   0x00000002 /* CONF: ATH comp enabled */
#define IEEE80211_F_FF     0x00000004 /* CONF: ATH FF enabled */
#define IEEE80211_F_BURST  0x00000008 /* CONF: bursting enabled */
/* NB: this is intentionally setup to be IEEE80211_CAPINFO_PRIVACY */
#define IEEE80211_F_PRIVACY 0x00000010 /* CONF: privacy enabled */
#define IEEE80211_F_PUREG   0x00000020 /* CONF: 11g w/o 11b sta's */
#define IEEE80211_F_SCAN    0x00000080 /* STATUS: scanning */
#define IEEE80211_F_ASCAN   0x00000100 /* STATUS: active scan */
#define IEEE80211_F_SIBSS   0x00000200 /* STATUS: start IBSS */
/* NB: this is intentionally setup to be IEEE80211_CAPINFO_SHORT_SLOTTIME */
#define IEEE80211_F_SHSLOT             0x00000400 /* STATUS: use short slot time*/
#define IEEE80211_F_PMGTON             0x00000800 /* CONF: Power mgmt enable */
#define IEEE80211_F_DESBSSID           0x00001000 /* CONF: des_bssid is set */
#define IEEE80211_F_WME                0x00002000 /* CONF: enable WME use */
#define IEEE80211_F_BGSCAN             0x00004000 /* CONF: bg scan enabled (???)*/
#define IEEE80211_F_SWRETRY            0x00008000 /* CONF: sw tx retry enabled */
#define IEEE80211_F_TXPOW_FIXED        0x00010000 /* TX Power: fixed rate */
#define IEEE80211_F_IBSSON             0x00020000 /* CONF: IBSS creation enable */
#define IEEE80211_F_SHPREAMBLE         0x00040000 /* STATUS: use short preamble */
#define IEEE80211_F_DATAPAD            0x00080000 /* CONF: do alignment pad */
#define IEEE80211_F_USEPROT            0x00100000 /* STATUS: protection enabled */
#define IEEE80211_F_USEBARKER          0x00200000 /* STATUS: use barker preamble*/
#define IEEE80211_F_CSAPENDING         0x00400000 /* STATUS: chan switch pending*/
#define IEEE80211_F_WPA1               0x00800000 /* CONF: WPA enabled */
#define IEEE80211_F_WPA2               0x01000000 /* CONF: WPA2 enabled */
#define IEEE80211_F_WPA                0x01800000 /* CONF: WPA/WPA2 enabled */
#define IEEE80211_F_DROPUNENC          0x02000000 /* CONF: drop unencrypted */
#define IEEE80211_F_COUNTERM           0x04000000 /* CONF: TKIP countermeasures */
#define IEEE80211_F_HIDESSID           0x08000000 /* CONF: hide SSID in beacon */
#define IEEE80211_F_NOBRIDGE           0x10000000 /* CONF: dis. internal bridge */
#define IEEE80211_F_PCF                0x20000000 /* CONF: PCF enabled */
#define IEEE80211_F_DOTH_OR_11D        0x40000000 /* CONF: 11h enabled */
#define IEEE80211_F_DWDS               0x80000000 /* CONF: Dynamic WDS enabled */
#define IEEE80211_F_IMMDIATE_BLOCK_ACK 0x80000000

/* Frame types for mgmt, ctrl and data */

#define IEEE80211_FC0_VERSION_MASK  0x03
#define IEEE80211_FC0_VERSION_SHIFT 0x00
#define IEEE80211_FC0_VERSION_0     0x00
#define IEEE80211_FC0_TYPE_MASK     0x0c
#define IEEE80211_FC0_TYPE_SHIFT    0x02
#define IEEE80211_FC0_TYPE_MGT      0x00
#define IEEE80211_FC0_TYPE_CTL      0x04
#define IEEE80211_FC0_TYPE_DATA     0x08

#define IEEE80211_FC0_SUBTYPE_MASK  0xf0
#define IEEE80211_FC0_SUBTYPE_SHIFT 0x04
/* for TYPE_MGT */
#define IEEE80211_FC0_SUBTYPE_ASSOC_REQ    0x00
#define IEEE80211_FC0_SUBTYPE_ASSOC_RESP   0x10
#define IEEE80211_FC0_SUBTYPE_REASSOC_REQ  0x20
#define IEEE80211_FC0_SUBTYPE_REASSOC_RESP 0x30
#define IEEE80211_FC0_SUBTYPE_PROBE_REQ    0x40
#define IEEE80211_FC0_SUBTYPE_PROBE_RESP   0x50
#define IEEE80211_FC0_SUBTYPE_BEACON       0x80
#define IEEE80211_FC0_SUBTYPE_ATIM         0x90
#define IEEE80211_FC0_SUBTYPE_DISASSOC     0xa0
#define IEEE80211_FC0_SUBTYPE_AUTH         0xb0
#define IEEE80211_FC0_SUBTYPE_DEAUTH       0xc0
#define IEEE80211_FC0_SUBTYPE_ACTION       0xd0
#define IEEE80211_FC0_SUBTYPE_ACTION_NOACK 0xe0

/* for TYPE_CTL */
#define IEEE80211_FC0_SUBTYPE_BAR        0x84
#define IEEE80211_FC0_SUBTYPE_BA         0x94
#define IEEE80211_FC0_SUBTYPE_PS_POLL    0xa4
#define IEEE80211_FC0_SUBTYPE_RTS        0xb4
#define IEEE80211_FC0_SUBTYPE_CTS        0xc4
#define IEEE80211_FC0_SUBTYPE_ACK        0xd4
#define IEEE80211_FC0_SUBTYPE_CF_END     0xe4
#define IEEE80211_FC0_SUBTYPE_CF_END_ACK 0xf4
/* for TYPE_DATA (bit combination) */
#define IEEE80211_FC0_SUBTYPE_DATA          0x00
#define IEEE80211_FC0_SUBTYPE_CF_ACK        0x10
#define IEEE80211_FC0_SUBTYPE_CF_POLL       0x20
#define IEEE80211_FC0_SUBTYPE_CF_ACPL       0x30
#define IEEE80211_FC0_SUBTYPE_NULL_DATA     0x40
#define IEEE80211_FC0_SUBTYPE_CFACK         0x50
#define IEEE80211_FC0_SUBTYPE_CFPOLL        0x60
#define IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK 0x70
#define IEEE80211_FC0_SUBTYPE_QOS           0x80
#define IEEE80211_FC0_SUBTYPE_QOS_NULL      0xc0

#define IEEE80211_FC1_DIR_MASK   0x03
#define IEEE80211_FC1_DIR_NODS   0x00 /* STA->STA */
#define IEEE80211_FC1_DIR_TODS   0x01 /* STA->AP  */
#define IEEE80211_FC1_DIR_FROMDS 0x02 /* AP ->STA */
#define IEEE80211_FC1_DIR_DSTODS 0x03 /* AP ->AP  */
#define IEEE80211_FC1_PROTECTED  0x40

/*
 * Refer to IEEE Std 802.11-2020 section 12.5.5.2
 * 802.11i defines an extended IV for use with non-WEP ciphers.
 * When the EXTIV bit is set in the key id byte an additional
 * 4 bytes immediately follow the IV for TKIP.  For CCMP the
 * EXTIV bit is likewise set but the 8 bytes represent the
 * CCMP header rather than IV+extended-IV.
 */
#define IEEE80211_WEP_EXTIV 0x20
/***************************************/

/**  Control frame - BAR  **/
typedef struct bar_ctrl_s {
  uint16 bar_ack_policy : 1;
  uint16 multi_tid : 1;
  uint16 compressed_bitmap : 1;
  uint16 reserved : 9;
  uint16 tid_indx : 4;
} __attribute__((__packed__)) bar_ctrl_t;

typedef struct bar_info_s {
  uint16 fragment : 4;
  uint16 start_seq_num : 12;
} __attribute__((__packed__)) bar_info_t;

/* BAR structure */
typedef struct ieee80211_bar_s {
  uint8 fc[2];
  uint8 dur[2];
  uint8 addr1[MAC_ADDR_LEN];
  uint8 addr2[MAC_ADDR_LEN];
  bar_ctrl_t bar_ctrl;
  bar_info_t bar_info;
  /* possibly followed by BAR INFO[MAC_ADDR_LEN]; */
  /* see below */
} __attribute__((__packed__)) ieee80211_bar_t;

/**  Country IE - Element ID 7 **/
#define COUNTRY_CODE_LEN 3

#ifdef DISABLE_CH144
#define MAX_POSSIBLE_CHANNELS_5G 24
#else
#define MAX_POSSIBLE_CHANNELS_5G 25
#endif
#define MAX_POSSIBLE_CHANNELS_2P4G 14

/** Power Capabilities - Element ID 33 **/

#define RSI_MODULE_MIN_POWER -5
#define RSI_MODULE_MAX_POWER 17

/** Channel Switch Announcement - Element ID 37 **/

#ifdef ENABLE_CH_SWITCH

/* Channel switch mode bit set */
#define CHANNEL_SWITCH_MODE_SET 0x0001

typedef struct csa_info_s {
  uint16 action_category;
  uint16 len_eleid;
  uint16 newch_mode;
  uint8 chswcnt;
} csa_info_t;

#endif /* ENABLE_CH_SWITCH */

/** Quiet - Element ID 40 **/

#ifdef ENABLE_QUIET

#define QUIET_LENGTH 6
/* First Quiet elment location in beacon receive frame from lmac*/
#define QUIET_ELE_LOC 9
/* Quiet Element data structure */
typedef struct quiet_info_s {
  uint8 element_id; //Element id of Quiet frame
  uint8 length;     //Length of the element id
  uint8 quiet_cnt;  //quiet count in TBTT's after which quiet is to be started
  uint8
    quiet_period; //period in number of beacon intervals, after which quiet is to be repeated(not being used in the present architecture)
  uint16 quiet_duration; //duration for which Quiet is to be maintained
  uint16 quiet_offset;   //Offset value in TU's which is to be added to Quiet count to get the start time of Quiet
  uint32 quiet_tsf_tout;
  struct quiet_info_s *next; //Pointer to the next Quiet element node
} quiet_info_t;

#endif /* ENABLE_QUIET */

/** HT Capabilities - Element ID 45 **/

/* HT Capabilities IE */

#define RSI_SHORT_GI_SUPPORTED_AP BIT(5)

/** RSNE - Element ID 48 **/

#define RSN_SELECTOR_LEN 4
#define SAE_AKM          8

/** HT Info - Element ID 61 **/

#ifdef ENABLE_40MHz_BANDWIDTH
#define IEEE80211_HTINFO_2NDCHAN       0x03 /* secondary/ext chan offset */
#define IEEE80211_HTINFO_2NDCHAN_S     0
#define IEEE80211_HTINFO_2NDCHAN_ABOVE 0x01   /* above private channel */
#define IEEE80211_HTINFO_2NDCHAN_BELOW 0x03   /* below primary channel */
#define IEEE80211_CHAN_HT40_UPPER      0x2000 /* HT 40 channel w/ ext above */
#define IEEE80211_CHAN_HT40_LOWER      0x4000 /* HT 40 channel w/ ext below */

typedef struct ieee80211_ie_htinfo {
  uint8 hi_id;              /* element ID */
  uint8 hi_len;             /* length in bytes */
  uint8 hi_ctrlchannel;     /* primary channel */
  uint8 hi_byte1;           /* ht ie byte 1 */
  uint8 hi_byte2;           /* ht ie byte 2 */
  uint8 hi_byte3;           /* ht ie byte 3 */
  uint16 hi_byte45;         /* ht ie bytes 4+5 */
  uint8 hi_basicmcsset[16]; /* basic MCS set */
} __PACKED__ ieee80211_ie_htinfo_t;

#endif /* ENABLE_40MHz_BANDWIDTH */

/**  Radio Measurement(RM) IE - Element ID 70 **/
#define RRM_CAPS_BEACON_REPORT_TABLE BIT(6)

/**  Extended Capabilities IE - Element ID 127 **/
#define EXT_BSS_TRANSITION 19
#define EXT_MULTIPLE_BSSID 22
#define EXT_TWT_REQ        77

#define EXT_CAPB2_BTM_SUPP 0x08

/** TWT element IE Element ID : 216 **/

/* Request Type - Setup Command Value */
#define REQUEST_TWT   0
#define SUGGEST_TWT   1
#define DEMAND_TWT    2
#define TWT_GROUPING  3
#define ACCEPT_TWT    4
#define ALTERNATE_TWT 5
#define DICTATE_TWT   6
#define REJECT_TWT    7

typedef struct twt_element_s {
  uint8 eid;
  uint8 elength;
  uint8 control_ndp_pi : 1;
  uint8 control_resp_pm_mode : 1;
  uint8 control_negotiation_type : 2;
  uint8 control_twt_info_frame_dis : 1;
  uint8 control_wake_duration_unit : 1;
  uint8 control_resereved : 2;
  uint16 req_type_twt_req : 1;
  uint16 req_type_twt_setup_command : 3;
  uint16 req_type_trigger : 1;
  uint16 req_type_implicit_twt : 1;
  uint16 req_type_flow_type : 1;
  uint16 req_type_twt_flow_id : 3;
  uint16 req_type_twt_wi_exp : 5;
  uint16 req_type_twt_protection : 1;
  uint32 target_wake_time[2];
  uint8 nom_min_wake_duration;
  uint16 wake_interval_mantisa;
  uint8 twt_channel;
} __PACKED__ twt_element_t;

/** Vendor Specific - Element ID 221 **/

typedef struct wmm_req_param_s {
  uint8 eid;
  uint8 eid_len;
  uint8 oui[3];
  uint8 oui_Type;
  uint8 wme_subtype;
  uint8 wme_version;
  uint8 wme_qos_info;
} wmm_req_param_t;

/** HE Capabilities IE Element ID : 255, Element ID Extension : 35 **/
#ifdef ENABLE_11AX

#define IEEE_HE_MAC_CAPB0_TWT_REQ_SUPP                0x2
#define IEEE_HE_MAC_CAPB1_TRIG_FRAME_MAC_PAD_DURATION 0x8
#define IEEE_HE_MAC_CAPB2_32BIT_BA_BITMAP_SUPP        0x20
#define IEEE_HE_MAC_CAPB2_ALL_ACK                     0x02
#define IEEE_HE_MAC_CAPB2_BSR                         0x08
#define IEEE_HE_MAC_CAPB3_MAX_AMPDU_LEN               0x10
#define IEEE_HE_MAC_CAPB3_RX_CTRL_FRAME_MBSS          0x80
#define IEEE_HE_MAC_CAPB5_HE_VHT_TRIG_FRAME_RX_SUPP   0x80

#define IEEE_HE_PHY_CAPB0_SUPP_OF_RU_2G                      0x20
#define IEEE_HE_PHY_CAPB1_DEV_CLASS                          0x10
#define IEEE_HE_PHY_CAPB1_HE_LTF_GI_SUPP                     0x40
#define IEEE_HE_PHY_CAPB2_NDP_4x_LTF_GI_3P2US_SUPP           0x2
#define IEEE_HE_PHY_CAPB2_STBC_RX_LESS_THAN_80MHZ            0x8
#define IEEE_HE_PHY_CAPB2_UL_MU_MIMO_BW_SUPP                 0xC0
#define IEEE_HE_PHY_CAPB3_DCM_CONF_TX_RX                     0x1B
#define IEEE_HE_PHY_CAPB4_SU_BEAM_FORMEE_SUPP                0x1
#define IEEE_HE_PHY_CAPB4_SU_BEAM_FORMEE_MAX_STS_BELOW_80MHZ 0xc
#define IEEE_HE_PHY_CAPB5_NG_16_FEEDBACK                     0xC0
#define IEEE_HE_PHY_CAPB6_CODE_BOOK_SIZE_SUPP                0x3
#define IEEE_HE_PHY_CAPB6_BF_FEEDBACK_SU_MU                  0x0C
#define IEEE_HE_PHY_CAPB6_TRIGGERED_CQI_FB_SUPP              0x10
#define IEEE_HE_PHY_CAPB6_PARTIAL_BW_EXT_RANGE               0x20
#define IEEE_HE_PHY_CAPB6_PARTIAL_BW_DL_MU_MIMO              0x40
#define IEEE_HE_PHY_CAPB6_PPE_THRESHOLD_PRESENT              0x80
#define IEEE_HE_PHY_CAPB7_POWER_BOOST_FACTOR_SUPP            0x2
#define IEEE_HE_PHY_CAPB7_RX_SU_MU_PPDU_4XLTF_GI_0P8US_SUPP  0x4
#define IEEE_HE_PHY_CAPB7_MAX_NC_SUPP                        0x8
#define IEEE_HE_PHY_CAPB8_ER_SU_PPDU_RX_4XLTF_GI_0P8US_SUPP  0x1
#define IEEE_HE_PHY_CAPB8_ER_SU_PPDU_RX_1XLTF_GI_0P8US_SUPP  0x10
#define IEEE_HE_PHY_CAPB8_DCM_MAX_RU                         0
#define IEEE_HE_PHY_CAPB9_NON_TRIG_CQI_FB_TX_SUPP            0x02
#define IEEE_HE_PHY_CAPB9_RX_FULL_BW_SU_IN_MU                0x30
#define IEEE_HE_PHY_CAPB9_NOMINAL_PKT_PADDING                0x80

#define IEEE_HE_TX_MCS_MAP_UNDER_80MHZ 0xFFFC
#define IEEE_HE_RX_MCS_MAP_UNDER_80MHZ 0xFFFC

#endif /* ENABLE_11AX */

/** MU ECDA Parameter Set Element ID : 255, Element ID Extension : 38 **/

typedef struct mu_edca_params_s {
  uint16 cw_max;
  unsigned char cw_min_lsb;
  unsigned char en_mu_edca_q;
  unsigned char cw_min_msb;
  unsigned char mu_edca_timer;
  uint16 mu_edca_aifsn;
} mu_edca_params_t;

typedef struct mu_edca_params_rec_s {
  uint8 aifsn;
  uint8 ecw_min_max;
  uint8 mu_edca_timer;
} __PACKED__ mu_edca_params_rec_t;

typedef struct mu_edca_params_ie_s {
  uint8 qos_info;
  mu_edca_params_rec_t mu_edca_params_rec[4];
} __PACKED__ mu_edca_params_ie_t;

/** WME related **/
#define WME_OUI               0xf25000
#define IEEE_WME_OUI_TYPE     0x02
#define WME_INFO_OUI_SUBTYPE  0x00
#define WME_PARAM_OUI_SUBTYPE 0x01
#define WME_VERSION           0x01

#define WME_NUM_AC  0x04 /* 4 AC categories */
#define WME_NUM_TID 16   /* 16 tids */

/*****************/
#define IEEE80211_RATE_VAL 0x7F

/** Supported rates related **/

#define IEEE80211_RATE_SIZE    0x08 /* 802.11 standard */
#define IEEE80211_RATE_MAXSIZE 12   //15		/* max rates we'll handle */
#define IEEE80211_RATE_MAXSZ   15   /* max rates we'll handle */

/*******************************/

/** Authentication frame related **/

#define IEEE80211_AUTH_ALG_OPEN   0x0000
#define IEEE80211_AUTH_ALG_SHARED 0x0001
#define IEEE80211_AUTH_ALG_FT     0x0002
#define IEEE80211_AUTH_ALG_SAE    0x0003
#define IEEE80211_AUTH_ALG_LEAP   0x0080

/** WPA related macros ***********************/

#define WPA_OUI           0xf25000
#define IEEE_WPA_OUI_TYPE 0x01
#define IEEE_WPA_VERSION  0x01 /* current supported version */

/* duration for mgmt packets*/
#define DURATION_ASSOC_REQ    314
#define DURATION_ASSOC_RESP   314
#define DURATION_REASSOC_REQ  314
#define DURATION_REASSOC_RESP 314
#define DURATION_PROBE_REQ    314
#define DURATION_PROBE_RESP   314
#define DURATION_BEACON       314
#define DURATION_DISASSOC     314
#define DURATION_AUTH         314
#define DURATION_DEAUTH       314
#define DURATION_ACTION       314
#define DURATION_ACTION_NOACK 314

/* Mac header structure */
typedef struct ieee80211_macframe_s {
  uint8 fc[2];
  uint8 dur[2];
  uint8 addr1[MAC_ADDR_LEN];
  uint8 addr2[MAC_ADDR_LEN];
  uint8 addr3[MAC_ADDR_LEN];
  uint8 seq[2];
  /* possibly followed by addr4[MAC_ADDR_LEN]; */
  /* see below */
} ieee80211_macframe_t;

typedef struct ie_cf_params_s {
  uint8 cfp_eid;
  uint8 cfp_len;
  uint8 cfp_count;
  uint8 cfp_period;
  uint16 max_duration;
  uint16 cfp_duration;
} ie_cf_params_t;

typedef struct ie_tim_s {
  uint8 tim_eid; /* IEEE80211_ELEMID_TIM */
  uint8 tim_len;
  uint8 tim_count;  /* DTIM count */
  uint8 tim_period; /* DTIM period */
  uint8 tim_bitctl; /* bitmap control */
  uint8 tim_bitmap; /* variable-length bitmap */
} ie_tim_t;

typedef struct ie_csa_s {
  uint8 csa_eid; /* IEEE80211_ELEMID_CHANSWITCHANN */
  uint8 csa_len;
  uint8 csa_mode;    /* Channel Switch Mode */
  uint8 csa_newchan; /* New Channel Number */
  uint8 csa_count;   /* Channel Switch Count */
} ie_csa_t;

typedef struct mobility_dom_ie_s {
  uint8 ie;
  uint8 len;
  uint8 mdid[2];
  uint8 ft_cap;
} __attribute__((packed)) mobility_dom_ie_t;

typedef struct ie_wme_info_s {
  uint8 wme_eid;     /* IEEE80211_ELEMID_VENDOR */
  uint8 wme_len;     /* length in bytes */
  uint8 wme_oui[3];  /* 0x00, 0x50, 0xf2 */
  uint8 wme_type;    /* OUI type */
  uint8 wme_subtype; /* OUI subtype */
  uint8 wme_version; /* spec revision */
  uint8 wme_info;    /* QoS info */
} ie_wme_info_t;

typedef struct wme_acparams_s {
  uint8 acp_aci_aifsn;
  uint8 acp_logcwminmax;
  uint16 acp_txop;
} wme_acparams_t;

typedef struct wme_param_s {
  uint8 param_eid;
  uint8 param_len;
  uint8 param_oui[3];
  uint8 param_oui_type;
  uint8 param_oui_subtype;
  uint8 param_version;
  uint8 param_qosInfo;
#define WME_QOSINFO_COUNT 0x0f /* Mask for param count field */
  uint8 param_reserved;
  wme_acparams_t params_acParams[WME_NUM_AC];
} __PACKED__ wme_params_t;

typedef struct ds_param_ie_s {
  uint8 dsparam_eid;
  uint8 dsparam_len;
  uint8 channel;
} __PACKED__ ds_params_ie_t;

typedef struct wpa_ie_s {
  uint8 wpa_eid;
  uint8 wpa_len;
  uint8 wpa_oui[4];        /* OUI */
  uint8 version[2];        /* Version */
  uint8 multi_ciph_oui[4]; /* Multicast cipher OUI */
  uint8 num_unicasts[2];   /* Number of unicasts */
  uint8 uni_ciph_oui[4];   /* unicast cipher OUI */
  uint8 num_auths[2];
  uint8 auth_oui[4];
} wpa_ie_t;

#define WLAN_WIFI_OUI_RSN \
  (uint8[])               \
  {                       \
    0x00, 0x0F, 0xAC      \
  }

typedef struct rsn_ie_s {
  uint8 rsn_eid;
  uint8 rsn_len;
  uint8 version[2];      /* Version */
  uint8 grp_ciph_oui[3]; /* OUI */
  uint8 group_ciph_type;
  uint8 pair_key_count[2]; /* Number of unicasts */
  uint8 pair_ciph_oui[4];  /* Multicast cipher OUI */
  uint8 auth_key_mgmt_cnt[2];
  uint8 akmp_suit_oui[4]; /* unicast cipher OUI */
  uint8 rsn_cap[2];
} rsn_ie_t;

#ifdef RSI_ENABLE_CCX

#define RSI_WLCCP_PROTOCOL         0
#define RSI_ADJACENT_AP_REPORT     0x30
#define RSI_ADJ_AP_SUBTYPE_REPORT  0x00
#define RSI_CCX_RADIO_MESSAGE      0x32
#define RSI_CCX_RM_SUBTYPE_REPORT  0x81
#define RSI_CCX_RM_SUBTYPE_REQUEST 0x01

typedef struct _ETHERNET_HEADER {
  uint8 da[6];
  uint8 sa[6];
  uint16 type;
} __PACKED__ ETHERNET_HEADER;

typedef struct _AIRONET_IAPP_HEADER {
  uint8 version;
  uint8 length;  /* IAPP length */
  uint8 type;    /* IAPP type */
  uint8 subtype; /* IAPP subtype */
  uint8 da[6];   /* Destination MAC address */
  uint8 sa[6];   /* Source MAC address */
} __PACKED__ AIRONET_IAPP_HEADER;

typedef struct aironet_frame_hdr_s {
  ETHERNET_HEADER ethernet_hdr;
  AIRONET_IAPP_HEADER iapp_hdr;
} __PACKED__ aironet_frame_hdr_t;

/*
 * CCX information element
 */
struct ieee80211_ccx_info {
  uint8 ccx_id;      /* IEEE80211_ELEMID_VENDOR */
  uint8 ccx_len;     /* length in bytes */
  uint8 ccx_oui[3];  /* 0x00, 0x40,0x96  */
  uint8 version;     /* version */
  uint8 ccx_version; /* ccx version */
} __PACKED__;

/*
 * CCX Radio Cap-Managment IE
 */
struct ccx_radio_cap_ie {
  uint8 ccx_id;         /* IEEE80211_ELEMID_VENDOR */
  uint8 ccx_len;        /* length in bytes */
  uint8 ccx_oui[3];     /* 0x00, 0x40,0x96  */
  uint8 radio_cap_mgmt; /* version */
  uint8 enable;         /* ccx version */
};

/*
 * Aironet Cisco Properietary element IE
 */
struct ieee80211_aironet_ie {
  uint8 airo_id;     /*IEEE80211_ELEMID_AIRONET 0x85 */
  uint8 airo_len;    /*length in bytes */
  uint8 airo_oui[3]; /* 0x00, 0x00, 0x66 */
  uint8 airo_value[7];
  uint8 airo_ap_name[16];
  uint8 airo_clients;
  uint8 value;
  uint8 value1[2];
} __PACKED__;

/*
 * Aironet AP IP IE 
 */
struct ieee80211_aironet_ap_ip_ie {
  uint8 airo_ap_ip_id;       /*IEEE80211_ELEMID_AIRONET 0x95*/
  uint8 airo_ap_ip_len;      /*length in bytes */
  uint8 airo_ap_ip_value[6]; /*value in bytes */
} __PACKED__;

/*
 * Aironet CCX AP Control of Client Transmit Power
 */
struct ieee80211_aironet_client_pwr_ie {
  uint8 aironet_client_pwr_id;       /*IEEE80211_ELEMID_AIRONET 0x96 */
  uint8 aironet_client_pwr_len;      /*length in bytes */
  uint8 aironet_client_pwr_value[6]; /*Power Value */
} __PACKED__;

/*
 * Aironet DDP Packet 
 */

#define CCX_DDP_OUI_BYTES 0x00, 0x40, 0x96, 0x00
struct ieee80211_ddp_pkt {
  uint8 aironet_ddp_id;        /*IEEE80211_ELEMID_AIRONET 0x9b */
  uint8 aironet_ddp_len;       /*length in bytes */
  uint8 aironet_ddp_oui[4];    /* CCX_DDP_OUI_BYTES*/
  uint8 aironet_mac_addr[6];   /*Previous Connected MAC */
  uint8 aironet_old_channel;   /*Channel it was connected */
  uint8 aironet_old_ssid_len;  /*SSID length*/
  uint8 aironet_old_ssid[32];  /*SSID */
  uint32 aironet_disconect_at; /*Time Stamp of station leaved from connected AP*/
} __PACKED__;

/*
 * Aironet CCX_CCKM_IE
 */
struct ieee80211_aironet_cckm_ie {
  uint8 aironet_cckm_id; /*IEEE80211_ELEMID_AIRONET */
  uint8 aironet_cckm_len;
  uint8 aironet_cckm_oui[3];
  uint8 aironet_cckm_oui_type;
  uint64 aironet_cckm_tsf;
  uint32 aironet_cckm_rn;
  uint64 aironet_cckm_mic;

} __PACKED__;
#endif

typedef struct wmm_info_s {
  uint8 aifsn : 4;
  uint8 acm : 1;
  uint8 aci : 2;
  uint8 res : 1;
  uint8 ECW_min : 4;
  uint8 ECW_max : 4;
  uint16 txop;
} wmm_info_t;

typedef struct wlan_gen_ie_t {
  uint8 eid;
  uint8 eid_len;
  uint8 oui[3];
  uint8 ouiType;

  union {
    struct {
      uint8 ouiSubTyp;
      uint8 version;
      uint8 ac_info;
      uint8 res;
      struct wmm_info_s ac_param[4];
    } wmm;

    struct {
      uint8 temp[100];
    } ele_11n;
  };
} genIe_t;

#ifdef ENABLE_11AX
typedef struct sl_he_cap {
  uint8 mac_cap[6];
  uint8 phy_cap[11];
} __PACKED__ sl_he_cap_t;
#endif /* ENABLE_11AX */

/**  Vendor Specific IE - Element ID 221 **/
#define WLAN_WIFI_OUI \
  (uint8[])           \
  {                   \
    0x00, 0x50, 0xF2  \
  }

/**********************************************/
/************  Action frame format ************/

#define IEEE80211_ACTION_CATEGORY_SPECTRUM_MGMT     0
#define IEEE80211_ACTION_CATEGORY_BLOCK_ACK         3
#define IEEE80211_ACTION_CATEGORY_RADIO_MEASUREMENT 5
#define IEEE80211_ACTION_CATEGORY_FT                6
#define IEEE80211_ACTION_CATEGORY_SA_QUERY          8
#define IEEE80211_ACTION_CATEGORY_WNM               10
#define IEEE80211_ACTION_CATEGORY_UNPROTECTED_S1G   22

/**** Spectrum Management Action frame - Code 0 ******/

#define CHANNEL_SWITCH_ACTION 4

/********* Block Ack Action frame - Code 3 ***********/

#define ADDBA_REQ_ACTION  0
#define ADDBA_RESP_ACTION 1
#define DELBA_ACTION      2

/** ADDBA Request frame - Action Value 0  **/

#define ADDBA_REQ_SIZE 9
typedef struct addba_req_s {
  uint16 action_category;
  uint16 paramlsb_dlgToken;
  uint16 timeoutlsb_parammsb;
  uint16 seqctrllsb_timeoutmsb;
  uint8 seqctrlmsb;
} addba_req_t;

/** ADDBA Response frame - Action Value 1  **/

#define ADDBA_RESP_SIZE 9
typedef struct addba_resp_s {
  uint16 action_category;
  uint16 statuscodelsb_dlgToken;
  uint16 paramlsb_statuscodemsb;
  uint16 timeoutlsb_parammsb;
  uint8 timeoutmsb;
} addba_resp_t;

/** DELBA frame - Action Value 2  **/

#define DELBA_SIZE 6
typedef struct delba_req_s {
  uint16 action_category;
  uint16 param_set;
  uint16 reasoncode;
} delba_req_t;

/********* FT Action frame - Code 6 ***********/

#define FT_ACTION_CODE_REQ 1
#define FT_ACTION_CODE_RSP 2

/** FT Request frame - Action Value 1  **/
typedef struct action_ft_req_s {
  uint8 category_code;
  uint8 action_code;
  uint8 sta_mac[6];
  uint8 target_ap_mac[6];
} __PACKED__ action_ft_req_t;

/** FT Response frame - Action Value 2  **/
typedef struct ft_action_resp_frm_con_ap_t {
  uint8 category_code;
  uint8 action_code;
  uint8 sta_mac[6];
  uint8 target_ap_mac[6];
  uint16 status;
} ft_action_resp_frm_con_ap_t;

/***** Radio Measurement Action frame - Code 5 *****/

/***** SA Query Action frame - Code 6 *****/

#define SA_QUERY_TYPE_REQUEST  0
#define SA_QUERY_TYPE_RESPONSE 1

typedef struct sa_query_frame_s {
  uint8 action_category;
  uint8 action_type;
  uint16 trans_id;
} __PACKED__ sa_query_frame_t;

/***** WNM Action frame - Code 10 *****/

#define BTM_REQ_ACTION         7
#define TIMING_MEASUREMENT_REQ 25

/** TSF Req/Resp frame - Action Value 13 & 14 **/

typedef struct tsf_sync_pkt_s {
  uint8 category;
  uint8 action;
  uint8 dialogToken;
  uint8 reserved;
} tsf_sync_pkt_t;

typedef struct tsf_sync_rsp_pkt_s {
  uint8 mac_addr[6];
  uint8 reserved[2];
  uint32 time_stamp[2];
} tsf_sync_rsp_pkt_t;

/*** Unprotected S1G Action frame - Code 22 ***/

#define UNPROTECTED_TWT_SETUP_ACTION     6
#define UNPROTECTED_TWT_TEAR_DOWN_ACTION 7
#define TWT_INFORMATION_ACTION           11

/** TWT Setup frame - Value 6  **/
typedef struct twt_setup_frame_s {
  uint8 action_category;
  uint8 action_type;
  uint8 dialog_token;
  twt_element_t twt_element;
} __PACKED__ twt_setup_frame_t;

/** TWT Teardown frame - Value 7  **/

#define TEAR_DOWN_ALL 0x80

typedef struct twt_teardown_frame_t {
  uint8 action_category;
  uint8 action_type;
  uint8 flow_id : 3;
  uint8 reserved : 2;
  uint8 negotiation_type : 2;
  uint8 teardown_all : 1;
} __PACKED__ twt_teardown_frame_t;

/** TWT Information field - Value 11  **/

#ifdef ENABLE_11AX

#define NEXT_TWT_SUBFIELD_SIZE 3

typedef struct twt_info_frame_s {
  uint8 action_category;
  uint8 action_type;
  uint8 flow_id : 3;
  uint8 response_requested : 1;
  uint8 next_twt_request : 1;
  uint8 next_twt_subfield_size : 2;
  uint8 all_twt : 1;
  uint64 suspend_duration;
} __PACKED__ twt_info_frame_t;

#endif /* ENABLE_11AX */

/**********************************************/

/***************** Security *******************/

/* RSNA Confidentiality and Integrity Protocols */

/***************** BIP ***************/

#define BIP_CMAC_128_MIC_LEN 8
#define MAX_MIC_LEN          32
#define AAD_FRAME_TOTAL_LEN  40
#define BIP_AAD_FC_MASK      ((WLAN_FC_RETRY | WLAN_FC_PWRMGT | WLAN_FC_MOREDATA) >> 8)

/**********************************************/

// TODO: Keep them in this file but in proper place, referencing the spec

#define IEEE80211_MMIE_LEN 18
#define IEEE80211_IPN_LEN  6

#endif /* IEEE_80211_DEF_H */
