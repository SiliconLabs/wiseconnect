/***************************************************************************/ /**
 * @file ulp_timer_freertos.c
 * @brief ULP Timer FreeRTOS
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
#include "ulp_timer_freertos.h"

#include <stdio.h>
#include <stdbool.h>
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"

#if defined(__has_include)
#if __has_include("sl_si91x_led.h")
#include "sl_si91x_led.h"
#include "sl_si91x_led_config.h"
#include "sl_si91x_led_instances.h"
#define ULP_TMR_LED_SIMPLE 1
#elif __has_include("sl_si91x_rgb_led.h")
#include "sl_si91x_rgb_led.h"
#include "sl_si91x_rgb_led_config.h"
#include "sl_si91x_rgb_led_instances.h"
#define ULP_TMR_LED_RGB 1
#endif
#else
#ifndef SI917_DEVKIT
#include "sl_si91x_led.h"
#include "sl_si91x_led_config.h"
#include "sl_si91x_led_instances.h"
#define ULP_TMR_LED_SIMPLE 1
#else
#include "sl_si91x_rgb_led.h"
#include "sl_si91x_rgb_led_config.h"
#include "sl_si91x_rgb_led_instances.h"
#define ULP_TMR_LED_RGB 1
#endif
#endif

#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif

#include "sl_ulp_timer_instances.h"
#include "sl_si91x_ulp_timer.h"
#include "sl_si91x_ulp_timer_common_config.h"
#include "sl_si91x_power_manager.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TIME_IN_MICROSECONDS 1000000U

#define ULP_TIMER_INSTANCE                   0
#define _CONCAT_TWO_TOKENS(token_1, token_2) token_1##token_2
#define CONCAT_TWO_TOKENS(token_1, token_2)  _CONCAT_TWO_TOKENS(token_1, token_2)
#define SL_ULP_TIMER_HANDLE                  CONCAT_TWO_TOKENS(sl_timer_handle_timer, ULP_TIMER_INSTANCE)

#define FIFTH_INTERRUPT_COUNT 5U
#define TENTH_INTERRUPT_COUNT 10U

/* Counting semaphore: ULP IRQ may run the callback more than once before the task runs; binary sem
 * would drop extra `GiveFromISR` and the task could block forever. */
#define ULP_TIMER_TICK_SEM_MAX 16U

#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 ***************************  Data Types  **************************************
 ******************************************************************************/
typedef enum {
  SL_ULP_TIMER_PROCESS_ACTION,
  SL_ULP_TIMER_POWER_STATE_TRANSITION,
  SL_ULP_TIMER_TRANSMISSION_COMPLETED,
} ulp_timer_enum_t;

/*******************************************************************************
 ***************************  Static Variables  ********************************
 ******************************************************************************/
static void ulp_timer_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);
static void on_timeout_callback0(void);

/*******************************************************************************
 ***************************  Local Variables  ********************************
 ******************************************************************************/
static volatile bool timer_parameter_change_flag    = false;
static volatile bool timer_callback_unregister_flag = false;
static volatile uint8_t interrupt_count             = 0;

static ulp_timer_enum_t ulp_timer_current_mode = SL_ULP_TIMER_PROCESS_ACTION;
static sl_power_state_t current_power_state    = SL_SI91X_POWER_MANAGER_PS4;
static bool ps4_to_ps2_transition_done         = false;
static bool ps2_to_ps4_transition_done         = false;
static uint32_t match_value;
static uint32_t timer_count_value;
static uint32_t timer_direction;

static osSemaphoreId_t ulp_timer_tick_sem;

static sl_power_manager_ps_transition_event_handle_t s_ulp_tmr_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_tmr_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_timer_pm_transition_callback,
};

/*******************************************************************************
 ***************************  Local Function Prototypes  ***********************
 ******************************************************************************/
static void ulp_timer_freertos_task(void *argument);
static sl_status_t ulp_timer_application_init(void);
static void ulp_timer_process_action_body(void);
static sl_status_t configuring_ps2_power_state(void);
static void ulp_timer_wait_tick(void);
static void ulp_timer_tick_sem_drain(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);

static const osThreadAttr_t ulp_timer_thread_attributes = {
  .name       = "ulp_tmr",
  .stack_size = 8192,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 *************************** task initialization  ***************************************
 ******************************************************************************/
void ulp_timer_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_timer_freertos_task, NULL, &ulp_timer_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP Timer thread\r\n");
  }
}

/*******************************************************************************
 * Timer callback function
 ******************************************************************************/
static void on_timeout_callback0(void)
{
  sl_status_t st;
  SL_PRINT_STRING_ERROR("On timer%d callback \n", ULP_TIMER_INSTANCE);
  interrupt_count++;

  if (interrupt_count == FIFTH_INTERRUPT_COUNT) {
    st = sl_si91x_ulp_timer_stop((ulp_timer_instance_t)ULP_TIMER_INSTANCE);
    if (st != SL_STATUS_OK) {
      return;
    }
    timer_parameter_change_flag = true;
  }
  if (interrupt_count == TENTH_INTERRUPT_COUNT) {
    timer_callback_unregister_flag = true;
    st                             = sl_si91x_ulp_timer_stop((ulp_timer_instance_t)ULP_TIMER_INSTANCE);
    if (st != SL_STATUS_OK) {
      return;
    }
  }

  if (ulp_timer_tick_sem != NULL) {
    (void)osSemaphoreRelease(ulp_timer_tick_sem);
  }

  /* After `osSemaphoreRelease` so a GPIO issue cannot prevent the task from waking. */
#if defined(ULP_TMR_LED_SIMPLE)
  (void)sl_si91x_led_toggle(LED_INSTANCE.pin);
#elif defined(ULP_TMR_LED_RGB)
  if (LED_INSTANCE.red != NULL) {
    (void)sl_si91x_rgb_led_toggle(LED_INSTANCE.red);
  }
#endif
}

/*******************************************************************************
 * Drain the semaphore
 ******************************************************************************/
static void ulp_timer_tick_sem_drain(void)
{
  if (ulp_timer_tick_sem == NULL) {
    return;
  }
  while (osSemaphoreAcquire(ulp_timer_tick_sem, 0U) == osOK) {
  }
}

/*******************************************************************************
 * Wait for the semaphore
 ******************************************************************************/
static void ulp_timer_wait_tick(void)
{
  osStatus_t w;
  if (ulp_timer_tick_sem == NULL) {
    return;
  }
  w = osSemaphoreAcquire(ulp_timer_tick_sem, osWaitForever);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR("osSemaphoreAcquire (ULP timer) error: %d\r\n", (int)w);
  }
}

/*******************************************************************************
 * Initialize the ULP timer
 ******************************************************************************/
static sl_status_t ulp_timer_application_init(void)
{
  sl_status_t status = SL_STATUS_OK;

  do {
    status = sl_si91x_ulp_timer_init(&sl_timer_clk_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_init : Invalid Parameters, Error Code : %lu \r\n",
                            (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully Configured ULP-timer clock input source \r\n");
    if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
      SL_PRINT_STRING_ERROR("Timer executing at PS4 state \r\n");
      status = sl_si91x_ulp_timer_get_match_value(SL_ULP_TIMER_HANDLE.timer_type, TIME_IN_MICROSECONDS, &match_value);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_match_value : Error Code : %lu \r\n", (unsigned long)status);
        break;
      }
      SL_PRINT_STRING_ERROR("Successfully match value is fetched\r\n");
      SL_ULP_TIMER_HANDLE.timer_match_value = match_value;
      status                                = sl_si91x_ulp_timer_set_configuration(&(SL_ULP_TIMER_HANDLE));
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_configuration : Error Code : %lu \r\n", (unsigned long)status);
        break;
      }
      SL_PRINT_STRING_ERROR("Successfully Configured ULP-timer parameters with mhz parameters \r\n");
    } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
      status = sl_si91x_ulp_timer_get_match_value(SL_ULP_TIMER_HANDLE.timer_type, TIME_IN_MICROSECONDS, &match_value);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_match_value : Error Code : %lu \r\n", (unsigned long)status);
        break;
      }
      SL_PRINT_STRING_ERROR("Successfully match value is fetched\r\n");
      SL_ULP_TIMER_HANDLE.timer_match_value = match_value;
      status                                = sl_si91x_ulp_timer_set_configuration(&(SL_ULP_TIMER_HANDLE));
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_configuration : Error Code : %lu \r\n", (unsigned long)status);
        break;
      }
      SL_PRINT_STRING_ERROR("Successfully Configured ULP-timer parameters with 20mhz parameters \r\n");
    }
    status =
      sl_si91x_ulp_timer_register_timeout_callback((ulp_timer_instance_t)ULP_TIMER_INSTANCE, on_timeout_callback0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_register_timeout_callback : Error Code : %lu \r\n",
                            (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully Registered timer instance timeout callback \r\n");
    ulp_timer_tick_sem_drain();
    status = sl_si91x_ulp_timer_start((ulp_timer_instance_t)ULP_TIMER_INSTANCE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_start : Error Code : %lu \r\n", (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully started ulp-timer instance with default parameters \r\n");

  } while (false);
  return status;
}

/*******************************************************************************
 * powering off the peripherals not in use,
 * Configuring power manager ram-retention
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;
  sl_status_t status = SL_STATUS_OK;

  peri.m4ss_peripheral  = 0;
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_AUX | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3;

  config.configure_ram_banks = true;
  config.m4ss_ram_banks      = 0;
  config.ulpss_ram_banks     = SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_2 | SL_SI91X_POWER_MANAGER_ULPSS_RAM_BANK_3;

  do {
    status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_remove_peripheral_requirement failed, Error Code: 0x%lX\r\n",
                            (unsigned long)status);
      break;
    }
    status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_configure_ram_retention failed, Error Code: 0x%lX\r\n",
                            (unsigned long)status);
      break;
    }
  } while (false);
  return status;
}

/*******************************************************************************
 * Power manager transition callback
 ******************************************************************************/
static void ulp_timer_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
{
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
    case SL_SI91X_POWER_MANAGER_SLEEP:
      SL_PRINT_STRING_ERROR("Leaving Sleep State \r\n");
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
    default:
      break;
  }
}

/*******************************************************************************
 * Process action body
 ******************************************************************************/
static void ulp_timer_process_action_body(void)
{
  sl_status_t status;

  switch (ulp_timer_current_mode) {
    case SL_ULP_TIMER_PROCESS_ACTION:
      if (timer_callback_unregister_flag) {
        ulp_timer_current_mode = SL_ULP_TIMER_POWER_STATE_TRANSITION;
      }
      if (timer_parameter_change_flag) {
        timer_parameter_change_flag = false;
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          status = sl_si91x_ulp_timer_set_type((ulp_timer_instance_t)ULP_TIMER_INSTANCE, ULP_TIMER_TYP_256US);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_type : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed timer-type to 256US mode \r\n");
          }
          status = sl_si91x_ulp_timer_set_direction((ulp_timer_instance_t)ULP_TIMER_INSTANCE, DOWN_COUNTER);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_direction : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed timer-direction to up-counter \r\n");
          }
          status = sl_si91x_ulp_timer_get_match_value(ULP_TIMER_TYP_256US, TIME_IN_MICROSECONDS, &match_value);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_match_value : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully match value is fetched\r\n");
          }
          status = sl_si91x_ulp_timer_set_count((ulp_timer_instance_t)ULP_TIMER_INSTANCE, match_value);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_count : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed the timer match value\r\n");
          }
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          status = sl_si91x_ulp_timer_set_type((ulp_timer_instance_t)ULP_TIMER_INSTANCE, ULP_TIMER_TYP_256US);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_type : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed timer-type to 256US mode \r\n");
          }
          status = sl_si91x_ulp_timer_set_direction((ulp_timer_instance_t)ULP_TIMER_INSTANCE, DOWN_COUNTER);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_direction : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed timer-direction to up-counter \r\n");
          }
          status = sl_si91x_ulp_timer_get_match_value(ULP_TIMER_TYP_256US, TIME_IN_MICROSECONDS, &match_value);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_match_value : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully match value is fetched\r\n");
          }
          status = sl_si91x_ulp_timer_set_count((ulp_timer_instance_t)ULP_TIMER_INSTANCE, match_value);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_count : Error Code : %lu \r\n", (unsigned long)status);
          } else {
            SL_PRINT_STRING_ERROR("Successfully changed the timer match value\r\n");
          }
        }
        status = sl_si91x_ulp_timer_start((ulp_timer_instance_t)ULP_TIMER_INSTANCE);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_start : Error Code : %lu \r\n", (unsigned long)status);
        } else {
          SL_PRINT_STRING_ERROR("Successfully started timer instance with new parameters \r\n");
        }
        status = sl_si91x_ulp_timer_get_count((ulp_timer_instance_t)ULP_TIMER_INSTANCE, &timer_count_value);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_count : Error Code : %lu \r\n", (unsigned long)status);
        } else {
          SL_PRINT_STRING_ERROR("Timer count value = %lu, after starting timer\r\n", (unsigned long)timer_count_value);
        }
        status = sl_si91x_ulp_timer_get_direction((ulp_timer_instance_t)ULP_TIMER_INSTANCE, &timer_direction);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_get_direction : Error Code : %lu \r\n", (unsigned long)status);
        } else {
          if (timer_direction) {
            SL_PRINT_STRING_ERROR("Timer running as up-counter\r\n");
          } else {
            SL_PRINT_STRING_ERROR("Timer running as down-counter\r\n");
          }
        }
      }
      break;

    case SL_ULP_TIMER_POWER_STATE_TRANSITION:
      if (interrupt_count == TENTH_INTERRUPT_COUNT) {
        interrupt_count             = FIFTH_INTERRUPT_COUNT;
        timer_parameter_change_flag = true;
        if (ps4_to_ps2_transition_done && ps2_to_ps4_transition_done) {
          sl_si91x_ulp_timer_deinit();
          SL_PRINT_STRING_ERROR("De Initialization is successful\r\n");
          ps2_to_ps4_transition_done = false;
          ps4_to_ps2_transition_done = false;
          ulp_timer_current_mode     = SL_ULP_TIMER_TRANSMISSION_COMPLETED;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("Switching power state from PS4 to PS2\r\n");
          // sl_si91x_wireless_shutdown();
          while (!sl_si91x_power_manager_ps2_pre_check()) {
            (void)osDelay(5);
          }
          status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          DEBUGINIT();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP timer: configuring_ps2_power_state failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          ps4_to_ps2_transition_done = true;
          current_power_state        = SL_SI91X_POWER_MANAGER_PS2;
          ulp_timer_current_mode     = SL_ULP_TIMER_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("Switching power state from PS2 to PS4\r\n");
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          DEBUGINIT();
          ps2_to_ps4_transition_done = true;
          current_power_state        = SL_SI91X_POWER_MANAGER_PS4;
          ulp_timer_current_mode     = SL_ULP_TIMER_PROCESS_ACTION;
        }

        timer_callback_unregister_flag = false;
      }
      break;

    case SL_ULP_TIMER_TRANSMISSION_COMPLETED:
      current_power_state = LAST_ENUM_POWER_STATE;
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Initialize the wireless
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
                     .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
  };
  sl_status_t st;
  sl_wifi_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr          = { 0 };

  st = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start Wi-Fi Client interface: 0x%lx\r\n", (unsigned long)st);
    return st;
  }
  st = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (st == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  }
  st = sl_wifi_get_firmware_version(&version);
  if (st == SL_STATUS_OK) {
    print_firmware_version(&version);
  }
  wireless_sleep(true);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Set the wireless sleep mode
 ******************************************************************************/
static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_status_t st;
  sl_wifi_performance_profile_v2_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  st = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (st != SL_STATUS_OK) {
    return;
  }
  ta_performance_profile.profile = sleep_with_retention ? DEEP_SLEEP_WITH_RAM_RETENTION
                                                        : DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  (void)sl_wifi_set_performance_profile_v2(&ta_performance_profile);
}

/*******************************************************************************
 * ULP timer freertos task
 ******************************************************************************/
static void ulp_timer_freertos_task(void *argument)
{
  (void)argument;
  sl_status_t status = SL_STATUS_OK;
  ulp_timer_tick_sem = osSemaphoreNew(ULP_TIMER_TICK_SEM_MAX, 0U, NULL);
  if (ulp_timer_tick_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP Timer semaphore\r\n");
    osThreadExit();
  }

  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized; NWP in sleep with RAM retention\r\n");

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_tmr_pm_event_handle, &s_ulp_tmr_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PM subscribe failed: 0x%lX\r\n", (unsigned long)status);
  } else {
    SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");
  }
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", (unsigned long)status);
    osThreadExit();
  }
  status = ulp_timer_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP timer application init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  while (1) {
    /* Block for next tick except when PROCESS must run immediately (parameter / PS handoff). */
    if (ulp_timer_current_mode == SL_ULP_TIMER_PROCESS_ACTION) {
      if (!timer_parameter_change_flag && !timer_callback_unregister_flag) {
        ulp_timer_wait_tick();
      }
    }
    ulp_timer_process_action_body();
    if (ulp_timer_current_mode == SL_ULP_TIMER_TRANSMISSION_COMPLETED) {
      (void)osDelay(1000);
    }
  }
}
