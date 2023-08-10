/***************************************************************************/ /**
* @file sl_si91x_sio.c
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
#include "sl_si91x_sio.h"
#include "sl_si91x_sio_config.h"
/*******************************************************************************
 ***************************  LOCAL MACROS   ***********************************
 ******************************************************************************/
#define CS_NUM_MIN          0        ///< Minimum chip select number
#define CS_NUM_MAX          7        ///< Maximum chip select number
#define SIO_RELEASE_VERSION 0        ///< SIO Release version
#define SIO_SQA_VERSION     0        ///< SIO SQA version
#define SIO_DEV_VERSION     2        ///< SIO Developer version
#define SLI_SIO_INTERRUPT   SIO_IRQn ///< SIO interrupt handler
#define SIO_CHANNEL         7        ///< SIO maximum channel
#define FLAG                3        ///< Maximum interrupt flag
#define TWO                 2
#define SIO_UC \
  1 /*!< SIO_UC is defined by default. when this macro (SIO_UC) is defined,    \
       peripheral configuration is directly taken from the configuration set   \
       in the universal configuration (UC). if the application requires the    \
       configuration to be changed in run-time, undefined this macro and       \
       change the peripheral configuration through the \ref                    \
       sl_si91x_sio_spi_init API. */

/*******************************************************************************
 ***************************  Global  VARIABLES ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
sl_sio_i2s_callback_t i2s_user_callback;
sl_sio_spi_callback_t spi_user_callback;
sl_sio_uart_callback_t uart_user_callback;
stc_sio_uart_config_t UartInitstc = { 0 };

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(error_t error);
static void callback_event_handler(en_sio_spi_events_t en_event);

/*******************************************************************************
 **************************   Global function Definitions   ********************
 ******************************************************************************/

/*******************************************************************************
 * This API initializes SIO gpio's and enables SIO module clock
 ******************************************************************************/
sl_status_t sl_si91x_sio_init(void)
{
  sl_status_t status;
  error_t error_status;
  // Initialize the SIO
  error_status = RSI_SIO_Initialization();
  if (error_status == RSI_OK) {
    status = SL_STATUS_OK; // Returns status OK if no error occurs
  } else {
    status = SL_STATUS_FAIL; // Returns status error code
  }
  return status;
}

/*******************************************************************************
 * This API initializes SIO-SPI i.e SPI mode, bit length, bit order, SIO
 * frequency and the SIO channels for the SPI transfer lines. The actions to be
 * performed before SPI initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_init(sl_sio_spi_config_t *configuration)
{
#ifdef SIO_UC
  configuration = &pstcSpiConfigUc;
#endif
  sl_status_t status;
  error_t error_status;
  do {
    if (configuration == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Initialize the SIO-SPI
    error_status = RSI_SIO_InitSpi(SIO, configuration);
    status       = convert_rsi_to_sl_error_code(error_status); // Returns status code
  } while (false);
  return status;
}

/*******************************************************************************
 * This API asserts the SIO SPI chip select
 * The actions to be performed before SPI chip select assert are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-SPI using @ref sl_si91x_sio_spi_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_assert(uint8_t chip_select_num)
{
  sl_status_t status;
  do {
    if (chip_select_num > CS_NUM_MAX) {
      // Returns invalid parameter status code if chip select > 7
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Assert the SPI chip select
    RSI_SIO_SpiCsAssert(SIO, chip_select_num);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API de-asserts the SIO-SPI chip select
 * The actions to be performed before SPI chip select de-assert are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-SPI using @ref sl_si91x_sio_spi_init() API
 *    - Assert SIO chip select using @ref sl_si91x_sio_spi_cs_assert() API
 *    - Transfer data using @ref sl_si91x_sio_spi_transfer() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_cs_deassert(uint8_t chip_select_num)
{
  sl_status_t status;
  do {
    if (chip_select_num > CS_NUM_MAX) {
      // Returns invalid parameter status code if chip select > 7
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // Deassert the SPI chip select
    RSI_SIO_SpiCsDeAssert(SIO, chip_select_num);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to register the event callback.
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback
 * then, it returns an error code, so it is mandatory to unregister the callback
 * before registering another callback. It will returns error if any callback is
 * already registered. The actions to be performed before callback are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-SPI using @ref sl_si91x_sio_spi_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_register_event_callback(sl_sio_spi_callback_t callback_event)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (callback_event == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if (spi_user_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // Enable the SIO interrupt
    NVIC_EnableIRQ(SLI_SIO_INTERRUPT);
    // User callback address is passed to the static variable which is called at
    // the time of interrupt
    spi_user_callback = callback_event;
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
void sl_si91x_sio_spi_unregister_event_callback(void)
{
  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  spi_user_callback = NULL;
  NVIC_DisableIRQ(SLI_SIO_INTERRUPT);
}

/*******************************************************************************
 * This API updates the callback function which in turns call the user callback
 *    API and used to make the SIO-SPI Transfer in non blocking mode.
 * The actions to be performed before SPI transfer are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-SPI using @ref sl_si91x_sio_spi_init() API
 *    - Assert SIO chip select using @ref sl_si91x_sio_spi_cs_assert() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_spi_transfer(sl_sio_spi_xfer_config_t *xfer_config)
{
#ifdef SIO_UC
  xfer_config->u8BitLen = pstcSpiConfigUc.u8BitLen;
#endif
  sl_status_t status;
  error_t error_status;
  do {
    if (xfer_config == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Update the callback function
    xfer_config->pfnCb = callback_event_handler;
    // Initiate the SIO data transfer
    error_status = RSI_SIO_SpiTrasnfer(SIO, xfer_config);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is a Static callback function for handling the callbacks
 ******************************************************************************/
static void callback_event_handler(en_sio_spi_events_t en_event)
{
  switch (en_event) {
    // Handle SPI TX done event here
    case SioSpiTxDone:
      break;
    // Handle SPI RX done event here
    case SioSpiRxDone:
      break;
  }
  spi_user_callback(en_event);
  return;
}

/*******************************************************************************
 * This API is to validate the RSI error code.
 * While calling the RSI APIs, it returns the RSI Error codes.
 * This function converts the RSI error codes into SL error codes.
 * It takes argument as RSI error type and returns the SL error type.
 * It has a single switch statement which is mapped with the SL error code and
 * after successful conversion it breaks the switch statement.
 * If the error code is not listed, by default is SL_STATUS_FAIL.
 ******************************************************************************/
static sl_status_t convert_rsi_to_sl_error_code(error_t error)
{
  sl_status_t status;
  switch (error) {
    case RSI_OK:
      status = SL_STATUS_OK;
      break;
    case INVALID_PARAMETERS:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_INVALID_INPUT_FREQUENCY:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    case ERROR_CLOCK_NOT_ENABLED:
      status = SL_STATUS_NOT_INITIALIZED;
      break;
    default:
      status = SL_STATUS_FAIL;
      break;
  }
  return status;
}

/*******************************************************************************
 * This API is used to get the release, SQA and DEV version of SIO
 * It returns the structure for SIO version.
 * Structure includes three members:
 * - Release version
 * - SQA version
 * - DEV version
 ******************************************************************************/
sl_sio_version_t sl_si91x_sio_get_version(void)
{
  sl_sio_version_t version;
  version.minor   = SIO_DEV_VERSION;
  version.release = SIO_RELEASE_VERSION;
  version.major   = SIO_SQA_VERSION;
  return version;
}

/*******************************************************************************
 * This API is used for SIO-UART initialization i.e., setting baud rate, parity,
 *    channel selection, stop bits, data length.
 * The actions to be performed before UART initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_init(sl_sio_uart_config_t *configuration)
{
#ifdef SIO_UC
  configuration = &UartInitstcUc;
  UartInitstc   = UartInitstcUc;
#endif
  sl_status_t status;
  error_t error_status;
  configuration->pfn = uart_user_callback;
  do {
    if (configuration == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Initialize the SIO-UART
    error_status = RSI_SIO_UartInit(SIO, configuration);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to send the data over SIO-UART.
 * The actions to be performed before UART initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-UART using @ref sl_si91x_sio_uart_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_send(const void *buffer, uint16_t length)
{
  sl_status_t status;
  error_t error_status;
  do {
    if (buffer == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Send the data over UART
    error_status = RSI_SIO_UARTSend(SIO, buffer, length);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to write data in SIO-UART mode.
 * The actions to be performed before UART initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-UART using @ref sl_si91x_sio_uart_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_send_blocking(const void *buffer, uint16_t length)
{
  sl_status_t status;
  error_t error_status;
  do {
    if (buffer == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // write data in UART mode
    error_status = RSI_SIO_UARTSendBlocking(SIO, buffer, length);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read data in UART mode.
 * The actions to be performed before UART initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-UART using @ref sl_si91x_sio_uart_init() API
 *    - Send data using @ref sl_si91x_sio_uart_send() API(or)
 *                      @ref sl_si91x_sio_uart_send_blocking() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_read(void *data_buffer, uint16_t num_bytes)
{
  sl_status_t status;
  error_t error_status;
  do {
    if (data_buffer == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Read data in UART mode

    error_status = RSI_SIO_UARTRead(SIO, data_buffer, num_bytes);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read data in UART blocking mode
 * The actions to be performed before UART initialization is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-UART using @ref sl_si91x_sio_uart_init() API
 *    - Send data using @ref sl_si91x_sio_uart_send() API(or)
 *                      @ref sl_si91x_sio_uart_send_blocking() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_read_blocking(void *data_buffer, uint16_t num_bytes)
{
  sl_status_t status;
  error_t error_status;
  do {
    if (data_buffer == NULL) {
      status = SL_STATUS_NULL_POINTER; // Returns null pointer status code
      break;
    }
    // Read data in UART blocking mode
    error_status = RSI_SIO_UARTReadBlocking(SIO, data_buffer, num_bytes);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to register the event callback.
 * It registers the callback, i.e., stores the callback function address
 * and pass to the variable that is called in Interrupt Handler.
 * If another callback is registered without unregistering previous callback
 * then, it returns an error code, so it is mandatory to unregister the callback
 * before registering another callback. It will returns error if any callback is
 * already registered. The actions to be performed before callback are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SIO-UART using @ref sl_si91x_sio_uart_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_uart_register_event_callback(sl_sio_uart_callback_t callback_event)
{
  sl_status_t status;
  do {
    // Validates the null pointer, if true returns error code
    if (callback_event == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer if the parameters is not NULL then, it
    // returns an error code
    if (uart_user_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // Enable the SIO interrupt
    NVIC_EnableIRQ(SLI_SIO_INTERRUPT);
    // User callback address is passed to the static variable which is called at
    // the time of interrupt
    uart_user_callback = callback_event;
    // Returns SL_STATUS_OK if callback is successfully registered
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 ******************************************************************************/
void sl_si91x_sio_uart_unregister_event_callback(void)
{
  // Pass the NULL value to the static variable which is called at the time of
  // interrupt.
  // It is further validated in register callback API.
  uart_user_callback = NULL;
  NVIC_DisableIRQ(SLI_SIO_INTERRUPT);
}

/*******************************************************************************
 * This API is used when UART receive is done.
 ******************************************************************************/
void sl_si91x_sio_uart_rx_done(void)
{
  gstcSioCb.uart_sio.u16UartRxDone = 0;
}

/*******************************************************************************
 * This API is used to write the data over SIO-I2C.
 * The actions to be performed before I2C write are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - I2C generate start using @ref sl_si91x_sio_i2c_generate_start() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_i2c_write(stc_sio_i2c_config_t *configuration, uint8_t address, uint8_t *data, uint16_t length)
{
  sl_status_t status;
  error_t error_status;
#ifdef SIO_UC
  configuration = &i2cConfigUc;
#endif
  do {
    // Returns null pointer status code
    if (configuration == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Write data over I2C
    error_status = RSI_SIO_I2cWrite(SIO, configuration, address, data, length);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read the data over SIO-I2C.
 * The actions to be performed before I2C read are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - I2C generate start using @ref sl_si91x_sio_i2c_generate_start() API
 *    - Write data using @ref sl_si91x_sio_i2c_write() API
 *                        (OR) @ref sl_si91x_sio_i2c_transfer() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_i2c_read(stc_sio_i2c_config_t *configuration, uint8_t address, uint8_t *data, uint16_t length)
{
  sl_status_t status;
  error_t error_status;
#ifdef SIO_UC
  configuration = &i2cConfigUc;
#endif
  do {
    // Returns null pointer status code
    if (configuration == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Read data over I2C
    error_status = RSI_SIO_I2cRead(SIO, configuration, address, data, length);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to transfer the data over SIO-I2C.
 * The actions to be performed before I2C transfer are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - I2C generate start using @ref sl_si91x_sio_i2c_generate_start() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_i2c_transfer(stc_sio_i2c_config_t *configuration,
                                      uint8_t address,
                                      uint8_t *tx_buffer,
                                      uint16_t tx_length,
                                      uint8_t *rx_buffer,
                                      uint16_t rx_length)
{
#ifdef SIO_UC
  configuration = &i2cConfigUc;
#endif
  sl_status_t status;
  error_t error_status;
  do {
    // Returns null pointer status code
    if (configuration == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // Transfer data over I2C
    error_status = RSI_SIO_I2cTransfer(SIO, configuration, address, tx_buffer, tx_length, rx_buffer, rx_length);
    status       = convert_rsi_to_sl_error_code(error_status);
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for i2c generate start over SIO-I2C.
 * The actions to be performed before I2C generate start is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
void sl_si91x_sio_i2c_generate_start(void)
{
  RSI_SIO_I2cGenerateStart(SIO);
}

/*******************************************************************************
 * This API is used for i2c generate stop over SIO-I2C.
 * The actions to be performed before i2c generate stop are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - I2C generate start using @ref sl_si91x_sio_i2c_generate_start() API
 *    - Write data using @ref sl_si91x_sio_i2c_write() API
 *                        (OR) @ref sl_si91x_sio_i2c_transfer() API
 ******************************************************************************/
void sl_si91x_sio_i2c_generate_stop(void)
{
  RSI_SIO_I2cGenerateStop(SIO);
}

/*******************************************************************************
 * This API is used for configure gpio interrupt.
 * The actions to be performed before configuring interrupt is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_configure_interrupt(en_sio_channels_t channel, interrupt_flag_t flag)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and flag > 3
    if ((channel > SIO_CHANNEL) || (flag > FLAG)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Configure_Interrupt(SIO, channel, flag);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to match the pattern with data to be detected
 * When pattern match bit is enabled, pattern match register should be
 *  programmed with data pattern to be detected.
 * Pattern mask register to be programmed with valid bits to match.
 * The actions to be performed before pattern match is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_match_pattern(en_sio_channels_t channel,
                                       pattern_match_t pattern,
                                       uint8_t slice,
                                       uint32_t slice_pattern)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and pattern > 1
    if ((channel > SIO_CHANNEL) || (pattern >= TWO)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Match_Pattern(SIO, channel, pattern, slice, slice_pattern);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to generate shift clock
 * The actions to be performed before shift clock is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_shift_clock(uint32_t divider, en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Shift_Clock(SIO, divider, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to select internal/external clock
 * When external clock is used, shift/capture happens one system clock
 *  after the external clock edge
 * The actions to be performed before select clock is:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_select_clock(en_sio_channels_t channel, clock_type_t clock)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and clock > 1
    if ((channel > SIO_CHANNEL) || (clock >= TWO)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Select_Clock(SIO, channel, clock);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to shift number of bits
 * No. of shifts to happen before reloading the shift register with data/pausing
 * the operation. i.e. value to be set = (total no. of valid bits in shift
 * register/ number of bits per shift) – 1 The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_position_counter(en_sio_channels_t channel, uint32_t data_shift)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Position_Counter(SIO, channel, data_shift);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to enable/disable flow control bit. If flow control
 * disabled, Data shifting continues even when data in shift register is not
 * valid. Counters keep moving. If flow control is enabled, Counters and shift
 *    operations pause when data is not available.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_control_flow(en_sio_channels_t channel, flow_control_t flow_control)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and flow_control > 1
    if ((channel > SIO_CHANNEL) || (flow_control >= TWO)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Control_Flow(SIO, channel, flow_control);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to load data to buffer in reverse order
 * When set, the data on APB is loaded to buffer is reverse order. If the
 *    requirement is to shift out the MSB first, this has to be enabled.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_reverse_load(en_sio_channels_t channel, reverse_load_t reverse)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and reverse > 1
    if ((channel > SIO_CHANNEL) || (reverse >= TWO)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Reverse_Load(SIO, channel, reverse);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common swap interrupt enable set
 * Common swap interrupt enable set register for all SIOs. Each bit
 *  corresponds to one SIO. When '1' is written, swap interrupt
 *  enable gets set. Interrupt would be raised if not masked and interrupt
 *  status would be reflected in interrupt status register.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_set_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Set_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common swap interrupt enable clear
 * Common swap interrupt enable clear register for all SIOs. Each bit
 *  corresponds to one SIO. When '1' is written, swap interrupt
 *  enable gets cleared. Interrupt would not be raised and interrupt status
 *  would not be reflected in interrupt status register.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_clear_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Clear_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common swap interrupt mask set
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_mask_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Mask_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common swap interrupt mask clear
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_unmask_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_UnMask_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read common swap interrupt status
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
uint32_t sl_si91x_sio_get_interrupt_status(void)
{
  return RSI_SIO_Get_Interrupt_Status(SIO);
}

/*******************************************************************************
 * This API is used for common shift interrupt enable set
 * Common shift interrupt enable set register for all SIOs. Each bit
 *   corresponds to one SIO. When '1' is written, shift interrupt
 *   enable gets set. Interrupt would be raised if not masked and interrupt
 *    status would be reflected in interrupt status register.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_set_shift_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Set_Shift_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common shift interrupt enable clear
 * Common shift interrupt enable clear register for all SIOs. Each bit
 *  corresponds to one SIO. When '1' is written, shift interrupt
 *  enable gets cleared. Interrupt would not be raised and interrupt status
 *   would not be reflected in interrupt status register.
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_clear_shift_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Clear_Shift_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common shift interrupt mask set
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_mask_shift_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Mask_Shift_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used for common shift interrupt mask clear
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_unmask_shift_interrupt(en_sio_channels_t channel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_UnMask_Shift_Interrupt(SIO, channel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read common shift interrupt status
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
uint32_t sl_si91x_sio_shift_interrupt_status(void)
{
  return RSI_SIO_Shift_Interrupt_Status(SIO);
}

/*******************************************************************************
 * This API is used to select edge of the clock cycle for sampling bits
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 ******************************************************************************/
sl_status_t sl_si91x_sio_edge_select(en_sio_channels_t channel, edge_select_t edge_sel)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7 and edge_sel > 1
    if ((channel > SIO_CHANNEL) || (edge_sel >= TWO)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Edge_Select(SIO, channel, edge_sel);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * This API is used to read sio buffer register
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SPI using @ref sl_si91x_sio_spi_init() API for SIO-SPI
 *    - Initialize UART using @ref sl_si91x_sio_uart_init() API for SIO-UART
 ******************************************************************************/
uint32_t sl_si91x_sio_read_buffer(en_sio_channels_t channel)
{
  return RSI_SIO_Read_Buffer(SIO, channel);
}

/*******************************************************************************
 * This API is used to write into sio buffer register
 * The actions to be performed are:
 *    - Initialize SIO using @ref sl_si91x_sio_init() API
 *    - Initialize SPI using @ref sl_si91x_sio_spi_init() API for SIO-SPI
 *    - Initialize UART using @ref sl_si91x_sio_uart_init() API for SIO-UART
 ******************************************************************************/
sl_status_t sl_si91x_sio_write_buffer(en_sio_channels_t channel, uint32_t data)
{
  sl_status_t status;
  do {
    // Returns invalid parameter status code if channel > 7
    if (channel > SIO_CHANNEL) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_SIO_Write_Buffer(SIO, channel, data);
    status = SL_STATUS_OK;
  } while (false);
  return status;
}
