/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_wifi_types.h"
#include "sl_wifi_device.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_status.h"
#include "sl_si91x_types.h"
#include "sl_si91x_protocol_types.h"
#include "sl_si91x_driver.h"
#include "sl_rsi_utility.h"
#if defined(SLI_SI91X_SOCKETS)
#include "sl_si91x_socket_utility.h"
#endif
#include <stdint.h>
#include <string.h>

#ifndef SL_NCP_DEFAULT_COMMAND_WAIT_TIME
#define SL_NCP_DEFAULT_COMMAND_WAIT_TIME 3000
#endif

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))

#ifdef SL_SI91X_SIDE_BAND_CRYPTO
#include "sl_si91x_driver.h"
#include "rsi_m4.h"
extern rsi_m4ta_desc_t crypto_desc[2];
#endif

#ifdef SLI_SI91X_MCU_INTERFACE
#include "rsi_wisemcu_hardware_setup.h"
#endif

#ifdef SLI_SI91X_OFFLOAD_NETWORK_STACK
#include "sl_net_si91x_integration_handler.h"
#else

// This macro defines a handler for cleaning up resources.
#define SLI_NETWORK_CLEANUP_HANDLER() \
  {                                   \
  }
#endif
// Enterprise configuration command parameters
/*=======================================================================*/

// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP

#define SL_EAP_TLS_METHOD  "TLS"
#define SL_EAP_TTLS_METHOD "TTLS"
#define SL_EAP_FAST_METHOD "FAST"
#define SL_EAP_PEAP_METHOD "PEAP"
#define SL_EAP_LEAP_METHOD "LEAP"

// This parameter is used to configure the module in Enterprise security mode
#define SL_EAP_INNER_METHOD "\"auth=MSCHAPV2\""

// Private Key Password is required for encrypted private key, format is like "\"12345678\""
#define SL_DEFAULT_PRIVATE_KEY_PASSWORD ""

#define JOIN_TIMEOUT 8000

#define ALWAYS_ROAM                          1
#define NEVER_ROAM                           0
#define MAX_FLOW_ID                          7
#define MAX_WAKE_INTERVAL_EXPONENT           31
#define MAX_WAKE_INTERVAL_EXPONENT_TOLERANCE 31
#define MAX_WAKE_DURATION_UNIT               1
#define MAX_TWT_RETRY_LIMIT                  15
#define MIN_TWT_RETRY_INTERVAL               5
#define MAX_TWT_REQ_TYPE                     2
#define REQUEST_TWT                          0
#define SUGGEST_TWT                          1
#define DEMAND_TWT                           1
#define TWT_WAKE_DURATION_UNIT_1024TU        1024
#define TWT_WAKE_DURATION_UNIT_256TU         256
#define DEVICE_AVERAGE_THROUGHPUT            20000
#define MAX_TX_AND_RX_LATENCY_LIMIT          21600000
#define MAX_TWT_SUSPEND_DURATION             0x5265c00
#define ABSOLUTE_POWER_VALUE_TOGGLE          0x80
#define PASSIVE_SCAN_ENABLE                  BIT(7)
#define LP_CHAIN_ENABLE                      BIT(6)
#define QUICK_SCAN_ENABLE                    1
#define SCAN_RESULTS_TO_HOST                 2
#define MAX_2_4G_CHANNEL                     14

/*========================================================================*/
// 11ax params
/*========================================================================*/
#define NOMINAL_PE                              2
#define DCM_ENABLE                              0
#define LDPC_ENABLE                             0
#define NG_CB_ENABLE                            0
#define NG_CB_VALUES                            0
#define UORA_ENABLE                             0
#define TRIGGER_RESP_IND                        0xF
#define IPPS_VALID_VALUE                        0
#define TX_ONLY_ON_AP_TRIG                      0
#define CONFIG_ER_SU                            0 // 0 - NO ER_SU support, 1 - Use ER_SU rates along with Non_ER_SU rates, 2 - Use ER_SU rates only
#define SLI_SI91X_ENABLE_TWT_FEATURE            1
#define SLI_SI91X_DISABLE_SU_BEAMFORMEE_SUPPORT 0
/*=======================================================================*/
extern bool device_initialized;
extern bool bg_enabled;
extern bool interface_is_up[SL_WIFI_MAX_INTERFACE_INDEX];
extern sl_wifi_interface_t default_interface;
static sl_wifi_advanced_scan_configuration_t advanced_scan_configuration     = { 0 };
static sl_wifi_advanced_client_configuration_t advanced_client_configuration = { 0 };
int32_t validate_datarate(sl_wifi_data_rate_t data_rate);
sl_status_t sl_wifi_get_associated_client_list(void *client_list_buffer, uint16_t buffer_length, uint32_t timeout);
static sl_wifi_client_info_response_t sli_si91x_client_info = { 0 };

static sl_status_t fill_join_request_security_using_encryption(sl_wifi_encryption_t encryption_mode,
                                                               uint8_t *security_type)
{
  if (encryption_mode == SL_WIFI_CCMP_ENCRYPTION) {
    *security_type |= BIT(7);
  }
  return SL_STATUS_OK;
}

static sl_status_t get_configured_join_request(sl_wifi_interface_t module_interface,
                                               const void *configuration,
                                               sl_si91x_join_request_t *join_request)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(configuration);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(join_request);
  sl_status_t status = SL_STATUS_OK;

  if (module_interface & SL_WIFI_CLIENT_INTERFACE) {
    // get join feature bitmap
    status = sl_si91x_get_join_configuration(SL_WIFI_CLIENT_INTERFACE, &(join_request->join_feature_bitmap));
    VERIFY_STATUS_AND_RETURN(status);

    sl_wifi_client_configuration_t *client_configuration = (sl_wifi_client_configuration_t *)configuration;

    // narrowing conversion from Enum of uint16 to uint8
    get_saved_sl_wifi_rate(&join_request->data_rate);
    memcpy(join_request->ssid, client_configuration->ssid.value, client_configuration->ssid.length);

    join_request->ssid_len      = client_configuration->ssid.length;
    join_request->security_type = client_configuration->security;
    if ((join_request->security_type == SL_WIFI_WPA3)
        || (join_request->security_type == SL_WIFI_WPA3_ENTERPRISE)) { //check for WPA3 security
      join_request->join_feature_bitmap |= SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED;
    } else if ((join_request->security_type == SL_WIFI_WPA3_TRANSITION)
               || join_request->security_type == SL_WIFI_WPA3_TRANSITION_ENTERPRISE) {
      join_request->join_feature_bitmap &= ~(SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED);
      join_request->join_feature_bitmap |= SL_SI91X_JOIN_FEAT_MFP_CAPABLE_ONLY;
    } else if (join_request->security_type == SL_WIFI_WPA2 || join_request->security_type == SL_WIFI_WPA_WPA2_MIXED) {
      join_request->join_feature_bitmap |= SL_SI91X_JOIN_FEAT_MFP_CAPABLE_ONLY;
    }

    fill_join_request_security_using_encryption(client_configuration->encryption, &(join_request->security_type));

    join_request->vap_id          = SL_SI91X_WIFI_CLIENT_VAP_ID; // For Station vap_id will be 0
    join_request->listen_interval = sl_si91x_get_listen_interval();
    memcpy(join_request->join_bssid, client_configuration->bssid.octet, sizeof(join_request->join_bssid));
  } else if (module_interface & SL_WIFI_AP_INTERFACE) {
    // get join feature bitmap
    status = sl_si91x_get_join_configuration(SL_WIFI_AP_INTERFACE, &(join_request->join_feature_bitmap));
    VERIFY_STATUS_AND_RETURN(status);

    sl_wifi_ap_configuration_t *ap_configuration = (sl_wifi_ap_configuration_t *)configuration;

    get_saved_sl_wifi_rate(&join_request->data_rate);
    memcpy(join_request->ssid, ap_configuration->ssid.value, ap_configuration->ssid.length);

    join_request->ssid_len      = ap_configuration->ssid.length;
    join_request->security_type = ap_configuration->security;
    join_request->vap_id        = 0;
    if (join_request->security_type == SL_WIFI_WPA3) { //check for WPA3 security
      join_request->join_feature_bitmap |= SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED;
    } else if (join_request->security_type == SL_WIFI_WPA3_TRANSITION) { //check for WPA3 Tranisition security
      join_request->join_feature_bitmap &= ~(SL_SI91X_JOIN_FEAT_MFP_CAPABLE_REQUIRED);
      join_request->join_feature_bitmap |= SL_SI91X_JOIN_FEAT_MFP_CAPABLE_ONLY;
    }

    if (get_opermode() == SL_SI91X_CONCURRENT_MODE) {
      join_request->vap_id = SL_SI91X_WIFI_AP_VAP_ID; // For Concurrent mode AP vap_id should be 1 else 0.
    }
  } else {
    return SL_STATUS_FAIL;
  }

  sl_wifi_max_tx_power_t wifi_max_tx_power = get_max_tx_power();

  /* Within the 1-byte 'power_level' variable, bit 0 and bit 1 are allocated for encoding power level thresholds(low, mid, high).
   * The Most Significant Bit serves as an indicator for toggling between absolute power value representation.
   * When the MSB is set, the 'power_level' variable encodes the absolute power value using bits 2 through 6. */
  join_request->power_level = (wifi_max_tx_power.join_tx_power << 2) | ABSOLUTE_POWER_VALUE_TOGGLE;

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief
 *   to enable/disable ht capabilities
 * @pre Pre-conditions:
 * - @ref sl_wifi_init should be called before this API.
 * @param[in] interface
 *   Wi-Fi Access Point interface as identified by @ref sl_wifi_interface_t
 *  @param[in] HtCaps
 * HtCaps is identified by @ref sl_si91x_request_ap_high_throughput_capability_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 * @note 
 *   Client interfaces are not supported.
 ******************************************************************************/
static sl_status_t sli_si91x_set_high_throughput_capability(sl_wifi_interface_t interface,
                                                            sl_si91x_request_ap_high_throughput_capability_t HtCaps)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  sl_status_t status = SL_STATUS_OK;
  status             = sl_si91x_driver_send_command(RSI_WLAN_REQ_HT_CAPABILITIES,
                                        SI91X_WLAN_CMD,
                                        &HtCaps,
                                        sizeof(sl_si91x_request_ap_high_throughput_capability_t),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_init(const sl_wifi_device_configuration_t *configuration,
                         sl_wifi_device_context_t *device_context,
                         sl_wifi_event_handler_t event_handler)
{
  UNUSED_PARAMETER(device_context);
#ifdef SLI_SI91X_MCU_INTERFACE
#if defined(SLI_SI917)
  sl_si91x_efuse_data_t efuse_data;
#endif
#endif
  sl_status_t status = SL_STATUS_OK;
  status             = sl_si91x_driver_init(configuration, event_handler);
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
  if (status == SL_STATUS_OK) {
    uint32_t crypto_desc_ptr = (uint32_t)crypto_desc;
    uint32_t *desc_ptr       = &crypto_desc_ptr;
    status = sl_si91x_m4_ta_secure_handshake(SL_SI91X_ENABLE_SIDE_BAND, sizeof(uint32_t), (uint8_t *)desc_ptr, 0, NULL);
  }
#endif
#ifdef SLI_SI91X_MCU_INTERFACE
#if defined(SLI_SI917)
  if (status == SL_STATUS_OK) {
    /*Getting PTE CRC value to distinguish firmware 17 and 18 boards.*/
    sl_si91x_get_flash_efuse_data(&efuse_data, SL_SI91X_EFUSE_PTE_CRC);

    /*PTE FW version check.*/
    if (efuse_data.pte_crc == FIRMWARE_17_PTE_CRC_VALUE) {
      /* Enable Higher PWM RO Frequency Mode for PMU for FW17 boards*/
      RSI_IPMU_Set_Higher_Pwm_Ro_Frequency_Mode_to_PMU();
      /* Set the RETN_LDO voltage to 0.8V for FW17 boards*/
      RSI_IPMU_Retn_Voltage_To_Default();
    }
  }
#endif
#endif
  return status;
}

sl_status_t sl_wifi_set_antenna(sl_wifi_interface_t interface, sl_wifi_antenna_t antenna)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;
  sl_si91x_antenna_select_t rsi_antenna = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  // Antenna type
  rsi_antenna.antenna_value = (antenna == SL_WIFI_ANTENNA_EXTERNAL) ? SL_WIFI_SELECT_EXTERNAL_ANTENNA
                                                                    : SL_WIFI_SELECT_INTERNAL_ANTENNA;

  status = sl_si91x_driver_send_command(RSI_COMMON_REQ_ANTENNA_SELECT,
                                        SI91X_COMMON_CMD,
                                        &rsi_antenna,
                                        sizeof(rsi_antenna),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_wait_for_scan_results(sl_wifi_scan_result_t **scan_results, uint32_t max_scan_result_count)
{
  UNUSED_PARAMETER(scan_results);
  UNUSED_PARAMETER(max_scan_result_count);

#ifdef SI91x_ENABLE_WAIT_ON_RESULTS
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  status = sl_si91x_driver_wait_for_response(RSI_WLAN_RSP_SCAN, SL_SI91X_WAIT_FOR(3000));
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_si91x_host_remove_from_queue(SI91X_WLAN_RESPONSE_QUEUE, &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  status = convert_and_save_firmware_status(get_si91x_frame_status(packet));
  VERIFY_STATUS_AND_RETURN(status);

  if (packet->command == RSI_WLAN_RSP_SCAN) {
    *scan_results = (sl_wifi_scan_result_t *)malloc(packet->length);
    if (scan_results == NULL) {
      sl_si91x_host_free_buffer(buffer);
      return SL_STATUS_ALLOCATION_FAILED;
    }
    memcpy(*scan_results, packet->data, packet->length);
  }
  sl_si91x_host_free_buffer(buffer);
#endif

  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_start_scan(sl_wifi_interface_t interface,
                               const sl_wifi_ssid_t *optional_ssid,
                               const sl_wifi_scan_configuration_t *configuration)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if (SL_WIFI_SCAN_TYPE_ADV_SCAN != configuration->type) {
    sl_si91x_req_scan_t scan_request = { 0 };

    //! copying SSID & length
    if (optional_ssid != NULL) {
      memcpy(scan_request.ssid, optional_ssid->value, optional_ssid->length);
    }

    if ((interface & SL_WIFI_2_4GHZ_INTERFACE) && (configuration->channel_bitmap_2g4 != 0xFFFF)) {
      memcpy(&scan_request.channel_bit_map_2_4,
             &configuration->channel_bitmap_2g4,
             sizeof(scan_request.channel_bit_map_2_4));

      uint16_t channel_bitmap = configuration->channel_bitmap_2g4;
      // Set channel, if only one channel is selected in channel_bitmap
      // if channel is 0, scan on all channels.
      if ((channel_bitmap > 0) && (0 == (channel_bitmap & (channel_bitmap - 1)))) {
        for (uint8_t channel = 1; channel <= MAX_2_4G_CHANNEL; channel++) {
          if (BIT((channel - 1)) == channel_bitmap) {
            scan_request.channel[0] = channel;
            break;
          }
        }
      }
    }

    if (configuration->type == SL_WIFI_SCAN_TYPE_PASSIVE) {
      scan_request.pscan_bitmap[3] |= PASSIVE_SCAN_ENABLE;
    }
    if (configuration->lp_mode) {
      scan_request.pscan_bitmap[3] |= LP_CHAIN_ENABLE;
    }
    sl_wifi_max_tx_power_t wifi_max_tx_power = get_max_tx_power();
    // Within the 1-byte scan_feature_bimap variable, last 5 bits(bit 3 through bit 7) are allocated for
    // encoding the transmit power during scan procedure.
    scan_request.scan_feature_bitmap = (wifi_max_tx_power.scan_tx_power << 3);

    // Enable Quick Scan, if SSID and channel are available
    // Quick Scan is disabled, if channel is 0
    if ((optional_ssid != NULL) && (scan_request.channel[0] != 0)) {
      scan_request.scan_feature_bitmap |= QUICK_SCAN_ENABLE;
    }

    if (advanced_scan_configuration.active_channel_time != SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME) {
      sl_status_t status = sl_si91x_configure_timeout(SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT,
                                                      advanced_scan_configuration.active_channel_time);
      VERIFY_STATUS_AND_RETURN(status);
    }

    if (SL_WIFI_SCAN_TYPE_EXTENDED == configuration->type) {
      scan_request.scan_feature_bitmap |= SCAN_RESULTS_TO_HOST;
    }
    sli_wifi_flush_scan_results_database();

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SCAN,
                                          SI91X_WLAN_CMD,
                                          &scan_request,
                                          sizeof(scan_request),
                                          SL_SI91X_RETURN_IMMEDIATELY,
                                          NULL,
                                          NULL);
  } else {
    sl_si91x_req_bg_scan_t scan_request = { 0 };

    scan_request.bgscan_enable            = SI91X_BG_SCAN_ENABLE;
    scan_request.enable_instant_bgscan    = advanced_scan_configuration.enable_instant_scan;
    scan_request.passive_scan_duration    = advanced_scan_configuration.passive_channel_time;
    scan_request.active_scan_duration     = advanced_scan_configuration.active_channel_time;
    scan_request.bgscan_periodicity       = (uint16_t)configuration->periodic_scan_interval;
    scan_request.rssi_tolerance_threshold = (uint16_t)advanced_scan_configuration.trigger_level_change;
    scan_request.bgscan_threshold         = -1 * advanced_scan_configuration.trigger_level;
    scan_request.multi_probe              = advanced_scan_configuration.enable_multi_probe;

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_BG_SCAN,
                                          SI91X_WLAN_CMD,
                                          &scan_request,
                                          sizeof(sl_si91x_req_bg_scan_t),
                                          SL_SI91X_RETURN_IMMEDIATELY,
                                          NULL,
                                          NULL);
  }

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_get_stored_scan_results(sl_wifi_interface_t interface,
                                            sl_wifi_extended_scan_result_parameters_t *extended_scan_parameters)
{
  return sli_wifi_get_stored_scan_results(interface, extended_scan_parameters);
}

sl_status_t sl_wifi_connect(sl_wifi_interface_t interface,
                            const sl_wifi_client_configuration_t *ap,
                            uint32_t timeout_ms)
{
  sl_status_t status;
  sl_si91x_req_psk_t psk_request;
  sl_si91x_req_eap_config_t eap_req = { 0 };
  sl_si91x_join_request_t join_request;
  sl_wifi_buffer_t *buffer         = NULL;
  sl_si91x_packet_t *packet        = NULL;
  sl_si91x_req_scan_t scan_request = { 0 };
  uint8_t uid_len                  = 0;
  uint8_t psk_len                  = 0;
  uint8_t key_len                  = 0;
  sl_wifi_credential_t cred        = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(ap);
  memset(&scan_request, 0, sizeof(scan_request));
  //! copying SSID & length
  if (ap->ssid.length > 0) {
    memcpy(scan_request.ssid, ap->ssid.value, ap->ssid.length);
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Configure channel bitmap for selective channel scan

  if ((interface & SL_WIFI_CLIENT_2_4GHZ_INTERFACE) && (ap->channel_bitmap.channel_bitmap_2_4 > 0)) {
    memcpy(&scan_request.channel_bit_map_2_4,
           &ap->channel_bitmap.channel_bitmap_2_4,
           sizeof(scan_request.channel_bit_map_2_4));
  }

  if ((interface & SL_WIFI_CLIENT_5GHZ_INTERFACE) && (ap->channel_bitmap.channel_bitmap_5 > 0)) {
    memcpy(&scan_request.channel_bit_map_5,
           &ap->channel_bitmap.channel_bitmap_5,
           sizeof(scan_request.channel_bit_map_5));
  }

  sl_wifi_max_tx_power_t wifi_max_tx_power = get_max_tx_power();
  // Within the 1-byte scan_feature_bimap variable, last 5 bits(bit 3 through bit 7) are allocated for
  // encoding the transmit power during scan procedure.
  scan_request.scan_feature_bitmap = (wifi_max_tx_power.scan_tx_power << 3);

  if (advanced_scan_configuration.active_channel_time != SL_WIFI_DEFAULT_ACTIVE_CHANNEL_SCAN_TIME) {
    sl_status_t status =
      sl_si91x_configure_timeout(SL_SI91X_CHANNEL_ACTIVE_SCAN_TIMEOUT, advanced_scan_configuration.active_channel_time);
    VERIFY_STATUS_AND_RETURN(status);
  }
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SCAN,
                                        SI91X_WLAN_CMD,
                                        &scan_request,
                                        sizeof(scan_request),
                                        SL_SI91X_WAIT_FOR(60000),
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);

  if ((SL_WIFI_WPA_ENTERPRISE == ap->security) || (SL_WIFI_WPA2_ENTERPRISE == ap->security)
      || (SL_WIFI_WPA3_ENTERPRISE == ap->security) || (SL_WIFI_WPA3_TRANSITION_ENTERPRISE == ap->security)) {
    memset(&eap_req, 0, sizeof(eap_req));
    eap_req.user_identity[0] = '"';
    eap_req.password[0]      = '"';

    status = sl_si91x_host_get_credentials(ap->credential_id, SL_WIFI_EAP_CREDENTIAL, &cred);
    VERIFY_STATUS_AND_RETURN(status);

    memcpy(&(eap_req.user_identity[1]), cred.eap.username, 63);
    memcpy(&(eap_req.password[1]), cred.eap.password, 127);
    uid_len                            = strlen((char *)eap_req.user_identity);
    psk_len                            = strlen((char *)eap_req.password);
    eap_req.user_identity[uid_len]     = '"';
    eap_req.password[psk_len]          = '"';
    eap_req.user_identity[uid_len + 1] = 0;
    eap_req.password[psk_len + 1]      = 0;

    // copy enterprise configuration data
    if (SL_WIFI_EAP_TLS_ENCRYPTION == ap->encryption) {
      strcpy((char *)eap_req.eap_method, SL_EAP_TLS_METHOD);
    } else if (SL_WIFI_EAP_TTLS_ENCRYPTION == ap->encryption) {
      strcpy((char *)eap_req.eap_method, SL_EAP_TTLS_METHOD);
    } else if (SL_WIFI_EAP_FAST_ENCRYPTION == ap->encryption) {
      strcpy((char *)eap_req.eap_method, SL_EAP_FAST_METHOD);
    } else if (SL_WIFI_PEAP_MSCHAPV2_ENCRYPTION == ap->encryption) {
      strcpy((char *)eap_req.eap_method, SL_EAP_PEAP_METHOD);
    } else if (SL_WIFI_EAP_LEAP_ENCRYPTION == ap->encryption) {
      strcpy((char *)eap_req.eap_method, SL_EAP_LEAP_METHOD);
    } else {
      return SL_STATUS_WIFI_INVALID_ENCRYPTION_METHOD;
    }

    strcpy((char *)eap_req.inner_method, SL_EAP_INNER_METHOD);
    memcpy(eap_req.okc_enable, &cred.eap.eap_flags, sizeof(cred.eap.eap_flags));

    key_len = strlen((const char *)cred.eap.certificate_key);
    if ((key_len > 0)) {
      eap_req.private_key_password[0] = '"';
      strcpy((char *)eap_req.private_key_password, (const char *)cred.eap.certificate_key);
      eap_req.private_key_password[key_len + 1] = '"';
    } else {
      strcpy((char *)eap_req.private_key_password, SL_DEFAULT_PRIVATE_KEY_PASSWORD);
    }

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EAP_CONFIG,
                                          SI91X_WLAN_CMD,
                                          &eap_req,
                                          sizeof(eap_req),
                                          SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
  } else if ((SL_WIFI_WPA == ap->security) || (SL_WIFI_WPA2 == ap->security) || (SL_WIFI_WPA_WPA2_MIXED == ap->security)
             || (SL_WIFI_WPA3 == ap->security) || (SL_WIFI_WPA3_TRANSITION == ap->security)) {

    memset(&psk_request, 0, sizeof(psk_request));
    status = sl_si91x_host_get_credentials(ap->credential_id, SL_WIFI_PSK_CREDENTIAL, &cred);
    VERIFY_STATUS_AND_RETURN(status);
    psk_request.type = cred.type == SL_WIFI_PSK_CREDENTIAL ? 1 : 2;
    memcpy(psk_request.psk_or_pmk, cred.pmk.value, SL_WIFI_MAX_PMK_LENGTH);

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HOST_PSK,
                                          SI91X_WLAN_CMD,
                                          &psk_request,
                                          sizeof(psk_request),
                                          SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
  } else if (SL_WIFI_WEP == ap->security) {
    return SL_STATUS_NOT_SUPPORTED;
  } else if (SL_WIFI_OPEN != ap->security) {
    return SL_STATUS_WIFI_UNKNOWN_SECURITY_TYPE;
  }

  memset(&join_request, 0, sizeof(join_request));

  status = get_configured_join_request(SL_WIFI_CLIENT_INTERFACE, ap, &join_request);

  VERIFY_STATUS_AND_RETURN(status);

  status =
    sl_si91x_driver_send_command(RSI_WLAN_REQ_JOIN,
                                 SI91X_WLAN_CMD,
                                 &join_request,
                                 sizeof(join_request),
                                 timeout_ms ? SL_SI91X_WAIT_FOR_RESPONSE(timeout_ms) : SL_SI91X_RETURN_IMMEDIATELY,
                                 NULL,
                                 &buffer);

  if (timeout_ms != 0 && status != SL_STATUS_OK) {
    if (buffer != NULL) {
      sl_si91x_host_free_buffer(buffer);
    }
    sl_status_t temp_status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                                           SI91X_WLAN_CMD,
                                                           NULL,
                                                           0,
                                                           SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                           NULL,
                                                           NULL);
    VERIFY_STATUS_AND_RETURN(temp_status);
  }

  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  if (packet->data[0] != 'C') {
    sl_si91x_host_free_buffer(buffer);
    return SL_STATUS_NOT_AVAILABLE;
  }

  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_advanced_client_configuration(sl_wifi_interface_t interface,
                                                      const sl_wifi_advanced_client_configuration_t *configuration)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  sl_si91x_rejoin_params_t rejoin_request = { .max_retry_attempts      = configuration->max_retry_attempts,
                                              .scan_interval           = configuration->scan_interval,
                                              .beacon_missed_count     = configuration->beacon_missed_count,
                                              .first_time_retry_enable = configuration->first_time_retry_enable };

  if (rejoin_request.beacon_missed_count < 40) {
    SL_DEBUG_LOG("\r\nBeacon Missed Count minimum value should be 40, Updating to the minimum value.\r\n");
    rejoin_request.beacon_missed_count = 40;
  }

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_REJOIN_PARAMS,
                                                    SI91X_WLAN_CMD,
                                                    &rejoin_request,
                                                    sizeof(rejoin_request),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);

  memcpy(&advanced_client_configuration, configuration, sizeof(sl_wifi_advanced_client_configuration_t));

  return status;
}

sl_status_t sl_wifi_get_signal_strength(sl_wifi_interface_t interface, int32_t *rssi)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(rssi, SL_STATUS_WIFI_NULL_PTR_ARG);

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_RSSI,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  *rssi                     = -(packet->data[0]);
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_sta_tsf(sl_wifi_interface_t interface, sl_wifi_tsf64_t *tsf)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(tsf, SL_STATUS_WIFI_NULL_PTR_ARG);

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TSF,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(tsf, packet->data, packet->length);
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_mac_address(sl_wifi_interface_t interface, const sl_mac_address_t *mac_address)
{
  UNUSED_PARAMETER(interface);

  SL_VERIFY_POINTER_OR_RETURN(mac_address, SL_STATUS_NULL_POINTER);

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_MAC_ADDRESS,
                                                    SI91X_WLAN_CMD,
                                                    mac_address,
                                                    sizeof(sl_mac_address_t),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_mac_address(sl_wifi_interface_t interface, sl_mac_address_t *mac)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;

  SL_WIFI_ARGS_CHECK_NULL_POINTER(mac);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MAC_ADDRESS,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    // In Concurrent mode, for Client Interface, mac address will be at offset 0
    uint8_t mac_address_offset = 0;
    if ((SL_SI91X_CONCURRENT_MODE == get_opermode()) && (SL_WIFI_AP_INTERFACE == interface)) {
      // In Concurrent mode, for AP Interface, mac address will be at offset 6
      mac_address_offset = sizeof(sl_mac_address_t);
    }
    memcpy(mac->octet, packet->data + mac_address_offset, sizeof(*mac));
  }
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_channel(sl_wifi_interface_t interface, sl_wifi_channel_t channel)
{
  UNUSED_PARAMETER(channel);
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  // Firmware doesn't support to change the channel of a running Access Point interface.
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_get_channel(sl_wifi_interface_t interface, sl_wifi_channel_t *channel_info)
{
  sl_status_t status             = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer       = NULL;
  rsi_wlan_cmd_request_t command = 0;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((interface == SL_WIFI_CLIENT_2_4GHZ_INTERFACE) || (interface == SL_WIFI_CLIENT_5GHZ_INTERFACE))
    command = RSI_WLAN_REQ_QUERY_NETWORK_PARAMS;
  else if ((interface == SL_WIFI_AP_2_4GHZ_INTERFACE) || (interface == SL_WIFI_AP_5GHZ_INTERFACE))
    command = RSI_WLAN_REQ_QUERY_GO_PARAMS;

  status = sl_si91x_driver_send_command(command,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_GET_CHANNEL_TIMEOUT),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  switch (interface) {
    case SL_WIFI_CLIENT_2_4GHZ_INTERFACE: {
      channel_info->channel = ((sl_si91x_network_params_response_t *)packet->data)->channel_number;
      channel_info->band    = SL_WIFI_BAND_2_4GHZ;
      break;
    }
    case SL_WIFI_AP_2_4GHZ_INTERFACE: {
      memcpy(&channel_info->channel, ((sl_si91x_client_info_response *)packet->data)->channel_number, 2);
      channel_info->band = SL_WIFI_BAND_2_4GHZ;
      break;
    }
    default:
      break;
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

/*
 * This API doesn't have any affect if it is called after connect/start ap.
 */
sl_status_t sl_wifi_set_max_tx_power(sl_wifi_interface_t interface, sl_wifi_max_tx_power_t max_tx_power)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (
    (interface & SL_WIFI_CLIENT_INTERFACE) // For the client interface, it’s necessary to check if the interface is up. However, for the AP interface, this check isn’t required since this API will be called before starting the AP.
    && (!sl_wifi_is_interface_up(interface))) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  uint8_t max_scan_tx_power = max_tx_power.scan_tx_power;
  uint8_t max_join_tx_power = max_tx_power.join_tx_power;

  if ((max_scan_tx_power < 1 || max_scan_tx_power > 31) || (max_join_tx_power < 1 || max_join_tx_power > 31)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  save_max_tx_power(max_scan_tx_power, max_join_tx_power);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_max_tx_power(sl_wifi_interface_t interface, sl_wifi_max_tx_power_t *max_tx_power)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(max_tx_power);

  *max_tx_power = get_max_tx_power();

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_start_ap(sl_wifi_interface_t interface, const sl_wifi_ap_configuration_t *configuration)
{
  sl_status_t status                   = SL_STATUS_OK;
  sl_wifi_buffer_t *rx_buffer          = NULL;
  sl_si91x_packet_t *join_response     = NULL;
  sl_si91x_ap_config_request request   = { 0 };
  sl_si91x_join_request_t join_request = { 0 };
  sl_wifi_credential_t cred            = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  memcpy(request.ssid, configuration->ssid.value, configuration->ssid.length);
  request.security_type = configuration->security;

  //This encryption convertions is only required in access point mode
  status = convert_sl_wifi_to_sl_si91x_encryption(configuration->encryption, &request.encryption_mode);
  VERIFY_STATUS_AND_RETURN(status);

  //Configures TDI[7-4] value here
  if ((configuration->security == SL_WIFI_WPA3) || (configuration->security == SL_WIFI_WPA3_TRANSITION)) {
    request.encryption_mode |= configuration->tdi_flags & 0xF0;
  }
  status = sl_si91x_host_get_credentials(configuration->credential_id, SL_WIFI_PSK_CREDENTIAL, &cred);
  VERIFY_STATUS_AND_RETURN(status);

  memcpy(request.psk, cred.psk.value, sizeof(request.psk));

  request.channel         = configuration->channel.channel;
  request.beacon_interval = configuration->beacon_interval;
  request.dtim_period     = configuration->dtim_beacon_count;
  request.max_sta_support = configuration->maximum_clients;
  if (configuration->keepalive_type) {
    request.ap_keepalive_type   = configuration->keepalive_type;
    request.ap_keepalive_period = configuration->client_idle_timeout;
  }
  if (configuration->beacon_stop) {
    // Using a free bit in ap_keepalive_type since there are no available bits in join feature bitmap.
    request.ap_keepalive_type |= BIT(2);
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_AP_CONFIGURATION,
                                        SI91X_WLAN_CMD,
                                        &request,
                                        sizeof(request),
                                        SL_SI91X_WAIT_FOR(15000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  if (configuration->is_11n_enabled) {
    sl_si91x_request_ap_high_throughput_capability_t htcaps = { 0 };
    htcaps.mode_11n_enable                                  = true;
    htcaps.ht_caps_bitmap = SL_WIFI_HT_CAPS_NUM_RX_STBC | SL_WIFI_HT_CAPS_SHORT_GI_20MHZ | SL_WIFI_HT_CAPS_GREENFIELD_EN
                            | SL_WIFI_HT_CAPS_SUPPORT_CH_WIDTH;
    status = sli_si91x_set_high_throughput_capability(SL_WIFI_AP_INTERFACE, htcaps);
    VERIFY_STATUS_AND_RETURN(status);
  }

  status = get_configured_join_request(SL_WIFI_AP_INTERFACE, configuration, &join_request);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_JOIN,
                                        SI91X_WLAN_CMD,
                                        &join_request,
                                        sizeof(join_request),
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &rx_buffer);
  if ((status != SL_STATUS_OK) && (rx_buffer != NULL)) {
    sl_si91x_host_free_buffer(rx_buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  join_response = sl_si91x_host_get_buffer_data(rx_buffer, 0, NULL);

  if (join_response->data[0] != 'G') {
    sl_si91x_host_free_buffer(rx_buffer);
    return SL_STATUS_NOT_AVAILABLE;
  }

  save_ap_configuration(configuration);
  if (interface == SL_WIFI_AP_2_4GHZ_INTERFACE || interface == SL_WIFI_AP_INTERFACE)
    interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX] = true;
  else if (interface == SL_WIFI_AP_5GHZ_INTERFACE)
    interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX] = true;

  sl_si91x_host_free_buffer(rx_buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_pairwise_master_key(sl_wifi_interface_t interface,
                                            const uint8_t type,
                                            const sl_wifi_ssid_t *ssid,
                                            const char *pre_shared_key,
                                            uint8_t *pairwise_master_key)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;
  sl_wifi_buffer_t *buffer                       = NULL;
  sl_si91x_req_psk_t pairwise_master_key_request = { 0 };

  if (pairwise_master_key == NULL) {
    return SL_STATUS_WIFI_NULL_PTR_ARG;
  }

  pairwise_master_key_request.type = type;
  memcpy(pairwise_master_key_request.psk_or_pmk, pre_shared_key, strlen(pre_shared_key));
  memcpy(pairwise_master_key_request.ap_ssid, ssid->value, ssid->length);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HOST_PSK,
                                        SI91X_WLAN_CMD,
                                        &pairwise_master_key_request,
                                        sizeof(sl_si91x_req_psk_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(35000),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(pairwise_master_key, packet->data, packet->length);
  sl_si91x_host_free_buffer(buffer);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_associated_client_list(void *client_list_buffer, uint16_t buffer_length, uint32_t timeout)
{
  UNUSED_PARAMETER(client_list_buffer);
  UNUSED_PARAMETER(buffer_length);
  UNUSED_PARAMETER(timeout);
  return SL_STATUS_NOT_SUPPORTED;
  // sl_status_t status;
  // sl_wifi_buffer_t *buffer;

  // status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_GO_PARAMS,
  //                                       SI91X_WLAN_CMD,
  //                                       NULL,
  //                                       0,
  //                                       SL_SI91X_WAIT_FOR_RESPONSE(10000),
  //                                       NULL,
  //                                       &buffer);
  // if (status == SL_STATUS_OK) {
  //   sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  //       //  si91x_go_parameters_t* data = &packet->data;
  //   // Process data here

  //   sl_si91x_host_free_buffer(buffer);
  // }
  // return status;
}

sl_status_t sl_wifi_disconnect_ap_client(sl_wifi_interface_t interface,
                                         const sl_mac_address_t *mac,
                                         sl_wifi_deauth_reason_t reason)
{
  UNUSED_PARAMETER(reason);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(mac);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_si91x_disassociation_request_t disconnect_request = { 0 };

  disconnect_request.mode_flag = SL_SI91X_WIFI_AP_VAP_ID;
  memcpy(&disconnect_request.client_mac_address, mac, sizeof(sl_mac_address_t));

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DISCONNECT,
                                                    SI91X_WLAN_CMD,
                                                    &disconnect_request,
                                                    sizeof(disconnect_request),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    (void *)mac,
                                                    NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_get_ap_client_info(sl_wifi_interface_t interface, sl_wifi_client_info_response_t *client_info)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer = NULL;
  sl_si91x_packet_t *packet;
  sl_si91x_client_info_response sl_si91x_client_info_response = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(SL_WIFI_AP_INTERFACE)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(client_info);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_GO_PARAMS,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  memcpy(&sl_si91x_client_info_response, packet->data, sizeof(sl_si91x_client_info_response));
  convert_si91x_wifi_client_info(client_info, &sl_si91x_client_info_response);

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_get_firmware_version(sl_wifi_firmware_version_t *version)
{
  return sl_si91x_get_firmware_version((sl_si91x_firmware_version_t *)version);
}

sl_status_t sl_wifi_get_wireless_info(sl_si91x_rsp_wireless_info_t *info)
{
  sl_status_t status       = 0;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  SL_WIFI_ARGS_CHECK_NULL_POINTER(info);

  if (get_opermode() == SL_SI91X_ACCESS_POINT_MODE) {
    // Send cmd for wlan info in AP mode
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_GO_PARAMS,
                                          SI91X_WLAN_CMD,
                                          NULL,
                                          0,
                                          SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                          NULL,
                                          &buffer);
  } else if ((get_opermode() == SL_SI91X_CLIENT_MODE) || (get_opermode() == SL_SI91X_ENTERPRISE_CLIENT_MODE)) {
    //! Send cmd for wlan info in client mode
    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS,
                                          SI91X_WLAN_CMD,
                                          NULL,
                                          0,
                                          SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                          NULL,
                                          &buffer);
  } else {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  memset((sl_si91x_rsp_wireless_info_t *)info, 0, sizeof(sl_si91x_rsp_wireless_info_t));

  //In AP mode, receives a buffer equivalent to sl_si91x_client_info_response.
  if (packet->length > 0 && get_opermode() == SL_SI91X_ACCESS_POINT_MODE) {
    sl_si91x_client_info_response *response = (sl_si91x_client_info_response *)packet->data;
    // wlan state: no of stations connected in AP mode
    memcpy(&info->wlan_state, (uint16_t *)&response->sta_count, sizeof(uint16_t));
    memcpy(&info->channel_number, (uint16_t *)&response->channel_number, sizeof(uint16_t));
    memcpy(info->ssid, response->ssid, MIN(sizeof(info->ssid), sizeof(response->ssid)));
    memcpy(info->mac_address, response->mac_address, 6);
    // PSK for AP mode, PMK for Client mode
    memcpy(info->psk_pmk, response->psk, 64);
    memcpy(info->ipv4_address, response->ipv4_address, 4);
    memcpy(info->ipv6_address, response->ipv6_address, 16);
  }
  //In Client mode, receives a buffer equivalent to sl_si91x_network_params_response_t.
  else if (packet->length > 0
           && ((get_opermode() == SL_SI91X_CLIENT_MODE) || (get_opermode() == SL_SI91X_ENTERPRISE_CLIENT_MODE))) {
    sl_si91x_network_params_response_t *response = (sl_si91x_network_params_response_t *)packet->data;
    memcpy(&info->wlan_state, (uint16_t *)&response->wlan_state, sizeof(uint8_t));
    memcpy((uint8_t *)&info->channel_number, &response->channel_number, sizeof(uint8_t));
    memcpy(info->ssid, response->ssid, MIN(sizeof(info->ssid), sizeof(response->ssid)));
    memcpy(info->mac_address, response->mac_address, 6);
    memcpy(&info->sec_type, &response->sec_type, sizeof(uint8_t));
    // PSK for AP mode, PMK for Client mode
    memcpy(info->psk_pmk, response->psk, 64);
    memcpy(info->ipv4_address, response->ipv4_address, 4);
    memcpy(info->ipv6_address, response->ipv6_address, 16);
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_get_firmware_size(void *buffer, uint32_t *fw_image_size)
{
  return sl_si91x_get_firmware_size(buffer, fw_image_size);
}

sl_status_t sl_wifi_disconnect(sl_wifi_interface_t interface)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_si91x_disassociation_request_t disconnect_request = { 0 };

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DISCONNECT,
                                                    SI91X_WLAN_CMD,
                                                    &disconnect_request,
                                                    sizeof(disconnect_request),
                                                    SL_NCP_DEFAULT_COMMAND_WAIT_TIME,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);

  reset_coex_current_performance_profile();

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sl_wifi_stop_ap(sl_wifi_interface_t interface)
{
  sl_status_t status                                   = SL_STATUS_OK;
  sl_si91x_disassociation_request_t disconnect_request = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  disconnect_request.mode_flag = SL_SI91X_WIFI_AP_VAP_ID;
  status                       = sl_si91x_driver_send_command(RSI_WLAN_REQ_AP_STOP,
                                        SI91X_WLAN_CMD,
                                        &disconnect_request,
                                        sizeof(disconnect_request),
                                        SL_NCP_DEFAULT_COMMAND_WAIT_TIME,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  reset_ap_configuration();
  if (interface == SL_WIFI_AP_2_4GHZ_INTERFACE || interface == SL_WIFI_AP_INTERFACE)
    interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX] = false;
  else if (interface == SL_WIFI_AP_5GHZ_INTERFACE)
    interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX] = false;

  return status;
}

sl_status_t sl_wifi_get_statistics(sl_wifi_interface_t interface, sl_wifi_statistics_t *statistics)
{
  sl_status_t status       = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer = NULL;
  sl_si91x_packet_t *packet;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_INVALID_INTERFACE(interface);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(statistics);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EXT_STATS,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(30500),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length != sizeof(sl_wifi_statistics_t)) {
    sl_si91x_host_free_buffer(buffer);
    return SL_STATUS_FAIL;
  }

  if (packet->length > 0) {
    memcpy(statistics, packet->data, packet->length);
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_get_operational_statistics(sl_wifi_interface_t interface,
                                               sl_wifi_operational_statistics_t *operational_statistics)
{
  sl_status_t status       = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer = NULL;
  sl_si91x_packet_t *packet;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_INVALID_INTERFACE(interface);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(operational_statistics);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_GET_STATS,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(30500),
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length != sizeof(sl_wifi_operational_statistics_t)) {
    sl_si91x_host_free_buffer(buffer);
    return SL_STATUS_FAIL;
  }

  if (packet->length > 0) {
    memcpy(operational_statistics, packet->data, packet->length);
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_start_statistic_report(sl_wifi_interface_t interface, sl_wifi_channel_t channel)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_status_t status               = SL_STATUS_OK;
  sl_si91x_req_rx_stats_t rx_stats = { 0 };

  // Configure to start RX stats
  rx_stats.start[0] = START_STATISTICS_REPORT;
  // Copy the channel number
  memcpy(rx_stats.channel, &channel.channel, sizeof(rx_stats.channel));
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_RX_STATS,
                                        SI91X_WLAN_CMD,
                                        &rx_stats,
                                        sizeof(rx_stats),
                                        SL_SI91X_RETURN_IMMEDIATELY,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_stop_statistic_report(sl_wifi_interface_t interface)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_si91x_req_rx_stats_t rx_stats = { 0 };

  // Configure to stop RX stats
  rx_stats.start[0] = STOP_STATISTICS_REPORT;

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_RX_STATS,
                                                    SI91X_WLAN_CMD,
                                                    &rx_stats,
                                                    sizeof(rx_stats),
                                                    SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_set_performance_profile(const sl_wifi_performance_profile_t *profile)
{
  sl_status_t status;
  sl_si91x_performance_profile_t selected_coex_profile_mode = { 0 };
  sl_wifi_performance_profile_t current_wifi_profile_mode   = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(profile);

  if (profile->profile > DEEP_SLEEP_WITH_RAM_RETENTION) {
    return SL_STATUS_INVALID_MODE;
  }

  // Take backup of current wifi profile
  get_wifi_current_performance_profile(&current_wifi_profile_mode);

  // Send the power save command for the requested profile
  status = sli_si91x_send_power_save_request(profile, NULL);
  if (status != SL_STATUS_OK) {
    save_wifi_current_performance_profile(&current_wifi_profile_mode);
    return status;
  }
  get_coex_performance_profile(&selected_coex_profile_mode);

  // Set device_initialized as false since RAM of module will be not retained
  // in ULTRA_POWER_SAVE and module needs to be started from init again
  if (selected_coex_profile_mode == DEEP_SLEEP_WITHOUT_RAM_RETENTION) {
    device_initialized = false;

#ifdef SLI_SI91X_MCU_INTERFACE
    // In soc mode m4 does not get the card ready for next init after deinit, but if device in DEEP_SLEEP_WITHOUT_RAM_RETENTION mode, m4 should wait for card ready for next init
    set_card_ready_required(true);
#endif
    reset_coex_current_performance_profile();
  }

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_performance_profile(sl_wifi_performance_profile_t *profile)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  get_wifi_current_performance_profile(profile);
  return SL_STATUS_OK;
}

sl_wifi_interface_t sl_wifi_get_default_interface(void)
{
  return default_interface;
}

void sl_wifi_set_default_interface(sl_wifi_interface_t interface)
{
  default_interface = interface;
}

sl_status_t sl_wifi_deinit(void)
{
  sl_status_t status;
  bg_enabled = false;
  reset_coex_current_performance_profile();
  reset_max_tx_power();
  reset_ap_configuration();
  reset_sl_wifi_rate();
  memset(&advanced_scan_configuration, 0, sizeof(sl_wifi_advanced_scan_configuration_t));
  status = sl_si91x_driver_deinit();
  sli_wifi_flush_scan_results_database();

  SLI_NETWORK_CLEANUP_HANDLER();

  return status;
}

// 5GHz interface is currently not supported for Si91x
bool sl_wifi_is_interface_up(sl_wifi_interface_t interface)
{
  switch (interface) {
    case SL_WIFI_CLIENT_INTERFACE:
      return interface_is_up[SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX]
             | interface_is_up[SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX];
    case SL_WIFI_AP_INTERFACE:
      return interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX] | interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX];
    case SL_WIFI_CLIENT_2_4GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX];
    case SL_WIFI_AP_2_4GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX];
    case SL_WIFI_CLIENT_5GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX];
    case SL_WIFI_AP_5GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX];
    case SL_WIFI_2_4GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX]
             | interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX];
    case SL_WIFI_5GHZ_INTERFACE:
      return interface_is_up[SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX] | interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX];
    case SL_WIFI_ALL_INTERFACES:
      return interface_is_up[SL_WIFI_CLIENT_5GHZ_INTERFACE_INDEX] | interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX]
             | interface_is_up[SL_WIFI_CLIENT_2_4GHZ_INTERFACE_INDEX]
             | interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX];
    case SL_WIFI_TRANSCEIVER_INTERFACE:
      return interface_is_up[SL_WIFI_TRANSCEIVER_INTERFACE_INDEX];
    default:
      return false;
  }
}

sl_status_t sl_wifi_set_certificate_with_index(uint8_t certificate_type,
                                               uint8_t certificate_index,
                                               uint8_t *buffer,
                                               uint32_t certificate_length)
{
  return sl_si91x_wifi_set_certificate_index(certificate_type, certificate_index, buffer, certificate_length);
}

sl_status_t sl_wifi_set_certificate(uint8_t certificate_type, const uint8_t *buffer, uint32_t certificate_length)
{
  return sl_si91x_wifi_set_certificate_index(certificate_type, 0, buffer, certificate_length);
}

sl_status_t sl_wifi_set_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t rate_protocol,
                                      sl_wifi_rate_t mask)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (sl_wifi_is_interface_up(interface)) {
    if (interface == SL_WIFI_AP_INTERFACE) {
      return SL_STATUS_WIFI_UNSUPPORTED;
    }
  } else {
    if (interface == SL_WIFI_CLIENT_INTERFACE) {
      return SL_STATUS_WIFI_INTERFACE_NOT_UP;
    }
  }

  switch (rate_protocol) {
    case SL_WIFI_RATE_PROTOCOL_B_ONLY: {
      if (mask < SL_WIFI_RATE_11B_MIN || mask > SL_WIFI_RATE_11B_MAX) {
        return SL_STATUS_INVALID_CONFIGURATION;
      }
      break;
    }
    case SL_WIFI_RATE_PROTOCOL_G_ONLY: {
      if (mask < SL_WIFI_RATE_11G_MIN || mask > SL_WIFI_RATE_11G_MAX) {
        return SL_STATUS_INVALID_CONFIGURATION;
      }
      break;
    }
    case SL_WIFI_RATE_PROTOCOL_N_ONLY: {
      if (mask < SL_WIFI_RATE_11N_MIN || mask > SL_WIFI_RATE_11N_MAX) {
        return SL_STATUS_INVALID_CONFIGURATION;
      }
      break;
    }
    case SL_WIFI_RATE_PROTOCOL_AC_ONLY: {
      return SL_STATUS_NOT_SUPPORTED;
    }
    case SL_WIFI_RATE_PROTOCOL_AX_ONLY: {
      if (mask < SL_WIFI_RATE_11AX_MIN || mask > SL_WIFI_RATE_11AX_MAX) {
        return SL_STATUS_INVALID_CONFIGURATION;
      }
      break;
    }
    case SL_WIFI_RATE_PROTOCOL_AUTO: {
      if (mask != SL_WIFI_AUTO_RATE) {
        return SL_STATUS_INVALID_CONFIGURATION;
      }
      break;
    }
    default: {
      return SL_STATUS_INVALID_CONFIGURATION;
    }
  }

  return save_sl_wifi_rate(mask);
}

sl_status_t sl_wifi_get_transmit_rate(sl_wifi_interface_t interface,
                                      sl_wifi_rate_protocol_t *rate_protocol,
                                      sl_wifi_rate_t *mask)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(rate_protocol);
  SL_WIFI_ARGS_CHECK_NULL_POINTER(mask);

  sl_wifi_rate_t transfer_rate;
  get_saved_sl_wifi_rate(&transfer_rate);

  return get_rate_protocol_and_data_rate(transfer_rate, rate_protocol, mask);
}

sl_status_t sl_wifi_get_ap_client_count(sl_wifi_interface_t interface, uint32_t *client_list_count)
{
  sl_wifi_client_info_response_t client_info = { 0 };

  SL_WIFI_ARGS_CHECK_NULL_POINTER(client_list_count);

  sl_status_t status = sl_wifi_get_ap_client_info(interface, &client_info);

  if (status == SL_STATUS_OK) {
    memcpy(client_list_count, (uint32_t *)&client_info.client_count, sizeof(uint32_t));
  }

  return status;
}

sl_status_t sl_wifi_get_ap_client_list(sl_wifi_interface_t interface,
                                       uint16_t client_list_count,
                                       sl_mac_address_t *client_list)
{
  sl_wifi_client_info_response_t client_info = { 0 };

  sl_status_t status = sl_wifi_get_ap_client_info(interface, &client_info);

  if (status == SL_STATUS_OK) {
    for (uint16_t station_info_index = 0;
         station_info_index < client_info.client_count && station_info_index < client_list_count;
         station_info_index++) {
      memcpy(client_list[station_info_index].octet,
             client_info.client_info[station_info_index].mac_adddress.octet,
             sizeof(sl_mac_address_t));
    }
  }

  return status;
}
sl_status_t sl_wifi_generate_wps_pin(sl_wifi_wps_pin_t *wps_pin)
{
  sl_status_t status                               = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer                         = NULL;
  sl_si91x_packet_t *packet                        = NULL;
  sl_si91x_wps_method_request_t wps_method_request = { 0 };

  SL_WIFI_ARGS_CHECK_NULL_POINTER(wps_pin);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  // Set configuration to generate new WPS pin
  wps_method_request.wps_method   = SI91X_SET_WPS_METHOD_PIN;
  wps_method_request.generate_pin = SI91X_SET_WPS_GENERATE_PIN;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_WPS_METHOD,
                                        SI91X_WLAN_CMD,
                                        &wps_method_request,
                                        sizeof(sl_si91x_wps_method_request_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                        NULL,
                                        &buffer);
  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(wps_pin->digits, packet->data, sizeof(sl_wifi_wps_pin_t));
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_start_wps(sl_wifi_interface_t interface,
                              sl_wifi_wps_mode_t mode,
                              const sl_wifi_wps_pin_t *optional_wps_pin)
{
  UNUSED_PARAMETER(optional_wps_pin);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (mode != SL_WIFI_WPS_PUSH_BUTTON_MODE || (interface & SL_WIFI_AP_INTERFACE) == 0) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_status_t status;
  sl_si91x_join_request_t wps_button_press_request = { 0 };
  sl_wifi_ap_configuration_t ap_configuration      = { 0 };

  get_saved_ap_configuration(&ap_configuration);
  status = get_configured_join_request(SL_WIFI_AP_INTERFACE, &ap_configuration, &wps_button_press_request);

  VERIFY_STATUS_AND_RETURN(status);
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_JOIN,
                                        SI91X_WLAN_CMD,
                                        &wps_button_press_request,
                                        sizeof(wps_button_press_request),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}
sl_status_t sl_wifi_set_roam_configuration(sl_wifi_interface_t interface,
                                           sl_wifi_roam_configuration_t *roam_configuration)
{
  sl_si91x_req_roam_params_t roam_param_request = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if (roam_configuration->trigger_level != SL_WIFI_NEVER_ROAM) {
    roam_param_request.roam_enable = ALWAYS_ROAM;
  }

  roam_param_request.roam_threshold  = -1 * roam_configuration->trigger_level;
  roam_param_request.roam_hysteresis = roam_configuration->trigger_level_change;

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_ROAM_PARAMS,
                                                    SI91X_WLAN_CMD,
                                                    &roam_param_request,
                                                    sizeof(sl_si91x_req_roam_params_t),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_set_advanced_scan_configuration(const sl_wifi_advanced_scan_configuration_t *configuration)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(configuration);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  memcpy(&advanced_scan_configuration, configuration, sizeof(sl_wifi_advanced_scan_configuration_t));

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_advanced_scan_configuration(sl_wifi_advanced_scan_configuration_t *configuration)
{
  SL_WIFI_ARGS_CHECK_NULL_POINTER(configuration);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  memcpy(configuration, &advanced_scan_configuration, sizeof(sl_wifi_advanced_scan_configuration_t));

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_stop_scan(sl_wifi_interface_t interface)
{
  sl_status_t status                  = SL_STATUS_OK;
  sl_si91x_req_bg_scan_t scan_request = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  // Once stop_scan() support for foreground scan is available, "bg_enabled" should be removed.
  if (bg_enabled == true) {
    scan_request.bgscan_enable = SI91X_BG_SCAN_DISABLE;
    status                     = sl_si91x_driver_send_command(RSI_WLAN_REQ_BG_SCAN,
                                          SI91X_WLAN_CMD,
                                          &scan_request,
                                          sizeof(sl_si91x_req_bg_scan_t),
                                          SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
    bg_enabled = false;
  } else {
    return SL_STATUS_NOT_SUPPORTED;
  }

  return status;
}

sl_status_t sl_wifi_set_ap_configuration(sl_wifi_interface_t interface, const sl_wifi_ap_configuration_t *configuration)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(configuration);
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  //Firmware unable to configure the ap configuration of a running AP interface
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_get_ap_configuration(sl_wifi_interface_t interface, sl_wifi_ap_configuration_t *configuration)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(configuration, SL_STATUS_NULL_POINTER);

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_wifi_ap_configuration_t saved_ap_configuration = { 0 };
  get_saved_ap_configuration(&saved_ap_configuration);

  memcpy(configuration, &saved_ap_configuration, sizeof(sl_wifi_ap_configuration_t));

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_reconfigure_ap(sl_wifi_interface_t interface, sl_si91x_ap_reconfiguration_t config)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_BEACON_STOP,
                                        SI91X_WLAN_CMD,
                                        &config,
                                        sizeof(sl_si91x_ap_reconfiguration_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sl_wifi_test_client_configuration(sl_wifi_interface_t interface,
                                              const sl_wifi_client_configuration_t *ap,
                                              uint32_t timeout_ms)
{
  sl_status_t status = sl_wifi_connect(interface, ap, timeout_ms);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_wifi_disconnect(SL_WIFI_CLIENT_INTERFACE);
  VERIFY_STATUS_AND_RETURN(status);

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_send_raw_data_frame(sl_wifi_interface_t interface, const void *data, uint16_t data_length)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_VERIFY_POINTER_OR_RETURN(data, SL_STATUS_NULL_POINTER);

  if (data_length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  return sl_si91x_driver_raw_send_command(RSI_SEND_RAW_DATA, data, data_length, RSI_SEND_RAW_DATA_RESPONSE_WAIT_TIME);
}

sl_status_t sl_wifi_enable_target_wake_time(sl_wifi_twt_request_t *twt_req)
{
  if (!twt_req->twt_enable) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->twt_flow_id > MAX_FLOW_ID) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->wake_int_exp > MAX_WAKE_INTERVAL_EXPONENT) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->wake_int_exp_tol > MAX_WAKE_INTERVAL_EXPONENT_TOLERANCE) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->wake_duration_unit > MAX_WAKE_DURATION_UNIT) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->un_announced_twt > 1 || twt_req->triggered_twt > 1 || twt_req->restrict_tx_outside_tsp > 1) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->twt_retry_limit > MAX_TWT_RETRY_LIMIT) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->twt_retry_interval < MIN_TWT_RETRY_INTERVAL) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_req->req_type > MAX_TWT_REQ_TYPE) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((twt_req->req_type != REQUEST_TWT)
      && (twt_req->wake_duration == 0 || twt_req->wake_int_mantissa == 0
          || ((uint32_t)twt_req->wake_duration
              * (uint32_t)(twt_req->wake_duration_unit ? TWT_WAKE_DURATION_UNIT_1024TU : TWT_WAKE_DURATION_UNIT_256TU))
               > ((uint64_t)((twt_req->wake_int_mantissa) * ((uint64_t)1 << twt_req->wake_int_exp))))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((twt_req->twt_channel != 0) || (twt_req->twt_protection != 0) || (twt_req->implicit_twt != 1)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TWT_PARAMS,
                                                    SI91X_WLAN_CMD,
                                                    twt_req,
                                                    sizeof(sl_wifi_twt_request_t),
                                                    SL_SI91X_WAIT_FOR(35000),
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_target_wake_time_auto_selection(sl_wifi_twt_selection_t *twt_auto_request)
{
  if ((twt_auto_request->twt_enable != 0) && (twt_auto_request->twt_enable != 1)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((twt_auto_request->twt_enable == 1)
      && ((twt_auto_request->rx_latency > MAX_TX_AND_RX_LATENCY_LIMIT)
          || (twt_auto_request->tx_latency > MAX_TX_AND_RX_LATENCY_LIMIT))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  twt_auto_request->rx_latency = (twt_auto_request->rx_latency == 0) ? 2000 : twt_auto_request->rx_latency;
  if ((twt_auto_request->rx_latency < 2000)
      || (twt_auto_request->average_tx_throughput > (DEVICE_AVERAGE_THROUGHPUT / 2))) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((twt_auto_request->tx_latency < 200) && (twt_auto_request->tx_latency != 0)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TWT_AUTO_CONFIG,
                                                    SI91X_WLAN_CMD,
                                                    twt_auto_request,
                                                    sizeof(sl_wifi_twt_selection_t),
                                                    SL_SI91X_WAIT_FOR(35000),
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_disable_target_wake_time(sl_wifi_twt_request_t *twt_req)
{
  if ((!twt_req->twt_enable) && ((twt_req->twt_flow_id == 0xFF) || (twt_req->twt_flow_id <= 7))) {
    sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TWT_PARAMS,
                                                      SI91X_WLAN_CMD,
                                                      twt_req,
                                                      sizeof(sl_wifi_twt_request_t),
                                                      SL_SI91X_WAIT_FOR(35000),
                                                      NULL,
                                                      NULL);
    VERIFY_STATUS_AND_RETURN(status);
    return status;
  }

  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t sl_wifi_reschedule_twt(uint8_t flow_id,
                                   sl_wifi_reschedule_twt_action_t twt_action,
                                   uint64_t suspend_duration)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (flow_id > MAX_FLOW_ID) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((twt_action == SL_WIFI_SUSPEND_INDEFINITELY || twt_action == SL_WIFI_RESUME_IMMEDIATELY)
      && suspend_duration > 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_action == SL_WIFI_SUSPEND_FOR_DURATION
      && (suspend_duration < 1 || suspend_duration > MAX_TWT_SUSPEND_DURATION)) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (twt_action != SL_WIFI_SUSPEND_INDEFINITELY && twt_action != SL_WIFI_SUSPEND_FOR_DURATION
      && twt_action != SL_WIFI_RESUME_IMMEDIATELY) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sl_wifi_reschedule_twt_config_t reschedule_twt_config = { 0 };
  reschedule_twt_config.flow_id                         = flow_id;
  reschedule_twt_config.twt_action                      = twt_action;
  reschedule_twt_config.suspend_duration                = suspend_duration;
  sl_status_t status                                    = sl_si91x_driver_send_command(SL_WIFI_REQ_RESCHEDULE_TWT,
                                                    SI91X_WLAN_CMD,
                                                    &reschedule_twt_config,
                                                    sizeof(sl_wifi_reschedule_twt_config_t),
                                                    SL_SI91X_WAIT_FOR(35000),
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_filter_broadcast(uint16_t beacon_drop_threshold,
                                     uint8_t filter_bcast_in_tim,
                                     uint8_t filter_bcast_tim_till_next_cmd)
{
  sl_status_t status                                       = SL_STATUS_OK;
  sl_si91x_request_wlan_filter_broadcast_t sl_filter_bcast = { 0 };
  sl_filter_bcast.beacon_drop_threshold[0]                 = beacon_drop_threshold & 0x00FF;
  sl_filter_bcast.beacon_drop_threshold[1]                 = (beacon_drop_threshold >> 8) & 0x00FF;
  sl_filter_bcast.filter_bcast_in_tim                      = filter_bcast_in_tim;
  sl_filter_bcast.filter_bcast_tim_till_next_cmd           = filter_bcast_tim_till_next_cmd;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_FILTER_BCAST_PACKETS,
                                        SI91X_WLAN_CMD,
                                        &sl_filter_bcast,
                                        sizeof(sl_si91x_request_wlan_filter_broadcast_t),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_update_gain_table(uint8_t band, uint8_t bandwidth, uint8_t *payload, uint16_t payload_length)
{
  sl_status_t status                             = SL_STATUS_OK;
  sl_si91x_gain_table_info_t *sl_gain_table_info = malloc(sizeof(sl_si91x_gain_table_info_t) + payload_length);
  if (sl_gain_table_info == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }
  sl_gain_table_info->band      = band;
  sl_gain_table_info->bandwidth = bandwidth;
  sl_gain_table_info->size      = payload_length;
  sl_gain_table_info->reserved  = 0;

  memcpy(sl_gain_table_info->gain_table, payload, payload_length);
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_GAIN_TABLE,
                                        SI91X_WLAN_CMD,
                                        sl_gain_table_info,
                                        sizeof(sl_si91x_gain_table_info_t) + (sl_gain_table_info->size),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);
  free(sl_gain_table_info);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_set_11ax_config(uint8_t guard_interval)
{
  sl_status_t status = SL_STATUS_OK;
#if !(SLI_SI91X_CONFIG_WIFI6_PARAMS)
  return SL_STATUS_NOT_SUPPORTED;
#endif
  sl_si91x_11ax_config_params_t config_11ax_params = { 0 };
  config_11ax_params.guard_interval                = guard_interval;
  config_11ax_params.nominal_pe                    = NOMINAL_PE;
  config_11ax_params.dcm_enable                    = DCM_ENABLE;
  config_11ax_params.ldpc_enable                   = LDPC_ENABLE;
  config_11ax_params.ng_cb_enable                  = NG_CB_ENABLE;
  config_11ax_params.ng_cb_values                  = NG_CB_VALUES;
  config_11ax_params.uora_enable                   = UORA_ENABLE;
  config_11ax_params.trigger_rsp_ind               = TRIGGER_RESP_IND;
  config_11ax_params.ipps_valid_value              = IPPS_VALID_VALUE;
  config_11ax_params.tx_only_on_ap_trig            = TX_ONLY_ON_AP_TRIG;
  config_11ax_params.twt_support                   = SLI_SI91X_ENABLE_TWT_FEATURE;
  config_11ax_params.config_er_su                  = CONFIG_ER_SU;
  config_11ax_params.disable_su_beamformee_support = SLI_SI91X_DISABLE_SU_BEAMFORMEE_SUPPORT;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_11AX_PARAMS,
                                        SI91X_WLAN_CMD,
                                        &config_11ax_params,
                                        sizeof(config_11ax_params),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_set_listen_interval(sl_wifi_interface_t interface, sl_wifi_listen_interval_t listen_interval)
{
  UNUSED_PARAMETER(interface);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sl_si91x_set_listen_interval(listen_interval.listen_interval);

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_listen_interval(sl_wifi_interface_t interface, sl_wifi_listen_interval_t *listen_interval)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  SL_WIFI_ARGS_CHECK_NULL_POINTER(listen_interval);

  listen_interval->listen_interval = sl_si91x_get_listen_interval();

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_enable_monitor_mode(sl_wifi_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_disable_monitor_mode(sl_wifi_interface_t interface)
{
  UNUSED_PARAMETER(interface);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_start_p2p_discovery(sl_wifi_interface_t interface,
                                        const sl_wifi_p2p_configuration_t *configuration,
                                        sl_wifi_credential_id_t credential_id)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(configuration);
  UNUSED_PARAMETER(credential_id);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_p2p_connect(sl_wifi_interface_t interface, const sl_wifi_p2p_configuration_t *configuration)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(configuration);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_transceiver_set_channel(sl_wifi_interface_t interface, sl_wifi_transceiver_set_channel_t channel)
{
  sl_status_t status = SL_STATUS_OK;
  sl_si91x_operation_mode_t opermode;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  opermode = get_opermode();
  if (opermode != SL_SI91X_TRANSCEIVER_MODE) {
    SL_DEBUG_LOG("Invalid mode: %d. Command only supported in Wi-Fi transceiver opermode(7)\r\n", opermode);
    return SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE;
  }

  if ((channel.chan_info.channel < 1) || (channel.chan_info.channel > 14)) {
    return SL_STATUS_TRANSCEIVER_INVALID_CHANNEL;
  }

  // User configuration for band and bandwidth are unsupported
  channel.chan_info.band      = SL_WIFI_BAND_2_4GHZ;
  channel.chan_info.bandwidth = SL_WIFI_BANDWIDTH_20MHz;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_TRANSCEIVER_CHANNEL,
                                        SI91X_WLAN_CMD,
                                        &channel,
                                        sizeof(channel),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);

  if (interface == SL_WIFI_TRANSCEIVER_INTERFACE) {
    interface_is_up[SL_WIFI_TRANSCEIVER_INTERFACE_INDEX] = true;
  }

  return status;
}

sl_status_t sl_wifi_set_transceiver_parameters(sl_wifi_interface_t interface, sl_wifi_transceiver_parameters_t *params)
{
  sl_status_t status                 = SL_STATUS_OK;
  sl_si91x_operation_mode_t opermode = 0;
  sl_wifi_buffer_t *buffer           = NULL;
  sl_si91x_packet_t *packet          = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  opermode = get_opermode();
  if (opermode != SL_SI91X_TRANSCEIVER_MODE) {
    SL_DEBUG_LOG("Invalid mode: %d. Command only supported in Wi-Fi transceiver opermode(7)\r\n", opermode);
    return SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE;
  }

  SL_VERIFY_POINTER_OR_RETURN(params, SL_STATUS_NULL_POINTER);

  if (params->set) {
    //! Transceiver configurations shall not be changed dynamically
    if (sl_wifi_is_interface_up(interface)) {
      return SL_STATUS_SI91X_COMMAND_GIVEN_IN_INVALID_STATE;
    }

    if ((!params->retransmit_count) || (params->retransmit_count > MAX_RETRANSMIT_COUNT)) {
      return SL_STATUS_TRANSCEIVER_INVALID_CONFIG;
    }
    for (uint8_t i = 0; i < 4; i++) {
      if ((params->cw_params[i].cwmin > MAX_CW_EXPN_COUNT) || (params->cw_params[i].cwmax > MAX_CW_EXPN_COUNT)
          || (params->cw_params[i].aifsn > MAX_AIFSN)) {
        return SL_STATUS_TRANSCEIVER_INVALID_CONFIG;
      }
    }
  }

  status =
    sl_si91x_driver_send_command(RSI_WLAN_REQ_TRANSCEIVER_CONFIG_PARAMS,
                                 SI91X_WLAN_CMD,
                                 params,
                                 sizeof(sl_wifi_transceiver_parameters_t),
                                 params->set ? SL_SI91X_WAIT_FOR_COMMAND_SUCCESS : SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                 NULL,
                                 &buffer);

  //! Return if API called to set params. Otherwise, if API called to get params, continue further to copy params received from firmware.
  if (params->set) {
    return status;
  }

  if ((status != SL_STATUS_OK) && (buffer != NULL)) {
    sl_si91x_host_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(params, packet->data, sizeof(sl_wifi_transceiver_parameters_t));
  }

  sl_si91x_host_free_buffer(buffer);
  return status;
}

sl_status_t sl_wifi_transceiver_up(sl_wifi_interface_t interface, sl_wifi_transceiver_configuration_t *config)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(config, SL_STATUS_NULL_POINTER);

  status = sl_wifi_set_transceiver_parameters(interface, &config->parameters);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG("\r\nSet transceiver config params failed, error code : 0x%lX\r\n", status);
    return status;
  } else {
    SL_DEBUG_LOG("\r\nSet transceiver config params successful");
  }

  // Update params.set = 0 to fetch transceiver config params
  memset(&config->parameters, 0, sizeof(sl_wifi_transceiver_parameters_t));
  status = sl_wifi_set_transceiver_parameters(interface, &config->parameters);
  if (status != RSI_SUCCESS) {
    SL_DEBUG_LOG("\r\nGet transceiver config params failed, error code : 0x%lX\r\n", status);
  } else {
    SL_DEBUG_LOG("\r\nTransceiver config params:");
    SL_DEBUG_LOG("\r\nRetransmit count: %d", config->parameters.retransmit_count);
    for (uint8_t i = 0; i < 4; i++) {
      SL_DEBUG_LOG("\r\nAC index[%d] - cwmin: %d, cwmax: %d, aifsn: %d",
                   i,
                   config->parameters.cw_params[i].cwmin,
                   config->parameters.cw_params[i].cwmax,
                   config->parameters.cw_params[i].aifsn);
    }
  }

  // Set transceiver mode channel
  status = sl_wifi_transceiver_set_channel(interface, config->channel);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nSet Channel Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    SL_DEBUG_LOG("\r\nSet Channel(%d) Initialization success\r\n", config->channel.chan_info.channel);
  }

  // Get DUT MAC address to use as Addr2/Transmitter Addresss
  status = sl_wifi_get_mac_address(interface, &config->dut_mac);
  if (status == SL_STATUS_OK) {
    SL_DEBUG_LOG("\r\nDevice MAC address: %x:%x:%x:%x:%x:%x\r\n",
                 config->dut_mac.octet[0],
                 config->dut_mac.octet[1],
                 config->dut_mac.octet[2],
                 config->dut_mac.octet[3],
                 config->dut_mac.octet[4],
                 config->dut_mac.octet[5]);
  } else {
    SL_DEBUG_LOG("\r\nFailed to get mac address: 0x%lX\r\n", status);
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

int32_t validate_datarate(sl_wifi_data_rate_t data_rate)
{
  switch (data_rate) {
    case SL_WIFI_DATA_RATE_1:
    case SL_WIFI_DATA_RATE_2:
    case SL_WIFI_DATA_RATE_5_5:
    case SL_WIFI_DATA_RATE_11:
    case SL_WIFI_DATA_RATE_6:
    case SL_WIFI_DATA_RATE_9:
    case SL_WIFI_DATA_RATE_12:
    case SL_WIFI_DATA_RATE_18:
    case SL_WIFI_DATA_RATE_24:
    case SL_WIFI_DATA_RATE_36:
    case SL_WIFI_DATA_RATE_48:
    case SL_WIFI_DATA_RATE_54:
      return SL_STATUS_OK;
    default:
      return SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE;
  }
}

sl_status_t sl_wifi_send_transceiver_data(sl_wifi_interface_t interface,
                                          sl_wifi_transceiver_tx_data_control_t *control,
                                          uint8_t *payload,
                                          uint16_t payload_len)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((!payload_len) || (payload_len > MAX_PAYLOAD_LEN)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SL_VERIFY_POINTER_OR_RETURN(control, SL_STATUS_NULL_POINTER);
  SL_VERIFY_POINTER_OR_RETURN(payload, SL_STATUS_NULL_POINTER);

  if (IS_FIXED_DATA_RATE(control->ctrl_flags)) {
    if (validate_datarate(control->rate)) {
      return SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE;
    }
  }

  status = sl_si91x_driver_send_transceiver_data(control, payload, payload_len, SL_SI91X_WAIT_FOR(1000));

  return status;
}

sl_status_t sl_wifi_update_transceiver_peer_list(sl_wifi_interface_t interface, sl_wifi_transceiver_peer_update_t peer)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((peer.flags & TRANSCEIVER_PEER_ADD_FLAG) && (peer.flags & TRANSCEIVER_PEER_AUTO_RATE_FLAG)
      && (!peer.peer_supported_rate_bitmap)) {
    return SL_STATUS_TRANSCEIVER_INVALID_DATA_RATE;
  }

  if (IS_MAC_ZERO(peer.peer_mac_address)) {
    return SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS;
  }
  if (IS_BCAST_MCAST_MAC(peer.peer_mac_address[0])) {
    return SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TRANSCEIVER_PEER_LIST_UPDATE,
                                        SI91X_WLAN_CMD,
                                        &peer,
                                        sizeof(sl_wifi_transceiver_peer_update_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  return status;
}

sl_status_t sl_wifi_set_transceiver_multicast_filter(sl_wifi_interface_t interface,
                                                     sl_wifi_transceiver_mcast_filter_t mcast)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if (mcast.flags & TRANSCEIVER_MCAST_FILTER_EN) {
    if ((!mcast.num_of_mcast_addr) || mcast.num_of_mcast_addr > TRANSCEIVER_MCAST_FILTER_ADDR_LIMIT) {
      return SL_STATUS_INVALID_PARAMETER;
    }
    for (uint8_t i = 0; i < mcast.num_of_mcast_addr; i++) {
      if (IS_MAC_ZERO(mcast.mac[i])) {
        return SL_STATUS_TRANSCEIVER_INVALID_MAC_ADDRESS;
      }
    }
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_TRANSCEIVER_MCAST_FILTER,
                                        SI91X_WLAN_CMD,
                                        &mcast,
                                        sizeof(sl_wifi_transceiver_mcast_filter_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  return status;
}

sl_status_t sl_wifi_flush_transceiver_data(sl_wifi_interface_t interface)
{
  sl_status_t status = SL_STATUS_OK;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TRANSCEIVER_FLUSH_DATA_Q,
                                        SI91X_WLAN_CMD,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);

  return status;
}

sl_status_t sl_wifi_configure_multicast_filter(sl_wifi_multicast_filter_info_t *multicast_filter_info)
{
  uint16_t multicast_bitmap = 0;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  multicast_bitmap = (uint16_t)multicast_filter_info->command_type;

  if ((multicast_filter_info->command_type == SL_WIFI_MULTICAST_MAC_ADD_BIT)
      || (multicast_filter_info->command_type == SL_WIFI_MULTICAST_MAC_CLEAR_BIT)) {
    multicast_bitmap |= (sli_multicast_mac_hash((uint8_t *)&(multicast_filter_info->mac_address)) << 8);
  }

  sl_status_t status = SL_STATUS_OK;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_MULTICAST_FILTER,
                                        SI91X_WLAN_CMD,
                                        &multicast_bitmap,
                                        sizeof(multicast_bitmap),
                                        SL_SI91X_WAIT_FOR(30100),
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_si91x_update_ap_client_info()
{
  return sl_wifi_get_ap_client_info(SL_WIFI_AP_INTERFACE, &sli_si91x_client_info);
}

sl_ip_address_t *sli_si91x_get_ap_client_ip_address_from_mac_address(const sl_mac_address_t mac_add)
{

  for (uint16_t station_info_index = 0; station_info_index < sli_si91x_client_info.client_count; station_info_index++) {
    sl_wifi_client_info_t *station_info = &sli_si91x_client_info.client_info[station_info_index];
    if (!memcmp((const uint8_t *)&mac_add, (uint8_t *)&station_info->mac_adddress, sizeof(sl_mac_address_t))) {
      return &station_info->ip_address;
    }
  }
  return NULL;
}
