/***************************************************************************/ /**
 * @file analog_comparator_test_main.c
 * @brief Analog Comparator Unity Test Functions
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
#include "sl_si91x_analog_comparator.h"
#include "sl_system_init.h"
#include "unity.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void on_comparator1_callback(void);
static void on_comparator2_callback(void);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_analog_comparator_init(void);
void test_analog_comparator_set_configurations(void);
void test_analog_comparator_register_callback(void);
void test_analog_comparator_unregister_callback(void);
void test_analog_comparator_set_resistor_bank_threshold(void);
void test_analog_comparator_set_reference_scaler_output(void);
void test_analog_comparator_deinit(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("ANALOG COMPARATOR");
  RUN_TEST(test_analog_comparator_init, __LINE__);
  RUN_TEST(test_analog_comparator_set_configurations, __LINE__);
  RUN_TEST(test_analog_comparator_set_resistor_bank_threshold, __LINE__);
  RUN_TEST(test_analog_comparator_set_reference_scaler_output, __LINE__);
  RUN_TEST(test_analog_comparator_register_callback, __LINE__);
  RUN_TEST(test_analog_comparator_unregister_callback, __LINE__);
  RUN_TEST(test_analog_comparator_deinit, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test init API of comparator
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator init API \n");
  sl_si91x_analog_comparator_init();
  TEST_ASSERT_EQUAL_HEX(AUX_ADC_DAC_COMP->AUX_LDO_b.ENABLE_LDO, ENABLE);
  TEST_ASSERT_EQUAL_HEX(AUX_ADC_DAC_COMP->AUX_LDO_b.BYPASS_LDO, ENABLE);
  UnityPrintf("Comparator reference voltage is configured properly\n");
  UnityPrintf("Analog Comparator init API test completed \n");
}

/*******************************************************************************
 * Function to test the set configuration API of Analog comparator
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_set_configurations(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator set configuration API \n");
  sl_status_t status;
  sl_analog_comparator_config_t handle1;
  sl_analog_comparator_config_t handle2;
  sl_analog_comparator_config_t handle3;
  sl_analog_comparator_config_t handle4;
  sl_analog_comparator_config_t handle5;
  sl_analog_comparator_config_t *test_null = NULL;

  handle1.comparator_number   = SL_COMPARATOR_LAST;
  handle1.non_inverting_input = SL_COMPARATOR_GPIO_INPUT_1;
  handle1.inverting_input     = SL_COMPARATOR_GPIO_INPUT_1;
  handle1.hystersis_value     = 0;
  handle1.is_filter_enabled   = false;

  handle2.comparator_number   = SL_COMPARATOR_2;
  handle2.non_inverting_input = SL_COMPARATOR_INPUT_LAST;
  handle2.inverting_input     = SL_COMPARATOR_GPIO_INPUT_1;
  handle2.hystersis_value     = 0;
  handle2.is_filter_enabled   = false;

  handle3.comparator_number   = SL_COMPARATOR_2;
  handle3.non_inverting_input = SL_COMPARATOR_GPIO_INPUT_1;
  handle3.inverting_input     = SL_COMPARATOR_INPUT_LAST;
  handle3.hystersis_value     = 0;
  handle3.is_filter_enabled   = false;

  handle4.comparator_number   = SL_COMPARATOR_2;
  handle4.non_inverting_input = SL_COMPARATOR_GPIO_INPUT_1;
  handle4.inverting_input     = SL_COMPARATOR_GPIO_INPUT_1;
  handle4.hystersis_value     = SL_COMPARATOR_HYSTERSIS_VALUE_LAST;
  handle4.is_filter_enabled   = false;

  handle5.comparator_number   = SL_COMPARATOR_2;
  handle5.non_inverting_input = SL_COMPARATOR_GPIO_INPUT_1;
  handle5.inverting_input     = SL_COMPARATOR_GPIO_INPUT_1;
  handle5.hystersis_value     = 0;
  handle5.is_filter_enabled   = false;

  status = sl_si91x_analog_comparator_set_configurations(test_null);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, when tested with NULL pointer \n");

  // testing with invalid parameters
  status = sl_si91x_analog_comparator_set_configurations(&handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid comparator number \n");

  status = sl_si91x_analog_comparator_set_configurations(&handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid non-inverting input parameter \n");

  status = sl_si91x_analog_comparator_set_configurations(&handle3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid inverting input parameter \n");

  status = sl_si91x_analog_comparator_set_configurations(&handle4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid Hysteresis value parameter \n");

  // testing with valid parameters
  status = sl_si91x_analog_comparator_set_configurations(&handle5);
  TEST_ASSERT_EQUAL_HEX(AUX_ADC_DAC_COMP->COMPARATOR1_b.CMP2_EN, 0);
  UnityPrintf("Comparator number is configured properly\n");
  TEST_ASSERT_EQUAL_HEX(handle5.non_inverting_input, AUX_ADC_DAC_COMP->COMPARATOR1_b.CMP2_MUX_SEL_P);
  UnityPrintf("Comparator non-inverting input is configured properly\n");
  TEST_ASSERT(handle5.inverting_input == AUX_ADC_DAC_COMP->COMPARATOR1_b.CMP2_MUX_SEL_N);
  UnityPrintf("Comparator inverting input is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("Analog comparator set configuration API test completed \n");
}

/*******************************************************************************
 * Function to test analog comparator set resistor bank threshold API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_set_resistor_bank_threshold(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator set resistor bank threshold API \n");
  sl_status_t status;

  status = sl_si91x_analog_comparator_set_resistor_bank_threshold(SL_COMPARATOR_THRESHOLD_VALUE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid threshold value \n");

  status = sl_si91x_analog_comparator_set_resistor_bank_threshold(SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_86_VOLT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(AUX_ADC_DAC_COMP->BOD_b.BOD_THRSH, (SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_86_VOLT & 0x1F));
  UnityPrintf("Comparator threshold value is configured properly\n");
  UnityPrintf("Analog Comparator set resistor bank threshold API test completed \n");
}

/*******************************************************************************
 * Function to test analog comparator set reference scaler output API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_set_reference_scaler_output(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator set reference scaler output API \n");
  sl_status_t status;
  sl_analog_comparator_config_t handle6;
  handle6.comparator_number   = SL_COMPARATOR_2;
  handle6.non_inverting_input = SL_COMPARATOR_RESISTOR_BANK_OUTPUT;
  handle6.inverting_input     = SL_COMPARATOR_REFERRENCE_SCALER_OUTPUT;
  handle6.hystersis_value     = 0;
  handle6.is_filter_enabled   = false;
  status                      = sl_si91x_analog_comparator_set_configurations(&handle6);
  //setting non-inverting input through BOD output
  sl_si91x_analog_comparator_set_resistor_bank_threshold(SL_COMPARATOR_THRESHOLD_VALUE_FOR_1_86_VOLT);
  status = sl_si91x_analog_comparator_set_reference_scaler_output(SL_COMPARATOR_SCALE_FACTOR_VALUE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid threshold value \n");

  //setting inverting input through reference scaler
  status = sl_si91x_analog_comparator_set_reference_scaler_output(SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_8_VOLT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(AUX_ADC_DAC_COMP->BOD_b.REFBUF_VOLT_SEL, SL_COMPARATOR_SCALE_FACTOR_VALUE_FOR_0_8_VOLT);
  UnityPrintf("Comparator threshold value is configured properly\n");
  UnityPrintf("Analog Comparator set reference scaler output API test completed \n");
}

/*******************************************************************************
 * Function to test comparator callback register api
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_register_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator callback register API \n");
  sl_status_t status;

  status = sl_si91x_analog_comparator_register_callback(SL_COMPARATOR_2, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  status = sl_si91x_analog_comparator_register_callback(SL_COMPARATOR_LAST, on_comparator2_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid comparator number \n");

  status = sl_si91x_analog_comparator_register_callback(SL_COMPARATOR_2, on_comparator2_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  UnityPrintf("Callback is registered  successfully \n");
  UnityPrintf("Analog Comparator callback register test completed \n");
}

/*******************************************************************************
 * Function to test comparator callback unregister api
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator callback unregister Api \n");
  sl_status_t status;

  status = sl_si91x_analog_comparator_unregister_callback(SL_COMPARATOR_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid comparator number \n");

  status = sl_si91x_analog_comparator_unregister_callback(SL_COMPARATOR_2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  status = sl_si91x_analog_comparator_register_callback(SL_COMPARATOR_2, on_comparator2_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("An already registered callback is registered successfully after unregister\n");
  UnityPrintf("Analog Comparator callback unregister test completed \n");
}
/*******************************************************************************
 * Function to test comparator deinit api
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_analog_comparator_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing analog comparator deinit API \n");
  sl_si91x_analog_comparator_deinit();
  TEST_ASSERT_EQUAL_HEX(ULPCLK->ULP_AUXADC_CLK_GEN_REG_b.ULP_AUX_CLK_EN_b, DISABLE);
  UnityPrintf("Comparator AUXADC clock disabled successfully \n");
  UnityPrintf("Analog Comparator deinit API test completed \n");
}
/*******************************************************************************
 Comparator-1 callback function
 ******************************************************************************/
void on_comparator1_callback(void)
{
  UnityPrintf("In callback, it is registered successfully\n");
  sl_si91x_analog_comparator_unregister_callback(SL_COMPARATOR_1);
}

/*******************************************************************************
 Comparator-2 callback function
 ******************************************************************************/
void on_comparator2_callback(void)
{
  UnityPrintf("In callback, it is registered successfully\n");
  sl_si91x_analog_comparator_unregister_callback(SL_COMPARATOR_2);
}
