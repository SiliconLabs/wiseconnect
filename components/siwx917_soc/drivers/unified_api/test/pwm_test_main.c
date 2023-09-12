/***************************************************************************/ /**
 * @file pwm_test_main.c
 * @brief PWM Unity Test Functions
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
#include "sl_si91x_pwm.h"
#include "sl_system_init.h"
#include "rsi_pwm.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define MAX_DIRECTION     2     // PWM maximum direction value
#define MAX_VALUE         2     // PWM maximum value
#define MAX_CHANNEL       4     // PWM maximum channel
#define MAX_MODE          6     // PWM mode
#define PRE_SCALE         7     // PWM pre-scalar value
#define MAX_PWM_OUTPUT    8     // PWM maximum output
#define POST_SCALE        16    // PWM post scalar value
#define MAX_FLAG          16    // PWM maximum flag
#define DUTY_CYCLE        50    // 50% duty cycle
#define PWM_FREQ          2500  // PWM frequency
#define DT_ENABLE         0x01  // Dead time enable for channel 0
#define INTR_EVENT        0x01  // Rise PWM time period match channel 0 event
#define DUTY_CYCLE_UPDATE 0x01  // Enable duty cycle updating bit in register
#define FAULT_A_ENABLE    0x11  // Fault A enable for channel 0
#define DT_COUNTER_A      0x00  // Dead time counter A enable
#define PRESCALE_A        0x100 // PWM Prescale_A value
#define DEADTIME_A        0x08  // PWM deadtime_A
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pwm_callback(uint16_t event);
uint32_t pwm_percentagetoticks(uint8_t percent, uint8_t chnl_num);

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
sl_pwm_init_t pwm_init;
/*******************************************************************************
 ************************  Test Function Prototypes ****************************
 ******************************************************************************/
void test_pwm_get_version(void);
void test_pwm_init(void);
void test_pwm_start(void);
void test_pwm_stop(void);
void test_pwm_set_time_period(void);
void test_pwm_trigger_special_event(void);
void test_pwm_configure_dead_time(void);
void test_pwm_reset_channel(void);
void test_pwm_reset_counter(void);
void test_pwm_control_period(void);
void test_pwm_control_fault(void);
void test_pwm_set_base_timer_mode(void);
void test_pwm_set_output_mode(void);
void test_pwm_set_read_counter(void);
void test_pwm_get_counter_direction(void);
void test_pwm_control_dead_time(void);
void test_pwm_clear_interrupt(void);
void test_pwm_register_timeout_callback(void);
void test_pwm_unregister_timeout_callback(void);
void test_pwm_control_duty_cycle(void);
void test_pwm_output_override(void);
void test_pwm_control_override(void);
void test_pwm_control_override_value(void);
void test_pwm_control_output_fault(void);
void test_pwm_control_special_event_trigger(void);
void test_pwm_select_dead_time(void);
void test_pwm_select_duty_cycle(void);
void test_pwm_enable_external_trigger(void);
void test_pwm_control_base_timer(void);
void test_pwm_get_time_period(void);
void test_pwm_deinit(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("PWM");

  RUN_TEST(test_pwm_init, __LINE__);
  RUN_TEST(test_pwm_control_base_timer, __LINE__);
  RUN_TEST(test_pwm_set_time_period, __LINE__);
  RUN_TEST(test_pwm_set_base_timer_mode, __LINE__);
  RUN_TEST(test_pwm_select_duty_cycle, __LINE__);
  RUN_TEST(test_pwm_control_duty_cycle, __LINE__);
  RUN_TEST(test_pwm_set_output_mode, __LINE__);
  RUN_TEST(test_pwm_register_timeout_callback, __LINE__);
  RUN_TEST(test_pwm_control_special_event_trigger, __LINE__);
  RUN_TEST(test_pwm_trigger_special_event, __LINE__);
  RUN_TEST(test_pwm_select_dead_time, __LINE__);
  RUN_TEST(test_pwm_control_dead_time, __LINE__);
  RUN_TEST(test_pwm_configure_dead_time, __LINE__);
  RUN_TEST(test_pwm_output_override, __LINE__);
  RUN_TEST(test_pwm_control_override, __LINE__);
  RUN_TEST(test_pwm_control_override_value, __LINE__);
  RUN_TEST(test_pwm_control_output_fault, __LINE__);
  RUN_TEST(test_pwm_control_fault, __LINE__);
  RUN_TEST(test_pwm_reset_channel, __LINE__);
  RUN_TEST(test_pwm_reset_counter, __LINE__);
  RUN_TEST(test_pwm_control_period, __LINE__);
  RUN_TEST(test_pwm_set_read_counter, __LINE__);
  RUN_TEST(test_pwm_get_counter_direction, __LINE__);
  RUN_TEST(test_pwm_enable_external_trigger, __LINE__);
  RUN_TEST(test_pwm_start, __LINE__);
  RUN_TEST(test_pwm_get_time_period, __LINE__);
  RUN_TEST(test_pwm_clear_interrupt, __LINE__);
  RUN_TEST(test_pwm_unregister_timeout_callback, __LINE__);
  RUN_TEST(test_pwm_get_version, __LINE__);
  RUN_TEST(test_pwm_stop, __LINE__);
  RUN_TEST(test_pwm_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the PWM initialize and clock
 ******************************************************************************/
void test_pwm_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM Init \n");
  sl_status_t status;

  pwm_init.port_l = 0;  // port 0 selected for pwm_l
  pwm_init.pin_l  = 6;  // Pin 6 selected for pwm_l
  pwm_init.port_h = 0;  // port 0 selected for pwm_h
  pwm_init.pin_h  = 65; // pin 65 selected for pwm_h
  pwm_init.mux_l  = 10; // pin 10 selected for pwm_l
  pwm_init.mux_h  = 8;  // mode 8 selected for pwm_h
  pwm_init.pad_l  = 1;  // pad 1 selected for pwm_l
  pwm_init.pad_h  = 22; // pad 22 selected for pwm_l

  UnityPrintf("Enable PWM initialize and clock \n");
  status = sl_si91x_pwm_init(&pwm_init);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Enable PWM initialize and clock completed \n");

  UnityPrintf("PWM Init completed \n");
}

/*******************************************************************************
 * Function to test the start of PWM
 ******************************************************************************/
void test_pwm_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM Start \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_start(MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_pwm_start(SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM start test completed \n");
}

/*******************************************************************************
 * Function to test the stop of PWM
 ******************************************************************************/
void test_pwm_stop(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM stop \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_stop(MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid channel parameter \n");
  status = sl_si91x_pwm_stop(SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM stop test completed \n");
}

/*******************************************************************************
 * Function to test the time period
 ******************************************************************************/
void test_pwm_set_time_period(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM set time period \n");
  sl_status_t status;
  uint16_t initial_value = 0;
  uint32_t rate          = 0;
  uint32_t freq          = PWM_FREQ;
  rate                   = SystemCoreClock / freq;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_set_time_period(MAX_CHANNEL, rate, initial_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_set_time_period(SL_CHANNEL_1, rate, initial_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM set time period test completed \n");
}

/*******************************************************************************
 * Function to test the PWM special event trigger
 ******************************************************************************/
void test_pwm_trigger_special_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM special event trigger \n");
  sl_status_t status;
  sl_si91x_pwm_svt_config_t pwm_config = { SL_TIME_PERIOD_POSTSCALE_1_1, PRESCALE_A, 0 };

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_pwm_trigger_special_event(SL_SVT_COUNT_UP, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with invalid direction parameter \n");
  status = sl_si91x_pwm_trigger_special_event(MAX_DIRECTION, &pwm_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_trigger_special_event(SL_SVT_COUNT_UP, &pwm_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM special event trigger test completed \n");
}

/*******************************************************************************
 * Function to test the dead time configuration
 ******************************************************************************/
void test_pwm_configure_dead_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM dead time configuration \n");
  sl_status_t status;
  sl_si91x_pwm_dt_config_t dead_time;
  dead_time.counterSelect = 0;
  dead_time.preScaleA     = 0;
  dead_time.preScaleB     = 0;
  dead_time.deadTimeA     = DEADTIME_A;
  dead_time.deadTimeB     = 0;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_pwm_configure_dead_time(NULL, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_configure_dead_time(&dead_time, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_configure_dead_time(&dead_time, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM dead time configuration test completed \n");
}

/*******************************************************************************
 * Function to test the PWM reset channel
 ******************************************************************************/
void test_pwm_reset_channel(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM reset channel \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_reset_channel(MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_reset_channel(SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM reset channel test completed \n");
}

/*******************************************************************************
 * Function to test the PWM reset counter
 ******************************************************************************/
void test_pwm_reset_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM reset counter \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_reset_counter(MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_reset_counter(SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM reset counter test completed \n");
}

/*******************************************************************************
 * Function to test the get version API
 ******************************************************************************/
void test_pwm_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM get version \n");
  sl_pwm_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_pwm_get_version();
  UnityPrintf("Verifying PWM Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("PWM get version test completed \n ");
}

/*******************************************************************************
 * Function to test the PWM set base time period control
 ******************************************************************************/
void test_pwm_control_period(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM set base time period control \n");
  sl_status_t status;
  uint32_t post_scale = 0, pre_scale = 0;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_control_period(post_scale, pre_scale, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid post scale parameter \n");
  status = sl_si91x_pwm_control_period(POST_SCALE, pre_scale, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid pre scale parameter \n");
  status = sl_si91x_pwm_control_period(post_scale, PRE_SCALE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_period(post_scale, pre_scale, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM set base time period control test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control fault A/B
 ******************************************************************************/
void test_pwm_control_fault(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control fault A/B \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid fault parameter \n");
  status = sl_si91x_pwm_control_fault(MAX_VALUE, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid fault parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid pwm_output parameter \n");
  status = sl_si91x_pwm_control_fault(SL_FAULTB, MAX_PWM_OUTPUT, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid value parameter \n");
  status = sl_si91x_pwm_control_fault(SL_FAULTB, SL_OUTPUT_LOW0, MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_fault(SL_FAULTB, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control fault A/B test completed \n");
}

/*******************************************************************************
 * Function to test the PWM set the mode of base timer
 ******************************************************************************/
void test_pwm_set_base_timer_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM set the mode of base timer \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_set_base_timer_mode(SL_FREE_RUN_MODE, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid mode parameter \n");
  status = sl_si91x_pwm_set_base_timer_mode(MAX_MODE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_set_base_timer_mode(SL_FREE_RUN_MODE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM set the mode of base timer test completed \n");
}

/*******************************************************************************
 * Function to test the PWM set output mode
 ******************************************************************************/
void test_pwm_set_output_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM set output mode \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_set_output_mode(SL_MODE_INDEPENDENT, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid mode parameter \n");
  status = sl_si91x_pwm_set_output_mode(MAX_MODE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_set_output_mode(SL_MODE_INDEPENDENT, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM set output mode test completed \n");
}

/*******************************************************************************
 * Function to test the PWM read the counter current value
 ******************************************************************************/
void test_pwm_set_read_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM read the counter current value \n");
  sl_status_t status;
  uint16_t *counter_value = (uint16_t *)2;

  UnityPrintf("Testing with Null Pointer parameter \n");
  status = sl_si91x_pwm_read_counter(NULL, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_read_counter(counter_value, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_read_counter(counter_value, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM read the counter current value test completed \n");
}

/*******************************************************************************
 * Function to test the PWM get time period counter direction
 ******************************************************************************/
void test_pwm_get_counter_direction(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM get time period counter direction \n");
  sl_status_t status;
  uint8_t *counter_direction = (uint8_t *)2;

  UnityPrintf("Testing with NULL parameter \n");
  status = sl_si91x_pwm_get_counter_direction(NULL, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, NULL POINTER Test Passed successfully \n");

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_get_counter_direction(counter_direction, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_get_counter_direction(counter_direction, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM get time period counter direction test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control dead time insertion
 ******************************************************************************/
void test_pwm_control_dead_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control dead time insertion \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid dead time parameter \n");
  status = sl_si91x_pwm_control_dead_time(SL_DEAD_TIME_LAST, DT_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_dead_time(SL_DEAD_TIME_ENABLE, DT_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control dead time insertion test completed \n");
}

/*******************************************************************************
 * Function to test the PWM clear the interrupts
 ******************************************************************************/
void test_pwm_clear_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM clear the interrupts \n");
  sl_status_t status;

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_clear_interrupt(INTR_EVENT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM clear the interrupts test completed \n");
}

/*******************************************************************************
 * Function to test the PWM enable the interrupts
 ******************************************************************************/
void test_pwm_register_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM enable the interrupts \n");
  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_pwm_register_callback(NULL, INTR_EVENT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_register_callback(pwm_callback, INTR_EVENT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM enable the interrupts test completed \n");
}

/*******************************************************************************
 * Function to test the PWM disable the interrupts
 ******************************************************************************/
void test_pwm_unregister_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM disable the interrupts \n");
  sl_status_t status;

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_unregister_callback(INTR_EVENT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM disable the interrupts test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control duty cycle
 ******************************************************************************/
void test_pwm_control_duty_cycle(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control duty cycle \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameter \n");
  status = sl_si91x_pwm_configure_duty_cycle(SL_DUTY_CYCLE_ENABLE, DUTY_CYCLE_UPDATE, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid duty_cycle parameter \n");
  status = sl_si91x_pwm_configure_duty_cycle(MAX_VALUE, DUTY_CYCLE_UPDATE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_configure_duty_cycle(SL_DUTY_CYCLE_ENABLE, DUTY_CYCLE_UPDATE, SL_CHANNEL_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control duty cycle test completed \n");
}

/*******************************************************************************
 * Function to test the PWM output override operation
 ******************************************************************************/
void test_pwm_output_override(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM output override operation \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid pwm output parameter \n");
  status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, MAX_PWM_OUTPUT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid override parameter \n");
  status = sl_si91x_pwm_output_override(MAX_VALUE, SL_OUTPUT_LOW0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, SL_OUTPUT_LOW0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM output override operation test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control the override control parameter
 ******************************************************************************/
void test_pwm_control_override(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control the override control parameter \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid value parameter \n");
  status = sl_si91x_pwm_control_override(SL_OVERRIDE_SET, MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid override parameter \n");
  status = sl_si91x_pwm_control_override(MAX_VALUE, SL_OP_OVERRIDE_UNSYNC);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_override(SL_OVERRIDE_SET, SL_OP_OVERRIDE_UNSYNC);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control the override control parameter test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control the override value
 ******************************************************************************/
void test_pwm_control_override_value(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control the override value \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid value parameter \n");
  status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_LOW0, MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid override parameter \n");
  status = sl_si91x_pwm_control_override_value(MAX_VALUE, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with invalid pwm output parameter \n");
  status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, MAX_PWM_OUTPUT, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, Invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control the override value test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control output fault override control parameters
 ******************************************************************************/
void test_pwm_control_output_fault(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control output fault override \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid output_fault parameters \n");
  status = sl_si91x_pwm_control_output_fault(MAX_VALUE, FAULT_A_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_output_fault(SL_OUTPUT_FAULT_SET, FAULT_A_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control output fault override test completed \n");
}

/*******************************************************************************
 * Function to test the PWM control generation of special event trigger
 ******************************************************************************/
void test_pwm_control_special_event_trigger(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM control generation of special event trigger \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid event parameter \n");
  status = sl_si91x_pwm_control_special_event_trigger(MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_special_event_trigger(SL_EVENT_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM control generation of special event trigger test completed \n");
}

/*******************************************************************************
 * Function to test the PWM select dead time control parameters
 ******************************************************************************/
void test_pwm_select_dead_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM select dead time control parameters \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid dead_time parameter \n");
  status = sl_si91x_pwm_select_dead_time(MAX_VALUE, DT_COUNTER_A);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_select_dead_time(SL_DEAD_TIME_ENABLE, DT_COUNTER_A);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM select dead time control parameters test completed \n");
}

/*******************************************************************************
 * Function to test the PWM select duty cycle
 ******************************************************************************/
void test_pwm_select_duty_cycle(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM select duty cycle \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid channel parameters \n");
  status = sl_si91x_pwm_set_duty_cycle(DUTY_CYCLE, MAX_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_set_duty_cycle(pwm_percentagetoticks(DUTY_CYCLE, PWM_CHNL_0), SL_CHANNEL_2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM select duty cycle test completed \n");
}

/*******************************************************************************
 * Function to test the PWM enable to use external trigger
 ******************************************************************************/
void test_pwm_enable_external_trigger(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM enable to use external trigger \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid enable parameters \n");
  status = sl_si91x_pwm_enable_external_trigger(MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_enable_external_trigger(SL_TRIGGER_ENABLE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM enable to use external trigger test completed \n");
}

/*******************************************************************************
 * Function to test the PWM select number of base timers
 ******************************************************************************/
void test_pwm_control_base_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM select number of base timers \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid base timer parameters \n");
  status = sl_si91x_pwm_control_base_timer(MAX_VALUE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_control_base_timer(SL_BASE_TIMER_EACH_CHANNEL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM select number of base timers test completed \n");
}

/*******************************************************************************
 * Function to test the PWM get time period
 ******************************************************************************/
void test_pwm_get_time_period(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing PWM get time period \n");
  sl_status_t status;
  uint16_t *time_period = (uint16_t *)2;

  UnityPrintf("Testing with Null pointer parameters \n");
  status = sl_si91x_pwm_get_time_period(SL_CHANNEL_1, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null pointer Test Passed successfully \n");

  UnityPrintf("Testing with invalid channel parameters \n");
  status = sl_si91x_pwm_get_time_period(MAX_CHANNEL, time_period);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, invalid Parameter Test Passed successfully \n");

  UnityPrintf("Testing with valid parameters \n");
  status = sl_si91x_pwm_get_time_period(SL_CHANNEL_1, time_period);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Valid Parameter Test Passed successfully \n");

  UnityPrintf("PWM get time period test completed \n");
}

/*******************************************************************************
 * Function to test the de-initialization of PWM
 ******************************************************************************/
void test_pwm_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing de-initialization of PWM \n");
  sl_si91x_pwm_deinit();
  UnityPrintf("de-initialization of PWM test completed \n");
}
/*******************************************************************************
 * Converts duty cycle percentage to system ticks
 ******************************************************************************/
uint32_t pwm_percentagetoticks(uint8_t percent, uint8_t chnl_num)
{
  uint16_t time_period = 0;
  uint32_t ticks       = 0;
  /* Get time period */
  sl_si91x_pwm_get_time_period(chnl_num, &time_period);
  ticks = (uint32_t)(time_period * percent) / 100;
  return ticks;
}

/*******************************************************************************
 * PWM callback to indicate any event
 ******************************************************************************/
static void pwm_callback(uint16_t event)
{
  switch (event) {
    case RISE_TIME_PERIOD_MATCH_CH0:
      break;

    case TIME_PERIOD_MATCH_CH0:
      break;

    case FAULT_A:
      break;

    case FAULT_B:
      break;

    case RISE_TIME_PERIOD_MATCH_CH1:
      break;

    case TIME_PERIOD_MATCH_CH1:
      break;

    case RISE_TIME_PERIOD_MATCH_CH2:
      break;

    case TIME_PERIOD_MATCH_CH2:
      break;

    case RISE_TIME_PERIOD_MATCH_CH3:
      break;

    case TIME_PERIOD_MATCH_CH3:
      break;
  }
}
