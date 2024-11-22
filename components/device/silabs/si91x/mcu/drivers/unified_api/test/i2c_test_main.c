/***************************************************************************/ /**
 * @file i2c_test_main.c
 * @brief i2c Unity Test Functions
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
#include "sl_system_init.h"
#include "sl_si91x_i2c.h"
#include "unity.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define BUFFER_SIZE             1024  // Buffer Size
#define MAX_FOLLOWER_ADDR_10BIT 0x3FF // max 10-bit address
#define I2C_RESET_CON           0x6   // reset value of IC_CON register
#define OWN_I2C_ADDR            0x50
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void i2c_callback(sl_i2c_instance_t i2c_instance, uint32_t status);
static void i2c_dma_transfer_complete_callback(uint32_t channel, void *data);
static void i2c_dma_error_callback(uint32_t channel, void *data);
/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static boolean_t i2c_transfer_complete0 = false;
static boolean_t i2c_transfer_complete1 = false;
static boolean_t i2c_transfer_complete2 = false;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_i2c_driver_init(void);
void test_i2c_driver_configure_fifo_threshold(void);
void test_i2c_driver_receive_data_blocking(void);
void test_i2c_driver_send_data_blocking(void);
void test_i2c_driver_receive_data_non_blocking(void);
void test_i2c_driver_send_data_non_blocking(void);
void test_i2c_driver_transfer_data(void);
void test_i2c_driver_get_frequency(void);
void test_i2c_driver_set_follower_address(void);
void test_i2c_driver_deinit(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("I2C");

  RUN_TEST(test_i2c_driver_init, __LINE__);
  RUN_TEST(test_i2c_driver_configure_fifo_threshold, __LINE__);
  RUN_TEST(test_i2c_driver_receive_data_blocking, __LINE__);
  RUN_TEST(test_i2c_driver_send_data_blocking, __LINE__);
  RUN_TEST(test_i2c_driver_receive_data_non_blocking, __LINE__);
  RUN_TEST(test_i2c_driver_send_data_non_blocking, __LINE__);
  RUN_TEST(test_i2c_driver_transfer_data, __LINE__);
  RUN_TEST(test_i2c_driver_get_frequency, __LINE__);
  RUN_TEST(test_i2c_driver_deinit, __LINE__);
  RUN_TEST(test_i2c_driver_set_follower_address, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the initialization of I2C
 ******************************************************************************/
void test_i2c_driver_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Initialization \n");
  sl_i2c_status_t status;
  sl_i2c_config_t *i2c_handle1 = NULL;
  sl_i2c_config_t i2c_handle2;

  i2c_handle2.mode           = SL_I2C_LEADER_MODE;
  i2c_handle2.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_handle2.i2c_callback   = i2c_callback;
  i2c_handle2.transfer_type  = SL_I2C_USING_NON_DMA;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_init(SL_I2C0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, when Null Pointer passed \n");

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_init(SL_I2C_LAST, &i2c_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  i2c_handle2.mode = 3;
  UnityPrintf("Testing with wrong I2C mode \n");
  status = sl_i2c_driver_init(SL_I2C0, &i2c_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  i2c_handle2.mode           = SL_I2C_LEADER_MODE;
  i2c_handle2.operating_mode = SL_I2C_OPERATING_MODE_LAST;
  UnityPrintf("Testing with wrong I2C operating mode \n");
  status = sl_i2c_driver_init(SL_I2C0, &i2c_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong mode \n");

  i2c_handle2.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_handle2.transfer_type  = SL_I2C_TRANFER_TYPE_LAST;
  UnityPrintf("Testing with wrong I2C transfer type \n");
  status = sl_i2c_driver_init(SL_I2C0, &i2c_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong operating mode\n");

  i2c_handle2.transfer_type = SL_I2C_USING_NON_DMA;
  UnityPrintf("Testing with correct parameters \n");
  status = sl_i2c_driver_init(SL_I2C0, &i2c_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, with correct parameters \n");

  UnityPrintf("Testing the call of init again after it is enabled \n");
  status = sl_i2c_driver_init(SL_I2C0, &i2c_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_CALLBACK_BUSY, status);
  UnityPrintf("Status of API is correct, when I2C callback is already registered \n");

  UnityPrintf("I2C Initialization test completed \n");
}

/*******************************************************************************
 * Function to test I2C configure fifo threshold
 ******************************************************************************/
void test_i2c_driver_configure_fifo_threshold(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C configure fifo threshold \n");
  sl_i2c_status_t status;
  uint8_t tx_threshold_value = 0;
  uint8_t rx_threshold_value = 0;

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_configure_fifo_threshold(SL_I2C_LAST, tx_threshold_value, rx_threshold_value);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance  \n");

  UnityPrintf("Testing with correct parameters\n");
  status = sl_i2c_driver_configure_fifo_threshold(SL_I2C0, tx_threshold_value, rx_threshold_value);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, with correct parameters \n");

  TEST_ASSERT_EQUAL_HEX(I2C0->IC_TX_TL_b.TX_TL, tx_threshold_value);
  UnityPrintf("tx threshold value is assigned properly \n");
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_RX_TL_b.RX_TL, rx_threshold_value);
  UnityPrintf("rx threshold value is assigned properly \n");

  uint8_t threshold = 0;
  UnityPrintf("Testing all possible values for TX threshold \n");
  for (threshold = 0; threshold < 8; threshold++) {
    sl_i2c_driver_configure_fifo_threshold(SL_I2C0, threshold, rx_threshold_value);
    TEST_ASSERT_EQUAL_HEX(threshold, I2C0->IC_TX_TL_b.TX_TL);
  }
  UnityPrintf("All possible values for TX threshold are verified \n");

  UnityPrintf("Testing all possible values for RX threshold \n");
  for (threshold = 0; threshold < 8; threshold++) {
    sl_i2c_driver_configure_fifo_threshold(SL_I2C0, tx_threshold_value, threshold);
    TEST_ASSERT_EQUAL_HEX(threshold, I2C0->IC_RX_TL_b.RX_TL);
  }
  UnityPrintf("All possible values for RX threshold are verified \n");
  UnityPrintf("I2C configure fifo threshold test completed \n");
}

/*******************************************************************************
  * Function to test the receive data function of I2C
  ******************************************************************************/
void test_i2c_driver_receive_data_blocking(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Receive Data blocking API \n");
  sl_i2c_status_t status;
  uint8_t data_in[BUFFER_SIZE];
  uint16_t address = 0X50;

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_receive_data_blocking(SL_I2C_LAST, address, data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance  \n");

  UnityPrintf("Testing with wrong I2C follower address \n");
  status = sl_i2c_driver_receive_data_blocking(SL_I2C_LAST, (MAX_FOLLOWER_ADDR_10BIT + 1), data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong greater than maximum 10-bit address \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_receive_data_blocking(SL_I2C0, address, NULL, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_i2c_driver_receive_data_blocking(SL_I2C0, address, data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, with correct parameters \n");
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_TAR, (address & 0x3FF));
  UnityPrintf("Follower address is configured properly \n");

  TEST_ASSERT_EQUAL_HEX(I2C0->IC_INTR_MASK, SL_I2C_EVENT_RECEIVE_FULL);
  UnityPrintf("Receive full interrupt flag is masked properly \n");

  UnityPrintf("I2C Receive Data blocking test completed \n ");
}

/*******************************************************************************
  * Function to test the send data function of GSPI
  ******************************************************************************/
void test_i2c_driver_send_data_blocking(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Send Data blocking API\n");
  sl_i2c_status_t status;
  uint8_t data_out[BUFFER_SIZE];
  uint16_t address = 0X50;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_send_data_blocking(SL_I2C0, address, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_send_data_blocking(SL_I2C_LAST, address, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with wrong I2C follower address \n");
  status = sl_i2c_driver_send_data_blocking(SL_I2C_LAST, (MAX_FOLLOWER_ADDR_10BIT + 1), data_out, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong greater than maximum 10-bit address \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_i2c_driver_send_data_blocking(SL_I2C0, address, data_out, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_TAR, (address & 0x3FF));
  UnityPrintf("Follower address is configured properly \n");
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_INTR_MASK, (I2C0->IC_INTR_MASK | SL_I2C_EVENT_TRANSMIT_EMPTY));
  UnityPrintf("Transmit empty interrupt flag is masked properly \n");

  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
  }
  status = sl_i2c_driver_send_data_blocking(SL_I2C0, address, data_out, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, i2c data sending begins successfully \n");
  UnityPrintf("I2C data send complete \n");

  UnityPrintf("I2C Send Data blocking test completed \n ");
}

/*******************************************************************************
  * Function to test the transfer data function of GSPI
  * It includes the loopback testing.
  * NOTE: To pass this test, the MISO and MISO must be short circuited.
  ******************************************************************************/
void test_i2c_driver_transfer_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Transfer data API \n");
  sl_i2c_status_t status;
  uint8_t data_in[BUFFER_SIZE];
  uint8_t data_out[BUFFER_SIZE];
  uint16_t address = 0X50;
  sl_i2c_transfer_config_t i2c_transfer_handle;
  i2c_transfer_handle.rx_buffer = data_in;
  i2c_transfer_handle.tx_buffer = data_out;
  i2c_transfer_handle.rx_len    = BUFFER_SIZE;
  i2c_transfer_handle.tx_len    = BUFFER_SIZE;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_transfer_data(SL_I2C0, NULL, address);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  i2c_transfer_handle.rx_buffer = NULL;
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_transfer_data(SL_I2C0, &i2c_transfer_handle, address);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  i2c_transfer_handle.tx_buffer = NULL;
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_transfer_data(SL_I2C0, &i2c_transfer_handle, address);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  i2c_transfer_handle.rx_buffer = data_in;
  i2c_transfer_handle.tx_buffer = data_out;
  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_transfer_data(SL_I2C_LAST, &i2c_transfer_handle, address);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with wrong I2C follower address \n");
  status = sl_i2c_driver_transfer_data(SL_I2C_LAST, &i2c_transfer_handle, (MAX_FOLLOWER_ADDR_10BIT + 1));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong greater than maximum 10-bit address \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_i2c_driver_transfer_data(SL_I2C0, &i2c_transfer_handle, address);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("Testing API with valid parameter \n");
  UnityPrintf("Loopback Test started \n");
  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
    data_in[i]  = 0;
  }

  status = sl_i2c_driver_transfer_data(SL_I2C0, &i2c_transfer_handle, address);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, i2c data transfer begins successfully \n");

  UnityPrintf("I2C transfer data test completed \n ");
}

/*******************************************************************************
  * Function to test the de-initialization of I2C
  ******************************************************************************/
void test_i2c_driver_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C De-initialization \n");
  sl_i2c_status_t status;

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_deinit(SL_I2C_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_i2c_driver_deinit(SL_I2C0);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("\nStatus of API is correct, with correct parameter \n");
  // Checking I2C0 instance register values cleared or not
  TEST_ASSERT_EQUAL_HEX(0x00, I2C0->IC_CON);
  TEST_ASSERT_EQUAL_HEX(0x00, I2C0->IC_RAW_INTR_STAT);
  TEST_ASSERT_EQUAL_HEX(0x00, I2C0->IC_TAR);
  TEST_ASSERT_EQUAL_HEX(0x00, I2C0->IC_SAR);
  UnityPrintf("\n I2C control, interrupt , TAR and SAR registers set to zero, I2C is deinitd successfully \n");
  UnityPrintf("I2C De-initialization test completed \n");
}

/*******************************************************************************
  * Function to test Receive Data blocking API
  ******************************************************************************/
void test_i2c_driver_receive_data_non_blocking(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Receive Data blocking API \n");
  sl_i2c_status_t status;
  uint8_t data_in[BUFFER_SIZE];
  uint16_t address = 0X50;

  sl_i2c_dma_config_t p_dma_config;
  p_dma_config.dma_rx_channel = 30;
  p_dma_config.dma_tx_channel = 31;

  sl_i2c_config_t i2c_init_params;
  i2c_init_params.mode           = SL_I2C_LEADER_MODE;
  i2c_init_params.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_init_params.i2c_callback   = i2c_callback;
  i2c_init_params.transfer_type  = SL_I2C_USING_DMA;

  // Initializing i2c
  status = sl_i2c_driver_init(SL_I2C0, &i2c_init_params);
  // testing for NULL pointers
  UnityPrintf("Testing with DMA config NULL pointer parameter \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C0, address, data_in, BUFFER_SIZE, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  // testing for NULL pointers
  UnityPrintf("Testing with rx-buffer NULL pointer \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C0, address, NULL, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  // testing for wrong instance
  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C_LAST, address, data_in, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with wrong I2C follower address \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C0,
                                                   (MAX_FOLLOWER_ADDR_10BIT + 1),
                                                   data_in,
                                                   BUFFER_SIZE,
                                                   &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong greater than maximum 10-bit address \n");

  UnityPrintf("Testing with correct parameter \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C0, address, data_in, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_TAR, (address & 0x3FF));
  UnityPrintf("Follower address is configured properly \n");
  UnityPrintf("I2C Receive Data non-blocking test completed \n ");
}

/*******************************************************************************
  * Function to test Receive Data blocking API
  ******************************************************************************/
void test_i2c_driver_send_data_non_blocking(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Receive Data blocking API \n");
  sl_i2c_status_t status;
  uint8_t data_out[BUFFER_SIZE];
  uint16_t address = 0X50;

  sl_i2c_dma_config_t p_dma_config;
  p_dma_config.dma_rx_channel = 30;
  p_dma_config.dma_tx_channel = 31;

  // testing for NULL pointers
  UnityPrintf("Testing with DMA config NULL pointer parameter \n");
  status = sl_i2c_driver_send_data_non_blocking(SL_I2C0, address, data_out, BUFFER_SIZE, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  // testing for NULL pointers
  UnityPrintf("Testing with rx-buffer NULL pointer \n");
  status = sl_i2c_driver_send_data_non_blocking(SL_I2C0, address, NULL, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  // testing for wrong instance
  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_send_data_non_blocking(SL_I2C_LAST, address, data_out, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with invalid I2C follower address \n");
  status = sl_i2c_driver_receive_data_non_blocking(SL_I2C0,
                                                   (MAX_FOLLOWER_ADDR_10BIT + 1),
                                                   data_out,
                                                   BUFFER_SIZE,
                                                   &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong greater than maximum 10-bit address \n");

  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
  }
  UnityPrintf("Testing with correct parameter \n");
  status = sl_i2c_driver_send_data_non_blocking(SL_I2C0, address, data_out, BUFFER_SIZE, &p_dma_config);
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_TAR, (address & 0x3FF));
  UnityPrintf("Follower address is configured properly \n");
  UnityPrintf("I2C Receive Data non-blocking test completed \n ");
}

/*******************************************************************************
  * Function to test get frequency API
  ******************************************************************************/
void test_i2c_driver_get_frequency(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C get frequency API \n");
  sl_i2c_status_t status;
  uint32_t frequency;

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_get_frequency(SL_I2C_LAST, &frequency);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_i2c_driver_get_frequency(SL_I2C_LAST, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("I2C get frequency test completed \n ");
}

/*******************************************************************************
  * Function to test set follower address API
  ******************************************************************************/
void test_i2c_driver_set_follower_address(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C set follower mask address \n");
  sl_i2c_status_t status;
  sl_i2c_config_t i2c_handle;

  i2c_handle.mode           = SL_I2C_FOLLOWER_MODE;
  i2c_handle.operating_mode = SL_I2C_STANDARD_MODE;
  i2c_handle.i2c_callback   = i2c_callback;
  i2c_handle.transfer_type  = SL_I2C_USING_NON_DMA;

  // Initializing as follower
  sl_i2c_driver_init(SL_I2C0, &i2c_handle);

  UnityPrintf("Testing with wrong I2C instance \n");
  status = sl_i2c_driver_set_follower_address(SL_I2C_LAST, OWN_I2C_ADDR);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with invalid I2C address \n");
  status = sl_i2c_driver_set_follower_address(SL_I2C_LAST, (MAX_FOLLOWER_ADDR_10BIT + 1));
  TEST_ASSERT_EQUAL_HEX(SL_I2C_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, with wrong instance \n");

  UnityPrintf("Testing with correct parameter \n");
  status = sl_i2c_driver_set_follower_address(SL_I2C0, OWN_I2C_ADDR);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_SUCCESS, status);
  UnityPrintf("Status of API is correct, with correct parameters \n");
  TEST_ASSERT_EQUAL_HEX(I2C0->IC_SAR, ((OWN_I2C_ADDR & 0x3FF) & 0x03FF));
  UnityPrintf("Follower mask address is configured properly \n");
  // De-initializing I2C
  sl_i2c_driver_deinit(SL_I2C0);
}
/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by GSPI interface
 * It updates the respective member of the structure as the event is triggered.
 ******************************************************************************/
static void i2c_callback(sl_i2c_instance_t i2c_instance, uint32_t status)
{
  switch (status) {
    case SL_I2C_DATA_TRANSFER_COMPLETE:
      if (i2c_instance = SL_I2C0) {
        i2c_transfer_complete0 = true;
      }
      if (i2c_instance = SL_I2C1) {
        i2c_transfer_complete1 = true;
      }
      if (i2c_instance = SL_ULP_I2C) {
        i2c_transfer_complete2 = true;
      }
      break;
    default:
      break;
  }
}

/*******************************************************************************
 I2C DMA transfer callback function
 ******************************************************************************/
static void i2c_dma_transfer_complete_callback(uint32_t channel, void *data)
{
  uint32_t status;
  status = SL_I2C_DATA_TRANSFER_COMPLETE;
  // Calling leader callback
  i2c_callback(SL_I2C0, status);
}

/*******************************************************************************
 I2C DMA error callback function
 ******************************************************************************/
static void i2c_dma_error_callback(uint32_t channel, void *data)
{
  uint32_t status;
  status = SL_I2C_DMA_TRANSFER_ERROR;
  // Calling leader callback
  i2c_callback(SL_I2C0, status);
}
