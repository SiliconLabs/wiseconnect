/***************************************************************************/ /**
 * @file sl_si91x_calendar.c
 * @brief Calendar API implementation
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
#include "sl_si91x_calendar.h"
#include "sl_si91x_calendar_config.h"
#include <stdlib.h>

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define TIME_UNIX_EPOCH                  (1970u)                                 // Unix Epoch Year Macro
#define TIME_NTP_EPOCH                   (1900u)                                 // NTP Epoch Year Macro
#define TIME_SEC_PER_DAY                 (60u * 60u * 24u)                       // Seconds in one day
#define TIME_NTP_UNIX_EPOCH_DIFF         (TIME_UNIX_EPOCH - TIME_NTP_EPOCH)      // NTP and Unix Time difference
#define TIME_DAY_COUNT_NTP_TO_UNIX_EPOCH (TIME_NTP_UNIX_EPOCH_DIFF * 365u + 17u) // Day Count wrt to difference
#define TIME_NTP_EPOCH_OFFSET_SEC        (TIME_DAY_COUNT_NTP_TO_UNIX_EPOCH * TIME_SEC_PER_DAY) // NTP offset in seconds
#define TIME_UNIX_TO_NTP_MAX             (0xFFFFFFFF - TIME_NTP_EPOCH_OFFSET_SEC)              // NTP Maximum timestamp
#define TIME_UNIX_TIMESTAMP_MAX          (0x7FFFFFFF)                                          // Unix Maximum timestamp
#define TIME_UNIX_YEAR_MAX               (2038u - TIME_NTP_EPOCH)                              // Maximum unix year
#define TIME_ZONE_DEFAULT                (0u)                                                  // Default time zone

#define MINIMUM_DAY          (0u)   // Minimum day in a week
#define MAXIMUM_HOUR         (23u)  // Maximum hour in a day
#define MAXIMUM_MINUTE       (59u)  // Maximum minute in an hour
#define MAXIMUM_SECOND       (59u)  // Maximum second in a minute
#define MAXIMUM_MILLISECONDS (999u) // Maximum milliseconds in second
#define MAXIMUM_CENTURY      (4u)   // Maximum supported century
#define MAXIMUM_YEAR         (99u)  // Maximum year in a century

#define MAXIMUM_EPOCH_DAY    (19u) // Maximum day in epoch time
#define MAXIMUM_EPOCH_HOUR   (3u)  // Maximum hour in epoch time
#define MAXIMUM_EPOCH_MINUTE (14u) // Maximum minute in epoch time
#define MAXIMUM_EPOCH_SECOND (7u)  // Maximum second in epoch time

#define CALENDAR_RELEASE_VERSION (0u) // CALENDAR Release version
#define CALENDAR_SQA_VERSION     (0u) // CALENDAR SQA version
#define CALENDAR_DEV_VERSION     (2u) // CALENDAR Developer version

#define CALENDAR_UC \
  (1u) /*!< CALENDAR_UC is defined by default. when this macro (CALENDAR_UC) is defined, peripheral
                                                                  configuration is directly taken from the configuration set in the universal configuration (UC).
                                                                  if the application requires the configuration to be changed in run-time, undefined this macro
                                                                  and change the peripheral configuration through the \ref sl_si91x_calendar_set_configuration API. */

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static const uint8_t days_in_month[12] = {
  /* Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec */
  31u, 28u, 31u, 30u, 31u, 30u, 31u, 31u, 30u, 31u, 30u, 31u
};

static calendar_callback_t msec_callback  = NULL;
static calendar_callback_t sec_callback   = NULL;
static calendar_callback_t alarm_callback = NULL;

/*******************************************************************************
 ***************************  Local Types  ********************************
 ******************************************************************************/

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static bool is_valid_time(uint32_t time, time_conversion_enum format, int32_t time_zone);
static bool is_valid_date(sl_calendar_datetime_config_t *date);
static bool is_leap_year(uint16_t year);
static bool is_valid_alarm(sl_calendar_datetime_config_t *alarm);

/*******************************************************************************
 ***********************  Global function Prototypes *************************
 ******************************************************************************/
void SLI_ALARM_IRQHandler(void);
void SLI_MSEC_SEC_IRQHandler(void);

/*******************************************************************************
 **********************  Local Function Definition****************************
 ******************************************************************************/

/*******************************************************************************
***********************  Global function Definitions *************************
 ******************************************************************************/

/*******************************************************************************
 * Calendar Clock Configuration and Initialization
 * It should be called before starting the RTC as it sets the clock for RTC
 * RC, RO and XTAL clock is supported, by default, RC clock is enabled.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_configuration(sl_calendar_clock_t clock_type)
{
  sl_status_t status;
  /* CALENDAR_UC is defined by default. when this macro (CALENDAR_UC) is defined, peripheral
   * configuration is directly taken from the configuration set in the universal configuration (UC).
   * if the application requires the configuration to be changed in run-time, undefined this macro
   * and change the peripheral configuration through the sl_si91x_calendar_set_configuration API.
   */
#ifdef CALENDAR_UC
  clock_type = configuration.calendar_clock_type;
#endif
  do {
    // To validate the input parameters, if the parameters are not in range, it
    // returns an error code
    if ((clock_type != KHZ_RO_CLK_SEL) && (clock_type != KHZ_RC_CLK_SEL) && (clock_type != KHZ_XTAL_CLK_SEL)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_PS_FsmLfClkSel(clock_type);
    // To validate the register with the appropriate values, if the values are
    // incorrect, it returns an error code
    if (MCU_AON->MCUAON_KHZ_CLK_SEL_POR_RESET_STATUS_b.AON_KHZ_CLK_SEL != clock_type) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and clock is successfully configured and rtc is started then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Set the calendar date and time
 * It is used to set the date and time of calendar, if the invalid date structure 
 * is entered, error is returned otherwise it configure according
 * to the value of input date time structure.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_date_time(sl_calendar_datetime_config_t *config)
{
  sl_status_t status;
  error_t error_status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the date, if the date is not valid like any of the parameter of date
    // is invalid, it returns an error code
    if (!is_valid_date(config)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // It converts the RSI error code which is returned by the below API
    // to the SL error code
    error_status = RSI_RTC_SetDateTime(RTC, config);
    if (error_status == ERROR_CAL_INVALID_PARAMETERS) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required, and the date and time is successfully configured then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Get the calendar date and time
 * It is used to fetch the date and time of calendar, it stores the date and time 
 * in the structure provided in function argument. It has the current 
 * date and time value of calendar.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_date_time(sl_calendar_datetime_config_t *config)
{
  sl_status_t status;
  error_t error_status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // It converts the RSI error code which is returned by the below API
    // to the SL error code
    error_status = RSI_RTC_GetDateTime(RTC, config);
    if (error_status == ERROR_CAL_INVALID_PARAMETERS) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required, and the date and time is successfully fetched then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * RC Clock calibration
 * It is used to calibrate the RC clock "Calibration init function must be called
 * before calling this function" it takes the required value from the clock
 * configuration structure and stores it in the respective register.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_rcclk_calibration(clock_calibration_config_t *clock_calibration_config)
{
  sl_status_t status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (clock_calibration_config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the rc trigger time, if not in range,
    // it returns an error code
    if (clock_calibration_config->rc_trigger_time >= SL_RC_LAST_ENUM) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    RSI_RTC_RCCLK_Calib(TIME_PERIOD,
                        clock_calibration_config->rc_enable_calibration,
                        clock_calibration_config->rc_enable_periodic_calibration,
                        clock_calibration_config->rc_trigger_time);

    // To validate the register with the appropriate values, if the values are
    // incorrect, it returns an error code
    if (TIME_PERIOD->MCU_CAL_TEMP_PROG_REG_b.RTC_TIMER_PERIOD_MUX_SEL != ENABLE) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and rc clock is successfully calibrated then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * RO Clock calibration
 * It is used to calibrate the RO clock "Calibration init function must be called
 * before calling this function" it takes the required value from the clock
 * configuration structure and store it in the respective register.
 * The RO clock calibration internally calls RC clock calibration function first,
 * sets the register values for RC clock calibration and then sets the register values
 * for RO clock calibration.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_roclk_calibration(clock_calibration_config_t *clock_calibration_config)
{
  sl_status_t status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (clock_calibration_config == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the rc trigger time and ro trigger time, if not in range,
    // it returns an error code
    if ((clock_calibration_config->rc_trigger_time >= SL_RC_LAST_ENUM)
        || (clock_calibration_config->ro_trigger_time >= SL_RO_LAST_ENUM)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // The RSI_RTC_ROCLK_Calib API, interally calls the RSI_RTC_RCCLK_Calib so that is the
    // reason this API has parameters for both ro and rc clock calibration
    RSI_RTC_ROCLK_Calib(TIME_PERIOD,
                        clock_calibration_config->rc_enable_calibration,
                        clock_calibration_config->rc_enable_periodic_calibration,
                        clock_calibration_config->rc_trigger_time,
                        clock_calibration_config->ro_enable_calibration,
                        clock_calibration_config->ro_enable_periodic_calibration,
                        clock_calibration_config->ro_trigger_time);

    // To validate the register with the appropriate values, if the values are
    // incorrect, it returns an error code
    if (TIME_PERIOD->MCU_CAL_TEMP_PROG_REG_b.RTC_TIMER_PERIOD_MUX_SEL != ENABLE) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and ro clock is successfully calibrated then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Set the alarm date and time
 * It is used to set the date and time of alarm, if the invalid date structure 
 * is entered, error is returned otherwise it configures according
 * to the value of input date time structure.
 * The alarm interrupt is triggered once the calendar clock is reached to the 
 * alarm value.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_alarm(sl_calendar_datetime_config_t *alarm)
{
  sl_status_t status;
  error_t error_status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (alarm == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the date, if the date is not valid like any of the parameter of date
    // is invalid, it returns an error code
    if (!is_valid_date(alarm)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // To validate the alarm, if the alarm is not valid i.e., if the configuration is of past
    // datetime, it returns an error code
    if (!is_valid_alarm(alarm)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // It converts the RSI error code which is returned by the below API
    // to the SL error code
    error_status = RSI_RTC_SetAlarmDateTime(RTC, alarm);
    if (error_status == ERROR_CAL_INVALID_PARAMETERS) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required, and the alarm is successfully configured then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Get the alarm date and time
 * It is used to fetch the date and time of alarm, it stores the date and time 
 * in the structure provided in function argument. It has the existing
 * value of alarm.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_alarm(sl_calendar_datetime_config_t *alarm)
{
  sl_status_t status;
  error_t error_status;
  do {
    // To validate the structure pointer, if the parameters is NULL, it
    // returns an error code
    if (alarm == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // It converts the RSI error code which is returned by the below API
    // to the SL error code
    error_status = RSI_RTC_GetAlarmDateTime(RTC, alarm);
    if (error_status == ERROR_CAL_INVALID_PARAMETERS) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required, and the alarm is successfully fetched then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To register the callback of the msec trigger and enable it
 * It registers the callback, i.e., stores the callback function address 
 * and pass to the variable that is called in Interrupt Handler.
 * It also unmask and enable the millisecond trigger.
 * If any callback is already registered, the user needs to unregister the 
 * callback first before registering another callback.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_msec_trigger_callback(calendar_callback_t callback)
{
  sl_status_t status;
  do {
    // To validate the function pointer and void pointer, if the parameters is NULL then, it
    // returns an error code
    if (callback == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer and void pointer, if the parameters is not NULL then, it
    // returns an error code
    // If another callback is registered without unregistering previous callback then, it
    // returns an error code, so it is mandatory to unregister the callback before registering
    // another callback
    if (msec_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // The function pointer is feeded to the static variable which is called in the IRQ handler
    msec_callback = callback;
    RSI_RTC_IntrUnMask(RTC_MSEC_INTR);
    NVIC_EnableIRQ(SLI_NVIC_MSEC_SEC);
    // When the trigger is enabled, the bit 18 is 0, so the comparision is to validate whether the
    // trigger is enabled or not
    if ((NPSS_INTR_MASK_CLR_REG & RTC_MSEC_INTR) != DISABLE) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and the trigger is successfully enabled and the callback is registerd then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To register the callback of the sec trigger and enable it
 * It registers the callback, i.e., stores the callback function address 
 * and pass to the variable that is called in Interrupt Handler.
 * It also unmask and enable the second trigger.
 * If any callback is already registered, the user needs to unregister the 
 * callback first before registering another callback.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_sec_trigger_callback(calendar_callback_t callback)
{
  sl_status_t status;
  do {
    // To validate the function pointer and void pointer, if the parameters is NULL then, it
    // returns an error code
    if (callback == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer and void pointer, if the parameters is not NULL then, it
    // returns an error code
    // If another callback is registered without unregistering previous callback then, it
    // returns an error code, so it is mandatory to unregister the callback before registering
    // another callback
    if (sec_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // The function pointer is feeded to the static variable which is called in the IRQ handler
    sec_callback = callback;
    RSI_RTC_IntrUnMask(RTC_SEC_INTR);
    NVIC_EnableIRQ(SLI_NVIC_MSEC_SEC);
    // When the trigger is enabled, the bit 17 is 0, so the comparision is to validate whether the
    // trigger is enabled or not
    if ((NPSS_INTR_MASK_CLR_REG & RTC_SEC_INTR) != DISABLE) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and the trigger is successfully enabled and the callback is registerd then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To register the callback of the alarm trigger and enable it
 * It registers the callback, i.e., stores the callback function address 
 * and pass to the variable that is called in Interrupt Handler.
 * It also unmask and enable the alarm trigger.
 * If any callback is already registered, the user needs to unregister the 
 * callback first before registering another callback.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_alarm_trigger_callback(calendar_callback_t callback)
{
  sl_status_t status;
  do {
    // To validate the function pointer and void pointer, if the parameters is NULL then, it
    // returns an error code
    if (callback == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To validate the function pointer and void pointer, if the parameters is not NULL then, it
    // returns an error code
    // If another callback is registered without unregistering previous callback then, it
    // returns an error code, so it is mandatory to unregister the callback before registering
    // another callback
    if (alarm_callback != NULL) {
      status = SL_STATUS_BUSY;
      break;
    }
    // The function pointer is feeded to the static variable which is called in the IRQ handler
    alarm_callback = callback;
    RSI_RTC_AlamEnable(RTC, true);
    RSI_RTC_IntrUnMask(RTC_ALARM_INTR);
    NVIC_EnableIRQ(SLI_NVIC_ALARM);
    // When the trigger is enabled, the bit 16 is 0, so the comparision is to validate whether the
    // trigger is enabled or not, and the MCU_CAL_ALARM_PROG_2_b.ALARM_EN needs to be enabled
    if (((NPSS_INTR_MASK_CLR_REG & RTC_ALARM_INTR) != DISABLE) && (RTC->MCU_CAL_ALARM_PROG_2_b.ALARM_EN != ENABLE)) {
      status = SL_STATUS_FAIL;
      break;
    }
    // If everything is as required, and the trigger is successfully enabled and the callback is registerd then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * To unregister the callback of the msec trigger and disable it
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 * It also mask and disable the millisecond trigger.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_msec_trigger_callback(void)
{
  sl_status_t status;
  RSI_RTC_IntrMask(RTC_MSEC_INTR);
  // The callback should be null in the unregister callback api because
  // one cannot register the callback if it is not none
  msec_callback = NULL;
  // When the trigger is enabled, the bit 18 is 1, so the comparision is to validate whether the
  // trigger is disabled or not and return the error code accordingly
  if ((NPSS_INTR_MASK_SET_REG & RTC_MSEC_INTR) == DISABLE) {
    status = SL_STATUS_FAIL;
  } else {
    status = SL_STATUS_OK;
  }
  return status;
}

/*******************************************************************************
 * To unregister the callback of the sec trigger and disable it
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 * It also mask and disable the second trigger.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_sec_trigger_callback(void)
{
  sl_status_t status;
  RSI_RTC_IntrMask(RTC_SEC_INTR);
  // The callback should be null in the unregister callback api because
  // one cannot register the callback if it is not none
  sec_callback = NULL;
  // When the trigger is enabled, the bit 17 is 1, so the comparision is to validate whether the
  // trigger is disabled or not and return the error code accordingly
  if ((NPSS_INTR_MASK_SET_REG & RTC_SEC_INTR) == DISABLE) {
    status = SL_STATUS_FAIL;
  } else {
    status = SL_STATUS_OK;
  }
  return status;
}

/*******************************************************************************
 * To unregister the callback of the alarm trigger and disable it
 * It unregisters the callback, i.e., clear the callback function address
 * and pass NULL value to the variable
 * It also mask and disable the alarm trigger.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_alarm_trigger_callback(void)
{
  // The callback should be null in the unregister callback api because
  // one cannot register the callback if it is not none
  sl_status_t status;
  RSI_RTC_IntrMask(RTC_ALARM_INTR);
  RSI_RTC_AlamEnable(RTC, false);
  alarm_callback = NULL;
  // When the trigger is enabled, the bit 16 is 1, so the comparision is to validate whether the
  // trigger is disabled or not and return the error code accordingly
  // Here the value of MCU_CAL_ALARM_PROG_2_b.ALARM_EN should be 0 after disabling the trigger
  if (((NPSS_INTR_MASK_SET_REG & RTC_ALARM_INTR) == DISABLE) && (RTC->MCU_CAL_ALARM_PROG_2_b.ALARM_EN != DISABLE)) {
    status = SL_STATUS_FAIL;
  } else {
    status = SL_STATUS_OK;
  }
  return status;
}
/*******************************************************************************
 * Get the status of millisecond trigger
 * It performs 'and' operation between the interrupt register and bit 18 
 * to check whether the millisecond trigger is enabled or not.
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_msec_trigger_enabled(void)
{
  // The and operation is performed between the NPSS_INTR_STATUS_REG register and the
  // bit 18, if the value is non-zero then the trigger is enabled
  // It returns true if the trigger is enabled, otherwise false
  boolean_t status;
  uint32_t temp = 0;
  temp          = RSI_RTC_GetIntrStatus();
  if (temp & NPSS_TO_MCU_MSEC_INTR) {
    status = true;
  } else {
    status = false;
  }
  return status;
}

/*******************************************************************************
 * Get the status of second trigger
 * It performs 'and' operation between the interrupt register and bit 17 
 * to check whether the second trigger is enabled or not.
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_sec_trigger_enabled(void)
{
  // The and operation is performed between the NPSS_INTR_STATUS_REG register and the
  // bit 17, if the value is non-zero then the trigger is enabled
  // It returns true if the trigger is enabled, otherwise false
  boolean_t status;
  uint32_t temp = 0;
  temp          = RSI_RTC_GetIntrStatus();
  if (temp & NPSS_TO_MCU_SEC_INTR) {
    status = true;
  } else {
    status = false;
  }
  return status;
}

/*******************************************************************************
 * Get the status of alarm trigger
 * It performs 'and' operation between the interrupt register and bit 16 
 * to check whether the alarm trigger is enabled or not.
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_alarm_trigger_enabled(void)
{
  // The and operation is performed between the NPSS_INTR_STATUS_REG register and the
  // bit 16, if the value is non-zero then the trigger is enabled
  // It returns true if the trigger is enabled, otherwise false
  boolean_t status;
  uint32_t temp = 0;
  temp          = RSI_RTC_GetIntrStatus();
  if (temp & NPSS_TO_MCU_ALARM_INTR) {
    status = true;
  } else {
    status = false;
  }
  return status;
}

/*******************************************************************************
 * Build the Date time structure
 * It takes empty date time structure and function arguments like year, month,
 * day, hour, minute, second, millisecond
 * It fills the date time structure with the arguments provided by user
 ******************************************************************************/
sl_status_t sl_si91x_calendar_build_datetime_struct(sl_calendar_datetime_config_t *date,
                                                    uint8_t Century,
                                                    uint8_t Year,
                                                    sl_calendar_month_t Month,
                                                    sl_calendar_days_of_week_t DayOfWeek,
                                                    uint8_t Day,
                                                    uint8_t Hour,
                                                    uint8_t Minute,
                                                    uint8_t Second,
                                                    uint16_t Milliseconds)
{
  sl_status_t status;
  do {
    // To validate the structure pointer, if the parameters is NULL then, it
    // returns an error code
    if (date == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    date->Century      = Century;
    date->Year         = Year;
    date->Month        = Month;
    date->DayOfWeek    = DayOfWeek;
    date->Day          = Day;
    date->Hour         = Hour;
    date->Minute       = Minute;
    date->Second       = Second;
    date->MilliSeconds = Milliseconds;
    // To validate the date, if the date is not valid i.e. any of the parameter of date
    // is invalid, it returns an error code
    if (!is_valid_date(date)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required, and the structure is successfully built then,
    // it returns SL_STATUS_OK
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Time converison from Unix to NTP
 * It is used to convert Unix time format to NTP time format
 * It verifies the unix time entered by user and adds the ntp time offset.
 * Here while addition, the uint32_t variable is never overflowed.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_unix_time_to_ntp_time(uint32_t time, uint32_t *ntp_time)
{
  uint32_t temp = 0;
  sl_status_t status;
  do {
    // To validate the integer pointer, if the parameters is NULL then, it
    // returns an error code
    if (ntp_time == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To compare the time with maximum time, if it exceeds the maximum time
    // then it returns an error code
    // Maximum Unix timestamp that can be converted to NTP is 2085978495
    if (time > TIME_UNIX_TO_NTP_MAX) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    temp = time + TIME_NTP_EPOCH_OFFSET_SEC;
    // To validate the timestamp, if it is not proper then it returns an error code
    if (!is_valid_time(temp, TIME_FORMAT_NTP, TIME_ZONE_DEFAULT)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required then, the integer pointer is updated with the new value and
    // the API returns SL_STATUS_OK
    *ntp_time = temp;
    status    = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Time converison from NTP to Unix
 * It is used to convert NTP time format to Unix time format
 * It verifies the NTP time entered by user and subtracts the ntp time offset.
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_ntp_time_to_unix_time(uint32_t ntp_time, uint32_t *time)
{
  uint32_t temp = 0;
  sl_status_t status;
  do {
    // To validate the integer pointer, if the parameters is NULL then, it
    // returns an error code
    if (time == NULL) {
      status = SL_STATUS_NULL_POINTER;
      break;
    }
    // To compare the ntp_time with TIME_NTP_EPOCH_OFFSET_SEC, if it less than ntp_time
    // then the operation is negative so it returns an error code
    if (ntp_time < TIME_NTP_EPOCH_OFFSET_SEC) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    temp = ntp_time - TIME_NTP_EPOCH_OFFSET_SEC;
    // To validate the timestamp, if it is not proper then it returns an error code
    if (!is_valid_time(temp, TIME_FORMAT_UNIX, TIME_ZONE_DEFAULT)) {
      status = SL_STATUS_INVALID_PARAMETER;
      break;
    }
    // If everything is as required then, the integer pointer is updated with the new value and
    // the API returns SL_STATUS_OK
    *time  = temp;
    status = SL_STATUS_OK;
  } while (false);
  return status;
}

/*******************************************************************************
 * Alarm IRQ Handler
 * 
 * @param none
 * @return none
 ******************************************************************************/
void SLI_ALARM_IRQHandler(void)
{
  // It checks the trigger is enabled or not, then it clears the trigger
  // and calls the callback function
  if (sl_si91x_calendar_is_alarm_trigger_enabled()) {
    sl_si91x_calendar_clear_alarm_trigger();
    alarm_callback();
  }
}

/*******************************************************************************
 * Second and Millisecond IRQ Handler
 * 
 * @param none
 * @return none
 ******************************************************************************/
void SLI_MSEC_SEC_IRQHandler(void)
{
  // It checks the trigger is enabled or not, then it clears the trigger
  // and calls the callback function
  if (sl_si91x_calendar_is_msec_trigger_enabled()) {
    sl_si91x_calendar_clear_msec_trigger();
    msec_callback();
  }
  if (sl_si91x_calendar_is_sec_trigger_enabled()) {
    sl_si91x_calendar_clear_sec_trigger();
    sec_callback();
  }
}

/*******************************************************************************
 * Internal function to validate the time entered by user
 * 
 * @param time time (in unix or ntp format)
 * @param format (the format for which time should be validated unix or ntp)
 * @param time_zone (can be entered 0u if not known)
 * @return valid_time (0 or 1)the entered time is valid or not
 ******************************************************************************/
static bool is_valid_time(uint32_t time, time_conversion_enum format, int32_t time_zone)
{
  bool valid_time = false;
  // Check for overflow.
  if ((time_zone < 0 && time > abs(time_zone)) || (time_zone >= 0 && (time <= UINT32_MAX - (uint32_t)time_zone))) {
    valid_time = true;
  }
  if (format == TIME_FORMAT_UNIX) {
    // Check if Unix time stamp is an unsigned 31 bits.
    if (time > TIME_UNIX_TIMESTAMP_MAX) {
      valid_time = false;
    }
  } else {
    if ((format == TIME_FORMAT_NTP) && (time >= TIME_NTP_EPOCH_OFFSET_SEC)) {
      valid_time = true;
    } else {
      valid_time = false;
    }
  }
  return valid_time;
}

/*******************************************************************************
 * Internal function to validate the date entered by user
 * 
 * @param date Pointer to the Date Configuration Structure
 * @return valid_date (0 or 1)the entered date is valid or not
 ******************************************************************************/
static bool is_valid_date(sl_calendar_datetime_config_t *date)
{
  bool valid_date      = true;
  uint8_t maximum_days = 0;
  uint8_t February     = 2;

  do {
    if (date == NULL) {
      valid_date = false;
      break;
    }
    // Updates the days in month for the month entered by user
    // If the month is february and the leap year is there, then the days are 29
    // else it is as stored in the (days_in_month) array
    if (is_leap_year(date->Year) && date->Month == February) {
      maximum_days = days_in_month[(date->Month) - 1] + 1;
    } else {
      maximum_days = days_in_month[(date->Month) - 1];
    }
    // If the year, month, day, hour, minute, second, millisecond is greater than the maximum
    // then it returns false, else true
    if ((date->Century > MAXIMUM_CENTURY) || (date->Year > MAXIMUM_YEAR) || (date->Month > December)
        || (date->Day == MINIMUM_DAY || date->Day > maximum_days) || (date->Hour > MAXIMUM_HOUR)
        || (date->Minute > MAXIMUM_MINUTE) || (date->Second > MAXIMUM_SECOND)
        || (date->MilliSeconds > MAXIMUM_MILLISECONDS)) {
      valid_date = false;
      break;
    }
    // If the year, month, day, hour, minute, second is greater than the unix date supports,
    // then it returns false, else true
    // Unix date is valid until the 19th of January 2038 at 03:14:07
    if (date->Year == TIME_UNIX_YEAR_MAX) {
      if (((uint8_t)date->Month > (uint8_t)January) || (date->Day > MAXIMUM_EPOCH_DAY)
          || (date->Hour > MAXIMUM_EPOCH_HOUR) || (date->Minute > MAXIMUM_EPOCH_MINUTE)
          || (date->Second > MAXIMUM_EPOCH_SECOND)) {
        valid_date = false;
        break;
      }
    }
  } while (false);
  return valid_date;
}

/*******************************************************************************
 * Internal function to validate the leap year
 * 
 * @param year the year which needs to be validated
 * @return leap_year (0 or 1)the entered date is valid or not
 ******************************************************************************/
static bool is_leap_year(uint16_t year)
{
  // 1900 is not a leap year but 0 % anything is 0.
  bool leap_year = false;
  if (year == 0) {
    leap_year = false;
  } else {
    leap_year = (((year % 4u) == 0u) && (((year % 100u) != 0u) || ((year % 400u) == 0u))) ? true : false;
  }
  return (leap_year);
}

/*******************************************************************************
 * Internal function to validate the alarm date-time entered by the user]
 * If the alarm time parameters are greater than current time parameters, the 
 * result will be true.
 * If the alarm time parameters are less than current time parameters, the
 * result will be false. (Alarm cannot be set in past)
 * 
 * @param date Pointer to the Alarm Date Configuration Structure
 * @return valid_date (0 or 1)the entered date is valid or not
 ******************************************************************************/
static bool is_valid_alarm(sl_calendar_datetime_config_t *alarm)
{
  bool valid_alarm = false;
  sl_calendar_datetime_config_t date;
  sl_si91x_calendar_get_date_time(&date);
  do {
    if (alarm->Century < date.Century) {
      // If the alarm century is less than current century,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Century > date.Century) {
      // If the alarm century is greater than current century,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current century is same, so year needs to be verified.
    if (alarm->Year < date.Year) {
      // If the alarm year is less than current year,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Year > date.Year) {
      // If the alarm year is greater than current year,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current year is same, so month needs to be verified.
    if (alarm->Month < date.Month) {
      // If the alarm month is less than current month,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Month > date.Month) {
      // If the alarm month is greater than current month,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current month is same, so day needs to be verified.
    if (alarm->Day < date.Day) {
      // If the alarm day is less than current day,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Day > date.Day) {
      // If the alarm day is greater than current day,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current day is same, so hour needs to be verified.
    if (alarm->Hour < date.Hour) {
      // If the alarm hour is less than current hour,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Hour > date.Hour) {
      // If the alarm hour is greater than current hour,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current hour is same, so minute needs to be verified.
    if (alarm->Minute < date.Minute) {
      // If the alarm minute is less than current minute,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Minute > date.Minute) {
      // If the alarm minute is greater than current minute,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current minute is same, so second needs to be verified.
    if (alarm->Second < date.Second) {
      // If the alarm second is less than current second,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->Second > date.Second) {
      // If the alarm second is greater than current second,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // If both the conditions didnot break the loop then alarm
    // and current second is same, so milliseconds needs to be verified.
    if (alarm->MilliSeconds < date.MilliSeconds) {
      // If the alarm milliseconds is less than current milliseconds,
      // the alarm cannot be set in past.
      valid_alarm = false;
      break;
    }
    if (alarm->MilliSeconds > date.MilliSeconds) {
      // If the alarm milliseconds is greater than current milliseconds,
      // the alarm is always set in future.
      valid_alarm = true;
      break;
    }
    // The code should not reach here, if it is so, then the alarm parameters are invalid.
    valid_alarm = false;
  } while (false);
  return valid_alarm;
}

/*******************************************************************************
 * @brief
 * To get the release, sqa and dev version of CALENDAR
 * It returns the structure for CALENDAR version.
 * Structure includes three members:
 * - Release version
 * - SQA version
 * - Dev version
 ******************************************************************************/
sl_calendar_version_t sl_si91x_calendar_get_version(void)
{
  sl_calendar_version_t version;
  version.minor   = CALENDAR_DEV_VERSION;
  version.release = CALENDAR_RELEASE_VERSION;
  version.major   = CALENDAR_SQA_VERSION;
  return version;
}
