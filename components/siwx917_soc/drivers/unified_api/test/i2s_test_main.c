/***************************************************************************/ /**
 * @file gspi_test_main.c
 * @brief Calendar Unity Test Functions
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
#include "sl_si91x_i2s.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2S_INSTANCE0        0
#define I2S_INSTANCE1        1
#define I2S_INSTANCE_INVALID 2
#define BUFFER_SIZE          1024
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void test_i2s_init(void);
static void test_i2s_deinit(void);
static void test_i2s_configure_power_mode(void);
static void test_i2s_transmit_receive_config(void);
static void test_i2s_transmit_data(void);
static void test_i2s_receive_data(void);
static void test_i2s_register_unregister_event_callback(void);
static void test_i2s_transfer_end(void);
static void test_i2s_get_transmit_data_count(void);
static void test_i2s_get_receive_data_count(void);
static void test_i2s_get_version(void);
static void test_i2s_get_status(void);
static void callback_event(uint32_t event);
static sl_status_t compare_loop_back_data(void);
/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static const void *i2s_driver_handle = NULL;
static uint32_t dummy_address        = 0xFF445;
uint16_t data_in[BUFFER_SIZE];
uint16_t data_out[BUFFER_SIZE];
uint8_t send_complete    = 0;
uint8_t receive_complete = 0;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("I2S");
  // Filling the data out array with integer values
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i;
  }
  RUN_TEST(test_i2s_get_version, __LINE__);
  RUN_TEST(test_i2s_init, __LINE__);
  RUN_TEST(test_i2s_get_status, __LINE__);
  RUN_TEST(test_i2s_configure_power_mode, __LINE__);
  RUN_TEST(test_i2s_transmit_receive_config, __LINE__);
  RUN_TEST(test_i2s_register_unregister_event_callback, __LINE__);
  RUN_TEST(test_i2s_receive_data, __LINE__);
  RUN_TEST(test_i2s_transmit_data, __LINE__);
  RUN_TEST(test_i2s_get_transmit_data_count, __LINE__);
  RUN_TEST(test_i2s_get_receive_data_count, __LINE__);
  RUN_TEST(test_i2s_transfer_end, __LINE__);
  RUN_TEST(test_i2s_deinit, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

static void test_i2s_init(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Initialization \n");
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_i2s_init(I2S_INSTANCE0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid I2S instance \n");
  status = sl_si91x_i2s_init(I2S_INSTANCE_INVALID, &i2s_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid instance Test  success \n");
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_i2s_init(I2S_INSTANCE0, &i2s_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Initialization Test success \n");
  UnityPrintf("Testing double initialization \n");
  status = sl_si91x_i2s_init(I2S_INSTANCE0, &i2s_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Re-initialization test success \n");
}

static void test_i2s_deinit(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S De-initialization \n");
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_i2s_deinit(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success\n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_deinit(&dummy_address);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
#if 0
  UnityPrintf("Testing de-initialization\n");
  status = sl_si91x_i2s_deinit(i2s_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
#endif
}

static void test_i2s_configure_power_mode(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Power mode configuration \n");
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_i2s_configure_power_mode(NULL, SL_I2S_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_configure_power_mode(&dummy_address, SL_I2S_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Testing with invalid power state \n");
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, (SL_I2S_FULL_POWER + 1));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid power state test success\n");
  UnityPrintf("Testing with full power state \n");
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, full power state configuration success\n");
  UnityPrintf("Testing power off state configuration \n");
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_POWER_OFF);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, power off state configuration success\n");
  UnityPrintf("Reconfiguring full power state \n");
  status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, full power state reconfiguration success\n");
}

static void test_i2s_transmit_receive_config(void)
{
  sl_status_t status;
  sl_i2s_xfer_config_t xfer_config = { 0 };
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Transmit/Receive configuration \n");
  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_i2s_config_transmit_receive(NULL, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_config_transmit_receive(&dummy_address, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Testing with invalid I2S mode \n");
  xfer_config.mode = 200;
  status           = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid mode test success\n");
  UnityPrintf("Testing with invalid protocol \n");
  xfer_config.mode     = SL_I2S_MASTER;
  xfer_config.protocol = 0;
  status               = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid protocol test success\n");
  UnityPrintf("Testing with invalid I2S sync mode \n");
  xfer_config.protocol = SL_I2S_PROTOCOL;
  xfer_config.sync     = 200;
  status               = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid sync mode test success\n");
  UnityPrintf("Testing with invalid I2S data size \n");
  xfer_config.sync      = SL_I2S_ASYNC;
  xfer_config.data_size = 0;
  status                = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid data size test success\n");
  UnityPrintf("Testing with invalid I2S transfer type \n");
  xfer_config.data_size     = SL_I2S_DATA_SIZE16;
  xfer_config.transfer_type = 0;
  status                    = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid transfer type test success\n");
  UnityPrintf("Testing with invalid I2S resolution \n");
  xfer_config.transfer_type = SL_I2S_TRANSMIT;
  xfer_config.resolution    = 0;
  status                    = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid resolution test success\n");
  UnityPrintf("Testing with invalid I2S sampling rate \n");
  xfer_config.sampling_rate = 0;
  xfer_config.resolution    = SL_I2S_RESOLUTION_16;
  status                    = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid sampling rate test success\n");
  UnityPrintf("Testing transmit configuration \n");
  xfer_config.sampling_rate = 8000;
  xfer_config.transfer_type = SL_I2S_TRANSMIT;
  status                    = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Transmit configuration success\n");
  UnityPrintf("Testing receive configuration \n");
  xfer_config.transfer_type = SL_I2S_RECEIVE;
  status                    = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &xfer_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Receive configuration success\n");
}

static void test_i2s_transmit_data(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Transmit data \n");
  UnityPrintf("Testing with NULL pointer for i2s handle\n");
  status = sl_si91x_i2s_transmit_data(NULL, data_out, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with NULL pointer for data buffer\n");
  status = sl_si91x_i2s_transmit_data(i2s_driver_handle, NULL, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_transmit_data(&dummy_address, data_out, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Testing transmit enable for loopback \n");
  status = sl_si91x_i2s_transmit_data(i2s_driver_handle, data_out, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Transmit enable success\n");
  UnityPrintf("Waiting for loopback transfer\n");
  while (!((send_complete) && (receive_complete)))
    ;
  UnityPrintf("Data transfer success\n");
  UnityPrintf("Comparing tx and rx data\n");
  status = compare_loop_back_data();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Data comparison success\n");
}

static void test_i2s_receive_data(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Receive data \n");
  UnityPrintf("Testing with NULL pointer for i2s handle\n");
  status = sl_si91x_i2s_receive_data(NULL, data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with NULL pointer for data buffer\n");
  status = sl_si91x_i2s_receive_data(i2s_driver_handle, NULL, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_receive_data(&dummy_address, data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Testing Receive enable for loopback \n");
  status = sl_si91x_i2s_receive_data(i2s_driver_handle, data_in, BUFFER_SIZE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Receive enable success\n");
}

static void test_i2s_register_unregister_event_callback(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing I2S Register/Unregister callback \n");
  UnityPrintf("Testing with NULL pointer for i2s handle\n");
  status = sl_si91x_i2s_register_event_callback(NULL, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with NULL pointer for callback\n");
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_register_event_callback(&dummy_address, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Registering user callback\n");
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback registration success\n");
  UnityPrintf("Unregistering user callback\n");
  status = sl_si91x_i2s_unregister_event_callback(i2s_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback unregistered successfully\n");
  UnityPrintf("Registering user callback again\n");
  status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback registration success\n");
}

static void test_i2s_transfer_end(void)
{
  sl_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Testing transfer end \n");
  UnityPrintf("Testing with NULL pointer for i2s handle\n");
  status = sl_si91x_i2s_end_transfer(NULL, SL_I2S_SEND_ABORT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test success \n");
  UnityPrintf("Testing with invalid driver handle \n");
  status = sl_si91x_i2s_end_transfer(&dummy_address, SL_I2S_SEND_ABORT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid handle test success\n");
  UnityPrintf("Testing with invalid abort type \n");
  status = sl_si91x_i2s_end_transfer(i2s_driver_handle, 12);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid abort type test success\n");
  UnityPrintf("Testing send abort \n");
  status = sl_si91x_i2s_end_transfer(i2s_driver_handle, SL_I2S_SEND_ABORT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Send Abort \n");
}

static void test_i2s_get_transmit_data_count(void)
{
  uint32_t count;
  UnityPrintf("\n");
  UnityPrintf("Read Transmit data count \n");
  count = sl_si91x_i2s_get_transmit_data_count(i2s_driver_handle);
  TEST_ASSERT_EQUAL_UINT32(BUFFER_SIZE, count);
  UnityPrintf("Transmit data count = %d\n", count);
}

static void test_i2s_get_receive_data_count(void)
{
  uint32_t count;
  UnityPrintf("\n");
  UnityPrintf("Read Receive data count \n");
  count = sl_si91x_i2s_get_receive_data_count(i2s_driver_handle);
  TEST_ASSERT_EQUAL_UINT32(BUFFER_SIZE, count);
  UnityPrintf("Receive data count = %d\n", count);
}

static void test_i2s_get_version(void)
{
  sl_i2s_version_t version;
  UnityPrintf("\n");
  UnityPrintf("Read I2S driver version \n");
  version = sl_si91x_i2s_get_version();
  TEST_ASSERT_EQUAL_INT(1, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(0, version.minor);
  UnityPrintf("I2S driver version %d.%d.%d\n", version.release, version.major, version.minor);
}

static void test_i2s_get_status(void)
{
  sl_i2s_status_t status;
  UnityPrintf("\n");
  UnityPrintf("Read I2S driver status \n");
  status = sl_si91x_i2s_get_status(i2s_driver_handle);
  TEST_ASSERT_EQUAL_INT(0, status.frame_error);
  TEST_ASSERT_EQUAL_INT(0, status.rx_busy);
  TEST_ASSERT_EQUAL_INT(0, status.tx_busy);
  TEST_ASSERT_EQUAL_INT(0, status.rx_overflow);
  TEST_ASSERT_EQUAL_INT(0, status.tx_underflow);
  UnityPrintf("I2S frame error status %d\n", status.frame_error);
  UnityPrintf("I2S Rx status %d\n", status.rx_busy);
  UnityPrintf("I2S Tx status %d\n", status.tx_busy);
  UnityPrintf("I2S Rx overflow status %d\n", status.rx_overflow);
  UnityPrintf("I2S Tx underflow status %d\n", status.tx_underflow);
}

static sl_status_t compare_loop_back_data(void)
{
  sl_status_t status  = SL_STATUS_OK;
  uint16_t data_index = 0;
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    // If the data in and data out are same, it will be continued else, the for
    // loop will be break.
    if (data_in[data_index] != data_out[data_index]) {
      break;
    }
  }
  if (data_index != BUFFER_SIZE) {
    status = SL_STATUS_FAIL;
  }
  return status;
}

static void callback_event(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      send_complete = 1;
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      receive_complete = 1;
      break;
    case SL_I2S_TX_UNDERFLOW:
      break;
    case SL_I2S_RX_OVERFLOW:
      break;
    case SL_I2S_FRAME_ERROR:
      break;
  }
}
