/*******************************************************************************
 * @file pwm_example.c
 * @brief pwm example
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
/**============================================================================
 * @brief : Motor Control PWM (MCPWM) controller is used to generate a periodic 
 * pulse waveform, which is useful in motor control and power control applications.
 * @section Description : 
 * It generates four different duty cycles on different channels using programmable 
 * duty cycle and time period and different interrupts are supported.
 ============================================================================**/

/* Includes ------------------------------------------------------------------*/
#include "pwm_example.h"
#include "rsi_debug.h"
#include "sl_si91x_pwm.h"
#include "sl_pwm_instances.h"
#include "sl_si91x_peripheral_gpio.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define EVENT_COUNT       10    // Count of events that can generate
#define PRESCALE_A        0x100 // PWM Prescale_A value
#define DEADTIME_A        32    // PWM deadtime_A
#define DT_COUNTER_A      0x00  // Dead time counter A enable
#define DUTY_CYCLE_UPDATE 0x01  // Enable duty cycle updating bit in register
#define OUTPUT_VALUE      1     // Output value set
#define SL_ULP_PORT       4     // GPIO ULP port

#define FAULT_A_ENABLE 0x11                          // Fault A enable for channel 0
#define INTR_EVENT     SL_RISE_TIME_PERIOD_MATCH_CH0 // Rise PWM time period match channel 0 event
/*******************************************************************************
 **********************  GLOBAL variables   ************************************
 ******************************************************************************/
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pwm_callback_function(uint16_t event);

/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
/* Volatile: written in PWM callback (ISR), read in main loop. Required for LTO. */
static uint8_t flag[10];
static volatile boolean_t event_flag = 1;
/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * PWM Example Initialization function
 ******************************************************************************/
void pwm_example_init(void)
{
  sl_status_t status;
  sl_pwm_version_t version;

  // Get PWM version
  version = sl_si91x_pwm_get_version();
  /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
  SL_PRINT_STRING_ERROR("PWM version is fetched successfully \r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", version.release, version.major, version.minor);
  do {
    // set the PWM configuration parameters
    status = sl_si91x_pwm_set_configuration(&sl_pwm_channel_0_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_set_configuration, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set configuration is successful \r\n");
    // set base time period control
    status = sl_si91x_pwm_control_period(SL_TIME_PERIOD_POSTSCALE_1_1,
                                         SL_TIME_PERIOD_PRESCALE_1,
                                         sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_period, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set base time period control is successful \r\n");
    // set duty cycle control parameters
    status =
      sl_si91x_pwm_configure_duty_cycle(SL_DUTY_CYCLE_ENABLE, DUTY_CYCLE_UPDATE, sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_duty_cycle, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set duty cycle control parameters is successful \r\n");
    sl_si91x_pwm_callback_t pwm_callback = { .cbFunc = pwm_callback_function };
    // Enables PWM interrupt flags
    status = sl_si91x_pwm_register_callback(&pwm_callback, INTR_EVENT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_enable_interrupt, Error code: %lu\r\n", status);
      break;
    }
#ifdef DEAD_TIME
    sl_si91x_pwm_dt_config_t dead_time;
    dead_time.counterSelect = 0;
    dead_time.preScaleA     = SL_TIME_PERIOD_PRESCALE_8;
    dead_time.preScaleB     = 0;
    dead_time.deadTimeA     = DEADTIME_A;
    dead_time.deadTimeB     = 0;

    // Set dead time control parameters for the required channel
    status = sl_si91x_pwm_select_dead_time(SL_DEAD_TIME_ENABLE, DT_COUNTER_A);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_select_dead_time, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM dead time control set is successful \r\n");

    // Configure Dead time insertion parameters
    status = sl_si91x_pwm_configure_dead_time(&dead_time, sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_configure_dead_time, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Configure Dead time insertion is successful \r\n");
#endif
#ifdef OVERRIDE
    // Set the override control parameter
    status = sl_si91x_pwm_control_override(SL_OVERRIDE_SET, SL_OP_OVERRIDE_SYNC);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set the override control parameter is successful \r\n");
    // Set override value for the required output of MCPWM
    status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override_value, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set override value is successful \r\n");

    status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_HIGH0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override_value, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set override value is successful \r\n");
    // Enable the output override operation of MCPWM
    status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, SL_OUTPUT_LOW0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_output_override, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Enable the output override is successful \r\n");

    status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, SL_OUTPUT_HIGH0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_output_override, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Enable the output override is successful \r\n");
#endif
#ifdef FAULT
    status = sl_si91x_pwm_fault_init(&sl_pwm_channel_0_event_init);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_fault_init, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM fault initialization is successful \r\n");
    // Set output fault override control parameters for required PWM channel
    status = sl_si91x_pwm_control_output_fault(SL_OUTPUT_FAULT_SET, FAULT_A_ENABLE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_output_fault, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set output fault override is successful \r\n");

    // Control fault A/B pin output value
    status = sl_si91x_pwm_control_fault(SL_FAULTA, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_fault, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Control fault A/B is successful \r\n");

    status = sl_si91x_pwm_control_fault(SL_FAULTA, SL_OUTPUT_HIGH0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_fault, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Control fault A/B is successful \r\n");
#endif
#ifdef SVT
    /* Known GPIO level before peripheral mux for SVT pads. Not gated on PWM_OUTPUT_LOW (unlike
     * sl_si91x_pwm_init); only this optional SVT pin setup path in the example. */
    sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(SL_SI91X_PWM_SLP_EVENT_TRIG_PIN - GPIO_MAX_PIN));
    sl_gpio_pin_configure_gpio_output_level(SL_ULP_PORT,
                                            (uint8_t)(SL_SI91X_PWM_SLP_EVENT_TRIG_PIN - GPIO_MAX_PIN),
                                            OUTPUT_VALUE);
    sl_gpio_set_pin_mode(SL_ULP_PORT,
                         (uint8_t)(SL_SI91X_PWM_SLP_EVENT_TRIG_PIN - GPIO_MAX_PIN),
                         ULP_GPIO_MODE_6,
                         OUTPUT_VALUE);
    sl_si91x_gpio_enable_pad_selection(SL_SI91X_PWM_SLP_EVENT_TRIG_PAD);
    sl_gpio_pin_configure_gpio_output_level(SL_SI91X_PWM_SLP_EVENT_TRIG_PORT,
                                            SL_SI91X_PWM_SLP_EVENT_TRIG_PIN,
                                            OUTPUT_VALUE);
    sl_gpio_set_pin_mode(SL_SI91X_PWM_SLP_EVENT_TRIG_PORT,
                         SL_SI91X_PWM_SLP_EVENT_TRIG_PIN,
                         SL_SI91X_PWM_SLP_EVENT_TRIG_MUX,
                         OUTPUT_VALUE);
    sl_si91x_pwm_svt_config_t svt_config = { SL_TIME_PERIOD_POSTSCALE_1_7,
                                             PRESCALE_A,
                                             sl_pwm_channel_0_config.channel };
    // enable special event trigger
    status = sl_si91x_pwm_control_special_event_trigger(SL_EVENT_ENABLE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_special_event_trigger, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM enable SVT is successful \r\n");
    // special trigger configuration
    status = sl_si91x_pwm_trigger_special_event(COUNTDOWN, &svt_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_trigger_special_event, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM special trigger event is successful \r\n");
#endif
    // Starts timer for channel
    status = sl_si91x_pwm_start(sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_start, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Start timer is successful \r\n");
  } while (false);
}

/*******************************************************************************
 * PWM example process action function
 ******************************************************************************/
void pwm_example_process_action(void)
{
  if (event_flag) {
    SL_PRINT_STRING_ERROR("PWM enable interrupt flag is successful \r\n");
    for (uint8_t i = 0; i < EVENT_COUNT; i++) {
      if (flag[i] == 1) {
        SL_PRINT_STRING_ERROR("event %d raised\r\n", i);
      }
    }
    event_flag = FALSE;
  }
}

/*******************************************************************************
 * @brief    PWM callback to indicate any event
 * @param[in]    event : PWM interrupt events
 * @return       none
 ******************************************************************************/
static void pwm_callback_function(uint16_t event)
{
  switch (event) {
    case RISE_TIME_PERIOD_MATCH_CH0: // Event for 0th channel without considering post scaler
      event_flag = 1;
      flag[0]    = 1;
      break;
    case TIME_PERIOD_MATCH_CH0: // Event for 0th channel which considering post scaler
      event_flag = 1;
      flag[1]    = 1;
      break;
    case FAULT_A: // Fault A pin Event
      event_flag = 1;
      flag[2]    = 1;
      break;
    case FAULT_B: // Fault B pin Event
      event_flag = 1;
      flag[3]    = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH1: // Event for 1st channel without considering post scaler
      event_flag = 1;
      flag[4]    = 1;
      break;
    case TIME_PERIOD_MATCH_CH1: // Event for 1st channel which considering post scaler
      event_flag = 1;
      flag[5]    = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH2: // Event for 2nd channel without considering post scaler
      event_flag = 1;
      flag[6]    = 1;
      break;
    case TIME_PERIOD_MATCH_CH2: // Event for 2nd channel which considering post scaler
      event_flag = 1;
      flag[7]    = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH3: // Event for 3rd channel without considering post scaler
      event_flag = 1;
      flag[8]    = 1;
      break;
    case TIME_PERIOD_MATCH_CH3: // Event for 3rd channel which considering post scaler
      event_flag = 1;
      flag[9]    = 1;
      break;
  }
}
