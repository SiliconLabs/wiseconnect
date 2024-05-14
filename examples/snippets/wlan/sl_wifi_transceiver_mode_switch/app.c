/***************************************************************************/ /**
 * @file 
 * @brief Wi-Fi Transceiver and Concurrent dual IP mode switch 
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_net.h"
#include "cmsis_os2.h"
#include "errno.h"
#include "sl_wifi_device.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_net_wifi_types.h"
#include "socket.h"
#include "sl_net_wifi_types.h"
#include "sl_net.h"
#include "sl_si91x_socket_support.h"
#include "sl_si91x_socket_constants.h"
#include <string.h>
#include "sl_wifi_callback_framework.h"
#include "sl_net_default_values.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_utility.h"
#include "sl_net_si91x.h"

/******************************************************
 *                      Macros
 ******************************************************/
#define ITER_COUNT 2
#define AP_VAP     1

/******************************************************
 *               Variable Definitions
 ******************************************************/

uint8_t address_buffer[SL_IPV6_ADDRESS_LENGTH];
uint8_t ap_vap = AP_VAP;

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define DEFAULT_WIFI_MODULE_IPV4_ADDRESS 0x0A0AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define DEFAULT_WIFI_SN_MASK_ADDRESS 0x00FFFFFF

//! IP address of Gateway
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define DEFAULT_WIFI_GATEWAY4_ADDRESS 0x0A0AA8C0

//! IPv6 address of the module
#define DEFAULT_WIFI_MODULE_IP6_ADDRESS "2001:db8:0:1::121"

//! IP address of Gateway
#define DEFAULT_WIFI_GATEWAY6_ADDRESS "2001:db8:0:1::121"

static const sl_net_wifi_client_profile_t wifi_client_profile_4 = {
    .config = {
        .ssid.value = DEFAULT_WIFI_CLIENT_PROFILE_SSID,
        .ssid.length = sizeof(DEFAULT_WIFI_CLIENT_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .bssid = {{0}},
        .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
        .security = DEFAULT_WIFI_CLIENT_SECURITY_TYPE,
        .encryption = DEFAULT_WIFI_CLIENT_ENCRYPTION_TYPE,
        .client_options = 0,
        .credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
    },
    .ip = {
        .mode = SL_IP_MANAGEMENT_DHCP,
        .type = SL_IPV4,
        .host_name = NULL,
        .ip = {{{0}}},

    }
};

static const sl_net_wifi_client_profile_t wifi_client_profile_6 = {
    .config = {
        .ssid.value = DEFAULT_WIFI_CLIENT_PROFILE_SSID,
        .ssid.length = sizeof(DEFAULT_WIFI_CLIENT_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .bssid = {{0}},
        .bss_type = SL_WIFI_BSS_TYPE_INFRASTRUCTURE,
        .security = DEFAULT_WIFI_CLIENT_SECURITY_TYPE,
        .encryption = DEFAULT_WIFI_CLIENT_ENCRYPTION_TYPE,
        .client_options = 0,
        .credential_id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
    },
    .ip = {
        .mode = SL_IP_MANAGEMENT_DHCP,
        .type = SL_IPV6,
        .host_name = NULL,
        .ip = {{{0}}},

    }
};

static sl_net_wifi_ap_profile_t wifi_ap_profile_4 = {
    .config = {
        .ssid.value = DEFAULT_WIFI_AP_PROFILE_SSID,
        .ssid.length = sizeof(DEFAULT_WIFI_AP_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .security = SL_WIFI_WPA2,
        .encryption = SL_WIFI_CCMP_ENCRYPTION,
        .rate_protocol = SL_WIFI_RATE_PROTOCOL_AUTO,
        .options = 0,
        .credential_id = SL_NET_USER_CREDENTIAL_ID,
        .keepalive_type = SL_SI91X_AP_NULL_BASED_KEEP_ALIVE,
        .beacon_interval = 100,
        .client_idle_timeout = 0xFF,
        .dtim_beacon_count = 3,
        .maximum_clients = 3,
    },
    .ip = {
        .mode = SL_IP_MANAGEMENT_STATIC_IP,
        .type = SL_IPV4,
        .host_name = NULL,
        .ip = {
            .v4.ip_address.value = DEFAULT_WIFI_MODULE_IPV4_ADDRESS,
        .v4.gateway.value = DEFAULT_WIFI_GATEWAY4_ADDRESS,
        .v4.netmask.value = DEFAULT_WIFI_SN_MASK_ADDRESS
        },
    }
};

static sl_net_wifi_ap_profile_t wifi_ap_profile_6 = {
    .config = {
        .ssid.value = DEFAULT_WIFI_AP_PROFILE_SSID,
        .ssid.length = sizeof(DEFAULT_WIFI_AP_PROFILE_SSID)-1,
        .channel.channel = SL_WIFI_AUTO_CHANNEL,
        .channel.band = SL_WIFI_AUTO_BAND,
        .channel.bandwidth = SL_WIFI_AUTO_BANDWIDTH,
        .security = SL_WIFI_WPA2,
        .encryption = SL_WIFI_CCMP_ENCRYPTION,
        .rate_protocol = SL_WIFI_RATE_PROTOCOL_AUTO,
        .options = 0,
        .credential_id = SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
        .keepalive_type = SL_SI91X_AP_NULL_BASED_KEEP_ALIVE,
        .beacon_interval = 100,
        .client_idle_timeout = 0xFF,
        .dtim_beacon_count = 3,
        .maximum_clients = 3,
    },
    .ip = {
        .mode = SL_IP_MANAGEMENT_STATIC_IP,
        .type = SL_IPV6,
        .host_name = NULL,
        .ip = {{{0}}},
    }
};

static const sl_wifi_device_configuration_t device_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = JP,
  .boot_config = { .oper_mode              = SL_SI91X_TRANSCEIVER_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = SL_SI91X_FEAT_SECURITY_OPEN,
                   .tcp_ip_feature_bit_map = SL_SI91X_TCP_IP_FEAT_BYPASS,
                   .custom_feature_bit_map = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_XTAL_CLK | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};

static sl_wifi_transceiver_configuration_t transceiver_profile = {
  .channel.chan_info = { .channel = SL_CHANNEL_NO, .band = SL_WIFI_BAND_2_4GHZ, .bandwidth = SL_WIFI_BANDWIDTH_20MHz },
  .channel.tx_power  = SL_TX_POWER,
  .parameters        = { .set              = 1,
                         .retransmit_count = DEFAULT_RETRANSMIT_COUNT,
                         .cw_params[0]     = { .cwmin = DEFAULT_QOS_BE_CWMIN,
                                               .cwmax = DEFAULT_QOS_BE_CWMAX,
                                               .aifsn = DEFAULT_QOS_BE_AIFSN },
                         .cw_params[1]     = { .cwmin = DEFAULT_QOS_BK_CWMIN,
                                               .cwmax = DEFAULT_QOS_BK_CWMAX,
                                               .aifsn = DEFAULT_QOS_BK_AIFSN },
                         .cw_params[2]     = { .cwmin = DEFAULT_QOS_VI_CWMIN,
                                               .cwmax = DEFAULT_QOS_VI_CWMAX,
                                               .aifsn = DEFAULT_QOS_VI_AIFSN },
                         .cw_params[3]     = { .cwmin = DEFAULT_QOS_VO_CWMIN,
                                               .cwmax = DEFAULT_QOS_VO_CWMAX,
                                               .aifsn = DEFAULT_QOS_VO_AIFSN } },
};

/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
static sl_status_t sl_dual_ip_client_up(void);
static sl_status_t sl_dual_ip_ap_up(void);
static sl_status_t sl_wifi_transceiver_initialize(void);
static void perform_transceiver_tx_test(void);
static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);
static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg);

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status = SL_STATUS_OK;

  for (int iteration = 0; iteration <= ITER_COUNT; ++iteration) {

    // Bring up DUT in dual IP client mode
    status = sl_dual_ip_client_up();
    if (status != SL_STATUS_OK) {
      return;
    }

    // Bring up DUT in dual IP AP mode (concurrent)
    status = sl_dual_ip_ap_up();
    if (status != SL_STATUS_OK) {
      return;
    }

    // De-init client and AP interfaces
    status = sl_net_down(SL_NET_WIFI_AP_INTERFACE);
    if (status != SL_STATUS_OK) {
      printf("\r\nFailed to bring Wi-Fi AP interface down: 0x%lX\r\n", status);
    } else {
      printf("\r\nWi-Fi AP interface down\r\n");
    }

    status = sl_net_down(SL_NET_WIFI_CLIENT_INTERFACE);
    if (status != SL_STATUS_OK) {
      printf("\r\nFailed to bring Wi-Fi Client interface down: 0x%lX\r\n", status);
    } else {
      printf("\r\nWi-Fi client interface down\r\n");
    }

    status = sl_net_deinit(SL_NET_WIFI_CLIENT_INTERFACE);
    if (status != SL_STATUS_OK) {
      printf("\r\nFailed to deinit Wi-Fi concurrent interface up: 0x%lX\r\n", status);
    } else {
      printf("\r\nWi-Fi concurrent interface deinit successful\r\n");
    }

    // Switch to transceiver mode
    status = sl_wifi_transceiver_initialize();
    if (status != SL_STATUS_OK) {
      return;
    }

    // Send TX data to configured peer
    perform_transceiver_tx_test();

    osDelay(10);

    // De-init transceiver mode
    status = sl_wifi_deinit();
    if (status != SL_STATUS_OK) {
      printf("\r\nFailed to deinit Wi-Fi Transceiver interface up: 0x%lX\r\n", status);
    } else {
      printf("\r\nWi-Fi Transceiver interface deinit successful\r\n");
    }
  }
}

static sl_status_t sl_dual_ip_client_up(void)
{
  sl_status_t status                   = SL_STATUS_OK;
  sl_net_wifi_client_profile_t profile = { 0 };
  sl_ip_address_t ip_address           = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_default_concurrent_v6_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi client interface: 0x%lx\r \n", status);
    return status;
  }
  printf("\r\nWi-Fi Client interface init\r\n");

  sl_wifi_set_callback(SL_WIFI_CLIENT_CONNECTED_EVENTS, ap_connected_event_handler, NULL);
  sl_wifi_set_callback(SL_WIFI_CLIENT_DISCONNECTED_EVENTS, ap_disconnected_event_handler, NULL);

  //  Keeping the station ipv4 record in profile_id_0
  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &wifi_client_profile_4);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set client profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nWi-Fi set client profile v4 success\r\n");

  //  Keeping the station ipv6 record in profile_id_1
  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_client_profile_6);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set client profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nWi-Fi set client profile v6 success\r\n");

  status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                 default_wifi_client_credential.type,
                                 &default_wifi_client_credential.data,
                                 default_wifi_client_credential.data_length);
  if (status != SL_STATUS_OK) {
    printf("Failed to set credentials: 0x%lx\r\n", status);
    return status;
  }
  printf("\nWi-Fi set credential success\n");

  //  Station ipv6 config using profile_id_1
  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_1);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nIPv6 address configuration is done");

  //  Station ipv4 config using profile_id_0
  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return status;
  }

  status = sl_si91x_configure_ip_address(&profile.ip, SL_SI91X_WIFI_CLIENT_VAP_ID);
  if (status != SL_STATUS_OK) {
    printf("IPv4 address configuration is failed : 0x%lx\r\n", status);
    while (1)
      ;

    return status;
  }
  printf("\r\nIPv4 address configuration is done");

  status = sl_net_set_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set client profile: 0x%lx\r\n", status);
    return status;
  }

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_1, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get client_v6 profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nSTA IPv6 : ");

  if (profile.ip.type == SL_IPV6) {
    sl_ip_address_t link_local_address = { 0 };
    link_local_address.ip.v6           = profile.ip.ip.v6.link_local_address;
    link_local_address.type            = SL_IPV6;
    printf("\nLink Local Address: ");
    print_sl_ip_address(&link_local_address);

    sl_ip_address_t global_address = { 0 };
    global_address.ip.v6           = profile.ip.ip.v6.global_address;
    global_address.type            = SL_IPV6;
    printf("Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway = { 0 };
    gateway.ip.v6           = profile.ip.ip.v6.gateway;
    gateway.type            = SL_IPV6;
    printf("Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

  status = sl_net_get_profile(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_PROFILE_ID_0, &profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to get client_v4 profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nClient profile v4 is fetched successfully : ");

  if (profile.ip.type == SL_IPV4) {
    ip_address.type = SL_IPV4;
    memcpy(&ip_address.ip.v4.bytes, &profile.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    printf("\r\nClient IPv4 : ");
    print_sl_ip_address(&ip_address);
  }

  return SL_STATUS_OK;
}

static sl_status_t sl_dual_ip_ap_up(void)
{
  sl_status_t status;
  sl_wifi_channel_t client_channel               = { 0 };
  sl_ip_address_t ip_address                     = { 0 };
  unsigned char hex_addr[SL_IPV6_ADDRESS_LENGTH] = { 0 };
  uint32_t ip_addr[4]                            = { 0 };
  uint32_t gateway[4]                            = { 0 };
  int return_value;

  status = sl_net_init(SL_NET_WIFI_AP_INTERFACE, &sl_wifi_default_concurrent_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to start Wi-Fi AP interface: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nWi-Fi AP interface init\r\n");

  return_value = sl_inet_pton6(DEFAULT_WIFI_MODULE_IP6_ADDRESS,
                               DEFAULT_WIFI_MODULE_IP6_ADDRESS + strlen(DEFAULT_WIFI_MODULE_IP6_ADDRESS),
                               hex_addr,
                               (unsigned int *)ip_addr);
  if (return_value != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return status;
  }
  return_value = sl_inet_pton6(DEFAULT_WIFI_GATEWAY6_ADDRESS,
                               DEFAULT_WIFI_GATEWAY6_ADDRESS + strlen(DEFAULT_WIFI_GATEWAY6_ADDRESS),
                               hex_addr,
                               (unsigned int *)gateway);
  if (return_value != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    return status;
  }

  memcpy(wifi_ap_profile_6.ip.ip.v6.global_address.value, ip_addr, sizeof(sl_ipv6_address_t));
  memcpy(wifi_ap_profile_6.ip.ip.v6.gateway.value, gateway, sizeof(sl_ipv6_address_t));

  status = sl_wifi_get_channel(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &client_channel);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to get client channel: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nSuccess to get client channel\r\n");

  wifi_ap_profile_4.config.channel.channel = client_channel.channel;
  wifi_ap_profile_6.config.channel.channel = client_channel.channel;

  status = sl_net_set_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_0, &wifi_ap_profile_4);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set AP profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nSuccess to set AP v4 profile \r\n");

  status = sl_net_set_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_ap_profile_6);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to set AP profile: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nSuccess to set AP v6 profile \r\n");

  status = sl_net_set_credential(SL_NET_USER_CREDENTIAL_ID,
                                 default_wifi_ap_credential.type,
                                 &default_wifi_ap_credential.data,
                                 default_wifi_ap_credential.data_length);
  if (status != SL_STATUS_OK) {
    printf("Failed to set credentials: 0x%lx\r\n", status);
    return status;
  }
  printf("\nWi-Fi set credential success\n");

  status = sl_si91x_configure_ip_address(&wifi_ap_profile_4.ip, SL_SI91X_WIFI_AP_VAP_ID);
  status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_0, &wifi_ap_profile_4);

  if (wifi_ap_profile_4.ip.type == SL_IPV4) {
    ip_address.type = SL_IPV4;
    printf("\r\nAP IPv4 : ");
    memcpy(&ip_address.ip.v4.bytes, &wifi_ap_profile_4.ip.ip.v4.ip_address.bytes, sizeof(sl_ipv4_address_t));
    print_sl_ip_address(&ip_address);
  }

  status = sl_net_get_profile(SL_NET_WIFI_AP_INTERFACE, SL_NET_PROFILE_ID_1, &wifi_ap_profile_6);

  status = sl_si91x_configure_ip_address(&wifi_ap_profile_6.ip, SL_SI91X_WIFI_AP_VAP_ID);

  printf("\r\nAP IPv6 : ");
  if (wifi_ap_profile_6.ip.type == SL_IPV6) {
    sl_ip_address_t link_local_address = { 0 };
    link_local_address.ip.v6           = wifi_ap_profile_6.ip.ip.v6.link_local_address;
    link_local_address.type            = SL_IPV6;
    printf("\nLink Local Address: ");
    print_sl_ip_address(&link_local_address);
    sl_ip_address_t global_address = { 0 };
    global_address.ip.v6           = wifi_ap_profile_6.ip.ip.v6.global_address;
    global_address.type            = SL_IPV6;
    printf("Global Address: ");
    print_sl_ip_address(&global_address);

    sl_ip_address_t gateway = { 0 };
    gateway.ip.v6           = wifi_ap_profile_6.ip.ip.v6.gateway;
    gateway.type            = SL_IPV6;
    printf("Gateway Address: ");
    print_sl_ip_address(&gateway);
  }

  status = sl_wifi_start_ap(SL_WIFI_AP_2_4GHZ_INTERFACE, &wifi_ap_profile_4.config);
  if (status != SL_STATUS_OK) {
    printf("Ap start fail: 0x%lx\r\n", status);
    return status;
  }
  printf("\r\nAP started\r\n");

  printf("\r\nWait 10s for third-party station to connect to AP\r\n");
  osDelay(10000);

  return SL_STATUS_OK;
}

static sl_status_t sl_wifi_transceiver_initialize(void)
{
  sl_status_t status = SL_STATUS_OK;

  status = sl_wifi_init(&device_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("\r\nTransceiver initialization failed: 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nTransceiver initialization successful\r\n");

  status = sl_wifi_transceiver_up(SL_WIFI_TRANSCEIVER_INTERFACE, &transceiver_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nFailed to bring up Transceiver interface: 0x%lX\r\n", status);
    return status;
  }
  printf("\r\nTransceiver interface up\r\n");

  return status;
}

static sl_status_t get_sample_packet(sl_wifi_transceiver_tx_data_control_t *control,
                                     uint8_t *payload,
                                     uint16_t payload_length)
{
  uint32_t sample_data  = 0;
  static uint16_t token = 0;
  uint32_t pos          = 0;
  uint8_t llc_header[8] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00 };
  uint8_t addr1_ra[6]   = { 0x00, 0x23, 0xa7, 0x20, 0x21, 0x23 }; //RA
  uint8_t addr3_da[6]   = { 0x00, 0xDA, 0x11, 0x11, 0x11, 0x11 }; //DA
  uint8_t addr4_sa[6]   = { 0x00, 0x5A, 0x22, 0x22, 0x22, 0x22 }; //SA

  if (payload_length < 8) {
    return SL_STATUS_FAIL;
  }

  memset(control, 0, sizeof(sl_wifi_transceiver_tx_data_control_t));

#define TX_FRAME_TYPE_4ADDR BIT(0)
#define TX_FRAME_TYPE_QOS   BIT(1)
#define TX_FRAME_FIXED_RATE BIT(2)

  control->ctrl_flags = (TX_FRAME_TYPE_QOS | TX_FRAME_TYPE_4ADDR | TX_FRAME_FIXED_RATE);

  if (control->ctrl_flags & TX_FRAME_TYPE_4ADDR) {
    memcpy(control->addr4, addr4_sa, 6);
  }

  if (control->ctrl_flags & TX_FRAME_TYPE_QOS) {
    control->priority = 2;
  }

  if (control->ctrl_flags & TX_FRAME_FIXED_RATE) {
    control->rate = SL_WIFI_DATA_RATE_36;
  }

  control->token = token++;
  if (token > 0xfff) {
    token = 0;
  }

  memcpy(control->addr1, addr1_ra, 6);
  memcpy(control->addr2, &transceiver_profile.dut_mac.octet, 6);
  memcpy(control->addr3, addr3_da, 6);

  memcpy(payload, llc_header, 8);
  pos += 8;

  /* Fill payload */
  while (pos < payload_length) {
    payload[pos++] = ++sample_data;
  }

  return SL_STATUS_OK;
}

static void perform_transceiver_tx_test(void)
{
  sl_status_t status;
  for (int packet_count = 2; packet_count != 0; packet_count--) {
    sl_wifi_transceiver_tx_data_control_t transceiver_control;
    uint8_t payload[50] = { 0 };

    status = get_sample_packet(&transceiver_control, payload, sizeof(payload));
    if (status != SL_STATUS_OK) {
      printf("Packet format error\r\n");
      return;
    }

    status =
      sl_wifi_send_transceiver_data(SL_WIFI_TRANSCEIVER_INTERFACE, &transceiver_control, payload, sizeof(payload));
    if (status != SL_STATUS_OK) {
      printf("Sending data packet failed with error: 0x%lX\r\n", status);
      return;
    }
  }
}

static sl_status_t ap_connected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client connected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  return SL_STATUS_OK;
}

static sl_status_t ap_disconnected_event_handler(sl_wifi_event_t event, void *data, uint32_t data_length, void *arg)
{
  UNUSED_PARAMETER(data_length);
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(event);

  printf("Remote Client disconnected: ");
  print_mac_address((sl_mac_address_t *)data);
  printf("\n");

  return SL_STATUS_OK;
}
