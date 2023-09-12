/*******************************************************************************
 * @file  app.c
 * @brief
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

/**
 * Include files
 * */

//! SL Wi-Fi SDK includes
#include "sl_board_configuration.h"
#include "sl_constants.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"

//! BLE include files to refer BLE APIs
#include <string.h>

#include "ble_config.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common.h"
#include "rsi_bt_common_apis.h"
#include "rsi_common_apis.h"
#include "sl_si91x_driver.h"
#include "app.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#include "rsi_m4.h"
#include "rsi_ds_timer.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_rom_ulpss_clk.h"
#include "rsi_rom_timer.h"
#include "rsi_rom_power_save.h"
#include "sl_event_handler.h"
#endif

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
//! Maximum number of advertise reports to hold
#define NO_OF_ADV_REPORTS 10

#ifdef COMMON_FLASH_EN
#define NWPAON_MEM_HOST_ACCESS_CTRL_CLEAR_1 (*(volatile uint32_t *)(0x41300000 + 0x4))
#define M4SS_TASS_CTRL_SET_REG              (*(volatile uint32_t *)(0x24048400 + 0x34))
#define M4SS_TASS_CTRL_CLR_REG              (*(volatile uint32_t *)(0x24048400 + 0x38))
#endif

/*=======================================================================*/
//!    Application powersave configurations
/*=======================================================================*/
#if ENABLE_POWER_SAVE
sl_wifi_performance_profile_t wifi_profile = { ASSOCIATED_POWER_SAVE, 0, 0, 1000 };

#ifdef RSI_M4_INTERFACE
#ifdef COMMON_FLASH_EN
#ifdef CHIP_917B0
#define IVT_OFFSET_ADDR 0x81C2000 /*<!Application IVT location VTOR offset for B0>  */
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset for A0>  */
#endif
#else
#define IVT_OFFSET_ADDR 0x8012000 /*<!Application IVT location VTOR offset for dual flash A0 and B0>  */
#endif
#ifdef CHIP_917B0
#define WKP_RAM_USAGE_LOCATION 0x24061EFC /*<!Bootloader RAM usage location upon wake up  for B0 */
#else
#define WKP_RAM_USAGE_LOCATION 0x24061000 /*<!Bootloader RAM usage location upon wake up for A0  */
#endif
#define WIRELESS_WAKEUP_IRQ_PRI    8
#define portNVIC_SHPR3_REG         (*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI        (((uint32_t)(0x3f << 4)) << 16UL)
#define portNVIC_SYSTICK_PRI       (((uint32_t)(0x3f << 4)) << 24UL)
#define WKP_RAM_USAGE_LOCATION     0x24061EFC /*<! Bootloader RAM usage location upon wake up  */
#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn
void M4_sleep_wakeup(void);
void IRQ026_Handler();
void fpuInit(void);
#endif

int32_t rsi_initiate_power_save(void);
#endif
/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Memory to initialize driver
uint8_t wlan_radio_initialized = 0, powersave_cmd_given = 0;
uint8_t device_found          = 0;
uint8_t remote_dev_addr[18]   = { 0 };
uint8_t remote_dev_bd_addr[6] = { 0 };
static uint8_t remote_name[31];
static uint8_t remote_addr_type                                 = 0;
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static rsi_ble_event_conn_status_t rsi_app_connected_device     = { 0 };
static rsi_ble_event_disconnect_t rsi_app_disconnected_device   = { 0 };
uint8_t rsi_ble_states_bitmap;

osSemaphoreId_t ble_slave_conn_sem;
osSemaphoreId_t ble_main_task_sem;
static volatile uint32_t ble_app_event_map;
static volatile uint32_t ble_app_event_map1;

static const sl_wifi_device_configuration_t config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                   .coex_mode       = SL_SI91X_WLAN_BLE_MODE,
                   .feature_bit_map = (SL_SI91X_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
                                       | SL_SI91X_FEAT_DEV_TO_HOST_ULP_GPIO_1),
                   .tcp_ip_feature_bit_map =
                     (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map =
                     (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
#ifdef CHIP_917
                      | RAM_LEVEL_NWP_ADV_MCU_BASIC | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#else
#ifdef RSI_M4_INTERFACE
                      | RAM_LEVEL_NWP_MEDIUM_MCU_MEDIUM
#else
                      | RAM_LEVEL_NWP_ALL_MCU_ZERO
#endif
#endif
                      | (SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE)),
                   .bt_feature_bit_map = (SL_SI91X_BT_RF_TYPE | SL_SI91X_ENABLE_BLE_PROTOCOL
#if (RSI_BT_GATT_ON_CLASSIC)
                                          | SL_SI91X_BT_ATT_OVER_CLASSIC_ACL /* to support att over classic acl link */
#endif
                                          ),
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   //!ENABLE_BLE_PROTOCOL in bt_feature_bit_map
                   .ble_feature_bit_map =
                     ((SL_SI91X_BLE_MAX_NBR_PERIPHERALS(RSI_BLE_MAX_NBR_PERIPHERALS)
                       | SL_SI91X_BLE_MAX_NBR_CENTRALS(RSI_BLE_MAX_NBR_CENTRALS)
                       | SL_SI91X_BLE_MAX_NBR_ATT_SERV(RSI_BLE_MAX_NBR_ATT_SERV)
                       | SL_SI91X_BLE_MAX_NBR_ATT_REC(RSI_BLE_MAX_NBR_ATT_REC))
                      | SL_SI91X_FEAT_BLE_CUSTOM_FEAT_EXTENTION_VALID | SL_SI91X_BLE_PWR_INX(RSI_BLE_PWR_INX)
                      | SL_SI91X_BLE_PWR_SAVE_OPTIONS(RSI_BLE_PWR_SAVE_OPTIONS)
                      | SL_SI91X_916_BLE_COMPATIBLE_FEAT_ENABLE
#if RSI_BLE_GATT_ASYNC_ENABLE
                      | SL_SI91X_BLE_GATT_ASYNC_ENABLE
#endif
                      ),
                   .ble_ext_feature_bit_map =
                     ((SL_SI91X_BLE_NUM_CONN_EVENTS(RSI_BLE_NUM_CONN_EVENTS)
                       | SL_SI91X_BLE_NUM_REC_BYTES(RSI_BLE_NUM_REC_BYTES))
#if RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
                      | SL_SI91X_BLE_INDICATE_CONFIRMATION_FROM_HOST //indication response from app
#endif
#if RSI_BLE_MTU_EXCHANGE_FROM_HOST
                      | SL_SI91X_BLE_MTU_EXCHANGE_FROM_HOST //MTU Exchange request initiation from app
#endif
#if RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST
                      | (SL_SI91X_BLE_SET_SCAN_RESP_DATA_FROM_HOST) //Set SCAN Resp Data from app
#endif
#if RSI_BLE_DISABLE_CODED_PHY_FROM_HOST
                      | (SL_SI91X_BLE_DISABLE_CODED_PHY_FROM_HOST) //Disable Coded PHY from app
#endif
#if BLE_SIMPLE_GATT
                      | SL_SI91X_BLE_GATT_INIT
#endif
                      ),
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP) }
};

const osThreadAttr_t thread_attributes = {
  .name       = "application_thread",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityNormal,
  .tz_module  = 0,
  .reserved   = 0,
};
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
/*==============================================*/
void IRQ026_Handler()
{
  volatile uint32_t wakeUpSrc = 0;

  /*Get the wake up source */
  wakeUpSrc = RSI_PS_GetWkpUpStatus();

  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);
  return;
}
/**
 * @fn         M4_sleep_wakeup
 * @brief      Keeps the M4 In the Sleep
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to trigger sleep in the M4 and in the case of the retention submitting the buffer valid
 * to the TA for the rx packets.
 */
#if (defined RSI_M4_INTERFACE && ENABLE_POWER_SAVE)
void M4_sleep_wakeup(void)
{
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);

  /* sets the priority of an Wireless wakeup interrupt. */
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQ_PRI);

  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  sl_si91x_disable_default_ldo_mode();

  /* bypass_ldorf_ctrl needs to be enabled */
  sl_si91x_enable_bypass_ldo_rf();

  sl_si91x_disable_flash_ldo();

  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         0,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         0,
                         RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);
#else

#ifdef COMMON_FLASH_EN
  M4SS_P2P_INTR_SET_REG &= ~BIT(3);
#endif
  /* Configure RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep*/
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);
#endif

  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();

#ifdef DEBUG_UART
  fpuInit();
  /*Initialize UART after wake up*/
  sl_service_init();
#endif
}
#endif
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_app_set_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }

  if (ble_main_task_sem) {
    osSemaphoreRelease(ble_main_task_sem);
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  } else {
    ble_app_event_map1 &= ~BIT((event_num - 32));
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{
  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, RSI_REMOTE_DEVICE_NAME)) == 0))
      || ((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE)
          && ((strcmp((const char *)remote_dev_addr, RSI_BLE_DEV_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);

  //! unblock connection semaphore
  osSemaphoreRelease(ble_slave_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  //! Comparing Remote slave bd address to check the scan bitmap
  if (!(memcmp(remote_dev_bd_addr, (uint8_t *)resp_disconnect->dev_addr, 6))) {
    CLR_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
  } else {
    CLR_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
  }
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  //! unblock connection semaphore
  if (ble_slave_conn_sem) {
    osSemaphoreRelease(ble_slave_conn_sem);
  }
}

#if ENABLE_POWER_SAVE
/*==============================================*/
/**
 * @fn         rsi_initiate_power_save
 * @brief      send power save command to RS9116 module
 *
 * @param[out] none
 * @return     status of commands, success-> 0, failure ->-1
 * @section description
 * This function sends command to keep module in power save
 */
int32_t rsi_initiate_power_save(void)
{
  int32_t status = RSI_SUCCESS;

  LOG_PRINT("\r\n keep module in to power save \r\n");
  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
    return status;
  }

  //! initiating power save in wlan mode
  status = sl_wifi_set_performance_profile(&wifi_profile);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\n Failed to initiate power save in Wi-Fi mode :%ld\r\n", status);
    return status;
  }

  LOG_PRINT("\r\n Module is in power save \r\n");
  return status;
}
#endif

/*==============================================*/
/**
 * @fn         ble_app_task
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the BLE peripheral/central role and simple GAP API's.
 */

void ble_app_task(void *argument)
{
  UNUSED_PARAMETER(argument);

  int32_t status                   = 0;
  int32_t temp_event_map           = 0;
  int32_t temp_event_map1          = 0;
  sl_wifi_version_string_t version = { 0 };

#if ((BLE_ROLE == PERIPHERAL_ROLE) || (BLE_ROLE == DUAL_MODE))
  uint8_t adv[31] = { 2, 1, 6 };
#endif

  status = sl_wifi_init(&config, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\n Wi-Fi Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  } else {
    LOG_PRINT("\r\n Wi-Fi Initialization Success\n");
  }

  //! Firmware version Prints
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
  } else {
    LOG_PRINT("\r\nfirmware_version = %s\r\n", version.version);
  }

  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(rsi_ble_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  ble_main_task_sem = osSemaphoreNew(1, 0, NULL);

  //! initialize the event map
  rsi_ble_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble get local device address cmd failed with reason code : %lX \n", status);
    return;
  }
  LOG_PRINT("\n Get local device address: %x:%x:%x:%x\n",
            rsi_app_resp_get_dev_addr[3],
            rsi_app_resp_get_dev_addr[2],
            rsi_app_resp_get_dev_addr[1],
            rsi_app_resp_get_dev_addr[0]);

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble set local name cmd failed with reason code : %lX \n", status);
    return;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble get local name cmd failed with reason code : %lX \n", status);
    return;
  }
  LOG_PRINT("\n Local name set to: %s\n", rsi_app_resp_get_local_name.name);

  ble_slave_conn_sem = osSemaphoreNew(1, 0, NULL);

#if ((BLE_ROLE == PERIPHERAL_ROLE) || (BLE_ROLE == DUAL_MODE))
  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_LOCAL_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_LOCAL_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_LOCAL_NAME) + 5);

  //! start the advertising
  LOG_PRINT("\n Start advertising \n");
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble start advertising failed with reason code : %lX \n", status);
    return;
  }
  SET_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
#endif

#if ((BLE_ROLE == CENTRAL_ROLE) || (BLE_ROLE == DUAL_MODE))
  //! start scanning
  LOG_PRINT("\n Start scanning \n");
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n ble start scanning failed with reason code : %lX \n", status);
    return;
  }
  SET_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
#endif
#if ENABLE_POWER_SAVE
  if (!powersave_cmd_given) {
    LOG_PRINT("\r\n Initiating PowerSave\r\n");
    status = rsi_initiate_power_save();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
      return;
    }
    powersave_cmd_given = 1;
  }

#ifdef RSI_M4_INTERFACE
  P2P_STATUS_REG &= ~M4_wakeup_TA;
  // LOG_PRINT("\n RSI_BLE_REQ_PWRMODE\n ");
#endif
#endif
  while (1) {
    //! Application main loop
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
#if (defined RSI_M4_INTERFACE && ENABLE_POWER_SAVE)
      //! if events are not received loop will be continued.

      if (!(P2P_STATUS_REG & TA_wakeup_M4)) {
        P2P_STATUS_REG &= ~M4_wakeup_TA;
        LOG_PRINT("\r\n triggering M4 sleep\r\n");
        M4_sleep_wakeup();
      }
#else
      if (ble_main_task_sem) {
        osSemaphoreAcquire(ble_main_task_sem, osWaitForever);
      }
      continue;
#endif
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        //! advertise report event.
        //! initiate stop scanning command.
        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n ble stop scanning failed with reason code : %lX \n", status);
          return;
        }

        //! initiating the connection with remote BLE device
        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n ble connect command failed with reason code : %lX \n", status);
          return;
        }

        if (ble_slave_conn_sem) {
          osSemaphoreAcquire(ble_slave_conn_sem, 10000);
        }
        // need to give sufficient time to connect to remote device
        osDelay(10000);

        temp_event_map1 = rsi_ble_app_get_event();

        if ((temp_event_map1 == -1) || (!(temp_event_map1 & RSI_APP_EVENT_CONNECTED))) {
          LOG_PRINT("\r\n Initiating connect cancel command \n");
          status = rsi_ble_connect_cancel((int8_t *)remote_dev_bd_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble connect cancel cmd status = %lX \n", status);
          } else {
            CLR_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
            rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
          }
        }

      } break;
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event
        LOG_PRINT("\n Connection is success .............\n");
        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);

      } break;
      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnected event
        LOG_PRINT("\n In disconnect event............ \n ");
        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);

        LOG_PRINT("\n Keep module in to active state \n");
#if ENABLE_POWER_SAVE
        LOG_PRINT("\r\n Keep module in to active state \r\n");
        //! initiating Active mode in BT mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return;
        }

        //! initiating power save in wlan mode
        wifi_profile.profile = HIGH_PERFORMANCE;
        status               = sl_wifi_set_performance_profile(&wifi_profile);
        if (status != SL_STATUS_OK) {
          LOG_PRINT("\r\n Failed to keep module in HIGH_PERFORMANCE mode \r\n");
          return;
        }
#endif
        if (((BLE_ROLE == PERIPHERAL_ROLE) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE)))) {
          //! set device in advertising mode.
          LOG_PRINT("\n Start advertising \n");
adv:
          status = rsi_ble_start_advertising();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble start advertising failed with reason code : %lX \n", status);
            goto adv;
          }
          SET_BIT1(rsi_ble_states_bitmap, RSI_ADV_STATE);
        }
        if (((BLE_ROLE == CENTRAL_ROLE) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE)))) {
          device_found = 0;
          //! set device in scanning mode.
          LOG_PRINT("\n Start scanning \n");
scan:
          status = rsi_ble_start_scanning();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble start scanning failed with reason code : %lX \n", status);
            goto scan;
          }
          SET_BIT1(rsi_ble_states_bitmap, RSI_SCAN_STATE);
        }
#if ENABLE_POWER_SAVE
        LOG_PRINT("\r\n Keep module in to power save \r\n");
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return;
        }

        //! initiating power save in wlan mode
        wifi_profile.profile = ASSOCIATED_POWER_SAVE;
        status               = sl_wifi_set_performance_profile(&wifi_profile);
        if (status != SL_STATUS_OK) {
          LOG_PRINT("\r\n Failed to keep module in power save \r\n");
          return;
        }
        LOG_PRINT("\r\n Module is in power save \r\n");
#endif
      } break;
    }
  }
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)ble_app_task, NULL, &thread_attributes);
}
