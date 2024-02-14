/***************************************************************************/ /**
 * @file sl_si91x_calendar.h
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

#ifndef SL_SI91X_CALENDAR_H_
#define SL_SI91X_CALENDAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_status.h"
#include "rsi_power_save.h"
#include "rsi_rtc.h"
/***************************************************************************/ /**
 * @addtogroup CALENDAR Calendar
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 ******************************************************************************/
// -----------------------------------------------------------------------------
//Macros for IRQ Handlers
#define SLI_ALARM_IRQHandler    IRQ028_Handler     ///< Alarm IRQ Handler
#define SLI_MSEC_SEC_IRQHandler IRQ029_Handler     ///< RTC IRQ Handler
#define SLI_NVIC_ALARM          MCU_CAL_ALARM_IRQn ///< Alarm NVIQ enable
#define SLI_NVIC_MSEC_SEC       MCU_CAL_RTC_IRQn   ///< RTC NVIQ enable

// -----------------------------------------------------------------------------
// Data Types
#define TIME_CONVERSION_ENUM(name) \
  typedef uint8_t name;            \
  enum name##_enum ///< Time Conversion format enum declaration
#define RC_CLOCK_CALIBRATION_ENUM(name) \
  typedef uint8_t name;                 \
  enum name##_enum ///< Time Conversion format enum declaration
#define RO_CLOCK_CALIBRATION_ENUM(name) \
  typedef uint8_t name;                 \
  enum name##_enum ///< Time Conversion format enum declaration

typedef AON_CLK_T sl_calendar_clock_t;                   ///< Renaming clock type enum
typedef RTC_TIME_CONFIG_T sl_calendar_datetime_config_t; ///< Renaming datetime structure
typedef RTC_MONTH_T sl_calendar_month_t;                 ///< Renaming month structure
typedef RTC_DAY_OF_WEEK_T sl_calendar_days_of_week_t;    ///< Renaming days of week structure

/***************************************************************************/ /**
 * Typedef for the function pointer of the callback function
 ******************************************************************************/
typedef void (*calendar_callback_t)(void);

/// @brief Structure to hold the parameters of clock calibration, trigger time macros can be used to fill trigger time
typedef struct {
  boolean_t rc_enable_calibration;          ///< true to enable and false to disable RC calibration
  boolean_t rc_enable_periodic_calibration; ///< true to enable and false to disable RC periodic calibration
  uint8_t rc_trigger_time;                  ///< RC trigger time, 5 sec, 10 sec, 15 sec, 30 sec, 1 min, 2 min
  boolean_t ro_enable_calibration;          ///< true to enable and false to disable RO calibration
  boolean_t ro_enable_periodic_calibration; ///< true to enable and false to disable periodic calibration
  uint8_t ro_trigger_time;                  ///< RO trigger time, 1 sec, 2 sec, 4 sec, 8 sec
} clock_calibration_config_t;

/// @brief Enumeration to represent time conversion format
TIME_CONVERSION_ENUM(time_conversion_enum){
  TIME_FORMAT_UNIX, ///< Number of seconds since January 1, 1970, 00:00. Type is signed, so represented on 31 bit.
  TIME_FORMAT_NTP,  ///< Number of seconds since January 1, 1900, 00:00. Type is unsigned, so represented on 32 bit.
  TIME_FORMAT_LAST, ///< Last member of enum for validation
};

/// @brief Enumeration to represent the different RC clock calibration configurations
RC_CLOCK_CALIBRATION_ENUM(rc_clock_calibration_enum){
  SL_RC_FIVE_SEC,    ///< RC 5 Second Calibration
  SL_RC_TEN_SEC,     ///< RC 10 Second Calibration
  SL_RC_FIFTEEN_SEC, ///< RC 15 Second Calibration
  SL_RC_THIRTY_SEC,  ///< RC 30 Second Calibration
  SL_RC_ONE_MIN,     ///< RC 1 Minute Calibration
  SL_RC_TWO_MIN,     ///< RC 2 Minute Calibration
  SL_RC_LAST_ENUM,   ///< Last member of enum for validation
};

/// @brief Enumeration to represent different ro clock calibration configurations
RO_CLOCK_CALIBRATION_ENUM(ro_clock_calibration_enum){
  SL_RO_ONE_SEC,   ///< RC 1 Second Calibration
  SL_RO_TWO_SEC,   ///< RC 2 Second Calibration
  SL_RO_FOUR_SEC,  ///< RC 4 Second Calibration
  SL_RO_EIGHT_SEC, ///< RC 8 Second Calibration
  SL_RO_LAST_ENUM, ///< Last member of enum for validation
};

/// @brief Structure to hold the different versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< SQA version number
  uint8_t minor;   ///< DEV version number
} sl_calendar_version_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Configuration and initialization of Calendar i.e., RTC clock.
 * It takes input of clock type enum \ref sl_calendar_clock_t.
 * The clock type can be RO, RC or XTAL.
 * It configures the clock type and after configuration, initialized the RTC clock.
 * 
 * @param[in] clock_type (sl_calendar_clock_t) Enum for RTC Clock Type (RO, RC or XTAL)
 * @return status 0 if successful, else error code as follow. 
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed 
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_configuration(sl_calendar_clock_t clock_type);

/***************************************************************************/ /**
 * Set the date and time of the Calendar RTC.
 * The input parameters consists of date-time structure, the members of structure are listed below:
 * - date Pointer to the Date Configuration Structure
 * - Century (uint8_t) Century (0-4)
 * - Year (uint8_t) Year (1-99) + (Century * 1000)
 * - Month (enum) Month from the sl_calendar_month_t enum
 * - DayOfWeek (enum) Day of Week from the sl_calendar_days_of_week_t enum
 * - Day Day (uint8_t) (1-31)
 * - Hour Hour (uint8_t) (0-23)
 * - Minute Minutes (uint8_t) (0-59)
 * - Second Seconds (uint8_t) (0-59)
 * - Milliseconds Milliseconds (uint16_t) (0-999)
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
*  - \ref sl_si91x_calendar_init 
*
 * 
 * @param[in] config (sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_date_time(sl_calendar_datetime_config_t *config);

/***************************************************************************/ /**
 * Fetch the current date and time of an existing Calendar RTC.
 * The input parameters consists of dummy date-time structure.
 * The structure is updated with the current date-time parameters.
 * The members of structure are listed below:
 * - date Pointer to the Date Configuration Structure
 * - Century (uint8_t) Century (0-4)
 * - Year (uint8_t) Year (1-99) + (Century * 1000)
 * - Month (enum) Month from the sl_calendar_month_t enum
 * - DayOfWeek (enum) Day of Week from the sl_calendar_days_of_week_t enum
 * - Day Day (uint8_t) (1-31)
 * - Hour Hour (uint8_t) (0-23)
 * - Minute Minutes (uint8_t) (0-59)
 * - Second Seconds (uint8_t) (0-59)
 * - Milliseconds Milliseconds (uint16_t) (0-999)
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
 * - \ref sl_si91x_calendar_init 
 * - \ref sl_si91x_calendar_set_date_time 
 * 
 * @param[in] config (sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_date_time(sl_calendar_datetime_config_t *config);

/***************************************************************************/ /**
 * Calibrate the RC Clock.
 * If the RC clock is selected as the input clock to calendar, it is recommended 
 * to calibrate it after power-sequence.
 * It expects clock calibration structure \refclock_calibration_config_t as input, 
 * the members are listed below:
 * - rc_enable_calibration: true to enable and false to disable RC calibration
 * - rc_enable_periodic_calibration: true to enable and false to disable RC periodic calibration
 * - rc_trigger_time: Expected values - 5 sec, 10 sec, 15 sec, 30 sec, 1 min, 2 min \ref RC_CLOCK_CALIBRATION_ENUM
 * 
 * @note Only RC parameters are utilized in this function.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_calibration_init 
*
 * 
 * @param[in] clock_calibration_config  ( \ref clock_calibration_config_t) 
 *            pointer to the clock calibration structure
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_rcclk_calibration(clock_calibration_config_t *clock_calibration_config);

/***************************************************************************/ /**
 * Calibrate the RO Clock.
 * If the RO clock is selected as the input clock to calendar, it is recommended 
 * to calibrate it after power-sequence.
 * This function internally calibrates the RC clock also.
 * It expects clock calibration structure \ref clock_calibration_config_t as input, 
 * the members are listed below:
 * - rc_enable_calibration: true to enable and false to disable RC calibration
 * - rc_enable_periodic_calibration: true to enable and false to disable RC periodic calibration
 * - rc_trigger_time: Expected values - 5 sec, 10 sec, 15 sec, 30 sec, 1 min, 2 min \ref RC_CLOCK_CALIBRATION_ENUM
 * - ro_enable_calibration: true to enable and false to disable RO calibration
 * - ro_enable_periodic_calibration: true to enable and false to disable periodic calibration
 * - ro_trigger_time: Expected values - 1 sec, 2 sec, 4 sec, 8 sec \ref RO_CLOCK_CALIBRATION_ENUM
 * 
 * @note Both RC and RO parameters are utilized in this function, so it is compulsory to update all the parameters.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_calibration_init 
*
 * 
 * @param[in] clock_calibration_config ( \ref clock_calibration_config_t) 
 *            pointer to the clock calibration structure
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_roclk_calibration(clock_calibration_config_t *clock_calibration_config);

/***************************************************************************/ /**
 * Register the callback for one milli-second trigger and enables it.
 * At the time of trigger the function passed in this function as a argument is called.
 * It expects the function pointer as input argument.
 * Before calling this function again, it is mandatory to call \ref sl_si91x_calendar_unregister_msec_trigger_callback,
 * otherwise it returns SL_STATUS_BUSY error-code.
 * 
 * @param[in] msec_callback_ (function pointer \ref calendar_callback_t) Callback function pointer to be called when msec 
 *                           interrupt is triggered
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_msec_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Register the callback for one second trigger and enable it.
 * At the time of trigger the function passed in this function as a argument is called.
 * It expects the function pointer as input argument.
 * Before calling this function again, it is mandatory to call \ref sl_si91x_calendar_unregister_sec_trigger_callback,
 * otherwise it returns SL_STATUS_BUSY error-code.
 * 
 * @param[in] sec_callback_ (function pointer  \ref calendar_callback_t) Callback function pointer to be called when sec 
 *                          interrupt is triggered
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_sec_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Register the callback for alarm trigger and enable it.
 * At the time of trigger the function passed in this function as a argument is called.
 * It expects the function pointer as input argument.
 * Before calling this function again, it is mandatory to call \ref sl_si91x_calendar_unregister_alarm_trigger_callback,
 * otherwise it returns SL_STATUS_BUSY error-code.
 * 
 * @param[in] alarm_callback_ (function pointer \ref calendar_callback_t) Callback function pointer to be called when alarm 
 *                            interrupt is triggered
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_alarm_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Unregister the callback for one milli-second trigger and disable it.
 * It is mandatory to call this function before registering the callback again.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_register_msec_trigger_callback 
*
 * 
 * @param none
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_msec_trigger_callback(void);

/***************************************************************************/ /**
 * Unregister the callback for one second trigger and disable it.
 * It is mandatory to call this function before registering the callback again.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_register_sec_trigger_callback 
* 
 * 
 * @param none
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_sec_trigger_callback(void);

/***************************************************************************/ /**
 * Unregister the callback for alarm trigger and disable it.
 * It is mandatory to call this function before registering the callback again.
 * 
 * @pre Pre-conditions:
 * - sl_si91x_calendar_register_alarm_trigger_callback 
*
 * 
 * @param none
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_OK (0x0000) - Success 
*-
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_alarm_trigger_callback(void);

/***************************************************************************/ /**
 * Set the date and time of new alarm in RTC.
 * It is a one-shot alarm, after triggering the alarm it is elapsed.
 * The input parameters consists of date-time structure, the members of structure are listed below:
 * - date Pointer to the Date Configuration Structure
 * - Century (uint8_t) Century (0-4)
 * - Year (uint8_t) Year (1-99) + (Century * 1000)
 * - Month (enum) Month from the sl_calendar_month_t enum
 * - DayOfWeek (enum) Day of Week from the sl_calendar_days_of_week_t enum
 * - Day Day (uint8_t) (1-31)
 * - Hour Hour (uint8_t) (0-23)
 * - Minute Minutes (uint8_t) (0-59)
 * - Second Seconds (uint8_t) (0-59)
 * - Milliseconds Milliseconds (uint16_t) (0-999)
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
*-
 *      \ref sl_si91x_calendar_init 
*
 * 
 * @param[in] alarm ( \ref sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_alarm(sl_calendar_datetime_config_t *alarm);

/***************************************************************************/ /**
 * Fetch the date and time of an existing alarm set in RTC.
 * The input parameters consists of dummy date-time structure.
 * The structure is updated with the current date-time parameters.
 * The members of structure are listed below:
 * - date Pointer to the Date Configuration Structure
 * - Century (uint8_t) Century (0-4)
 * - Year (uint8_t) Year (1-99) + (Century * 1000)
 * - Month (enum) Month from the sl_calendar_month_t enum
 * - DayOfWeek (enum) Day of Week from the sl_calendar_days_of_week_t enum
 * - Day Day (uint8_t) (1-31)
 * - Hour Hour (uint8_t) (0-23)
 * - Minute Minutes (uint8_t) (0-59)
 * - Second Seconds (uint8_t) (0-59)
 * - Milliseconds Milliseconds (uint16_t) (0-999)
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
*-
 *      \ref sl_si91x_calendar_init 
*-
 *      \ref sl_si91x_calendar_set_alarm 
*
 * 
 * @param[in] alarm ( \ref sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_alarm(sl_calendar_datetime_config_t *alarm);

/***************************************************************************/ /**
 * Build the structure for date-time configuration.
 * By providing the following parameters as input, it validates and fills the 
 * date-time configuration structure with the respective values.
 * 
 * @param[in] config ( \ref sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @param[in] Century (uint8_t) Century (0-4)
 * @param[in] Year (uint8_t) Year (1-99) + (Century * 1000)
 * @param[in] Month (enum) Month from the RTC_MONTH_T enum
 * @param[in] DayOfWeek (enum) Day of Week from the RTC_DAY_OF_WEEK_T enum
 * @param[in] Day Day (uint8_t) (1-31)
 * @param[in] Hour Hour (uint8_t) (0-23)
 * @param[in] Minute Minutes (uint8_t) (0-59)
 * @param[in] Second Seconds (uint8_t) (0-59)
 * @param[in] Milliseconds Milliseconds (uint16_t) (0-999)
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid
 *         \ref SL_STATUS_OK (0x0000) - Success
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
                                                    uint16_t Milliseconds);

/***************************************************************************/ /**
 * Convert Unix timestamp to NTP timestamp.
 * 
 * @param[in] time (uint32_t) Unix timestamp
 * @param[in] ntp_time (uint32_t) variable to store NTP timestamp
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_unix_time_to_ntp_time(uint32_t time, uint32_t *ntp_time);

/***************************************************************************/ /**
 * Convert NTP timestamp to Unix timestamp.
 * 
 * @param[in] ntp_time (uint32_t) NTP timestamp
 * @param[in] time (uint32_t) variable to store Unix timestamp
 * @return status 0 if successful, else error code as follow
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
*-
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid 
*-
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_ntp_time_to_unix_time(uint32_t ntp_time, uint32_t *time);

/***************************************************************************/ /**
 * Return the state of one milli-second trigger of RTC (enabled or disabled).
 * If enabled, returns true.
 * If disabled, returns false.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_register_msec_trigger_callback 
*
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_msec_trigger_enabled(void);

/***************************************************************************/ /**
 * Return the state of one second trigger of RTC (enabled or disabled).
 * If enabled, returns true.
 * If disabled, returns false.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_register_sec_trigger_callback 
* 
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_sec_trigger_enabled(void);

/***************************************************************************/ /**
 * Return the state of alarm trigger of RTC (enabled or disabled).
 * If enabled, returns true.
 * If disabled, returns false.
 * 
 * @pre Pre-conditions:
 * - sl_si91x_calendar_register_alarm_trigger_callback 
*
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_alarm_trigger_enabled(void);

/***************************************************************************/ /**
 * Starts the Calendar RTC.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
*-
 *      \ref sl_si91x_calendar_init 
*
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_rtc_start(void)
{
  RSI_RTC_Start(RTC);
}

/***************************************************************************/ /**
 * Stops the Calendar RTC.
 * 
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration 
*-
 *      \ref sl_si91x_calendar_init 
*-
 *      \ref sl_si91x_calendar_rtc_start 
*
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_rtc_stop(void)
{
  RSI_RTC_Stop(RTC);
}

/***************************************************************************/ /**
 * Initialize the calibration for Calendar clocks.
 * It is mandatory to call this function before calling RO clock calibration or
 * RC clock calibration function.
 * 
 * @post \ref sl_si91x_calendar_rcclk_calibration \n
 *       \ref sl_si91x_calendar_roclk_calibration \n
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_calibration_init(void)
{
  RSI_RTC_CalibInitilization();
}

/***************************************************************************/ /**
 * Clear the one milli-second trigger.
 * It is generally called in the IRQ handler.
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_clear_msec_trigger(void)
{
  RSI_RTC_IntrClear(RTC_MSEC_INTR);
}

/***************************************************************************/ /**
 * Clear the one second trigger.
 * It is generally called in the IRQ handler.
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_clear_sec_trigger(void)
{
  RSI_RTC_IntrClear(RTC_SEC_INTR);
}

/***************************************************************************/ /**
 * Clear the alarm trigger.
 * It is generally called in the IRQ handler.
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_clear_alarm_trigger(void)
{
  RSI_RTC_IntrClear(RTC_ALARM_INTR);
}

/***************************************************************************/ /**
 * Initialize calendar operation.
 * It power ups the RTC domain and starts the calendar clock.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_set_configuration
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_init(void)
{
  RSI_RTC_Init(RTC);
}

/***************************************************************************/ /**
 * De-initialize calendar operation.
 * It power down the RTC domain and stops the calendar clock.
 *
 * @pre Pre-conditions:
 * - \ref sl_si91x_calendar_init
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_deinit(void)
{
  RSI_PS_NpssPeriPowerDown(SLPSS_PWRGATE_ULP_MCURTC);
}

/***************************************************************************/ /**
 * Get the calendar version.
 * It returns the API version of calendar.
 *
 * @param[in] none
 * @return \ref sl_calendar_version_t type version
 ******************************************************************************/
sl_calendar_version_t sl_si91x_calendar_get_version(void);

/** @} (end addtogroup Calendar) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_CALENDAR_H_ */
