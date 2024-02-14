/***************************************************************************/ /**
 * @file sl_si91x_ulp_timer.h
 * @brief ULP TIMER API implementation
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

#ifndef SL_SI91X_ULP_TIMER_
#define SL_SI91X_ULP_TIMER_

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "rsi_timers.h"
#include "base_types.h"
#include "rsi_pll.h"

/***************************************************************************/ /**
 * @addtogroup ULP-TIMER Ultra Low-Power Timer
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{ 
 * 
 ******************************************************************************/

// Macros
#define SL_TIMER_MATCH_VALUE_DEFAULT \
  20000000 ///< default 1-second timer match-value for down-counter timer-type with 20Mhz clock
// -----------------------------------------------------------------------------
// Data Types
/***************************************************************************/ /**
 * Typedef for the function pointer of the callback function
 ******************************************************************************/
typedef void (*ulp_timer_callback_t)(void);

typedef CLK_ENABLE_T ulp_timer_clock_t;        ///< Renaming clock type enum
typedef ulp_timer_dir_t ulp_timer_direction_t; ///< Renaming clock type enum

/// @brief Enumeration to represent ULP-timer instances
typedef enum {
  ULP_TIMER_0,    ///< ULP Timer0 Instance
  ULP_TIMER_1,    ///< ULP Timer1 Instance
  ULP_TIMER_2,    ///< ULP Timer2 Instance
  ULP_TIMER_3,    ///< ULP Timer3 Instance
  ULP_TIMER_LAST, ///< Last member of enum for validation
} ulp_timer_instance_t;

/// @brief Enumeration to represent ULP-timer modes
typedef enum {
  ULP_TIMER_MODE_ONESHOT,  ///< ULP Timer one-shot mode
  ULP_TIMER_MODE_PERIODIC, ///< ULP Timer periodic mode
  ULP_TIMER_MODE_LAST,     ///< Last member of enum for validation
} ulp_timer_mode_t;

/// @brief Enumeration to represent ULP-timer types
typedef enum {
  ULP_TIMER_TYP_DEFAULT, ///< ULP Timer normal down counter type
  ULP_TIMER_TYP_1US,     ///< ULP Timer one microsecond type
  ULP_TIMER_TYP_256US,   ///< ULP Timer 256 microsecond type
  ULP_TIMER_TYP_LAST,    ///< Last member of enum for validation
} ulp_timer_type_t;

/// @brief Enumeration to represent values of clock sources to select as Timer clock
typedef enum {
  ULP_TIMER_REF_CLK_SRC,        ///< ref clock input source
  ULP_TIMER_32KHZ_RO_CLK_SRC,   ///< 32 kHz ro clock input source
  ULP_TIMER_32KHZ_RC_CLK_SRC,   ///< 32 kHz rc clock input source
  ULP_TIMER_32KHZ_XTAL_CLK_SRC, ///< 32 kHz xtal clock input source
  ULP_TIMER_32MHZ_RC_CLK_SRC,   ///< 32 MHz rc clock input source
  ULP_TIMER_20MHZ_RO_CLK_SRC,   ///< 20 MHz  ro clock input source
  ULP_TIMER_ULP_CLK_SRC_LAST,   ///< Last member of enum for validation
} ulp_timer_clk_input_source_t;

/// @brief Structure to hold the parameters of timer clock-source configurations
typedef struct {
  uint8_t ulp_timer_clk_type; ///< true to enable static and false to enable dynamic clock type
  boolean_t
    ulp_timer_sync_to_ulpss_pclk; ///< true to enable and false to disable ULP timer in synchronous mode to ULPSS pclk
  uint8_t
    ulp_timer_clk_input_src; ///< timer input clock source, refer #ulp_timer_clk_input_source_t for possible values
  boolean_t ulp_timer_skip_switch_time; ///< true to wait and false to Skip waiting for switching timer clk
} ulp_timer_clk_src_config_t;

/// @brief Structure to hold the parameters of timer configurations
typedef struct {
  uint8_t timer_num;          ///< timer number, \ref SL_ULP_TIMER_NUMBER for default values
  uint8_t timer_mode;         ///< timer mode, \ref SL_ULP_TIMER_MODE for default values
  uint8_t timer_type;         ///< timer type, \ref SL_ULP_TIMER_TYPE for default values
  uint32_t timer_match_value; ///< timer match value \ref SL_ULP_TIMER_MATCH_VALUE(delay time) in microseconds
  uint8_t timer_direction;    ///< timer direction, \ref SL_ULP_TIMER_DIRECTION for default values
} ulp_timer_config_t;

/// @brief Structure to hold the different versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa version number
  uint8_t minor;   ///< dev version number
} sl_ulp_timer_version_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Configures and enables the ULP-Timer input clock source.
 * By default, configures the 'reference clock source' (32 MHZ) as the timer clock source,
 * clock-type as 'static', disables synchronization to ULPSS pclk and skips waiting
 * for switching of timer clock.
 * It is called inside \ref sl_si91x_ulp_timer_init API.
 * @note:
 * There are two \ref XTAL Clk sources one is Internal and external \ref XTAL clk source.
 * In order to enable the external XTAL clk source need to configure the \ref NPSS_GPIO pins
 * which can be done through sl_si91x_ulp_timer_configure_xtal_clock(uint8_t xtal_pin) API
 * i.e user need to call that API first.
 * In order to enable the soc CLK source need to configure the Ulpss soc Clk from M4 soc clk
 * please refer \ref sl_si91x_ulp_timer_configure_soc_clock(boolean_t div_factor_type, 
                                          uint16_t div_factor )  
 *
 * @param[in] timer_clk_ptr Pointer to timer_clk configuration structure
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer clock type or timer clock source values are invalid 
 *         - \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer clock-source configuration structure members have invalid configurations. 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_clock(ulp_timer_clk_src_config_t *timer_clk_ptr);

/***************************************************************************/ /**
 * Initializes the ULP-Timer module by configuring and enabling the clock source.
 * By default, configures the 'reference clock source' (32 MHZ) as the timer clock source,
 * clock-type as 'static', disables synchronization to ULPSS pclk and skips waiting
 * for switching of timer clock.
 * 
 * @param[in] timer_clk_ptr Pointer to timer_clk configuration structure
 * @return    status 0 if successful, else error code.
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer clock type or timer clock source values are invalid \n
 *         \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer clock-source configuration structure members has invalid configurations. \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_init(ulp_timer_clk_src_config_t *timer_clk_ptr);

/***************************************************************************/ /**
 * Configures the ULP-Timer parameters such as timer number, mode, type, match-value, and direction.
 * Also configures the integral and fractional values of clock cycles per microseconds or
 * per 256 microseconds, as per the timer-type value.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock();
 *
 * @param[in] timer_config_ptr Pointer to the timer_configuration structure
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - Timer configuration structure member 'timer_num' has an invalid value. 
 *         - \ref SL_STATUS_INVALID_MODE (0x0024) - Timer configuration structure member 'timer_mode' has an invalid value. 
 *         - \ref SL_STATUS_INVALID_TYPE (0x0026) - Timer configuration structure member 'timer_type' has an invalid value. 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer configuration structure member 'timer_direction' has an invalid value. 
 *         - \ref SL_STATUS_OK (0x0000) - Success, timer parameters are configured properly
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_configuration(ulp_timer_config_t *timer_config_ptr);

/***************************************************************************/ /**
 * Starts the ULP-Timer.
 * Sets the start bit of timer control register, which is a self clearing bit.
 * Should be called after clock & timer configurations and timeout-callback registrations.
 * Timer instance must not be in running state when calling this API.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_register_timeout_callback 
 * 
 * @param[in] timer_num enum for ULP-timer Number (0 to 3), \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully started the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_start(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Stops the running ULP-Timer instance.
 * Sets the stop bit of timer control register, which is a self clearing bit.
 * This will only stops the timer but not disables & unregisters its interrupts.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_start 
 * 
 * @param[in] timer_num enum for ULP-timer Number (0 to 3), \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully stopped the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_stop(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Restarts an already running ULP-Timer, which means it will first stop the timer instance
 * and then starts it again .
 * This will only restarts the count of an already running counter.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_start 
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully restarted the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_restart(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Set the ULP-Timer type as 256 µs type, 1µs type or Count-down timer type
 * - 1 microsecond type : The time unit is 1µs.
 * - 256 microsecond type : The time unit is 256µs.
 * - Count-down timer type : The time unit is one clock cycle period.
 * This API should be used when timer is stopped
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_stop 
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_type enum for ULP-timer Type, \ref ulp_timer_type_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_TYPE (0x0026) - 'timer_type' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-type
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_type(ulp_timer_instance_t timer_num, ulp_timer_type_t timer_type);

/***************************************************************************/ /**
 * Set the ULP-Timer direction as up-counting or down-counting,
 * by default, it works as down-counter
 * This API should be used when timer is stopped.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_stop 
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_type enum for ULP-timer direction, \ref ulp_timer_direction_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'timer_direction' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-direction
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_direction(ulp_timer_instance_t timer_num, ulp_timer_direction_t timer_direction);

/***************************************************************************/ /**
 * Sets the ULP-Timer Mode as one-shot or periodic.
 * In one shot operation, the timer counts till the timeout and then generates a single interrupt 
 * after which it returns to idle state. Where as in periodic operation the timer when reaches the 
 * timeout value generates an interrupt and starts counting again from the originally set value.
 * This API should be used when timer is stopped
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock 
 *      - \ref sl_si91x_ulp_timer_stop 
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_mode enum for ULP-timer mode, \ref ulp_timer_mode_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_MODE (0x0024) - 'timer_mode' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-mode
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_mode(ulp_timer_instance_t timer_num, ulp_timer_mode_t timer_mode);

/***************************************************************************/ /**
 * Sets the ULP-Timer match value.
 * When the timer reaches the timeout value & if the interrupt is enabled the timeout condition 
 * will generate an interrupt to the processor. 
 * - For one microsecond type, match-value = number of microseconds
 * - For 256 microsecond type, match-value = (time in microseconds)/256
 * - For normal up/down counter type, timer =  (number of clock cycles per microseconds * time in microseconds)
 * This API should be used when timer is stopped.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_stop
 *      - \ref sl_si91x_ulp_timer_set_type
 *      - \ref sl_si91x_ulp_timer_set_direction
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_match_value for ULP-timer timeout value
 * @return status 0 if successful, else error-code
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer match value.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_count(ulp_timer_instance_t timer_num, uint32_t timer_match_value);

/***************************************************************************/ /**
 * Reads the ULP-Timer current count.
 * If counter-down is set register bits indicate time remaining before timeout
 * (read as 32'hFFFF_FFFF initially).
 * If counter-up is set these bits directly gives out the up-running counter/timer value.
 * Updates the timer current-count value to the count-value variable, passed as
 * reference by user (2nd argument)
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_start
 *      - \ref sl_si91x_ulp_timer_set_count
 *
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] count_value Pointer to variable which will store the current count of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to count_value' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_count(ulp_timer_instance_t timer_num, uint32_t *count_value);

/***************************************************************************/ /**
 * Reads the ULP-Timer type.
 * Updates the timer type value to the timer_type variable, passed as
 * reference by user (2nd argument) as below :
 * 0 for down-counter type
 * 1 for 1us type
 * 2 for 256us type
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_set_type
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_type Pointer to variable which will store the current type of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_type' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_type(ulp_timer_instance_t timer_num, uint32_t *timer_type);

/***************************************************************************/ /**
 * Reads the ULP-Timer mode.
 * Updates the timer mode value to the timer_mode variable, passed as
 * reference by user (2nd argument) as below :
 * 0 for one-shot mode
 * 1 for periodic mode
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_set_mode
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_mode Pointer to variable which will store the current mode of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_mode' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current mode
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_mode(ulp_timer_instance_t timer_num, uint32_t *timer_mode);

/***************************************************************************/ /**
 * Read the ULP-Timer direction (up-counter or down-counter).
 * Updates the timer direction to the timer_direction variable, passed as
 * reference by user (2nd argument) as below :
 * 0 for down-counting
 * 1 for up-counting
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_set_direction
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_direction Pointer to variable which will store the current direction of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_direction' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current direction
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_direction(ulp_timer_instance_t timer_num, uint32_t *timer_direction);

/***************************************************************************/ /**
 * Registers the callback for the timer timeout interrupt and enables the interrupt,
 * as per instance.
 * If any callback is already registered, the user needs to unregister the
 * callback first before registering the callback else it will return call \ref SL_STATUS_BUSY
 * error.
 * 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] on_timeout_callback (function pointer) Callback function pointer
 *             to be called when the timer timeout interrupt occurred.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'pointer to callback_data_input' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_ALLOCATION_FAILED (0x0019) - Timer interrupt enabling failed.
 *         - \ref SL_STATUS_BUSY (0x0004) - The callback is already registered,
 *           unregister the previous callback before registering a new one.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered the timer timeout callback
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_register_timeout_callback(ulp_timer_instance_t timer_num,
                                                         ulp_timer_callback_t on_timeout_callback);

/***************************************************************************/ /**
 * Unregisters the callback for the timer timeout interrupt and disabling its interrupts
 * as per timer instance.
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_register_timeout_callback
 *
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_ALLOCATION_FAILED (0x0019) - Timer interrupt disabling failed.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully unregistered the timer timeout callback
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_unregister_timeout_callback(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Configures the Ulpss SoC clock from M4 SOC clock, to enable the SOC clock source.
 * This API is used to select the ULPSS processor clock source when input is soc clock 
 * source which is greater than 100MHz(used for high-power mode)
 * To divide the clock, ensure that it should be odd number if div_factor_type is 1 & viceversa
 * Div_factor_type selects the type of divider for m4_soc_clk_2ulpss
 * - 0 => Even Divider is selected
 * - 1 => Odd Divider is selected
 * Use this API in place of timer clock config API, when using soc clock source
 * with timer in high power mode
 *             
 * @param[in] div_factor value to divide the clock, ensure that it should be an odd number
 *               if div_factor_type is 1 & vice versa
 * @param[in] div_factor_type selects the type of divider for m4_soc_clk_2ulpss
 *              -  0 => Even Divider is selected
 *              -  1 => Odd Divider is selected
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'div_factor' is not according to div_factor_type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_soc_clock(boolean_t div_factor_type, uint16_t div_factor);

/***************************************************************************/ /**
 * Configures the XTAL clock, when clock source is external XTAL clock.
 * To enable the external XTAL clk source need to call this API before calling 
 * \ref sl_si91x_ulp_timer_configure_clock API.
 *
 * @param[in] xtal_pin : Pin number of NPSS_GPIO. Possible values are 0,1,2,3,4
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'xtal_pin' parameter value is invalid.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_xtal_clock(uint8_t xtal_pin);

/***************************************************************************/ /**
 * Initializes the ULP-Timer clock by configuring the clock source.
 * 
 * @param[in] timer_clk_ptr Pointer to timer_clk configuration structure
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer clock type or timer clock source values are invalid
 *         - \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer clock-source configuration structure members have invalid configurations.
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer
 *         - \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_init(ulp_timer_clk_src_config_t *timer_clk_ptr);

/***************************************************************************/ /**
 * De-Initializes the ULP-Timer clock by disabling the peripheral clock.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_unregister_timeout_callback
 *
 * @param[in] none
 * @return none
 * @note Unregisters the callbacks of all timer instances
*******************************************************************************/
void sl_si91x_ulp_timer_deinit(void);

/***************************************************************************/ /**
 * Gets the ULP-timer version.
 * It returns the API version of ULP-timer.
 *
 * @details
 * This function is used to know the ULP_TIMER version
 *
 * @param[in] none
 * @return \ref sl_ulp_timer_version_t type version
 ******************************************************************************/
sl_ulp_timer_version_t sl_si91x_ulp_timer_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /*SL_SI917_ULP_TIMER_*/
