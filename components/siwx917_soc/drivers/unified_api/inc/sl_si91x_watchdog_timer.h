/***************************************************************************/ /**
 * @file  sl_si91x_watchdog_timer.h
 * @brief Watchdog-Timer API implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licenser of this software is Silicon Laboratories Inc.
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

#ifndef SL_SI91X_WATCHDOG_TIMER_
#define SL_SI91X_WATCHDOG_TIMER_

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "rsi_power_save.h"
#include "rsi_wwdt.h"

/***************************************************************************/ /**
 * @addtogroup WATCHDOG-TIMER Watchdog Timer
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Macros for watchdog-timer parameters

// -----------------------------------------------------------------------------
// Data Types
/***************************************************************************/ /**
 * Typedef for the function pointer of the callback function
 *
 * @param data (void *)extra parameter for user application
 ******************************************************************************/
typedef void (*watchdog_timer_callback_t)(void);

typedef AON_CLK_T low_freq_fsm_clock_t;  ///< Renaming low frequency fsm-clock type enum
typedef FSM_CLK_T high_freq_fsm_clock_t; ///< Renaming high frequency fsm-clock type enum

/// @brief Enumeration to represent bg-pmu clock sources
typedef enum {
  RO_32KHZ_CLOCK = 1, ///< RO_32KHz_clock
  MCU_FSM__CLOCK = 2, ///< mcu_fsm__clock
} bg_pmu_clock_t;

/// @brief Enumeration to represent possible time delays values for WDT interrupt time and system reset time with 32 KHZ clock freq.
typedef enum {
  TIME_DELAY_0,    ///< for time delay of 0.03125 milliseconds
  TIME_DELAY_1,    ///< for time delay of 0.0625 milliseconds
  TIME_DELAY_2,    ///< for time delay of 0.125 milliseconds
  TIME_DELAY_3,    ///< for time delay of 0.25 milliseconds
  TIME_DELAY_4,    ///< for time delay of 0.5 milliseconds
  TIME_DELAY_5,    ///< for time delay of 1 milliseconds
  TIME_DELAY_6,    ///< for time delay of 2 milliseconds
  TIME_DELAY_7,    ///< for time delay of 4 milliseconds
  TIME_DELAY_8,    ///< for time delay of 8 milliseconds
  TIME_DELAY_9,    ///< for time delay of 16 milliseconds
  TIME_DELAY_10,   ///< for time delay of 32 milliseconds
  TIME_DELAY_11,   ///< for time delay of 64 milliseconds
  TIME_DELAY_12,   ///< for time delay of 128 milliseconds
  TIME_DELAY_13,   ///< for time delay of 256 milliseconds
  TIME_DELAY_14,   ///< for time delay of 512 milliseconds
  TIME_DELAY_15,   ///< for time delay of 1.024 seconds
  TIME_DELAY_16,   ///< for time delay of 2.048 seconds
  TIME_DELAY_17,   ///< for time delay of 4.096 seconds
  TIME_DELAY_18,   ///< for time delay of 8.192 seconds
  TIME_DELAY_19,   ///< for time delay of 16.384 seconds
  TIME_DELAY_20,   ///< for time delay of 32.768 seconds
  TIME_DELAY_21,   ///< for time delay of 65.536 seconds
  TIME_DELAY_22,   ///< for time delay of 131.072 seconds
  TIME_DELAY_23,   ///< for time delay of 262.144 seconds
  TIME_DELAY_24,   ///< for time delay of 524.288 seconds
  TIME_DELAY_25,   ///< for time delay of 1048.576 seconds
  TIME_DELAY_26,   ///< for time delay of 2097.152 seconds
  TIME_DELAY_27,   ///< for time delay of 4194.304 seconds
  TIME_DELAY_28,   ///< for time delay of 8388.60 seconds
  TIME_DELAY_29,   ///< for time delay of 16777.216 seconds
  TIME_DELAY_30,   ///< for time delay of 33554.432 seconds
  TIME_DELAY_31,   ///< for time delay of 67108.864 seconds
  TIME_DELAY_LAST, ///< for time delay  value validation
} time_delays_t;

/// @brief Structure to hold the parameters of watchdog timer clock-source configurations
typedef struct {
  uint8_t low_freq_fsm_clock_src;  ///< low frequency FSM clock source, \ref #low_freq_fsm_clock_t
  uint8_t high_freq_fsm_clock_src; ///< high frequency FSM clock source, \ref #high_freq_fsm_clock_t
  uint8_t bg_pmu_clock_source;     ///< bg_pmu clock source, \ref #bg_pmu_clock_t
} watchdog_timer_clock_config_t;

/// @brief Structure to hold the parameters of watchdog timer configurations
typedef struct {
  uint8_t
    interrupt_time; ///< timer timeout interrupt duration, number of clock pulses = 2^(system_reset_time), \ref #time_delays_t
  uint8_t
    system_reset_time; ///< timer system-reset duration, number of clock pulses = 2^(system_reset_time), \ref #time_delays_t
  uint8_t
    window_time; ///< timer window duration, number of clock pulses = 2^(window_time), \ref #time_delays_t, but should be less than TIME_DELAY_16
} watchdog_timer_config_t;

/// @brief Structure to hold the versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_watchdog_timer_version_t;

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * Initialize the Watchdog timer (Power-up WDT and enables it to run during sleep mode).
 * Also un-masks its interrupt & sets RTC clock time-period.
 *
 * @param[in]   none
 * @return      none
*******************************************************************************/
void sl_si91x_watchdog_init_timer(void);

/*******************************************************************************
 * Configure timer clock sources.
 *
 * @pre \ref sl_si91x_watchdog_init_timer \n
 * 
 * @param[in] timer_clk_config_ptr Pointer to timer clock configuration structure
 * @return    status 0 if successful, else error code.
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer configuration structure members has invalid value,
 *                                                for members \ref #watchdog_timer_clock_config_t \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_watchdog_configure_clock(watchdog_timer_clock_config_t *timer_clk_config_ptr);

/*******************************************************************************
 * Configure clock sources & timer parameters like interrupt time(WDT restart time),
 * system reset time & window time (another time stamp for WDT restart, if required).
 * Also un-masks its interrupt & sets RTC clock time-period.
 * 
 * @pre \ref sl_si91x_watchdog_init_timer \n 
 *      \ref sl_si91x_watchdog_configure_clock \n
 * 
 * @param[in] timer_config_ptr Pointer to timer clock configuration structure
 * @return    status 0 if successful, else error code.
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer configuration structure members has invalid value,
 *                                                for members \ref #watchdog_timer_config_t \n
 *         \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer configuration structure member 'system_reset_time' is less than or equal to 'interrupt_time'.
 *                                                    It should be greater than interrupt time of timer. \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success, timer parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_watchdog_set_configuration(watchdog_timer_config_t *timer_config_ptr);

/***************************************************************************/ /**
 * Register Watchdog timer timeout callback and enables NVIC.
 * 
 * @param[in]  on_timeout_callback (function pointer) Callback function pointer
 *             to be called when timer timeout interrupt occurred.
 * @return     status 0 if successful, else error code
 *          \ref SL_STATUS_NULL_POINTER (0x0022) - Callback function pointer parameter is a null pointer. \n
 *          \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, user must
 *                                   unregister previous callback before registering again \n
 *          \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback
*******************************************************************************/
sl_status_t sl_si91x_watchdog_register_timeout_callback(watchdog_timer_callback_t on_timeout_callback);

/***************************************************************************/ /**
 * Set Watchdog timer interrupt time(upper time-stamp for WDT restart) duration.
 *
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_configure_clock \n
 *      \ref sl_si91x_watchdog_set_system_reset_time \n
 * 
 * @param[in] interrupt_time (time_delays_t), timer timeout interrupt duration,
 *             Number of clock pulses = 2^(interrupt_time), \ref #time_delays_t
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'interrupt_time' parameter has invalid value. \n
 *         \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - 'interrupt_time' value is less than window time or greater than system reset time \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully set watchdog timer timeout time value
*******************************************************************************/
sl_status_t sl_si91x_watchdog_set_interrupt_time(time_delays_t interrupt_time);

/***************************************************************************/ /**
 * Read Watchdog timer interrupt time set value (in terms of power of two)
 *
 * @pre sl_si91x_watchdog_set_interrupt_time \n
 * 
 * @param[in]  none
 * @return     returns interrupt time (uint8_t) value, \ref #time_delays_t
*******************************************************************************/
uint8_t sl_si91x_watchdog_get_interrupt_time(void);

/***************************************************************************/ /**
 * Set Watchdog timer system-reset time duration.
 *
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_configure_clock \n
 * 
 * @param[in] system_reset_time (time_delays_t) timer system-reset duration,
 *            Number of clock pulses = 2^(system_reset_time), \ref #time_delays_t
 * @return    status 0 if successful, else error code
 *          \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'system_reset_time' parameter has invalid value. \n
 *          \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - 'system_reset_time' value is less than window time or interrupt time \n
 *          \ref SL_STATUS_OK (0x0000) - Successfully set watchdog timer system-reset time value
*******************************************************************************/
sl_status_t sl_si91x_watchdog_set_system_reset_time(time_delays_t system_reset_time);

/***************************************************************************/ /**
 * Read Watchdog timer system-reset time set value (in terms of power of two).
 *
 * @pre \ref sl_si91x_watchdog_set_system_reset_time \n
 * 
 * @param[in]  none
 * @return     returns system-reset time (uint8_t) value, \ref #time_delays_t
*******************************************************************************/
uint8_t sl_si91x_watchdog_get_system_reset_time(void);

/***************************************************************************/ /**
 * Set Watchdog timer window time value (lower time-stamp for WDT restart).
 *
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_configure_clock \n
 *      \ref sl_si91x_watchdog_set_system_reset_time \n
 *      \ref sl_si91x_watchdog_set_interrupt_time \n
 * 
 * @param[in] window_time (time_delays_t), timer window time,
 *            Number of clock pulses = 2^(window_time),\ref #time_delays_t
 * @return    status 0 if successful, else error code
 *          \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'window_time' parameter has invalid value. \n
 *          \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - 'window_time' value is greater than interrupt time or system reset time \n
 *          \ref SL_STATUS_OK (0x0000) - Successfully set watchdog timer window time value
*******************************************************************************/
sl_status_t sl_si91x_watchdog_set_window_time(time_delays_t window_time);

/***************************************************************************/ /**
 * Read Watchdog timer window time set value (in terms of power of two).
 *
 * @pre \ref sl_si91x_watchdog_set_window_time \n
 * 
 * @param[in]  none
 * @return     returns window time (uint8_t) value, \ref #time_delays_t
*******************************************************************************/
uint8_t sl_si91x_watchdog_get_window_time(void);

/***************************************************************************/ /**
 * Read Watchdog timer system-reset status.
 * 
 * @param[in]  none
 * @return     returns true if watchdog timer resets system, else returns false
*******************************************************************************/
boolean_t sl_si91x_watchdog_get_timer_system_reset_status(void);

/***************************************************************************/ /**
 * De-initialize Watchdog timer, mask its interrupt, de-power and disable timer.
 *
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_unregister_timeout_callback \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
void sl_si91x_watchdog_deinit_timer(void);

/***************************************************************************/ /**
 * Unregister Watchdog timer timeout callback.
 *
 * @pre \ref sl_si91x_watchdog_register_timeout_callback \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
void sl_si91x_watchdog_unregister_timeout_callback(void);

/***************************************************************************/ /**
 * Get the release version of Watchdog timer.
 *
 * @param[in] none
 * @return (sl_watchdog_version_t) type structure
 *
 ******************************************************************************/
sl_watchdog_timer_version_t sl_si91x_watchdog_get_version();

/***************************************************************************/ /**
 * Start and enable the Watchdog timer.
 * 
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_configure_clock \n
 *      \ref sl_si91x_watchdog_register_timeout_callback \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
/*******************************************************************************
* @brief: Start & enable the Watchdog timer.
* @details:
* It enables the timer and sets the restart bit of watchdog timer, by updating
*  'WWD_TIMER_ENABLE' register bits as:
*
* It updates 'wwd_timer_en'(23:16) bits of timer enable register by 0XAA, to enable
*  the WDT timer.
*
* Also sets 'wwd_timer_rstart' bit of the register to start the
*  timer
*******************************************************************************/
__STATIC_INLINE void sl_si91x_watchdog_start_timer(void)
{
  RSI_WWDT_Start(MCU_WDT);
}

/***************************************************************************/ /**
 * Stop Watchdog timer by disabling it.
 *
 * @pre \ref sl_si91x_watchdog_start_timer \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
/*******************************************************************************
* @brief: Disable the Watchdog timer to stop it.
*
* @details:
* It disables the timer by updating 'WWD_TIMER_ENABLE' register bits.
*
* It updates 'wwd_timer_en'(23:16) bits of timer enable register by 0xF0, to disable
* the WDT timer.
*******************************************************************************/
__STATIC_INLINE void sl_si91x_watchdog_stop_timer(void)
{
  RSI_WWDT_Disable(MCU_WDT);
}

/***************************************************************************/ /**
 * Restart (kick) an already running Watchdog timer.
 * 
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_configure_clock \n
 *      \ref sl_si91x_watchdog_start_timer \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
/*******************************************************************************
* @brief: Restart or kick Watchdog timer.
*
* @details:
* It sets the restart bit of timer, when it is already enabled.
*
* It only sets 'wwd_timer_rstart' bit of the register to restart an already enabled
* timer.
*
* It should be used for already started WDT timer, not for starting a stopped timer
*******************************************************************************/
__STATIC_INLINE void sl_si91x_watchdog_restart_timer(void)
{
  RSI_WWDT_ReStart(MCU_WDT);
}

/***************************************************************************/ /**
 * Enable the Watchdog timer to reset system on processor lockup.
 * 
 * @pre \ref sl_si91x_watchdog_init_timer \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
/*******************************************************************************
* @brief: Enables watchdog timer to reset system on processor lockup
*
* @details:
* It enables WDT to generate system-reset when processor got stuck or reached
* lock-up state, by setting 'PROCESSOR_STUCK_RESET_EN' bit of 'WWD_ARM_STUCK_EN'
* register of WDT
*******************************************************************************/
__STATIC_INLINE void sl_si91x_watchdog_enable_system_reset_on_processor_lockup(void)
{
  RSI_WWDT_SysRstOnProcLockEnable(MCU_WDT);
}

/***************************************************************************/ /**
 * Disable the Watchdog timer to reset system on processor lockup.
 *  
 * @pre \ref sl_si91x_watchdog_init_timer \n
 *      \ref sl_si91x_watchdog_enable_system_reset_on_processor_lockup \n
 * 
 * @param[in]   none
 * @return      none
*******************************************************************************/
/*******************************************************************************
* @brief: Disable Watchdog timer to reset system on processor lockup.
*
* @details:
* It disables WDT to generate system-reset when processor got stuck or reached
* lock-up state, by writing '0' to 'PROCESSOR_STUCK_RESET_EN' bit of 'WWD_ARM_STUCK_EN'
* register of WDT.
*******************************************************************************/
__STATIC_INLINE void sl_si91x_watchdog_disable_system_reset_on_processor_lockup(void)
{
  RSI_WWDT_SysRstOnProcLockDisable(MCU_WDT);
}

/** @} (end addtogroup Watchdog-Timer) */

#ifdef __cplusplus
}
#endif

#endif /*SL_SI91X_WATCHDOG_TIMER_*/
