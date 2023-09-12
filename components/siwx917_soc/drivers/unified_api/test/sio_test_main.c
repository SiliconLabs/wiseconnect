/***************************************************************************/ /**
 * @file sio_test_main.c
 * @brief SIO Unity Test Functions
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
 ******************************************************************************
 ******************************************************************************/
#include "rsi_board.h"
#include "rsi_chip.h"
#include "sl_si91x_sio.h"
#include "sl_system_init.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SIO_SPI_CLK_FREQUENCY 1000000 // SIO SPI clock frequency
#define SIO_SPI_MSB_LSB       1       // SIO SPI MSB bit order
#define SIO_SPI_CLK_CH        1       // SIO SPI channel clock
#define SIO_SPI_MOSI_CH       3       // SIO SPI MOSI
#define SIO_SPI_MISO_CH       2       // SIO SPI MISO
#define SIO_SPI_CS_CH         0       // SIO SPI chip select
#define SIO_SPI_BIT_LEN       16      // SIO SPI bit length
#define SIO_SPI_MODE          0       // SIO SPI mode

#define SIO_UART_BAUD_RATE  115200 // SIO-UART baud rate
#define SIO_UART_BIT_LENGTH 8      // SIO-UART bit length
#define SIO_UART_PARITY     0      // SIO_UART parity
#define SIO_UART_RX_CHANNEL 2      // SIO_UART receive channel
#define SIO_UART_TX_CHANNEL 3      // SIO_UART transmit channel
#define SIO_UART_STOP_BIT   1      // SIO_UART stop bit

#define CHIP_SELECT 8  // SPI chip select number
#define BUFFER_SIZE 10 // Buffer Size
#define CHANNEL     8
#define ZERO        0
#define ONE         1
#define TWO         2
#define THREE       3
#define FLAG        4
#define SIZE        6
#define TEN         10
#define DATA        0x45

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void sio_spi_callback(en_sio_spi_events_t event);
static void default_spi_xfer_config_structure(sl_sio_spi_xfer_config_t *sio_spi_xfer_config);
static void default_spi_config_struct(sl_sio_spi_config_t *config);
static void default_uart_config_struct(sl_sio_uart_config_t *config);
static void sio_uart_callback(en_sio_Uart_events_t event, uint16_t rev_char);

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
uint8_t data_in[BUFFER_SIZE];
uint8_t data_out[BUFFER_SIZE];
volatile boolean_t send_comp = false, recv_comp = false, transfer_complete = false;
extern sl_sio_spi_callback_t spi_user_callback;
uint8_t tx_buffer1[SIZE];
uint8_t rx_buffer1[SIZE];
uint8_t buffer_in[BUFFER_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

/*******************************************************************************
 ************************  Test Function Prototypes ****************************
 ******************************************************************************/
void test_sio_init(void);
void test_sio_spi_init(void);
void test_sio_spi_cs_assert(void);
void test_spi_register_callback(void);
void test_spi_unregister_callback(void);
void test_sio_spi_transfer(void);
void test_sio_spi_cs_deassert(void);
void test_sio_uart_init(void);
void test_uart_register_callback(void);
void test_uart_unregister_callback(void);
void test_sio_uart_send_read(void);
void test_sio_uart_send_read_blocking(void);
void test_sio_get_version(void);
void test_sio_configure_interrupt(void);
void test_sio_match_pattern(void);
void test_sio_spi_shift_clock(void);
void test_sio_select_clock(void);
void test_sio_control_flow(void);
void test_sio_spi_reverse_load(void);
void test_sio_edge_select(void);
void test_sio_spi_set_interrupt(void);
void test_sio_clear_interrupt(void);
void test_sio_mask_interrupt(void);
void test_sio_unmask_interrupt(void);
void test_sio_set_shift_interrupt(void);
void test_sio_clear_shift_interrupt(void);
void test_sio_mask_shift_interrupt(void);
void test_sio_unmask_shift_interrupt(void);
void test_sio_spi_pos_count(void);
void test_sio_uart_shift_clock(void);
void test_sio_uart_reverse_load(void);
void test_sio_uart_set_interrupt(void);
void test_sio_uart_pos_count(void);
void test_sio_read_write_buffer(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("SIO");

  RUN_TEST(test_sio_init, __LINE__);
  RUN_TEST(test_sio_select_clock, __LINE__);
  RUN_TEST(test_sio_spi_shift_clock, __LINE__);
  RUN_TEST(test_sio_spi_reverse_load, __LINE__);
  RUN_TEST(test_sio_spi_init, __LINE__);
  RUN_TEST(test_sio_spi_pos_count, __LINE__);
  RUN_TEST(test_sio_control_flow, __LINE__);
  RUN_TEST(test_sio_edge_select, __LINE__);
  RUN_TEST(test_sio_spi_set_interrupt, __LINE__);
  RUN_TEST(test_sio_unmask_interrupt, __LINE__);
  RUN_TEST(test_spi_register_callback, __LINE__);
  RUN_TEST(test_sio_spi_cs_assert, __LINE__);
  RUN_TEST(test_sio_spi_transfer, __LINE__);
  RUN_TEST(test_sio_spi_cs_deassert, __LINE__);
  RUN_TEST(test_spi_unregister_callback, __LINE__);
  RUN_TEST(test_sio_clear_interrupt, __LINE__);
  RUN_TEST(test_sio_mask_interrupt, __LINE__);
  RUN_TEST(test_sio_uart_shift_clock, __LINE__);
  RUN_TEST(test_sio_configure_interrupt, __LINE__);
  RUN_TEST(test_sio_uart_init, __LINE__);
  RUN_TEST(test_sio_uart_reverse_load, __LINE__);
  RUN_TEST(test_sio_uart_pos_count, __LINE__);
  RUN_TEST(test_sio_uart_set_interrupt, __LINE__);
  RUN_TEST(test_sio_unmask_interrupt, __LINE__);
  RUN_TEST(test_sio_set_shift_interrupt, __LINE__);
  RUN_TEST(test_sio_unmask_shift_interrupt, __LINE__);
  RUN_TEST(test_uart_register_callback, __LINE__);
  RUN_TEST(test_sio_uart_send_read, __LINE__);
  RUN_TEST(test_sio_uart_send_read_blocking, __LINE__);
  RUN_TEST(test_uart_unregister_callback, __LINE__);
  RUN_TEST(test_sio_clear_interrupt, __LINE__);
  RUN_TEST(test_sio_mask_interrupt, __LINE__);
  RUN_TEST(test_sio_clear_shift_interrupt, __LINE__);
  RUN_TEST(test_sio_mask_shift_interrupt, __LINE__);
  RUN_TEST(test_sio_get_version, __LINE__);
  RUN_TEST(test_sio_read_write_buffer, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the initialization of SIO
 ******************************************************************************/
void test_sio_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO Initialization \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns OK (or) not \n");
  status = sl_si91x_sio_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SIO is initialization successfully \n");

  UnityPrintf("SIO Initialization test completed \n");
}

/*******************************************************************************
 * Function to test the SPI initialization
 ******************************************************************************/
void test_sio_spi_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-SPI Initialization \n");
  sl_status_t status;
  sl_sio_spi_config_t sio_spi_config;

  UnityPrintf("Testing with all the correct parameters \n");
  default_spi_config_struct(&sio_spi_config);
  status = sl_si91x_sio_spi_init(&sio_spi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SIO-SPI initialization is successful \n");

  UnityPrintf("SIO-SPI initialization test completed \n ");
}

/*******************************************************************************
 * Function to test the SIO SPI chip select assert
 ******************************************************************************/
void test_sio_spi_cs_assert(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SPI chip select assert \n");
  sl_status_t status;
  uint8_t chip_select_num = ZERO;

  UnityPrintf("Testing with invalid chip select parameter \n");
  status = sl_si91x_sio_spi_cs_assert(CHIP_SELECT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Chip Select Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid chip select parameter \n");
  status = sl_si91x_sio_spi_cs_assert(chip_select_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Chip Select Test Passed "
              "successfully \n");

  UnityPrintf("SPI chip select assert test completed \n ");
}

/*******************************************************************************
 * Function to test the register callback SIO SPI
 ******************************************************************************/
void test_spi_register_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SPI Register Callback \n");
  sl_status_t status;

  UnityPrintf("Registering callback \n");
  status = sl_si91x_sio_spi_register_event_callback(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing Callback register \n");
  status = sl_si91x_sio_spi_register_event_callback(sio_spi_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback Register Test Passed "
              "successfully \n");

  UnityPrintf("Testing Callback register again \n");
  status = sl_si91x_sio_spi_register_event_callback(sio_spi_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Callback is already registered, "
              "cannot register another\n");

  UnityPrintf("Unregistering the callback \n");
  sl_si91x_sio_spi_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");

  UnityPrintf("Testing Callback register again \n");
  status = sl_si91x_sio_spi_register_event_callback(sio_spi_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is again registered successfully \n");

  UnityPrintf("SPI register callback test completed \n ");
}

/*******************************************************************************
 * Function to test the unregister callback SIO SPI
 ******************************************************************************/
void test_spi_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SPI unregister Callback \n");

  UnityPrintf("Unregistering the callback \n");
  sl_si91x_sio_spi_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");

  UnityPrintf("SPI unregister callback test completed \n ");
}

/*******************************************************************************
 * Function to test the transfer data function of SIO
 * It includes the loop back testing.
 * NOTE: To pass this test, the MISO and MISO must be short circuited.
 ******************************************************************************/
void test_sio_spi_transfer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO Transfer \n");
  sl_status_t status;
  sl_sio_spi_xfer_config_t spi_xfer_config;
  uint16_t data_index = ZERO;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_sio_spi_transfer(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing API with valid parameter \n");
  UnityPrintf("Loop back Test started \n");
  default_spi_xfer_config_structure(&spi_xfer_config);
  for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + ONE;
    data_in[i]  = ZERO;
  }

  status = sl_si91x_sio_spi_transfer(&spi_xfer_config);
  while (!send_comp)
    ;
  send_comp = ZERO;
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SPI data transfer begins successfully \n");

  UnityPrintf("SPI data transfer complete \n");
  UnityPrintf("Comparing the values of send data and receive \n");
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    if (data_in[data_index] != buffer_in[data_index]) {
      break;
    }
  }

  TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, data_index);
  UnityPrintf("Output and Input values are same \n");
  UnityPrintf("Loop back Test passed \n");

  UnityPrintf("SPI transfer data test completed \n ");

  UnityPrintf("SPI data transfer test completed \n ");
}

/*******************************************************************************
 * Function to test the SIO SPI chip select de-assert
 ******************************************************************************/
void test_sio_spi_cs_deassert(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SPI chip select de-assert \n");
  sl_status_t status;
  uint8_t chip_select_num = ZERO;

  UnityPrintf("Testing with invalid chip select parameter \n");
  status = sl_si91x_sio_spi_cs_deassert(CHIP_SELECT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Chip Select Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid chip select parameter \n");
  status = sl_si91x_sio_spi_cs_deassert(chip_select_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Chip Select Test Passed "
              "successfully \n");

  UnityPrintf("SPI chip select de-assert test completed \n ");
}

/*******************************************************************************
 * Function to test the get version API
 ******************************************************************************/
void test_sio_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO get version \n");
  sl_sio_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_sio_get_version();
  UnityPrintf("Verifying SIO Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(ZERO, version.release);
  TEST_ASSERT_EQUAL_INT(ZERO, version.major);
  TEST_ASSERT_EQUAL_INT(ONE, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("SIO get version test completed \n ");
}

/*******************************************************************************
 * Function to test the UART initialization
 ******************************************************************************/
void test_sio_uart_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-UART Initialization \n");
  sl_status_t status;
  sl_sio_uart_config_t sio_uart_config;

  UnityPrintf("Testing with all the correct parameters \n");
  default_uart_config_struct(&sio_uart_config);
  status = sl_si91x_sio_uart_init(&sio_uart_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SIO-UART initialization is successful \n");

  UnityPrintf("SIO-UART initialization test completed \n ");
}

/*******************************************************************************
 * Function to test the register callback SIO UART
 ******************************************************************************/
void test_uart_register_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing UART Register Callback \n");
  sl_status_t status;

  UnityPrintf("Registering callback \n");
  status = sl_si91x_sio_uart_register_event_callback(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing Callback register \n");
  status = sl_si91x_sio_uart_register_event_callback(sio_uart_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback Register Test Passed "
              "successfully \n");

  UnityPrintf("Testing Callback register again \n");
  status = sl_si91x_sio_uart_register_event_callback(sio_uart_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Callback is already registered, "
              "cannot register another\n");

  UnityPrintf("Unregistering the callback \n");
  sl_si91x_sio_uart_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");

  UnityPrintf("Testing Callback register again \n");
  status = sl_si91x_sio_uart_register_event_callback(sio_uart_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is again registered successfully \n");

  UnityPrintf("UART register callback test completed \n ");
}

/*******************************************************************************
 * Function to test the unregister callback SIO UART
 ******************************************************************************/
void test_uart_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing UART unregister Callback \n");

  UnityPrintf("Unregistering the callback \n");
  sl_si91x_sio_uart_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");

  UnityPrintf("UART unregister callback test completed \n ");
}

/*******************************************************************************
 * Function to test the send and read data function of SIO UART
 ******************************************************************************/
void test_sio_uart_send_read(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing UART send and read Data \n");
  sl_status_t status;
  sl_sio_uart_config_t sio_uart_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_sio_uart_send(NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  strcpy((char *)tx_buffer1, "HELLO");
  // SIO UART configuration structure
  sio_uart_config.u32BaudRate        = SIO_UART_BAUD_RATE;
  sio_uart_config.u8Bitlen           = SIO_UART_BIT_LENGTH;
  sio_uart_config.u8Parity           = SIO_UART_PARITY;
  sio_uart_config.u8SioUartRxChannel = SIO_UART_RX_CHANNEL;
  sio_uart_config.u8SioUartTxChannel = SIO_UART_TX_CHANNEL;
  sio_uart_config.u8StopBits         = SIO_UART_STOP_BIT;
  sio_uart_config.pfn                = sio_uart_callback;
  status                             = sl_si91x_sio_uart_init(&sio_uart_config);

  UnityPrintf("Status of API is correct, UART data send and read begins "
              "successfully \n");
  sl_si91x_sio_uart_read(rx_buffer1, (uint16_t)strlen((const char *)tx_buffer1));
  status = sl_si91x_sio_uart_send(tx_buffer1, (uint16_t)strlen((const char *)tx_buffer1));
  while (!recv_comp)
    ;

  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UART data send and read complete \n");

  UnityPrintf("UART Send and Read Data test completed \n ");
}

/*******************************************************************************
 * Function to test the send and read blocking data in UART mode
 ******************************************************************************/
void test_sio_uart_send_read_blocking(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing UART send and read blocking data \n");
  sl_status_t status;
  sl_sio_uart_config_t sio_uart_config;

  strcpy((char *)tx_buffer1, "HELLO");
  // SIO UART configuration structure
  sio_uart_config.u32BaudRate        = SIO_UART_BAUD_RATE;
  sio_uart_config.u8Bitlen           = SIO_UART_BIT_LENGTH;
  sio_uart_config.u8Parity           = SIO_UART_PARITY;
  sio_uart_config.u8SioUartRxChannel = SIO_UART_RX_CHANNEL;
  sio_uart_config.u8SioUartTxChannel = SIO_UART_TX_CHANNEL;
  sio_uart_config.u8StopBits         = SIO_UART_STOP_BIT;
  sio_uart_config.pfn                = sio_uart_callback;
  status                             = sl_si91x_sio_uart_init(&sio_uart_config);
  gstcSioCb.uart_sio.pvUartRx        = ((uint8_t *)rx_buffer1);

  UnityPrintf("Status of API is correct, UART data read blocking Data  begins "
              "successfully \n");
  status = sl_si91x_sio_uart_send_blocking(tx_buffer1, (uint16_t)strlen((const char *)tx_buffer1));
  while (!send_comp)
    ;
  sl_si91x_sio_uart_read(rx_buffer1, (uint16_t)strlen((const char *)tx_buffer1));

  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("UART send and read blocking data complete \n");

  UnityPrintf("UART send and read blocking data test completed \n ");
}

/*******************************************************************************
 * Function to test the gpio interrupt configuration
 ******************************************************************************/
void test_sio_configure_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO configure interrupt \n");
  sl_status_t status;
  uint8_t channel = TWO, flag = ONE;

  UnityPrintf("Testing with invalid channel and flag parameter \n");
  status = sl_si91x_sio_configure_interrupt(CHANNEL, FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and flag parameter \n");
  status = sl_si91x_sio_configure_interrupt(channel, flag);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO configure interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the pattern match detection
 ******************************************************************************/
void test_sio_match_pattern(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO pattern match \n");
  sl_status_t status;
  uint8_t channel = ZERO;

  UnityPrintf("Testing with invalid channel and pattern match parameter \n");
  status = sl_si91x_sio_match_pattern(CHANNEL, TWO, ZERO, ZERO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and pattern match parameter \n");
  status = sl_si91x_sio_match_pattern(channel, ZERO, ZERO, ZERO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO pattern match test completed \n ");
}

/*******************************************************************************
 * Function to test the shift clock generator in spi
 ******************************************************************************/
void test_sio_spi_shift_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-SPI shift clock \n");
  sl_status_t status;
  uint8_t channel = TWO;
  uint32_t divider;
  sl_sio_spi_config_t sio_spi_config;

  UnityPrintf("Testing with invalid channel and divider parameter \n");
  status = sl_si91x_sio_shift_clock(ZERO, CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");
  default_spi_config_struct(&sio_spi_config);

  divider = sio_spi_config.u32SpiClockFrq;
  UnityPrintf("Testing with valid channel and divider parameter \n");
  status = sl_si91x_sio_shift_clock(divider, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-SPI shift clock test completed \n ");
}

/*******************************************************************************
 * Function to test the internal/external clock
 ******************************************************************************/
void test_sio_select_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO select clock \n");
  sl_status_t status;
  uint8_t channel = ONE, clock = ZERO;

  UnityPrintf("Testing with invalid channel and clock parameter \n");
  status = sl_si91x_sio_select_clock(CHANNEL, TWO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and clock parameter \n");
  status = sl_si91x_sio_select_clock(channel, clock);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO select clock test completed \n ");
}

/*******************************************************************************
 * Function to test the data position counter in spi
 ******************************************************************************/
void test_sio_spi_pos_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-SPI position counter \n");
  sl_status_t status;
  uint8_t channel = THREE;
  uint32_t data_shift;
  sl_sio_spi_config_t sio_spi_config;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_position_counter(CHANNEL, ZERO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");
  default_spi_config_struct(&sio_spi_config);

  data_shift = sio_spi_config.u8BitLen - 1;
  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_position_counter(channel, data_shift);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-SPI position counter test completed \n ");
}

/*******************************************************************************
 * Function to test the flow control
 ******************************************************************************/
void test_sio_control_flow(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO flow control \n");
  sl_status_t status;
  uint8_t channel = THREE, flow_control = ZERO;

  UnityPrintf("Testing with invalid channel and flow control parameter \n");
  status = sl_si91x_sio_control_flow(CHANNEL, TWO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and flow control parameter \n");
  status = sl_si91x_sio_control_flow(channel, flow_control);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO flow control test completed \n ");
}

/*******************************************************************************
 * Function to test the reverse load in spi
 ******************************************************************************/
void test_sio_spi_reverse_load(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-SPI reverse load \n");
  sl_status_t status;
  uint8_t channel = THREE, reverse_load = ONE;

  UnityPrintf("Testing with invalid channel and reverse load parameter \n");
  status = sl_si91x_sio_reverse_load(CHANNEL, TWO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and reverse load parameter \n");
  status = sl_si91x_sio_reverse_load(channel, reverse_load);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-SPI reverse load test completed \n ");
}

/*******************************************************************************
 * Function to test the edge selection
 ******************************************************************************/
void test_sio_edge_select(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO edge select \n");
  sl_status_t status;
  uint8_t channel = THREE, edge_select = ONE;

  UnityPrintf("Testing with invalid channel and edge select parameter \n");
  status = sl_si91x_sio_edge_select(CHANNEL, TWO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and edge select parameter \n");
  status = sl_si91x_sio_edge_select(channel, edge_select);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO edge select test completed \n ");
}

/*******************************************************************************
 * Function to test the common swap interrupt enable in spi
 ******************************************************************************/
void test_sio_spi_set_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-SPI set interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_set_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_set_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-SPI set interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common swap interrupt clear
 ******************************************************************************/
void test_sio_clear_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO clear interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_clear_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_clear_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO clear interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common swap interrupt mask enable
 ******************************************************************************/
void test_sio_mask_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO mask interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_mask_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_mask_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO mask interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common swap interrupt mask enable
 ******************************************************************************/
void test_sio_unmask_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO un-mask interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_unmask_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_unmask_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO un-mask interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common shift interrupt enable
 ******************************************************************************/
void test_sio_set_shift_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO set shift interrupt \n");
  sl_status_t status;
  uint8_t channel = TWO;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_set_shift_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_set_shift_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO set shift interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common shift interrupt clear
 ******************************************************************************/
void test_sio_clear_shift_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO clear shift interrupt \n");
  sl_status_t status;
  uint8_t channel = TWO;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_clear_shift_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_clear_shift_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO clear shift interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common shift interrupt mask enable
 ******************************************************************************/
void test_sio_mask_shift_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO mask shift interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_mask_shift_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_mask_shift_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO mask shift interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the common shift interrupt mask clear
 ******************************************************************************/
void test_sio_unmask_shift_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO un-mask shift interrupt \n");
  sl_status_t status;
  uint8_t channel = THREE;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_unmask_shift_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_unmask_shift_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO un-mask shift interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the shift clock generator in uart
 ******************************************************************************/
void test_sio_uart_shift_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-UART shift clock \n");
  sl_status_t status;
  uint8_t channel = TWO;
  uint32_t divider;
  sl_sio_uart_config_t sio_uart_config;

  UnityPrintf("Testing with invalid channel and divider parameter \n");
  status = sl_si91x_sio_shift_clock(ZERO, CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");
  default_uart_config_struct(&sio_uart_config);

  divider = sio_uart_config.u32BaudRate;
  UnityPrintf("Testing with valid channel and divider parameter \n");
  status = sl_si91x_sio_shift_clock(divider, channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-UART shift clock test completed \n ");
}

/*******************************************************************************
 * Function to test the reverse load in uart
 ******************************************************************************/
void test_sio_uart_reverse_load(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-UART reverse load \n");
  sl_status_t status;
  uint8_t channel = TWO, reverse_load = ZERO;

  UnityPrintf("Testing with invalid channel and reverse load parameter \n");
  status = sl_si91x_sio_reverse_load(CHANNEL, TWO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and reverse load parameter \n");
  status = sl_si91x_sio_reverse_load(channel, reverse_load);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-UART reverse load test completed \n ");
}

/*******************************************************************************
 * Function to test the common swap interrupt enable in uart
 ******************************************************************************/
void test_sio_uart_set_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-UART set interrupt \n");
  sl_status_t status;
  uint8_t channel = TWO;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_set_interrupt(CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_set_interrupt(channel);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-UART set interrupt test completed \n ");
}

/*******************************************************************************
 * Function to test the data position counter in uart
 ******************************************************************************/
void test_sio_uart_pos_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO-UART position counter \n");
  sl_status_t status;
  uint8_t channel     = THREE;
  uint32_t data_shift = TEN;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_sio_position_counter(CHANNEL, ZERO);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_sio_position_counter(channel, data_shift);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO-UART position counter test completed \n ");
}

/*******************************************************************************
 * Function to test the SIO Buffer Read Write
 ******************************************************************************/
void test_sio_read_write_buffer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SIO Buffer Read Write \n");
  sl_status_t status;
  uint32_t data   = DATA, read_data;
  uint8_t channel = TWO;

  UnityPrintf("Testing write buffer with invalid channel parameter \n");
  status = sl_si91x_sio_write_buffer(CHANNEL, data);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed "
              "successfully \n");

  UnityPrintf("Testing with valid channel and data parameter \n");
  status = sl_si91x_sio_write_buffer(channel, data);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("Testing read buffer \n");
  read_data = sl_si91x_sio_read_buffer(channel);

  TEST_ASSERT_EQUAL_HEX(data, read_data);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("SIO Buffer Read Write test completed \n ");
}

/*******************************************************************************
 * Static function to update the SPI configuration structure with default values
 ******************************************************************************/
static void default_spi_config_struct(sl_sio_spi_config_t *config)
{
  config->u32SpiClockFrq = SIO_SPI_CLK_FREQUENCY; // SIO SPI clock frequency
  config->u8BitOrder     = SIO_SPI_MSB_LSB;       // SIO SPI MSB bit order
  config->u8SpiClkCh     = SIO_SPI_CLK_CH;        // SIO SPI channel clock
  config->u8SpiMosiCh    = SIO_SPI_MOSI_CH;       // SIO SPI MOSI
  config->u8SpiMisoCh    = SIO_SPI_MISO_CH;       // SIO SPI MISO
  config->u8SpiCsCh      = SIO_SPI_CS_CH;         // SIO SPI chip select
  config->u8BitLen       = SIO_SPI_BIT_LEN;       // SIO SPI bit length
  config->u8Mode         = SIO_SPI_MODE;          // SIO SPI mode
}

/*******************************************************************************
 * Static function to update the UART configuration structure with default
 *values
 ******************************************************************************/
static void default_uart_config_struct(sl_sio_uart_config_t *config)
{
  // UART configuration
  config->u32BaudRate        = SIO_UART_BAUD_RATE;  // SIO-UART baud rate
  config->u8Bitlen           = SIO_UART_BIT_LENGTH; // SIO-UART bit length
  config->u8Parity           = SIO_UART_PARITY;     // SIO_UART parity
  config->u8SioUartRxChannel = SIO_UART_RX_CHANNEL; // SIO_UART receive channel
  config->u8SioUartTxChannel = SIO_UART_TX_CHANNEL; // SIO_UART transmit channel
  config->u8StopBits         = SIO_UART_STOP_BIT;   // SIO_UART stop bit
  config->pfn                = sio_uart_callback;
}

/*******************************************************************************
 * Static function to update the SPI transfer configuration structure
 *    with default values
 ******************************************************************************/
static void default_spi_xfer_config_structure(sl_sio_spi_xfer_config_t *sio_spi_xfer_config)
{
  // SPI Transfer configuration
  sio_spi_xfer_config->rxBuff   = data_in;
  sio_spi_xfer_config->rxCount  = BUFFER_SIZE;
  sio_spi_xfer_config->sselNum  = ZERO;
  sio_spi_xfer_config->txBuff   = data_out;
  sio_spi_xfer_config->txCount  = BUFFER_SIZE;
  sio_spi_xfer_config->u8BitLen = SIO_SPI_BIT_LEN;
  sio_spi_xfer_config->u8Status = SioSpiIdle;
  sio_spi_xfer_config->pfnCb    = spi_user_callback;
}

/*******************************************************************************
 * SIO SPI Callback function triggered on data Transfer and reception
 ******************************************************************************/
void sio_spi_callback(en_sio_spi_events_t event)
{
  switch (event) {
    case SL_SIO_SPI_TX_DONE: // SIO SPI send complete
      send_comp = true;
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
      send_comp                        = true;
      gstcSioCb.uart_sio.u16UartTxDone = 0;
      break;
    case SL_SIO_UART_RX_DONE: // SIO UART receive complete event
      recv_comp = true;
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
