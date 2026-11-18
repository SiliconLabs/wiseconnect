/***************************************************************************/ /**
 * @file ulp_i2s_freertos.c
 * @brief ULP I2S loopback example
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
#include "ulp_i2s_freertos.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_si91x_i2s.h"
#include "sl_si91x_i2s_config.h"
#include "sl_si91x_power_manager.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2S_LOWPOWER_BUFFER_SIZE   1024 // Transmit/Receive buffer size
#define ULP_I2S_INSTANCE           1    // I2S instance
#define I2S_ULP_BANK_OFFSET        0x800
#define I2S_TX_BUF_MEMORY          (ULP_SRAM_START_ADDR + (1 * I2S_ULP_BANK_OFFSET))
#define I2S_RX_BUF_MEMORY          (ULP_SRAM_START_ADDR + (2 * I2S_ULP_BANK_OFFSET))
#define FIVE_SECOND_DELAY_MS       5000U
#define ULP_I2S_POST_INIT_DELAY_MS 50U
/** Max time to wait for each of TX and RX DMA completion (prevents silent hang in `ulp_i2s_wait_transfer_done`). */
#define ULP_I2S_XFER_WAIT_MS 15000U

#if (ULP_I2S_INSTANCE == 1) && !defined(I2S1_LOOP_BACK)
#error \
  "This ULP I2S1 loopback example requires the preprocessor symbol I2S1_LOOP_BACK (see platform_siwx91x_ulp_i2s_freertos.slcp). Without it, sl_si91x_i2s.c disables WSCLK in the I2S1 ISR and the second completion event may never occur, so the task blocks forever in ulp_i2s_wait_transfer_done()."
#endif

/** PS transition events  */
#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint16_t i2s_lowpower_data_in[I2S_LOWPOWER_BUFFER_SIZE];
static uint16_t i2s_lowpower_data_out[I2S_LOWPOWER_BUFFER_SIZE];
static sl_i2s_handle_t i2s_driver_handle    = NULL;
static sl_i2s_xfer_config_t i2s_xfer_config = { 0 };
static sl_status_t status;

/** Binary semaphores: released from I2S callback on send/receive complete. */
static osSemaphoreId_t ulp_i2s_tx_sem;
static osSemaphoreId_t ulp_i2s_rx_sem;

/*******************************************************************************
 ******************************  Data Types  ***********************************
 ******************************************************************************/
typedef enum {
  SL_ULP_I2S_PROCESS_ACTION,
  SL_ULP_I2S_POWER_STATE_TRANSITION,
  SL_ULP_I2S_TRANSMISSION_COMPLETED,
} ulp_i2s_enum_t;

static void ulp_i2s_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);

static ulp_i2s_enum_t ulp_i2s_current_mode  = SL_ULP_I2S_PROCESS_ACTION;
static sl_power_state_t current_power_state = SL_SI91X_POWER_MANAGER_PS4;

/** Must outlive subscribe: PM stores pointers to these in a linked list. */
static sl_power_manager_ps_transition_event_handle_t s_ulp_i2s_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_i2s_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_i2s_pm_transition_callback,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t ulp_i2s_application_init(void);
static void ulp_i2s_freertos_task(void *argument);
static void callback_event(uint32_t event);
static void compare_loop_back_data(void);
static sl_status_t configuring_ps2_power_state(void);
static void ulp_i2s_xfer_sem_drain(void);
static bool ulp_i2s_wait_transfer_done(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);
static void ulp_i2s_deinit();

static const osThreadAttr_t ulp_i2s_thread_attributes = {
  .name       = "ulp_i2s",
  .stack_size = 10240,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void ulp_i2s_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_i2s_freertos_task, NULL, &ulp_i2s_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP I2S thread\r\n");
  }
}

/*******************************************************************************
 * Drop stray semaphore tokens before a new TX/RX window.
 ******************************************************************************/
static void ulp_i2s_xfer_sem_drain(void)
{
  if (ulp_i2s_tx_sem != NULL) {
    while (osSemaphoreAcquire(ulp_i2s_tx_sem, 0U) == osOK) {
      /* drain */
    }
  }
  if (ulp_i2s_rx_sem != NULL) {
    while (osSemaphoreAcquire(ulp_i2s_rx_sem, 0U) == osOK) {
      /* drain */
    }
  }
}

/*******************************************************************************
 * Block until send and receive callbacks have each signaled once.
 *
 ******************************************************************************/
static bool ulp_i2s_wait_transfer_done(void)
{
  osStatus_t w;
  sl_status_t pst;
  bool ps4_held = false;
  bool ok       = false;

  if ((ulp_i2s_tx_sem == NULL) || (ulp_i2s_rx_sem == NULL)) {
    return false;
  }
  if (current_power_state != SL_SI91X_POWER_MANAGER_PS2) {
    pst = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
    if (pst == SL_STATUS_OK) {
      ps4_held = true;
    } else {
      SL_PRINT_STRING_ERROR("ULP I2S: add PS4 for DMA wait failed: 0x%lx (continuing without hold)\r\n",
                            (unsigned long)pst);
    }
  }
  w = osSemaphoreAcquire(ulp_i2s_tx_sem, ULP_I2S_XFER_WAIT_MS);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR(
      "ULP I2S: TX completion wait failed (%d). Add I2S1_LOOP_BACK to compiler defines (see .slcp).\r\n",
      (int)w);
    goto cleanup;
  }
  w = osSemaphoreAcquire(ulp_i2s_rx_sem, ULP_I2S_XFER_WAIT_MS);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR(
      "ULP I2S: RX completion wait failed (%d); was tickless sleep starving ULP I2S? PS4 hold should fix.\r\n",
      (int)w);
    goto cleanup;
  }
  ok = true;

cleanup:
  if (ps4_held) {
    (void)sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  }
  return ok;
}

/*******************************************************************************
 * I2S example initialization (same sequence as ulp_i2s_example_init)
 ******************************************************************************/
static sl_status_t ulp_i2s_application_init(void)
{
  status                        = SL_STATUS_OK;
  i2s_xfer_config.mode          = SL_I2S_MASTER;
  i2s_xfer_config.protocol      = SL_I2S_PROTOCOL;
  i2s_xfer_config.resolution    = SL_ULP_I2S_RESOLUTION;
  i2s_xfer_config.sampling_rate = SL_ULP_I2S_SAMPLING_RATE;
  i2s_xfer_config.sync          = SL_I2S_ASYNC;
  i2s_xfer_config.data_size     = SL_I2S_DATA_SIZE16;

  for (uint32_t i = 0; i < I2S_LOWPOWER_BUFFER_SIZE; i++) {
    i2s_lowpower_data_out[i] = (uint16_t)i;
  }
  memcpy((uint16_t *)I2S_TX_BUF_MEMORY, i2s_lowpower_data_out, sizeof(i2s_lowpower_data_out));
  do {
    status = sl_si91x_i2s_init(ULP_I2S_INSTANCE, &i2s_driver_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S Initialization fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S Initialization success\r\n");
    status = sl_si91x_i2s_configure_power_mode(i2s_driver_handle, SL_I2S_FULL_POWER);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S power mode config fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S power mode config success\r\n");
    status = sl_si91x_i2s_register_event_callback(i2s_driver_handle, callback_event);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S user callback register fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S user callback register success\r\n");
    ulp_i2s_xfer_sem_drain();

    i2s_xfer_config.transfer_type = SL_I2S_TRANSMIT;
    status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S transmit config fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S transmit config success\r\n");
    i2s_xfer_config.transfer_type = SL_I2S_RECEIVE;
    status                        = sl_si91x_i2s_config_transmit_receive(i2s_driver_handle, &i2s_xfer_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S receive config fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S receive config success\r\n");
    status = sl_si91x_i2s_receive_data(i2s_driver_handle, (uint16_t *)I2S_RX_BUF_MEMORY, I2S_LOWPOWER_BUFFER_SIZE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S receive start fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S receive start success\r\n");
    status = sl_si91x_i2s_transmit_data(i2s_driver_handle, (uint16_t *)I2S_TX_BUF_MEMORY, I2S_LOWPOWER_BUFFER_SIZE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("I2S transmit start fail\r\n");
      break;
    }
    SL_PRINT_STRING_ERROR("I2S transmit start success\r\n");
  } while (false);
  return status;
}

/*******************************************************************************
 * Stop I2S callbacks then deinit (same teardown ordering concern as ULP ADC).
 ******************************************************************************/
static void ulp_i2s_deinit()
{
  if (i2s_driver_handle == NULL) {
    return;
  }
  (void)sl_si91x_i2s_unregister_event_callback(i2s_driver_handle);
  status = sl_si91x_i2s_deinit_v2(i2s_driver_handle);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_i2s_deinit failed, error code :%lx \r\n", (unsigned long)status);

  } else {
    SL_PRINT_STRING_ERROR("I2S deinitialized sucessfully\r\n");
  }
}

/*******************************************************************************
 * NWP deep sleep with retention is applied only after the first I2S DMA
 * completion (see `wireless_place_nwp_in_retention_after_first_i2s_xfer`):
 * putting the NWP in retention immediately after `sl_net_init` has been seen
 * to prevent ULP I2S UDMA completion on some setups.
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
  sl_status_t wst;
  sl_si91x_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr           = { 0 };

  wst = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start Wi-Fi Client interface: 0x%lx\r\n", wst);
    return wst;
  }
  wst = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (wst == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  } else {
    SL_PRINT_STRING_ERROR("Failed to get mac address: 0x%lx\r\n", wst);
  }
  wst = sl_si91x_get_firmware_version(&version);
  if (wst != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to fetch firmware version: 0x%lx\r\n", wst);
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

/*******************************************************************************
 * FreeRTOS task: loopback + PS4/PS2 (aligned with `ulp_adc_freertos.c`).
 ******************************************************************************/
static void ulp_i2s_freertos_task(void *argument)
{
  (void)argument;

  /* `osSemaphoreRelease` from I2S callback is ISR-safe in this CMSIS port. */
  ulp_i2s_tx_sem = osSemaphoreNew(1U, 0U, NULL);
  ulp_i2s_rx_sem = osSemaphoreNew(1U, 0U, NULL);
  if ((ulp_i2s_tx_sem == NULL) || (ulp_i2s_rx_sem == NULL)) {
    SL_PRINT_STRING_ERROR("Failed to create ULP I2S transfer semaphores\r\n");
    osThreadExit();
  }

  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless API initialization failed, Error Code: 0x%lX\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized\r\n");

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_i2s_pm_event_handle, &s_ulp_i2s_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Power Manager transition event subscription failed, Error Code: 0x%lX\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");
  }

  /* Do not call DEBUGINIT() here: it re-bases the debug UART for a new clock
   * context and often prints garbage if run before the first I2S wait. Keep
   * PS4 bracketing inside ulp_i2s_wait_transfer_done() so tickless idle does
   * not sleep during DMA. */

  status = ulp_i2s_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP I2S application init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  while (1) {
    switch (ulp_i2s_current_mode) {
      case SL_ULP_I2S_PROCESS_ACTION:
        if (!ulp_i2s_wait_transfer_done()) {
          SL_PRINT_STRING_ERROR("ULP I2S: stopping after transfer wait failure.\r\n");
          ulp_i2s_current_mode = SL_ULP_I2S_TRANSMISSION_COMPLETED;
          break;
        }
        // wireless_place_nwp_in_retention_after_first_i2s_xfer();
        memcpy(i2s_lowpower_data_in, (uint16_t *)I2S_RX_BUF_MEMORY, sizeof(i2s_lowpower_data_in));
        if ((sl_si91x_i2s_get_transmit_data_count(i2s_driver_handle) == I2S_LOWPOWER_BUFFER_SIZE)
            && (sl_si91x_i2s_get_receive_data_count(i2s_driver_handle) == I2S_LOWPOWER_BUFFER_SIZE)) {
          SL_PRINT_STRING_ERROR("I2S transfer complete\r\n");
          compare_loop_back_data();
        }
        ulp_i2s_current_mode = SL_ULP_I2S_POWER_STATE_TRANSITION;
        break;

      case SL_ULP_I2S_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("Switching i2s from PS4->PS2 state \r\n");
          ulp_i2s_deinit();
          while (!sl_si91x_power_manager_ps2_pre_check()) {
            /* Wait until PS2 is safe to enter (same as ulp_adc_freertos_9). */
            (void)osDelay(5);
          }

          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", status);
            break;
          }
          DEBUGINIT();

          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2S: configuring_ps2_power_state failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }

          status = ulp_i2s_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2S application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state  = SL_SI91X_POWER_MANAGER_PS2;
          ulp_i2s_current_mode = SL_ULP_I2S_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("Switching the i2s from PS2->PS4 state\r\n");
          ulp_i2s_deinit();
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement: Error Code : %lu \r\n", status);
            break;
          }
          DEBUGINIT();

          status = ulp_i2s_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2S application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state  = LAST_ENUM_POWER_STATE;
          ulp_i2s_current_mode = SL_ULP_I2S_PROCESS_ACTION;
        } else {
          ulp_i2s_deinit();
          SL_PRINT_STRING_ERROR("I2S stopped and deinitialized \r\n");
          ulp_i2s_current_mode = SL_ULP_I2S_TRANSMISSION_COMPLETED;
        }
        break;

      case SL_ULP_I2S_TRANSMISSION_COMPLETED:
      default:
        (void)osDelay(1000);
        break;
    }
  }
}

/*******************************************************************************
 * Function to compare loopback data
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < I2S_LOWPOWER_BUFFER_SIZE; data_index++) {
    if (i2s_lowpower_data_in[data_index] != i2s_lowpower_data_out[data_index]) {
      break;
    }
  }
  if (data_index == I2S_LOWPOWER_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("Data comparison successful, Loop Back Test Passed \n");
  } else {
    SL_PRINT_STRING_ERROR("Data comparison failed, Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * Callback event function
 *
 * Runs from interrupt / DMA completion context. Do not call SL_PRINT_STRING_ERROR() here:
 * it will interleave with task-level prints and corrupt the UART (random
 * 0x7F / 0x01 / control bytes right after "Waiting for I2Sâ€¦"). Signal the
 * task with semaphores only; log from the task after osSemaphoreAcquire returns.
 ******************************************************************************/
static void callback_event(uint32_t event)
{
  switch (event) {
    case SL_I2S_SEND_COMPLETE:
      if (ulp_i2s_tx_sem != NULL) {
        (void)osSemaphoreRelease(ulp_i2s_tx_sem);
      }
      break;
    case SL_I2S_RECEIVE_COMPLETE:
      if (ulp_i2s_rx_sem != NULL) {
        (void)osSemaphoreRelease(ulp_i2s_rx_sem);
      }
      break;
    case SL_I2S_TX_UNDERFLOW:
    case SL_I2S_RX_OVERFLOW:
    case SL_I2S_FRAME_ERROR:
    default:
      break;
  }
}

/*******************************************************************************
 * PS2 peripheral gating and RAM retention
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;

  status                     = SL_STATUS_OK;
  config.configure_ram_banks = true;
  /* Sl_si91x ULP examples may power-down upper M4SS SRAM. That is unsafe here:
   * WiFi + FreeRTOS + SLI_SI91X_MCU_ENABLE_RAM_BASED_EXECUTION typically place code/stack/heap in
   * those banks. RSI_PS_M4ssRamBanksPowerDown() then powers off the memory the CPU is executing
   * from â€” lockup before the next instruction (no second SL_PRINT_STRING_ERROR in sli_si91x_power_manager.c).
   * Keep m4ss_ram_banks == 0 so no M4SS bank is powered down; retention is still set via
   * get_ram_retention_mode(~0, ...). For minimum PS2 current, use a linker map that keeps all
   * executable RAM in banks you are allowed to shut down, then restore a non-zero mask. */
  config.m4ss_ram_banks  = 0;
  config.ulpss_ram_banks = 0;
  peri.m4ss_peripheral   = 0;
  peri.ulpss_peripheral  = SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
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
 * Power Manager PS transition callback (aligned with tickless idle / ULP ADC example).
 ******************************************************************************/
static void ulp_i2s_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
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
