/*******************************************************************************
* @file  mgmt_if_init.h
* @brief This file contains functions related to initializations
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
#ifndef MGMT_IF_INIT_H
#define MGMT_IF_INIT_H

#include "mgmt_supplicant_if.h"

void set_security_protocol(void *ssid, uint8 auth_mode);
int16 start_sta(void *ctx, uint8 *psk, uint8 *ssid_str, uint8 auth_mode);
int16 start_wps_sta(void *ctx);
int ap_supp_start(void *wpa_s);
int wsc_supp_start();
int16 start_enterprise_security(void *wpa_s, uint8 *bssid, uint8 *tmp_ssid, uint8 auth_mode);
void wpas_start_wps_apconf(void *wpa_s);
void init_wsc_supp_interface(); // TODO: This function shall be moved to mgmt_if_init.c

#endif /* MGMT_IF_INIT_H */
