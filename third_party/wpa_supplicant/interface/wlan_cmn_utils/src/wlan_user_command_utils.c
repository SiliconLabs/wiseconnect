/*******************************************************************************
* @file  wlan_user_command_utils.c
* @brief This file contains utilities related to user commands that can be used
*         by Mgmt I/f, umac and lmac.
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
#include "wlan_user_command_utils.h"
#include "wlan_cmn_utils.h"

uint8 parse_dyn_sta_ap_switch_selection(P2PmodeFrameSnd *op)
{
  if (op->modeVal == 9) {
    if ((op->tcp_ip_feature_bit_map & TCP_IP_FEAT_EXTENTION_VALID)
        && (op->ext_tcp_ip_feature_bit_map & CONFIG_FEAT_EXTENTION_VALID)
        && (op->config_feature_bit_map & FEAT_CONC_STA_AP_DYN_SWITCH_SEL)) {
      return 1;
    }
  }
  return 0;
}

int16 parse_max_stations_supported_from_opermode(rsi_uP2Pmode_ram *p2p_cb,
                                                 uint8 *max_sta_supported,
                                                 uint8 *max_clients_support)
{
  uint8 temp_modeVal   = 0;
  *max_sta_supported   = 0;
  *max_clients_support = 0;

  if (p2p_cb->P2PmodeFrameSnd.modeVal == 6) {
    //! BIT[13] to BIT[17] in custom feature bitmap is used to give the number of stations to support
    if (p2p_cb->P2PmodeFrameSnd.custom_feature_bit_map & EXTRACT_NUMBER_OF_STATIONS_SUPPORTED) {
      //! Change in case of change in station count
      if (p2p_cb->P2PmodeFrameSnd.ext_custom_feature_bit_map & (EXT_FEAT_16th_STATION_IN_AP_MODE)) {
        *max_sta_supported =
          (((p2p_cb->P2PmodeFrameSnd.custom_feature_bit_map & EXTRACT_NUMBER_OF_STATIONS_SUPPORTED) >> 13) + 1) * 2;
        if (*max_sta_supported > 16) {
          return -1;
        }
      } else {
        //! For backward compitability we are allowing only 8 clients if ext cust bit is not set
        *max_sta_supported =
          ((p2p_cb->P2PmodeFrameSnd.custom_feature_bit_map & EXTRACT_NUMBER_OF_STATIONS_SUPPORTED) >> 13);
        if (*max_sta_supported > 8) {
          return -1;
        }
      }
    } else {
      *max_sta_supported = 4;
    }
  } else if (p2p_cb->P2PmodeFrameSnd.modeVal == 1) {
    //! P2P mode
    *max_sta_supported = 4;
  } else {
    //! Client mode
    *max_sta_supported   = 1;
    *max_clients_support = 1;
  }

  temp_modeVal = set_operating_mode(p2p_cb->P2PmodeFrameSnd.modeVal);

  if (temp_modeVal == WISE_CONCURRENT_MODE) {
    /*Giving 5 station entry for LMAC (1 for station + 4 for AP)*/
    *max_sta_supported = 5;
  }

  if (*max_clients_support != 1) {
    *max_clients_support = *max_sta_supported + 1;
  }

  return 0;
}