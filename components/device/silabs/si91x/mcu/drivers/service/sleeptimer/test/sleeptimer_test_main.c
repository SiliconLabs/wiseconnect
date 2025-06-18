/***************************************************************************/
/**
 * @file sleeptimer_hal_test_main.c
 * @brief Sleeptimer HAL Unity Test Functions
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
 ******************************************************************************/
#include "sl_sleeptimer.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TOOGLE_DELAY_MS1_ONESHOT  160000 //sleeptimer1 oneshot timeout in ticks
#define TOOGLE_DELAY_MS1_PERIODIC 400    //sleeptimer1 periodic timeout in ms

#define TIME_UNIX_EPOCH                     (1970u)
#define TIME_NTP_EPOCH                      (1900u)
#define TIME_ZIGBEE_EPOCH                   (2000u)
#define TIME_64_EPOCH                       TIME_NTP_EPOCH
#define TIME_NTP_UNIX_EPOCH_DIFF            (TIME_UNIX_EPOCH - TIME_NTP_EPOCH)
#define TIME_ZIGBEE_UNIX_EPOCH_DIFF         (TIME_ZIGBEE_EPOCH - TIME_UNIX_EPOCH)
#define TIME_DAY_COUNT_NTP_TO_UNIX_EPOCH    (TIME_NTP_UNIX_EPOCH_DIFF * 365u + 17u)   ///< 70 years and 17 leap days
#define TIME_DAY_COUNT_ZIGBEE_TO_UNIX_EPOCH (TIME_ZIGBEE_UNIX_EPOCH_DIFF * 365u + 7u) ///< 30 years and 7 leap days
#define TIME_SEC_PER_DAY                    (60u * 60u * 24u)
#define TIME_NTP_EPOCH_OFFSET_SEC           (TIME_DAY_COUNT_NTP_TO_UNIX_EPOCH * TIME_SEC_PER_DAY)
#define TIME_ZIGBEE_EPOCH_OFFSET_SEC        (TIME_DAY_COUNT_ZIGBEE_TO_UNIX_EPOCH * TIME_SEC_PER_DAY)
#define TIME_DAY_PER_YEAR                   (365u)
#define TIME_SEC_PER_YEAR                   (TIME_SEC_PER_DAY * TIME_DAY_PER_YEAR)
#define TIME_UNIX_TIMESTAMP_MAX             (0x7FFFFFFF)
#define TIME_64_BIT_UNIX_TIMESTAMP_MAX      (0x497968BD7F) /// Max 64 bit timestamp supported is 11:59:59 PM 12/31/11899
#define TIME_UNIX_YEAR_MAX                  (2038u - TIME_NTP_EPOCH)  ///< Max UNIX year based from a 1900 epoch
#define TIME_64_BIT_YEAR_MAX                (11899u - TIME_NTP_EPOCH) ///< Max 64 bit format year based from a 1900 epoch
#define TIME_64_TO_32_EPOCH_OFFSET_SEC      TIME_NTP_EPOCH_OFFSET_SEC
#define TIME_UNIX_TO_NTP_MAX                (0xFFFFFFFF - TIME_NTP_EPOCH_OFFSET_SEC)
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
//Sleeptimer timeout callbacks
static void on_timeout_timer(sl_sleeptimer_timer_handle_t *handle,
                             void *data); //sleeptimer1 callback
/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
sl_sleeptimer_timer_handle_t test_timer; //sleeptimer handle
static uint32_t max_ms    = 0;
bool test_oneshot_timeout = true;
/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sleeptimer_init(void);
void test_sleeptimer_get_tick_count(void);
void test_sleeptimer_get_tick_count64(void);
void test_sleeptimer_start_timer(void);
void test_sleeptimer_restart_timer(void);
void test_sleeptimer_is_timer_running(void);
void test_sleeptimer_start_periodic_timer(void);
void test_sleeptimer_restart_periodic_timer(void);
void test_sleeptimer_stop_timer(void);
void test_sleeptimer_get_timer_time_remaining(void);
void test_sleeptimer_get_remaining_time_of_first_timer(void);
void test_sleeptimer_convert_time_to_date(void);
void test_sleeptimer_convert_time_to_date64(void);
void test_sleeptimer_convert_date_to_time(void);
void test_sleeptimer_convert_date_to_time64(void);
void test_sleeptimer_convert_date_to_str(void);
void test_sleeptimer_get_timer_frequency(void);
void test_sleeptimer_get_clock_accuracy(void);
void test_sleeptimer_set_tz(void);
void test_sleeptimer_get_tz(void);
void test_sleeptimer_get_time(void);
void test_sleeptimer_get_time64(void);
void test_sleeptimer_set_time(void);
void test_sleeptimer_set_time64(void);
void test_sleeptimer_get_datetime(void);
void test_sleeptimer_set_datetime(void);
void test_sleeptimer_build_datetime(void);
void test_sleeptimer_build_datetime64(void);
void test_sleeptimer_convert_unix_time_to_ntp(void);
void test_sleeptimer_convert_ntp_time_to_unix(void);
void test_sleeptimer_convert_unix_time_to_zigbee(void);
void test_sleeptimer_convert_zigbee_time_to_unix(void);
void test_sleeptimer_delay_millisecond(void);
void test_sleeptimer_ms_to_tick(void);
void test_sleeptimer_ms32_to_tick(void);
void test_sleeptimer_get_max_ms32_conversion(void);
void test_sleeptimer_tick_to_ms(void);
void test_sleeptimer_tick64_to_ms(void);
void test_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire(void);
void test_sleeptimer_start_timer_ms(void);
void test_sleeptimer_restart_timer_ms(void);
void test_sleeptimer_start_periodic_timer_ms(void);
void test_sleeptimer_restart_periodic_timer_ms(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("Sleeptimer");

  RUN_TEST(test_sleeptimer_init, __LINE__);
  RUN_TEST(test_sleeptimer_get_tick_count, __LINE__);
  RUN_TEST(test_sleeptimer_get_tick_count64, __LINE__);
  RUN_TEST(test_sleeptimer_start_timer, __LINE__);
  RUN_TEST(test_sleeptimer_restart_timer, __LINE__);
  RUN_TEST(test_sleeptimer_start_periodic_timer, __LINE__);
  RUN_TEST(test_sleeptimer_restart_periodic_timer, __LINE__);
  RUN_TEST(test_sleeptimer_get_timer_frequency, __LINE__);
  RUN_TEST(test_sleeptimer_get_clock_accuracy, __LINE__);
  RUN_TEST(test_sleeptimer_is_timer_running, __LINE__);
  RUN_TEST(test_sleeptimer_get_timer_time_remaining, __LINE__);
  RUN_TEST(test_sleeptimer_get_remaining_time_of_first_timer, __LINE__);
  RUN_TEST(test_sleeptimer_set_tz, __LINE__);
  RUN_TEST(test_sleeptimer_set_time, __LINE__);
  RUN_TEST(test_sleeptimer_set_time64, __LINE__);
  RUN_TEST(test_sleeptimer_set_datetime, __LINE__);
  RUN_TEST(test_sleeptimer_build_datetime, __LINE__);
  RUN_TEST(test_sleeptimer_build_datetime64, __LINE__);
  RUN_TEST(test_sleeptimer_get_tz, __LINE__);
  RUN_TEST(test_sleeptimer_get_time, __LINE__);
  RUN_TEST(test_sleeptimer_get_time64, __LINE__);
  RUN_TEST(test_sleeptimer_get_datetime, __LINE__);
  RUN_TEST(test_sleeptimer_convert_time_to_date, __LINE__);
  RUN_TEST(test_sleeptimer_convert_time_to_date64, __LINE__);
  RUN_TEST(test_sleeptimer_convert_date_to_time, __LINE__);
  RUN_TEST(test_sleeptimer_convert_date_to_time64, __LINE__);
  RUN_TEST(test_sleeptimer_convert_date_to_str, __LINE__);
  RUN_TEST(test_sleeptimer_convert_unix_time_to_ntp, __LINE__);
  RUN_TEST(test_sleeptimer_convert_ntp_time_to_unix, __LINE__);
  RUN_TEST(test_sleeptimer_convert_unix_time_to_zigbee, __LINE__);
  RUN_TEST(test_sleeptimer_convert_zigbee_time_to_unix, __LINE__);
  RUN_TEST(test_sleeptimer_delay_millisecond, __LINE__);
  RUN_TEST(test_sleeptimer_get_max_ms32_conversion, __LINE__);
  RUN_TEST(test_sleeptimer_ms_to_tick, __LINE__);
  RUN_TEST(test_sleeptimer_ms32_to_tick, __LINE__);
  RUN_TEST(test_sleeptimer_tick_to_ms, __LINE__);
  RUN_TEST(test_sleeptimer_tick64_to_ms, __LINE__);
  RUN_TEST(test_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire, __LINE__);
  RUN_TEST(test_sleeptimer_start_timer_ms, __LINE__);
  RUN_TEST(test_sleeptimer_restart_timer_ms, __LINE__);
  RUN_TEST(test_sleeptimer_start_periodic_timer_ms, __LINE__);
  RUN_TEST(test_sleeptimer_restart_periodic_timer_ms, __LINE__);
  RUN_TEST(test_sleeptimer_stop_timer, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initialization of Sleeptimer
 ******************************************************************************/
void test_sleeptimer_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer Init Timer \n");
  sl_status_t status;

  status = sl_sleeptimer_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer Init Timer successfully \n");

  UnityPrintf("Sleeptimer Init Timer test completed \n");
}

/*******************************************************************************
 * Function to test getting the current timer count
 ******************************************************************************/
void test_sleeptimer_get_tick_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer Get Counter \n");
  uint32_t count;

  UnityPrintf("Testing with correct parameters \n");
  count = sl_sleeptimer_get_tick_count();
  UnityPrintf("Counter = %ld \n", count);
  UnityPrintf("Sleeptimer Get Counter successfully \n");

  UnityPrintf("Sleeptimer Get Counter test completed \n");
}

/*******************************************************************************
 * Function to test getting the current timer count for 64 bit.
 ******************************************************************************/
void test_sleeptimer_get_tick_count64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer Get Counter for 64 bit \n");
  uint64_t count;

  UnityPrintf("Testing with correct parameters \n");
  count = sl_sleeptimer_get_tick_count64();
  UnityPrintf("Counter = %ld \n", count);
  UnityPrintf("Sleeptimer Get Counter for 64 bit successfully \n");

  UnityPrintf("Sleeptimer Get Counter for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test the Sleeptimer start.
 ******************************************************************************/
void test_sleeptimer_start_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer start \n");
  sl_status_t status;

  status = sl_sleeptimer_start_timer(NULL,
                                     TOOGLE_DELAY_MS1_ONESHOT,
                                     on_timeout_timer,
                                     NULL,
                                     0,
                                     SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_start_timer(&test_timer,
                                     TOOGLE_DELAY_MS1_ONESHOT,
                                     on_timeout_timer,
                                     NULL,
                                     0,
                                     SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer start successfully \n");

  UnityPrintf("Testing with correct parameters to check timer is running or not \n");
  status = sl_sleeptimer_start_timer(&test_timer,
                                     TOOGLE_DELAY_MS1_ONESHOT,
                                     on_timeout_timer,
                                     NULL,
                                     0,
                                     SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_READY, status);
  UnityPrintf("Sleeptimer start timer to check timer is running or not successfully \n");

  UnityPrintf("Sleeptimer start test completed \n");
}

/*******************************************************************************
 * Function to test restart the Sleeptimer.
 ******************************************************************************/
void test_sleeptimer_restart_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer restart \n");
  sl_status_t status;

  status = sl_sleeptimer_restart_timer(NULL,
                                       TOOGLE_DELAY_MS1_ONESHOT,
                                       on_timeout_timer,
                                       NULL,
                                       0,
                                       SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_restart_timer(&test_timer,
                                       TOOGLE_DELAY_MS1_ONESHOT,
                                       on_timeout_timer,
                                       NULL,
                                       0,
                                       SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer restart successfully \n");

  UnityPrintf("Sleeptimer restart test completed \n");
}

/*******************************************************************************
 * Function to test timer is running or not.
 ******************************************************************************/
void test_sleeptimer_is_timer_running(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer is running \n");
  sl_status_t status;
  bool is_running = false;

  UnityPrintf("Testing with null parameters \n");
  status = sl_sleeptimer_is_timer_running(NULL, &is_running);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing null parameter successfully \n");

  UnityPrintf("Testing with null parameters \n");
  status = sl_sleeptimer_is_timer_running(&test_timer, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing null parameter successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_is_timer_running(&test_timer, &is_running);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("is_running = %d \n", is_running);
  UnityPrintf("Sleeptimer is running successfully \n");
  test_oneshot_timeout = false;

  UnityPrintf("Sleeptimer is running test completed \n");
}

/*******************************************************************************
 * Function to test the Sleeptimer start periodic timer
 ******************************************************************************/
void test_sleeptimer_start_periodic_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer start periodic timer \n");
  sl_status_t status;

  status = sl_sleeptimer_start_periodic_timer(NULL,
                                              TOOGLE_DELAY_MS1_ONESHOT,
                                              on_timeout_timer,
                                              NULL,
                                              0,
                                              SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  sl_sleeptimer_stop_timer(&test_timer);
  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_start_periodic_timer(&test_timer,
                                              TOOGLE_DELAY_MS1_ONESHOT,
                                              on_timeout_timer,
                                              NULL,
                                              0,
                                              SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer start periodic timer successfully \n");

  UnityPrintf("Testing with correct parameters to check timer is running or not \n");
  status = sl_sleeptimer_start_periodic_timer(&test_timer,
                                              TOOGLE_DELAY_MS1_ONESHOT,
                                              on_timeout_timer,
                                              NULL,
                                              0,
                                              SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_STATE, status);
  UnityPrintf("Sleeptimer start periodic timer to check timer is running or not successfully \n");

  UnityPrintf("Sleeptimer start periodic timer test completed \n");
}

/*******************************************************************************
 * Function to test get timer frequency
 ******************************************************************************/
void test_sleeptimer_get_timer_frequency(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get timer frequency \n");
  uint32_t timer_freq;

  timer_freq = sl_sleeptimer_get_timer_frequency();
  UnityPrintf("Timer Frequency = %ld \n", timer_freq);
  TEST_ASSERT_EQUAL_UINT32(DEFAULT_32KHZ_XTAL_CLOCK, timer_freq);
  UnityPrintf("Sleeptimer get timer frequency successfully \n");

  UnityPrintf("Sleeptimer get timer frequency completed \n");
}

/*******************************************************************************
 * Function to test restart the periodic timer
 ******************************************************************************/
void test_sleeptimer_restart_periodic_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer start periodic timer \n");
  sl_status_t status;

  status = sl_sleeptimer_restart_periodic_timer(NULL,
                                                TOOGLE_DELAY_MS1_ONESHOT,
                                                on_timeout_timer,
                                                NULL,
                                                0,
                                                SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_restart_periodic_timer(&test_timer,
                                                TOOGLE_DELAY_MS1_ONESHOT,
                                                on_timeout_timer,
                                                NULL,
                                                0,
                                                SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer start periodic timer successfully \n");

  UnityPrintf("Sleeptimer start periodic timer test completed \n");
}

/*******************************************************************************
 * Function to test get clock accuracy.
 ******************************************************************************/
void test_sleeptimer_get_clock_accuracy(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get clock accuracy \n");
  uint16_t clock = 0;

  clock = sl_sleeptimer_get_clock_accuracy();
  UnityPrintf("%ld \n", clock);
  UnityPrintf("Sleeptimer get clock accuracy successfully \n");

  UnityPrintf("Sleeptimer get clock accuracy test completed \n");
}

/*******************************************************************************
 * Function to test stop the Sleeptimer
 ******************************************************************************/
void test_sleeptimer_stop_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer stop \n");
  sl_status_t status;

  status = sl_sleeptimer_stop_timer(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_stop_timer(&test_timer);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer stopped successfully \n");

  UnityPrintf("Sleeptimer stop test completed \n");
}

/*******************************************************************************
 * Function to test get timer time remaining
 ******************************************************************************/
void test_sleeptimer_get_timer_time_remaining(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get time remaining \n");
  sl_status_t status;
  uint32_t time;

  status = sl_sleeptimer_get_timer_time_remaining(NULL, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  status = sl_sleeptimer_get_timer_time_remaining(&test_timer, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_get_timer_time_remaining(&test_timer, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer get time remaining successfully \n");

  UnityPrintf("Sleeptimer get time remaining test completed \n");
}

/*******************************************************************************
 * Function to test get remaining time of first timer
 ******************************************************************************/
void test_sleeptimer_get_remaining_time_of_first_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get remaining time of first timer \n");
  sl_status_t status;
  uint32_t time;

  UnityPrintf("Testing with correct parameters \n");
  status =
    sl_sleeptimer_get_remaining_time_of_first_timer(SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer get remaining time of first timer successfully \n");

  UnityPrintf("Sleeptimer get remaining time of first timer test completed \n");
}

/***************************************************************************/ /**
 * Function to test delay millisecond
 ******************************************************************************/
void test_sleeptimer_delay_millisecond(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer delay millisecond \n");
  uint16_t delay_ms = 5;

  sl_sleeptimer_delay_millisecond(delay_ms);
  UnityPrintf("Sleeptimer delay millisecond successfully \n");

  UnityPrintf("Sleeptimer delay millisecond completed \n");
}

/***************************************************************************/ /**
 * Function to test convert ms to tick.
 ******************************************************************************/
void test_sleeptimer_ms_to_tick(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer ms to tick \n");
  uint32_t tick_count = 0;
  uint16_t delay_ms   = 5;

  tick_count = sl_sleeptimer_ms_to_tick(delay_ms);
  UnityPrintf("%ld \n", tick_count);
  UnityPrintf("Sleeptimer ms to tick successfully \n");

  UnityPrintf("Sleeptimer ms to tick completed \n");
}

void test_sleeptimer_ms32_to_tick(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer ms to tick \n");
  sl_status_t status;
  uint32_t delay_ms = 5;
  uint32_t time     = 0;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_ms32_to_tick(delay_ms, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer ms to tick successfully \n");

  UnityPrintf("Testing with maximum ms \n");
  status = sl_sleeptimer_ms32_to_tick(max_ms + 1, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Sleeptimer invalid parameter successfully \n");

  UnityPrintf("Sleeptimer ms to tick test completed \n");
}

/***************************************************************************/ /**
 * Function to test get the maximum value that can be passed to the functions
 * that have a 32-bits time or timeout argument expressed in milliseconds.
 ******************************************************************************/
void test_sleeptimer_get_max_ms32_conversion(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer max ms \n");

  max_ms = sl_sleeptimer_get_max_ms32_conversion();
  UnityPrintf("Sleeptimer max ms successfully \n");

  UnityPrintf("Sleeptimer max ms test completed \n");
}

/*******************************************************************************
 * Function to test converts ticks in milliseconds.
 ******************************************************************************/
void test_sleeptimer_tick_to_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert tick to ms \n");
  uint32_t tick = 32000;
  uint32_t msec = 0;

  msec = sl_sleeptimer_tick_to_ms(tick);
  UnityPrintf("%ld \n", msec);
  UnityPrintf("Sleeptimer convert tick to ms successfully \n");

  UnityPrintf("Sleeptimer convert tick to ms test completed \n");
}

/*******************************************************************************
 * Function to test converts 64-bits ticks in milliseconds.
 ******************************************************************************/
void test_sleeptimer_tick64_to_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert 64-bits tick to ms \n");
  sl_status_t status;
  uint64_t tick = UINT64_MAX / 1000;
  uint64_t msec = 0;

  status = sl_sleeptimer_tick64_to_ms(tick + 1, &msec);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Sleeptimer testing with invalid parameter successfully \n");

  tick   = 32000;
  status = sl_sleeptimer_tick64_to_ms(tick, &msec);
  UnityPrintf("%ld \n", msec);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert 64-bits tick to ms successfully \n");

  UnityPrintf("Sleeptimer convert 64-bits tick to ms test completed \n");
}

/*******************************************************************************
 * Function to test if the power manager's early wakeup expired during the
 * last ISR.
 ******************************************************************************/
void test_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer is power manager early restore timer latest to expire \n");
  bool is_early_wakup_expired = false;

  is_early_wakup_expired = sl_sleeptimer_is_power_manager_early_restore_timer_latest_to_expire();
  UnityPrintf("%d \n", is_early_wakup_expired);
  UnityPrintf("Sleeptimer convert 64-bits tick to ms successfully \n");

  UnityPrintf("Sleeptimer convert 64-bits tick to ms test completed \n");
}

/*******************************************************************************
 * Function to test starts a 32 bits timer.
 ******************************************************************************/
void test_sleeptimer_start_timer_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer starts a 32 bits timer \n");
  sl_status_t status;

  sl_sleeptimer_stop_timer(&test_timer);
  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_start_timer_ms(&test_timer,
                                        TOOGLE_DELAY_MS1_ONESHOT,
                                        on_timeout_timer,
                                        NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer starts a 32 bits timer successfully \n");

  UnityPrintf("Sleeptimer starts a 32 bits timer test completed \n");
}

/*******************************************************************************
 * Function to test restarts a 32 bits timer.
 ******************************************************************************/
void test_sleeptimer_restart_timer_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer restarts a 32 bits timer \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_restart_timer_ms(&test_timer,
                                          TOOGLE_DELAY_MS1_ONESHOT,
                                          on_timeout_timer,
                                          NULL,
                                          0,
                                          SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer restarts a 32 bits timer successfully \n");

  UnityPrintf("Sleeptimer restarts a 32 bits timer test completed \n");
}

/*******************************************************************************
 * Function to test starts periodic timer ms.
 ******************************************************************************/
void test_sleeptimer_start_periodic_timer_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer starts periodic timer ms \n");
  sl_status_t status;

  status = sl_sleeptimer_start_periodic_timer_ms(NULL,
                                                 TOOGLE_DELAY_MS1_ONESHOT,
                                                 on_timeout_timer,
                                                 NULL,
                                                 0,
                                                 SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  sl_sleeptimer_stop_timer(&test_timer);
  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_start_periodic_timer_ms(&test_timer,
                                                 TOOGLE_DELAY_MS1_ONESHOT,
                                                 on_timeout_timer,
                                                 NULL,
                                                 0,
                                                 SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer starts periodic timer ms successfully \n");

  UnityPrintf("Sleeptimer starts periodic timer ms test completed \n");
}

/*******************************************************************************
 * Function to test restarts periodic timer ms.
 ******************************************************************************/
void test_sleeptimer_restart_periodic_timer_ms(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer restarts periodic timer ms \n");
  sl_status_t status;

  status = sl_sleeptimer_restart_periodic_timer_ms(NULL,
                                                   TOOGLE_DELAY_MS1_ONESHOT,
                                                   on_timeout_timer,
                                                   NULL,
                                                   0,
                                                   SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null pointer successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_sleeptimer_restart_periodic_timer_ms(&test_timer,
                                                   TOOGLE_DELAY_MS1_ONESHOT,
                                                   on_timeout_timer,
                                                   NULL,
                                                   0,
                                                   SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer restarts periodic timer ms successfully \n");

  UnityPrintf("Sleeptimer restarts periodic timer ms test completed \n");
}

/*******************************************************************************
 * Function to test set time zone offset.
 ******************************************************************************/
void test_sleeptimer_set_tz(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer set time zone offset \n");
  sl_sleeptimer_time_zone_offset_t test_time_zone_offset = 5;

  sl_sleeptimer_set_tz(test_time_zone_offset);
  UnityPrintf("Sleeptimer set time zone offset successfully\n");

  UnityPrintf("Sleeptimer set time zone offset test completed \n");
}

/*******************************************************************************
 * Function to test sets current time from 32 bit variable..
 ******************************************************************************/
void test_sleeptimer_set_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer set time \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_t time = 50;

  status = sl_sleeptimer_set_time(time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer set time successfully\n");

  UnityPrintf("Sleeptimer set time test completed \n");
}

/*******************************************************************************
 * Function to test sets current time from 64 bit variable..
 ******************************************************************************/
void test_sleeptimer_set_time64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer set time for 64 bit \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_64_t time = 50;

  status = sl_sleeptimer_set_time_64(time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer set time for 64 bit successfully\n");

  UnityPrintf("Sleeptimer set time for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test sets date and time..
 ******************************************************************************/
void test_sleeptimer_set_datetime(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer set date and time \n");
  sl_status_t status;
  sl_sleeptimer_date_t datetime;

  datetime.day_of_week = 4;
  datetime.day_of_year = 100;
  datetime.hour        = 11;
  datetime.min         = 29;
  datetime.month       = 5;
  datetime.month_day   = 20;
  datetime.sec         = 13;
  datetime.time_zone   = 5;
  datetime.year        = 1992;

  status = sl_sleeptimer_set_datetime(&datetime);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer set date and time successfully\n");

  UnityPrintf("Sleeptimer set date and time test completed \n");
}

/*******************************************************************************
 * Function to test build date and time..
 ******************************************************************************/
void test_sleeptimer_build_datetime(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer build date and time \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;

  date.day_of_week = 4;
  date.day_of_year = 100;
  date.hour        = 11;
  date.min         = 29;
  date.month       = 5;
  date.month_day   = 20;
  date.sec         = 13;
  date.time_zone   = 5;
  date.year        = 1992;

  status = sl_sleeptimer_build_datetime(NULL,
                                        date.year,
                                        date.month,
                                        date.month_day,
                                        date.hour,
                                        date.min,
                                        date.sec,
                                        date.time_zone);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully\n");

  status = sl_sleeptimer_build_datetime(&date,
                                        date.year,
                                        date.month,
                                        date.month_day,
                                        date.hour,
                                        date.min,
                                        date.sec,
                                        date.time_zone);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer build date and time successfully\n");

  UnityPrintf("Sleeptimer build date and time test completed \n");
}

/*******************************************************************************
 * Function to test build date and time for 64 bit..
 ******************************************************************************/
void test_sleeptimer_build_datetime64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer build date and time for 64 bit \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;

  date.day_of_week = 4;
  date.day_of_year = 100;
  date.hour        = 11;
  date.min         = 29;
  date.month       = 5;
  date.month_day   = 20;
  date.sec         = 13;
  date.time_zone   = 5;
  date.year        = 1992;

  status = sl_sleeptimer_build_datetime_64(NULL,
                                           date.year,
                                           date.month,
                                           date.month_day,
                                           date.hour,
                                           date.min,
                                           date.sec,
                                           date.time_zone);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully\n");

  status = sl_sleeptimer_build_datetime_64(&date,
                                           date.year,
                                           date.month,
                                           date.month_day,
                                           date.hour,
                                           date.min,
                                           date.sec,
                                           date.time_zone);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer build date and time for 64 bit successfully\n");

  UnityPrintf("Sleeptimer build date and time for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test get date and time..
 ******************************************************************************/
void test_sleeptimer_get_datetime(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get date and time \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;

  status = sl_sleeptimer_get_datetime(&date);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Day_of_week = %ld \n", date.day_of_week);
  UnityPrintf("Day_of_year = %ld \n", date.day_of_year);
  UnityPrintf("Hour = %ld \n", date.hour);
  UnityPrintf("Minute = %ld \n", date.min);
  UnityPrintf("Month = %ld \n", date.month);
  UnityPrintf("Month_of_day = %ld \n", date.month_day);
  UnityPrintf("Second = %ld \n", date.sec);
  UnityPrintf("Year = %ld \n", date.year);
  UnityPrintf("Time zone = %ld \n", date.time_zone);
  UnityPrintf("Sleeptimer get date and time successfully\n");

  UnityPrintf("Sleeptimer get date and time test completed \n");
}

/*******************************************************************************
 * Function to test convert time to date..
 ******************************************************************************/
void test_sleeptimer_convert_time_to_date(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert time to date \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;
  sl_sleeptimer_timestamp_t time             = 0;
  sl_sleeptimer_time_zone_offset_t time_zone = 5;

  status = sl_sleeptimer_convert_time_to_date(time, time_zone, &date);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert time to date successfully\n");

  UnityPrintf("Sleeptimer convert time to date test completed \n");
}

/*******************************************************************************
 * Function to test convert time to date for 64 bit..
 ******************************************************************************/
void test_sleeptimer_convert_time_to_date64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert time to date for 64 bit \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;
  sl_sleeptimer_timestamp_t time             = 0;
  sl_sleeptimer_time_zone_offset_t time_zone = 5;

  status = sl_sleeptimer_convert_time_to_date_64(time, time_zone, &date);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert time to date for 64 bit successfully\n");

  UnityPrintf("Sleeptimer convert time to date for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test convert date to time..
 ******************************************************************************/
void test_sleeptimer_convert_date_to_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert date to time \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;
  sl_sleeptimer_timestamp_t time;

  date.day_of_week = 4;
  date.day_of_year = 100;
  date.hour        = 11;
  date.min         = 29;
  date.month       = 5;
  date.month_day   = 20;
  date.sec         = 13;
  date.time_zone   = 5;
  date.year        = 1992;

  status = sl_sleeptimer_convert_date_to_time(&date, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert date to time successfully\n");

  UnityPrintf("Sleeptimer convert date to time test completed \n");
}

/*******************************************************************************
 * Function to test convert date to time for 64 bit..
 ******************************************************************************/
void test_sleeptimer_convert_date_to_time64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert date to time for 64 bit \n");
  sl_status_t status;
  sl_sleeptimer_date_t date;
  sl_sleeptimer_timestamp_64_t time;

  date.day_of_week = 4;
  date.day_of_year = 100;
  date.hour        = 11;
  date.min         = 29;
  date.month       = 5;
  date.month_day   = 20;
  date.sec         = 13;
  date.time_zone   = 5;
  date.year        = 1992;

  status = sl_sleeptimer_convert_date_to_time_64(&date, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert date to time for 64 bit successfully\n");

  UnityPrintf("Sleeptimer convert date to time for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test convert date to string..
 ******************************************************************************/
void test_sleeptimer_convert_date_to_str(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert date to string \n");
  sl_sleeptimer_date_t date;
  char str;
  size_t size = 10;
  const uint8_t format;
  uint32_t str_size;
  date.day_of_week = 4;
  date.day_of_year = 100;
  date.hour        = 11;
  date.min         = 29;
  date.month       = 5;
  date.month_day   = 20;
  date.sec         = 13;
  date.time_zone   = 5;
  date.year        = 1992;

  str_size = sl_sleeptimer_convert_date_to_str(&str, size, &format, &date);
  UnityPrintf("String size = %ld \n", str_size);
  UnityPrintf("Sleeptimer convert date to string successfully\n");

  UnityPrintf("Sleeptimer convert date to string test completed \n");
}

/*******************************************************************************
 * Function to test convert unix to ntp time..
 ******************************************************************************/
void test_sleeptimer_convert_unix_time_to_ntp(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert unix to ntp time \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_t time = TIME_UNIX_TO_NTP_MAX;
  uint32_t ntp_time;

  status = sl_sleeptimer_convert_unix_time_to_ntp(time + 1, &ntp_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Sleeptimer testing with invalid parameter successfully\n");

  status = sl_sleeptimer_convert_unix_time_to_ntp(time, &ntp_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert unix to ntp time successfully\n");

  UnityPrintf("Sleeptimer convert unix to ntp time test completed \n");
}

/*******************************************************************************
 * Function to test convert ntp to unix time..
 ******************************************************************************/
void test_sleeptimer_convert_ntp_time_to_unix(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert ntp to unix time \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_t time;
  uint32_t ntp_time = TIME_NTP_EPOCH_OFFSET_SEC;

  status = sl_sleeptimer_convert_ntp_time_to_unix(ntp_time, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert ntp to unix time successfully\n");

  UnityPrintf("Sleeptimer convert ntp to unix time test completed \n");
}

/*******************************************************************************
 * Function to test convert Unix timestamp into Zigbee timestamp...
 ******************************************************************************/
void test_sleeptimer_convert_unix_time_to_zigbee(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert Unix timestamp into Zigbee timestamp \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_t time = TIME_ZIGBEE_EPOCH_OFFSET_SEC;
  uint32_t zigbee_time;

  status = sl_sleeptimer_convert_unix_time_to_zigbee(time, &zigbee_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert Unix timestamp into Zigbee timestamp successfully\n");

  UnityPrintf("Sleeptimer convert Unix timestamp into Zigbee timestamp test completed \n");
}

/*******************************************************************************
 * Function to test convert Zigbee timestamp into Unix timestamp...
 ******************************************************************************/
void test_sleeptimer_convert_zigbee_time_to_unix(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer convert Zigbee timestamp into Unix timestamp \n");
  sl_status_t status;
  sl_sleeptimer_timestamp_t time;
  uint32_t zigbee_time = TIME_ZIGBEE_EPOCH_OFFSET_SEC;

  status = sl_sleeptimer_convert_zigbee_time_to_unix(zigbee_time, &time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Sleeptimer convert Zigbee timestamp into Unix timestamp successfully\n");

  UnityPrintf("Sleeptimer convert Zigbee timestamp into Unix timestamp test completed \n");
}

/*******************************************************************************
 * Function to test get current time from 64 bit variable..
 ******************************************************************************/
void test_sleeptimer_get_time64(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get time for 64 bit \n");
  sl_sleeptimer_timestamp_64_t time;

  time = sl_sleeptimer_get_time_64();
  UnityPrintf("Time zone offset = %ld \n", time);
  UnityPrintf("Sleeptimer get time for 64 bit successfully\n");

  UnityPrintf("Sleeptimer get time for 64 bit test completed \n");
}

/*******************************************************************************
 * Function to test get current time from 32 bit variable..
 ******************************************************************************/
void test_sleeptimer_get_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get time \n");
  sl_sleeptimer_timestamp_t time;

  time = sl_sleeptimer_get_time();
  UnityPrintf("Time zone offset = %ld \n", time);
  UnityPrintf("Sleeptimer get time successfully\n");

  UnityPrintf("Sleeptimer get time test completed \n");
}

/*******************************************************************************
 * Function to test get time zone offset.
 ******************************************************************************/
void test_sleeptimer_get_tz(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Sleeptimer get time zone offset \n");
  sl_sleeptimer_time_zone_offset_t test_time_zone_offset;

  test_time_zone_offset = sl_sleeptimer_get_tz();
  UnityPrintf("Time zone offset = %ld \n", test_time_zone_offset);
  UnityPrintf("Sleeptimer get time zone offset successfully\n");

  UnityPrintf("Sleeptimer get time zone offset test completed \n");
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout_timer(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  if (test_oneshot_timeout) {
    UnityPrintf("\r\n *******Timer1 oneshot timeout******* \r\n");
  } else {
    UnityPrintf("\r\n Timer1 periodic timeout \r\n");
  }
}
