/***************************************************************************/ /**
* @file bjt_temperature_sensor_example.c
* @brief BJT Temperature Sensor example
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
#include "rsi_debug.h"
#include "sl_si91x_bjt_temperature_sensor.h"
#include "bjt_temperature_sensor_example.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/
#define SOC_PLL_CLK  ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor
#define INTF_PLL_CLK ((uint32_t)(180000000)) // 180MHz default Interface PLL Clock as source to all peripherals
/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/
static void default_clock_configuration(void);

/*******************************************************************************
 *************************** GLOBAL VARIABLES   *******************************
 ******************************************************************************/
extern adc_config_t sl_bjt_config;
extern adc_ch_config_t sl_bjt_channel_config;

/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
// Function to configure clock on powerup
static void default_clock_configuration(void)
{
  // Core Clock runs at 180MHz SOC PLL Clock
  sl_si91x_clock_manager_m4_set_core_clk(M4_SOCPLLCLK, SOC_PLL_CLK);

  // All peripherals' source to be set to Interface PLL Clock
  // and it runs at 180MHz
  sl_si91x_clock_manager_set_pll_freq(INFT_PLL, INTF_PLL_CLK, PLL_REF_CLK_VAL_XTAL);
}
/*******************************************************************************
 * ADC example initialization function
 ******************************************************************************/
void bjt_temperature_sensor_example_init(void)
{
  sl_status_t status;

  // default clock configuration by application common for whole system
  default_clock_configuration();

  status = sl_si91x_bjt_temperature_sensor_init(sl_bjt_channel_config, sl_bjt_config);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("sl_si91x_bjt_temp_init: Error Code : %lu \n", status);
    return;
  }
  /* Due to calling trim_efuse API on BJT Temperature sensor init in driver
     * it will change the clock frequency, if we are not initialize the debug
     * again it will print the garbage data in console output */
  DEBUGINIT();
  DEBUGOUT("BJT temperature sensor Initialization Success\n");
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void bjt_temperature_sensor_example_process_action(void)
{
  sl_status_t status;
  double temperature = 0;
  // ADC operation mode if STATIC then it will execute, here it will give
  // equivalent voltage of 12 bit adc output.
  status = sl_si91x_bjt_temperature_sensor_read_data(&temperature);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("BJT Temperature sensor read failed, Error Code: 0x%ld \n", status);
  }
  DEBUGOUT("BJT Temperature = %lf\n", temperature);
}
