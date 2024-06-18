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
#pragma once

#include "sl_status.h"
#include "sl_wifi_device.h"
#include "sl_wifi_host_interface.h"
#include "sl_si91x_host_interface.h"
#include "sl_net_types.h"
#include "sl_rsi_utility.h"
#include "sl_si91x_constants.h"

#define VERIFY_STATUS(s)   \
  do {                     \
    if (s != SL_STATUS_OK) \
      return s;            \
  } while (0)

/**
 * All flags used in bus event mask
 * There are three main groups of flags, each with SL_SI91X_COMMAND_TYPE_COUNT number of unique flags
 *
 */
#define SL_SI91X_TX_PENDING_FLAG(x) (1 << (x))
#define SL_SI91X_RESPONSE_FLAG(x)   (1 << (x))

//! Tx Flags
#define SL_SI91X_COMMON_TX_PENDING_EVENT      SL_SI91X_TX_PENDING_FLAG(SI91X_COMMON_CMD)
#define SL_SI91X_WLAN_TX_PENDING_EVENT        SL_SI91X_TX_PENDING_FLAG(SI91X_WLAN_CMD)
#define SL_SI91X_NETWORK_TX_PENDING_EVENT     SL_SI91X_TX_PENDING_FLAG(SI91X_NETWORK_CMD)
#define SL_SI91X_BT_TX_PENDING_EVENT          SL_SI91X_TX_PENDING_FLAG(SI91X_BT_CMD)
#define SL_SI91X_SOCKET_TX_PENDING_EVENT      SL_SI91X_TX_PENDING_FLAG(SI91X_SOCKET_CMD)
#define SL_SI91X_SOCKET_DATA_TX_PENDING_EVENT SL_SI91X_TX_PENDING_FLAG(SI91X_SOCKET_DATA)

//! Rx Flags
#define SL_SI91X_NCP_HOST_BUS_RX_EVENT \
  SL_SI91X_RESPONSE_FLAG(SI91X_CMD_MAX) //Triggered by IRQ to indicate something to read

#define SL_SI91X_ALL_TX_PENDING_COMMAND_EVENTS                                                           \
  (SL_SI91X_COMMON_TX_PENDING_EVENT | SL_SI91X_WLAN_TX_PENDING_EVENT | SL_SI91X_NETWORK_TX_PENDING_EVENT \
   | SL_SI91X_BT_TX_PENDING_EVENT | SL_SI91X_SOCKET_TX_PENDING_EVENT)

typedef enum { SL_NCP_NORMAL_POWER_MODE, SL_NCP_LOW_POWER_MODE, SL_NCP_ULTRA_LOW_POWER_MODE } sl_si91x_power_mode_t;

typedef struct sl_si91x_power_configuration sl_si91x_power_configuration_t;

/***************************************************************************/ /**
 * @brief
 *   Initialize the driver.
 * @param[in] config
 *   @ref sl_wifi_device_configuration_t Pointer to device configuration.
 * @param[in] event_handler
 *   [sl_wifi_event_handler_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-types#sl-wifi-event-handler-t) Function pointer to receive asynchronous events.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_init(const sl_wifi_device_configuration_t *config, sl_wifi_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief
 *   De-initialize the driver.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_deinit(void);

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for scan results callback.
 * @param[in] command
 *   Command type to be sent to TA firmware.
 * @param[in] queue_type
 *   @ref sl_si91x_queue_type_t Queue type to be used to send the command on.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of command packet.
 * @param[in] wait_period
 *   @ref sl_si91x_wait_period_t Timeout for the command response.
 * @param[in] sdk_context
 *   Pointer to the context.
 * @param[in] data_buffer
 *   [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) Pointer to a data buffer pointer for the response data to be returned in.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_command(uint32_t command,
                                         sl_si91x_queue_type_t queue_type,
                                         const void *data,
                                         uint32_t data_length,
                                         sl_si91x_wait_period_t wait_period,
                                         void *sdk_context,
                                         sl_wifi_buffer_t **data_buffer);

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for scan results callback.
 * @param[in] command
 *   Command type to be sent to TA firmware.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of command packet.
 * @param[in] wait_period
 *   @ref sl_si91x_wait_period_t Timeout for the command response.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_side_band_crypto(uint32_t command,
                                                  const void *data,
                                                  uint32_t data_length,
                                                  sl_si91x_wait_period_t wait_period);

/***************************************************************************/ /**
 * @brief
 * Send commands to the TA; whose response needs to be handled asynchronously.
 * Note: This function doesn't acquire "command_in_flight" boolean
 * @param[in] command
 *   Command type to be sent to TA firmware.
 * @param[in] queue_type
 *   @ref sl_si91x_queue_type_t Queue type to be used to send the command on.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of the command packet.
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_async_command(uint32_t command,
                                               sl_si91x_queue_type_t queue_type,
                                               void *data,
                                               uint32_t data_length);
/***************************************************************************/ /**
 * @brief
 *   Wait for a command response.
 * @param[in] command
 *   @ref rsi_wlan_cmd_request_t Command type to wait .
 * @param[in] wait_period
 *   @ref sl_si91x_wait_period_t Wait time in milliseconds to wait for command response.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_wait_for_response(rsi_wlan_cmd_request_t command, sl_si91x_wait_period_t wait_period);

/***************************************************************************/ /**
 * @brief
 *   Send a socket command.
 * @param[in] request
 *   @ref sl_si91x_socket_send_request_t Pointer to socket command packet.
 * @param[in] data
 *   Pointer to socket data.
 * @param[in] wait_time
 *   Timeout  for the command response.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_socket_data(const sl_si91x_socket_send_request_t *request,
                                             const void *data,
                                             uint32_t wait_time);

/***************************************************************************/ /**
 * @brief
 *   Send a Bluetooth command.
 * @param[in] command
 *   @ref rsi_wlan_cmd_request_t Command type to be sent.
 * @param[in] queue_type
 *   @ref sl_si91x_queue_type_t Queue type to send command on.
 * @param[in] data
 *   [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) Pointer to Bluetooth data.
 * @param[in] sync_command
 *   Sync or Async command.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_bt_command(rsi_wlan_cmd_request_t command,
                                            sl_si91x_queue_type_t queue_type,
                                            sl_wifi_buffer_t *data,
                                            uint8_t sync_command);

/***************************************************************************/ /**
 * @brief
 *   Load a certificate into a specified index.
 * @param[in] certificate_type
 *   Type of certificate being loaded
 * @param[in] certificate_index
 *   Index where the certificate is to be loaded.
 * @param[in] buffer
 *   Pointer to the buffer containing the certificate to be loaded.
 * @param[in] certificate_length
 *   Length of the certificate buffer.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_wifi_set_certificate_index(uint8_t certificate_type,
                                                uint8_t certificate_index,
                                                const uint8_t *buffer,
                                                uint32_t certificate_length);

/***************************************************************************/ /**
 * @brief
 *   Set the host rtc timer. This is a blocking API.
 * @param[in]   timer
 *  @ref sl_si91x_module_rtc_time_t Pointer to fill RTC time.
 *	     second -->  seconds [0-59]
 *	     minute -->  minutes [0-59]
 *	  	 hour   -->  hours since midnight [0-23]
 *	  	 day    -->  day of the month [1-31]
 *	  	 month  -->  months since January [0-11]
 *	  	 year   -->  year since 1990.
 *	 	 Weekday-->  Weekday from Sunday to Saturday [1-7].
 * @note        Hour is 24-hour format only (valid values are 0 to 23).
 *              Valid values for Month are 0 to 11 (January to December).
 * @pre Pre-conditions:
 * -
 *   @ref sl_si91x_driver_init() API needs to be called before this API
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_rtc_timer(sl_si91x_module_rtc_time_t *timer);

/***************************************************************************/ /**
 * @brief
 *   Fetch current time from hardware Real Time Clock. This is a blocking API.
 * @param[out]  response
 *  @ref sl_si91x_module_rtc_time_t Response of the requested command. 
 -
 * @note	Response parameters: 
 -
 * *   Pointer to fill RTC time.
 *	     second -->  seconds [0-59]
 *	     minute -->  minutes [0-59]
 *	  	 hour   -->  hours since midnight [0-23]
 *	  	 day    -->  day of the month [1-31]
 *	  	 month  -->  months since January [0-11]
 *	  	 year   -->  year since 1990.
 *	 	 Weekday-->  Weekday from Sunday to Saturday [1-7].
 * @note        Hour is 24-hour format only (valid values are 0 to 23).
 *              Valid values for Month are 0 to 11 (January to December).
 * @pre Pre-conditions:
 * -
 *   @ref sl_si91x_set_rtc_timer() API needs to be called before this API.
 *   @ref sl_si91x_driver_init() API needs to be called before this API also.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_get_rtc_timer(sl_si91x_module_rtc_time_t *response);

/***************************************************************************/ /**
 * @brief
 *   Writes calibration data to non-volatile device memory.
 * @param[in] data
 *   @ref si91x_calibration_data_t Pointer to buffer containing calibration data.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_write_calibration_data(const si91x_calibration_data_t *data);

/** \addtogroup SI91X_RADIO_FUNCTIONS 
  * \ingroup SL_SI91X_API
  * @{ */

/***************************************************************************/ /**
 * @brief
 *   Start the transmit test. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *  [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] tx_test_info
 *   @ref sl_si91x_request_tx_test_info_t Configurable Tx test mode request structure
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_transmit_test_start(sl_si91x_request_tx_test_info_t *tx_test_info);
/***************************************************************************/ /**
 * @brief
 *   Stop the transmit test. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] void
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_transmit_test_stop(void);

/***************************************************************************/ /**
 * @brief
 *  Used to provide feedback of Frequency error in KHz. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] frequency_calibration
 *   Frequency in KHz of type @ref sl_si91x_freq_offset_t. 
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_frequency_offset(const sl_si91x_freq_offset_t *frequency_calibration);

/***************************************************************************/ /**
 * @brief
 *   Set the device region.
 * @param[in] operation_mode
 *   @ref sl_si91x_operation_mode_t Operation mode of the device.
 * @param[in] band
 *   @ref sl_si91x_band_mode_t Operational band of the device.
 * @param[in] region_code
 *   @ref sl_si91x_region_code_t Region code to be set in the device.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_device_region(sl_si91x_operation_mode_t operation_mode,
                                       sl_si91x_band_mode_t band,
                                       sl_si91x_region_code_t region_code);

/***************************************************************************/ /**
 * @brief
 *   This API will command the firmware to update the existing Flash/EFuse calibration data. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init), sl_si91x_transmit_test_start and sl_si91x_frequency_offset should be called before this API.
 * @param[in] calib_write
 *   Write calibration configuration of type @ref sl_si91x_calibration_write_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_calibration_write(sl_si91x_calibration_write_t calib_write);

/***************************************************************************/ /**
 * @brief
 *   This API reads the calibration data from the Flash/EFuse storage. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] target
 * 			0 - READ_FROM_EFUSE (read calibration data from the EFuse) 
 * 			1 - READ_FROM_FLASH (read calibration data from the Flash) 
 * @param[out] calibration_read
 *   Read the calibration configuration of type @ref sl_si91x_calibration_read_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_calibration_read(sl_si91x_calibration_read_t target,
                                      sl_si91x_calibration_read_t *calibration_read);

/***************************************************************************/ /**
 * @brief
 *   Application that offers feedback on the error caused by the EVM offset. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] evm_offset
 *   evm offset of type @ref sl_si91x_evm_offset_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_evm_offset(const sl_si91x_evm_offset_t *evm_offset);

/***************************************************************************/ /**
 * @brief
 *   This API will command the firmware to update the existing Flash/EFuse calibration data. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init), @ref sl_si91x_evm_offset and @ref sl_si91x_transmit_test_start should be called before this API.
 * @param[in] evm_write
 *   Write the evm calibration configuration  of type @ref sl_si91x_evm_write_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_evm_write(const sl_si91x_evm_write_t *evm_write);

/***************************************************************************/ /**
 * @brief
 *   This API updates Flash/Efuse DPD data. This is a synchronous API. 
 * @pre Pre-conditions:
 * -
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init), @ref sl_si91x_transmit_test_start should be called before this API.
 * @param[in] dpd_calib_data
 *   Write DPD calibration data of type @ref sl_si91x_get_dpd_calib_data_t
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_dpd_calibration(const sl_si91x_get_dpd_calib_data_t *dpd_calib_data);

/***************************************************************************/ /**
 * @brief
 *  This API will command the firmware to get the data from the Efuse memory location. This is a blocking API.
 * @pre Pre-conditions:
 * - 
 *   [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) should be called before this API.
 * @param[in] efuse_read
 *  efuse read structure, which contains efuse read address offset and read data length of type @ref sl_si91x_efuse_read_t
 * @param[out] efuse_read_buf
 *  efuse read buffer
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_efuse_read(sl_si91x_efuse_read_t *efuse_read, uint8_t *efuse_read_buf);

/** @} */

/***************************************************************************/ /**
 * @brief
 *   Send a raw command frame.
 * @param[in] command
 *   Command type to be sent.
 * @param[in] data
 *   Pointer to the command data to be sent.
 * @param[in] data_length
 *   Length of the data length
 * @param[in] wait_time
 *   Wait time for the command response.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_raw_send_command(uint8_t command,
                                             const void *data,
                                             uint32_t data_length,
                                             uint32_t wait_time);

/***************************************************************************/ /**
 * @brief
 *   Register a event handler for network events.
 * @param[in] function
 *   Function pointer to callback.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_register_event_handler(sl_net_event_handler_t function);

/***************************************************************************/ /**
 * @brief
 *   Default event handler for all events.
 * @param[in] event
 *   [sl_net_event_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-event-t) Asynchronous event received.
 * @param[in] buffer
 *   [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) Buffer containing data related to asynchronous event.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_default_handler(sl_net_event_t event, sl_wifi_buffer_t *buffer);

/***************************************************************************/ /**
 * @brief
 *   Enable wireless radio.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_enable_radio(void);

/***************************************************************************/ /**
 * @brief
 *   Disable wireless radio.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_disable_radio(void);

/***************************************************************************/ /**
 * @brief
 *   Set device power configuration.
 * @param[in] mode
 *   @ref sl_si91x_power_mode_t Power mode to be set to the device.
 * @param[in] config
 *   @ref sl_si91x_power_configuration_t Pointer to structure containing power configuration.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_power_mode(sl_si91x_power_mode_t mode, const sl_si91x_power_configuration_t *config);

/** @} */

/** \addtogroup SI91X_FIRMWARE_UPDATE_FROM_HOST_FUNCTIONS Firmware Update From Host
 * \ingroup SI91X_FIRMWARE_UPDATE_FUNCTIONS
 * @{ */

/***************************************************************************/ /**
 * @brief   
 *   Send the RPS header content of firmware file.This is a blocking API.
 * @param[in] rps_header   
 *   Pointer to the RPS header content.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_fwup_start(uint8_t *rps_header);

/***************************************************************************/ /**
 * @brief      
 *   Send the firmware file content.This is a blocking API.
 * @param[in] content       
 *   Pointer to the firmware file content.
 * @param[in] length     
 *   Length of the content
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_fwup_load(uint8_t *content, uint16_t length);

/***************************************************************************/ /**
 * @brief      
 *    This API is used to abort the firmware update process on the SI91x device. This is a blocking API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_fwup_abort();
/** @} */

/** \addtogroup SI91X_DRIVER_FUNCTIONS 
 * \ingroup SL_SI91X_API
 * @{ */

#if defined(SLI_SI91X_MCU_INTERFACE) || defined(DOXYGEN)
/*==============================================*//**
 * @brief      Secure handshake. This is a blocking API.
 *
 * @param[in]  sub_cmd_type Specifies the Sub command type for the secure handshake.
 * @param[in]  input_data  Input data is a pointer that contains the information used during a secure handshake.
 * @param[in]  input_len  Specifies the length of input data.
 * @param[in]  output_len  Specifies the length of output data.
 * @param[in]  output_data Pointer to store the response data after the secure handshake process.
 *
 * @return       sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *
 */
sl_status_t sl_si91x_m4_ta_secure_handshake(uint8_t sub_cmd_type,
                                            uint8_t input_len,
                                            uint8_t *input_data,
                                            uint8_t output_len,
                                            uint8_t *output_data);
#endif

/***************************************************************************/ /**
 * @brief
 *   Si91X specific set join feature bitmap configuration
 * @param[in] interface 
 *   [sl_wifi_interface_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-constants#sl-wifi-interface-t) Selected interface.
 * @param[in] join_feature_bitmap
 *   Join feature bitmap configuration. One of values from @ref SI91X_JOIN_FEATURE_BIT_MAP
 * @return     
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note 
 *     By default SL_SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID bitmap is enabled. User can call this API before calling [sl_wifi_connect](../wiseconnect-api-reference-guide-wi-fi/wifi-client-api#sl-wifi-connect), [sl_wifi_start_ap](../wiseconnect-api-reference-guide-wi-fi/wifi-ap-api#sl-wifi-start-ap), [sl_wifi_start_wps](../wiseconnect-api-reference-guide-wi-fi/wifi-wps-api#sl-wifi-start-wps) to overwrite the join feature bitmap
 *******************************************************************************/

sl_status_t sl_si91x_set_join_configuration(sl_wifi_interface_t interface, uint8_t join_feature_bitmap);

/***************************************************************************/ /**
 * @brief
 *   Si91X specific get join feature bitmap configuration
 * @param[in] interface
 *   [sl_wifi_interface_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-constants#sl-wifi-interface-t) Selected interface.
 * @param[out] join_feature_bitmap
 *   join feature bitmap configuration. One of values from @ref SI91X_JOIN_FEATURE_BIT_MAP
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note
 *     By default SL_SI91X_JOIN_FEAT_LISTEN_INTERVAL_VALID bitmap is enabled.
 *******************************************************************************/
sl_status_t sl_si91x_get_join_configuration(sl_wifi_interface_t interface, uint8_t *join_feature_bitmap);

/***************************************************************************/ /**
 * @brief
 *   This API is used to set different module timeouts.
 * @pre Pre-conditions:
 * -
 *   @ref This API should be called after [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init)
 * @param[in] timeout_type
 *   It is used to identify which timeout type to be set. The possible values can be @ref sl_si91x_timeout_type_t 
 - 
 * @param[in] timeout_value
 *   timeout value to be set. The time resolution depends on timeout_type.
 * @note 
 *   After a successful IP configuration, Gratuitous ARP is used as the periodic WLAN Keep-Alive packet with the configured keep_alive_timeout interval. 
 -
 *   If there is no IP configuration, the NULL Data Packets is used as the WLAN Keep-Alive packet.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *
 */
sl_status_t sl_si91x_configure_timeout(sl_si91x_timeout_type_t timeout_type, uint16_t timeout_value);

/***************************************************************************/ /**
 * @brief
 *   Si91x specific set timeout. This function is used to set active channel scan timeout, authentication association timeout and keep alive timeout of module.
 * @param[in] timeout_config
 *   Timeout configuration of type @ref sl_si91x_timeout_t.
 * @return None
 * @note
 *   This API should ONLY be called before [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init) and repeated call to this API will overwrite timeout values stored in SDK, will be applied on next call to [sl_wifi_init](../wiseconnect-api-reference-guide-wi-fi/wifi-common-api#sl-wifi-init).
 *******************************************************************************/
void sl_si91x_set_timeout(sl_si91x_timeout_t *timeout_config);

/** @} */

/** \addtogroup SI91X_FIRMWARE_UPDATE_FROM_MODULE_FUNCTIONS Firmware Update From Module
 * \ingroup SI91X_FIRMWARE_UPDATE_FUNCTIONS
 * @{ */
/***************************************************************************/ /**
 * @brief
 *   Create an OTAF client. Initialize the client with a given configuration.
 * @pre Pre-conditions:
 * -
 *   [sl_net_up](../wiseconnect-api-reference-guide-nwk-mgmt/net-interface-functions#sl-net-up) API needs to be called before this API.
 * @param[in]   server_ip
 *   OTAF server IP address @ref sl_ip_address_t
 * @param[in]   server_port
 *   OTAF server port number.
 * @param[in]   chunk_number 
 *   Firmware content request chunk number.
 * @param[in]   timeout
 *   TCP receive packet timeout.
 * @param[in]   tcp_retry_count
 *   TCP retransmissions count.
 * @param[in]   asynchronous
 *   OTAF upgrade done aynchronously when this is set, else synchronous upgrade.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 * @note        For safe firmware upgrade via TCP server, 
 -
 *              it will take approx. 65 sec duration for upgrading the firmware of 1.5 MB file.
 * @note
 *   This is an asynchronous API. The response is recieved via [sl_net_event_handler_t](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-types#sl-net-event-handler-t) with [SL_NET_OTA_FW_UPDATE_EVENT](../wiseconnect-api-reference-guide-nwk-mgmt/sl-net-constants#sl-net-event-t) as event
 ******************************************************************************/
sl_status_t sl_si91x_ota_firmware_upgradation(sl_ip_address_t server_ip,
                                              uint16_t server_port,
                                              uint16_t chunk_number,
                                              uint16_t timeout,
                                              uint16_t tcp_retry_count,
                                              bool asynchronous);

/** @} */

/** \addtogroup SI91X_DRIVER_FUNCTIONS 
 * \ingroup SL_SI91X_API
 * @{ */
/***************************************************************************/ /**
 * @brief      This function allows the Network Processor (NWP) to write content to the common flash from M4. 
 *             It's a blocking API.
 * @param[in]  write_address                 
 *             The address in the common flash memory where the write operation should begin.
 *             For the M4 region, the write address should start from 0x8000000. Possible values range from the M4 image end address to the M4 region end address.
 *             For the NWP region, the write address should range from 0 to (20K-1).
 * @param[in]  write_data                    
 *             The data to be written. For sector erase it should be multiples of 4K.
 * @param[in]  write_data_length             
 *              The total length of the data, which should be multiples of 4K for sector erase.
 * @param[in]  flash_sector_erase_enable     
 *             Enable or disable sector erase.
 *             1 - Erases multiples of 4 KB of data.
 *             0 - Disable, allows to write data onto flash.
 * @return     sl_status_t. Refer to https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_command_to_write_common_flash(uint32_t write_address,
                                                   uint8_t *write_data,
                                                   uint16_t write_data_length,
                                                   uint8_t flash_sector_erase_enable);

/***************************************************************************/ /**
 * @brief     Sends a command to read data from the NWP flash memory of the SI91x wireless device. This is a blocking API.
 *            This function is a command to the SI91x wireless device to read data from the NWP flash memory
 *            at the specified address. The read data is stored in the provider's output_data buffer.
 * @param[in]   read_address
 *              The address in the NWP flash memory to read from. The address should range from 0 to (20K-1).
 * @param[in]   length
 *              The number of bytes to read from the NWP flash memory.
 * @param[out]  output_buffer
 *              Pointer to the buffer where the  data will be.
 * @return      sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_command_to_read_common_flash(uint32_t read_address, size_t length, uint8_t *output_buffer);
/** @} */

/***************************************************************************/ /**
 * @brief
 *   Get the current Opermode of the module.
 * @param[in] void
 * @return
 *   sl_si91x_operation_mode_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_si91x_operation_mode_t get_opermode(void);

/***************************************************************************/ /**
 * @brief
 *   Si91X specific set listen interval
 * @param[in] listen_interval
 *   Wi-Fi Listen interval.
 * @return none
 *******************************************************************************/
void sl_si91x_set_listen_interval(uint32_t listen_interval);

/***************************************************************************/ /**
 * @brief
 *   Si91X specific get listen interval
 * @return uint32_t
 *     Wi-Fi Listen interval
 *******************************************************************************/
uint32_t sl_si91x_get_listen_interval(void);

/** \addtogroup SI91X_DRIVER_FUNCTIONS 
 * \ingroup SL_SI91X_API
 * @{ */

/**
 * @brief      Signals the occurrence of an assertion in the firmware.
 * @return     sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
*/
sl_status_t sl_si91x_assert(void);

/***************************************************************************/ /**
 * @brief       
 *   Retrieves TA RAM log/dump via Si91x UART/UART2.
 *  @param[in]   address
 *    Address in Si91x module.
 *  @param[in]   length       
 *    Chunk length to read from Si91x module.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_get_ram_log(uint32_t address, uint32_t length);

/** @} */

/***************************************************************************/ /**
 * @brief     Si91X specific Wi-Fi transceiver mode driver function to send Tx data
 * @param[in] control - Meta data for the payload.
 * @param[in] payload      - Pointer to payload to be sent to LMAC.
 * @param[in] payload_len  - Length of the payload.
 * @param[in] wait_time    - Wait time for the command response.
 * @return    sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *******************************************************************************/
sl_status_t sl_si91x_driver_send_transceiver_data(sl_wifi_transceiver_tx_data_control_t *control,
                                                  uint8_t *payload,
                                                  uint16_t payload_len,
                                                  uint32_t wait_time);

/***************************************************************************/ /**
 * @brief
 *   Register a function and optional argument for scan results callback.
 * @param[in] command
 *   Command type to be sent to TA firmware.
 * @param[in] queue_type
 *   @ref sl_si91x_queue_type_t Queue type to be used to send the command on.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of command packet.
 * @param[in] wait_period
 *   @ref sl_si91x_wait_period_t Timeout for the command response.
 * @param[in] sdk_context
 *   Pointer to the context.
 * @param[in] data_buffer
 *   [sl_wifi_buffer_t](../wiseconnect-api-reference-guide-wi-fi/sl-wifi-buffer-t) Pointer to a data buffer pointer for the response data to be returned in.
 *  @param[in] custom_host_desc
 *   Custom Variable to send additional data to the firmware through the host descriptor.
 * @pre Pre-conditions:
 * - 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_custom_driver_send_command(uint32_t command,
                                                sl_si91x_queue_type_t queue_type,
                                                const void *data,
                                                uint32_t data_length,
                                                sl_si91x_wait_period_t wait_period,
                                                void *sdk_context,
                                                sl_wifi_buffer_t **data_buffer,
                                                uint8_t custom_host_desc);

/** \addtogroup SI91X_FIRMWARE_UPDATE_FROM_HOST_FUNCTIONS 
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Flashes firmware to Wi-Fi module via bootloader.
 * @param[in]   firmware_image
 *   Pointer to firmware.
 * @param[in]   fw_image_size
 *   Size of firmware image..
 * @param[in]   flags 
 *   Flags indicating chuck position in file
 *   0 = Middle of file
 *   1 = Start of file
 *   2 = End of file 
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_bl_upgrade_firmware(uint8_t *firmware_image, uint32_t fw_image_size, uint8_t flags);

/***************************************************************************/ /**
 * @brief      
 *   Set fast firmware upgrade.
 * @param[in] void       
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_fast_fw_up(void);

/** @} */
