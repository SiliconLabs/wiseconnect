/***************************************************************************/ /**
 * @file adc_fifo_mode_freertos.c
 * @brief ADC FreeRTOS FIFO-mode example: sample each enabled channel in a task,
 *        wait for DMA completion via CMSIS-RTOS2 event flags (osEventFlags),
 *        read FIFO data and print voltage (per-channel local config, like
 *        read_adc_value). Includes DAC FIFO path when DAC_FIFO_MODE_EN is set.
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
#include "adc_fifo_mode_freertos.h"
#include "rsi_debug.h"
#include "rsi_rom_clks.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_adc_common_config.h"
#ifdef DAC_FIFO_MODE_EN
#include "sl_si91x_dac.h"
#include "sl_si91x_dac_config.h"
#endif
#include "cmsis_os2.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PS4_SOC_FREQ          180000000
#define SOC_PLL_REF_FREQUENCY 40000000
#define DIVISION_FACTOR       0
#define CHANNEL_SAMPLE_LENGTH 1023
#define ADC_MAX_OP_VALUE      4095
#define ADC_DATA_CLEAR        0xF7FF
#define VREF_VALUE            3.3
#define ADC_PING_BUFFER       0x0000A000
#define MEASUREMENT_DELAY_MS  1000

/** Event flag: FIFO DMA transfer complete (set in callback when using slot 0). */
#define ADC_EVT_FIFO_DMA_DONE (1UL << 0)

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vref_value = (float)VREF_VALUE;
static int16_t adc_output[CHANNEL_SAMPLE_LENGTH];
static osEventFlagsId_t adc_event_flags;
#ifdef DAC_FIFO_MODE_EN
static volatile boolean_t dac_fifo_intr_flag = false;

#endif

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void adc_fifo_mode_task(void *argument);
static void read_adc_value(uint8_t channel_num);
static void adc_event_callback_handler(uint8_t event_channel, uint8_t event);
#ifdef DAC_FIFO_MODE_EN
static void dac_event_callback_handler(uint8_t event);
#endif

static const osThreadAttr_t adc_thread_attributes = {
  .name       = "adc_fifo",
  .stack_size = 4096,
  .priority   = osPriorityLow1,
};
#ifdef DAC_FIFO_MODE_EN
static sl_dac_clock_config_t dac_clock_config = { 0 };
#endif
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void adc_fifo_mode_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)adc_fifo_mode_task, NULL, &adc_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create ADC FIFO thread\r\n");
    return;
  }
}

/*******************************************************************************
 * Callback: signal task when FIFO DMA completes (we use slot 0 only in local
 * config, so event_channel is 0).
 ******************************************************************************/
static void adc_event_callback_handler(uint8_t event_channel, uint8_t event)
{
  if ((event == SL_INTERNAL_DMA) && (event_channel == 0)) {
    if (adc_event_flags != NULL) {
      (void)osEventFlagsSet(adc_event_flags, ADC_EVT_FIFO_DMA_DONE);
    }
  }
}

#ifdef DAC_FIFO_MODE_EN
/*******************************************************************************
 * DAC callback: set flag when DAC FIFO needs rewrite.
 ******************************************************************************/
static void dac_event_callback_handler(uint8_t event)
{
  if (event == SL_DAC_FIFO_MODE_EVENT) {
    dac_fifo_intr_flag = true;
  }
}
#endif

/*******************************************************************************
 * Read one channel in FIFO mode: local config (slot 0), init -> start ->
 * wait on event -> read -> deinit; return average voltage.
 ******************************************************************************/
static void read_adc_value(uint8_t channel_num)
{
  sl_status_t status;
  uint32_t sample_length;
  float vout = 0.0f;
  uint32_t wait_result;

  sl_adc_channel_config_t local_adc_channel_config = { 0 };
  sl_adc_config_t local_sl_adc_config              = { 0 };

  local_sl_adc_config.num_of_channel_enable = 1;

  if (channel_num >= sl_adc_config.num_of_channel_enable) {
    return;
  }

  local_adc_channel_config.input_type[0]     = sl_adc_channel_config.input_type[channel_num];
  local_adc_channel_config.pos_inp_sel[0]    = sl_adc_channel_config.pos_inp_sel[channel_num];
  local_adc_channel_config.neg_inp_sel[0]    = sl_adc_channel_config.neg_inp_sel[channel_num];
  local_adc_channel_config.num_of_samples[0] = sl_adc_channel_config.num_of_samples[channel_num];
  local_adc_channel_config.sampling_rate[0]  = sl_adc_channel_config.sampling_rate[channel_num];

  local_adc_channel_config.rx_buf[0]            = adc_output;
  local_adc_channel_config.chnl_ping_address[0] = ADC_PING_BUFFER;
  local_adc_channel_config.chnl_pong_address[0] = ADC_PING_BUFFER + local_adc_channel_config.num_of_samples[0];

  status = sl_si91x_adc_init(local_adc_channel_config, local_sl_adc_config, vref_value);
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_init: Error Code : %lu\r\n", status);
    return;
  }

  status = sl_si91x_adc_set_channel_configuration(local_adc_channel_config, local_sl_adc_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_channel_set_configuration: Error Code : %lu\r\n", status);
    sl_si91x_adc_deinit(local_sl_adc_config);
    return;
  }

  status = sl_si91x_adc_register_event_callback(adc_event_callback_handler);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_register_event_callback: Error Code : %lu\r\n", status);
    sl_si91x_adc_deinit(local_sl_adc_config);
    return;
  }
  /* Clear stale flag before this acquisition */
  if (adc_event_flags != NULL) {
    (void)osEventFlagsClear(adc_event_flags, ADC_EVT_FIFO_DMA_DONE);
  }

  status = sl_si91x_adc_start(local_sl_adc_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_start: Error Code : %lu\r\n", status);

    sl_si91x_adc_unregister_event_callback();
    sl_si91x_adc_deinit(local_sl_adc_config);
    return;
  }

  /* Wait for DMA complete (replaces while (!data_sample_complete_flag)) */
  if (adc_event_flags != NULL) {
    wait_result = osEventFlagsWait(adc_event_flags, ADC_EVT_FIFO_DMA_DONE, osFlagsWaitAny, osWaitForever);
    if ((int32_t)wait_result < 0) {
      SL_PRINT_STRING_ERROR("osEventFlagsWait error: 0x%lx\r\n", (unsigned long)wait_result);

      sl_si91x_adc_unregister_event_callback();
      sl_si91x_adc_stop(local_sl_adc_config);
      sl_si91x_adc_deinit(local_sl_adc_config);
      return;
    }
  }

  status = sl_si91x_adc_read_data(local_adc_channel_config, 0);

  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_read_data: Error Code : %lu\r\n", status);
    sl_si91x_adc_unregister_event_callback();
    sl_si91x_adc_stop(local_sl_adc_config);
    sl_si91x_adc_deinit(local_sl_adc_config);
    return;
  }

  osDelay(MEASUREMENT_DELAY_MS);
  sl_si91x_adc_unregister_event_callback();
  sl_si91x_adc_stop(local_sl_adc_config);
  sl_si91x_adc_deinit(local_sl_adc_config);

  for (sample_length = 0; sample_length < local_adc_channel_config.num_of_samples[0]; sample_length++) {
    if (adc_output[sample_length] & SIGN_BIT) {
      adc_output[sample_length] = (int16_t)(adc_output[sample_length] & (ADC_DATA_CLEAR));
    } else {
      adc_output[sample_length] = adc_output[sample_length] | SIGN_BIT;
    }
    vout = (((float)adc_output[sample_length] / (float)ADC_MAX_OP_VALUE) * vref_value);
    if (sl_adc_channel_config.input_type[channel_num]) {
      vout = vout - (vref_value / 2);
    }
    SL_PRINT_STRING_ERROR("Channel %d ADC Measured input[%ld] :%ldmV\r\n",
                          channel_num,
                          sample_length,
                          (int32_t)(vout * 1000.0f));
  }
#ifdef DAC_FIFO_MODE_EN

  dac_clock_config.soc_pll_clock           = PS4_SOC_FREQ;
  dac_clock_config.soc_pll_reference_clock = SOC_PLL_REF_FREQUENCY;
  dac_clock_config.division_factor         = DIVISION_FACTOR;

  status = sl_si91x_dac_init(&dac_clock_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_dac_init: Error Code : %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("SL_DAC initialization is successful\r\n");
    status = sl_si91x_dac_set_configuration(sl_dac_config, vref_value);
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_dac_set_configuration: Error Code : %lu\r\n", status);
    } else {
      SL_PRINT_STRING_ERROR("SL_DAC set configuration is successful\r\n");
      status = sl_si91x_dac_register_event_callback(dac_event_callback_handler);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_dac_register_event_callback: Error Code : %lu\r\n", status);
      } else {
        SL_PRINT_STRING_ERROR("SL_DAC register event callback is successful\r\n");
      }
    }
  }

  if (sl_adc_config.num_of_channel_enable > 0) {

    status = sl_si91x_dac_write_data((int16_t *)adc_output, local_adc_channel_config.num_of_samples[0]);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_dac_write_data: Error Code : %lu\r\n", status);
    }

    status = sl_si91x_dac_start();
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_dac_start: Error Code : %lu\r\n", status);
    }

    while (!dac_fifo_intr_flag) {
    }
    dac_fifo_intr_flag = false;
    SL_PRINT_STRING_ERROR("DAC successfully sampled\r\n");
    osDelay(MEASUREMENT_DELAY_MS);
  }

  status = sl_si91x_dac_stop();

  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_dac_stop: Error Code : %lu\r\n", status);
  }
  status = sl_si91x_dac_deinit();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_dac_deinit: Error Code : %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("SL_DAC deinitialization is successful\r\n");
  }

#endif

  return;
}

/*******************************************************************************
 * ADC FIFO task: create event flags, optional DAC init, print version, then
 * loop over enabled channels (read each via read_adc_value), DAC
 * (write/start or rewrite using last channel's data), print result, delay.
 ******************************************************************************/
static void adc_fifo_mode_task(void *argument)
{
  (void)argument;

  sl_adc_channel_config.channel = SL_ADC_CHANNEL_1;

  sl_adc_version_t version = sl_si91x_adc_get_version();
  SL_PRINT_STRING_ERROR("ADC version is fetched successfully\r\n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", version.release, version.major, version.minor);

  adc_event_flags = osEventFlagsNew(NULL);
  if (adc_event_flags == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ADC event flags\r\n");
    osThreadExit();
  }

  while (1) {

    for (uint8_t channel_num_read = 0; channel_num_read < sl_adc_config.num_of_channel_enable; channel_num_read++) {
      read_adc_value(channel_num_read);
    }

    osDelay(MEASUREMENT_DELAY_MS);
  }
}
