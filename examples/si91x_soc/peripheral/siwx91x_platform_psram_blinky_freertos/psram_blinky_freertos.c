/***************************************************************************/ /**
 * @file psram_blinky_freertos.c
 * @brief PSRAM Blinky FreeRTOS Example
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
#include "psram_blinky_freertos.h"
#include "sl_log_helper.h"
#include <stdint.h>
#include "rsi_ccp_user_config.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_si91x_led.h"
#include "sl_si91x_led_instances.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 500U
#endif

#define BLINK_SEM_MAX 1U

/*******************************************************************************
 ***************************  Local state  *************************************
 ******************************************************************************/
static osSemaphoreId_t s_blink_tick_sem;
static osTimerId_t s_blink_tick_timer;

/*******************************************************************************
 ***************************  Local Function Prototypes  ***********************
 ******************************************************************************/
static void psram_blinky_task(void *argument);
static void blink_tick_timer_callback(void *argument);
static uint32_t blink_delay_ms_to_ticks(uint32_t ms);

static const osThreadAttr_t psram_blinky_thread_attributes = {
  .name       = "psram_blinky",
  .stack_size = 2048U,
  .priority   = osPriorityLow1,
};

static const osTimerAttr_t s_blink_timer_attributes = {
  .name = "psram_blnk",
};

/*******************************************************************************
 ***************************  Example Initialization  **************************
 ******************************************************************************/
void psram_blinky_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)psram_blinky_task, NULL, &psram_blinky_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create psram_blinky thread\r\n");
  }
}

/*******************************************************************************
 ***************************  ms → kernel ticks  *******************************
 ******************************************************************************/
static uint32_t blink_delay_ms_to_ticks(uint32_t ms)
{
  const uint32_t hz = osKernelGetTickFreq();
  uint64_t t;
  if (hz == 0U) {
    return (ms == 0U) ? 1U : ms;
  }
  t = ((uint64_t)ms * (uint64_t)hz + 999ULL) / 1000ULL;
  if (t < 1ULL) {
    t = 1ULL;
  }
  if (t > (uint64_t)(UINT32_MAX - 1U)) {
    t = (uint64_t)(UINT32_MAX - 1U);
  }
  return (uint32_t)t;
}

/*******************************************************************************
 ***************************  osTimer callback  ********************************
 * Releases `s_blink_tick_sem` each period so the blink task toggles the LED.
 ******************************************************************************/
static void blink_tick_timer_callback(void *argument)
{
  (void)argument;
  if (s_blink_tick_sem != NULL) {
    (void)osSemaphoreRelease(s_blink_tick_sem);
  }
}

/*******************************************************************************
 ***************************  Main Task  **************************************
 ******************************************************************************/
static void psram_blinky_task(void *argument)
{
  uint32_t period_ticks;
  osStatus_t w;

  (void)argument;

  s_blink_tick_sem = osSemaphoreNew(BLINK_SEM_MAX, 0U, NULL);
  if (s_blink_tick_sem == NULL) {
    SL_PRINT_STRING_ERROR("PSRAM blinky: failed to create semaphore\r\n");
    osThreadExit();
  }

  s_blink_tick_timer = osTimerNew(blink_tick_timer_callback, osTimerPeriodic, NULL, &s_blink_timer_attributes);
  if (s_blink_tick_timer == NULL) {
    SL_PRINT_STRING_ERROR("PSRAM blinky: failed to create osTimer\r\n");
    (void)osSemaphoreDelete(s_blink_tick_sem);
    s_blink_tick_sem = NULL;
    osThreadExit();
  }

  period_ticks = blink_delay_ms_to_ticks(BLINK_DELAY_MS);
  if (osTimerStart(s_blink_tick_timer, period_ticks) != osOK) {
    SL_PRINT_STRING_ERROR("PSRAM blinky: osTimerStart failed\r\n");
    (void)osTimerDelete(s_blink_tick_timer);
    s_blink_tick_timer = NULL;
    (void)osSemaphoreDelete(s_blink_tick_sem);
    s_blink_tick_sem = NULL;
    osThreadExit();
  }

  for (;;) {
    w = osSemaphoreAcquire(s_blink_tick_sem, osWaitForever);
    if (w != osOK) {
      SL_PRINT_STRING_ERROR("PSRAM blinky: osSemaphoreAcquire error %d\r\n", (int)w);
      continue;
    }
    sl_si91x_led_toggle(LED_INSTANCE.pin);
  }
}
