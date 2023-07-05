/***************************************************************************/ /**
 * @file watchdog_timer_test_main.c
 * @brief watchdog-Timer Unity Test Functions
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licenser of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************
 ******************************************************************************/

#include "sl_si91x_watchdog_timer.h"
#include "rsi_board.h"
#include "unity.h"
#include "sl_system_init.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#undef WDT_TIMER_UC
/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void on_timeout_callback();

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_watchdog_init_timer(void);
void test_watchdog_configure_clock(void);
void test_watchdog_set_configuration(void);
void test_watchdog_set_interrupt_time(void);
void test_watchdog_set_system_reset_time(void);
void test_watchdog_set_window_time(void);
void test_watchdog_get_interrupt_time(void);
void test_watchdog_get_system_reset_time(void);
void test_watchdog_get_window_time(void);
void test_watchdog_register_timeout_callback(void);
void test_watchdog_get_timer_system_reset_status(void);
void test_watchdog_enable_system_reset_on_processor_lockup(void);
void test_watchdog_disable_system_reset_on_processor_lockup(void);
void test_watchdog_start_timer(void);
void test_watchdog_restart_timer(void);
void test_watchdog_stop_timer(void);
void test_watchdog_unregister_timeout_callback(void);
void test_watchdog_deinit_timer(void);
void test_watchdog_timer_get_version(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("WATCHDOG-TIMER");

  RUN_TEST(test_watchdog_init_timer, __LINE__);
  RUN_TEST(test_watchdog_configure_clock, __LINE__);
  RUN_TEST(test_watchdog_set_configuration, __LINE__);
  RUN_TEST(test_watchdog_set_system_reset_time, __LINE__);
  RUN_TEST(test_watchdog_get_system_reset_time, __LINE__);
  RUN_TEST(test_watchdog_set_interrupt_time, __LINE__);
  RUN_TEST(test_watchdog_get_interrupt_time, __LINE__);
  RUN_TEST(test_watchdog_set_window_time, __LINE__);
  RUN_TEST(test_watchdog_get_window_time, __LINE__);
  RUN_TEST(test_watchdog_register_timeout_callback, __LINE__);
  RUN_TEST(test_watchdog_get_timer_system_reset_status, __LINE__);
  RUN_TEST(test_watchdog_enable_system_reset_on_processor_lockup, __LINE__);
  RUN_TEST(test_watchdog_disable_system_reset_on_processor_lockup, __LINE__);
  RUN_TEST(test_watchdog_start_timer, __LINE__);
  RUN_TEST(test_watchdog_restart_timer, __LINE__);
  RUN_TEST(test_watchdog_stop_timer, __LINE__);
  RUN_TEST(test_watchdog_unregister_timeout_callback, __LINE__);
  RUN_TEST(test_watchdog_deinit_timer, __LINE__);
  RUN_TEST(test_watchdog_timer_get_version, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test the init-API of watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_init_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog init timer-API \n");

  sl_si91x_watchdog_init_timer();
  // Testing interrupt mask register value after deinit
  TEST_ASSERT((NPSS_INTR_MASK_CLR_REG & NPSS_TO_MCU_WDT_INTR) == 0);
  UnityPrintf("Watchdog-timer interrupt is unmasked properly\n");
  // Testing wdt enabled for sleep mode or not
  TEST_ASSERT(MCU_FSM->MCU_FSM_CRTL_PDM_AND_ENABLES_b.ENABLE_WDT_IN_SLEEP_b == 1);
  UnityPrintf("Watchdog-timer is enabled properly to run during sleep mode \n");
  UnityPrintf("Watchdog-timer is initialized properly \n");
  UnityPrintf("Watchdog-Timer init-API test completed \n");
}

/*******************************************************************************
 * Function to test the clock-configure API of watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_configure_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog configure clock API \n");
  sl_status_t status;
  watchdog_timer_clock_config_t timer_clk_handle1;
  watchdog_timer_clock_config_t timer_clk_handle2;
  watchdog_timer_clock_config_t timer_clk_handle3;
  watchdog_timer_clock_config_t timer_clk_handle4;
  watchdog_timer_clock_config_t *timer_clk_handle5 = NULL;

  // handle for all valid parameters
  timer_clk_handle1.low_freq_fsm_clock_src  = KHZ_RO_CLK_SEL;
  timer_clk_handle1.high_freq_fsm_clock_src = FSM_32MHZ_RC;
  timer_clk_handle1.bg_pmu_clock_source     = RO_32KHZ_CLOCK;
  // handle with invalid low_freq_fsm_clock_src value
  timer_clk_handle2.low_freq_fsm_clock_src  = 3;
  timer_clk_handle2.high_freq_fsm_clock_src = FSM_32MHZ_RC;
  timer_clk_handle2.bg_pmu_clock_source     = MCU_FSM__CLOCK;
  // handle for invalid high_freq_fsm_clock_src value
  timer_clk_handle3.low_freq_fsm_clock_src  = KHZ_RO_CLK_SEL;
  timer_clk_handle3.high_freq_fsm_clock_src = 3;
  timer_clk_handle3.bg_pmu_clock_source     = RO_32KHZ_CLOCK;
  // handle with invalid bg_pmu_clock_source value
  timer_clk_handle4.low_freq_fsm_clock_src  = KHZ_RO_CLK_SEL;
  timer_clk_handle4.high_freq_fsm_clock_src = 3;
  timer_clk_handle4.bg_pmu_clock_source     = RO_32KHZ_CLOCK;

  // testing with NULL pointer
  status = sl_si91x_watchdog_configure_clock(timer_clk_handle5);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");
  // testing with invalid low freq clock source parameter
  status = sl_si91x_watchdog_configure_clock(&timer_clk_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid low frequency clock parameter \n");
  // testing with invalid high freq clock source parameter
  status = sl_si91x_watchdog_configure_clock(&timer_clk_handle3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid high frequency clock parameter \n");
  // testing with invalid bg_pmu_clock_source parameter
  status = sl_si91x_watchdog_configure_clock(&timer_clk_handle4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid bg_pmu clock parameter \n");
  // testing with valid parameters
  status = sl_si91x_watchdog_configure_clock(&timer_clk_handle1);
  TEST_ASSERT(MCU_AON->MCUAON_KHZ_CLK_SEL_POR_RESET_STATUS_b.AON_KHZ_CLK_SEL == KHZ_RO_CLK_SEL);
  UnityPrintf("low frequency clock value is configured properly\n");
  TEST_ASSERT(MCU_FSM->MCU_FSM_CLKS_REG_b.HF_FSM_CLK_SELECT == FSM_32MHZ_RC);
  UnityPrintf("high frequency clock value is configured properly\n");
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid clock parameters \n");
  UnityPrintf("Watchdog-Timer config-API test completed \n");
}

/*******************************************************************************
 * Function to test the config API of watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_set_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog set configuration API \n");
  sl_status_t status;
  watchdog_timer_config_t timer_handle1;
  watchdog_timer_config_t timer_handle2;
  watchdog_timer_config_t timer_handle3;
  watchdog_timer_config_t timer_handle4;
  watchdog_timer_config_t timer_handle5;
  watchdog_timer_config_t timer_handle6;

  // handle for all valid parameters
  timer_handle1.interrupt_time    = TIME_DELAY_15;
  timer_handle1.system_reset_time = TIME_DELAY_17;
  timer_handle1.window_time       = TIME_DELAY_0;
  // handle for invalid interrupt time parameters
  timer_handle2.interrupt_time    = 38;
  timer_handle2.system_reset_time = TIME_DELAY_17;
  timer_handle2.window_time       = TIME_DELAY_0;
  // handle for invalid system-reset time parameters
  timer_handle3.interrupt_time    = TIME_DELAY_15;
  timer_handle3.system_reset_time = 59;
  timer_handle3.window_time       = TIME_DELAY_0;
  // handle for invalid window time parameters
  timer_handle4.interrupt_time    = TIME_DELAY_15;
  timer_handle4.system_reset_time = TIME_DELAY_17;
  timer_handle4.window_time       = 20;
  // handle for system-reset time less than interrupt time parameter
  timer_handle5.interrupt_time    = 18;
  timer_handle5.system_reset_time = 14;
  timer_handle5.window_time       = 0;
  // handle for interrupt time less than window time parameter
  timer_handle6.interrupt_time    = 10;
  timer_handle6.system_reset_time = 16;
  timer_handle6.window_time       = 12;

  // testing with invalid interrupt time parameters
  status = sl_si91x_watchdog_set_configuration(&timer_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  //TEST_ASSERT(status == SL_STATUS_INVALID_PARAMETER);
  UnityPrintf("Status of API is correct, tested with invalid interrupt time parameters \n");
  // Testing with invalid system-reset time parameters
  status = sl_si91x_watchdog_set_configuration(&timer_handle3);
  TEST_ASSERT(status == SL_STATUS_INVALID_PARAMETER);
  UnityPrintf("Status of API is correct, tested with invalid system-reset time parameters \n");
  // Testing with invalid window time parameters
  status = sl_si91x_watchdog_set_configuration(&timer_handle4);
  TEST_ASSERT(status == SL_STATUS_INVALID_PARAMETER);
  UnityPrintf("Status of API is correct, tested with invalid window time parameters \n");
  // Testing when system-reset time is less than interrupt time parameters
  status = sl_si91x_watchdog_set_configuration(&timer_handle5);
  TEST_ASSERT(status == SL_STATUS_INVALID_CONFIGURATION);
  UnityPrintf("Status of API is correct, tested with system-reset time less than interrupt time parameter\n");
  // Testing when interrupt time less than window time parameter
  status = sl_si91x_watchdog_set_configuration(&timer_handle6);
  TEST_ASSERT(status == SL_STATUS_INVALID_CONFIGURATION);
  UnityPrintf("Status of API is correct, tested with interrupt time less than window time parameter\n");
  // Testing with valid parameters
  status = sl_si91x_watchdog_set_configuration(&timer_handle1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  // Testing interrupt-timer bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_INTERRUPT_TIMER_b.WWD_INTERRUPT_TIMER == timer_handle1.interrupt_time);
  UnityPrintf("Watchdog-timer interrupt-time set properly\n");
  // Testing interrupt-timer bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_SYSTEM_RESET_TIMER_b.WWD_SYSTEM_RESET_TIMER == timer_handle1.system_reset_time);
  UnityPrintf("Watchdog-timer system reset-time set properly\n");
  // Testing interrupt-timer bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_WINDOW_TIMER_b.WINDOW_TIMER == timer_handle1.window_time);
  UnityPrintf("Watchdog-timer window-time set properly\n");
  UnityPrintf("Watchdog set configuration API test completed \n");
}

/*******************************************************************************
 * Function to test set system-reset-time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_set_system_reset_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog set system reset-time API \n");
  sl_status_t status;
  time_delays_t valid_system_reset_time   = TIME_DELAY_18;
  time_delays_t lesser_system_reset_time  = TIME_DELAY_14;
  time_delays_t invalid_system_reset_time = 38;
  // setting interrupt time as 15
  sl_si91x_watchdog_set_interrupt_time(TIME_DELAY_15);
  // Testing with system-reset time value less than interrupt time value
  status = sl_si91x_watchdog_set_system_reset_time(lesser_system_reset_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_CONFIGURATION, status);
  UnityPrintf("Status of API is correct, tested with sys-reset time less than interrupt time \n");
  // Testing with invalid system-reset time
  status = sl_si91x_watchdog_set_system_reset_time(invalid_system_reset_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");
  // Testing with valid system reset time
  status = sl_si91x_watchdog_set_system_reset_time(valid_system_reset_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Testing system-reset-time bits from register
  TEST_ASSERT(MCU_WDT->MCU_WWD_SYSTEM_RESET_TIMER_b.WWD_SYSTEM_RESET_TIMER == valid_system_reset_time);
  UnityPrintf("Watchdog-timer system reset-time set properly\n");
  UnityPrintf("Watchdog set system reset-time API test completed \n");
}

/*******************************************************************************
 * Function to test get system-reset-time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_get_system_reset_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog get system-reset time API \n");
  uint8_t system_reset_time;
  sl_si91x_watchdog_set_system_reset_time(TIME_DELAY_17);
  system_reset_time = sl_si91x_watchdog_get_system_reset_time();
  // Testing system-reset-time bits from register
  TEST_ASSERT(MCU_WDT->MCU_WWD_SYSTEM_RESET_TIMER_b.WWD_SYSTEM_RESET_TIMER == system_reset_time);
  UnityPrintf("Watchdog-timer system-reset time value read properly\n");
  UnityPrintf("Watchdog get system-reset time API test completed \n");
}

/*******************************************************************************
 * Function to test set interrupt-time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_set_interrupt_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog set interrupt-time API \n");
  sl_status_t status;
  time_delays_t valid_interrupt_time   = TIME_DELAY_15;
  time_delays_t greater_interrupt_time = TIME_DELAY_19;
  time_delays_t invalid_interrupt_time = 57;
  // Testing with interrupt-time greater than system reset time
  status = sl_si91x_watchdog_set_interrupt_time(greater_interrupt_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_CONFIGURATION, status);
  UnityPrintf("Status of API is correct, tested with interrupt time greater than sys-rst time\n");
  // Testing with invalid interrupt-time
  status = sl_si91x_watchdog_set_interrupt_time(invalid_interrupt_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");
  // Testing with valid interrupt-time
  status = sl_si91x_watchdog_set_interrupt_time(valid_interrupt_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Testing interrupt-time bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_INTERRUPT_TIMER_b.WWD_INTERRUPT_TIMER == valid_interrupt_time);
  UnityPrintf("Watchdog-timer interrupt-time set properly\n");
  UnityPrintf("Watchdog set interrupt-time API test completed \n");
}

/*******************************************************************************
 * Function to test get interrupt time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_get_interrupt_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog get interrupt time API \n");
  uint8_t interrupt_time;
  sl_si91x_watchdog_set_interrupt_time(TIME_DELAY_15);
  interrupt_time = sl_si91x_watchdog_get_interrupt_time();
  // Testing system-reset-time value from register
  TEST_ASSERT(MCU_WDT->MCU_WWD_INTERRUPT_TIMER_b.WWD_INTERRUPT_TIMER == interrupt_time);
  UnityPrintf("Watchdog-timer interrupt time value read properly\n");
  UnityPrintf("Watchdog get interrupt time API test completed \n");
}

/*******************************************************************************
 * Function to test set window-time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_set_window_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog set window-time API \n");
  sl_status_t status;
  time_delays_t valid_window_time   = TIME_DELAY_12;
  time_delays_t greater_window_time = TIME_DELAY_15;
  time_delays_t invalid_window_time = TIME_DELAY_18;
  // Testing with window time value greater than interrupt time
  status = sl_si91x_watchdog_set_window_time(greater_window_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_CONFIGURATION, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");
  // Testing with invalid window time
  status = sl_si91x_watchdog_set_window_time(invalid_window_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");
  // Testing with invalid window time
  status = sl_si91x_watchdog_set_window_time(invalid_window_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid parameter \n");
  // Testing with valid window time
  status = sl_si91x_watchdog_set_window_time(valid_window_time);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Testing window-time bits from register
  TEST_ASSERT(MCU_WDT->MCU_WWD_WINDOW_TIMER_b.WINDOW_TIMER == valid_window_time);
  UnityPrintf("Watchdog-timer window-time set properly\n");
  UnityPrintf("Watchdog set window-time API test completed \n");
}

/*******************************************************************************
 * Function to test get window time API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_get_window_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog get window time API \n");
  uint8_t interrupt_time;
  sl_si91x_watchdog_set_window_time(TIME_DELAY_12);
  interrupt_time = sl_si91x_watchdog_get_window_time();
  // Testing window time value from register
  TEST_ASSERT(MCU_WDT->MCU_WWD_WINDOW_TIMER_b.WINDOW_TIMER == interrupt_time);
  UnityPrintf("Watchdog-timer interrupt time value read properly\n");
  UnityPrintf("Watchdog get interrupt time API test completed \n");
}
/*******************************************************************************
* Function to test the ulp-timer timeout callback register api
*
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_register_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog register timeout callback API \n");
  sl_status_t status;
  // Testing for NULL pointer
  status = sl_si91x_watchdog_register_timeout_callback(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");
  // Testing with valid parameters
  status = sl_si91x_watchdog_register_timeout_callback(on_timeout_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Registering Callback again without unregistering, if status returns busy that means
  // callback is already registered
  status = sl_si91x_watchdog_register_timeout_callback(on_timeout_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, when registering an already registered callback \n");
  UnityPrintf("Watchdog register timeout callback API test completed \n");
}

/*******************************************************************************
 * Function to test get system-reset-status API of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_get_timer_system_reset_status(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog get timer system-reset status API \n");
  boolean_t reset_status = false;
  // Testing return value when WDT not resets system
  reset_status = sl_si91x_watchdog_get_timer_system_reset_status();
  TEST_ASSERT_FALSE(reset_status);
  UnityPrintf("API return status is correct, when watchdog-timer not resets system \n");
  UnityPrintf("Watchdog get timer system-reset status API test completed \n");
}

/*******************************************************************************
 * Function to test enabling of Watchdog-timer to reset system on processor lockup
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_enable_system_reset_on_processor_lockup(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog-timer system reset enable on processor lockup API \n");
  sl_si91x_watchdog_enable_system_reset_on_processor_lockup();
  TEST_ASSERT(MCU_WDT->MCU_WWD_ARM_STUCK_EN_b.PROCESSOR_STUCK_RESET_EN_ == 1);
  UnityPrintf("Watchdog-timer enabled properly to generate reset on processor lockup\n");
  UnityPrintf("Watchdog-timer system reset enable on processor lockup API test completed \n");
}

/*******************************************************************************
 * Function to test disabling of Watchdog-timer to reset system on processor lockup
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_disable_system_reset_on_processor_lockup(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog-timer system reset disable on processor lockup API \n");
  sl_si91x_watchdog_disable_system_reset_on_processor_lockup();
  TEST_ASSERT(MCU_WDT->MCU_WWD_ARM_STUCK_EN_b.PROCESSOR_STUCK_RESET_EN == 0);
  UnityPrintf("Watchdog-timer disabled properly to generate reset on processor lockup\n");
  UnityPrintf("Watchdog-timer system reset disable on processor lockup API test completed \n");
}
/*******************************************************************************
 * Function to test the start api of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_start_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog start timer API \n");
  // Starting watchdog-timer
  sl_si91x_watchdog_start_timer();
  // Testing timer enabling bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_EN_STATUS == 1);
  UnityPrintf("Watchdog-timer enabled and started properly\n");
  UnityPrintf("Watchdog start timer API test completed \n");
  // Stopping watchdog-timer
  sl_si91x_watchdog_stop_timer();
}

/*******************************************************************************
 * Function to test the restart api of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_restart_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog restart timer API \n");
  // Starting watchdog-timer
  sl_si91x_watchdog_start_timer();
  // Restarting already running watchdog-timer
  sl_si91x_watchdog_restart_timer();
  // Testing timer enabling bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_EN_STATUS == 1);
  UnityPrintf("Watchdog-timer enabled and restarted properly\n");
  // Testing timer start bit value
  UnityPrintf("Watchdog restart timer API test completed \n");
}

/*******************************************************************************
 * Function to test the stop api of Watchdog-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_stop_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog stop timer API \n");
  // Stopping watchdog-timer
  sl_si91x_watchdog_stop_timer();
  // Testing timer disabling bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_EN_STATUS == 0);
  UnityPrintf("Watchdog-timer disabled properly\n");
  UnityPrintf("Watchdog stop timer API test completed \n");
}

/*******************************************************************************
* Function to test the watchdog-timer timeout callback unregister api
*
 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_unregister_timeout_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog unregister timeout callback API \n");
  sl_status_t status;
  // Unregistering Callback and then registering again, if status comes OK after
  // registering that means callback was unregistered properly, else it would have
  // returned busy status
  sl_si91x_watchdog_unregister_timeout_callback();
  status = sl_si91x_watchdog_register_timeout_callback(on_timeout_callback);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Timer timeout unregistered callback properly \n");
  UnityPrintf("Watchdog unregister timeout callback API test completed \n");
  sl_si91x_watchdog_unregister_timeout_callback();
}

/*******************************************************************************
 * Function to test deinit API of Watchdog-timer

 * @param none
 * @return none
 ******************************************************************************/
void test_watchdog_deinit_timer(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog de-init timer API \n");
  // De-initializing timer
  sl_si91x_watchdog_deinit_timer();
  // Testing interrupt mask register value after deinit
  TEST_ASSERT((NPSS_INTR_MASK_SET_REG & NPSS_TO_MCU_WDT_INTR) == 1);
  UnityPrintf("Watchdog-timer interrupt is masked properly\n");
  // Testing timer disabling bits value
  TEST_ASSERT(MCU_WDT->MCU_WWD_MODE_AND_RSTART_b.WWD_MODE_EN_STATUS == 0);
  UnityPrintf("Watchdog-timer disabled properly\n");
  UnityPrintf("watchdog de-init timer API test completed \n");
}

/*******************************************************************************
 * Function to test the get verison API
 ******************************************************************************/
void test_watchdog_timer_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing watchdog get version  \n");
  sl_watchdog_timer_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_watchdog_get_version();
  UnityPrintf("Verifying Watchdog-timer Driver and API version \n");
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");
  UnityPrintf("Watchdog get version test completed \n ");
}
/*******************************************************************************
 * Callback function of WDT timeout interrupt
 *
 * @param none
 * @return none
 ******************************************************************************/
void on_timeout_callback(void)
{
  DEBUGOUT("on timeout callback of timer0 \n");
}
