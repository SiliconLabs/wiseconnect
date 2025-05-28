#include "unity.h"
#include "sl_memlcd.h"

// Mock memory LCD device structure
struct sl_memlcd_t mock_device;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sl_memlcd_configure(void);
void test_sl_memlcd_display_enable(void);
void test_sl_memlcd_display_disable(void);
void test_sl_memlcd_power_on(void);
void test_sl_memlcd_post_wakeup_init(void);
void test_sl_memlcd_clear(void);
void test_sl_memlcd_draw(void);
void test_sl_memlcd_refresh(void);
void test_sl_memlcd_get(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("MEMLCD");

  RUN_TEST(test_sl_memlcd_configure, __LINE__);
  RUN_TEST(test_sl_memlcd_display_enable, __LINE__);
  RUN_TEST(test_sl_memlcd_display_disable, __LINE__);
  RUN_TEST(test_sl_memlcd_power_on, __LINE__);
  RUN_TEST(test_sl_memlcd_post_wakeup_init, __LINE__);
  RUN_TEST(test_sl_memlcd_clear, __LINE__);
  RUN_TEST(test_sl_memlcd_draw, __LINE__);
  RUN_TEST(test_sl_memlcd_refresh, __LINE__);
  RUN_TEST(test_sl_memlcd_get, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test configuring the memory LCD
 ******************************************************************************/
void test_sl_memlcd_configure(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Configure \n");
  sl_status_t status;

  status = sl_memlcd_configure(&mock_device);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD configured successfully \n");
}

/*******************************************************************************
 * Function to test enabling the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_display_enable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Display Enable \n");

  sl_memlcd_display_enable();
  // Assuming there's a way to verify the display enable, add assertions here
  UnityPrintf("MEMLCD display enabled successfully \n");
}

/*******************************************************************************
 * Function to test disabling the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_display_disable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Display Disable \n");

  sl_memlcd_display_disable();
  // Assuming there's a way to verify the display disable, add assertions here
  UnityPrintf("MEMLCD display disabled successfully \n");
}

/*******************************************************************************
 * Function to test powering on/off the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_power_on(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Power On \n");
  sl_status_t status;

  status = sl_memlcd_power_on(&mock_device, true);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD powered on successfully \n");

  status = sl_memlcd_power_on(&mock_device, false);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD powered off successfully \n");
}

/*******************************************************************************
 * Function to test post wakeup initialization of the memory LCD
 ******************************************************************************/
void test_sl_memlcd_post_wakeup_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Post Wakeup Init \n");
  sl_status_t status;

  status = sl_memlcd_post_wakeup_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD post wakeup initialized successfully \n");
}

/*******************************************************************************
 * Function to test clearing the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_clear(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Clear \n");
  sl_status_t status;

  status = sl_memlcd_clear(&mock_device);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD cleared successfully \n");
}

/*******************************************************************************
 * Function to test drawing on the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_draw(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Draw \n");
  sl_status_t status;
  const void *data       = NULL; // Mock data
  unsigned int row_start = 0;
  unsigned int row_count = 10;

  status = sl_memlcd_draw(&mock_device, data, row_start, row_count);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD draw successfully \n");
}

/*******************************************************************************
 * Function to test refreshing the memory LCD display
 ******************************************************************************/
void test_sl_memlcd_refresh(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Refresh \n");
  sl_status_t status;

  status = sl_memlcd_refresh(&mock_device);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("MEMLCD refreshed successfully \n");
}

/*******************************************************************************
 * Function to test getting the memory LCD handle
 ******************************************************************************/
void test_sl_memlcd_get(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing MEMLCD Get \n");

  const sl_memlcd_t *lcd = sl_memlcd_get();
  TEST_ASSERT_NOT_NULL(lcd);
  UnityPrintf("MEMLCD handle retrieved successfully \n");
}