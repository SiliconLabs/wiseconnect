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
