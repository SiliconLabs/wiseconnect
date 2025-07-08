/******************************************************************************
* @file sl_si91x_ssi.h
* @brief SSI Peripheral API implementation
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

#ifndef SL_SI91X_SSI_H
#define SL_SI91X_SSI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "SPI.h"

/***************************************************************************/
/**
 * @addtogroup SSI Synchronous Serial Interface
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 * 
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Data Types

/***************************************************************************/
/**
 * @typedef sl_ssi_power_state_t
 * @brief Typedef for ARM power state structure.
 */
typedef ARM_POWER_STATE sl_ssi_power_state_t; ///< ARM power state structure.

/***************************************************************************/
/**
 * @typedef sl_ssi_status_t
 * @brief Typedef for ARM SPI status.
 */
typedef ARM_SPI_STATUS sl_ssi_status_t; ///< ARM SPI status.

/***************************************************************************/
/**
 * @typedef sl_ssi_driver_t
 * @brief Typedef for SSI driver structure.
 */
typedef ARM_DRIVER_SPI sl_ssi_driver_t; ///< SSI driver structure.

/***************************************************************************/
/**
 * @typedef sl_ssi_handle_t
 * @brief Typedef for SSI handle to be passed into APIs.
 */
typedef const void *sl_ssi_handle_t; ///< SSI handle to be passed into APIs.

/***************************************************************************/
/**
 * @typedef sl_ssi_signal_event_t
 * @brief Typedef for callback function for SSI.
 */
typedef ARM_SPI_SignalEvent_t sl_ssi_signal_event_t; ///< Callback typedef for SSI.

/***************************************************************************/
/**
 * @brief Structure to hold the version numbers of the peripheral API.
 */
typedef struct {
  uint8_t release; ///< Release version number.
  uint8_t major;   ///< Major version number.
  uint8_t minor;   ///< Minor version number.
} sl_ssi_version_t;

/***************************************************************************/
/**
 * @brief Enumeration for different SSI callback events.
 */
typedef enum {
  SSI_EVENT_TRANSFER_COMPLETE = ARM_SPI_EVENT_TRANSFER_COMPLETE, ///< Transfer complete event.
  SSI_EVENT_DATA_LOST         = ARM_SPI_EVENT_DATA_LOST,         ///< Data lost event.
  SSI_EVENT_MODE_FAULT        = ARM_SPI_EVENT_MODE_FAULT,        ///< Mode fault event.
} ssi_event_typedef_t;

/***************************************************************************/
/**
 * @brief Enumeration for different SSI peripheral clock modes.
 */
typedef enum {
  SL_SSI_PERIPHERAL_CPOL0_CPHA0 = ARM_SPI_CPOL0_CPHA0, ///< Mode 0 (CPOL = 0, CPHA = 0).
  SL_SSI_PERIPHERAL_CPOL0_CPHA1 = ARM_SPI_CPOL0_CPHA1, ///< Mode 1 (CPOL = 0, CPHA = 1).
  SL_SSI_PERIPHERAL_CPOL1_CPHA0 = ARM_SPI_CPOL1_CPHA0, ///< Mode 2 (CPOL = 1, CPHA = 0).
  SL_SSI_PERIPHERAL_CPOL1_CPHA1 = ARM_SPI_CPOL1_CPHA1, ///< Mode 3 (CPOL = 1, CPHA = 1).
  SL_SSI_PERIPHERAL_TI_SSI      = ARM_SPI_TI_SSI,      ///< TI mode.
  SL_SSI_PERIPHERAL_MICROWIRE   = ARM_SPI_MICROWIRE,   ///< Microwire/Half-duplex mode.
  SL_SSI_PERIPHERAL_MODE_LAST                          ///< Last member of enum for validation.
} ssi_peripheral_clock_mode_t;

/***************************************************************************/
/**
 * @brief Enumeration for different SSI peripheral device modes.
 */
typedef enum {
  SL_SSI_MASTER_ACTIVE    = ARM_SPI_MODE_MASTER,        ///< For the purpose of migration
  SL_SSI_PRIMARY_ACTIVE   = SL_SSI_MASTER_ACTIVE,       ///< Primary mode is active.
  SL_SSI_SLAVE_ACTIVE     = ARM_SPI_MODE_SLAVE,         ///< For the purpose of migration
  SL_SSI_SECONDARY_ACTIVE = SL_SSI_SLAVE_ACTIVE,        ///< Secondary mode is active.
  SL_SSI_ULP_MASTER_ACTIVE,                             ///< For the purpose of migration
  SL_SSI_ULP_PRIMARY_ACTIVE = SL_SSI_ULP_MASTER_ACTIVE, ///< ULP primary mode is active.
  SL_SSI_INSTANCE_LAST,                                 ///< Last member of enum for validation.
} sl_ssi_instance_t;

/***************************************************************************/
/**
 * @brief Structure to hold the parameters for the configuration of the SSI peripheral.
 */
typedef struct {
  uint8_t bit_width;             ///< Bit width, 4 - 16 bits.
  uint32_t device_mode;          ///< Mode such as Master or Slave mode.
  uint32_t clock_mode;           ///< Clock mode such as CPOL0 CPHA1.
  uint32_t baud_rate;            ///< Baud rate for SSI.
  uint32_t receive_sample_delay; ///< Delay for the receive input signal.
} sl_ssi_control_config_t;

/***************************************************************************/
/**
 * @brief Structure to hold the clock configuration parameters.
 */
typedef struct {
  uint16_t division_factor;            ///< Clock division factor.
  uint16_t intf_pll_500_control_value; ///< Interface PLL control value.
  uint32_t intf_pll_clock;             ///< Interface PLL clock frequency.
  uint32_t intf_pll_reference_clock;   ///< Interface PLL reference clock frequency.
  uint32_t soc_pll_clock;              ///< SoC PLL clock frequency.
  uint32_t soc_pll_reference_clock;    ///< SoC PLL reference clock frequency.
  uint8_t soc_pll_mm_count_value;      ///< SoC PLL count value.
} sl_ssi_clock_config_t;

/***************************************************************************/
/**
 * @brief Enumeration for SSI secondary numbers.
 */
typedef enum {
  SSI_SLAVE_0,                ///< Secondary No. 1.
  SSI_SLAVE_1,                ///< Secondary No. 2.
  SSI_SLAVE_2,                ///< Secondary No. 3.
  SSI_SLAVE_3,                ///< Secondary No. 4.
  SSI_SLAVE_NUMBER_LAST_ENUM, ///< Last member of enum for validation.
} sl_ssi_slave_number_t;

/***************************************************************************/
/**
 * @brief This API is no longer supported due to the restriction on peripheral drivers to configuring clocks.
 * @brief To configure the SSI clock.
 * 
 * @details Configures the PLL clock and SOC clock with the values set by the user in the clock configuration structure.
 * 
 * @param[in] clock_config Pointer to the clock configuration structure \ref sl_ssi_clock_config_t.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_configure_clock(sl_ssi_clock_config_t *clock_config);

/***************************************************************************/
/**
 * @brief To initialize the SSI.
 * 
 * @details This API initializes the SSI. If DMA is enabled, it also initializes the DMA module.
 *          Pass the address of the pointer for storing the SSI Primary/Secondary/ULP Primary
 *          handle, which can be used in the future for other function calls.
 *
 * @param[in] instance The instance of the SSI (Primary/Secondary/ULP Primary) (\ref sl_ssi_instance_t).
 *
 * @param[in] ssi_handle Double pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 *
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_BUSY               - Driver is busy.
 *         - SL_STATUS_INVALID_PARAMETER  - Parameters are invalid.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_init(sl_ssi_instance_t instance, sl_ssi_handle_t *ssi_handle);

/***************************************************************************/
/**
 * @brief To uninitialize the SSI.
 * 
 * @details This API will uninitialize the SSI. If DMA is enabled, it also uninitializes the DMA module.
 *
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 *
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_NULL_POINTER       - The parameter is a null pointer.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 *
 * @note
 * When the SSI module is used in combination with other peripherals, see the notes below while deinitializing in the application:
 * 1. Whenever sl_si91x_ssi_deinit() is called, it will disable the clock for the peripheral. To power off the peripheral, we have to power down the
 *    power domain (PERI_EFUSE) which contains the following peripherals:
 *    USART, UART, I2C, SSI Master, SSI Slave, Generic-SPI Master, I2S Master, I2S Slave, Micro-DMA Controller, Config Timer,
 *    Random-Number Generator, CRC Accelerator, SIO, QEI, MCPWM, and EFUSE.
 *    Use the following API to power down the particular power domain if other peripherals are not being used:
 *    sl_si91x_peri_efuse_power_down(power_down);
 *
 * 2. A few peripherals (ULP Peripherals, UULP Peripherals, GPDMA, and SDIO-SPI) have separate domains; those can be powered down independently. For additional details, see the Power architecture section in the Hardware Reference Manual.
 *    Here, ULP_UART has a separate power domain ULPSS_PWRGATE_ULP_UART, which can be powered down independently. See the rsi_power_save.h file for all power gate definitions.
 ******************************************************************************/
sl_status_t sl_si91x_ssi_deinit(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To control and configure the SSI.
 * 
 * @details This API will control and configure the SSI. As part of that, the following parameters are configured:
 *          - Mode (Primary/Secondary/ULP Primary)
 *          - Bit width (4 bits to 16 bits)
 *          - Clock mode/Frame format (mode0 to mode3, TI, and Microwire)
 *          - Bitrate (10 Mbps to 40 Mbps)
 *          - Rx sample delay (0 to 63)
 * 
 * @param[in] ssi_handle Pointer to the SSI instance handle ( \ref sl_ssi_handle_t).
 * @param[in] control_configuration Pointer to the control config structure (\ref sl_ssi_control_config_t).
 * @param[in] slave_number In the current implementation, the variable slave_number is unused (\ref sl_ssi_slave_number_t).
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_FAIL               - Function failed.
 *         - SL_STATUS_BUSY               - Driver is busy.
 *         - SL_STATUS_NOT_SUPPORTED      - Parameter is not supported.
 *         - SL_STATUS_INVALID_PARAMETER  - Parameters are invalid.
 *         - SL_STATUS_NULL_POINTER       - The parameter is a null pointer.
 *         - SL_STATUS_INVALID_MODE       - Slave select mode is invalid.
 *         - SL_STATUS_INVALID_TYPE       - SPI frame format is not valid.
 *         - SL_STATUS_INVALID_RANGE      - Data bits (frame length) are not in range.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_set_configuration(sl_ssi_handle_t ssi_handle,
                                           sl_ssi_control_config_t *control_configuration,
                                           sl_ssi_slave_number_t slave_number);

/***************************************************************************/
/**
 * @brief To receive data from the secondary device.
 * 
 * @details This API will receive data from the secondary device. If DMA is enabled, it configures the DMA channel and required parameters.
 *          When the received data is equal to the data_length passed in this function, a callback event is generated which can be registered using \ref sl_si91x_ssi_register_event_callback.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init 
 *      - \ref sl_si91x_ssi_set_configuration 
 *      - \ref sl_si91x_ssi_set_slave_number
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * @param[in] data Pointer to the variable which will store the received data.
 * @param[in] data_length (uint32_t) Number of data items to receive.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_FAIL               - Function failed.
 *         - SL_STATUS_BUSY               - Driver is busy.
 *         - SL_STATUS_INVALID_PARAMETER  - Parameters are invalid.
 *         - SL_STATUS_NULL_POINTER       - The parameter is a null pointer.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_receive_data(sl_ssi_handle_t ssi_handle, void *data, uint32_t data_length);

/***************************************************************************/
/**
 * @brief To send data to the secondary device.
 * 
 * @details This API will send data to the secondary device. If DMA is enabled, it configures the DMA channel and required parameters.
 *          When the sent data is equal to the data_length passed in this function, a callback event is generated which can be registered using \ref sl_si91x_ssi_register_event_callback.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init 
 *      - \ref sl_si91x_ssi_set_configuration 
 *      - \ref sl_si91x_ssi_set_slave_number
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * @param[in] data Pointer to the variable which will store the data to be sent.
 * @param[in] data_length (uint32_t) Number of data items to send.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_FAIL               - Function failed.
 *         - SL_STATUS_BUSY               - Driver is busy.
 *         - SL_STATUS_INVALID_PARAMETER  - Parameters are invalid.
 *         - SL_STATUS_NULL_POINTER       - The parameter is a null pointer.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_send_data(sl_ssi_handle_t ssi_handle, const void *data, uint32_t data_length);

/***************************************************************************/
/**
 * @brief To send and receive data to the secondary device simultaneously.
 * 
 * @details If DMA is enabled, it configures the DMA channel and required parameters.
 *          When the received data and sent data are equal to the data_length passed in this function,
 *          a callback event is generated which can be registered using \ref sl_si91x_ssi_register_event_callback.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init 
 *      - \ref sl_si91x_ssi_set_configuration 
 *      - \ref sl_si91x_ssi_set_slave_number
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * @param[in] data_out Const pointer to the variable that has data which needs to be sent.
 * @param[in] data_in Pointer to the variable which will store the received data.
 * @param[in] data_length (uint32_t) Number of data items to receive.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success.
 *         - SL_STATUS_FAIL               - Function failed.
 *         - SL_STATUS_BUSY               - Driver is busy.
 *         - SL_STATUS_INVALID_PARAMETER  - Parameters are invalid.
 *         - SL_STATUS_NULL_POINTER       - The parameter is a null pointer.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_transfer_data(sl_ssi_handle_t ssi_handle,
                                       const void *data_out,
                                       void *data_in,
                                       uint32_t data_length);

/***************************************************************************/
/**
 * @brief To get the SSI version.
 * 
 * @details This API retrieves the SSI version. It returns the API version of SSI.
 * 
 * @return sl_ssi_version_t Structure containing the driver version.
 ******************************************************************************/
sl_ssi_version_t sl_si91x_ssi_get_version(void);

/***************************************************************************/
/**
 * @brief To get the transfer status of SSI.
 * 
 * @details This API retrieves the transfer status of SSI. It returns the \ref sl_ssi_status_t type structure.
 *          The members are:
 *          - Busy
 *          - Data Lost
 *          - Mode Fault
 *          It is generally used to poll the busy status of SSI Master/ULP Master.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init
 *      - \ref sl_si91x_ssi_set_configuration
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return sl_ssi_status_t Structure containing the transfer status.
 ******************************************************************************/
sl_ssi_status_t sl_si91x_ssi_get_status(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To get the data receive count of the SSI.
 * 
 * @details This API retrieves the data receive count of the SSI and returns the number of 
 *          data items received at the time of the function call.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init 
 *      - \ref sl_si91x_ssi_set_configuration 
 *  
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t Value of the RX data count.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_rx_data_count(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To get the transmit data count of the SSI.
 * 
 * @details This API retrieves the transmit data count of the SSI. It returns the number of 
 *          data items transmitted at the time of the function call.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_init 
 *      - \ref sl_si91x_ssi_set_configuration 
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t Value of the TX data count.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_tx_data_count(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To register the user event callback.
 * 
 * @details This API registers the callback (that is, stores the callback function address
 *          and passes it to the variable that is called in the Interrupt Handler).
 *          If another callback is registered without unregistering the previous callback, 
 *          it returns an error code as follows. Therefore, it is mandatory to unregister 
 *          the callback before registering another callback.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * @param[in] callback_event Pointer to the function which needs to be called at the time of interrupt.
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK            - Success.
 *         - SL_STATUS_BUSY          - The callback is already registered, unregister the previous callback before registering a new one.
 *         - SL_STATUS_NULL_POINTER  - The parameter is a null pointer.
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
sl_status_t sl_si91x_ssi_register_event_callback(sl_ssi_handle_t ssi_handle, sl_ssi_signal_event_t callback_event);

/***************************************************************************/
/**
 * @brief To unregister the user event callback.
 * 
 * @details This API unregisters the callback, that is, clears the callback function address
 *          and passes a NULL value to the variable.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_register_event_callback
 ******************************************************************************/
void sl_si91x_ssi_unregister_event_callback(void);

/***************************************************************************/
/**
 * @brief To unregister the user event callback on a per-instance basis.
 *
 * @details This API unregisters the callback on a per-instance basis, that is,
 *          clears the callback function address and NULL is passed to the callback
 *          function pointer of instance.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ssi_register_event_callback
 *
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
*******************************************************************************/
void sl_si91x_ssi_per_instance_unregister_event_callback(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To fetch the clock division factor.
 * 
 * @details The clock division factor is calculated based on the peripheral clock configured. 
 *          It decides the baud rate of SSI.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t The value of the clock division factor in the range of 0 - 127.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_clock_division_factor(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To fetch the frame length (bit width) of the SSI.
 * 
 * @details This API retrieves the frame length, which ranges between 4 and 16 bits.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t The value of the frame length.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_frame_length(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To fetch the transmit FIFO threshold value.
 * 
 * @details This API retrieves the transmit FIFO threshold value, which controls the level of
 *          entries at which the transmit FIFO controller triggers an interrupt.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t The value of the transmit FIFO threshold.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_tx_fifo_threshold(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To fetch the receiver FIFO threshold value.
 * 
 * @details This API retrieves the receiver FIFO threshold value, which controls the level of
 *          entries at which the receive FIFO controller triggers an interrupt.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t The value of the receiver FIFO threshold.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_rx_fifo_threshold(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To fetch the receiver sample delay value.
 * 
 * @details This API is used to delay the sample of the RX input signal. Each value represents 
 *          a single ssi_clk delay on the sample of the rxd signal.
 * 
 * @param[in] ssi_handle Pointer to the SSI driver handle (\ref sl_ssi_handle_t).
 * 
 * @return uint32_t The value of the receiver sample delay.
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_receiver_sample_delay(sl_ssi_handle_t ssi_handle);

/***************************************************************************/
/**
 * @brief To set the secondary number in multi-secondary operation.
 * 
 * @details For single secondary operation, this API also needs to be called before transferring the data.
 * 
 * @param[in] number Secondary number (\ref sl_ssi_slave_number_t )
 * 
 * @return sl_status_t Status code indicating the result:
 *         - SL_STATUS_OK                 - Success
 *         - SL_STATUS_INVALID_PARAMETER  - Invalid parameter
 * 
 * For more information on status codes, see [SL STATUS DOCUMENTATION](https://docs.silabs.com/gecko-platform/latest/platform-common/status).
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_si91x_ssi_set_slave_number(uint8_t number)
{
  sl_status_t status;
  if (number >= SSI_SLAVE_NUMBER_LAST_ENUM) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    RSI_SPI_SetSlaveSelectNumber(number);
    status = SL_STATUS_OK;
  }
  return status;
}

/// @} end group SSI ********************************************************/

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/** 
* @addtogroup SSI
* @{
*   @details
*
*   @n @section SSI_Intro Introduction
*
*   The Synchronous Serial Interface (SSI) is a communication protocol used to transfer data synchronously over a
*   serial interface between a microcontroller and a peripheral device. A clock signal is used to synchronize
*   the data transfer.
*
*   @li The Serial Peripheral Interface (SPI) or Microwire interfaces are typically used to implement an SSI connection.
*
*   @li The SiWx91x device supports SSI over SPI.
*
*   @li SSI is widely supported in microcontroller architectures and is simple and flexible.
*
*   @image html ssi_block.png "SSI Block Diagram"
*
*   This mode enables serial communication with Master peripheral devices. 
*   All serial transfers are initiated and controlled by the serial bus Master. The figure above shows an example of the SSI 
*   configured as a serial Slave in a single-Master bus system. 
*   All data transfers to and from the serial Slave are regulated by the serial clock line (sclk_in), driven 
*   by the serial Master device. Data is propagated from the serial Slave on one edge of the serial clock line 
*   and sampled on the opposite edge. 
*   The Slave remains in an idle state until selected by the bus Master. When not 
*   actively transmitting data, the Slave must hold its txd line in a high-impedance state to avoid interference 
*   with serial transfers to other Slave devices.
*
*   @n @section SSI_Config Configuration
*
*   @li SSI allows the configuration of several parameters. The mode of the SSI peripheral clock, such as clock polarity and clock phase,
*       can be set using @ref ssi_peripheral_clock_mode_t. The SSI peripheral device mode
*       (SSI master/SSI slave/ULP SSI Master) can be configured using @ref sl_ssi_instance_t.
*       The bit width may be adjusted between 4 and 16, and the bit rate between 500,000 and 40,000,000 bits per second.
*       All configurations may be done using a structure @ref sl_ssi_control_config_t by sending this structure to the API
*       @ref sl_si91x_ssi_set_configuration().
*       The SSI slave number can be set using @ref sl_ssi_slave_number_t and called through the API @ref sl_si91x_ssi_set_slave_number().
*
*   @li For more information on configuring available parameters, see the respective peripheral example readme document.
*
*   @n @section SSI_Usage Usage
*
*   Most of the SSI functions take @ref sl_ssi_control_config_t as an argument and are invoked in
*   the following order:
*
*   1. Initialize the SSI using @ref sl_si91x_ssi_init.
*   2. Control and configure the SSI using @ref sl_si91x_ssi_set_configuration.
*   3. Register the user event callback using @ref sl_si91x_ssi_register_event_callback.
*   4. Send data to the secondary device using @ref sl_si91x_ssi_send_data.
*   5. Uninitialize the SSI using @ref sl_si91x_ssi_deinit.
*
* @} (end addtogroup SSI ) ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_SSI_H */
