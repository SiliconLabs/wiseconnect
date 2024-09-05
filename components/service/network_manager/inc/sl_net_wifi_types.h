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

/** \addtogroup SL_NET_TYPES
 * @{ */
/**
 * @brief Network Wi-Fi client profile.
 * 
 * @details
 * This structure holds the configuration for a Wi-Fi client profile, including the Wi-Fi client settings and the network IP configuration.
 */
typedef struct {
  sl_wifi_client_configuration_t
    config; ///< Wi-Fi client configuration of type [sl_wifi_client_configuration_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-client-configuration-t)
  sl_net_ip_configuration_t ip; ///< Network IP configuration of type @ref sl_net_ip_configuration_t
} sl_net_wifi_client_profile_t;

/**
 * @brief Network Wi-Fi AP profile.
 * 
 * @details
 * This structure holds the configuration for a Wi-Fi Access Point (AP) profile, including the Wi-Fi AP settings and the network IP configuration. 
 */
typedef struct {
  sl_wifi_ap_configuration_t
    config; ///< Wi-Fi AP configuration of type [sl_wifi_ap_configuration_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-ap-configuration-t)
  sl_net_ip_configuration_t ip; ///< Network IP configuration of type @ref sl_net_ip_configuration_t
} sl_net_wifi_ap_profile_t;

/**
 * @brief Network Wi-Fi PSK credential entry.
 * 
 * @details
 * This structure holds the Pre-Shared Key (PSK) credentials for a Wi-Fi network, including the credential type, data length, and the actual credential data.
 */
typedef struct {
  sl_net_credential_type_t type; ///< Network credential type of @ref sl_net_credential_type_t
  uint16_t data_length;          ///< Data length
  uint8_t data[196];             ///< Data
} sl_net_wifi_psk_credential_entry_t;

/**
 * @brief Network Wi-Fi EAP credential entry.
 * 
 * @details
 * This structure holds the Extensible Authentication Protocol (EAP) credentials for a Wi-Fi network, including the credential type, data length, and the actual EAP credential data.
 */
typedef struct {
  sl_net_credential_type_t type; ///< Network credential type of @ref sl_net_credential_type_t
  uint16_t data_length;          ///< Data length
  sl_wifi_eap_credential_t
    data; ///< Data of type [sl_wifi_eap_credential_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-eap-credential-t)
} sl_net_wifi_eap_credential_entry_t;
/** @} */
