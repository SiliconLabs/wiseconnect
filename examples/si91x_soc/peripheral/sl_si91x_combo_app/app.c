/*******************************************************************************
 * @file app.c
 * @brief Top level application functions
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
#include "app.h"
#include "push_button_app.h"
#include "pwm_app.h"
#include "timer_app.h"
#include "i2c_app.h"
#include "stddef.h"
#include "cmsis_os2.h"
#include "rsi_debug.h"
/******************************************************
 *               Variable Definitions
 ******************************************************/
static osThreadId_t tid_thread_button; // thread id for button thread
static osThreadId_t tid_thread_pwm;    // thread id for pwm thread
static osThreadId_t tid_thread_i2c;    // thread id for i2c thread

static const osThreadAttr_t button_thread_attributes = {
  .name = "button",
  .stack_size =
    1024, // using 1024 for all the threads, may use as low as possible based on the need of respective thread
  .priority = osPriorityLow,
};

static const osThreadAttr_t pwm_thread_attributes = {
  .name = "pwm",
  .stack_size =
    1024, // using 1024 for all the threads, may use as low as possible based on the need of respective thread
  .priority = osPriorityLow1,
};

static const osThreadAttr_t i2c_thread_attributes = {
  .name = "i2c",
  .stack_size =
    1024, // using 1024 for all the threads, may use as low as possible based on the need of respective thread
  .priority = osPriorityLow1,
};
/*******************************************************************************
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  button_init();
  pwm_init();
  i2c_init();
  timer_init();

  // create a thread for button functionality
  tid_thread_button = osThreadNew((osThreadFunc_t)button_app, NULL, &button_thread_attributes);
  if (tid_thread_button == NULL) {
    // Handle error - Thread creation
    return;
  }

  // create a thread for pwm functionality
  tid_thread_pwm = osThreadNew((osThreadFunc_t)pwm_app, NULL, &pwm_thread_attributes);
  if (tid_thread_pwm == NULL) {
    // Handle error - Thread creation
    return;
  }

  // create a thread for i2c leader functionality
  tid_thread_i2c = osThreadNew((osThreadFunc_t)i2c_app, NULL, &i2c_thread_attributes);
  if (tid_thread_i2c == NULL) {
    // Handle error - Thread creation
    return;
  }
}
/*******************************************************************************
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
}
