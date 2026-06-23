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
#include "rsi_rom_clks.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
/* Core clock modification Macros */
#define ADC_MAX_OP_VALUE 4095   // Maximum output value get from adc data register
#define ADC_DATA_CLEAR   0xF7FF // Clear the data if 12th bit is enabled
#define VREF_VALUE       3.3    // reference voltage
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

/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * ADC example initialization function
 ******************************************************************************/
void adc_static_mode_example_init(void)
{
  sl_adc_version_t version;
  sl_status_t status;
  sl_adc_channel_config.channel = SL_ADC_CHANNEL_1;

  do {
    // Version information of ADC driver
    version = sl_si91x_adc_get_version();
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("ADC version is fetched successfully \n");
    SL_PRINT_STRING_ERROR("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    status = sl_si91x_adc_init(sl_adc_channel_config, sl_adc_config, vref_value);
    /* Due to calling trim_efuse API on ADC init in driver it will change the
      clock frequency, if we are not initialize the debug again it will print
      the garbage data in console output. */
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_init: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC Initialization Success\n");
    // Configure ADC channel.
    status = sl_si91x_adc_set_channel_configuration(sl_adc_channel_config, sl_adc_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_channel_set_configuration: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC Channel Configuration Successfully \n");
    // Register user callback function
    status = sl_si91x_adc_register_event_callback(callback_event);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC user event callback registered successfully \n");
    status = sl_si91x_adc_start(sl_adc_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_start: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC started Successfully\n");
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
        SL_PRINT_STRING_ERROR("sl_si91x_adc_read_data_static: Error Code : %lu \n", status);
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
        SL_PRINT_STRING_ERROR("Differential ended input  :%ldmV\n", (int32_t)(vout * 1000.0f));
      } else {
        SL_PRINT_STRING_ERROR("ADC Channel[%d] Measured input :%ldmV\n", channel_num, (int32_t)(vout * 1000.0f));
      }
      if (sl_adc_config.num_of_channel_enable > 1) {
        if (channel_num >= (sl_adc_config.num_of_channel_enable - 1)) {
          SL_PRINT_STRING_ERROR("\n\n");
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
