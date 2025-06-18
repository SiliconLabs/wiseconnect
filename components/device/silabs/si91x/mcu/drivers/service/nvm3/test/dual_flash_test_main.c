/***************************************************************************/
/**
 * @file dual_flash_intf_test_main.c
 * @brief Dual Flash Interface Unity Test Functions
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
#include "sl_si91x_dual_flash_intf.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TEST_SECTOR_ADDRESS 0x00000000 // Example sector address
#define TEST_ADDRESS        0x00000000 // Example address
#define TEST_LENGTH         256        // Example length
#define TEST_AUTO_MODE      1          // Example auto mode

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
static unsigned char data[TEST_LENGTH];
spi_config_t spi_config;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_rsi_flash_init(void);
void test_rsi_flash_erase_sector(void);
void test_rsi_flash_write(void);
void test_rsi_flash_read(void);
void test_rsi_flash_uninitialize(void);
void test_get_qspi_config(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("Dual Flash Interface");

  RUN_TEST(test_rsi_flash_init, __LINE__);
  RUN_TEST(test_rsi_flash_erase_sector, __LINE__);
  RUN_TEST(test_rsi_flash_write, __LINE__);
  RUN_TEST(test_rsi_flash_read, __LINE__);
  RUN_TEST(test_rsi_flash_uninitialize, __LINE__);
  RUN_TEST(test_get_qspi_config, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initialization of flash
 ******************************************************************************/
void test_rsi_flash_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Flash Init \n");
  bool status;

  UnityPrintf("Testing with correct parameters \n");
  status = rsi_flash_init();
  TEST_ASSERT_TRUE(status);
  UnityPrintf("Status of API is correct, Flash Init successfully \n");

  UnityPrintf("Flash Init test completed \n");
}

/*******************************************************************************
 * Function to test erasing a flash sector
 ******************************************************************************/
void test_rsi_flash_erase_sector(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Flash Erase Sector \n");
  bool status;

  UnityPrintf("Testing with correct parameters \n");
  status = rsi_flash_erase_sector((uint32_t *)TEST_SECTOR_ADDRESS);
  TEST_ASSERT_TRUE(status);
  UnityPrintf("Status of API is correct, Flash Erase Sector successfully \n");

  UnityPrintf("Flash Erase Sector test completed \n");
}

/*******************************************************************************
 * Function to test writing to flash
 ******************************************************************************/
void test_rsi_flash_write(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Flash Write \n");
  bool status;

  UnityPrintf("Testing with correct parameters \n");
  status = rsi_flash_write((uint32_t *)TEST_ADDRESS, data, TEST_LENGTH);
  TEST_ASSERT_TRUE(status);
  UnityPrintf("Status of API is correct, Flash Write successfully \n");

  UnityPrintf("Flash Write test completed \n");
}

/*******************************************************************************
 * Function to test reading from flash
 ******************************************************************************/
void test_rsi_flash_read(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Flash Read \n");
  bool status;

  UnityPrintf("Testing with correct parameters \n");
  status = rsi_flash_read((uint32_t *)TEST_ADDRESS, data, TEST_LENGTH, TEST_AUTO_MODE);
  TEST_ASSERT_TRUE(status);
  UnityPrintf("Status of API is correct, Flash Read successfully \n");

  UnityPrintf("Flash Read test completed \n");
}

/*******************************************************************************
 * Function to test uninitializing flash
 ******************************************************************************/
void test_rsi_flash_uninitialize(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Flash Uninitialize \n");
  bool status;

  UnityPrintf("Testing with correct parameters \n");
  status = rsi_flash_uninitialize();
  TEST_ASSERT_TRUE(status);
  UnityPrintf("Status of API is correct, Flash Uninitialize successfully \n");

  UnityPrintf("Flash Uninitialize test completed \n");
}

/*******************************************************************************
 * Function to test getting QSPI configuration
 ******************************************************************************/
void test_get_qspi_config(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Get QSPI Config \n");

  UnityPrintf("Testing with correct parameters \n");
  get_qspi_config(&spi_config);
  UnityPrintf("Get QSPI Config successfully \n");

  UnityPrintf("Get QSPI Config test completed \n");
}