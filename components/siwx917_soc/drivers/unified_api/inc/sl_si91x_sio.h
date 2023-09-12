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

///@brief SIO-SPI channels
typedef enum {
  SL_SIO_CH_0, ///< SIO-SPI channel 0
  SL_SIO_CH_1, ///< SIO-SPI channel 1
  SL_SIO_CH_2, ///< SIO-SPI channel 2
  SL_SIO_CH_3, ///< SIO-SPI channel 3
  SL_SIO_CH_4, ///< SIO-SPI channel 4
  SL_SIO_CH_5, ///< SIO-SPI channel 5
  SL_SIO_CH_6, ///< SIO-SPI channel 6
  SL_SIO_CH_7, ///< SIO-SPI channel 0
} sl_sio_channel_t;

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/
/// @brief Structure to hold the different versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< major version number
  uint8_t minor;   ///< minor version number
} sl_sio_version_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief Initialize the SIO module. It initializes the SIO
 *      GPIO's and enables the SIO module clock.
 * @param[in] none
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_FAIL (0x0001) - Fail, SIO initialization failed \n
 *       \ref SL_STATUS _OK (0X000)   - Success, SIO initialization successful \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_init(void);

/***************************************************************************/ /**
 * @brief Initialize the SIO-SPI module. It configures
 *    the SPI mode, bit length, bit order, SIO frequency and the SIO channels for
 *    the SPI transfer lines.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] configuration - Pointer to SIO-SPI configuration structure
 *                  \ref sl_sio_spi_config_t
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success, SPI initialization done properly \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_init(sl_sio_spi_config_t *configuration);

/***************************************************************************/ /**
 * @brief Assert the SIO SPI chip select.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_spi_init() \n
 * @param[in] chip_select_num - Chip select number(0 to 7)
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_assert(uint8_t chip_select_num);

/***************************************************************************/ /**
 * @brief De-assert the SIO SPI chip select.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_spi_init() \n
 *        \ref sl_si91x_sio_spi_cs_assert() \n
 *        \ref sl_si91x_sio_spi_transfer() \n
 * @param[in] chip_select_num - Chip select number(0 to 7)
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_deassert(uint8_t chip_select_num);

/***************************************************************************/ /**
 * @brief Register the user callback function.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_spi_init() \n
 * @param[in] callback_event - Pointer to the function \ref sl_sio_spi_callback_t
 *                 which needs to be called at the time of interrupt.
 * @return returns status 0 if successful,
 *               else error code.
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_register_event_callback(sl_sio_spi_callback_t callback_event);

/***************************************************************************/ /**
 * @brief Un-register the user callback function.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_spi_init() \n
 *        \ref sl_si91x_sio_spi_register_event_callback()
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_sio_spi_unregister_event_callback(void);

/***************************************************************************/ /**
 * @brief Transfer the SIO SPI data. It is used make the SIO-SPI
 *          transfer in non blocking mode.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_spi_init() \n
 *        \ref sl_si91x_sio_spi_cs_assert() \n
 * @param[in] xfer_config - Pointer to SIO-SPI transfer configuration structure
 *                    \ref sl_sio_spi_xfer_config_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_transfer(sl_sio_spi_xfer_config_t *xfer_config);

/***************************************************************************/ /**
 * @brief  Get the SIO version.
 * @param[in] none
 * @return returns structure of type \ref sl_sio_version_t
 ******************************************************************************/
sl_sio_version_t sl_si91x_sio_get_version(void);

/*******************************************************************************
 * @brief  Initialize SIO-UART, i.e., set baud rate, parity,
 *    channel selection, stop bits, and data length.
 * @pre    \ref sl_si91x_sio_init() \n
 * @param[in] configuration - Pointer to SIO-UART configuration structure
 *                  \ref sl_sio_uart_config_t
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success, UART initialization done properly \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_init(sl_sio_uart_config_t *configuration);

/*******************************************************************************
 * @brief  Send the data over SIO-UART.
 * @pre    \ref sl_si91x_sio_init() \n
 *         \ref sl_si91x_sio_uart_init() \n
 * @param[in]    buffer  - data pointer to send
 * @param[in]    length  - data length
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_send(const void *buffer, uint16_t length);

/*******************************************************************************
 * Send the data over SIO-UART in blocking mode.
 * @pre    \ref sl_si91x_sio_init() \n
 *         \ref sl_si91x_sio_uart_init() \n
 * @param[in]    buffer  - data pointer to send
 * @param[in]    length  - number of bytes to send
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_send_blocking(const void *buffer, uint16_t length);

/*******************************************************************************
 * Read data from UART.
 * @pre    \ref sl_si91x_sio_init() \n
 *         \ref sl_si91x_sio_uart_init() \n
 *         \ref sl_si91x_sio_uart_send() (or) \ref sl_si91x_sio_uart_send_blocking()
 * @param[in]    data_buffer  - data buffer pointer to read
 * @param[in]    num_bytes  - number of bytes read
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_read(void *data_buffer, uint16_t num_bytes);

/*******************************************************************************
 * Read data from UART in blocking mode.
 * @pre    \ref sl_si91x_sio_init() \n
 *         \ref sl_si91x_sio_uart_init() \n
 *         \ref sl_si91x_sio_uart_send() (or) \n
 *         \ref sl_si91x_sio_uart_send_blocking() \n
 * @param[in]    data_buffer  - data buffer pointer to read
 * @param[in]    num_bytes  - number of bytes read
 * @return returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_read_blocking(void *data_buffer, uint16_t num_bytes);

/***************************************************************************/ /**
 * @brief Register the user callback function.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 * @param[in] callback_event - Pointer to the function \ref sl_sio_uart_callback_t
 *                 which needs to be called at the time of interrupt.
 * @return returns status 0 if successful,
 *               else error code.
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_BUSY (0x0004) - Driver is busy \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_register_event_callback(sl_sio_uart_callback_t callback_event);

/***************************************************************************/ /**
 * @brief   Write data using SIO-I2C.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_i2c_generate_start() \n
 * @param[in]  configuration - pointer to the I2C configuration structure
 *                           \ref stc_sio_i2c_config_t in SIO module
 * @param[in]  address  -  slave address
 * @param[in]  data     -  pointer to the data
 * @param[in]  length   -  data length
 * @return  returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_sio_i2c_write(stc_sio_i2c_config_t *configuration,
                                   uint8_t address,
                                   uint8_t *data,
                                   uint16_t length);

/***************************************************************************/ /**
 * @brief   Read data using SIO-I2C.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_i2c_generate_start() \n
 *        \ref sl_si91x_sio_i2c_write() (or) \n
 *        \ref sl_si91x_sio_i2c_transfer() \n
 * @param[in]  configuration - pointer to the I2C configuration structure
 *                           \ref stc_sio_i2c_config_t in SIO module
 * @param[in]  address  -  slave address
 * @param[in]  data     -  pointer to the data
 * @param[in]  length   -  data length
 * @return  returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_sio_i2c_read(stc_sio_i2c_config_t *configuration, uint8_t address, uint8_t *data, uint16_t length);

/***************************************************************************/ /**
 * @brief   Transfer data using SIO-I2C.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_i2c_generate_start() \n
 * @param[in]  configuration - pointer to the I2C configuration structure
 *                           \ref stc_sio_i2c_config_t in SIO module
 * @param[in]  address   -  slave address
 * @param[in]  tx_buffer -  pointer to the data transmit buffer
 * @param[in]  tx_length -  TX data length
 * @param[in]  rx_buffer -  pointer to the data receive buffer
 * @param[in]  rx_length -  RX data length
 * @return  returns status 0 if successful,
 *                 else error code.
 *       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *       \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_sio_i2c_transfer(stc_sio_i2c_config_t *configuration,
                                      uint8_t address,
                                      uint8_t *tx_buffer,
                                      uint16_t tx_length,
                                      uint8_t *rx_buffer,
                                      uint16_t rx_length);

/***************************************************************************/ /**
 * @brief   Generate I2C start in SIO.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in]   none
 * @return      none
 *******************************************************************************/
void sl_si91x_sio_i2c_generate_start(void);

/***************************************************************************/ /**
 * @brief   Generate I2C stop in SIO.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_i2c_generate_start() \n
 *        \ref sl_si91x_sio_i2c_write() (or) \n
 *        \ref sl_si91x_sio_i2c_transfer() \n
 * @param[in]   none
 * @return      none
 *******************************************************************************/
void sl_si91x_sio_i2c_generate_stop(void);

/***************************************************************************/ /**
 * @brief Un-register the user callback function.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 *        \ref sl_si91x_sio_uart_register_event_callback()
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_sio_uart_unregister_event_callback(void);

/***************************************************************************/ /**
 * @brief  Used when UART receive is done.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 *        \ref sl_si91x_sio_uart_send() \n
 *        \ref sl_si91x_sio_uart_read()
 * @param[in] none
 * @return none
 ******************************************************************************/
void sl_si91x_sio_uart_rx_done(void);

/***************************************************************************/ /**
 * @brief Configure pin detection mode to be considered for GPIO interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] flag  : gpio interrupt generated \ref interrupt_flag_t
 *                      rise edge : 0 \n
 *                      fall edge : 1 \n
 *                      level 0   : 2 \n
 *                      level 1   : 3 \n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_configure_interrupt(en_sio_channels_t channel, interrupt_flag_t flag);

/***************************************************************************/ /**
 * @brief Match the pattern with data to be detected.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] pattern   : pattern match bit to be enabled for pattern match to take place \ref pattern_match_t
 *                        pattern match disable : 0 \n
 *                        pattern match enable  : 1 \n
 * @param[in] slice     : slice number(0,1,2,8,9,10) to select
 * @param[in] slice_pattern : pattern to match for selected slice
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_match_pattern(en_sio_channels_t channel,
                                       pattern_match_t pattern,
                                       uint8_t slice,
                                       uint32_t slice_pattern);

/***************************************************************************/ /**
 * @brief Generate the shift clock.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] divider   : Desired clock frequency configuration
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_shift_clock(uint32_t divider, en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Select clock.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 *        \ref sl_si91x_sio_uart_register_event_callback() \n
 *        \ref sl_si91x_sio_uart_send()
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] clock   : clock used for shift operations \ref clock_type_t
 *                      internal clock : 0 \n
 *                      external clock : 1 \n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_select_clock(en_sio_channels_t channel, clock_type_t clock);

/***************************************************************************/ /**
 * @brief Shift the number of bits.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] data_shift  : Number of shifts to happen before reloading register with data.
 *                      value to be set = (total no. of valid bits in shift register/ no. of bits per shift) -1
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_position_counter(en_sio_channels_t channel, uint32_t data_shift);

/***************************************************************************/ /**
 * @brief Enable/disable the flow control bit.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] clock   : It decides whether to continue data shifting  based on data
 *                        present in shift register validation \ref flow_control_t
 *                      flow control disable : 0 \n
 *                      flow control enable : 1 \n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_control_flow(en_sio_channels_t channel, flow_control_t flow_control);

/***************************************************************************/ /**
 * @brief Load data to buffer in reverse order.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] reverse   : If data to be shifted out MSB first, it is to be set \ref reverse_load_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_reverse_load(en_sio_channels_t channel, reverse_load_t reverse);

/***************************************************************************/ /**
 * @brief Enable the common swap interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_set_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Disable the common swap interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_clear_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Mask the common swap interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_mask_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Unmask the common swap interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_unmask_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Read the common swap interrupt status.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] none
 * @return returns interrupt status
 ******************************************************************************/
uint32_t sl_si91x_sio_get_interrupt_status(void);

/***************************************************************************/ /**
 * @brief Enable the common shift interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_set_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Disable the common shift interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_clear_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Mask the common shift interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_mask_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Unmask the common shift interrupt.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_sio_unmask_shift_interrupt(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Read the common shift interrupt status.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns shift interrupt status
 ******************************************************************************/
uint32_t sl_si91x_sio_shift_interrupt_status(void);

/***************************************************************************/ /**
 * @brief Select edge of the clock cycle for sampling bits.
 * @pre   \ref sl_si91x_sio_init() \n
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] edge_sel  : Select the edge for bit sample to start \ref edge_select_t
 *                      positive edge : 0 \n
 *                      negative edge : 1 \n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \
 ******************************************************************************/
sl_status_t sl_si91x_sio_edge_select(en_sio_channels_t channel, edge_select_t edge_sel);

/***************************************************************************/ /**
 * @brief Read SIO buffer register.
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 *        \ref sl_si91x_sio_uart_register_event_callback() \n
 *        \ref sl_si91x_sio_uart_send()
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @return returns data from buffer
 ******************************************************************************/
uint32_t sl_si91x_sio_read_buffer(en_sio_channels_t channel);

/***************************************************************************/ /**
 * @brief Write into SIO buffer register
 * @pre   \ref sl_si91x_sio_init() \n
 *        \ref sl_si91x_sio_uart_init() \n
 *        \ref sl_si91x_sio_uart_register_event_callback() \n
 *        \ref sl_si91x_sio_uart_send()
 * @param[in] channel   : SIO channel to be selected \ref en_sio_channels_t
 * @param[in] data      : data to be written to buffer
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022n) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \
 ******************************************************************************/
sl_status_t sl_si91x_sio_write_buffer(en_sio_channels_t channel, uint32_t data);

/** @} (end addtogroup SIO) */

#ifdef __cplusplus
}
#endif

#endif ///< SL_SI91X_SIO_H
/**************************************************************************************************/
