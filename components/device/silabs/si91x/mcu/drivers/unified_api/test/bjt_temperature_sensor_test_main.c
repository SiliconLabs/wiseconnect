/***************************************************************************/ /**
 * @file BJT_temperature_sensor_test_main.c
 * @brief BJT temperature Unity Test Functions
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
#include "rsi_debug.h"
#include "sl_si91x_bjt_temperature_sensor.h"
#include "sl_system_init.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define CHANNEL0_SAMPLE_LENGTH 1      // Number of BJT sample collect for operation
#define NUMBER_OF_CHA_EN       1      // Single channel data acquisition
#define CHANNEL0_SAMPLING_RATE 100000 // Number of samples to collect Minimum of 1sps and Maximum to 10Msps.
#define ADC_POS_INPUT_SEL      5      // BJT positive input channel
#define ADC_NEG_INPUT_SEL      5      // BJT Negative input channel (for single ended this value is not used)
#define OPAMP_GAIN             2      // Set the gain value for opamp input.
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
extern adc_config_t sl_bjt_config;
extern adc_ch_config_t sl_bjt_channel_config;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void default_bjt_config_struct(adc_config_t *sl_bjt_config);
static void default_bjt_channel_config_struct(adc_ch_config_t *sl_bjt_channel_config);

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Test Function Prototypes ****************************
 ******************************************************************************/
void test_bjt_temperature_sensor_read_data(void);
void test_bjt_temperature_sensor_state(void);
void test_bjt_temperature_sensor_deinit(void);
void test_bjt_temperature_sensor_init(void);
void test_bjt_temperature_sensor_temperature_conversion(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("BJT temperature");

  RUN_TEST(test_bjt_temperature_sensor_init, __LINE__);
  RUN_TEST(test_bjt_temperature_sensor_state, __LINE__);
  RUN_TEST(test_bjt_temperature_sensor_temperature_conversion, __LINE__);
  RUN_TEST(test_bjt_temperature_sensor_read_data, __LINE__);
  RUN_TEST(test_bjt_temperature_sensor_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}
/*******************************************************************************
 * Function to test the un-initialization of BJT temperature sensor
 ******************************************************************************/
void test_bjt_temperature_sensor_init(void)
{
  sl_status_t status;

  UnityPrintf("\n");
  UnityPrintf("Testing BJT temperature sensor init \n");

  UnityPrintf("Testing with above maximum sampling rate instance \n");
  default_bjt_config_struct(&sl_bjt_config);
  default_bjt_channel_config_struct(&sl_bjt_channel_config);
  sl_bjt_channel_config.sampling_rate[0] = 10000000 + 1;
  status                                 = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_RANGE, status);
  UnityPrintf("Status of API is correct, BJT maximum sampling rate Test Passed successfully \n");

  UnityPrintf("Testing with invalid input type \n");
  default_bjt_config_struct(&sl_bjt_config);
  default_bjt_channel_config_struct(&sl_bjt_channel_config);
  sl_bjt_channel_config.input_type[0] = SL_ADC_INPUT_TYPE_LAST;
  status                              = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid input type Test Passed successfully \n");

  UnityPrintf("Testing with invalid operation mode \n");
  default_bjt_config_struct(&sl_bjt_config);
  default_bjt_channel_config_struct(&sl_bjt_channel_config);
  sl_bjt_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                       = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters for Static mode \n");
  default_bjt_config_struct(&sl_bjt_config);
  default_bjt_channel_config_struct(&sl_bjt_channel_config);
  sl_bjt_config.operation_mode            = SL_ADC_STATIC_MODE;
  sl_bjt_channel_config.num_of_samples[0] = 1;
  status                                  = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, BJT Init successfully \n");

  UnityPrintf("BJT temperature sensor de-init test completed \n ");
}

/*******************************************************************************
 * BJT Temperature sensing enable bit is set.
 ******************************************************************************/
void test_bjt_temperature_sensor_state(void)
{
  sl_status_t status;

  UnityPrintf("\n");
  UnityPrintf("Testing BJT temperature sensor enable \n");

  UnityPrintf("Testing with invalid enable parameter \n");
  status = sl_si91x_bjt_temperature_sensor_state(SL_BJT_TEMPERATURE_SENSOR_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid enable parameter \n");
  status = sl_si91x_bjt_temperature_sensor_state(SL_BJT_TEMPERATURE_SENSOR_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid enable parameter \n");
  status = sl_si91x_bjt_temperature_sensor_state(SL_BJT_TEMPERATURE_SENSOR_DISABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("BJT temperature sensor enable test completed \n");
}

/*******************************************************************************
 * Reads the temperature from BJT sensor
 ******************************************************************************/
void test_bjt_temperature_sensor_read_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing BJT temperature read \n");
  sl_status_t status;
  double temperature;

  UnityPrintf("Testing with NULL parameter \n");
  status = sl_si91x_bjt_temperature_sensor_read_data(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("Testing with valid parameter \n");
  status = sl_si91x_bjt_temperature_sensor_read_data(&temperature);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("BJT temperature read completed \n");
}

/*******************************************************************************
 * BJT Temperature sensor temperature conversion.
 ******************************************************************************/
void test_bjt_temperature_sensor_temperature_conversion(void)
{
  sl_status_t status;
  double temperature;

  UnityPrintf("\n");
  UnityPrintf("Testing BJT temperature sensor temperature conversion \n");

  UnityPrintf("Testing with NULL parameter \n");
  status = sl_si91x_get_bjt_temperature_sensor_conversion(NULL, SL_BJT_FAHRENHEIT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("Testing with NULL parameter \n");
  status = sl_si91x_get_bjt_temperature_sensor_conversion(&temperature, 4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("Testing with valid parameter \n");
  status = sl_si91x_get_bjt_temperature_sensor_conversion(&temperature, SL_BJT_DEGREE_CELSIUS);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("BJT temperature read completed \n");
}

/*******************************************************************************
 * Function to test the un-initialization of BJT temperature sensor
 ******************************************************************************/
void test_bjt_temperature_sensor_deinit(void)
{
  sl_status_t status;

  UnityPrintf("Testing with invalid operation mode \n");
  default_bjt_config_struct(&sl_bjt_config);
  sl_bjt_config.operation_mode = SL_ADC_OPERATION_MODE_LAST;
  status                       = sl_si91x_bjt_temperature_sensor_deinit(sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid operation mode Test Passed successfully \n");

  UnityPrintf("Testing with invalid maximum channel number \n");
  default_bjt_config_struct(&sl_bjt_config);
  sl_bjt_config.num_of_channel_enable = 17;
  status                              = sl_si91x_bjt_temperature_sensor_deinit(sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid maximum channel number Test Passed successfully \n");

  UnityPrintf("Testing with invalid minimum channel number \n");
  default_bjt_config_struct(&sl_bjt_config);
  sl_bjt_config.num_of_channel_enable = 0;
  status                              = sl_si91x_bjt_temperature_sensor_deinit(sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid minimum channel number Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  default_bjt_config_struct(&sl_bjt_config);
  status = sl_si91x_bjt_temperature_sensor_deinit(sl_bjt_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, BJT deinit Test Passed successfully \n");

  UnityPrintf("BJT deinit test completed \n ");
}
/*******************************************************************************
 * Static function to update the BJT config structure with default values
 ******************************************************************************/
static void default_bjt_config_struct(adc_config_t *sl_bjt_config)
{
  sl_bjt_config->operation_mode        = SL_ADC_STATIC_MODE;
  sl_bjt_config->num_of_channel_enable = NUMBER_OF_CHA_EN;
}

/*******************************************************************************
 * Static function to update the BJT channel config structure with default values
 ******************************************************************************/
static void default_bjt_channel_config_struct(adc_ch_config_t *sl_bjt_channel_config)
{
  sl_bjt_channel_config->input_type[0]     = SL_ADC_SINGLE_ENDED;
  sl_bjt_channel_config->num_of_samples[0] = CHANNEL0_SAMPLE_LENGTH;
  sl_bjt_channel_config->sampling_rate[0]  = CHANNEL0_SAMPLING_RATE;
  sl_bjt_channel_config->pos_inp_sel[0]    = ADC_POS_INPUT_SEL;
  sl_bjt_channel_config->neg_inp_sel[0]    = ADC_NEG_INPUT_SEL;
  sl_bjt_channel_config->opamp_gain[0]     = OPAMP_GAIN;
}
