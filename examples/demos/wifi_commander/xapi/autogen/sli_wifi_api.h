/********************************************************************************
 * @file  sli_wifi_api.h
 * @brief SL_WIFI_API library at application layer provided as source code
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SLI_WIFI_API_H
#define SLI_WIFI_API_H
#define VERSION no_link_tables

#include "sl_wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

enum app_wifi_class_id {
  SLI_WIFI_SYSTEM_CLASS_ID      = 0x00,
  SLI_WIFI_NET_INTF_CLASS_ID    = 0x11,
  SLI_WIFI_NET_PROFILE_CLASS_ID = 0x12,
  SLI_WIFI_AP_CLASS_ID          = 0x05,
  SLI_WIFI_NET_CRED_CLASS_ID    = 0x13,
  SLI_WIFI_COMMON_CLASS_ID      = 0x01,
  SLI_WIFI_SCAN_CLASS_ID        = 0x03,
  SLI_WIFI_PING_CLASS_ID        = 0x15,
  SLI_WIFI_CLIENT_CLASS_ID      = 0x04,
};

enum app_wifi_command_id {
  APP_WIFI_SYSTEM_HELLO_COMMAND_ID                        = 0x00,
  APP_WIFI_SYSTEM_ECHO_COMMAND_ID                         = 0x01,
  APP_WIFI_SYSTEM_RESET_COMMAND_ID                        = 0x02,
  APP_WIFI_NET_INTF_INIT_COMMAND_ID                       = 0x00,
  APP_WIFI_NET_INTF_DEINIT_COMMAND_ID                     = 0x01,
  APP_WIFI_NET_INTF_UP_COMMAND_ID                         = 0x02,
  APP_WIFI_NET_INTF_DOWN_COMMAND_ID                       = 0x03,
  APP_WIFI_NET_INTF_SET_DEVICE_CONFIG_COMMAND_ID          = 0x04,
  APP_WIFI_NET_INTF_GET_INIT_STATUS_COMMAND_ID            = 0x06,
  APP_WIFI_NET_INTF_GET_NETWORK_STATUS_COMMAND_ID         = 0x07,
  APP_WIFI_NET_INTF_GET_DEVICE_CONFIG_COMMAND_ID          = 0x08,
  APP_WIFI_NET_INTF_SET_SI91X_BOOT_CONFIG_COMMAND_ID      = 0x05,
  APP_WIFI_NET_INTF_GET_SI91X_BOOT_CONFIG_COMMAND_ID      = 0x09,
  APP_WIFI_NET_INTF_LOAD_DEFAULT_DEVICE_CONFIG_COMMAND_ID = 0x0A,
  APP_WIFI_NET_PROFILE_SET_COMMAND_ID                     = 0x00,
  APP_WIFI_NET_PROFILE_DELETE_COMMAND_ID                  = 0x02,
  APP_WIFI_NET_PROFILE_SET_AP_SSID_COMMAND_ID             = 0x03,
  APP_WIFI_NET_PROFILE_SET_AP_CONFIG_COMMAND_ID           = 0x04,
  APP_WIFI_NET_PROFILE_SET_AP_CHANNEL_COMMAND_ID          = 0x05,
  APP_WIFI_NET_PROFILE_SET_AP_IP_CONFIG_COMMAND_ID        = 0x06,
  APP_WIFI_NET_PROFILE_SET_AP_IPV4_CONFIG_COMMAND_ID      = 0x07,
  APP_WIFI_NET_PROFILE_SET_AP_IPV6_CONFIG_COMMAND_ID      = 0x08,
  APP_WIFI_NET_PROFILE_GET_AP_SSID_COMMAND_ID             = 0x01,
  APP_WIFI_NET_PROFILE_GET_AP_CONFIG_COMMAND_ID           = 0x09,
  APP_WIFI_NET_PROFILE_GET_AP_CHANNEL_COMMAND_ID          = 0x0A,
  APP_WIFI_NET_PROFILE_GET_AP_IP_CONFIG_COMMAND_ID        = 0x0B,
  APP_WIFI_NET_PROFILE_GET_AP_IPV4_CONFIG_COMMAND_ID      = 0x0C,
  APP_WIFI_NET_PROFILE_GET_AP_IPV6_CONFIG_COMMAND_ID      = 0x0D,
  APP_WIFI_NET_PROFILE_SET_CLIENT_SSID_COMMAND_ID         = 0x15,
  APP_WIFI_NET_PROFILE_SET_CLIENT_CONFIG_COMMAND_ID       = 0x16,
  APP_WIFI_NET_PROFILE_SET_CLIENT_CHANNEL_COMMAND_ID      = 0x10,
  APP_WIFI_NET_PROFILE_GET_CLIENT_SSID_COMMAND_ID         = 0x11,
  APP_WIFI_NET_PROFILE_GET_CLIENT_CONFIG_COMMAND_ID       = 0x12,
  APP_WIFI_NET_PROFILE_GET_CLIENT_CHANNEL_COMMAND_ID      = 0x13,
  APP_WIFI_AP_DISCONNECT_CLIENT_COMMAND_ID                = 0x01,
  APP_WIFI_AP_GET_CLIENT_INFO_COMMAND_ID                  = 0x00,
  APP_WIFI_NET_CRED_SET_COMMAND_ID                        = 0x00,
  APP_WIFI_NET_CRED_GET_COMMAND_ID                        = 0x01,
  APP_WIFI_NET_CRED_DELETE_COMMAND_ID                     = 0x02,
  APP_WIFI_COMMON_GET_FIRMWARE_VERSION_COMMAND_ID         = 0x00,
  APP_WIFI_SCAN_START_COMMAND_ID                          = 0x00,
  APP_WIFI_SCAN_START_WITH_SSID_COMMAND_ID                = 0x01,
  APP_WIFI_SCAN_SET_ADVANCED_CONFIGURATION_COMMAND_ID     = 0x02,
  APP_WIFI_SCAN_GET_ADVANCED_CONFIGURATION_COMMAND_ID     = 0x03,
  APP_WIFI_SCAN_STOP_COMMAND_ID                           = 0x04,
  APP_WIFI_PING_IPV4_COMMAND_ID                           = 0x00,
  APP_WIFI_CLIENT_SET_ROAM_CONFIGURATION_COMMAND_ID       = 0x02,
  APP_WIFI_CLIENT_GET_ROAM_CONFIGURATION_COMMAND_ID       = 0x03,
};

enum app_wifi_response_id {
  APP_WIFI_SYSTEM_HELLO_RESPONSE_ID                        = 0x00,
  APP_WIFI_SYSTEM_ECHO_RESPONSE_ID                         = 0x01,
  APP_WIFI_SYSTEM_RESET_RESPONSE_ID                        = 0x02,
  APP_WIFI_NET_INTF_INIT_RESPONSE_ID                       = 0x00,
  APP_WIFI_NET_INTF_DEINIT_RESPONSE_ID                     = 0x01,
  APP_WIFI_NET_INTF_UP_RESPONSE_ID                         = 0x02,
  APP_WIFI_NET_INTF_DOWN_RESPONSE_ID                       = 0x03,
  APP_WIFI_NET_INTF_SET_DEVICE_CONFIG_RESPONSE_ID          = 0x04,
  APP_WIFI_NET_INTF_GET_INIT_STATUS_RESPONSE_ID            = 0x06,
  APP_WIFI_NET_INTF_GET_NETWORK_STATUS_RESPONSE_ID         = 0x07,
  APP_WIFI_NET_INTF_GET_DEVICE_CONFIG_RESPONSE_ID          = 0x08,
  APP_WIFI_NET_INTF_SET_SI91X_BOOT_CONFIG_RESPONSE_ID      = 0x05,
  APP_WIFI_NET_INTF_GET_SI91X_BOOT_CONFIG_RESPONSE_ID      = 0x09,
  APP_WIFI_NET_INTF_LOAD_DEFAULT_DEVICE_CONFIG_RESPONSE_ID = 0x0A,
  APP_WIFI_NET_PROFILE_SET_RESPONSE_ID                     = 0x00,
  APP_WIFI_NET_PROFILE_DELETE_RESPONSE_ID                  = 0x02,
  APP_WIFI_NET_PROFILE_SET_AP_SSID_RESPONSE_ID             = 0x03,
  APP_WIFI_NET_PROFILE_SET_AP_CONFIG_RESPONSE_ID           = 0x04,
  APP_WIFI_NET_PROFILE_SET_AP_CHANNEL_RESPONSE_ID          = 0x05,
  APP_WIFI_NET_PROFILE_SET_AP_IP_CONFIG_RESPONSE_ID        = 0x06,
  APP_WIFI_NET_PROFILE_SET_AP_IPV4_CONFIG_RESPONSE_ID      = 0x07,
  APP_WIFI_NET_PROFILE_SET_AP_IPV6_CONFIG_RESPONSE_ID      = 0x08,
  APP_WIFI_NET_PROFILE_GET_AP_SSID_RESPONSE_ID             = 0x01,
  APP_WIFI_NET_PROFILE_GET_AP_CONFIG_RESPONSE_ID           = 0x09,
  APP_WIFI_NET_PROFILE_GET_AP_CHANNEL_RESPONSE_ID          = 0x0A,
  APP_WIFI_NET_PROFILE_GET_AP_IP_CONFIG_RESPONSE_ID        = 0x0B,
  APP_WIFI_NET_PROFILE_GET_AP_IPV4_CONFIG_RESPONSE_ID      = 0x0C,
  APP_WIFI_NET_PROFILE_GET_AP_IPV6_CONFIG_RESPONSE_ID      = 0x0D,
  APP_WIFI_NET_PROFILE_SET_CLIENT_SSID_RESPONSE_ID         = 0x15,
  APP_WIFI_NET_PROFILE_SET_CLIENT_CONFIG_RESPONSE_ID       = 0x16,
  APP_WIFI_NET_PROFILE_SET_CLIENT_CHANNEL_RESPONSE_ID      = 0x10,
  APP_WIFI_NET_PROFILE_GET_CLIENT_SSID_RESPONSE_ID         = 0x11,
  APP_WIFI_NET_PROFILE_GET_CLIENT_CONFIG_RESPONSE_ID       = 0x12,
  APP_WIFI_NET_PROFILE_GET_CLIENT_CHANNEL_RESPONSE_ID      = 0x13,
  APP_WIFI_AP_DISCONNECT_CLIENT_RESPONSE_ID                = 0x01,
  APP_WIFI_AP_GET_CLIENT_INFO_RESPONSE_ID                  = 0x00,
  APP_WIFI_NET_CRED_SET_RESPONSE_ID                        = 0x00,
  APP_WIFI_NET_CRED_GET_RESPONSE_ID                        = 0x01,
  APP_WIFI_NET_CRED_DELETE_RESPONSE_ID                     = 0x02,
  APP_WIFI_COMMON_GET_FIRMWARE_VERSION_RESPONSE_ID         = 0x00,
  APP_WIFI_SCAN_START_RESPONSE_ID                          = 0x00,
  APP_WIFI_SCAN_START_WITH_SSID_RESPONSE_ID                = 0x01,
  APP_WIFI_SCAN_SET_ADVANCED_CONFIGURATION_RESPONSE_ID     = 0x02,
  APP_WIFI_SCAN_GET_ADVANCED_CONFIGURATION_RESPONSE_ID     = 0x03,
  APP_WIFI_SCAN_STOP_RESPONSE_ID                           = 0x04,
  APP_WIFI_PING_IPV4_RESPONSE_ID                           = 0x00,
  APP_WIFI_CLIENT_SET_ROAM_CONFIGURATION_RESPONSE_ID       = 0x02,
  APP_WIFI_CLIENT_GET_ROAM_CONFIGURATION_RESPONSE_ID       = 0x03,
};

enum app_wifi_event_id {
  APP_WIFI_SYSTEM_BOOT_EVENT_ID                   = 0x00,
  APP_WIFI_SYSTEM_ERROR_EVENT_ID                  = 0x01,
  APP_WIFI_NET_INTF_NETWORK_UP_COMPLETED_EVENT_ID = 0x00,
  APP_WIFI_AP_CLIENT_CONNECTED_EVENT_EVENT_ID     = 0x00,
  APP_WIFI_AP_CLIENT_DISCONNECTED_EVENT_EVENT_ID  = 0x01,
  APP_WIFI_AP_GET_CLIENT_INFO_EVENT_EVENT_ID      = 0x02,
  APP_WIFI_SCAN_RESULTS_EVENT_EVENT_ID            = 0x00,
  APP_WIFI_SCAN_ERROR_EVENT_EVENT_ID              = 0x01,
  APP_WIFI_SCAN_COMPLETE_EVENT_EVENT_ID           = 0x02,
  APP_WIFI_PING_IPV4_RESPONSE_EVENT_EVENT_ID      = 0x00,
  APP_WIFI_CLIENT_JOIN_EVENT_EVENT_ID             = 0x00,
  APP_WIFI_CLIENT_STATS_RESPONSE_EVENT_EVENT_ID   = 0x01,
};

PACKSTRUCT(struct app_wifi_rsp_error_s { uint16_t result; });

typedef struct app_wifi_rsp_error_s app_wifi_rsp_error_t;

PACKSTRUCT(struct app_wifi_cmd_system_echo_s { uint32_t number_in; });

typedef struct app_wifi_cmd_system_echo_s app_wifi_cmd_system_echo_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_init_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_init_s app_wifi_cmd_net_intf_init_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_deinit_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_deinit_s app_wifi_cmd_net_intf_deinit_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_up_s {
  uint8_t net_interface;
  uint8_t profile_id;
});

typedef struct app_wifi_cmd_net_intf_up_s app_wifi_cmd_net_intf_up_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_down_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_down_s app_wifi_cmd_net_intf_down_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_set_device_config_s {
  uint8_t boot_option;
  uint8_t mac_address_octet_0;
  uint8_t mac_address_octet_1;
  uint8_t mac_address_octet_2;
  uint8_t mac_address_octet_3;
  uint8_t mac_address_octet_4;
  uint8_t mac_address_octet_5;
  uint8_t band;
  uint8_t region_code;
  uint8_t tx_ratio_in_buffer_pool;
  uint8_t rx_ratio_in_buffer_pool;
  uint8_t global_ratio_in_buffer_pool;
  uint8_t efuse_data_type;
});

typedef struct app_wifi_cmd_net_intf_set_device_config_s app_wifi_cmd_net_intf_set_device_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_get_init_status_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_get_init_status_s app_wifi_cmd_net_intf_get_init_status_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_get_network_status_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_get_network_status_s app_wifi_cmd_net_intf_get_network_status_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_set_si91x_boot_config_s {
  uint16_t oper_mode;
  uint16_t coex_mode;
  uint32_t feature_bit_map;
  uint32_t tcp_ip_feature_bit_map;
  uint32_t custom_feature_bit_map;
  uint32_t ext_custom_feature_bit_map;
  uint32_t bt_feature_bit_map;
  uint32_t ext_tcp_ip_feature_bit_map;
  uint32_t ble_feature_bit_map;
  uint32_t ble_ext_feature_bit_map;
  uint32_t config_feature_bit_map;
});

typedef struct app_wifi_cmd_net_intf_set_si91x_boot_config_s app_wifi_cmd_net_intf_set_si91x_boot_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_intf_load_default_device_config_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_net_intf_load_default_device_config_s app_wifi_cmd_net_intf_load_default_device_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_s {
  uint8_t net_interface;
  uint8_t profile_id;
});

typedef struct app_wifi_cmd_net_profile_set_s app_wifi_cmd_net_profile_set_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_delete_s {
  uint8_t net_interface;
  uint8_t profile_id;
});

typedef struct app_wifi_cmd_net_profile_delete_s app_wifi_cmd_net_profile_delete_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_ssid_s { uint8array ssid; });

typedef struct app_wifi_cmd_net_profile_set_ap_ssid_s app_wifi_cmd_net_profile_set_ap_ssid_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_config_s {
  uint16_t ap_security;
  uint8_t ap_encryption;
  uint8_t ap_rate_protocol;
  uint8_t ap_options;
  uint32_t ap_credential_id;
  uint8_t ap_keepalive_type;
  uint16_t ap_beacon_interval;
  uint32_t ap_client_idle_timeout;
  uint16_t ap_dtim_beacon_count;
  uint8_t ap_maximum_clients;
  uint8_t ap_beacon_stop;
  uint8_t ap_tdi;
  uint8_t ap_is_11n_enabled;
});

typedef struct app_wifi_cmd_net_profile_set_ap_config_s app_wifi_cmd_net_profile_set_ap_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_channel_s {
  uint16_t channel;
  uint8_t band;
  uint8_t bandwidth;
});

typedef struct app_wifi_cmd_net_profile_set_ap_channel_s app_wifi_cmd_net_profile_set_ap_channel_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_ip_config_s {
  uint8_t mode;
  uint8_t type;
});

typedef struct app_wifi_cmd_net_profile_set_ap_ip_config_s app_wifi_cmd_net_profile_set_ap_ip_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_ipv4_config_s {
  ipv4_addr ipv4_address;
  ipv4_addr gateway;
  ipv4_addr netmask;
});

typedef struct app_wifi_cmd_net_profile_set_ap_ipv4_config_s app_wifi_cmd_net_profile_set_ap_ipv4_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_ap_ipv6_config_s {
  uint8_t link_local_address_octet_1;
  uint8_t link_local_address_octet_2;
  uint8_t link_local_address_octet_3;
  uint8_t link_local_address_octet_4;
  uint8_t link_local_address_octet_5;
  uint8_t link_local_address_octet_6;
  uint8_t link_local_address_octet_7;
  uint8_t link_local_address_octet_8;
  uint8_t link_local_address_octet_9;
  uint8_t link_local_address_octet_10;
  uint8_t link_local_address_octet_11;
  uint8_t link_local_address_octet_12;
  uint8_t link_local_address_octet_13;
  uint8_t link_local_address_octet_14;
  uint8_t link_local_address_octet_15;
  uint8_t link_local_address_octet_16;
  uint8_t global_address_octet_1;
  uint8_t global_address_octet_2;
  uint8_t global_address_octet_3;
  uint8_t global_address_octet_4;
  uint8_t global_address_octet_5;
  uint8_t global_address_octet_6;
  uint8_t global_address_octet_7;
  uint8_t global_address_octet_8;
  uint8_t global_address_octet_9;
  uint8_t global_address_octet_10;
  uint8_t global_address_octet_11;
  uint8_t global_address_octet_12;
  uint8_t global_address_octet_13;
  uint8_t global_address_octet_14;
  uint8_t global_address_octet_15;
  uint8_t global_address_octet_16;
  uint8_t gateway_octet_1;
  uint8_t gateway_octet_2;
  uint8_t gateway_octet_3;
  uint8_t gateway_octet_4;
  uint8_t gateway_octet_5;
  uint8_t gateway_octet_6;
  uint8_t gateway_octet_7;
  uint8_t gateway_octet_8;
  uint8_t gateway_octet_9;
  uint8_t gateway_octet_10;
  uint8_t gateway_octet_11;
  uint8_t gateway_octet_12;
  uint8_t gateway_octet_13;
  uint8_t gateway_octet_14;
  uint8_t gateway_octet_15;
  uint8_t gateway_octet_16;
});

typedef struct app_wifi_cmd_net_profile_set_ap_ipv6_config_s app_wifi_cmd_net_profile_set_ap_ipv6_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_client_ssid_s { uint8array ssid; });

typedef struct app_wifi_cmd_net_profile_set_client_ssid_s app_wifi_cmd_net_profile_set_client_ssid_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_client_config_s {
  sl_wifi_bssid_t bssid;
  uint16_t bss_type;
  uint16_t client_security;
  uint8_t client_encryption;
  uint32_t client_options;
  uint32_t credential_id;
  uint16_t channel_bitmap_2_4g;
});

typedef struct app_wifi_cmd_net_profile_set_client_config_s app_wifi_cmd_net_profile_set_client_config_t;

PACKSTRUCT(struct app_wifi_cmd_net_profile_set_client_channel_s {
  uint16_t channel;
  uint8_t band;
  uint8_t bandwidth;
});

typedef struct app_wifi_cmd_net_profile_set_client_channel_s app_wifi_cmd_net_profile_set_client_channel_t;

PACKSTRUCT(struct app_wifi_cmd_ap_disconnect_client_s {
  uint8_t net_interface;
  bd_addr mac_address;
  uint8_t reason;
});

typedef struct app_wifi_cmd_ap_disconnect_client_s app_wifi_cmd_ap_disconnect_client_t;

PACKSTRUCT(struct app_wifi_cmd_ap_get_client_info_s { uint8_t net_interface; });

typedef struct app_wifi_cmd_ap_get_client_info_s app_wifi_cmd_ap_get_client_info_t;

PACKSTRUCT(struct app_wifi_cmd_net_cred_set_s {
  uint32_t id;
  uint32_t type;
  uint8array credential;
});

typedef struct app_wifi_cmd_net_cred_set_s app_wifi_cmd_net_cred_set_t;

PACKSTRUCT(struct app_wifi_cmd_net_cred_get_s {
  uint32_t id;
  uint32_t type;
});

typedef struct app_wifi_cmd_net_cred_get_s app_wifi_cmd_net_cred_get_t;

PACKSTRUCT(struct app_wifi_cmd_net_cred_delete_s {
  uint32_t id;
  uint32_t type;
});

typedef struct app_wifi_cmd_net_cred_delete_s app_wifi_cmd_net_cred_delete_t;

PACKSTRUCT(struct app_wifi_cmd_scan_start_s {
  uint8_t wifi_interface;
  uint8_t mode;
  uint32_t flags;
  uint16_t channel_bitmap_2_4ghz;
  uint8_t lp_mode;
});

typedef struct app_wifi_cmd_scan_start_s app_wifi_cmd_scan_start_t;

PACKSTRUCT(struct app_wifi_cmd_scan_start_with_ssid_s {
  uint8_t wifi_interface;
  uint8array ssid;
  uint8_t mode;
  uint32_t flags;
  uint16_t channel_bitmap_2_4ghz;
  uint8_t lp_mode;
});

typedef struct app_wifi_cmd_scan_start_with_ssid_s app_wifi_cmd_scan_start_with_ssid_t;

PACKSTRUCT(struct app_wifi_cmd_scan_set_advanced_configuration_s {
  int32_t trigger_level;
  uint32_t trigger_level_change;
  uint16_t active_channel_time;
  uint16_t passive_channel_time;
  uint8_t enable_instant_scan;
  uint8_t enable_multi_probe;
});

typedef struct app_wifi_cmd_scan_set_advanced_configuration_s app_wifi_cmd_scan_set_advanced_configuration_t;

PACKSTRUCT(struct app_wifi_cmd_scan_stop_s { uint8_t wifi_interface; });

typedef struct app_wifi_cmd_scan_stop_s app_wifi_cmd_scan_stop_t;

PACKSTRUCT(struct app_wifi_cmd_ping_ipv4_s {
  ipv4_addr ipv4_address;
  uint16_t ping_size;
});

typedef struct app_wifi_cmd_ping_ipv4_s app_wifi_cmd_ping_ipv4_t;

PACKSTRUCT(struct app_wifi_cmd_client_set_roam_configuration_s {
  uint8_t wifi_interface;
  int32_t trigger_level;
  uint32_t trigger_level_change;
});

typedef struct app_wifi_cmd_client_set_roam_configuration_s app_wifi_cmd_client_set_roam_configuration_t;

PACKSTRUCT(struct app_wifi_cmd_client_get_roam_configuration_s { uint8_t wifi_interface; });

typedef struct app_wifi_cmd_client_get_roam_configuration_s app_wifi_cmd_client_get_roam_configuration_t;

PACKSTRUCT(struct app_wifi_rsp_system_hello_s { uint16_t result; });

typedef struct app_wifi_rsp_system_hello_s app_wifi_rsp_system_hello_t;

PACKSTRUCT(struct app_wifi_rsp_system_echo_s {
  uint16_t result;
  uint32_t number_out;
});

typedef struct app_wifi_rsp_system_echo_s app_wifi_rsp_system_echo_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_init_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_init_s app_wifi_rsp_net_intf_init_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_deinit_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_deinit_s app_wifi_rsp_net_intf_deinit_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_up_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_up_s app_wifi_rsp_net_intf_up_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_down_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_down_s app_wifi_rsp_net_intf_down_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_set_device_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_set_device_config_s app_wifi_rsp_net_intf_set_device_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_get_init_status_s {
  uint16_t result;
  uint8_t is_init;
});

typedef struct app_wifi_rsp_net_intf_get_init_status_s app_wifi_rsp_net_intf_get_init_status_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_get_network_status_s {
  uint16_t result;
  uint8_t is_up;
});

typedef struct app_wifi_rsp_net_intf_get_network_status_s app_wifi_rsp_net_intf_get_network_status_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_get_device_config_s {
  uint16_t result;
  uint8_t boot_option;
  uint8_t mac_address_octet_0;
  uint8_t mac_address_octet_1;
  uint8_t mac_address_octet_2;
  uint8_t mac_address_octet_3;
  uint8_t mac_address_octet_4;
  uint8_t mac_address_octet_5;
  uint8_t band;
  uint8_t region_code;
  uint8_t tx_ratio_in_buffer_pool;
  uint8_t rx_ratio_in_buffer_pool;
  uint8_t global_ratio_in_buffer_pool;
  uint8_t efuse_data_type;
});

typedef struct app_wifi_rsp_net_intf_get_device_config_s app_wifi_rsp_net_intf_get_device_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_set_si91x_boot_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_set_si91x_boot_config_s app_wifi_rsp_net_intf_set_si91x_boot_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_get_si91x_boot_config_s {
  uint16_t result;
  uint16_t oper_mode;
  uint16_t coex_mode;
  uint32_t feature_bit_map;
  uint32_t tcp_ip_feature_bit_map;
  uint32_t custom_feature_bit_map;
  uint32_t ext_custom_feature_bit_map;
  uint32_t bt_feature_bit_map;
  uint32_t ext_tcp_ip_feature_bit_map;
  uint32_t ble_feature_bit_map;
  uint32_t ble_ext_feature_bit_map;
  uint32_t config_feature_bit_map;
});

typedef struct app_wifi_rsp_net_intf_get_si91x_boot_config_s app_wifi_rsp_net_intf_get_si91x_boot_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_intf_load_default_device_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_intf_load_default_device_config_s app_wifi_rsp_net_intf_load_default_device_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_s app_wifi_rsp_net_profile_set_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_delete_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_delete_s app_wifi_rsp_net_profile_delete_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_ssid_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_ssid_s app_wifi_rsp_net_profile_set_ap_ssid_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_config_s app_wifi_rsp_net_profile_set_ap_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_channel_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_channel_s app_wifi_rsp_net_profile_set_ap_channel_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_ip_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_ip_config_s app_wifi_rsp_net_profile_set_ap_ip_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_ipv4_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_ipv4_config_s app_wifi_rsp_net_profile_set_ap_ipv4_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_ap_ipv6_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_ap_ipv6_config_s app_wifi_rsp_net_profile_set_ap_ipv6_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_ssid_s {
  uint16_t result;
  uint8array ssid;
});

typedef struct app_wifi_rsp_net_profile_get_ap_ssid_s app_wifi_rsp_net_profile_get_ap_ssid_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_config_s {
  uint16_t result;
  uint16_t ap_security;
  uint8_t ap_encryption;
  uint8_t ap_rate_protocol;
  uint8_t ap_options;
  uint32_t ap_credential_id;
  uint8_t ap_keepalive_type;
  uint16_t ap_beacon_interval;
  uint32_t ap_client_idle_timeout;
  uint16_t ap_dtim_beacon_count;
  uint8_t ap_maximum_clients;
  uint8_t ap_beacon_stop;
  uint8_t ap_tdi;
  uint8_t ap_is_11n_enabled;
});

typedef struct app_wifi_rsp_net_profile_get_ap_config_s app_wifi_rsp_net_profile_get_ap_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_channel_s {
  uint16_t result;
  uint16_t channel;
  uint8_t band;
  uint8_t bandwidth;
});

typedef struct app_wifi_rsp_net_profile_get_ap_channel_s app_wifi_rsp_net_profile_get_ap_channel_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_ip_config_s {
  uint16_t result;
  uint8_t mode;
  uint8_t type;
});

typedef struct app_wifi_rsp_net_profile_get_ap_ip_config_s app_wifi_rsp_net_profile_get_ap_ip_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_ipv4_config_s {
  uint16_t result;
  ipv4_addr ipv4_address;
  ipv4_addr gateway;
  ipv4_addr netmask;
});

typedef struct app_wifi_rsp_net_profile_get_ap_ipv4_config_s app_wifi_rsp_net_profile_get_ap_ipv4_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_ap_ipv6_config_s {
  uint16_t result;
  uint8_t link_local_address_octet_1;
  uint8_t link_local_address_octet_2;
  uint8_t link_local_address_octet_3;
  uint8_t link_local_address_octet_4;
  uint8_t link_local_address_octet_5;
  uint8_t link_local_address_octet_6;
  uint8_t link_local_address_octet_7;
  uint8_t link_local_address_octet_8;
  uint8_t link_local_address_octet_9;
  uint8_t link_local_address_octet_10;
  uint8_t link_local_address_octet_11;
  uint8_t link_local_address_octet_12;
  uint8_t link_local_address_octet_13;
  uint8_t link_local_address_octet_14;
  uint8_t link_local_address_octet_15;
  uint8_t link_local_address_octet_16;
  uint8_t global_address_octet_1;
  uint8_t global_address_octet_2;
  uint8_t global_address_octet_3;
  uint8_t global_address_octet_4;
  uint8_t global_address_octet_5;
  uint8_t global_address_octet_6;
  uint8_t global_address_octet_7;
  uint8_t global_address_octet_8;
  uint8_t global_address_octet_9;
  uint8_t global_address_octet_10;
  uint8_t global_address_octet_11;
  uint8_t global_address_octet_12;
  uint8_t global_address_octet_13;
  uint8_t global_address_octet_14;
  uint8_t global_address_octet_15;
  uint8_t global_address_octet_16;
  uint8_t gateway_address_octet_1;
  uint8_t gateway_address_octet_2;
  uint8_t gateway_address_octet_3;
  uint8_t gateway_address_octet_4;
  uint8_t gateway_address_octet_5;
  uint8_t gateway_address_octet_6;
  uint8_t gateway_address_octet_7;
  uint8_t gateway_address_octet_8;
  uint8_t gateway_address_octet_9;
  uint8_t gateway_address_octet_10;
  uint8_t gateway_address_octet_11;
  uint8_t gateway_address_octet_12;
  uint8_t gateway_address_octet_13;
  uint8_t gateway_address_octet_14;
  uint8_t gateway_address_octet_15;
  uint8_t gateway_address_octet_16;
});

typedef struct app_wifi_rsp_net_profile_get_ap_ipv6_config_s app_wifi_rsp_net_profile_get_ap_ipv6_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_client_ssid_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_client_ssid_s app_wifi_rsp_net_profile_set_client_ssid_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_client_config_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_client_config_s app_wifi_rsp_net_profile_set_client_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_set_client_channel_s { uint16_t result; });

typedef struct app_wifi_rsp_net_profile_set_client_channel_s app_wifi_rsp_net_profile_set_client_channel_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_client_ssid_s {
  uint16_t result;
  uint8array ssid;
});

typedef struct app_wifi_rsp_net_profile_get_client_ssid_s app_wifi_rsp_net_profile_get_client_ssid_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_client_config_s {
  uint16_t result;
  sl_wifi_bssid_t bssid;
  uint16_t bss_type;
  uint16_t client_security;
  uint8_t client_encryption;
  uint32_t client_options;
  uint32_t credential_id;
  uint16_t channel_bitmap_2_4g;
});

typedef struct app_wifi_rsp_net_profile_get_client_config_s app_wifi_rsp_net_profile_get_client_config_t;

PACKSTRUCT(struct app_wifi_rsp_net_profile_get_client_channel_s {
  uint16_t result;
  uint16_t channel;
  uint8_t band;
  uint8_t bandwidth;
});

typedef struct app_wifi_rsp_net_profile_get_client_channel_s app_wifi_rsp_net_profile_get_client_channel_t;

PACKSTRUCT(struct app_wifi_rsp_ap_disconnect_client_s { uint16_t result; });

typedef struct app_wifi_rsp_ap_disconnect_client_s app_wifi_rsp_ap_disconnect_client_t;

PACKSTRUCT(struct app_wifi_rsp_ap_get_client_info_s {
  uint16_t result;
  uint8_t client_count;
});

typedef struct app_wifi_rsp_ap_get_client_info_s app_wifi_rsp_ap_get_client_info_t;

PACKSTRUCT(struct app_wifi_rsp_net_cred_set_s { uint16_t result; });

typedef struct app_wifi_rsp_net_cred_set_s app_wifi_rsp_net_cred_set_t;

PACKSTRUCT(struct app_wifi_rsp_net_cred_get_s {
  uint16_t result;
  uint32_t credential_length;
  uint8array credential;
});

typedef struct app_wifi_rsp_net_cred_get_s app_wifi_rsp_net_cred_get_t;

PACKSTRUCT(struct app_wifi_rsp_net_cred_delete_s { uint16_t result; });

typedef struct app_wifi_rsp_net_cred_delete_s app_wifi_rsp_net_cred_delete_t;

PACKSTRUCT(struct app_wifi_rsp_common_get_firmware_version_s {
  uint16_t result;
  uint8_t chip_id;
  uint8_t rom_id;
  uint8_t major;
  uint8_t minor;
  uint8_t security_version;
  uint8_t patch_num;
  uint8_t customer_id;
  uint16_t build_num;
});

typedef struct app_wifi_rsp_common_get_firmware_version_s app_wifi_rsp_common_get_firmware_version_t;

PACKSTRUCT(struct app_wifi_rsp_scan_start_s {
  uint16_t result;
  uint8_t is_in_progress;
});

typedef struct app_wifi_rsp_scan_start_s app_wifi_rsp_scan_start_t;

PACKSTRUCT(struct app_wifi_rsp_scan_start_with_ssid_s {
  uint16_t result;
  uint8_t is_in_progress;
});

typedef struct app_wifi_rsp_scan_start_with_ssid_s app_wifi_rsp_scan_start_with_ssid_t;

PACKSTRUCT(struct app_wifi_rsp_scan_set_advanced_configuration_s { uint16_t result; });

typedef struct app_wifi_rsp_scan_set_advanced_configuration_s app_wifi_rsp_scan_set_advanced_configuration_t;

PACKSTRUCT(struct app_wifi_rsp_scan_get_advanced_configuration_s {
  uint16_t result;
  int32_t trigger_level;
  uint32_t trigger_level_change;
  uint16_t active_channel_time;
  uint16_t passive_channel_time;
  uint8_t enable_instant_scan;
  uint8_t enable_multi_probe;
});

typedef struct app_wifi_rsp_scan_get_advanced_configuration_s app_wifi_rsp_scan_get_advanced_configuration_t;

PACKSTRUCT(struct app_wifi_rsp_scan_stop_s { uint16_t result; });

typedef struct app_wifi_rsp_scan_stop_s app_wifi_rsp_scan_stop_t;

PACKSTRUCT(struct app_wifi_rsp_ping_ipv4_s { uint16_t result; });

typedef struct app_wifi_rsp_ping_ipv4_s app_wifi_rsp_ping_ipv4_t;

PACKSTRUCT(struct app_wifi_rsp_client_set_roam_configuration_s { uint16_t result; });

typedef struct app_wifi_rsp_client_set_roam_configuration_s app_wifi_rsp_client_set_roam_configuration_t;

PACKSTRUCT(struct app_wifi_rsp_client_get_roam_configuration_s {
  uint16_t result;
  int32_t trigger_level;
  uint32_t trigger_level_change;
});

typedef struct app_wifi_rsp_client_get_roam_configuration_s app_wifi_rsp_client_get_roam_configuration_t;

PACKSTRUCT(struct app_wifi_packet {
  uint32_t header;
  union {
    uint8_t handle;
    app_wifi_rsp_error_t rsp_error;
    app_wifi_cmd_system_echo_t cmd_system_echo;
    app_wifi_cmd_net_intf_init_t cmd_net_intf_init;
    app_wifi_cmd_net_intf_deinit_t cmd_net_intf_deinit;
    app_wifi_cmd_net_intf_up_t cmd_net_intf_up;
    app_wifi_cmd_net_intf_down_t cmd_net_intf_down;
    app_wifi_cmd_net_intf_set_device_config_t cmd_net_intf_set_device_config;
    app_wifi_cmd_net_intf_get_init_status_t cmd_net_intf_get_init_status;
    app_wifi_cmd_net_intf_get_network_status_t cmd_net_intf_get_network_status;
    app_wifi_cmd_net_intf_set_si91x_boot_config_t cmd_net_intf_set_si91x_boot_config;
    app_wifi_cmd_net_intf_load_default_device_config_t cmd_net_intf_load_default_device_config;
    app_wifi_cmd_net_profile_set_t cmd_net_profile_set;
    app_wifi_cmd_net_profile_delete_t cmd_net_profile_delete;
    app_wifi_cmd_net_profile_set_ap_ssid_t cmd_net_profile_set_ap_ssid;
    app_wifi_cmd_net_profile_set_ap_config_t cmd_net_profile_set_ap_config;
    app_wifi_cmd_net_profile_set_ap_channel_t cmd_net_profile_set_ap_channel;
    app_wifi_cmd_net_profile_set_ap_ip_config_t cmd_net_profile_set_ap_ip_config;
    app_wifi_cmd_net_profile_set_ap_ipv4_config_t cmd_net_profile_set_ap_ipv4_config;
    app_wifi_cmd_net_profile_set_ap_ipv6_config_t cmd_net_profile_set_ap_ipv6_config;
    app_wifi_cmd_net_profile_set_client_ssid_t cmd_net_profile_set_client_ssid;
    app_wifi_cmd_net_profile_set_client_config_t cmd_net_profile_set_client_config;
    app_wifi_cmd_net_profile_set_client_channel_t cmd_net_profile_set_client_channel;
    app_wifi_cmd_ap_disconnect_client_t cmd_ap_disconnect_client;
    app_wifi_cmd_ap_get_client_info_t cmd_ap_get_client_info;
    app_wifi_cmd_net_cred_set_t cmd_net_cred_set;
    app_wifi_cmd_net_cred_get_t cmd_net_cred_get;
    app_wifi_cmd_net_cred_delete_t cmd_net_cred_delete;
    app_wifi_cmd_scan_start_t cmd_scan_start;
    app_wifi_cmd_scan_start_with_ssid_t cmd_scan_start_with_ssid;
    app_wifi_cmd_scan_set_advanced_configuration_t cmd_scan_set_advanced_configuration;
    app_wifi_cmd_scan_stop_t cmd_scan_stop;
    app_wifi_cmd_ping_ipv4_t cmd_ping_ipv4;
    app_wifi_cmd_client_set_roam_configuration_t cmd_client_set_roam_configuration;
    app_wifi_cmd_client_get_roam_configuration_t cmd_client_get_roam_configuration;
    app_wifi_rsp_system_hello_t rsp_system_hello;
    app_wifi_rsp_system_echo_t rsp_system_echo;
    app_wifi_rsp_net_intf_init_t rsp_net_intf_init;
    app_wifi_rsp_net_intf_deinit_t rsp_net_intf_deinit;
    app_wifi_rsp_net_intf_up_t rsp_net_intf_up;
    app_wifi_rsp_net_intf_down_t rsp_net_intf_down;
    app_wifi_rsp_net_intf_set_device_config_t rsp_net_intf_set_device_config;
    app_wifi_rsp_net_intf_get_init_status_t rsp_net_intf_get_init_status;
    app_wifi_rsp_net_intf_get_network_status_t rsp_net_intf_get_network_status;
    app_wifi_rsp_net_intf_get_device_config_t rsp_net_intf_get_device_config;
    app_wifi_rsp_net_intf_set_si91x_boot_config_t rsp_net_intf_set_si91x_boot_config;
    app_wifi_rsp_net_intf_get_si91x_boot_config_t rsp_net_intf_get_si91x_boot_config;
    app_wifi_rsp_net_intf_load_default_device_config_t rsp_net_intf_load_default_device_config;
    app_wifi_rsp_net_profile_set_t rsp_net_profile_set;
    app_wifi_rsp_net_profile_delete_t rsp_net_profile_delete;
    app_wifi_rsp_net_profile_set_ap_ssid_t rsp_net_profile_set_ap_ssid;
    app_wifi_rsp_net_profile_set_ap_config_t rsp_net_profile_set_ap_config;
    app_wifi_rsp_net_profile_set_ap_channel_t rsp_net_profile_set_ap_channel;
    app_wifi_rsp_net_profile_set_ap_ip_config_t rsp_net_profile_set_ap_ip_config;
    app_wifi_rsp_net_profile_set_ap_ipv4_config_t rsp_net_profile_set_ap_ipv4_config;
    app_wifi_rsp_net_profile_set_ap_ipv6_config_t rsp_net_profile_set_ap_ipv6_config;
    app_wifi_rsp_net_profile_get_ap_ssid_t rsp_net_profile_get_ap_ssid;
    app_wifi_rsp_net_profile_get_ap_config_t rsp_net_profile_get_ap_config;
    app_wifi_rsp_net_profile_get_ap_channel_t rsp_net_profile_get_ap_channel;
    app_wifi_rsp_net_profile_get_ap_ip_config_t rsp_net_profile_get_ap_ip_config;
    app_wifi_rsp_net_profile_get_ap_ipv4_config_t rsp_net_profile_get_ap_ipv4_config;
    app_wifi_rsp_net_profile_get_ap_ipv6_config_t rsp_net_profile_get_ap_ipv6_config;
    app_wifi_rsp_net_profile_set_client_ssid_t rsp_net_profile_set_client_ssid;
    app_wifi_rsp_net_profile_set_client_config_t rsp_net_profile_set_client_config;
    app_wifi_rsp_net_profile_set_client_channel_t rsp_net_profile_set_client_channel;
    app_wifi_rsp_net_profile_get_client_ssid_t rsp_net_profile_get_client_ssid;
    app_wifi_rsp_net_profile_get_client_config_t rsp_net_profile_get_client_config;
    app_wifi_rsp_net_profile_get_client_channel_t rsp_net_profile_get_client_channel;
    app_wifi_rsp_ap_disconnect_client_t rsp_ap_disconnect_client;
    app_wifi_rsp_ap_get_client_info_t rsp_ap_get_client_info;
    app_wifi_rsp_net_cred_set_t rsp_net_cred_set;
    app_wifi_rsp_net_cred_get_t rsp_net_cred_get;
    app_wifi_rsp_net_cred_delete_t rsp_net_cred_delete;
    app_wifi_rsp_common_get_firmware_version_t rsp_common_get_firmware_version;
    app_wifi_rsp_scan_start_t rsp_scan_start;
    app_wifi_rsp_scan_start_with_ssid_t rsp_scan_start_with_ssid;
    app_wifi_rsp_scan_set_advanced_configuration_t rsp_scan_set_advanced_configuration;
    app_wifi_rsp_scan_get_advanced_configuration_t rsp_scan_get_advanced_configuration;
    app_wifi_rsp_scan_stop_t rsp_scan_stop;
    app_wifi_rsp_ping_ipv4_t rsp_ping_ipv4;
    app_wifi_rsp_client_set_roam_configuration_t rsp_client_set_roam_configuration;
    app_wifi_rsp_client_get_roam_configuration_t rsp_client_get_roam_configuration;
    app_wifi_evt_system_boot_t evt_system_boot;
    app_wifi_evt_system_error_t evt_system_error;
    app_wifi_evt_net_intf_network_up_completed_t evt_net_intf_network_up_completed;
    app_wifi_evt_ap_client_connected_event_t evt_ap_client_connected_event;
    app_wifi_evt_ap_client_disconnected_event_t evt_ap_client_disconnected_event;
    app_wifi_evt_ap_get_client_info_event_t evt_ap_get_client_info_event;
    app_wifi_evt_scan_results_event_t evt_scan_results_event;
    app_wifi_evt_scan_error_event_t evt_scan_error_event;
    app_wifi_evt_scan_complete_event_t evt_scan_complete_event;
    app_wifi_evt_ping_ipv4_response_event_t evt_ping_ipv4_response_event;
    app_wifi_evt_client_join_event_t evt_client_join_event;
    app_wifi_evt_client_stats_response_event_t evt_client_stats_response_event;
    uint8_t payload[SL_BGAPI_MAX_PAYLOAD_SIZE];
  } data;
});

#ifdef __cplusplus
}
#endif

#endif