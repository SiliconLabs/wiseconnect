/*******************************************************************************
 * @file sio_example.c
 * @brief sio example
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
/**============================================================================
 * @brief : This file contains example application for SIO-SPI example
 * @section Description :
 * This application demonstrates setting SIO in SPI mode and transferring data
 *  in SPI mode and performing loop back test on data sent and received.
 ============================================================================**/
#include "sio_example.h"
#include "rsi_board.h"
#include "rsi_chip.h"
#include "sl_si91x_sio.h"
#include "string.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define BUFFER_SIZE 10        // Data buffer size
#define CS_NUMBER   0         // Chip select number
#define MSB_FIRST   1         // Select MSB first
#define SYS_CLOCK   180000000 // System clock

// SIO-SPI Configuration parameters
#define SIO_SPI_CLK_FREQUENCY 1000000 // SIO-SPI 1Mhz clock frequency
#define SIO_SPI_BIT_LEN       16      // SIO-SPI data length

// SIO-UART Configuration parameters
#define SIO_UART_BAUD_RATE 115200 // SIO-UART baud rate

// SIO-I2C Configuration parameters
#define SIO_I2C_SAMPLE_RATE 100000 // SIO-I2C sample rate
#define SIO_I2C_OEN         2      // SIO-I2C OEN selection
#define SIO_SLAVE_ADDRESS   0x50   // SIO-I2C slave address
/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
// Enum for enabling SIO instances functionality
typedef enum {
  SL_SIO_SPI  = 1, // Initialize SIO SPI instance
  SL_SIO_UART = 0, // Initialize SIO UART instance
  SL_SIO_I2C  = 0, // Initialize SIO I2C instance
} sio_instance_type_t;
/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
uint8_t data_out[BUFFER_SIZE]    = { 0 }, buffer_in[BUFFER_SIZE];
uint16_t data_in[BUFFER_SIZE]    = { 0 }, buffer_out[BUFFER_SIZE];
volatile boolean_t send_complete = false, receive_complete = false;
static boolean_t begin_transmission = true;
extern sl_sio_spi_callback_t spi_user_callback;
extern uint32_t SystemCoreClock;
uint8_t tx_data[BUFFER_SIZE] = { 0 };
uint8_t rx_data[BUFFER_SIZE] = { 0 };
extern uint8_t bit_length;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
void sio_spi_callback(en_sio_spi_events_t event);
static void compare_loop_back_data(void);
void sio_uart_callback(en_sio_Uart_events_t event, uint16_t rev_char);
static void compare_uart_loop_back_data(void);
/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/
sio_uart_mode_enum_t current_mode = SL_SEND_DATA;
stc_sio_spi_cfg_t pstcSpiConfig   = { 0 };
stc_sio_spi_xfer_t pstcSpiXfer    = { 0 };
extern stc_sio_uart_config_t UartInitstc;
uint8_t tx_buffer[BUFFER_SIZE] = { 0 };
uint8_t rx_buffer[BUFFER_SIZE] = { 0 };
stc_sio_i2c_config_t i2cConfig = { 0 };
extern sl_sio_spi_t sl_sio_spi_init;
extern sl_sio_uart_t sl_sio_uart_init;
extern sl_sio_i2c_t sl_sio_i2c_init;
/*******************************************************************************
 * SIO Example Initialization function for setting clock frequency, SIO
 *initialize, SPI initialize etc.
 ******************************************************************************/
void sio_example_init(void)
{
  sl_status_t status;
  sl_sio_version_t version;
  SystemCoreClock = SYS_CLOCK;

  // Version information of sio
  version = sl_si91x_sio_get_version();
  DEBUGOUT("SIO version is fetched successfully \n");
  DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
  do {
    // Initialize SIO SPI instance
    if (SL_SIO_SPI == 1) {
      for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        buffer_in[i] = (uint8_t)(i + 1);
      }
      for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        data_in[i] = (uint8_t)(i + 1);
      }
      // Initialize the SIO
      status = sl_si91x_sio_init();
      // Check the status error code
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_init: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO GPIO initialization is successful \n");
      status = sl_si91x_sio_spi_pin_initialization(&sl_sio_spi_init);
      // Check the status error code
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_spi_pin_initialization: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO SPI GPIO pin initialization is successful \n");
      pstcSpiConfig.u32SpiClockFrq = SIO_SPI_CLK_FREQUENCY; // SIO SPI clock frequency
      pstcSpiConfig.u8BitOrder     = SL_SIO_SPI_MSB_FIRST;  // SIO SPI MSB bit order
      pstcSpiConfig.u8SpiClkCh     = SL_SIO_CH_1;           // SIO SPI channel clock
      pstcSpiConfig.u8SpiMosiCh    = SL_SIO_CH_3;           // SIO SPI MOSI
      pstcSpiConfig.u8SpiMisoCh    = SL_SIO_CH_2;           // SIO SPI MISO
      pstcSpiConfig.u8SpiCsCh      = SL_SIO_CH_0;           // SIO SPI chip select
      pstcSpiConfig.u8BitLen       = SL_SIO_SPI_BIT_16;     // SIO SPI bit length
      pstcSpiConfig.u8Mode         = SL_SIO_SPI_MODE_3;     // SIO SPI mode
      uint32_t divider             = pstcSpiConfig.u32SpiClockFrq;
      status                       = sl_si91x_sio_select_clock(pstcSpiConfig.u8SpiClkCh, INTERNAL_CLOCK);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_select_clock: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO clock selection is successful \n");
      // Initialize the SIO-SPI
      // Generate shift clock frequency
      status = sl_si91x_sio_shift_clock(divider, pstcSpiConfig.u8SpiMisoCh);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_shift_clock: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO shift clock is successful \n");
      // Bit order configuration
      if (pstcSpiConfig.u8BitOrder == MSB_FIRST) {
        // MSB First
        status = sl_si91x_sio_reverse_load(pstcSpiConfig.u8SpiMosiCh, REVERSE_ENABLE);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_sio_reverse_load: Error Code : %lu \n", status);
          break;
        }
        DEBUGOUT("SIO reverse load is successful \n");
      } else {
        // LSB First
        status = sl_si91x_sio_reverse_load(pstcSpiConfig.u8SpiMosiCh, REVERSE_DISABLE);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_sio_reverse_load: Error Code : %lu \n", status);
          break;
        }
        DEBUGOUT("SIO reverse load is successful \n");
      }
      status = sl_si91x_sio_spi_init(&pstcSpiConfig);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_spi_init: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO SPI initialization is successful \n");
      status = sl_si91x_sio_control_flow(pstcSpiConfig.u8SpiMosiCh, FLOW_CONTROL_DISABLE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_control_flow: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO flow control is successful \n");
      // Enable Swap interrupt
      status = sl_si91x_sio_set_interrupt(pstcSpiConfig.u8SpiMosiCh);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_set_interrupt: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO set interrupt is successful \n");
      // Data sampling on negative edge configuration
      status = sl_si91x_sio_edge_select(pstcSpiConfig.u8SpiMosiCh, NEG_EDGE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_edge_select: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO edge selection is successful \n");
      // Register user callback function
      status = sl_si91x_sio_spi_register_event_callback(sio_spi_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_spi_register_event_callback: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO SPI user event callback registered successfully \n");
    }
    // Initialize SIO UART instance
    if (SL_SIO_UART == 1) {
      // Filling the tx_buffer array with integer values
      for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        tx_buffer[i] = (uint8_t)i;
      }
      // SIO UART configuration structure
      UartInitstc.u32BaudRate        = SIO_UART_BAUD_RATE;
      UartInitstc.u8Bitlen           = SL_SIO_UART_BIT_8;
      UartInitstc.u8Parity           = SL_SIO_UART_EVEN_PARITY;
      UartInitstc.u8SioUartRxChannel = SL_SIO_CH_2;
      UartInitstc.u8SioUartTxChannel = SL_SIO_CH_3;
      UartInitstc.u8StopBits         = SL_SIO_UART_STOP_BIT_1;
      UartInitstc.pfn                = sio_uart_callback;
      // Initialize the SIO
      status = sl_si91x_sio_init();
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_init: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO GPIO initialization is successful \n");
      status = sl_si91x_sio_uart_pin_initialization(&sl_sio_uart_init);
      // Check the status error code
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_uart_pin_initialization: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO UART GPIO pin initialization is successful \n");
      // Register user callback function
      status = sl_si91x_sio_uart_register_event_callback(sio_uart_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_uart_register_event_callback: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO UART user event callback registered successfully \n");
      // Initialize SIO UART
      uint32_t divider = UartInitstc.u32BaudRate;
      // Initialize the SIO-SPI
      // Generate shift clock frequency
      status = sl_si91x_sio_shift_clock(divider, UartInitstc.u8SioUartRxChannel);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_shift_clock: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO shift clock is successful \n");
      // Fall Edge detection
      status = sl_si91x_sio_configure_interrupt(UartInitstc.u8SioUartRxChannel, INTERRUPT_FALL_EDGE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_configure_interrupt: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO interrupt configure is successful \n");
      status = sl_si91x_sio_uart_init(&UartInitstc);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_uart_init: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO UART initialization is successful \n");
      // Reverse load is disabled in RX
      status = sl_si91x_sio_reverse_load(UartInitstc.u8SioUartRxChannel, REVERSE_DISABLE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_reverse_load: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO reverse load is successful \n");
      // Configure data position counters
      status = sl_si91x_sio_position_counter(
        UartInitstc.u8SioUartTxChannel,
        (uint32_t)((UartInitstc.u8Bitlen + UartInitstc.u8StopBits + gstcSioCb.uart_sio.u8ParityEn + 10) - 1));
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_position_counter: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO position counter is successful \n");
      // Enable Swap interrupt
      status = sl_si91x_sio_set_interrupt(UartInitstc.u8SioUartRxChannel);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_set_interrupt: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO set interrupt is successful \n");
      // Enable shift interrupt
      status = sl_si91x_sio_set_shift_interrupt(UartInitstc.u8SioUartRxChannel);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_set_shift_interrupt: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO set shift interrupt is successful \n");
      status = sl_si91x_sio_unmask_shift_interrupt(UartInitstc.u8SioUartRxChannel);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_unmask_shift_interrupt: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO unmask shift interrupt is successful \n");
      status = sl_si91x_sio_uart_read(rx_buffer, BUFFER_SIZE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_uart_read: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO uart read is successful \n");
    }
    // Initialize SIO UART instance
    if (SL_SIO_I2C == 1) {
      // Prepare the I2C Write buffer
      for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        tx_data[i] = i + 1;
      }
      // Initialize the SIO
      status = sl_si91x_sio_init();
      // Check the status error code
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_init: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO GPIO initialization is successful \n");
      status = sl_si91x_sio_i2c_pin_initialization(&sl_sio_i2c_init);
      // Check the status error code
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_sio_i2c_pin_initialization: Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("SIO I2C GPIO pin initialization is successful \n");
      i2cConfig.u32SampleRate = SIO_I2C_SAMPLE_RATE;
      i2cConfig.u8SioI2cOen   = SIO_I2C_OEN;
      i2cConfig.u8SioI2cScl   = SL_SIO_CH_6;
      i2cConfig.u8SioI2cSda   = SL_SIO_CH_7;
    }
  } while (false);
}

/*******************************************************************************
 * SIO example process action function to transfer the data
 ******************************************************************************/
void sio_example_process_action(void)
{
  sl_status_t status;
  // In this switch case, according to the macros enabled in header file, it
  // starts to execute the APIs Assuming all the macros are enabled, after
  // transfer, send will be executed and after send receive will be executed.
  // Initialize SIO SPI instance
  if (SL_SIO_SPI == 1) {
    switch (current_mode) {
      case SL_SEND_DATA:
        if (begin_transmission == true) {
          // Assert the SIO SPI chip select
          status = sl_si91x_sio_spi_cs_assert(pstcSpiConfig.u8SpiCsCh);
          if (status != SL_STATUS_OK) {
            // If it fails to execute the API, it will not execute rest of the
            // things
            DEBUGOUT("sl_si91x_sio_spi_cs_assert: Error Code : %lu \n", status);
            current_mode = SL_TRANSMISSION_COMPLETED;
          }
          DEBUGOUT("SIO SPI chip select assert successfully \n");
          if (bit_length == 8) {
            pstcSpiXfer.rxBuff = data_out;
            pstcSpiXfer.txBuff = buffer_in;
          } else {
            pstcSpiXfer.rxBuff = buffer_out;
            pstcSpiXfer.txBuff = data_in;
          }
          // SPI Transfer configuration
          pstcSpiXfer.rxCount  = BUFFER_SIZE;
          pstcSpiXfer.sselNum  = CS_NUMBER;
          pstcSpiXfer.txCount  = BUFFER_SIZE;
          pstcSpiXfer.u8BitLen = SIO_SPI_BIT_LEN;
          pstcSpiXfer.u8Status = SioSpiIdle;
          pstcSpiXfer.pfnCb    = spi_user_callback;

          // SIO-SPI transfer function
          status = sl_si91x_sio_spi_transfer(&pstcSpiXfer);
          if (status != SL_STATUS_OK) {
            current_mode = SL_TRANSMISSION_COMPLETED;
            // If it fails to execute the API, it will not execute rest of the
            // things
            DEBUGOUT("sl_si91x_sio_spi_transfer: Error Code : %lu \n", status);
          }
          DEBUGOUT("SIO SPI transfer begin successfully \n");
          begin_transmission = false;
        }
        // Waiting till the send is completed
        if (send_complete) {
          // Update send_complete flag with 0.
          send_complete = false;
          DEBUGOUT("SIO SPI transfer completed \n");
          // De-assert SIO-SPI chip select
          sl_si91x_sio_spi_cs_deassert(pstcSpiConfig.u8SpiCsCh);
          current_mode = SL_SEND_DATA;
          // After comparing the loopback transfer, it compares the data_out and
          // data_outin.
          compare_loop_back_data();
          begin_transmission = true;
        }
        break;
      case SL_TRANSMISSION_COMPLETED:
        break;
      case SL_RECEIVE_DATA:
        break;
      case SL_TRANSFER_DATA:
        break;
    }
  }
  // Initialize SIO UART instance
  if (SL_SIO_UART == 1) {
    switch (current_mode) {
      case SL_RECEIVE_DATA:
        if (receive_complete == true) {
          receive_complete = false;
          DEBUGOUT("SIO UART receive completed \n");
          // After comparing the loopback transfer, it compares the tx_buffer and
          // rx_buffer.
          compare_uart_loop_back_data();
          begin_transmission = true;
          current_mode       = SL_SEND_DATA;
        }
        break;
      case SL_SEND_DATA:
        if (begin_transmission == true) {
          // Validation for executing the API only once
          status = sl_si91x_sio_uart_send(tx_buffer, (sizeof(tx_buffer) / sizeof(tx_buffer[0])));
          if (status != SL_STATUS_OK) {
            // If it fails to execute the API, it will not execute rest of the
            // things
            DEBUGOUT("sl_si91x_sio_uart_send: Error Code : %lu \n", status);
            current_mode = SL_TRANSMISSION_COMPLETED;
            break;
          }
          DEBUGOUT("SIO UART send begin successfully \n");
          begin_transmission = false;
          current_mode       = SL_RECEIVE_DATA;
          DEBUGOUT("SIO UART send completed \n");
        }
        break;
      case SL_TRANSMISSION_COMPLETED:
        break;
      case SL_TRANSFER_DATA:
        break;
    }
  }
  // Initialize SIO UART instance
  if (SL_SIO_I2C == 1) {
    sl_si91x_sio_i2c_generate_start();
    sl_si91x_sio_i2c_transfer(&i2cConfig, SIO_SLAVE_ADDRESS, tx_data, BUFFER_SIZE, rx_data, BUFFER_SIZE);
    sl_si91x_sio_i2c_generate_stop();

    sl_si91x_sio_i2c_generate_start();
    sl_si91x_sio_i2c_transfer(&i2cConfig, SIO_SLAVE_ADDRESS, tx_data, BUFFER_SIZE, rx_data, BUFFER_SIZE);
    DEBUGOUT("SIO i2c transfer is successful \n");
    sl_si91x_sio_i2c_generate_stop();

    sl_si91x_sio_i2c_write(&i2cConfig, SIO_SLAVE_ADDRESS, tx_data, BUFFER_SIZE);
    sl_si91x_sio_i2c_read(&i2cConfig, SIO_SLAVE_ADDRESS, rx_data, BUFFER_SIZE);
    sl_si91x_sio_i2c_generate_stop();

    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
      DEBUGOUT("rx_data[%d] = %d\n", i, rx_data[i]);
    }
    while (1)
      ;
  }
}

/*******************************************************************************
 * Compares data received buffer with data transfer buffer via SIO UART and
 *print the loop back test passed (or) failed
 ******************************************************************************/
static void compare_uart_loop_back_data(void)
{
  uint16_t data_index = 0;
  // Check for tx_buffe and rx_buffer are same, if same then comparison
  // will continue till end of the buffer
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    if (rx_buffer[data_index] != tx_buffer[data_index]) {
      break;
    }
  }
  if (data_index == BUFFER_SIZE) {
    DEBUGOUT("Data comparison successful, UART Loop Back Test Passed \n");
  } else {
    DEBUGOUT("Data comparison failed, UART Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * Compares data out buffer via SIO SPI with data in buffer and print
 *   the loop back test passed (or) failed
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;
  // Check for buffer_in and data_out are same, if same then comparison
  // will continue till end of the buffer
  if (bit_length == 8) {
    for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
      if (buffer_in[data_index] != data_out[data_index]) {
        break;
      }
    }
  }
  if (bit_length == 16) {
    for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
      if (data_in[data_index] != buffer_out[data_index]) {
        break;
      }
    }
  }
  if (data_index == BUFFER_SIZE) {
    DEBUGOUT("Data comparison successful, Loop Back Test Passed \n");
  } else {
    DEBUGOUT("Data comparison failed, Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * SIO SPI Callback function triggered on data Transfer and reception
 ******************************************************************************/
void sio_spi_callback(en_sio_spi_events_t event)
{
  switch (event) {
    case SL_SIO_SPI_TX_DONE: // SIO SPI send complete
      send_complete = true;
      break;
    case SL_SIO_SPI_RX_DONE: // SIO SPI receive complete
      break;
  }
}

/*******************************************************************************
 * SIO UART Callback function triggered on data Transfer and reception
 ******************************************************************************/
void sio_uart_callback(en_sio_Uart_events_t event, uint16_t rev_char)
{
  (void)rev_char;
  switch (event) {
    case SL_SIO_UART_TX_DONE: // SIO UART transfer complete event
      break;
    case SL_SIO_UART_RX_DONE: // SIO UART receive complete event
      receive_complete = true;
      sl_si91x_sio_uart_rx_done();
      break;
    case SL_SIO_UART_PARITY_ERR: // SIO UART parity error event
      break;
    case SL_SIO_UART_FRAMING_ERR: // SIO UART framing error event
      break;
    case SL_SIO_UART_RECEIVE_CHAR: // SIO UART receive character
      break;
  }
}
