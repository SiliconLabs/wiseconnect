/***************************************************************************/ /**
 * @file
 * @brief Secure Storage Example: write/read/verify MCU secure registers (REG0–REG7).
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Overview
 *
 * This app demonstrates MCU secure storage: write values to secure registers
 * (REG0–REG7), optionally enter low-power, then
 * read back and verify. Uses per-register write/read APIs only.
 *
 * # Defines (app.c)
 *
 * - APP_SECURE_STORAGE_REGISTER_1..8   Map to MCU_STORAGE_REG0..REG7.
 * - APP_SECURE_STORAGE_NUM_REGISTERS   Number of registers (8).
 * - APP_SECURE_STORAGE_REG_VALUE_1..8  Value written to each register.
 * - APP_SECURE_STORAGE_DEBUG_PRINT_VALUES  1 = print values; 0 = disabled.
 * - APP_SLEEP_WAKE_SEM_TIMEOUT_MS      Semaphore timeout (ms).
 *
 ******************************************************************************/

#include "app.h"
#include "sl_si91x_secure_storage.h"
#include "sl_status.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_utility.h"
#include "sl_net_si91x.h"
#include "sl_si91x_driver.h"
#include "base_types.h"
#include "rsi_debug.h"
#include <float.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef SLI_SI91X_MCU_INTERFACE
#include "sl_si91x_power_manager.h"
#include "sl_si91x_m4_ps.h"
#endif

/* --- Secure storage: register map and values --- */
/* Register identifiers (REG0..REG7 from SDK). */
#define APP_SECURE_STORAGE_REGISTER_1 MCU_STORAGE_REG0
#define APP_SECURE_STORAGE_REGISTER_2 MCU_STORAGE_REG1
#define APP_SECURE_STORAGE_REGISTER_3 MCU_STORAGE_REG2
#define APP_SECURE_STORAGE_REGISTER_4 MCU_STORAGE_REG3
#define APP_SECURE_STORAGE_REGISTER_5 MCU_STORAGE_REG4
#define APP_SECURE_STORAGE_REGISTER_6 MCU_STORAGE_REG5
#define APP_SECURE_STORAGE_REGISTER_7 MCU_STORAGE_REG6
#define APP_SECURE_STORAGE_REGISTER_8 MCU_STORAGE_REG7

#define APP_SECURE_STORAGE_NUM_REGISTERS 8U

/* Values written to each register (REG0..REG7); distinct for verification. */
#define APP_SECURE_STORAGE_REG_VALUE_1 0xA5B5C5D5U /* REG0 */
#define APP_SECURE_STORAGE_REG_VALUE_2 0x5A4A3A2AU /* REG1 */
#define APP_SECURE_STORAGE_REG_VALUE_3 0x98765432U /* REG2 */
#define APP_SECURE_STORAGE_REG_VALUE_4 0x12345678U /* REG3 */
#define APP_SECURE_STORAGE_REG_VALUE_5 0x11111111U /* REG4 */
#define APP_SECURE_STORAGE_REG_VALUE_6 0x22222222U /* REG5 */
#define APP_SECURE_STORAGE_REG_VALUE_7 0x33333333U /* REG6 */
#define APP_SECURE_STORAGE_REG_VALUE_8 0x44444444U /* REG7 */

/** Set to 1 to print written/read values in init and after wake; 0 = disabled. */
#define APP_SECURE_STORAGE_DEBUG_PRINT_VALUES 0

/* Array of 8 registers (REG0..REG7) */
static const mcu_secure_storage_register_t app_secure_storage_registers[APP_SECURE_STORAGE_NUM_REGISTERS] = {
  APP_SECURE_STORAGE_REGISTER_1, APP_SECURE_STORAGE_REGISTER_2, APP_SECURE_STORAGE_REGISTER_3,
  APP_SECURE_STORAGE_REGISTER_4, APP_SECURE_STORAGE_REGISTER_5, APP_SECURE_STORAGE_REGISTER_6,
  APP_SECURE_STORAGE_REGISTER_7, APP_SECURE_STORAGE_REGISTER_8,
};

static const uint32_t app_secure_storage_reg_write_values[APP_SECURE_STORAGE_NUM_REGISTERS] = {
  APP_SECURE_STORAGE_REG_VALUE_1, APP_SECURE_STORAGE_REG_VALUE_2, APP_SECURE_STORAGE_REG_VALUE_3,
  APP_SECURE_STORAGE_REG_VALUE_4, APP_SECURE_STORAGE_REG_VALUE_5, APP_SECURE_STORAGE_REG_VALUE_6,
  APP_SECURE_STORAGE_REG_VALUE_7, APP_SECURE_STORAGE_REG_VALUE_8,
};

/** Semaphore timeout for sleep/wake (milliseconds). */
#define APP_SLEEP_WAKE_SEM_TIMEOUT_MS (2U * 2000U)

/* Thread attributes for the application thread. */
static const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .stack_size = 3072,
  .priority   = osPriorityLow,
};

#ifndef SL_SI91X_WC_DEVICE_CONFIGURATION
static const sl_wifi_device_configuration_t station_init_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map =
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                      | SL_SI91X_FEAT_WPS_DISABLE
#endif
                      ),
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENTION_VALID,
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_SI91X_ENABLE_ENHANCED_MAX_PSP) }
};
#else
#include "sl_si91x_wc_device_configuration.h"
static sl_mac_address_t mac = { .octet = SL_SI91X_WC_MAC_ADDRESS };
static const sl_wifi_device_configuration_t station_init_configuration = {
    .boot_option = SL_SI91X_WC_BOOT_OPTION,
#if SL_SI91X_WC_SET_MAC_ADDRESS
    .mac_address = &mac,
#else
    .mac_address = NULL,
#endif
    .band        = SL_SI91X_WC_WIFI_BAND,
    .region_code = SL_SI91X_WC_REGION,
    .boot_config = {
                  .oper_mode                  = SL_SI91X_WC_OPERMODE,
                  .coex_mode                  = SL_SI91X_WC_COEXMODE,
                  .feature_bit_map            = SL_SI91X_WC_FEATURE_BITMAP,
                  .tcp_ip_feature_bit_map     = SL_SI91X_WC_TCP_IP_FEATURE_BITMAP,
                  .custom_feature_bit_map     = SL_SI91X_WC_CUSTOM_FEATURE_BITMAP,
                  .ext_custom_feature_bit_map = SL_SI91X_WC_EXTENDED_CUSTOM_FEATURE_BITMAP,
                  .bt_feature_bit_map         = SL_SI91X_WC_BT_FEATURE_BITMAP,
                  .ext_tcp_ip_feature_bit_map = SL_SI91X_WC_EXTENDED_TCPIP_FEATURE_BITMAP,
                  .ble_feature_bit_map        = SL_SI91X_WC_BLE_FEATURE_BITMAP,
                  .ble_ext_feature_bit_map    = SL_SI91X_WC_EXTENDED_BLE_CUSTOM_FEATURE_BITMAP,
                  .config_feature_bit_map     = SL_SI91X_WC_CONFIG_FEATURE_BITMAP,
                },
    .ta_pool = { .tx_ratio_in_buffer_pool     = SL_SI91X_WC_TX_POOL_RATIO,
                 .rx_ratio_in_buffer_pool     = SL_SI91X_WC_RX_POOL_RATIO,
                 .global_ratio_in_buffer_pool = SL_SI91X_WC_GLOBAL_POOL_RATIO },
    .efuse_data_type = SL_SI91X_WC_EFUSE_DATA_TYPE,
    .nwp_fw_image_number = SL_SI91X_WC_NWP_FW_IMAGE_NUMBER
  };
#endif

/* Application thread and secure-storage helpers (forward declarations). */
static void application_thread(void *argument);
sl_status_t sl_app_write_secure_storage_registers(const mcu_secure_storage_register_t *registers,
                                                  const uint32_t *values,
                                                  uint32_t count);
sl_status_t sl_app_read_secure_storage_registers(const mcu_secure_storage_register_t *registers,
                                                 uint32_t *values,
                                                 uint32_t count);
void sl_app_verify_secure_storage_read_values(const uint32_t *read_values,
                                              const uint32_t *expected_values,
                                              uint32_t count);
void app_exit(void);

/* Define the application entry point */
void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_thread, NULL, &thread_attributes);
}

/* Define the application thread function */
static void application_thread(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status = SL_STATUS_OK;
  uint32_t app_secure_storage_read_values[APP_SECURE_STORAGE_NUM_REGISTERS];
  sl_wifi_performance_profile_v2_t performance_profile = { .profile = DEEP_SLEEP_WITH_RAM_RETENTION };
  sl_wifi_firmware_version_t version                   = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted through SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged through SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("\r\nFailed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    app_exit();
  }

  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\n\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
  } else {
    print_firmware_version(&version);
  }

  /* Write 8 registers (REG0..REG7); read/compare done in loop after wake. */
  status = sl_app_write_secure_storage_registers(app_secure_storage_registers,
                                                 app_secure_storage_reg_write_values,
                                                 APP_SECURE_STORAGE_NUM_REGISTERS);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to write secure storage registers: 0x%lx\r\n", status);
    app_exit();
  }
#if APP_SECURE_STORAGE_DEBUG_PRINT_VALUES
  for (uint32_t i = 0; i < APP_SECURE_STORAGE_NUM_REGISTERS; i++) {
    SL_PRINT_STRING_ERROR("\r\nWrite secure storage register[%lu]: 0x%08lX\r\n",
                          (unsigned long)i,
                          (unsigned long)app_secure_storage_reg_write_values[i]);
  }
#endif
  status = sl_si91x_secure_storage_enable_protection_and_lock();
  if (status == SL_STATUS_NOT_AVAILABLE) {
    SL_PRINT_STRING_ERROR("\r\nSecure protection disabled in config\r\n");
    app_exit();
  } else if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to enable MCU secure storage protection: 0x%lx\r\n", status);
    app_exit();
  } else {
    SL_PRINT_STRING_ERROR("\r\nMCU secure storage protection enabled, secure storage write "
                          "disabled\r\n");
  }

  status = sl_wifi_set_performance_profile_v2(&performance_profile);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\n\r\nPower save configuration Failed, Error Code : 0x%lX\r\n", status);
    app_exit();
  }

  status = sl_app_read_secure_storage_registers(app_secure_storage_registers,
                                                app_secure_storage_read_values,
                                                APP_SECURE_STORAGE_NUM_REGISTERS);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to read secure storage registers: 0x%lx\r\n", status);
    app_exit();
  }
  sl_app_verify_secure_storage_read_values(app_secure_storage_read_values,
                                           app_secure_storage_reg_write_values,
                                           APP_SECURE_STORAGE_NUM_REGISTERS);
  /* Disable interrupts so WDT is no longer kicked; WDT will reset the system. */
  SL_PRINT_STRING_ERROR("\r\ndisabled interrupts so WDT is no longer kicked; WDT will reset "
                        "the system. \r\n");
  __disable_irq();
  while (1) {
    ; /* Idle; no sleep/wake loop */
  }
}

/***************************************************************************/ /**
   * Write multiple secure storage registers from arrays.
   *
   * @param[in] registers  Array of register identifiers (must not be NULL).
   * @param[in] values     Array of 32-bit values to write (must not be NULL).
   * @param[in] count     Number of registers/values (must be > 0 and match array lengths).
   * @return SL_STATUS_OK on success, or an error from the driver on first failure.
   ******************************************************************************/
sl_status_t sl_app_write_secure_storage_registers(const mcu_secure_storage_register_t *registers,
                                                  const uint32_t *values,
                                                  uint32_t count)
{
  sl_status_t status                                     = SL_STATUS_OK;
  uint32_t read_values[APP_SECURE_STORAGE_NUM_REGISTERS] = { 0 };
  uint32_t i                                             = 0;
  bool all_match                                         = true;

  if (registers == NULL || values == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (count == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (count > APP_SECURE_STORAGE_NUM_REGISTERS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  /* Read current values once before any write. */
  for (i = 0; i < count; i++) {
    status = sl_si91x_secure_storage_read_register(registers[i], &read_values[i]);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }

  /* Check if read and write match; if all match, skip write. */
  for (i = 0; i < count; i++) {
    if (read_values[i] != values[i]) {
      all_match = false;
      break;
    }
  }
  if (all_match) {
    SL_PRINT_STRING_ERROR("\r\nRegister Read and before write values matched, skipping "
                          "write.\r\n");
    return SL_STATUS_OK;
  }
  SL_PRINT_STRING_ERROR("\r\nRegister Read and write values differ, writing registers.\r\n");

  for (i = 0; i < count; i++) {
    status = sl_si91x_secure_storage_write_register(registers[i], values[i]);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
   * Read multiple secure storage registers into an array.
   *
   * @param[in]  registers  Array of register identifiers (must not be NULL).
   * @param[out] values     Array to store read values (must not be NULL).
   * @param[in]  count      Number of registers to read (must be > 0 and match array lengths).
   * @return SL_STATUS_OK on success, or an error from the driver on first failure.
   ******************************************************************************/
sl_status_t sl_app_read_secure_storage_registers(const mcu_secure_storage_register_t *registers,
                                                 uint32_t *values,
                                                 uint32_t count)
{
  sl_status_t status = SL_STATUS_OK;
  uint32_t i         = 0;
  if (registers == NULL || values == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (count == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (count > APP_SECURE_STORAGE_NUM_REGISTERS) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  for (i = 0; i < count; i++) {
    status = sl_si91x_secure_storage_read_register(registers[i], &values[i]);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
   * Verify read values against expected values: optionally print read values
   * (when APP_SECURE_STORAGE_DEBUG_PRINT_VALUES is 1), then compare and report
   * mismatches.
   *
   * @param[in] read_values     Array of values read from secure storage registers.
   * @param[in] expected_values Array of expected values (e.g. values written).
   * @param[in] count          Number of elements to compare.
   ******************************************************************************/
void sl_app_verify_secure_storage_read_values(const uint32_t *read_values,
                                              const uint32_t *expected_values,
                                              uint32_t count)
{
  uint32_t i              = 0;
  uint32_t mismatch_count = 0;
#if (APP_SECURE_STORAGE_DEBUG_PRINT_VALUES == 1)
  for (i = 0; i < count; i++) {
    SL_PRINT_STRING_ERROR("\r\nRead secure storage register[%lu]: 0x%08lX\r\n",
                          (unsigned long)i,
                          (unsigned long)read_values[i]);
  }
#endif

  for (i = 0; i < count; i++) {
    if (read_values[i] != expected_values[i]) {
      SL_PRINT_STRING_ERROR("\r\nRegister[%lu] MISMATCH: expected 0x%08lX, read 0x%08lX\r\n",
                            (unsigned long)i,
                            (unsigned long)expected_values[i],
                            (unsigned long)read_values[i]);
      mismatch_count++;
    }
  }
  if (mismatch_count == 0) {
    SL_PRINT_STRING_ERROR("\r\nAll register values matched.\r\n");
  } else {
    SL_PRINT_STRING_ERROR("\r\n%lu register(s) failed.\r\n", (unsigned long)mismatch_count);
  }
}

/***************************************************************************/ /**
   * Periodic tick callback (empty). All work is done in application_thread.
   ******************************************************************************/
void app_process_action(void)
{
  /* Empty */
}

/***************************************************************************/ /**
   * @brief Exit the application (infinite halt; no return).
   ******************************************************************************/
void app_exit(void)
{
  for (;;) {
    /* Intentional halt; system may be in unrecoverable state. */
  }
}
