/***************************************************************************
 * @file sl_si91x_m4_ps.h
 * @brief  M4 power save
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licenser of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "sl_si91x_m4_ps.h"
#include "FreeRTOSConfig.h"
#include "cmsis_os2.h"
#include "sl_rsi_utility.h"
#ifdef SL_WIFI_COMPONENT_INCLUDED
#include "sl_si91x_host_interface.h"
#endif

#ifdef SLI_SI91X_MCU_INTERFACE

#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn

#define RTC_ALARM_INTR         BIT(16)
#define NPSS_GPIO_2            2
#define NPSSGPIO_PIN_MUX_MODE2 2
#define NPSS_GPIO_INTR_LOW     0
#define NPSS_GPIO_DIR_INPUT    1
#define NPSS_GPIO_2_INTR       BIT(2)

#define RC_TRIGGER_TIME                5
#define RO_TRIGGER_TIME                0
#define NO_OF_HOURS_IN_A_DAY           24
#define NO_OF_MINUTES_IN_AN_HOUR       60
#define NO_OF_SECONDS_IN_A_MINUTE      60
#define NO_OF_MONTHS_IN_A_YEAR         12
#define BASE_YEAR                      2000
#define NO_OF_DAYS_IN_A_MONTH_1        28
#define NO_OF_DAYS_IN_A_MONTH_2        29
#define NO_OF_DAYS_IN_A_MONTH_3        30
#define NO_OF_DAYS_IN_A_MONTH_4        31
#define NO_OF_MILLISECONDS_IN_A_SECOND 1000

#define ALARM_PERIODIC_TIME \
  30 * NO_OF_MILLISECONDS_IN_A_SECOND /*<! periodic alarm configuration in     \
                                         milliseconds */

#define RTC_ALARM_IRQHandler                IRQ028_Handler
#define NVIC_RTC_ALARM                      MCU_CAL_ALARM_IRQn
#define WIRELESS_WAKEUP_IRQHandler_Priority 8

#ifdef SLI_SI91X_MCU_COMMON_FLASH_MODE
#ifdef SLI_SI917B0
#define IVT_OFFSET_ADDR 0x8202000 /*<!Application IVT location VTOR offset for B0>  */
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset for A0>  */
#endif
#else
#define IVT_OFFSET_ADDR \
  0x8012000 /*<!Application IVT location VTOR offset for dual flash A0 and B0> \
             */
#endif
#ifdef SLI_SI917B0
#define WKP_RAM_USAGE_LOCATION 0x24061EFC /*<!Bootloader RAM usage location upon wake up  for B0 */
#else
#define WKP_RAM_USAGE_LOCATION 0x24061000 /*<!Bootloader RAM usage location upon wake up for A0  */
#endif

#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
static RTC_TIME_CONFIG_T rtc_configuration, alarm_configuration, rtc_get_time;
static uint8_t m4_alarm_initialization_done;
#endif

RTC_TIME_CONFIG_T sl_rtc_get_Time;
uint32_t sl_bf_rtc_ticks, sl_af_rtc_ticks, sl_rtc_ticks;

void sli_m4_ta_interrupt_init(void);
void set_alarm_interrupt_timer(uint16_t interval);
void vPortSetupTimerInterrupt(void);

extern osEventFlagsId_t si91x_events;
extern osEventFlagsId_t si91x_bus_events;
extern osEventFlagsId_t si91x_async_events;

bool is_sleep_ready = false;

#if SL_SI91X_MCU_BUTTON_BASED_WAKEUP
/**
 * @brief  Configure the UULP GPIO 2 as wakeup source
 * @param  none
 * @return none
 */
void wakeup_source_config(void)
{
  /*Configure the NPSS GPIO mode to wake up  */
  RSI_NPSSGPIO_SetPinMux(NPSS_GPIO_2, NPSSGPIO_PIN_MUX_MODE2);

  /*Configure the NPSS GPIO direction to input */
  RSI_NPSSGPIO_SetDir(NPSS_GPIO_2, NPSS_GPIO_DIR_INPUT);

  /*Configure the NPSS GPIO interrupt polarity */
  RSI_NPSSGPIO_SetPolarity(NPSS_GPIO_2, NPSS_GPIO_INTR_LOW);

  /*Enable the REN*/
  RSI_NPSSGPIO_InputBufferEn(NPSS_GPIO_2, 1);

  /* Set the GPIO to wake from deep sleep */
  RSI_NPSSGPIO_SetWkpGpio(NPSS_GPIO_2_INTR);

  /* Un mask the NPSS GPIO interrupt*/
  RSI_NPSSGPIO_IntrUnMask(NPSS_GPIO_2_INTR);

  /*Select wake up sources */
  RSI_PS_SetWkpSources(GPIO_BASED_WAKEUP);

  /*Enable the NPSS GPIO interrupt slot*/
  NVIC_EnableIRQ(NPSS_TO_MCU_GPIO_INTR_IRQn);
}

/**
 * @brief  GPIO based wake up IRQ
 * @param  none
 * @return none
 */
__attribute__((weak)) void IRQ021_Handler(void)
{
  /* clear NPSS GPIO interrupt*/
  RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
  // LOG_PRINT("BUTTON_PRESSED");
}
#endif /* SL_SI91X_MCU_BUTTON_BASED_WAKEUP */
#ifdef SL_SI91X_MCU_WIRELESS_BASED_WAKEUP
void IRQ026_Handler()
{
  // volatile uint32_t wakeUpSrc = 0;

  /*Get the wake up source */
  // wakeUpSrc = RSI_PS_GetWkpUpStatus();
  RSI_PS_GetWkpUpStatus();

  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);
  // LOG_PRINT("\r\n received packet from sleep \r\n");
  return;
}
#endif /* SL_SI91X_MCU_WIRELESS_BASED_WAKEUP */

#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
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

  alarm_configuration.MilliSeconds += (interval % 1000);
  if (alarm_configuration.MilliSeconds >= (NO_OF_MILLISECONDS_IN_A_SECOND)) {
    alarm_configuration.MilliSeconds -= NO_OF_MILLISECONDS_IN_A_SECOND;
    alarm_configuration.Second += 1;
  }

  /*Update seconds for next boundary alarm */
  alarm_configuration.Second = alarm_configuration.Second + (interval / 1000 % 60);
  if (alarm_configuration.Second >= (NO_OF_SECONDS_IN_A_MINUTE)) {
    alarm_configuration.Second -= NO_OF_SECONDS_IN_A_MINUTE;
    alarm_configuration.Minute += 1;
  }

  /*Update minutes for next boundary alarm */
  alarm_configuration.Minute = alarm_configuration.Minute + ((interval / (1000 * 60)) % 60);
  if (alarm_configuration.Minute >= (NO_OF_MINUTES_IN_AN_HOUR)) {
    alarm_configuration.Minute -= NO_OF_MINUTES_IN_AN_HOUR;
    alarm_configuration.Hour += 1;
  }

  /*Update hour for next boundary alarm */
  alarm_configuration.Hour = alarm_configuration.Hour + (interval / (1000 * 3600)) % 24;
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
  m4_alarm_initialization_done = 1;
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
#endif /* SL_SI91X_MCU_ALARM_BASED_WAKEUP */

/**************************************************************************
 * @fn           sl_si91x_get_rtc_ticks(void)
 * @brief        In this function convert the RTC time into the Ticks
 * @param[in]    None
 * @param[out]   sl_ps_rtc_ticks :  RTC value in Ticks
 *******************************************************************************/
uint32_t sl_si91x_get_rtc_ticks(void)
{
  volatile uint32_t sl_ps_rtc_ticks = 0;
  RSI_RTC_GetDateTime(RTC, &sl_rtc_get_Time);
  sl_ps_rtc_ticks = sl_rtc_get_Time.MilliSeconds + sl_rtc_get_Time.Second * 1000 + sl_rtc_get_Time.Minute * 60 * 1000
                    + sl_rtc_get_Time.Hour * 60 * 60 * 1000;
  // TODO: Add 24 hour spill over logic later once the device is stable for 24 hours,
  // here there will be one period which will give negative outcome at 24 hour mark
  return sl_ps_rtc_ticks;
}

/**************************************************************************
 * @fn           sl_si91x_post_sleep_update_ticks(uint32_t *xExpectedIdleTime)
 * @brief        In this function xExpectedIdleTime > sl_rtc_ticks
 *               appending the sl_rtc_ticks to the expected idle time
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
void sl_si91x_post_sleep_update_ticks(uint32_t *xExpectedIdleTime)
{
  if ((sl_rtc_ticks > 0) && (*xExpectedIdleTime > sl_rtc_ticks)) {
    *xExpectedIdleTime = sl_rtc_ticks;
  }
}
/**************************************************************************
 * @fn           sli_si91x_is_sleep_ready()
 * @brief        This function checks the readiness of the SI91x device for
 *sleep.
 * @return        The status indicating whether the device is ready for sleep.
 *                - Returns a non-zero value if the device is ready for sleep.
 *                - Returns 0 if the device is not ready for sleep.
 *******************************************************************************/
uint32_t sli_si91x_is_sleep_ready()
{
  if ((osEventFlagsGet(si91x_events) | osEventFlagsGet(si91x_bus_events) | osEventFlagsGet(si91x_async_events))
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
      || (osMutexGetOwner(side_band_crypto_mutex) != NULL)
#endif // SL_SI91X_SIDE_BAND_CRYPTO
      || ((sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_COMMON_CMD)
           | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_WLAN_CMD)
           | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_NETWORK_CMD)
           | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_SOCKET_CMD)
           | sl_si91x_host_queue_status((sl_si91x_queue_type_t)SI91X_BT_CMD)))) {
    return 0;
  }
  return 1;
}

/**************************************************************************
 * @fn           vApplicationIdleHook(void)
 * @brief        The idle hook is called repeatedly as long as the idle task is
 *running. Verifying whether the WiFi allows M4 to enter sleep mode or not.
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
/*
void vApplicationIdleHook(void)
{
  // Users can incorporate their APIs into the applicationIdleHook according to
  // their application usage patterns.

  // Please refer to the commented code below for reference.
  is_sleep_ready = sli_si91x_is_sleep_ready();
}
*/

/**************************************************************************
 * @fn           sl_si91x_pre_supress_ticks_and_sleep(uint16_t
 **xExpectedIdleTime)
 * @brief        set xExpectedIdleTime to 0 if the application prevents the
 *device Sleep
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
void sl_si91x_pre_supress_ticks_and_sleep(uint16_t *xExpectedIdleTime)
{
  // Verify the conditions preventing sleep and adjust xExpectedIdleTime to 0
  // accordingly to prevent the device from entering sleep mode

  // Please refer to the commented code below for reference.
  /*
  if (!is_sleep_ready) {
    *xExpectedIdleTime = 0;
  }
  */

  (void)*xExpectedIdleTime; // This addition is made to prevent the occurrence
                            // of errors.
}

/**************************************************************************
 * @fn           void sl_si91x_m4_sleep_wakeup()
 * @brief        Keeps the M4 In the Sleep
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
#if (configUSE_TICKLESS_IDLE == 1)
void sl_si91x_m4_sleep_wakeup(uint16_t *idle_sleep_time)
#else
void sl_si91x_m4_sleep_wakeup(void)
#endif
{

#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
  /* Initialize the M4 alarm for the first time*/
  if (m4_alarm_initialization_done == false) {
    initialize_m4_alarm();
  }

  /* Update the alarm time interval, when to get next interrupt  */
#if (configUSE_TICKLESS_IDLE == 1) // configUSE_TICKLESS_IDLE == 1
  set_alarm_interrupt_timer(*idle_sleep_time);
#else
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);
#endif

#endif
#ifdef SL_SI91X_MCU_WIRELESS_BASED_WAKEUP
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);

  /* sets the priority of an Wireless wakeup interrupt. */
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQ_PRI);

  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);
#endif
#if SL_SI91X_MCU_BUTTON_BASED_WAKEUP
  /*Configure the UULP GPIO 2 as wakeup source */
  wakeup_source_config();
#endif

#ifndef SLI_SI91X_MCU_ENABLE_FLASH_BASED_EXECUTION
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

#if (configUSE_TICKLESS_IDLE == 1)
  sl_bf_rtc_ticks = sl_si91x_get_rtc_ticks();
#endif // configUSE_TICKLESS_IDLE == 1

#if SL_SI91X_SI917_RAM_MEM_CONFIG == 1
  /* Configure 192K RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);
#elif SL_SI91X_SI917_RAM_MEM_CONFIG == 2
  /* Configure 256K RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_256KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);
#elif SL_SI91X_SI917_RAM_MEM_CONFIG == 3
  /* Configure 320K RAM Usage and Retention Size */
  sl_si91x_configure_ram_retention(WISEMCU_320KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);
#endif

  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);

#if (configUSE_TICKLESS_IDLE == 1)
  NVIC_SetPriority(SVCall_IRQn, 0);
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);

  sl_af_rtc_ticks = sl_si91x_get_rtc_ticks();
  if (sl_af_rtc_ticks > sl_bf_rtc_ticks) {
    sl_rtc_ticks = sl_af_rtc_ticks - sl_bf_rtc_ticks;
  }
  *idle_sleep_time = 0;
#endif // configUSE_TICKLESS_IDLE == 1

#endif

  /* Enable M4_TA interrupt */
  sli_m4_ta_interrupt_init();

  /* Clear M4_wakeup_TA bit so that TA will go to sleep after M4 wakeup*/
  sl_si91x_host_clear_sleep_indicator();

#if (configUSE_TICKLESS_IDLE == 0)
  osDelay(100);
#endif // configUSE_TICKLESS_IDLE == 0
  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();
}

#endif