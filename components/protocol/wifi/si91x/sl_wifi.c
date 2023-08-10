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
#if defined(SI91X_SOCKET_FEATURE)
#include "sl_si91x_socket_utility.h"
#endif
#include <stdint.h>
#include <string.h>

#ifndef SL_NCP_DEFAULT_COMMAND_WAIT_TIME
#define SL_NCP_DEFAULT_COMMAND_WAIT_TIME 1000
#endif

// Enterprise configuration command parameters
/*=======================================================================*/

// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP

#define SL_EAP_TLS_METHOD  "TLS"
#define SL_EAP_TTLS_METHOD "TTLS"
#define SL_EAP_FAST_METHOD "FAST"
#define SL_EAP_PEAP_METHOD "PEAP"

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

/*========================================================================*/
// 11ax params
/*========================================================================*/
#define NOMINAL_PE         2
#define DCM_ENABLE         0
#define LDPC_ENABLE        0
#define NG_CB_ENABLE       0
#define NG_CB_VALUES       0
#define UORA_ENABLE        0
#define TRIGGER_RESP_IND   0xF
#define IPPS_VALID_VALUE   0
#define TX_ONLY_ON_AP_TRIG 0
#define CONFIG_ER_SU       0 // 0 - NO ER_SU support, 1 - Use ER_SU rates along with Non_ER_SU rates, 2 - Use ER_SU rates only
#define TWT_SUPPORT        1
/*=======================================================================*/
extern bool device_initialized;
extern bool bg_enabled;
extern bool interface_is_up[SL_WIFI_MAX_INTERFACE_INDEX];
extern sl_wifi_interface_t default_interface;
static sl_wifi_advanced_scan_configuration_t advanced_scan_configuration     = { 0 };
static sl_wifi_ap_configuration_t ap_configuration                           = { 0 };
static sl_wifi_advanced_client_configuration_t advanced_client_configuration = { 0 };

static sl_status_t get_configured_join_request(sl_wifi_interface_t module_interface,
                                               const void *configuration,
                                               sl_si91x_join_request_t *join_request)
{
  ARGS_CHECK_NULL_POINTER(configuration);
  ARGS_CHECK_NULL_POINTER(join_request);

  if (module_interface & SL_WIFI_CLIENT_INTERFACE) {
    sl_wifi_client_configuration_t *client_configuration = (sl_wifi_client_configuration_t *)configuration;

    // narrowing conversion from Enum of uint16 to uint8
    get_saved_sl_wifi_rate(&join_request->data_rate);
    memcpy(join_request->ssid, client_configuration->ssid.value, client_configuration->ssid.length);

    join_request->ssid_len      = client_configuration->ssid.length;
    join_request->security_type = client_configuration->security;
    join_request->power_level   = convert_dbm_to_si91x_power_level(get_max_tx_power());
    convert_uint32_to_bytestream(SI91X_LISTEN_INTERVAL, join_request->listen_interval);
    join_request->join_feature_bitmap = SI91X_JOIN_FEAT_BIT_MAP;

  } else if (module_interface & SL_WIFI_AP_INTERFACE) {
    sl_wifi_ap_configuration_t *ap_configuration = (sl_wifi_ap_configuration_t *)configuration;

    get_saved_sl_wifi_rate(&join_request->data_rate);
    memcpy(join_request->ssid, ap_configuration->ssid.value, ap_configuration->ssid.length);

    join_request->ssid_len      = ap_configuration->ssid.length;
    join_request->security_type = ap_configuration->security;
    join_request->vap_id        = AP_MODE;
    join_request->power_level   = convert_dbm_to_si91x_power_level(get_max_tx_power());
  } else {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}
sl_status_t sl_wifi_init(const sl_wifi_device_configuration_t *configuration, sl_wifi_event_handler_t event_handler)
{
  memset(&ap_configuration, 0, sizeof(sl_wifi_ap_configuration_t));
  return sl_si91x_driver_init(configuration, event_handler);
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
                                        SI91X_COMMON_CMD_QUEUE,
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
  status = convert_firmware_status(get_si91x_frame_status(packet));
  VERIFY_STATUS_AND_RETURN(status);

  if (packet->command == RSI_WLAN_RSP_SCAN) {
    *scan_results = (sl_wifi_scan_result_t *)malloc(packet->length);
    memcpy(*scan_results, packet->data, packet->length);
  }
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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

  if (interface & SL_WIFI_AP_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
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
    }

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SCAN,
                                          SI91X_WLAN_CMD_QUEUE,
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
                                          SI91X_WLAN_CMD_QUEUE,
                                          &scan_request,
                                          sizeof(sl_si91x_req_bg_scan_t),
                                          SL_SI91X_RETURN_IMMEDIATELY,
                                          NULL,
                                          NULL);
  }

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_connect(sl_wifi_interface_t interface,
                            const sl_wifi_client_configuration_t *ap,
                            uint32_t timeout_ms)
{
  sl_status_t status;
  sl_si91x_req_psk_t psk_request;
  sl_si91x_req_eap_config_t eap_req = { 0 };
  sl_si91x_join_request_t join_request;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet        = NULL;
  sl_si91x_req_scan_t scan_request = { 0 };
  uint8_t uid_len                  = 0;
  uint8_t psk_len                  = 0;
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

  ARGS_CHECK_NULL_POINTER(ap);
  memset(&scan_request, 0, sizeof(scan_request));
  //! copying SSID & length
  if (ap->ssid.length > 0) {
    memcpy(scan_request.ssid, ap->ssid.value, ap->ssid.length);
  } else {
    return SL_STATUS_INVALID_PARAMETER;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SCAN,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &scan_request,
                                        sizeof(scan_request),
                                        SL_SI91X_WAIT_FOR(6000),
                                        NULL,
                                        NULL);

  VERIFY_STATUS_AND_RETURN(status);

  if ((SL_WIFI_WPA_ENTERPRISE == ap->security) || (SL_WIFI_WPA2_ENTERPRISE == ap->security)) {
    memset(&eap_req, 0, sizeof(eap_req));
    eap_req.user_identity[0] = '"';
    eap_req.password[0]      = '"';

    status = sl_si91x_host_get_credentials(ap->credential_id, SL_WIFI_CRED_EAP, &cred);
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
    } else {
      return SL_STATUS_WIFI_INVALID_ENCRYPTION_METHOD;
    }

    strcpy((char *)eap_req.inner_method, SL_EAP_INNER_METHOD);
    memcpy(eap_req.okc_enable,
           &advanced_client_configuration.eap_flags,
           sizeof(advanced_client_configuration.eap_flags));

    strcpy((char *)eap_req.private_key_password, SL_DEFAULT_PRIVATE_KEY_PASSWORD);

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EAP_CONFIG,
                                          SI91X_WLAN_CMD_QUEUE,
                                          &eap_req,
                                          sizeof(eap_req),
                                          SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                          NULL,
                                          NULL);
    VERIFY_STATUS_AND_RETURN(status);
  } else if ((SL_WIFI_WPA == ap->security) || (SL_WIFI_WPA2 == ap->security) || (SL_WIFI_WPA_WPA2_MIXED == ap->security)
             || (SL_WIFI_WPA3 == ap->security) || (SL_WIFI_WPA3_TRANSITION == ap->security)) {

    memset(&psk_request, 0, sizeof(psk_request));
    status = sl_si91x_host_get_credentials(ap->credential_id, SL_WIFI_CRED_PSK, &cred);
    VERIFY_STATUS_AND_RETURN(status);
    psk_request.type = cred.type == SL_WIFI_CRED_PSK ? 1 : 2;
    memcpy(psk_request.psk_or_pmk, cred.pmk.value, SL_WIFI_MAX_PMK_LENGTH);

    status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HOST_PSK,
                                          SI91X_WLAN_CMD_QUEUE,
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
                                 SI91X_WLAN_CMD_QUEUE,
                                 &join_request,
                                 sizeof(join_request),
                                 timeout_ms ? SL_SI91X_WAIT_FOR_RESPONSE(timeout_ms) : SL_SI91X_RETURN_IMMEDIATELY,
                                 NULL,
                                 &buffer);

  if (timeout_ms != 0 && status != SL_STATUS_OK) {
    sl_status_t temp_status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                                           SI91X_WLAN_CMD_QUEUE,
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
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    return SL_STATUS_NOT_AVAILABLE;
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_si91x_rejoin_params_t rejoin_request = { .max_retry_attempts      = configuration->max_retry_attempts,
                                              .scan_interval           = configuration->scan_interval,
                                              .beacon_missed_count     = configuration->beacon_missed_count,
                                              .first_time_retry_enable = configuration->first_time_retry_enable };

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_REJOIN_PARAMS,
                                                    SI91X_WLAN_CMD_QUEUE,
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
  sl_wifi_buffer_t *buffer;

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
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  *rssi                     = -(packet->data[0]);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_set_mac_address(sl_wifi_interface_t interface, const sl_mac_address_t *mac_address)
{
  UNUSED_PARAMETER(interface);
  sl_status_t status;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  SL_VERIFY_POINTER_OR_RETURN(mac_address, SL_STATUS_NULL_POINTER);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_SET_MAC_ADDRESS,
                                        SI91X_WLAN_CMD_QUEUE,
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
  sl_wifi_buffer_t *buffer;

  ARGS_CHECK_NULL_POINTER(mac);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_MAC_ADDRESS,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(mac->octet, packet->data, sizeof(*mac));
  }
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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
  sl_status_t status = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer;
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
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(SL_SI91X_GET_CHANNEL_TIMEOUT),
                                        NULL,
                                        &buffer);
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

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return status;
}

/*
 * This API doesn't have any affect if it is called after connect/start ap.
 */
sl_status_t sl_wifi_set_max_tx_power(sl_wifi_interface_t interface, uint8_t max_tx_power)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  save_max_tx_power(max_tx_power);
  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_max_tx_power(sl_wifi_interface_t interface, uint8_t *max_tx_power)
{
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  ARGS_CHECK_NULL_POINTER(max_tx_power);
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
  request.security_type   = configuration->security;
  request.encryption_mode = configuration->encryption;

  status = sl_si91x_host_get_credentials(configuration->credential_id, SL_WIFI_CRED_PSK, &cred);
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

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_AP_CONFIGURATION,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &request,
                                        sizeof(request),
                                        SL_SI91X_WAIT_FOR(15000),
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status);

  status = get_configured_join_request(SL_WIFI_AP_INTERFACE, configuration, &join_request);
  VERIFY_STATUS_AND_RETURN(status);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_JOIN,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &join_request,
                                        sizeof(join_request),
                                        SL_SI91X_WAIT_FOR_RESPONSE(15000),
                                        NULL,
                                        &rx_buffer);
  VERIFY_STATUS_AND_RETURN(status);

  join_response = sl_si91x_host_get_buffer_data(rx_buffer, 0, NULL);

  if (join_response->data[0] != 'G') {
    sl_si91x_host_free_buffer(rx_buffer, SL_WIFI_RX_FRAME_BUFFER);
    return SL_STATUS_NOT_AVAILABLE;
  }

  save_ap_configuration(configuration);
  memcpy(&ap_configuration, configuration, sizeof(sl_wifi_ap_configuration_t));
  if (interface == SL_WIFI_AP_2_4GHZ_INTERFACE || interface == SL_WIFI_AP_INTERFACE)
    interface_is_up[SL_WIFI_AP_2_4GHZ_INTERFACE_INDEX] = true;
  else if (interface == SL_WIFI_AP_5GHZ_INTERFACE)
    interface_is_up[SL_WIFI_AP_5GHZ_INTERFACE_INDEX] = true;

  sl_si91x_host_free_buffer(rx_buffer, SL_WIFI_RX_FRAME_BUFFER);
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
  sl_wifi_buffer_t *buffer;
  sl_si91x_req_psk_t pairwise_master_key_request = { 0 };

  if (pairwise_master_key == NULL) {
    return SL_STATUS_WIFI_NULL_PTR_ARG;
  }

  pairwise_master_key_request.type = type;
  memcpy(pairwise_master_key_request.psk_or_pmk, pre_shared_key, strlen(pre_shared_key));
  memcpy(pairwise_master_key_request.ap_ssid, ssid->value, ssid->length);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_HOST_PSK,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &pairwise_master_key_request,
                                        sizeof(sl_si91x_req_psk_t),
                                        SL_SI91X_WAIT_FOR_RESPONSE(35000),
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  memcpy(pairwise_master_key, packet->data, SL_WIFI_MAX_PMK_LENGTH);
  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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
  //                                       SI91X_WLAN_CMD_QUEUE,
  //                                       NULL,
  //                                       0,
  //                                       SL_SI91X_WAIT_FOR_RESPONSE(10000),
  //                                       NULL,
  //                                       &buffer);
  // if (status == SL_STATUS_OK) {
  //   sl_si91x_packet_t *packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  //       //  si91x_go_parameters_t* data = &packet->data;
  //   // Process data here

  //   sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  // }
  // return status;
}

sl_status_t sl_wifi_disconnect_ap_client(sl_wifi_interface_t interface,
                                         const sl_mac_address_t *mac,
                                         sl_wifi_deauth_reason_t reason)
{
  UNUSED_PARAMETER(reason);
  ARGS_CHECK_NULL_POINTER(mac);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  sl_si91x_disassociation_request_t disconnect_request = { 0 };

  disconnect_request.mode_flag = AP_MODE;
  memcpy(&disconnect_request.client_mac_address, mac, sizeof(sl_mac_address_t));

  sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_DISCONNECT,
                                                    SI91X_WLAN_CMD_QUEUE,
                                                    &disconnect_request,
                                                    sizeof(disconnect_request),
                                                    SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                                    NULL,
                                                    NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sl_wifi_get_ap_client_info(sl_wifi_interface_t interface, sl_wifi_client_info_response *client_info)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
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

  ARGS_CHECK_NULL_POINTER(client_info);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_QUERY_GO_PARAMS,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);

  memcpy(&sl_si91x_client_info_response, packet->data, sizeof(sl_si91x_client_info_response));
  convert_si91x_wifi_client_info(client_info, &sl_si91x_client_info_response);

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return status;
}

sl_status_t sl_wifi_get_firmware_version(sl_wifi_version_string_t *version)
{
  sl_status_t status;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  ARGS_CHECK_NULL_POINTER(version);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_FW_VERSION,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(1000),
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(version->version, packet->data, packet->length);
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
  return status;
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
                                                    SI91X_WLAN_CMD_QUEUE,
                                                    &disconnect_request,
                                                    sizeof(disconnect_request),
                                                    SL_NCP_DEFAULT_COMMAND_WAIT_TIME,
                                                    NULL,
                                                    NULL);
  VERIFY_STATUS_AND_RETURN(status);

  reset_coex_current_performance_profile();

#if defined(SI91X_NETWORK_OFFLOAD_ENABLED) && defined(sockets_FEATURE_REQUIRED)
  // Free all allocated sockets
  status = sl_si91x_socket_deinit();
  VERIFY_STATUS(status);
#endif

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_INIT,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_COMMAND_SUCCESS,
                                        NULL,
                                        NULL);
  VERIFY_STATUS_AND_RETURN(status)

  return status;
}

sl_status_t sl_wifi_get_statistics(sl_wifi_interface_t interface, sl_wifi_statistics_t *statistics)
{
  sl_status_t status = SL_STATUS_OK;
  sl_wifi_buffer_t *buffer;
  sl_si91x_packet_t *packet;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  ARGS_CHECK_INVALID_INTERFACE(interface);
  ARGS_CHECK_NULL_POINTER(statistics);

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_EXT_STATS,
                                        SI91X_WLAN_CMD_QUEUE,
                                        NULL,
                                        0,
                                        SL_SI91X_WAIT_FOR_RESPONSE(30500),
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length != sizeof(sl_wifi_statistics_t)) {
    sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
    return SL_STATUS_FAIL;
  }

  if (packet->length > 0) {
    memcpy(statistics, packet->data, packet->length);
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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
                                        SI91X_WLAN_CMD_QUEUE,
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
                                                    SI91X_WLAN_CMD_QUEUE,
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
  sl_performance_profile_t selected_coex_profile_mode     = { 0 };
  sl_performance_profile_t current_coex_profile_mode      = { 0 };
  sl_wifi_performance_profile_t current_wifi_profile_mode = { 0 };

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  ARGS_CHECK_NULL_POINTER(profile);

  if (profile->profile > STANDBY_POWER_SAVE_WITH_RAM_RETENTION) {
    return SL_STATUS_INVALID_MODE;
  }

  // Take backup of current wifi profile
  get_wifi_current_performance_profile(&current_wifi_profile_mode);
  get_coex_performance_profile(&current_coex_profile_mode);

  // Compute selected coex profile
  // Check if current coex profile and selected coex profile are the same
  save_wifi_current_performance_profile(profile);
  get_coex_performance_profile(&selected_coex_profile_mode);
  if (selected_coex_profile_mode == current_coex_profile_mode) {
    return SL_STATUS_OK;
  }

  status = sl_si91x_send_power_save_request(selected_coex_profile_mode);
  if (status != SL_STATUS_OK) {
    save_wifi_current_performance_profile(&current_wifi_profile_mode);
    return status;
  }

  // Set device_initialized as false since RAM of module will be not retained
  // in ULTRA_POWER_SAVE and module needs to be started from init again
  if (selected_coex_profile_mode == STANDBY_POWER_SAVE) {
    device_initialized = false;

#ifdef RSI_M4_INTERFACE
    // In soc mode m4 does not get the card ready for next init after deinit, but if device in STANDBY_POWER_SAVE mode, m4 should wait for card ready for next init
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
  bg_enabled = false;
  reset_coex_current_performance_profile();
  reset_max_tx_power();
  reset_ap_configuration();
  reset_sl_wifi_rate();
  memset(&advanced_scan_configuration, 0, sizeof(sl_wifi_advanced_scan_configuration_t));
  return sl_si91x_driver_deinit();
}

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

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
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

  ARGS_CHECK_NULL_POINTER(rate_protocol);
  ARGS_CHECK_NULL_POINTER(mask);

  sl_wifi_rate_t transfer_rate;
  get_saved_sl_wifi_rate(&transfer_rate);

  return get_rate_protocol_and_data_rate(transfer_rate, rate_protocol, mask);
}

sl_status_t sl_wifi_get_ap_client_count(sl_wifi_interface_t interface, uint32_t *client_list_count)
{
  sl_wifi_client_info_response client_info = { 0 };

  ARGS_CHECK_NULL_POINTER(client_list_count);

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
  sl_wifi_client_info_response client_info = { 0 };

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
  sl_wifi_buffer_t *buffer                         = 0;
  sl_si91x_packet_t *packet                        = NULL;
  sl_si91x_wps_method_request_t wps_method_request = { 0 };

  ARGS_CHECK_NULL_POINTER(wps_pin);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  // Set configuration to generate new WPS pin
  wps_method_request.wps_method   = SI91X_SET_WPS_METHOD_PIN;
  wps_method_request.generate_pin = SI91X_SET_WPS_GENERATE_PIN;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_WPS_METHOD,
                                        SI91X_WLAN_CMD_QUEUE,
                                        &wps_method_request,
                                        sizeof(sl_si91x_wps_method_request_t),
                                        SL_SI91X_WAIT_FOR_COMMAND_RESPONSE,
                                        NULL,
                                        &buffer);
  VERIFY_STATUS_AND_RETURN(status);

  packet = sl_si91x_host_get_buffer_data(buffer, 0, NULL);
  if (packet->length > 0) {
    memcpy(wps_pin->digits, packet->data, sizeof(sl_wifi_wps_pin_t));
  }

  sl_si91x_host_free_buffer(buffer, SL_WIFI_RX_FRAME_BUFFER);
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
                                        SI91X_WLAN_CMD_QUEUE,
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
                                                    SI91X_WLAN_CMD_QUEUE,
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
  ARGS_CHECK_NULL_POINTER(configuration);

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  memcpy(&advanced_scan_configuration, configuration, sizeof(sl_wifi_advanced_scan_configuration_t));
  advanced_scan_configuration.enable_instant_scan = SI91X_INSTANT_SCAN_ENABLE;

  return SL_STATUS_OK;
}

sl_status_t sl_wifi_get_advanced_scan_configuration(sl_wifi_advanced_scan_configuration_t *configuration)
{
  ARGS_CHECK_NULL_POINTER(configuration);

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
                                          SI91X_WLAN_CMD_QUEUE,
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

  if (interface & SL_WIFI_CLIENT_INTERFACE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  memcpy(configuration, &ap_configuration, sizeof(sl_wifi_ap_configuration_t));

  return SL_STATUS_OK;
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
                                                    SI91X_WLAN_CMD_QUEUE,
                                                    twt_req,
                                                    sizeof(sl_wifi_twt_request_t),
                                                    SL_SI91X_RETURN_IMMEDIATELY,
                                                    NULL,
                                                    NULL);
  return status;
}

sl_status_t sl_wifi_disable_target_wake_time(sl_wifi_twt_request_t *twt_req)
{
  if ((!twt_req->twt_enable) && ((twt_req->twt_flow_id == 0xFF) || (twt_req->twt_flow_id <= 7))) {
    sl_status_t status = sl_si91x_driver_send_command(RSI_WLAN_REQ_TWT_PARAMS,
                                                      SI91X_WLAN_CMD_QUEUE,
                                                      twt_req,
                                                      sizeof(sl_wifi_twt_request_t),
                                                      SL_SI91X_RETURN_IMMEDIATELY,
                                                      NULL,
                                                      NULL);
    return status;
  }

  return SL_STATUS_INVALID_PARAMETER;
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
                                        SI91X_WLAN_CMD_QUEUE,
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

  sl_gain_table_info->band      = band;
  sl_gain_table_info->bandwidth = bandwidth;
  sl_gain_table_info->size      = payload_length;
  sl_gain_table_info->reserved  = 0;

  memcpy(sl_gain_table_info->gain_table, payload, payload_length);
  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_GAIN_TABLE,
                                        SI91X_WLAN_CMD_QUEUE,
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
#if !(HIGH_EFFICIENCY_PARAMS_SUPPORT)
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
  config_11ax_params.twt_support                   = TWT_SUPPORT;
  config_11ax_params.config_er_su                  = CONFIG_ER_SU;

  status = sl_si91x_driver_send_command(RSI_WLAN_REQ_11AX_PARAMS,
                                        SI91X_WLAN_CMD_QUEUE,
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
  UNUSED_PARAMETER(listen_interval);
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_wifi_get_listen_interval(sl_wifi_interface_t interface, sl_wifi_listen_interval_t *listen_interval)
{
  UNUSED_PARAMETER(interface);
  UNUSED_PARAMETER(listen_interval);
  return SL_STATUS_NOT_SUPPORTED;
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
