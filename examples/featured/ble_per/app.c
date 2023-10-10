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
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "cmsis_os2.h"

//! BLE include file to refer BLE APIs
#include "rsi_ble_apis.h"
#include "ble_config.h"
#include "rsi_ble_common_config.h"
#include "rsi_bt_common_apis.h"
#include "rsi_ble.h"
#include "rsi_bt_common.h"
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
//! Common include file
#include "rsi_common_apis.h"

#include <string.h>

#define RSI_BLE_LOCAL_NAME (void *)"BLE_PERIPHERAL"

#define DISABLE 0
#define ENABLE  1

#define RSI_BLE_PER_TRANSMIT_MODE 1
#define RSI_BLE_PER_RECEIVE_MODE  2
#define RSI_PER_STATS             3

#define RSI_CONFIG_PER_MODE RSI_BLE_PER_RECEIVE_MODE

#define LE_ONE_MBPS       1
#define LE_TWO_MBPS       2
#define LE_125_KBPS_CODED 4
#define LE_500_KBPS_CODED 8

#define DATA_PRBS9                 0x00
#define DATA_FOUR_ONES_FOUR_ZEROES 0x01
#define DATA_ALT_ONES_AND_ZEROES   0x02
#define DATA_PRSB15                0x03
#define DATA_ALL_ONES              0x04
#define DATA_ALL_ZEROES            0x05
#define DATA_FOUR_ZEROES_FOUR_ONES 0x06
#define DATA_ALT_ZEROES_AND_ONES   0x07

#define LE_ADV_CHNL_TYPE  0
#define LE_DATA_CHNL_TYPE 1

#define BURST_MODE     0
#define CONTIUOUS_MODE 1

#define NO_HOPPING     0
#define FIXED_HOPPING  1
#define RANDOM_HOPPING 2

#define BT_PER_STATS_CMD_ID 0x08
#define BLE_TRANSMIT_CMD_ID 0x13
#define BLE_RECEIVE_CMD_ID  0x14

#define BLE_ACCESS_ADDR    0x71764129
#define BLE_TX_PKT_LEN     32
#define BLE_PHY_RATE       LE_ONE_MBPS
#define BLE_RX_CHNL_NUM    10
#define BLE_TX_CHNL_NUM    10
#define BLE_TX_POWER_INDEX 8
#define SCRAMBLER_SEED     0
#define NUM_PKTS           0
#define RSI_INTER_PKT_GAP  0

#define ONBOARD_ANT_SEL 2
#define EXT_ANT_SEL     3

#define BLE_EXTERNAL_RF 0
#define BLE_INTERNAL_RF 1

#define NO_CHAIN_SEL      0
#define WLAN_HP_CHAIN_BIT 0
#define WLAN_LP_CHAIN_BIT 1
#define BT_HP_CHAIN_BIT   2
#define BT_LP_CHAIN_BIT   3

#define PLL_MODE_0 0
#define PLL_MODE_1 1

#define LOOP_BACK_MODE_DISABLE 0
#define LOOP_BACK_MODE_ENABLE  1

#define EXT_DATA_LEN_IND 1

#define DUTY_CYCLING_DISABLE 0
#define DUTY_CYCLING_ENABLE  1

#ifdef RSI_M4_INTERFACE
#define WIRELESS_WAKEUP_IRQ_PRI    8
#define portNVIC_SHPR3_REG         (*((volatile uint32_t *)0xe000ed20))
#define portNVIC_PENDSV_PRI        (((uint32_t)(0x3f << 4)) << 16UL)
#define portNVIC_SYSTICK_PRI       (((uint32_t)(0x3f << 4)) << 24UL)
#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn
void M4_sleep_wakeup(void);
void IRQ026_Handler();
void fpuInit(void);

#ifdef ALARM_TIMER_BASED_WAKEUP
void InitM4AlarmConfig(void);
void RSI_Set_Alarm_Intr_Time(uint16_t interval);
static RTC_TIME_CONFIG_T rtcConfig, readTime, alarmConfig, readAlarmConfig, rtc_get_Time;
#define RTC_ALARM_IRQHandler IRQ028_Handler
#define NVIC_RTC_ALARM       MCU_CAL_ALARM_IRQn
#define RC_TRIGGER_TIME      3
#define RO_TRIGGER_TIME      0

#define ALARM_PERIODIC_TIME       5 /*<! periodic alarm configuration in SEC */
#define NO_OF_HOURS_IN_A_DAY      24
#define NO_OF_MINUTES_IN_AN_HOUR  60
#define NO_OF_SECONDS_IN_A_MINUTE 60
#define NO_OF_MONTHS_IN_A_YEAR    12
#define BASE_YEAR                 2000
#define NO_OF_DAYS_IN_A_MONTH_1   28
#define NO_OF_DAYS_IN_A_MONTH_2   29
#define NO_OF_DAYS_IN_A_MONTH_3   30
#define NO_OF_DAYS_IN_A_MONTH_4   31
#endif

#endif
//! Application global parameters.
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static rsi_ble_per_transmit_t rsi_ble_per_tx;
static rsi_ble_per_receive_t rsi_ble_per_rx;
static rsi_bt_per_stats_t per_stats;

static const sl_wifi_device_configuration_t config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_BLE_MODE,
#ifdef RSI_M4_INTERFACE
                   .feature_bit_map = (SL_SI91X_FEAT_WPS_DISABLE | RSI_FEATURE_BIT_MAP),
#else
                   .feature_bit_map        = RSI_FEATURE_BIT_MAP,
#endif
#if RSI_TCP_IP_BYPASS
                   .tcp_ip_feature_bit_map = RSI_TCP_IP_FEATURE_BIT_MAP,
#else
                   .tcp_ip_feature_bit_map = (RSI_TCP_IP_FEATURE_BIT_MAP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
#endif
                   .custom_feature_bit_map = (SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID | RSI_CUSTOM_FEATURE_BIT_MAP),
                   .ext_custom_feature_bit_map = (
#ifdef CHIP_917
                     (RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#else //defaults
#ifdef RSI_M4_INTERFACE
                     (SL_SI91X_EXT_FEAT_256K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#else
                     (SL_SI91X_EXT_FEAT_384K_MODE | RSI_EXT_CUSTOM_FEATURE_BIT_MAP)
#endif
#endif
                     | (SL_SI91X_EXT_FEAT_BT_CUSTOM_FEAT_ENABLE)
#if (defined A2DP_POWER_SAVE_ENABLE)
                     | SL_SI91X_EXT_FEAT_XTAL_CLK
#endif
                     ),
                   .bt_feature_bit_map = (RSI_BT_FEATURE_BITMAP
#if (RSI_BT_GATT_ON_CLASSIC)
                                          | SL_SI91X_BT_ATT_OVER_CLASSIC_ACL /* to support att over classic acl link */
#endif
                                          ),
#ifdef RSI_PROCESS_MAX_RX_DATA
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID
                                                  | SL_SI91X_EXT_TCP_MAX_RECV_LENGTH),
#else
                   .ext_tcp_ip_feature_bit_map = (RSI_EXT_TCPIP_FEATURE_BITMAP | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
#endif
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
                   .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | RSI_CONFIG_FEATURE_BITMAP) }
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
/*==============================================*/
/**
 * @fn         M4_sleep_wakeup
 * @brief      Keeps the M4 In the Sleep
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to trigger sleep in the M4 and in the case of the retention submitting the buffer valid
 * to the TA for the rx packets.
 */
/*==============================================*/
#ifdef RSI_M4_INTERFACE
void M4_sleep_wakeup(void)
{
#ifdef ALARM_TIMER_BASED_WAKEUP
  /* Update the alarm time interval, when to get next interrupt  */
  RSI_Set_Alarm_Intr_Time(ALARM_PERIODIC_TIME);

#endif
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
  //fpuInit();
  /*Initialize UART after wake up*/
  sl_service_init();
#endif
}
#endif
/**
 * @fn         ble_per
 * @brief      Tests the BLE PER Modes.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the different BLE PER Modes.
 */
void ble_per(void *unused)
{
  UNUSED_PARAMETER(unused);
  sl_status_t status;
  sl_wifi_version_string_t version = { 0 };

  //! Wi-Fi initialization
  status = sl_wifi_init(&config, default_wifi_event_handler);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  LOG_PRINT("\r\nWireless Initialization Success\n");

#if (defined RSI_M4_INTERFACE && defined ALARM_TIMER_BASED_WAKEUP)
  InitM4AlarmConfig();
#endif

  //! Firmware version Prints
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
  } else {
    LOG_PRINT("\r\nfirmware_version = %s\r\n", version.version);
  }

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Device Address Failed = %lx\r\n", status);
  } else {

    LOG_PRINT("Get local device address: %x:%x:%x:%x\n",
              rsi_app_resp_get_dev_addr[3],
              rsi_app_resp_get_dev_addr[2],
              rsi_app_resp_get_dev_addr[1],
              rsi_app_resp_get_dev_addr[0]);
  }

  //! set the local device name
  status = rsi_bt_set_local_name(RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Set Local Name Failed = %lx\r\n", status);
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Get Local Name Failed = %lx\r\n", status);
  }
  LOG_PRINT("Local name set to: %s\n", rsi_app_resp_get_local_name.name);

  if (RSI_CONFIG_PER_MODE == RSI_BLE_PER_TRANSMIT_MODE) {
    rsi_ble_per_tx.cmd_ix                       = BLE_TRANSMIT_CMD_ID;
    rsi_ble_per_tx.transmit_enable              = ENABLE;
    *(uint32_t *)&rsi_ble_per_tx.access_addr[0] = BLE_ACCESS_ADDR;
    *(uint16_t *)&rsi_ble_per_tx.pkt_len[0]     = BLE_TX_PKT_LEN;
    rsi_ble_per_tx.phy_rate                     = BLE_PHY_RATE;
    rsi_ble_per_tx.rx_chnl_num                  = BLE_RX_CHNL_NUM;
    rsi_ble_per_tx.tx_chnl_num                  = BLE_TX_CHNL_NUM;
    rsi_ble_per_tx.scrambler_seed               = SCRAMBLER_SEED;
    rsi_ble_per_tx.payload_type                 = DATA_FOUR_ZEROES_FOUR_ONES;
    rsi_ble_per_tx.le_chnl_type                 = LE_DATA_CHNL_TYPE;
    rsi_ble_per_tx.tx_power                     = BLE_TX_POWER_INDEX;
    rsi_ble_per_tx.transmit_mode                = BURST_MODE;
    rsi_ble_per_tx.freq_hop_en                  = NO_HOPPING;
    rsi_ble_per_tx.ant_sel                      = ONBOARD_ANT_SEL;
    rsi_ble_per_tx.inter_pkt_gap                = RSI_INTER_PKT_GAP;
    rsi_ble_per_tx.pll_mode                     = PLL_MODE_0;
    rsi_ble_per_tx.rf_type                      = BLE_INTERNAL_RF;
    rsi_ble_per_tx.rf_chain                     = BT_HP_CHAIN_BIT;
    //! start the Transmit PER functionality
    status = rsi_ble_per_transmit(&rsi_ble_per_tx);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per transmit cmd failed : 0x%lX \n", status);
    } else {
      LOG_PRINT("\nRSI_BLE_PER_TRANSMIT_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "access_addr: 0x%lX \n"
                "pkt_len: %d \n"
                "phy_rate: %d \n"
                "rx_chnl_num: %d \n"
                "tx_chnl_num: %d \n"
                "scrambler_seed: %d \n"
                "payload_type: %d \n"
                "le_chnl_type: %d \n"
                "tx_power: %d \n"
                "transmit_mode: %d \n"
                "freq_hop_en: %d \n"
                "ant_sel: %d \n"
                "inter_pkt_gap: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                rsi_ble_per_tx.cmd_ix,
                rsi_ble_per_tx.transmit_enable,
                *(uint32_t *)&rsi_ble_per_tx.access_addr[0],
                (*(uint16_t *)&rsi_ble_per_tx.pkt_len[0]),
                rsi_ble_per_tx.phy_rate,
                rsi_ble_per_tx.rx_chnl_num,
                rsi_ble_per_tx.tx_chnl_num,
                rsi_ble_per_tx.scrambler_seed,
                rsi_ble_per_tx.payload_type,
                rsi_ble_per_tx.le_chnl_type,
                rsi_ble_per_tx.tx_power,
                rsi_ble_per_tx.transmit_mode,
                rsi_ble_per_tx.freq_hop_en,
                rsi_ble_per_tx.ant_sel,
                rsi_ble_per_tx.inter_pkt_gap,
                rsi_ble_per_tx.pll_mode,
                rsi_ble_per_tx.rf_type,
                rsi_ble_per_tx.rf_chain);
    }
  } else if (RSI_CONFIG_PER_MODE == RSI_BLE_PER_RECEIVE_MODE) {
    rsi_ble_per_rx.cmd_ix                       = BLE_RECEIVE_CMD_ID;
    rsi_ble_per_rx.receive_enable               = ENABLE;
    *(uint32_t *)&rsi_ble_per_rx.access_addr[0] = BLE_ACCESS_ADDR;
    rsi_ble_per_rx.ext_data_len_indication      = EXT_DATA_LEN_IND;
    rsi_ble_per_rx.phy_rate                     = BLE_PHY_RATE;
    rsi_ble_per_rx.rx_chnl_num                  = BLE_RX_CHNL_NUM;
    rsi_ble_per_rx.tx_chnl_num                  = BLE_TX_CHNL_NUM;
    rsi_ble_per_rx.scrambler_seed               = SCRAMBLER_SEED;
    rsi_ble_per_rx.le_chnl_type                 = LE_DATA_CHNL_TYPE;
    rsi_ble_per_rx.loop_back_mode               = LOOP_BACK_MODE_DISABLE;
    rsi_ble_per_rx.freq_hop_en                  = NO_HOPPING;
    rsi_ble_per_rx.ant_sel                      = ONBOARD_ANT_SEL;
    rsi_ble_per_rx.duty_cycling_en              = DUTY_CYCLING_DISABLE;
    rsi_ble_per_rx.pll_mode                     = PLL_MODE_0;
    rsi_ble_per_rx.rf_type                      = BLE_INTERNAL_RF;
    rsi_ble_per_rx.rf_chain                     = BT_HP_CHAIN_BIT;
    //! start the Receive PER functionality
    rsi_ble_per_receive(&rsi_ble_per_rx);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per receive cmd failed : %lx \n", status);
    } else {
      LOG_PRINT("\nRSI_BLE_PER_RECEIVE_MODE \n"
                "cmd id: 0x%X \n"
                "enable: %d \n"
                "access_addr: 0x%lX \n"
                "ext_data_len_indication: %d \n"
                "phy_rate: %d \n"
                "rx_chnl_num: %d \n"
                "tx_chnl_num: %d \n"
                "scrambler_seed: %d \n"
                "le_chnl_type: %d \n"
                "loop_back_mode: %d \n"
                "freq_hop_en: %d \n"
                "ant_sel: %d \n"
                "duty_cycling_en: %d \n"
                "pll_mode: %d \n"
                "rf_type: %d \n"
                "rf_chain: %d \n",
                rsi_ble_per_rx.cmd_ix,
                rsi_ble_per_rx.receive_enable,
                *(uint32_t *)&rsi_ble_per_rx.access_addr[0],
                rsi_ble_per_rx.ext_data_len_indication,
                rsi_ble_per_rx.phy_rate,
                rsi_ble_per_rx.rx_chnl_num,
                rsi_ble_per_rx.tx_chnl_num,
                rsi_ble_per_rx.scrambler_seed,
                rsi_ble_per_rx.le_chnl_type,
                rsi_ble_per_rx.loop_back_mode,
                rsi_ble_per_rx.freq_hop_en,
                rsi_ble_per_rx.ant_sel,
                rsi_ble_per_rx.duty_cycling_en,
                rsi_ble_per_rx.pll_mode,
                rsi_ble_per_rx.rf_type,
                rsi_ble_per_rx.rf_chain);
    }
  }

  while (1) {
    status = rsi_bt_per_stats(BT_PER_STATS_CMD_ID, &per_stats);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n per stats cmd failed : %lx \n", status);
    } else {
      LOG_PRINT("\nPER Stats \n"
                "crc_fail_cnt : %d \n"
                "crc_pass_cnt: %d \n"
                "tx_dones: %d \n"
                "rssi: %d \n"
                "id_pkts_rcvd :%d \n",
                per_stats.crc_fail_cnt,
                per_stats.crc_pass_cnt,
                per_stats.tx_dones,
                per_stats.rssi,
                per_stats.id_pkts_rcvd);
    }
#if (defined RSI_M4_INTERFACE)
    if (!(P2P_STATUS_REG & TA_wakeup_M4)) {
      P2P_STATUS_REG &= ~M4_wakeup_TA;
      LOG_PRINT("\r\n triggering M4 sleep\r\n");
      M4_sleep_wakeup();
    }
#endif
  }
  return;
}

void app_init(void)
{

  osThreadNew((osThreadFunc_t)ble_per, NULL, &thread_attributes);
}

#if (defined RSI_M4_INTERFACE && defined ALARM_TIMER_BASED_WAKEUP)
/**
 * @fn           void RSI_Set_Alarm_Time(uint16_t interval)
 * @brief        This function will update the alarm time ,when to get next alarm interrupt .
 * @param[in]    interval  : Alarm  timer interrupt time
 * @param[out]   None
 *
 */
void RSI_Set_Alarm_Intr_Time(uint16_t interval)
{
  /* Get the RTC time,which is used to update alarm time as per RTC time  */
  RSI_RTC_GetDateTime(RTC, &rtc_get_Time);
  /*RTC alarm configuration */
  alarmConfig.DayOfWeek    = rtc_get_Time.DayOfWeek;
  alarmConfig.Month        = rtc_get_Time.Month;
  alarmConfig.Century      = rtc_get_Time.Century;
  alarmConfig.MilliSeconds = rtc_get_Time.MilliSeconds;
  alarmConfig.Day          = rtc_get_Time.Day;
  alarmConfig.Year         = rtc_get_Time.Year;
  alarmConfig.Minute       = rtc_get_Time.Minute;
  alarmConfig.Hour         = rtc_get_Time.Hour;
  alarmConfig.Second       = rtc_get_Time.Second;
  /*Update seconds for next boundary alarm */
  alarmConfig.Second = alarmConfig.Second + (interval % 60);
  if (alarmConfig.Second >= (NO_OF_SECONDS_IN_A_MINUTE)) {
    alarmConfig.Second -= NO_OF_SECONDS_IN_A_MINUTE;
    alarmConfig.Minute += 1;
  }
  /*Update minutes for next boundary alarm */
  alarmConfig.Minute = alarmConfig.Minute + ((interval / 60) % 60);
  if (alarmConfig.Minute >= (NO_OF_MINUTES_IN_AN_HOUR)) {
    alarmConfig.Minute -= NO_OF_MINUTES_IN_AN_HOUR;
    alarmConfig.Hour += 1;
  }
  /*Update hour for next boundary alarm */
  alarmConfig.Hour = alarmConfig.Hour + (interval / 3600) % 24;
  if (alarmConfig.Hour >= (NO_OF_HOURS_IN_A_DAY)) {
    alarmConfig.Hour -= NO_OF_HOURS_IN_A_DAY;
    alarmConfig.Day += 1;
  }
  /*Update month for next boundary alarm */
  if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_1) {
    if (alarmConfig.Month == February) {
      if (alarmConfig.Year % 4) {
        alarmConfig.Day = 1;
        alarmConfig.Month += 1;
      } else if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_2) {
        alarmConfig.Day = 1;
        alarmConfig.Month += 1;
      }
    }
    if (alarmConfig.Month <= July) {
      if (alarmConfig.Month % 2 == 0) {
        if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_3) {
          alarmConfig.Day = 1;
          alarmConfig.Month += 1;
        }
      } else if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarmConfig.Day = 1;
        alarmConfig.Month += 1;
      }

    } else if (alarmConfig.Month % 2 == 0) {
      if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarmConfig.Day = 1;
        alarmConfig.Month += 1;
      }
    } else if (alarmConfig.Day > NO_OF_DAYS_IN_A_MONTH_3) {
      alarmConfig.Day = 1;
      alarmConfig.Month += 1;
    }
  }
  /*Update year  for next boundary alarm */
  if (alarmConfig.Month > (NO_OF_MONTHS_IN_A_YEAR)) {
    alarmConfig.Month = 1;
    alarmConfig.Year += 1;
  }
  /*Set Alarm configuration */
  RSI_RTC_SetAlarmDateTime(RTC, &alarmConfig);
}
/**
 * @fn           void InitM4AlarmConfig(void)
 * @brief        This function is to initialize Alarm block .
 *
 */
void InitM4AlarmConfig(void)
{
  /*Init RTC*/
  RSI_RTC_Init(RTC);

  /*RTC configuration with some default time */
  rtcConfig.DayOfWeek    = Saturday;
  rtcConfig.Month        = March;
  rtcConfig.Day          = 19;
  rtcConfig.Century      = 0;
  rtcConfig.Year         = 19;
  rtcConfig.Hour         = 23;
  rtcConfig.Minute       = 59;
  rtcConfig.Second       = 50;
  rtcConfig.MilliSeconds = 0;
  /*Set the RTC configuration*/
  RSI_RTC_SetDateTime(RTC, &rtcConfig);
  /*Enable Alarm feature*/
  RSI_RTC_AlamEnable(RTC, ENABLE);
  /*Enable RTC ALARM interrupts*/
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Initilization RTC CALIBRATION*/
  RSI_RTC_CalibInitilization();
  /* To calibrate rc and ro */
  RSI_RTC_ROCLK_Calib(TIME_PERIOD, ENABLE, ENABLE, RC_TRIGGER_TIME, ENABLE, ENABLE, RO_TRIGGER_TIME);

  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);

  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);

  /*Enable NVIC for RTC */
  NVIC_EnableIRQ(NVIC_RTC_ALARM);
}

/*RTC Alarm interrupt*/
void RTC_ALARM_IRQHandler(void)
{
  volatile uint32_t statusRead = 0;
  static RTC_TIME_CONFIG_T alarmConfig, readTime;
  /*Get the interrupt status */
  statusRead = RSI_RTC_GetIntrStatus();
  if (statusRead & NPSS_TO_MCU_ALARM_INTR) {
    /*Clear wake up interrupt */
    RSI_RTC_IntrClear(RTC_ALARM_INTR);
  }
  return;
}

void IRQ026_Handler()
{
  volatile uint32_t wakeUpSrc = 0;

  /*Get the wake up source */
  wakeUpSrc = RSI_PS_GetWkpUpStatus();

  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);

  return;
}
#endif
