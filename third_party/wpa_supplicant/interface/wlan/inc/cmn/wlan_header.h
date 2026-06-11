/*******************************************************************************
* @file  wlan_header.h
* @brief
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
#ifndef WLAN_HEADER_H
#define WLAN_HEADER_H

#include "sl_wlan_global.h"
#include "sl_wlan_data_types.h"

/** Queue related macros **/
#define MAX_HW_QUEUES  12
#define TH0_COMMON_Q   0x0
#define SL_WLAN_MGMT_Q 0x4
#define SL_WLAN_DATA_Q 0x5

#define UMAC_DATA_QUEUE0  0
#define UMAC_DATA_QUEUE1  1
#define UMAC_DATA_QUEUE2  2
#define UMAC_DATA_QUEUE3  3
#define UMAC_BC_QUEUE     9
#define UMAC_MGMT_QUEUE   10
#define UMAC_BEACON_QUEUE 11

/* Delete this after integrating with LMAC, this is currently used by LMAC */
#define WIFI_DATA_Q 0x5

#define SL_ETH_HDR_LEN           14
#define SL_EAPOL_4_OF_4_PKT_SIZE 99

#define MAC_ADDR_LEN      6
#define BSSID_LENGTH      6
#define MAC_HEADER_LENGTH 24
#define CCMP_HEADER_SIZE  8
#define SL_SSID_LEN       34 /* TODO: Check if this should be 32 */
#define SL_IP_ADDR_LEN    4
#define SL_PSK_LEN        64
#define SL_PMK_LEN        32 // maximum length of PMK
#define SL_WPS_PIN_LEN    8  // maximum length of WPS PIN

typedef enum NETWORK_TYPE_s {
  IBSS,
  STA_INFRASTRUCTURE,
  MODE_IBSS_SEC,
  MODE_AP,
  P2P_GROUP_OWN,
  P2P_CLIENT,
  AutoUnknown,
  InfrastructureMax,
} NETWORK_TYPE;

/** TWT Constraint Parameters - Element ID : 255, Element ID Extension : **/
/* TODO: These will be moved to ieee_80211_def.h */
#define RSI_WLAN_EID_EXT_TWT_CONSTRAINT 61
#define MAX_TWT_SESSIONS_SUPORTED       1

/********** Security mode definitions ************/
#define SEC_MODE_OPEN            0
#define SEC_MODE_WPA             1
#define SEC_MODE_WPA2            2
#define SEC_MODE_WEP             3
#define SEC_MODE_WPA_ENTERPRISE  4
#define SEC_MODE_WPA2_ENTERPRISE 5
#define SEC_MODE_MIXED_MODE      6
#define SEC_MODE_WPA3            7
#define SEC_MODE_WPA3_TRANSITION 8

#ifdef ENABLE_11AX
typedef struct sl_mbssid_params_s {
  uint8 tx_bssid_B5;
  uint8 mbssid_index;
  uint8 full_set_rx_per;
  uint8 max_bssid_indicator;
} __PACKED__ sl_mbssid_params_t;
#endif

/* The enum sl_end_point_type_s has been copied from 
  bb_rf_prog.h. The name of the copied enum is 
  end_point_type_t */
typedef enum sl_end_point_type_s {
  SL_WLAN_2_20 = 0,
  SL_WLAN_2_40,
  SL_WLAN_5_9_20,
  SL_WLAN_5_9_40,
  SL_WLAN_5_9_10,
  SL_WLAN_5_9_5,
  SL_BTCLASSIC,
  SL_BTLE,
  SL_ZIGBEE,
  SL_WLAN_11AH_2 = 9,
  SL_WLAN_11AH_4 = 10,
} sl_end_point_type_t;

/********** Encryption type definitions ************/

#define ENCRYPTION_TYPE_CCMP 2

#endif /* WLAN_HEADER_H */
