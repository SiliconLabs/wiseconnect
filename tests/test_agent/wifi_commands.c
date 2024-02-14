/** @file
 *
 * File Description
 *
 */
#include "console.h"
#include "sl_utility.h"
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_wifi_callback_framework.h"
#include "errno.h"
#include "sl_string.h"
#include "sl_ip_types.h"
#include "cmsis_os2.h"
#include "sl_utility.h"
#include "sl_net.h"
#include "cacert.pem.h"
#include "wifiuser.pem.h"
#include "aws_starfield_ca.pem.h"
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "sl_si91x_driver.h"
#include "printf.h"
#include "command_identifiers.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_types.h"
#include "rsi_utils.h"
#include "sl_si91x_socket_constants.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/******************************************************
 *                    Constants
 ******************************************************/

#define SOFT_AP_PSK          "123456789"
#define SOFT_AP_SSID         "SILICON_LABS_AP"
#define WIFI_CONNECT_TIMEOUT 1000
#define WIFI_SCAN_TIMEOUT    10000
#define TWT_SCAN_TIMEOUT     10000

#define WIFI_MODE_STA     0
#define WIFI_MODE_AP      1
#define WIFI_MODE_APSTA   2
#define WIFI_MODE_EAP     3
#define WIFI_MODE_DEFAULT 0xFF

#define WIFI_MODE_BLE_COEX 4 // BLE Coex mode

#define OPENSSL_SERVER 0
#define AWS_SERVER     1
#define AZURE_SERVER   2

#define ADV_SCAN_THRESHOLD           -40
#define ADV_RSSI_TOLERANCE_THRESHOLD 5
#define ADV_ACTIVE_SCAN_DURATION     15
#define ADV_PASSIVE_SCAN_DURATION    20
#define ADV_MULTIPROBE               0
#define ADV_INSTANT_SCAN             0
#define ADV_SCAN_PERIODICITY         10
#define WLAN_MODULE_STATES           11
#define WLAN_REASON_CODES            50

#define MAX_RECEIVE_STATS_COUNT 5

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static void show_twt_results(sl_si91x_twt_response_t *twt_result);
static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_si91x_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg);
static sl_status_t sl_wifi_module_state_stats_callback(sl_wifi_event_t event,
                                                       void *data,
                                                       uint32_t data_length,
                                                       void *arg);
/******************************************************
 *               Extern Variable
 ******************************************************/

/******************************************************
 *               Global Variable
 ******************************************************/

static uint8_t stats_count = 0;
static uint32_t wifi_connections, wifi_disconnections;
volatile bool scan_results_complete  = false;
volatile bool twt_results_complete   = false;
volatile sl_status_t callback_status = SL_STATUS_OK;

uint32_t g_sl_ssl_ciphers =
  (SSL_DEFAULT_CIPHERS | BIT_TLS_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_RSA_WITH_AES_128_CBC_SHA256
   | BIT_TLS_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CCM_8
   | BIT_TLS_RSA_WITH_AES_256_CCM_8);

uint32_t g_sl_ext_ssl_ciphers =
  (BIT_TLS13_AES_128_GCM_SHA256 | BIT_TLS13_AES_256_GCM_SHA384 | BIT_TLS13_CHACHA20_POLY1305_SHA256
   | BIT_TLS13_AES_128_CCM_SHA256 | BIT_TLS13_AES_128_CCM_8_SHA256);
/******************************************************
 *               Variable Definitions
 ******************************************************/

sl_wifi_device_configuration_t si91x_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                                       | SL_SI91X_FEAT_WPS_DISABLE
#else
                                       | SL_SI91X_FEAT_AGGREGATION
#endif
                                       ),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
#ifdef SLI_SI91X_ENABLE_IPV6
                      | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                      | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK | MEMORY_CONFIG
#ifdef SLI_SI917
                      | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                      ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                      | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

sl_wifi_twt_request_t default_twt_setup_configuration = {
  .twt_enable              = 1,
  .twt_flow_id             = 1,
  .wake_duration           = 0x60,
  .wake_duration_unit      = 0,
  .wake_duration_tol       = 0x60,
  .wake_int_exp            = 13,
  .wake_int_exp_tol        = 13,
  .wake_int_mantissa       = 0x1D4C,
  .wake_int_mantissa_tol   = 0x1D4C,
  .implicit_twt            = 1,
  .un_announced_twt        = 1,
  .triggered_twt           = 0,
  .twt_channel             = 0,
  .twt_protection          = 0,
  .restrict_tx_outside_tsp = 1,
  .twt_retry_limit         = 6,
  .twt_retry_interval      = 10,
  .req_type                = 1,
  .negotiation_type        = 0,
};

sl_wifi_twt_request_t default_twt_teardown_configuration = {
  .twt_enable  = 0,
  .twt_flow_id = 0xFF,
};
struct sl_si91x_bit_2_string {
  uint8_t bit;
  char *string;
};

/******************************************************
 *                Error Codes
 ******************************************************/

const struct sl_si91x_bit_2_string STATE[WLAN_MODULE_STATES] = {
  { 0x10, "Beacon Loss (Failover Roam)" },
  { 0x20, "De-authentication (AP induced Roam / Disconnect from supplicant" },
  { 0x50, "Current AP is best" },
  { 0x60, "Better AP found" },
  { 0x70, "No AP found" },
  { 0x80, "Associated" },
  { 0x90, "Unassociated" },
  { 0x01, "Authentication denial" },
  { 0x02, "Association denial" },
  { 0x03, "AP not present" },
  { 0x05, "WPA2 key exchange failed" }

};

const struct sl_si91x_bit_2_string REASONCODE[WLAN_REASON_CODES] = {
  { 0x01, "Authentication denial" },
  { 0x02, "Association denial" },
  { 0x10, "Beacon Loss (Failover Roam)" },
  { 0x20, "De-authentication (AP induced Roam/Deauth from supplicant)" },
  { 0x07, "PSK not configured" },
  { 0x05, "Roaming not enabled" },
  { 0x28, "TLS CA Cert not present" },
  { 0x29, "TLS PRIVATE key not present " },
  { 0x2A, "TLS Client Cert not present" },
  { 0x2B, "TLS no Cert present" },
  { 0x2C, "PEAP CA Cert not present" },
  { 0x2D, "Server Cert Invalid Key Type" },
  { 0x2E, "Server Intermediate CA Invalid Key Type" },
  { 0x2F, "Server Root CA Invalid Key Type" },
  { 0x30, "Client Cert Invalid Key Type" },
  { 0x31, "Client Root CA Invalid Key Type" },
  { 0x32, "FIPS Server Cert Invalid Length" },
  { 0x33, "FIPS Server Intermediate CA Invalid Length" },
  { 0x34, "FIPS Server Root CA Invalid Length" },
  { 0x35, "FIPS Client Cert Invlaid Length" },
  { 0x36, "FIPS Client Root CA Invalid Length" },
  { 0x37, "Server Cert Invalid Length" },
  { 0x38, "Server Intermediate CA Invalid Length" },
  { 0x39, "Server Root CA Invalid Length" },
  { 0x3A, "Client Cert Invalid Lenght" },
  { 0x3B, "Client Root CA Invalid Length" },
  { 0x3C, "Server Cert Invalid Sign Alg" },
  { 0x3D, "Server Intermediate CA Invalid Sign Alg" },
  { 0x3E, "Server Root CA Invalid Sign Length" },
  { 0x3F, "Client Cert Invalid Sign Alg" },
  { 0x40, "Client Root CA Invalid Sign Length" },
  { 0x41, "Server Intermediate CA not Present" },
  { 0x42, "Server Root CA Parse Error" },
  { 0x43, "Server Intermediate Root CA Parse Error" },
  { 0x44, "Sever Cert Parse Error" },
  { 0x45, "Client Cert Parse Error" },
  { 0x46, "Incorrect Private Key Password" },
  { 0x47, "EAP Failure Received" },
  { 0x48, "Client Cert Bad Date Error" },
  { 0x49, "Server Cert Bad Date Error" },
  { 0x4A, "Server Root CA Bad Date Error" },
  { 0x4B, "Client Root CA Bad Date Error" },
  { 0x4C, "Server Intermediate Root CA Bad Date Error" },
  { 0x4D, "Pem Header Error" },
  { 0x4E, "Pem Footer Error" },
  { 0x4F, "Client Intermediate CA Invalid Sign Length" },
  { 0x50, "Client Intermediate CA Invalid Length" },
  { 0x51, "FIPS Client Intermediate CA Invalid Length" },
  { 0x52, "Client Intermediate CA invalid Key Type" },
  { 0x53, "Pem Error" }
};

/******************************************************
 *               Function Definitions
 ******************************************************/
static const char *get_wifi_interface_name(sl_wifi_interface_t interface)
{
  switch (interface) {
    case SL_WIFI_CLIENT_2_4GHZ_INTERFACE:
      return "2.4GHz client";
    case SL_WIFI_AP_2_4GHZ_INTERFACE:
      return "2.4GHz access point";
    case SL_WIFI_CLIENT_5GHZ_INTERFACE:
      return "5GHz client";
    case SL_WIFI_AP_5GHZ_INTERFACE:
      return "5GHz access point";
    default:
      return "Unknown";
  }
}

sl_status_t sl_wifi_module_state_stats_callback(sl_wifi_event_t event,
                                                void *payload,
                                                uint32_t payload_length,
                                                void *arg)
{
  PRINT_EVENT_START(event, WIFI)
  UNUSED_PARAMETER(arg);
  int i = 0, j = 0;
  char *unknown       = "unknown";
  char *higher_nibble = unknown;
  char *lower_nibble  = unknown;
  char *reason_code   = unknown;
  uint8_t bssid_string[18];

  if (payload_length != sizeof(sl_si91x_module_state_stats_response_t)) {
    PRINT_EVENT_END();
    return SL_STATUS_FAIL;
  }
  sl_si91x_module_state_stats_response_t *state = (sl_si91x_module_state_stats_response_t *)payload;
  for (i = 0; i < WLAN_MODULE_STATES; i++) {
    //higher nibble information
    if (STATE[i].bit == (state->state_code & 0xF0)) {
      higher_nibble = STATE[i].string;
    }
    //lower nibble information
    if (STATE[i].bit == (state->state_code & 0x0F)) {
      lower_nibble = STATE[i].string;
    }
  }
  for (j = 0; j < WLAN_REASON_CODES; j++) {
    //!reason code info
    if (REASONCODE[j].bit == (state->reason_code & 0xFF)) {
      reason_code = REASONCODE[j].string;
    }
  }
  if (higher_nibble == unknown && lower_nibble == unknown && reason_code == unknown) {
    PRINT_EVENT_END();
    return SL_STATUS_INVALID_STATE;
  }
  rsi_6byte_dev_address_to_ascii((uint8_t *)bssid_string, state->bssid);

  if (*higher_nibble == 'A') {
    wifi_connections++;
    printf("\nWi-Fi connections %ld\n", wifi_connections);
  }
  if (*higher_nibble == 'U') {
    wifi_disconnections++;
    printf("\nWi-Fi disconnections %ld\n", wifi_disconnections);
    printf("\nReason code : %X\n", state->reason_code);
  }
  if (state->channel == 0 && state->rssi == 100) {

  } else if (state->channel == 0 && state->rssi != 100) {

  } else if (state->channel != 0 && state->rssi == 100) {

  } else {
  }
  PRINT_EVENT_END();
  return SL_STATUS_OK;
}

static const char *get_sl_wifi_protocol_name(sl_wifi_rate_protocol_t rate_protocol)
{
  switch (rate_protocol) {
    case SL_WIFI_RATE_PROTOCOL_B_ONLY:
      return "802.11b";
    case SL_WIFI_RATE_PROTOCOL_G_ONLY:
      return "802.11g";
    case SL_WIFI_RATE_PROTOCOL_N_ONLY:
      return "802.11n";
    case SL_WIFI_RATE_PROTOCOL_AC_ONLY:
      return "802.11ac";
    case SL_WIFI_RATE_PROTOCOL_AX_ONLY:
      return "802.11ax";
    case SL_WIFI_RATE_PROTOCOL_AUTO:
      return "AUTO";
    default:
      return "Unknown";
  }
}

static const char *get_sl_wifi_rate_name(sl_wifi_rate_t mask)
{
  switch (mask) {
    case SL_WIFI_AUTO_RATE:
      return "AUTO";
    case SL_WIFI_RATE_11B_1:
      return "1Mbps";
    case SL_WIFI_RATE_11B_2:
      return "2Mbps";
    case SL_WIFI_RATE_11B_5_5:
      return "5.5Mbps";
    case SL_WIFI_RATE_11B_11:
      return "11Mbps";
    case SL_WIFI_RATE_11G_6:
      return "6Mbps";
    case SL_WIFI_RATE_11G_9:
      return "9Mbps";
    case SL_WIFI_RATE_11G_12:
      return "12Mbps";
    case SL_WIFI_RATE_11G_18:
      return "18Mbps";
    case SL_WIFI_RATE_11G_24:
      return "24Mbps";
    case SL_WIFI_RATE_11G_36:
      return "36Mbps";
    case SL_WIFI_RATE_11G_48:
      return "48Mbps";
    case SL_WIFI_RATE_11G_54:
      return "54Mbps";
    case SL_WIFI_RATE_11N_MCS0:
      return "7.2Mbps";
    case SL_WIFI_RATE_11N_MCS1:
      return "14.4Mbps";
    case SL_WIFI_RATE_11N_MCS2:
      return "21.7Mbps";
    case SL_WIFI_RATE_11N_MCS3:
      return "28.9Mbps";
    case SL_WIFI_RATE_11N_MCS4:
      return "43.3Mbps";
    case SL_WIFI_RATE_11N_MCS5:
      return "57.8Mbps";
    case SL_WIFI_RATE_11N_MCS6:
      return "65Mbps";
    case SL_WIFI_RATE_11N_MCS7:
      return "72.2Mbps";
    case SL_WIFI_RATE_11AX_MCS0:
      return "8.6Mbps";
    case SL_WIFI_RATE_11AX_MCS1:
      return "17.2Mbps";
    case SL_WIFI_RATE_11AX_MCS2:
      return "25.8Mbps";
    case SL_WIFI_RATE_11AX_MCS3:
      return "34.4Mbps";
    case SL_WIFI_RATE_11AX_MCS4:
      return "51.6Mbps";
    case SL_WIFI_RATE_11AX_MCS5:
      return "68.8Mbps";
    case SL_WIFI_RATE_11AX_MCS6:
      return "77.4Mbps";
    case SL_WIFI_RATE_11AX_MCS7:
      return "86Mbps";
    default:
      return "Unknown";
  }
}

static inline const char *get_performance_profile_name(sl_si91x_performance_profile_t profile)
{
  switch (profile) {
    case HIGH_PERFORMANCE:
      return "High Performance";
    case ASSOCIATED_POWER_SAVE:
      return "Associated power save";
    case ASSOCIATED_POWER_SAVE_LOW_LATENCY:
      return "Associated power save low latency";
    case STANDBY_POWER_SAVE:
      return "Standby low power";
    default:
      return "Unknown";
  }
}

static sl_net_credential_id_t combine_credential_index(sl_net_credential_id_t cert_identifier, uint8_t cert_index)
{
  sl_net_credential_id_t group_id = (SL_NET_CREDENTIAL_GROUP_MASK & cert_identifier);

  switch ((group_id)) {
    case SL_NET_TLS_CLIENT_CREDENTIAL_START:
      return SL_NET_TLS_CLIENT_CREDENTIAL_ID(cert_index);
      break;
    case SL_NET_TLS_SERVER_CREDENTIAL_START:
      return SL_NET_TLS_SERVER_CREDENTIAL_ID(cert_index);
      break;
    case SL_NET_MQTT_SERVER_CREDENTIAL_START:
      return SL_NET_MQTT_SERVER_CREDENTIAL_ID(cert_index);
      break;
    case SL_NET_MQTT_CLIENT_CREDENTIAL_START:
      return SL_NET_MQTT_CLIENT_CREDENTIAL_ID(cert_index);
      break;
    case SL_NET_HTTP_SERVER_CREDENTIAL_START:
      return SL_NET_HTTP_SERVER_CREDENTIAL_ID(cert_index);
      break;
    case SL_NET_HTTP_CLIENT_CREDENTIAL_START:
      return SL_NET_HTTP_CLIENT_CREDENTIAL_ID(cert_index);
      break;

    default:
      break;
  }

  return cert_identifier;
}

sl_status_t wifi_init_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_wifi_device_configuration_t config;

  const int mode = GET_OPTIONAL_COMMAND_ARG(arguments, 0, WIFI_MODE_DEFAULT, const int);

  switch (mode) {
    case WIFI_MODE_STA:
      config = sl_wifi_default_client_configuration;
      break;
    case WIFI_MODE_AP:
      config = sl_wifi_default_ap_configuration;
      break;
    case WIFI_MODE_APSTA:
      config = sl_wifi_default_concurrent_configuration;
      break;
    case WIFI_MODE_EAP:
      config = sl_wifi_default_enterprise_client_configuration;
      break;
    case WIFI_MODE_DEFAULT:
      config = si91x_init_configuration;
      break;
    default:
      printf("Selected Wi-Fi mode is not supported. Try 'help'");
      return SL_STATUS_WIFI_UNKNOWN_INTERFACE;
  }

  config.band = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_SI91X_WIFI_BAND_2_4GHZ, const int);

  status = sl_wifi_init(&config, NULL, sl_wifi_default_event_handler);
  VERIFY_STATUS_AND_RETURN(status);
  sl_wifi_set_stats_callback(sl_wifi_module_state_stats_callback, NULL);

  switch (config.boot_config.oper_mode) {
    case SL_SI91X_CLIENT_MODE:
      printf("Started client mode");
      break;
    case SL_SI91X_ACCESS_POINT_MODE:
      printf("Started AP mode");
      break;
    case SL_SI91X_CONCURRENT_MODE:
      printf("Started STA + AP mode");
      break;
    case SL_SI91X_ENTERPRISE_CLIENT_MODE:
      printf("Started enterprise client mode");
      break;
  }

  switch (config.boot_config.coex_mode) {
    case SL_SI91X_WLAN_BLE_MODE:
      printf(" with BLE");
      break;
    case SL_SI91X_WLAN_BLUETOOTH_MODE:
      printf(" with Bluetooth");
      break;
    case SL_SI91X_WLAN_ONLY_MODE:
    case SL_SI91X_WLAN_MODE:
    case SL_SI91X_BLUETOOTH_MODE:
    case SL_SI91X_DUAL_MODE:
    case SL_SI91X_WLAN_DUAL_MODE:
    case SL_SI91X_BLE_MODE:
      break;
  }

  return status;
}

sl_status_t wifi_set_antenna_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  sl_wifi_antenna_t antenna = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_WIFI_ANTENNA_INTERNAL, sl_wifi_antenna_t);
  return sl_wifi_set_antenna(interface, antenna);
}

static sl_status_t show_scan_results(sl_wifi_scan_result_t *scan_result)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(scan_result);

  printf("%lu scan results:\n", scan_result->scan_count);
  for (uint32_t a = 0; a < scan_result->scan_count; ++a) {
    printf("%s,", scan_result->scan_info[a].ssid);
    printf("%d,", scan_result->scan_info[a].security_mode);
    printf("%d,", scan_result->scan_info[a].network_type);
    printf("%02x:%02x:%02x:%02x:%02x:%02x,",
           scan_result->scan_info[a].bssid[0],
           scan_result->scan_info[a].bssid[1],
           scan_result->scan_info[a].bssid[2],
           scan_result->scan_info[a].bssid[3],
           scan_result->scan_info[a].bssid[4],
           scan_result->scan_info[a].bssid[5]);
    printf("%d,", scan_result->scan_info[a].rf_channel);
    printf("-%u\n", scan_result->scan_info[a].rssi_val);
  }

  return SL_STATUS_OK;
}

sl_status_t scan_callback_handler(sl_wifi_event_t event,
                                  sl_wifi_scan_result_t *result,
                                  uint32_t result_length,
                                  void *arg)
{
  UNUSED_PARAMETER(arg);
  PRINT_EVENT_START(event, WIFI)
  sl_status_t status = SL_STATUS_OK;
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)result;
    printf("F: %ld\n", callback_status);
    status = SL_STATUS_FAIL;
  } else if (result_length != 0) {
    callback_status = show_scan_results(result);
  }
  PRINT_EVENT_END();
  scan_results_complete = true;
  return status;
}

sl_status_t wifi_stats_receive_handler(sl_wifi_event_t event,
                                       sl_si91x_per_stats_rsp_t *result,
                                       uint32_t result_length,
                                       void *arg)
{
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)result;
    return SL_STATUS_FAIL;
  }

  printf("%s: WIFI STATS Recieved packet# %d\n", __func__, stats_count);
  printf("stats : crc_pass %x, crc_fail %x, cal_rssi :%x\n", result->crc_pass, result->crc_fail, result->cal_rssi);
  stats_count++;

  callback_status = SL_STATUS_OK;
  return SL_STATUS_OK;
}

sl_status_t wifi_get_signal_strength_command_handler(console_args_t *arguments)
{
  int32_t signal_strength = 0;
  sl_status_t status      = SL_STATUS_OK;
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_get_signal_strength(interface, &signal_strength);
  VERIFY_STATUS_AND_RETURN(status);

  printf("%ld", signal_strength);
  return status;
}

sl_status_t wifi_scan_command_handler(console_args_t *arguments)
{
  sl_status_t status = SL_STATUS_OK;
  sl_wifi_ssid_t optional_ssid_arg;
  sl_wifi_scan_configuration_t wifi_scan_configuration = { 0 };
  callback_status                                      = status;
  scan_results_complete                                = false;
  const char *ssid                                     = GET_OPTIONAL_COMMAND_ARG(arguments, 0, NULL, const char *);
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  bool advanced_scan_enabled = GET_OPTIONAL_COMMAND_ARG(arguments, 2, false, const bool);
  uint16_t channel           = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, const uint16_t);
  bool synchronous           = GET_OPTIONAL_COMMAND_ARG(arguments, 4, false, const bool);

  if (ssid != NULL) {
    optional_ssid_arg.length = (uint8_t)strlen(ssid);
    memcpy(optional_ssid_arg.value, ssid, optional_ssid_arg.length);
  }

  wifi_scan_configuration = default_wifi_scan_configuration;

  if (advanced_scan_enabled) {
    wifi_scan_configuration.type = SL_WIFI_SCAN_TYPE_ADV_SCAN;

    wifi_scan_configuration.periodic_scan_interval =
      GET_OPTIONAL_COMMAND_ARG(arguments, 5, ADV_SCAN_PERIODICITY, uint32_t);
  }
  if (interface & SL_WIFI_2_4GHZ_INTERFACE) {
    if (channel) {
      wifi_scan_configuration.channel_bitmap_2g4 = (1 << (channel - 1));
    } else {
      uint32_t scan_bitmap_2_4                   = GET_OPTIONAL_COMMAND_ARG(arguments, 6, 0, uint32_t);
      wifi_scan_configuration.channel_bitmap_2g4 = scan_bitmap_2_4;
    }
  }

  if (!synchronous) {
    printf("registering sl_wifi_set_scan_callback");
    sl_wifi_set_scan_callback(scan_callback_handler, NULL);
  }

  status = sl_wifi_start_scan(interface, (ssid != NULL) ? &optional_ssid_arg : NULL, &wifi_scan_configuration);

  if (synchronous) {
    sl_wifi_scan_result_t *scan_results = NULL;
    status                              = sl_wifi_wait_for_scan_results(&scan_results, SL_WIFI_MAX_SCANNED_AP);
    if (status != SL_STATUS_OK || scan_results == NULL) {
      return SL_STATUS_FAIL;
    }

    show_scan_results(scan_results);
    free(scan_results);

    return SL_STATUS_OK;
  }

  if (SL_STATUS_IN_PROGRESS == status) {
    printf("Scanning...\r\n");
    const uint32_t start = osKernelGetTickCount();

    while (!scan_results_complete && (osKernelGetTickCount() - start) <= WIFI_SCAN_TIMEOUT) {
      osDelay(100);
    }

    status = scan_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }

  return status;
}

sl_status_t join_callback_handler(sl_wifi_event_t event, char *result, uint32_t result_length, void *arg)
{
  UNUSED_PARAMETER(arg);
  PRINT_EVENT_START(event, WIFI)
  sl_status_t status = SL_STATUS_OK;
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)result;
    printf("F: %ld\n", callback_status);
    status = SL_STATUS_FAIL;
  } else {
    callback_status = SL_STATUS_OK;
    printf("%c: Join Event received with %lu bytes payload\n", *result, result_length);
  }
  PRINT_EVENT_END();
  return status;
}
sl_status_t wifi_set_tx_power_command_handler(console_args_t *arguments)
{
  sl_wifi_max_tx_power_t max_tx_power = { 0 };
  max_tx_power.scan_tx_power          = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0x1F, uint8_t);
  max_tx_power.join_tx_power          = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0x1F, uint8_t);

  return sl_wifi_set_max_tx_power(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, max_tx_power);
}

sl_status_t wifi_get_tx_power_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_max_tx_power_t max_tx_power = { 0 };
  sl_status_t status                  = sl_wifi_get_max_tx_power(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &max_tx_power);

  if (status == SL_STATUS_OK) {
    printf("Scan Power value: %d", max_tx_power.scan_tx_power);
    printf("Join Power value: %d", max_tx_power.join_tx_power);
  }
  return status;
}

sl_status_t wifi_connect_command_handler(console_args_t *arguments)
{
  sl_status_t status                = SL_STATUS_OK;
  sl_wifi_credential_id_t id        = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
  sl_wifi_client_configuration_t ap = { 0 };
  sl_wifi_credential_t cred         = { 0 };
  char *ssid                        = (char *)arguments->arg[0];
  char *password                    = (char *)arguments->arg[1];
  sl_wifi_security_t secType        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_WPA2, sl_wifi_security_t);
  const char *userName              = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, const char *);
  sl_wifi_encryption_t encType = GET_OPTIONAL_COMMAND_ARG(arguments, 4, SL_WIFI_CCMP_ENCRYPTION, sl_wifi_encryption_t);
  uint32_t timeout_ms          = GET_OPTIONAL_COMMAND_ARG(arguments, 5, 0, uint32_t);
  ap.ssid.length               = GET_OPTIONAL_COMMAND_ARG(arguments, 6, strlen(ssid), uint8_t);
  ap.channel.channel           = GET_OPTIONAL_COMMAND_ARG(arguments, 7, 0, uint8_t);
  ap.channel.band              = GET_OPTIONAL_COMMAND_ARG(arguments, 8, SL_WIFI_AUTO_BAND, sl_wifi_band_t);
  ap.channel.bandwidth         = GET_OPTIONAL_COMMAND_ARG(arguments, 9, SL_WIFI_BANDWIDTH_10MHz, sl_wifi_bandwidth_t);
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 10, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  bool pairwise_master_key = GET_OPTIONAL_COMMAND_ARG(arguments, 11, false, bool);

  SL_DEBUG_LOG("ssid=%s\n", ssid);
  SL_DEBUG_LOG("secType=%u\n", secType);
  SL_DEBUG_LOG("encType=%u\n", encType);
  SL_DEBUG_LOG("userName=%s\n", userName);
  SL_DEBUG_LOG("pwd=%s\n", password);

  if (timeout_ms == 0) {
    printf("registering sl_wifi_set_join_callback");
    sl_wifi_set_join_callback(join_callback_handler, NULL);
  }

  if ((SL_WIFI_WPA_ENTERPRISE == secType) || ((SL_WIFI_WPA2_ENTERPRISE == secType))) {
    if (NULL == userName) {
      return SL_STATUS_INVALID_CREDENTIALS;
    }

    cred.type = SL_WIFI_EAP_CREDENTIAL;
    memcpy(cred.eap.username, userName, strlen(userName));
    memcpy(cred.eap.password, password, strlen(password));
    id = SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID;
  } else {
    cred.type = SL_WIFI_PSK_CREDENTIAL;
    memcpy(cred.psk.value, password, strlen(password));

    status = sl_net_set_credential(id, SL_NET_WIFI_PSK, password, strlen(password));
    if (SL_STATUS_OK != status) {
      return status;
    }
  }

  SL_DEBUG_LOG("Credentials set\n");
  memcpy(ap.ssid.value, ssid, ap.ssid.length);
  ap.security      = secType;
  ap.encryption    = encType;
  ap.credential_id = id;

  if (pairwise_master_key) {
    uint8_t pairwise_master_key[32] = { 0 };
    status = sl_wifi_get_pairwise_master_key(interface, 3, &ap.ssid, password, pairwise_master_key);
    if (status != SL_STATUS_OK) {
      return status;
    }
    status = sl_net_set_credential(SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID,
                                   SL_NET_WIFI_PMK,
                                   pairwise_master_key,
                                   sizeof(sl_wifi_pmk_credential_t));
    if (status != SL_STATUS_OK) {
      return status;
    }
    printf("\nThe PMK generated is: ");
    for (uint8_t index = 0; index < 32; index++) {
      printf("%x", pairwise_master_key[index]);
    }
  }

  SL_DEBUG_LOG("Connecting\n");
  status = sl_wifi_connect(interface, &ap, timeout_ms);
  if (SL_STATUS_IN_PROGRESS == status) {
    callback_status = SL_STATUS_IN_PROGRESS;
    while (callback_status == SL_STATUS_IN_PROGRESS) {
      osDelay(100);
    }
    status = callback_status;
  }

  return status;
}

sl_status_t wifi_disconnect_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);

  return sl_wifi_disconnect(interface);
}

sl_status_t wifi_get_firmware_version_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_firmware_version_t version = { 0 };

  sl_status_t status = sl_wifi_get_firmware_version(&version);
  VERIFY_STATUS_AND_RETURN(status);

  printf("Firmware version is: %x%x.%d.%d.%d.%d.%d.%d ",
         version.chip_id,
         version.rom_id,
         version.major,
         version.minor,
         version.security_version,
         version.patch_num,
         version.customer_id,
         version.build_num);

  return status;
}

sl_status_t wifi_get_statistics_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status              = SL_STATUS_OK;
  sl_wifi_statistics_t wifi_stats = { 0 };

  status = sl_wifi_get_statistics(SL_WIFI_CLIENT_INTERFACE, &wifi_stats);
  VERIFY_STATUS_AND_RETURN(status);

  printf("beacon_lost_count: %lu\r\n", wifi_stats.beacon_lost_count);
  printf("beacon_rx_count: %lu\r\n", wifi_stats.beacon_rx_count);
  printf("mcast_rx_count: %lu\r\n", wifi_stats.mcast_rx_count);
  printf("mcast_tx_count: %lu\r\n", wifi_stats.mcast_tx_count);
  printf("overrun_count: %lu\r\n", wifi_stats.overrun_count);
  printf("ucast_rx_count: %lu\r\n", wifi_stats.ucast_rx_count);
  printf("ucast_tx_count: %lu", wifi_stats.ucast_tx_count);
  return status;
}

sl_status_t wifi_start_statistic_report_command_handler(console_args_t *arguments)
{
  sl_status_t status            = SL_STATUS_OK;
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);
  sl_wifi_channel_t channel     = { 0 };
  channel.channel               = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 1, const uint16_t);
  stats_count                   = 0;
  sl_wifi_set_stats_callback((sl_wifi_stats_callback_t)wifi_stats_receive_handler, NULL);

  status = sl_wifi_start_statistic_report(interface, channel);
  if (SL_STATUS_IN_PROGRESS == status) {
    callback_status = SL_STATUS_IN_PROGRESS;
    printf("Receive Statistics...\r\n");
    while (stats_count <= MAX_RECEIVE_STATS_COUNT) {
      osDelay(100);
      if (stats_count == MAX_RECEIVE_STATS_COUNT && callback_status != SL_STATUS_IN_PROGRESS) {
        printf("%s: Stop Statistics Report\n", __func__);
        sl_wifi_stop_statistic_report(SL_WIFI_CLIENT_INTERFACE);
        return SL_STATUS_OK;
      }
    }
    status = callback_status;
  }
  return status;
}

sl_status_t wifi_stop_statistic_report_command_handler(console_args_t *arguments)
{
  sl_status_t status            = SL_STATUS_OK;
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_stop_statistic_report(interface);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t wifi_get_mac_address_command_handler(console_args_t *arguments)
{
  sl_status_t status            = SL_STATUS_OK;
  sl_mac_address_t mac_addr     = { 0 };
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_get_mac_address(interface, &mac_addr);
  VERIFY_STATUS_AND_RETURN(status);

  printf("%02x:%02x:%02x:%02x:%02x:%02x",
         mac_addr.octet[0],
         mac_addr.octet[1],
         mac_addr.octet[2],
         mac_addr.octet[3],
         mac_addr.octet[4],
         mac_addr.octet[5]);
  return status;
}

sl_status_t wifi_set_channel_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_wifi_interface_t interface  = (sl_wifi_interface_t)GET_COMMAND_ARG(arguments, 0);
  uint16_t channel               = (uint16_t)GET_COMMAND_ARG(arguments, 1);
  sl_wifi_channel_t channel_info = { .channel = channel };

  status = sl_wifi_set_channel(interface, channel_info);
  VERIFY_STATUS_AND_RETURN(status);

  printf("Channel is set to: %d\r\n", channel);
  return status;
}

sl_status_t wifi_get_channel_command_handler(console_args_t *arguments)
{
  sl_status_t status             = SL_STATUS_OK;
  sl_wifi_interface_t interface  = (sl_wifi_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_wifi_channel_t channel_info = { 0 };
  char *band_string;

  status = sl_wifi_get_channel(interface, &channel_info);
  VERIFY_STATUS_AND_RETURN(status);

  switch (channel_info.band) {
    case SL_WIFI_BAND_2_4GHZ:
      band_string = "2.4 Ghz";
      break;
    case SL_WIFI_BAND_5GHZ:
      band_string = "5 GHz";
      break;
    default:
      band_string = "Auto Band";
      break;
  }
  printf("The operating channel: %d and band: %s\r\n", channel_info.channel, band_string);
  return status;
}

sl_status_t wifi_disconnect_ap_client_command_handler(console_args_t *arguments)
{
  sl_status_t status;

  sl_mac_address_t *mac_address = (sl_mac_address_t *)GET_COMMAND_ARG(arguments, 0);
  sl_wifi_deauth_reason_t deauth_reason =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_WIFI_DEAUTH_UNSPECIFIED, sl_wifi_deauth_reason_t);

  status = sl_wifi_disconnect_ap_client(SL_WIFI_AP_INTERFACE, mac_address, deauth_reason);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t wifi_get_ap_client_info_command_handler(console_args_t *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  sl_wifi_client_info_response_t client_info = { 0 };

  status = sl_wifi_get_ap_client_info(SL_WIFI_AP_INTERFACE, &client_info);

  if (client_info.client_count <= 0) {
    printf("No clients!!");
    return status;
  }

  for (uint16_t station_info_index = 0; station_info_index < client_info.client_count; station_info_index++) {
    sl_wifi_client_info_t *station_info = &client_info.client_info[station_info_index];
    printf("%d) MAC Address is %x:%x:%x:%x:%x:%x",
           station_info_index + 1,
           station_info->mac_adddress.octet[0],
           station_info->mac_adddress.octet[1],
           station_info->mac_adddress.octet[2],
           station_info->mac_adddress.octet[3],
           station_info->mac_adddress.octet[4],
           station_info->mac_adddress.octet[5]);

    print_sl_ip_address(&station_info->ip_address);
    printf("\r\n");
  }

  return status;
}

sl_status_t wifi_set_performance_profile_command_handler(console_args_t *argument)
{
  sl_wifi_performance_profile_t performance_profile = { 0 };
  performance_profile.profile                       = GET_COMMAND_ARG(argument, 0);
  performance_profile.dtim_aligned_type             = GET_COMMAND_ARG(argument, 1);
  performance_profile.num_of_dtim_skip              = GET_COMMAND_ARG(argument, 2);
  performance_profile.listen_interval               = GET_COMMAND_ARG(argument, 3);
  performance_profile.monitor_interval              = GET_COMMAND_ARG(argument, 4);

  return sl_wifi_set_performance_profile(&performance_profile);
}

sl_status_t wifi_get_performance_profile_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status;
  sl_wifi_performance_profile_t performance_profile = { 0 };

  status = sl_wifi_get_performance_profile(&performance_profile);
  VERIFY_STATUS_AND_RETURN(status);

  printf("%s", get_performance_profile_name(performance_profile.profile));

  return status;
}

sl_status_t wifi_deinit_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  return sl_wifi_deinit();
}

sl_status_t wifi_start_ap_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_wifi_ap_configuration_t ap_configuration = { 0 };
  char *ssid = GET_OPTIONAL_COMMAND_ARG(arguments, 0, default_wifi_ap_configuration.ssid.value, char *);

  memcpy(ap_configuration.ssid.value, ssid, strlen(ssid));
  ap_configuration.ssid.length = (uint8_t)strlen((char *)ap_configuration.ssid.value);

  ap_configuration.security =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, default_wifi_ap_configuration.security, sl_wifi_security_t);
  ap_configuration.encryption =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, default_wifi_ap_configuration.encryption, sl_wifi_encryption_t);

  ap_configuration.channel.channel =
    GET_OPTIONAL_COMMAND_ARG(arguments, 3, default_wifi_ap_configuration.channel.channel, uint16_t);
  ap_configuration.channel.band =
    GET_OPTIONAL_COMMAND_ARG(arguments, 4, default_wifi_ap_configuration.channel.band, sl_wifi_band_t);
  ap_configuration.channel.bandwidth =
    GET_OPTIONAL_COMMAND_ARG(arguments, 5, default_wifi_ap_configuration.channel.bandwidth, sl_wifi_bandwidth_t);

  ap_configuration.rate_protocol =
    GET_OPTIONAL_COMMAND_ARG(arguments, 6, default_wifi_ap_configuration.rate_protocol, sl_wifi_rate_protocol_t);
  ap_configuration.options =
    GET_OPTIONAL_COMMAND_ARG(arguments, 7, default_wifi_ap_configuration.options, sl_wifi_ap_flag_t);
  ap_configuration.credential_id =
    GET_OPTIONAL_COMMAND_ARG(arguments, 8, default_wifi_ap_configuration.credential_id, sl_wifi_credential_id_t);
  ap_configuration.client_idle_timeout =
    GET_OPTIONAL_COMMAND_ARG(arguments, 9, default_wifi_ap_configuration.client_idle_timeout, uint32_t);
  ap_configuration.dtim_beacon_count =
    GET_OPTIONAL_COMMAND_ARG(arguments, 10, default_wifi_ap_configuration.dtim_beacon_count, uint16_t);
  ap_configuration.maximum_clients =
    GET_OPTIONAL_COMMAND_ARG(arguments, 11, default_wifi_ap_configuration.maximum_clients, uint8_t);

  ap_configuration.keepalive_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 12, default_wifi_ap_configuration.keepalive_type, uint8_t);
  ap_configuration.beacon_interval =
    GET_OPTIONAL_COMMAND_ARG(arguments, 13, default_wifi_ap_configuration.beacon_interval, uint16_t);
  const char *psk = GET_OPTIONAL_COMMAND_ARG(arguments, 14, SOFT_AP_PSK, const char *);

  ap_configuration.credential_id = SL_NET_DEFAULT_WIFI_AP_CREDENTIAL_ID;

  status = sl_net_set_credential(ap_configuration.credential_id, SL_NET_WIFI_PSK, psk, strlen(psk));
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_start_ap(SL_WIFI_AP_2_4GHZ_INTERFACE, &ap_configuration);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t wifi_clear_tls_certs_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  // Currently not supported
  return SL_STATUS_FAIL;
}

sl_status_t wifi_clear_ca_store_handler(console_args_t *arguments)
{
  sl_net_credential_type_t cert_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_NET_SIGNING_CERTIFICATE, sl_net_credential_type_t);
  sl_net_credential_id_t cert_identifier =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_TLS_SERVER_CREDENTIAL_START, sl_net_credential_id_t);

  uint8_t cert_index                      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, uint8_t);
  sl_net_credential_id_t combined_cert_id = combine_credential_index(cert_identifier, cert_index);

  return sl_net_delete_credential(combined_cert_id, cert_type);
}

sl_status_t wifi_load_certificate_handler(console_args_t *arguments)
{
  uint8_t *buffer = (uint8_t *)GET_COMMAND_ARG(arguments, 0);
  sl_net_credential_type_t cert_type =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_NET_SIGNING_CERTIFICATE, sl_net_credential_type_t);
  sl_net_credential_id_t cert_identifier =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_NET_TLS_SERVER_CREDENTIAL_START, sl_net_credential_id_t);
  uint8_t cert_index                      = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 0, uint8_t);
  sl_net_credential_id_t combined_cert_id = combine_credential_index(cert_identifier, cert_index);

  return sl_net_set_credential(combined_cert_id, cert_type, buffer, strlen((const char *)buffer));
}

sl_status_t wifi_is_interface_up_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface = (sl_wifi_interface_t)GET_COMMAND_ARG(arguments, 0);

  if (sl_wifi_is_interface_up(interface)) {
    printf("Yes");
    return SL_STATUS_OK;
  } else {
    printf("No");
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
}

sl_status_t wifi_get_default_interface_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_interface_t interface = sl_wifi_get_default_interface();
  if (interface == SL_WIFI_INVALID_INTERFACE) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  printf("%s", get_wifi_interface_name(interface));

  return SL_STATUS_OK;
}

sl_status_t wifi_set_default_interface_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface = (sl_wifi_interface_t)GET_COMMAND_ARG(arguments, 0);

  sl_wifi_set_default_interface(interface);

  return SL_STATUS_OK;
}

sl_status_t wifi_set_transmit_rate_command_handler(console_args_t *arguments)
{
  sl_wifi_rate_protocol_t rate_protocol = (sl_wifi_rate_protocol_t)GET_COMMAND_ARG(arguments, 0);
  sl_wifi_rate_t mask                   = (sl_wifi_rate_t)GET_COMMAND_ARG(arguments, 1);
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);

  sl_status_t status = sl_wifi_set_transmit_rate(interface, rate_protocol, mask);

  if (status != SL_STATUS_OK) {
    printf("\n Failed to configure transmit rate!\n");
  } else {
    printf("\n Transmit rate configured successfully!\n");
  }

  return status;
}

sl_status_t wifi_set_roam_configuration_command_handler(console_args_t *arguments)
{
  sl_status_t status = SL_STATUS_OK;
  sl_wifi_roam_configuration_t roam_configuration;
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);

  roam_configuration.trigger_level        = (int32_t)GET_COMMAND_ARG(arguments, 1);
  roam_configuration.trigger_level_change = (uint32_t)GET_COMMAND_ARG(arguments, 2);

  status = sl_wifi_set_roam_configuration(interface, &roam_configuration);

  return status;
}

sl_status_t wifi_get_transmit_rate_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  sl_wifi_rate_protocol_t rate_protocol = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, sl_wifi_rate_protocol_t);
  sl_wifi_rate_t mask                   = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0, sl_wifi_rate_t);

  sl_status_t status = sl_wifi_get_transmit_rate(interface, &rate_protocol, &mask);

  if (status == SL_STATUS_OK) {
    printf("\n wifi protocol is: %s\n wifi rate is: %s\n",
           get_sl_wifi_protocol_name(rate_protocol),
           get_sl_wifi_rate_name(mask));
  }

  return status;
}

sl_status_t wifi_get_ap_client_list_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_mac_address_t client_list[SL_WIFI_MAX_CLIENT_COUNT] = { 0 };

  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_AP_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_get_ap_client_list(interface, SL_WIFI_MAX_CLIENT_COUNT, client_list);

  printf("Mac address list of clients connected to AP:");
  for (uint16_t index = 0; client_list[index].octet[0] != 0; index++) {
    printf("\n %d -> %x:%x:%x:%x:%x:%x",
           index,
           client_list[index].octet[0],
           client_list[index].octet[1],
           client_list[index].octet[2],
           client_list[index].octet[3],
           client_list[index].octet[4],
           client_list[index].octet[5]);
  }

  return status;
}

sl_status_t wifi_get_ap_client_count_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  uint32_t client_list_count;

  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_AP_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_get_ap_client_count(interface, &client_list_count);

  if (status == SL_STATUS_OK) {
    printf("Client count = %lu\n", client_list_count);
  }

  return status;
}

sl_status_t wifi_generate_wps_pin_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_wps_pin_t wps_pin;

  sl_status_t status = sl_wifi_generate_wps_pin(&wps_pin);

  if (status == SL_STATUS_OK) {
    printf("\n wps pin: %s\n", wps_pin.digits);
  }

  return status;
}

sl_status_t wifi_start_wps(console_args_t *arguments)
{
  sl_wifi_wps_pin_t pin         = { 0 };
  sl_wifi_interface_t interface = arguments->arg[0];
  sl_wifi_wps_mode_t wps_mode   = arguments->arg[1];

  char *wps_pin = GET_OPTIONAL_COMMAND_ARG(arguments, 2, NULL, char *);

  if (wps_pin != NULL) {
    memcpy(pin.digits, wps_pin, sizeof(pin.digits));
  }

  return sl_wifi_start_wps(interface, wps_mode, wps_pin == NULL ? NULL : &pin);
}
sl_status_t sl_wifi_set_advanced_scan_configuration_command_handler(console_args_t *arguments)
{
  sl_status_t status                                                = SL_STATUS_OK;
  sl_wifi_advanced_scan_configuration_t advanced_scan_configuration = { 0 };

  // Advance scanning parameters
  advanced_scan_configuration.active_channel_time =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, ADV_ACTIVE_SCAN_DURATION, uint16_t);
  advanced_scan_configuration.passive_channel_time =
    GET_OPTIONAL_COMMAND_ARG(arguments, 1, ADV_PASSIVE_SCAN_DURATION, uint16_t);
  advanced_scan_configuration.trigger_level = GET_OPTIONAL_COMMAND_ARG(arguments, 2, ADV_SCAN_THRESHOLD, int32_t);
  advanced_scan_configuration.trigger_level_change =
    GET_OPTIONAL_COMMAND_ARG(arguments, 3, ADV_RSSI_TOLERANCE_THRESHOLD, uint16_t);
  advanced_scan_configuration.enable_multi_probe  = GET_OPTIONAL_COMMAND_ARG(arguments, 4, ADV_MULTIPROBE, uint8_t);
  advanced_scan_configuration.enable_instant_scan = GET_OPTIONAL_COMMAND_ARG(arguments, 5, ADV_INSTANT_SCAN, uint8_t);

  status = sl_wifi_set_advanced_scan_configuration(&advanced_scan_configuration);

  return status;
}

sl_status_t sl_wifi_get_advanced_scan_configuration_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_advanced_scan_configuration_t advanced_scan_configuration = { 0 };

  sl_status_t status = sl_wifi_get_advanced_scan_configuration(&advanced_scan_configuration);

  if (status == SL_STATUS_OK) {
    printf("\nAdvanced scan configurations:\n active channel time: %d\n passive channel time: %d\n trigger "
           "level: %d\n trigger level change: %lu\n enable multi probe: %d\n",
           advanced_scan_configuration.active_channel_time,
           advanced_scan_configuration.passive_channel_time,
           (int)advanced_scan_configuration.trigger_level,
           advanced_scan_configuration.trigger_level_change,
           advanced_scan_configuration.enable_multi_probe);
  }

  return status;
}

sl_status_t wifi_stop_scan_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);
  return sl_wifi_stop_scan(interface);
}

sl_status_t wifi_get_ap_configuration_command_handler(console_args_t *arguments)
{
  sl_status_t status;
  sl_wifi_ap_configuration_t configuration;
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_AP_2_4GHZ_INTERFACE, sl_wifi_interface_t);

  status = sl_wifi_get_ap_configuration(interface, &configuration);
  VERIFY_STATUS_AND_RETURN(status);

  printf("SSID: %s\nsecurity: %d\nencryption: %d\nchannel: %d\nrate_protocol: %d\noptions: %d\ncredential_id: "
         "%lu\nclient idle timeout: %lu\ndtim beacon count: %d\nmaximum client: %d\n",
         configuration.ssid.value,
         configuration.security,
         configuration.encryption,
         configuration.channel.channel,
         configuration.rate_protocol,
         configuration.options,
         configuration.credential_id,
         configuration.client_idle_timeout,
         configuration.dtim_beacon_count,
         configuration.maximum_clients);
  return SL_STATUS_OK;
}

sl_status_t wifi_set_ap_configuration_command_handler(console_args_t *arguments)
{
  sl_wifi_ap_configuration_t configuration = { 0 };
  sl_wifi_interface_t interface            = SL_WIFI_AP_INTERFACE;
  sl_status_t status;

  const char *ssid         = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SOFT_AP_SSID, char *);
  const size_t ssid_length = strlen(ssid);
  memcpy(configuration.ssid.value, ssid, ssid_length);

  configuration.ssid.length     = (uint8_t)ssid_length;
  configuration.security        = GET_OPTIONAL_COMMAND_ARG(arguments, 1, SL_WIFI_WPA2, sl_wifi_security_t);
  configuration.encryption      = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_CCMP_ENCRYPTION, sl_wifi_encryption_t);
  configuration.channel.channel = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 11, uint8_t);
  configuration.rate_protocol =
    GET_OPTIONAL_COMMAND_ARG(arguments, 4, SL_WIFI_RATE_PROTOCOL_AUTO, sl_wifi_rate_protocol_t);
  configuration.options             = GET_OPTIONAL_COMMAND_ARG(arguments, 5, 0, uint8_t);
  configuration.credential_id       = GET_OPTIONAL_COMMAND_ARG(arguments, 6, 0, sl_wifi_credential_id_t);
  configuration.client_idle_timeout = GET_OPTIONAL_COMMAND_ARG(arguments, 7, 120000, uint32_t);
  configuration.dtim_beacon_count   = GET_OPTIONAL_COMMAND_ARG(arguments, 8, 4, uint8_t);
  configuration.maximum_clients     = GET_OPTIONAL_COMMAND_ARG(arguments, 9, 4, uint8_t);

  status = sl_wifi_set_ap_configuration(interface, &configuration);
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

sl_status_t wifi_test_client_configuration_command_handler(console_args_t *arguments)
{
  sl_status_t status                = SL_STATUS_OK;
  sl_wifi_credential_id_t id        = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
  sl_wifi_interface_t interface     = SL_WIFI_CLIENT_2_4GHZ_INTERFACE;
  sl_wifi_client_configuration_t ap = { 0 };
  sl_wifi_credential_t cred         = { 0 };
  char *ssid                        = (char *)arguments->arg[0];
  char *password                    = (char *)arguments->arg[1];
  sl_wifi_security_t secType        = GET_OPTIONAL_COMMAND_ARG(arguments, 2, SL_WIFI_WPA2, sl_wifi_security_t);
  const char *user_name             = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, const char *);
  sl_wifi_encryption_t encType = GET_OPTIONAL_COMMAND_ARG(arguments, 4, SL_WIFI_CCMP_ENCRYPTION, sl_wifi_encryption_t);
  uint32_t timeout_ms          = GET_OPTIONAL_COMMAND_ARG(arguments, 5, 0, uint32_t);
  ap.ssid.length               = GET_OPTIONAL_COMMAND_ARG(arguments, 6, strlen(ssid), uint8_t);
  ap.channel.channel           = GET_OPTIONAL_COMMAND_ARG(arguments, 7, 0, uint8_t);
  ap.channel.band              = GET_OPTIONAL_COMMAND_ARG(arguments, 8, SL_WIFI_AUTO_BAND, sl_wifi_band_t);
  ap.channel.bandwidth         = GET_OPTIONAL_COMMAND_ARG(arguments, 9, SL_WIFI_BANDWIDTH_10MHz, sl_wifi_bandwidth_t);

  UNUSED_PARAMETER(user_name);

  cred.type = SL_WIFI_PSK_CREDENTIAL;
  memcpy(cred.psk.value, password, strlen(password));

  if (timeout_ms == 0) {
    printf("registering sl_wifi_set_join_callback");
    sl_wifi_set_join_callback(join_callback_handler, NULL);
  }

  status = sl_net_set_credential(id, SL_NET_WIFI_PSK, password, strlen(password));
  if (SL_STATUS_OK != status) {
    return status;
  }

  memcpy(ap.ssid.value, ssid, ap.ssid.length);
  ap.security      = secType;
  ap.encryption    = encType;
  ap.credential_id = id;

  status = sl_wifi_test_client_configuration(interface, &ap, timeout_ms);

  if (SL_STATUS_IN_PROGRESS == status) {
    callback_status = SL_STATUS_IN_PROGRESS;
    while (callback_status == SL_STATUS_IN_PROGRESS) {
      osDelay(100);
    }
    status = callback_status;
    if (status == SL_STATUS_OK) {
      status = sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
    }
  }
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_send_raw_data_command_handler(console_args_t *arguments)
{
  uint8_t buffer[300]           = { 0 };
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  uint16_t data_length;

  /*PING packet */

  //destination mac
  buffer[0] = 0x48;
  buffer[1] = 0x51;
  buffer[2] = 0xc5;
  buffer[3] = 0x09;
  buffer[4] = 0x7e;
  buffer[5] = 0xfa;

  //src mac
  buffer[6]  = 0x80;
  buffer[7]  = 0xc9;
  buffer[8]  = 0x55;
  buffer[9]  = 0x3d;
  buffer[10] = 0x23;
  buffer[11] = 0x90;

  //ip type
  buffer[12] = 0x08;

  //version and header length
  buffer[14] = 0x45;

  //total lenght
  buffer[17] = 0x26;

  //identification
  buffer[19] = 0x04;

  //time to live
  buffer[22] = 0x80;

  //protocol
  buffer[23] = 0x01;

  //header checksum
  buffer[24] = 0xb7;
  buffer[25] = 0xe1;

  //src ip
  buffer[26] = 192;
  buffer[27] = 168;
  buffer[28] = 0;
  buffer[29] = 202;

  //dst ip
  buffer[30] = 192;
  buffer[31] = 168;
  buffer[32] = 0;
  buffer[33] = 215;

  //ping type (request)
  buffer[34] = 0x8;

  //checksum
  buffer[36] = 0xe3;
  buffer[37] = 0x1c;

  //identifier le
  buffer[39] = 0xca;

  //data;
  buffer[43] = 0x01;
  buffer[44] = 0x02;
  buffer[45] = 0x03;
  buffer[46] = 0x04;
  buffer[47] = 0x05;
  buffer[48] = 0x06;
  buffer[49] = 0x07;
  buffer[50] = 0x08;
  buffer[51] = 0x09;
  buffer[52] = 0x00;

  data_length = sizeof(buffer);

  return sl_wifi_send_raw_data_frame(interface, buffer, data_length);
}

sl_status_t sl_wifi_enable_twt(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_set_twt_config_callback(twt_callback_handler, NULL);
  sl_wifi_performance_profile_t performance_profile = { .twt_request = default_twt_setup_configuration };
  sl_status_t status                                = SL_STATUS_OK;
  status                                            = sl_wifi_enable_target_wake_time(&performance_profile.twt_request);
  if (SL_STATUS_IN_PROGRESS == status) {
    const uint32_t start = osKernelGetTickCount();

    while (!twt_results_complete && (osKernelGetTickCount() - start) <= TWT_SCAN_TIMEOUT) {
      osDelay(100);
    }

    status = twt_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }
  return status;
}

sl_status_t sl_wifi_disable_twt(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_wifi_set_twt_config_callback(twt_callback_handler, NULL);
  sl_wifi_performance_profile_t performance_profile = { .twt_request = default_twt_teardown_configuration };
  sl_status_t status                                = SL_STATUS_OK;
  status = sl_wifi_disable_target_wake_time(&performance_profile.twt_request);
  if (SL_STATUS_IN_PROGRESS == status) {
    const uint32_t start = osKernelGetTickCount();

    while (!twt_results_complete && (osKernelGetTickCount() - start) <= TWT_SCAN_TIMEOUT) {
      osDelay(100);
    }

    status = twt_results_complete ? callback_status : SL_STATUS_TIMEOUT;
  }
  return status;
}

static sl_status_t twt_callback_handler(sl_wifi_event_t event,
                                        sl_si91x_twt_response_t *result,
                                        uint32_t result_length,
                                        void *arg)
{
  UNUSED_PARAMETER(arg);
  UNUSED_PARAMETER(result_length);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status      = *(sl_status_t *)result;
    twt_results_complete = true;
    return SL_STATUS_FAIL;
  }
  show_twt_results(result);
  twt_results_complete = true;
  return SL_STATUS_OK;
}

static void show_twt_results(sl_si91x_twt_response_t *response)
{
  printf("\r\n wake duration : 0x%uX", response->wake_duration);
  printf("\r\n wake_duration_unit: 0x%uX", response->wake_duration_unit);
  printf("\r\n wake_int_exp : 0x%uX", response->wake_int_exp);
  printf("\r\n negotiation_type : 0x%uX", response->negotiation_type);
  printf("\r\n wake_int_mantissa : 0x%uX", response->wake_int_mantissa);
  printf("\r\n implicit_twt : 0x%uX", response->implicit_twt);
  printf("\r\n un_announced_twt : 0x%uX", response->un_announced_twt);
  printf("\r\n triggered_twt : 0x%uX", response->triggered_twt);
  printf("\r\n twt_channel : 0x%uX", response->twt_channel);
  printf("\r\n twt_protection : 0x%uX", response->twt_protection);
  printf("\r\n twt_flow_id : 0x%uX\r\n", response->twt_flow_id);
}

sl_status_t sl_wifi_filter_broadcast_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status                     = SL_STATUS_OK;
  uint16_t beacon_drop_threshold         = 5000;
  uint8_t filter_bcast_in_tim            = 1;
  uint8_t filter_bcast_tim_till_next_cmd = 1;
  status = sl_wifi_filter_broadcast(beacon_drop_threshold, filter_bcast_in_tim, filter_bcast_tim_till_next_cmd);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t wifi_set_advanced_client_config(console_args_t *arguments)
{
  sl_status_t status;
  sl_wifi_advanced_client_configuration_t config = {
    .eap_flags               = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0, uint32_t),
    .max_retry_attempts      = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint32_t),
    .scan_interval           = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 4, uint32_t),
    .beacon_missed_count     = GET_OPTIONAL_COMMAND_ARG(arguments, 3, 40, uint32_t),
    .first_time_retry_enable = GET_OPTIONAL_COMMAND_ARG(arguments, 4, 0, uint32_t),
  };
  status = sl_wifi_set_advanced_client_configuration(SL_WIFI_CLIENT_INTERFACE, &config);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

void sl_si91x_set_timeout(sl_si91x_timeout_t *timeout_config);
sl_status_t wifi_set_timeout_config(console_args_t *arguments)
{
  sl_si91x_timeout_t timeout_configuration = {
    .keep_alive_timeout_value       = GET_OPTIONAL_COMMAND_ARG(arguments, 0, 0xFFFF, uint16_t),
    .auth_assoc_timeout_value       = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0xFFFF, uint16_t),
    .active_chan_scan_timeout_value = GET_OPTIONAL_COMMAND_ARG(arguments, 2, 0xFFFF, uint16_t),
  };
  sl_si91x_set_timeout(&timeout_configuration);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_update_gain_table_command_handler(console_args_t *arguments)
{
  sl_status_t status           = SL_STATUS_OK;
  uint8_t band                 = (uint8_t)GET_COMMAND_ARG(arguments, 0);
  uint8_t bandwidth            = (uint8_t)GET_COMMAND_ARG(arguments, 1);
  uint8_t gain_table_payload[] = { 3,  0,  13, 1,  34, 20, 20,  2,  34, 28, 28, 3,  34,  32, 32, 4,  34,
                                   36, 36, 5,  34, 38, 38, 6,   34, 40, 40, 7,  34, 38,  38, 8,  34, 36,
                                   36, 9,  34, 32, 32, 10, 34,  32, 32, 11, 34, 24, 24,  12, 34, 16, 24,
                                   13, 34, 12, 12, 2,  17, 255, 20, 16, 16, 4,  17, 255, 26, 20, 20 };

  status = sl_wifi_update_gain_table(band, bandwidth, gain_table_payload, sizeof(gain_table_payload));
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_11ax_config_command_handler(console_args_t *arguments)
{
  sl_status_t status     = SL_STATUS_OK;
  uint8_t guard_interval = (uint8_t)GET_COMMAND_ARG(arguments, 0);
  status                 = sl_wifi_set_11ax_config(guard_interval);
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

sl_status_t set_wifi_config_handler(console_args_t *arguments)
{
  wifi_connections                     = 0;
  wifi_disconnections                  = 0;
  si91x_init_configuration.boot_config = (sl_si91x_boot_configuration_t){
    .oper_mode                  = GET_COMMAND_ARG(arguments, 0),
    .coex_mode                  = GET_COMMAND_ARG(arguments, 1),
    .feature_bit_map            = GET_COMMAND_ARG(arguments, 2),
    .tcp_ip_feature_bit_map     = GET_COMMAND_ARG(arguments, 3),
    .custom_feature_bit_map     = GET_COMMAND_ARG(arguments, 4),
    .ext_custom_feature_bit_map = GET_COMMAND_ARG(arguments, 5),
    .bt_feature_bit_map         = GET_COMMAND_ARG(arguments, 6),
    .ext_tcp_ip_feature_bit_map = GET_COMMAND_ARG(arguments, 7),
    .ble_feature_bit_map        = GET_COMMAND_ARG(arguments, 8),
    .ble_ext_feature_bit_map    = GET_COMMAND_ARG(arguments, 9),
    .config_feature_bit_map     = GET_COMMAND_ARG(arguments, 10),
  };
  return SL_STATUS_OK;
}

sl_status_t set_region_command_handler(console_args_t *arguments)
{
  si91x_init_configuration.region_code = GET_OPTIONAL_COMMAND_ARG(arguments, 0, US, sl_si91x_region_code_t);
  return SL_STATUS_OK;
}

sl_status_t set_listen_interval_command_handler(console_args_t *arguments)
{

  sl_si91x_set_listen_interval((uint32_t)arguments->arg[0]);
  return SL_STATUS_OK;
}

sl_status_t set_join_configuration_command_handler(console_args_t *arguments)
{

  sl_status_t status            = SL_STATUS_OK;
  sl_wifi_interface_t interface = GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_INTERFACE, sl_wifi_interface_t);
  uint8_t join_feature_bitmap   = GET_OPTIONAL_COMMAND_ARG(arguments, 1, 0, uint8_t);
  status                        = sl_si91x_set_join_configuration(interface, join_feature_bitmap);
  if (status != SL_STATUS_OK) {
    printf("Failed to start set join configuration: 0x%lx", status);
  }
  return status;
}

sl_status_t set_eap_credentials_command_handler(console_args_t *arguments)
{

  sl_status_t status = SL_STATUS_OK;

  //sl_wifi_eap_credential_t eap_credential = { 0 };
  sl_net_wifi_eap_credential_entry_t eap_credential = { 0 };

  memcpy(eap_credential.data.username,
         (uint8_t *)arguments->arg[0],
         MIN(strlen((char *)arguments->arg[0]), sizeof(eap_credential.data.username)));
  memcpy(eap_credential.data.password,
         (uint8_t *)arguments->arg[1],
         MIN(strlen((char *)arguments->arg[1]), sizeof(eap_credential.data.password)));
  eap_credential.data.certificate_id = (uint32_t)arguments->arg[2];
  eap_credential.data_length         = sizeof(sl_wifi_eap_credential_t);
  eap_credential.type                = 1;
  //eap_credential.data.certificate_key = {};

  sl_net_set_credential(SL_NET_WIFI_EAP_CLIENT_CREDENTIAL_ID,
                        SL_NET_EAP_CLIENT_CREDENTIAL,
                        &(eap_credential.data),
                        sizeof(sl_wifi_eap_credential_t));

  return status;
}

sl_status_t set_rtc_timer_command_handler(console_args_t *arguments)
{
  sl_status_t status = SL_STATUS_OK;

  sl_si91x_module_rtc_time_t tm_rtc;

  memcpy(&tm_rtc.tm_sec, &arguments->arg[0], sizeof(tm_rtc.tm_sec));
  memcpy(&tm_rtc.tm_min, &arguments->arg[1], sizeof(tm_rtc.tm_min));
  memcpy(&tm_rtc.tm_hour, &arguments->arg[2], sizeof(tm_rtc.tm_hour));
  memcpy(&tm_rtc.tm_mday, &arguments->arg[3], sizeof(tm_rtc.tm_mday));
  memcpy(&tm_rtc.tm_mon, &arguments->arg[4], sizeof(tm_rtc.tm_mon));
  memcpy(&tm_rtc.tm_wday, &arguments->arg[5], sizeof(tm_rtc.tm_wday));
  memcpy(&tm_rtc.tm_year, &arguments->arg[6], sizeof(tm_rtc.tm_year));

  status = sl_si91x_set_rtc_timer(&tm_rtc);
  return status;
}

sl_status_t get_rtc_timer_command_handler(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  sl_status_t status = SL_STATUS_OK;

  sl_si91x_module_rtc_time_t tm_rtc;

  status = sl_si91x_get_rtc_timer(&tm_rtc);

  printf("seconds:%lu\n", tm_rtc.tm_sec);
  printf("minutes:%lu\n", tm_rtc.tm_min);
  printf("hours:%lu\n", tm_rtc.tm_hour);
  printf("month_day:%lu\n", tm_rtc.tm_mday);
  printf("month:%lu\n", tm_rtc.tm_mon);
  printf("year:%lu\n", tm_rtc.tm_year);
  printf("day:%lu\n", tm_rtc.tm_wday);

  return status;
}

sl_status_t set_default_connection_stats(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  wifi_connections    = 0;
  wifi_disconnections = 0;
  return SL_STATUS_OK;
}

sl_status_t wifi_stop_ap_command_handler(console_args_t *arguments)
{
  sl_wifi_interface_t interface =
    GET_OPTIONAL_COMMAND_ARG(arguments, 0, SL_WIFI_CLIENT_2_4GHZ_INTERFACE, sl_wifi_interface_t);
  sl_status_t status = sl_wifi_stop_ap(interface);
  return status;
}

sl_status_t wifi_ssl_cipher_select(console_args_t *arguments)
{
  g_sl_ssl_ciphers = sl_cli_get_argument_uint32(arguments, 0);
  printf("cipher list: %lu\n", g_sl_ssl_ciphers);
  return SL_STATUS_OK;
}

sl_status_t wifi_ssl_extended_cipher_select(console_args_t *arguments)
{
  g_sl_ext_ssl_ciphers = sl_cli_get_argument_uint32(arguments, 0);
  printf("extended cipher list: %lu\n", g_sl_ext_ssl_ciphers);
  return SL_STATUS_OK;
}

sl_status_t wifi_set_default_ssl_cipher(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  g_sl_ssl_ciphers = (SSL_DEFAULT_CIPHERS | BIT_TLS_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_RSA_WITH_AES_128_CBC_SHA256
                      | BIT_TLS_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CBC_SHA
                      | BIT_TLS_RSA_WITH_AES_128_CCM_8 | BIT_TLS_RSA_WITH_AES_256_CCM_8);
  printf("cipher list: %lu\n", g_sl_ssl_ciphers);
  return SL_STATUS_OK;
}

sl_status_t wifi_set_extended_default_ssl_cipher(console_args_t *arguments)
{
  UNUSED_PARAMETER(arguments);
  g_sl_ext_ssl_ciphers =
    (BIT_TLS13_AES_128_GCM_SHA256 | BIT_TLS13_AES_256_GCM_SHA384 | BIT_TLS13_CHACHA20_POLY1305_SHA256
     | BIT_TLS13_AES_128_CCM_SHA256 | BIT_TLS13_AES_128_CCM_8_SHA256);
  printf("extended cipher list: %lu\n", g_sl_ext_ssl_ciphers);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_antenna_command_handler(console_args_t *arguments)
{
  sl_status_t status            = SL_STATUS_OK;
  sl_wifi_interface_t interface = (sl_wifi_interface_t)GET_COMMAND_ARG(arguments, 0);
  sl_wifi_antenna_t antenna     = (sl_wifi_antenna_t)GET_COMMAND_ARG(arguments, 1);

  // Register WLAN receive stats call back handler
  sl_wifi_set_stats_callback((sl_wifi_stats_callback_t)wifi_stats_receive_handler, NULL);

  printf("Antenna interface : %d\r\n", interface);
  printf("Antenna channel: %d\r\n", antenna);
  status = sl_wifi_set_antenna(interface, antenna);
  if (status != SL_STATUS_OK) {
    printf("Failed to start set Antenna: 0x%lx\r\n", status);
  }
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nAntenna Command Frame Success \r\n");

  return SL_STATUS_OK;
}

sl_status_t sl_si91x_transmit_test_start_command_handler(console_args_t *arguments)
{
  sl_si91x_request_tx_test_info_t tx_test_info = {
    .enable                 = 1,
    .power                  = 127,
    .rate                   = SL_WIFI_DATA_RATE_6,
    .length                 = 100,
    .mode                   = 0,
    .channel                = 1,
    .aggr_enable            = 0,
    .enable_11ax            = 0,
    .coding_type            = 0,
    .nominal_pe             = 0,
    .ul_dl                  = 0,
    .he_ppdu_type           = 0,
    .beam_change            = 0,
    .bw                     = 0,
    .stbc                   = 0,
    .tx_bf                  = 0,
    .gi_ltf                 = 0,
    .dcm                    = 0,
    .nsts_midamble          = 0,
    .spatial_reuse          = 0,
    .bss_color              = 0,
    .he_siga2_reserved      = 0,
    .ru_allocation          = 0,
    .n_heltf_tot            = 0,
    .sigb_dcm               = 0,
    .sigb_mcs               = 0,
    .user_sta_id            = 0,
    .user_idx               = 0,
    .sigb_compression_field = 0,
  };
  sl_status_t status       = SL_STATUS_OK;
  tx_test_info.enable      = (uint8_t)GET_COMMAND_ARG(arguments, 0);
  tx_test_info.power       = (uint8_t)GET_COMMAND_ARG(arguments, 1);
  tx_test_info.rate        = (uint8_t)GET_COMMAND_ARG(arguments, 2);
  tx_test_info.length      = (uint8_t)GET_COMMAND_ARG(arguments, 3);
  tx_test_info.mode        = (uint8_t)GET_COMMAND_ARG(arguments, 4);
  tx_test_info.channel     = (uint8_t)GET_COMMAND_ARG(arguments, 5);
  tx_test_info.aggr_enable = (uint8_t)GET_COMMAND_ARG(arguments, 6);

  status = sl_si91x_transmit_test_start(&tx_test_info);
  if (status != SL_STATUS_OK) {
    printf("\r\nTransmit test start Failed, Error Code : 0x%lX\r\n", status);
  }
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nTransmit test start Success\r\n");
  return SL_STATUS_OK;
}

sl_status_t sl_si91x_transmit_test_stop_command_handler(console_args_t *arguments)
{
  sl_status_t status = SL_STATUS_OK;

  status = sl_si91x_transmit_test_stop();
  if (status != SL_STATUS_OK) {
    printf("\r\nTransmit test stop Failed, Error Code : 0x%lX\r\n", status);
  }
  VERIFY_STATUS_AND_RETURN(status);
  printf("\r\nTransmit test stop Success\r\n");

  return SL_STATUS_OK;
}