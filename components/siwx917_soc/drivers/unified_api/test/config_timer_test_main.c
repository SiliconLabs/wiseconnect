/***************************************************************************/ /**
 * @file ct_test_main.c
 * @brief Config-Timer Unity Test Functions
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
#include "sl_si91x_config_timer.h"
#include "sl_system_init.h"
#include "rsi_ct.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void on_timeout_callback(void *callback_flag);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_config_timer_init(void);
void test_config_timer_set_configuration(void);
void test_config_timer_start_on_software_trigger(void);
void test_config_timer_reset_configuration(void);
void test_config_timer_set_match_count(void);
void test_config_timer_set_initial_count(void);
void test_config_timer_get_count(void);
void test_config_timer_set_mode(void);
void test_config_timer_set_ocu_configuration(void);
void test_config_timer_reset_ocu_configuration(void);
void test_config_timer_register_callback(void);
void test_config_timer_unregister_callback(void);
void test_config_timer_reset_counter(void);
void test_config_timer_set_counter_sync(void);
void test_config_timer_select_action_event(void);
void test_config_timer_configure_action_event(void);
void test_config_timer_resume_halt_event(void);
void test_config_timer_read_capture(void);
void test_config_timer_set_output_adc_pin(void);
void test_config_timer_set_wfg_compare_values(void);
void test_config_timer_get_version(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("CONFIG-TIMER");
  RUN_TEST(test_config_timer_init, __LINE__);
  RUN_TEST(test_config_timer_set_configuration, __LINE__);
  RUN_TEST(test_config_timer_set_mode, __LINE__);
  RUN_TEST(test_config_timer_set_match_count, __LINE__);
  RUN_TEST(test_config_timer_set_initial_count, __LINE__);
  RUN_TEST(test_config_timer_get_count, __LINE__);
  RUN_TEST(test_config_timer_reset_counter, __LINE__);
  RUN_TEST(test_config_timer_set_counter_sync, __LINE__);
  RUN_TEST(test_config_timer_register_callback, __LINE__);
  RUN_TEST(test_config_timer_unregister_callback, __LINE__);
  RUN_TEST(test_config_timer_select_action_event, __LINE__);
  RUN_TEST(test_config_timer_configure_action_event, __LINE__);
  RUN_TEST(test_config_timer_set_ocu_configuration, __LINE__);
  RUN_TEST(test_config_timer_reset_configuration, __LINE__);
  RUN_TEST(test_config_timer_reset_ocu_configuration, __LINE__);
  RUN_TEST(test_config_timer_resume_halt_event, __LINE__);
  RUN_TEST(test_config_timer_read_capture, __LINE__);
  RUN_TEST(test_config_timer_set_output_adc_pin, __LINE__);
  RUN_TEST(test_config_timer_set_wfg_compare_values, __LINE__);
  RUN_TEST(test_config_timer_get_version, __LINE__);
  RUN_TEST(test_config_timer_start_on_software_trigger, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test init API of Config timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer init api \n");
  sl_si91x_config_timer_init();
  TEST_ASSERT(M4CLK->CLK_CONFIG_REG5_b.CT_CLK_SEL == 0X03);
  UnityPrintf("config-timer is initialized properly \n");
  UnityPrintf("config-Timer init-API test completed \n");
}

/*******************************************************************************
 * Function to test set timer configuration API of Config-timer
 * 
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer set configuration API \n");

  sl_status_t status;
  sl_config_timer_config_t timer_config_handle1;
  sl_config_timer_config_t *test_null = NULL;

  timer_config_handle1.is_counter_mode_32bit_enabled    = false;
  timer_config_handle1.is_counter0_soft_reset_enabled   = true;
  timer_config_handle1.is_counter0_periodic_enabled     = true;
  timer_config_handle1.is_counter0_trigger_enabled      = false;
  timer_config_handle1.is_counter0_sync_trigger_enabled = true;
  timer_config_handle1.is_counter0_buffer_enabled       = true;
  timer_config_handle1.is_counter1_soft_reset_enabled   = true;
  timer_config_handle1.is_counter1_periodic_enabled     = true;
  timer_config_handle1.is_counter1_trigger_enabled      = false;
  timer_config_handle1.is_counter1_sync_trigger_enabled = true;
  timer_config_handle1.is_counter1_buffer_enabled       = true;
  timer_config_handle1.counter0_direction               = SL_COUNTER0_UP;
  timer_config_handle1.counter1_direction               = SL_COUNTER0_UP;

  // testing with NULL pointer
  status = sl_si91x_config_timer_set_configuration(test_null);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct when tested with NULL pointer \n");
  UnityPrintf("Config-Timer set configuration API test completed \n");

  // testing with invalid counter0 direction
  timer_config_handle1.counter0_direction = 3;
  status                                  = sl_si91x_config_timer_set_configuration(&timer_config_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter0 direction \n");

  // correcting counter0 direction
  timer_config_handle1.counter0_direction = SL_COUNTER0_UP;

  // testing with invalid counter1 direction
  timer_config_handle1.counter1_direction = 4;
  status                                  = sl_si91x_config_timer_set_configuration(&timer_config_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter1 direction \n");

  // correcting counter1 direction
  timer_config_handle1.counter1_direction = SL_COUNTER1_UP;

  // testing with valid parameters
  status = sl_si91x_config_timer_set_configuration(&timer_config_handle1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  TEST_ASSERT(CT->CT_GEN_CTRL_SET_REG_b.COUNTER_IN_32_BIT_MODE == false);
  UnityPrintf("Config-Timer 16-bit mode is configured properly\n");

  TEST_ASSERT((CT->CT_GEN_CTRL_SET_REG & (1 << 2)) == PERIODIC_ENCOUNTER_0);
  UnityPrintf("Config-Timer counter0 periodic mode is enabled properly\n");

  TEST_ASSERT((CT->CT_GEN_CTRL_SET_REG & (1 << 18)) == PERIODIC_ENCOUNTER_1);
  UnityPrintf("Config-Timer counter1 periodic mode is enabled properly\n");
  UnityPrintf("config-timer is configured properly \n");
  UnityPrintf("config-Timer set configuration API test completed \n");
}

/*******************************************************************************
 * Function to test the ocu configuration API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_ocu_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config timer set ocu configuration API \n");
  sl_status_t status;
  sl_config_timer_ocu_config_t ocu_config_handle1;

  ocu_config_handle1.is_counter0_ocu_output_enabled         = true;
  ocu_config_handle1.is_counter0_ocu_dma_enabled            = true;
  ocu_config_handle1.is_counter0_ocu_8bit_mode_enabled      = true;
  ocu_config_handle1.is_counter0_ocu_sync_enabled           = true;
  ocu_config_handle1.is_counter1_ocu_output_enabled         = true;
  ocu_config_handle1.is_counter1_ocu_dma_enabled            = true;
  ocu_config_handle1.is_counter1_ocu_mode_enabled           = true;
  ocu_config_handle1.is_counter1_ocu_sync_enabled           = true;
  ocu_config_handle1.is_counter0_toggle_output_high_enabled = true;
  ocu_config_handle1.is_counter0_toggle_output_low_enabled  = true;
  ocu_config_handle1.is_counter1_toggle_output_high_enabled = true;
  ocu_config_handle1.is_counter1_toggle_output_low_enabled  = true;

  // testing with NULL pointer
  status = sl_si91x_config_timer_set_ocu_configuration(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct when tested with NULL pointer \n");

  // testing with valid parameters
  status = sl_si91x_config_timer_set_ocu_configuration(&ocu_config_handle1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 0)) == OUTPUT_OCU_0);
  UnityPrintf("Config-Timer counter0 output configured properly in ocu mode\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 4)) == OCU_DMA_0);
  UnityPrintf("Config-Timer counter0 output dma mode is enabled properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 5)) == OCU_8_MODE_0);
  UnityPrintf("Config-Timer counter0 output 8-bit mode is enabled properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 1)) == OCU_SYNC_WITH_0);
  UnityPrintf("Config-Timer counter0 output is in sync with other channels\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 16)) == OUTPUT_OCU_1);
  UnityPrintf("Config-Timer counter1 output configured properly in ocu mode\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 20)) == OCU_DMA_1);
  UnityPrintf("Config-Timer counter0 output dma mode is enabled properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 21)) == OCU_8_MODE_1);
  UnityPrintf("Config-Timer counter1 output 8-bit mode is enabled properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (1 << 17)) == OCU_SYNC_WITH_1);
  UnityPrintf("Config-Timer counter1 output is in sync with other channels\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (2 << 6)) == MAKE_OUTPUT_0_HIGH_SEL_0);
  UnityPrintf("Config-Timer counter0 output is made high properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (3 << 9)) == MAKE_OUTPUT_0_LOW_SEL_0);
  UnityPrintf("Config-Timer counter0 output is made low properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (3 << 22)) == MAKE_OUTPUT_1_HIGH_SEL_1);
  UnityPrintf("Config-Timer counter1 output is made high properly\n");

  TEST_ASSERT((CT->CT_OCU_CTRL_REG & (2 << 25)) == MAKE_OUTPUT_1_LOW_SEL_1);
  UnityPrintf("Config-Timer counter1 output is made low properly\n");
  UnityPrintf("Config-Timer set ocu configuration API test completed \n");
}

/*******************************************************************************
 * Function to test set mode API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer set mode API \n");

  sl_status_t status;
  sl_config_timer_mode_t mode1 = SL_COUNTER_16BIT;
  sl_config_timer_mode_t mode3 = 2;

  // Testing with invalid timer-mode
  status = sl_si91x_config_timer_set_mode(mode3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid mode parameter \n");

  // Testing with valid timer-mode
  status = sl_si91x_config_timer_set_mode(mode1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT(CT->CT_GEN_CTRL_SET_REG_b.COUNTER_IN_32_BIT_MODE == false);
  UnityPrintf("Timer mode is configured properly as 16-bit\n");
  UnityPrintf("Config-Timer set mode test completed \n");
}

/*******************************************************************************
 * Function to test set match-count API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_match_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer set match count API \n");

  sl_status_t status;
  sl_config_timer_mode_t mode1        = SL_COUNTER_16BIT;
  sl_config_timer_mode_t mode3        = 2;
  sl_counter_number_t counter_number3 = 2;
  uint32_t match_value1               = 1600;
  uint32_t match_value2               = 65557;
  uint32_t match_value3               = 16000;
  uint32_t match_value4               = 160000;

  // Testing with invalid counter-number
  status = sl_si91x_config_timer_set_match_count(mode1, counter_number3, match_value1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");

  // Testing with invalid counter-mode
  status = sl_si91x_config_timer_set_match_count(mode3, SL_COUNTER_0, match_value1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid timer mode \n");

  // Testing with invalid match value for 16-bit counter
  status = sl_si91x_config_timer_set_match_count(SL_COUNTER_16BIT, SL_COUNTER_0, match_value2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid match value for 16-bit counter\n");

  // Testing with valid parameters for 16-bit counter
  status = sl_si91x_config_timer_set_match_count(SL_COUNTER_16BIT, SL_COUNTER_0, match_value3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 16-bit \n");
  TEST_ASSERT(CT0->CT_MATCH_REG_b.COUNTER_0_MATCH == match_value3);
  UnityPrintf("Config-Timer counter0 match value is configured properly in 16-bit mode\n");

  // Testing with valid parameters for 32-bit counter
  status = sl_si91x_config_timer_set_match_count(SL_COUNTER_32BIT, SL_COUNTER_0, match_value4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 32-bit\n");
  TEST_ASSERT(CT0->CT_MATCH_REG == match_value4);
  UnityPrintf("Config-Timer counter0 match value is configured properly in 16-bit mode\n");
  UnityPrintf("Config-Timer set match-count test completed \n");
}

/*******************************************************************************
 * Function to test set initial count API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_initial_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer set initial count API \n");

  sl_status_t status;
  sl_config_timer_mode_t mode1 = SL_COUNTER_16BIT;
  sl_config_timer_mode_t mode2 = SL_COUNTER_32BIT;
  sl_config_timer_mode_t mode3 = 2;

  uint32_t initial_value1 = 1600;
  uint32_t initial_value2 = 65557;
  uint16_t initial_value3 = 16000;
  uint32_t initial_value4 = 160000;

  // Testing with invalid counter-mode
  status = sl_si91x_config_timer_set_initial_count(mode3, initial_value1, initial_value3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid timer mode \n");

  // Testing with invalid count value for 16-bit counter
  status = sl_si91x_config_timer_set_initial_count(mode1, initial_value2, initial_value3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid count value for 16-bit counter\n");

  // Testing with valid parameters for 16-bit counter
  status = sl_si91x_config_timer_set_initial_count(mode1, initial_value1, initial_value3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 16-bit \n");

  TEST_ASSERT(CT0->CT_COUNTER_REG_b.COUNTER0 == initial_value1);
  TEST_ASSERT(CT0->CT_COUNTER_REG_b.COUNTER1 == initial_value3);
  UnityPrintf("Config-Timer counter0  & counter1 initial values is configured properly in 16-bit mode\n");

  // Testing with valid parameters for 32-bit counter
  status = sl_si91x_config_timer_set_initial_count(mode2, initial_value4, 0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 32-bit\n");
  TEST_ASSERT(CT0->CT_COUNTER_REG == initial_value4);
  UnityPrintf("Config-Timer counter0 match value is configured properly in 16-bit mode\n");
  UnityPrintf("Config-Timer set match-count test completed \n");
}

/*******************************************************************************
 * Function to test get count API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_get_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer get count API \n");
  uint32_t count_value = 0;
  sl_status_t status;
  sl_config_timer_mode_t mode3 = 2;
  uint16_t initial_value3      = 16000;
  uint32_t initial_value4      = 160000;

  // Testing with invalid counter-mode
  status = sl_si91x_config_timer_get_count(mode3, SL_COUNTER_0, &count_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_MODE, status);
  UnityPrintf("Status of API is correct, tested with invalid timer mode \n");
  // Testing with NULL pointer
  status = sl_si91x_config_timer_get_count(mode3, SL_COUNTER_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with null pointer \n");
  // Testing with valid parameters for 16-bit counter
  status = sl_si91x_config_timer_set_initial_count(SL_COUNTER_16BIT, initial_value3, 0);
  status = sl_si91x_config_timer_get_count(SL_COUNTER_16BIT, SL_COUNTER_0, &count_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 16-bit\n");
  TEST_ASSERT(count_value == initial_value3);
  UnityPrintf("Config-Timer counter0 count value is read properly in 16-bit mode\n");
  // Testing with valid parameters for 32-bit counter
  status = sl_si91x_config_timer_set_initial_count(SL_COUNTER_32BIT, initial_value4, 0);
  status = sl_si91x_config_timer_get_count(SL_COUNTER_32BIT, SL_COUNTER_0, &count_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for 32-bit\n");
  TEST_ASSERT(count_value == initial_value4);
  UnityPrintf("Config-Timer counter0 count value is read properly in 32-bit mode\n");
  UnityPrintf("Config-Timer get count test completed \n");
}

/*******************************************************************************
 * Function to test reset counter API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_reset_counter()
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer reset counter API \n");
  sl_status_t status;

  // Testing with invalid counter-number
  status = sl_si91x_config_timer_reset_counter(3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");

  // Resetting counter0 count value
  status = sl_si91x_config_timer_reset_counter(SL_COUNTER_0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters\n");
  TEST_ASSERT(CT0->CT_COUNTER_REG_b.COUNTER0 == 0);
  UnityPrintf("Counter0 count value is reset properly\n");

  // Resetting counter0 count value
  status = sl_si91x_config_timer_reset_counter(SL_COUNTER_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters\n");
  TEST_ASSERT(CT0->CT_COUNTER_REG_b.COUNTER1 == 0);
  UnityPrintf("Counter1 count value is reset properly\n");

  UnityPrintf("Config-Timer reset counter test completed \n");
}

/*******************************************************************************
 * Function to test set counter sync API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_counter_sync()
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer set counter sync API \n");
  sl_status_t status;
  uint8_t sync_counter_value1 = 1;
  uint8_t sync_counter_value2 = 2;

  // Testing with invalid counter-number
  status = sl_si91x_config_timer_set_counter_sync(3, sync_counter_value1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");
  // Setting output0 in sync with channel 1
  sl_si91x_config_timer_set_counter_sync(SL_COUNTER_0, sync_counter_value1);
  TEST_ASSERT(CT0->CT_OCU_CTRL_REG_b.SYNC_WITH_0 == sync_counter_value1);
  UnityPrintf("Counter0 output is in sync with channel 1\n");

  // Setting output0 in sync with channel 2
  sl_si91x_config_timer_set_counter_sync(SL_COUNTER_1, sync_counter_value2);
  TEST_ASSERT(CT0->CT_OCU_CTRL_REG_b.SYNC_WITH_1 == sync_counter_value2);
  UnityPrintf("Counter1 output is in sync with channel 2\n");
  UnityPrintf("Config-Timer set counter sync channel test completed \n");
}

/*******************************************************************************
 * Function to test register callback API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_register_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer register callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_config_timer_interrupt_flags_t interrupt_flags;

  interrupt_flags.is_counter0_event_interrupt_enabled     = 0;
  interrupt_flags.is_counter0_fifo_full_interrupt_enabled = 0;
  interrupt_flags.is_counter0_hit_zero_interrupt_enabled  = 0;
  interrupt_flags.is_counter0_hit_peak_interrupt_enabled  = 1;
  interrupt_flags.is_counter1_event_interrupt_enabled     = 0;
  interrupt_flags.is_counter1_fifo_full_interrupt_enabled = 0;
  interrupt_flags.is_counter1_hit_zero_interrupt_enabled  = 0;
  interrupt_flags.is_counter1_hit_peak_interrupt_enabled  = 0;

  // Testing for NULL pointer
  status = sl_si91x_config_timer_register_callback(NULL, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Testing with valid parameters
  status = sl_si91x_config_timer_register_callback(on_timeout_callback, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Registering Callback again without unregistering, if status returns busy that means
  // callback is already registered
  status = sl_si91x_config_timer_register_callback(on_timeout_callback, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, when registering an already registered callback \n");
  UnityPrintf("Config-timer register timeout callback API test completed \n");
}

/*******************************************************************************
 * Function to test unregister callback API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Config-timer unregister callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_config_timer_interrupt_flags_t interrupt_flags;

  interrupt_flags.is_counter0_event_interrupt_enabled     = 0;
  interrupt_flags.is_counter0_fifo_full_interrupt_enabled = 0;
  interrupt_flags.is_counter0_hit_zero_interrupt_enabled  = 0;
  interrupt_flags.is_counter0_hit_peak_interrupt_enabled  = 1;
  interrupt_flags.is_counter1_event_interrupt_enabled     = 0;
  interrupt_flags.is_counter1_fifo_full_interrupt_enabled = 0;
  interrupt_flags.is_counter1_hit_zero_interrupt_enabled  = 0;
  interrupt_flags.is_counter1_hit_peak_interrupt_enabled  = 0;
  // Unregistering Callback and then registering again, if status comes OK after
  // registering that means callback was unregistered properly, else it would have
  // returned busy status
  status = sl_si91x_config_timer_unregister_callback(&interrupt_flags);
  status = sl_si91x_config_timer_register_callback(on_timeout_callback, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Timer timeout unregistered callback properly \n");
  UnityPrintf("Config-timer unregister timeout callback API test completed \n");
}

/*******************************************************************************
 * Function to test select action event API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_select_action_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Config-timer Select Action Event API \n");
  sl_status_t status;
  uint32_t register_value = 0;
  // Setting CT mode as 16-bit
  sl_si91x_config_timer_set_mode(SL_COUNTER_16BIT);

  // Testing with invalid action parameter
  status = sl_si91x_config_timer_select_action_event(ACTION_LAST, SL_EVENT_LAST, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid timer action \n");

  // Testing with invalid input event parameter for counter1
  status = sl_si91x_config_timer_select_action_event(START, SL_EVENT0_RISING_EDGE, SL_EVENT_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid input event for counter1 \n");

  // Testing with invalid input event parameter for counter0
  status = sl_si91x_config_timer_select_action_event(START, SL_EVENT0_RISING_EDGE, SL_EVENT_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid input event for counter0\n");

  // Testing with valid parameters for start action
  status = sl_si91x_config_timer_select_action_event(START, SL_EVENT0_RISING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT0_RISING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_START_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Start action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for stop action
  status = sl_si91x_config_timer_select_action_event(STOP, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_STOP_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Stop action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for continue action
  status = sl_si91x_config_timer_select_action_event(CONTINUE, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_CONTINUE_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Continue action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for halt action
  status = sl_si91x_config_timer_select_action_event(HALT, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_HALT_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Halt action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for increment action
  status = sl_si91x_config_timer_select_action_event(INCREMENT, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_INCREMENT_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Increment action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for capture action
  status = sl_si91x_config_timer_select_action_event(CAPTURE, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_CAPTURE_COUNTER_EVENT_SEL == register_value);
  UnityPrintf("Capture action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for Interrupt action
  status = sl_si91x_config_timer_select_action_event(INTERRUPT, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_INTR_EVENT_SEL == register_value);
  UnityPrintf("Interrupt action events for counter0 and counter1 are configured properly\n");

  // Testing with valid parameters for Output action
  status = sl_si91x_config_timer_select_action_event(OUTPUT, SL_EVENT3_RISING_FALLING_EDGE, SL_EVENT0_FALLING_EDGE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  register_value = (((uint32_t)SL_EVENT3_RISING_FALLING_EDGE) | ((uint32_t)(SL_EVENT0_FALLING_EDGE << 16)));
  TEST_ASSERT(CT0->CT_OUTPUT_EVENT_SEL == register_value);
  UnityPrintf("Output action events for counter0 and counter1 are configured properly\n");
  UnityPrintf("Config-timer select-action- event API test completed \n");
}

/*******************************************************************************
 * Function to test configure action event API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_configure_action_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Config-timer Select Action Event API \n");
  sl_status_t status;
  uint32_t and_value = 0;
  uint32_t or_value  = 0;
  sl_config_action_event_t event_config_handle1;
  sl_config_action_event_t *event_config_handle2 = NULL;

  event_config_handle1.action                        = START;
  event_config_handle1.and_event_counter0            = SL_EVENT0_RISING_EDGE;
  event_config_handle1.or_event_counter0             = SL_EVENT0_FALLING_EDGE;
  event_config_handle1.and_event_valid_bits_counter0 = 10;
  event_config_handle1.or_event_valid_bits_counter0  = 10;
  event_config_handle1.and_event_counter1            = SL_EVENT1_RISING_EDGE;
  event_config_handle1.or_event_counter1             = SL_EVENT1_FALLING_EDGE;
  event_config_handle1.and_event_valid_bits_counter1 = 10;
  event_config_handle1.or_event_valid_bits_counter1  = 10;

  and_value = event_config_handle1.and_event_counter0;
  and_value |= ((event_config_handle1.and_event_valid_bits_counter0) << 8);
  and_value |= ((event_config_handle1.and_event_counter1) << 16);
  and_value |= ((event_config_handle1.and_event_valid_bits_counter1) << 24);

  or_value = event_config_handle1.or_event_counter0;
  or_value |= ((event_config_handle1.or_event_valid_bits_counter0) << 8);
  or_value |= ((event_config_handle1.or_event_counter1) << 16);
  or_value |= ((event_config_handle1.or_event_valid_bits_counter1) << 24);

  // Testing for NULL pointer
  status = sl_si91x_config_timer_configure_action_event(event_config_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Testing with valid parameters
  status = sl_si91x_config_timer_configure_action_event(&event_config_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  TEST_ASSERT(CT0->CT_START_COUNTER_AND_EVENT == and_value);
  UnityPrintf("Start action and-events are configured properly\n");
  TEST_ASSERT(CT0->CT_START_COUNTER_OR_EVENT == or_value);
  UnityPrintf("Start action or-events are configured properly\n");
  UnityPrintf("Config-timer config-action- event API test completed \n");
}

/*******************************************************************************
 * Function to test reset configuration API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_reset_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing reset configuration API \n");
  sl_si91x_config_timer_reset_configuration();
  TEST_ASSERT(CT->CT_GEN_CTRL_RESET_REG == 0x540054);
  UnityPrintf("Timer config-reset value is configured properly\n");
  UnityPrintf("Config-timer reset-configuration API test completed \n");
}

/*******************************************************************************
 * Function to test reset ocu configuration API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_reset_ocu_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing reset OCU configuration API \n");
  sl_si91x_config_timer_reset_ocu_configuration();
  TEST_ASSERT(CT->CT_OCU_CTRL_REG == 0);
  UnityPrintf("OCU reset value is configured properly\n");
  UnityPrintf("Config-timer reset ocu configuration API test completed \n");
}

/*******************************************************************************
 * Function to test resume halt event API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_resume_halt_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing resume halt event API \n");
  sl_status_t status;

  // Setting CT mode as 16-bit
  sl_si91x_config_timer_set_mode(SL_COUNTER_16BIT);

  // testing with invalid counter number
  status = sl_si91x_config_timer_resume_halt_event(SL_COUNTER_NUMBER_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");

  // testing for counter 0
  status = sl_si91x_config_timer_resume_halt_event(SL_COUNTER_0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for counter0 \n");

  // testing for counter 1
  status = sl_si91x_config_timer_resume_halt_event(SL_COUNTER_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameters for counter1\n");
  UnityPrintf("Config-timer resume halt event API test completed \n");
}

/*******************************************************************************
 * Function to test start on software trigger API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_start_on_software_trigger(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing start on software trigger API \n");
  sl_status_t status;

  // Testing with invalid counter-number
  status = sl_si91x_config_timer_start_on_software_trigger(3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");

  // Testing with valid counter-number 0
  status = sl_si91x_config_timer_start_on_software_trigger(SL_COUNTER_0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid counter number \n");
  TEST_ASSERT((CT->CT_GEN_CTRL_SET_REG & (1 << 3)) == COUNTER0_TRIG);
}

/*******************************************************************************
 * Function to test read capture API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_read_capture(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing read capture API \n");
  sl_status_t status;
  uint16_t capture_value = 0;
  // Testing for NULL pointer
  status = sl_si91x_config_timer_read_capture(SL_COUNTER_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");
  // Testing with invalid counter-number
  status = sl_si91x_config_timer_read_capture(SL_COUNTER_NUMBER_LAST, &capture_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");
  // Testing with valid parameters
  status = sl_si91x_config_timer_read_capture(SL_COUNTER_0, &capture_value);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");
  UnityPrintf("Config-timer read capture API test completed \n");
}

/*******************************************************************************
 * Function to test set output ADC pin API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_output_adc_pin(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing set output ADC pin API \n");
  sl_status_t status;
  // testing with invalid parameter for output0
  status = sl_si91x_config_timer_set_output_adc_pin(32, 30);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid pin number \n");

  // testing with invalid parameter for output1
  status = sl_si91x_config_timer_set_output_adc_pin(30, 33);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid pin number \n");

  // testing with valid parameter for output1
  status = sl_si91x_config_timer_set_output_adc_pin(15, 15);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with invalid pin number \n");
  TEST_ASSERT(CT_MUX_REG->CT_OUTPUT_EVENT1_ADC_SEL_b.OUTPUT_EVENT_ADC_SEL == 15);
  TEST_ASSERT(CT_MUX_REG->CT_OUTPUT_EVENT2_ADC_SEL_b.OUTPUT_EVENT_ADC_SEL == 15);
  UnityPrintf("ADC pins for outputs are configured properly");
  UnityPrintf("Config-timer set ADC output pin API test completed \n");
}

/*******************************************************************************
 * Function to test set wfg compare values API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_set_wfg_compare_values(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing set output ADC pin API \n");
  sl_status_t status;
  sl_config_timer_ocu_params_t ocu_params;
  ocu_params.CompareVal1_0 = 1600;
  ocu_params.CompareVal2_0 = 1800;
  ocu_params.CompareVal1_1 = 1600;
  ocu_params.CompareVal2_1 = 1800;

  // Testing for NULL pointer
  status = sl_si91x_config_timer_set_wfg_compare_values(SL_COUNTER_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");
  // Testing with invalid counter-number
  status = sl_si91x_config_timer_set_wfg_compare_values(SL_COUNTER_NUMBER_LAST, &ocu_params);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);

  // Testing with valid parameters for counter0
  status = sl_si91x_config_timer_set_wfg_compare_values(SL_COUNTER_0, &ocu_params);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");
  TEST_ASSERT(CT0->CT_OCU_COMPARE_REG_b.OCU_COMPARE_0_REG == ocu_params.CompareVal1_0);
  TEST_ASSERT(CT0->CT_OCU_COMPARE_REG_b.OCU_COMPARE_1_REG == ocu_params.CompareVal2_0);

  // Testing with valid parameters for counter0
  status = sl_si91x_config_timer_set_wfg_compare_values(SL_COUNTER_1, &ocu_params);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with invalid counter number \n");
  TEST_ASSERT(CT0->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_0_REG == ocu_params.CompareVal1_1);
  TEST_ASSERT(CT0->CT_OCU_COMPARE2_REG_b.OCU_COMPARE2_1_REG == ocu_params.CompareVal2_1);
  UnityPrintf("WFG compare values are configured properly\n");
  UnityPrintf("Config-timer set wfg compare values API test completed \n");
}

/*******************************************************************************
 * Function to test the get verison API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_config_timer_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing config-timer get version  \n");
  sl_config_timer_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_config_timer_get_version();
  UnityPrintf("Verifying config-timer Driver and API version \n");
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(1, version.minor);
  UnityPrintf("API version is correct\n");
  UnityPrintf("Config-timer get version test completed \n ");
}
// todo
/*******************************************************************************
 * Function to test set WFG configuration API of Config-timer
 *
 * @param none
 * @return none
 ******************************************************************************/

/*******************************************************************************
 * Callback function of config-timer timeout interrupt
 *
 * @param none
 * @return none
 ******************************************************************************/
void on_timeout_callback(void *callback_flag)
{
  DEBUGOUT("on timeout callback of config-timer \n");
  UnityPrintf("Config-Timer counter0 started properly on software trigger\n");
  if (*(uint32_t *)callback_flag == SL_CT_COUNTER_0_IS_PEAK_FLAG) {
    UnityPrintf("Config-timer start API test completed \n");
    sl_si91x_config_timer_deinit();
  }
}
