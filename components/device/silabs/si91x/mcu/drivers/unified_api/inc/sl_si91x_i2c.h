/***************************************************************************/ /**
 * @file sl_si91x_i2c.h
 * @brief I2C API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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
//// Includes
#ifndef SL_SI91X_I2C_H
#define SL_SI91X_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_si91x_peripheral_i2c.h"
#include "stdio.h"
#include "sl_i2c_board.h"

/***************************************************************************/ /**
* @addtogroup I2C
* @ingroup SI91X_PERIPHERAL_APIS
* @{
******************************************************************************/
/***************************************************************************/ /**
* @brief I2C Peripheral
*
* ##Overview
* I2C standard compliant bus interface with open-drain pins
* Configurable as Master or Slave
* Four speed modes: Standard Mode (100 kbps), Fast Mode (400 kbps),
* Fast Mode Plus (1Mbps) and High-Speed Mode (3.4 Mbps)
* 7 or 10-bit addressing
* 7 or 10-bit combined format transfers
* Support for Clock synchronization and Bus Clear
* Programmable SDA Hold time
* Integrated transmit and receive buffers with support for DMA
* Bulk transmit mode in I2C Slave mode
* Interrupt based operation (polled mode also available)
*
* ##Initialization
* Call init API with the init parameters.
* Call set follower address API, in follower mode
* Call Set FIFO threshold API
*
******************************************************************************/

// -----------------------------------------------------------------------------
// Macros for i2c parameters
#define SL_I2C0_DMA_TX_CHANNEL 31 // I2C0 DMA TX channel number
#define SL_I2C0_DMA_RX_CHANNEL 30 // I2C0 DMA RX channel number
#define SL_I2C1_DMA_TX_CHANNEL 3  // I2C1 DMA TX channel number
#define SL_I2C1_DMA_RX_CHANNEL 2  // I2C1 DMA RX channel number
#define SL_I2C2_DMA_TX_CHANNEL 5  // I2C2 DMA TX channel number
#define SL_I2C2_DMA_RX_CHANNEL 4  // I2C2 DMA RX channel number
// -----------------------------------------------------------------------------
// Data Types

/// @brief Enumeration to represent i2c instances
typedef enum {
  SL_I2C0,     ///< I2C Instance 0.
  SL_I2C1,     ///< I2C Instance 1.
  SL_I2C2,     ///< I2C Instance 2.
  SL_I2C_LAST, ///< Last member of enum for validation
} sl_i2c_instance_t;

/// @brief Enumeration to represent i2c driver status values
typedef enum {
  SL_I2C_SUCCESS,                  ///< Success.
  SL_I2C_IDLE,                     ///< I2C Idle.
  SL_I2C_7BIT_ADD,                 ///< I2C 7 Bit address transfer.
  SL_I2C_10BIT_ADD,                ///< I2C 10 Bit address transfer.
  SL_I2C_10BIT_ADD_WITH_REP_START, ///< I2C 10 Bit address transfer with Repeated Start.
  SL_I2C_ACKNOWLEDGE,              ///< I2C ACK.
  SL_I2C_NACK,                     ///< I2C NACK.
  SL_I2C_DATA_TRANSFER_COMPLETE,   ///< I2C data transfer complete.
  SL_I2C_ARIBITRATION_LOST,        ///< I2C Arbitration Lost.
  SL_I2C_BUS_ERROR,                ///< I2C Bus Error.
  SL_I2C_BUS_HOLD,                 ///< I2C Bus held by I2C Bus.
  SL_I2C_SDA_ERROR,                ///< I2C SDA at loopback path is not equal to SDA output.
  SL_I2C_SCL_ERROR,                ///< I2C SCL at loopback path is not equal to SCL output.
  SL_I2C_CALLBACK_BUSY,            ///< I2C instance callback already registered
  SL_I2C_DMA_TRANSFER_ERROR,       ///< I2C DMA transfer error
  SL_I2C_INVALID_PARAMETER,        ///< Invalid Parameter.
} sl_i2c_status_t;

/// @brief Enumeration to represent i2c transfer type
typedef enum {
  SL_I2C_USING_INTERRUPT,   ///< The driver will use interrupts to perform I2C transfer.
  SL_I2C_USING_DMA,         ///< The driver will use DMA to perform I2C transfer.
  SL_I2C_TRANFER_TYPE_LAST, ///< For Validation
} sl_i2c_transfer_type_t;

/// @brief Enumeration to represent i2c operating mode
typedef enum {
  SL_I2C_STANDARD_MODE = 1,   ///< Standard-mode, bidirectional data transfers up to 100 kbit/s.
  SL_I2C_FAST_MODE,           ///< Fast-mode, bidirectional data transfers up to 400 kbit/s.
  SL_I2C_HIGH_SPEED_MODE,     ///< High speed-mode, bidirectional data transfers up to 3.4 Mbit/s.
  SL_I2C_FAST_PLUS_MODE,      ///< Fast-mode Plus, bidirectional data transfers up to 1 Mbit/s.
  SL_I2C_OPERATING_MODE_LAST, ///< Last member of enum for validation
} sl_i2c_operating_mode_t;

typedef void (*sl_i2c_callback_t)(sl_i2c_instance_t i2c_instance, uint32_t status); ///< Callback for I2C Driver

/// @brief Structure to hold the parameters of i2c instance configurations
typedef struct {
  sl_i2c_mode_t mode;                     ///< Leader/Follower Mode, 0 for leader mode and 1 for follower mode
  sl_i2c_operating_mode_t operating_mode; ///< Speed mode \ref sl_i2c_operating_mode_t for possible values
  sl_i2c_transfer_type_t transfer_type;   ///< Transfer type \ref sl_i2c_transfer_type_t for possible values
  sl_i2c_callback_t i2c_callback;         ///< I2C callback \ref sl_i2c_callback_t
} sl_i2c_config_t;

/// @brief Structure to hold the parameters of DMA configuration
typedef struct {
  uint32_t dma_tx_channel; ///< DMA transmit channel number
  uint32_t dma_rx_channel; ///< DMA receive channel number
} sl_i2c_dma_config_t;

/// @brief Structure to hold the parameters of I2C transfer configuration
typedef struct {
  uint8_t *tx_buffer; ///< Pointer to Tx Data buffer
  uint32_t tx_len;    ///< Number of bytes to transmit
  uint8_t *rx_buffer; ///< Pointer to Rx Data buffer
  uint32_t rx_len;    ///< Number of bytes to receive
} sl_i2c_transfer_config_t;

///@brief Structure to hold port and pin of i2c
typedef struct {
  uint8_t sda_port; ///< PWM GPIO port
  uint8_t sda_pin;  ///< PWM GPIO pin
  uint8_t sda_mux;  ///< PWM GPIO mux
  uint8_t sda_pad;  ///< PWM GPIO pad
  uint8_t scl_port; ///< PWM GPIO port
  uint8_t scl_pin;  ///< PWM GPIO pin
  uint8_t scl_mux;  ///< PWM GPIO mux
  uint8_t scl_pad;  ///< PWM GPIO pad
} sl_i2c_pin_init_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/
/**
 * Initialize the I2C Module and clock. Sets I2C instance mode,
 * operating mode (bus-speed), frequency, and transfer type (using Interrupt or DMA).
 * If the transfer type is DMA, it initializes DMA as well.
 * Registers I2C instance callback and clears pending interrupts.
 * Configures SDL and SCL pins as per the instance.
 * 
 * @param[in] i2c_instance I2C Instance to be initialized \ref sl_i2c_instance_t
 * @param[in] p_user_config A pointer to I2C configuration structure \ref sl_i2c_config_t
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 *         - \ref SL_I2C_CALLBACK_BUSY (0x0D) - Driver is busy
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_init(sl_i2c_instance_t i2c_instance, const sl_i2c_config_t *p_user_config);

/***************************************************************************/
/**
 * This API configures the follower address of the I2C module. Should be
 * used only in Follower mode after instance initialization.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here I2C mode should be set as follower mode
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower own address
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_set_follower_address(sl_i2c_instance_t i2c_instance, uint16_t address);

/***************************************************************************/
/**
 * This function is used to configure the TX and RX FIFO threshold values.
 * The valid range is 0-255, with the additional restriction that hardware does
 * not allow this value to be set to a value larger than the depth of the buffer.
 * A value of 0 sets the threshold for 1 entry,
 * and a value of 255 sets the threshold for 256 entries.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] tx_threshold_value Transmit FIFO threshold value
 * @param[in] rx_threshold_value Receive FIFO threshold value
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_configure_fifo_threshold(sl_i2c_instance_t i2c_instance,
                                                       uint8_t tx_threshold_value,
                                                       uint8_t rx_threshold_value);

/***************************************************************************/
/**
 * This API gets the current frequency of I2C transfer in MHz,
 * by reading the system core clock frequency.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[out] frequency Currently configured frequency.
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_get_frequency(sl_i2c_instance_t i2c_instance, uint32_t *frequency);

/***************************************************************************/
/**
 * This function sends the data in blocking mode (using interrupt),
 * also sets the follower address when used in Leader application.
 * Sets transmit empty interrupt and enables I2C interrupts.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address (if sending from slave)
 *      - \ref sl_i2c_driver_configure_fifo_threshold
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address, in follower mode, this parameter will be ignored
 * @param[in] tx_buffer A pointer to transmit data buffer
 * @param[in] tx_len Data length in number of bytes
 * @note Maximum tx_len used can be 80000 (receives in around 10 seconds)
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_send_data_blocking(sl_i2c_instance_t i2c_instance,
                                                 uint16_t address,
                                                 uint8_t *tx_buffer,
                                                 uint32_t tx_len);

/***************************************************************************/
/**
 * This function sends the data in non-blocking mode (using DMA),
 * also sets the follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as DMA-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in salve application
 * 
 * @param[in] i2c_instance I2C Instance.
 * @param[in] address Follower address
 * @param[in] tx_buffer A pointer to transmit data buffer
 * @param[in] tx_len Data length in number of bytes
 * @param[in] dma_config A pointer to DMA configuration structure \ref sl_i2c_dma_config_t
 * @note Maximum tx_len values can be 30000 (receives back in around 4 seconds)
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 *         - \ref SL_DMA_TRANSFER_ERROR - DMA parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_send_data_non_blocking(sl_i2c_instance_t i2c_instance,
                                                     uint16_t address,
                                                     uint32_t *tx_buffer,
                                                     uint32_t tx_len,
                                                     sl_i2c_dma_config_t *p_dma_config);

/***************************************************************************/
/**
 * This function receives the data in blocking mode (using interrupt),
 * also sets the follower address when used in Leader application.
 * Sets receive full interrupt while receiving data.
 * Sets transmit empty interrupt while sending data.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in follower application
 *      - \ref sl_i2c_driver_configure_fifo_threshold
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address, in follower mode this parameter will be ignored
 * @param[in] rx_buffer A pointer to receive data buffer
 * @param[in] rx_len Data length in number of bytes
 * @note Maximum rx_len used can be 80000 (receives in around 10 seconds)
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_receive_data_blocking(sl_i2c_instance_t i2c_instance,
                                                    uint16_t address,
                                                    uint8_t *rx_buffer,
                                                    uint32_t rx_len);

/***************************************************************************/
/**
 * This function receives the data in non-blocking mode (using DMA),
 * also sets the follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as DMA-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in salve application
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address
 * @param[in] rx_buffer A pointer to receive data buffer
 * @param[in] rx_len Data length in number of bytes
 * @param[in] dma_config A pointer to DMA configuration structure \ref sl_i2c_dma_config_t
 * @note Maximum rx_len values can be 30000 (receives back in around 4 seconds)
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 *         - \ref SL_DMA_TRANSFER_ERROR - DMA parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_receive_data_non_blocking(sl_i2c_instance_t i2c_instance,
                                                        uint16_t address,
                                                        uint8_t *rx_buffer,
                                                        uint32_t rx_len,
                                                        sl_i2c_dma_config_t *p_dma_config);

/***************************************************************************/
/**
 * This function transmits and receives the data in blocking mode
 * also sets the follower address when used in Leader application.
 * Sets receive full interrupt and enables I2C interrupts.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as Interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in follower application
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address
 * @param[in] p_transfer_config A pointer to transfer configuration structure \ref sl_i2c_transfer_config_t
 * @note Maximum tx_len & rx_len values can be 80000 (sends & receives back in around 20 seconds)
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_transfer_data(sl_i2c_instance_t i2c_instance,
                                            sl_i2c_transfer_config_t const *p_transfer_config,
                                            uint16_t address);

/***************************************************************************/
/**
 * De-initializes the I2C peripheral, disables clock, and unregisters the callback.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init
 * 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_deinit(sl_i2c_instance_t i2c_instance);

/***************************************************************************/
/**
 * Set the Pin configuration for I2C. It configures the SDA and SCL pins.
 * 
 * @param[in] pin_init Pointer to pin init structure \ref sl_i2c_pin_init_t
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_si91x_i2c_pin_init(sl_i2c_pin_init_t *pin_init);

/** @} (end addtogroup I2C) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_I2C_H */
