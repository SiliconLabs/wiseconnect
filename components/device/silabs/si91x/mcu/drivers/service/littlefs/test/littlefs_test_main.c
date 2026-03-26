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
#include "cmsis_os2.h"
#include "sl_wifi_constants.h"
#include "sl_wifi_device.h"
#include "sl_net_constants.h"
#include "sl_net.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TEST_BLOCK      0x00000000 // Example block address
#define TEST_OFFSET     0x0000     // Example offset
#define TEST_SIZE       256        // Example size
#define LITTLE_FS_OK    0          // LittleFS success code
#define LITTLE_FS_ERROR 1          // LittleFS Error code

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
const struct lfs_config cfg = {
  .read   = si91x_block_device_read,   // Function to read data from the block device
  .prog   = si91x_block_device_prog,   // Function to program data to the block device
  .erase  = si91x_block_device_erase,  // Function to erase a block on the block device
  .sync   = si91x_block_device_sync,   // Function to synchronize the block device
  .lock   = si91x_block_device_lock,   // Function to lock the block device
  .unlock = si91x_block_device_unlock, // Function to unlock the block device

  .read_size      = 64,   // Minimum size of a read operation in bytes
  .prog_size      = 64,   // Minimum size of a program operation in bytes
  .block_size     = 4096, // Size of an erasable block in bytes
  .block_count    = 64,   // Number of blocks in the block device
  .cache_size     = 256,  // Size of the cache in bytes
  .lookahead_size = 32,   // Size of the lookahead buffer in bytes
  .block_cycles   = 500,  // Number of erase cycles before the block is considered worn out
};
static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION
#ifdef SLI_SI91X_MCU_INTERFACE
                                       | SL_SI91X_FEAT_WPS_DISABLE
#endif
                                       ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT),
                   .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (
#ifdef SLI_SI91X_MCU_INTERFACE
                     SL_SI91X_RAM_LEVEL_NWP_ADV_MCU_BASIC
#else
                     SL_SI91X_RAM_LEVEL_NWP_ALL_MCU_ZERO
#endif
#if defined(SLI_SI917) || defined(SLI_SI915)
                     | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                     ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = 0,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};
/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_si91x_block_device_read(void);
void test_si91x_block_device_prog(void);
void test_si91x_block_device_erase(void);
void test_si91x_block_device_sync(void);
void test_si91x_block_device_lock_and_unlock(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int app_init()
{
  UnityBeginGroup("LittleFS HAL");
  sl_status_t status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi client interface: 0x%lx\r\n", status);
    return 0;
  }
  RUN_TEST(test_si91x_block_device_read, __LINE__);
  RUN_TEST(test_si91x_block_device_prog, __LINE__);
  RUN_TEST(test_si91x_block_device_erase, __LINE__);
  RUN_TEST(test_si91x_block_device_sync, __LINE__);
  RUN_TEST(test_si91x_block_device_lock_and_unlock, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
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
  TEST_ASSERT_EQUAL_HEX(0, status);
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
/*******************************************************************************
 * Function to test lock and unlock LittleFS block device
 ******************************************************************************/
void test_si91x_block_device_lock_and_unlock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing  block device lock callback \n");
  int result = si91x_block_device_lock(&cfg);
  TEST_ASSERT_EQUAL(LITTLE_FS_OK, result);
  UnityPrintf("Tested  block device lock callback done\n");
  result = si91x_block_device_unlock(&cfg);
  TEST_ASSERT_EQUAL_HEX(LITTLE_FS_OK, result);
  UnityPrintf("tested  block device unlock callback \n");
  UnityPrintf("Testing  block device lock and unlock callback API's completed successfully \n");
}
