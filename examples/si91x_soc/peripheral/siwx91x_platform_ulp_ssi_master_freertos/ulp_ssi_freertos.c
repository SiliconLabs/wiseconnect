/***************************************************************************/ /**
 * @file ulp_ssi_freertos.c
 * @brief ULP SSI Master FreeRTOS
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
#include "ulp_ssi_freertos.h"

#include <string.h>

#include "cmsis_os2.h"
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_si91x_ssi.h"
#include "sl_si91x_power_manager.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define ULP_SSI_BUFFER_SIZE     1024U
#define ULP_SSI_BIT_WIDTH       8U
#define ULP_SSI_BAUDRATE        5000000UL
#define ULP_SSI_RX_SAMPLE_DELAY 0U
#define ULP_SSI_MAX_FRAME_BITS  16U
#define ULP_SSI_BANK_OFFSET     0x800UL
#define ULP_SSI_TX_BUF_MEMORY   (ULP_SRAM_START_ADDR + (1U * ULP_SSI_BANK_OFFSET))
#define ULP_SSI_RX_BUF_MEMORY   (ULP_SRAM_START_ADDR + (2U * ULP_SSI_BANK_OFFSET))
#define FIVE_SECOND_DELAY_MS    5000U

#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 ***************************  Data Types  **************************************
 ******************************************************************************/
typedef enum {
  SL_ULP_SSI_PROCESS_ACTION,
  SL_ULP_SSI_POWER_STATE_TRANSITION,
  SL_ULP_SSI_TRANSMISSION_COMPLETED,
} ulp_ssi_enum_t;

/*******************************************************************************
 ***************************  Static Variables  ********************************
 ******************************************************************************/
static void ulp_ssi_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);

static ulp_ssi_enum_t ulp_ssi_current_mode = SL_ULP_SSI_PROCESS_ACTION;
static sl_status_t status;
static sl_power_state_t current_power_state = SL_SI91X_POWER_MANAGER_PS4;

/*******************************************************************************
 ***************************  Local Variables  ********************************
 ******************************************************************************/
static uint8_t ulp_ssi_tx_buffer[ULP_SSI_BUFFER_SIZE];
#if (ULP_SSI_MASTER_RECEIVE) || (ULP_SSI_MASTER_TRANSFER)
static uint8_t ulp_ssi_rx_buffer[ULP_SSI_BUFFER_SIZE];
#endif

/*******************************************************************************
 ***************************  Static Variables  ********************************
 ******************************************************************************/
static sl_ssi_handle_t ssi_driver_handle = NULL;
static osSemaphoreId_t ulp_ssi_xfer_sem;
#ifdef SL_SI91X_ACX_MODULE
static uint32_t ulp_ssi_slave_number = SSI_SLAVE_1;
#else
static uint32_t ulp_ssi_slave_number = SSI_SLAVE_0;
#endif

/*******************************************************************************
 ***************************  Static Variables  ********************************
 ******************************************************************************/
static sl_power_manager_ps_transition_event_handle_t s_ulp_ssi_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_ssi_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_ssi_pm_transition_callback,
};

/*******************************************************************************
 ***************************  Local Function Prototypes  ***********************
 ******************************************************************************/
static void ulp_ssi_freertos_task(void *argument);
static sl_status_t ulp_ssi_application_init(void);
static void ulp_ssi_xfer_sem_drain(void);
static void ulp_ssi_wait_transfer_done(void);
static void ulp_ssi_teardown(void);
#if (ULP_SSI_MASTER_RECEIVE) || (ULP_SSI_MASTER_TRANSFER)
static void ulp_ssi_compare_loopback(void);
#endif
static sl_status_t configuring_ps2_power_state(void);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);
static void ulp_ssi_event_callback(uint32_t event);

static const osThreadAttr_t ulp_ssi_thread_attributes = {
  .name       = "ulp_ssi",
  .stack_size = 8192,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 *************************** task initialization  ***************************************
 ******************************************************************************/
void ulp_ssi_master_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_ssi_freertos_task, NULL, &ulp_ssi_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP SSI thread\r\n");
  }
}

/*******************************************************************************
 *************************** SSI event callback  ***************************************
 ******************************************************************************/
static void ulp_ssi_event_callback(uint32_t event)
{
  if (event == SSI_EVENT_TRANSFER_COMPLETE) {
    if (ulp_ssi_xfer_sem != NULL) {
      (void)osSemaphoreRelease(ulp_ssi_xfer_sem);
    }
  }
}

/*******************************************************************************
 *************************** SSI transfer semaphore drain  ***************************************
 ******************************************************************************/
static void ulp_ssi_xfer_sem_drain(void)
{

  if (ulp_ssi_xfer_sem == NULL) {
    return;
  }
  if (current_power_state != SL_SI91X_POWER_MANAGER_PS2) {
    if (sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4) == SL_STATUS_OK) {
    }
  }

  while (osSemaphoreAcquire(ulp_ssi_xfer_sem, 0U) == osOK) {
  }
}

/*******************************************************************************
 *************************** SSI transfer wait done  ***************************************
 ******************************************************************************/
static void ulp_ssi_wait_transfer_done(void)
{
  osStatus_t w;
  bool ps4_held = false;

  if (ulp_ssi_xfer_sem == NULL) {
    return;
  }
  if (current_power_state != SL_SI91X_POWER_MANAGER_PS2) {
    if (sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4) == SL_STATUS_OK) {
      ps4_held = true;
    }
  }
  w = osSemaphoreAcquire(ulp_ssi_xfer_sem, osWaitForever);
  if (w != osOK) {
    SL_PRINT_STRING_ERROR("osSemaphoreAcquire (ULP SSI) error: %d\r\n", (int)w);
  }
  if (ps4_held) {
    (void)sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  }
}

/*******************************************************************************
 *************************** SSI teardown  ***************************************
 ******************************************************************************/
static void ulp_ssi_teardown(void)
{
  if (ssi_driver_handle != NULL) {
    sl_si91x_ssi_unregister_event_callback();
    (void)sl_si91x_ssi_deinit(ssi_driver_handle);
    ssi_driver_handle = NULL;
  }
}
#if (ULP_SSI_MASTER_RECEIVE) || (ULP_SSI_MASTER_TRANSFER)
/*******************************************************************************
 *************************** SSI compare loopback  ***************************************
 ******************************************************************************/
static void ulp_ssi_compare_loopback(void)
{
  uint32_t i;
  uint8_t frame_len = (uint8_t)sl_si91x_ssi_get_frame_length(ssi_driver_handle);
  uint16_t mask     = (uint16_t)~0U;
  mask              = (uint16_t)(mask >> (ULP_SSI_MAX_FRAME_BITS - frame_len));

  memcpy(ulp_ssi_rx_buffer, (uint8_t *)ULP_SSI_RX_BUF_MEMORY, ULP_SSI_BUFFER_SIZE);
  for (i = 0; i < ULP_SSI_BUFFER_SIZE; i++) {
    ulp_ssi_rx_buffer[i] &= (uint8_t)mask;
    ulp_ssi_tx_buffer[i] &= (uint8_t)mask;
    if (ulp_ssi_tx_buffer[i] != ulp_ssi_rx_buffer[i]) {
      break;
    }
  }
  if (i == ULP_SSI_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("ULP SSI loopback compare: PASS\r\n");
  } else {
    SL_PRINT_STRING_ERROR("ULP SSI loopback compare: FAIL at index %lu\r\n", (unsigned long)i);
  }
}
#endif
/*******************************************************************************
 *************************** SSI application initialization  ***************************************
 ******************************************************************************/
static sl_status_t ulp_ssi_application_init(void)
{
  sl_ssi_control_config_t cfg = { 0 };
  sl_ssi_version_t ver;
  uint16_t j;

  status                   = SL_STATUS_OK;
  cfg.bit_width            = ULP_SSI_BIT_WIDTH;
  cfg.device_mode          = SL_SSI_ULP_MASTER_ACTIVE;
  cfg.clock_mode           = SL_SSI_PERIPHERAL_CPOL0_CPHA0;
  cfg.baud_rate            = ULP_SSI_BAUDRATE;
  cfg.receive_sample_delay = ULP_SSI_RX_SAMPLE_DELAY;
  cfg.transfer_mode        = 0;

  for (j = 0; j < ULP_SSI_BUFFER_SIZE; j++) {
    ulp_ssi_tx_buffer[j] = (uint8_t)(j + 1U);
  }
  memcpy((uint8_t *)ULP_SSI_TX_BUF_MEMORY, ulp_ssi_tx_buffer, ULP_SSI_BUFFER_SIZE);

  do {
    ver = sl_si91x_ssi_get_version();
    SL_PRINT_STRING_ERROR("SSI API version %d.%d.%d\r\n", ver.release, ver.major, ver.minor);

    status = sl_si91x_ssi_init(cfg.device_mode, &ssi_driver_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_init: %lu\r\n", (unsigned long)status);
      break;
    }

    status = sl_si91x_ssi_set_configuration(ssi_driver_handle, &cfg, ulp_ssi_slave_number);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_set_configuration: %lu\r\n", (unsigned long)status);
      break;
    }
    status = sl_si91x_ssi_register_event_callback(ssi_driver_handle, ulp_ssi_event_callback);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ssi_register_event_callback: %lu\r\n", (unsigned long)status);
      break;
    }

    SL_PRINT_STRING_ERROR("ULP SSI initialized successfully\r\n");

  } while (false);
  return status;
}

/*******************************************************************************
 *************************** SSI PS2 power state configuration  ***************************************
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;

  peri.m4ss_peripheral       = 0;
  config.configure_ram_banks = true;
  config.m4ss_ram_banks      = 0;
  config.ulpss_ram_banks     = 0;
  /* ULPSS peripheral mask for PS2: I2S/I2C/IR/FIM groups; ULPSS_PG_SSI is not asserted here. */
  peri.ulpss_peripheral = SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3;
  status = SL_STATUS_OK;
  do {
    status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("remove_peripheral_requirement: 0x%lX\r\n", (unsigned long)status);
      break;
    }
    status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("configure_ram_retention: 0x%lX\r\n", (unsigned long)status);
      break;
    }

  } while (false);
  return status;
}

/*******************************************************************************
 *************************** SSI PM transition callback  ***************************************
 ******************************************************************************/
static void ulp_ssi_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
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
 *************************** SSI wireless initialization  ***************************************
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
  sl_status_t st;
  sl_si91x_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr           = { 0 };

  st = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (st != SL_STATUS_OK) {
    return st;
  }
  if (sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr) == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:\r\n", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x:\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
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

/*******************************************************************************
 *************************** SSI wireless sleep  ***************************************
 ******************************************************************************/
static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_wifi_performance_profile_v2_t p = { .profile = HIGH_PERFORMANCE };
  if (sl_wifi_set_performance_profile_v2(&p) != SL_STATUS_OK) {
    return;
  }
  p.profile = sleep_with_retention ? DEEP_SLEEP_WITH_RAM_RETENTION : DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  (void)sl_wifi_set_performance_profile_v2(&p);
}

/*******************************************************************************
  *************************** SSI FreeRTOS task  ***************************************
 ******************************************************************************/
static void ulp_ssi_freertos_task(void *argument)
{
  (void)argument;

  ulp_ssi_xfer_sem = osSemaphoreNew(1U, 0U, NULL);
  if (ulp_ssi_xfer_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP SSI semaphore\r\n");
    osThreadExit();
  }

  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized; NWP in sleep with RAM retention\r\n");

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_ssi_pm_event_handle, &s_ulp_ssi_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("PM subscribe failed: 0x%lX\r\n", (unsigned long)status);
  } else {
    SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");
  }

  status = ulp_ssi_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP SSI application init failed: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  while (1) {
    switch (ulp_ssi_current_mode) {
      case SL_ULP_SSI_PROCESS_ACTION:

        ulp_ssi_xfer_sem_drain();
        sl_si91x_ssi_set_slave_number(ulp_ssi_slave_number);

#if ULP_SSI_MASTER_TRANSFER
        (void)osDelay(FIVE_SECOND_DELAY_MS);
        status = sl_si91x_ssi_transfer_data(ssi_driver_handle,
                                            (uint8_t *)ULP_SSI_TX_BUF_MEMORY,
                                            (uint8_t *)ULP_SSI_RX_BUF_MEMORY,
                                            ULP_SSI_BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ssi_transfer_data: %lu\r\n", (unsigned long)status);
          ulp_ssi_current_mode = SL_ULP_SSI_TRANSMISSION_COMPLETED;
          break;
        }

        ulp_ssi_wait_transfer_done();
        ulp_ssi_compare_loopback();
#endif

#if ULP_SSI_MASTER_SEND
        (void)osDelay(FIVE_SECOND_DELAY_MS);
        status = sl_si91x_ssi_send_data(ssi_driver_handle, (uint8_t *)ULP_SSI_TX_BUF_MEMORY, ULP_SSI_BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ssi_send_data: %lu\r\n", (unsigned long)status);
          ulp_ssi_current_mode = SL_ULP_SSI_TRANSMISSION_COMPLETED;
          break;
        }
        ulp_ssi_wait_transfer_done();
        SL_PRINT_STRING_ERROR("data sent successfully\r\n");
        (void)osDelay(FIVE_SECOND_DELAY_MS);
#endif

#if ULP_SSI_MASTER_RECEIVE

        (void)osDelay(FIVE_SECOND_DELAY_MS);

        status = sl_si91x_ssi_receive_data(ssi_driver_handle, (uint8_t *)ULP_SSI_RX_BUF_MEMORY, ULP_SSI_BUFFER_SIZE);
        if (status != SL_STATUS_OK) {
          SL_PRINT_STRING_ERROR("sl_si91x_ssi_receive_data: %lu\r\n", (unsigned long)status);
          ulp_ssi_current_mode = SL_ULP_SSI_TRANSMISSION_COMPLETED;
          break;
        }
        ulp_ssi_wait_transfer_done();
        SL_PRINT_STRING_ERROR("data received successfully\r\n");
        ulp_ssi_compare_loopback();
#endif

        ulp_ssi_current_mode = SL_ULP_SSI_POWER_STATE_TRANSITION;
        break;

      case SL_ULP_SSI_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("ULP SSI: PS4 -> PS2\r\n");
          ulp_ssi_teardown();

          (void)sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          while (!sl_si91x_power_manager_ps2_pre_check()) {
            (void)osDelay(5);
          }

          // sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            break;
          }

          DEBUGINIT();
          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP SSI: configuring_ps2_power_state failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          (void)osDelay(FIVE_SECOND_DELAY_MS);
          status = ulp_ssi_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP SSI application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state  = SL_SI91X_POWER_MANAGER_PS2;
          ulp_ssi_current_mode = SL_ULP_SSI_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("ULP SSI: PS2 -> PS4\r\n");
          ulp_ssi_teardown();
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            break;
          }
          DEBUGINIT();
          (void)osDelay(FIVE_SECOND_DELAY_MS);
          status = ulp_ssi_application_init();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP SSI application init failed: 0x%lX\r\n", (unsigned long)status);
            osThreadExit();
          }
          current_power_state  = LAST_ENUM_POWER_STATE;
          ulp_ssi_current_mode = SL_ULP_SSI_PROCESS_ACTION;
        } else {
          (void)osDelay(FIVE_SECOND_DELAY_MS);
          ulp_ssi_teardown();
          SL_PRINT_STRING_ERROR("ULP SSI: de-initialized successfully\r\n");
          ulp_ssi_current_mode = SL_ULP_SSI_TRANSMISSION_COMPLETED;
        }
        break;

      case SL_ULP_SSI_TRANSMISSION_COMPLETED:
      default:
        (void)osDelay(1000);
        break;
    }
  }
}
