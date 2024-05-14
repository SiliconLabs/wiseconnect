/***************************************************************************/ /**
 * @file sl_si91x_sio.h
 * @brief SIO API implementation
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

#ifndef SL_SI91X_SIO_H
#define SL_SI91X_SIO_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "rsi_sio.h"
#include "sl_status.h"

/***************************************************************************/ /**
 * @addtogroup SIO Serial Input-Output
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 *******************************************************************************/

/*******************************************************************************
 ***************************  DEFINES / MACROS  ********************************
 ******************************************************************************/
#define SL_SIO_CH_0 0 ///< SIO-SPI channel 0
#define SL_SIO_CH_1 1 ///< SIO-SPI channel 1
#define SL_SIO_CH_2 2 ///< SIO-SPI channel 2
#define SL_SIO_CH_3 3 ///< SIO-SPI channel 3
#define SL_SIO_CH_4 4 ///< SIO-SPI channel 4
#define SL_SIO_CH_5 5 ///< SIO-SPI channel 5
#define SL_SIO_CH_6 6 ///< SIO-SPI channel 6
#define SL_SIO_CH_7 7 ///< SIO-SPI channel 7

typedef stc_sio_spi_cfg_t sl_sio_spi_config_t;       ///< SIO-SPI configuration structure
typedef stc_sio_spi_xfer_t sl_sio_spi_xfer_config_t; ///< SIO-SPI Transfer structure
typedef stc_sio_i2s_config_t sl_sio_i2s_config_t;    ///< SIO-I2S configuration structure
typedef stc_sio_i2s_xfer_t sl_sio_i2s_xfer_config_t; ///< SIO-I2S Transfer structure
typedef stc_sio_uart_config_t sl_sio_uart_config_t;  ///< SIO-UART configuration structure
typedef sio_i2s_func_ptr_t sl_sio_i2s_callback_t;    ///< SIO-I2S callback function pointer
typedef sio_Spi_func_ptr_t sl_sio_spi_callback_t;    ///< SIO-SPI callback function pointer
typedef sio_Uart_func_ptr_t sl_sio_uart_callback_t;  ///< SIO_UART callback function pointer
typedef stc_sio_i2c_config_t sl_sio_i2c_config_t;    ///< SIO-I2C configuration structure

/*******************************************************************************
 ********************************   Local Variables   **************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///@brief SIO-SPI Events
typedef enum {
  SL_SIO_SPI_TX_DONE, ///< SIO-SPI Transfer done event
  SL_SIO_SPI_RX_DONE, ///< SIO-SPI Receive done event
} sl_sio_spi_event_t;

///@brief SIO-UART Events
typedef enum {
  SL_SIO_UART_TX_DONE,      ///< SIO-UART transfer done event
  SL_SIO_UART_RX_DONE,      ///< SIO-UART receive done event
  SL_SIO_UART_PARITY_ERR,   ///< SIO-UART parity error
  SL_SIO_UART_FRAMING_ERR,  ///< SIO-UART framing error
  SL_SIO_UART_RECEIVE_CHAR, ///< SIO-UART receive character
} sl_sio_uart_event_t;

///@brief SIO-SPI modes configurations
typedef enum {
  SL_SIO_SPI_MODE_0 = 0, ///< SIO-SPI mode 0
  SL_SIO_SPI_MODE_3 = 3, ///< SIO-SPI mode 3
} sl_sio_spi_mode_t;

///@brief SIO-SPI bit width configurations
typedef enum {
  SL_SIO_SPI_BIT_8  = 8,  ///< SIO-SPI bit width 8
  SL_SIO_SPI_BIT_16 = 16, ///< SIO-SPI bit width 16
} sl_sio_spi_bit_width_t;

///@brief SIO-SPI MSB/LSB first configurations
typedef enum {
  SL_SIO_SPI_LSB_FIRST = 0, ///< SIO-SPI LSB first
  SL_SIO_SPI_MSB_FIRST = 1, ///< SIO-SPI MSB first
} sl_sio_spi_msb_lsb_t;

///@brief SIO-UART bit length configurations
typedef enum {
  SL_SIO_UART_BIT_8  = 8,  ///< SIO-SPI bit length 8
  SL_SIO_UART_BIT_16 = 16, ///< SIO-SPI bit length 16
} sl_sio_spi_bit_length_t;

///@brief SIO-UART parity configurations
typedef enum {
  SL_SIO_UART_EVEN_PARITY = 0, ///< SIO-SPI even parity
  SL_SIO_UART_ODD_PARITY  = 1, ///< SIO-SPI odd parity
} sl_sio_spi_parity_t;

///@brief SIO-UART stop bit configurations
typedef enum {
  SL_SIO_UART_STOP_BIT_1 = 1, ///< SIO-UART stop bit 1
  SL_SIO_UART_STOP_BIT_2 = 2, ///< SIO-UART stop bit 2
} sl_sio_spi_stop_bit_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/// @brief Structure to hold the different versions of the peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< Major version number
  uint8_t minor;   ///< Minor version number
} sl_sio_version_t;

///@brief Structure to hold the port and pin of SIO SPI
typedef struct {
  uint8_t spi_cs_port;   ///< SIO SPI CS port
  uint8_t spi_cs_pin;    ///< SIO SPI CS pin
  uint8_t spi_cs_mux;    ///< SIO SPI CS mux
  uint8_t spi_cs_pad;    ///< SIO SPI CS pad
  uint8_t spi_clk_port;  ///< SIO SPI CLK port
  uint8_t spi_clk_pin;   ///< SIO SPI CLK pin
  uint8_t spi_clk_mux;   ///< SIO SPI CLK mux
  uint8_t spi_clk_pad;   ///< SIO SPI CLK pad
  uint8_t spi_mosi_port; ///< SIO SPI MOSI port
  uint8_t spi_mosi_pin;  ///< SIO SPI MOSI pin
  uint8_t spi_mosi_mux;  ///< SIO SPI MOSI mux
  uint8_t spi_mosi_pad;  ///< SIO SPI MOSI pad
  uint8_t spi_miso_port; ///< SIO SPI MISO port
  uint8_t spi_miso_pin;  ///< SIO SPI MISO pin
  uint8_t spi_miso_mux;  ///< SIO SPI MISO mux
  uint8_t spi_miso_pad;  ///< SIO SPI MISO pad
} sl_sio_spi_t;

///@brief Structure to hold the port and pin of SIO UART
typedef struct {
  uint8_t uart_tx_port; ///< SIO UART TX port
  uint8_t uart_tx_pin;  ///< SIO UART TX pin
  uint8_t uart_tx_mux;  ///< SIO UART TX mux
  uint8_t uart_tx_pad;  ///< SIO UART TX pad
  uint8_t uart_rx_port; ///< SIO UART RX port
  uint8_t uart_rx_pin;  ///< SIO UART RX pin
  uint8_t uart_rx_mux;  ///< SIO UART RX mux
  uint8_t uart_rx_pad;  ///< SIO UART RX pad
} sl_sio_uart_t;

///@brief Structure to hold the port and pin of SIO I2C
typedef struct {
  uint8_t i2c_sda_port; ///< SIO I2C SDA port
  uint8_t i2c_sda_pin;  ///< SIO I2C SDA pin
  uint8_t i2c_sda_mux;  ///< SIO I2C SDA mux
  uint8_t i2c_sda_pad;  ///< SIO I2C SDA pad
  uint8_t i2c_scl_port; ///< SIO I2C SCL port
  uint8_t i2c_scl_pin;  ///< SIO I2C SCL pin
  uint8_t i2c_scl_mux;  ///< SIO I2C SCL mux
  uint8_t i2c_scl_pad;  ///< SIO I2C SCL pad
} sl_sio_i2c_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief Initialize the SIO module. It initializes the SIO
 *        GPIO's and enables the SIO module clock.
 *
 * @param none
 *
 * @return    returns status 0 if successful, else error code as follows:
 *           - \ref SL_STATUS_FAIL (0x0001) - Fail, SIO initialization failed
 *           - \ref SL_STATUS_OK (0x0000) - Success, SIO initialization successful
******************************************************************************/
sl_status_t sl_si91x_sio_init(void);

/***************************************************************************/ /**
 * @brief Initialize the SIO-SPI module. It configures
 *        the SPI mode, bit length, bit order, SIO frequency, and the SIO channels for
 *        the SPI transfer lines.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] configuration - Pointer to SIO-SPI configuration structure
 *                            \ref sl_sio_spi_config_t
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *           - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *           - \ref SL_STATUS_OK (0x0000) - Success, SPI initialization done properly
******************************************************************************/
sl_status_t sl_si91x_sio_spi_init(sl_sio_spi_config_t *configuration);

/***************************************************************************/ /**
 * @brief This API is used to De-initialize SIO module
 * @param none
 * @return none
 ******************************************************************************/
void sl_si91x_sio_deinit(void);

/***************************************************************************/ /**
 * @brief Initialize SIO SPI pins and clock
 * @param[in] sio_spi_init : Pointer to the structure of type \ref sl_sio_spi_t
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_OK - Success
 *           - \ref SL_STATUS_NULL_POINTER - The parameter is a null pointer
******************************************************************************/
sl_status_t sl_si91x_sio_spi_pin_initialization(sl_sio_spi_t *sio_spi_init);

/***************************************************************************/ /**
 * @brief Initialize SIO UART pins and clock. This holds the UART Tx, Rx pins
 * 		configuration.
 *
 * @param[in] sio_uart_init : Pointer to the structure of type \ref sl_sio_uart_t
 *
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_OK - Success
 *           - \ref SL_STATUS_NULL_POINTER - The parameter is a null pointer
******************************************************************************/
sl_status_t sl_si91x_sio_uart_pin_initialization(sl_sio_uart_t *sio_uart_init);

/***************************************************************************/ /**
 * @brief Initialize SIO I2C pins and clock. This holds the I2C SDA, SCL pins
 * 		configuration.
 *
 * @param[in] sio_i2c_init : Pointer to the structure of type \ref sl_sio_i2c_t
 *
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_OK - Success
 *           - \ref SL_STATUS_NULL_POINTER - The parameter is a null pointer
******************************************************************************/
sl_status_t sl_si91x_sio_i2c_pin_initialization(sl_sio_i2c_t *sio_i2c_init);

/***************************************************************************/ /**
 * @brief Assert the SIO SPI chip select. 
 * @details This tells peripheral that it should wake up 
 *     and receive / send data and is also used when multiple peripherals are present to 
 *	   select the one you'd like to communicate with.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_spi_init()
 * @param[in] chip_select_num - Chip select number (0 to 7)
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *           - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *           - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_assert(uint8_t chip_select_num);

/***************************************************************************/ /**
 * @brief De-assert the SIO SPI chip select. This is used to disable after the last byte
 *     is transmitted / received.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_spi_init()
 *      - \ref sl_si91x_sio_spi_cs_assert()
 *      - \ref sl_si91x_sio_spi_transfer()
 * @param[in] chip_select_num - Chip select number (0 to 7)
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *           - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *           - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_deassert(uint8_t chip_select_num);

/***************************************************************************/ /**
 * @brief Register the user callback function. It registers the callback, i.e., 
 *    stores the callback function address and pass to the variable that is called 
 *    in Interrupt Handler. If another callback is registered without unregistering 
 *    previous callback then, it returns an error code, so it is mandatory to unregister 
 *    the callback before registering another callback. It will returns error if any 
 *    callback is already registered.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_spi_init()
 * @param[in] callback_event - Pointer to the function \ref sl_sio_spi_callback_t
 *                             which needs to be called at the time of interrupt.
 * @return    returns status 0 if successful,
 *           else error code as follows:
 *           - \ref SL_STATUS_OK (0x0000) - Success
 *           - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *           - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
******************************************************************************/
sl_status_t sl_si91x_sio_spi_register_event_callback(sl_sio_spi_callback_t callback_event);

/***************************************************************************/ /**
 * @brief Un-register the user callback function, i.e., clear the callback 
 *     function address and disables IRQ handler.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_spi_init()
 *      - \ref sl_si91x_sio_spi_register_event_callback()
 * @param none
 * @return none
******************************************************************************/
void sl_si91x_sio_spi_unregister_event_callback(void);

/***************************************************************************/ /**
 * @brief Transfer the SIO SPI data. It is used to make the SIO-SPI
 *        transfer in non-blocking mode.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_spi_init()
 *      - \ref sl_si91x_sio_spi_cs_assert()
 *
 * @param[in] xfer_config - Pointer to SIO-SPI transfer configuration structure
 *                         \ref sl_sio_spi_xfer_config_t
 *
 * @return returns status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_spi_transfer(sl_sio_spi_xfer_config_t *xfer_config);

/***************************************************************************/ /**
 * @brief Get the SIO version. It is used to get the release, SQA and DEV version 
 *			of the SIO module.
 * @param none
 * @return returns structure of type \ref sl_sio_version_t
******************************************************************************/
sl_sio_version_t sl_si91x_sio_get_version(void);

/***************************************************************************/ /**
 * @brief Initialize SIO-UART, i.e., set baud rate, parity,
 *        channel selection, stop bits, and data length.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] configuration - Pointer to SIO-UART configuration structure
 *                           \ref sl_sio_uart_config_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success, UART initialization done properly
******************************************************************************/
sl_status_t sl_si91x_sio_uart_init(sl_sio_uart_config_t *configuration);

/***************************************************************************/ /**
 * @brief Send the data over SIO-UART in non-blocking mode. Transmit bytes from the 
 *    buffer using an interrupt service. Will return immediately, but cannot be called 
 *    again until the previous call has finished.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 * @param[in] buffer - data pointer to send
 * @param[in] length - data length
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_uart_send(const void *buffer, uint16_t length);

/***************************************************************************/ /**
 * @brief Send the data over SIO-UART in blocking mode. Transmit bytes from the 
 *     buffer using a blocking send byte function. Does not return until all bytes are sent.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_send() (or) \ref sl_si91x_sio_uart_send_blocking()
 * @param[in] buffer - data pointer to send
 * @param[in] length - number of bytes to send
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_uart_send_blocking(const void *buffer, uint16_t length);

/***************************************************************************/ /**
 * @brief Read data from SIO-UART in non-blocking mode.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_send() (or) \ref sl_si91x_sio_uart_send_blocking()
 * @param[out] data_buffer - data buffer pointer to read
 * @param[in] num_bytes - number of bytes read
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_uart_read(void *data_buffer, uint16_t num_bytes);

/***************************************************************************/ /**
 * @brief Read data from UART in blocking mode.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_send() (or)
 *      - \ref sl_si91x_sio_uart_send_blocking()
 * @param[out] data_buffer - data buffer pointer to read
 * @param[in] num_bytes - number of bytes read
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_uart_read_blocking(void *data_buffer, uint16_t num_bytes);

/***************************************************************************/ /**
 * @brief Register the user callback function.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 * @param[in] callback_event - Pointer to the function \ref sl_sio_uart_callback_t
 *                             which needs to be called at the time of interrupt.
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_OK (0x0000) - Success
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_BUSY (0x0004) - Driver is busy
******************************************************************************/
sl_status_t sl_si91x_sio_uart_register_event_callback(sl_sio_uart_callback_t callback_event);

/***************************************************************************/ /**
 * @brief Write data using SIO-I2C.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_i2c_generate_start()
 * @param[in] configuration - pointer to the I2C configuration structure
 *                           \ref stc_sio_i2c_config_t in SIO module
 * @param[in] address - slave address(1- 255).
 * @param[in] data - pointer to the data
 * @param[in] length - data length
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_i2c_write(stc_sio_i2c_config_t *configuration,
                                   uint8_t address,
                                   uint8_t *data,
                                   uint16_t length);

/***************************************************************************/ /**
 * @brief Read data using SIO-I2C.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_i2c_generate_start()
 *      - \ref sl_si91x_sio_i2c_write() (or)
 *      - \ref sl_si91x_sio_i2c_transfer()
 * @param[in] configuration - pointer to the I2C configuration structure
 *                           \ref stc_sio_i2c_config_t in SIO module
 * @param[in] address - slave address(1- 255).
 * @param[out] data - pointer to the data
 * @param[in] length - data length
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_i2c_read(stc_sio_i2c_config_t *configuration, uint8_t address, uint8_t *data, uint16_t length);

/***************************************************************************/ /**
 * @brief Transfer data using SIO-I2C.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_i2c_generate_start()
 * @param[in] configuration - Pointer to the I2C configuration structure
 *                            \ref stc_sio_i2c_config_t in SIO module
 * @param[in] address - Slave address (1- 255).
 * @param[in] tx_buffer - Pointer to the data transmit buffer
 * @param[in] tx_length - TX data length
 * @param[out] rx_buffer - Pointer to the data receive buffer
 * @param[in] rx_length - RX data length
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_i2c_transfer(stc_sio_i2c_config_t *configuration,
                                      uint8_t address,
                                      uint8_t *tx_buffer,
                                      uint16_t tx_length,
                                      uint8_t *rx_buffer,
                                      uint16_t rx_length);

/***************************************************************************/ /**
 * @brief Generate I2C start in SIO.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param none
 * @return none
******************************************************************************/
void sl_si91x_sio_i2c_generate_start(void);

/***************************************************************************/ /**
 * @brief Generate I2C stop in SIO.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_i2c_generate_start()
 *      - \ref sl_si91x_sio_i2c_write() (or)
 *      - \ref sl_si91x_sio_i2c_transfer()
 * @param none
 * @return none
******************************************************************************/
void sl_si91x_sio_i2c_generate_stop(void);

/***************************************************************************/
/**
 * @brief Un-register the user callback function.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_register_event_callback()
 * @param none
 * @return none
******************************************************************************/
void sl_si91x_sio_uart_unregister_event_callback(void);

/***************************************************************************/ /**
 * @brief Used when UART receive is done.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_send()
 *      - \ref sl_si91x_sio_uart_read()
 * @param none
 * @return none
******************************************************************************/
void sl_si91x_sio_uart_rx_done(void);

/***************************************************************************/ /**
 * @brief Configure pin detection mode to be considered for GPIO interrupt.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] flag - GPIO interrupt generated \ref interrupt_flag_t
 *                  - Rise edge: 0
 *                  - Fall edge: 1
 *                  - Level 0: 2
 *                  - Level 1: 3
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_configure_interrupt(en_sio_channels_t channel, interrupt_flag_t flag);

/***************************************************************************/ /**
 * @brief Match the pattern with data to be detected.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] pattern - Pattern match bit to be enabled for pattern match to take place \ref pattern_match_t
 *                    - Pattern match disable: 0
 *                    - Pattern match enable: 1
 * @param[in] slice - Slice number (0,1,2,8,9,10) to select.
 * @param[in] slice_pattern - Pattern to match for selected slice
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_match_pattern(en_sio_channels_t channel,
                                       pattern_match_t pattern,
                                       uint8_t slice,
                                       uint32_t slice_pattern);

/***************************************************************************/ /**
 * @brief Generate the shift clock.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] divider - Desired clock frequency configuration
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_shift_clock(uint32_t divider, en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Select SIO peripheral clock.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_register_event_callback()
 *      - \ref sl_si91x_sio_uart_send()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] clock - Clock used for shift operations \ref clock_type_t
 *                  - Internal clock: 0
 *                  - External clock: 1
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_select_clock(en_sio_channels_t channel, clock_type_t clock);

/***************************************************************************/ /**
 * @brief Shift the number of bits. Number of shifts to happen before reloading 
 *       the shift register with data/pausing the operation. i.e. value to be 
 *      set = (total no. of valid bits in shift register/ number of bits per shift) – 1 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] data_shift - Number of shifts to happen before reloading register with data.
 *                         Value to be set = (total no. of valid bits in shift register/ no. of bits per shift) - 1
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_position_counter(en_sio_channels_t channel, uint32_t data_shift);

/***************************************************************************/ /**
 * @brief Enable/disable the flow control bit.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] flow_control - It decides whether to continue data shifting based on data
 *                           present in shift register validation \ref flow_control_t
 *                         - Flow control disable: 0
 *                         - Flow control enable: 1
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_control_flow(en_sio_channels_t channel, flow_control_t flow_control);

/***************************************************************************/ /**
 * @brief Load data to buffer in reverse order.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] reverse - If data to be shifted out MSB first, it is to be set \ref reverse_load_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_reverse_load(en_sio_channels_t channel, reverse_load_t reverse);

/***************************************************************************/ /**
 * @brief Enable the common swap interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_set_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Disable the common swap interrupt.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_i2c_generate_start()
 *      - \ref sl_si91x_sio_i2c_write() (or)
 *      - \ref sl_si91x_sio_i2c_transfer()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_clear_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Mask the common swap interrupt.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_mask_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Unmask the common swap interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_unmask_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Read the common swap interrupt status.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param none
 * @return returns interrupt status
******************************************************************************/
uint32_t sl_si91x_sio_get_interrupt_status(void);

/***************************************************************************/ /**
 * @brief Enable the common shift interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_set_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Disable the common shift interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_clear_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Mask the common shift interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_mask_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Unmask the common shift interrupt.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_unmask_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Read the common shift interrupt status.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns shift interrupt status
******************************************************************************/
uint32_t sl_si91x_sio_shift_interrupt_status(void);

/***************************************************************************/ /**
 * @brief Select edge of the clock cycle for sampling bits.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] edge_sel - Select the edge for bit sample to start \ref edge_select_t
 *                     - Positive edge: 0
 *                     - Negative edge: 1
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_edge_select(en_sio_channels_t channel, edge_select_t edge_sel);

/***************************************************************************/ /**
 * @brief Read SIO buffer register.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_register_event_callback()
 *      - \ref sl_si91x_sio_uart_send()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @return returns data from buffer
******************************************************************************/
uint32_t sl_si91x_sio_read_buffer(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Write into SIO buffer register.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sio_init()
 *      - \ref sl_si91x_sio_uart_init()
 *      - \ref sl_si91x_sio_uart_register_event_callback()
 *      - \ref sl_si91x_sio_uart_send()
 * @param[in] channel - SIO channel to be selected \ref en_sio_channels_t
 * @param[in] data - Data to be written to buffer
 * @return returns status 0 if successful,
 *         else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument
 *         - \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer
 *         - \ref SL_STATUS_OK (0X000) - Success
******************************************************************************/
sl_status_t sl_si91x_sio_write_buffer(en_sio_channels_t channel, uint32_t data);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/// @addtogroup SIO Serial Input-Output
/// @{
///
///   @details
///
///
///   @n @section SIO_Intro Introduction
///
///
///   Serial Input/Output (SIO) refers to a method of data transfer where information is sent and received
///   sequentially, one bit at a time, over a single communication channel. This communication can occur between
///   different electronic devices, such as micro-controllers, sensors, or other components.
///
///   Key features of SIO include:
///
///   @li  SIO Functionality is supported by Eight GPIO pins.
///
///   @li  Detects a 32-bit pattern on the input pins.
///
///   @li  SIO will transmit or receive the MSB First.
///
///   @n @section SIO_Config Configuration
///
///   Various parameters SPI bit order, SPI clock, MOSI channel, MISO channel,
///   SPI CS channel, bit length, SPI mode can be configured.
///   Various parameters UART baud rate, bit length, parity, receive channel selection, transmit
///   channel selection, stop bits can be configured with below given Enumerations.
///
///   1. @ref  sl_sio_spi_event_t
///
///   2. @ref  sl_sio_uart_event_t
///
///   3. @ref  sl_sio_spi_mode_t
///
///   4. @ref  sl_sio_spi_bit_width_t
///
///   5. @ref  sl_sio_spi_msb_lsb_t
///
///   6. @ref  sl_sio_spi_bit_length_t
///
///   7. @ref  sl_sio_spi_parity_t
///
///   8. @ref  sl_sio_spi_stop_bit_t
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section SIO_Use Usage
///
///   Data is transferred over SIO SPI after it has been initialized and configured. A callback
///   API is then called to register the callback at the moment of the event.  Before transfer has
///   begin, chip select assert is to be done.  In order to do this test, the MISO and MOSI pins must
///   be connected in loopback mode.
///
///   After transfer completion, chip select de-assert is to be done using When the transfer complete event
///   is generated, it compares the sent and received data.
///
///   1. @ref sl_si91x_sio_spi_pin_initialization
///
///   2. @ref sl_si91x_sio_spi_init
///
///   3. @ref sl_si91x_sio_spi_register_event_callback
///
///   4. @ref sl_si91x_sio_spi_cs_assert
///
///   5. @ref sl_si91x_sio_spi_transfer
///
///   6. @ref sl_si91x_sio_spi_cs_deassert
///
///   7. @ref sl_si91x_sio_deinit
///
///  Data is sent and received by SIO UART after it has been initiated and configured. A callback API is
///  then called to register the callback at the moment of the event. Subsequently, the transfer buffer will
///  transmit the required number of bytes, and the receive buffer will obtain the data bytes. It compares the
///  sent and received data when the receive complete event is created.
///
///  1. @ref sl_si91x_sio_uart_pin_initialization
///
///  2. @ref sl_si91x_sio_uart_init
///
///  2. @ref sl_si91x_sio_uart_register_event_callback
///
///  3. @ref sl_si91x_sio_uart_send
///
///  4. @ref sl_si91x_sio_uart_read
///
///  5. @ref sl_si91x_sio_deinit
///
/** @} (end addtogroup PWM) */

#ifdef __cplusplus
}
#endif

#endif ///< SL_SI91X_SIO_H
/**************************************************************************************************/
