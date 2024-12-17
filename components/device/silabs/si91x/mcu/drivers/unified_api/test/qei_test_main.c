/***************************************************************************/ /**
 * @file qei_test_main.c
 * @brief QEI Unity Test Functions
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_qei.h"
#include "sl_system_init.h"
#include "unity.h"
#include "sl_status.h"
#include "sl_si91x_qei_config.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define VALID_POSITION_COUNT   0xFFFF   // Maximum valid count for the position counter (16-bit).
#define VALID_INDEX_COUNT      0xFFFF   // Maximum valid count for the index counter (16-bit).
#define DEFAULT_POSITION_COUNT 0x0      // Default starting value for the position counter.
#define DEFAULT_INDEX_COUNT    0x0      // Default starting value for the index counter.
#define FREQUENCY              30000000 // Operating frequency of the system on chip (SoC) in Hz (30 MHz).
#define TIME_PERIOD_US         1000000  // Time period for delta time measurement in microseconds (1 second).
#define VALID_POS_MATCH        0xFFFF   // Maximum valid value for position match comparison.
#define DEFAULT_POS_MATCH      0x0      // Default value for position match comparison.
#define VALID_VELOCITY_CMP     0xFFFF   // Maximum valid value for velocity comparison (16-bit).
#define DEFAULT_VELOCITY_CMP   0x0      // Default value for velocity comparison.
#define VALID_INDEX            0xFFFF   // Maximum valid value for index (16-bit).
#define DEFAULT_INDEX          0xFFFF   // Default value for index (16-bit).
#define VALID_MAX_POS_CNT      0xFFFF   // Maximum allowed value for the maximum position counter (16-bit).
#define DEFAULT_MAX_POS_CNT    0x0      // Default value for the maximum position counter.
#define DELTA_TIME_MAX         1048577  // Maximum delta time value that can be configured (in microseconds).

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
void callback_qei(void *callback_flag);
;

/*******************************************************************************
 ************************  Test Function Prototypes ****************************
 ******************************************************************************/
void test_sl_si91x_qei_set_configuration(void);
void test_sl_si91x_qei_get_configuration(void);
void test_sl_si91x_qei_set_position_counter(void);
void test_sl_si91x_qei_get_position_counter(void);
void test_sl_si91x_qei_set_encoding_mode(void);
void test_sl_si91x_qei_get_encoding_mode(void);
void test_sl_si91x_qei_set_direction(void);
void test_sl_si91x_qei_get_direction(void);
void test_sl_si91x_qei_set_index_counter(void);
void test_sl_si91x_qei_get_index_counter(void);
void test_sl_si91x_qei_configure_delta_time_frequency(void);
void test_sl_si91x_qei_set_delta_time(void);
void test_sl_si91x_qei_get_delta_time(void);
void test_sl_si91x_qei_configure_velocity_counter(void);
void test_sl_si91x_qei_set_position_match(void);
void test_sl_si91x_qei_get_position_match(void);
void test_sl_si91x_qei_velocity_value_to_compare(void);
void test_sl_si91x_qei_set_index_maximum_counter(void);
void test_sl_si91x_qei_get_index_maximum_counter(void);
void test_sl_si91x_qei_swap_a_b(void);
void test_sl_si91x_qei_configure_timer_mode(void);
void test_sl_si91x_qei_init(void);
void test_sl_si91x_qei_deinit(void);
void test_sl_si91x_qei_clear_interrupt(void);
void test_sl_si91x_qei_set_maximum_position_counter(void);
void test_sl_si91x_qei_get_maximum_position_counter(void);
void test_sl_si91x_qei_set_digital_filter_clock_division(void);
void test_sl_si91x_qei_get_digital_filter_clock_division(void);
void test_sl_si91x_qei_get_module_frequency(void);
void test_sl_si91x_qei_set_module_frequency(void);
void test_sl_si91x_qei_get_status(void);
void test_sl_si91x_qei_get_interrupt_clear(void);
void test_sl_si91x_qei_unregister_callback(void);
void test_sl_si91x_qei_register_callback(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  // Initialize the system
  sl_system_init();

  // Begin the Unity test group for QEI
  UnityBeginGroup("QEI");

  // Run individual test cases
  RUN_TEST(test_sl_si91x_qei_init, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_module_frequency, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_module_frequency, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_configuration, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_configuration, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_direction, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_direction, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_position_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_position_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_digital_filter_clock_division, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_digital_filter_clock_division, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_encoding_mode, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_encoding_mode, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_index_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_index_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_swap_a_b, __LINE__);
  RUN_TEST(test_sl_si91x_qei_configure_timer_mode, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_position_match, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_position_match, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_maximum_position_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_configure_delta_time_frequency, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_delta_time, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_delta_time, __LINE__);
  RUN_TEST(test_sl_si91x_qei_configure_velocity_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_register_callback, __LINE__);
  RUN_TEST(test_sl_si91x_qei_unregister_callback, __LINE__);
  RUN_TEST(test_sl_si91x_qei_velocity_value_to_compare, __LINE__);
  RUN_TEST(test_sl_si91x_qei_set_index_maximum_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_index_maximum_counter, __LINE__);
  RUN_TEST(test_sl_si91x_qei_clear_interrupt, __LINE__);
  RUN_TEST(test_sl_si91x_qei_get_interrupt_clear, __LINE__);
  RUN_TEST(test_sl_si91x_qei_deinit, __LINE__);

  // End the test group and report results
  UnityEnd();
  UnityPrintf("END");

  // Infinite loop to keep the program running
  while (1) {
  }
}

/*******************************************************************************
 * @brief Function to test the API for setting the QEI position counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_position_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI position counter\n");
  sl_status_t status;
  uint32_t position;

  // Test setting a valid position counter value
  UnityPrintf("Testing setting the QEI position counter with a valid value\n");
  status = sl_si91x_qei_set_position_counter(VALID_POSITION_COUNT);

  // Assert that the status returned is SL_STATUS_OK
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting QEI position counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to retrieve the current position counter value
  position = sl_si91x_qei_get_position_counter();

  TEST_ASSERT_EQUAL_HEX(VALID_POSITION_COUNT, position);
  UnityPrintf("Get position counter test completed successfully. Expected VALID_POSITION_COUNT.\n");

  // Test setting a default position counter value
  UnityPrintf("Testing setting the QEI position counter with a default value\n");
  status = sl_si91x_qei_set_position_counter(DEFAULT_POSITION_COUNT);

  // Assert that the status returned is SL_STATUS_OK
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting QEI position counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Indicate that the test for setting the QEI position counter has completed
  UnityPrintf("Status of API is correct; setting the QEI position counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for getting the QEI position counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_position_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI position counter\n");
  uint32_t position;

  // Attempt to get the position counter
  position = sl_si91x_qei_get_position_counter();
  TEST_ASSERT_EQUAL_HEX(DEFAULT_POSITION_COUNT, position);
  UnityPrintf("Get position counter test completed successfully. Expected DEFAULT_POSITION_COUNT.\n");

  UnityPrintf("Status of API is correct, getting the QEI position counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for setting the QEI encoding mode.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_encoding_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI encoding mode\n");
  sl_status_t status;
  sl_qei_encoding_mode_t mode;

  // Test setting an invalid encoding mode
  status = sl_si91x_qei_set_encoding_mode(SL_QEI_ENCODE_MODE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Setting encoding mode test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test setting a valid encoding mode
  status = sl_si91x_qei_set_encoding_mode(SL_QEI_ENCODE_MODE_2X);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting encoding mode test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the encoding mode
  mode = sl_si91x_qei_get_encoding_mode();
  TEST_ASSERT_EQUAL_HEX(SL_QEI_ENCODE_MODE_2X, mode);
  UnityPrintf("Get encoding mode test completed successfully. Expected SL_QEI_ENCODE_MODE_2X.\n");

  // Set another valid encoding mode
  status = sl_si91x_qei_set_encoding_mode(SL_QEI_ENCODE_MODE_1X);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting encoding mode test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, setting the QEI encoder mode test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for getting the QEI encoding mode.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_encoding_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI encoding mode\n");
  sl_qei_encoding_mode_t mode;

  // Attempt to get the encoding mode
  mode = sl_si91x_qei_get_encoding_mode();
  TEST_ASSERT_EQUAL_HEX(SL_QEI_ENCODE_MODE_1X, mode);
  UnityPrintf("Get encoding mode test completed successfully. Expected SL_QEI_ENCODE_MODE_1X.\n");

  UnityPrintf("Status of API is correct, getting the QEI encoder mode test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for setting the QEI direction.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_direction(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI direction\n");
  sl_status_t status;

  // Test setting an invalid position counter direction
  status = sl_si91x_qei_set_pos_direction(SL_QEI_DIRECTION_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf(
    "Setting position counter direction test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test setting a valid position counter direction
  status = sl_si91x_qei_set_pos_direction(SL_QEI_POS_DIRECTION);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting position counter direction test completed successfully. Expected SL_STATUS_OK.\n");

  // Test setting another valid position counter direction
  status = sl_si91x_qei_set_pos_direction(SL_QEI_NEG_DIRECTION);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting position counter direction test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, setting the QEI encoder mode test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for getting the QEI direction.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_direction(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI direction\n");
  sl_qei_direction_t direction;

  // Attempt to get the direction
  direction = sl_si91x_qei_get_direction();
  TEST_ASSERT_EQUAL_HEX(SL_QEI_NEG_DIRECTION, direction);
  UnityPrintf("Get direction test completed successfully. Expected SL_QEI_NEG_DIRECTION.\n");

  UnityPrintf("Status of API is correct, getting direction test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for setting the QEI index counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_index_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI index counter\n");
  sl_status_t status;
  uint32_t index_count;

  // Test setting a valid index counter value
  status = sl_si91x_qei_set_index_counter(VALID_INDEX_COUNT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting index counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the index counter
  index_count = sl_si91x_qei_get_index_counter();
  TEST_ASSERT_EQUAL_HEX(VALID_INDEX_COUNT, index_count);
  UnityPrintf("Get index counter test completed successfully. Expected VALID_INDEX_COUNT.\n");

  // Set the index counter to a default value
  status = sl_si91x_qei_set_index_counter(DEFAULT_INDEX_COUNT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting index counter test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting index counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for getting the QEI index counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_index_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI index counter\n");
  uint32_t index_count;

  // Attempt to get the index counter
  index_count = sl_si91x_qei_get_index_counter();
  TEST_ASSERT_EQUAL_HEX(DEFAULT_INDEX_COUNT, index_count);
  UnityPrintf("Get index counter test completed successfully. Expected DEFAULT_INDEX_COUNT.\n");

  UnityPrintf("Status of API is correct, Get index counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for configuring delta time frequency.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_configure_delta_time_frequency(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing configuring delta time frequency\n");
  sl_status_t status;

  // Test configuring with valid frequency and period
  status = sl_si91x_qei_configure_delta_time_frequency(FREQUENCY, TIME_PERIOD_US);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Configuring delta time frequency test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Configuring delta time frequency test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for setting the QEI delta time.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_delta_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI delta time\n");
  sl_status_t status;

  // Test setting a valid delta time
  status = sl_si91x_qei_set_delta_time(TIME_PERIOD_US);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting delta time test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting delta time test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for getting the QEI delta time.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_delta_time(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI delta time\n");
  uint32_t delta_time;

  // Attempt to get the delta time
  delta_time = sl_si91x_qei_get_delta_time();
  TEST_ASSERT_EQUAL_HEX(TIME_PERIOD_US, delta_time);
  UnityPrintf("Get delta time test completed successfully. Expected TIME_PERIOD_US.\n");

  UnityPrintf("Status of API is correct, Get delta time test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the API for configuring the QEI velocity counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_configure_velocity_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing configuring the QEI velocity counter\n");
  sl_status_t status;
  uint32_t period = 100000;

  // Test setting an invalid delta time
  status = sl_si91x_qei_start_velocity(DELTA_TIME_MAX);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Setting velocity counter test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test configuring with a valid velocity state
  status = sl_si91x_qei_start_velocity(period);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Configuring velocity counter test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Configuring velocity counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI position match value.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_position_match(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI position match\n");
  sl_status_t status;
  uint32_t pos_match;

  // Test setting a valid position match value
  status = sl_si91x_qei_set_position_match(VALID_POS_MATCH);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting position match test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the position match
  pos_match = sl_si91x_qei_get_position_match();
  TEST_ASSERT_EQUAL_HEX(VALID_POS_MATCH, pos_match);
  UnityPrintf("Get position match test completed successfully. Expected VALID_POS_MATCH.\n");

  // Test setting a valid position match value
  status = sl_si91x_qei_set_position_match(DEFAULT_POS_MATCH);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting position match test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting position match test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI position match value.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_position_match(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI position match\n");
  uint32_t pos_match;

  // Attempt to get the position match
  pos_match = sl_si91x_qei_get_position_match();
  TEST_ASSERT_EQUAL_HEX(DEFAULT_POS_MATCH, pos_match);
  UnityPrintf("Get position match test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Get position match test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting a velocity value to compare.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_velocity_value_to_compare(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting velocity value to compare\n");
  sl_status_t status;

  // Test with a valid velocity value
  status = sl_si91x_qei_set_velocity_value_to_compare(VALID_VELOCITY_CMP);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Velocity value to compare test completed successfully. Expected SL_STATUS_OK.\n");

  // Test with a valid velocity value
  status = sl_si91x_qei_set_velocity_value_to_compare(DEFAULT_VELOCITY_CMP);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Velocity value to compare test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Velocity value to compare test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI index maximum counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_index_maximum_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI index maximum counter\n");
  sl_status_t status;
  uint32_t max_index;

  // Test setting a valid maximum index value
  status = sl_si91x_qei_set_index_maximum_counter(VALID_INDEX);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting index maximum counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the maximum index counter
  max_index = sl_si91x_qei_get_index_maximum_counter();
  TEST_ASSERT_EQUAL_HEX(VALID_INDEX, max_index);
  UnityPrintf("Get index maximum counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Test setting a valid maximum index value
  status = sl_si91x_qei_set_index_maximum_counter(DEFAULT_INDEX);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting index maximum counter test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting index maximum counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI index maximum counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_index_maximum_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI index maximum counter\n");
  uint32_t max_index;

  // Attempt to get the maximum index counter
  max_index = sl_si91x_qei_get_index_maximum_counter();
  TEST_ASSERT_EQUAL_HEX(DEFAULT_INDEX, max_index);
  UnityPrintf("Get index maximum counter test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Get index maximum counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test swapping the A and B inputs for the QEI.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_swap_a_b(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing swapping A and B inputs\n");
  sl_status_t status;

  // Test swapping A and B with an invalid selection
  status = sl_si91x_qei_swap_a_b(SL_QEI_SWAP_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Swap A and B test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test swapping A and B with a valid selection
  status = sl_si91x_qei_swap_a_b(SL_QEI_SWAP_AB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Swap A and B test completed successfully. Expected SL_STATUS_OK.\n");

  // Test swapping A and B with another valid selection
  status = sl_si91x_qei_swap_a_b(SL_QEI_NO_SWAP_AB);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Swap A and B test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct,Swap A and B test completed.\n");
}

/*******************************************************************************
 * @brief Function to test configuring the QEI timer mode.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_configure_timer_mode(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing configuring QEI timer mode\n");
  sl_status_t status;

  // Test configuring with an invalid timer mode
  status = sl_si91x_qei_configure_timer_mode(SL_QEI_MODE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Configuring timer mode test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test configuring with a valid timer mode
  status = sl_si91x_qei_configure_timer_mode(SL_QEI_TIMER_MODE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Configuring timer mode test completed successfully. Expected SL_STATUS_OK.\n");

  // Test configuring with another valid timer mode
  status = sl_si91x_qei_configure_timer_mode(SL_QEI_ENCODER_MODE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Configuring timer mode test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Configuring timer mode test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the initialization of the QEI.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing QEI initialization\n");
  sl_si91x_qei_init(&sl_qei_init);
  UnityPrintf("QEI initialization completed successfully.\n");

  UnityPrintf("Status of API is correct, QEI initialization test completed.\n");
}

/*******************************************************************************
 * @brief Function to test the deinitialization of the QEI.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing QEI deinitialization\n");
  sl_si91x_qei_deinit();
  UnityPrintf("QEI deinitialization completed successfully.\n");

  UnityPrintf("Status of API is correct, QEI deinitialization test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI interrupt clear status.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_interrupt_clear(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI interrupt clear\n");
  uint32_t intr_clear;

  // Attempt to get the interrupt clear status
  intr_clear = sl_si91x_qei_get_interrupt_clear();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, intr_clear);
  UnityPrintf("Get interrupt clear test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Get interrupt clear test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI module status.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_status(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI module status\n");
  uint32_t module_status;

  // Attempt to get the module status
  module_status = sl_si91x_qei_get_status();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, module_status);
  UnityPrintf("Get module status test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct,Get module status test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI module frequency.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_module_frequency(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI module frequency\n");
  sl_status_t status;

  // Test setting a valid frequency
  status = sl_si91x_qei_set_module_frequency(FREQUENCY);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting module frequency test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting module frequency test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI module frequency.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_module_frequency(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI module frequency\n");
  uint32_t frequency;

  // Attempt to get the module frequency
  frequency = sl_si91x_qei_get_module_frequency();
  TEST_ASSERT_EQUAL_HEX(FREQUENCY, frequency);
  UnityPrintf("Get module frequency test completed successfully. Expected FREQUENCY.\n");

  UnityPrintf("Status of API is correct, Get module frequency test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI digital filter clock division.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_digital_filter_clock_division(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI digital filter clock division\n");
  sl_status_t status;
  sl_qei_clk_div_t clock_division;

  // Test setting an invalid clock division
  status = sl_si91x_qei_set_digital_filter_clock_division(SL_QEI_CLK_DIV_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf(
    "Setting digital filter clock division test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test setting a valid clock division
  status = sl_si91x_qei_set_digital_filter_clock_division(SL_QEI_CLK_DIV_8);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting digital filter clock division test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the digital filter clock division
  clock_division = sl_si91x_qei_get_digital_filter_clock_division();
  TEST_ASSERT_EQUAL_HEX(SL_QEI_CLK_DIV_8, clock_division);
  UnityPrintf("Get digital filter clock division test completed successfully. Expected SL_QEI_CLK_DIV_8.\n");

  // Test setting another valid clock division
  status = sl_si91x_qei_set_digital_filter_clock_division(SL_QEI_CLK_DIV_1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting digital filter clock division test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting digital filter clock division test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI digital filter clock division.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_digital_filter_clock_division(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI digital filter clock division\n");
  sl_qei_clk_div_t clock_division;

  // Attempt to get the digital filter clock division
  clock_division = sl_si91x_qei_get_digital_filter_clock_division();
  TEST_ASSERT_EQUAL_HEX(SL_QEI_CLK_DIV_1, clock_division);
  UnityPrintf("Get digital filter clock division test completed successfully. Expected SL_QEI_CLK_DIV_1.\n");

  UnityPrintf("Status of API is correct, get digital filter clock division test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI maximum position counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_maximum_position_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI maximum position counter\n");
  sl_status_t status;
  uint32_t max_pos_cnt;

  // Test setting a valid maximum position counter
  status = sl_si91x_qei_set_maximum_position_counter(VALID_MAX_POS_CNT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting maximum position counter test completed successfully. Expected SL_STATUS_OK.\n");

  // Attempt to get the maximum position counter
  max_pos_cnt = sl_si91x_qei_get_maximum_position_counter();
  TEST_ASSERT_EQUAL_HEX(VALID_MAX_POS_CNT, max_pos_cnt);
  UnityPrintf("Get maximum position counter test completed successfully. Expected VALID_MAX_POS_CNT.\n");

  // Test setting another valid maximum position counter
  status = sl_si91x_qei_set_maximum_position_counter(DEFAULT_MAX_POS_CNT);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Setting maximum position counter test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Setting maximum position counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI maximum position counter.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_maximum_position_counter(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI maximum position counter\n");
  uint32_t max_pos_cnt;

  // Attempt to get the maximum position counter
  max_pos_cnt = sl_si91x_qei_get_maximum_position_counter();
  TEST_ASSERT_EQUAL_HEX(DEFAULT_MAX_POS_CNT, max_pos_cnt);
  UnityPrintf("Get maximum position counter test completed successfully. Expected DEFAULT_MAX_POS_CNT.\n");

  UnityPrintf("Status of API is correct, get maximum position counter test completed.\n");
}

/*******************************************************************************
 * @brief Function to test setting the QEI configuration.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_set_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the QEI configuration\n");
  sl_status_t status;
  sl_qei_config_t config = {};

  // Test with an invalid control state and configuration
  status = sl_si91x_qei_set_configuration(SL_QEI_STATE_LAST, &config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Control configuration test completed successfully. Expected SL_STATUS_INVALID_PARAMETER.\n");

  // Test with a valid control state and configuration
  status = sl_si91x_qei_set_configuration(SL_QEI_RESET, &config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Control configuration test completed successfully. Expected SL_STATUS_OK.\n");

  // Test with another valid control state and configuration
  status = sl_si91x_qei_set_configuration(SL_QEI_SET, &config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Control configuration test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Control configuration test completed.\n");
}

/*******************************************************************************
 * @brief Function to test registering a callback for QEI interrupts.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_register_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing QEI register callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_qei_intr_unmask_t interrupt_flags;

  interrupt_flags.qei_index_cnt_match_intr_unmask           = 0;
  interrupt_flags.qei_position_cnt_match_intr_unmask        = 0;
  interrupt_flags.qei_position_cnt_reset_intr_unmask        = 0;
  interrupt_flags.qei_position_cntr_err_intr_unmask         = 0;
  interrupt_flags.qei_velocity_computation_over_intr_unmask = 1;
  interrupt_flags.qei_velocity_less_than_intr_unmask        = 0;

  // Testing for NULL pointer
  status = sl_si91x_qei_register_callback(NULL, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, tested with NULL-pointer parameter \n");

  // Testing with valid parameters
  status = sl_si91x_qei_register_callback(callback_qei, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, tested with valid parameter \n");

  // Registering callback again without unregistering
  status = sl_si91x_qei_register_callback(callback_qei, callback_flag_value, &interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, when registering an already registered callback \n");

  UnityPrintf("QEI register callback API test completed \n");
}

/*******************************************************************************
 * @brief Function to test unregistering a callback for QEI interrupts.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing QEI unregister callback API \n");
  sl_status_t status;
  void *callback_flag_value = NULL;
  sl_qei_intr_mask_t interrupt_flags;
  sl_qei_intr_unmask_t interrupt_mask;

  interrupt_flags.qei_index_cnt_match_intr_mask           = 0;
  interrupt_flags.qei_position_cnt_match_intr_mask        = 0;
  interrupt_flags.qei_position_cnt_reset_intr_mask        = 0;
  interrupt_flags.qei_position_cntr_err_intr_mask         = 0;
  interrupt_flags.qei_velocity_computation_over_intr_mask = 1;
  interrupt_flags.qei_velocity_less_than_intr_mask        = 0;

  // Attempt to unregister the callback
  status = sl_si91x_qei_unregister_callback(&interrupt_flags);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);

  // Register the callback again to ensure it's available
  status = sl_si91x_qei_register_callback(callback_qei, callback_flag_value, &interrupt_mask);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);

  UnityPrintf("QEI unregister callback API test completed \n");
}

/*******************************************************************************
 * @brief Function to test getting the QEI configuration.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_get_configuration(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the QEI configuration\n");
  sl_status_t status;
  sl_qei_config_t config;

  // Attempt to get the configuration
  status = sl_si91x_qei_get_configuration(SL_QEI_SET, &config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Get configuration test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, get configuration test completed.\n");
}

/*******************************************************************************
 * @brief Function to test clearing a QEI interrupt.
 * @param none
 * @return none
 ******************************************************************************/
void test_sl_si91x_qei_clear_interrupt(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing clearing the QEI interrupt\n");
  sl_status_t status;

  // Test clearing with a valid interrupt mask
  status = sl_si91x_qei_clear_interrupt(BIT(5));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Clear interrupt test completed successfully. Expected SL_STATUS_OK.\n");

  UnityPrintf("Status of API is correct, Clear interrupt test completed.\n");
}

/*******************************************************************************
 * @brief Function to handle QEI interrupt callbacks.
 * @param callback_flag Pointer to a value indicating the callback's status.
 * @return none
 ******************************************************************************/
void callback_qei(void *callback_flag)
{
  DEBUGOUT("callback of QEI \n");
  UnityPrintf("QEI interrupt triggered for Velocity\n");
  if (*(uint32_t *)callback_flag == SL_QEI_VELOCITY_COMP_OVER_INTR_LVL) {
    UnityPrintf("QEI velocity test completed \n");
    sl_si91x_qei_deinit();
  }
}
