/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_wifi.h"
#include "sl_si91x_host_interface.h"
#include "sl_wifi_types.h"
#include "sl_si91x_status.h"
#include "sl_si91x_types.h"
#include "sl_si91x_protocol_types.h"
#include "sl_wifi_device.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_driver.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "sl_si91x_core_utilities.h"
#include "em_core.h"
#ifdef SLI_SI91X_MCU_INTERFACE
#include "sli_siwx917_soc.h"
#include "rsi_rom_clks.h"
#include "rsi_m4.h"
#include "rsi_wisemcu_hardware_setup.h"
#endif
#ifdef SLI_SI91X_ENABLE_BLE
#include "rsi_common_apis.h"
#endif

#include "sl_wifi_region_db_config.h"

#ifdef SLI_SI91X_SOCKETS
#include "sl_si91x_socket_utility.h"
#include "sl_si91x_socket_callback_framework.h"
#endif

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
#include "rsi_m4.h"
#define SIDE_BAND_DONE (1 << 2) //! had to be redefined as this macro is not in .h
rsi_m4ta_desc_t crypto_desc[2];
extern osEventFlagsId_t ta_events;
extern osMutexId_t side_band_crypto_mutex;
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#define htole16(x) (x)
#define htole32(x) (x)

#ifndef SL_WIFI_SET_MAC_COMMAND_TIME_OUT
#define SL_WIFI_SET_MAC_COMMAND_TIME_OUT 30100 // Retrieved from SAPI 1.0
#endif

#define SLI_SI91X_GET_TCP_IP_TOTAL_SELECTS_BITS(x) ((x & 0x0000F000) >> 12)

#define SL_SI91X_INVALID_MODE 0xFFFF

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
#define SL_HOST_DESC_LEN  16
#define SL_CRYPTO_PKT_LEN 128
#endif

// Enterprise configuration command parameters
/*=======================================================================*/

// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP

#define SL_EAP_TLS_METHOD  "TLS"
#define SL_EAP_TTLS_METHOD "TTLS"

// This parameter is used to configure the module in Enterprise security mode
#define SL_EAP_INNER_METHOD "\"auth=MSCHAPV2\""

// Private Key Password is required for encrypted private key, format is like "\"12345678\""
#define SL_DEFAULT_PRIVATE_KEY_PASSWORD ""

/*========================================================================*/
// 11ax params
/*========================================================================*/
#define GUARD_INTERVAL 3

#ifdef SLI_SI91X_MCU_INTERFACE
#define TX_PKT_TRANSFER_DONE_INTERRUPT BIT(2)
#define RX_PKT_TRANSFER_DONE_INTERRUPT BIT(1)

// Function declarations related to M4 interface
sl_status_t sli_si91x_submit_rx_pkt(void);
static sl_status_t sl_si91x_soft_reset(void);
void sli_siwx917_update_system_core_clock(void);
void sli_m4_ta_interrupt_init(void);
#endif

// Structure to hold packet information and payload
typedef struct {
  uint16_t packet_id;
  sli_si91x_queue_packet_t *packet;
  void *payload;
} sl_si91x_driver_context_t;

static sl_si91x_timeout_t timeout_glbl = { .auth_assoc_timeout_value       = SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT,
                                           .active_chan_scan_timeout_value = SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME,
                                           .keep_alive_timeout_value       = SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT,
                                           .passive_scan_timeout_value = SL_WIFI_DEFAULT_PASSIVE_CHANNEL_SCAN_TIME };

sl_status_t sl_si91x_driver_send_command_packet(uint32_t command,
                                                sl_si91x_command_type_t command_type,
                                                sl_wifi_buffer_t *buffer,
                                                sl_si91x_wait_period_t wait_period,
                                                void *sdk_context,
                                                sl_wifi_buffer_t **data_buffer);
static sl_status_t sl_si91x_driver_send_data_packet(sl_wifi_buffer_t *buffer, uint32_t wait_time);
sl_status_t sl_si91x_driver_raw_send_command(uint8_t command,
                                             const void *data,
                                             uint32_t data_length,
                                             uint32_t wait_time);
sl_status_t sl_si91x_allocate_data_buffer(sl_wifi_buffer_t **host_buffer,
                                          void **buffer,
                                          uint32_t data_size,
                                          uint32_t wait_duration_ms);
sl_status_t sl_si91x_driver_init_wifi_radio(const sl_wifi_device_configuration_t *config);
sl_status_t sli_verify_device_boot(uint32_t *rom_version);
sl_status_t sl_si91x_enable_radio(void);
sl_status_t sli_wifi_select_option(const uint8_t configuration);
sl_status_t si91x_bootup_firmware(const uint8_t select_option);
sl_status_t sl_si91x_host_power_cycle(void);

// This variable stores the frame status of response packet in case of API executed being failed.
// Note: This will not store the error values of asynchronous events.
sl_wifi_event_handler_t si91x_event_handler = NULL;

// Global variables for device and driver management
sl_wifi_interface_t default_interface;
bool device_initialized                           = false;
bool interface_is_up[SL_WIFI_MAX_INTERFACE_INDEX] = { false, false, false, false, false };
bool bg_enabled                                   = false;
uint32_t frontend_switch_control                  = 0;
static uint32_t feature_bit_map                   = 0;
//static uint16_t queue_packet_id[SI91X_CMD_MAX]    = { 0 };
static uint8_t ap_join_feature_bitmap         = SL_SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID;
static uint8_t client_join_feature_bitmap     = SL_SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID;
static uint32_t client_listen_interval        = 1000;
static sl_si91x_efuse_data_t si91x_efuse_data = { 0 };
//! Currently, initialized_opermode is used only to handle concurrent mode using sl_net_init()
static uint16_t initialized_opermode = SL_SI91X_INVALID_MODE;
extern sli_si91x_command_queue_t cmd_queues[SI91X_CMD_MAX];
extern sl_si91x_buffer_queue_t sli_tx_data_queue;

sli_si91x_performance_profile_t performance_profile;
extern osEventFlagsId_t si91x_events;
extern volatile uint32_t tx_command_queues_status;
extern volatile uint32_t tx_generic_socket_data_queues_status;

#ifdef SLI_SI91X_ENABLE_BLE
//! Memory length for driver
#define GLOBAL_BUFF_LEN 1500

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
#endif
// clang-format off
const sl_wifi_scan_configuration_t default_wifi_scan_configuration = { .type  = SL_WIFI_SCAN_TYPE_ACTIVE,
                                                                       .flags = 0,
                                                                       .periodic_scan_interval = 0,
                                                                       .channel_bitmap_2g4     = 0xFFFF,
                                                                       .channel_bitmap_5g      = { 0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF,
                                                                                              0xFFFFFFFF }, 
                                                                       .lp_mode = 0};

// clang-format on
const sl_wifi_buffer_configuration_t default_buffer_configuration = {
  .control_buffer_quota = 10,
  .tx_buffer_quota      = 10,
  .rx_buffer_quota      = 10,
#ifdef SPI_EXTENDED_TX_LEN_2K
  .block_size = 2300,
#else
  .block_size = 1616,
#endif
  .buffer_memory = NULL,
};

const sl_wifi_ap_configuration_t default_wifi_ap_configuration = {
  .ssid                = { .value = "SILICON_LABS_AP", .length = sizeof("SILICON_LABS_AP") - 1 },
  .security            = SL_WIFI_WPA2,
  .encryption          = SL_WIFI_CCMP_ENCRYPTION,
  .channel             = { .channel = 11 },
  .rate_protocol       = SL_WIFI_RATE_PROTOCOL_AUTO,
  .options             = 0,
  .credential_id       = 2, //SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID,
  .keepalive_type      = SL_SI91X_AP_NULL_BASED_KEEP_ALIVE,
  .beacon_interval     = 100,
  .client_idle_timeout = 120000,
  .dtim_beacon_count   = 4,
  .maximum_clients     = 4
};

// clang-format off
static uint8_t firmware_queue_id[SI91X_CMD_MAX]   = { [SI91X_COMMON_CMD]  = RSI_WLAN_MGMT_Q,
                                                    [SI91X_WLAN_CMD]      = RSI_WLAN_MGMT_Q,
                                                    [SI91X_NETWORK_CMD]   = RSI_WLAN_MGMT_Q,
													[SI91X_SOCKET_CMD]    = RSI_WLAN_MGMT_Q,
                                                    [SI91X_BT_CMD]        = RSI_BT_Q };
//static uint32_t response_event_map[SI91X_CMD_MAX] = { [SI91X_COMMON_CMD]  = NCP_HOST_COMMON_RESPONSE_EVENT,
//                                                      [SI91X_WLAN_CMD]    = NCP_HOST_WLAN_RESPONSE_EVENT,
//                                                      [SI91X_NETWORK_CMD] = NCP_HOST_NETWORK_RESPONSE_EVENT,
//                                                      [SI91X_SOCKET_CMD]  = NCP_HOST_SOCKET_RESPONSE_EVENT,
//                                                      [SI91X_BT_CMD]      = NCP_HOST_BT_RESPONSE_EVENT };
// clang-format on
#ifdef SLI_SI91X_MCU_INTERFACE
extern sl_wifi_buffer_t *rx_pkt_buffer;
#endif

static bool si91x_packet_identification_function(const sl_wifi_buffer_t *buffer, const void *user_data)
{
  const uint8_t *packet_id = (const uint8_t *)user_data;

  // Check if the packet's packet ID matches the expected one
  return (*packet_id == buffer->id);
}

void sl_si91x_get_efuse_data(sl_si91x_efuse_data_t *efuse_data)
{
  memcpy(efuse_data, &si91x_efuse_data, sizeof(sl_si91x_efuse_data_t));
}

void sl_si91x_set_efuse_data(const sl_si91x_efuse_data_t *efuse_data)
{
  memcpy(&si91x_efuse_data, efuse_data, sizeof(sl_si91x_efuse_data_t));
}

sl_status_t sl_si91x_driver_init_wifi_radio(const sl_wifi_device_configuration_t *config)
{
  sl_status_t status;

// Set 11ax configuration with guard interval if SLI_SI91X_CONFIG_WIFI6_PARAMS is supported
#ifdef SLI_SI91X_CONFIG_WIFI6_PARAMS
  status = sl_wifi_set_11ax_config(GUARD_INTERVAL);
  VERIFY_STATUS_AND_RETURN(status);
#endif

  // Send WLAN request to set the operating band (2.4GHz or 5GHz)
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_BAND,
                                        SI91X_WLAN_CMD,
                                        &config->band,
                                        1,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  if (config->boot_config.oper_mode != SL_SI91X_ACCESS_POINT_MODE) {
    if (timeout_glbl.active_chan_scan_timeout_value != SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME) {
      status =
        sl_si91x_configure_timeout(SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT, timeout_glbl.active_chan_scan_timeout_value);
      VERIFY_STATUS_AND_RETURN(status);
    }

    if (timeout_glbl.auth_assoc_timeout_value != SL_WIFI_DEFAULT_AUTH_ASSOCIATION_TIMEOUT) {
      status =
        sl_si91x_configure_timeout(SL_SI91X_AUTHENTICATION_ASSOCIATION_TIMEOUT, timeout_glbl.auth_assoc_timeout_value);
      VERIFY_STATUS_AND_RETURN(status);
    }

    if (timeout_glbl.keep_alive_timeout_value != SL_WIFI_DEFAULT_KEEP_ALIVE_TIMEOUT) {
      status = sl_si91x_configure_timeout(SL_SI91X_KEEP_ALIVE_TIMEOUT, timeout_glbl.keep_alive_timeout_value);
      VERIFY_STATUS_AND_RETURN(status);
    }
    if (timeout_glbl.passive_scan_timeout_value != SL_WIFI_DEFAULT_PASSIVE_CHANNEL_SCAN_TIME) {
      status =
        sl_si91x_configure_timeout(SL_SI91X_CHANNEL_PASSIVE_SCAN_TIMEOUT, timeout_glbl.passive_scan_timeout_value);
      VERIFY_STATUS_AND_RETURN(status);
    }
  }

  // Initialize the WLAN subsystem
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

#ifndef SL_SI91X_ACX_MODULE
  if (IGNORE_REGION != config->region_code) {
    // Set the device's region based on configuration
    status = sl_si91x_set_device_region(config->boot_config.oper_mode, config->band, config->region_code);
    VERIFY_STATUS_AND_RETURN(status);
  }
#endif

  // Configure the RTS threshold for WLAN
  sl_si91x_config_request_t config_request = { .config_type = CONFIG_RTSTHRESHOLD, .value = RSI_RTS_THRESHOLD };
  status                                   = sl_si91x_driver_send_command(RSI_WLAN_REQ_CONFIG,
                                        SI91X_WLAN_CMD,
                                        &config_request,
                                        sizeof(config_request),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_driver_init(const sl_wifi_device_configuration_t *config, sl_wifi_event_handler_t event_handler)
{
  sl_status_t status;
  sl_si91x_host_init_configuration init_config = { 0 };
  const uint8_t select_option                  = config->boot_option;
  if (config->boot_config.coex_mode == SL_SI91X_WLAN_MODE) { // Check for not support mode
    return SL_STATUS_NOT_SUPPORTED;
  }

  // Determine the default interface based on operating mode (AP or Client)
  if (config->boot_config.oper_mode == SL_SI91X_ACCESS_POINT_MODE) {
    default_interface = SL_WIFI_AP_INTERFACE;
  } else {
    default_interface = SL_WIFI_CLIENT_INTERFACE;
  }

  // Configure the interface for 5GHz band if selected (currently not supported for Si91x)
  if (config->band == SL_SI91X_WIFI_BAND_5GHZ) {
    default_interface |= SL_WIFI_5GHZ_INTERFACE;
  } else {
    default_interface |= SL_WIFI_2_4GHZ_INTERFACE;
  }

  // Set the event handler for the SI91x wireless driver
  si91x_event_handler = event_handler;

  // Check if the device is already initialized
  if (device_initialized) {
    // If it's already initialized, ensure the requested operating mode is compatible
    if (initialized_opermode == SL_SI91X_CONCURRENT_MODE) {
      return (initialized_opermode == config->boot_config.oper_mode) ? SL_STATUS_OK : SL_STATUS_WIFI_INVALID_OPERMODE;
    }
    return SL_STATUS_ALREADY_INITIALIZED;
  }
  // Initialize BLE if BLE is enabled
#ifdef SLI_SI91X_ENABLE_BLE
  int32_t rsi_status = 0;
  rsi_status         = rsi_ble_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((rsi_status < 0) || (rsi_status > GLOBAL_BUFF_LEN)) {
    return SL_STATUS_FAIL;
  }
#endif

#ifndef SLI_SI91X_ENABLE_IPV6
  uint32_t *tcp_ip_feature_bit_map = (uint32_t *)&(config->boot_config.tcp_ip_feature_bit_map);

  // check if the module network stack is in IPV6 mode.
  if (*tcp_ip_feature_bit_map & (SI91X_IPV6_MODE)) {
    return SL_STATUS_NOT_AVAILABLE;
  }
#endif

  // Initialize the buffer manager
  status = sl_si91x_host_init_buffer_manager(&default_buffer_configuration);
  if (status != SL_STATUS_OK) {
    return status;
  }

  init_config.rx_irq      = sl_si91x_bus_rx_irq_handler;
  init_config.rx_done     = sl_si91x_bus_rx_done_handler;
  init_config.boot_option = config->boot_option;

  // Initialize the SI91x host
  status = sl_si91x_host_init(&init_config);
  VERIFY_STATUS_AND_RETURN(status);

  // Initialize the SI91x platform
  status = sl_si91x_platform_init();
  VERIFY_STATUS_AND_RETURN(status);

  // Power cycle the SI91x device
  status = sl_si91x_host_power_cycle();
  VERIFY_STATUS_AND_RETURN(status);

  // sl_si91x_bus_init() will be implemented for all available buses
  status = sl_si91x_bus_init();
  VERIFY_STATUS_AND_RETURN(status);

#ifdef SLI_SI91X_MCU_INTERFACE
  // firmware bootup is require only for the first time, no need to do it again if we call init after deinit
  static bool is_bootup_firmware_required = true;
  if (is_bootup_firmware_required) {
    status = si91x_bootup_firmware(select_option);
    VERIFY_STATUS_AND_RETURN(status);
    is_bootup_firmware_required = false;
  } else {
    // Initialize NWP interrupt and submit RX packets
    sli_m4_ta_interrupt_init();
    sli_si91x_submit_rx_pkt();
  }
#else
  status = si91x_bootup_firmware(select_option);
  VERIFY_STATUS_AND_RETURN(status);
#endif

  // Initialize task register index to save firmware status
  status = sli_fw_status_storage_index_init();
  VERIFY_STATUS_AND_RETURN(status);

#ifdef SL_SI91X_SPI_HIGH_SPEED_ENABLE
  // Enable high speed bus on the device and the host
  status = sl_si91x_bus_enable_high_speed();
  VERIFY_STATUS_AND_RETURN(status);
  sl_si91x_host_enable_high_speed_bus();
#endif
  sl_si91x_host_enable_bus_interrupt();

// Wait for card ready command response
#ifdef SLI_SI91X_MCU_INTERFACE
  // NWP would not send card ready command response, if we call init after deinit

  if (get_card_ready_required()) {
    uint32_t events = sli_si91x_wait_for_event(NCP_HOST_COMMON_RESPONSE_EVENT, 5000);
    if (!(events & NCP_HOST_COMMON_RESPONSE_EVENT)) {
      return SL_STATUS_CARD_READY_TIMEOUT;
    }
    set_card_ready_required(false);
  }
#else
  uint32_t events = sli_si91x_wait_for_event(NCP_HOST_COMMON_RESPONSE_EVENT, 3000);
  if (!(events & NCP_HOST_COMMON_RESPONSE_EVENT)) {
    return SL_STATUS_CARD_READY_TIMEOUT;
  }
#endif
  // Send WLAN request to set the operating mode and configuration
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_OPERMODE,
                                        SI91X_COMMON_CMD,
                                        &config->boot_config,
                                        sizeof(sl_si91x_boot_configuration_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
#ifdef SL_WDT_MANAGER_PRESENT
  sl_si91x_nwp_configuration_t nwp_config;

  // NWP WDT configuration
  memset(&nwp_config, 0, sizeof(sl_si91x_nwp_configuration_t));
  nwp_config.code                    = SL_SI91X_ENABLE_NWP_WDT_FROM_HOST;
  nwp_config.values.wdt_timer_val    = 0x20;
  nwp_config.values.wdt_enable_in_ps = 0;
  status                             = sl_si91x_set_nwp_config_request(nwp_config);
  VERIFY_STATUS_AND_RETURN(status);
#endif

  feature_bit_map = config->boot_config.feature_bit_map;

#ifdef SLI_SI91X_ENABLE_BLE
  if (config->boot_config.coex_mode == SL_SI91X_BLE_MODE || config->boot_config.coex_mode == SL_SI91X_WLAN_BLE_MODE) {
    // Wait for BT card ready
    rsi_bt_common_init();
  }
#endif

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DYNAMIC_POOL,
                                        SI91X_WLAN_CMD,
                                        &config->ta_pool,
                                        sizeof(sl_si91x_dynamic_pool),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  // Configure various wireless features
  sl_si91x_feature_frame_request feature_frame_request = { .pll_mode        = PLL_MODE,
                                                           .rf_type         = RF_TYPE,
                                                           .wireless_mode   = WIRELESS_MODE,
                                                           .enable_ppp      = ENABLE_PPP,
                                                           .afe_type        = AFE_TYPE,
                                                           .feature_enables = FEATURE_ENABLES };

  // Setting PLL mode to 1 in case of high clock frequency
  //pll_mode 1 is not supported in coex mode
  if ((!config->boot_config.coex_mode)
      && (config->boot_config.custom_feature_bit_map
          & (SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ | SL_SI91X_CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ))) {
    feature_frame_request.pll_mode = 1;
  } else {
    feature_frame_request.pll_mode = 0;
  }

  // For the transmit test mode we need to disable BIT 0, 4, 5. These bitmaps are only required in powersave.
  // To receive broadcast data packets in transceiver opermode, we need to enable BIT 1.
  if (config->boot_config.oper_mode == SL_SI91X_TRANSMIT_TEST_MODE) {
    feature_frame_request.feature_enables &= ~(FEATURE_ENABLES);
  } else if (config->boot_config.oper_mode == SL_SI91X_TRANSCEIVER_MODE) {
    feature_frame_request.feature_enables |= SI91X_FEAT_FRAME_PERMIT_UNDESTINED_PACKETS;
  } else {
    feature_frame_request.feature_enables = feature_frame_request.feature_enables;
  }

  // Dispatch a feature request frame to the SI91x driver
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_FEATURE_FRAME,
                                        SI91X_COMMON_CMD,
                                        &feature_frame_request,
                                        sizeof(feature_frame_request),
                                        SL_SI91X_WAIT_FOR(10000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  // if 16th bit of ext_tcp_ip_feature_bit_map is not set, then firmware auto closes the TCP socket on remote termination.
  save_tcp_auto_close_choice(
    (config->boot_config.ext_tcp_ip_feature_bit_map & SL_SI91X_EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE) == 0);

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
  sli_si91x_socket_init(SLI_SI91X_GET_TCP_IP_TOTAL_SELECTS_BITS(config->boot_config.ext_tcp_ip_feature_bit_map));
  VERIFY_STATUS_AND_RETURN(status);
#endif

  // Set the MAC address if provided in the configuration
  if (config->mac_address != NULL) {
    status = sl_wifi_set_mac_address(default_interface, config->mac_address);
    VERIFY_STATUS_AND_RETURN(status);
  }

  // Initialize the WiFi radio if the coexistence mode is not BLE
  if (config->boot_config.coex_mode != SL_SI91X_BLE_MODE) {
    status = sl_si91x_driver_init_wifi_radio(config);
    VERIFY_STATUS_AND_RETURN(status);
  }

  // Check and update the frontend switch control based on custom feature bit map
  if (config->boot_config.custom_feature_bit_map & SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID) {
    frontend_switch_control = (config->boot_config.ext_custom_feature_bit_map & (BIT(29) | (BIT(30))));
  }

#ifdef SLI_SI91X_MCU_INTERFACE
  // Program wireless GPIO front-end switch controls
  if (frontend_switch_control != 0) {
    sli_si91x_configure_wireless_frontend_controls(frontend_switch_control);
  }
#endif
  // Mark the device as initialized
  device_initialized   = true;
  initialized_opermode = config->boot_config.oper_mode;

  // Set interface status flags based on operating mode and band
  if ((config->boot_config.oper_mode == SL_SI91X_CLIENT_MODE)
      || (config->boot_config.oper_mode == SL_SI91X_ENTERPRISE_CLIENT_MODE)
      || (config->boot_config.oper_mode == SL_SI91X_CONCURRENT_MODE)
      || (config->boot_config.oper_mode == SL_SI91X_TRANSMIT_TEST_MODE)) {
    if (config->band == SL_SI91X_WIFI_BAND_2_4GHZ) {
      interface_is_up[SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX] = true;
    } else if (config->band == SL_SI91X_WIFI_BAND_5GHZ) {
      interface_is_up[SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX] = true;
    }
  }
  // Save the coexistence mode in the driver
  save_coex_mode(config->boot_config.coex_mode);
#ifdef SL_SI91X_GET_EFUSE_DATA
  status = sl_si91x_get_flash_efuse_data(&si91x_efuse_data, config->efuse_data_type);
#endif
#ifdef SLI_SI91X_MCU_INTERFACE
  if (status == SL_STATUS_OK) {
    /* send a notification to the NWP indicating whether the M4 core is currently utilizing the XTAL as its clock source*/
    sli_si91x_send_m4_xtal_usage_notification_to_ta();
  }
#endif

  return status;
}

sl_status_t sl_si91x_driver_deinit(void)
{
  sl_status_t status = SL_STATUS_OK;

  // Check if the device has been initialized if not, return an error
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

#ifdef SLI_SI91X_MCU_INTERFACE
  // If the SLI_SI91X_MCU_INTERFACE is defined, perform a soft reset
  status = sl_si91x_soft_reset();
  VERIFY_STATUS_AND_RETURN(status);

  // Mask specific interrupts related to packet transfer
  mask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT | RX_PKT_TRANSFER_DONE_INTERRUPT);
#endif

#ifdef SLI_SI91X_ENABLE_BLE
  int32_t rsi_status = 0;
  // If SLI_SI91X_ENABLE_BLE is defined, deinitialize the BLE driver and check for errors
  rsi_status = rsi_ble_driver_deinit();
  if (rsi_status != RSI_SUCCESS) {
    return SL_STATUS_FAIL;
  }
#endif

  // Flush all TX Wi-Fi queues with the status indicating Wi-Fi connection is lost
  sli_si91x_flush_all_tx_wifi_queues(SL_STATUS_WIFI_CONNECTION_LOST);

  // Flush the generic TX data queue
  sli_si91x_flush_generic_data_queues(&sli_tx_data_queue);

#if defined(SLI_SI91X_OFFLOAD_NETWORK_STACK) && defined(SLI_SI91X_SOCKETS)

  // Flush all pending socket commands in the client VAP queue due to Wi-Fi connection loss
  sli_si91x_flush_all_socket_command_queues(SL_STATUS_WIFI_CONNECTION_LOST, SL_SI91X_WIFI_CLIENT_VAP_ID);

  // Flush all pending socket data in the client VAP queue due to Wi-Fi connection loss
  sli_si91x_flush_all_socket_data_queues(SL_SI91X_WIFI_CLIENT_VAP_ID);

  // Flush all pending socket commands in the AP VAP queue due to Wi-Fi connection loss
  sli_si91x_flush_all_socket_command_queues(SL_STATUS_WIFI_CONNECTION_LOST, SL_SI91X_WIFI_AP_VAP_ID);

  // Flush all pending socket data in the AP VAP queue due to Wi-Fi connection loss
  sli_si91x_flush_all_socket_data_queues(SL_SI91X_WIFI_AP_VAP_ID);

  // Shutdown and change the state of the client VAP sockets
  status = sli_si91x_vap_shutdown(SL_SI91X_WIFI_CLIENT_VAP_ID);
  VERIFY_STATUS_AND_RETURN(status);

  // Shutdown and change the state of the AP VAP sockets
  status = sli_si91x_vap_shutdown(SL_SI91X_WIFI_AP_VAP_ID);
  VERIFY_STATUS_AND_RETURN(status);

  // Deinitialize and free all socket-related resources
  status = sli_si91x_socket_deinit();
  VERIFY_STATUS_AND_RETURN(status);

#endif // End of check for offloaded network stack and sockets

  // Deinitialize the SI91x platform
  status = sl_si91x_platform_deinit();
  VERIFY_STATUS_AND_RETURN(status);

  // Deinitialize the SI91x host
  status = sl_si91x_host_deinit();
  VERIFY_STATUS_AND_RETURN(status);

#ifdef SLI_SI91X_MCU_INTERFACE
  // Check the RX buffer valid bit is set or not.
  if (M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID) {

    // Clear the RX buffer valid bit.
    M4SS_P2P_INTR_CLR_REG = (RX_BUFFER_VALID);

    // Clear the RX buffer.
    sl_si91x_host_free_buffer(rx_pkt_buffer);
  }
#endif

  // Deinitialize the buffer manager
  status = sl_si91x_host_deinit_buffer_manager();
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_host_disable_bus_interrupt();

  status = sl_si91x_host_power_cycle();
  VERIFY_STATUS_AND_RETURN(status);

  // Clear the event handler and reset initialization status
  si91x_event_handler  = NULL;
  device_initialized   = false;
  initialized_opermode = SL_SI91X_INVALID_MODE;

  // Reset all the interfaces
  memset(interface_is_up, 0, sizeof(interface_is_up));

  return status;
}

sl_status_t sl_si91x_get_flash_efuse_data(sl_si91x_efuse_data_t *efuse_data, uint8_t efuse_data_type)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;
  SL_WIFI_ARGS_CHECK_NULL_POINTER(efuse_data);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_GET_EFUSE_DATA,
                                        SI91X_COMMON_CMD,
                                        &efuse_data_type,
                                        sizeof(efuse_data_type),
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  const sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    switch (efuse_data_type) {
      case SL_SI91X_EFUSE_MFG_SW_VERSION:
        memcpy(&efuse_data->mfg_sw_version, packet->data, packet->length);
        break;
      case SL_SI91X_EFUSE_PTE_CRC:
        memcpy(&efuse_data->pte_crc, packet->data, packet->length);
        break;
      default:
        break;
    }
  }
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_driver_raw_send_command(uint8_t command,
                                             const void *data,
                                             uint32_t data_length,
                                             uint32_t wait_time)
{
  UNUSED_PARAMETER(wait_time);
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_status_t status = SL_STATUS_OK;

  // Allocate a data buffer with space for the data and metadata
  status = sl_si91x_allocate_data_buffer(&buffer,
                                         (void **)&packet,
                                         sizeof(sl_si91x_packet_t) + data_length,
                                         SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  VERIFY_STATUS_AND_RETURN(status);

  // If the packet is not allocated successfully, return an allocation failed error
  if (packet == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // Clear the packet descriptor and copy the command data if available
  memset(packet->desc, 0, sizeof(packet->desc));
  if (data != NULL) {
    memcpy(packet->data, data, data_length);
  }
  packet->length  = data_length & 0xFFF;
  packet->command = command;

  // Adding the packet to the queue with atomic action
  return sl_si91x_driver_send_data_packet(buffer, wait_time);
}

sl_status_t sl_si91x_driver_send_socket_data(const sli_si91x_socket_send_request_t *request,
                                             const void *data,
                                             uint32_t wait_time)
{
  UNUSED_PARAMETER(wait_time);
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sli_si91x_socket_send_request_t *send;

  sl_status_t status     = SL_STATUS_OK;
  uint16_t header_length = (request->data_offset - sizeof(sli_si91x_socket_send_request_t));
  uint32_t data_length   = request->length;

  if (data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Allocate a buffer for the socket data with appropriate size
  status = sl_si91x_host_allocate_buffer(
    &buffer,
    SL_WIFI_TX_FRAME_BUFFER,
    sizeof(sl_si91x_packet_t) + sizeof(sli_si91x_socket_send_request_t) + header_length + data_length,
    SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);

  VERIFY_STATUS_AND_RETURN(status);
  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  // If the packet is not allocated successfully, return an allocation failed error
  if (packet == NULL) {
    return SL_STATUS_WIFI_BUFFER_ALLOC_FAIL;
  }

  memset(packet->desc, 0, sizeof(packet->desc));

  send = (sli_si91x_socket_send_request_t *)packet->data;
  memcpy(send, request, sizeof(sli_si91x_socket_send_request_t));
  memcpy((send->send_buffer + header_length), data, data_length);

  // Fill frame type
  packet->length = (sizeof(sli_si91x_socket_send_request_t) + header_length + data_length) & 0xFFF;

  return sl_si91x_driver_send_data_packet(buffer, wait_time);
}

sl_status_t sl_si91x_custom_driver_send_command(uint32_t command,
                                                sl_si91x_command_type_t command_type,
                                                const void *data,
                                                uint32_t data_length,
                                                sl_si91x_wait_period_t wait_period,
                                                void *sdk_context,
                                                sl_wifi_buffer_t **data_buffer,
                                                uint8_t custom_host_desc)
{
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_status_t status;

  // Check if the queue type is within valid range
  if (command_type >= SI91X_CMD_MAX) {
    return SL_STATUS_INVALID_INDEX;
  }

  // Allocate a buffer for the command with appropriate size
  status = sli_si91x_allocate_command_buffer(&buffer,
                                             (void **)&packet,
                                             sizeof(sl_si91x_packet_t) + data_length,
                                             SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  VERIFY_STATUS_AND_RETURN(status);

  // Clear the packet descriptor and copy the command data if available
  memset(packet->desc, 0, sizeof(packet->desc));
  if (data != NULL) {
    memcpy(packet->data, data, data_length);
  }

  // Fill frame type
  packet->length  = data_length & 0xFFF;
  packet->command = (uint16_t)command;
  // Fill the packet identifier
  packet->unused[1] = custom_host_desc;
  return sl_si91x_driver_send_command_packet(command, command_type, buffer, wait_period, sdk_context, data_buffer);
}

sl_status_t sl_si91x_driver_send_command(uint32_t command,
                                         sl_si91x_command_type_t command_type,
                                         const void *data,
                                         uint32_t data_length,
                                         sl_si91x_wait_period_t wait_period,
                                         void *sdk_context,
                                         sl_wifi_buffer_t **data_buffer)
{
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_status_t status;

  // Check if the queue type is within valid range
  if (command_type >= SI91X_CMD_MAX) {
    return SL_STATUS_INVALID_INDEX;
  }

  // Allocate a buffer for the command with appropriate size
  status = sli_si91x_allocate_command_buffer(&buffer,
                                             (void **)&packet,
                                             sizeof(sl_si91x_packet_t) + data_length,
                                             SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  VERIFY_STATUS_AND_RETURN(status);

  // Clear the packet descriptor and copy the command data if available
  memset(packet->desc, 0, sizeof(packet->desc));
  if (data != NULL) {
    memcpy(packet->data, data, data_length);
  }

  // Set SLI_SI91X_FEAT_FW_UPDATE_NEW_CODE in the feature bit map to retrieve the latest firmware result codes
  if (command == RSI_COMMON_REQ_OPERMODE) {
    sl_si91x_boot_configuration_t *boot_configuration = (sl_si91x_boot_configuration_t *)packet->data;
    boot_configuration->feature_bit_map |= SLI_SI91X_FEAT_FW_UPDATE_NEW_CODE;
  }

  // Fill frame type
  packet->length  = data_length & 0xFFF;
  packet->command = (uint16_t)command;
  return sl_si91x_driver_send_command_packet(command, command_type, buffer, wait_period, sdk_context, data_buffer);
}

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
sl_status_t sl_si91x_driver_send_side_band_crypto(uint32_t command,
                                                  const void *data,
                                                  uint32_t data_length,
                                                  sl_si91x_wait_period_t wait_period)
{
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  uint32_t result;
  sl_status_t status = SL_STATUS_OK;

  // Allocate a buffer for the command with appropriate size
  status = sli_si91x_allocate_command_buffer(&buffer, (void **)&packet, sizeof(sl_si91x_packet_t) + data_length, 1000);
  VERIFY_STATUS_AND_RETURN(status);

  // Clear the packet descriptor and copy the command data if available
  memset(packet->desc, 0, sizeof(packet->desc));
  if (data != NULL) {
    memcpy(packet->data, data, data_length);
  }

  // Fill frame type
  packet->length  = data_length & 0xFFF;
  packet->command = command;

  // Acquire Mutex
  osMutexAcquire(side_band_crypto_mutex, 0xFFFFFFFFUL);

  // fill crypto desc
  crypto_desc[0].addr   = (uint32_t)packet->desc;
  crypto_desc[0].length = SL_HOST_DESC_LEN;
  crypto_desc[1].addr   = (uint32_t)packet->data;
  crypto_desc[1].length = SL_CRYPTO_PKT_LEN;

  //! Enter Critical Section
  __disable_irq();

  sli_si91x_raise_side_band_interrupt_to_ta();

  //! Exit Critical Section
  __enable_irq();

  result =
    osEventFlagsWait(ta_events, SIDE_BAND_DONE, (osFlagsWaitAny), (wait_period & ~SL_SI91X_WAIT_FOR_RESPONSE_BIT));
  if (result == (uint32_t)osErrorTimeout || result == (uint32_t)osErrorResource) {
    osMutexRelease(side_band_crypto_mutex);
    sl_si91x_host_free_buffer(buffer);
    return SL_STATUS_TIMEOUT;
  }

  status =
    (((uint8_t *)crypto_desc[0].addr)[12] + (((uint8_t *)crypto_desc[0].addr)[13] << 8)); // Extract the frame status

  // Release Mutex
  osMutexRelease(side_band_crypto_mutex);

  sl_si91x_host_free_buffer(buffer);
  return status;
}
#endif

sl_status_t sl_si91x_driver_send_bt_command(rsi_wlan_cmd_request_t command,
                                            sl_si91x_command_type_t command_type,
                                            sl_wifi_buffer_t *data,
                                            uint8_t sync_command)
{
  sl_si91x_wait_period_t wait_period = SL_SI91X_RETURN_IMMEDIATELY;

  // Check if the queue type is within valid range
  if (command_type >= SI91X_CMD_MAX) {

    return SL_STATUS_INVALID_INDEX;
  }

  if (sync_command) {
    return sl_si91x_driver_send_command_packet(command, command_type, data, wait_period, NULL, NULL);
  } else {
    return sl_si91x_driver_send_async_command(command, command_type, data, 0);
  }
}

sl_status_t sl_si91x_driver_wait_for_response(rsi_wlan_cmd_request_t command, sl_si91x_wait_period_t wait_period)
{
  UNUSED_PARAMETER(command);
  UNUSED_PARAMETER(wait_period);
#ifdef SI91x_ENABLE_WAIT_ON_RESULTS
  // Wait for WLAN response events with a specified timeout
  uint32_t events =
    sli_si91x_wait_for_event(NCP_HOST_WLAN_RESPONSE_EVENT, (wait_period & ~SL_SI91X_WAIT_FOR_RESPONSE_BIT));

  sli_si91x_clear_event(NCP_HOST_WLAN_RESPONSE_EVENT);

  //TODO: Change error handling from event based to response
  if (events & NCP_HOST_WLAN_RESPONSE_EVENT) {
    return convert_and_save_firmware_status(si91x_frame_error_status);
  } else if (events == 0) {
    return SL_STATUS_TIMEOUT;
  }
#endif
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sli_si91x_driver_wait_for_response_packet(sl_si91x_buffer_queue_t *queue,
                                                      osEventFlagsId_t event_flag,
                                                      uint32_t event_mask,
                                                      uint16_t packet_id,
                                                      sl_si91x_wait_period_t wait_period,
                                                      sl_wifi_buffer_t **packet_buffer)
{
  // Verify that packet_buffer is a valid pointer, return error if invalid
  SL_VERIFY_POINTER_OR_RETURN(packet_buffer, SL_STATUS_INVALID_PARAMETER);

  // Variables to store event flags, start time, and elapsed time
  uint32_t events       = 0;
  uint32_t start_time   = osKernelGetTickCount(); // Capture the start time of the wait period
  uint32_t elapsed_time = 0;                      // Elapsed time tracker
  sl_wifi_buffer_t *buffer;

  do {
    // Wait for event flag(s) to be set within the specified wait period.
    // This blocks the thread until any event in the mask is set or a timeout occurs.
    events = osEventFlagsWait(event_flag, event_mask, (osFlagsWaitAny | osFlagsNoClear), (wait_period - elapsed_time));

    // If the event wait times out or resources are unavailable, return timeout status
    if (events == (uint32_t)osErrorTimeout || events == (uint32_t)osErrorResource) {
      return SL_STATUS_TIMEOUT;
    }

    // Log the event and queue details (for debugging purposes)
    SL_DEBUG_LOG("Event: %u, queue %u\n", events, queue);

    // Traverse the queue to check if the packet with the desired packet_id is at the head.
    // Introduce a delay if the head of the queue packet doesn't belong to the current thread.
    // This allows other threads to process the packet at the head.
    do {
      buffer = queue->head; // Peek at the head of the queue
    } while ((buffer != NULL) && (buffer->id != packet_id)
             && osDelay(1) == 0); // Delay to yield if packet_id does not match

    // Update the elapsed time since the start of the wait
    elapsed_time = sl_si91x_host_elapsed_time(start_time);

  } while (buffer == NULL || (buffer->id != packet_id)); // Loop until the correct packet is found or timeout occurs

  // Remove the identified packet from the queue
  sli_si91x_pop_from_buffer_queue(queue, &buffer);

  // Assign the identified packet to packet_buffer
  *packet_buffer = buffer;

  // Enter atomic section to ensure thread safety while clearing the event flag
  CORE_irqState_t state = CORE_EnterAtomic();
  // If the queue is empty after popping the packet, clear the event flag to avoid unnecessary waits
  if (queue->head == NULL) {
    osEventFlagsClear(event_flag, event_mask);
  }
  CORE_ExitAtomic(state); // Exit atomic section

  return SL_STATUS_OK; // Return success status
}

sl_status_t sl_si91x_driver_send_command_packet(uint32_t command,
                                                sl_si91x_command_type_t command_type,
                                                sl_wifi_buffer_t *buffer,
                                                sl_si91x_wait_period_t wait_period,
                                                void *sdk_context,
                                                sl_wifi_buffer_t **data_buffer)
{
  uint16_t firmware_status;
  sli_si91x_queue_packet_t *node = NULL;
  sl_status_t status;
  sl_wifi_buffer_t *packet;
  sl_wifi_buffer_t *response;
  uint8_t flags        = 0;
  uint16_t data_length = 0;
  //  sl_si91x_driver_context_t context = { 0 };
  sl_si91x_wait_period_t wait_time = 0;
  static uint8_t command_packet_id = 0;

  // Allocate a command packet and set flags based on the command type
  status = sli_si91x_allocate_command_buffer(&packet,
                                             (void **)&node,
                                             sizeof(sli_si91x_queue_packet_t),
                                             SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  if (status != SL_STATUS_OK) {
    sl_si91x_host_free_buffer(buffer);
    return status;
  }

#ifdef RSI_CHIP_MFG_EN
  // WLAN soc frequency
  if (command == RSI_BOOTUP_PARAMS) {
    host_desc[14] = RSI_SOC_FREQ;
  }
#endif

  // Check the wait_period to determine the flags for packet handling
  if (wait_period == SL_SI91X_RETURN_IMMEDIATELY) {
    // If wait_period indicates an immediate return, set flags to 0
    flags = 0;
  } else {
    // If not an immediate return, set the SI91X_PACKET_RESPONSE_STATUS flag
    flags |= SI91X_PACKET_RESPONSE_STATUS;
    // Additionally, set the SI91X_PACKET_RESPONSE_PACKET flag if the SL_SI91X_WAIT_FOR_RESPONSE_BIT is set in wait_period
    if (data_buffer != NULL) {
      flags |= ((wait_period & SL_SI91X_WAIT_FOR_RESPONSE_BIT) ? SI91X_PACKET_RESPONSE_PACKET : 0);
    }
  }

  // Check the command type and set the flags accordingly
  switch (command) {
    case RSI_COMMON_REQ_PWRMODE:
    case RSI_COMMON_REQ_OPERMODE:
    case RSI_COMMON_REQ_SOFT_RESET:
      flags |= SI91X_PACKET_GLOBAL_QUEUE_BLOCK;
      break;
    default:
      break;
  }

  // Set various properties of the node representing the command packet
  node->host_packet       = buffer;
  node->firmware_queue_id = firmware_queue_id[command_type];
  node->command_type      = command_type;
  node->flags             = flags;
  node->sdk_context       = sdk_context;

  // Configure the context for packet handling
  //  context.packet = node;
  //  context.payload = &(queue_packet_id[command_type]);

  if (flags != SI91X_PACKET_WITH_ASYNC_RESPONSE) {
    node->command_tickcount = osKernelGetTickCount();
    // Calculate the wait time based on wait_period
    if ((wait_period & SL_SI91X_WAIT_FOR_EVER) == SL_SI91X_WAIT_FOR_EVER) {
      node->command_timeout = osWaitForever;
    } else {
      node->command_timeout = (wait_period & ~SL_SI91X_WAIT_FOR_RESPONSE_BIT);
    }
  }

  //! Enter Critical Section
  CORE_irqState_t state = CORE_EnterAtomic();

  const uint8_t this_packet_id = command_packet_id;
  command_packet_id++;
  buffer->id        = this_packet_id;
  packet->id        = this_packet_id;
  packet->node.node = NULL;
  sli_si91x_append_to_buffer_queue(&cmd_queues[command_type].tx_queue, packet);
  tx_command_queues_status |= SL_SI91X_TX_PENDING_FLAG(command_type);
  sli_si91x_set_event(SL_SI91X_TX_PENDING_FLAG(command_type));
  CORE_ExitAtomic(state);

  // Check if the command should return immediately or wait for a response
  if (wait_period == SL_SI91X_RETURN_IMMEDIATELY) {
    return SL_STATUS_IN_PROGRESS;
  }

  // Calculate the wait time based on wait_period
  if ((wait_period & SL_SI91X_WAIT_FOR_EVER) == SL_SI91X_WAIT_FOR_EVER) {
    wait_time = osWaitForever;
  } else {
    wait_time = (wait_period & ~SL_SI91X_WAIT_FOR_RESPONSE_BIT);
  }

  // Wait for a response packet and handle it
  status = sli_si91x_driver_wait_for_response_packet(&cmd_queues[command_type].rx_queue,
                                                     si91x_events,
                                                     SL_SI91X_RESPONSE_FLAG(command_type),
                                                     this_packet_id,
                                                     wait_time,
                                                     &response);
  // Check if the status is SL_STATUS_TIMEOUT, indicating a timeout has occurred
  if (status == SL_STATUS_TIMEOUT) {
    // Declare a temporary packet pointer to hold the packet to be removed
    sl_wifi_buffer_t *temp_packet;
    sl_status_t temp_status = sli_si91x_remove_buffer_from_queue_by_comparator(&cmd_queues[command_type].tx_queue,
                                                                               &this_packet_id,
                                                                               si91x_packet_identification_function,
                                                                               &temp_packet);

    // Check if the packet removal was successful
    if (temp_status == SL_STATUS_OK) {

      // Retrieve the actual packet node data from the removed buffer
      sli_si91x_queue_packet_t *temp_node = sl_si91x_host_get_buffer_data(temp_packet, 0, NULL);

      // Free the host packet memory associated with the node (TX packet memory)
      sl_si91x_host_free_buffer(temp_node->host_packet);

      // Free the temporary buffer memory that held the packet
      sl_si91x_host_free_buffer(temp_packet);
    }
  }
  VERIFY_STATUS_AND_RETURN(status);

  // Process the response packet and return the firmware status
  node            = (sli_si91x_queue_packet_t *)sl_si91x_host_get_buffer_data(response, 0, &data_length);
  firmware_status = node->frame_status;

  // If a data_buffer is provided, set it to the host_packet
  if (NULL != data_buffer) {
    *data_buffer = node->host_packet;
  }
  // If the response packet flag is set, free the host_packet buffer
  else if (SI91X_PACKET_RESPONSE_PACKET == (node->flags & SI91X_PACKET_RESPONSE_PACKET)) {
    sl_si91x_host_free_buffer(node->host_packet);
  }

  // Free the response buffer and return the firmware status
  sl_si91x_host_free_buffer(response);
  return convert_and_save_firmware_status(firmware_status);
}

static sl_status_t sl_si91x_driver_send_data_packet(sl_wifi_buffer_t *buffer, uint32_t wait_time)
{
  UNUSED_PARAMETER(wait_time);
  sli_si91x_append_to_buffer_queue(&sli_tx_data_queue, buffer);
  CORE_irqState_t state = CORE_EnterAtomic();
  tx_generic_socket_data_queues_status |= SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT;
  sli_si91x_set_event(SL_SI91X_GENERIC_DATA_TX_PENDING_EVENT);
  CORE_ExitAtomic(state);

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_driver_send_async_command(uint32_t command,
                                               sl_si91x_command_type_t command_type,
                                               void *data,
                                               uint32_t data_length)
{

  sli_si91x_queue_packet_t *node = NULL;
  sl_status_t return_status;
  //  sl_si91x_driver_context_t context = { 0 };
  sl_wifi_buffer_t *raw_rx_buffer;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *raw_rx_packet;
  sl_status_t status;

  if (command_type == SI91X_BT_CMD) {
    // BLE packet is created in upper layer, no allocations required here.
    raw_rx_buffer = (sl_wifi_buffer_t *)data;
  } else {
    status = sli_si91x_allocate_command_buffer(&raw_rx_buffer,
                                               (void **)&raw_rx_packet,
                                               sizeof(sl_si91x_packet_t) + data_length,
                                               SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
    VERIFY_STATUS_AND_RETURN(status);

    memset(raw_rx_packet->desc, 0, sizeof(raw_rx_packet->desc));
    if (data != NULL) {
      memcpy(raw_rx_packet->data, data, data_length);
    }

    // Fill frame type
    raw_rx_packet->length  = data_length & 0xFFF;
    raw_rx_packet->command = (uint16_t)command;
  }

  return_status = sli_si91x_allocate_command_buffer(&buffer,
                                                    (void **)&node,
                                                    sizeof(sli_si91x_queue_packet_t),
                                                    SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);

  if (return_status != SL_STATUS_OK) {
    sl_si91x_host_free_buffer(raw_rx_buffer);
    return return_status;
  }

#ifdef RSI_CHIP_MFG_EN
  // WLAN soc frequency
  if (command == RSI_BOOTUP_PARAMS) {
    host_desc[14] = RSI_SOC_FREQ;
  }
#endif

  // Configure the node representing the command packet
  node->host_packet       = raw_rx_buffer;
  node->firmware_queue_id = firmware_queue_id[command_type];
  node->command_type      = command_type;
  node->sdk_context       = NULL;
  node->flags             = SI91X_PACKET_WITH_ASYNC_RESPONSE;

  CORE_irqState_t irqState = CORE_EnterAtomic();
  buffer->id               = 0; // Does not use packet ID as async packets do not have a matching response
  sli_si91x_append_to_buffer_queue(&cmd_queues[command_type].tx_queue, buffer);
  tx_command_queues_status |= SL_SI91X_TX_PENDING_FLAG(command_type);
  sli_si91x_set_event(SL_SI91X_TX_PENDING_FLAG(command_type));
  CORE_ExitAtomic(irqState);

  return SL_STATUS_OK;
}

/*
 * Verifies that the device has booted successfully.
 * @return SL_STATUS_OK if verification successful
 *         otherwise RSI_ERROR_WAITING_FOR_BOARD_READY, RSI_ERROR_BOOTUP_OPTIONS_NOT_SAVED, RSI_ERROR_BOOTUP_OPTIONS_CHECKSUM_FAIL
 */
sl_status_t sli_verify_device_boot(uint32_t *rom_version)
{
  sl_status_t status;
  uint8_t value[2] = { 0, 0 };

  // Read a memory register to check if it's valid
  status = sl_si91x_bus_read_memory(RSI_HOST_INTF_REG_OUT, 2, &value[0]);
  VERIFY_STATUS(status);

  // Verify register read was valid
  if (value[1] != SLI_WIFI_REGISTER_VALID) {
    return SL_STATUS_WAITING_FOR_BOARD_READY;
  }

  // Verify register value
  if (value[0] == RSI_BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED) {
    return SL_STATUS_BOOTUP_OPTIONS_NOT_SAVED;
  } else if (value[0] == RSI_BOOTUP_OPTIONS_CHECKSUM_FAIL) {
    return SL_STATUS_BOOTUP_OPTIONS_CHECKSUM_FAILURE;
  }
#if RSI_BOOTLOADER_VERSION_CHECK
  else if (value[0] == RSI_BOOTLOADER_VERSION) {
  } else {
    return SL_STATUS_BOOTLOADER_VERSION_MISMATCH;
  }
#endif

  // Extract ROM version info
  if (value[0] == RSI_BOOTLOADER_VERSION_1P0) {
    *rom_version = RSI_ROM_VERSION_1P0;
  } else if (value[0] == RSI_BOOTLOADER_VERSION_1P1) {
    *rom_version = RSI_ROM_VERSION_1P1;
  }

  return SL_STATUS_OK;
}

/**************************************************************************/ /**
 * @brief Allocate a buffer for the Wi-Fi driver
 *
 * @param buffer
 * @param buffer_size is the size of the buffer to allocate
 * @param wait_duration_ms is the duration before returning SL_TIMEOUT
 * @return SL_STATUS_OK if the values are retrieved correctly,
 * SL_TIMEOUT if the buffer is not allocated in time, SL_ERROR otherwise
 *****************************************************************************/
sl_status_t sli_si91x_allocate_command_buffer(sl_wifi_buffer_t **host_buffer,
                                              void **buffer,
                                              uint32_t requested_buffer_size,
                                              uint32_t wait_duration_ms)
{
  // Allocate a buffer from the SI91x host for WLAN control messages
  sl_status_t status =
    sl_si91x_host_allocate_buffer(host_buffer, SL_WIFI_CONTROL_BUFFER, requested_buffer_size, wait_duration_ms);
  VERIFY_STATUS_AND_RETURN(status);

  uint16_t temp;
  // Get a pointer to the allocated buffer's data area
  *buffer = sl_si91x_host_get_buffer_data(*host_buffer, 0, &temp);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_allocate_data_buffer(sl_wifi_buffer_t **host_buffer,
                                          void **buffer,
                                          uint32_t data_size,
                                          uint32_t wait_duration_ms)
{
  // Allocate a buffer from the SI91x host for WLAN data transmission
  sl_status_t status =
    sl_si91x_host_allocate_buffer(host_buffer,
                                  SL_WIFI_TX_FRAME_BUFFER,
                                  sizeof(sl_si91x_packet_t) + sizeof(sli_si91x_socket_send_request_t) + data_size,
                                  wait_duration_ms);
  VERIFY_STATUS_AND_RETURN(status);

  uint16_t temp;
  // Get a pointer to the allocated buffer's data area
  *buffer = sl_si91x_host_get_buffer_data(*host_buffer, 0, &temp);
  return SL_STATUS_OK;
}

sl_status_t sli_wifi_select_option(const uint8_t configuration)
{
  uint16_t boot_command = 0;
  sl_status_t status    = 0;
  uint16_t read_value   = 0;

  // Write a boot command to initiate the option selection
  status = sl_si91x_bus_write_memory(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&boot_command);
  VERIFY_STATUS_AND_RETURN(status);

  if ((configuration == BURN_NWP_FW) || (configuration == BURN_M4_FW)) {
    boot_command = RSI_HOST_INTERACT_REG_VALID_FW | configuration;
  } else {
    boot_command = RSI_HOST_INTERACT_REG_VALID | configuration;
  }

  if (configuration == BURN_M4_FW) {
    boot_command |= M4_FW_IMAGE_NUMBER;
  }

  // Write the configuration to the SI91x host for option selection
  status = sl_si91x_bus_write_memory(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&boot_command);
  VERIFY_STATUS_AND_RETURN(status);

  // Check for a specific response to ensure successful option selection
  if ((configuration != LOAD_NWP_FW) && (configuration != LOAD_DEFAULT_NWP_FW_ACTIVE_LOW)) {
    uint32_t timestamp = sl_si91x_host_get_timestamp();
    while (sl_si91x_host_elapsed_time(timestamp) < 300) {
      status = sl_si91x_bus_read_memory(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_value);
      VERIFY_STATUS_AND_RETURN(status);

      if ((configuration == BURN_NWP_FW) || (configuration == BURN_M4_FW)) {
        if (read_value == (RSI_HOST_INTERACT_REG_VALID | RSI_SEND_RPS_FILE)) {
          return SL_STATUS_OK;
        }
      } else if (read_value == (RSI_HOST_INTERACT_REG_VALID | configuration)) {
        return SL_STATUS_OK;
      }
    }
  } else {
    // Check up to 3 seconds for firmware load or upgrade status
    uint32_t timestamp               = sl_si91x_host_get_timestamp();
    uint16_t default_nwp_fw_selected = 0;
    while (sl_si91x_host_elapsed_time(timestamp) < 3000) {
      status = sl_si91x_bus_read_memory(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_value);
      if (status != SL_STATUS_OK)
        continue;

      if ((read_value & 0xF000) == (RSI_HOST_INTERACT_REG_VALID_FW & 0xF000)) {
        if ((read_value & 0xFF) == VALID_FIRMWARE_NOT_PRESENT) {
          if (default_nwp_fw_selected == 0) {
            boot_command = RSI_HOST_INTERACT_REG_VALID_FW | SELECT_DEFAULT_NWP_FW_IMAGE_NUMBER;
            status       = sl_si91x_bus_write_memory(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&boot_command);
            if (status != SL_STATUS_OK) {
              return status;
            }

            while (sl_si91x_host_elapsed_time(timestamp) < 2000) {
              status = sl_si91x_bus_read_memory(RSI_HOST_INTF_REG_OUT, 2, (uint8_t *)&read_value);
              if (status != SL_STATUS_OK)
                continue;
              if (read_value == (RSI_HOST_INTERACT_REG_VALID | SELECT_DEFAULT_NWP_FW_IMAGE_NUMBER)) {
                break;
              }
            }

            boot_command = RSI_HOST_INTERACT_REG_VALID_FW | configuration;
            status       = sl_si91x_bus_write_memory(RSI_HOST_INTF_REG_IN, 2, (uint8_t *)&boot_command);
            if (status != SL_STATUS_OK) {
              return status;
            }
            default_nwp_fw_selected = 1;
            continue;
          } else {
            return SL_STATUS_VALID_FIRMWARE_NOT_PRESENT;
          }
        }
        if ((read_value & 0xFF) == RSI_INVALID_OPTION) {
          return SL_STATUS_INVALID_OPTION;
        }
        if ((read_value & 0xFF) == RSI_CHECKSUM_SUCCESS) {
          return status;
        }
      }
    }
  }
  return SL_STATUS_FW_LOAD_OR_UPGRADE_TIMEOUT;
}

sl_status_t sl_si91x_enable_radio(void)
{
  uint8_t data       = 1;
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_RADIO,
                                                    SI91X_WLAN_CMD,
                                                    &data,
                                                    1,
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_disable_radio(void)
{
  uint8_t data       = 0;
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_RADIO,
                                                    SI91X_WLAN_CMD,
                                                    &data,
                                                    1,
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_write_calibration_data(const si91x_calibration_data_t *data)
{
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_CALIB_WRITE,
                                                    SI91X_WLAN_CMD,
                                                    data,
                                                    sizeof(si91x_calibration_data_t),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_wifi_set_certificate_index(uint8_t certificate_type,
                                                uint8_t certificate_index,
                                                const uint8_t *buffer,
                                                uint32_t certificate_length)
{
  uint32_t rem_len                         = 0;
  uint16_t chunk_size                      = 0;
  uint16_t data_size                       = 0;
  uint8_t chunks_remaining                 = 0;
  uint32_t offset                          = 0;
  sl_status_t status                       = SL_STATUS_OK;
  sl_si91x_req_set_certificate_t chunk_ptr = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  // Get the certificate chunk size
  chunk_size = (SI91X_MAX_CERT_SEND_SIZE - sizeof(sl_si91x_cert_info_t));

  // Get certificate length
  rem_len = certificate_length;

  do {
    // Memset the PKT
    memset(&chunk_ptr, 0, sizeof(sl_si91x_req_set_certificate_t));

    // If certificate length is 0, erase certificate
    if (rem_len == 0) {
      // Mark Data Size
      data_size = 0;

      // More chunks to send
      chunks_remaining = 0;
    } else if (rem_len >= chunk_size) {
      // Mark Data Size
      data_size = chunk_size;

      // More chunks to send
      chunks_remaining = 1;
    } else {
      // Mark Data Size
      data_size = (uint16_t)rem_len;

      // More chunks to send
      chunks_remaining = 0;
    }

    if ((data_size > 0) && (NULL != buffer)) {
      // Copy the certificate chunk
      memcpy(chunk_ptr.certificate, buffer + offset, data_size);
    }

    // Move the offset by chunk size
    offset += data_size;

    // Subtract the rem_len by the chunk size
    rem_len -= data_size;

    //Set the total length of certificate
    memcpy(&chunk_ptr.cert_info.total_len, &certificate_length, sizeof(chunk_ptr.cert_info.total_len));

    // Set the certificate type
    chunk_ptr.cert_info.certificate_type = certificate_type;

    // Set the certificate index
    chunk_ptr.cert_info.certificate_inx = certificate_index;

    // More chunks to send
    chunk_ptr.cert_info.more_chunks = chunks_remaining;

    // Set the length of the certificate chunk
    chunk_ptr.cert_info.certificate_length = data_size;

    // Send the driver command
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_CERTIFICATE,
                                          SI91X_WLAN_CMD,
                                          &chunk_ptr,
                                          (sizeof(sl_si91x_cert_info_t) + data_size),
                                          SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
  } while (rem_len > 0);

  // Return status
  return status;
}

sl_status_t sl_si91x_set_rtc_timer(const sl_si91x_module_rtc_time_t *timer)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((timer->tm_sec > 59) || (timer->tm_min > 59) || (timer->tm_hour > 23)
      || ((timer->tm_mday < 1) || (timer->tm_mday > 31)) || (timer->tm_mon > 11)
      || ((timer->tm_wday < 1) || (timer->tm_wday > 7))) {

    // Checking Invalid Parameters
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Send set RTC timer request
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_SET_RTC_TIMER,
                                        SI91X_COMMON_CMD,
                                        timer,
                                        sizeof(sl_si91x_module_rtc_time_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_get_rtc_timer(sl_si91x_module_rtc_time_t *response)
{
  sl_status_t status       = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(response);

  // Send get RTC timer request
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_GET_RTC_TIMER,
                                        SI91X_COMMON_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(30000),
                                        NULL,
                                        &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  // Extract the RTC timer data from the response
  const sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(response, packet->data, sizeof(sl_si91x_module_rtc_time_t));
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_set_device_region(sl_si91x_operation_mode_t operation_mode,
                                       sl_si91x_band_mode_t band,
                                       sl_si91x_region_code_t region_code)
{
  sl_status_t status = SL_STATUS_OK;

  switch (operation_mode) {
    case SL_SI91X_CLIENT_MODE:
    case SL_SI91X_ENTERPRISE_CLIENT_MODE:
    case SL_SI91X_TRANSMIT_TEST_MODE:
    case SL_SI91X_TRANSCEIVER_MODE: {
      // For client and transmit test modes, send a command to set the region code
      sl_si91x_set_region_request_t request = { 0 };

      request.set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER;
      request.region_code                   = (uint8_t)region_code;

      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_REGION,
                                            SI91X_WLAN_CMD,
                                            &request,
                                            sizeof(sl_si91x_set_region_request_t),
                                            SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                            NULL,
                                            NULL);
      VERIFY_STATUS_AND_RETURN(status);
      break;
    }

    case SL_SI91X_CONCURRENT_MODE:
    case SL_SI91X_ACCESS_POINT_MODE: {
      if (operation_mode == SL_SI91X_CONCURRENT_MODE) {
        // For concurrent mode, send a command to set the region code
        sl_si91x_set_region_request_t request = { 0 };

        request.set_region_code_from_user_cmd = SET_REGION_CODE_FROM_USER;
        request.region_code                   = (uint8_t)region_code;

        status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_REGION,
                                              SI91X_WLAN_CMD,
                                              &request,
                                              sizeof(sl_si91x_set_region_request_t),
                                              SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                              NULL,
                                              NULL);
        VERIFY_STATUS_AND_RETURN(status);
      }

      // For AP and concurrent modes, configure region-specific settings
      sl_si91x_set_region_ap_request_t request = { 0 };
      request.set_region_code_from_user_cmd    = SET_REGION_CODE_FROM_USER;

      // Configure region-specific settings based on the region code and band
      switch (region_code) {
        // Configure settings for different regions and bands
        case DEFAULT_REGION:
        case US: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_US_region_2_4GHZ_configurations;
          } else {
            request = default_US_region_5GHZ_configurations;
          }
          break;
        }
        case EU: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_EU_region_2_4GHZ_configurations;
          } else {
            request = default_EU_region_5GHZ_configurations;
          }
          break;
        }
        case JP: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_JP_region_2_4GHZ_configurations;
          } else {
            request = default_JP_region_5GHZ_configurations;
          }
          break;
        }
        case KR: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_KR_region_2_4GHZ_configurations;
          } else {
            request = default_KR_region_5GHZ_configurations;
          }
          break;
        }
        case SG: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_SG_region_2_4GHZ_configurations;
          } else {
            request = default_SG_region_5GHZ_configurations;
          }
          break;
        }
        case CN: {
          if (band == SL_SI91X_WIFI_BAND_2_4GHZ) {
            request = default_CN_region_2_4GHZ_configurations;
          } else {
            request = default_CN_region_5GHZ_configurations;
          }
          break;
        }
        default:
          return SL_STATUS_NOT_SUPPORTED;
      }
      status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_REGION_AP,
                                            SI91X_WLAN_CMD,
                                            &request,
                                            sizeof(sl_si91x_set_region_ap_request_t),
                                            SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                            NULL,
                                            NULL);
      VERIFY_STATUS_AND_RETURN(status);
      break;
    }

    default:
      break;
  }

  return status;
}

#ifdef SLI_SI91X_MCU_INTERFACE

sl_status_t sl_si91x_command_to_write_common_flash(uint32_t write_address,
                                                   const uint8_t *write_data,
                                                   uint16_t write_data_length,
                                                   uint8_t flash_sector_erase_enable)
{
  // Check if write_data_length is non-zero
  if (write_data_length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status                        = SL_STATUS_OK;
  sl_si91x_request_ta2m4_t ta_to_m4_request = { 0 };
  uint32_t send_size                        = 0;
  uint16_t remaining_length                 = write_data_length;

  // If flash_sector_erase_enable is 1, Send request to NWP in chunks of 4k
  if (flash_sector_erase_enable == 1) {
    while (remaining_length > 0) {
      // Calculate the chunk size, capped at 4k
      size_t chunkSize = (remaining_length < FLASH_SECTOR_SIZE) ? remaining_length : FLASH_SECTOR_SIZE;

      // Fill the request structure
      memset(&ta_to_m4_request, 0, sizeof(sl_si91x_request_ta2m4_t));
      ta_to_m4_request.sub_cmd                   = SL_SI91X_WRITE_TO_COMMON_FLASH;
      ta_to_m4_request.addr                      = write_address;
      ta_to_m4_request.input_buffer_length       = (uint16_t)chunkSize;
      ta_to_m4_request.flash_sector_erase_enable = flash_sector_erase_enable;

      send_size = sizeof(sl_si91x_request_ta2m4_t);

      status = sl_si91x_driver_send_command(RSI_COMMON_REQ_TA_M4_COMMANDS,
                                            SI91X_COMMON_CMD,
                                            &ta_to_m4_request,
                                            send_size,
                                            SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                            NULL,
                                            NULL);
      VERIFY_STATUS_AND_RETURN(status);

      // Adjust write_address for the next chunk
      write_address += chunkSize;

      // Adjust remaining_length for the next chunk
      remaining_length -= chunkSize;
    }
  }

  else {
    // Check if write_data pointer is valid
    SL_VERIFY_POINTER_OR_RETURN(write_data, SL_STATUS_INVALID_PARAMETER);

    // Write in chunks of MAX_CHUNK_SIZE for flash_sector_erase_enable != 1
    while (write_data_length > 0) {
      size_t chunkSize = (write_data_length < MAX_CHUNK_SIZE) ? write_data_length : MAX_CHUNK_SIZE;

      // Fill the request structure
      memset(&ta_to_m4_request, 0, sizeof(sl_si91x_request_ta2m4_t));
      ta_to_m4_request.sub_cmd                   = SL_SI91X_WRITE_TO_COMMON_FLASH;
      ta_to_m4_request.addr                      = write_address;
      ta_to_m4_request.input_buffer_length       = (uint16_t)chunkSize;
      ta_to_m4_request.flash_sector_erase_enable = flash_sector_erase_enable;

      // Copy write_data into the request structure
      memcpy(&ta_to_m4_request.input_data, write_data, chunkSize);

      // Calculate the send size and send the command to write to common flash
      send_size = sizeof(sl_si91x_request_ta2m4_t) - MAX_CHUNK_SIZE + chunkSize;
      status    = sl_si91x_driver_send_command(RSI_COMMON_REQ_TA_M4_COMMANDS,
                                            SI91X_COMMON_CMD,
                                            &ta_to_m4_request,
                                            send_size,
                                            SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                            NULL,
                                            NULL);
      VERIFY_STATUS_AND_RETURN(status);

      // Adjust pointers and counters
      write_address += chunkSize;
      write_data += chunkSize;
      write_data_length -= chunkSize;
    }
  }
  return status;
}

sl_status_t sl_si91x_command_to_read_common_flash(uint32_t read_address, size_t length, uint8_t *output_buffer)
{
  // Check if output_buffer pointer is valid
  SL_VERIFY_POINTER_OR_RETURN(output_buffer, SL_STATUS_INVALID_PARAMETER);

  // Check if length is non-zero
  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_status_t status              = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer        = NULL;
  const sl_si91x_packet_t *packet = NULL;

  while (length > 0) {
    size_t chunkSize = (length < MAX_CHUNK_SIZE) ? length : MAX_CHUNK_SIZE;

    sl_si91x_read_flash_request_t m4_to_ta_read_request = { 0 };
    memset(&m4_to_ta_read_request, 0, sizeof(sl_si91x_read_flash_request_t));
    m4_to_ta_read_request.sub_cmd              = SL_SI91X_READ_FROM_COMMON_FLASH;
    m4_to_ta_read_request.nwp_address          = read_address;
    m4_to_ta_read_request.output_buffer_length = (uint16_t)chunkSize;

    uint32_t send_size = sizeof(sl_si91x_read_flash_request_t);

    status = sl_si91x_driver_send_command(RSI_COMMON_REQ_TA_M4_COMMANDS,
                                          SI91X_COMMON_CMD,
                                          &m4_to_ta_read_request,
                                          send_size,
                                          SL_SI91X_WAIT_FOR_RESPONSE(32000),
                                          NULL,
                                          &buffer);
    if (status != SL_STATUS_OK) {
      if (buffer != NULL)
        sl_si91x_host_free_buffer(buffer);
      return status;
    }
    VERIFY_STATUS_AND_RETURN(status);

    packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
    memcpy(output_buffer, packet->data, packet->length);
    sl_si91x_host_free_buffer(buffer);

    // Adjust pointers and counters
    read_address += chunkSize;
    output_buffer += chunkSize;
    length -= chunkSize;
  }

  return status;
}

sl_status_t sl_si91x_m4_ta_secure_handshake(uint8_t sub_cmd_type,
                                            uint8_t input_len,
                                            const uint8_t *input_data,
                                            uint8_t output_len,
                                            const uint8_t *output_data)
{
  UNUSED_PARAMETER(output_len);
  UNUSED_PARAMETER(output_data);
  sl_si91x_ta_m4_handshake_parameters_t *handshake_request = NULL;
  sl_status_t status                                       = SL_STATUS_OK;

  SL_VERIFY_POINTER_OR_RETURN(input_data, SL_STATUS_INVALID_PARAMETER);

  handshake_request = malloc(sizeof(sl_si91x_ta_m4_handshake_parameters_t) + input_len);
  SL_VERIFY_POINTER_OR_RETURN(handshake_request, SL_STATUS_ALLOCATION_FAILED);
  memset(handshake_request, 0, sizeof(sl_si91x_ta_m4_handshake_parameters_t) + input_len);
  handshake_request->sub_cmd         = sub_cmd_type;
  handshake_request->input_data_size = input_len;
  memcpy(handshake_request->input_data, input_data, input_len);

  // Send the secure handshake command to the M4 core
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_TA_M4_COMMANDS,
                                        SI91X_COMMON_CMD,
                                        handshake_request,
                                        sizeof(sl_si91x_ta_m4_handshake_parameters_t) + input_len,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  free(handshake_request);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

// Perform a soft reset
static sl_status_t sl_si91x_soft_reset(void)
{
  sl_status_t status;
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_SOFT_RESET,
                                        SI91X_COMMON_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(30000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
#endif

sl_status_t sl_si91x_assert()
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  status =
    sl_si91x_driver_send_command(RSI_COMMON_REQ_ASSERT, SI91X_WLAN_CMD, NULL, 0, SL_SI91X_WAIT_FOR(30000), NULL, NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_get_ram_log(uint32_t address, uint32_t length)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  sl_status_t status      = SL_STATUS_OK;
  sl_si91x_ram_dump_t ram = { .address = address, .length = length };

  // Check for invalid length parameter
  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Send RAM log request
  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_GET_RAM_DUMP,
                                        SI91X_COMMON_CMD,
                                        &ram,
                                        sizeof(sl_si91x_ram_dump_t),
                                        SL_SI91X_WAIT_FOR(31000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_transmit_test_start(const sl_si91x_request_tx_test_info_t *tx_test_info)
{
  sl_status_t status = SL_STATUS_OK;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TX_TEST_MODE,
                                        SI91X_WLAN_CMD,
                                        tx_test_info,
                                        sizeof(sl_si91x_request_tx_test_info_t),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_transmit_test_stop(void)
{
  sl_status_t status                           = SL_STATUS_OK;
  sl_si91x_request_tx_test_info_t tx_test_info = { 0 };
  tx_test_info.enable                          = 0;
  // Send the transmit test stop command
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TX_TEST_MODE,
                                        SI91X_WLAN_CMD,
                                        &tx_test_info,
                                        sizeof(sl_si91x_request_tx_test_info_t),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_si91x_operation_mode_t get_opermode(void)
{
  return initialized_opermode;
}

sl_status_t sl_si91x_calibration_write(sl_si91x_calibration_write_t calib_write)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_CALIB_WRITE,
                                        SI91X_WLAN_CMD,
                                        &calib_write,
                                        sizeof(sl_si91x_calibration_write_t),
                                        SL_SI91X_WAIT_FOR(30000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_calibration_read(sl_si91x_calibration_read_t target, sl_si91x_calibration_read_t *calibration_read)
{
  sl_wifi_buffer_t *buffer = NULL;
  sl_status_t status       = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  SL_VERIFY_POINTER_OR_RETURN(calibration_read, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_CALIB_READ,
                                        SI91X_WLAN_CMD,
                                        &target,
                                        sizeof(sl_si91x_calibration_read_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(30100),
                                        NULL,
                                        &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
    return status;
  }

  const sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(calibration_read, packet->data, sizeof(sl_si91x_calibration_read_t));
  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_frequency_offset(const sl_si91x_freq_offset_t *frequency_calibration)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(frequency_calibration, SL_STATUS_NULL_POINTER);

  // Send the frequency offset calibration command to the SI91x WLAN module
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_FREQ_OFFSET,
                                        SI91X_WLAN_CMD,
                                        frequency_calibration,
                                        sizeof(sl_si91x_freq_offset_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(35000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_evm_offset(const sl_si91x_evm_offset_t *evm_offset)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(evm_offset, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EVM_OFFSET,
                                        SI91X_WLAN_CMD,
                                        evm_offset,
                                        sizeof(sl_si91x_evm_offset_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(35000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_evm_write(const sl_si91x_evm_write_t *evm_write)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(evm_write, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EVM_WRITE,
                                        SI91X_WLAN_CMD,
                                        evm_write,
                                        sizeof(sl_si91x_evm_write_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(35000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_dpd_calibration(const sl_si91x_get_dpd_calib_data_t *dpd_calib_data)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(dpd_calib_data, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_GET_DPD_DATA,
                                        SI91X_WLAN_CMD,
                                        dpd_calib_data,
                                        sizeof(sl_si91x_get_dpd_calib_data_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_si91x_efuse_read(const sl_si91x_efuse_read_t *efuse_read, uint8_t *efuse_read_buf)
{
  sl_wifi_buffer_t *buffer = NULL;
  sl_status_t status       = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  SL_VERIFY_POINTER_OR_RETURN(efuse_read, SL_STATUS_NULL_POINTER);
  SL_VERIFY_POINTER_OR_RETURN(efuse_read_buf, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EFUSE_READ,
                                        SI91X_WLAN_CMD,
                                        efuse_read,
                                        sizeof(sl_si91x_efuse_read_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(30100),
                                        NULL,
                                        &buffer);

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
    return status;
  }

  const sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(efuse_read_buf, packet->data, efuse_read->efuse_read_data_len);
  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_set_join_configuration(sl_wifi_interface_t interface, uint8_t join_feature_bitmap)
{
  // Determine whether the configuration is for the client or AP interface
  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    client_join_feature_bitmap = join_feature_bitmap;
  } else if (interface & SL_WIFI_AP_INTERFACE) {
    ap_join_feature_bitmap = join_feature_bitmap;
  } else {
    return SL_STATUS_FAIL;
  }
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_get_join_configuration(sl_wifi_interface_t interface, uint8_t *join_feature_bitmap)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(join_feature_bitmap);

  // Determine whether to retrieve the configuration for the client or AP interface
  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    *join_feature_bitmap = client_join_feature_bitmap;
  } else if (interface & SL_WIFI_AP_INTERFACE) {
    *join_feature_bitmap = ap_join_feature_bitmap;
  } else {
    return SL_STATUS_WIFI_UNKNOWN_INTERFACE;
  }

  return SL_STATUS_OK;
}

void sl_si91x_set_listen_interval(uint32_t listen_interval)
{
  client_listen_interval = listen_interval;
  return;
}

uint32_t sl_si91x_get_listen_interval(void)
{
  return client_listen_interval;
}

void sl_si91x_set_timeout(const sl_si91x_timeout_t *timeout_config)
{
  memcpy(&timeout_glbl, timeout_config, sizeof(sl_si91x_timeout_t));
  return;
}

sl_status_t sl_si91x_configure_timeout(sl_si91x_timeout_type_t timeout_type, uint16_t timeout_value)
{
  sl_status_t status                         = SL_STATUS_OK;
  sl_si91x_request_timeout_t timeout_request = { 0 };

  if (timeout_type > SL_SI91X_CHANNEL_PASSIVE_SCAN_TIMEOUT) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  timeout_request.timeout_bitmap = BIT(timeout_type);
  timeout_request.timeout_value  = timeout_value;
  status                         = sl_si91x_driver_send_command(RSI_WLAN_REQ_TIMEOUT,
                                        SI91X_WLAN_CMD,
                                        &timeout_request,
                                        sizeof(sl_si91x_request_timeout_t),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  return status;
}

uint16_t get_seq_ctrl(uint8_t is_qos)
{
  static uint16_t qos_pkt_count     = 0;
  static uint16_t non_qos_pkt_count = 0;

  if (qos_pkt_count > 4095)
    qos_pkt_count = 0;

  if (non_qos_pkt_count > 4095)
    non_qos_pkt_count = 0;

  return is_qos ? qos_pkt_count++ : non_qos_pkt_count++;
}

int32_t encapsulate_tx_data_packet(sl_wifi_transceiver_tx_data_control_t *control,
                                   uint8_t *pkt_data,
                                   uint32_t mac_hdr_len)
{
  uint16_t seq_ctrl = 0;
  uint16_t *frame_ctrl;
  uint32_t qos_ctrl_off = MAC80211_HDR_MIN_LEN;

  SL_VERIFY_POINTER_OR_RETURN(control, SL_STATUS_NULL_POINTER);

  if (IS_MAC_ZERO(control->addr1)) {
    return SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS;
  }

  /* Auto-rate is unsupported if Peer DS feature in MAC layer is disabled */
  if ((!IS_PEER_DS_SUPPORT_ENABLED(feature_bit_map)) && !IS_FIXED_DATA_RATE(control->ctrl_flags)) {
    return SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE;
  }

  /* Ignore QoS flag for bcast/mcast frames */
  if (IS_BCAST_MCAST_MAC(control->addr1[0])) {
    control->ctrl_flags &= ~TX_DATA_CTRL_FLAG_QOS_BIT;
  }

  if ((IS_QOS_PKT(control->ctrl_flags) && !IS_BCAST_MCAST_MAC(control->addr1[0])) && (control->priority > 3)) {
    return SL_STATUS_TRANSCEIVER_INVALID_QOS_PRIORITY;
  }

  if (IS_4ADDR(control->ctrl_flags)) {
    qos_ctrl_off += MAC80211_HDR_ADDR4_LEN;
  }

  memset(pkt_data, 0, mac_hdr_len);
  /* Add frame control (2 bytes) */
  frame_ctrl = (uint16_t *)&pkt_data[0];
  *frame_ctrl |= FC_TYPE_DATA;

  if (IS_4ADDR(control->ctrl_flags)) {
    *frame_ctrl |= FC_TO_DS;
    *frame_ctrl |= FC_FROM_DS;
  } else {
    *frame_ctrl |= IS_TODS(control->ctrl_flags) ? FC_TO_DS : 0;
    *frame_ctrl |= IS_FROMDS(control->ctrl_flags) ? FC_FROM_DS : 0;
  }

  /* Add Addr1, Addr2, Addr3 (18 bytes) */
  memcpy(&pkt_data[4], control->addr1, 6);
  memcpy(&pkt_data[10], control->addr2, 6);
  memcpy(&pkt_data[16], control->addr3, 6);

  if (!IS_PEER_DS_SUPPORT_ENABLED(feature_bit_map)) {
    seq_ctrl = (uint16_t)(get_seq_ctrl(IS_QOS_PKT(control->ctrl_flags)) << 4);
    memcpy(&pkt_data[22], &seq_ctrl, 2);
  }

  /* Add Addr4 optionally based on ctrl_flag (6 bytes) */
  if (IS_4ADDR(control->ctrl_flags)) {
    memcpy(&pkt_data[24], control->addr4, 6); /* sa */
  }
  /* Add QoS control optionally based on ctrl_flag (2 bytes) */
  if (IS_QOS_PKT(control->ctrl_flags) && !IS_BCAST_MCAST_MAC(control->addr1[0])) {
    *frame_ctrl |= FC_SUBTYPE_QOS_DATA;
    pkt_data[qos_ctrl_off] = WME_AC_TO_TID(control->priority);
  }

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_driver_send_transceiver_data(sl_wifi_transceiver_tx_data_control_t *control,
                                                  const uint8_t *payload,
                                                  uint16_t payload_len,
                                                  uint32_t wait_time)
{
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;
  sl_status_t status = SL_STATUS_OK;
  uint8_t *pkt_offset;
  uint8_t ext_desc_size;
  uint8_t *host_desc;
  uint32_t mac_hdr_len = MAC80211_HDR_MIN_LEN;

  if (IS_QOS_PKT(control->ctrl_flags) && !IS_BCAST_MCAST_MAC(control->addr1[0])) {
    mac_hdr_len += MAC80211_HDR_QOS_CTRL_LEN;
  }

  if (IS_4ADDR(control->ctrl_flags)) {
    mac_hdr_len += MAC80211_HDR_ADDR4_LEN;
  }

  ext_desc_size = TRANSCEIVER_TX_DATA_EXT_DESC_SIZE;

  // Allocate a data buffer with space for the data and metadata
  status = sl_si91x_allocate_data_buffer(&buffer,
                                         (void **)&packet,
                                         sizeof(sl_si91x_packet_t) + ext_desc_size + mac_hdr_len + payload_len,
                                         SL_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME);
  VERIFY_STATUS_AND_RETURN(status);

  // If the packet is not allocated successfully, return an allocation failed error
  if (packet == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  pkt_offset = packet->data + ext_desc_size;
  status     = encapsulate_tx_data_packet(control, pkt_offset, mac_hdr_len);
  if (status != SL_STATUS_OK) {
    sl_si91x_host_free_buffer(buffer);
    return status;
  }

  memcpy(pkt_offset + mac_hdr_len, payload, payload_len);

#ifdef TX_RX_FRAME_DUMP_BYTE_COUNT
  print_80211_packet(pkt_offset, mac_hdr_len + payload_len, TX_RX_FRAME_DUMP_BYTE_COUNT);
#endif

  // Clear the packet descriptor and copy the command data if available
  memset(packet->desc, 0, sizeof(packet->desc));

  // Fill length in first 2 host_desc bytes
  packet->length = (ext_desc_size + mac_hdr_len + payload_len) & 0xFFF;

  // Fill packet type
  host_desc = packet->desc;

  host_desc[2] = 0x01; //! Frame Type
  if (IS_CFM_TO_HOST_SET(control->ctrl_flags)) {
    host_desc[3] |= CONFIRM_REQUIRED_TO_HOST; //! This bit is used to set CONFIRM_REQUIRED_TO_HOST in firmware.
  }
  host_desc[4] = ext_desc_size;                     //! xtend_desc size
  host_desc[5] = (uint8_t)((mac_hdr_len + 3) & ~3); //! Mac_header length

  if (IS_BCAST_MCAST_MAC(control->addr1[0])) {
    host_desc[7] |= BCAST_INDICATION; //! Bcast_indication
    //! If auto-rate is enabled for bcast/mcast pkts, use 1 Mbps
    if (!IS_FIXED_DATA_RATE(control->ctrl_flags)) {
      host_desc[6] |= MAC_INFO_ENABLE; //! Fixed Rate
      host_desc[8] = SL_WIFI_DATA_RATE_1;
    }
  }

  if (IS_FIXED_DATA_RATE(control->ctrl_flags)) {
    host_desc[6] |= MAC_INFO_ENABLE; //! Fixed Rate
    host_desc[8] = (uint8_t)control->rate;
  }

  if (IS_QOS_PKT(control->ctrl_flags) && !IS_BCAST_MCAST_MAC(control->addr1[0])) {
    host_desc[13] |= QOS_ENABLE; // QOS ENABLE
  }

  host_desc[14] =
    (uint8_t)(((WME_AC_TO_TID(control->priority) & 0xf) << 4) | (WME_AC_TO_QNUM(control->priority) & 0xf));

  //! Initialize extended desc
  memcpy(&host_desc[16], &control->token, TRANSCEIVER_TX_DATA_EXT_DESC_SIZE);

  // Send command packet to the SI91x socket data queue and await a response
  return sl_si91x_driver_send_data_packet(buffer, wait_time);
}

sl_status_t sl_si91x_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags)
{
  static uint16_t boot_cmd = 0;
  uint16_t read_value      = 0;
  uint32_t offset          = 0;
  uint32_t retval          = 0;
  uint32_t boot_insn       = 0;
  uint32_t poll_resp       = 0;

  //! If it is a start of file set the boot cmd to pong valid
  if (flags & SL_SI91X_FW_START_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
  }

  //! check for invalid packet
  if ((fw_image_size % SL_SI91X_MIN_CHUNK_SIZE != 0) && (!(flags & SL_SI91X_FW_END_OF_FILE))) {
    return SL_STATUS_FAIL;
  }

  //! loop to execute multiple of 4K chunks
  while (offset < fw_image_size) {
    switch (boot_cmd) {
      case (RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID):
        boot_insn = RSI_PONG_WRITE;
        poll_resp = RSI_PING_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID;
        break;

      case (RSI_HOST_INTERACT_REG_VALID | RSI_PONG_VALID):
        boot_insn = RSI_PING_WRITE;
        poll_resp = RSI_PONG_AVAIL;
        boot_cmd  = RSI_HOST_INTERACT_REG_VALID | RSI_PING_VALID;
        break;

      default:
        return SL_STATUS_FAIL;
    }

    retval = sl_si91x_boot_instruction((uint8_t)boot_insn, (uint16_t *)(firmware_image + offset));
    VERIFY_STATUS_AND_RETURN(retval);

    while (1) {
      retval = sl_si91x_boot_instruction(RSI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

      if (read_value == (RSI_HOST_INTERACT_REG_VALID | poll_resp)) {
        break;
      }
    }
    offset += SL_SI91X_MIN_CHUNK_SIZE;
  }

  //! For last chunk set boot cmd as End of file reached
  if (flags & SL_SI91X_FW_END_OF_FILE) {
    boot_cmd = RSI_HOST_INTERACT_REG_VALID | RSI_EOF_REACHED;

    retval = sl_si91x_boot_instruction(RSI_REG_WRITE, &boot_cmd);
    VERIFY_STATUS_AND_RETURN(retval);

    //! check for successful firmware upgrade
    do {
      retval = sl_si91x_boot_instruction(RSI_REG_READ, &read_value);
      VERIFY_STATUS_AND_RETURN(retval);

    } while (read_value != (RSI_HOST_INTERACT_REG_VALID | RSI_FWUP_SUCCESSFUL));
  }
  return retval;
}

sl_status_t sl_si91x_set_fast_fw_up(void)
{
  uint32_t read_data = 0;
  sl_status_t retval = 0;
  retval             = sl_si91x_bus_read_memory(SL_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
  VERIFY_STATUS_AND_RETURN(retval);

  //disabling safe upgradation bit
  if (read_data & SL_SI91X_SAFE_UPGRADE) {
    read_data &= ~(SL_SI91X_SAFE_UPGRADE);
    retval = sl_si91x_bus_write_memory(SL_SI91X_SAFE_UPGRADE_ADDR, 4, (uint8_t *)&read_data);
    VERIFY_STATUS_AND_RETURN(retval);
  }
  return retval;
}

void sli_si91x_append_to_buffer_queue(sl_si91x_buffer_queue_t *queue, sl_wifi_buffer_t *buffer)
{
  CORE_irqState_t state = CORE_EnterAtomic();
  if (queue->tail == NULL) {
    assert(queue->head == NULL); // Both should be NULL at the same time
    queue->head = buffer;
    queue->tail = buffer;
  } else {
    queue->tail->node.node = &buffer->node;
    queue->tail            = buffer;
  }
  CORE_ExitAtomic(state);
}

sl_status_t sli_si91x_pop_from_buffer_queue(sl_si91x_buffer_queue_t *queue, sl_wifi_buffer_t **buffer)
{
  sl_status_t status    = SL_STATUS_EMPTY;
  CORE_irqState_t state = CORE_EnterAtomic();
  if (queue->head == NULL) {
    assert(queue->tail == NULL); // Both should be NULL at the same time
    *buffer = NULL;
    status  = SL_STATUS_EMPTY;
  } else {
    *buffer = queue->head;
    status  = SL_STATUS_OK;
    if (queue->head == queue->tail) {
      queue->head = NULL;
      queue->tail = NULL;
    } else {
      queue->head = (sl_wifi_buffer_t *)queue->head->node.node;
    }
  }
  CORE_ExitAtomic(state);
  return status;
}

sl_status_t sl_si91x_get_firmware_version(sl_si91x_firmware_version_t *version)
{
  sl_status_t status       = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  SL_WIFI_ARGS_CHECK_NULL_POINTER(version);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_FULL_FW_VERSION,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  if (packet == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (packet->length > 0) {
    const sl_wifi_firmware_version_t *response = (const sl_wifi_firmware_version_t *)packet->data;

    if (response == NULL) {
      return SL_STATUS_NULL_POINTER;
    }

    version->chip_id          = response->chip_id;
    version->rom_id           = response->rom_id;
    version->major            = response->major;
    version->minor            = response->minor;
    version->security_version = response->security_version;
    version->patch_num        = response->patch_num;
    version->customer_id      = response->customer_id;
    version->build_num        = (packet->data[7] | packet->data[8]);
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_si91x_get_firmware_size(void *buffer, uint32_t *fw_image_size)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(buffer);

  const sl_si91x_firmware_header_t *fw = (const sl_si91x_firmware_header_t *)buffer;

  *fw_image_size = (fw->image_size + sizeof(sl_si91x_firmware_header_t));

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_set_nwp_config_request(sl_si91x_nwp_configuration_t nwp_config)
{
  sl_status_t status = SL_STATUS_OK;

  if ((nwp_config.code & SL_SI91X_XO_CTUNE_FROM_HOST) || (nwp_config.code & SL_SI91X_ENABLE_NWP_WDT_FROM_HOST)
      || (nwp_config.code & SL_SI91X_DISABLE_NWP_WDT_FROM_HOST)) {
    status = sl_si91x_driver_send_command(RSI_COMMON_REQ_SET_CONFIG,
                                          SI91X_COMMON_CMD,
                                          &nwp_config,
                                          sizeof(sl_si91x_nwp_configuration_t),
                                          SL_SI91X_WAIT_FOR_RESPONSE(3000),
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
  } else
    return SL_STATUS_NOT_SUPPORTED;

  return status;
}

sl_status_t sl_si91x_get_nwp_config(const sl_si91x_nwp_get_configuration_t *nwp_config, uint8_t *response)
{
  sl_status_t status        = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer  = NULL;
  sl_si91x_packet_t *packet = NULL;

  if (nwp_config->sub_command_type == GET_OPN_BOARD_CONFIG) {
    status = sl_si91x_driver_send_command(RSI_COMMON_REQ_GET_CONFIG,
                                          SI91X_COMMON_CMD,
                                          nwp_config,
                                          sizeof(sl_si91x_nwp_get_configuration_t),
                                          SL_SI91X_WAIT_FOR_RESPONSE(3000),
                                          NULL,
                                          &buffer);
    if (status != SL_STATUS_OK) {
      if (buffer != NULL)
        sl_si91x_host_free_buffer(buffer);
    }
    VERIFY_STATUS_AND_RETURN(status);

    packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
    memcpy(response, packet->data, packet->length);
    sl_si91x_host_free_buffer(buffer);
  } else {
    return SL_STATUS_NOT_SUPPORTED;
  }
  return status;
}

sl_status_t sl_si91x_debug_log(sl_si91x_assertion_t *assertion)
{
  sl_status_t status                = SL_STATUS_OK;
  sl_si91x_debug_log_t debug_config = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if ((debug_config.assertion_type > SL_SI91X_ASSERTION_TYPE_ALL)
      || (debug_config.assertion_level > SL_SI91X_ASSERTION_LEVEL_MAX)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  debug_config.assertion_type  = assertion->assert_type;
  debug_config.assertion_level = assertion->assert_level;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_DEBUG_LOG,
                                        SI91X_COMMON_CMD,
                                        &debug_config,
                                        sizeof(sl_si91x_debug_log_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
