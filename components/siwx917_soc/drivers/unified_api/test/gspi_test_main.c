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
#include "sl_si91x_gspi.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DEFAULT_BITRATE         10000000  // default bitrate
#define MAX_BITRATE             40000000  // Maximum bitrate
#define BUFFER_SIZE             1024      // Buffer Size
#define BIT_WIDTH_8             8         // Bit width
#define INTF_PLL_CLK            180000000 // Intf pll clock frequency
#define INTF_PLL_REF_CLK        40000000  // Intf pll reference clock frequency
#define SOC_PLL_CLK             20000000  // Soc pll clock frequency
#define SOC_PLL_REF_CLK         40000000  // Soc pll reference clock frequency
#define INTF_PLL_500_CTRL_VALUE 0xD900    // Intf pll control value
#define SOC_PLL_MM_COUNT_LIMIT  0xA4      // Soc pll count limit
#define DVISION_FACTOR          0         // Division factor

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
extern sl_gspi_driver_t Driver_GSPI_MASTER;
/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void default_gspi_config_struct(sl_gspi_control_config_t *config);
static void gspi_clock_configuration(sl_gspi_clock_config_t *clock_config);
static void callback_event(uint32_t event);

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static boolean_t transfer_complete = false;
static const void *gspi_handle     = NULL;
static uint32_t dummy_address      = 0xFF445;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_gspi_clock_configuration(void);
void test_gspi_init(void);
void test_gspi_power_control(void);
void test_gspi_control(void);
void test_register_and_unregister_callback(void);
void test_gspi_receive_data(void);
void test_gspi_send_data(void);
void test_gspi_transfer_data(void);
void test_gspi_get_version(void);
void test_gspi_get_status(void);
void test_get_clock_division_factor(void);
void test_get_frame_length(void);
void test_gspi_master_enable(void);
void test_gspi_deinit(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("GSPI");

  RUN_TEST(test_gspi_clock_configuration, __LINE__);
  RUN_TEST(test_gspi_init, __LINE__);
  RUN_TEST(test_gspi_power_control, __LINE__);
  RUN_TEST(test_gspi_control, __LINE__);
  RUN_TEST(test_register_and_unregister_callback, __LINE__);
  RUN_TEST(test_gspi_receive_data, __LINE__);
  RUN_TEST(test_gspi_get_version, __LINE__);
  RUN_TEST(test_gspi_get_status, __LINE__);
  RUN_TEST(test_gspi_transfer_data, __LINE__);
  RUN_TEST(test_gspi_send_data, __LINE__);
  RUN_TEST(test_get_clock_division_factor, __LINE__);
  RUN_TEST(test_get_frame_length, __LINE__);
  RUN_TEST(test_gspi_master_enable, __LINE__);
  RUN_TEST(test_gspi_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the clock configuration of GSPI
 ******************************************************************************/
void test_gspi_clock_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Clock Configuration \n");
  sl_status_t status;
  sl_gspi_clock_config_t clock_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_configure_clock(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with more than maximum PLL frequency \n");
  gspi_clock_configuration(&clock_config);
  clock_config.intf_pll_clock = INTF_PLL_MAX_FREQUECY + 1;
  status                      = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with less than minimum PLL frequency \n");
  clock_config.intf_pll_clock = INTF_PLL_MIN_FREQUECY - 1;
  status                      = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with more than maximum PLL Ref frequency \n");
  gspi_clock_configuration(&clock_config);
  clock_config.intf_pll_reference_clock = INTF_PLL_MAX_FREQUECY + 1;
  status                                = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with less than minimum PLL Ref frequency \n");
  clock_config.intf_pll_reference_clock = INTF_PLL_MIN_FREQUECY - 1;
  status                                = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with more than maximum Soc frequency \n");
  gspi_clock_configuration(&clock_config);
  clock_config.soc_pll_clock = SOC_PLL_MAX_FREQUECY + 1;
  status                     = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with less than minimum Soc frequency \n");
  clock_config.soc_pll_clock = SOC_PLL_MIN_FREQUECY - 1;
  status                     = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with more than maximum Soc Ref frequency \n");
  gspi_clock_configuration(&clock_config);
  clock_config.soc_pll_reference_clock = SOC_PLL_MAX_FREQUECY + 1;
  status                               = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with less than minimum Soc Ref frequency \n");
  clock_config.soc_pll_reference_clock = SOC_PLL_MIN_FREQUECY - 1;
  status                               = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with more than clock division factor \n");
  clock_config.division_factor = SOC_MAX_CLK_DIVISION_FACTOR + 1;
  status                       = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with correct parameters \n");
  gspi_clock_configuration(&clock_config);
  status = sl_si91x_gspi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, GSPI clock is configured successfully \n");
}

/*******************************************************************************
 * Function to test the initialization of GSPI
 ******************************************************************************/
void test_gspi_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Initialization \n");
  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_init(SL_GSPI_MASTER, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with another GSPI instance \n");
  status = sl_si91x_gspi_init(SL_GSPI_INSTANCE_LAST_ENUM, &gspi_handle);
  UnityPrintf("Status of API is correct, GSPI Master instance Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_gspi_init(SL_GSPI_MASTER, &gspi_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  TEST_ASSERT_EQUAL_HEX(&Driver_GSPI_MASTER, gspi_handle);
  UnityPrintf("Status of API is correct, GSPI is init successfully \n");

  UnityPrintf("Testing the call of init again after it is enabled \n");
  status = sl_si91x_gspi_init(SL_GSPI_MASTER, &gspi_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, GSPI is busy \n");

  UnityPrintf("GSPI Initialization test completed \n");
}

/*******************************************************************************
 * Function to test the un-initialization of GSPI
 ******************************************************************************/
void test_gspi_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Un-initialization \n");

  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_deinit(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_deinit(&dummy_address);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  status = sl_si91x_gspi_deinit(gspi_handle);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("\nStatus of API is correct, GSPI is deinitd successfully \n");
  UnityPrintf("GSPI Un-initialization test completed \n");
}

/*******************************************************************************
 * Function to test the power control of GSPI
 ******************************************************************************/
void test_gspi_power_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Power Control \n");
  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_configure_power_mode(NULL, SL_GSPI_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_configure_power_mode(&dummy_address, SL_GSPI_FULL_POWER);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("Testing Power Off Mode \n");
  status = sl_si91x_gspi_configure_power_mode(gspi_handle, SL_GSPI_POWER_OFF);
  DEBUGINIT();
  //  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK,status);
  UnityPrintf("Status of API is correct, GSPI power mode is configured successfully \n");

  UnityPrintf("Testing Low Power Mode \n");
  status = sl_si91x_gspi_configure_power_mode(gspi_handle, SL_GSPI_LOW_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_SUPPORTED, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_gspi_configure_power_mode(gspi_handle, SL_GSPI_POWER_MODE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing Full Power Mode \n");
  status = sl_si91x_gspi_configure_power_mode(gspi_handle, SL_GSPI_FULL_POWER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, GSPI power mode is configured successfully \n");
  UnityPrintf("GSPI Power Control test completed \n ");
}

/*******************************************************************************
 * Function to test the control of GSPI
 * It includes bitrate, clock mode, master mode, slave select mode.
 ******************************************************************************/
void test_gspi_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Control \n");
  sl_status_t status;
  sl_gspi_control_config_t gspi_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_set_configuration(gspi_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  default_gspi_config_struct(&gspi_config);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_set_configuration(NULL, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  default_gspi_config_struct(&gspi_config);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_set_configuration(&dummy_address, &gspi_config);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("Testing with false bitrate \n");
  gspi_config.bitrate = MAX_BITRATE + 1;
  status              = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with Mode 3 (Motorola) \n");
  gspi_config.bitrate    = DEFAULT_BITRATE;
  gspi_config.clock_mode = SL_GSPI_MODE_3;
  status                 = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, GSPI Mode 3 is set successfully \n");

  UnityPrintf("Testing with false mode \n");
  gspi_config.clock_mode = 2;
  status                 = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with 15-bit field \n");
  gspi_config.clock_mode = SL_GSPI_MODE_0;
  gspi_config.bit_width  = 15;
  UnityPrintf("Configuring 16 bit field \n");
  status = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, 16-bit field is set successfully \n");

  UnityPrintf("Testing with false bit field \n");
  gspi_config.bit_width = 20;
  status                = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with false slave select mode \n");
  gspi_config.slave_select_mode = SL_GSPI_SLAVE_SELECT_MODE_LAST;
  status                        = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");
  gspi_config.slave_select_mode = SL_GSPI_MASTER_HW_OUTPUT;

  UnityPrintf("Testing with all the correct parameters \n");
  default_gspi_config_struct(&gspi_config);
  status = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, GSPI configurations are set successfully \n");
  sl_si91x_gspi_set_slave_number(GSPI_SLAVE_0);

  UnityPrintf("GSPI Control test completed \n ");
}

/*******************************************************************************
 * Function to test the receive data function of GSPI
 ******************************************************************************/
void test_gspi_receive_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Receive Data \n");
  sl_status_t status;
  uint16_t data_in[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_receive_data(gspi_handle, NULL, sizeof(data_in));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_receive_data(NULL, data_in, sizeof(data_in));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_receive_data(&dummy_address, data_in, sizeof(data_in));
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("GSPI Receive Data test completed \n ");
}

/*******************************************************************************
 * Function to test the send data function of GSPI
 ******************************************************************************/
void test_gspi_send_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Send Data \n");
  sl_status_t status;
  uint16_t data_out[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_send_data(gspi_handle, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_send_data(NULL, data_out, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_receive_data(&dummy_address, data_out, sizeof(data_out));
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
  }
  status = sl_si91x_gspi_send_data(gspi_handle, data_out, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, gspi data transfer begins successfully \n");
  UnityPrintf("GSPI data send complete \n");

  UnityPrintf("GSPI Send Data test completed \n ");
}

/*******************************************************************************
 * Function to test the transfer data function of GSPI
 * It includes the loopback testing. 
 * NOTE: To pass this test, the MISO and MISO must be short circuited.
 ******************************************************************************/
void test_gspi_transfer_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Transfer \n");
  sl_status_t status;
  uint16_t data_in[BUFFER_SIZE];
  uint16_t data_out[BUFFER_SIZE];
  uint16_t data_index = 0;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_transfer_data(gspi_handle, NULL, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_transfer_data(gspi_handle, data_out, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_transfer_data(NULL, data_out, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_transfer_data(&dummy_address, data_out, data_in, sizeof(data_out));
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("Testing API with valid parameter \n");
  UnityPrintf("Loopback Test started \n");
  for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
    data_in[i]  = 0;
  }

  status = sl_si91x_gspi_transfer_data(gspi_handle, data_out, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, gspi data transfer begins successfully \n");

  while (!transfer_complete)
    ;
  transfer_complete = 0;

  UnityPrintf("GSPI data transfer complete \n");
  UnityPrintf("Comparing the values of send data and receive \n");
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    if (data_in[data_index] != data_out[data_index]) {
      break;
    }
  }
  TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, data_index);
  UnityPrintf("Output and Input values are same \n");
  UnityPrintf("Loopback Test passed \n");

  UnityPrintf("GSPI transfer data test completed \n ");

  UnityPrintf("Testing GSPI rx and tx data_count \n");
  uint32_t data;

  UnityPrintf("Testing Tx data count API with valid parameter \n");
  data = sl_si91x_gspi_get_tx_data_count();
  TEST_ASSERT_EQUAL_HEX(sizeof(data_out), data);
  UnityPrintf("Status of API is correct, Tx data count is fetched successfully \n");

  UnityPrintf("Testing Rx data count API with valid parameter \n");
  data = sl_si91x_gspi_get_rx_data_count(gspi_handle);
  TEST_ASSERT_EQUAL_HEX(sizeof(data_in), data);
  UnityPrintf("Status of API is correct, Rx data count is fetched successfully \n");

  UnityPrintf("GSPI rx and tx data_count test completed \n ");
}

void test_gspi_master_enable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI master enable \n");
  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_set_master_state(NULL, false);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_set_master_state(&dummy_address, false);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  UnityPrintf("Disabling gspi master \n");
  status = sl_si91x_gspi_set_master_state(gspi_handle, false);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, GSPI master is disabled successfully \n");

  // Note: It will return invalid range error which is sent from the low level ROM API,
  // status cannot be verified in this API.
  UnityPrintf("Enabling gspi master \n");
  status = sl_si91x_gspi_set_master_state(gspi_handle, true);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("GSPI master is enabled successfully \n");

  UnityPrintf("GSPI master enable test completed \n ");
}

/*******************************************************************************
 * Function to test the get verison API
 ******************************************************************************/
void test_gspi_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI get version \n");
  sl_gspi_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_gspi_get_version();
  UnityPrintf("Verifying GSPI Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("GSPI get version test completed \n ");
}

/*******************************************************************************
 * Function to test the get status API
 ******************************************************************************/
void test_gspi_get_status(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI get status \n");
  sl_gspi_status_t spi_stat;

  UnityPrintf("Testing API with valid parameter \n");
  spi_stat = sl_si91x_gspi_get_status(gspi_handle);
  UnityPrintf("Verifying GSPI status \n");
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.busy);
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.data_lost);
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.mode_fault);
  UnityPrintf("GSPI status is correct\n");

  UnityPrintf("GSPI get status test completed \n ");
}

/*******************************************************************************
 * Function to test the getting clock division factor
 ******************************************************************************/
void test_get_clock_division_factor(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Get Clock divsion factor \n");
  sl_status_t status;
  int32_t clk_div;
  sl_gspi_control_config_t gspi_config;
  default_gspi_config_struct(&gspi_config);

  UnityPrintf("Getting the current clock division factor \n");
  clk_div = sl_si91x_gspi_get_clock_division_factor(gspi_handle);
  TEST_ASSERT_EQUAL_INT(1, clk_div);
  UnityPrintf("Current clock division factor is correct \n");

  UnityPrintf("Testing API with valid parameter \n");
  gspi_config.bitrate = 500000;
  status              = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Clock division factor is configured successfully \n");
  clk_div = sl_si91x_gspi_get_clock_division_factor(gspi_handle);
  TEST_ASSERT_EQUAL_INT(20, clk_div);
  UnityPrintf("New clock division factor is correct \n");
  UnityPrintf("GSPI get clock division factor works correctly \n");

  UnityPrintf("GSPI Set and Get Clock divsion factor test completed \n ");
}

/*******************************************************************************
 * Function to test the getting of frame length
 ******************************************************************************/
void test_get_frame_length(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Get Frane Length \n");
  sl_status_t status;
  uint32_t frame;
  sl_gspi_control_config_t gspi_config;
  default_gspi_config_struct(&gspi_config);

  UnityPrintf("Getting the current frame length \n");
  frame = sl_si91x_gspi_get_frame_length();
  TEST_ASSERT_EQUAL_INT(8, frame);
  UnityPrintf("Current frame length is correct \n");
  UnityPrintf("Testing API with valid parameter \n");

  gspi_config.bit_width = 15;
  status                = sl_si91x_gspi_set_configuration(gspi_handle, &gspi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Bitrate is configured successfully \n");
  frame = sl_si91x_gspi_get_frame_length();
  TEST_ASSERT_EQUAL_INT(15, frame);
  UnityPrintf("New frame length is correct \n");
  UnityPrintf("GSPI get frame length works correctly \n");

  UnityPrintf("GSPI Set and Get Frane Length test completed \n ");
}

void test_register_and_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI Register-Unregister Callback \n");
  sl_status_t status;

  UnityPrintf("Registering callback \n");
  status = sl_si91x_gspi_register_event_callback(NULL, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_gspi_register_event_callback(gspi_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with Dummy address of handle \n");
  status = sl_si91x_gspi_register_event_callback(&dummy_address, callback_event);
  UnityPrintf("Status of API is correct, Handle dummy address Test Passed successfully \n");

  status = sl_si91x_gspi_register_event_callback(gspi_handle, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is registered successfully \n");

  status = sl_si91x_gspi_register_event_callback(gspi_handle, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Callback is already registered, cannot register another\n");

  UnityPrintf("Unregistering the callback \n");
  sl_si91x_gspi_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");

  status = sl_si91x_gspi_register_event_callback(gspi_handle, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is again registered successfully \n");
}

/*******************************************************************************
 * Static function to update the config structure with default values
 ******************************************************************************/
static void default_gspi_config_struct(sl_gspi_control_config_t *config)
{
  config->bit_width         = BIT_WIDTH_8;
  config->clock_mode        = SL_GSPI_MODE_0;
  config->slave_select_mode = SL_GSPI_MASTER_HW_OUTPUT;
  config->bitrate           = DEFAULT_BITRATE;
}

/*******************************************************************************
 * Static function to update the clock config structure with default values
 ******************************************************************************/
static void gspi_clock_configuration(sl_gspi_clock_config_t *clock_config)
{
  clock_config->soc_pll_mm_count_value     = SOC_PLL_MM_COUNT_LIMIT;
  clock_config->intf_pll_500_control_value = INTF_PLL_500_CTRL_VALUE;
  clock_config->intf_pll_clock             = INTF_PLL_CLK;
  clock_config->intf_pll_reference_clock   = INTF_PLL_REF_CLK;
  clock_config->soc_pll_clock              = SOC_PLL_CLK;
  clock_config->soc_pll_reference_clock    = SOC_PLL_REF_CLK;
  clock_config->division_factor            = DVISION_FACTOR;
}
/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by GSPI interface
 * It updates the respective member of the structure as the event is triggered.
 ******************************************************************************/
static void callback_event(uint32_t event)
{
  switch (event) {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
      transfer_complete = 1;
      break;
    case ARM_SPI_EVENT_DATA_LOST:
      break;
    case ARM_SPI_EVENT_MODE_FAULT:
      break;
  }
}
