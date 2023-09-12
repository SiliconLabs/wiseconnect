/***************************************************************************/ /**
 * @file adc_test_main.c
 * @brief ADC Unity Test Functions
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
#include "sl_si91x_adc.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PS4_SOC_FREQ           180000000 /*<! PLL out clock 180MHz            */
#define SOC_PLL_REF_FREQUENCY  32000000  /*<! PLL input REFERENCE clock 32MHZ */
#define DVISION_FACTOR         0         // Division factor
#define CHANNEL0_SAMPLE_LENGTH 1023      // Number of ADC sample collect for operation
#define NUMBER_OF_CHA_EN       1         // Single channel data acquisition
#define CHANNEL0_SAMPLING_RATE \
  100000                    // Number of samples to collect Minimum is 100ksps(1000) and Maximum is 2.5msps (2500000)
#define ADC_POS_INPUT_SEL 5 // ADC positive input channel
#define ADC_NEG_INPUT_SEL 5 // ADC Negative input channel (for single ended this value is not used)
#define OPEN_GAIN         2
#define ADC_MAX_OP_VALUE  4096  // Maximum output value get from adc data register
#define MAX_WORD_LENGTH   65536 //Maximun output value of 16 bit data
#define M4SS_MEM
#ifdef ULPSS_MEM
#define adc_ping_buffer 0x24060800
#define adc_pong_buffer (0x24060800 + CHANNEL0_SAMPLE_LENGTH)
#endif
#ifdef M4SS_MEM
#define adc_ping_buffer 0x0000F000
#define adc_pong_buffer (0x0000F000 + CHANNEL0_SAMPLE_LENGTH)
#endif

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void default_adc_config_struct(sl_adc_config_t *adc_config);
static void default_adc_channel_config_struct(sl_adc_channel_config_t *adc_channel_config);
static void default_adc_external_trigger_struct(sl_adc_external_config_t *adc_external_trigger);
static void default_adc_internal_struct(sl_adc_internal_config_t *adc_internal_config);
static void default_adc_fifo_threshold_struct(sl_adc_fifo_thrld_config_t *adc_fifo_threshold);
static void adc_clock_configuration(sl_adc_clock_config_t *clock_config);
static void callback_event(uint8_t channel_no, uint8_t event);

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static sl_adc_channel_config_t adc_channel_config;
static sl_adc_external_config_t adc_external_trigger;
static sl_adc_internal_config_t adc_internal_config;
static sl_adc_fifo_thrld_config_t adc_fifo_threshold;
static sl_adc_config_t adc_config;
volatile uint8_t chnl0_complete_flag = 0;
int16_t adc_output_ch0[CHANNEL0_SAMPLE_LENGTH];

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_adc_clock_configuration(void);
void test_adc_init(void);
void test_adc_power_control(void);
void test_adc_reference_voltage_configure(void);
void test_adc_control(void);
void test_adc_external_trigger_configuration(void);
void test_adc_channel_sampling_rate(void);
void test_adc_external_trigger_status(void);
void test_adc_external_trigger_clear(void);
void test_adc_ping_pong_memory_address_configuration(void);
void test_adc_ping_pong_enable(void);
void test_adc_ping_pong_disable(void);
void test_adc_internal_per_channel_dma_enable(void);
void test_adc_internal_per_channel_dma_disable(void);
void test_adc_configure_static_mode(void);
void test_adc_configure_fifo_mode(void);
void test_adc_channel_enable(void);
void test_adc_channel_disable(void);
void test_adc_noise_average_mode(void);
void test_adc_get_version(void);
void test_adc_fifo_threshold_configuration(void);
void test_adc_threshold_configuration(void);
void test_adc_read_data(void);
void test_adc_read_data_static(void);
void test_adc_start(void);
void test_adc_stop(void);
void test_adc_deinit(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("ADC");

  RUN_TEST(test_adc_clock_configuration, __LINE__);
  RUN_TEST(test_adc_init, __LINE__);
  RUN_TEST(test_adc_power_control, __LINE__);
  RUN_TEST(test_adc_reference_voltage_configure, __LINE__);
  RUN_TEST(test_adc_control, __LINE__);
  RUN_TEST(test_adc_noise_average_mode, __LINE__);
  RUN_TEST(test_adc_configure_static_mode, __LINE__);
  RUN_TEST(test_adc_configure_fifo_mode, __LINE__);
  RUN_TEST(test_adc_channel_sampling_rate, __LINE__);
  RUN_TEST(test_adc_channel_enable, __LINE__);
  RUN_TEST(test_adc_internal_per_channel_dma_enable, __LINE__);
  RUN_TEST(test_adc_ping_pong_memory_address_configuration, __LINE__);
  RUN_TEST(test_adc_ping_pong_enable, __LINE__);
  RUN_TEST(test_adc_external_trigger_configuration, __LINE__);
  RUN_TEST(test_adc_external_trigger_status, __LINE__);
  RUN_TEST(test_adc_external_trigger_clear, __LINE__);
  RUN_TEST(test_adc_start, __LINE__);
  RUN_TEST(test_adc_fifo_threshold_configuration, __LINE__);
  RUN_TEST(test_adc_threshold_configuration, __LINE__);
  RUN_TEST(test_adc_read_data, __LINE__);
  RUN_TEST(test_adc_read_data_static, __LINE__);
  RUN_TEST(test_adc_get_version, __LINE__);
  RUN_TEST(test_adc_ping_pong_disable, __LINE__);
  RUN_TEST(test_adc_internal_per_channel_dma_disable, __LINE__);
  RUN_TEST(test_adc_channel_disable, __LINE__);
  RUN_TEST(test_adc_stop, __LINE__);
  RUN_TEST(test_adc_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the clock configuration of ADC
 ******************************************************************************/
void test_adc_clock_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC Clock Configuration \n");
  sl_status_t status;
  sl_adc_clock_config_t clock_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_adc_configure_clock(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  adc_clock_configuration(&clock_config);
  status = sl_si91x_adc_configure_clock(&clock_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC clock is configured successfully \n");
}

/*******************************************************************************
 * Function to test the initialization of ADC
 ******************************************************************************/
void test_adc_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC Initialization \n");
  sl_status_t status;

  UnityPrintf("Testing with event NULL pointer \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_init(adc_channel_config, adc_config, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, adc event Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters for Static mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_config.operation_mode            = SL_ADC_STATIC_MODE;
  adc_channel_config.num_of_samples[0] = 1;
  status                               = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC Init successfully \n");

  UnityPrintf("Testing with correct parameters for FIFO mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  sl_si91x_adc_deinit(adc_config);
  status = sl_si91x_adc_init(adc_channel_config, adc_config, callback_event);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC Init successfully \n");

  UnityPrintf("ADC Initialization test completed \n");
}

/*******************************************************************************
 * Function to test the power control of ADC
 ******************************************************************************/
void test_adc_power_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC Power Control \n");
  sl_status_t status;

  UnityPrintf("Testing Power Off Mode \n");
  status = sl_si91x_adc_power_control(SL_ADC_POWER_OFF);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC power off mode is configured successfully \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_adc_power_control(SL_ADC_POWER_MODE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing Power ON Mode \n");
  status = sl_si91x_adc_power_control(SL_ADC_POWER_ON);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC power on mode is configured successfully \n");

  UnityPrintf("ADC Power Control test completed \n ");
}

/*******************************************************************************
 * Function to test the ADC channel configure.
 ******************************************************************************/
void test_adc_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC Control \n");
  sl_status_t status;

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters for Static mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  adc_config.operation_mode            = SL_ADC_STATIC_MODE;
  adc_channel_config.num_of_samples[0] = 1;
  status                               = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC Init successfully \n");

  UnityPrintf("Testing with correct parameters for FIFO mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_channel_set_configuration(adc_channel_config, adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC channel configurations are set successfully \n");

  UnityPrintf("ADC Control test completed \n ");
}

/*******************************************************************************
 * Function to test the reference voltage configure to ADC
 ******************************************************************************/
void test_adc_reference_voltage_configure(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC Reference voltage \n");
  sl_status_t status;
  volatile float battery_status = 0;
  float vref_value              = 1.7;

  UnityPrintf("Testing with ADC minimum reference voltage configure  \n");
  battery_status = sl_si91x_adc_get_chip_voltage();
  status         = sl_si91x_adc_configure_reference_voltage(vref_value, battery_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, ADC minimum reference voltage Test Passed successfully \n");

  UnityPrintf("Testing with ADC maximum reference voltage configure  \n");
  vref_value = 3.7;
  status     = sl_si91x_adc_configure_reference_voltage(vref_value, battery_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, ADC maximum reference voltage Test Passed successfully \n");

  UnityPrintf("Testing with ADC reference voltage configure  \n");
  vref_value = 3.3;
  status     = sl_si91x_adc_configure_reference_voltage(vref_value, battery_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC reference voltage configure Test Passed successfully \n");

  UnityPrintf("ADC reference voltage test completed \n ");
}

/*******************************************************************************
 * Function to test the ADC external trigger configure.
 ******************************************************************************/
void test_adc_external_trigger_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC external trigger \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid adc_external_trigger_type \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_type = SL_ADC_EXT_TRIGGER_TYPE_LAST;
  status                            = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_num = SL_ADC_EXT_TRIGGER_LAST;
  status                           = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_channel_id \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel_val = 16;
  status                               = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_channel_id Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection_edge \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.detection_edge_sel = SL_ADC_EXT_TRIGGER_EDGE_LAST;
  status                                  = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection_edge Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_selection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel = SL_ADC_EXT_TRIGGER_SEL_LAST;
  status                           = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_selection Test Passed successfully \n");

  UnityPrintf("Testing with all the correct parameters \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  status = sl_si91x_adc_configure_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC channel configurations are set successfully \n");

  UnityPrintf("ADC external trigger test completed \n ");
}

/*******************************************************************************
 * Function to test the ADC external trigger status reading
 ******************************************************************************/
void test_adc_external_trigger_status(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing to read ADC external trigger status \n");
  sl_status_t status;
  uint8_t ext_trigger_status;

  UnityPrintf("Testing with invalid adc_external_trigger_type \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_type = SL_ADC_EXT_TRIGGER_TYPE_LAST;
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_num = SL_ADC_EXT_TRIGGER_LAST;
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_channel_id \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel_val = 16;
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_channel_id Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection_edge \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.detection_edge_sel = SL_ADC_EXT_TRIGGER_EDGE_LAST;
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection_edge Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_selection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel = SL_ADC_EXT_TRIGGER_SEL_LAST;
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_selection Test Passed successfully \n");

  UnityPrintf("Testing to read ext_trigger_status \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  status = sl_si91x_adc_get_external_trigger_status(adc_external_trigger, &ext_trigger_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ext_trigger_status Test Passed successfully \n");
  UnityPrintf("ADC external trigger status = %d \n", ext_trigger_status);

  UnityPrintf("ADC external trigger status test completed \n ");
}

/*******************************************************************************
 * Function to test the ADC external trigger clear.
 ******************************************************************************/
void test_adc_external_trigger_clear(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC external trigger clear \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid adc_external_trigger_type \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_type = SL_ADC_EXT_TRIGGER_TYPE_LAST;
  status                            = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_num = 5;
  status                           = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_channel_id \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel_val = 16;
  status                               = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_channel_id Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_detection_edge \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.detection_edge_sel = 4;
  status                                  = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_detection_edge Test Passed successfully \n");

  UnityPrintf("Testing with invalid adc_external_trigger_selection \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  adc_external_trigger.trigger_sel = 5;
  status                           = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, adc_external_trigger_selection Test Passed successfully \n");

  UnityPrintf("Testing with correct parameter \n");
  default_adc_external_trigger_struct(&adc_external_trigger);
  status = sl_si91x_adc_clear_external_trigger(adc_external_trigger);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC external trigger status cleared successfully \n");

  UnityPrintf("ADC external trigger clear test completed \n ");
}

/*******************************************************************************
 * Function to test the get version API
 ******************************************************************************/
void test_adc_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC get version \n");
  sl_adc_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_adc_get_version();
  UnityPrintf("Verifying ADC Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(1, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("ADC get version test completed \n ");
}

/*******************************************************************************
 * Function to enable the ADC ping and pong
 ******************************************************************************/
void test_adc_ping_pong_enable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC ping pong enable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_enable_ping_pong(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_enable_ping_pong(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC ping pong enable successfully \n");

  UnityPrintf("ADC ping pong enable test completed \n ");
}

/*******************************************************************************
 * Function to disable the ADC ping and pong
 ******************************************************************************/
void test_adc_ping_pong_disable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC ping pong disable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_disable_ping_pong(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_disable_ping_pong(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC ping pong disable successfully \n");

  UnityPrintf("ADC ping pong disable test completed \n ");
}

/*******************************************************************************
 * Function to enable the ADC channel
 ******************************************************************************/
void test_adc_channel_enable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC channel enable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_channel_enable(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_channel_enable(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC channel enable successfully \n");

  UnityPrintf("ADC channel enable test completed \n ");
}

/*******************************************************************************
 * Function to disable the ADC channel
 ******************************************************************************/
void test_adc_channel_disable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC channel disable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_channel_disable(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_channel_disable(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC channel disable successfully \n");

  UnityPrintf("ADC channel disable test completed \n ");
}

/*******************************************************************************
 * Function to test the ADC noise average mode.
 ******************************************************************************/
void test_adc_noise_average_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC noise average mode \n");
  sl_status_t status;

  UnityPrintf("Testing API with enable noise average mode \n");
  status = sl_si91x_adc_set_noise_average_mode(ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, enable noise average mode Test Passed successfully \n");

  UnityPrintf("Testing API with disable noise average mode \n");
  status = sl_si91x_adc_set_noise_average_mode(DISABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, disable noise average mode successfully \n");

  UnityPrintf("ADC disable noise average mode test completed \n ");
}

/*******************************************************************************
 * Function to enable the ADC per channel dma.
 ******************************************************************************/
void test_adc_internal_per_channel_dma_enable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC per channel dma enable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_internal_per_channel_dma_enable(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_internal_per_channel_dma_enable(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC per channel dma enable successfully \n");

  UnityPrintf("ADC per channel dma enable test completed \n ");
}

/*******************************************************************************
 * Function to disable the ADC per channel dma.
 ******************************************************************************/
void test_adc_internal_per_channel_dma_disable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC per channel dma disable \n");
  sl_status_t status;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_internal_per_channel_dma_disable(16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with valid channel \n");
  status = sl_si91x_adc_internal_per_channel_dma_disable(0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC per channel dma disable successfully \n");

  UnityPrintf("ADC per channel dma disable test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC static mode.
 ******************************************************************************/
void test_adc_configure_static_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC static mode configuration \n");
  sl_status_t status;

  UnityPrintf("Testing with maximum sampling rate instance \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_configure_static_mode(adc_channel_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_configure_static_mode(adc_channel_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_configure_static_mode(adc_channel_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC static mode configure successfully \n");

  UnityPrintf("ADC static mode configure test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC fifo mode.
 ******************************************************************************/
void test_adc_configure_fifo_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC fifo mode configuration \n");
  sl_status_t status;
  uint8_t chnl_num = 0;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_configure_fifo_mode(adc_channel_config, 16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_configure_fifo_mode(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_configure_fifo_mode(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_configure_fifo_mode(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC fifo mode configure successfully \n");

  UnityPrintf("ADC fifo mode configure test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC channel sampling rate.
 ******************************************************************************/
void test_adc_channel_sampling_rate(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC channel sampling rate configuration \n");
  sl_status_t status;
  uint8_t chnl_num = 0;

  UnityPrintf("Testing API with Maximum channel number \n");
  default_adc_internal_struct(&adc_internal_config);
  status = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, 16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with invalid input type \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  chnl_num                          = 0;
  status                            = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing API with invalid ping address \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.ping_addr[0] = 4294967296;
  status                           = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid ping address Test Passed successfully \n");

  UnityPrintf("Testing API with invalid pong address \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.pong_addr[0] = 4294967295 + 1;
  status                           = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid pong address Test Passed successfully \n");

  UnityPrintf("Testing API with invalid ping length \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.ping_length[0] = 1023 + 1;
  status                             = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid ping length Test Passed successfully \n");

  UnityPrintf("Testing API with invalid pong length \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.pong_length[0] = 1023 + 1;
  status                             = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid pong length Test Passed successfully \n");

  UnityPrintf("Testing API with invalid maximum sample number \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.num_of_samples[0] = 1024;
  status                                = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_COUNT, status);
  UnityPrintf("Status of API is correct, invalid maximum sample number Test Passed successfully \n");

  UnityPrintf("Testing API with invalid minimum sample number \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.num_of_samples[0] = 0;
  status                                = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_COUNT, status);
  UnityPrintf("Status of API is correct, invalid minimum sample number Test Passed successfully \n");

  UnityPrintf("Testing API with correct parameters \n");
  default_adc_internal_struct(&adc_internal_config);
  status = sl_si91x_adc_configure_channel_sampling_rate(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, correct parameters Test Passed successfully \n");

  UnityPrintf("ADC channel sampling rate configure test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC ping pong memory address.
 ******************************************************************************/
void test_adc_ping_pong_memory_address_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC ping pong memory address configuration \n");
  sl_status_t status;
  uint8_t chnl_num = 0;

  UnityPrintf("Testing API with Maximum channel number \n");
  default_adc_internal_struct(&adc_internal_config);
  status = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, 16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing API with invalid input type \n");
  default_adc_internal_struct(&adc_internal_config);
  chnl_num                          = 0;
  adc_internal_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                            = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing API with invalid minimum sample number \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.num_of_samples[0] = 0;
  status = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_COUNT, status);
  UnityPrintf("Status of API is correct, invalid minimum sample number Test Passed successfully \n");

  UnityPrintf("Testing API with invalid maximum sample number \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.num_of_samples[0] = 1024;
  status = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_COUNT, status);
  UnityPrintf("Status of API is correct, invalid maximum sample number Test Passed successfully \n");

  UnityPrintf("Testing API with invalid ping address \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.ping_addr[0] = 4294967295 + 1;
  status                           = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid ping address Test Passed successfully \n");

  UnityPrintf("Testing API with invalid pong address \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.pong_addr[0] = 4294967295 + 1;
  status                           = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid pong address Test Passed successfully \n");

  UnityPrintf("Testing API with invalid ping length \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.ping_length[0] = 1023 + 1;
  status                             = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid ping length Test Passed successfully \n");

  UnityPrintf("Testing API with invalid pong length \n");
  default_adc_internal_struct(&adc_internal_config);
  adc_internal_config.pong_length[0] = 1023 + 1;
  status                             = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid pong length Test Passed successfully \n");

  UnityPrintf("Testing API with correct parameters \n");
  default_adc_internal_struct(&adc_internal_config);
  status = sl_si91x_adc_configure_ping_pong_memory_address(adc_internal_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, correct parameters Test Passed successfully \n");

  UnityPrintf("ADC ping pong memory address configure test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC fifo threshold.
 ******************************************************************************/
void test_adc_fifo_threshold_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC fifo threshold configuration \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid dma \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_fifo_threshold.dma_type = SL_ADC_DMA_TYPE_LAST;
  status                      = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid dma Test Passed successfully \n");

  UnityPrintf("Testing with invalid channel type \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  adc_fifo_threshold.num_of_channel_en = SL_ADC_CHANNEL_TYPE_LAST;
  status                               = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, invalid channel type Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_config_struct(&adc_config);
  default_adc_fifo_threshold_struct(&adc_fifo_threshold);
  status = sl_si91x_adc_fifo_threshold_configuration(adc_config, adc_fifo_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC fifo threshold configure Test Passed successfully \n");

  UnityPrintf("ADC fifo threshold configure test completed \n ");
}

/*******************************************************************************
 * Function to configure ADC threshold.
 ******************************************************************************/
void test_adc_threshold_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC threshold configuration \n");
  sl_status_t status;
  sl_adc_threshold_config_t adc_threshold;

  UnityPrintf("Testing with invalid threshold1 value \n");
  adc_threshold.threshold1 = 8;
  status                   = sl_si91x_adc_threshold_configuration(adc_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid threshold1 value Test Passed successfully \n");

  UnityPrintf("Testing with invalid threshold2 value \n");
  adc_threshold.threshold2 = 8;
  status                   = sl_si91x_adc_threshold_configuration(adc_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid threshold2 value Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  adc_threshold.threshold1 = 3;
  adc_threshold.threshold2 = 3;
  adc_threshold.range      = 0;
  status                   = sl_si91x_adc_threshold_configuration(adc_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC threshold1 condition = %d \n", adc_threshold.threshold1_cond);
  UnityPrintf("ADC threshold2 condition = %d \n", adc_threshold.threshold2_cond);
  UnityPrintf("Status of API is correct, ADC threshold configure Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters of range 1 \n");
  adc_threshold.threshold1 = 3;
  adc_threshold.threshold2 = 3;
  adc_threshold.range      = 1;
  status                   = sl_si91x_adc_threshold_configuration(adc_threshold);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC threshold1 condition = %d \n", adc_threshold.threshold1_cond);
  UnityPrintf("ADC threshold2 condition = %d \n", adc_threshold.threshold2_cond);
  UnityPrintf("Status of API is correct, ADC threshold configure of range 1 Test Passed successfully \n");

  UnityPrintf("ADC threshold configure test completed \n ");
}

/*******************************************************************************
 * Function to Read ADC data for fifo mode.
 ******************************************************************************/
void test_adc_read_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC data read for fifo mode \n");
  sl_status_t status;
  uint8_t chnl_num = 0;

  UnityPrintf("Testing API with Maximum channel number \n");
  status = sl_si91x_adc_read_data(adc_channel_config, 16);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_read_data(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_read_data(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_read_data(adc_channel_config, chnl_num);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC data read for fifo mode configure successfully \n");

  UnityPrintf("ADC data read for fifo mode test completed \n ");
}

/*******************************************************************************
 * Function to Read ADC data for static mode.
 ******************************************************************************/
void test_adc_read_data_static(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC data read for static mode \n");
  sl_status_t status;
  int16_t read_static_data;

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.sampling_rate[0] = 5000000 + 1;
  status                              = sl_si91x_adc_read_data_static(adc_channel_config, &read_static_data);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, ADC maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_adc_channel_config_struct(&adc_channel_config);
  adc_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                           = sl_si91x_adc_read_data_static(adc_channel_config, &read_static_data);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_channel_config_struct(&adc_channel_config);
  status = sl_si91x_adc_read_data_static(adc_channel_config, &read_static_data);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("ADC data in static mode = %d \n", read_static_data);
  UnityPrintf("Status of API is correct, ADC data read for static mode successfully \n");

  UnityPrintf("ADC data read for static mode test completed \n ");
}

/*******************************************************************************
 * Function to Read ADC start.
 ******************************************************************************/
void test_adc_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC start \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_start(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_start(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_start(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_config_struct(&adc_config);
  status = sl_si91x_adc_start(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC start Test Passed successfully \n");

  UnityPrintf("ADC start test completed \n ");
}

/*******************************************************************************
 * Function to Read ADC stop.
 ******************************************************************************/
void test_adc_stop(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC stop \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_stop(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_stop(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_stop(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_config_struct(&adc_config);
  status = sl_si91x_adc_stop(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC stop Test Passed successfully \n");

  UnityPrintf("ADC stop test completed \n ");
}

/*******************************************************************************
 * Function to test the un-initialization of ADC
 ******************************************************************************/
void test_adc_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ADC deinit \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid operation mode \n");
  default_adc_config_struct(&adc_config);
  adc_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                    = sl_si91x_adc_deinit(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 17;
  status                           = sl_si91x_adc_deinit(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_adc_config_struct(&adc_config);
  adc_config.num_of_channel_enable = 0;
  status                           = sl_si91x_adc_deinit(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_adc_config_struct(&adc_config);
  status = sl_si91x_adc_deinit(adc_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, ADC deinit Test Passed successfully \n");

  UnityPrintf("ADC deinit test completed \n ");
}

/*******************************************************************************
 * Static function to update the ADC fifo threshold configuration structure
 * with default values
 ******************************************************************************/
static void default_adc_fifo_threshold_struct(sl_adc_fifo_thrld_config_t *adc_fifo_threshold)
{
  adc_fifo_threshold->dma_type          = SL_ADC_INTERNAL_DMA;
  adc_fifo_threshold->num_of_channel_en = 1;
  adc_fifo_threshold->a_empty_threshold = 3;
  adc_fifo_threshold->a_full_threshold  = 3;
}

/*******************************************************************************
 * Static function to update the ADC internal configuration structure with
 * default values
 ******************************************************************************/
static void default_adc_internal_struct(sl_adc_internal_config_t *adc_internal_config)
{
  adc_internal_config->input_type[0]     = SL_ADC_SINGLE_ENDED;
  adc_internal_config->num_of_samples[0] = 1023;
  adc_internal_config->ping_addr[0]      = 4294967295;
  adc_internal_config->pong_addr[0]      = 4294967295;
  adc_internal_config->ping_length[0]    = 1023;
  adc_internal_config->pong_length[0]    = 1023;
}

/*******************************************************************************
 * Static function to update the ADC external trigger structure with default values
 ******************************************************************************/
static void default_adc_external_trigger_struct(sl_adc_external_config_t *adc_external_trigger)
{
  adc_external_trigger->trigger_num        = SL_ADC_EXT_TRIGGER_1;
  adc_external_trigger->trigger_type       = SL_ULP_TIMER_EXT_TRIGGER;
  adc_external_trigger->detection_edge_sel = SL_ADC_EXT_TRIGGER_NEG_EDGE;
  adc_external_trigger->trigger_sel_val    = SL_ADC_CHANNEL_1;
  adc_external_trigger->trigger_sel        = SL_ADC_EXT_TRIGGER_SEL_1;
}

/*******************************************************************************
 * Static function to update the ADC config structure with default values
 ******************************************************************************/
static void default_adc_config_struct(sl_adc_config_t *adc_config)
{
  adc_config->operation_mode        = SL_ADC_FIFO_MODE;
  adc_config->num_of_channel_enable = NUMBER_OF_CHA_EN;
}

/*******************************************************************************
 * Static function to update the ADC channel config structure with default values
 ******************************************************************************/
static void default_adc_channel_config_struct(sl_adc_channel_config_t *adc_channel_config)
{
  adc_channel_config->input_type[0]        = SL_ADC_SINGLE_ENDED;
  adc_channel_config->num_of_samples[0]    = CHANNEL0_SAMPLE_LENGTH;
  adc_channel_config->sampling_rate[0]     = CHANNEL0_SAMPLING_RATE;
  adc_channel_config->pos_inp_sel[0]       = ADC_POS_INPUT_SEL;
  adc_channel_config->neg_inp_sel[0]       = ADC_NEG_INPUT_SEL;
  adc_channel_config->opamp_gain[0]        = OPEN_GAIN;
  adc_channel_config->rx_buf[0]            = adc_output_ch0;
  adc_channel_config->chnl_ping_address[0] = adc_ping_buffer;
  adc_channel_config->chnl_pong_address[0] = adc_pong_buffer;
}

/*******************************************************************************
 * Static function to update the clock config structure with default values
 ******************************************************************************/
static void adc_clock_configuration(sl_adc_clock_config_t *clock_config)
{

  clock_config->soc_pll_clock           = PS4_SOC_FREQ;
  clock_config->soc_pll_reference_clock = SOC_PLL_REF_FREQUENCY;
  clock_config->division_factor         = DVISION_FACTOR;
}
/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by ADC interface
 * It updates the respective member of the structure as the event is triggered.
 ******************************************************************************/
static void callback_event(uint8_t channel_no, uint8_t event)
{
  if (event == INTERNAL_DMA) {
    chnl0_complete_flag = 1;
  } else if (event == ADC_STATIC_MODE_CALLBACK) {
    chnl0_complete_flag = 1;
  }
}
