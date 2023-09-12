/***************************************************************************/ /**
 * @file
 * @brief SSI_MASTER Unity Test Functions
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

#include "sl_si91x_ssi.h"
#include "rsi_board.h"
#include "unity.h"
#include "sl_system_init.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DEFAULT_BITRATE         10000000
#define MAX_BITRATE             40000000
#define BIT_WIDTH_8             8
#define BIT_WIDTH_16            16
#define RELEASE_VERSION         0
#define MAJOR_VERSION           0
#define MINOR_VERSION           2
#define BUFFER_SIZE             10
#define DIVISION_FACTOR         0         ///< Division Factor
#define INTF_PLL_CLK            180000000 ///< PLL Clock frequency
#define INTF_PLL_REF_CLK        40000000  ///< PLL Ref Clock frequency
#define SOC_PLL_CLK             20000000  ///< SOC PLL Clock frequency
#define SOC_PLL_REF_CLK         40000000  ///< SOC PLL REFERENCE CLOCK frequency
#define INTF_PLL_500_CTRL_VALUE 0xD900    ///< Interface PLL control value
#define SOC_PLL_MM_COUNT_LIMIT  0xA4      ///< SOC PLL count limit

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
sl_ssi_handle_t ssi_driver_handle = NULL;
boolean_t transfer_complete       = false;
/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void default_ssi_config_struct(sl_ssi_control_config_t *config);
static sl_status_t init_clock_configuration_structure(sl_ssi_clock_config_t *clock_config);
static void callback_event_handler(uint32_t event);
/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_ssi_clock_configuration(void);
void test_ssi_master_init(void);
void test_ssi_master_uninitialize(void);
void test_ssi_set_power_mode(void);
void test_ssi_master_control(void);
void test_ssi_master_transfer(void);
void test_ssi_master_send(void);
void test_ssi_master_receive(void);
void test_ssi_master_get_data_count(void);
void test_ssi_get_version(void);
void test_ssi_master_get_status(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("SSI MASTER");

  RUN_TEST(test_ssi_clock_configuration, __LINE__);
  RUN_TEST(test_ssi_master_init, __LINE__);
  RUN_TEST(test_ssi_set_power_mode, __LINE__);
  RUN_TEST(test_ssi_master_control, __LINE__);
  RUN_TEST(test_ssi_master_transfer, __LINE__);
  RUN_TEST(test_ssi_master_send, __LINE__);
  RUN_TEST(test_ssi_master_receive, __LINE__);
  RUN_TEST(test_ssi_master_get_data_count, __LINE__);
  RUN_TEST(test_ssi_master_uninitialize, __LINE__);
  RUN_TEST(test_ssi_get_version, __LINE__);
  RUN_TEST(test_ssi_master_get_status, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the clock configuration of SSI
 ******************************************************************************/
void test_ssi_clock_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Clock Configuration\n");
  sl_status_t status;
  sl_ssi_clock_config_t clock_config;

  UnityPrintf("Testing with NULL pointer\n");
  status = sl_si91x_ssi_configure_clock(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully\n");

  UnityPrintf("Testing with less than minimum PLL frequency\n");
  init_clock_configuration_structure(&clock_config);
  clock_config.intf_pll_clock = INTF_PLL_MIN_FREQUECY - 1;
  status                      = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with more than maximum PLL frequency\n");
  clock_config.intf_pll_clock = INTF_PLL_MAX_FREQUECY + 1;
  status                      = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with more than maximum PLL Reference frequency\n");
  init_clock_configuration_structure(&clock_config);
  clock_config.intf_pll_reference_clock = INTF_PLL_MAX_FREQUECY + 1;
  status                                = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with less than minimum PLL Reference frequency\n");
  clock_config.intf_pll_reference_clock = INTF_PLL_MIN_FREQUECY - 1;
  status                                = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with more than maximum SOC frequency\n");
  init_clock_configuration_structure(&clock_config);
  clock_config.soc_pll_clock = SOC_PLL_MAX_FREQUECY + 1;
  status                     = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with less than minimum SOC frequency\n");
  clock_config.soc_pll_clock = SOC_PLL_MIN_FREQUECY - 1;
  status                     = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with more than maximum SOC Reference frequency\n");
  clock_config.soc_pll_reference_clock = SOC_PLL_MAX_FREQUECY + 1;
  status                               = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with more than clock division factor\n");
  clock_config.division_factor = SOC_MAX_CLK_DIVISION_FACTOR + 1;
  status                       = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with correct parameters\n");
  init_clock_configuration_structure(&clock_config);
  status = sl_si91x_ssi_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI clock is configured successfully\n");

  UnityPrintf("SSI Clock Configuration test completed\n");
}

/*******************************************************************************
 * Function to test the initialization of SSI
 ******************************************************************************/
void test_ssi_master_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Initialization\n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters\n");
  status = sl_si91x_ssi_init(SL_SSI_MASTER, &ssi_driver_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI initialization successful\n");

  UnityPrintf("SSI Initialization test completed\n");
}

/*******************************************************************************
 * Function to test the control of SSI
 ******************************************************************************/
void test_ssi_master_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Control \n");
  sl_status_t status;
  sl_ssi_control_config_t ssi_config;

  UnityPrintf("Testing with NULL pointer\n");
  status = sl_si91x_ssi_set_configuration(ssi_driver_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  default_ssi_config_struct(&ssi_config);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully\n");

  UnityPrintf("Testing with false bit-rate\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.baud_rate = MAX_BITRATE + 1;
  status               = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with false clock mode\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.clock_mode = SL_SSI_PERIPHERAL_MODE_LAST;
  status                = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with 16-bit field\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.bit_width = 10;
  UnityPrintf("Configuring 16 bit field\n");
  status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, 16-bit field is set successfully\n");

  UnityPrintf("Testing with false bit field\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.bit_width = 35;
  status               = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with master inactive mode\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.bit_width   = BIT_WIDTH_8;
  ssi_config.device_mode = 0;
  status                 = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, master in-activated successfully \n");

  UnityPrintf("Testing with false master mode\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.device_mode = 2;
  status                 = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing with master unused SSM\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.device_mode = 1;
  ssi_config.master_ssm  = 0;
  status                 = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, master unused is set successfully\n");

  UnityPrintf("Testing with false slave select mode\n");
  default_ssi_config_struct(&ssi_config);
  ssi_config.master_ssm = 3;
  status                = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");
  ssi_config.master_ssm = 2;

  UnityPrintf("Testing with all the correct parameters\n");
  default_ssi_config_struct(&ssi_config);
  status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &ssi_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI configurations are set successfully\n");
  sl_si91x_ssi_set_slave_number(SSI_SLAVE_0);

  UnityPrintf("SSI Control test completed \n ");
}

/*******************************************************************************
 * Function to test the data count function of SSI
 ******************************************************************************/
void test_ssi_master_get_data_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Get Data Count API\n");
  uint32_t data;

  UnityPrintf("Testing API with valid parameter\n");
  data = sl_si91x_ssi_get_rx_data_count(ssi_driver_handle);
  TEST_ASSERT_EQUAL_HEX(10, data);
  UnityPrintf("Status of API is correct, data count is fetched successfully\n");

  UnityPrintf("SSI Get Data Count API test completed\n ");
}

/*******************************************************************************
 * Function to test the receive data function of SSI
 ******************************************************************************/
void test_ssi_master_receive(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Receive Data\n");
  sl_status_t status;
  uint8_t data_in[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer\n");
  status = sl_si91x_ssi_receive_data(ssi_driver_handle, NULL, sizeof(data_in));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully\n");

  UnityPrintf("SSI Receive Data test completed\n ");
}

/*******************************************************************************
 * Function to test the send data function of SSI
 ******************************************************************************/
void test_ssi_master_send(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Send Data\n");
  sl_status_t status;
  uint8_t data_out[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer\n");
  status = sl_si91x_ssi_send_data(ssi_driver_handle, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully\n");

  UnityPrintf("SSI Send Data test completed \n ");
}

/*******************************************************************************
 * Function to test the transfer data function of SSI
 * It includes the loopback testing.
 * NOTE: To pass this test, the MISO and MISO must be short circuited.
 ******************************************************************************/
void test_ssi_master_transfer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Transfer\n");
  sl_status_t status;
  uint8_t data_in[BUFFER_SIZE];
  uint8_t data_out[BUFFER_SIZE];
  uint16_t data_index = 0;

  UnityPrintf("Testing with NULL pointer\n");
  status = sl_si91x_ssi_transfer_data(ssi_driver_handle, NULL, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully\n");

  UnityPrintf("Testing API with valid parameter\n");
  UnityPrintf("Loop back Test started\n");
  for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
    data_out[i] = i + 1;
  }
  status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, callback_event_handler);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  status = sl_si91x_ssi_transfer_data(ssi_driver_handle, data_out, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI data transfer begins successfully\n");

  while (!transfer_complete)
    ;
  transfer_complete = false;
  UnityPrintf("SSI data transfer complete\n");
  UnityPrintf("Comparing the values of send data and receive\n");
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
    DEBUGOUT("data : %d\n", data_in[data_index]);
    if (data_in[data_index] == data_out[data_index]) {
      continue;
    } else {
      break;
    }
  }
  TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, data_index);
  UnityPrintf("Output and Input values are same\n");
  UnityPrintf("Loop back Test passed\n");

  UnityPrintf("SSI transfer data test completed\n ");
}

/*******************************************************************************
 * Function to test the get version API
 ******************************************************************************/
void test_ssi_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI get version\n");
  sl_ssi_version_t version;

  UnityPrintf("Testing API with valid parameter\n");
  version = sl_si91x_ssi_get_version();
  UnityPrintf("Verifying SSI Driver and API version\n");
  TEST_ASSERT_EQUAL_INT(RELEASE_VERSION, version.release);
  TEST_ASSERT_EQUAL_INT(MAJOR_VERSION, version.major);
  TEST_ASSERT_EQUAL_INT(MINOR_VERSION, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("SSI get version test completed\n ");
}

/*******************************************************************************
 * Function to test the get status API
 ******************************************************************************/
void test_ssi_master_get_status(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI get status\n");
  sl_ssi_status_t spi_stat;

  UnityPrintf("Testing API with valid parameter\n");
  spi_stat = sl_si91x_ssi_get_status(ssi_driver_handle);
  UnityPrintf("Verifying SSI status\n");
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.busy);
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.data_lost);
  TEST_ASSERT_EQUAL_INT(DISABLE, spi_stat.mode_fault);
  UnityPrintf("SSI status is correct\n");

  UnityPrintf("SSI get status test completed\n ");
}

/*******************************************************************************
 * Function to test the un-initialization of SSI
 ******************************************************************************/
void test_ssi_master_uninitialize(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Un-initialization\n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters\n");
  status = sl_si91x_ssi_deinit(ssi_driver_handle);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("\nStatus of API is correct, SSI is de-init successfully\n");

  UnityPrintf("SSI Un-initialization test completed\n");
}

/*******************************************************************************
 * Function to test the power mode of SSI
 ******************************************************************************/
void test_ssi_set_power_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SSI Power mode\n");
  sl_status_t status;
  sl_ssi_power_state_t pow_config;

  UnityPrintf("Testing Power Off Mode\n");
  pow_config = ARM_POWER_OFF;
  status     = sl_si91x_ssi_configure_power_mode(ssi_driver_handle, pow_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI power mode is configured successfully\n");

  UnityPrintf("Testing with invalid parameters\n");
  pow_config = 3;
  status     = sl_si91x_ssi_configure_power_mode(ssi_driver_handle, pow_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error\n");

  UnityPrintf("Testing Full Power Mode\n");
  pow_config = ARM_POWER_FULL;
  status     = sl_si91x_ssi_configure_power_mode(ssi_driver_handle, pow_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, SSI power mode is configured successfully\n");

  UnityPrintf("SSI Power mode test completed\n ");
}

/*******************************************************************************
 * Static function to update the configure structure with default values
 ******************************************************************************/
static void default_ssi_config_struct(sl_ssi_control_config_t *config)
{
  config->bit_width   = BIT_WIDTH_8;
  config->device_mode = 1;
  config->clock_mode  = SL_SSI_PERIPHERAL_CPOL0_CPHA0;
  config->master_ssm  = SL_SSI_MASTER_HW_OUTPUT;
  config->baud_rate   = DEFAULT_BITRATE;
}

static sl_status_t init_clock_configuration_structure(sl_ssi_clock_config_t *clock_config)
{
  if (clock_config == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  clock_config->soc_pll_mm_count_value     = SOC_PLL_MM_COUNT_LIMIT;
  clock_config->intf_pll_500_control_value = INTF_PLL_500_CTRL_VALUE;
  clock_config->intf_pll_clock             = INTF_PLL_CLK;
  clock_config->intf_pll_reference_clock   = INTF_PLL_REF_CLK;
  clock_config->soc_pll_clock              = SOC_PLL_CLK;
  clock_config->soc_pll_reference_clock    = SOC_PLL_REF_CLK;
  clock_config->division_factor            = DIVISION_FACTOR;
  return SL_STATUS_OK;
}

static void callback_event_handler(uint32_t event)
{
  switch (event) {
    case SSI_EVENT_TRANSFER_COMPLETE:
      DEBUGOUT("\r\n SSI_EVENT_TRANSFER_COMPLETE \r\n");
      transfer_complete = true;
      break;

    case SSI_EVENT_DATA_LOST:
      /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. Occurs also in master mode
            when driver cannot transfer data fast enough. */
      DEBUGOUT("\r\n SSI_EVENT_DATA_LOST \r\n");
      break;

    case SSI_EVENT_MODE_FAULT:
      /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
      DEBUGOUT("\r\n SSI_EVENT_MODE_FAULT \r\n");
      break;
    default:
      DEBUGOUT("\r\n Unknown event %lu in callback function \r\n", event);
  }
}
