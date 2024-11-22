/*******************************************************************************
 * @file config-timer_example.c
 * @brief Config-Timer example
 ****************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 ****************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 *****************************************************************************/

/*==============================================================================
 * @brief : This files contains example describing usage of Configurable Timer
            APIs.
 * @section Description :
 * Counter mode Use-case :
 * One as free running timer with GPIO toggle functionality. The timer instance
 * is configured to generate interrupts upon expiration toggles the GPIO.
 * The counter-0 is set to expire at 1-millisecond (1KHz) intervals.
 * PWM output mode Use-case :
 * Here it will generate 2 PWM outputs, Counter-1 will generate square wave
 * output (50%-duty cycle) and counter-0 will generates PWM output whose duty
 * cycle continuously varies from 100% to 0% then 0% to 100%, in steps of 1%.
 =============================================================================*/

/* Includes ------------------------------------------------------------------*/
#include "config_timer_example.h"
#include "rsi_debug.h"
#include "rsi_rom_egpio.h"
#include "sl_si91x_config_timer.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define CT_COUNTER_USED       SL_COUNTER_0 // counter number used for normal counter application
#define CT_RATE               1000         // value for dividing system core clock
#define TICKS                 1000         // tick value
#define TENTH_INTERRUPT_COUNT 10           // Count for tenth timeout interrupt
#define INITIAL_VALUE         0            // for zero value
#define PIN_1                 1            // for ulp-gpio-1
#define PORT_0                0            // for gpio port 0
#define HUNDRED               100          // for value 100
#define ONE                   1            // for value 1
#define TWO                   2            // for value 2
#define TWENTY                20           // for value 20
#define SET                   1            // for setting any value high
#define CLEAR                 0            // for clearing any value
#define DELAY_COUNT           10           // delay count value
#define TIME_PERIOD_VALUE     1000         // Time period in microseconds
/*******************************************************************************
 **********************  GLOBAL variables   ***************************
 ******************************************************************************/
sl_config_timer_pwm_callback_t pwm_callback;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
#if (CT_PWM_MODE_USECASE == SET)
static uint32_t CT_PercentageToTicks(uint8_t percent, uint32_t freq);
#endif
static void on_config_timer_callback(void *callback_flag);
/*******************************************************************************
 **********************  Local variables   ***************************
 ******************************************************************************/
static sl_status_t status;
static void *callback_flag_data;
#if (CT_PWM_MODE_USECASE == SET)
static uint32_t delay = INITIAL_VALUE;
static int pwm_out_0, duty_p = INITIAL_VALUE, incr = ONE;
static sl_config_timer_ocu_control_t ocu_params0;
static sl_config_timer_ocu_control_t ocu_params1;
static sl_config_timer_ocu_params_t vsOCUparams = { INITIAL_VALUE };
#endif
static sl_config_timer_interrupt_flags_t ct_interrupt_flags;
#if (CT_COUNTER_MODE_USECASE == SET)
sl_counter_number_t counter_used       = CT_COUNTER_USED;
static uint32_t interrupt_count        = INITIAL_VALUE;
static uint32_t counter0_initial_value = 0;
static uint32_t counter1_initial_value = 0;
volatile boolean_t interrupt_flag      = 0;
#endif
/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * Config-Timer example initialization function
 ******************************************************************************/
void config_timer_example_init(void)
{
  sl_config_timer_version_t version;
  sl_config_timer_config_t ct_config;
  static uint32_t match_value;

  // Initializing ct configuration structure
  ct_config.is_counter_mode_32bit_enabled    = SL_COUNTER_16BIT;
  ct_config.counter0_direction               = SL_COUNTER0_UP;
  ct_config.is_counter0_periodic_enabled     = false;
  ct_config.is_counter0_sync_trigger_enabled = true;
  ct_config.counter1_direction               = SL_COUNTER0_UP;
  ct_config.is_counter1_periodic_enabled     = true;
  ct_config.is_counter1_sync_trigger_enabled = true;

  //Version information of config timer
  version = sl_si91x_config_timer_get_version();
  DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
#if (CT_PWM_MODE_USECASE == SET)
  sl_config_timer_ocu_config_t ct_ocu_config;
  // Initializing ct OCU configuration structure
  ct_ocu_config.is_counter0_ocu_output_enabled         = true;
  ct_ocu_config.is_counter1_ocu_output_enabled         = true;
  ct_ocu_config.is_counter0_ocu_sync_enabled           = true;
  ct_ocu_config.is_counter1_ocu_sync_enabled           = true;
  ct_ocu_config.is_counter0_toggle_output_high_enabled = true;
  ct_ocu_config.is_counter1_toggle_output_high_enabled = true;
  ct_ocu_config.is_counter0_toggle_output_low_enabled  = true;
  ct_ocu_config.is_counter1_toggle_output_low_enabled  = true;
  do {
    /*Initialize pins and clock */
    sl_si91x_config_timer_init();
    DEBUGOUT("CT initialized successfully \n");
    // Configuring CT parameters
    status = sl_si91x_config_timer_set_configuration(&ct_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_configuration, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT configuration is set successfully \n");
    // Get the match value of the timer
    status = sl_si91x_config_timer_get_match_value(CT_RATE, &match_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_get_match_value, Error code: %lu", status);
      break;
    }
    // Setting match value for counter 0
    status = sl_si91x_config_timer_set_match_count(SL_COUNTER_16BIT, SL_COUNTER_0, match_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_match_count, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("Counter0 Match Count is set successfully \n");
    // Setting match value for counter 1
    status = sl_si91x_config_timer_set_match_count(SL_COUNTER_16BIT, SL_COUNTER_1, match_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_match_count, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("Counter1 Match Count is set successfully \n");
    // Setting OCU threshold values
    vsOCUparams.CompareVal1_0 = INITIAL_VALUE;
    vsOCUparams.CompareVal1_1 = (uint16_t)(match_value / TWO);
    // Setting OCU configurations
    status = sl_si91x_config_timer_set_ocu_configuration(&ct_ocu_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_ocu_configuration, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT OCU configuration is set successfully \n");
    ocu_params0.is_counter_number_1  = SL_COUNTER_0;
    ocu_params0.is_dma_state_enabled = false;
    ocu_params0.params               = &vsOCUparams;
    ocu_params0.callback             = &pwm_callback;
    status                           = sl_si91x_config_timer_set_ocu_control(&ocu_params0);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_ocu_compare_values, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT OCU configuration is set successfully \n");
    ocu_params1.is_counter_number_1  = SL_COUNTER_1;
    ocu_params1.is_dma_state_enabled = false;
    ocu_params1.params               = &vsOCUparams;
    ocu_params1.callback             = &pwm_callback;
    status                           = sl_si91x_config_timer_set_ocu_control(&ocu_params1);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_ocu_compare_values, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT OCU configuration is set successfully \n");
    // Enabling interrupt on match value for counter-0
    ct_interrupt_flags.is_counter1_hit_peak_interrupt_enabled = true;
    // Registering callback
    status = sl_si91x_config_timer_register_callback(on_config_timer_callback, callback_flag_data, &ct_interrupt_flags);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_register_callback, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT callback registered successfully \n");
    // Starting CT counter0 on software trigger
    status = sl_si91x_config_timer_start_on_software_trigger(SL_COUNTER_0);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_start_on_software_trigger, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT started successfully on software trigger \n");
    // Starting CT counter1 on software trigger
    status = sl_si91x_config_timer_start_on_software_trigger(SL_COUNTER_1);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_start_on_software_trigger, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT started successfully on software trigger \n");
  } while (false);
#endif
#if (CT_COUNTER_MODE_USECASE == SET)
  // Setting ULP-GPIO-1 mode
  RSI_EGPIO_SetPinMux(EGPIO1, PORT_0, PIN_1, EGPIO_PIN_MUX_MODE0);
  // Setting ULP-GPIO-1 output direction
  RSI_EGPIO_SetDir(EGPIO1, PORT_0, PIN_1, EGPIO_CONFIG_DIR_OUTPUT);
  do {
    // Initializing CT
    sl_si91x_config_timer_init();
    DEBUGOUT("CT initialized successfully \n");
    // Configuring CT parameters from UC values
    status = sl_si91x_config_timer_set_configuration(&ct_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_configuration, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT configuration is set successfully \n");
    // Get the match value of the timer
    status = sl_si91x_config_timer_get_match_value(TIME_PERIOD_VALUE, &match_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_get_match_value, Error code: %lu", status);
      break;
    }
    DEBUGOUT("CT match value get is successful \n");
    // Setting Initial count value
    status = sl_si91x_config_timer_set_initial_count(SL_COUNTER_16BIT, counter0_initial_value, counter1_initial_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_initial_count, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT Initial Count is set successfully \n");
    // Setting match value
    status = sl_si91x_config_timer_set_match_count(SL_COUNTER_16BIT, counter_used, match_value);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_set_match_count, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT Match Count is set successfully\n");
    // Enabling interrupt at match value for counter-used
    if (counter_used == SL_COUNTER_0) {
      ct_interrupt_flags.is_counter0_hit_peak_interrupt_enabled = true;
    } else {
      ct_interrupt_flags.is_counter1_hit_peak_interrupt_enabled = true;
    }
    // Un-registering callback
    status = sl_si91x_config_timer_unregister_callback(&ct_interrupt_flags);
    // Registering callback
    status = sl_si91x_config_timer_register_callback(on_config_timer_callback, callback_flag_data, &ct_interrupt_flags);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_register_callback, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT callback registered successfully \n");
    // Starting CT counter0 on software trigger
    status = sl_si91x_config_timer_start_on_software_trigger(counter_used);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_config_timer_start_on_software_trigger, Error code: %lu\n", status);
      break;
    }
    DEBUGOUT("CT started successfully on software trigger \n");
  } while (false);
#endif
}

void config_timer_example_process_action(void)
{
#if (CT_COUNTER_MODE_USECASE == SET)
  // interrupt_flag is set when interrupt count is greater than TENTH_INTERRUPT_COUNT
  if (interrupt_flag) {
    interrupt_flag = 0;
    DEBUGOUT("Config timer unregistered & de-inits callback after 10th interrupt\n");
  }
#endif
#if (CT_PWM_MODE_USECASE == SET)
  do {
    if (delay >= DELAY_COUNT) {
      // Changing duty-cycle value
      duty_p = duty_p + incr;
      if (duty_p <= 0) {
        duty_p = INITIAL_VALUE;
        incr   = ONE;
      }
      if (duty_p >= HUNDRED) {
        duty_p = HUNDRED;
        incr   = -1;
      }
      pwm_out_0 = duty_p;
      // Converts duty cycle percentage to system ticks and updating compare value
      vsOCUparams.CompareVal1_0 = CT_PercentageToTicks(pwm_out_0, CT_RATE);
      if (vsOCUparams.CompareVal1_0 == CLEAR) {
        break;
      }
      // Setting OCU control parameters
      status = sl_si91x_config_timer_set_ocu_control(&ocu_params0);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_config_timer_set_ocu_compare_values, Error code: %lu\n", status);
        break;
      }
      status = sl_si91x_config_timer_set_ocu_control(&ocu_params1);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_config_timer_set_ocu_compare_values, Error code: %lu\n", status);
        break;
      }
      delay = INITIAL_VALUE;
    }
  } while (false);
#endif
}
#if (CT_PWM_MODE_USECASE == SET)
/* Converts duty cycle percentage to system ticks */
static uint32_t CT_PercentageToTicks(uint8_t percent, uint32_t freq)
{
  uint32_t ticks = INITIAL_VALUE;
  uint32_t rate  = INITIAL_VALUE;
  if (freq != 0) {
    sl_si91x_config_timer_get_match_value(freq, &rate);
    ticks = (rate * percent) / HUNDRED;
    DEBUGOUT("ticks:%ld, rate:%ld, percent:%d\n", ticks, rate, percent);
    return ticks;
  } else {
    return 0;
  }
}
#endif
// Remove all debugs from callback function for proper delays
void on_config_timer_callback(void *callback_flag)
{
#if (CT_PWM_MODE_USECASE == SET)
  if (*(uint32_t *)callback_flag == SL_CT_COUNTER_1_IS_PEAK_FLAG) {
    // incrementing delay variable on every interrupt
    delay++;
  }
#endif
#if (CT_COUNTER_MODE_USECASE == SET)
  uint32_t interrupt_flag_value = 0;
  // Updating expected interrupt flag value as per enabled interrupt and counter used
  if (counter_used == SL_COUNTER_0) {
    interrupt_flag_value = SL_CT_COUNTER_0_IS_PEAK_FLAG;
  } else {
    interrupt_flag_value = SL_CT_COUNTER_1_IS_PEAK_FLAG;
  }
  // Checking interrupt flag value
  if (*(uint32_t *)callback_flag == interrupt_flag_value) {
    // Incrementing interrupt count
    interrupt_count++;
    // Toggles ULP_GPIO_1 on every interrupt
    RSI_EGPIO_SetPin(EGPIO1, PORT_0, PIN_1, SET);
    RSI_EGPIO_SetPin(EGPIO1, PORT_0, PIN_1, CLEAR);
  }
  // Checking interrupt count
  if (interrupt_count >= TENTH_INTERRUPT_COUNT) {
    interrupt_flag = 1;
    // De-initializing config-timer
    sl_si91x_config_timer_deinit();
  }
#endif
}
