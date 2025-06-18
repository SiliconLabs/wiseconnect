/*******************************************************************************
* @file  mgmt_if_rsi_wsc.h
* @brief This file is a temporary header for interfacing with rsi_wsc
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
#ifndef MGMT_IF_RSI_WSC_HEADER_H
#define MGMT_IF_RSI_WSC_HEADER_H

#if defined(RSI_GLOBAL_VARIABLES) || defined(PS_DEPENDENT_VARIABLES) || defined(DATA_PATH_UMAC_ENABLE)

#include "sl_wlan_data_types.h"

void set_rejoin_going_on(uint16 rejoin_going_on);
void set_roam_ind(uint8 roam_ind);
void rsi_dhcp_release(uint8 *buf);
void update_umac_pmmode_cmd_info();
void set_okc(int32 okc);
void set_client_mode(uint8 client_mode);
void set_module_state(uint8 state);
void sc_save_listen_interval(uint32 listen_interval);
void sc_save_join_feature_bitmap(uint8 join_feature_bitmap);
void sc_save_eap_method(uint8 *eap_method);
void sc_save_private_key_passwd(uint8 *private_key_passwd);
void clear_private_key_passwd();
void set_prev_pn(uint8_t *src, uint8 len);

#endif /* RSI_GLOBAL_VARIABLES || PS_DEPENDENT_VARIABLES || DATA_PATH_UMAC_ENABLE */
#endif /* MGMT_IF_RSI_WSC_HEADER_H */
