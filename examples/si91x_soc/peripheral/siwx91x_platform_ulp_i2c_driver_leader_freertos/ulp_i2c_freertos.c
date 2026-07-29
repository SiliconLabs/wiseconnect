/***************************************************************************/ /**
 * @file ulp_i2c_freertos.c
 * @brief ULP I2C examle running under FreeRTOS on the SiWx91x SoC.
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
#include "ulp_i2c_freertos.h"

#include <stdio.h>
#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_i2c_instances.h"
#include "sl_si91x_i2c.h"
#include "sl_si91x_peripheral_i2c.h"
#include "sl_si91x_power_manager.h"
#include "sl_status.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define I2C_INSTANCE_USED        2
#define FOLLOWER_I2C_ADDR        0x50
#define I2C_BUFFER_SIZE          1024U
#define I2C_TX_FIFO_THRESHOLD    0U
#define I2C_RX_FIFO_THRESHOLD    0U
#define BUFFER_OFFSET            0x1U
#define FOLLOWER_RESET_WINDOW_MS 5000U

#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

typedef enum {
  SL_ULP_I2C_PROCESS_ACTION,
  SL_ULP_I2C_POWER_STATE_TRANSITION,
  SL_ULP_I2C_TRANSMISSION_COMPLETED,
} ulp_i2c_enum_t;

/*******************************************************************************
 ***************************  Static Variables  ********************************
 ******************************************************************************/
static void ulp_i2c_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);

static ulp_i2c_enum_t ulp_i2c_current_mode  = SL_ULP_I2C_PROCESS_ACTION;
static sl_power_state_t current_power_state = SL_SI91X_POWER_MANAGER_PS4;
static sl_i2c_instance_t i2c_instance       = I2C_INSTANCE_USED;
static uint8_t i2c_read_buffer[I2C_BUFFER_SIZE];
static uint8_t i2c_write_buffer[I2C_BUFFER_SIZE];
static sl_i2c_config_t i2c_runtime_config;

static sl_power_manager_ps_transition_event_handle_t s_ulp_i2c_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_i2c_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_i2c_pm_transition_callback,
};
/*******************************************************************************
 ***************************  Static Functions  ********************************
 ******************************************************************************/
static void ulp_i2c_freertos_task(void *argument);
static sl_status_t ulp_i2c_application_init(void);
static void ulp_i2c_run_send_receive_compare(void);
static sl_status_t configuring_ps2_power_state(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);

static const osThreadAttr_t ulp_i2c_thread_attributes = {
  .name       = "ulp_i2c",
  .stack_size = 8192,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 ***************************  Functions  ***************************************
 ******************************************************************************/

/***************************************************************************************
 ***************************** ULP I2C Example Init ********************************
 *
 * @brief Creates the ULP I2C thread running under FreeRTOS.
 * @return None   
 ****************************************************************************************/
void ulp_i2c_leader_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_i2c_freertos_task, NULL, &ulp_i2c_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP I2C thread\r\n");
  }
}

/***************************************************************************************
 ***************************** ULP I2C Application Init ********************************
 *
 * @brief Initializes the ULP I2C application.
 * @return None   
 ****************************************************************************************/
static sl_status_t ulp_i2c_application_init(void)
{
  sl_i2c_status_t ist;

  i2c_runtime_config = sl_i2c_i2c2_config;
  ist                = sl_i2c_driver_init(i2c_instance, &i2c_runtime_config);
  if (ist != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_init: %u\r\n", (unsigned int)ist);
    return SL_STATUS_FAIL;
  }
  ist = sl_i2c_driver_configure_fifo_threshold(i2c_instance, I2C_TX_FIFO_THRESHOLD, I2C_RX_FIFO_THRESHOLD);
  if (ist != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("sl_i2c_driver_configure_fifo_threshold: %u\r\n", (unsigned int)ist);
    return SL_STATUS_FAIL;
  }
  for (uint32_t i = 0; i < I2C_BUFFER_SIZE; i++) {
    i2c_write_buffer[i] = (uint8_t)(i + BUFFER_OFFSET);
  }
  SL_PRINT_STRING_ERROR("ULP I2C leader initialized successfully (instance %d)\r\n", (int)i2c_instance);
  return SL_STATUS_OK;
}

/***************************************************************************************
 ***************************** ULP I2C Run Send Receive Compare ********************************
 *
 * @brief Runs the ULP I2C send and receive compare.
 * @return None   
 ****************************************************************************************/
static void ulp_i2c_run_send_receive_compare(void)
{
  sl_i2c_status_t st;

  st = sl_i2c_driver_send_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_write_buffer, I2C_BUFFER_SIZE);
  if (st != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("send_data_blocking: %u\r\n", (unsigned int)st);
    return;
  }
  st = sl_i2c_driver_receive_data_blocking(i2c_instance, FOLLOWER_I2C_ADDR, i2c_read_buffer, I2C_BUFFER_SIZE);
  if (st != SL_I2C_SUCCESS) {
    SL_PRINT_STRING_ERROR("receive_data_blocking: %u\r\n", (unsigned int)st);
    return;
  }
  uint32_t idx;
  for (idx = 0; idx < I2C_BUFFER_SIZE; idx++) {
    if (i2c_write_buffer[idx] != i2c_read_buffer[idx]) {
      break;
    }
  }
  if (idx == I2C_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("ULP I2C compare: PASS\r\n");
  } else {
    SL_PRINT_STRING_ERROR("ULP I2C compare: FAIL at %lu\r\n", (unsigned long)idx);
  }
}

/***************************************************************************************
 ***************************** Configuring PS2 Power State ********************************
 *
 * @brief Configures the PS2 power state.
 * @return None   
 ****************************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_status_t st;
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;

  st                         = SL_STATUS_OK;
  config.configure_ram_banks = true;
  /* Wi‑Fi + FreeRTOS: keep M4SS banks powered (same rationale as ULP ADC FreeRTOS). */
  config.m4ss_ram_banks  = 0;
  config.ulpss_ram_banks = 0;
  peri.m4ss_peripheral   = SL_SI91X_POWER_MANAGER_M4SS_PG_QSPI | SL_SI91X_POWER_MANAGER_M4SS_PG_EFUSE
                         | SL_SI91X_POWER_MANAGER_M4SS_PG_SDIO_SPI;
  /* Match `ulp_i2c_leader_example.c`: do not remove ULPSS_PG_I2C while I2C is in use. */
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM | SL_SI91X_POWER_MANAGER_ULPSS_PG_AUX;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3;
  do {
    st = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (st != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("remove_peripheral_requirement: 0x%lX\r\n", (unsigned long)st);
      break;
    }
    st = sl_si91x_power_manager_configure_ram_retention(&config);
    if (st != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("configure_ram_retention: 0x%lX\r\n", (unsigned long)st);
      break;
    }

  } while (false);
  return st;
}

/***************************************************************************************
 ***************************** ULP I2C PM Transition Callback ********************************
 *
 * @brief Handles the ULP I2C power manager transition callback.
 * @return None   
 ****************************************************************************************/
static void ulp_i2c_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
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

/***************************************************************************************
 ***************************** Initialize Wireless ********************************
 *
 * @brief Initializes the wireless.
 * @return None   
 ****************************************************************************************/
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
  sl_status_t st;
  sl_si91x_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr           = { 0 };

  st = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (st != SL_STATUS_OK) {
    return st;
  }
  if (sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr) == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  }
  if (sl_si91x_get_firmware_version(&version) == SL_STATUS_OK) {
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

/***************************************************************************************
 ***************************** Wireless Sleep ********************************
 *
 * @brief Puts the wireless into sleep.
 * @return None   
 ****************************************************************************************/
static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_wifi_performance_profile_v2_t p = { .profile = HIGH_PERFORMANCE };
  if (sl_wifi_set_performance_profile_v2(&p) != SL_STATUS_OK) {
    return;
  }
  p.profile = sleep_with_retention ? DEEP_SLEEP_WITH_RAM_RETENTION : DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  (void)sl_wifi_set_performance_profile_v2(&p);
}

/***************************************************************************************
 ***************************** ULP I2C FreeRTOS Task ********************************
 *
 * @brief The ULP I2C FreeRTOS task.
 * @return None   
 ****************************************************************************************/
static void ulp_i2c_freertos_task(void *argument)
{
  sl_status_t st;
  sl_i2c_status_t ist;

  (void)argument;

  st = initialize_wireless();
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless init failed: 0x%lX\r\n", (unsigned long)st);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized; NWP in sleep with RAM retention\r\n");

  st = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_i2c_pm_event_handle, &s_ulp_i2c_pm_event_info);
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PM subscribe failed: 0x%lX\r\n", (unsigned long)st);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");

  st = ulp_i2c_application_init();
  if (st != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP I2C application init failed: 0x%lX\r\n", (unsigned long)st);
    osThreadExit();
  }

  while (1) {
    switch (ulp_i2c_current_mode) {
      case SL_ULP_I2C_PROCESS_ACTION:

        SL_PRINT_STRING_ERROR("ULP I2C: send/receive cycle\r\n");

        ulp_i2c_run_send_receive_compare();
        ulp_i2c_current_mode = SL_ULP_I2C_POWER_STATE_TRANSITION;
        break;

      case SL_ULP_I2C_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("ULP I2C: PS4 -> PS2 (reset follower within %u ms)\r\n",
                                (unsigned int)FOLLOWER_RESET_WINDOW_MS);

          ist = sl_i2c_driver_deinit(i2c_instance);
          if (ist != SL_I2C_SUCCESS) {
            SL_PRINT_STRING_ERROR("sl_i2c_driver_deinit: %u\r\n", (unsigned int)ist);
          }
          SL_PRINT_STRING_ERROR("ULP I2C deinitialized successfully\r\n");

          st = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (st != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("add_ps_requirement PS2: %lu\r\n", (unsigned long)st);
            break;
          }
          DEBUGINIT();
          st = configuring_ps2_power_state();
          if (st != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2C: configuring_ps2_power_state failed: 0x%lX\r\n", (unsigned long)st);
            osThreadExit();
          }
          ist = sl_i2c_driver_leader_reconfig_on_power_mode_change(SL_I2C_ULP_MODE);
          if (ist != SL_I2C_SUCCESS) {
            SL_PRINT_STRING_ERROR("reconfig ULP: %u\r\n", (unsigned int)ist);
          }
          st = ulp_i2c_application_init();
          if (st != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2C application init failed: 0x%lX\r\n", (unsigned long)st);
            osThreadExit();
          }
          (void)osDelay(FOLLOWER_RESET_WINDOW_MS);
          current_power_state  = SL_SI91X_POWER_MANAGER_PS2;
          ulp_i2c_current_mode = SL_ULP_I2C_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("ULP I2C: PS2 -> PS4 (reset follower within %u ms)\r\n",
                                (unsigned int)FOLLOWER_RESET_WINDOW_MS);

          ist = sl_i2c_driver_deinit(i2c_instance);
          if (ist != SL_I2C_SUCCESS) {
            SL_PRINT_STRING_ERROR("sl_i2c_driver_deinit: %u\r\n", (unsigned int)ist);
          }
          SL_PRINT_STRING_ERROR("ULP I2C deinitialized successfully\r\n");
          st = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (st != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("add_ps_requirement PS4: %lu\r\n", (unsigned long)st);
            break;
          }
          DEBUGINIT();
          ist = sl_i2c_driver_leader_reconfig_on_power_mode_change(SL_I2C_HP_MODE);
          if (ist != SL_I2C_SUCCESS) {
            SL_PRINT_STRING_ERROR("reconfig HP: %u\r\n", (unsigned int)ist);
          }
          st = ulp_i2c_application_init();
          if (st != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP I2C application init failed: 0x%lX\r\n", (unsigned long)st);
            osThreadExit();
          }
          (void)osDelay(FOLLOWER_RESET_WINDOW_MS);
          current_power_state  = LAST_ENUM_POWER_STATE;
          ulp_i2c_current_mode = SL_ULP_I2C_PROCESS_ACTION;
        } else {
          ist = sl_i2c_driver_deinit(i2c_instance);
          if (ist != SL_I2C_SUCCESS) {
            SL_PRINT_STRING_ERROR("sl_i2c_driver_deinit: %u\r\n", (unsigned int)ist);
          }
          SL_PRINT_STRING_ERROR("ULP I2C deinitialized successfully\r\n");
          ulp_i2c_current_mode = SL_ULP_I2C_TRANSMISSION_COMPLETED;
        }
        break;

      case SL_ULP_I2C_TRANSMISSION_COMPLETED:
      default:
        (void)osDelay(1000);
        break;
    }
  }
}
