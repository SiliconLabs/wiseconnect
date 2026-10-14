/***************************************************************************/ /**
 * @file sli_wifi_per_mode.c
 * @brief
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
#include "sl_wifi.h"
#include "sl_status.h"
#include "sli_wifi_per_mode.h"
#include "sli_buffer_manager.h"
#include "sli_wifi_utility.h"
#include <string.h>
#include "sl_wlan_types.h"

extern bool device_initialized;
extern sl_wifi_interface_t default_interface;
#define SLI_WIFI_PER_MODE_MIN(a, b) (((a) < (b)) ? (a) : (b))

sl_status_t sli_wifi_per_reset_statistics(sl_wifi_interface_t interface)
{
  sl_status_t status = SL_STATUS_FAIL;
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_per_reset_t per_reset = { 0 };
  per_reset.frame_body_type.sub_type     = SLI_WIFI_SUBTYPE_PER_RESET;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &per_reset,
                                 sizeof(sli_wifi_request_per_reset_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_configure_chipscope_capture(sl_wifi_interface_t interface,
                                                 void *data_buffer,
                                                 sli_wifi_request_adc_params_t *adc_run_capture)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  if (adc_run_capture == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_adc_run_capture_t buffer_read = { 0 };
  buffer_read.input_source                       = adc_run_capture->source;
  buffer_read.trigger_start_sel                  = adc_run_capture->trigger_select;
  buffer_read.no_of_samples_before_trigger       = adc_run_capture->num_of_samples_bef_trigger;
  buffer_read.no_of_samples                      = adc_run_capture->num_samples;
  buffer_read.capture_mode                       = adc_run_capture->capture_mode;
  buffer_read.trigger_type                       = adc_run_capture->trigger_type;
  buffer_read.stop_mode                          = adc_run_capture->stop_mode;
  buffer_read.flags                              = adc_run_capture->flags;
  buffer_read.frame_body_type.sub_type           = SLI_WIFI_SUBTYPE_RUN_CAPTURE;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &buffer_read,
                                 sizeof(sli_wifi_request_adc_run_capture_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((data_buffer != NULL) && (packet != NULL)) {
    memcpy(data_buffer, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_measure_noise_density(sl_wifi_interface_t interface, void *data_buffer)
{
  sl_status_t status     = SL_STATUS_FAIL;
  sl_wifi_buffer_t *data = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_measnoise_density_t measnoise_density = { 0 };
  measnoise_density.frame_body_type.sub_type             = SLI_WIFI_SUBTYPE_MEAS_NOISE_DENS;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &measnoise_density,
                                 sizeof(sli_wifi_request_measnoise_density_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&data);
  if ((status != SL_STATUS_OK) && (NULL != data)) {
    sli_buffer_manager_free_buffer(data);
  }
  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)data, 0, NULL);

  if ((data_buffer != NULL) && (packet != NULL)) {
    memcpy(data_buffer, packet->data, packet->length);
    sli_buffer_manager_free_buffer(data);
  }

  return status;
}

sl_status_t sli_wifi_set_channel_config(sl_wifi_interface_t interface, sli_wifi_channel_config_t channel)
{
  sl_status_t status = SL_STATUS_FAIL;
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_channel_t set_channel = { 0 };
  memcpy(&set_channel.channel, &channel, sizeof(sli_wifi_channel_config_t));
  set_channel.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_SET_CHANNEL;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &set_channel,
                                 sizeof(sli_wifi_request_channel_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_config_dpd(sl_wifi_interface_t interface, sli_wifi_config_dpd_t config_dpd)
{
  sl_status_t status = SL_STATUS_FAIL;
  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_config_dpd_t config_dpd_request = { 0 };
  // Copy the contents of config_dpd into config_dpd_request
  memcpy(&config_dpd_request.config_dpd, &config_dpd, sizeof(sli_wifi_config_dpd_t));
  config_dpd_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_CONFIG_DPD;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &config_dpd_request,
                                 sizeof(sli_wifi_request_config_dpd_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_readback_dpd_lut_gain(sl_wifi_interface_t interface,
                                           sli_wifi_config_dpd_t config_dpd,
                                           sli_wifi_dpd_data_config_t *readback_dpd_gain)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_dpd_response_t readback_dpd_gain_request = { 0 };
  // Copy the contents of config_dpd into readback_dpd_gain_request
  memcpy(&readback_dpd_gain_request.dpd_config, &config_dpd, sizeof(sli_wifi_config_dpd_t));
  readback_dpd_gain_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_DPD_LUT_GAIN;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &readback_dpd_gain_request,
                                 sizeof(sli_wifi_request_dpd_response_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

    if ((readback_dpd_gain != NULL) && (packet != NULL)) {
      memcpy(readback_dpd_gain, packet->data, packet->length);
    }
    sli_buffer_manager_free_buffer(buffer);
  }
  return status;
}

sl_status_t sli_wifi_populate_dpd_lut(sl_wifi_interface_t interface, sli_wifi_dpd_data_config_t config_dpd)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_dpd_response_t populate_dpd_lut_request = { 0 };

  // Copy the contents of config_dpd into config_dpd_request
  memcpy(&populate_dpd_lut_request.dpd_config, &config_dpd, sizeof(sli_wifi_dpd_data_config_t));
  populate_dpd_lut_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_POPULATE_DPD_LUT;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &populate_dpd_lut_request,
                                 sizeof(sli_wifi_request_dpd_response_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);

  return status;
}

sl_status_t sli_wifi_txir_calib(sl_wifi_interface_t interface,
                                sli_wifi_tx_cal_t tx_cal_conf,
                                sli_wifi_tx_cal_t *tx_cal_data)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_tx_ir_calibration_t tx_ir_calibration_request = { 0 };
  memcpy(&tx_ir_calibration_request.TxIRCal_config, &tx_cal_conf, sizeof(sli_wifi_tx_cal_t));
  tx_ir_calibration_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_TX_IR_CALIBRATION;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &tx_ir_calibration_request,
                                 sizeof(sli_wifi_request_tx_ir_calibration_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((tx_cal_data != NULL) && (packet != NULL)) {
    memcpy(tx_cal_data, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}
sl_status_t sli_wifi_get_txir_dcoc_calibration(sl_wifi_interface_t interface, sli_wifi_tx_cal_t *tx_ir_dcoc_calib)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_tx_ir_calibration_t tx_ir_dcoc_calibration_request = { 0 };
  tx_ir_dcoc_calibration_request.frame_body_type.sub_type             = SLI_WIFI_SUBTYPE_GET_TX_IR_CALIBRATION;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &tx_ir_dcoc_calibration_request,
                                 sizeof(sli_wifi_request_tx_ir_calibration_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((tx_ir_dcoc_calib != NULL) && (packet != NULL)) {
    memcpy(tx_ir_dcoc_calib, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_rxir_calib(sl_wifi_interface_t interface,
                                sli_wifi_rx_cal_t rx_cal_conf,
                                sli_wifi_rx_cal_t *rx_cal_data)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_rx_ir_calibration_t rx_ir_calibration_request = { 0 };
  memcpy(&rx_ir_calibration_request.RxIRCal_config, &rx_cal_conf, sizeof(sli_wifi_rx_cal_t));
  rx_ir_calibration_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_SET_RX_IR_CALIBRATION;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &rx_ir_calibration_request,
                                 sizeof(sli_wifi_request_rx_ir_calibration_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);

  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((rx_cal_data != NULL) && (packet != NULL)) {
    memcpy(rx_cal_data, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_get_rxircal_data(sl_wifi_interface_t interface, sli_wifi_rx_cal_t *rx_cal_data)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_rx_ir_calibration_t rx_ir_calibration_request = { 0 };
  rx_ir_calibration_request.frame_body_type.sub_type             = SLI_WIFI_SUBTYPE_GET_RX_IR_CALIBRATION;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &rx_ir_calibration_request,
                                 sizeof(sli_wifi_request_rx_ir_calibration_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((rx_cal_data != NULL) && (packet != NULL)) {
    memcpy(rx_cal_data, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_set_raw_tx_power(uint32_t tx_power_config)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sli_wifi_request_raw_tx_power_t tx_power_request = { 0 };
  memcpy(&tx_power_request.TxPower_config, &tx_power_config, sizeof(uint32_t));
  tx_power_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_SET_TX_POWER;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &tx_power_request,
                                 sizeof(sli_wifi_request_raw_tx_power_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_get_tx_power(sli_wifi_response_get_tx_power_log_t *get_tx_power)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  sli_wifi_request_get_tx_power_t get_tx_power_request = { 0 };
  get_tx_power_request.frame_body_type.sub_type        = SLI_WIFI_SUBTYPE_GET_TX_POWER;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &get_tx_power_request,
                                 sizeof(sli_wifi_request_get_tx_power_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((get_tx_power != NULL) && (packet != NULL)) {
    memcpy(get_tx_power, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_read_aux_adc(sl_wifi_interface_t interface,
                                  sli_wifi_response_get_aux_adc_data_t *aux_adc,
                                  uint32_t aux_adc_type,
                                  uint32_t flag)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }
  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_aux_adc_t aux_adc_request = { 0 };
  memcpy(&aux_adc_request.AUX_ADC_Type, &aux_adc_type, sizeof(uint32_t));
  memcpy(&aux_adc_request.flags, &flag, sizeof(uint32_t));
  aux_adc_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_READ_AUX_ADC;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &aux_adc_request,
                                 sizeof(sli_wifi_request_aux_adc_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((aux_adc != NULL) && (packet != NULL)) {
    memcpy(aux_adc, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_tx_dac(sl_wifi_interface_t interface, uint32_t enable, uint32_t *tx_dac_data, uint32_t size)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Allocate buffer for tx_dac_request structure
  status = sli_buffer_manager_allocate_buffer(SLI_BUFFER_MANAGER_CE_TX_POOL,
                                              SLI_BUFFER_MANAGER_ALLOCATION_TYPE_DEDICATED,
                                              SLI_WIFI_ALLOCATE_COMMAND_BUFFER_WAIT_TIME,
                                              (sli_buffer_t *)&buffer);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_wifi_request_tx_dac_t *tx_dac_request =
    (sli_wifi_request_tx_dac_t *)sli_wifi_host_get_buffer_data(buffer, 0, NULL);
  if (tx_dac_request == NULL) {
    sli_buffer_manager_free_buffer(buffer);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  memset(tx_dac_request, 0, sizeof(sli_wifi_request_tx_dac_t));

  tx_dac_request->enable = enable;

  // Only copy DAC data if TX DAC is enabled
  if (enable != 0) {
    uint32_t clamped_size = (size > sizeof(tx_dac_request->dac_data)) ? sizeof(tx_dac_request->dac_data) : size;
    tx_dac_request->size  = clamped_size;
    memcpy(tx_dac_request->dac_data, tx_dac_data, clamped_size);
  }

  tx_dac_request->frame_body_type.sub_type = SLI_WIFI_SUBTYPE_TX_DAC_PLAY;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 tx_dac_request,
                                 sizeof(sli_wifi_request_tx_dac_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);

  sli_buffer_manager_free_buffer(buffer);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_config_mlo(sl_wifi_interface_t interface, mlo_config_t mlo_config)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_mlo_configuration_t mlo_config_request = { 0 };
  memcpy(&mlo_config_request.mlo_config, &mlo_config, sizeof(mlo_config_t));
  mlo_config_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_CONFIG_MLO;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &mlo_config_request,
                                 sizeof(sli_wifi_request_mlo_configuration_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_mlo_set_link(sl_wifi_interface_t interface, mlo_set_link_resp_t *mlo_set_link, mlo_set_link_t link)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_mlo_set_link_t mlo_set_link_request = { 0 };
  memcpy(&mlo_set_link_request.link, &link, sizeof(mlo_set_link_t));
  mlo_set_link_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_MLO_SET_LINK;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &mlo_set_link_request,
                                 sizeof(sli_wifi_request_mlo_set_link_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  const sl_wifi_system_packet_t *packet =
    (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

  if ((mlo_set_link != NULL) && (packet != NULL)) {
    memcpy(mlo_set_link, packet->data, packet->length);
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_mlo_add_rem_link(sl_wifi_interface_t interface, mlo_add_link_t mlo_add_rem_link)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_mlo_add_rem_link_t mlo_rem_link_request = { 0 };
  // Copy the contents of mlo_add_rem_link into mlo_rem_link_request
  memcpy(&mlo_rem_link_request.mlo_add_rem_link, &mlo_add_rem_link, sizeof(mlo_add_link_t));
  mlo_rem_link_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_MLO_ADD_REM_LINK;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &mlo_rem_link_request,
                                 sizeof(sli_wifi_request_mlo_add_rem_link_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_loadphy(sl_wifi_interface_t interface)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_loadphy_t loadphy_request = { 0 };
  loadphy_request.frame_body_type.sub_type   = SLI_WIFI_SUBTYPE_LOAD_PHY;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &loadphy_request,
                                 sizeof(sli_wifi_request_loadphy_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_configure_frequency_planning(sl_wifi_interface_t interface,
                                                  sli_wifi_freq_planning_params_t freq_plan_params)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_config_freq_planning_t freq_planning_request = { 0 };
  memcpy(&freq_planning_request.freq_plan_params, &freq_plan_params, sizeof(sli_wifi_freq_planning_params_t));
  freq_planning_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_CONFIG_FREQ_PLANNING;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &freq_planning_request,
                                 sizeof(sli_wifi_request_config_freq_planning_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_get_frequency_planning_params(sl_wifi_interface_t interface,
                                                   sli_wifi_freq_planning_configs_t input_configs,
                                                   sli_wifi_response_get_freq_planning_t *freq_plan_response)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  if (freq_plan_response == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sli_wifi_request_get_freq_planning_t get_freq_planning_request = { 0 };
  memcpy(&get_freq_planning_request.input_configs, &input_configs, sizeof(sli_wifi_freq_planning_configs_t));
  get_freq_planning_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_GET_FREQ_PLANNING_PARAMS;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &get_freq_planning_request,
                                 sizeof(sli_wifi_request_get_freq_planning_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);
  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

    if ((freq_plan_response != NULL) && (packet != NULL)) {
      uint16_t memcpy_length = SLI_WIFI_PER_MODE_MIN(packet->length, sizeof(sli_wifi_response_get_freq_planning_t));
      memcpy(freq_plan_response, packet->data, memcpy_length);
    }
    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_set_get_dig_clk_div(sl_wifi_interface_t interface, sli_wifi_dig_clk_params_t dig_clk_params)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_dig_clk_div_t dig_clk_div_request = { 0 };
  memcpy(&dig_clk_div_request.dig_clk_params, &dig_clk_params, sizeof(sli_wifi_dig_clk_params_t));
  dig_clk_div_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_SET_GET_DIG_CLK_DIV;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &dig_clk_div_request,
                                 sizeof(sli_wifi_request_dig_clk_div_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_enable_disable_dig_freq_plan(sl_wifi_interface_t interface,
                                                  sli_wifi_dig_freq_params_t dig_freq_params)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check if the operation mode is PER mode
  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_request_dig_freq_plan_t dig_freq_plan_request = { 0 };
  memcpy(&dig_freq_plan_request.dig_freq_params, &dig_freq_params, sizeof(sli_wifi_dig_freq_params_t));
  dig_freq_plan_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_ENABLE_DISABLE_DIG_FREQ;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &dig_freq_plan_request,
                                 sizeof(sli_wifi_request_dig_freq_plan_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);
  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_set_syth_config(sl_wifi_interface_t interface, sli_wifi_set_syth_config_t *syth_config)
{
  sl_status_t status = SL_STATUS_FAIL;

  if (syth_config == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_set_syth_config_request_t set_syth_config_request = { 0 };

  memcpy(&set_syth_config_request.syth_config, syth_config, sizeof(sli_wifi_set_syth_config_t));
  set_syth_config_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_SET_SYTH_CONFIG;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &set_syth_config_request,
                                 sizeof(sli_wifi_set_syth_config_request_t),
                                 SLI_WIFI_WAIT_FOR_COMMAND_SUCCESS,
                                 NULL,
                                 NULL);

  VERIFY_STATUS_AND_RETURN(status);
  return status;
}

sl_status_t sli_wifi_read_hmatrix(sl_wifi_interface_t interface,
                                  sli_wifi_read_hmatrix_t *read_hmatrix,
                                  sli_wifi_read_hmatrix_response_t *read_hmatrix_response)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (read_hmatrix == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_read_hmatrix_request_t read_hmatrix_request = { 0 };

  memcpy(&read_hmatrix_request.read_hmatrix, read_hmatrix, sizeof(sli_wifi_read_hmatrix_t));
  read_hmatrix_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_READ_HMATRIX;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &read_hmatrix_request,
                                 sizeof(sli_wifi_read_hmatrix_request_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);

  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);
    if ((read_hmatrix_response != NULL) && (packet != NULL)) {
      uint16_t memcpy_length = SLI_WIFI_PER_MODE_MIN(packet->length, sizeof(sli_wifi_read_hmatrix_response_t));
      memcpy(read_hmatrix_response, packet->data, memcpy_length);
    }

    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_query_command(sl_wifi_interface_t interface,
                                   sli_wifi_query_command_t *query_command,
                                   sli_wifi_query_command_response_t *query_command_response)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (query_command == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_query_command_request_t query_command_request = { 0 };
  memcpy(&query_command_request.query_command, query_command, sizeof(sli_wifi_query_command_t));
  query_command_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_QUERY_COMMAND;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &query_command_request,
                                 sizeof(sli_wifi_query_command_request_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);

  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

    if ((query_command_response != NULL) && (packet != NULL)) {

      uint16_t memcpy_length = SLI_WIFI_PER_MODE_MIN(packet->length, sizeof(sli_wifi_query_command_response_t));
      memcpy(query_command_response, packet->data, memcpy_length);
    }

    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_statics_command(sl_wifi_interface_t interface,
                                     sli_wifi_statics_command_t *statics_command,
                                     sli_wifi_statics_command_response_t *statics_command_response)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (statics_command == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  sli_wifi_statics_command_request_t statics_command_request = { 0 };

  memcpy(&statics_command_request.statics_command, statics_command, sizeof(sli_wifi_statics_command_t));
  statics_command_request.frame_body_type.sub_type = SLI_WIFI_SUBTYPE_STATICS_COMMAND;

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 &statics_command_request,
                                 sizeof(sli_wifi_statics_command_request_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);

  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }
  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

    if ((statics_command_response != NULL) && (packet != NULL)) {
      uint16_t memcpy_length = SLI_WIFI_PER_MODE_MIN(packet->length, sizeof(sli_wifi_statics_command_response_t));
      memcpy(statics_command_response, packet->data, memcpy_length);
    }

    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}

sl_status_t sli_wifi_transmit_loopback(sl_wifi_interface_t interface,
                                       sli_wifi_transmit_loopback_request_t *request,
                                       sli_wifi_transmit_loopback_response_t *response)
{
  sl_status_t status       = SL_STATUS_FAIL;
  sl_wifi_buffer_t *buffer = NULL;

  if (request == NULL || response == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (!device_initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!sl_wifi_is_interface_up(interface)) {
    return SL_STATUS_WIFI_INTERFACE_NOT_UP;
  }

  if ((default_interface & interface) != interface) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (sli_wifi_get_opermode() != SL_WIFI_TRANSMIT_TEST_MODE) {
    return SL_STATUS_INVALID_MODE;
  }

  status = sli_wifi_send_command(SLI_WIFI_REQ_WIFI_RAIL,
                                 SLI_WIFI_WLAN_CMD,
                                 request,
                                 sizeof(sli_wifi_transmit_loopback_request_t),
                                 SLI_WIFI_WAIT_FOR_RESPONSE(1000),
                                 NULL,
                                 (void **)&buffer);

  if ((status != SL_STATUS_OK) && (NULL != buffer)) {
    sli_buffer_manager_free_buffer(buffer);
  }

  VERIFY_STATUS_AND_RETURN(status);

  if (buffer != NULL) {
    const sl_wifi_system_packet_t *packet =
      (sl_wifi_system_packet_t *)sli_wifi_host_get_buffer_data((void *)buffer, 0, NULL);

    if (packet != NULL) {
      uint16_t memcpy_length = SLI_WIFI_PER_MODE_MIN(packet->length, sizeof(sli_wifi_transmit_loopback_response_t));
      memcpy(response->data, packet->data, memcpy_length);
    }

    sli_buffer_manager_free_buffer(buffer);
  }

  return status;
}
