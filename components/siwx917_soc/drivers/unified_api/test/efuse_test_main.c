/***************************************************************************/ /**
* @file efuse_test_main.c
* @brief EFUSE Unity Test Functions
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
#include "sl_si91x_efuse.h"
#include "sl_system_init.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define INVALID_ADDRESS     0x33                  // efuse invalid ADDRESS
#define ADDRESS1            0x00001               // efuse ADDRESS 1
#define ADDRESS2            0x00002               // efuse ADDRESS 2
#define HOLD_TIME           40                    // Hold Time
#define SOC_CLK             170000000             // Clock Time
#define INVALID_BIT_POS     255                   // Invalid bit position
#define BIT_POS_0           0                     // Bit position 0
#define BIT_POS_1           1                     // Bit position 1
#define BIT_POS_3           3                     // Bit position 3
#define BIT_POS_5           5                     // Bit position 5
#define MATCH_BYTE          0x2A                  // 1 byte to be read
#define MATCH_WORD          0x032A                // 1 word to be read
#define SOC_MIN_FREQUENCY   1000000               // Minimum frequency of efuse clock
#define SOC_MAX_FREQUENCY   180000000             // Maximum frequency of efuse clock
#define INVALID_MIN_SOC_CLK SOC_MIN_FREQUENCY - 1 // Invalid Min Value of Clock
#define INVALID_MAX_SOC_CLK SOC_MAX_FREQUENCY + 1 // Invalid Max Value of Clock

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
static uint8_t read_byte      = 0;
static uint16_t read_word     = 0;
static uint16_t address_value = 0;
/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
/*******************************************************************************
 ************************  Test Function Prototypes ****************************
 ******************************************************************************/
void test_sl_si91x_efuse_init(void);
void test_sl_si91x_efuse_set_address(void);
void test_sl_si91x_efuse_get_address(void);
void test_sl_si91x_efuse_write_bit(void);
void test_sl_si91x_efuse_memory_mapped_read_word(void);
void test_sl_si91x_efuse_memory_mapped_read_byte(void);
void test_sl_si91x_efuse_fsm_read_byte(void);
void test_sl_si91x_efuse_deinit(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("EFUSE");

  RUN_TEST(test_sl_si91x_efuse_init, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_set_address, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_get_address, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_write_bit, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_fsm_read_byte, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_memory_mapped_read_byte, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_memory_mapped_read_word, __LINE__);
  RUN_TEST(test_sl_si91x_efuse_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function used to initialize the EFUSE.
 * Efuse clock is enabled.
 * Efuse is enabled.
 ******************************************************************************/
void test_sl_si91x_efuse_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing the initialization of efuse \n");
  sl_status_t status;

  UnityPrintf("Initializing the efuse \n");
  status = sl_si91x_efuse_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Initializing the efuse completed successfully \n");

  UnityPrintf("Testing if EFUSE_CLK_ENABLE is enabled (or) not for enabling "
              "the efuse clock \n");
  TEST_ASSERT(M4CLK->CLK_ENABLE_SET_REG3_b.EFUSE_CLK_ENABLE_b == 1);
  UnityPrintf("EFUSE_CLK_ENABLE is enabled properly to enable the efuse "
              "clock \n");

  UnityPrintf("Testing if EFUSE_PCLK_ENABLE is enabled (or) not for enabling "
              "the efuse clock \n");
  TEST_ASSERT(M4CLK->CLK_ENABLE_SET_REG3_b.EFUSE_PCLK_ENABLE_b == 1);
  UnityPrintf("EFUSE_PCLK_ENABLE is enabled properly to enable the efuse "
              "clock \n");

  UnityPrintf("Testing if EFUSE_ENABLE is enabled (or) not for enabling the "
              "efuse \n");
  TEST_ASSERT(EFUSE->EFUSE_CTRL_REG_b.EFUSE_ENABLE == 1);
  UnityPrintf("EFUSE_ENABLE is enabled properly to enable the efuse \n");

  UnityPrintf("Status of API is correct, initialization of efuse test "
              "completed \n");
}

/*******************************************************************************
 * Function used to set the eFUSE ADDRESS1 for read and write operations.
 ******************************************************************************/
void test_sl_si91x_efuse_set_address(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing setting the eFUSE ADDRESS1 for read and write "
              "operations \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not \n");
  status = sl_si91x_efuse_set_address(INVALID_ADDRESS);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not completed "
              "successfully \n");

  UnityPrintf("Testing setting the eFUSE ADDRESS1 for read and write "
              "operations \n");
  status = sl_si91x_efuse_set_address(ADDRESS1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing setting the eFUSE ADDRESS1 for read and write "
              "operations completed successfully \n");

  UnityPrintf("Status of API is correct, setting the eFUSE ADDRESS1 for read "
              "and write operations test completed \n");
}

/*******************************************************************************
 * Function used to get the eFUSE ADDRESS1 for read and write operations.
 ******************************************************************************/
void test_sl_si91x_efuse_get_address(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing getting the eFUSE ADDRESS1 for read and write "
              "operations \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns NULL POINTER (or) not for null "
              "value \n");
  status = sl_si91x_efuse_get_address(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing if status returns NULL POINTER (or) not for null value "
              "completed successfully \n");

  UnityPrintf("Testing if read address matches with the expected address (or) "
              "not \n");
  status = sl_si91x_efuse_get_address(&address_value);
  if (address_value == ADDRESS1) {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("MATCH\n");
  } else {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("UNMATCH\n");
  }
  UnityPrintf("Testing if read address matches with the expected address (or) "
              "not completed successfully \n");

  UnityPrintf("Testing if status returns OK (or) not \n");
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not completed successfully "
              "\n");

  UnityPrintf("Status of API is correct, getting the eFUSE ADDRESS1 for read "
              "and write operations test completed \n");
}

/*******************************************************************************
 * Function used to write the eFUSE data in the specified ADDRESS1 and ADDRESS2.
 ******************************************************************************/
void test_sl_si91x_efuse_write_bit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing writing the eFUSE data in the specified ADDRESS1 and "
              "ADDRESS2\n");
  sl_status_t status;

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid address \n");
  status = sl_si91x_efuse_write_bit(INVALID_ADDRESS, BIT_POS_1, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid address completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid bit position \n");
  status = sl_si91x_efuse_write_bit(ADDRESS1, INVALID_BIT_POS, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid bit position completed successfully \n");

  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 1 of ADDRESS1 \n");
  status = sl_si91x_efuse_write_bit(ADDRESS1, BIT_POS_1, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 1 of ADDRESS1 completed successfully\n");

  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 3 of ADDRESS1\n");
  status = sl_si91x_efuse_write_bit(ADDRESS1, BIT_POS_3, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 3 of ADDRESS1 completed successfully\n");

  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 5 of ADDRESS1 \n");
  status = sl_si91x_efuse_write_bit(ADDRESS1, BIT_POS_5, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 5 of ADDRESS1 completed successfully\n");

  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 0 of ADDRESS2 \n");
  status = sl_si91x_efuse_write_bit(ADDRESS2, BIT_POS_0, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 0 of ADDRESS2 completed successfully\n");

  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 1 of ADDRESS2 \n");
  status = sl_si91x_efuse_write_bit(ADDRESS2, BIT_POS_1, HOLD_TIME);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not when writing on bit "
              "location 1 of ADDRESS2 completed successfully\n");

  UnityPrintf("Status of API is correct, writing the eFUSE data in the "
              "specified ADDRESS1 and ADDRESS2 test completed \n");
}

/*******************************************************************************
 * Function used to read the data from 32x8 byte eFUSE memory(OTP) in fsm mode.
 ******************************************************************************/
void test_sl_si91x_efuse_fsm_read_byte(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing reading the data from 32x8 byte eFUSE memory(OTP) in "
              "fsm mode \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns INVALID PARAMETER for "
              "(or) not for invalid address \n");
  status = sl_si91x_efuse_fsm_read_byte(INVALID_ADDRESS, &read_byte, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid address completed successfully \n");

  UnityPrintf("Testing if status returns NULL POINTER (or) not for null "
              "value \n");
  status = sl_si91x_efuse_fsm_read_byte(ADDRESS1, NULL, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing if status returns NULL POINTER (or) not for null value "
              "completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value \n");
  status = sl_si91x_efuse_fsm_read_byte(ADDRESS1, &read_byte, INVALID_MIN_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value \n");
  status = sl_si91x_efuse_fsm_read_byte(ADDRESS1, &read_byte, INVALID_MAX_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value completed successfully \n");

  UnityPrintf("Testing if data read matches with expected data (or) not \n");
  status = sl_si91x_efuse_fsm_read_byte(ADDRESS1, &read_byte, SOC_CLK);
  if (read_byte == MATCH_BYTE) {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("MATCH\n");
  } else {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("UNMATCH\n");
  }
  UnityPrintf("Testing if data read matches with expected data (or) not "
              "completed successfully \n");

  UnityPrintf("Testing if status returns OK (or) not \n");
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not completed successfully "
              "\n");

  UnityPrintf("Status of API is correct, reading the data from 32x8 byte eFUSE "
              "memory(OTP) in fsm mode test completed \n");
}

/*******************************************************************************
 * Function used to read the data from 32x8 byte eFUSE memory(OTP) in memory
 * mapped mode.
 ******************************************************************************/
void test_sl_si91x_efuse_memory_mapped_read_byte(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing reading the data from 32x8 byte eFUSE memory(OTP) in "
              "memory mapped mode \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns INVALID PARAMETER for invalid address "
              "(or) not for invalid address  \n");
  status = sl_si91x_efuse_memory_mapped_read_byte(INVALID_ADDRESS, &read_byte, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid address completed successfully \n");

  UnityPrintf("Testing if status returns NULL POINTER (or) not for null "
              "value \n");
  status = sl_si91x_efuse_memory_mapped_read_byte(ADDRESS1, NULL, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing if status returns NULL POINTER (or) not for null value "
              "completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value \n");
  status = sl_si91x_efuse_memory_mapped_read_byte(ADDRESS1, &read_byte, INVALID_MIN_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value \n");
  status = sl_si91x_efuse_memory_mapped_read_byte(ADDRESS1, &read_byte, INVALID_MAX_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value completed successfully \n");

  UnityPrintf("Testing if data read matches with expected data (or) not \n");
  status = sl_si91x_efuse_memory_mapped_read_byte(ADDRESS1, &read_byte, SOC_CLK);
  if (read_byte == MATCH_BYTE) {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("MATCH\n");
  } else {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("UNMATCH\n");
  }
  UnityPrintf("Testing if data read matches with expected data (or) not "
              "completed successfully \n");

  UnityPrintf("Testing if status returns OK (or) not \n");
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not completed successfully \n");

  UnityPrintf("Status of API is correct, reading the data from 32x8 byte eFUSE "
              "memory(OTP) in memory mapped mode test completed \n");
}

/*******************************************************************************
 * Function used to Read the 1 word(16 bits) of data from 32x8 byte eFUSE
 * memory(OTP) in memory mapped mode.
 ******************************************************************************/
void test_sl_si91x_efuse_memory_mapped_read_word(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing reading the 1 word(16 bits) of data from 32x8 byte "
              "eFUSE memory(OTP) in memory mapped mode \n");
  sl_status_t status;

  UnityPrintf("Testing if status returns INVALID PARAMETER for invalid address "
              "(or) not for invalid address  \n");
  status = sl_si91x_efuse_memory_mapped_read_word(INVALID_ADDRESS, &read_word, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for "
              "invalid address completed successfully \n");

  UnityPrintf("Testing if status returns NULL POINTER (or) not for null "
              "value \n");
  status = sl_si91x_efuse_memory_mapped_read_word(ADDRESS1, NULL, SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing if status returns NULL POINTER (or) not for null value "
              "completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value \n");
  status = sl_si91x_efuse_memory_mapped_read_word(ADDRESS1, &read_word, INVALID_MIN_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "less than minimum clock value completed successfully \n");

  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value \n");
  status = sl_si91x_efuse_memory_mapped_read_word(ADDRESS1, &read_word, INVALID_MAX_SOC_CLK);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing if status returns INVALID PARAMETER (or) not for clock "
              "more than maximum clock value completed successfully \n");

  UnityPrintf("Testing if data read matches with expected data (or) not \n");
  status = sl_si91x_efuse_memory_mapped_read_word(ADDRESS1, &read_word, SOC_CLK);
  if (read_word == MATCH_WORD) {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("MATCH\n");
  } else {
    /*  Prints on hyper-terminal  */
    DEBUGOUT("UNMATCH\n");
  }
  UnityPrintf("Testing if data read matches with expected data (or) not "
              "completed successfully \n");

  UnityPrintf("Testing if status returns OK (or) not \n");
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Testing if status returns OK (or) not completed successfully \n");

  UnityPrintf("Status of API is correct, reading the 1 word(16 bits) of data "
              "from 32x8 byte eFUSE memory(OTP) in memory mapped mode test "
              "completed \n");
}

/*******************************************************************************
 * Function used to un-initialize the EFUSE.
 * Efuse is disabled.
 * Efuse clock is disabled.
 ******************************************************************************/
void test_sl_si91x_efuse_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing the un-initialization of efuse \n");
  sl_status_t status;

  UnityPrintf("Un-Initializing the efuse \n");
  status = sl_si91x_efuse_deinit();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Un-Initializing the efuse completed successfully \n");

  UnityPrintf("Testing if EFUSE_ENABLE is disabled (or) not for disabling the "
              "efuse \n");
  TEST_ASSERT(EFUSE->EFUSE_CTRL_REG_b.EFUSE_ENABLE == 0);
  UnityPrintf("EFUSE_ENABLE is disabled properly to disable the efuse \n");

  UnityPrintf("Testing if EFUSE_CLK_ENABLE is disabled (or) not for disabling "
              "the efuse clock \n");
  TEST_ASSERT(M4CLK->CLK_ENABLE_CLEAR_REG3_b.EFUSE_CLK_ENABLE_b == 1);
  UnityPrintf("EFUSE_CLK_ENABLE is disabled properly to disable the efuse "
              "clock \n");

  UnityPrintf("Testing if EFUSE_PCLK_ENABLE is disabled (or) not for disabling "
              "the efuse clock \n");
  TEST_ASSERT(M4CLK->CLK_ENABLE_CLEAR_REG3_b.EFUSE_PCLK_ENABLE_b == 1);
  UnityPrintf("EFUSE_PCLK_ENABLE is disabled properly to disable the efuse "
              "clock \n");

  UnityPrintf("Status of API is correct, un-initialization of efuse test "
              "completed \n");
}
