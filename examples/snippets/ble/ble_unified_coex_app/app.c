/*******************************************************************************
* @file  app.c
* @brief BLE Unified Coex Application - Minimal Example
*******************************************************************************
* # License
* <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "cmsis_os2.h"
#include "sl_status.h"
#include "sl_wifi.h"
#include "sl_wifi_device.h"
#include "sl_wifi_callback_framework.h"
#include "ble_config.h"
#include <stdio.h>
#include "sl_constants.h"

/******************************************************
 * Forward Declarations
 ******************************************************/
static void ble_task(void *argument);

/******************************************************
 * BLE Task Thread Attributes
 ******************************************************/
static const osThreadAttr_t ble_thread_attributes = {
  .name       = "ble_task",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
};

/******************************************************
 * WiFi Device Configuration
 * Required for BLE operation on Si91x wireless interface
 * BLE-specific configuration comes from ble_config.h
 ******************************************************/
#if (defined(SLI_SI91X_MCU_INTERFACE) || defined(SLI_SI91X_NCP_INTERFACE))
/*
 * FreeRTOS idle hook: drains the logger ring buffer via sl_log_flush().
 * Active only for backends that emit the proprietary stream (IOStream
 * Compact over UART/VCOM and the proprietary UART backend); a no-op for
 * IOStream Compact over RTT, IOStream Formatted, SystemView, and Log None.
 */
void vApplicationIdleHook(void)
{
  sl_log_flush();
}
#endif

static sl_wifi_device_configuration_t ble_device_config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_BLE_MODE,
                   .feature_bit_map = (SL_WIFI_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
                                       | SL_SI91X_FEAT_DEV_TO_HOST_ULP_GPIO_1),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                   .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | MEMORY_CONFIG | SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   // BLE feature bitmaps set by set_bt_ble_bitmaps() from component config
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
};

/******************************************************
 * Application entry point
 ******************************************************/
void app_init(const void *unused)
{
  int32_t status;
  UNUSED_PARAMETER(unused);

  // Set BLE feature bitmaps from component config (ORs into boot_config; do not reset)
  set_bt_ble_bitmaps(&ble_device_config);

  // Initialize WiFi/wireless interface (required for BLE on Si91x)
  status = sl_wifi_init(&ble_device_config, NULL, sl_wifi_default_event_handler);
  if (status != SL_STATUS_OK) {
    SL_DEBUG_LOG_V2(ERROR, "WiFi/Wireless initialization failed: 0x%lx", status);
    return;
  }

  SL_DEBUG_LOG_V2(INFO, "BLE Unified Application Initialized");

  // Create BLE task - enters unified event loop (never returns)
  osThreadNew((osThreadFunc_t)ble_task, NULL, &ble_thread_attributes);
}

/******************************************************
 * Application process action
 ******************************************************/
void app_process_action(void)
{
  // Application-level processing (if any)
  // BLE event processing is handled by the unified framework
}

/******************************************************
 * BLE Task - Entry point for BLE unified framework
 ******************************************************/
static void ble_task(void *argument)
{
  UNUSED_PARAMETER(argument);

  // Enter BLE unified framework
  // This never returns - all BLE operations handled by unified event loop
  rsi_ble_main_app_task();
}
