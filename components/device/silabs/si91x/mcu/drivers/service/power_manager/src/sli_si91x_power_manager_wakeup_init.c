/***************************************************************************/ /**
 * @file sli_si91x_power_manager_wakeup_initialization.c
 * @brief Power Manager Wakeup Source Initialization API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_si91x_power_manager.h"
#include "sl_si91x_power_manager_wakeup_handler.h"
#include "rsi_rom_power_save.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define DS_IRQ                   IRQ027_Handler     // DS IRQ Handler
#define NVIC_DS_TIMER            NPSS_MCU_INTR_IRQn // DS Timer NVIC enable
#define WIRELESS_WAKEUP_IRQ      IRQ026_Handler     // Wireless Wakeup IRQ Handler
#define WIRELESS_WAKEUP_Priority 8                  // Wireless Wakeup IRQ priority
#define UULP_GPIO_0              0                  // UULP GPIO Pin 0
#define UULP_GPIO_1              1                  // UULP GPIO Pin 1
#define UULP_GPIO_2              2                  // UULP GPIO Pin 2
#define UULP_GPIO_3              3                  // UULP GPIO Pin 3
#define POLARITY_LOW             0                  // low polarity
#define POLARITY_HIGH            1                  // High polarity
#define CALENDAR_RC_CLOCK        2                  // RC clock value for Calendar RTC
#define NO_OF_MSEC_IN_SEC        1000               // Number of milliseconds in second
#define NO_OF_SEC_IN_MIN         60                 // Number of seconds in a minute
#define NO_OF_MIN_IN_HOUR        60                 // Number of minutes in an hour
#define NO_OF_HOUR_IN_DAY        24                 // Number of hour in a day
#define NO_OF_DAY_IN_MONTH_1     28                 // Number of days in february month
#define NO_OF_DAY_IN_MONTH_2     29                 // Number of days in february month which has leap year
#define NO_OF_DAY_IN_MONTH_3     30                 // Number of days in even month
#define NO_OF_DAY_IN_MONTH_4     31                 // Number of days in odd month
#define NO_OF_MONTH_IN_YEAR      12                 // Number of months in a year
#define NEXT_OCCURENECE          1                  // Next time occurence
#define RESET_DAY                1                  // Reset day after incrementing month
#define RESET_MONTH              1                  // Reset month after incrementing year
#define LEAP_YEAR_VALIDATION     4                  // Value to validate the leap year
#define DAY_IN_MONTH_VALIDATION  2                  // Value to validate the days in month
#define CENTURY                  2                  // Default Century to be configured
#define YEAR                     1                  // Default year to be configured
#define DAY                      2                  // Default day to be configured
#define HOUR                     18                 // Default hour to be configured
#define MINUTE                   10                 // Default minute to be configured
#define SECONDS                  10                 // Default seconds to be configured
#define MILLI_SECONDS            100                // Default milli seconds to be configured

/*******************************************************************************
 ***************************  Local Types  ********************************
 ******************************************************************************/
/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
#if defined(SL_ENABLE_CALENDAR_WAKEUP_SOURCE) && (SL_ENABLE_CALENDAR_WAKEUP_SOURCE == ENABLE)
static void calendar_callback_function(void);
#if defined(ENABLE_ALARM) && (ENABLE_ALARM == ENABLE)
static void set_periodic_alarm(uint32_t alarm_time);
#endif // ENABLE_ALARM
#endif // SL_ENABLE_CALENDAR_WAKEUP_SOURCE

#if defined(SL_ENABLE_GPIO_WAKEUP_SOURCE) && (SL_ENABLE_GPIO_WAKEUP_SOURCE == ENABLE)
__attribute__((weak)) void gpio_uulp_pin_interrupt_callback(uint32_t pin_intr);
static sl_status_t uulp_gpio_configuration(uint8_t pin);
#endif // SL_ENABLE_GPIO_WAKEUP_SOURCE

#if defined(SL_ENABLE_WDT_WAKEUP_SOURCE) && (SL_ENABLE_WDT_WAKEUP_SOURCE == ENABLE)
static void wdt_callback_function(void);
#endif // SL_ENABLE_WDT_WAKEUP_SOURCE

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/
/*******************************************************************************
 *
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_calendar_init(void)
{
  sl_status_t status = SL_STATUS_OK;
#if defined(SL_ENABLE_CALENDAR_WAKEUP_SOURCE) && (SL_ENABLE_CALENDAR_WAKEUP_SOURCE == ENABLE)
  // Calendar is initialized.
  sl_si91x_calendar_init();
#if defined(ENABLE_SECOND) && (ENABLE_SECOND == ENABLE)
  // Second trigger callback is configured.
  status = sl_si91x_calendar_register_sec_trigger_callback(calendar_callback_function);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  // Second based wakeup source is configured
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_SEC_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
#endif // ENABLE_SECOND

#if defined(ENABLE_ALARM) && (ENABLE_ALARM == ENABLE)
  sl_calendar_datetime_config_t datetime_config;
  // Current date time is fetched
  status = sl_si91x_calendar_get_date_time(&datetime_config);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // If the date time is not configured, then only it is configured. Evaluated by the default values
  if ((!datetime_config.Year) && !(datetime_config.Century) && !(datetime_config.Hour)) {
    // Setting datetime for Calendar
    status = sl_si91x_calendar_build_datetime_struct(&datetime_config,
                                                     CENTURY,
                                                     YEAR,
                                                     February,
                                                     Friday,
                                                     DAY,
                                                     HOUR,
                                                     MINUTE,
                                                     SECONDS,
                                                     MILLI_SECONDS);
    if (status != SL_STATUS_OK) {
      return status;
    }
    status = sl_si91x_calendar_set_date_time(&datetime_config);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }
  // Periodic alarm setting API is called.
  set_periodic_alarm(ALARM_TIME_MSEC);

  // Alarm callback is registered
  status = sl_si91x_calendar_register_alarm_trigger_callback(calendar_callback_function);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Alarm based wakeup source is configured
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_ALARM_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    return status;
  }
#endif // ENABLE_ALARM

#if defined(ENABLE_MSEC) && (ENABLE_MSEC == ENABLE)
  // Milli Second trigger callback is configured.
  status = sl_si91x_calendar_register_msec_trigger_callback(calendar_callback_function);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_MSEC_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
#endif // ENABLE_MSEC
#endif // SL_ENABLE_CALENDAR_WAKEUP_SOURCE

  return status;
}

/*******************************************************************************
 *
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_gpio_init(void)
{
  sl_status_t status = SL_STATUS_OK;

#if defined(SL_ENABLE_GPIO_WAKEUP_SOURCE) && (SL_ENABLE_GPIO_WAKEUP_SOURCE == ENABLE)

#if defined(ENABLE_NPSS_GPIO_0) && (ENABLE_NPSS_GPIO_0 == ENABLE)
  uulp_gpio_configuration(UULP_GPIO_0);
#endif // ENABLE_NPSS_GPIO_0

#if defined(ENABLE_NPSS_GPIO_1) && (ENABLE_NPSS_GPIO_1 == ENABLE)
  uulp_gpio_configuration(UULP_GPIO_1);
#endif // ENABLE_NPSS_GPIO_1

#if defined(ENABLE_NPSS_GPIO_2) && (ENABLE_NPSS_GPIO_2 == ENABLE)
  uulp_gpio_configuration(UULP_GPIO_2);
#endif // ENABLE_NPSS_GPIO_2

#if defined(ENABLE_NPSS_GPIO_3) && (ENABLE_NPSS_GPIO_3 == ENABLE)
  uulp_gpio_configuration(UULP_GPIO_3);
#endif // ENABLE_NPSS_GPIO_4
#endif // SL_ENABLE_GPIO_WAKEUP_SOURCE
  return status;
}

/*******************************************************************************
 *
 ******************************************************************************/
sl_status_t sli_si91x_power_manager_wdt_init(void)
{
  sl_status_t status = SL_STATUS_OK;
#if defined(SL_ENABLE_WDT_WAKEUP_SOURCE) && (SL_ENABLE_WDT_WAKEUP_SOURCE == ENABLE)
  watchdog_timer_config_t wdt_config;
  sl_si91x_watchdog_init_timer();
  // Configuring watchdog-timer
  status = sl_si91x_watchdog_set_configuration(&wdt_config);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Registering timeout callback
  status = sl_si91x_watchdog_register_timeout_callback(wdt_callback_function);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_WDT_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  sl_si91x_watchdog_start_timer();
#endif // SL_ENABLE_WDT_WAKEUP_SOURCE
  return status;
}

sl_status_t sli_si91x_power_manager_dst_init(void)
{
  sl_status_t status = SL_STATUS_OK;
#if defined(SL_ENABLE_DST_WAKEUP_SOURCE) && (SL_ENABLE_DST_WAKEUP_SOURCE == ENABLE)

  // Power-up the RTC and Time period block
  RSI_PS_NpssPeriPowerUp(SLPSS_PWRGATE_ULP_MCURTC | SLPSS_PWRGATE_ULP_TIMEPERIOD);

  // Power-up the Deep-sleep timer
  RSI_PS_PowerSupplyEnable(POWER_ENABLE_DEEPSLEEP_TIMER);

  // by using this API we programmed the RTC timer clock in SOC
  // MSB 8-bits for the Integer part &
  // LSB 17-bits for the Fractional part
  // Eg:- 32KHz = 31.25µs ==> 31.25*2^17 = 4096000 = 0x3E8000
  /* Time Period Programming */
  RSI_TIMEPERIOD_TimerClkSel(TIME_PERIOD, 0x003E7FFF);

  RSI_DST_DurationSet(DST_WAKEUP_TIME);
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_DST_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  /* Interrupt unmask */
  RSI_DST_IntrUnMask();
  NVIC_EnableIRQ(NVIC_DS_TIMER);
#endif // SL_ENABLE_DST_WAKEUP_SOURCE
  return status;
}

sl_status_t sli_si91x_power_manager_wireless_wakeup_init(void)
{
  sl_status_t status = SL_STATUS_OK;

#if defined(SL_ENABLE_WIRELESS_WAKEUP_SOURCE) && (SL_ENABLE_WIRELESS_WAKEUP_SOURCE == ENABLE)
  NVIC_SetPriority(NPSS_TO_MCU_WIRELESS_INTR_IRQn, WIRELESS_WAKEUP_Priority);
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_WIRELESS_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  /* Enable NVIC */
  NVIC_EnableIRQ(NPSS_TO_MCU_WIRELESS_INTR_IRQn);
#endif // SL_ENABLE_WIRELESS_WAKEUP_SOURCE

  return status;
}

#if defined(SL_ENABLE_CALENDAR_WAKEUP_SOURCE) && (SL_ENABLE_CALENDAR_WAKEUP_SOURCE == ENABLE)

#if defined(ENABLE_ALARM) && (ENABLE_ALARM == ENABLE)
static void set_periodic_alarm(uint32_t alarm_time)
{
  sl_calendar_datetime_config_t set_alarm_config, get_datetime_config;

  sl_si91x_calendar_get_date_time(&get_datetime_config);

  set_alarm_config = get_datetime_config;

  set_alarm_config.MilliSeconds = set_alarm_config.MilliSeconds + (alarm_time % NO_OF_MSEC_IN_SEC);
  if (set_alarm_config.MilliSeconds >= NO_OF_MSEC_IN_SEC) {
    set_alarm_config.MilliSeconds -= NO_OF_MSEC_IN_SEC;
    set_alarm_config.Second += NEXT_OCCURENECE;
  }

  alarm_time /= NO_OF_MSEC_IN_SEC;

  set_alarm_config.Second = set_alarm_config.Second + (alarm_time % NO_OF_SEC_IN_MIN);
  if (set_alarm_config.Second >= NO_OF_SEC_IN_MIN) {
    set_alarm_config.Second -= NO_OF_SEC_IN_MIN;
    set_alarm_config.Minute += NEXT_OCCURENECE;
  }

  alarm_time /= NO_OF_SEC_IN_MIN;

  set_alarm_config.Minute = set_alarm_config.Minute + (alarm_time % NO_OF_MIN_IN_HOUR);
  if (set_alarm_config.Minute >= NO_OF_MIN_IN_HOUR) {
    set_alarm_config.Minute -= NO_OF_MIN_IN_HOUR;
    set_alarm_config.Hour += NEXT_OCCURENECE;
  }

  alarm_time /= NO_OF_MIN_IN_HOUR;

  set_alarm_config.Hour = set_alarm_config.Hour + alarm_time;
  if (set_alarm_config.Hour >= NO_OF_HOUR_IN_DAY) {
    set_alarm_config.Hour -= NO_OF_HOUR_IN_DAY;
    set_alarm_config.Day += NEXT_OCCURENECE;
  }

  if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_1) {
    if (set_alarm_config.Month == February) {
      if (set_alarm_config.Year % LEAP_YEAR_VALIDATION) {
        set_alarm_config.Day = RESET_DAY;
        set_alarm_config.Month += NEXT_OCCURENECE;
      } else if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_2) {
        set_alarm_config.Day = RESET_DAY;
        set_alarm_config.Month += NEXT_OCCURENECE;
      }
    }

    if (set_alarm_config.Month <= July) {
      if (!(set_alarm_config.Month % DAY_IN_MONTH_VALIDATION)) {
        if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_3) {
          set_alarm_config.Day = RESET_DAY;
          set_alarm_config.Month += NEXT_OCCURENECE;
        } else if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_4) {
          set_alarm_config.Day = RESET_DAY;
          set_alarm_config.Month += NEXT_OCCURENECE;
        }
      }
    } else if (!(set_alarm_config.Month % DAY_IN_MONTH_VALIDATION)) {
      if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_4) {
        set_alarm_config.Day = RESET_DAY;
        set_alarm_config.Month += NEXT_OCCURENECE;
      } else if (set_alarm_config.Day > NO_OF_DAY_IN_MONTH_3) {
        set_alarm_config.Day = RESET_DAY;
        set_alarm_config.Month += NEXT_OCCURENECE;
      }
    }
  }

  if (set_alarm_config.Month > NO_OF_MONTH_IN_YEAR) {
    set_alarm_config.Month = RESET_MONTH;
    set_alarm_config.Year += NEXT_OCCURENECE;
  }

  sl_si91x_calendar_set_alarm(&set_alarm_config);
}
#endif // ENABLE_ALARM

static void calendar_callback_function(void)
{
#if defined(ENABLE_ALARM) && (ENABLE_ALARM == ENABLE)
  set_periodic_alarm(ALARM_TIME_MSEC);
#endif // ENABLE_ALARM
}
#endif // SL_ENABLE_CALENDAR_WAKEUP_SOURCE

#if defined(SL_ENABLE_GPIO_WAKEUP_SOURCE) && (SL_ENABLE_GPIO_WAKEUP_SOURCE == ENABLE)
static sl_status_t uulp_gpio_configuration(uint8_t pin)
{
  sl_status_t status = SL_STATUS_OK;
  if (pin > 3) {
    status = SL_STATUS_INVALID_PARAMETER;
    return status;
  }
  // Unregister NPSS GPIO interrupts
  status = sl_gpio_driver_unregister(UULP_GPIO_INSTANCE, pin, pin);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sl_si91x_gpio_driver_select_uulp_npss_receiver(pin, SET);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Set NPSS GPIO pin MUX
  status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(pin, NPSS_GPIO_PIN_MUX_MODE2);
  if (status != SL_STATUS_OK) {
    return status;
  }
  // Set NPSS GPIO pin direction
  status = sl_si91x_gpio_driver_set_uulp_npss_direction(pin, (sl_si91x_gpio_direction_t)GPIO_INPUT);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sl_si91x_gpio_driver_select_uulp_npss_polarity(pin, POLARITY_LOW);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sl_si91x_gpio_driver_set_uulp_npss_wakeup_interrupt(pin);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status =
    sl_si91x_gpio_driver_configure_uulp_interrupt((sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_LEVEL_LOW,
                                                  pin,
                                                  (void *)&gpio_uulp_pin_interrupt_callback);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_GPIO_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    return status;
  }
  return status;
}

__attribute__((weak)) void gpio_uulp_pin_interrupt_callback(uint32_t pin_intr)
{
  (void)(pin_intr);
  // Empty funtion
}
#endif // SL_ENABLE_GPIO_WAKEUP_SOURCE

#if defined(SL_ENABLE_WDT_WAKEUP_SOURCE) && (SL_ENABLE_WDT_WAKEUP_SOURCE == ENABLE)
static void wdt_callback_function(void)
{
  // Empty funtion
}
#endif // SL_ENABLE_WDT_WAKEUP_SOURCE

#if defined(SL_ENABLE_DST_WAKEUP_SOURCE) && (SL_ENABLE_DST_WAKEUP_SOURCE == ENABLE)
void DS_IRQ(void)
{
  // Deep sleep timer interrupt
  RSI_DST_TimerIntrClear();
}
#endif // SL_ENABLE_DST_WAKEUP_SOURCE

#if defined(SL_ENABLE_WIRELESS_WAKEUP_SOURCE) && (SL_ENABLE_WIRELESS_WAKEUP_SOURCE == ENABLE)
void WIRELESS_WAKEUP_IRQ()
{
  /*Clear interrupt */
  RSI_PS_ClrWkpUpStatus(NPSS_TO_MCU_WIRELESS_INTR);
}
#endif // SL_ENABLE_WIRELESS_WAKEUP_SOURCE