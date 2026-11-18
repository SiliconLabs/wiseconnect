/***************************************************************************/ /**
 * @file joystick_freertos.c
 * @brief Joystick FreeRTOS example
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "joystick_freertos.h"
#include <stdint.h>
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_si91x_joystick.h"
#include "sl_status.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

#define JOYSTICK_SAMPLE_PERIOD_MS 400U
/** Counting semaphore depth : avoids losing timer releases if the task briefly falls behind. */
#define JOYSTICK_SEM_MAX 8U

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static osSemaphoreId_t s_joystick_sample_sem;
static osTimerId_t s_joystick_tick_timer;

static sl_status_t joystick_init_function(void);
static void joystick_task(void *argument);
static void joystick_tick_timer_callback(void *argument);
static uint32_t joystick_ms_to_kernel_ticks(uint32_t ms);

static const osThreadAttr_t joystick_thread_attributes = {
  .name       = "joystick",
  .stack_size = 2048U,
  .priority   = osPriorityLow1,
};

static const osTimerAttr_t s_joystick_timer_attributes = {
  .name = "joy_tick",
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void joystick_example_init(void)
{
  /* Status prints use SL_PRINT_STRING_ERROR so they stay visible at the default log level. */
  osThreadId_t tid = osThreadNew((osThreadFunc_t)joystick_task, NULL, &joystick_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create joystick thread\r\n");
  }
}

/*******************************************************************************
 ***********************  joystick ms to kernel ticks  **************************
 ******************************************************************************/

static uint32_t joystick_ms_to_kernel_ticks(uint32_t ms)
{
  const uint32_t hz = osKernelGetTickFreq();
  uint64_t t;

  if (ms == 0U) {
    return 1U;
  }
  if (hz == 0U) {
    return ms;
  }
  t = ((uint64_t)ms * (uint64_t)hz + 999ULL) / 1000ULL;
  if (t > (uint64_t)(UINT32_MAX - 1U)) {
    t = (uint64_t)(UINT32_MAX - 1U);
  }
  return (uint32_t)t;
}

/*******************************************************************************
 ***************************  Timer Callback  **********************************
 ******************************************************************************/
static void joystick_tick_timer_callback(void *argument)
{
  (void)argument;
  if (s_joystick_sample_sem != NULL) {
    (void)osSemaphoreRelease(s_joystick_sample_sem);
  }
}

/*******************************************************************************
 ***************************  Initialization  **********************************
 ******************************************************************************/
static sl_status_t joystick_init_function(void)
{
  sl_status_t status;
  uint32_t period_ticks;

  DEBUGINIT();

  s_joystick_sample_sem = osSemaphoreNew(JOYSTICK_SEM_MAX, 0U, NULL);
  if (s_joystick_sample_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create joystick sample semaphore\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  s_joystick_tick_timer = osTimerNew(joystick_tick_timer_callback, osTimerPeriodic, NULL, &s_joystick_timer_attributes);
  if (s_joystick_tick_timer == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create joystick tick timer\r\n");
    (void)osSemaphoreDelete(s_joystick_sample_sem);
    s_joystick_sample_sem = NULL;
    return SL_STATUS_ALLOCATION_FAILED;
  }

  status = sl_si91x_joystick_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_joystick_init: Error Code : 0x%lx \r\n", (unsigned long)status);
    (void)osTimerDelete(s_joystick_tick_timer);
    s_joystick_tick_timer = NULL;
    (void)osSemaphoreDelete(s_joystick_sample_sem);
    s_joystick_sample_sem = NULL;
    return status;
  }
  SL_PRINT_STRING_ERROR("Joystick Initialization Success\r\n");

  status = sl_si91x_joystick_start(SL_JOYSTICK_ENABLED);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_joystick_start: Error Code : 0x%lx \r\n", (unsigned long)status);
    (void)osTimerDelete(s_joystick_tick_timer);
    s_joystick_tick_timer = NULL;
    (void)osSemaphoreDelete(s_joystick_sample_sem);
    s_joystick_sample_sem = NULL;
    return status;
  }
  SL_PRINT_STRING_ERROR("Joystick started successfully\r\n");

  period_ticks = joystick_ms_to_kernel_ticks(JOYSTICK_SAMPLE_PERIOD_MS);
  if (osTimerStart(s_joystick_tick_timer, period_ticks) != osOK) {
    SL_PRINT_STRING_ERROR("osTimerStart (joystick) failed\r\n");
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 ***************************  Main Task  ***************************************
 ******************************************************************************/
static void joystick_task(void *argument)
{
  (void)argument;
  sl_status_t status;
  sl_joystick_position_t pos;

  status = joystick_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Joystick init failed: 0x%lx, exiting task\r\n", (unsigned long)status);
    osThreadExit();
  }

  for (;;) {
    (void)osSemaphoreAcquire(s_joystick_sample_sem, osWaitForever);

    status = sl_si91x_joystick_get_position(SL_JOYSTICK_ENABLED, &pos);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_joystick_get_position: Error Code : 0x%lx \r\n", (unsigned long)status);
      continue;
    }
    switch (pos) {
      case SL_JOYSTICK_NONE:
        SL_PRINT_STRING_ERROR("Not Pressed\r\n");
        break;
      case SL_JOYSTICK_C:
        SL_PRINT_STRING_ERROR("Center\r\n");
        break;
      case SL_JOYSTICK_N:
        SL_PRINT_STRING_ERROR("North\r\n");
        break;
      case SL_JOYSTICK_S:
        SL_PRINT_STRING_ERROR("South\r\n");
        break;
      case SL_JOYSTICK_E:
        SL_PRINT_STRING_ERROR("East\r\n");
        break;
      case SL_JOYSTICK_W:
        SL_PRINT_STRING_ERROR("West\r\n");
        break;
      default:
        SL_PRINT_STRING_ERROR("Unknown position\r\n");
        break;
    }
  }
}
