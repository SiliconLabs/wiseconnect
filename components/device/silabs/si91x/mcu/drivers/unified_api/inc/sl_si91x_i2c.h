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
* Configurable as Primary or Secondary
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
#define SL_I2C0_DMA_TX_CHANNEL 31      // I2C0 DMA TX channel number
#define SL_I2C0_DMA_RX_CHANNEL 30      // I2C0 DMA RX channel number
#define SL_I2C1_DMA_TX_CHANNEL 3       // I2C1 DMA TX channel number
#define SL_I2C1_DMA_RX_CHANNEL 2       // I2C1 DMA RX channel number
#define SL_I2C2_DMA_TX_CHANNEL 5       // I2C2 DMA TX channel number
#define SL_I2C2_DMA_RX_CHANNEL 4       // I2C2 DMA RX channel number
#define ULP_I2C                SL_I2C2 // defining ULP_I2C instance

// -----------------------------------------------------------------------------
// Data Types

/// @brief Enumeration to represent I2C instances
typedef enum {
  SL_I2C0,     ///< I2C Instance 0.
  SL_I2C1,     ///< I2C Instance 1.
  SL_I2C2,     ///< I2C Instance 2.
  SL_I2C_LAST, ///< Last member of enum for validation
} sl_i2c_instance_t;

/// @brief Enumeration to represent I2C driver status values
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

/// @brief Enumeration to represent I2C transfer type
typedef enum {
  SL_I2C_USING_INTERRUPT,   ///< The driver will use interrupts to perform I2C transfer.
  SL_I2C_USING_DMA,         ///< The driver will use DMA to perform I2C transfer.
  SL_I2C_TRANFER_TYPE_LAST, ///< For Validation
} sl_i2c_transfer_type_t;

/// @brief Enumeration to represent I2C operating mode
typedef enum {
  SL_I2C_STANDARD_MODE = 1,   ///< Standard-mode, bidirectional data transfers up to 100 kbit/s.
  SL_I2C_FAST_MODE,           ///< Fast-mode, bidirectional data transfers up to 400 kbit/s.
  SL_I2C_HIGH_SPEED_MODE,     ///< High speed-mode, bidirectional data transfers up to 3.4 Mbit/s.
  SL_I2C_FAST_PLUS_MODE,      ///< Fast-mode Plus, bidirectional data transfers up to 1 Mbit/s.
  SL_I2C_OPERATING_MODE_LAST, ///< Last member of enum for validation
} sl_i2c_operating_mode_t;

typedef void (*sl_i2c_callback_t)(sl_i2c_instance_t i2c_instance, uint32_t status); ///< Callback for I2C Driver

/// @brief Structure to hold the parameters of I2C instance configurations
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

///@brief Structure to hold port and pin of I2C
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

/// @brief Enumeration to represent I2C power modes
typedef enum {
  SL_I2C_ULP_MODE,        ///< The driver leader ULP mode
  SL_I2C_HP_MODE,         ///< The driver leader HP mode
  SL_I2C_POWER_MODE_LAST, ///< For Validation
} sl_i2c_power_modes_t;
// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/
/**
 * @brief Initialize the I2C Module and clock. 
 * @details Sets I2C instance mode,operating mode (bus-speed), frequency, and transfer type (using Interrupt or DMA).
 * If the transfer type is DMA, it initializes DMA as well.
 * Registers I2C instance callback and clears pending interrupts.
 * Configures SDL and SCL pins as per the instance.
 *
 * @param[in] i2c_instance I2C Instance to be initialized \ref sl_i2c_instance_t. 
 *
 * @param[in] p_user_config A pointer to I2C configuration structure \ref sl_i2c_config_t
 *
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 *         - \ref SL_I2C_CALLBACK_BUSY (0x0D) - Driver is busy
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_init(sl_i2c_instance_t i2c_instance, const sl_i2c_config_t *p_user_config);

/***************************************************************************/
/**
 * @brief This API configures the follower address of the I2C module. 
 * @details Should be used only in Follower mode after instance initialization.
 * It updates the slave address register 
 * I2C leader device initiates transfer with the address that matches slave address register of follower device.
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here I2C mode should be set as follower mode
 *
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 *
 * @param[in] address Follower own address, can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023).
 *
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_set_follower_address(sl_i2c_instance_t i2c_instance, uint16_t address);

/***************************************************************************/
/**
 * @brief This API configures the TX and RX FIFO threshold values.
 * @details The valid range is 0-255, with the additional restriction that hardware does
 * not allow this value to be set to a value larger than the depth of the buffer.
 * A value of 0 sets the threshold for 1 entry,
 * and a value of 255 sets the threshold for 256 entries.
 * This should be called after I2C instance initialization.
 * @pre Pre-condition:
 *      - \ref sl_i2c_driver_init 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] tx_threshold_value Transmit FIFO threshold value: Range between 0 to 255.
 * @param[in] rx_threshold_value Receive FIFO threshold value: Range between 0 to 255.
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_configure_fifo_threshold(sl_i2c_instance_t i2c_instance,
                                                       uint8_t tx_threshold_value,
                                                       uint8_t rx_threshold_value);

/***************************************************************************/
/**
 * @brief This API will read the current I2C clock frequency. 
 * @details This API gets the current clock frequency of I2C in MHz,
 * by reading the system core clock frequency.
 * @pre Pre-condition:
 *      - \ref sl_i2c_driver_init 
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] frequency Pointer to a variable to store currently configured frequency.
 * @return status 0 if successful, else error code as follow.
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success. 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid. 
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_get_frequency(sl_i2c_instance_t i2c_instance, uint32_t *frequency);

/***************************************************************************/
/**
 * @brief This API sends the data in blocking mode.
 * @details This API sends the data in blocking mode using interrupt,
 * sets follower address when used in leader application,
 * sets transmit empty interrupt and enable I2C interrupts.
 * It keeps the call blocked until the last byte is transferred. 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address (if sending from slave)
 *      - \ref sl_i2c_driver_configure_fifo_threshold
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t.
 * @param[in] address Follower address, can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023). In follower mode this parameter will be ignored.
 * @param[in] tx_buffer A pointer to transmit data buffer.
 * @param[in] tx_len Data length in number of bytes in the range of 1- 80000 bytes.
 * @note Maximum tx_len used can be 80000 (receives in around 10 seconds).
 * @return status 0 if successful, else error code as follow.
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success. 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid.
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_send_data_blocking(sl_i2c_instance_t i2c_instance,
                                                 uint16_t address,
                                                 uint8_t *tx_buffer,
                                                 uint32_t tx_len);

/***************************************************************************/
/**
 * @brief This API sends the data in non-blocking mode.
 * @details This API sends the data in non-blocking mode using DMA 
 * also sets the follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 * Registers DMA callback & enables DMA channels.
 * It updates the transfer complete flag \ref SL_I2C_DATA_TRANSFER_COMPLETE after data transfer completion.
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as DMA-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in salve application
 * @param[in] i2c_instance I2C Instance.
 * @param[in] address Follower address can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023).
 * @param[in] tx_buffer A pointer to transmit data buffer
 * @param[in] tx_len Data length in number of bytes in the range of 1- 30000 bytes.
 * @param[in] dma_config A pointer to DMA configuration structure \ref sl_i2c_dma_config_t.
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
 * @brief This API receives the data in blocking mode.
 * @details This API receives the data in blocking mode using interrupt,
 * also sets the follower address when used in Leader application.
 * Sets receive full interrupt and enables I2C interrupts.
 * It keeps the call blocked until the last byte is transferred. 
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in follower application
 *      - \ref sl_i2c_driver_configure_fifo_threshold
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address, can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023). In follower mode this parameter will be ignored.
 * @param[in] rx_buffer A pointer to receive data buffer
 * @param[in] rx_len Data length in number of bytes in the range of 1-80000.
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
 * @brief This API receives the data in non-blocking mode.
 * @details This API receives the data in non-blocking mode (using DMA),
 * also sets the follower address when used in Leader application.
 * Configures DMA rx and tx channels.
 * Registers DMA callback & enables DMA channels.
 * It updates the transfer complete flag \ref SL_I2C_DATA_TRANSFER_COMPLETE after data transfer completion.
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as DMA-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in salve application
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address, can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023).
 * @param[in] rx_buffer A pointer to receive data buffer.
 * @param[in] rx_len Data length in number of bytes in the range of 1- 30000 Bytes.
 * @param[in] dma_config A pointer to DMA configuration structure \ref sl_i2c_dma_config_t
 * @note 
 *       -  Maximum rx_len values can be 30000 (receives back in around 4 seconds).
 *       - The default values cannot be any of the reserved address locations: that is, 0x00 to 0x07, 
 *        or 0x78 to 0x7f.
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
 * @brief This API transmits data and then receives data from an I2C device.
 * @details This API first transmits data and then receives data from an I2C device using interrupts.
 * Also sets follower address when used in Leader application.
 * Sets & enable transmit empty interrupt while transmitting data .
 * Sets & enable receive full interrupt while receiving data.
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init 
 *      - Here transfer-type should be set as Interrupt-type
 *      - \ref sl_i2c_driver_set_follower_address, if used in follower application
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @param[in] address Follower address, can be provided in 7-bit length (0-127)
 * or in 10-bit length(0-1023).
 * @param[in] p_transfer_config A pointer to transfer configuration structure \ref sl_i2c_transfer_config_t
 * @note Maximum tx_len & rx_len values can be 80000 (sends & receives back in around 20 seconds).
 * - The default values cannot be any of the reserved address locations: that is, 0x00 to 0x07, 
 *        or 0x78 to 0x7f.
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid  
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_transfer_data(sl_i2c_instance_t i2c_instance,
                                            sl_i2c_transfer_config_t const *p_transfer_config,
                                            uint16_t address);

/***************************************************************************/
/**
 * @brief De-initializes the I2C peripheral and clock
 * @details This API will de-initializes the I2C peripheral, disables its clock and unregisters the callback.
 * @pre Pre-condition:
 *      - \ref sl_i2c_driver_init
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 * @note
 *   When the I2C module is used in combination with other peripherals, while de-initializing in the application, refer to the notes below:
 *   1. Whenever sl_i2c_driver_deinit() gets called, it will power down the domain (PERI_EFUSE), which contains different peripherals mentioned below:
 *      USART, UART, I2C, SSI Master, SSI Slave, Generic-SPI Master, I2S Master, I2S Slave, Micro-DMA Controller, Config Timer,
 *      Random-Number Generator, CRC Accelerator, SIO, QEI, MCPWM and EFUSE.
 *      Since deinit power downs the PERI_EFUSE doamin, it's recommended to call the sl_i2c_driver_deinit() API at the end of the application.
 *   2. Few peripherals (ULP Peripherals, UULP Peripherals, GPDMA and SDIO-SPI) have seperate domains that can be powered down indepedently. For additional details, refer to the Power architecture section in the Hardware Reference Manual
 *      e.g., To power down ULP I2C, use the API below:
 *      RSI_PS_M4ssPeriPowerDown(ULPSS_PWRGATE_ULP_I2C); 
 *      Here, ULP_I2C has seperate power domain ULPSS_PWRGATE_ULP_I2C, which can be powered down indepedently. Refer to the rsi_power_save.h file for all power gates definitions.
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_deinit(sl_i2c_instance_t i2c_instance);

/***************************************************************************/
/**
 * @brief Set the Pin configuration for I2C.
 * @details It configures the SDA and SCL pins.
 * Also enables internal pullups of SDA & SCL lines of follower device.
 *
 * @param[in] pin_init Pointer to pin init structure \ref sl_i2c_pin_init_t
 *
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success 
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid 
 ******************************************************************************/
sl_i2c_status_t sl_si91x_i2c_pin_init(sl_i2c_pin_init_t *pin_init);

/***************************************************************************/
/**
 * @brief This API re-configures the I2C leader device on power mode changes.
 * @details Re-Config the I2C driver leader instance (I2C2) for ULP/HP mode.
 * It will re-config I2C clock frequency required for I2C operating(speed) mode to run in ULP/HP mode.
 * @pre Pre-conditions:
 *      - \ref sl_i2c_driver_init
 *      - Instance should be ULP mode when new mode is HP
 *      - Instance should be HP mode when new mode is ULP
 *      - Instance used should be ULP-I2C instance
 * @param[in] power_mode , new power state to switch to \ref sl_i2c_power_modes_t
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_leader_reconfig_on_power_mode_change(sl_i2c_power_modes_t new_power_mode);

/***************************************************************************/
/**
 * @brief To enable or disable repeated start, to enable or disable combined format transfer
 * @details Details :
 *          - This API should be called by a leader application to enable repeated start, before starting data transfer.
 *          - By enabling this, one can use combined format transfers.
 *          - With this, I2C can perform combined write/read operations to same or different slave without releasing the
 *            line, thus with the guarantee that the operation is not interrupted.
 * @pre Pre-condition:
 *      - \ref sl_i2c_driver_init
 *
 * @param[in] i2c_instance I2C Instance \ref sl_i2c_instance_t
 *
 * @param[in] enable_rep_start true to enable & false to disable repeated start
 *
 * @return status 0 if successful, else error code as follow
 *         - \ref SL_I2C_SUCCESS (0x0000) - Success
 *         - \ref SL_I2C_INVALID_PARAMETER (0x0F) - Parameters are invalid
 ******************************************************************************/
sl_i2c_status_t sl_i2c_driver_enable_repeated_start(sl_i2c_instance_t i2c_instance, boolean_t enable_rep_start);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup I2C
/// @{
///
///   @details
///
///   @n @section I2C_Intro Introduction
///
///  I2C, which stands for Inter-Integrated Circuit,
///  is a serial communication protocol commonly used in micro-controllers
///  and other integrated circuits for communication between various peripheral devices and sensors.
///  In micro-controllers, I2C is often used to connect peripherals like sensors, EEPROMs, real-time clocks,
///  and other integrated circuits. The I2C protocol uses only two wires for communication:
///  a serial data line (SDA) and a serial clock line (SCL). This makes it very efficient in terms of pin usage,
///  which is particularly important in micro-controller systems where pin count might be limited.
///
///  @li In embedded industries, I2C (Inter-Integrated Circuit) is widely used for various applications due to its simplicity,
///    flexibility, and low pin count requirements. Here are some common use cases of I2C in embedded systems:
///
///  1. Sensor Integration
///
///  2. Sensor Hubs and Fusion Algorithms
///
///  3. Memory Expansion
///
///  4. Real-Time Clock (RTC)
///
///  5. Display Interfaces
///
///  6. Peripheral Control
///
///  7. Communication with Other Micro-controllers and Modules
///
///  8. System Configuration and Control
///
///   @n @section Configuration
///
///   @li I2C can be configured by configuring parameters such as Leader or Follower mode using @ref sl_i2c_mode_t,
///   the Transfer type using @ref sl_i2c_transfer_type_t and operating speed Mode using @ref sl_i2c_operating_mode_t.
///   All of these configurations can be made using a structure called @ref sl_i2c_config_t, after which the API
///   @ref sl_i2c_driver_init() can be called.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section Usage
///
///     The common I2C functions can be used after the I2C Structures are specified, passing an instance of   @ref sl_i2c_config_t.
///     These functions will initiate and configure the I2C below, which is the flow for implementation.
///
///    1. @ref sl_i2c_driver_init
///    2. @ref sl_i2c_driver_set_follower_address (for follower)
///    3. @ref sl_i2c_driver_configure_fifo_threshold
///    4. @ref sl_i2c_driver_receive_data_blocking or
///    5. @ref sl_i2c_driver_send_data_blocking
///    6. @ref sl_i2c_driver_deinit
///
/// **Note** : The above flow is for I2C non-DMA operations. If the requirement is to use DMA Mode, then use send and receive non-blocking APIs
/// @ref sl_i2c_driver_receive_data_non_blocking and @ref sl_i2c_driver_send_data_non_blocking.
///
/** @} (end addtogroup I2C) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_I2C_H */
