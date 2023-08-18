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

#define VERIFY_STATUS(s)   \
  do {                     \
    if (s != SL_STATUS_OK) \
      return s;            \
  } while (0);

typedef enum { SL_NCP_NORMAL_POWER_MODE, SL_NCP_LOW_POWER_MODE, SL_NCP_ULTRA_LOW_POWER_MODE } sl_si91x_power_mode_t;

typedef struct sl_si91x_power_configuration sl_si91x_power_configuration_t;

extern sl_si91x_performance_profile_t performance_profile;

/** \addtogroup SI91X_DRIVER_FUNCTIONS Core
 * \ingroup SL_SI91X_API
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Initialize the driver.
 * @param[in] config
 *   Pointer to device configuration.
 * @param[in] event_handler
 *   Function pointer to receive asynchronous events.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_init(const sl_wifi_device_configuration_t *config, sl_wifi_event_handler_t event_handler);

/***************************************************************************/ /**
 * @brief
 *   De-initialize the driver.
 * @pre 
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
 *   Queue type to be used to send the command on.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of command packet.
 * @param[in] wait_period
 *   Timeout  for the command response.
 * @param[in] sdk_context
 *   Pointer to the context.
 * @param[in] data_buffer
 *   Pointer to a data buffer pointer for the response data to be returned in.
 * @pre 
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
 * Send commands to the TA; whose response needs to be handled asynchronously.
 * Note: This function doesn't acquire "command_in_flight" boolean
 * @param[in] command
 *   Command type to be sent to TA firmware.
 * @param[in] queue_type
 *   Queue type to be used to send the command on.
 * @param[in] data
 *   Command packet to be sent to the TA firmware.
 * @param[in] data_length
 *   Length of the command packet.
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_asycn_command(uint32_t command,
                                               sl_si91x_queue_type_t queue_type,
                                               void *data,
                                               uint32_t data_length);
/***************************************************************************/ /**
 * @brief
 *   Wait for a command response.
 * @param[in] command
 *   Command type to wait .
 * @param[in] wait_period
 *   Wait time in milliseconds to wait for command response.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_wait_for_response(rsi_wlan_cmd_request_t command, sl_si91x_wait_period_t wait_period);

/***************************************************************************/ /**
 * @brief
 *   Send a socket command.
 * @param[in] request
 *   Pointer to socket command packet.
 * @param[in] data
 *   Pointer to socket data.
 * @param[in] wait_time
 *   Timeout  for the command response.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_socket_data(sl_si91x_socket_send_request_t *request,
                                             const void *data,
                                             uint32_t wait_time);

/***************************************************************************/ /**
 * @brief
 *   Send a Bluetooth command.
 * @param[in] command
 *   Command type to be sent.
 * @param[in] queue_type
 *   Queue type to send command on.
 * @param[in] data
 *   Pointer to Bluetooth data.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_driver_send_bt_command(rsi_wlan_cmd_request_t command,
                                            sl_si91x_queue_type_t queue_type,
                                            sl_wifi_buffer_t *data);

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
 * @pre 
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
 *   Send a raw command frame.
 * @param[in] command
 *   Command type to be sent.
 * @param[in] data
 *   Pointer to the command data to be sent.
 * @param[in] data_length
 *   Length of the data length
 * @param[in] wait_time
 *   Wait time for the command response.
 * @pre 
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
 *   Register a callback function for a specific event.
 * @param[in] event
 *   Event to register a callback for.
 * @param[in] function
 *   Function pointer to callback.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_register_callback(sl_net_event_t event, sl_net_event_handler_t function);

/***************************************************************************/ /**
 * @brief
 *   Default event handler for all events.
 * @param[in] event
 *   Asynchronous event received.
 * @param[in] buffer
 *   Buffer containing data related to asynchronous event.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_default_handler(sl_net_event_t event, sl_wifi_buffer_t *buffer);
/** @} */

/** \addtogroup SI91X_RADIO_FUNCTIONS Radio
 * \ingroup SL_SI91X_API
 * @{ */

/***************************************************************************/ /**
 * @brief
 *   Enable wireless radio.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_enable_radio(void);

/***************************************************************************/ /**
 * @brief
 *   Disable wireless radio.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_disable_radio(void);

/***************************************************************************/ /**
 * @brief
 *   Write calibration data to non-volatile device memory.
 * @param[in] data
 *   Pointer to buffer containing calibration data.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_write_calibration_data(const si91x_calibration_data_t *data);

/***************************************************************************/ /**
 * @brief
 *   Set the device region.
 * @param[in] operation_mode
 *   Operation mode of the device.
 * @param[in] band
 *   Operational band of the device.
 * @param[in] region_code
 *   Region code to be set in the device.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_device_region(sl_si91x_operation_mode_t operation_mode,
                                       sl_si91x_band_mode_t band,
                                       sl_si91x_region_code_t region_code);

/***************************************************************************/ /**
 * @brief
 *   Set device power configuration.
 * @param[in] mode
 *   Power mode to be set to the device.
 * @param[in] config
 *   Pointer to structure containing power configuration.
 * @pre 
 *   @ref sl_si91x_driver_init should be called before this API.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_set_power_mode(sl_si91x_power_mode_t mode, const sl_si91x_power_configuration_t *config);

/** @} */

/** \addtogroup SI91X_FIRMWARE_UPDATE_FUNCTIONS Firmware Update
 * \ingroup SL_SI91X_API
 * @{ */

/***************************************************************************/ /**
 * @brief  
 *   Helper function for actual APIs
 * @param[in] type   
 *   Firmware upgrade chunk type
 * @param[in] content 
 *   Firmware content
 * @param[in] length  
 *   Length of the content
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_fwup(uint16_t type, uint8_t *content, uint16_t length);

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
 *   Create an OTAF client. Initialize the client with a given configuration.
 * @pre
 *   @ref sl_net_up API needs to be called before this API.
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
 * @note        For safe firmware upgrade via TCP server, \n
 *              it will take approx. 65 sec duration for upgrading the firmware of 1.5 MB file.
 ******************************************************************************/
sl_status_t sl_si91x_ota_firmware_upgradation(sl_ip_address_t server_ip,
                                              uint16_t server_port,
                                              uint16_t chunk_number,
                                              uint16_t timeout,
                                              uint16_t tcp_retry_count,
                                              bool asynchronous);

/** @} */

/***************************************************************************/ /**
 * @brief      Write content on TA flash from M4. This is a blocking API.
 * @param[in]  write_address                 - address at which data will be written on TA flash memory from M4 image end address to M4 region end address
 * @param[in]  write_data                    - Input data
 * @param[in]  write_data_length             - total length, should be multiples of 4K in case of sector erase
 * @param[in]  flash_sector_erase_enable     - Enable or Disable sector erase \n
 *                                             1 - Erases multiples of 4 KB of data from M4 Image end address to M4 region end address. \n
 *                                             0 - Disable, allows to write data onto flash \n
 * @return       sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_command_to_write_common_flash(uint32_t write_address,
                                                   uint8_t *write_data,
                                                   uint16_t write_data_length,
                                                   uint8_t flash_sector_erase_enable);

/** \addtogroup SI91X_DRIVER_FUNCTIONS Core API
 * \ingroup SL_SI91X_API
 * @{ */

/*==============================================*/
/**
 * @brief      Secure handshake. This is a blocking API.
 * @param[in]  sub_cmd_type - Sub command.
 * @param[in]  input_data  - Input data.
 * @param[in]  input_len  - Length length.
 * @param[in]  output_len  - Output length.
 * @param[in]  output_data - Output data.
 * @return       sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 *
 */
sl_status_t sl_si91x_m4_ta_secure_handshake(uint8_t sub_cmd_type,
                                            uint8_t input_len,
                                            uint8_t *input_data,
                                            uint8_t output_len,
                                            uint8_t *output_data);

/**
 * @brief      Informs when assertion happens in the firmware.
 * @return     sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
*/
sl_status_t sl_si91x_assert(void);

/** @} */

/***************************************************************************/ /**
 * @brief       
 *   Get ram log on Firmware UART/UART2.
 *  @param[in]   address    
 *    Address in Si91x module.
 *  @param[in]   length       
 *    Chunk length to read from Si91x module.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_get_ram_log(uint32_t address, uint32_t length);

/***************************************************************************/ /**
 * @brief
 *   Start the transmit test. This is a blocking API.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] power
 *   Set TX power in dbm. The valid values are from 2dbm to 18dbm for WiSeConnect module.
 * @param[in] rate
 *   Set transmit data rate.
 * @param[in] length
 *   Configure length of the TX packet. The valid values are in the range of 24 to 1500 bytes in the burst mode and range of 24 to 260 bytes in the continuous mode.
 * @param[in] mode
 *   0- Burst Mode, 1- Continuous Mode, 2- Continuous wave Mode (non modulation) in DC mode, 3- Continuous wave Mode (non modulation) in single tone mode (center frequency -2.5MHz), 4- Continuous wave Mode (non modulation) in single tone mode (center frequency +5MHz).
 * @param[in] channel
 *   Set the channel number in 2.4 GHz / 5GHz.
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_transmit_test_start(uint16_t power,
                                         uint32_t rate,
                                         uint16_t length,
                                         uint16_t mode,
                                         uint16_t channel);
/***************************************************************************/ /**
 * @brief
 *   Stop the transmit test. This is a blocking API.
 * @pre 
 *   @ref sl_wifi_init should be called before this API.
 * @param[in] void
 * @return
 *   sl_status_t. See https://docs.silabs.com/gecko-platform/4.1/common/api/group-status for details.
 ******************************************************************************/
sl_status_t sl_si91x_transmit_test_stop(void);
