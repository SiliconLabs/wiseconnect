/***************************************************************************/
/**
 * @file clock_manager_test_main.c
 * @brief Clock Manager Unity Test Functions
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
#include "sl_si91x_clock_manager.h"
#include "rsi_debug.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
M4_SOC_CLK_SRC_SEL_T clk_source = M4_SOC_CLK_SRC_SEL_T::M4_SOC_CLK_SRC_SEL_T_PLL;
uint32_t pll_freq               = 100; // Example PLL frequency in MHz
uint32_t pll_ref_clk            = 50;  // Example PLL reference clock frequency in MHz
PLL_TYPE_T pll_type             = PLL_TYPE_T::PLL_TYPE_T_M4PLL;

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sl_si91x_clock_manager_init(void);
void test_sl_si91x_clock_manager_m4_set_core_clk(void);
void test_sl_si91x_clock_manager_set_pll_clk(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("Clock Manager");

  RUN_TEST(test_sl_si91x_clock_manager_init, __LINE__);
  RUN_TEST(test_sl_si91x_clock_manager_m4_set_core_clk, __LINE__);
  RUN_TEST(test_sl_si91x_clock_manager_set_pll_clk, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initialization of Clock Manager
 ******************************************************************************/
void test_sl_si91x_clock_manager_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager Init \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_clock_manager_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager Init successfully \n");

  UnityPrintf("Clock Manager Init test completed \n");
}

/*******************************************************************************
 * Function to test setting M4 core clock source and PLL frequency
 ******************************************************************************/
void test_sl_si91x_clock_manager_m4_set_core_clk(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager M4 Set Core Clock \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(clk_source, pll_freq);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager M4 Set Core Clock successfully \n");

  UnityPrintf("Clock Manager M4 Set Core Clock test completed \n");
}

/*******************************************************************************
 * Function to test setting PLL clock frequency
 ******************************************************************************/
void test_sl_si91x_clock_manager_set_pll_clk(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager Set PLL Clock \n");
  sl_status_t status;

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_clock_manager_set_pll_clk(pll_type, pll_freq, pll_ref_clk);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager Set PLL Clock successfully \n");

  UnityPrintf("Clock Manager Set PLL Clock test completed \n");
}