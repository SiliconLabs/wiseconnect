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
#pragma once

#ifndef _SL_RSI_UTILITY_H_
#define _SL_RSI_UTILITY_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "sl_status.h"
#include "sl_constants.h"
#include "sl_wifi_constants.h"
#include "sl_si91x_host_interface.h"
#include "sl_si91x_protocol_types.h"
#include "sl_utility.h"
#include "sl_si91x_driver.h"
#include "sl_wifi_device.h"
#include "sl_si91x_types.h"

#define NCP_HOST_COMMON_RESPONSE_EVENT   (1 << 3) // Indicates RX response received for COMMON command type
#define NCP_HOST_WLAN_RESPONSE_EVENT     (1 << 4) // Indicates synchronous RX response received for WLAN command type
#define NCP_HOST_WLAN_NOTIFICATION_EVENT (1 << 5) // Indicates asynchronous RX response received for WLAN command type
#define NCP_HOST_NETWORK_RESPONSE_EVENT  (1 << 6) // Indicates synchronous RX response received for NETWORK command type
#define NCP_HOST_NETWORK_NOTIFICATION_EVENT \
  (1 << 7)                                      // Indicates asynchronous RX response received for NETWORK command type
#define NCP_HOST_SOCKET_RESPONSE_EVENT (1 << 8) // Indicates RX response received for SOCKET command type
#define NCP_HOST_SOCKET_NOTIFICATION_EVENT \
  (1 << 9)                                   // Indicates asynchronous RX response received for SOCKET command type
#define NCP_HOST_BT_RESPONSE_EVENT (1 << 10) // Indicates RX response received for BLE command type

/// Low Transmit Power Threshold for Wi-Fi.
#define SI91X_LOW_TRANSMIT_POWER_THRESHOLD 6

/// Medium Transmit Power Threshold for Wi-Fi.
#define SI91X_MEDIUM_TRANSMIT_POWER_THRESHOLD 4

typedef struct {
  sl_wifi_performance_profile_t wifi_performance_profile;
  sl_bt_performance_profile_t bt_performance_profile;
  sl_si91x_coex_mode_t coex_mode;
} sli_si91x_performance_profile_t;

/// Efuse data information
typedef union {
  uint8_t mfg_sw_version; ///< Manufacturing PTE software version
  uint16_t pte_crc;       ///< PTE CRC value
} sl_si91x_efuse_data_t;

/// \details Mutex ID identifies the mutex.
typedef void *osMutexId_t;

// Structure to represent a packet queue
typedef struct {
  sl_wifi_buffer_t *head;
  sl_wifi_buffer_t *tail;
  osMutexId_t mutex;
  uint32_t flag;
  uint32_t queued_packet_count;
} si91x_packet_queue_t;

typedef uint32_t sl_si91x_host_timestamp_t;

typedef void (*sl_si91x_host_atomic_action_function_t)(void *user_data);
typedef uint8_t (*sl_si91x_compare_function_t)(sl_wifi_buffer_t *node, void *user_data);
typedef void (*sl_si91x_node_free_function_t)(sl_wifi_buffer_t *node);

extern sl_si91x_performance_profile_t current_performance_profile; /*Indicates the current performance profile*/

sl_status_t convert_si91x_wifi_client_info(
  sl_wifi_client_info_response_t *client_info_response,
  sl_si91x_client_info_response *sl_si91x_client_info_response); /*Function converts TA client info to SDK client info*/
sl_wifi_event_t convert_si91x_event_to_sl_wifi_event(
  rsi_wlan_cmd_response_t command,
  uint16_t frame_status); /*Function converts TA events to SDK events*/

sl_status_t save_sl_wifi_rate(
  sl_wifi_rate_t transfer_rate); /*Function used to update the variable that stores the wifi rate*/
sl_status_t get_saved_sl_wifi_rate(sl_wifi_rate_t *transfer_rate); /*Function used to retrieve the wifi rate*/
void reset_sl_wifi_rate(); /*Function used to set wifi rate to default value of 1 Mbps*/

sl_status_t get_rate_protocol_and_data_rate(
  const uint8_t data_rate,
  sl_wifi_rate_protocol_t *rate_protocol,
  sl_wifi_rate_t *transfer_rate); /*Function used to retrieve protocol and transfer rate*/

sl_status_t save_ap_configuration(
  const sl_wifi_ap_configuration_t *wifi_ap_configuration); /*Function used to update the access point configuration*/
sl_status_t get_saved_ap_configuration(
  sl_wifi_ap_configuration_t *wifi_ap_confuguration); /*Function used to retrieve the access point configuration*/
void reset_ap_configuration(); /*Function used to destroy the current access point configuration*/

void save_tcp_auto_close_choice(
  bool is_tcp_auto_close_enabled); /*Function used to set whether tcp auto close is enabled or disabled*/
bool is_tcp_auto_close_enabled();  /*Function used to check whether tcp auto close is enabled or disabled*/
void set_card_ready_required(bool card_ready_required); /*Function used to set whether card ready is required or not*/
bool get_card_ready_required();                         /*Function used to check whether card ready is required or not*/

void save_max_tx_power(uint8_t max_scan_tx_power,
                       uint8_t max_join_tx_power); /*Function used to set the maximum transmission power*/
sl_wifi_max_tx_power_t get_max_tx_power();         /*Function used to get maximum transmission power*/
void reset_max_tx_power(); /*Function used to set maximum transmission power to default value(31dBm) */

void save_wifi_current_performance_profile(
  const sl_wifi_performance_profile_t *profile); /*Function used to set the current performance profile*/
void get_wifi_current_performance_profile(
  sl_wifi_performance_profile_t *profile); /*Function used to get current wifi performance profile*/

void save_bt_current_performance_profile(
  const sl_bt_performance_profile_t *profile); /*Function used to set the bluetooth performance profile*/
void get_bt_current_performance_profile(
  sl_bt_performance_profile_t *profile); /*Function used to retrieve bluetooth performance profile*/

void get_coex_performance_profile(
  sl_si91x_performance_profile_t *profile);        /*Function used to retrieve the coex performance profile*/
void reset_coex_current_performance_profile(void); /*Function used to zero out the coex performance profile */

void save_boot_configuration(
  const sl_si91x_boot_configuration_t *boot_configuration); /*Function used to update the boot configuration*/
void get_saved_boot_configuration(
  sl_si91x_boot_configuration_t *boot_configuration); /*Function used to retrieve the boot configuration*/

void save_coex_mode(sl_si91x_coex_mode_t coex_mode); /*Function used to update the coex mode*/
sl_si91x_coex_mode_t get_coex_mode(void);            /*Function used to retrieve the coex mode*/

sl_status_t convert_sl_wifi_to_sl_si91x_encryption(
  sl_wifi_encryption_t encryption_mode,
  uint8_t *encryption_request); /*Function converts SDK encryption mode to TA supported mode*/

/*********************************************************************************************
 * @brief
 * 	 An utility function computes coex performance profile internally and converts into sl_si91x_power_save_request_t.
 * @param profile
 *   performance profile which needs to be converted to its equivalent si91x_power_save_request structure.
 * @param power_save_request
 *   si91x specific structure that holds required configuration for the given performance profile.
 *   This is optional parameter.
 * @return coex_profile
 *   Coex power profile
 * @return
 * 	 sl_status_t
 *********************************************************************************************/
sl_status_t sl_si91x_send_power_save_request(sl_si91x_performance_profile_t profile);

/***************************************************************************/ /**
 * @brief
 *   Get the Efuse Data content from flash.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] efuse_data
 *   @ref sl_si91x_efuse_data_t object that contains the Manufacturing software version.
 *   efuse_data_type which holds the type of efuse data to be read.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *   This API is not supported in the current release.
 ******************************************************************************/
sl_status_t sl_si91x_get_flash_efuse_data(sl_si91x_efuse_data_t *efuse_data, uint8_t efuse_data_type);

/***************************************************************************/ /**
 * @brief
 *   Get the Efuse Data content from driver context.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] efuse_data
 *   @ref sl_si91x_efuse_data_t object that contains the Manufacturing software version.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
void sl_si91x_get_efuse_data(sl_si91x_efuse_data_t *efuse_data);

/***************************************************************************/ /**
 * @brief
 *   Set the Efuse Data content in driver context.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_wifi_init should be called before this API.
 * @param[out] efuse_data
 *   @ref sl_si91x_efuse_data_t object that contains the Manufacturing software version.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
void sl_si91x_set_efuse_data(const sl_si91x_efuse_data_t *efuse_data);

/**
 * An utility function to convert dBm value  to si91x specific power value
 * @param wifi_max_tx_power which holds the join power value with dBm as units.
 * @return si91x power level
 */
static inline uint8_t convert_dbm_to_si91x_power_level(sl_wifi_max_tx_power_t wifi_max_tx_power)
{
  uint8_t power_value_in_dBm = wifi_max_tx_power.join_tx_power;
  if (power_value_in_dBm >= SI91X_LOW_TRANSMIT_POWER_THRESHOLD) {
    return SL_SI91X_LOW_POWER_LEVEL;
  } else if (power_value_in_dBm >= SI91X_MEDIUM_TRANSMIT_POWER_THRESHOLD) {
    return SL_SI91X_MEDIUM_POWER_LEVEL;
  } else {
    return SL_SI91X_HIGH_POWER_LEVEL;
  }
}

sl_status_t sl_si91x_platform_init(void);
sl_status_t sl_si91x_platform_deinit(void);

/**
* @addtogroup EXTERNAL_HOST_INTERFACE_FUNCTIONS
* @{ 
*/

/**
 * @brief  This API used to block MCU for specified time.
 * 
 * @param[in] delay_milliseconds 
 *  time delay in milliseconds
 */
void sl_si91x_host_delay_ms(uint32_t delay_milliseconds);

/**
 * @brief  It retrieves a timestamp.
 * 
 * @return sl_si91x_host_timestamp_t 
 */
sl_si91x_host_timestamp_t sl_si91x_host_get_timestamp(void);

/**
 * @brief This API calculates the timestamp difference.
 * 
 * @param[in] starting_timestamp
 *  This parameter is used to calculate the elapsed time.
 *  
 * @return sl_si91x_host_timestamp_t 
 */
sl_si91x_host_timestamp_t sl_si91x_host_elapsed_time(uint32_t starting_timestamp);

/** @} */

// Event API
void sl_si91x_host_set_event(uint32_t event_mask);       /*Function used to set specified flags for event*/
void sl_si91x_host_set_bus_event(uint32_t event_mask);   /*Function used to set specified flags for event*/
void sl_si91x_host_set_async_event(uint32_t event_mask); /*Function used to set specified flags for event*/
uint32_t si91x_host_wait_for_event(uint32_t event_mask,
                                   uint32_t timeout); /*Function used to wait for specific event to occur*/
uint32_t si91x_host_wait_for_bus_event(uint32_t event_mask,
                                       uint32_t timeout); /*Function used to wait for specific event to occur*/
uint32_t si91x_host_wait_for_async_event(uint32_t event_mask,
                                         uint32_t timeout);  /*Function used to wait for specific event to occur*/
uint32_t si91x_host_clear_events(uint32_t event_mask);       /*Function used to clear flags for specific event*/
uint32_t si91x_host_clear_bus_events(uint32_t event_mask);   /*Function used to clear flags for specific event*/
uint32_t si91x_host_clear_async_events(uint32_t event_mask); /*Function used to clear flags for specific event*/

sl_status_t sl_si91x_host_init_buffer_manager(const sl_wifi_buffer_configuration_t *config);
sl_status_t sl_si91x_host_deinit_buffer_manager(void);
sl_status_t sl_si91x_host_allocate_buffer(sl_wifi_buffer_t **buffer,
                                          sl_wifi_buffer_type_t type,
                                          uint32_t buffer_size,
                                          uint32_t wait_duration_ms); /*Function used to allocate memory*/
void *sl_si91x_host_get_buffer_data(
  sl_wifi_buffer_t *buffer,
  uint16_t offset,
  uint16_t *data_length); /*Function used to obtain pointer to a specified location in the buffer*/
void sl_si91x_host_free_buffer(
  sl_wifi_buffer_t *buffer); /*Function used to deallocate the memory associated with buffer*/
// ---------------

sl_status_t sl_si91x_host_add_to_queue(
  sl_si91x_queue_type_t queue,
  sl_wifi_buffer_t *buffer); /*Function enqueues response into corresponding response queue*/
sl_status_t sl_si91x_host_add_to_queue_with_atomic_action(
  sl_si91x_queue_type_t queue,
  sl_wifi_buffer_t *buffer,
  void *user_data,
  sl_si91x_host_atomic_action_function_t handler); /*Function enqueues command into corresponding command queue*/
sl_status_t sl_si91x_host_remove_from_queue(
  sl_si91x_queue_type_t queue,
  sl_wifi_buffer_t **buffer); /*Function dequeues responses from Asynch response queues*/
sl_status_t sl_si91x_host_remove_node_from_queue(
  sl_si91x_queue_type_t queue,
  sl_wifi_buffer_t **buffer,
  void *user_data,
  sl_si91x_compare_function_t compare_function); /*Function dequeues response from Synch response queues*/
/* Function used to flush the pending TX packets from the specified queue */
sl_status_t sl_si91x_host_flush_nodes_from_queue(sl_si91x_queue_type_t queue,
                                                 void *user_data,
                                                 sl_si91x_compare_function_t compare_function,
                                                 sl_si91x_node_free_function_t node_free_function);

/* Function used to flush all the pending TX packets from the specified queue */
sl_status_t sl_si91x_flush_queue_based_on_type(sl_si91x_queue_type_t queue,
                                               sl_si91x_node_free_function_t node_free_function);

uint32_t sl_si91x_host_queue_status(
  sl_si91x_queue_type_t queue); /*Function used to check whether queue is empty or not*/

uint32_t sl_si91x_host_get_queue_packet_count(
  sl_si91x_queue_type_t queue); /*Function used to get the number of packets in the queue*/

// These aren't host APIs. These should go into a wifi bus API header
sl_status_t sl_si91x_bus_read_memory(
  uint32_t addr,
  uint16_t length,
  uint8_t *buffer); /*Function used to set buffer pointer to point to specified memory address*/
sl_status_t sl_si91x_bus_write_memory(
  uint32_t addr,
  uint16_t length,
  const uint8_t *buffer); /*Function used to set specified memory address to point to buffer*/
sl_status_t sl_si91x_bus_read_register(uint8_t address,
                                       uint8_t register_size,
                                       uint16_t *output); /*Function used to read contents of the register*/
sl_status_t sl_si91x_bus_write_register(uint8_t address,
                                        uint8_t register_size,
                                        uint16_t data);         /*Function used to write data into register*/
sl_status_t sl_si91x_bus_read_frame(sl_wifi_buffer_t **buffer); /*Function used to read frame*/
sl_status_t sl_si91x_bus_write_frame(sl_si91x_packet_t *packet,
                                     const uint8_t *payloadparam,
                                     uint16_t size_param); /*Function used to write frames*/

sl_status_t sl_si91x_bus_init();               /*Function used to check the bus availability */
sl_status_t sl_si91x_bus_rx_irq_handler(void); /*Function used to check the bus availability */
void sl_si91x_bus_rx_done_handler(void);       /*Function used to check the bus availability */

/*==============================================*/
/**
 * @brief       Calculate crc for a given byte and accumulate crc.
 * @param[in]   crc8_din   -  crc byte input  
 * @param[in]   crc8_state - accumulated crc  
 * @param[in]   end        - last byte crc  
 * @return      crc value  
 *
 */
uint8_t sli_lmac_crc8_c(uint8_t crc8_din, uint8_t crc8_state, uint8_t end);

/*==============================================*/
/**
 * @brief      Calculate 6-bit hash value for given mac address. 
 * @param[in]  mac - pointer to mac address  
 * @return     6-bit Hash value
 *
 */
uint8_t sli_multicast_mac_hash(uint8_t *mac);

/*==============================================*/
/**
 * @brief       Sends boot instructions to WiFi module
 * @param[in]   uint8 type, type of the insruction to perform
 * @param[in]   uint32 *data, pointer to data which is to be read/write
 * @param[out]  none
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @section description 
 * This API is used to send boot instructions to WiFi module.
 **************************************************/
sl_status_t sl_si91x_boot_instruction(uint8_t type, uint16_t *data);

/***************************************************************************/ /**
 * @brief
 *   The @ref sl_si91x_bus_enable_high_speed() should be called only if the SPI clock frequency is more than 25 MHz.
 * @note
 *   SPI initialization has to be done in low-speed mode only.
 *   After device SPI is configured this API is been used for high-speed mode (>25 MHz).
 *   In addition to this API, the following API sl_si91x_host_enable_high_speed_bus has to be ported by the user to implement the host clock switch.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_bus_enable_high_speed(); /*Function used to start DMA channels for transmission and reception*/

sl_status_t sl_si91x_bus_read_interrupt_status(
  uint16_t *interrupt_status);                              /*Function used to read the interrupt register*/
sl_status_t sl_si91x_bus_set_interrupt_mask(uint32_t mask); /*Function used to block specified interrupts*/

void sl_si91x_ulp_wakeup_init(void); /*Function used to initialize SPI interface on ulp wakeup*/

/**
 * @brief 
 *  Function used to obtain wifi credential type like EsAP,PMK,etc..
 * @param id 
 *  Credential ID as identified by [sl_wifi_credential_id_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-types#sl-wifi-credential-id-t).
 * @param type 
 *  It specifies type of credential.
 * @param cred 
 *  Pointer to store the WiFi credential information of type [sl_wifi_credential_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-credential-t)
 * @return sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details. 
 */
sl_status_t sl_si91x_host_get_credentials(sl_wifi_credential_id_t id, uint8_t type, sl_wifi_credential_t *cred);

bool sli_si91x_get_flash_command_status();

void sli_si91x_update_flash_command_status(bool flag);

bool sli_si91x_is_sdk_ok_to_sleep();

#endif // _SL_RSI_UTILITY_H_
