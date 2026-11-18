/***************************************************************************/ /**
 * @file ulp_adc_freertos.c
 * @brief ULP ADC FreeRTOS example
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
#include "ulp_adc_freertos.h"
#include <stdio.h>
#include <stdint.h>
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_adc_instances.h"
#include "sl_si91x_adc.h"
#include "sl_si91x_adc_common_config.h"
#include "sl_si91x_power_manager.h"
#include "cmsis_os2.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define CHANNEL_SAMPLE_LENGTH 1023
#define ADC_MAX_OP_VALUE      4095
#define ADC_DATA_CLEAR        0xF7FF
#define VREF_VALUE            3.3
#define ADC_PING_BUFFER       0x24060800

/** PS transition events (same mask as `power_manager_tickless_idle_example.c`). */
#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
typedef enum {
  SL_ULP_ADC_PROCESS_ACTION,
  SL_ULP_ADC_POWER_STATE_TRANSITION,
  SL_ULP_ADC_TRANSMISSION_COMPLETED,
} ulp_adc_enum_t;

static void ulp_adc_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static float vref_value = (float)VREF_VALUE;
static int16_t adc_output[CHANNEL_SAMPLE_LENGTH];
static uint8_t adc_channel = 0;

static sl_adc_config_t adc_runtime_config;
static ulp_adc_enum_t ulp_adc_current_mode = SL_ULP_ADC_PROCESS_ACTION;
static sl_status_t status;
static sl_power_state_t current_power_state = SL_SI91X_POWER_MANAGER_PS4;

/** Binary semaphore: released from ADC callback on FIFO DMA or static sample complete. */
static osSemaphoreId_t ulp_adc_sample_sem;

static sl_power_manager_ps_transition_event_handle_t s_ulp_adc_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_adc_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_adc_pm_transition_callback,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void ulp_adc_freertos_task(void *argument);
static sl_status_t ulp_adc_application_init(void);
static void callback_event(uint8_t channel_no, uint8_t event);
static void adc_read_data_fifo_mode(void);
static void adc_read_data_static_mode(void);
static sl_status_t configuring_ps2_power_state(void);
static void ulp_adc_wait_sample_done(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);
static void ulp_adc_sample_sem_drain(void);

static const osThreadAttr_t ulp_adc_thread_attributes = {
  .name       = "ulp_adc",
  .stack_size = 3072,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

void adc_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)ulp_adc_freertos_task, NULL, &ulp_adc_thread_attributes);
  if (thread_id == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP ADC thread\r\n");
  }
}

/*******************************************************************************
 * Remove stray binary semaphore tokens before a new capture window.
 ******************************************************************************/
static void ulp_adc_sample_sem_drain(void)
{
  if (ulp_adc_sample_sem == NULL) {
    return;
  }
  while (osSemaphoreAcquire(ulp_adc_sample_sem, 0U) == osOK) {
    /* drain */
  }
}

/*******************************************************************************
 * Block until the driver callback signals FIFO DMA or static sample complete.
 ******************************************************************************/
static void ulp_adc_wait_sample_done(void)
{
  osStatus_t w;

  if (ulp_adc_sample_sem == NULL) {
    return;
  }

  w = osSemaphoreAcquire(ulp_adc_sample_sem, osWaitForever);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR("osSemaphoreAcquire  error: %d\r\n", (int)w);
  }
}

/*******************************************************************************
 * ADC callback event handler
 ******************************************************************************/
static void callback_event(uint8_t channel_no, uint8_t event)
{
  if (event == SL_INTERNAL_DMA) {
    /* ADC IRQ uses logical ADC channel; UDMA completion passes DMA channel index â€” do not require equality for single-channel builds. */
    if ((sl_adc_config.num_of_channel_enable == 1U) || (channel_no == adc_channel)) {
      if (ulp_adc_sample_sem != NULL) {
        (void)osSemaphoreRelease(ulp_adc_sample_sem);
      }
    }
  } else if (event == SL_ADC_STATIC_MODE_EVENT) {
    if (ulp_adc_sample_sem != NULL) {
      (void)osSemaphoreRelease(ulp_adc_sample_sem);
    }
  }
}

/*******************************************************************************
 * ADC application initialization
 ******************************************************************************/
static sl_status_t ulp_adc_application_init(void)
{
  sl_adc_version_t version;

  status                        = SL_STATUS_OK;
  sl_adc_channel_config.channel = SL_ADC_CHANNEL_1;
  adc_channel                   = sl_adc_channel_config.channel;
  adc_runtime_config            = sl_adc_config;
  if ((adc_runtime_config.num_of_channel_enable < 1U) || (adc_runtime_config.num_of_channel_enable > 16U)) {
    adc_runtime_config.num_of_channel_enable = 1U;
  }
  if (adc_runtime_config.operation_mode >= SL_ADC_OPERATION_MODE_LAST) {
    adc_runtime_config.operation_mode = SL_ADC_FIFO_MODE;
  }
  sl_adc_channel_config.rx_buf[adc_channel]            = adc_output;
  sl_adc_channel_config.chnl_ping_address[adc_channel] = ADC_PING_BUFFER;
  sl_adc_channel_config.chnl_pong_address[adc_channel] =
    ADC_PING_BUFFER + (sl_adc_channel_config.num_of_samples[adc_channel]);

  do {
    /* Driver requires callback cleared before a new register; deinit also clears.
     * Call here so re-init after partial failure or stop/deinit race is safe. */
    sl_si91x_adc_unregister_event_callback();

    version = sl_si91x_adc_get_version();
    SL_PRINT_STRING_ERROR("ADC version is fetched successfully \r\n");
    SL_PRINT_STRING_ERROR("API version is %d.%d.%d\r\n", version.release, version.major, version.minor);
    status = sl_si91x_adc_init(sl_adc_channel_config, adc_runtime_config, vref_value);
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_init: Error Code : %lu \r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC Initialization Success\r\n");
    status = sl_si91x_adc_set_channel_configuration(sl_adc_channel_config, adc_runtime_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_channel_set_configuration: Error Code : %lu \r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC Channel Configuration Successfully \r\n");
    status = sl_si91x_adc_register_event_callback(callback_event);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_register_event_callback: Error Code : %lu \r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC user event callback registered successfully \r\n");
    ulp_adc_sample_sem_drain();

    SL_PRINT_STRING_ERROR("ADC configuration: channels=%u ch=%u samples=%lu\r\n",
                          (unsigned int)adc_runtime_config.num_of_channel_enable,
                          (unsigned int)adc_channel,
                          (unsigned long)sl_adc_channel_config.num_of_samples[adc_channel]);

    status = sl_si91x_adc_start(adc_runtime_config);

    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_start: Error Code : %lu \r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("ADC started Successfully\r\n");
  } while (false);
  return status;
}

/*******************************************************************************
 * ADC read data in FIFO mode
 ******************************************************************************/
static void adc_read_data_fifo_mode(void)
{
  uint32_t sample_length;
  uint8_t chnl_num = 0;
  float vout       = 0.0f;

  for (chnl_num = 0; chnl_num < adc_runtime_config.num_of_channel_enable; chnl_num++) {
    status = sl_si91x_adc_read_data(sl_adc_channel_config, adc_channel);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_adc_read_data: Error Code : %lu \r\n", status);
    }

    for (sample_length = 0; sample_length < sl_adc_channel_config.num_of_samples[adc_channel]; sample_length++) {
      if (adc_output[sample_length] & SIGN_BIT) {
        adc_output[sample_length] = (int16_t)(adc_output[sample_length] & (ADC_DATA_CLEAR));
      } else {
        adc_output[sample_length] = adc_output[sample_length] | SIGN_BIT;
      }
      vout = (((float)adc_output[sample_length] / (float)ADC_MAX_OP_VALUE) * vref_value);
      if (sl_adc_channel_config.input_type[adc_channel]) {
        vout = vout - (vref_value / 2);
      }

      SL_PRINT_STRING_ERROR("ADC Measured input[%ld] :%ldmV \n", sample_length, (int32_t)(vout * 1000.0f));
    }
  }
}

/*******************************************************************************
 * ADC read data in static mode
 ******************************************************************************/
static void adc_read_data_static_mode(void)
{
  uint16_t adc_value;
  float vout = 0.0f;

  status = sl_si91x_adc_read_data_static(sl_adc_channel_config, adc_runtime_config, &adc_value);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_adc_read_data_static: Error Code : %lu \r\n", status);
  }
  adc_output[0] = (int16_t)adc_value;
  if (adc_output[0] & SIGN_BIT) {
    adc_output[0] = (int16_t)(adc_output[0] & (ADC_DATA_CLEAR));
  } else {
    adc_output[0] = adc_output[0] | SIGN_BIT;
  }
  vout = (((float)adc_output[0] / (float)ADC_MAX_OP_VALUE) * vref_value);
  if (sl_adc_channel_config.input_type[adc_channel]) {
    vout = vout - (vref_value / 2);
    SL_PRINT_STRING_ERROR("Differential ended input  :%ldmV\r\n", (int32_t)(vout * 1000.0f));
  } else {
    SL_PRINT_STRING_ERROR("Single ended input :%ldmV\n", (int32_t)(vout * 1000.0f));
  }
}

/*******************************************************************************
 * Configure PS2 power state
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;

  status                     = SL_STATUS_OK;
  config.configure_ram_banks = true;
  /* Sl_si91x ULP ADC uses BANK_8|9|10 to power-down upper M4SS SRAM. That is unsafe here:
   * WiFi + FreeRTOS + SLI_SI91X_MCU_ENABLE_RAM_BASED_EXECUTION typically place code/stack/heap in
   * those banks. RSI_PS_M4ssRamBanksPowerDown() then powers off the memory the CPU is executing
   * from â€” lockup before the next instruction (no second DEBUGOUT in sli_si91x_power_manager.c).
   * Keep m4ss_ram_banks == 0 so no M4SS bank is powered down; retention is still set via
   * get_ram_retention_mode(~0, ...). For minimum PS2 current, use a linker map that keeps all
   * executable RAM in banks you are allowed to shut down, then restore a non-zero mask. */
  config.m4ss_ram_banks  = 0;
  config.ulpss_ram_banks = 0;
  peri.m4ss_peripheral   = 0;
  peri.ulpss_peripheral  = SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCURTC | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2 | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_TIMEPERIOD;
  do {
    status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_remove_peripheral_requirement failed, Error Code: 0x%lX\r\n",
                            status);
      break;
    }
    status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_configure_ram_retention failed, Error Code: 0x%lX\r\n", status);
      break;
    }

  } while (false);
  return status;
}

/*******************************************************************************
 * Power Manager PS transition callback (aligned with tickless idle example).
 ******************************************************************************/
static void ulp_adc_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
{
  /* Do not call DEBUGINIT() here: it runs mid-transition before the task's PS2
   * sequence (DEBUGINIT + configuring_ps2_power_state) and can leave ULP UART
   * out of sync with clocks after trim. Logging only. */
  switch (from) {
    case SL_SI91X_POWER_MANAGER_PS4:
      SL_PRINT_STRING_ERROR("Leaving PS4 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      SL_PRINT_STRING_ERROR("Leaving PS3 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      SL_PRINT_STRING_ERROR("Leaving PS2 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      SL_PRINT_STRING_ERROR("Leaving PS1 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_SLEEP:
      SL_PRINT_STRING_ERROR("Leaving Sleep State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_STANDBY:
      SL_PRINT_STRING_ERROR("Leaving Standby State \r\n");
      break;
    default:
      break;
  }

  switch (to) {
    case SL_SI91X_POWER_MANAGER_PS4:
      SL_PRINT_STRING_ERROR("Entering PS4 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      SL_PRINT_STRING_ERROR("Entering PS3 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      SL_PRINT_STRING_ERROR("Entering PS2 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      SL_PRINT_STRING_ERROR("Entering PS1 State \r\n");
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Initialize wireless
 ******************************************************************************/
static sl_status_t initialize_wireless(void)
{
  static const sl_wifi_device_configuration_t station_init_configuration = {
    .boot_option = LOAD_NWP_FW,
    .mac_address = NULL,
    .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
    .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                     .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                     .feature_bit_map =
                       (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                        | SL_WIFI_FEAT_WPS_DISABLE
#endif
                        ),
                     .tcp_ip_feature_bit_map =
                       (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                     .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                     .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                    | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                    | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                    ),
                     .bt_feature_bit_map         = 0,
                     .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                     .ble_feature_bit_map        = 0,
                     .ble_ext_feature_bit_map    = 0,
                     .config_feature_bit_map =
                       (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) },
    .ta_pool         = { .tx_ratio_in_buffer_pool = 0, .rx_ratio_in_buffer_pool = 0, .global_ratio_in_buffer_pool = 0 },
    .efuse_data_type = SL_SI91X_EFUSE_MFG_SW_VERSION,
    .nwp_fw_image_number = SL_SI91X_NWP_FW_IMAGE_NUMBER_0
  };
  sl_status_t status;
  sl_si91x_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr           = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return status;
  }
  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  } else {
    SL_PRINT_STRING_ERROR("Failed to get mac address: 0x%lx\r\n", status);
  }
  status = sl_si91x_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
  } else {
    printf("\r\nFirmware version is: %x%x.%d.%d.%d.%d.%d.%d\r\n",
           version.chip_id,
           version.rom_id,
           version.major,
           version.minor,
           version.security_version,
           version.patch_num,
           version.customer_id,
           version.build_num);
  }
  wireless_sleep(true);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Set wireless sleep with retention
 ******************************************************************************/
static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_status_t status;
  sl_wifi_performance_profile_v2_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  status = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", status);
    return;
  }
  if (sleep_with_retention) {
    ta_performance_profile.profile = DEEP_SLEEP_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  }
  status = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", status);
  }
}

/*******************************************************************************
 * ULP ADC FreeRTOS task
 ******************************************************************************/
static void ulp_adc_freertos_task(void *argument)
{
  (void)argument;

  /* Max count 1: binary semaphore. `osSemaphoreRelease` from ADC callback is ISR-safe in this CMSIS port. */
  ulp_adc_sample_sem = osSemaphoreNew(1U, 0U, NULL);
  if (ulp_adc_sample_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP ADC sample semaphore\r\n");
    osThreadExit();
  }
  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless API initialization failed, Error Code: 0x%lX\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized; NWP in sleep with RAM retention\r\n");

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_adc_pm_event_handle, &s_ulp_adc_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Power Manager transition event subscription failed, Error Code: 0x%lX\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");

  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement (PS4): Error Code : %lu \r\n", status);
    osThreadExit();
  }

  status = ulp_adc_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ulp_adc_application_init failed (boot): Error Code : %lu \r\n", status);
    osThreadExit();
  }

  while (1) {
    switch (ulp_adc_current_mode) {
      case SL_ULP_ADC_PROCESS_ACTION:
        ulp_adc_wait_sample_done();

        if (!adc_runtime_config.operation_mode) {

          adc_read_data_fifo_mode();
        } else {
          adc_read_data_static_mode();
        }
        ulp_adc_current_mode = SL_ULP_ADC_POWER_STATE_TRANSITION;
        break;

      case SL_ULP_ADC_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("Switching adc from PS4->PS2 state \r\n");
          /* Stop callbacks before stop/deinit to avoid DMA IRQ -> osSemaphoreRelease during teardown. */
          sl_si91x_adc_unregister_event_callback();
          status = sl_si91x_adc_stop(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_stop (PS4->PS2): Error Code : %lu \r\n", status);
          }
          status = sl_si91x_adc_deinit(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_deinit (PS4->PS2): Error Code : %lu \r\n", status);
          }

          status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", status);
            break;
          }

          while (!sl_si91x_power_manager_ps2_pre_check()) {
            // Wait for the PS2 state to be safe to enter.
            osDelay(5);
          }

          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", status);
            break;
          }
          /* Power manager trim/clock change after PS2 request: re-init debug before any
           * DEBUGOUT (including inside configuring_ps2_power_state), or console shows
           * nothing/garbage â€” same order as sl_ulp_adc_example.c. */
          DEBUGINIT();

          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("configuring_ps2_power_state failed: Error Code : %lu \r\n", status);
            osThreadExit();
          }

          status = ulp_adc_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ulp_adc_application_init (PS4->PS2) failed: Error Code : %lu \r\n", status);
            osThreadExit();
          }
          current_power_state  = SL_SI91X_POWER_MANAGER_PS2;
          ulp_adc_current_mode = SL_ULP_ADC_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("Switching the adc from PS2->PS4 state\r\n");
          sl_si91x_adc_unregister_event_callback();
          status = sl_si91x_adc_stop(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_stop (PS2->PS4): Error Code : %lu \r\n", status);
          }
          status = sl_si91x_adc_deinit(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_deinit (PS2->PS4): Error Code : %lu \r\n", status);
          }
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", status);
            break;
          }
          DEBUGINIT();

          status = ulp_adc_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ulp_adc_application_init (PS2->PS4) failed: Error Code : %lu \r\n", status);
            osThreadExit();
          }
          current_power_state  = LAST_ENUM_POWER_STATE;
          ulp_adc_current_mode = SL_ULP_ADC_PROCESS_ACTION;
        } else {
          sl_si91x_adc_unregister_event_callback();
          status = sl_si91x_adc_stop(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_stop: Error Code : %lu \r\n", status);
          }
          SL_PRINT_STRING_ERROR("ADC stopped successfully \r\n");
          status = sl_si91x_adc_deinit(adc_runtime_config);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_adc_deinit: Error Code : %lu \r\n", status);
          }
          SL_PRINT_STRING_ERROR("ADC deinit successfully \r\n");
          ulp_adc_current_mode = SL_ULP_ADC_TRANSMISSION_COMPLETED;
        }
        break;

      case SL_ULP_ADC_TRANSMISSION_COMPLETED:
      default:
        (void)osDelay(1000);
        break;
    }
  }
}
