/***************************************************************************/
/**
 * @file littlefs_hal_test_main.c
 * @brief LittleFS HAL Unity Test Functions
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
#include "sl_si91x_littlefs_hal.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TEST_BLOCK  0x00000000 // Example block address
#define TEST_OFFSET 0x0000     // Example offset
#define TEST_SIZE   256        // Example size

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static uint8_t buffer[TEST_SIZE];
const struct lfs_config cfg;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_set_qspi_configs(void);
void test_si91x_block_device_read(void);
void test_si91x_block_device_prog(void);
void test_si91x_block_device_erase(void);
void test_si91x_block_device_sync(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("LittleFS HAL");

  RUN_TEST(test_set_qspi_configs, __LINE__);
  RUN_TEST(test_si91x_block_device_read, __LINE__);
  RUN_TEST(test_si91x_block_device_prog, __LINE__);
  RUN_TEST(test_si91x_block_device_erase, __LINE__);
  RUN_TEST(test_si91x_block_device_sync, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test setting QSPI configurations
 ******************************************************************************/
void test_set_qspi_configs(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Set QSPI Configs \n");

  UnityPrintf("Testing with correct parameters \n");
  set_qspi_configs();
  UnityPrintf("Set QSPI Configs successfully \n");

  UnityPrintf("Set QSPI Configs test completed \n");
}

/*******************************************************************************
 * Function to test reading from LittleFS block device
 ******************************************************************************/
void test_si91x_block_device_read(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing LittleFS Block Device Read \n");
  int status;

  UnityPrintf("Testing with correct parameters \n");
  status = si91x_block_device_read(&cfg, TEST_BLOCK, TEST_OFFSET, buffer, TEST_SIZE);
  TEST_ASSERT_EQUAL_INT(0, status);
  UnityPrintf("Status of API is correct, LittleFS Block Device Read successfully \n");

  UnityPrintf("LittleFS Block Device Read test completed \n");
}

/*******************************************************************************
 * Function to test programming LittleFS block device
 ******************************************************************************/
void test_si91x_block_device_prog(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing LittleFS Block Device Program \n");
  int status;

  UnityPrintf("Testing with correct parameters \n");
  status = si91x_block_device_prog(&cfg, TEST_BLOCK, TEST_OFFSET, buffer, TEST_SIZE);
  TEST_ASSERT_EQUAL_INT(0, status);
  UnityPrintf("Status of API is correct, LittleFS Block Device Program successfully \n");

  UnityPrintf("LittleFS Block Device Program test completed \n");
}

/*******************************************************************************
 * Function to test erasing LittleFS block device
 ******************************************************************************/
void test_si91x_block_device_erase(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing LittleFS Block Device Erase \n");
  int status;

  UnityPrintf("Testing with correct parameters \n");
  status = si91x_block_device_erase(&cfg, TEST_BLOCK);
  TEST_ASSERT_EQUAL_INT(0, status);
  UnityPrintf("Status of API is correct, LittleFS Block Device Erase successfully \n");

  UnityPrintf("LittleFS Block Device Erase test completed \n");
}

/*******************************************************************************
 * Function to test syncing LittleFS block device
 ******************************************************************************/
void test_si91x_block_device_sync(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing LittleFS Block Device Sync \n");
  int status;

  UnityPrintf("Testing with correct parameters \n");
  status = si91x_block_device_sync(&cfg);
  TEST_ASSERT_EQUAL_INT(0, status);
  UnityPrintf("Status of API is correct, LittleFS Block Device Sync successfully \n");

  UnityPrintf("LittleFS Block Device Sync test completed \n");
}