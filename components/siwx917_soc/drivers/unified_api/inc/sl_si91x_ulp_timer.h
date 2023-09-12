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

/// @brief Enumeration to represent ulp-timer instances
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
  ULP_TIMER_ULP_SOC_CLK_SRC,    ///< SoC clock input source
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
 * Configure the ULP-Timer input clock source.
 *
 * @param[in] timer_clk_ptr Pointer to timer_clk configuration structure
 * @return    status 0 if successful, else error code.
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer clock type or timer clock source values are invalid \n
 *         \ref SL_STATUS_INVALID_CONFIGURATION (0x0023) - Timer clock-source configuration structure members has invalid configurations. \n
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_clock(ulp_timer_clk_src_config_t *timer_clk_ptr);

/***************************************************************************/ /**
 * Configure ULP-Timer parameters such as timer number, mode, type, match-value & direction.
 * Also configures integral and fractional values of clock cycles per microseconds or
 * per 256 microseconds, as per timer-type value.
 *
 * @pre  \ref sl_si91x_ulp_timer_configure_clock();
 *
 * @param[in] timer_config_ptr Pointer to timer_cofiguration structure
 *
 * @return
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) -Timer configuration structure member 'timer_num' has invalid value. \n
 *         \ref SL_STATUS_INVALID_MODE (0x0024) - Timer configuration structure member 'timer_mode' has invalid value. \n
 *         \ref SL_STATUS_INVALID_TYPE (0x0026) - Timer configuration structure member 'timer_type' has invalid value. \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Timer configuration structure member 'timer_direction' has invalid value. \n
 *         \ref SL_STATUS_OK (0x0000) - Success, timer parameters are configured properly
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_configuration(ulp_timer_config_t *timer_config_ptr);

/***************************************************************************/ /**
 * Start the ULP-Timer.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_set_configuration \n
 *      \ref sl_si91x_ulp_timer_register_timeout_callback \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number (0 to 3), \ref #ulp_timer_instance_t for possible values
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully started the timer-instance
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_start(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Stop the ULP-Timer.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_set_configuration \n
 *      \ref sl_si91x_ulp_timer_start \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number (0 to 3), \ref #ulp_timer_instance_t for possible values
 * @return    status 0 if successful, else error code
 *         SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         SL_STATUS_OK (0x0000) - Successfully stopped the timer instance
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_stop(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Restart an already running ULP-Timer, means it will first stop the timer and then start it again.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_set_configuration \n
 *      \ref sl_si91x_ulp_timer_start \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully restarted the timer instance
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_restart(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Set the ULP-Timer type.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_stop \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values
 * @param[in]  timer_type enum for ULP-timer Type, \ref #ulp_timer_type_t for possible values
 *
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_INVALID_TYPE (0x0026) - 'timer_type' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully set timer-type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_type(ulp_timer_instance_t timer_num, ulp_timer_type_t timer_type);

/***************************************************************************/ /**
 * Set the ULP-Timer direction as up-counting or down-counting
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_stop \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values
 * @param[in]  timer_type enum for ULP-timer direction, \ref #ulp_timer_direction_t for possible values
 *
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_INVALID_PARAMETER (0x0021) - 'timer_direction' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully set timer-direction
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_direction(ulp_timer_instance_t timer_num, ulp_timer_direction_t timer_direction);

/***************************************************************************/ /**
 * Set ULP-Timer Mode.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_stop \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values
 * @param[in]  timer_mode enum for ULP-timer mode, \ref #ulp_timer_mode_t for possible values
 *
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_INVALID_MODE (0x0024) - 'timer_mode' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully set timer-mode
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_mode(ulp_timer_instance_t timer_num, ulp_timer_mode_t timer_mode);

/***************************************************************************/ /**
 * Set ULP-Timer match value.
 *
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_stop \n
 *      \ref sl_si91x_ulp_timer_set_type \n
 *      \ref sl_si91x_ulp_timer_set_direction \n 
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  timer_match_value for ULP-timer timeout value
 *
 * @return    status 0 if successful, else error-code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully set timer match value.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_set_count(ulp_timer_instance_t timer_num, uint32_t timer_match_value);

/***************************************************************************/ /**
 * Read the ULP-Timer count.
 * 
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_start \n
 *      \ref sl_si91x_ulp_timer_set_count \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  count_value Pointer to variable which will store current count of timer
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) -'Pointer to count_value' parameter is a null pointer. \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully read timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_count(ulp_timer_instance_t timer_num, uint32_t *count_value);

/***************************************************************************/ /**
 * Read the ULP-Timer type.
 * 
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n 
 *      \ref sl_si91x_ulp_timer_set_type \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  count_value Pointer to variable which will store current count of timer
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) -'Pointer to timer_type' parameter is a null pointer. \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully read timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_type(ulp_timer_instance_t timer_num, uint32_t *timer_type);

/***************************************************************************/ /**
 * Read the ULP-Timer mode.
 * 
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_set_mode \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  count_value Pointer to variable which will store current count of timer
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) -'Pointer to timer_mode' parameter is a null pointer. \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully read timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_mode(ulp_timer_instance_t timer_num, uint32_t *timer_mode);

/***************************************************************************/ /**
 * Read the ULP-Timer direction (up-counter or down-counter).
 * 
 * @pre \ref sl_si91x_ulp_timer_configure_clock \n
 *      \ref sl_si91x_ulp_timer_set_direction \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  count_value Pointer to variable which will store current count of timer
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) -'Pointer to timer_direction' parameter is a null pointer. \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully read timer's current count value
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_get_direction(ulp_timer_instance_t timer_num, uint32_t *timer_direction);

/***************************************************************************/ /**
 * Register callback of timer timeout interrupt and enabling it.
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @param[in]  on_timeout_callback (function pointer) Callback function pointer
 *             to be called when timer timeout interrupt occurred.
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_NULL_POINTER (0x0022) - 'pointer to callback_data_input' parameter is a null pointer. \n
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_ALLOCATION_FAILED (0x0019) - Timer interrupt enabling failed. \n
 *         \ref SL_STATUS_BUSY (0x0004) - The callback is already registered,
 *         unregister previous callback before registering new one \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully registered timer timer-out callback
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_register_timeout_callback(ulp_timer_instance_t timer_num,
                                                         ulp_timer_callback_t on_timeout_callback);

/***************************************************************************/ /**
 * Unregister callback of timer timeout interrupt and disabling it.
 * 
 * @pre \ref sl_si91x_ulp_timer_register_timeout_callback \n
 * 
 * @param[in]  timer_num enum for ULP-timer Number, \ref #ulp_timer_instance_t for possible values.
 * @return    status 0 if successful, else error code
 *         \ref SL_STATUS_INVALID_INDEX (0x0027) - 'timer_num' parameter value is invalid. \n
 *         \ref SL_STATUS_ALLOCATION_FAILED (0x0019) - Timer interrupt disabling failed. \n
 *         \ref SL_STATUS_OK (0x0000) - Successfully unregistered timer timer-out callback
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_unregister_timeout_callback(ulp_timer_instance_t timer_num);

/***************************************************************************/ /**
 * Configure the Ulpss SoC clock from M4 SOC clock, to enable the SOC clock source.
 *             
 * @param[in]   div_factor value to divide the clock, ensure that it should be odd number
 *               if div_factor_type is 1 & vice versa
 * @param[in]   div_factor_type selects the type of divider for m4_soc_clk_2ulpss
 *              -  0 => Even Divider is selected
 *               -  1 => Odd Divider is selected
 * @return      status 0 if successful, else error code
 *       \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'div_factor' is not according to div_factor_type
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_soc_clock(boolean_t div_factor_type, uint16_t div_factor);

/***************************************************************************/ /**
 * Configure the XTAL clock, when clock source is external XTAL clock.
 *
 * @param[in]   xtal_pin : Pin number of NPSS_GPIO. Possible values are 0,1,2,3,4
 * @return      status 0 if successful, else error code
 *        \ref SL_STATUS_INVALID_PARAMETER(0x0021) - 'xtal_pin' parameter value is invalid.
*******************************************************************************/
sl_status_t sl_si91x_ulp_timer_configure_xtal_clock(uint8_t xtal_pin);

/***************************************************************************/ /**
 * Initialize ULP-Timer clock, by configuring clock source.
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
 * De-Initializes ULP-Timer clock, by disabling peripheral clock.
 * 
 * @pre \ref sl_si91x_ulp_timer_init \n
 *      \ref sl_si91x_ulp_timer_unregister_timeout_callback \n
 *
 * @param[in]   none
 * @return      none
*******************************************************************************/
void sl_si91x_ulp_timer_deinit(void);

/***************************************************************************/ /**
 * @brief
 * Get the ULP_TIMER version.
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
