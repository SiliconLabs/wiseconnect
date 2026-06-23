/***************************************************************************/
/**
 * @file ulp_uart_freertos.c
 * @brief ULP UART FreeRTOS
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
#include "ulp_uart_freertos.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "rsi_egpio.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_wifi.h"
#include "sl_si91x_usart.h"
#include "sl_si91x_power_manager.h"
#include "sl_si91x_clock_manager.h"
#include "sl_status.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
/** Loopback TX/RX buffer length (bytes); must fit ULP SRAM layout below. */
#define ULP_UART_BUFFER_SIZE 1024
#define ULP_BANK_OFFSET      0x800
#define TX_BUF_MEMORY        (ULP_SRAM_START_ADDR + (1 * ULP_BANK_OFFSET))
#define RX_BUF_MEMORY        (ULP_SRAM_START_ADDR + (2 * ULP_BANK_OFFSET))
#define MINIMUM_COUNT_VALUE  0  // initiating the minimum count value as zero
#define MAXIMUM_COUNT_VALUE  10 // maximum comparison pin triggering for reference

/** ULP GPIO (port 4): toggle pin after successful loopback compare. */
#define ULP_GPIO_PORT   4
#define ULP_GPIO_PIN    2
#define ULP_GPIO_TOGGLE 8
#define OUTPUT_VALUE    1
#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint8_t ulp_uart_data_in[ULP_UART_BUFFER_SIZE];
static uint8_t ulp_uart_data_out[ULP_UART_BUFFER_SIZE];
static sl_usart_handle_t usart_handle = NULL;

static osSemaphoreId_t ulp_uart_tx_sem;
static osSemaphoreId_t ulp_uart_rx_sem;

static sl_gpio_t ulp_gpio_toggle = { ULP_GPIO_PORT, ULP_GPIO_TOGGLE };

typedef enum {
  SL_ULP_UART_PROCESS_ACTION,
  SL_ULP_UART_POWER_STATE_TRANSITION,
  SL_ULP_UART_TRANSMISSION_COMPLETED,
} ulp_uart_enum_t;

static void ulp_uart_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);

static ulp_uart_enum_t ulp_uart_current_mode = SL_ULP_UART_PROCESS_ACTION;
static sl_power_state_t current_power_state  = SL_SI91X_POWER_MANAGER_PS4;

static sl_power_manager_ps_transition_event_handle_t s_ulp_uart_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_uart_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_uart_pm_transition_callback,
};

static void ulp_uart_freertos_task(void *argument);
static sl_status_t ulp_uart_gpio_init_once(void);
static sl_status_t ulp_uart_application_init(void);
static void ulp_uart_xfer_sem_drain(void);

static sl_status_t ulp_uart_wait_one_sem(osSemaphoreId_t sem);
static sl_status_t ulp_uart_wait_tx_done(void);
static sl_status_t ulp_uart_wait_rx_done(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);

static void ulp_uart_teardown(void);
static void ulp_uart_on_usart_event(uint32_t event);
static void compare_loop_back_data(void);
static sl_status_t configuring_ps2_power_state(void);
static sl_si91x_usart_control_config_t s_usart_config = {
  .baudrate      = 115200,
  .mode          = SL_USART_MODE_ASYNCHRONOUS,
  .parity        = SL_USART_NO_PARITY,
  .stopbits      = SL_USART_STOP_BITS_1,
  .hwflowcontrol = SL_USART_FLOW_CONTROL_NONE,
  .databits      = SL_USART_DATA_BITS_8,
  .misc_control  = SL_USART_MISC_CONTROL_NONE,
  .usart_module  = ULPUART,
  .config_enable = ENABLE,
  .synch_mode    = DISABLE,
};

static const osThreadAttr_t ulp_uart_thread_attributes = {
  .name       = "ulp_uart",
  .stack_size = 10240,
  .priority   = osPriorityLow1,
};

/** Loop counter for **SLI_ULP_UART_USE_SEND** + **SLI_ULP_UART_USE_RECEIVE**. */
static uint32_t ulp_uart_count = MINIMUM_COUNT_VALUE;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void ulp_uart_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_uart_freertos_task, NULL, &ulp_uart_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP UART thread\r\n");
  }
}

/*******************************************************************************
 * ULP GPIO setup for the diagnostic toggle pin (output, MUX mode 0).
 ******************************************************************************/
static sl_status_t ulp_uart_gpio_init_once(void)
{
  static bool s_gpio_done;
  sl_status_t st;

  if (s_gpio_done) {
    return SL_STATUS_OK;
  }

  st = sl_gpio_driver_set_pin_mode(&ulp_gpio_toggle, (sl_gpio_mode_t)EGPIO_PIN_MUX_MODE0, OUTPUT_VALUE);
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP UART: sl_gpio_driver_set_pin_mode failed: 0x%lx\r\n", (unsigned long)st);
    return st;
  }
  st = sl_si91x_gpio_driver_set_pin_direction(ULP_GPIO_PORT, ULP_GPIO_TOGGLE, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP UART: set_pin_direction failed: 0x%lx\r\n", (unsigned long)st);
    return st;
  }
  s_gpio_done = true;
  return SL_STATUS_OK;
}

/*******************************************************************************
 ***************************  Functions  ***************************************
 ******************************************************************************/

/***************************************************************************************
 ***************************** ULP UART Xfer Sem Drain ********************************
 *
 * @brief Drains the ULP UART semaphore.
 * @return None
 ****************************************************************************************/
static void ulp_uart_xfer_sem_drain(void)
{
  if (ulp_uart_tx_sem != NULL) {
    while (osSemaphoreAcquire(ulp_uart_tx_sem, 0U) == osOK) {
    }
  }
  if (ulp_uart_rx_sem != NULL) {
    while (osSemaphoreAcquire(ulp_uart_rx_sem, 0U) == osOK) {
    }
  }
}

/*******************************************************************************
 * Wait for one async completion. Hold PS4 so tickless idle does not enter M4
 * sleep during ULP USART DMA (same issue as ULP I2S).
 ******************************************************************************/
static sl_status_t ulp_uart_wait_one_sem(osSemaphoreId_t sem)
{
  osStatus_t w;
  sl_status_t pst;
  bool ps4_held      = false;
  sl_status_t result = SL_STATUS_FAIL;

  if (sem == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (current_power_state != SL_SI91X_POWER_MANAGER_PS2) {
    pst = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
    if (pst == SL_STATUS_OK) {
      ps4_held = true;
    } else {
      SL_PRINT_STRING_ERROR("ULP UART: add PS4 for transfer wait failed: 0x%lx (continuing)\r\n", (unsigned long)pst);
    }
  }
  w = osSemaphoreAcquire(sem, osWaitForever);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR("ULP UART: completion wait failed (%d)\r\n", (int)w);
    goto cleanup;
  }
  result = SL_STATUS_OK;

cleanup:
  if (ps4_held) {
    (void)sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  }
  return result;
}

static sl_status_t ulp_uart_wait_tx_done(void)
{
  return ulp_uart_wait_one_sem(ulp_uart_tx_sem);
}

static sl_status_t ulp_uart_wait_rx_done(void)
{
  return ulp_uart_wait_one_sem(ulp_uart_rx_sem);
}

/*******************************************************************************
 * USART init + callback registration only (transfers run in the task loop so
 * **SLI_ULP_UART_USE_SEND** / **SLI_ULP_UART_USE_RECEIVE**).
 ******************************************************************************/
static sl_status_t ulp_uart_application_init(void)
{
  sl_si91x_usart_control_config_t get_config;
  sl_status_t status = SL_STATUS_OK;

  do {
    status = sl_si91x_usart_init(ULPUART, &usart_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_init: Error Code : 0x%lX \r\n", (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("USART initialization is successful \r\n");
    status = sl_si91x_usart_set_configuration(usart_handle, &s_usart_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_set_configuration: Error Code : 0x%lX \r\n", (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("USART configuration is successful \r\n");
    status = sl_si91x_usart_multiple_instance_register_event_callback(ULPUART, ulp_uart_on_usart_event);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_multiple_instance_register_event_callback: Error Code : 0x%lX \r\n",
                            (unsigned long)status);
      break;
    }
    SL_PRINT_STRING_ERROR("USART user event callback registered successfully \r\n");
    (void)sl_si91x_usart_get_configurations(ULPUART, &get_config);
    SL_PRINT_STRING_ERROR("Baud Rate = %ld \r\n", (long)get_config.baudrate);

    ulp_uart_xfer_sem_drain();

  } while (false);
  return status;
}

static void ulp_uart_teardown(void)
{
  sl_status_t status = SL_STATUS_OK;
  if (usart_handle == NULL) {
    return;
  }
  sl_si91x_usart_multiple_instance_unregister_event_callback(ULPUART);
  status = sl_si91x_usart_deinit(usart_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_usart_deinit failed: Error Code : 0x%lX\r\n", (unsigned long)status);

  } else {
    SL_PRINT_STRING_ERROR("ULP UART de-initialized successfully \r\n");
  }
  usart_handle = NULL;
}

/*******************************************************************************
 * Wireless bring-up (no NWP sleep here â€” deferred until first UART completion).
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
  sl_status_t wst;
  sl_wifi_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr          = { 0 };

  wst = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start Wi-Fi Client interface: 0x%lx\r\n", wst);
    return wst;
  }
  wst = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (wst == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:\r\n", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x:\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  } else {
    SL_PRINT_STRING_ERROR("Failed to get mac address: 0x%lx\r\n", wst);
  }
  wst = sl_wifi_get_firmware_version(&version);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to fetch firmware version: 0x%lx\r\n", wst);
  } else {
    print_firmware_version(&version);
  }
  wireless_sleep(true);

  return SL_STATUS_OK;
}

static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_status_t wst;
  sl_wifi_performance_profile_v2_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  wst = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", wst);
    return;
  }
  if (sleep_with_retention) {
    ta_performance_profile.profile = DEEP_SLEEP_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  }
  wst = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", wst);
  }
}

static void ulp_uart_freertos_task(void *argument)
{
  (void)argument;
  sl_status_t status = SL_STATUS_OK;
  ulp_uart_tx_sem    = osSemaphoreNew(1U, 0U, NULL);
  ulp_uart_rx_sem    = osSemaphoreNew(1U, 0U, NULL);
  if ((ulp_uart_tx_sem == NULL) || (ulp_uart_rx_sem == NULL)) {
    SL_PRINT_STRING_ERROR("Failed to create ULP UART transfer semaphores\r\n");
    osThreadExit();
  }

  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless API initialization failed, Error Code: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_uart_pm_event_handle, &s_ulp_uart_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Power Manager transition event subscription failed, Error Code: 0x%lX\r\n",
                          (unsigned long)status);
  } else {
    SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");
  }

  status = ulp_uart_gpio_init_once();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP UART: GPIO init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  status = ulp_uart_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP UART: application init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  while (1) {
    switch (ulp_uart_current_mode) {
      case SL_ULP_UART_PROCESS_ACTION: {
        bool xfer_ok = true;

        for (;;) {
          for (uint16_t i = 0; i < ULP_UART_BUFFER_SIZE; i++) {
            ulp_uart_data_out[i] = (uint8_t)(i + 1U);
          }
          memcpy((uint8_t *)TX_BUF_MEMORY, ulp_uart_data_out, sizeof(ulp_uart_data_out));

          ulp_uart_xfer_sem_drain();

          status = sl_si91x_usart_send_data(usart_handle, (uint8_t *)TX_BUF_MEMORY, sizeof(ulp_uart_data_out));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_usart_send_data: Error Code : 0x%lX \r\n", (unsigned long)status);
            xfer_ok = false;
            break;
          }
          if (ulp_uart_wait_tx_done() != SL_STATUS_OK) {
            xfer_ok = false;
            break;
          }

#if SLI_ULP_UART_USE_RECEIVE
          status = sl_si91x_usart_receive_data(usart_handle, (uint8_t *)RX_BUF_MEMORY, sizeof(ulp_uart_data_in));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_usart_receive_data: Error Code : 0x%lX \r\n", (unsigned long)status);
            xfer_ok = false;
            break;
          }
          if (ulp_uart_wait_rx_done() != SL_STATUS_OK) {
            xfer_ok = false;
            break;
          }
          memcpy(ulp_uart_data_in, (uint8_t *)RX_BUF_MEMORY, sizeof(ulp_uart_data_in));
          compare_loop_back_data();
#if SLI_ULP_UART_USE_SEND
          if (ulp_uart_count <= (uint32_t)MAXIMUM_COUNT_VALUE) {
            ulp_uart_count++;
            continue;
          }
#endif
#endif /* SLI_ULP_UART_USE_RECEIVE */
          break;
        }

        if (!xfer_ok) {
          SL_PRINT_STRING_ERROR("ULP UART: stopping after transfer failure\r\n");
          ulp_uart_xfer_sem_drain();
          ulp_uart_teardown();
          ulp_uart_current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
          break;
        }
        ulp_uart_current_mode = SL_ULP_UART_POWER_STATE_TRANSITION;
        break;
      }

      case SL_ULP_UART_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("Switching ULP UART from PS4->PS2 state \r\n");
          ulp_uart_teardown();

          while (!sl_si91x_power_manager_ps2_pre_check()) {
            (void)osDelay(5);
          }

          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          DEBUGINIT();

          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP UART: configuring_ps2_power_state failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }

          sl_si91x_delay_ms(1000);
          ulp_uart_count = MINIMUM_COUNT_VALUE;
          status         = ulp_uart_gpio_init_once();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP UART: GPIO init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          status = ulp_uart_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP UART: application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state   = SL_SI91X_POWER_MANAGER_PS2;
          ulp_uart_current_mode = SL_ULP_UART_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("Switching ULP UART from PS2->PS4 state\r\n");
          ulp_uart_teardown();
          sl_si91x_delay_ms(1000);
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          DEBUGINIT();

          ulp_uart_count = MINIMUM_COUNT_VALUE;
          status         = ulp_uart_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP UART: application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state   = LAST_ENUM_POWER_STATE;
          ulp_uart_current_mode = SL_ULP_UART_PROCESS_ACTION;
        } else {
          ulp_uart_teardown();
          SL_PRINT_STRING_ERROR("ULP UART stopped and de-initialized \r\n");
          ulp_uart_current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
        }
        break;

      case SL_ULP_UART_TRANSMISSION_COMPLETED:
      default:
        (void)osDelay(1000);
        break;
    }
  }
}

/*******************************************************************************
 * Compare loopback RX vs TX buffers; on full match, pulse the ULP GPIO toggle pin.
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index;
  for (data_index = 0; data_index < ULP_UART_BUFFER_SIZE; data_index++) {
    if (ulp_uart_data_in[data_index] != ulp_uart_data_out[data_index]) {
      break;
    }
  }
  if (data_index == ULP_UART_BUFFER_SIZE) {
    (void)sl_gpio_driver_set_pin(&ulp_gpio_toggle);
    (void)sl_gpio_driver_clear_pin(&ulp_gpio_toggle);
    SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed \n");
  } else {
    SL_PRINT_STRING_ERROR("Data comparison failed, Loop Back Test failed \n");
  }
}

static void ulp_uart_on_usart_event(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      if (ulp_uart_tx_sem != NULL) {
        (void)osSemaphoreRelease(ulp_uart_tx_sem);
      }
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      if (ulp_uart_rx_sem != NULL) {
        (void)osSemaphoreRelease(ulp_uart_rx_sem);
      }
      break;
    case SL_USART_EVENT_TX_UNDERFLOW:
      SL_PRINT_STRING_ERROR("ULP UART: TX underflow\r\n");
      break;
    case SL_USART_EVENT_RX_OVERFLOW:
      SL_PRINT_STRING_ERROR("ULP UART: RX overflow\r\n");
      break;
    default:
      SL_PRINT_STRING_ERROR("ULP UART: unexpected event 0x%lx\r\n", (unsigned long)event);
      break;
  }
}

/*******************************************************************************
 * PS2 entry: remove ULPSS/NPSS peripheral power requirements and configure
 * M4SS RAM retention (banks 9 and 10) for this example's low-power path.
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;
  sl_status_t status = SL_STATUS_OK;

  config.configure_ram_banks = true;

  config.m4ss_ram_banks = SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_9 | SL_SI91X_POWER_MANAGER_M4SS_RAM_BANK_10;

  peri.m4ss_peripheral  = 0;
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM | SL_SI91X_POWER_MANAGER_ULPSS_PG_AUX;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCURTC | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2 | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_TIMEPERIOD;
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
    SL_PRINT_STRING_ERROR("PS2: configuring_ps2_power_state (done)\r\n");

  } while (false);
  return status;
}

static void ulp_uart_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
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
