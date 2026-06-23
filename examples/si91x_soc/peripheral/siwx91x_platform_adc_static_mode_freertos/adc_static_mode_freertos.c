/***************************************************************************/ /**
 * @file adc_static_mode_freertos.c
 * @brief ADC FreeRTOS static-mode example: sample channels in a task, wait for
 *        completion per channel via semaphore, read and print voltage.
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
/* Project config and ADC driver */
#include "adc_static_mode_freertos.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_adc_common_config.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
/* FreeRTOS / CMSIS-RTOS2 */
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
/* ADC conversion and voltage calculation */
#define ADC_MAX_OP_VALUE 4095   // Max raw value from ADC data register (12-bit)
#define ADC_DATA_CLEAR   0xF7FF // Mask to clear sign bit for differential
#define VREF_VALUE       3.3    // Reference voltage (V) for Vout calculation
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vref_value = (float)VREF_VALUE;
static int16_t adc_output[1]; // Last read value (used for sign handling)
/* Counting semaphore: one token per static-mode completion (see adc_init_function) */
static osSemaphoreId_t adc_sample_sem;
static sl_status_t adc_init_function(void);
static void adc_static_mode_task(void *argument);
static const osThreadAttr_t adc_thread_attributes = {
  .name       = "adc_static",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void adc_event_callback_handler(uint8_t event_channel, uint8_t event);

/*******************************************************************************
**************************   GLOBAL FUNCTIONS   *******************************
******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the ADC static-mode FreeRTOS task.
 * @return None
 ******************************************************************************/
void adc_static_mode_example_init(void)
{
  /* Task runs adc_init_function then samples channels in a loop */
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)adc_static_mode_task, NULL, &adc_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create adc thread\r\n");
    return;
  }
}
/*******************************************************************************
 * @brief  One-time ADC init: version, init, channel config, callback, semaphore,
 *         and start. Callback releases semaphore on each static-mode sample.
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t adc_init_function(void)
{
  sl_adc_version_t version;
  sl_status_t status            = 0;
  sl_adc_channel_config.channel = SL_ADC_CHANNEL_1;

  /* Version information of ADC driver */
  version = sl_si91x_adc_get_version();
  SL_PRINT_STRING_ERROR("ADC version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", version.release, version.major, version.minor);

  status = sl_si91x_adc_init(sl_adc_channel_config, sl_adc_config, vref_value);
  /* Due to calling trim_efuse API on ADC init in driver it will change the
     clock frequency, if we are not initialize the debug again it will print
     the garbage data in console output. */
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_init: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("ADC Initialization Success\r\n");

  /* Configure ADC channel from instance config */
  status = sl_si91x_adc_set_channel_configuration(sl_adc_channel_config, sl_adc_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_channel_set_configuration: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("ADC Channel Configuration Successfully\r\n");

  /* Register callback; it releases adc_sample_sem on SL_ADC_STATIC_MODE_EVENT */
  status = sl_si91x_adc_register_event_callback(adc_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_register_event_callback: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("ADC user event callback registered successfully\r\n");

  /* Counting semaphore (max = enabled channels): each completion may signal before the
     task runs; a binary semaphore would drop extra releases when num_of_channel_enable > 1. */
  if (adc_sample_sem == NULL) {
    uint32_t sem_max = (uint32_t)sl_adc_config.num_of_channel_enable;
    if (sem_max < 1U) {
      sem_max = 1U;
    }
    adc_sample_sem = osSemaphoreNew(sem_max, 0U, NULL);
    if (adc_sample_sem == NULL) {
      SL_PRINT_STRING_ERROR("Failed to create adc semaphore\r\n");
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  /* Start ADC; static mode will trigger callback per sample */
  status = sl_si91x_adc_start(sl_adc_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_start: Error Code : %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("ADC started Successfully\r\n");

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  ADC event callback: gives one counting-semaphore token per static-mode conversion
 *         completion (one per channel per scan round when multiple channels are enabled).
 * @param  event_channel  Channel index (unused in this example)
 * @param  event          SL_ADC_STATIC_MODE_EVENT when static sample is ready
 * @return None
 ******************************************************************************/
static void adc_event_callback_handler(uint8_t event_channel, uint8_t event)
{
  (void)event_channel;
  if (event == SL_ADC_STATIC_MODE_EVENT) {
    if (adc_sample_sem != NULL) {
      (void)osSemaphoreRelease(adc_sample_sem);
    }
  }
}

/*******************************************************************************
 * @brief  ADC static-mode task: init, then for each enabled channel wait for
 *         semaphore (from callback), read data, convert to voltage and print.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void adc_static_mode_task(void *argument)
{
  (void)argument;

  sl_status_t init_status = adc_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ADC initialization failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

  while (1) {
    sl_status_t status;
    uint16_t adc_value;
    uint8_t channel_num;
    float vout = 0.0f;

    /* Iterate all enabled channels; one semaphore wait per channel read */
    for (channel_num = 0; channel_num < sl_adc_config.num_of_channel_enable; channel_num++) {
      /* Wait for this channel's completion (callback releases semaphore) */
      if (adc_sample_sem != NULL) {
        (void)osSemaphoreAcquire(adc_sample_sem, osWaitForever);
      }
      status = sl_si91x_adc_read_data_static(sl_adc_channel_config, sl_adc_config, &adc_value);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_adc_read_data_static: Error Code : %lu\r\n", status);
        continue;
      }
      /* Apply sign handling for 12-bit static result */
      adc_output[0] = (int16_t)adc_value;
      if (adc_output[0] & SIGN_BIT) {
        adc_output[0] = (int16_t)(adc_output[0] & (ADC_DATA_CLEAR));
      } else {
        adc_output[0] = adc_output[0] | SIGN_BIT;
      }
      /* Convert to voltage using Vref */
      vout = (((float)adc_output[0] / (float)ADC_MAX_OP_VALUE) * vref_value);
      /* Differential input: report relative to Vref/2 */
      if (sl_adc_channel_config.input_type[channel_num]) {
        vout = vout - (vref_value / 2);
        SL_PRINT_STRING_ERROR("Differential ended input  :%ldmV\r\n", (int32_t)(vout * 1000.0f));
      } else {
        SL_PRINT_STRING_ERROR("ADC Channel[%d] Measured input :%ldmV\r\n", channel_num, (int32_t)(vout * 1000.0f));
      }
      /* Extra newline after last channel in multi-channel mode */
      if (sl_adc_config.num_of_channel_enable > 1) {
        if (channel_num >= (sl_adc_config.num_of_channel_enable - 1)) {
          SL_PRINT_STRING_ERROR("\r\n\r\n");
        }
      }
    }
  }
}
