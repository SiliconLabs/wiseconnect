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
  boolean_t rc_enable_calibration;          ///< true to enable and false to disable rc calibration
  boolean_t rc_enable_periodic_calibration; ///< true to enable and false to disable rc periodic calibration
  uint8_t rc_trigger_time;                  ///< rc trigger time, 5 sec, 10 sec, 15 sec, 30 sec, 1 min, 2 min
  boolean_t ro_enable_calibration;          ///< true to enable and false to disable ro calibration
  boolean_t ro_enable_periodic_calibration; ///< true to enable and false to disable periodic calibration
  uint8_t ro_trigger_time;                  ///< ro trigger time, 1 sec, 2 sec, 4 sec, 8 sec
} clock_calibration_config_t;

/// @brief Enumeration to represent time conversion format
TIME_CONVERSION_ENUM(time_conversion_enum){
  TIME_FORMAT_UNIX, ///< Number of seconds since January 1, 1970, 00:00. Type is signed, so represented on 31 bit.
  TIME_FORMAT_NTP,  ///< Number of seconds since January 1, 1900, 00:00. Type is unsigned, so represented on 32 bit.
  TIME_FORMAT_LAST, ///< Last member of enum for validation
};

/// @brief Enumeration to represent different rc clock calibration configurations
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
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_calendar_version_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Configuration and initialization of Calendar i.e., RTC clock.
 * 
 * @param[in] clock_type (sl_calendar_clock_t) Enum for RTC Clock Type (RO, RC or XTAL)
 * @return status 0 if successful, else errorcode. 
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed 
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_configuration(sl_calendar_clock_t clock_type);

/***************************************************************************/ /**
 * Set the date and time of the Calendar RTC.
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 * 
 * @param[in] config (sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_date_time(sl_calendar_datetime_config_t *config);

/***************************************************************************/ /**
 * Get the date and time of an existing Calendar RTC.
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 *      \ref sl_si91x_calendar_set_date_time \n
 * 
 * @param[in] config (sl_calendar_datetime_config_t) Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_date_time(sl_calendar_datetime_config_t *config);

/***************************************************************************/ /**
 * Calibrate the RC Clock.
 * 
 * @pre \ref sl_si91x_calendar_calibration_init \n
 * 
 * @param[in] clock_calibration_config  (clock_calibration_config_t) 
 *            pointer to the clock calibration structure
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_rcclk_calibration(clock_calibration_config_t *clock_calibration_config);

/***************************************************************************/ /**
 * Calibrate the RO Clock.
 * 
 * @pre \ref sl_si91x_calendar_calibration_init \n
 * 
 * @param[in] clock_calibration_config (clock_calibration_config_t) 
 *            pointer to the clock calibration structure
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_roclk_calibration(clock_calibration_config_t *clock_calibration_config);

/***************************************************************************/ /**
 * Register the callback of the msec trigger and enable it.
 * 
 * @param[in] msec_callback_ (function pointer) Callback function pointer to be called when msec 
 *                           interrupt is triggered
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_msec_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Register the callback of the sec trigger and enable it.
 * 
 * @param[in] sec_callback_ (function pointer) Callback function pointer to be called when sec 
 *                          interrupt is triggered
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_sec_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Register the callback of the alarm trigger and enable it.
 * 
 * @param[in] alarm_callback_ (function pointer) Callback function pointer to be called when alarm 
 *                            interrupt is triggered
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, 
 *         unregister previous callback before registering new one
 ******************************************************************************/
sl_status_t sl_si91x_calendar_register_alarm_trigger_callback(calendar_callback_t callback);

/***************************************************************************/ /**
 * Unregister the callback of the msec trigger and disable it.
 * 
 * @pre \ref sl_si91x_calendar_register_msec_trigger_callback \n
 * 
 * @param none
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_msec_trigger_callback(void);

/***************************************************************************/ /**
 * Unregister the callback of the sec trigger and disable it.
 * 
 * @pre \ref sl_si91x_calendar_register_sec_trigger_callback \n 
 * 
 * @param none
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_sec_trigger_callback(void);

/***************************************************************************/ /**
 * Unregister the callback of the alarm trigger and disable it.
 * 
 * @pre sl_si91x_calendar_register_alarm_trigger_callback \n
 * 
 * @param none
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_OK (0x0000) - Success \n
 *         \ref SL_STATUS_FAIL (0x0001) - The function is failed
 ******************************************************************************/
sl_status_t sl_si91x_calendar_unregister_alarm_trigger_callback(void);

/***************************************************************************/ /**
 * Set the date and time of new alarm in RTC.
 * 
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 * 
 * @param[in] alarm Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_set_alarm(sl_calendar_datetime_config_t *alarm);

/***************************************************************************/ /**
 * Get the date and time of an existing alarm in RTC.
 * 
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 *      \ref sl_si91x_calendar_set_alarm \n
 * 
 * @param[in] alarm Pointer to the Date Configuration Structure 
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_get_alarm(sl_calendar_datetime_config_t *alarm);

/***************************************************************************/ /**
 * Build the structure for date-time configuration.
 * 
 * @param[in] date Pointer to the Date Configuration Structure
 * @param[in] Century (uint8_t) Century (0-4)
 * @param[in] Year (uint8_t) Year (1-99) + (Century * 1000)
 * @param[in] Month (enum) Month from the RTC_MONTH_T enum
 * @param[in] DayOfWeek (enum) Day of Week from the RTC_DAY_OF_WEEK_T enum
 * @param[in] Day Day (uint8_t) (1-31)
 * @param[in] Hour Hour (uint8_t) (0-23)
 * @param[in] Minute Minutes (uint8_t) (0-59)
 * @param[in] Second Seconds (uint8_t) (0-59)
 * @param[in] Milliseconds Milliseconds (uint16_t) (0-999)
 * @return status 0 if successful, else error code
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
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_unix_time_to_ntp_time(uint32_t time, uint32_t *ntp_time);

/***************************************************************************/ /**
 * Convert NTP timestamp to Unix timestamp.
 * 
 * @param[in] ntp_time (uint32_t) NTP timestamp
 * @param[in] time (uint32_t) variable to store Unix timestamp
 * @return status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Parameters are invalid \n
 *         \ref SL_STATUS_OK (0x0000) - Success
 ******************************************************************************/
sl_status_t sl_si91x_calendar_convert_ntp_time_to_unix_time(uint32_t ntp_time, uint32_t *time);

/***************************************************************************/ /**
 * Return the state of msec trigger of RTC (enabled or disabled).
 * 
 * @pre \ref sl_si91x_calendar_register_msec_trigger_callback \n
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_msec_trigger_enabled(void);

/***************************************************************************/ /**
 * Return the state of sec trigger of RTC (enabled or disabled).
 * 
 * @pre \ref sl_si91x_calendar_register_sec_trigger_callback \n 
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_sec_trigger_enabled(void);

/***************************************************************************/ /**
 * Return the state of alarm trigger of RTC (enabled or disabled).
 * 
 * @pre sl_si91x_calendar_register_alarm_trigger_callback \n
 * 
 * @param none
 * @return (boolean)true if trigger is enabled, false otherwise
 ******************************************************************************/
boolean_t sl_si91x_calendar_is_alarm_trigger_enabled(void);

/***************************************************************************/ /**
 * Start the Calendar RTC.
 * 
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_rtc_start(void)
{
  RSI_RTC_Start(RTC);
}

/***************************************************************************/ /**
 * Stop the Calendar RTC.
 * 
 * @pre \ref sl_si91x_calendar_set_configuration \n
 *      \ref sl_si91x_calendar_init \n
 *      \ref sl_si91x_calendar_rtc_start \n
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
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_calibration_init(void)
{
  RSI_RTC_CalibInitilization();
}

/***************************************************************************/ /**
 * Clear the msec trigger.
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_clear_msec_trigger(void)
{
  RSI_RTC_IntrClear(RTC_MSEC_INTR);
}

/***************************************************************************/ /**
 * Clear the sec trigger.
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
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_clear_alarm_trigger(void)
{
  RSI_RTC_IntrClear(RTC_ALARM_INTR);
}

/***************************************************************************/ /**
 * De-initialize calendar operation.
 *
 * @pre \ref sl_si91x_calendar_set_configuration
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
 *
 * @pre \ref sl_si91x_calendar_init
 * 
 * @param none
 * @return none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_calendar_deinit(void)
{
  RSI_PS_NpssPeriPowerDown(SLPSS_PWRGATE_ULP_MCURTC | SLPSS_PWRGATE_ULP_TIMEPERIOD);
}

/***************************************************************************/ /**
 * @brief
 * Get the calendar version.
 *
 * @details
 * This function is used to know the calendar version.
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
