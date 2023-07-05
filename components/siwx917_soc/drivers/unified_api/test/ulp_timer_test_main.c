/***************************************************************************/ /**
 * @file ulp_timer_test_main.c
 * @brief ULP-Timers Unity Test Functions
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

#include "sl_si91x_ulp_timer.h"
#include "sl_si91x_ulp_timer_inst_config.h"
#include "sl_system_init.h"
#include "rsi_timers.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define error_code_t error_t

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
boolean_t timer0_timeout_flag = false;
boolean_t timer1_timeout_flag = false;
boolean_t timer2_timeout_flag = false;
boolean_t timer3_timeout_flag = false;

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void on_timeout_callback0(void);
static sl_status_t ulp_timer_disable_interrupt(ulp_timer_instance_t timer_num);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_ulp_timer_configure_clock(void);
void test_ulp_timer_set_configuration(void);
void test_ulp_timer_start(void);
void test_ulp_timer_stop(void);
void test_ulp_timer_restart(void);
void test_ulp_timer_set_count(void);
void test_ulp_timer_get_count(void);
void test_ulp_timer_set_mode(void);
void test_ulp_timer_set_type(void);
void test_ulp_timer_get_type(void);
void test_ulp_timer_get_mode(void);
void test_ulp_timer_register_timeout_callback(void);
void test_ulp_timer_unregister_timeout_callback(void);
void test_ulp_timer_set_direction(void);
void test_ulp_timer_get_direction(void);
void test_ulp_timer_configure_xtal_clock(void);
void test_ulp_timer_configure_soc_clock(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("ULP-TIMER");

  RUN_TEST(test_ulp_timer_configure_clock, __LINE__);
  RUN_TEST(test_ulp_timer_set_configuration, __LINE__);
  RUN_TEST(test_ulp_timer_start, __LINE__);
  RUN_TEST(test_ulp_timer_get_count, __LINE__);
  RUN_TEST(test_ulp_timer_stop, __LINE__);
  RUN_TEST(test_ulp_timer_restart, __LINE__);
  RUN_TEST(test_ulp_timer_set_mode, __LINE__);
  RUN_TEST(test_ulp_timer_set_type, __LINE__);
  RUN_TEST(test_ulp_timer_set_count, __LINE__);
  RUN_TEST(test_ulp_timer_get_type, __LINE__);
  RUN_TEST(test_ulp_timer_get_mode, __LINE__);
  RUN_TEST(test_ulp_timer_get_direction, __LINE__);
  RUN_TEST(test_ulp_timer_register_timeout_callback, __LINE__);
  RUN_TEST(test_ulp_timer_unregister_timeout_callback, __LINE__);
  RUN_TEST(test_ulp_timer_set_direction, __LINE__);
  RUN_TEST(test_ulp_timer_configure_xtal_clock, __LINE__);
  RUN_TEST(test_ulp_timer_configure_soc_clock, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the clock configuration of ULP timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_configure_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer clock configure api \n");

  sl_status_t status;
  ulp_timer_clk_src_config_t timer_clk_handle1;
  ulp_timer_clk_src_config_t timer_clk_handle2;
  ulp_timer_clk_src_config_t *test_null = NULL;

  timer_clk_handle1.ulp_timer_clk_type           = SL_ULP_TIMER_CLK_TYPE_STATIC;
  timer_clk_handle1.ulp_timer_sync_to_ulpss_pclk = 0;
  timer_clk_handle1.ulp_timer_clk_input_src      = SL_ULP_TIMER_20MHZ_RO_CLK;
  timer_clk_handle1.ulp_timer_skip_switch_time   = 0;

  timer_clk_handle2.ulp_timer_clk_type           = 1;
  timer_clk_handle2.ulp_timer_sync_to_ulpss_pclk = 0;
  timer_clk_handle2.ulp_timer_clk_input_src      = 8;
  timer_clk_handle2.ulp_timer_skip_switch_time   = 0;
  // testing with invalid parameters
  status = sl_si91x_ulp_timer_configure_clock(&timer_clk_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");
  // tested with NULL pointer
  status = sl_si91x_ulp_timer_configure_clock(test_null);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL pointer parameter \n");
  // testing with valid parameters
  status = sl_si91x_ulp_timer_configure_clock(&timer_clk_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("ULP-Timer clock configuration test completed \n");
}

/*******************************************************************************
 * Function to test the config api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_set_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer config api \n");
  sl_status_t status;
  ulp_timer_config_t timer_handle1;
  ulp_timer_config_t timer_handle2;
  ulp_timer_config_t timer_handle3;
  ulp_timer_config_t timer_handle4;
  ulp_timer_config_t *test_null = NULL;

  timer_handle1.timer_num         = ULP_TIMER_0;
  timer_handle1.timer_mode        = ULP_TIMER_MODE_PERIODIC;
  timer_handle1.timer_type        = ULP_TIMER_TYP_1US;
  timer_handle1.timer_match_value = 20000000;
  timer_handle1.timer_direction   = DOWN_COUNTER;

  timer_handle2.timer_num         = 5; // wrong value for timer instance
  timer_handle2.timer_mode        = SL_ULP_TIMER_MODE_PERIODIC;
  timer_handle2.timer_type        = SL_ULP_TIMER_TYP_1US;
  timer_handle2.timer_match_value = SL_ULP_TIMER_MATCH_VALUE_DEFAULT;
  timer_handle2.timer_direction   = DOWN_COUNTER;

  timer_handle3.timer_num         = ULP_TIMER_1;
  timer_handle3.timer_mode        = 2; // wrong value for mode
  timer_handle3.timer_type        = SL_ULP_TIMER_TYP_256US;
  timer_handle3.timer_match_value = SL_ULP_TIMER_MATCH_VALUE_DEFAULT;
  timer_handle2.timer_direction   = DOWN_COUNTER;

  timer_handle4.timer_num         = ULP_TIMER_2;
  timer_handle4.timer_mode        = SL_ULP_TIMER_MODE_ONESHOT;
  timer_handle4.timer_type        = 3; // wrong value for timer type
  timer_handle4.timer_match_value = SL_ULP_TIMER_MATCH_VALUE_DEFAULT;
  timer_handle4.timer_direction   = UP_COUNTER;

  status = sl_si91x_ulp_timer_set_configuration(test_null);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct \n");
  UnityPrintf("ULP-Timer config api test completed, tested with NULL pointer \n");

  // testing with valid parameters
  status        = sl_si91x_ulp_timer_set_configuration(&timer_handle1);
  uint32_t temp = TIMERS->MATCH_CTRL[0].MCUULP_TMR_MATCH;
  temp          = TIMERS->MATCH_CTRL[0].MCUULP_TMR_MATCH;
  TEST_ASSERT(timer_handle1.timer_match_value == temp + 1);
  UnityPrintf("Timer match value is configured properly\n");
  TEST_ASSERT(timer_handle1.timer_mode == TIMERS->MATCH_CTRL[0].MCUULP_TMR_CNTRL_b.TMR_MODE);
  UnityPrintf("Timer mode is configured properly\n");
  TEST_ASSERT(timer_handle1.timer_type == TIMERS->MATCH_CTRL[0].MCUULP_TMR_CNTRL_b.TMR_TYPE);
  UnityPrintf("Timer type is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  // testing with invalid parameters
  status = sl_si91x_ulp_timer_set_configuration(&timer_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index parameter \n");

  status = sl_si91x_ulp_timer_set_configuration(&timer_handle3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid mode parameter \n");

  status = sl_si91x_ulp_timer_set_configuration(&timer_handle4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_TYPE, status);
  UnityPrintf("Status of API is correct, tested with invalid type parameter \n");
  UnityPrintf("ULP-Timer configuration test completed \n");
}

/*******************************************************************************
 * Function to test the start api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer start \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;

  //Stopping Timer
  status = sl_si91x_ulp_timer_stop(timer_num);

  // Testing with valid parameter
  status = sl_si91x_ulp_timer_start(timer_num);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  // Testing with invalid parameter
  status = sl_si91x_ulp_timer_start(4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");
  UnityPrintf("Timer-%d, started \n", timer_num);
  UnityPrintf("ULP-Timer start test completed \n");
}

/*******************************************************************************
 * Function to test the stop api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_stop(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer stop \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;

  status = sl_si91x_ulp_timer_stop(5); //5 for wrong timer instance
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");

  status = sl_si91x_ulp_timer_stop(timer_num);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("Timer-%d, stopped \n", timer_num);
  UnityPrintf("ULP-Timer stop test completed \n");
}

/*******************************************************************************
 * Function to test the restart api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_restart(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer restart \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  // Testing with invalid timer number
  status = sl_si91x_ulp_timer_restart(5);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct \n");
  // Starting timer instance
  sl_si91x_ulp_timer_start(timer_num);
  // Testing with valid timer number
  status = sl_si91x_ulp_timer_restart(timer_num);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct \n");
  UnityPrintf("Timer-%d, restarted \n", timer_num);
  UnityPrintf("ULP-Timer restart test completed \n");
}

/*******************************************************************************
 * Function to test set mode api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_set_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer set-mode api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  ulp_timer_mode_t timer_mode    = ULP_TIMER_MODE_PERIODIC;

  // First stopping timer, before changing its parameters
  sl_si91x_ulp_timer_stop(timer_num);

  // Testing with invalid timer-mode
  status = sl_si91x_ulp_timer_set_mode(timer_num, 3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid index parameter \n");

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_set_mode(5, timer_mode);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index parameter \n");
  UnityPrintf("ULP-Timer test set mode completed \n");

  // Testing with valid timer-mode
  status = sl_si91x_ulp_timer_set_mode(timer_num, timer_mode);
  TEST_ASSERT(timer_mode == TIMERS->MATCH_CTRL[timer_num].MCUULP_TMR_CNTRL_b.TMR_MODE);
  UnityPrintf("Timer mode is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
}

/*******************************************************************************
 * Function to test get mode api of ulp-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_get_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer get-mode api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  uint32_t timer_mode;

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_get_mode(5, &timer_mode);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  // Testing with NULL Pointer
  status = sl_si91x_ulp_timer_get_mode(timer_num, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Setting timer mode to periodic
  status = sl_si91x_ulp_timer_set_mode(timer_num, ULP_TIMER_MODE_PERIODIC);
  // Testing with valid parameter
  status = sl_si91x_ulp_timer_get_mode(timer_num, &timer_mode);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(ULP_TIMER_MODE_PERIODIC, timer_mode);
  UnityPrintf("Timer mode is read properly\n");
  UnityPrintf("ULP-Timer get mode test completed \n");
}

/*******************************************************************************
 * Function to test set type api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_set_type(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer set-type api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  ulp_timer_type_t timer_type    = ULP_TIMER_TYP_1US;

  // First stopping timer, before changing its parameters
  sl_si91x_ulp_timer_stop(timer_num);

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_set_type(5, timer_type);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  //Testing with invalid timer-type
  status = sl_si91x_ulp_timer_set_type(timer_num, 3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_TYPE, status);
  UnityPrintf("Status of API is correct, tested with invalid type-parameter \n");

  //Testing with valid timer-type
  status = sl_si91x_ulp_timer_set_type(timer_num, timer_type);
  TEST_ASSERT(timer_type == TIMERS->MATCH_CTRL[timer_num].MCUULP_TMR_CNTRL_b.TMR_TYPE);
  UnityPrintf("Timer type is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("ULP-Timer test set type completed \n");
}

/*******************************************************************************
 * Function to test get type api of ulp-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_get_type(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer get-type api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  uint32_t timer_type;

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_get_type(5, &timer_type);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  // Testing with NULL Pointer
  status = sl_si91x_ulp_timer_get_type(timer_num, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Setting timer type to 1us
  status = sl_si91x_ulp_timer_set_type(timer_num, ULP_TIMER_TYP_1US);
  // Testing with valid parameter
  status = sl_si91x_ulp_timer_get_type(timer_num, &timer_type);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(ULP_TIMER_TYP_1US, timer_type);
  UnityPrintf("Timer type is read properly\n");
  UnityPrintf("ULP-Timer get type test completed \n");
}

/*******************************************************************************
 * Function to test set direction api of ulp-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_set_direction(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer_set_direction Api \n");
  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;

  // First stopping timer, before changing its parameters
  sl_si91x_ulp_timer_stop(timer_num);

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_set_direction(5, UP_COUNTER);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");

  // Testing with invalid timer-DIRECTION
  status = sl_si91x_ulp_timer_set_direction(timer_num, 3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");

  //Testing with valid parameters
  status = sl_si91x_ulp_timer_set_direction(timer_num, DOWN_COUNTER);
  TEST_ASSERT(TIMERS->MATCH_CTRL[timer_num].MCUULP_TMR_CNTRL_b.COUNTER_UP == DISABLE);
  UnityPrintf("Timer direction is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("ULP-Timer test set direction completed \n");
}

/*******************************************************************************
 * Function to test get direction api of ulp-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_get_direction(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer get-direction api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  uint32_t timer_dir;

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_get_direction(5, &timer_dir);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  // Testing with NULL Pointer
  status = sl_si91x_ulp_timer_get_direction(timer_num, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Setting timer direction as
  status = sl_si91x_ulp_timer_set_direction(timer_num, DOWN_COUNTER);
  // Testing with valid parameter
  status = sl_si91x_ulp_timer_get_direction(timer_num, &timer_dir);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(DOWN_COUNTER, timer_dir);
  UnityPrintf("Timer direction is read properly\n");
  UnityPrintf("ULP-Timer get direction test completed \n");
}

/*******************************************************************************
 * Function to test set count api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_set_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer set-count api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  uint32_t timer_match_value     = SL_ULP_TIMER_MATCH_VALUE_DEFAULT;

  // First stopping timer, before changing its parameters
  sl_si91x_ulp_timer_stop(timer_num);

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_set_count(5, timer_match_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  // Testing with valid parameter
  status = sl_si91x_ulp_timer_set_count(timer_num, timer_match_value);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("Timer-%d, type set \n", timer_num);
}

/*******************************************************************************
 * Function to test get count api of ulp-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_get_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer get-count api \n");

  sl_status_t status;
  ulp_timer_instance_t timer_num = ULP_TIMER_0;
  uint32_t count_value           = 0;

  // Testing with invalid timer-number
  status = sl_si91x_ulp_timer_get_count(5, &count_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index-parameter \n");

  // Testing with NULL Pointer
  status = sl_si91x_ulp_timer_get_count(timer_num, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Testing with valid parameter
  count_value = 0;
  status      = sl_si91x_ulp_timer_get_count(timer_num, &count_value);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("ULP-Timer get count test completed \n");
}

/*******************************************************************************
 * Function to test the ulp-timer timeout callback register api
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_register_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer timeout callback register Api \n");
  sl_status_t status;
  ulp_timer_config_t timer_handle1;

  timer_handle1.timer_num         = ULP_TIMER_0;
  timer_handle1.timer_mode        = SL_ULP_TIMER_MODE_PERIODIC;
  timer_handle1.timer_type        = SL_ULP_TIMER_TYP_1US;
  timer_handle1.timer_match_value = 1000;
  timer_handle1.timer_direction   = DOWN_COUNTER;

  status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  status = sl_si91x_ulp_timer_register_timeout_callback(5, on_timeout_callback0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_INDEX, status);
  UnityPrintf("Status of API is correct, tested with invalid index parameter \n");

  sl_si91x_ulp_timer_config(&timer_handle1);

  status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_0, on_timeout_callback0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  status = sl_si91x_ulp_timer_start(ULP_TIMER_0);
  UnityPrintf("Timer timeout registered callback \n");
  TEST_ASSERT_TRUE(timer0_timeout_flag);
  UnityPrintf("Timer callback register test completed \n");
}

/*******************************************************************************
 * Function to test the ulp-timer timeout callback unregister api
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_unregister_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer timeout callback unregister Api \n");
  sl_status_t status;
  if (timer0_timeout_flag == true) {
    status = sl_si91x_ulp_timer_unregister_timeout_callback(ULP_TIMER_0);
    TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
    UnityPrintf("Status of API is correct, tested with valid parameters \n");
    UnityPrintf("Timer timeout unregistered callback \n");
    timer0_timeout_flag = false;
  }
  TEST_ASSERT_FALSE(timer0_timeout_flag);
  UnityPrintf("Timer timeout unregistered test completed \n");
}

/*******************************************************************************
 * Function to test the xtal clock configuration for ULP timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_configure_xtal_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer xtal clock configure api \n");
  uint8_t xtal_pin = 1;
  sl_status_t status;

  // testing with invalid parameters
  status = sl_si91x_ulp_timer_configure_xtal_clock(5);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");
  // testing with valid parameters
  status = sl_si91x_ulp_timer_configure_xtal_clock(xtal_pin);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  UnityPrintf("ULP-Timer xtal clock configuration test completed \n");
}

/*******************************************************************************
 * Function to test the xtal clock configuration for ULP timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_ulp_timer_configure_soc_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing ulp-timer soc clock configure api \n");

  sl_status_t status;
  boolean_t div_factor_type = false;
  uint16_t div_factor       = 2;

  // testing with invalid 'div_factor' for even type division factor
  status = sl_si91x_ulp_timer_configure_soc_clock(0, 3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");
  // testing with valid 'div_factor' for odd type division factor
  status = sl_si91x_ulp_timer_configure_soc_clock(1, 2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  // testing with valid parameters
  status = sl_si91x_ulp_timer_configure_soc_clock(0, 2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_HEX(M4CLK->CLK_CONFIG_REG4_b.ULPSS_CLK_DIV_FAC, div_factor);
  UnityPrintf("division factor is configured properly\n");
  TEST_ASSERT_EQUAL_HEX(M4CLK->CLK_CONFIG_REG5_b.ULPSS_ODD_DIV_SEL, div_factor_type);
  UnityPrintf("division factor type is configured properly\n");
  TEST_ASSERT_EQUAL_HEX((M4CLK->CLK_ENABLE_SET_REG1 >> 31), 1);
  UnityPrintf("SOC clock is enabled properly\n");
  UnityPrintf("ULP-Timer soc clock configuration test completed \n");
}

/*******************************************************************************
* Internal Function to disable ulp-timer interrupt.
*
* @param[in]  timer_num enum for ULP-timer Number (0 to 3)
* @return    status 0 if successful, else error code
*         SL_STATUS_INVALID_INDEX (0x0027) - Invalid index.
*         SL_STATUS_ALLOCATION_FAILED (0x0019) - Generic allocation error.
*         SL_STATUS_OK (0x0000) - Success
*******************************************************************************/
sl_status_t ulp_timer_disable_interrupt(ulp_timer_instance_t timer_num)
{
  sl_status_t status;
  error_code_t error_status;
  status = SL_STATUS_OK;
  do {
    if (timer_num >= ULP_TIMER_LAST) {
      status = SL_STATUS_INVALID_INDEX;
      break;
    }
    error_status = RSI_TIMERS_InterruptDisable(TIMERS, timer_num);
    if (error_status == ERROR_INVAL_TIMER_NUM) {
      status = SL_STATUS_INVALID_INDEX;
      break;
    }
    //reading register value
    if (TIMERS->MATCH_CTRL[timer_num].MCUULP_TMR_CNTRL_b.TMR_INTR_ENABLE != DISABLE) {
      status = SL_STATUS_ALLOCATION_FAILED;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * Callback function of timer0 timeout interrupt
 * 
 * @param none
 * @return none
 ******************************************************************************/
void on_timeout_callback0(void)
{
  DEBUGOUT("on timeout callback of timer0 \n");
  timer0_timeout_flag = true;
  ulp_timer_disable_interrupt(0);
}
