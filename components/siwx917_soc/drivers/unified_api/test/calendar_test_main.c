/***************************************************************************/ /**
 * @file calendar_test_main.c
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

#include "sl_si91x_calendar.h"
#include "unity.h"
#include "sl_si91x_ulp_timer_init.h"
#include "sl_system_init.h"

/*******************************************************************************
 * This file is for testing the calendar API using Unity Framework
 * @note: 
 *  - Go to UC screen and add instance for ULP timer
 *  - Select 'timer 0' and counter direction as 'UP counter'
 ******************************************************************************/

#define TIMER_0  0     // Timer 0
#define ONE_MSEC 32000 // Ticks required for every one millisecond

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
boolean_t sec_flag   = true;
boolean_t alarm_flag = true;
boolean_t msec_flag  = true;
uint32_t msec_count  = 0;

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static bool is_date_equal(sl_calendar_datetime_config_t *date1, sl_calendar_datetime_config_t *date2);
static void set_clock(void);
static void on_callback(void);
static void on_callback_msec(void);
static void on_callback_alarm(void);
static void delay(uint16_t milliseconds);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_calendar_init(void);
void test_rtc_start(void);
void test_rtc_stop(void);
void test_clk_calibration(void);
void test_build_structure(void);
void test_calendar_set_and_get_datetime(void);
void test_set_and_get_alarm(void);
void test_sec_trigger(void);
void test_msec_trigger(void);
void test_time_conversion(void);
void test_gspi_get_version(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  sl_si91x_ulp_timer_configure_clock(&sl_timer_clk_handle);
  sl_si91x_ulp_timer_set_configuration(&sl_timer_handle_timer0);
  sl_si91x_ulp_timer_set_count(TIMER_0, ONE_MSEC * 7000);
  sl_si91x_ulp_timer_start(TIMER_0);

  UnityBeginGroup("CALENDAR");

  RUN_TEST(test_calendar_init, __LINE__);
  RUN_TEST(test_rtc_start, __LINE__);
  RUN_TEST(test_rtc_stop, __LINE__);
  RUN_TEST(test_clk_calibration, __LINE__);
  RUN_TEST(test_build_structure, __LINE__);
  RUN_TEST(test_calendar_set_and_get_datetime, __LINE__);
  RUN_TEST(test_set_and_get_alarm, __LINE__);
  RUN_TEST(test_sec_trigger, __LINE__);
  RUN_TEST(test_msec_trigger, __LINE__);
  RUN_TEST(test_time_conversion, __LINE__);
  RUN_TEST(test_gspi_get_version, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the clock configuration and initialization of calendar
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_calendar_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar configuration \n");

  sl_status_t status = sl_si91x_calendar_set_configuration(KHZ_RC_CLK_SEL);
  sl_si91x_calendar_init();
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Configuration with RC Clock passed \n");

  status = sl_si91x_calendar_set_configuration(KHZ_RO_CLK_SEL);
  sl_si91x_calendar_init();
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Configuration with RO Clock passed \n");

  UnityPrintf("Calendar configuration test completed \n");
}

/*******************************************************************************
 * Function to test the start api of calendar
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_rtc_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar start \n");
  sl_si91x_calendar_rtc_start();
  TEST_ASSERT(RTC->MCU_CAL_POWERGATE_REG_b.PG_EN_CALENDER == 1u);
  UnityPrintf("Calendar start test completed \n");
}

/*******************************************************************************
 * Function to test the stop api of calendar
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_rtc_stop(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar stop \n");
  sl_si91x_calendar_rtc_stop();
  TEST_ASSERT(RTC->MCU_CAL_POWERGATE_REG_b.PG_EN_CALENDER == 0u);
  sl_si91x_calendar_rtc_start();
  UnityPrintf("Calendar stop test completed \n");
}

/*******************************************************************************
 * Function to test the clock calibration of calendar
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_clk_calibration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar clock calibration \n");

  clock_calibration_config_t clock_calibration_config;
  sl_status_t status;
  clock_calibration_config.rc_enable_calibration          = true;
  clock_calibration_config.rc_enable_periodic_calibration = true;
  clock_calibration_config.rc_trigger_time                = SL_RC_THIRTY_SEC;
  clock_calibration_config.ro_enable_calibration          = true;
  clock_calibration_config.ro_enable_periodic_calibration = true;
  clock_calibration_config.ro_trigger_time                = SL_RO_ONE_SEC;

  sl_si91x_calendar_calibration_init();

  status = sl_si91x_calendar_roclk_calibration(&clock_calibration_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct \n");
  TEST_ASSERT(TIME_PERIOD->MCU_CAL_TEMP_PROG_REG_b.RTC_TIMER_PERIOD_MUX_SEL == ENABLE);
  UnityPrintf("RC clock register is configured properly \n");
  TEST_ASSERT(clock_calibration_config.rc_enable_periodic_calibration
              == TIME_PERIOD->MCU_CAL_START_REG_b.PERIODIC_RC_CALIB_EN);
  UnityPrintf("RC Period calibration is enabled  \n");
  TEST_ASSERT(clock_calibration_config.rc_trigger_time == TIME_PERIOD->MCU_CAL_START_REG_b.RC_TRIGGER_TIME_SEL);
  UnityPrintf("RC trigger time is configured properly\n");

  TEST_ASSERT(clock_calibration_config.ro_enable_periodic_calibration
              == TIME_PERIOD->MCU_CAL_START_REG_b.PERIODIC_RO_CALIB_EN);
  UnityPrintf("RO Period calibration is enabled  \n");
  TEST_ASSERT_EQUAL_INT(clock_calibration_config.ro_trigger_time, TIME_PERIOD->MCU_CAL_START_REG_b.RO_TRIGGER_TIME_SEL);
  UnityPrintf("RO trigger time is configured properly\n");

  UnityPrintf("Calendar clock calibration test completed \n");
}

/*******************************************************************************
 * Function to test the build structure api which fills the structure with user inputs
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_build_structure(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar build datetime structure \n");

  sl_calendar_datetime_config_t refdate, test_date;
  sl_status_t status;
  //Entering proper value

  status = sl_si91x_calendar_build_datetime_struct(&refdate, 2, 23, March, Tuesday, 7, 15, 30, 10, 100);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  //29 feb on non-leap year, should return invalid para
  status = sl_si91x_calendar_build_datetime_struct(&refdate, 2, 23, Febuary, Friday, 30, 18, 10, 0, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");

  // 07-03-2023 15:30:10
  refdate.Day          = 7;
  refdate.DayOfWeek    = Tuesday;
  refdate.Year         = 23;
  refdate.Century      = 2;
  refdate.Month        = March;
  refdate.Hour         = 15u;
  refdate.Minute       = 30u;
  refdate.Second       = 10u;
  refdate.MilliSeconds = 100u;

  status = sl_si91x_calendar_build_datetime_struct(&test_date, 23, March, Tuesday, 7, 15, 30, 10, 100);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL(true, is_date_equal(&refdate, &test_date));
  UnityPrintf("Manual datetime structure and API datetime structure are same \n");

  // Invalid Para

  status = sl_si91x_calendar_build_datetime_struct(&test_date, 2, 23, March, Tuesday, 7, 23 + 1, 30, 10, 100);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");

  UnityPrintf("Calendar build datetime structure test completed \n");
}

/*******************************************************************************
 * Function to test the set and get api of calendar datetime
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_calendar_set_and_get_datetime(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar datetime functionality \n");

  sl_calendar_datetime_config_t date1, date2;
  sl_status_t status;
  sl_si91x_calendar_set_configuration(KHZ_RO_CLK_SEL);
  sl_si91x_calendar_init();

  date1.Day          = 7;
  date1.DayOfWeek    = Tuesday;
  date1.Year         = 23;
  date1.Century      = 2;
  date1.Month        = March;
  date1.Hour         = 15;
  date1.Minute       = 10;
  date1.Second       = 10;
  date1.MilliSeconds = 100;

  sl_si91x_calendar_rtc_start();
  status = sl_si91x_calendar_set_date_time(&date1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of set datetime API is correct\n");

  status = sl_si91x_calendar_get_date_time(&date2);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of get datetime API is correct\n");

  UnityPrintf("Comparing set datetime structure and get datetime structure \n");
  TEST_ASSERT_EQUAL_INT(date1.Day, date2.Day);
  TEST_ASSERT_EQUAL_INT(date1.Hour, date2.Hour);
  TEST_ASSERT_EQUAL_INT(date1.Minute, date2.Minute);
  TEST_ASSERT_EQUAL_INT(date1.Second, date2.Second);
  UnityPrintf("Compared all the values individually and they are correct \n");

  date1.Day          = 10;
  date1.DayOfWeek    = Tuesday;
  date1.Year         = 25;
  date1.Century      = 2;
  date1.Month        = June;
  date1.Hour         = 15u;
  date1.Minute       = 30u;
  date1.Second       = 10u;
  date1.MilliSeconds = 100u;

  status = sl_si91x_calendar_set_date_time(&date1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of set datetime API is correct\n");

  status = sl_si91x_calendar_get_date_time(&date2);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of get datetime API is correct \n");

  UnityPrintf("Comparing set datetime structure and get datetime structure \n");
  TEST_ASSERT_EQUAL_INT(date1.Day, date2.Day);
  TEST_ASSERT_EQUAL_INT(date1.Hour, date2.Hour);
  TEST_ASSERT_EQUAL_INT(date1.Minute, date2.Minute);
  TEST_ASSERT_EQUAL_INT(date1.Second, date2.Second);
  UnityPrintf("Compared all the values individually and they are correct \n");

  UnityPrintf("Calendar datetime functionality test completed \n");
}

/*******************************************************************************
 * Function to test the alarm functionality as well as alarm trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_set_and_get_alarm(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar alarm functionality \n");
  sl_calendar_datetime_config_t alarm, get_alrm;
  sl_status_t status;

  set_clock();

  alarm.Day          = 7;
  alarm.Year         = 23;
  alarm.Century      = 2;
  alarm.Month        = March;
  alarm.Hour         = 15u;
  alarm.Minute       = 30u;
  alarm.Second       = 12u;
  alarm.MilliSeconds = 10u;

  status = sl_si91x_calendar_set_alarm(&alarm);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of set alarm API is correct\n");

  sl_si91x_calendar_register_alarm_trigger_callback(on_callback_alarm);
  TEST_ASSERT_EQUAL_INT(RTC->MCU_CAL_ALARM_PROG_2_b.ALARM_EN, 1);
  UnityPrintf("Alarm Trigger is enabled successfully \n");

  status = sl_si91x_calendar_get_alarm(&get_alrm);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of get alarm API is correct\n");

  UnityPrintf("Comparing set alarm structure and get alarm structure \n");
  TEST_ASSERT_EQUAL_INT(alarm.Day, get_alrm.Day);
  TEST_ASSERT_EQUAL_INT(alarm.Year, get_alrm.Year);
  TEST_ASSERT_EQUAL_INT(alarm.Hour, get_alrm.Hour);
  TEST_ASSERT_EQUAL_INT(alarm.Minute, get_alrm.Minute);
  TEST_ASSERT_EQUAL_INT(alarm.Second, get_alrm.Second);
  UnityPrintf("Compared all the values individually and they are correct \n");
  alarm_flag = true;

  UnityPrintf("Testing alarm trigger \n");
  delay(5000);
  TEST_ASSERT_FALSE(alarm_flag);
  UnityPrintf("Alarm trigger registered callback \n");
  alarm_flag = true;
  delay(5000);

  sl_si91x_calendar_unregister_alarm_trigger_callback();
  UnityPrintf("Alarm trigger API disabled successfully \n");
  delay(3000);
  TEST_ASSERT_TRUE(alarm_flag);
  UnityPrintf("Alarm trigger didnot register callback \n");

  UnityPrintf("Calendar alarm functionality test completed \n");
}

/*******************************************************************************
 * Function to test the one second trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_sec_trigger(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar one second trigger \n");
  sl_status_t status;

  status = sl_si91x_calendar_register_sec_trigger_callback(on_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("One second trigger is enabled successfully \n");

  delay(1000);

  TEST_ASSERT_FALSE(sec_flag);
  UnityPrintf("One second trigger registered callback \n");
  sec_flag = true;

  sl_si91x_calendar_unregister_sec_trigger_callback();
  UnityPrintf("One second trigger API disabled successfully \n");
  delay(1000);
  TEST_ASSERT_TRUE(sec_flag);
  UnityPrintf("One second trigger did not register callback \n");

  UnityPrintf("Calendar one second trigger test completed \n");
}

/*******************************************************************************
 * Function to test the millisecond trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/

void test_msec_trigger(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar one millisecond trigger \n");
  sl_status_t status;

  delay(100);
  status = sl_si91x_calendar_register_msec_trigger_callback(on_callback_msec);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("One millisecond trigger is enabled successfully \n");

  TEST_ASSERT_FALSE(msec_flag);
  UnityPrintf("One millisecond trigger registered callback \n");
  msec_flag = true;

  sl_si91x_calendar_unregister_msec_trigger_callback();
  UnityPrintf("One millisecond trigger API disabled successfully \n");
  delay(2);
  TEST_ASSERT_TRUE(msec_flag);
  UnityPrintf("One millisecond trigger did not register callback \n");

  UnityPrintf("Calendar one millisecond trigger test completed \n");
}

/*******************************************************************************
 * Function to test the time conversion
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_time_conversion(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing calendar time format conversion \n");
  sl_status_t status;
  uint32_t unix = 981117600;
  uint32_t ntp  = 0;

  status = sl_si91x_calendar_convert_unix_time_to_ntp_time(2085978499, &ntp);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameters \n");

  status = sl_si91x_calendar_convert_unix_time_to_ntp_time(unix, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with null pointer \n");

  status = sl_si91x_calendar_convert_unix_time_to_ntp_time(unix, &ntp);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);

  uint32_t unix_new = 0;

  status = sl_si91x_calendar_convert_ntp_time_to_unix_time(ntp, &unix_new);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT_EQUAL_INT(unix, unix_new);
  UnityPrintf("Compared pervious and current unix time");

  status = sl_si91x_calendar_convert_ntp_time_to_unix_time(ntp, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);

  UnityPrintf("Calendar time format conversion test completed \n");
}

void test_gspi_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GSPI get version \n");
  sl_calendar_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_calendar_get_version();
  UnityPrintf("Verifying GSPI Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("GSPI get version test completed \n ");
}

/*******************************************************************************
 * Function to compare two datetime structures
 * 
 * @param[in] date1 First datetime structure
 * @param[in] date2 Second datetime structure
 * 
 * @return true/false, if they are same then true, else false
 ******************************************************************************/
static bool is_date_equal(sl_calendar_datetime_config_t *date1, sl_calendar_datetime_config_t *date2)
{
  bool res = true;

  //Perform comparison
  if (date1->Second != date2->Second) {
    res = false;
  } else if (date1->Minute != date2->Minute) {
    res = false;
  } else if (date1->Hour != date2->Hour) {
    res = false;
  } else if (date1->Day != date2->Day) {
    res = false;
  } else if (date1->DayOfWeek != date2->DayOfWeek) {
    res = false;
  } else if (date1->Month != date2->Month) {
    res = false;
  } else if (date1->Year != date2->Year) {
    res = false;
  }
  //Return comparison result
  return res;
}

/*******************************************************************************
 * Function to set the clock
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void set_clock(void)
{
  sl_calendar_datetime_config_t date1;
  date1.Day          = 7;
  date1.Year         = 23;
  date1.Century      = 2;
  date1.Month        = March;
  date1.Hour         = 15;
  date1.Minute       = 30;
  date1.Second       = 10;
  date1.MilliSeconds = 100;

  sl_si91x_calendar_set_date_time(&date1);
  sl_si91x_calendar_rtc_start();
}

/*******************************************************************************
 * Callaback function of sec trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void on_callback(void)
{
  sec_flag = false;
}

/*******************************************************************************
 * Callaback function of msec trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void on_callback_msec(void)
{
  msec_flag = false;
}

/*******************************************************************************
 * Callaback function of alarm trigger
 * 
 * @param none
 * @return none
 ******************************************************************************/
static void on_callback_alarm(void)
{
  //  sl_calendar_datetime_config_t alarm;
  alarm_flag = false;

  //  alarm.Day    = 7;
  //  alarm.Year   = 23; //25 + 2000
  //  alarm.Month  = March;
  //  alarm.Hour   = 15u;
  //  alarm.Minute = 30u;
  //  alarm.Second = 14u;
  //  sl_si91x_calendar_set_alarm(&alarm);
}

/*******************************************************************************
 * Function to generate delay
 * 
 * @param time_ms (uint16_t) time in second
 * @return none
 ******************************************************************************/
static void delay(uint16_t time_ms)
{
  uint32_t start_time;
  uint32_t temp;
  sl_si91x_ulp_timer_restart(TIMER_0);
  sl_si91x_ulp_timer_get_count(TIMER_0, &start_time);
  while (true) {
    sl_si91x_ulp_timer_get_count(TIMER_0, &temp);
    if ((temp - start_time) > (time_ms * ONE_MSEC)) {
      break;
    }
  }
}
