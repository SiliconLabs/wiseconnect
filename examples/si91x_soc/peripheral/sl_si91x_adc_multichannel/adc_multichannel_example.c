/***************************************************************************/ /**
* @file adc_multichannel_example.c
* @brief ADC multichannel example
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
#include "sl_si91x_adc.h"
#include "adc_multichannel_example.h"
#include "rsi_debug.h"
#include "rsi_chip.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc_common_config.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/
/* Core clock modification Macros */
#define PS4_SOC_FREQ          180000000  /*<! PLL out clock 180MHz            */
#define SOC_PLL_REF_FREQUENCY 40000000   /*<! PLL input REFERENCE clock 40MHZ */
#define DVISION_FACTOR        0          // Division factor
#define CHANNEL_SAMPLE_LENGTH 1023       // Number of ADC sample collect for operation
#define ADC_MAX_OP_VALUE      4095       // Maximum output value get from adc data register
#define ADC_DATA_CLEAR        0xF7FF     // Clear the data if 12th bit is enabled
#define VREF_VALUE            3.3        // reference voltage
#define ADC_PING_BUFFER_1     0x0000A000 // Start address of Ping for channel_0
#define ADC_PING_BUFFER_2     0x0000B000 // Start address of Ping for channel_1
#define ADC_PING_BUFFER_3     0x0000C000 // Start address of Ping for channel_2
#define ADC_PING_BUFFER_4     0x0000D000 // Start address of Ping for channel_3

/*******************************************************************************
*************************** LOCAL VARIABLES   *******************************
******************************************************************************/
static float vref_value = (float)VREF_VALUE;
static int16_t adc_output[CHANNEL_SAMPLE_LENGTH];
static boolean_t ch_flags[NUMBER_OF_CHANNEL];
/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/
static void callback_event(uint8_t channel_no, uint8_t event);

/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/

/*******************************************************************************
* ADC example initialization function
******************************************************************************/
void adc_multichannel_example_init(void)
{
  sl_adc_version_t version;
  sl_status_t status;
  sl_adc_clock_config_t clock_config;
  clock_config.soc_pll_clock           = PS4_SOC_FREQ;
  clock_config.soc_pll_reference_clock = SOC_PLL_REF_FREQUENCY;
  clock_config.division_factor         = DVISION_FACTOR;

  sl_adc_channel_config.rx_buf[0] =
    adc_output; /* In order for us to read the data from adc_output in the sample application, ADC will save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[0] =
    ADC_PING_BUFFER_1; /* Starting address of ADC Ping buffer for channel 0 */
  sl_adc_channel_config.chnl_pong_address[0] =
    ADC_PING_BUFFER_1
    + (sl_adc_channel_config.num_of_samples[0]); /* Starting address of ADC Pong buffer for channel 0 */

  sl_adc_channel_config.rx_buf[1] =
    adc_output; /* In order for us to read the data from adc_output in the sample application, ADC will save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[1] =
    ADC_PING_BUFFER_2; /* Starting address of ADC Ping buffer for channel 1 */
  sl_adc_channel_config.chnl_pong_address[1] =
    (ADC_PING_BUFFER_2
     + (sl_adc_channel_config.num_of_samples[1])); /* Starting address of ADC Pong buffer for channel 1 */

  sl_adc_channel_config.rx_buf[2] =
    adc_output; /* In order for us to read the data from adc_output in the sample application, ADC will save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[2] =
    ADC_PING_BUFFER_3; /* Starting address of ADC Ping buffer for channel 2 */
  sl_adc_channel_config.chnl_pong_address[2] =
    (ADC_PING_BUFFER_3
     + (sl_adc_channel_config.num_of_samples[2])); /* Starting address of ADC Pong buffer for channel 2 */

  sl_adc_channel_config.rx_buf[3] =
    adc_output; /* In order for us to read the data from adc_output in the sample application, ADC will save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[3] =
    ADC_PING_BUFFER_4; /* Starting address of ADC Ping buffer for channel 3 */
  sl_adc_channel_config.chnl_pong_address[3] =
    (ADC_PING_BUFFER_4
     + (sl_adc_channel_config.num_of_samples[3])); /* Starting address of ADC Pong buffer for channel 3 */

  do {
    // Version information of ADC driver
    version = sl_si91x_adc_get_version();
    DEBUGOUT("ADC version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    // Configure ADC clock
    status = sl_si91x_adc_configure_clock(&clock_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_clock_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Clock configuration is successful \n");

    status = sl_si91x_adc_init(sl_adc_channel_config, sl_adc_config, vref_value);
    /* Due to calling trim_efuse API on ADC init in driver it will change the clock frequency,
        if we are not initialize the debug again it will print the garbage data in console output. */
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_adc_init: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("ADC Initialization Success\n");
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
void adc_multichannel_example_process_action(void)
{
  sl_status_t status;
  uint32_t sample_length;
  uint8_t chnl_num = 0;
  float vout       = 0;

  // here we get the 12-bit value of ADC output in equivalent voltage.
  for (chnl_num = 0; chnl_num < sl_adc_config.num_of_channel_enable; chnl_num++) {
    if (ch_flags[chnl_num] == true) {
      ch_flags[chnl_num] = false;
      status             = sl_si91x_adc_read_data(sl_adc_channel_config, chnl_num);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_adc_read_data: Error Code : %lu \n", status);
      }
      for (sample_length = 0; sample_length < sl_adc_channel_config.num_of_samples[chnl_num]; sample_length++) {
        /* In two’s complement format, the MSb (11th bit) of the conversion result determines the polarity,
         when the MSb = ‘0’, the result is positive, and when the MSb = ‘1’, the result is negative*/
        if (adc_output[sample_length] & SIGN_BIT) {
          // Full-scale would be represented by a hexadecimal value, full-scale range of ADC result values in two’s complement.
          adc_output[sample_length] = (int16_t)(adc_output[sample_length] & (ADC_DATA_CLEAR));
        } else { // set the MSb bit.
          adc_output[sample_length] = adc_output[sample_length] | SIGN_BIT;
        }

        vout = (((float)adc_output[sample_length] / (float)ADC_MAX_OP_VALUE) * vref_value);
        //For differential type it will give vout.
        if (sl_adc_channel_config.input_type[chnl_num]) {
          vout = vout - (vref_value / 2);
        }
        DEBUGOUT("ADC channel_%d[%ld] :%0.2fV \n", chnl_num, sample_length, (float)vout);
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
static void callback_event(uint8_t channel_no, uint8_t event)
{
  if (event == SL_INTERNAL_DMA) {
    if (channel_no == 0) {
      ch_flags[channel_no] = true;
    }
    if (channel_no == 1) {
      ch_flags[channel_no] = true;
    }
    if (channel_no == 2) {
      ch_flags[channel_no] = true;
    }
    if (channel_no == 3) {
      ch_flags[channel_no] = true;
    }
  }
}
