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
#define SL_SI91X_MCU_WATCHDOG_TIMER 0

#if SL_SI91X_MCU_WATCHDOG_TIMER
#include "rsi_wwdt.h"
volatile uint32_t wdt_count = 0;
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
extern volatile uint8_t sl_si91x_packet_status;
#if (configUSE_TICKLESS_IDLE == 1)
uint32_t xMaximumPossibleSuppressedTicks = 0;
uint32_t ulTimerCountsForOneTick         = 0;
uint32_t ulStoppedTimerCompensation      = 0;
uint32_t sl_idle_sleep_time              = 0;

/**************************SYStick Registers***********************************/

/* Possible return values for eTaskConfirmSleepModeStatus(). */
typedef enum {
  eAbortSleep =
    0, /* A task has been made ready or a context switch pended since portSUPPORESS_TICKS_AND_SLEEP() was called - abort entering a sleep mode. */
  eStandardSleep,        /* Enter a sleep mode that will not last any longer than the expected idle time. */
  eNoTasksWaitingTimeout /* No tasks are waiting for a timeout so it is safe to enter a sleep mode that can only be exited by an external interrupt. */
} eSleepModeStatus;

#define portNVIC_SYSTICK_CTRL_REG          (*((volatile uint32_t *)0xe000e010))
#define portNVIC_SYSTICK_LOAD_REG          (*((volatile uint32_t *)0xe000e014))
#define portNVIC_SYSTICK_CURRENT_VALUE_REG (*((volatile uint32_t *)0xe000e018))

/* ...then bits in the registers. */
#define portNVIC_SYSTICK_INT_BIT        (1UL << 1UL)
#define portNVIC_SYSTICK_ENABLE_BIT     (1UL << 0UL)
#define portNVIC_SYSTICK_COUNT_FLAG_BIT (1UL << 16UL)
#define portNVIC_PENDSVCLEAR_BIT        (1UL << 27UL)
#define portNVIC_PEND_SYSTICK_CLEAR_BIT (1UL << 25UL)

/* Ensure the SysTick is clocked at the same frequency as the core. */
#define portNVIC_SYSTICK_CLK_BIT (1UL << 2UL)
#define configSYSTICK_CLOCK_HZ   configCPU_CLOCK_HZ

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER 0xffffffUL

/* A fiddle factor to estimate the number of SysTick counts that would have
 * occurred while the SysTick counter is stopped during tickless idle
 * calculations. */
#define portMISSED_COUNTS_FACTOR (45UL)
/*It can be utilized to correct the timer drift between sleep and wake cycles*/
#define DRIFT_VALUE (96)
/******************************************************************************/
eSleepModeStatus eTaskConfirmSleepModeStatus(void);
void vTaskStepTick(const uint32_t xTicksToJump);
uint16_t sli_si91x_apply_time_drift(uint16_t idle_ticks);
#endif

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
}
#endif /* SL_SI91X_MCU_BUTTON_BASED_WAKEUP */
#ifdef SL_SI91X_MCU_WIRELESS_BASED_WAKEUP
void IRQ026_Handler()
{
  /*Get the wake up source */
  RSI_PS_GetWkpUpStatus();

  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);

  return;
}
#endif /* SL_SI91X_MCU_WIRELESS_BASED_WAKEUP */
#if SL_SI91X_MCU_WATCHDOG_TIMER

void IRQ020_Handler(void)
{
  wdt_count++;
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
#ifndef XTAL_OFF
  /*when RO clock calibration is enabled HW has a dependency on XTAl clock,in case of XTAL OFF, to avoid this dependency this part of code should be disabled */
  /*To calibrate rc and ro */
  RSI_RTC_ROCLK_Calib(TIME_PERIOD, ENABLE, ENABLE, RC_TRIGGER_TIME, ENABLE, ENABLE, RO_TRIGGER_TIME);
#endif
  /*Set Alarm as a  wake up source to wake up from deep sleep */
  RSI_PS_SetWkpSources(ALARM_BASED_WAKEUP);
  /*Enable the RTC alarm interrupts */

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
  /*verifying common flash write progress before triggering sleep*/
  return !(sl_si91x_packet_status);
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
  set_alarm_interrupt_timer(sli_si91x_apply_time_drift(*idle_sleep_time));
#else
  set_alarm_interrupt_timer(ALARM_PERIODIC_TIME);
#endif

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

  /* Clear M4_wakeup_TA bit so that TA will go to sleep after M4 wakeup*/
  sl_si91x_host_clear_sleep_indicator();

#if (configUSE_TICKLESS_IDLE == 0)
  P2P_STATUS_REG |= M4_is_active;
  M4SS_P2P_INTR_SET_REG = RX_BUFFER_VALID;

  osDelay(100);
#endif // configUSE_TICKLESS_IDLE == 0
  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();
}

#if (configUSE_TICKLESS_IDLE == 1)

void vPortSuppressTicksAndSleep(uint32_t xExpectedIdleTime)
{
  uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
  uint32_t xModifiableIdleTime;

  /* Make sure the SysTick reload value does not overflow the counter. */
  if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
    xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
  }

  /* Stop the SysTick momentarily.  The time the SysTick is stopped for
         * is accounted for as best it can be, but using the tickless mode will
         * inevitably result in some tiny drift of the time maintained by the
         * kernel with respect to calendar time. */
  portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

  /* Calculate the reload value required to wait xExpectedIdleTime
         * tick periods.  -1 is used because this code will execute part way
         * through one of the tick periods. */
  ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + (ulTimerCountsForOneTick * (xExpectedIdleTime - 1UL));

  if (ulReloadValue > ulStoppedTimerCompensation) {
    ulReloadValue -= ulStoppedTimerCompensation;
  }

  /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
  __asm volatile("cpsid i" ::: "memory");
  __asm volatile("dsb");
  __asm volatile("isb");

  /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
  if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
    /* Restart from whatever is left in the count register to complete
             * this tick period. */
    portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;

    /* Restart SysTick. */
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

    /* Reset the reload register to the value required for normal tick
             * periods. */
    portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

    /* Re-enable interrupts - see comments above the cpsid instruction()
             * above. */
    __asm volatile("cpsie i" ::: "memory");
  } else {
    /* Set the new reload value. */
    portNVIC_SYSTICK_LOAD_REG = ulReloadValue;

    /* Clear the SysTick count flag and set the count value back to
             * zero. */
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

    /* Restart SysTick. */
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

    /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
    xModifiableIdleTime = xExpectedIdleTime;
    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);

    if (xModifiableIdleTime > 0) {
      __asm volatile("dsb" ::: "memory");
      __asm volatile("wfi");
      __asm volatile("isb");
    }

    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately.  see comments above
             * __disable_interrupt() call above. */
    __asm volatile("cpsie i" ::: "memory");
    __asm volatile("dsb");
    __asm volatile("isb");

    /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
    __asm volatile("cpsid i" ::: "memory");
    __asm volatile("dsb");
    __asm volatile("isb");

    /* Disable the SysTick clock without reading the
             * portNVIC_SYSTICK_CTRL_REG register to ensure the
             * portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
             * the time the SysTick is stopped for is accounted for as best it can
             * be, but using the tickless mode will inevitably result in some tiny
             * drift of the time maintained by the kernel with respect to calendar
             * time*/
    portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT);

    /* Determine if the SysTick clock has already counted to zero and
             * been set back to the current reload value (the reload back being
             * correct for the entire expected idle time) or if the SysTick is yet
             * to count to zero (in which case an interrupt other than the SysTick
             * must have brought the system out of sleep mode). */
    if ((portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT) != 0) {
      uint32_t ulCalculatedLoadValue;

      /* The tick interrupt is already pending, and the SysTick count
                 * reloaded with ulReloadValue.  Reset the
                 * portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
                 * period. */
      ulCalculatedLoadValue = (ulTimerCountsForOneTick - 1UL) - (ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG);

      /* Don't allow a tiny value, or values that have somehow
                 * underflowed because the post sleep hook did something
                 * that took too long. */
      if ((ulCalculatedLoadValue < ulStoppedTimerCompensation) || (ulCalculatedLoadValue > ulTimerCountsForOneTick)) {
        ulCalculatedLoadValue = (ulTimerCountsForOneTick - 1UL);
      }

      portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

      /* As the pending tick will be processed as soon as this
                 * function exits, the tick value maintained by the tick is stepped
                 * forward by one less than the time spent waiting. */
      ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
    } else {
      /* Something other than the tick interrupt ended the sleep.
                 * Work out how long the sleep lasted rounded to complete tick
                 * periods (not the ulReload value which accounted for part
                 * ticks). */
      ulCompletedSysTickDecrements = (xExpectedIdleTime * ulTimerCountsForOneTick) - portNVIC_SYSTICK_CURRENT_VALUE_REG;

      /* How many complete tick periods passed while the processor
                 * was waiting? */
      ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

      /* The reload value is set to whatever fraction of a single tick
                 * period remains. */
      portNVIC_SYSTICK_LOAD_REG =
        ((ulCompleteTickPeriods + 1UL) * ulTimerCountsForOneTick) - ulCompletedSysTickDecrements;
    }

    /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
             * again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
             * value. */
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
    portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
    vTaskStepTick(ulCompleteTickPeriods);
    portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

    /* Exit with interrupts enabled. */
    __asm volatile("cpsie i" ::: "memory");
  }
  // Indicate M4 is active
  P2P_STATUS_REG |= M4_is_active;

  //indicate M4 buffer availability to TA
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

void vPortSetupTimerInterrupt(void)
{

  /* Calculate the constants required to configure the tick interrupt. */
#if (configUSE_TICKLESS_IDLE == 1)
  {
    ulTimerCountsForOneTick         = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
    xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
    ulStoppedTimerCompensation      = portMISSED_COUNTS_FACTOR / (configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ);
  }
#endif /* configUSE_TICKLESS_IDLE */

  /* Stop and clear the SysTick. */
  portNVIC_SYSTICK_CTRL_REG          = 0UL;
  portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

  /* Configure SysTick to interrupt at the requested rate. */
  portNVIC_SYSTICK_LOAD_REG = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
  portNVIC_SYSTICK_CTRL_REG = (portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT);
}

uint16_t sli_si91x_apply_time_drift(uint16_t idle_ticks)
{
  return ((idle_ticks * DRIFT_VALUE) / 100);
}
#endif /* #if configUSE_TICKLESS_IDLE */

#endif