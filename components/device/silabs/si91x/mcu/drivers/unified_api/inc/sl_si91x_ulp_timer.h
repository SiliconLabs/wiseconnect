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
#define SL_ULP_TIMER_CLK_TYPE_STATIC  1
#define SL_ULP_TIMER_CLK_TYPE_DYNAMIC 0
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
 * @brief This API will initialize the ULP-Timer by enabling its clock source.
 * @details It configures clock frequency, clock-type, enables/disables synchronization to ULPSS pclk 
 * & skips/allows waiting for timer clock switching.
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
 * @brief This API will configures and enables the ULP-Timer input clock source.
 * @details It configures clock frequency, clock-type, enables/disables synchronization to ULPSS pclk 
 * & skips/allows waiting for timer clock switching.
 * It is called inside \ref sl_si91x_ulp_timer_init API.
 * 
 * @note 
 * @li There are two XTAL Clk sources internal and external
 * In order to enable the external XTAL clk source need to configure the NPSS_GPIO pins
 * through \ref sl_si91x_ulp_timer_configure_xtal_clock API
 * i.e user need to call this API first.
 * @li In order to use SOC source user need to use \ref sl_si91x_ulp_timer_configure_soc_clock API 
 * to configure the ULPSS SOC instead of clock configure API.
 *
 * @param[in] timer_clk_ptr Pointer to timer_clk configuration structure \ref ulp_timer_clk_src_config_t.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer clock type or timer clock source values are invalid 
 *         - \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer clock-source configuration structure members have invalid configurations. 
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 *         - \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_clock(ulp_timer_clk_src_config_t *timer_clk_ptr);

/***************************************************************************/ /**
 * @brief This API will configure the ULP-Timer parameters.
 * @details It configures the parameters such as timer number, 
 * mode, type, match-value, and direction.
 * Also configures the integral and fractional values of clock cycles per microseconds or
 * per 256 microseconds as per the selected timer-type.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init();
 *
 * @param[in] timer_config_ptr Pointer to the timer_configuration structure \ref ulp_timer_config_t.
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
 * @brief This API will start the ULP-Timer.
 * @note Timer instance must not be in running state when calling this API.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init 
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_register_timeout_callback 
 * @param[in] timer_num enum for ULP-timer Number (0 to 3), \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully started the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_start(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * @brief This API will stop the 'timer_num' instance of timer.
 * @details Sets the stop bit of timer control register, which is a self clearing bit.
 * This will only stops the timer but not disables it. To disable it use @ref sl_si91x_ulp_timer_deinit.
 * 
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_start 
 * @param[in] timer_num enum for ULP-timer Number (0 to 3), \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully stopped the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_stop(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * @brief This API will restart an already running ULP-Timer.
 * @details ULP-Timer restart which means it will first stop the timer instance
 * and then starts it again.
 * This will just restart the currently running counter.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_set_configuration 
 *      - \ref sl_si91x_ulp_timer_start 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully restarted the timer instance
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_restart(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * @brief This API will set the ULP-Timer type.
 * @details Timer types include 256 µs, 1 µs, and count-down.
 * - 1 microsecond type : The time unit is 1µs.
 * - 256 microsecond type : The time unit is 256µs.
 * - Count-down timer type : The time unit is one clock cycle period.
 * @note This API should be used when timer is stopped.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_stop 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_type enum for ULP-timer Type, \ref ulp_timer_type_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_TYPE (0x0026) - 'timer_type' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-type
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_type(ulp_timer_instance_t timer_num, ulp_timer_type_t timer_type);

/***************************************************************************/ /**
 * @brief This API will set the ULP-Timer direction. 
 * @details ULP timer set direction will set the ULP-Timer direction as up-counting or down-counting,
 * by default it works as down-counter.
 * @note This API should be used when timer is stopped.
 *
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_stop 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_type enum for ULP-timer direction, \ref ulp_timer_direction_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'timer_direction' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-direction
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_direction(ulp_timer_instance_t timer_num, ulp_timer_direction_t timer_direction);

/***************************************************************************/ /**
 * @brief This API will set the ULP-Timer Mode as one-shot or periodic.
 * @details In one shot operation, the timer counts till the timeout and then generates a single interrupt 
 * after which it returns to idle state. Where as in periodic operation the timer generates interrupts continously with
 * interval equal to timeout.
 * This API should be used when timer is stopped
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_stop 
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values
 * @param[in] timer_mode enum for ULP-timer mode, \ref ulp_timer_mode_t for possible values
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. 
 *         - \ref SL_STATUS_INVALID_MODE (0x0024) - 'timer_mode' parameter value is invalid. 
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer-mode
******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_mode(ulp_timer_instance_t timer_num, ulp_timer_mode_t timer_mode);

/***************************************************************************/ /**
 * @brief This API will sets the ULP-Timer match value.
 * @details When the timer reaches the match value & if the interrupt is enabled it
 * will generate an interrupt to the processor. 
 * - For one microsecond type, match-value = number of microseconds
 * - For 256 microsecond type, match-value = (time in microseconds)/256
 * - For normal up/down counter type, timer =  (number of clock cycles per microseconds * time in microseconds).
 * 
 * @note This API should be used when timer is stopped.
 * @pre Pre-conditions:
 *  - \ref sl_si91x_ulp_timer_init
 *  - \ref sl_si91x_ulp_timer_stop
 *  - \ref sl_si91x_ulp_timer_set_type
 *  - \ref sl_si91x_ulp_timer_set_direction
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_match_value for ULP-timer timeout value
 * @return status 0 if successful, else error-code
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully set timer match value.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_count(ulp_timer_instance_t timer_num, uint32_t timer_match_value);

/***************************************************************************/ /**
 * @brief This API will read the ULP-Timer current count.
 * @details If counter-down is set then counter register bits indicate time remaining before timeout
 * (read as 32'hFFFF_FFFF initially).
 * If counter-up is set then these bits directly gives out the up-running counter/timer value.
 * Updates the timer current-count value to the count-value variable, passed as
 * reference by user (2nd argument).
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_start
 *      - \ref sl_si91x_ulp_timer_set_count
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] count_value Pointer to variable which will store the current count of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to count_value' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_count(ulp_timer_instance_t timer_num, uint32_t *count_value);

/***************************************************************************/ /**
 * @brief This API will read the ULP-Timer type.
 * @details Updates the timer type value to the timer_type variable, passed as
 * reference by user.
 *  - '0' for down-counter type
 *  - '1' for 1us type
 *  - '2' for 256us type
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_set_type
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_type Pointer to variable which will store the current type of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_type' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_type(ulp_timer_instance_t timer_num, uint32_t *timer_type);

/***************************************************************************/ /**
 * @brief This API will reads the ULP-Timer mode.
 * @details Updates the timer mode value to the timer_mode variable, passed as
 * reference by user.
 *  - '0' for one-shot mode
 *  - '1' for periodic mode
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_configure_clock
 *      - \ref sl_si91x_ulp_timer_set_mode
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_mode Pointer to variable which will store the current mode of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_mode' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current mode
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_mode(ulp_timer_instance_t timer_num, uint32_t *timer_mode);

/***************************************************************************/ /**
 * @brief This API will read the ULP-Timer direction (up-counter or down-counter).
 * @details Updates the timer direction to the timer_direction variable, passed as
 * reference by user.
 *  - '0' for down-counting
 *  - '1' for up-counting
 * 
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_set_direction
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @param[in] timer_direction Pointer to variable which will store the current direction of the timer
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - 'Pointer to timer_direction' parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully read the timer's current direction
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_direction(ulp_timer_instance_t timer_num, uint32_t *timer_direction);

/***************************************************************************/ /**
 * @brief This API will registers the callback for the timer timeout interrupt and enables the interrupt,
 * as per instance.
 * @details If any callback is already registered, the user needs to unregister the
 * callback first before registering the callback else it will return call \ref SL_STATUS_BUSY
 * error.
 *
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 *
 * @param[in] on_timeout_callback (function pointer) Callback function pointer
 *             to be called when the timer timeout interrupt occurred.
 *
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
 * @brief This API will unregister the callback for the timer interrupt and disabling its interrupts
 * as per timer instance.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_register_timeout_callback
 * @param[in] timer_num enum for ULP-timer Number, \ref ulp_timer_instance_t for possible values.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid.
 *         - \ref SL_STATUS_ALLOCATION_FAILED (0x0019) - Timer interrupt disabling failed.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully unregistered the timer timeout callback
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_unregister_timeout_callback(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * @brief This API will configure the ULPSS SOC clock from M4 SOC clock, to enable the SOC clock source.
 * @details This API is used to select the ULPSS processor clock source as timer input clock 
 * source which is greater than 100MHz(used for high-power mode)
 * To divide the clock, ensure that it should be odd number if div_factor_type is 1 & viceversa
 * Div_factor_type selects the type of divider for m4_soc_clk_2ulpss
 *  - '0' Even Divider is selected
 *  - '1' Odd Divider is selected
 * Use this API in place of timer clock config API, when using SOC clock source
 * with timer in high power mode
 *             
 * @param[in] div_factor value to divide the clock, ensure that it should be an odd number
 *               if div_factor_type is 1 & vice versa
 * @param[in] div_factor_type selects the type of divider for m4_soc_clk_2ulpss
 *              -  '0' Even Divider is selected
 *              -  '1' Odd Divider is selected
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'div_factor' is not according to div_factor_type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_soc_clock(boolean_t div_factor_type, uint16_t div_factor);

/***************************************************************************/ /**
 * @brief This API will configure the XTAL clock, when clock source is external XTAL clock.
 * @details To enable the external XTAL clk source need to call this API before calling 
 * \ref sl_si91x_ulp_timer_configure_clock API.
 *
 * @param[in] xtal_pin : Pin number of NPSS_GPIO. Possible values are 0 to 4.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'xtal_pin' parameter value is invalid.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_xtal_clock(uint8_t xtal_pin);

/***************************************************************************/ /**
 * @brief This API will de-Initializes the ULP-Timer clock by disabling the peripheral clock.
 * @details  It also Unregisters callback of all timer instances
 * @pre Pre-conditions:
 *      - \ref sl_si91x_ulp_timer_init
 *      - \ref sl_si91x_ulp_timer_unregister_timeout_callback
 *
 * @param none
 * @return none
 * @note Unregisters the callbacks of all timer instances
*******************************************************************************/
void sl_si91x_ulp_timer_deinit(void);

/***************************************************************************/ /**
 * @brief This API will get the ULP-timer API version.
 * @details This API is used to know the API version of ULP_TIMER.
 *
 * @param none
 * @return \ref sl_ulp_timer_version_t type version
 ******************************************************************************/
sl_ulp_timer_version_t sl_si91x_ulp_timer_get_version(void);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY! ***********************
/// @addtogroup ULP-TIMER Ultra Low-Power Timer
/// @{
///
///   @details
///
///
///   @n @section ULP-TIMER_Intro Introduction
///
///   Ultra-Low-Power (ULP) timers typically used to operate with extremely low power consumption.
///   These timers are commonly found in battery-powered or energy-efficient devices where minimizing
///   power usage is crucial to extending battery life.
///
///   ULP timers typically provide configurable settings, allowing developers to adjust parameters such
///   as clock frequency, timer resolution, and interrupt behavior to meet the specific needs of their applications.
///
///  @li The ULP-Timer module supports 4 different instances @ref ulp_timer_instance_t.
///  @li Each of the ULP-timers can be independently programmed to work in periodic or one-shot mode @ref ulp_timer_mode_t.
///  @li Each of the ULP-timers can be independently configured as a 32-bit counter or as a microsecond timer @ref ulp_timer_type_t.
///
///   @n @section ULP-TIMER_Config Configuration
///   @li By calling the @ref sl_si91x_ulp_timer_init ulp-timer is initialized and its clocks are configured.
///   @li By using @ref sl_si91x_ulp_timer_set_configuration
///   ulp-timer parameters including match count, timer type, mode, timer direction can be configured.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section ULP-TIMER_Use Usage
///
///   The common ULP-timer functions can be used after ulp-timer Structures are specified,
///   passing an instance of   @ref ulp_timer_config_t.
///   @li These functions will initiate and configure the ULP-timer below, which is the flow for implementation.
///
///   1. @ref sl_si91x_ulp_timer_init
///   2. @ref sl_si91x_ulp_timer_set_configuration
///   3. @ref sl_si91x_ulp_timer_register_timeout_callback
///   4. @ref sl_si91x_ulp_timer_start
///   5. @ref sl_si91x_ulp_timer_stop
///   6. @ref sl_si91x_ulp_timer_deinit
///
///   @li Above functions allow for initializing the ULP-TIMER instance and generates interrupt upon expiration.
///
///   @li Other functions are called to change the timer type, direction and to unregister the callback.
///   1. @ref sl_si91x_ulp_timer_set_type
///   2. @ref sl_si91x_ulp_timer_set_mode
///   3. @ref sl_si91x_ulp_timer_set_direction
///   4. @ref sl_si91x_ulp_timer_set_count
///
///   @li Below section shows functions to read timer parameters:
///
///   1. @ref sl_si91x_ulp_timer_get_mode
///   2. @ref sl_si91x_ulp_timer_get_type
///   3. @ref sl_si91x_ulp_timer_get_direction
///   4. @ref sl_si91x_ulp_timer_get_count
///
/// @} end group ULP-TIMER ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /*SL_SI917_ULP_TIMER_*/
