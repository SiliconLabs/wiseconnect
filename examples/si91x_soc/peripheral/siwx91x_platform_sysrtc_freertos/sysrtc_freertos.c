/***************************************************************************/ /**
 * @file sysrtc_freertos.c
 * @brief SYSRTC peripheral
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
/* Project configuration and SYSRTC driver */
#include "sysrtc_freertos.h"
#include "sl_si91x_sysrtc.h"
#include "sl_si91x_sysrtc_config.h"
#include "sl_si91x_led.h"
#include "rsi_debug.h"
#include "sl_status.h"
/* FreeRTOS / CMSIS-RTOS2 */
#include "cmsis_os2.h"
#include <stdint.h>

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define SYSRTC_PRS 0 /* 1 = configure SYSRTC PRS_IN / PRS_OUT via GPIO pins */
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
#define COMPARE_CHANNEL SL_SYSRTC_CHANNEL_0
#endif

#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
#define COMPARE_CHANNEL SL_SYSRTC_CHANNEL_1
#endif

#if (SL_SYSRTC_CLK_SRC == CLK_RC_32KHZ)
#define SYSRTC_COMPARE_VALUE 32000 /* Compare tick for ~1 s @ 32 kHz RC */
#elif (SL_SYSRTC_CLK_SRC == CLK_32KHZ_XTAL)
#define SYSRTC_COMPARE_VALUE 32768 /* Compare tick for ~1 s @ 32.768 kHz XTAL */
#endif

#define COUNTER_VALUE1         0U /* Counter start (compare/capture paths) */
#define COUNTER_VALUE2         0U /* Counter start (overflow-only path) */
#define TENTH_INTERRUPT        10U
#define SYSRTC_SEM_MAX_COMPARE 16U
#define SYSRTC_SEM_MAX_CAPTURE 4U
#define SYSRTC_SEM_MAX_OTHER   128U

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
#if ((SYSRTC_PRS == 1) && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1))
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config = { { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN },
                                                         GPIO_OUTPUT };
#endif

static void *callback_flag_data;
static sl_sysrtc_interrupt_enables_t interrupt_enabled;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
uint32_t capture_val;
#endif

#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1) \
     || (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1))
static uint32_t counter_value1 = COUNTER_VALUE1;
#endif
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 0) && (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 0) \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0))
static uint32_t counter_value2 = COUNTER_VALUE2;
#endif

static osSemaphoreId_t s_sysrtc_event_sem;
static const osThreadAttr_t sysrtc_thread_attributes = {
  .name       = "sysrtc_freertos",
  .stack_size = 2048U,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void sysrtc_callback(void *callback_flag);
static void sysrtc_sem_release(void);
static void sysrtc_interrupt_enable_function(void);
static sl_status_t sysrtc_init_function(void);
static void sysrtc_freertos_task(void *argument);
static sl_status_t sysrtc_driver_init(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 * @brief  Entry point from app layer: creates the SYSRTC FreeRTOS task .
 * @return None
 ******************************************************************************/
void sysrtc_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)sysrtc_freertos_task, NULL, &sysrtc_thread_attributes);
  if (thread_id == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create SYSRTC thread\r\n");
    return;
  }
}

/*******************************************************************************
 *  Initializes the SYSRTC peripheral and creates the event semaphore.
 ******************************************************************************/
static sl_status_t sysrtc_init_function(void)
{
  uint32_t sem_max;
  DEBUGINIT();

#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1))
  sem_max = SYSRTC_SEM_MAX_COMPARE;
#elif (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  sem_max = SYSRTC_SEM_MAX_CAPTURE;
#else
  sem_max = SYSRTC_SEM_MAX_OTHER;
#endif

  if (s_sysrtc_event_sem == NULL) {
    s_sysrtc_event_sem = osSemaphoreNew(sem_max, 0U, NULL);
    if (s_sysrtc_event_sem == NULL) {
      SL_PRINT_STRING_ERROR("Failed to create SYSRTC event semaphore\r\n");
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  sl_status_t drv_status = sysrtc_driver_init();
  if (drv_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SYSRTC driver initialization failed: Error Code : %lu\r\n", drv_status);
    return drv_status;
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
 *  Applies interrupt-mask flags derived from SYSRTC UC (group 0/1).
 ******************************************************************************/
static void sysrtc_interrupt_enable_function(void)
{
#if (SL_SYSRTC_GROUP == 0)
  interrupt_enabled.group0_overflow_interrupt_is_enabled = false;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group0_capture0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_capture0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group0_compare0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_compare0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
  interrupt_enabled.group0_compare1_interrupt_is_enabled = true;
#else
  interrupt_enabled.group0_compare1_interrupt_is_enabled = false;
#endif

#endif
#if (SL_SYSRTC_GROUP == 1)
  interrupt_enabled.group1_overflow_interrupt_is_enabled = false;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group1_capture0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_capture0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
  interrupt_enabled.group1_compare0_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_compare0_interrupt_is_enabled = false;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
  interrupt_enabled.group1_compare1_interrupt_is_enabled = true;
#else
  interrupt_enabled.group1_compare1_interrupt_is_enabled = false;
#endif
#endif
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 0) && (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 0) \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0))
#if (SL_SYSRTC_GROUP == 0)
  interrupt_enabled.group0_overflow_interrupt_is_enabled = true;
#elif (SL_SYSRTC_GROUP == 1)
  interrupt_enabled.group1_overflow_interrupt_is_enabled = true;
#endif
#endif
}

/*******************************************************************************
 *   SYSRTC clock, groups, compares/capture or overflow-only path — UC-driven.
 ******************************************************************************/
static sl_status_t sysrtc_driver_init(void)
{
  sl_status_t drv_status = SL_STATUS_OK;

  sysrtc_interrupt_enable_function();

  do {
#if (SYSRTC_PRS == 1)
    sl_si91x_sysrtc_enable_input_output_gpio(true);
#endif
    drv_status = sl_si91x_sysrtc_configure_clock(&sl_sysrtc_clk_config_handle);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_configure_clock, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC clock configuration is done successfully\r\n");
    drv_status = sl_si91x_sysrtc_init(&sysrtc_handle);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_init, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC initialization is done successfully\r\n");
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1))
    uint32_t compare_value = SYSRTC_COMPARE_VALUE;
#if (SYSRTC_PRS == 1)
    sl_sysrtc_group_compare_channel_action_config_t SYSRTC_CMOA = SYSRTC_GROUP_CHANNEL_COMPARE_CONFIG_TOGGLE;
#if (SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1)
    sysrtc_group_config_handle.p_compare_channel0_config = &SYSRTC_CMOA;
#endif
#if (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1)
    sysrtc_group_config_handle.p_compare_channel1_config = &SYSRTC_CMOA;
#endif
#endif
    drv_status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP, &sysrtc_group_config_handle);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_configure_group, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC group configuration is done successfully\r\n");
#if (SYSRTC_PRS == 1)
    drv_status = sl_si91x_sysrtc_set_compare_output_prs_gpio(SL_SYSRTC_GROUP, COMPARE_CHANNEL, sysrtc_prs_pin);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_set_compare_output_gpio, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC PRS_OUT GPIO pin configuration is done successfully\r\n");
#endif
    drv_status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_register_callback, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC callback registered successfully\r\n");
    sl_si91x_sysrtc_set_count(counter_value1);
    drv_status = sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP, COMPARE_CHANNEL, compare_value);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_set_compare_value, Error code: %lu\r\n", (unsigned long)drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC group compare value set successfully\r\n");
    sl_si91x_sysrtc_start();
    SL_PRINT_STRING_ERROR("Started SYSRTC successfully\r\n");
#endif
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
#if (SYSRTC_PRS == 0)
    uint32_t current_count         = 0;
    uint32_t compare_value_capture = SYSRTC_COMPARE_VALUE;
#endif
#if (SYSRTC_PRS == 1)
    sl_sysrtc_group_capture_channel_input_edge_config_t group_capture_channel_config =
      SYSRTC_GROUP_CHANNEL_CAPTURE_CONFIG_RISE_EDGE;
    sysrtc_group_config_handle.p_capture_channel0_config = &group_capture_channel_config;
    sl_si91x_sysrtc_enable_input_output_gpio(true);
    drv_status = sl_si91x_sysrtc_set_capture_input_prs_gpio(SL_SYSRTC_GROUP, sysrtc_prs_pin);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_set_gpio_as_capture_input, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC PRS_IN gpio pin configuration is done successfully\r\n");

    drv_status = sl_gpio_set_configuration(sl_gpio_pin_config);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO set pin configuration is successful for ULP_GPIO %d\r\n",
                          sl_gpio_pin_config.port_pin.pin);
#else
    const sl_sysrtc_group_capture_channel_input_edge_config_t group_capture_channel_config =
      SYSRTC_GROUP_CHANNEL_CAPTURE_CONFIG_DEFAULT;
    sysrtc_group_config_handle.p_capture_channel0_config = &group_capture_channel_config;
    sl_si91x_sysrtc_enable_input_output_gpio(false);
#endif
    drv_status = sl_si91x_sysrtc_configure_group(SL_SYSRTC_GROUP, &sysrtc_group_config_handle);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_configure_group, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC group configuration is done successfully\r\n");
    drv_status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_config_timer_register_callback, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC callback registered successfully\r\n");
    sl_si91x_sysrtc_set_count(counter_value1);
    sl_si91x_sysrtc_start();
    SL_PRINT_STRING_ERROR("Started SYSRTC successfully\r\n");
#if (SYSRTC_PRS == 1)
    sl_gpio_driver_set_pin(&sl_gpio_pin_config.port_pin);
#else
    while (current_count < compare_value_capture) {
      sl_si91x_sysrtc_get_count(&current_count);
    }
    drv_status = sl_si91x_sysrtc_sets_register_capture_input(SL_SYSRTC_GROUP);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_sysrtc_sets_register_capture_input, Error code: %lu\r\n",
                            (unsigned long)drv_status);
      break;
    }
#endif
#endif
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 0) && (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 0) \
     && (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 0))
    drv_status =
      sl_si91x_sysrtc_register_callback(sysrtc_callback, &callback_flag_data, SL_SYSRTC_GROUP, &interrupt_enabled);
    if (drv_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_config_timer_register_callback, Error code: %lu\r\n", drv_status);
      break;
    }
    SL_PRINT_STRING_ERROR("SYSRTC callback registered successfully\r\n");
    sl_si91x_sysrtc_set_count(counter_value2);
    /* sl_si91x_sysrtc_start() is void; no sl_status_t to verify (same as compare/capture paths). */
    sl_si91x_sysrtc_start();
    SL_PRINT_STRING_ERROR("Started SYSRTC successfully\r\n");
#endif
  } while (false);
  return drv_status;
}

/*******************************************************************************
 *      SYSRTC callback function
 ******************************************************************************/
static void sysrtc_callback(void *callback_flags)
{
  (void)callback_flags;
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  sl_si91x_sysrtc_get_capture_value(SL_SYSRTC_GROUP, &capture_val);
#endif
  sysrtc_sem_release();
  SL_PRINT_STRING_ERROR("In handler... \n");
#if defined(SL_SI91X_ACX_MODULE)
  sl_si91x_led_toggle(SL_LED_LED0_PIN);
#else
  sl_si91x_led_toggle(SL_LED_LED1_PIN);
#endif
#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE))
  static uint8_t interrupt_count = 0;
  uint32_t compare_value_isr     = SYSRTC_COMPARE_VALUE;
  uint32_t current_count         = 0;
  interrupt_count++;
  sl_si91x_sysrtc_get_count(&current_count);
  sl_si91x_sysrtc_set_compare_value(SL_SYSRTC_GROUP, COMPARE_CHANNEL, (current_count + compare_value_isr));
  if (interrupt_count == TENTH_INTERRUPT) {
    sl_si91x_sysrtc_stop();
    sl_si91x_sysrtc_deinit();
  }
#endif
#if (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE)
  sl_si91x_sysrtc_stop();
  sl_si91x_sysrtc_deinit();
#endif
}

/*******************************************************************************
 *  Releases one counting-semaphore token from ISR context.
 ******************************************************************************/
static void sysrtc_sem_release(void)
{
  if (s_sysrtc_event_sem != NULL) {
    (void)osSemaphoreRelease(s_sysrtc_event_sem);
  }
}

/*******************************************************************************
 *  SYSRTC thread: peripheral init then blocking waits on ISR-driven semaphore(s).
 ******************************************************************************/
static void sysrtc_freertos_task(void *argument)
{
  (void)argument;

  sl_status_t init_status = sysrtc_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("SYSRTC initialization failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

#if ((SL_SYSRTC_COMPARE_CHANNEL0_ENABLE == 1) || (SL_SYSRTC_COMPARE_CHANNEL1_ENABLE == 1))
  {
    const int32_t n_events = TENTH_INTERRUPT;
    for (int32_t i = 0; i < n_events; i++) {
      if (s_sysrtc_event_sem != NULL) {
        (void)osSemaphoreAcquire(s_sysrtc_event_sem, osWaitForever);
      }
    }
  }
  while (1) {
    (void)osDelay(1000);
  }
#elif (SL_SYSRTC_CAPTURE_CHANNEL0_ENABLE == 1)
  if (s_sysrtc_event_sem != NULL) {
    (void)osSemaphoreAcquire(s_sysrtc_event_sem, osWaitForever);
  }
  SL_PRINT_STRING_ERROR("capture value: %lu\r\n", (unsigned long)capture_val);
  while (1) {
    (void)osDelay(1000);
  }
#else
  while (1) {
    if (s_sysrtc_event_sem != NULL) {
      (void)osSemaphoreAcquire(s_sysrtc_event_sem, osWaitForever);
    }
    SL_PRINT_STRING_ERROR("SYSRTC overflow / generic interrupt event\r\n");
  }
#endif
}
