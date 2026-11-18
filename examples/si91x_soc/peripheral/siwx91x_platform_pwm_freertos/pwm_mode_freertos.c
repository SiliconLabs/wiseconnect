/*******************************************************************************
 * @file pwm_mode_freertos.c
 * @brief PWM FreeRTOS example
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
#include "pwm_mode_freertos.h"
#include "cmsis_os2.h"
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
#define EVENT_WAIT_MS  1000                          // Timeout for waiting on PWM events
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void pwm_event_callback_handler(uint16_t event);
static void pwm_mode_task(void *argument);
static sl_status_t pwm_init_function(void);

static const osThreadAttr_t pwm_mode_thread_attributes = {
  .name       = "pwm_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};
/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static uint8_t flag[EVENT_COUNT];
static osSemaphoreId_t pwm_event_sem;
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the PWM FreeRTOS task.
 * @return None
 ******************************************************************************/
void pwm_mode_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)pwm_mode_task, NULL, &pwm_mode_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create PWM thread\r\n");
    return;
  }
}

/*******************************************************************************
 * @brief  One-time PWM hardware init: semaphore, version, config, interrupts,
 *         dead time, override, fault, SVT, and start.
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t pwm_init_function(void)
{
  sl_status_t status;
  sl_pwm_version_t version;

  pwm_event_sem = osSemaphoreNew(1U, 0U, NULL);
  if (pwm_event_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create PWM semaphore\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  version = sl_si91x_pwm_get_version();
  SL_PRINT_STRING_ERROR("PWM version is fetched successfully \n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\n", version.release, version.major, version.minor);

  do {
    status = sl_si91x_pwm_set_configuration(&sl_pwm_channel_0_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_set_configuration, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set configuration is successful \n");

    status = sl_si91x_pwm_control_period(SL_TIME_PERIOD_POSTSCALE_1_1,
                                         SL_TIME_PERIOD_PRESCALE_1,
                                         sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_period, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set base time period control is successful \n");

    status =
      sl_si91x_pwm_configure_duty_cycle(SL_DUTY_CYCLE_ENABLE, DUTY_CYCLE_UPDATE, sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_duty_cycle, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM set duty cycle control parameters is successful \n");

    sl_si91x_pwm_callback_t pwm_callback = { .cbFunc = pwm_event_callback_handler };
    status                               = sl_si91x_pwm_register_callback(&pwm_callback, INTR_EVENT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_enable_interrupt, Error code: %lu", status);
      break;
    }
#ifdef DEAD_TIME
    sl_si91x_pwm_dt_config_t dead_time;
    dead_time.counterSelect = 0;
    dead_time.preScaleA     = SL_TIME_PERIOD_PRESCALE_8;
    dead_time.preScaleB     = 0;
    dead_time.deadTimeA     = DEADTIME_A;
    dead_time.deadTimeB     = 0;

    status = sl_si91x_pwm_select_dead_time(SL_DEAD_TIME_ENABLE, DT_COUNTER_A);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_select_dead_time, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM dead time control set is successful \n");

    status = sl_si91x_pwm_configure_dead_time(&dead_time, sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_configure_dead_time, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Configure Dead time insertion is successful \n");
#endif
#ifdef OVERRIDE
    status = sl_si91x_pwm_control_override(SL_OVERRIDE_SET, SL_OP_OVERRIDE_SYNC);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set the override control parameter is successful \n");

    status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override_value, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set override value is successful \n");

    status = sl_si91x_pwm_control_override_value(SL_OVERRIDE_SET, SL_OUTPUT_HIGH0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_override_value, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set override value is successful \n");

    status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, SL_OUTPUT_LOW0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_output_override, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Enable the output override is successful \n");

    status = sl_si91x_pwm_output_override(SL_OVERRIDE_SET, SL_OUTPUT_HIGH0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_output_override, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Enable the output override is successful \n");
#endif
#ifdef FAULT
    status = sl_si91x_pwm_fault_init(&sl_pwm_channel_0_event_init);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_fault_init, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM fault initialization is successful \n");

    status = sl_si91x_pwm_control_output_fault(SL_OUTPUT_FAULT_SET, FAULT_A_ENABLE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_output_fault, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Set output fault override is successful \n");

    status = sl_si91x_pwm_control_fault(SL_FAULTA, SL_OUTPUT_LOW0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_fault, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Control fault A/B is successful \n");

    status = sl_si91x_pwm_control_fault(SL_FAULTA, SL_OUTPUT_HIGH0, SL_OVERRIDE_VALUE1);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_fault, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Control fault A/B is successful \n");
#endif
#ifdef SVT
    sl_si91x_gpio_enable_ulp_pad_receiver((uint8_t)(SL_SI91X_PWM_SLP_EVENT_TRIG_PIN - GPIO_MAX_PIN));
    sl_gpio_set_pin_mode(SL_ULP_PORT,
                         (uint8_t)(SL_SI91X_PWM_SLP_EVENT_TRIG_PIN - GPIO_MAX_PIN),
                         ULP_GPIO_MODE_6,
                         OUTPUT_VALUE);
    sl_si91x_gpio_enable_pad_selection(SL_SI91X_PWM_SLP_EVENT_TRIG_PAD);
    sl_gpio_set_pin_mode(SL_SI91X_PWM_SLP_EVENT_TRIG_PORT,
                         SL_SI91X_PWM_SLP_EVENT_TRIG_PIN,
                         SL_SI91X_PWM_SLP_EVENT_TRIG_MUX,
                         OUTPUT_VALUE);
    sl_si91x_pwm_svt_config_t svt_config = { SL_TIME_PERIOD_POSTSCALE_1_7,
                                             PRESCALE_A,
                                             sl_pwm_channel_0_config.channel };
    status                               = sl_si91x_pwm_control_special_event_trigger(SL_EVENT_ENABLE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_control_special_event_trigger, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM enable SVT is successful \n");

    status = sl_si91x_pwm_trigger_special_event(COUNTDOWN, &svt_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_trigger_special_event, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM special trigger event is successful \n");
#endif
    status = sl_si91x_pwm_start(sl_pwm_channel_0_config.channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_pwm_start, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("PWM Start timer is successful \n");
  } while (false);

  return status;
}

/*******************************************************************************
 * @brief  PWM FreeRTOS task. Initialises PWM hardware, then loops waiting on
 *         semaphore for PWM events raised by ISR callback.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void pwm_mode_task(void *argument)
{
  (void)argument;

  sl_status_t status = pwm_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PWM init failed, exiting task\r\n");
    osThreadExit();
  }

  while (1) {
    osStatus_t sem_status = osSemaphoreAcquire(pwm_event_sem, EVENT_WAIT_MS);
    if (sem_status == osOK) {
      SL_PRINT_STRING_ERROR("PWM enable interrupt flag is successful \n");
      for (uint8_t i = 0; i < EVENT_COUNT; i++) {
        if (flag[i] == 1) {
          SL_PRINT_STRING_ERROR("event %d raised\n", i);
          flag[i] = 0;
        }
      }
    }
  }
}

/*******************************************************************************
 * @brief    PWM callback to indicate any event
 * @param[in]    event : PWM interrupt events
 * @return       none
 ******************************************************************************/
static void pwm_event_callback_handler(uint16_t event)
{
  switch (event) {
    case RISE_TIME_PERIOD_MATCH_CH0:
      flag[0] = 1;
      break;
    case TIME_PERIOD_MATCH_CH0:
      flag[1] = 1;
      break;
    case FAULT_A:
      flag[2] = 1;
      break;
    case FAULT_B:
      flag[3] = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH1:
      flag[4] = 1;
      break;
    case TIME_PERIOD_MATCH_CH1:
      flag[5] = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH2:
      flag[6] = 1;
      break;
    case TIME_PERIOD_MATCH_CH2:
      flag[7] = 1;
      break;
    case RISE_TIME_PERIOD_MATCH_CH3:
      flag[8] = 1;
      break;
    case TIME_PERIOD_MATCH_CH3:
      flag[9] = 1;
      break;
  }
  if (pwm_event_sem != NULL) {
    (void)osSemaphoreRelease(pwm_event_sem);
  }
}
