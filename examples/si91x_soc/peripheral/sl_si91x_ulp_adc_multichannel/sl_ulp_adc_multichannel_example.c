/***************************************************************************/
/**
 * @file sl_ulp_adc_multichannel_example.c
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
#include "sl_ulp_adc_multichannel_example.h"

#include "rsi_debug.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_adc_common_config.h"
#include "sl_si91x_power_manager.h"
#include "sl_si91x_wireless_shutdown.h"

/*******************************************************************************
***************************  Defines / Macros  ********************************
******************************************************************************/
/* Core clock modification Macros */
#define CHANNEL_SAMPLE_LENGTH 1023       // Number of ADC sample collect for operation
#define ADC_MAX_OP_VALUE      4095       // Maximum output value get from adc data register
#define ADC_DATA_CLEAR        0xF7FF     // Clear the data if 12th bit is enabled
#define VREF_VALUE            3.3        // reference voltage
#define ADC_PING_BUFFER_1     0x24060000 // Start address of Ping for channel_0
#define ADC_PING_BUFFER_2     0x24060800 // Start address of Ping for channel_1
#define ADC_PING_BUFFER_3     0x24061000 // Start address of Ping for channel_2
#define ADC_PING_BUFFER_4     0x24061800 // Start address of Ping for channel_3

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
// Enum for different transmission scenarios
typedef enum {
  ULP_ADC_MULTI_CHANNEL_PROCESS_ACTION,         // adc multi channel event triggering
                                                // and operational transforming
  ULP_ADC_MULTI_CHANNEL_POWER_STATE_TRANSITION, // adc multi channel power state
                                                // transition mode
  ULP_ADC_MULTI_CHANNEL_TRANSMISSION_COMPLETED, // adc multi channel
                                                // transmission completed mode
} ulp_adc_multi_channel_enum_t;

/*******************************************************************************
*************************** LOCAL VARIABLES   *******************************
******************************************************************************/
static float vref_value = (float)VREF_VALUE;
static int16_t adc_output[CHANNEL_SAMPLE_LENGTH];
static boolean_t ch_flags[NUMBER_OF_CHANNEL];
sl_status_t status;

/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/
static void callback_event(uint8_t event_channel, uint8_t event);
static ulp_adc_multi_channel_enum_t ulp_adc_multi_channel_current_mode = ULP_ADC_MULTI_CHANNEL_PROCESS_ACTION;
static sl_power_state_t current_power_state                            = SL_SI91X_POWER_MANAGER_PS4;
static void configuring_ps2_power_state(void);
/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
// Function to check if all channels have completed their operation
static bool all_channels_completed(void)
{
  for (uint8_t i = 0; i < NUMBER_OF_CHANNEL; i++) {
    if (!ch_flags[i]) {
      return false; // Return false if any channel flag is not set
    }
  }
  return true; // Return true if all channel flags are set
}
/*******************************************************************************
 * ADC example initialization function
 ******************************************************************************/
void adc_multichannel_example_init(void)
{
  sl_adc_version_t version;

  sl_adc_channel_config.rx_buf[0] = adc_output; /* In order for us to read the
                     data from adc_output in the sample application, ADC will
                     save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[0] =
    ADC_PING_BUFFER_1; /* Starting address of ADC Ping buffer for channel 0 */
  sl_adc_channel_config.chnl_pong_address[0] =
    ADC_PING_BUFFER_1 + (sl_adc_channel_config.num_of_samples[0]); /* Starting
                                    address of ADC Pong buffer for channel 0 */

  sl_adc_channel_config.rx_buf[1] = adc_output; /* In order for us to read the
                     data from adc_output in the sample application, ADC will
                     save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[1] =
    ADC_PING_BUFFER_2; /* Starting address of ADC Ping buffer for channel 1 */
  sl_adc_channel_config.chnl_pong_address[1] =
    (ADC_PING_BUFFER_2 + (sl_adc_channel_config.num_of_samples[1])); /* Starting
 address of ADC Pong buffer for channel 1 */

  sl_adc_channel_config.rx_buf[2] = adc_output; /* In order for us to read the
                     data from adc_output in the sample application, ADC will
                     save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[2] =
    ADC_PING_BUFFER_3; /* Starting address of ADC Ping buffer for channel 2 */
  sl_adc_channel_config.chnl_pong_address[2] =
    (ADC_PING_BUFFER_3 + (sl_adc_channel_config.num_of_samples[2])); /* Starting
 address of ADC Pong buffer for channel 2 */

  sl_adc_channel_config.rx_buf[3] = adc_output; /* In order for us to read the
                     data from adc_output in the sample application, ADC will
                     save the data in the rx buffer. */
  sl_adc_channel_config.chnl_ping_address[3] =
    ADC_PING_BUFFER_4; /* Starting address of ADC Ping buffer for channel 3 */
  sl_adc_channel_config.chnl_pong_address[3] =
    (ADC_PING_BUFFER_4 + (sl_adc_channel_config.num_of_samples[3])); /* Starting
 address of ADC Pong buffer for channel 3 */
  do {
    // Version information of ADC driver
    version = sl_si91x_adc_get_version();
    DEBUGOUT("ADC version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    // Initializing the ADC Multi channel
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
    // channel configuration the ADC Multi channel
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
    // starting the ADC Multi channel
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
  /*************************************************************************************************
   * This section manages power state transitions within the system, optimizing
   *power consumption while maintaining essential functionality. It transitions
   *the system from a higher power state (PS4) to a lower one (PS2) during
   *specific operations to conserve power. This involves adjusting clock
   * references and shutting down unnecessary power supplies. After completing
   *the operation, the code transitions back to the higher power state (PS4) to
   *ensure adequate resources for subsequent tasks. This approach balances power
   *efficiency with operational requirements across various system functions.
   ***************************************************************************************************/
  uint32_t sample_length;
  uint8_t chnl_num = 0;
  float vout       = 0.0f;
  switch (ulp_adc_multi_channel_current_mode) {
    case ULP_ADC_MULTI_CHANNEL_PROCESS_ACTION:
      // here we get the 12-bit value of ADC output in equivalent voltage.
      for (chnl_num = 0; chnl_num < sl_adc_config.num_of_channel_enable; chnl_num++) {
        if (ch_flags[chnl_num] == true) {
          ch_flags[chnl_num] = false;
          status             = sl_si91x_adc_read_data(sl_adc_channel_config, chnl_num);
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_adc_read_data: Error Code : %lu \n", status);
          }
          for (sample_length = 0; sample_length < sl_adc_channel_config.num_of_samples[chnl_num]; sample_length++) {
            /* In two’s complement format, the MSb (11th bit) of the conversion
       result determines the polarity, when the MSb = ‘0’, the result is
       positive, and when the MSb = ‘1’, the result is negative*/
            if (adc_output[sample_length] & SIGN_BIT) {
              // Full-scale would be represented by a hexadecimal value,
              // full-scale range of ADC result values in two’s complement.
              adc_output[sample_length] = (int16_t)(adc_output[sample_length] & (ADC_DATA_CLEAR));
            } else { // set the MSb bit.
              adc_output[sample_length] = adc_output[sample_length] | SIGN_BIT;
            }

            vout = (((float)adc_output[sample_length] / (float)ADC_MAX_OP_VALUE) * vref_value);
            // For differential type it will give vout.
            if (sl_adc_channel_config.input_type[chnl_num]) {
              vout = vout - (vref_value / 2);
            }
            DEBUGOUT("ADC channel_%d[%ld] :%0.2fV \n", chnl_num, sample_length, (float)vout);
          }
        }
        if (all_channels_completed()) {
          // current mode being updated with power state transition to change the
          // power state mode
          ulp_adc_multi_channel_current_mode = ULP_ADC_MULTI_CHANNEL_POWER_STATE_TRANSITION;
        }
      }
      break;
    case ULP_ADC_MULTI_CHANNEL_POWER_STATE_TRANSITION:
      if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
        DEBUGOUT("Switching adc from PS4 -> PS2 state \n");
        // Control power management by adjusting clock references and shutting down
        // the power supply
        sl_si91x_wireless_shutdown();
        // switching the power state PS4 to PS2 mode.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \n", status);
          break;
        }
        /* Due to calling trim_efuse API om power manager it will change the clock
    frequency, if we are not initialize the debug again it will print the
    garbage data or no data in console output. */
        DEBUGINIT();
        // Configuring the ps2 power state by configuring
        // the ram retention and removing the unused peripherals
        configuring_ps2_power_state();
        // current power state is updated to PS2
        current_power_state = SL_SI91X_POWER_MANAGER_PS2;
        // current mode is updated with process action for verifying the
        // triggering and processing
        ulp_adc_multi_channel_current_mode = ULP_ADC_MULTI_CHANNEL_PROCESS_ACTION;
      } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
        DEBUGOUT("Switching the adc from PS2 -> PS4 state\n");
        // switching the power state from PS2 to PS4 mode
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \n", status);
          break;
        }
        // current power state is updated to last enum after the power state cycle
        // is completed
        current_power_state = LAST_ENUM_POWER_STATE;
        // current mode is updated with process action for verifying the
        // triggering and processing
        ulp_adc_multi_channel_current_mode = ULP_ADC_MULTI_CHANNEL_PROCESS_ACTION;
      } else {
        // Deinitializing the ADC Multi channel
        status = sl_si91x_adc_deinit(sl_adc_config);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_si91x_adc_deinit: Error Code : %lu \n", status);
        }
        DEBUGOUT("ADC deinit successfully \n");
        ulp_adc_multi_channel_current_mode = ULP_ADC_MULTI_CHANNEL_TRANSMISSION_COMPLETED;
      }
      break;
    case ULP_ADC_MULTI_CHANNEL_TRANSMISSION_COMPLETED:
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Callback event function
 * It is responsible for the event which are triggered by ADC interface
 * @param  event       : INTERNAL_DMA => Single channel data acquisition
 *done. ADC_STATIC_MODE_CALLBACK => Static mode adc data acquisition done.
 ******************************************************************************/
static void callback_event(uint8_t event_channel, uint8_t event)
{
  if (event == SL_INTERNAL_DMA) {
    ch_flags[event_channel] = true; // Set flag for the corresponding channel
  }
}
/*******************************************************************************
 * powering off the peripherals not in use,
 * Configuring power manager ram-retention
 ******************************************************************************/
static void configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;
  // Configure RAM banks for retention during power management
  config.configure_ram_banks = true; // Enable RAM bank configuration
  config.m4ss_ram_banks      = SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_8 | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_9
                          | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_10; // Specify the RAM banks to be
                                                                     // retained during power
                                                                     // management
  config.ulpss_ram_banks = 0;
  peri.m4ss_peripheral   = 0;
  // Ored value for ulpss peripheral.
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC | SL_SI91X_POWER_MANAGER_ULPSS_PG_CAP
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  // Ored value for npss peripheral.
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCURTC | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2 | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_TIMEPERIOD;
  do {
    // Peripherals passed in this API are powered off.
    status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_remove_peripheral_requirement failed, "
               "Error Code: 0x%lX",
               status);
      break;
    }
    // RAM retention modes are configured and passed into this API.
    status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_configure_ram_retention failed, Error "
               "Code: 0x%lX",
               status);
      break;
    }
  } while (false);
}
