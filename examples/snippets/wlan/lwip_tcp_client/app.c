/***************************************************************************/ /**
 * @file
 * @brief TCP Client Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
 ******************************************************************************/

#include "lwip/errno.h"
#include "sl_wifi_callback_framework.h"
#include "sl_status.h"
#include "sl_board_configuration.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_utility.h"
#include "sl_net.h"
#include <string.h>
#include <stdint.h>
#include "lwip/sockets.h"
#include "sl_net_for_lwip.h"
#include "sl_si91x_driver.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_rtc.h"
#include "rsi_power_save.h"
#include "rsi_chip.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_m4.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
/******************************************************
 *                    Constants
 ******************************************************/
#define ALARM_TIMER_BASED_WAKEUP 1
#define WIRELESS_WAKEUP          0

#define SERVER_IP         "192.168.0.206"
#define SERVER_PORT       5001
#define NUMBER_OF_PACKETS 20
#define DATA              "hello from tcp client"

#ifdef RSI_M4_INTERFACE

#define ALARM_PERIODIC_TIME 1 /*<! periodic alarm configuration in SEC */

#define RTC_ALARM_INTR BIT(16)

/*Update time configurations for next boundary alarm*/
#define RC_TRIGGER_TIME           5
#define RO_TRIGGER_TIME           0
#define NO_OF_HOURS_IN_A_DAY      24
#define NO_OF_MINUTES_IN_AN_HOUR  60
#define NO_OF_SECONDS_IN_A_MINUTE 60
#define NO_OF_MONTHS_IN_A_YEAR    12
#define BASE_YEAR                 2000
#define NO_OF_DAYS_IN_A_MONTH_1   28
#define NO_OF_DAYS_IN_A_MONTH_2   29
#define NO_OF_DAYS_IN_A_MONTH_3   30
#define NO_OF_DAYS_IN_A_MONTH_4   31
#define RTC_ALARM_IRQHandler      IRQ028_Handler
#define NVIC_RTC_ALARM            MCU_CAL_ALARM_IRQn
#endif

#define WIRELESS_WAKEUP_IRQHandler           NPSS_TO_MCU_WIRELESS_INTR_IRQn
#define WIRELESS_WAKEUP_IRQHandler_Periority 8

/******************************************************
 *               Variable Definitions
 ******************************************************/

const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

#ifdef RSI_M4_INTERFACE
#ifdef COMMON_FLASH_EN
#ifdef CHIP_917B0
#define IVT_OFFSET_ADDR 0x81C2000 /*<!Application IVT location VTOR offset>          B0 common flash Board*/
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset>          A0 Common flash Board*/
#endif
#else
#define IVT_OFFSET_ADDR \
  0x8012000 /*<!Application IVT location VTOR offset>          Dual Flash  (both A0 and B0) Board*/
#endif
#ifdef CHIP_917B0
#define WKP_RAM_USAGE_LOCATION \
  0x24061EFC /*<!Bootloader RAM usage location upon wake up  */ // B0 Boards (common flash & Dual flash)
#else
#define WKP_RAM_USAGE_LOCATION \
  0x24061000 /*<!Bootloader RAM usage location upon wake up  */ // A0 Boards (common flash & Dual flash)
#endif
#endif

static const sl_wifi_device_configuration_t sl_wifi_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                   .coex_mode = SL_SI91X_WLAN_MODE,
                   .feature_bit_map =
#ifdef RSI_M4_INTERFACE
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
                      | SL_SI91X_FEAT_AGGREGATION),
#else
                     (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_AGGREGATION),
#endif
                   .tcp_ip_feature_bit_map     = (SL_SI91X_TCP_IP_FEAT_BYPASS | SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                   .custom_feature_bit_map     = SL_SI91X_FEAT_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS |
#ifndef RSI_M4_INTERFACE
                                                  RAM_LEVEL_NWP_ALL_MCU_ZERO
#else
                                                  RAM_LEVEL_NWP_BASIC_MCU_ADV
#endif
#ifdef CHIP_917
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map = (SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
};

static sl_net_wifi_lwip_context_t wifi_client_context;

#ifdef RSI_M4_INTERFACE
static RTC_TIME_CONFIG_T rtc_configuration, alarm_configuration, rtc_get_time;
#endif
/******************************************************
 *               Function Declarations
 ******************************************************/
static void application_start(void *argument);
void send_data_to_tcp_server();

#ifdef RSI_M4_INTERFACE
void sli_m4_ta_interrupt_init(void);
void initialize_m4_alarm(void);
void set_alarm_interrupt_timer(uint16_t interval);
void M4_sleep_wakeup(void);
#endif

/******************************************************
 *               Function Definitions
 ******************************************************/

void app_init(const void *unused)
{
  UNUSED_PARAMETER(unused);
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  sl_status_t status;
  sl_wifi_performance_profile_t performance_profile = { .profile = ASSOCIATED_POWER_SAVE };
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &sl_wifi_client_configuration, &wifi_client_context, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi Client interface success\r\n");

#ifdef RSI_M4_INTERFACE
  uint8_t xtal_enable = 1;
  status              = sl_si91x_m4_ta_secure_handshake(SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring m4_ta_secure_handshake: 0x%lx\r\n", status);
    return;
  }
  printf("m4_ta_secure_handshake Success\r\n");
#endif

  status = sl_net_up(SL_NET_WIFI_CLIENT_INTERFACE, SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi client interface up: 0x%lx\r\n", status);
    return;
  }
  printf("Wi-Fi client connected\r\n");

  status = sl_wifi_filter_broadcast(5000, 1, 1);
  if (status != SL_STATUS_OK) {
    printf("\r\nsl_wifi_filter_broadcast Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("Filter Broadcast Done\r\n");

  // set performance profile
  status = sl_wifi_set_performance_profile(&performance_profile);
  if (status != SL_STATUS_OK) {
    printf("\r\nPower save configuration Failed, Error Code : 0x%lX\r\n", status);
    return;
  }
  printf("\r\nPerformance profile set\r\n");

#ifdef RSI_M4_INTERFACE
#if ALARM_TIMER_BASED_WAKEUP
  initialize_m4_alarm();
#endif
#endif

  send_data_to_tcp_server();
  printf("\r\nExample Demonstration completed\r\n");
}

void send_data_to_tcp_server()
{
  int client_socket = -1;
  int return_value  = 0;
  int sent_bytes;
  int packet_count     = 0;
  sl_ipv4_address_t ip = { 0 };

  struct sockaddr_in server_address = { 0 };
  socklen_t socket_length           = sizeof(struct sockaddr_in);

  sl_net_inet_addr(SERVER_IP, (uint32_t *)&ip);

  server_address.sin_family      = AF_INET;
  server_address.sin_port        = htons(SERVER_PORT);
  server_address.sin_addr.s_addr = ip.value;

  //!Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("\r\nSocket creation failed with bsd error: %d\r\n", errno);
    return;
  }
  printf("\r\nTCP Socket Create Success\r\n");

  //! Socket connect
  return_value = connect(client_socket, (struct sockaddr *)&server_address, socket_length);
  if (return_value < 0) {
    printf("\r\nSocket connect failed with bsd error: %d\r\n", errno);
    close(client_socket);
    return;
  }
  printf("\r\nTCP Socket Connect Success\r\n");

  //! send data
  while (packet_count < NUMBER_OF_PACKETS) {
    sent_bytes = send(client_socket, DATA, strlen(DATA), 0);
    if (sent_bytes < 0) {
      printf("\r\nSend failed with bsd error:%d\r\n", errno);
      close(client_socket);
      return;
    }
    packet_count++;
#ifdef RSI_M4_INTERFACE
    M4_sleep_wakeup();
#endif
  }

  printf("\r\n%d packets sent success\r\n", packet_count);

  //!Socket close
  close(client_socket);
  printf("\r\nSocket close success\r\n");
  return;
}

#ifdef RSI_M4_INTERFACE

void M4_sleep_wakeup(void)
{

#if WIRELESS_WAKEUP
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQHandler_Periority);
  /* Enable NVIC */
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);
#endif

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
#if ALARM_TIMER_BASED_WAKEUP
  /* Update the alarm time interval, when to get next interrupt  */
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);

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
  sli_m4_ta_interrupt_init();
#endif
}
#if ALARM_TIMER_BASED_WAKEUP
void set_alarm_interrupt_timer(uint16_t interval)
{
  /* Get the RTC time,which is used to update alarm time as per RTC time  */
  RSI_RTC_GetDateTime(RTC, &rtc_get_time);

  /*RTC alarm configuration */
  alarm_configuration.DayOfWeek    = rtc_get_time.DayOfWeek;
  alarm_configuration.Month        = rtc_get_time.Month;
  alarm_configuration.Century      = rtc_get_time.Century;
  alarm_configuration.MilliSeconds = rtc_get_time.MilliSeconds;
  alarm_configuration.Day          = rtc_get_time.Day;
  alarm_configuration.Year         = rtc_get_time.Year;
  alarm_configuration.Minute       = rtc_get_time.Minute;
  alarm_configuration.Hour         = rtc_get_time.Hour;
  alarm_configuration.Second       = rtc_get_time.Second;

  /*Update seconds for next boundary alarm */
  alarm_configuration.Second = alarm_configuration.Second + (interval % 60);
  if (alarm_configuration.Second >= (NO_OF_SECONDS_IN_A_MINUTE)) {
    alarm_configuration.Second -= NO_OF_SECONDS_IN_A_MINUTE;
    alarm_configuration.Minute += 1;
  }

  /*Update minutes for next boundary alarm */
  alarm_configuration.Minute = alarm_configuration.Minute + ((interval / 60) % 60);
  if (alarm_configuration.Minute >= (NO_OF_MINUTES_IN_AN_HOUR)) {
    alarm_configuration.Minute -= NO_OF_MINUTES_IN_AN_HOUR;
    alarm_configuration.Hour += 1;
  }

  /*Update hour for next boundary alarm */
  alarm_configuration.Hour = alarm_configuration.Hour + (interval / 3600) % 24;
  if (alarm_configuration.Hour >= (NO_OF_HOURS_IN_A_DAY)) {
    alarm_configuration.Hour -= NO_OF_HOURS_IN_A_DAY;
    alarm_configuration.Day += 1;
  }

  /*Update month for next boundary alarm */
  if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_1) {
    if (alarm_configuration.Month == February) {
      if (alarm_configuration.Year % 4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_2) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    }
    if (alarm_configuration.Month <= July) {
      if (alarm_configuration.Month % 2 == 0) {
        if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
          alarm_configuration.Day = 1;
          alarm_configuration.Month += 1;
        }
      } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }

    } else if (alarm_configuration.Month % 2 == 0) {
      if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_4) {
        alarm_configuration.Day = 1;
        alarm_configuration.Month += 1;
      }
    } else if (alarm_configuration.Day > NO_OF_DAYS_IN_A_MONTH_3) {
      alarm_configuration.Day = 1;
      alarm_configuration.Month += 1;
    }
  }

  /*Update year  for next boundary alarm */
  if (alarm_configuration.Month > (NO_OF_MONTHS_IN_A_YEAR)) {
    alarm_configuration.Month = 1;
    alarm_configuration.Year += 1;
  }

  /*Set Alarm configuration */
  RSI_RTC_SetAlarmDateTime(RTC, &alarm_configuration);
}

void initialize_m4_alarm(void)
{
  /*Init RTC*/
  RSI_RTC_Init(RTC);

  /*RTC configuration with some default time */
  rtc_configuration.DayOfWeek    = Saturday;
  rtc_configuration.Month        = March;
  rtc_configuration.Day          = 19;
  rtc_configuration.Century      = 0;
  rtc_configuration.Year         = 19;
  rtc_configuration.Hour         = 23;
  rtc_configuration.Minute       = 59;
  rtc_configuration.Second       = 50;
  rtc_configuration.MilliSeconds = 0;

  /*Set the RTC configuration*/
  RSI_RTC_SetDateTime(RTC, &rtc_configuration);
  /*Enable Alarm feature*/
  RSI_RTC_AlamEnable(RTC, ENABLE);
  /*Enable RTC ALARM interrupts*/
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Initialization of RTC CALIBRATION*/
  RSI_RTC_CalibInitilization();
  /*To calibrate rc and ro */
  RSI_RTC_ROCLK_Calib(TIME_PERIOD, ENABLE, ENABLE, RC_TRIGGER_TIME, ENABLE, ENABLE, RO_TRIGGER_TIME);
  /*Set Alarm as a  wake up source to wake up from deep sleep */
  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);
  /*Enable the RTC alarm interrupts */
  RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
  /*Enable NVIC for RTC */
  NVIC_EnableIRQ(NVIC_RTC_ALARM);
}
/*RTC Alarm interrupt*/
void RTC_ALARM_IRQHandler(void)
{
  volatile uint32_t statusRead = 0;
  /*Get the interrupt status */
  statusRead = RSI_RTC_GetIntrStatus();

  if (statusRead & NPSS_TO_MCU_ALARM_INTR) {

    /* TRIGGER SLEEP STATE */
    /*Clear wake up interrupt */
    RSI_RTC_IntrClear(RTC_ALARM_INTR);
  }
  return;
}
#endif
void IRQ026_Handler()
{
  RSI_PS_GetWkpUpStatus();
  /*Clear interrupt */
  ps_clr_wkp_up_status(NPSS_TO_MCU_WIRELESS_INTR);
  return;
}
#endif