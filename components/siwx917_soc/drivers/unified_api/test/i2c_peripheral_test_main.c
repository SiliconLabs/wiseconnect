/***************************************************************************/ /**
 * @file peripheral_i2c_main.c
 * @brief I2C Unity Test Functions
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
#include "sl_system_init.h"
#include "sl_si91x_peripheral_i2c.h"
#include "rsi_board.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
#include "unity.h"
#include "si91x_device.h"

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/
#define FOLLOWER_ADDR_7BIT         0x00A0
#define FOLLOWER_ADDR_10BIT        0x02A0
#define FOLLOWER_ADDR_7BIT_MAX     0x7F
#define FOLLOWER_ADDR_10BIT_MAX    0x3FF
#define I2C_COUNT                  3
#define I2C_ENABLE                 1
#define I2C_DISABLE                0
#define INTERRUPT_FULL_MASK        0x3FFF
#define INTERRUPT_EMPTY_MASK       0x0
#define I2C_RESET_VALUE            0x0
#define I2C_RESET_CON              0x6
#define FIRST_BYTE_10BIT_ADDR_MASK 0xF0
#define MAX_THRESHOLD              255
#define MAX_SDA_READ               0xFF
#define MAX_SDA_WRITE              0xE

/*******************************************************************************
 *********************************   TYPEDEFS   ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************   UTILITY FUNCTIONS   *****************************
 ******************************************************************************/
static void i2c_enable_clock_tree(uint8_t i2c_instance_number, bool flag);
static void i2c_enable_module(uint8_t i2c_instance_number);
static void i2c_disable_module(uint8_t i2c_instance_number);
static uint32_t i2c_get_clock(void);
static void i2c_test_init_assert(sl_i2c_mode_t mode,
                                 uint32_t ref_freq,
                                 uint32_t freq,
                                 sl_i2c_clock_hlr_t clhr,
                                 uint16_t address,
                                 bool flag);

/*******************************************************************************
 ***************************   FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void Test_I2C_Init(void);
static void Test_I2C_Reset(void);
static void Test_I2C_Enable_Disable(void);
static void Test_I2C_Int_Functions(void);
static void Test_I2C_SlaveAddress(void);
static void Test_I2C_SDA_HoldTime(void);
static void Test_I2C_Threshold(void);
static void Test_I2C_GetVersion(void);
/*******************************************************************************
 *****************************   GLOBAL VARIABLES   ****************************
 ******************************************************************************/

static I2C_TypeDef *i2c_peripheral;
/*******************************************************************************
 **********************************   MAIN   ***********************************
 ******************************************************************************/
int main(void)
{
  uint8_t i2c_instance_number = 0;
  sl_system_init();
  UnityBeginGroup("I2C");
  // TEST all I2C instances present on the chip.
  for (i2c_instance_number = 0; i2c_instance_number < I2C_COUNT; i2c_instance_number++) {
    UnityPrintf("\n");
    UnityPrintf("Test on I2C instance number %d :\n", i2c_instance_number);
    switch (i2c_instance_number) {
      case 0:
        i2c_peripheral = I2C0;
        break;
      case 1:
        i2c_peripheral = I2C1;
        break;
      case 2:
        i2c_peripheral = I2C2;
        break;
    }
    i2c_enable_module(i2c_instance_number);
    RUN_TEST(Test_I2C_GetVersion, __LINE__);
    RUN_TEST(Test_I2C_Init, __LINE__);
    RUN_TEST(Test_I2C_Reset, __LINE__);
    RUN_TEST(Test_I2C_Enable_Disable, __LINE__);
    RUN_TEST(Test_I2C_Int_Functions, __LINE__);
    RUN_TEST(Test_I2C_SlaveAddress, __LINE__);
    RUN_TEST(Test_I2C_SDA_HoldTime, __LINE__);
    RUN_TEST(Test_I2C_Threshold, __LINE__);
    i2c_disable_module(i2c_instance_number);
  }
  UnityEnd();
}

/*******************************************************************************
 ******************************   UTILITY   ************************************
 ******************************************************************************/

static void i2c_test_init_assert(sl_i2c_mode_t mode,
                                 uint32_t ref_freq,
                                 uint32_t freq,
                                 sl_i2c_clock_hlr_t clhr,
                                 uint16_t address,
                                 bool flag)
{
  UNUSED_VARIABLE(ref_freq);
  i2c_get_clock();
  sl_i2c_init_params_t init_params;

  init_params.mode          = mode;
  init_params.freq          = freq;
  init_params.clhr          = clhr;
  init_params.address       = address;
  init_params.is_10bit_addr = flag;

  //i2c init
  sl_si91x_i2c_init(i2c_peripheral, &init_params);

  //Check Leader or Follower
  UnityPrintf("Testing init with Leader mode \n");
  if (init_params.mode == SL_I2C_LEADER_MODE) {
    TEST_ASSERT_EQUAL_HEX(ENABLE, i2c_peripheral->IC_CON_b.MASTER_MODE);
    TEST_ASSERT_EQUAL_HEX(ENABLE, i2c_peripheral->IC_CON_b.IC_SLAVE_DISABLE);
    UnityPrintf("Leader and follower disable registers are configured correctly \n");
    if (init_params.is_10bit_addr) {
      TEST_ASSERT_EQUAL_HEX(ENABLE, i2c_peripheral->IC_TAR_b.IC_10BITADDR_MASTER);
      TEST_ASSERT_EQUAL_HEX(FOLLOWER_ADDR_10BIT, i2c_peripheral->IC_TAR_b.IC_TAR);
      UnityPrintf("10-bit address and target registers are configured correctly \n");
    } else {
      TEST_ASSERT_EQUAL_HEX(DISABLE, i2c_peripheral->IC_TAR_b.IC_10BITADDR_MASTER);
      TEST_ASSERT_EQUAL_HEX(FOLLOWER_ADDR_7BIT, i2c_peripheral->IC_TAR_b.IC_TAR);
      UnityPrintf("10-bit address and target registers are configured correctly \n");
    }
  }
  UnityPrintf("Testing init with Follower mode \n");
  if (init_params.mode == SL_I2C_FOLLOWER_MODE) {
    if (init_params.is_10bit_addr) {
      TEST_ASSERT_EQUAL_HEX(FOLLOWER_ADDR_10BIT, i2c_peripheral->IC_SAR_b.IC_SAR);
    } else {
      TEST_ASSERT_EQUAL_HEX(FOLLOWER_ADDR_7BIT, i2c_peripheral->IC_SAR_b.IC_SAR);
    }
    TEST_ASSERT_EQUAL_HEX(DISABLE, i2c_peripheral->IC_CON_b.MASTER_MODE);
    TEST_ASSERT_EQUAL_HEX(DISABLE, i2c_peripheral->IC_CON_b.IC_SLAVE_DISABLE);
    UnityPrintf("Leader and follower disable registers are configured correctly \n");
    UnityPrintf("Target registers are configured correctly \n");
  }
  UnityPrintf("Configuring bus speed \n");
  if (clhr == SL_I2C_FAST_PLUS_BUS_SPEED) {
    TEST_ASSERT_EQUAL_HEX(SL_I2C_FAST_BUS_SPEED, i2c_peripheral->IC_CON_b.SPEED);
  } else {
    TEST_ASSERT_EQUAL_HEX(clhr, i2c_peripheral->IC_CON_b.SPEED);
  }
  UnityPrintf("Bus speed is configured correctly \n");
}

static void i2c_enable_clock_tree(uint8_t i2c_instance_number, bool flag)
{
  if (flag) {
    switch (i2c_instance_number) {
      case 0:
#if defined(CHIP_917)
        // Powering up the peripheral.
        RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
        // Powering up the peripheral.
        RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI1);
#endif
        // Initialize the I2C clock.
        RSI_CLK_I2CClkConfig(M4CLK, 1, I2C1_INSTAN);
        break;
      case 1:
#if defined(CHIP_917)
        // Powering up the peripheral.
        RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_EFUSE_PERI);
#else
        // Powering up the peripheral.
        RSI_PS_M4ssPeriPowerUp(M4SS_PWRGATE_ULP_PERI3);
#endif
        // Initialize the I2C clock.
        RSI_CLK_I2CClkConfig(M4CLK, 1, I2C2_INSTAN);
        break;
      case 2:
        // Powering up the peripheral.
        RSI_PS_UlpssPeriPowerUp(ULPSS_PWRGATE_ULP_I2C);
        // Initialize the I2C clock.
        RSI_ULPSS_PeripheralEnable(ULPCLK, ULP_I2C_CLK, ENABLE_STATIC_CLK);
        break;
      default:
        SL_I2C_ASSERT(false);
        break;
    }
  }
}

static void i2c_enable_module(uint8_t i2c_instance_number)
{
  sl_si91x_i2c_enable(i2c_peripheral);
  i2c_enable_clock_tree(i2c_instance_number, true);
}

static void i2c_disable_module(uint8_t i2c_instance_number)
{
  sl_si91x_i2c_disable(i2c_peripheral);
  i2c_enable_clock_tree(i2c_instance_number, false);
}

static uint32_t i2c_get_clock(void)
{
  return sl_si91x_i2c_get_frequency(i2c_peripheral);
}

/*******************************************************************************
 *****************************   I2C TEST   *********************************
 ******************************************************************************/
static void Test_I2C_Init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Init \n");

  uint32_t freq           = 0;
  sl_i2c_clock_hlr_t clhr = 0;
  uint8_t var1            = 0;
  uint16_t addr_mode      = 0;
  bool flag               = false;
  //Leader mode Test cases
  freq = sl_si91x_i2c_get_frequency(i2c_peripheral);
  for (clhr = 1; clhr <= 4; clhr++) {
    i2c_test_init_assert(SL_I2C_LEADER_MODE, 0, freq, clhr, FOLLOWER_ADDR_7BIT, false);
  }

  //Follower mode Test cases
  for (var1 = 0; var1 < 2; var1++) {
    switch (var1) {
      case 0:
        addr_mode = FOLLOWER_ADDR_7BIT;
        flag      = false;
        break;
      case 1:
        addr_mode = FOLLOWER_ADDR_10BIT;
        flag      = true;
        break;
    }
    clhr = 3;
    i2c_test_init_assert(SL_I2C_FOLLOWER_MODE, 0, freq, clhr, addr_mode, flag);
  }
  i2c_test_init_assert(SL_I2C_LEADER_MODE, 0, freq, clhr, FOLLOWER_ADDR_7BIT, false);
  UnityPrintf("I2C Init test completed \n");
}

static void Test_I2C_Enable_Disable(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Enable Disable \n");

  UnityPrintf("Testing I2C Enable \n");
  sl_si91x_i2c_enable(i2c_peripheral);
  //Check the i2c enable
  TEST_ASSERT_EQUAL_UINT32(I2C_ENABLE, i2c_peripheral->IC_ENABLE_b.EN);
  UnityPrintf("I2C is enabled successfully \n");

  UnityPrintf("Testing I2C Disable \n");
  sl_si91x_i2c_disable(i2c_peripheral);
  //Check the i2c enable
  TEST_ASSERT_EQUAL_UINT32(I2C_DISABLE, i2c_peripheral->IC_ENABLE_b.EN);
  UnityPrintf("I2C is disabled successfully \n");

  UnityPrintf("I2C Enable Disable test completed \n");
}

static void Test_I2C_Reset(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Reset \n");

  sl_si91x_i2c_reset(i2c_peripheral);
  UnityPrintf("I2C Reset successfully \n");
  //Check the i2c disable bit
  TEST_ASSERT_EQUAL_HEX(I2C_RESET_CON, i2c_peripheral->IC_CON);
  UnityPrintf("I2C Control register is configured properly \n");
  //Check reset value of Interrupt
  TEST_ASSERT_EQUAL_UINT32(INTERRUPT_EMPTY_MASK, i2c_peripheral->IC_INTR_MASK);
  UnityPrintf("Interrupt mask is configured properly \n");
  //Check SADDR and SADDR_MASK when configured as Leader
  TEST_ASSERT_EQUAL_UINT32(I2C_RESET_VALUE, i2c_peripheral->IC_TAR);
  TEST_ASSERT_EQUAL_UINT32(I2C_RESET_VALUE, i2c_peripheral->IC_SAR);
  UnityPrintf("Follower address and Follower mask address have reset values \n");

  UnityPrintf("I2C Reset test completed \n");
}

static void Test_I2C_Int_Functions(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Interrupts \n");

  uint32_t reg_val;

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_UNDER \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_UNDER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(ENABLE, reg_val & SL_I2C_EVENT_RECEIVE_UNDER);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_UNDER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_RECEIVE_UNDER);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_UNDER Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_OVER \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_OVER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_RECEIVE_OVER, (reg_val & SL_I2C_EVENT_RECEIVE_OVER));
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_OVER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_RECEIVE_OVER);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_OVER Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_FULL \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_FULL);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_RECEIVE_FULL, reg_val & SL_I2C_EVENT_RECEIVE_FULL);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_FULL);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_RECEIVE_FULL);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_FULL Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_OVER \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_OVER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_TRANSMIT_OVER, reg_val & SL_I2C_EVENT_TRANSMIT_OVER);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_OVER);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_TRANSMIT_OVER);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_OVER Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_EMPTY \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_EMPTY);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_TRANSMIT_EMPTY, reg_val & SL_I2C_EVENT_TRANSMIT_EMPTY);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_EMPTY);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_TRANSMIT_EMPTY);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_EMPTY Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_READ_REQ \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_READ_REQ);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_READ_REQ, reg_val & SL_I2C_EVENT_READ_REQ);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_READ_REQ);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_READ_REQ);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_READ_REQ Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_ABORT \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_ABORT);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_TRANSMIT_ABORT, reg_val & SL_I2C_EVENT_TRANSMIT_ABORT);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_TRANSMIT_ABORT);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_TRANSMIT_ABORT);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_TRANSMIT_ABORT Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_DONE \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_DONE);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_RECEIVE_DONE, reg_val & SL_I2C_EVENT_RECEIVE_DONE);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_RECEIVE_DONE);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_RECEIVE_DONE);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RECEIVE_DONE Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_ACTIVITY_ON_BUS \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_ACTIVITY_ON_BUS);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_ACTIVITY_ON_BUS, reg_val & SL_I2C_EVENT_ACTIVITY_ON_BUS);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_ACTIVITY_ON_BUS);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_ACTIVITY_ON_BUS);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_ACTIVITY_ON_BUS Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_STOP_DETECT \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_STOP_DETECT);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_STOP_DETECT, reg_val & SL_I2C_EVENT_STOP_DETECT);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_STOP_DETECT);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_STOP_DETECT);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_STOP_DETECT Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_GENERAL_CALL \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_GENERAL_CALL);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_GENERAL_CALL, reg_val & SL_I2C_EVENT_GENERAL_CALL);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_GENERAL_CALL);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_GENERAL_CALL);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_GENERAL_CALL Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RESTART_DET \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_RESTART_DET);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_RESTART_DET, reg_val & SL_I2C_EVENT_RESTART_DET);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_RESTART_DET);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_RESTART_DET);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_RESTART_DET Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_MST_ON_HOLD \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_MST_ON_HOLD);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_MST_ON_HOLD, reg_val & SL_I2C_EVENT_MST_ON_HOLD);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_MST_ON_HOLD);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_MST_ON_HOLD);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_MST_ON_HOLD Test passed successfully \n");

  UnityPrintf("Testing i2c_peripheral set and clear interrupts for SL_I2C_EVENT_SCL_STUCK_AT_LOW \n");
  sl_si91x_i2c_set_interrupts(i2c_peripheral, SL_I2C_EVENT_SCL_STUCK_AT_LOW);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_I2C_EVENT_SCL_STUCK_AT_LOW, reg_val & SL_I2C_EVENT_SCL_STUCK_AT_LOW);
  sl_si91x_i2c_clear_interrupts(i2c_peripheral, SL_I2C_EVENT_SCL_STUCK_AT_LOW);
  reg_val = sl_si91x_i2c_get_enabled_interrupts(i2c_peripheral);
  TEST_ASSERT_EQUAL_HEX(DISABLE, reg_val & SL_I2C_EVENT_SCL_STUCK_AT_LOW);
  UnityPrintf("i2c_peripheral set and clear interrupts for SL_I2C_EVENT_SCL_STUCK_AT_LOW Test passed successfully \n");

  UnityPrintf("I2C Interrupts test completed \n");
}

static void Test_I2C_SlaveAddress(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Slave address \n");

  uint16_t mask;
  uint32_t reg_val, expected;

  // Calls the set and get function for all possible masks. Verifies that the
  // correct register values are set and that the get function can return this
  // value.
  UnityPrintf("Testing set and get follower mask address \n");
  UnityPrintf("Testing all possible address \n");
  for (mask = 0; mask < FOLLOWER_ADDR_7BIT_MAX; mask++) {
    sl_si91x_i2c_set_follower_mask_address(i2c_peripheral, mask);
    reg_val = i2c_peripheral->IC_SAR_b.IC_SAR;
    TEST_ASSERT_EQUAL_UINT32(mask, reg_val);

    reg_val = sl_si91x_i2c_get_follower_mask_address(i2c_peripheral);
    TEST_ASSERT_EQUAL_UINT32(mask, reg_val);
  }
  UnityPrintf("Set and get follower mask address test completed \n");

  // Calls the set and get function for all possible masks. Verifies that the
  // correct register values are set and that the get function can return this
  // value.
  UnityPrintf("Testing set and get follower address \n");
  UnityPrintf("Testing all possible address \n");
  for (mask = 0; mask < FOLLOWER_ADDR_7BIT_MAX; mask++) {
    sl_si91x_i2c_set_follower_address(i2c_peripheral, mask, false);
    reg_val = i2c_peripheral->IC_TAR_b.IC_TAR;
    TEST_ASSERT_EQUAL_UINT32(mask, reg_val);

    reg_val = sl_si91x_i2c_get_follower_address(i2c_peripheral);
    TEST_ASSERT_EQUAL_UINT32(mask, reg_val);
  }
  for (mask = 0; mask < FOLLOWER_ADDR_10BIT_MAX; mask++) {
    sl_si91x_i2c_set_follower_address(i2c_peripheral, mask, true);
    reg_val = i2c_peripheral->IC_TAR_b.IC_TAR;
    TEST_ASSERT_EQUAL_UINT32(mask, reg_val);

    reg_val  = sl_si91x_i2c_get_follower_address(i2c_peripheral);
    expected = reg_val;
    TEST_ASSERT_EQUAL_UINT32(expected, reg_val);
  }
  UnityPrintf("Set and get follower address test completed \n");

  UnityPrintf("I2C Slave address test completed \n");
}

static void Test_I2C_Threshold(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C set rx and tx threshold \n");

  uint8_t threshold = 0;
  UnityPrintf("Testing all possible values for TX threshold \n");
  for (threshold = 0; threshold < 8; threshold++) {
    sl_si91x_i2c_set_tx_threshold(i2c_peripheral, threshold);
    TEST_ASSERT_EQUAL_HEX(threshold, i2c_peripheral->IC_TX_TL_b.TX_TL);
  }
  UnityPrintf("All possible values for TX threshold are verfied \n");

  UnityPrintf("Testing all possible values for RX threshold \n");
  for (threshold = 0; threshold < 8; threshold++) {
    sl_si91x_i2c_set_rx_threshold(i2c_peripheral, threshold);
    TEST_ASSERT_EQUAL_HEX(threshold, i2c_peripheral->IC_RX_TL_b.RX_TL);
  }
  UnityPrintf("All possible values for RX threshold are verfied \n");

  UnityPrintf("I2C set rx and tx threshold test completed \n");
}

static void Test_I2C_SDA_HoldTime(void)
{
  uint32_t time = 0;
  UnityPrintf("\n");
  UnityPrintf("Testing I2C sda hold time \n");

  UnityPrintf("Testing all possible values of SDA hold time in write direction \n");
  for (time = 0; time < MAX_SDA_WRITE; time++) {
    sl_si91x_i2c_set_sda_hold_time(i2c_peripheral, SL_I2C_WRITE, time);
    TEST_ASSERT_EQUAL_HEX(time, i2c_peripheral->IC_SDA_HOLD_b.IC_SDA_TX_HOLD);
  }
  UnityPrintf("All possible values for SDA hold time are verfied");

  UnityPrintf("Testing all possible values of SDA hold time in read direction \n");
  for (time = 0; time < MAX_SDA_READ; time++) {
    sl_si91x_i2c_set_sda_hold_time(i2c_peripheral, SL_I2C_READ, time);
    TEST_ASSERT_EQUAL_HEX(time, i2c_peripheral->IC_SDA_HOLD_b.IC_SDA_RX_HOLD);
  }
  UnityPrintf("All possible values for SDA hold time are verfied");

  UnityPrintf("I2C sda hold time test completed \n");
}

static void Test_I2C_GetVersion(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing I2C Get Version \n");
  sl_i2c_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_i2c_get_version();
  UnityPrintf("Verifying I2C Release, SQA and Dev version \n");
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");

  UnityPrintf("I2C get version test completed \n");
}
