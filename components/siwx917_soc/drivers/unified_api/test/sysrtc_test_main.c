/***************************************************************************/ /**
 * @file sysrtc_test_main.c
 * @brief SYSRTC Unity Test Functions
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
#include "sl_si91x_sysrtc.h"
#include "sl_system_init.h"
#include "rsi_sysrtc.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void sysrtc_callback(void *callback_flag);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sysrtc_init(void);
void test_sysrtc_configure_clock(void);
void test_sysrtc_configure_group(void);
void test_sysrtc_set_count(void);
void test_sysrtc_get_count(void);
void test_sysrtc_set_compare_value(void);
void test_sysrtc_get_compare_value(void);
void test_sysrtc_set_compare_output_gpio(void);
void test_sysrtc_set_register_as_capture_input(void);
void test_sysrtc_set_gpio_as_capture_input(void);
void test_sysrtc_register_callback(void);
void test_sysrtc_unregister_callback(void);
void test_sysrtc_start(void);
void test_sysrtc_stop(void);
void test_sysrtc_is_running(void);
void test_sysrtc_lock(void);
void test_sysrtc_is_locked(void);
void test_sysrtc_unlock(void);
void test_sysrtc_reset(void);
void test_sysrtc_deinit(void);
void test_sysrtc_get_version(void);

/*******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("SYSRTC");
  RUN_TEST(test_sysrtc_init, __LINE__);
  RUN_TEST(test_sysrtc_configure_clock, __LINE__);
  RUN_TEST(test_sysrtc_configure_group, __LINE__);
  RUN_TEST(test_sysrtc_set_count, __LINE__);
  RUN_TEST(test_sysrtc_set_compare_value, __LINE__);
  RUN_TEST(test_sysrtc_get_compare_value, __LINE__);
  RUN_TEST(test_sysrtc_set_compare_output_gpio, __LINE__);
  RUN_TEST(test_sysrtc_set_gpio_as_capture_input, __LINE__);
  RUN_TEST(test_sysrtc_lock, __LINE__);
  RUN_TEST(test_sysrtc_is_locked, __LINE__);
  RUN_TEST(test_sysrtc_unlock, __LINE__);
  RUN_TEST(test_sysrtc_register_callback, __LINE__);
  RUN_TEST(test_sysrtc_is_running, __LINE__);
  RUN_TEST(test_sysrtc_stop, __LINE__);
  RUN_TEST(test_sysrtc_get_count, __LINE__);
  RUN_TEST(test_sysrtc_get_version, __LINE__);
  RUN_TEST(test_sysrtc_start, __LINE__);
  RUN_TEST(test_sysrtc_unregister_callback, __LINE__);
  RUN_TEST(test_sysrtc_reset, __LINE__);
  RUN_TEST(test_sysrtc_deinit, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test init API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC init api \n");
  sl_status_t status;
  static uint32_t u32Val, expected;
  sl_sysrtc_config_t config_ptr1;

  // handle for all valid parameters
  config_ptr1.enable_debug_run = false;

  // testing with NULL pointer
  status = sl_si91x_sysrtc_init(NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");

  status = sl_si91x_sysrtc_init(&config_ptr1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  u32Val   = SYSRTC0->EN;
  expected = SYSRTC0->EN | SYSRTC_EN_EN;
  TEST_ASSERT_EQUAL(expected, u32Val);
  UnityPrintf("SYSRTC is initialized properly \n");
  UnityPrintf("SYSRTC init-API test completed \n");
}

/*******************************************************************************
 * Function to test the clock-configure API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_configure_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC configure clock api \n");
  sl_status_t status;
  sl_sysrtc_clock_config_t clk_handle1;
  sl_sysrtc_clock_config_t clk_handle2;
  sl_sysrtc_clock_config_t clk_handle3;

  // handle for invalid clock parameters
  clk_handle1.clock_source    = 6;
  clk_handle1.division_factor = 0;

  // handle for invalid clock division factor
  clk_handle2.clock_source    = RSI_SYSRTC_CLK_32KHz_RO;
  clk_handle2.division_factor = 70;

  // handle for all valid parameters
  clk_handle3.clock_source    = RSI_SYSRTC_CLK_32kHz_RC;
  clk_handle3.division_factor = 0;

  // testing with NULL pointer
  status = sl_si91x_sysrtc_configure_clock(NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");

  // testing with invalid clock source parameter
  status = sl_si91x_sysrtc_configure_clock(&clk_handle1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid clock parameter \n");

  // testing with invalid invalid clock division factor
  status = sl_si91x_sysrtc_configure_clock(&clk_handle2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, tested with invalid division factor \n");

  // testing with valid parameters
  status = sl_si91x_sysrtc_configure_clock(&clk_handle3);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid clock parameters \n");
  TEST_ASSERT(MCU_AON->MCUAON_KHZ_CLK_SEL_POR_RESET_STATUS_b.AON_KHZ_CLK_SEL_SYSRTC == RSI_SYSRTC_CLK_32kHz_RC);
  UnityPrintf("clock value is configured properly\n");
  UnityPrintf("SYSRTC clock config-API test completed \n");
}

/*******************************************************************************
 * Function to test the configure_group API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_configure_group(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC configure group api \n");
  sl_status_t status;
  uint32_t temp1 = 0;
  uint32_t temp2 = 0;
  sl_sysrtc_group_config_t group_config_handle1;
  rsi_sysrtc_group_channel_compare_config_t compare_config_handle;
  compare_config_handle.compare_match_out_action = RSI_SYSRTC_COMPARE_MATCH_OUT_ACTION_SET;

  group_config_handle1.capture_channel0_enable   = 0;
  group_config_handle1.compare_channel0_enable   = 1;
  group_config_handle1.compare_channel1_enable   = 0;
  group_config_handle1.p_capture_channel0_config = NULL;
  group_config_handle1.p_compare_channel0_config = &compare_config_handle;
  group_config_handle1.p_compare_channel1_config = NULL;

  // testing with invalid clock source parameter
  status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP_0, NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_configure_group(3, &group_config_handle1);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with valid parameters for group 0
  status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP_0, &group_config_handle1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");
  temp1 = ((group_config_handle1.compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP0_CTRL_CMP0EN_SHIFT);
  temp1 |= ((uint32_t)group_config_handle1.p_compare_channel0_config->compare_match_out_action
            << _SYSRTC_GRP0_CTRL_CMP0CMOA_SHIFT);

  TEST_ASSERT_EQUAL(SYSRTC0->GRP0_CTRL, temp1);
  UnityPrintf("group-0 channel parameters are configured properly\n");

  // testing with valid parameters for group-1
  status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP_1, &group_config_handle1);
  TEST_ASSERT(status == SL_STATUS_OK);
  UnityPrintf("Status of API is correct, tested with valid parameters \n");

  temp2 = ((group_config_handle1.compare_channel0_enable ? 1UL : 0UL) << _SYSRTC_GRP1_CTRL_CMP0EN_SHIFT);
  temp2 |= ((uint32_t)group_config_handle1.p_compare_channel0_config->compare_match_out_action
            << _SYSRTC_GRP1_CTRL_CMP0CMOA_SHIFT);
  TEST_ASSERT_EQUAL(SYSRTC0->GRP1_CTRL, temp2);
  UnityPrintf("group-1 channel parameters are configured properly\n");
  UnityPrintf("SYSRTC group config-API test completed \n");
}

/*******************************************************************************
 * Function to test the set counter value API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_set_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC set count \n");
  uint32_t count_value = 100;
  // Setting count value
  sl_si91x_sysrtc_set_count(count_value);
  // Testing count register value
  TEST_ASSERT_EQUAL(SYSRTC0->CNT, count_value);
  UnityPrintf("SYSRTC counter value is configured properly\n");
  UnityPrintf("SYSRTC SYSRTC set count-API test completed \n");
}

/*******************************************************************************
 * Function to test the set compare value API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_set_compare_value(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC set compare value \n");
  sl_status_t status;
  uint32_t compare_value = 32000;

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_set_compare_value(3, SL_SYSRTC_CHANNEL_0, compare_value);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with invalid channel number parameter
  status = sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP_0, 3, compare_value);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid channel number parameter\n");

  // testing with valid parameters for group0
  status = sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP_0, SL_SYSRTC_CHANNEL_0, compare_value);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");
  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(SYSRTC0->GRP0_CMP0VALUE, compare_value);
  UnityPrintf("SYSRTC group-0 channel-0 compare value is configured properly\n");

  // testing with valid parameters for group1
  status = sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP_1, SL_SYSRTC_CHANNEL_1, compare_value);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");
  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(SYSRTC0->GRP1_CMP1VALUE, compare_value);
  UnityPrintf("SYSRTC group-1 channel-1 compare value is configured properly\n");
  UnityPrintf("SYSRTC SYSRTC set compare value API test completed \n");
}

/*******************************************************************************
 * Function to test the get compare value API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_get_compare_value(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC get compare value \n");
  sl_status_t status;
  uint32_t compare_value = 0;

  // testing with invalid clock source parameter
  status = sl_si91x_sysrtc_get_compare_value(SL_SYSRTC_GROUP_0, SL_SYSRTC_CHANNEL_0, NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_get_compare_value(3, SL_SYSRTC_CHANNEL_0, &compare_value);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with invalid channel number parameter
  status = sl_si91x_sysrtc_get_compare_value(SL_SYSRTC_GROUP_0, 3, &compare_value);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid channel number parameter\n");

  // testing with valid parameters for group0
  status = sl_si91x_sysrtc_get_compare_value(SL_SYSRTC_GROUP_0, SL_SYSRTC_CHANNEL_0, &compare_value);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");
  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(SYSRTC0->GRP0_CMP0VALUE, 32000);
  UnityPrintf("SYSRTC group-0 channel-0 compare value is read properly\n");

  // testing with valid parameters for group1
  status = sl_si91x_sysrtc_get_compare_value(SL_SYSRTC_GROUP_1, SL_SYSRTC_CHANNEL_1, &compare_value);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");
  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(SYSRTC0->GRP1_CMP1VALUE, 32000);
  UnityPrintf("SYSRTC group-1 channel-1 compare value is read properly\n");
  UnityPrintf("SYSRTC SYSRTC get compare value API test completed \n");
}

/*******************************************************************************
 * Function to test the set compare GPIO API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_set_compare_output_gpio(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC set compare output GPIO \n");
  sl_status_t status;

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_set_compare_output_gpio(3, SL_SYSRTC_CHANNEL_0);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with invalid channel number parameter
  status = sl_si91x_sysrtc_set_compare_output_gpio(SL_SYSRTC_GROUP_0, 3);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid channel number parameter\n");

  // testing with valid parameters for group0
  status = sl_si91x_sysrtc_set_compare_output_gpio(SL_SYSRTC_GROUP_0, SL_SYSRTC_CHANNEL_0);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");

  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(MCU_RET->NPSS_GPIO_CNTRL[3].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE, 0x03UL);
  UnityPrintf("SYSRTC group-0 channel-0 compare output GPIO is configured properly\n");

  // testing with valid parameters for group1
  status = sl_si91x_sysrtc_set_compare_output_gpio(SL_SYSRTC_GROUP_1, SL_SYSRTC_CHANNEL_1);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");
  // Testing group-0 channel-0 compare register value
  TEST_ASSERT_EQUAL(MCU_RET->NPSS_GPIO_CNTRL[2].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE, 0x05UL);
  UnityPrintf("SYSRTC group-1 channel-1 compare output GPIO is configured properly\n");
  UnityPrintf("SYSRTC SYSRTC set compare output GPIO API test completed \n");
}

/*******************************************************************************
 * Function to test the set register as input for capture channel
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_set_register_as_capture_input(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC set register as capture input \n");
  sl_status_t status;
  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_set_register_as_capture_input(3);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with valid parameters for group0
  status = sl_si91x_sysrtc_set_register_as_capture_input(SL_SYSRTC_GROUP_0);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for capture channel of group-0\n");
  //todo :cant read this bit only write access
  //  // Testing MCUSYSRTC_REG1 register value
  //  TEST_ASSERT(((SYSRTC0->MCUSYSRTC_REG1) & (SYSRTC_GRP0_CAP_INPUT_SET)) == 1);
}

/*******************************************************************************
 * Function to test the set gpio as input for capture channel
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_set_gpio_as_capture_input(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC set GPIO as capture input \n");
  sl_status_t status;

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_set_gpio_as_capture_input(3);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // testing with valid parameters for group0
  status = sl_si91x_sysrtc_set_gpio_as_capture_input(SL_SYSRTC_GROUP_0);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");

  // Testing register value
  TEST_ASSERT_EQUAL(MCU_RET->NPSS_GPIO_CNTRL[0].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE, 0x03UL);
  UnityPrintf("SYSRTC group-0 capture input GPIO is configured properly\n");

  // testing with valid parameters for group1
  status = sl_si91x_sysrtc_set_gpio_as_capture_input(SL_SYSRTC_GROUP_1);
  TEST_ASSERT(SL_STATUS_OK == status);
  UnityPrintf("Status of API is correct, tested with valid parameter for channel-0 of group-0\n");

  // Testing register value
  TEST_ASSERT_EQUAL(MCU_RET->NPSS_GPIO_CNTRL[0].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE, 0x05UL);
  UnityPrintf("SYSRTC group-1 capture input GPIO is configured properly\n");
  UnityPrintf("SYSRTC set GPIO as capture input API test completed \n");
}

/*******************************************************************************
 * Function to test SYSRTC LOCK API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_lock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC LOCK API \n");
  //sl_status_t status;
  // Locking SYSRTC
  sl_si91x_sysrtc_lock();
  uint32_t count_value1 = 200;
  uint32_t count_value2 = 0;
  // Resetting count value to  200, after sysrtc lock
  UnityPrintf("Changing counter register value after SYSRTC LOCK\n");
  sl_si91x_sysrtc_set_count(count_value1);
  sl_si91x_sysrtc_get_count(&count_value2);
  // testing SYSRTC->CNT register value
  TEST_ASSERT_FALSE(count_value2 == count_value1);
  UnityPrintf("Counter register value not changed\n");
  UnityPrintf("SYSRTC registers are locked properly\n");
  UnityPrintf("SYSRTC Testing SYSRTC LOCK API test completed \n");
  // Unlocking SYSRTC
  sl_si91x_sysrtc_unlock();
}

/*******************************************************************************
 * Function to test SYSRTC is locked API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_is_locked(void)
{
  boolean_t lock_status = false;
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC is locked API \n");
  sl_status_t status;

  // testing with NULL pointer
  status = sl_si91x_sysrtc_is_locked(NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  // Locking SYSRTC
  sl_si91x_sysrtc_lock();
  // testing with valid parameter, when timer is running
  status = sl_si91x_sysrtc_is_locked(&lock_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("lock-status after lock is %d\n", lock_status);
  TEST_ASSERT(lock_status == true);
  UnityPrintf("lock-status is correct when SYSRTC is locked\n");

  // Locking SYSRTC
  sl_si91x_sysrtc_unlock();
  // testing with valid parameter, when timer is running
  status = sl_si91x_sysrtc_is_locked(&lock_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("lock-status after lock is %d\n", lock_status);
  TEST_ASSERT(lock_status == false);
  UnityPrintf("lock-status is correct when SYSRTC is unlocked\n");
  UnityPrintf("SYSRTC is locked API test completed\n");
}
/*******************************************************************************
 * Function to test SYSRTC UNLOCK API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_unlock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC LOCK API \n");
  //sl_status_t status;
  // unLocking SYSRTC
  sl_si91x_sysrtc_unlock();
  uint32_t count_value1 = 200;
  uint32_t count_value2 = 0;
  // Resetting count value to  200, after sysrtc unlock
  UnityPrintf("Changing counter register value after SYSRTC UNLOCK\n");
  sl_si91x_sysrtc_set_count(count_value1);
  sl_si91x_sysrtc_get_count(&count_value2);
  // testing SYSRTC->CNT register value
  UnityPrintf("\n");
  TEST_ASSERT_TRUE(count_value2 == count_value1);
  UnityPrintf("Counter register value changed\n");
  UnityPrintf("SYSRTC registers are unlocked properly\n");
  UnityPrintf("SYSRTC Testing SYSRTC UNLOCK API test completed \n");
}

/*******************************************************************************
 * Function to test SYSRTC register callback API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_register_callback(void)
{
  UnityPrintf("Testing SYSRTC register callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_sysrtc_interrupt_enables_t interrupt_enable_handle;

  interrupt_enable_handle.group0_overflow_interrupt_is_enabled = 0;
  interrupt_enable_handle.group0_compare0_interrupt_is_enabled = 1;
  interrupt_enable_handle.group0_compare1_interrupt_is_enabled = 0;
  interrupt_enable_handle.group0_capture0_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_overflow_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_compare0_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_compare1_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_capture0_interrupt_is_enabled = 0;

  // Testing for NULL pointer
  status = sl_si91x_sysrtc_register_callback(NULL, callback_flag_value, SL_SYSRTC_GROUP_0, &interrupt_enable_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Testing for NULL pointer
  status = sl_si91x_sysrtc_register_callback(sysrtc_callback, callback_flag_value, SL_SYSRTC_GROUP_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with another NULL-pointer parameter \n");

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_register_callback(sysrtc_callback, callback_flag_value, 3, &interrupt_enable_handle);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");
  // Testing with valid parameters
  status = sl_si91x_sysrtc_register_callback(sysrtc_callback,
                                             callback_flag_value,
                                             SL_SYSRTC_GROUP_0,
                                             &interrupt_enable_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");
  // Registering Callback again without unregistering, if status returns busy that means
  // callback is already registered
  status = sl_si91x_sysrtc_register_callback(sysrtc_callback,
                                             callback_flag_value,
                                             SL_SYSRTC_GROUP_0,
                                             &interrupt_enable_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, when registering an already registered callback \n");
  UnityPrintf("SYSRTC register callback API test completed \n");
}

/*******************************************************************************
 * Function to test start SYSRTC API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_start(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC start API \n");
  // Starting SYSRTC and validating
  sl_si91x_sysrtc_start();
  UnityPrintf("Started SYSRTC using start API\n");
}

/*******************************************************************************
 * Function to test start SYSRTC API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_stop(void)
{
  boolean_t running_status;
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC stop API \n");
  // Stopping already started SYSRTC
  sl_si91x_sysrtc_stop();
  UnityPrintf("Stopped SYSRTC using stop API\n");
  UnityPrintf("Checking running status after stopping SYSRTC using stop API\n");
  sl_si91x_sysrtc_is_running(&running_status);
  UnityPrintf("running_status after stop is %d\n", running_status);
  TEST_ASSERT(running_status == false);
  UnityPrintf("running_status is correct when timer is not running\n");
  UnityPrintf("SYSRTC stop API test completed\n");
}

/*******************************************************************************
 * Function to test is running API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_is_running(void)
{
  boolean_t running_status = false;
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC is running API \n");
  sl_status_t status;
  // starting SYSRTC
  sl_si91x_sysrtc_start();
  // testing with NULL pointer
  status = sl_si91x_sysrtc_is_running(NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");

  // testing with valid parameter, when timer is running
  status = sl_si91x_sysrtc_is_running(&running_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("running_status after start is %d\n", running_status);
  TEST_ASSERT(running_status == true);
  UnityPrintf("running_status is correct when timer is running\n");

  sl_si91x_sysrtc_stop();
  // testing with valid parameter, when timer is not running
  status = sl_si91x_sysrtc_is_running(&running_status);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("running_status after stop is %d\n", running_status);
  TEST_ASSERT(running_status == false);
  UnityPrintf("running-status is correct when SYSRTC is stopped\n");
  UnityPrintf("SYSRTC is running API test completed\n");
}

/*******************************************************************************
 * Function to test unregister callback API of SYSRTC
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC unregister callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_sysrtc_interrupt_enables_t interrupt_enable_handle;
  interrupt_enable_handle.group0_overflow_interrupt_is_enabled = 0;
  interrupt_enable_handle.group0_compare0_interrupt_is_enabled = 1;
  interrupt_enable_handle.group0_compare1_interrupt_is_enabled = 0;
  interrupt_enable_handle.group0_capture0_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_overflow_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_compare0_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_compare1_interrupt_is_enabled = 0;
  interrupt_enable_handle.group1_capture0_interrupt_is_enabled = 0;

  // Testing for NULL pointer
  status = sl_si91x_sysrtc_unregister_callback(SL_SYSRTC_GROUP_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // testing with invalid group number parameter
  status = sl_si91x_sysrtc_unregister_callback(3, &interrupt_enable_handle);
  TEST_ASSERT(SL_STATUS_INVALID_PARAMETER == status);
  UnityPrintf("Status of API is correct, tested with invalid group number parameter\n");

  // Unregistering Callback and then registering again, if status comes OK after
  // registering that means callback was unregistered properly, else it would have
  // returned busy status
  status = sl_si91x_sysrtc_unregister_callback(SL_SYSRTC_GROUP_0, &interrupt_enable_handle);
  status = sl_si91x_sysrtc_register_callback(sysrtc_callback,
                                             callback_flag_value,
                                             SL_SYSRTC_GROUP_0,
                                             &interrupt_enable_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Timer timeout unregistered callback properly \n");
  UnityPrintf("SYSRTC unregister timeout callback API test completed \n");
}

/*******************************************************************************
 * Function to test the get verison API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing sysrtc get version  \n");
  sl_sysrtc_version_t version;
  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_sysrtc_get_version();
  UnityPrintf("Verifying SYSRTC Driver and API version \n");
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(1, version.minor);
  UnityPrintf("API version is correct\n");
  UnityPrintf("SYSRTC get version test completed \n ");
}

/*******************************************************************************
 * Function to test SYSRTC reset
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_reset(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC reset API  \n");
  uint32_t count_value1 = 100;
  uint32_t count_value2 = 0;
  uint32_t count_value3 = 0;
  // Setting counter register value as 100 before reset
  sl_si91x_sysrtc_set_count(count_value1);
  sl_si91x_sysrtc_get_count(&count_value2);
  UnityPrintf("Counter register value before SYSRTC reset is %d \n", count_value2);
  UnityPrintf("Resetting SYSRTC\n");
  sl_si91x_sysrtc_reset();
  UnityPrintf("Counter register value after SYSRTC reset is %d \n", count_value3);
  sl_si91x_sysrtc_get_count(&count_value3);
  TEST_ASSERT(count_value3 == 0);
  UnityPrintf("Counter register value resets to default value 0 properly\n");
  UnityPrintf("SYSRTC reset test completed \n ");
}

/*******************************************************************************
 * Function to test SYSRTC reset
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_get_count(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC get-count API  \n");
  sl_status_t status;
  uint32_t count_value1 = 300;
  uint32_t count_value2 = 0;
  // testing with NULL Parameter
  status = sl_si91x_sysrtc_get_count(NULL);
  TEST_ASSERT(SL_STATUS_NULL_POINTER == status);
  UnityPrintf("Status of API is correct, tested with NULL pointer \n");
  UnityPrintf("Setting Counter register value \n");
  sl_si91x_sysrtc_stop();
  sl_si91x_sysrtc_set_count(count_value1);
  UnityPrintf("Reading  Counter register value using get-count API\n");
  sl_si91x_sysrtc_get_count(&count_value2);
  TEST_ASSERT(count_value1 == count_value2);
  UnityPrintf("Counter register read value matches properly with current value\n");
  UnityPrintf("SYSRTC get-count test completed \n ");
}

/*******************************************************************************
 * Function to test SYSRTC deinit API
 *
 * @param none
 * @return none
 ******************************************************************************/
void test_sysrtc_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing SYSRTC de-init API  \n");
  static uint32_t u32Val, expected;
  UnityPrintf("Calling de-init API  \n");
  sl_si91x_sysrtc_deinit();
  u32Val   = SYSRTC0->EN;
  expected = SYSRTC0->EN & SYSRTC_EN_EN;
  TEST_ASSERT_EQUAL(expected, u32Val);
  UnityPrintf("SYSRTC enable bit is cleared properly after Calling de-init API  \n");
  UnityPrintf("SYSRTC is de-initialized properly \n");
  UnityPrintf("SYSRTC deinit-API test completed \n");
}

/*******************************************************************************
 * Callback function of  SYSRTC interrupt
 *
 * @param none
 * @return none
 ******************************************************************************/
void sysrtc_callback(void *callback_flag)
{
  //boolean_t running_status;
  DEBUGOUT("on SYSRTC callback.. \n");
  UnityPrintf(" Interrupt occurred, SYSRTC started properly \n");
  UnityPrintf("SYSRTC start API test completed \n");
  sl_si91x_sysrtc_stop();
}
