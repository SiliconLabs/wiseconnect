/***************************************************************************/ /**
* @file sl_si91x_ssi.h
* @brief SSI Peripheral API implementation
* *******************************************************************************
* * # License
* * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
* *******************************************************************************
* *
* * SPDX-License-Identifier: Zlib
* *
* * The licensor of this software is Silicon Laboratories Inc.
* *
* * This software is provided 'as-is', without any express or implied
* * warranty. In no event will the authors be held liable for any damages
* * arising from the use of this software.
* *
* * Permission is granted to anyone to use this software for any purpose,
* * including commercial applications, and to alter it and redistribute it
* * freely, subject to the following restrictions:
* *
* * 1. The origin of this software must not be misrepresented; you must not
* *    claim that you wrote the original software. If you use this software
* *    in a product, an acknowledgment in the product documentation would be
* *    appreciated but is not required.
* * 2. Altered source versions must be plainly marked as such, and must not be
* *    misrepresented as being the original software.
* * 3. This notice may not be removed or altered from any source distribution.
* *
*******************************************************************************/

#ifndef SL_SI91X_SSI_H
#define SL_SI91X_SSI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "SPI.h"

/***************************************************************************/ /**
 * @addtogroup SSI Synchronous Serial Interface
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 * 
 ******************************************************************************/

// -----------------------------------------------------------------------------
// //Macros

// Data Types
typedef ARM_POWER_STATE sl_ssi_power_state_t;        ///< renaming arm power state structure
typedef ARM_SPI_STATUS sl_ssi_status_t;              ///< renaming arm SPI status
typedef ARM_DRIVER_SPI sl_ssi_driver_t;              ///< Renaming SSI driver structure
typedef const void *sl_ssi_handle_t;                 ///< SSI Handle to be passed into APIs
typedef ARM_SPI_SignalEvent_t sl_ssi_signal_event_t; ///< Callback typedef for SSI

typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_ssi_version_t;

/// @brief Enumeration for different SSI callback events
typedef enum {
  SSI_EVENT_TRANSFER_COMPLETE = ARM_SPI_EVENT_TRANSFER_COMPLETE, ///< Transfer complete event
  SSI_EVENT_DATA_LOST         = ARM_SPI_EVENT_DATA_LOST,         ///< Data lost event
  SSI_EVENT_MODE_FAULT        = ARM_SPI_EVENT_MODE_FAULT,        ///< Mode fault event
} ssi_event_typedef_t;

// SSI Master clock mode, 4 modes with 2 bits
// for CPOL and CPHASE, TI SSI mode and Microwire
// mode.
/// @brief Enumeration for different SSI peripheral clock modes
typedef enum {
  SL_SSI_PERIPHERAL_CPOL0_CPHA0 = ARM_SPI_CPOL0_CPHA0, ///< SSI_PERIPHERAL MODE 0 CPOL0_CPHA0
  SL_SSI_PERIPHERAL_CPOL0_CPHA1 = ARM_SPI_CPOL0_CPHA1, ///< SSI_PERIPHERAL MODE 1 CPOL0_CPHA1
  SL_SSI_PERIPHERAL_CPOL1_CPHA0 = ARM_SPI_CPOL1_CPHA0, ///< SSI_PERIPHERAL MODE 2 CPOL1_CPHA0
  SL_SSI_PERIPHERAL_CPOL1_CPHA1 = ARM_SPI_CPOL1_CPHA1, ///< SSI_PERIPHERAL MODE 3 CPOL1_CPHA1
  SL_SSI_PERIPHERAL_TI_SSI      = ARM_SPI_TI_SSI,      ///< SSI_PERIPHERAL MODE TI_SSI
  SL_SSI_PERIPHERAL_MICROWIRE   = ARM_SPI_MICROWIRE,   ///< SSI_PERIPHERAL MODE MICROWIRE
  SL_SSI_PERIPHERAL_MODE_LAST                          ///< SSI_PERIPHERAL MODE_LAST
} ssi_peripheral_clock_mode_t;

// Master Mode - whether inactive or
// active
/// @brief Enumeration for different SSI peripheral device modes
typedef enum {
  SL_SSI_MASTER_ACTIVE = ARM_SPI_MODE_MASTER, ///< SSI DEVICE MODE MASTER
  SL_SSI_SLAVE_ACTIVE  = ARM_SPI_MODE_SLAVE,  ///< SSI DEVICE MODE SLAVE
  SL_SSI_ULP_MASTER_ACTIVE,
  SL_SSI_MASTER_MODE_LAST,
} ssi_peripheral_device_mode_t;

// Master slave select
// mode - whether software or
// hardware output
/// @brief Enumeration for different SSI master slave select modes
typedef enum {
  SL_SSI_MASTER_UNUSED    = ARM_SPI_SS_MASTER_UNUSED,    ///< SSI MASTER UNUSED
  SL_SSI_MASTER_SW        = ARM_SPI_SS_MASTER_SW,        ///< SSI MASTER SW
  SL_SSI_MASTER_HW_OUTPUT = ARM_SPI_SS_MASTER_HW_OUTPUT, ///< SSI MASTER HW OUTPUT
  SL_SSI_MASTER_SLAVE_SELECT_MODE_LAST,
} ssi_master_ssm_t;

/// @brief Enumeration for different SSI slave slave select modes.
typedef enum {
  SL_SSI_SLAVE_HW = ARM_SPI_SS_SLAVE_HW, ///< SSI SLAVE HW
  SL_SSI_SLAVE_SW = ARM_SPI_SS_SLAVE_SW, ///< SSI SLAVE SW
  SL_SSI_SLAVE_MODE_SELECT_LAST          ///< Last member of slave mode select
} ssi_slave_ssm_t;

/// @brief Enumeration for different SSI instances
typedef enum {
  SL_SSI_MASTER,             ///< SSI MASTER INSTANCE
  SL_SSI_SLAVE,              ///< SSI SLAVE INSTANCE
  SL_SSI_ULP_MASTER,         ///< SSI ULP MASTER INSTANCE
  SL_SSI_INSTANCE_LAST_ENUM, ///< Last member of enum for validation
} sl_ssi_instance_t;

// SSI control config data structure with
// fields for various parameters
/// @brief typedef for SSI control config struct
typedef struct {
  uint8_t bit_width;    ///< bit width either 8 or 16 bit
  uint32_t device_mode; ///< mode such as Master or Slave mode
  uint32_t clock_mode;  ///< clock mode such as CPOL0 CPHA1
  uint32_t master_ssm;  ///< Master SW or Master HW Output
  uint32_t slave_ssm;   ///< Slave SW or Slave HW Output
  uint32_t baud_rate;   ///< baud rate for SSI
} sl_ssi_control_config_t;

// SSI clock config data structure with
// fields for various clock parameters
/// @brief typedef for SSI clock config struct
typedef struct {
  uint16_t division_factor;            ///< Clock Division Factor
  uint16_t intf_pll_500_control_value; ///< intf PLL control value
  uint32_t intf_pll_clock;             ///< intf PLL clock frequency
  uint32_t intf_pll_reference_clock;   ///< intf PLL reference clock frequency
  uint32_t soc_pll_clock;              ///< SoC PLL Clock frequency
  uint32_t soc_pll_reference_clock;    ///< SoC PLL reference clock frequency
  uint8_t soc_pll_mm_count_value;      ///< SoC PLL count value
} sl_ssi_clock_config_t;

/// @brief Enumeration for SSI Slave numbers.
typedef enum {
  SSI_SLAVE_0,         ///< Slave No. 1
  SSI_SLAVE_1,         ///< Slave No. 2
  SSI_SLAVE_2,         ///< Slave No. 2
  SSI_SLAVE_LAST_ENUM, ///< Last member of enum for validation
} sl_ssi_slave_number_t;

/***************************************************************************/ /**
 * Configure the SSI clock.
 *
 * @param[in] pointer to clock config structure
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_ssi_configure_clock(sl_ssi_clock_config_t *clock_config);

/***************************************************************************/ /**
 * Initialize the SSI master.
 *
 * @param[in] instance (Master/Slave/ULP Master), ssi_handle
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, otherwise fail error code
*******************************************************************************/
sl_status_t sl_si91x_ssi_init(sl_ssi_instance_t instance, sl_ssi_handle_t *ssi_handle);

/*******************************************************************************
 * Uninitialize the SSI primary.
 *
 * @param[in] SSI Handle
 * @return    status 0 if successful, else error code.
 *
 *         SL_STATUS_OK (0x0000) - Success, otherwise fail error code
*******************************************************************************/
sl_status_t sl_si91x_ssi_deinit(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Control the SPI interface power.
 *
 * @param[in] ssi handle, power state power off or full state.
 * @return    status 0 if successful, else error code.
 *         SL_STATUS_INVALID_PARAMETER (0x0021) - power state is invalid
 *         SL_STATUS_OK (0x0000) - Success, otherwise error code on failure
*******************************************************************************/
sl_status_t sl_si91x_ssi_configure_power_mode(sl_ssi_handle_t ssi_handle, sl_ssi_power_state_t state);

/***************************************************************************/ /**
 * Control the SPI interface.
 *
 * @param[in]  ssi handle, pointer to control config structure.
 * @return    status 0 if successful, else error code.
 *         SL_STATUS_INVALID_PARAMETER (0x0021) - invalid parameters
 *         SL_STATUS_OK (0x0000) - Success,
*******************************************************************************/
sl_status_t sl_si91x_ssi_set_configuration(sl_ssi_handle_t ssi_handle, sl_ssi_control_config_t *control_configuration);

/***************************************************************************/ /**
 * Start receiving data from the SPI interface.
 *
 * @param[in] ssi handle, pointer to Rx data buffer, length of data
 * @return    status 0 if successful, else error code.
 *         SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         SL_STATUS_OK (0x0000) - Success
*******************************************************************************/
sl_status_t sl_si91x_ssi_receive_data(sl_ssi_handle_t ssi_handle, void *data, uint32_t data_length);

/***************************************************************************/ /**
 * Start sending data from SPI interface.
 *
 * @param[in] ssi handle, pointer to Tx data buffer, length of data
 * @return    status 0 if successful, else error code.
 *         SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         SL_STATUS_OK (0x0000) - Success
*******************************************************************************/
sl_status_t sl_si91x_ssi_send_data(sl_ssi_handle_t ssi_handle, const void *data, uint32_t data_length);

/***************************************************************************/ /**
 * Start sending/receiving bi-directional full duplex data to/from SPI secondary.
 *
 * @param[in] ssi handle, pointer to Tx buffer, pointer to Rx buffer and data
 *            length
 * @return    status 0 if successful, else error code.
 *         SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         SL_STATUS_OK (0x0000) - Success, otherwise error code on failure.
*******************************************************************************/
sl_status_t sl_si91x_ssi_transfer_data(sl_ssi_handle_t ssi_handle,
                                       const void *data_out,
                                       void *data_in,
                                       uint32_t data_length);

/***************************************************************************/ /**
 * Get the SSI status.
 *
 * @param[in] ssi handle
 * @return    busy, data lost or mode fault returns as a bit field.
 *
*******************************************************************************/
sl_ssi_status_t sl_si91x_ssi_get_status(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
* Get the driver version.
*
* @param[in] None
* @return    driver version.
*
*******************************************************************************/
sl_ssi_version_t sl_si91x_ssi_get_version(void);

/***************************************************************************/ /**
* Get the SPI secondary status.
*
* @param[in] ssi_handle
* @return    busy, data lost or mode fault
*******************************************************************************/
sl_ssi_status_t sl_si91x_ssi_get_status(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Get RX transferred data count with connected device.
 *
 * @param[in] ssi handle
 * @return   transferred data count
*******************************************************************************/
uint32_t sl_si91x_ssi_get_rx_data_count(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Get TX transferred data count with connected device.
 *
 * @param[in] ssi handle
 * @return   transferred data count
*******************************************************************************/
uint32_t sl_si91x_ssi_get_tx_data_count(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Register the user event callback.
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback then, it
 * returns an error code, so it is mandatory to unregister the callback before registering
 * another callback.
 *
 *  @param[in] ssi handle
 *  @param[in] function pointer callback_event
 * It will returns error if any callback is already registered.
 ******************************************************************************/
sl_status_t sl_si91x_ssi_register_event_callback(sl_ssi_handle_t ssi_handle, sl_ssi_signal_event_t callback_event);

/***************************************************************************/ /**
 * Unregister the user event callback.
 * It unregisters the callback, i.e., clears the callback function address
 * and passes a NULL value to the variable.
 *
 *  @param[in] None
 ******************************************************************************/
void sl_si91x_gspi_unregister_event_callback(void);

/***************************************************************************/ /**
 * Get the clock division factor.
 *
 * @param[in] ssi_handle Pointer to the SSI driver handle
 * @return factor(int32_t) Clock division factor
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_clock_division_factor(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Get the frame length (bit width).
 *
 * @param[in] ssi_handle Pointer to the SSI driver handle
 * @return frame_length (uint32_t) Frame length
 ******************************************************************************/
uint32_t sl_si91x_ssi_get_frame_length(sl_ssi_handle_t ssi_handle);

/***************************************************************************/ /**
 * Set the secondary number in multi-secondary operation.
 * For single secondary also, this API needs to be called before transferring the
 * data.
 * 
 * @param[in] number Slave number
 * @return none
 ******************************************************************************/
__STATIC_INLINE sl_status_t sl_si91x_ssi_set_slave_number(uint8_t number)
{
  sl_status_t status;
  if (number > SSI_SLAVE_LAST_ENUM) {
    status = SL_STATUS_INVALID_PARAMETER;
  } else {
    RSI_SPI_SetSlaveSelectNumber(number);
    status = SL_STATUS_OK;
  }
  return status;
}

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_SSI_H */
