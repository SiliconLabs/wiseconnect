/***************************************************************************/ /**
* @file sl_si91x_m4_ps.c
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
#if (configUSE_TICKLESS_IDLE == 1)
#include "sl_si91x_power_manager.h"
#endif
#ifdef SL_WIFI_COMPONENT_INCLUDED
#include "sl_si91x_host_interface.h"
#endif
#define SL_SI91X_MCU_WATCHDOG_TIMER 0

#if SL_SI91X_MCU_WATCHDOG_TIMER
#include "rsi_wwdt.h"
#endif

#ifdef SL_SI91X_MCU_BUTTON_BASED_WAKEUP
#include "sl_si91x_driver_gpio.h"
#endif

#ifdef SLI_SI91X_MCU_INTERFACE

#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn

#define RTC_ALARM_INTR         BIT(16)
#define NPSS_GPIO_2            2
#define NPSSGPIO_PIN_MUX_MODE2 2
#define NPSS_GPIO_INTR_LOW     0
#define NPSS_GPIO_DIR_INPUT    1
#define NPSS_GPIO_2_INTR       BIT(2)

/*Update time configurations for next boundary alarm*/
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

#define NVIC_RTC_ALARM                      MCU_CAL_ALARM_IRQn
#define WIRELESS_WAKEUP_IRQHandler_Priority 8

#ifdef SLI_SI91X_MCU_COMMON_FLASH_MODE
#ifdef SLI_SI917B0
#ifdef SLI_SI91X_MCU_4MB_LITE_IMAGE
#define IVT_OFFSET_ADDR 0x8172000 /*<!Application IVT location VTOR offset for B0>  */
#else
#define IVT_OFFSET_ADDR 0x8202000 /*<!Application IVT location VTOR offset for B0>  */
#endif
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset for A0>  */
#endif
#else
#define IVT_OFFSET_ADDR 0x8012000 /*<!Application IVT location VTOR offset for dual flash A0 and B0>  */
#endif
#ifdef SLI_SI917B0
#define WKP_RAM_USAGE_LOCATION 0x24061EFC /*<!Bootloader RAM usage location upon wake up  for B0 */
#else
#define WKP_RAM_USAGE_LOCATION 0x24061000 /*<!Bootloader RAM usage location upon wake up for A0  */
#endif

#if (configUSE_TICKLESS_IDLE == 0)
#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
static RTC_TIME_CONFIG_T rtc_configuration, alarm_configuration, rtc_get_time;
static uint8_t m4_alarm_initialization_done;
#endif
#endif
RTC_TIME_CONFIG_T sl_rtc_get_Time;
uint32_t sl_bf_rtc_ticks, sl_af_rtc_ticks, sl_rtc_ticks;

void set_alarm_interrupt_timer(uint16_t interval);
void wakeup_source_config(void);

#if (configUSE_TICKLESS_IDLE == 0)

#if SL_SI91X_MCU_BUTTON_BASED_WAKEUP

static void gpio_uulp_pin_callback(void);

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
  sl_si91x_gpio_driver_configure_uulp_interrupt((sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_RISE_EDGE,
                                                NPSS_GPIO_2,
                                                (void *)&gpio_uulp_pin_callback);
}

/**
 * @brief  GPIO based wake up IRQ
 * @param  none
 * @return none
 */
static void gpio_uulp_pin_callback(void)
{
  /* clear NPSS GPIO interrupt*/
  RSI_NPSSGPIO_ClrIntr(NPSS_GPIO_2_INTR);
  while (!sl_si91x_gpio_get_uulp_npss_pin(NPSS_GPIO_2))
    ; // waiting for the button release
}
#endif /* SL_SI91X_MCU_BUTTON_BASED_WAKEUP */

#ifdef SL_SI91X_MCU_WIRELESS_BASED_WAKEUP
void IRQ026_Handler()
{
  /*Get the wake up/NPSS interrupt status*/
  RSI_PS_GetWkpUpStatus();

  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);

  return;
}
#endif /* SL_SI91X_MCU_WIRELESS_BASED_WAKEUP */
#if SL_SI91X_MCU_WATCHDOG_TIMER

void IRQ020_Handler(void)
{
  // Clears interrupt
  RSI_WWDT_IntrClear();
  RSI_WWDT_ReStart(MCU_WDT);
}

#endif

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

  /*Initialization of RTC CALIBRATION*/
  RSI_RTC_CalibInitilization();
  /*Set Alarm as a  wake up source to wake up from deep sleep */
  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);
  /*Enable the RTC alarm interrupts */

  m4_alarm_initialization_done = 1;
}
#endif /* SL_SI91X_MCU_ALARM_BASED_WAKEUP */

/**************************************************************************
 * @fn           void sl_si91x_m4_sleep_wakeup()
 * @brief        Keeps the M4 In the Sleep
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
void sl_si91x_m4_sleep_wakeup(void)
{
#if SL_SI91X_MCU_ALARM_BASED_WAKEUP
  /* Initialize the M4 alarm for the first time*/
  if (m4_alarm_initialization_done == false) {
    initialize_m4_alarm();
  }
  /* Update the alarm time interval, when to get next interrupt  */
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);

#endif
#ifdef SL_SI91X_MCU_WIRELESS_BASED_WAKEUP
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
#endif
#if SL_SI91X_MCU_BUTTON_BASED_WAKEUP
  /*Configure the UULP GPIO 2 as wakeup source */
  wakeup_source_config();
#endif

#if SL_SI91X_MCU_WATCHDOG_TIMER
  // Un-masking WDT interrupt
  RSI_WWDT_IntrUnMask();
  // Initializing watchdog-timer (powering up WDT and enabling it to run during CPU sleep mode)
  RSI_WWDT_Init(MCU_WDT);
  // Configure the WDT system reset value
  RSI_WWDT_ConfigSysRstTimer(MCU_WDT, 19);
  // configure the WDT timeout interrupt time
  RSI_WWDT_ConfigIntrTimer(MCU_WDT, 18);
  NVIC_EnableIRQ(NVIC_WDT);
  RSI_WWDT_Start(MCU_WDT);
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
  /*Enable first boot up*/
  RSI_PS_EnableFirstBootUp(1);
  /* Trigger M4 Sleep*/
  sl_si91x_trigger_sleep(SLEEP_WITH_RETENTION,
                         DISABLE_LF_MODE,
                         WKP_RAM_USAGE_LOCATION,
                         (uint32_t)RSI_PS_RestoreCpuContext,
                         IVT_OFFSET_ADDR,
                         RSI_WAKEUP_FROM_FLASH_MODE);

#if (configUSE_TICKLESS_IDLE == 1)

  sl_af_rtc_ticks = sl_si91x_get_rtc_ticks();
  if (sl_af_rtc_ticks > sl_bf_rtc_ticks) {
    sl_rtc_ticks = sl_af_rtc_ticks - sl_bf_rtc_ticks;
  }
  *idle_sleep_time = 0;
#endif // configUSE_TICKLESS_IDLE == 1

#endif

  /* Clear M4_wakeup_TA bit so that NWP will go to sleep after M4 wakeup*/
  sl_si91x_host_clear_sleep_indicator();

#if (configUSE_TICKLESS_IDLE == 0)
  P2P_STATUS_REG |= M4_is_active;
  M4SS_P2P_INTR_SET_REG = RX_BUFFER_VALID;
#endif // configUSE_TICKLESS_IDLE == 0
  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();
}
#endif // #if (configUSE_TICKLESS_IDLE == 0)

#if (configUSE_TICKLESS_IDLE == 1)
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
 * @fn           bool sli_si91x_ta_packet_initiated_to_m4(void)
 * @brief        This function will verify whether there are any pending packets in NWP when m4 is inactive
 * @param[in]    None
 * @param[out]   true: allow to sleep
 *               false: Not allow to the sleep
 *******************************************************************************/
bool sli_si91x_ta_packet_initiated_to_m4(void)
{
  boolean_t sli_p2p_status = true;

  // Indicate M4 is Inactive
  P2P_STATUS_REG &= ~M4_is_active;

  // Wait one more clock cycle to ensure the M4 hardware register is updated
  P2P_STATUS_REG;

  // This delay is introduced to synchronize between the M4 and the NWP.
  for (uint8_t delay = 0; delay < 10; delay++) {
    __ASM("NOP");
  }

  // Verify if the NWP has already initiated a packet to the M4
  // The NWP will clear RX_BUFFER_VALID if a packet has been triggered
  if ((P2P_STATUS_REG & TA_wakeup_M4) || (P2P_STATUS_REG & M4_wakeup_TA)
      || (!(M4SS_P2P_INTR_SET_REG & RX_BUFFER_VALID))) {
    P2P_STATUS_REG |= M4_is_active;
    sli_p2p_status = false;
  } else {
    //Clearing the RX_Buffer valid bit
    M4SS_P2P_INTR_CLR_REG = RX_BUFFER_VALID;
    M4SS_P2P_INTR_CLR_REG;

    TASS_P2P_INTR_MASK_SET = (TX_PKT_TRANSFER_DONE_INTERRUPT | RX_PKT_TRANSFER_DONE_INTERRUPT | TA_WRITING_ON_COMM_FLASH
                              | NWP_DEINIT_IN_COMM_FLASH
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
                              | SIDE_BAND_CRYPTO_DONE
#endif
    );
  }
  return sli_p2p_status;
}
/**************************************************************************
 * @fn           sli_si91x_m4_ta_wakeup_configurations(void)
 * @brief        It is essential to properly configure the NWP and M4 status registers 
 *               to ensure the system resumes normal operation.   
 * @param[in]    None
 * @param[out]   None
 *******************************************************************************/
void sli_si91x_m4_ta_wakeup_configurations(void)
{
  // Indicate M4 is active
  P2P_STATUS_REG |= M4_is_active;

  //indicate M4 buffer availability to NWP
  M4SS_P2P_INTR_SET_REG = RX_BUFFER_VALID;

#ifdef SLI_SI917
  //! Unmask the P2P interrupts
  unmask_ta_interrupt(TX_PKT_TRANSFER_DONE_INTERRUPT | RX_PKT_TRANSFER_DONE_INTERRUPT | TA_WRITING_ON_COMM_FLASH
                      | NWP_DEINIT_IN_COMM_FLASH
#ifdef SLI_SI91X_MCU_FW_UPGRADE_OTA_DUAL_FLASH
                      | M4_IMAGE_UPGRADATION_PENDING_INTERRUPT
#endif
#ifdef SL_SI91X_SIDE_BAND_CRYPTO
                      | SIDE_BAND_CRYPTO_DONE
#endif
  );
#endif
}
#endif // #if (configUSE_TICKLESS_IDLE == 1)
#endif
