/*******************************************************************************
* @file  sl_net_wifi_types.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#pragma once
#include "sl_net_types.h"
#include "sl_net_ip_types.h"
#include "sl_wifi_device.h" // The device specific Wi-Fi structures and definitions
#include "sl_wifi_types.h"

typedef sl_wifi_device_configuration_t sl_net_wifi_configuration_t;

typedef struct {
  sl_wifi_client_configuration_t config;
  sl_net_ip_configuration_t ip;
} sl_net_wifi_client_profile_t;

typedef struct {
  sl_wifi_ap_configuration_t config;
  sl_net_ip_configuration_t ip;
} sl_net_wifi_ap_profile_t;

//typedef union {
//  sl_net_wifi_client_profile_t wifi_client_profile;
//  sl_net_wifi_ap_profile_t wifi_ap_profile;
//} sl_net_profile_t;

typedef struct {
  sl_net_credential_type_t type;
  uint16_t data_length;
  uint8_t data[196];
} sl_net_wifi_psk_credential_entry_t;

typedef struct {
  sl_net_credential_type_t type;
  uint16_t data_length;
  sl_wifi_eap_credential_t data;
} sl_net_wifi_eap_credential_entry_t;
