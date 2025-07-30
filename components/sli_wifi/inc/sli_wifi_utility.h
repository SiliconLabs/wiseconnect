/***************************************************************************/ /**
 * @file    sli_wifi_utility.h
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
#ifndef SLI_WIFI_UTILITY_H
#define SLI_WIFI_UTILITY_H
#include "sl_wifi_types.h"
#include "sli_wifi_types.h"
#include "sl_status.h"
#include "sl_wifi_host_interface.h"
#ifndef __ZEPHYR__
#include "sli_cmsis_os2_ext_task_register.h"
/// External variable representing the index of the thread local array at which the firmware status will be stored.
extern sli_task_register_id_t sli_fw_status_storage_index;
#endif
typedef bool (*sli_wifi_buffer_comparator)(const sl_wifi_buffer_t *buffer, const void *userdata);

/**
 * @brief
 *   Allocate a buffer to send a command
 * @param[out] host_buffer
 *   Destination buffer object
 * @param[out] buffer
 *   Start of the internal buffer data
 * @param[in] requested_buffer_size
 *   Requested buffer size
 * @param[in] wait_duration_ms
 *   Duration to wait for buffer to become available
 * @return sl_status_t Returns the status of the operation. A value of 0 (SL_STATUS_OK) indicates success.
 *                     Other values indicate failure. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 */
sl_status_t sli_wifi_allocate_command_buffer(sl_wifi_buffer_t **host_buffer,
                                             void **buffer,
                                             uint32_t requested_buffer_size,
                                             uint32_t wait_duration_ms);
/**
 * @brief Atomically append a given buffer to the end of a buffer queue.
 * 
 * This function appends a buffer to the end of a specified buffer queue in an atomic operation,
 * ensuring thread safety during the append operation.
 *
 * @param[in] queue Pointer to the destination buffer queue where the buffer will be appended.
 * @param[in] buffer Pointer to the buffer that is to be appended to the queue.
 */
void sli_wifi_append_to_buffer_queue(sli_wifi_buffer_queue_t *queue, sl_wifi_buffer_t *buffer);

// Event API
/* Function used to set specified flags for event */
void sli_wifi_set_event(uint32_t event_mask);

/* Function used to remove the buffer from the specified queue by using comparator */
sl_status_t sli_wifi_remove_buffer_from_queue_by_comparator(sli_wifi_buffer_queue_t *queue,
                                                            const void *user_data,
                                                            sli_wifi_buffer_comparator comparator,
                                                            sl_wifi_buffer_t **buffer);

/***************************************************************************/ /**
 * @brief
 *   Retrieve data from a buffer with a specified offset.
 *
 * @details
 *   This function is designed to retrieve data from a buffer at a specified offset.
 *
 * @param[in]  buffer
 *   A pointer to an [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) structure from which data is to be retrieved.
 * 
 * @param[in]  offset
 *   Offset from the start of the buffer where data retrieval begins.
 * 
 * @param[out] data_length
 *   Pointer to a variable where the remaining data length from the offset will be stored.
 *
 * @return
 *   Pointer to the data at the specified offset within the buffer.
 *
 ******************************************************************************/
void *sli_wifi_host_get_buffer_data(sl_wifi_buffer_t *buffer, uint16_t offset, uint16_t *data_length);

#ifndef __ZEPHYR__
/******************************************************************************
 * @brief
 * 	A utility function that store the firmware status code in thread specific storage.
 * @param[in] converted_firmware_status
 *	Firmware status code that needs to be saved.
 *****************************************************************************/
static inline void sli_wifi_save_firmware_status(sl_status_t converted_firmware_status)
{
  sli_osTaskRegisterSetValue(NULL, sli_fw_status_storage_index, converted_firmware_status);
}
#endif

/******************************************************************************
 * @brief
 *   A utility function that converts frame status sent by firmware to sl_status_t and stores in thread local storage of caller thread.
 * @param[in] si91x_firmware_status
 *   si91x_firmware_status that needs to be converted to sl_status_t.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details.
 *****************************************************************************/
static inline sl_status_t sli_wifi_convert_and_save_firmware_status(uint16_t wifi_firmware_status)
{
  sl_status_t converted_firmware_status = (wifi_firmware_status == SL_STATUS_OK) ? SL_STATUS_OK
                                                                                 : (wifi_firmware_status | BIT(16));
#ifndef __ZEPHYR__
  sli_wifi_save_firmware_status(converted_firmware_status);
#endif
  return converted_firmware_status;
}

/* Function used to get maximum transmission power */
sl_wifi_max_tx_power_t sli_get_max_tx_power();

/* Function used to retrieve the wifi rate */
sl_status_t sli_wifi_get_saved_rate(sl_wifi_rate_t *transfer_rate);

/**
 * @brief 
 *  Function used to obtain wifi credential type like EsAP,PMK,etc..
 * @param id 
 *  Credential ID as identified by [sl_wifi_credential_id_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-types#sl-wifi-credential-id-t).
 * @param type 
 *  It specifies type of credential.
 * @param cred 
 *  Pointer to store the wifi credential information of type [sl_wifi_credential_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-credential-t)
 * @return sl_status_t. See https://docs.silabs.com/gecko-platform/latest/platform-common/status for details. 
 */
sl_status_t sli_wifi_host_get_credentials(sl_wifi_credential_id_t id, uint8_t type, sl_wifi_credential_t *cred);

/* Function used to set the maximum transmission power */
void sli_save_max_tx_power(uint8_t max_scan_tx_power, uint8_t max_join_tx_power);

/* Function converts SDK encryption mode to NWP supported mode */
sl_status_t sli_wifi_get_nwp_encryption(sl_wifi_encryption_t encryption_mode, uint8_t *encryption_request);

/* Function used to update the access point configuration */
sl_status_t sli_wifi_save_ap_configuration(const sl_wifi_ap_configuration_t *wifi_ap_configuration);

/* Function used to destroy the current access point configuration */
void sli_wifi_reset_ap_configuration();

/* Function to send the requested Wi-Fi and BT/BLE performance profile to firmware */
sl_status_t sli_wifi_send_power_save_request(const sl_wifi_performance_profile_v2_t *wifi_profile,
                                             const sl_bt_performance_profile_t *bt_profile);
void sli_wifi_save_wifi_current_performance_profile(const sl_wifi_performance_profile_v2_t *profile);
void sli_wifi_flush_scan_results_database(void);
sl_status_t sli_wifi_get_stored_scan_results(
  sl_wifi_interface_t interface,
  sl_wifi_extended_scan_result_parameters_t *extended_scan_parameters); //Done
/* Function used to retrieve the access point configuration */
sl_status_t sli_wifi_get_saved_ap_configuration(sl_wifi_ap_configuration_t *wifi_ap_confuguration);
/* Function used to retrieve protocol and transfer rate */
sl_status_t sli_wifi_get_rate_protocol_and_data_rate(const uint8_t data_rate,
                                                     sl_wifi_rate_protocol_t *rate_protocol,
                                                     sl_wifi_rate_t *transfer_rate);
/* Function used to zero out the coex performance profile */
void sli_wifi_reset_coex_current_performance_profile(void);
/* Function used to set maximum transmission power to default value(31 dBm) */
void sli_wifi_reset_max_tx_power();
/* Function used to set wifi rate to default value of 1 Mbps */
void sli_wifi_reset_sl_wifi_rate();
/* Function used to set whether card ready is required or not */
void sli_wifi_set_card_ready_required(bool card_ready_required);
/* Function used to retrieve the coex performance profile */
void sli_get_coex_performance_profile(sl_wifi_system_performance_profile_t *profile);
/* Function used to get current wifi performance profile */
void sli_wifi_get_current_performance_profile(sl_wifi_performance_profile_v2_t *profile);
void sli_convert_performance_profile_to_power_save_command(sl_wifi_system_performance_profile_t profile,
                                                           sli_si91x_power_save_request_t *power_save_request);
void sli_wifi_save_current_performance_profile(const sl_wifi_performance_profile_v2_t *profile);
/* Function used to check whether card ready is required or not */
bool sli_wifi_get_card_ready_required();
#endif