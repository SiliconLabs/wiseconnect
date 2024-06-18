/***************************************************************************/ /**
* @file adc_static_mode_example.c
* @brief ADC example for static mode of operation
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
#include "adc_static_mode_example.h"
#include "rsi_debug.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_adc_common_config.h"
#include "sl_si91x_clock_manager.h"
#include "rsi_rom_clks.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
/* Core clock modification Macros */
#define ADC_MAX_OP_VALUE 4095   // Maximum output value get from adc data register
#define ADC_DATA_CLEAR   0xF7FF // Clear the data if 12th bit is enabled
#define VREF_VALUE       3.3    // reference voltage

#define SOC_PLL_CLK          ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor
#define INTF_PLL_CLK         ((uint32_t)(180000000)) // 180MHz default Interface PLL Clock as source to all peripherals
#define QSPI_ODD_DIV_ENABLE  0                       // Odd division enable for QSPI clock
#define QSPI_SWALLO_ENABLE   0                       // Swallo enable for QSPI clock
#define QSPI_DIVISION_FACTOR 0                       // Division factor for QSPI clock
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vref_value                    = (float)VREF_VALUE;
static boolean_t data_sample_complete_flag = false;
static int16_t adc_output[1];

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void callback_event(uint8_t event_channel, uint8_t event);
static void default_clock_configuration(void);

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

  // Configure QSPI clock as input source
  ROMAPI_M4SS_CLK_API->clk_qspi_clk_config(M4CLK,
                                           QSPI_INTFPLLCLK,
                                           QSPI_SWALLO_ENABLE,
                                           QSPI_ODD_DIV_ENABLE,
                                           QSPI_DIVISION_FACTOR);
}
/*******************************************************************************
 * ADC example initialization function
 ******************************************************************************/
void adc_static_mode_example_init(void)
{
  sl_adc_version_t version;
  sl_status_t status;
  sl_adc_channel_config.channel = 0;

  // default clock configuration by application common for whole system
  default_clock_configuration();

  do {
    // Version information of ADC driver
    version = sl_si91x_adc_get_version();
    DEBUGOUT("ADC version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    status = sl_si91x_adc_init(sl_adc_channel_config, sl_adc_config, vref_value);
    /* Due to calling trim_efuse API on ADC init in driver it will change the
      clock frequency, if we are not initialize the debug again it will print
      the garbage data in console output. */
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_init: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("ADC Initialization Success\n");
    // Configure ADC channel.
    status = sl_si91x_adc_set_channel_configuration(sl_adc_channel_config, sl_adc_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_channel_set_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("ADC Channel Configuration Successfully \n");
    // Register user callback function
    status = sl_si91x_adc_register_event_callback(callback_event);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("ADC user event callback registered successfully \n");
    status = sl_si91x_adc_start(sl_adc_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_start: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("ADC started Successfully\n");
  } while (false);
}

/*******************************************************************************
 * Function will run continuously and will wait for trigger
 ******************************************************************************/
void adc_static_mode_example_process_action(void)
{
  sl_status_t status;
  uint16_t adc_value;
  uint8_t channel_num;
  float vout = 0.0f;
  for (channel_num = 0; channel_num < sl_adc_config.num_of_channel_enable; channel_num++) {
    if (data_sample_complete_flag) {
      data_sample_complete_flag = false;
      status                    = sl_si91x_adc_read_data_static(sl_adc_channel_config, sl_adc_config, &adc_value);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_adc_read_data_static: Error Code : %lu \n", status);
      }
      // Read the data from register and store it in variable.
      adc_output[0] = (int16_t)adc_value;
      /* In two’s complement format, the MSb (11th bit) of the conversion result
         determines the polarity, when the MSb = ‘0’, the result is positive,
         and when the MSb = ‘1’, the result is negative*/
      if (adc_output[0] & SIGN_BIT) {
        // Full-scale would be represented by a hexadecimal value, full-scale
        // range of ADC result values in two’s complement.
        adc_output[0] = (int16_t)(adc_output[0] & (ADC_DATA_CLEAR));
      } else { // set the MSb bit.
        adc_output[0] = adc_output[0] | SIGN_BIT;
      }
      vout = (((float)adc_output[0] / (float)ADC_MAX_OP_VALUE) * vref_value);
      // For differential type it will give vout.
      if (sl_adc_channel_config.input_type[channel_num]) {
        vout = vout - (vref_value / 2);
        DEBUGOUT("Differential ended input  :%lf\n", (double)vout);
      } else {
        DEBUGOUT("ADC Channel[%d] Measured input :%lf\n", channel_num, (double)vout);
      }
      if (sl_adc_config.num_of_channel_enable > 1) {
        if (channel_num >= (sl_adc_config.num_of_channel_enable - 1)) {
          DEBUGOUT("\n\n");
        }
      }
    }
  }
}

/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by ADC interface
 * @param  event       : INTERNAL_DMA => Single channel data acquisition done.
 *                       ADC_STATIC_MODE_CALLBACK => Static mode adc data
 *                       acquisition done.
 ******************************************************************************/
static void callback_event(uint8_t event_channel, uint8_t event)
{
  (void)event_channel;
  if (event == SL_ADC_STATIC_MODE_EVENT) {
    data_sample_complete_flag = true;
  }
}
