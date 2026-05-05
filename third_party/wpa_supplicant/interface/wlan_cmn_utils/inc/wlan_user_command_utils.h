/*******************************************************************************
* @file  wlan_user_command_utils.h
* @brief This file contains utilities declarations related to user commands that
*        can be used by Mgmt I/f, umac and lmac.
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
#ifndef WLAN_USER_COMMAND_UTILS_H
#define WLAN_USER_COMMAND_UTILS_H

#include "wlan_user_command.h"

/************* SLI_WLAN_REQ_JOIN = 0x14 *************/

#define CHECK_JOIN_POWERLEVEL(x) \
  ((x & ABSOLUTE_JOIN_POWER_VALUE_TOGGLE) ? (((x & ABSOLUTE_JOIN_POWER_VALUE_MASK) >> 2) < 1) : (x > 2))

/***********************************************/

uint8 parse_dyn_sta_ap_switch_selection(P2PmodeFrameSnd *op);
int16 parse_max_stations_supported_from_opermode(rsi_uP2Pmode_ram *p2p_cb,
                                                 uint8 *max_sta_supported,
                                                 uint8 *max_clients_support);
#endif /* WLAN_USER_COMMAND_UTILS_H */