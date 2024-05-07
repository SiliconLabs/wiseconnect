/***************************************************************************/ /**
 * @file sl_si91x_sysrtc.h
 * @brief Sysrtc API implementation
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

#ifndef SL_SI91X_SYSRTC_H
#define SL_SI91X_SYSRTC_H

#ifdef __cplusplus
extern "C" {
#endif

//// Includes
#include "sl_status.h"
#include "rsi_sysrtc_headers.h"
#include "rsi_sysrtc.h"

/***************************************************************************/ /**
 * @addtogroup SYSRTC System RTC
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------
// Macros for SYSRTC parameters
// -----------------------------------------------------------------------------
// Data Types

typedef rsi_sysrtc_clk_inp_t sl_clock_sources_t;            // clock sources enum
typedef rsi_sysrtc_config_t sl_sysrtc_config_t;             // SYSRTC config structure
typedef rsi_sysrtc_group_config_t sl_sysrtc_group_config_t; // SYSRTC group config structure
typedef rsi_sysrtc_group_channel_compare_config_t
  sl_sysrtc_group_compare_channel_action_config_t; // renaming group compare channel config structure
typedef rsi_sysrtc_group_channel_capture_config_t
  sl_sysrtc_group_capture_channel_input_edge_config_t; // renaming group capture channel config structure

/***************************************************************************/ /**
 * Typedef for the function pointer of the interrupt callback function
 *
 * @param callback_flag (void *) parameter for updating flag values
 ******************************************************************************/
typedef void (*sl_sysrtc_callback_t)(void *callback_flag);

/// @brief Enumeration to represent SYSRTC group numbers
typedef enum {
  SL_SYSRTC_GROUP_0,    ///< enum for SYSRTC group-0
  SL_SYSRTC_GROUP_1,    ///< enum for SYSRTC group-1
  SL_SYSRTC_GROUP_LAST, ///< Last member of enum for validation
} sl_sysrtc_group_number_t;

/// @brief Enumeration to represent SYSRTC channel numbers
typedef enum {
  SL_SYSRTC_CHANNEL_0,    ///< enum for SYSRTC channel-0
  SL_SYSRTC_CHANNEL_1,    ///< enum for SYSRTC channel-1
  SL_SYSRTC_CHANNEL_LAST, ///< Last member of enum for validation
} sl_sysrtc_channel_number_t;

/// @brief Structure to hold the parameters of the SYSRTC clock-source configurations
typedef struct {
  uint8_t clock_source;     ///< input clock sources \refsl_clock_sources_t
  uint32_t division_factor; ///< clock division factor
} sl_sysrtc_clock_config_t;

/// @brief Structure to hold the parameters of SYSRTC group interrupt enabling bits
typedef struct {
  boolean_t group0_overflow_interrupt_is_enabled; ///< true to enable group0 overflow interrupt, false to disable it
  boolean_t group0_compare0_interrupt_is_enabled; ///< true to enable group0 overflow interrupt, false to disable it
  boolean_t group0_compare1_interrupt_is_enabled; ///< true to enable group0 overflow interrupt, false to disable it
  boolean_t group0_capture0_interrupt_is_enabled; ///< true to enable group0 overflow interrupt, false to disable it
  boolean_t group1_overflow_interrupt_is_enabled; ///< true to enable group1 overflow interrupt, false to disable it
  boolean_t group1_compare0_interrupt_is_enabled; ///< true to enable group1 overflow interrupt, false to disable it
  boolean_t group1_compare1_interrupt_is_enabled; ///< true to enable group1 overflow interrupt, false to disable it
  boolean_t group1_capture0_interrupt_is_enabled; ///< true to enable group1 overflow interrupt, false to disable it
} sl_sysrtc_interrupt_enables_t;

/// @brief Structure to hold the versions of peripheral API
typedef struct {
  uint8_t release; ///< Release version number
  uint8_t major;   ///< sqa-version number
  uint8_t minor;   ///< dev-version number
} sl_sysrtc_version_t;
// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * @brief Initializes SYSRTC and enables the peripheral.
 * @details Also configures that SYSRTC should run during debug halt, as per user input.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sysrtc_configure_clock
 * @param[in] config_ptr Pointer to SYSRTC config structure \ref sl_sysrtc_config_t.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, SYSRTC initialized properly.
*******************************************************************************/
sl_status_t sl_si91x_sysrtc_init(const sl_sysrtc_config_t *config_ptr);

/***************************************************************************/ /**
 * @brief Configures the SYSRTC input clock source frequency
 * @details It configures clock source as 32khz RC, 32khz RO or 1Khz RO clock source as per user input.
 * For using 1KHz clock source, pass division factor value as '16' else pass '0'.
 *
 * @param[in] clk_ptr Pointer to clock configuration structure \ref sl_sysrtc_clock_config_t.
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Clock source parameter has an invalid value.
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, timer clock-source parameters configured properly.
*******************************************************************************/
sl_status_t sl_si91x_sysrtc_configure_clock(sl_sysrtc_clock_config_t *clk_ptr);

/***************************************************************************/ /**
 * @brief To configure the SYSRTC groups of compare and capture channels
 * @details Configures group number and enables its compare & capture channels.
 * Also configures match out actions for respective compare channel and input events for capture
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sysrtc_configure_clock
 *      - \ref sl_si91x_sysrtc_init
 * @param[in] group_number SYSRTC group number to be used.
 * @param[in] config_ptr (const *) Pointer to group configuration structure \ref sl_sysrtc_group_config_t.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - Counter direction parameter has an invalid value.
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
*******************************************************************************/
sl_status_t sl_si91x_sysrtc_configure_group(sl_sysrtc_group_number_t group_number,
                                            sl_sysrtc_group_config_t const *config_ptr);

/***************************************************************************/ /**
 * @brief To register the callback of the timer interrupt.
 * @details Also enables the respective interrupts as per selected interrupt flags.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sysrtc_init
 *      - \ref sl_si91x_sysrtc_configure_clock
 *      - \ref sl_si91x_sysrtc_configure_group(), keep respective interrupt channel enable
 *      - \ref sl_si91x_sysrtc_unregister_timeout_callback(), if already registered for any interrupt
 * @param[in]  on_sysrtc_callback (function pointer) Callback function pointer
 *             to be called when timer interrupt occurred.
 * @param[in]  callback_flag (void *) pointer to interrupt flag value variable \ref sl_sysrtc_callback_t.
 * @param[in] group_number SYSRTC group number whose interrupts needs to be enabled.
 * @param[in]  interrupt_enable_ptr pointer to interrupt enable structure \ref sl_sysrtc_interrupt_enables_t.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - interrupt_enable_ptr parameter is a null pointer.
 *         - \ref SL_STATUS_BUSY (0x0004) - The callback is already registered, unregister
 *           previous callback before registering a new one.
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully registered timer timeout callback.
*******************************************************************************/
sl_status_t sl_si91x_sysrtc_register_callback(sl_sysrtc_callback_t on_sysrtc_callback,
                                              void *callback_flag_value,
                                              sl_sysrtc_group_number_t group_number,
                                              sl_sysrtc_interrupt_enables_t *interrupt_enable_ptr);

/***************************************************************************/ /**
 * @brief Unregisters timer interrupt callback and disables interrupts as per
 * selected interrupt flag.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sysrtc_register_callback(), first register a particular interrupt flag.
 * @param[in]  interrupt_enable_ptr pointer to interrupts enable structure \ref sl_sysrtc_interrupt_enables_t.
 * @param[in] group_number SYSRTC group number whose interrupts needs to be enabled.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number parameter has an invalid value.
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Successfully unregistered timer interrupt callback.
*******************************************************************************/
sl_status_t sl_si91x_sysrtc_unregister_callback(sl_sysrtc_group_number_t group_number,
                                                sl_sysrtc_interrupt_enables_t *interrupt_enabled_handle);

/***************************************************************************/ /**
 * @brief Sets SYSRTC compare value for the selected channel of the given group.
 * @pre Pre-conditions:
 *      - First enable the compare channel of the respective group through
 *        \ref sl_si91x_sysrtc_configure_group.
 * @param[in] group_number SYSRTC group number to use.
 * @param[in] channel Channel number to use.
 * @param[in] value Compare register value.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number or channel parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_set_compare_value(sl_sysrtc_group_number_t group_number,
                                              sl_sysrtc_channel_number_t channel,
                                              uint32_t compare_value);

/***************************************************************************/ /**
 * @brief Gets SYSRTC current compare register value for selected channel of selected group.
 * @param[in] group_number SYSRTC group number to use.
 * @param[in] channel Channel number to use.
 * @param[in] compare_value Pointer to the variable to store compare value read.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number or channel parameter has an invalid value.
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - compare_value parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_get_compare_value(sl_sysrtc_group_number_t group_number,
                                              sl_sysrtc_channel_number_t channel,
                                              uint32_t *compare_value);

/***************************************************************************/ /**
 * @brief Sets register-input for the capture channel of the selected group of SYSRTC.
 * @details In other words, configures the SYSRTC Input (Capture) from register.
 * @pre Pre-conditions:
 *      - First, enable and configure the capture channel of the respective group through
 *        \ref sl_si91x_sysrtc_configure_group.
 *      - Disable GPIO input through \ref sl_si91x_sysrtc_enable_input_output_gpio, by
 *        passing false.
 * @param[in] group_number SYSRTC group number to use.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_sets_register_capture_input(sl_sysrtc_group_number_t group_number);

/***************************************************************************/ /**
 * @brief Configures input pins for the SYSRTC capture channel of selected group.
 * @details Configures gpio-0 and gpio-1 as input pins for the capture channel of group0 & group-1 
 * respectively.
 * Sets pins mode, mux and direction.
 * @pre Pre-conditions:
 *      - First, enable and configure the capture channel of the respective group through
 *        \ref sl_si91x_sysrtc_configure_group.
 *      - Enable GPIO IO through \ref sl_si91x_sysrtc_enable_input_output_gpio, by
 *        passing true.
 * @param[in] group_number SYSRTC group number to use.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_set_gpio_as_capture_input(sl_sysrtc_group_number_t group_number);

/***************************************************************************/ /**
 * @brief Configures the output pins of the selected compare channel of given group.
 * @details Configures gpio-3 and gpio-4 as output pins for compare channel 0 & 1 of group0 respectively. 
 * Configures gpio-1 and gpio-2 as output pins for compare channel 0 & 1 of group1 respectively..
 * Sets pins mode, mux and direction.
 * Also enables pins input buffer.
 * @pre Pre-conditions:
 *      - First, enable and configure the output action of the compare channel of the respective group through
 *      - \ref sl_si91x_sysrtc_configure_group.
 *
 * @param[in] group_number SYSRTC group number to use.
 * @param[in] channel Channel number to use.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number or channel parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_set_compare_output_gpio(sl_sysrtc_group_number_t group_number,
                                                    sl_sysrtc_channel_number_t channel);

/***************************************************************************/ /**
 * @brief Gets the SYSRTC current counter register value.
 * @details Updates the current count of the timer to count_value input parameter.
 * @param[in] count_value Pointer to the variable to store count value read.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - count_value parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_get_count(uint32_t *count_value);

/***************************************************************************/ /**
 * @brief Gets the SYSRTC capture register value of a given group.
 * @details Updates the capture count value of timer to capture_value input parameter.
 * @param[in] group_number SYSRTC group number to use.
 * @param[in] capture_value Pointer to the variable to store capture value read.
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - capture_value parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_get_capture_value(sl_sysrtc_group_number_t group_number, uint32_t *capture_value);

/***************************************************************************/ /**
 * @brief Reads the SYSRTC compare channel output bit value of a given group.
 * @details Updates the bit value to compare_output_bit input parameter.
 *
 * @param[in] group_number SYSRTC group number to use.
 *
 * @param[in] channel Channel number to use.
 *
 * @param[in] compare_output Pointer to the variable to store compare output.
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - compare_output parameter is a null pointer.
 *         - \ref SL_STATUS_INVALID_PARAMETER (0x0021) - group_number or channel parameter has an invalid value.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_get_compare_output(sl_sysrtc_group_number_t group_number,
                                               sl_sysrtc_channel_number_t channel,
                                               uint32_t *compare_output_bit);

/***************************************************************************/ /**
 * @brief Gets the SYSRTC running status, status is true if running, false if stopped.
 * @details Updates 'true' if SYSRTC is running else 'false' if not running, to the running_status 
 * input parameter.
 *
 * @param[in] running_status Pointer to the variable to store the SYSRTC running status.
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - running_status parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_is_running(boolean_t *running_status);

/***************************************************************************/ /**
 * @brief Gets the SYSRTC lock status.
 * @details Updates 'true' if SYSRTC is locked, 'false' if unlocked, to the lock_status 
 * input parameter.
 *
 * @param[in] lock_status Pointer to the variable to store SYSRTC lock status.
 *
 * @return status 0 if successful, else error code as follows:
 *         - \ref SL_STATUS_NULL_POINTER (0x0022) - lock_status parameter is a null pointer.
 *         - \ref SL_STATUS_OK (0x0000) - Success, parameters configured properly.
 ******************************************************************************/
sl_status_t sl_si91x_sysrtc_is_locked(boolean_t *lock_status);

/***************************************************************************/ /**
 * @brief Sets the SYSRTC input output mode for the capture and compare channels.
 * @details If passed true, then input to capture channel and output of compare channel will
 * be through GPIO, else disabled.
 *
 * @param[in] is_gpio_enabled Bool to enable or disable IO through GPIO.
 *
 * @return none
 ******************************************************************************/
void sl_si91x_sysrtc_enable_input_output_gpio(bool is_gpio_enabled);

/***************************************************************************/ /**
 * @brief Starts the SYSRTC counter.
 * @details This function will send a start command to the SYSRTC peripheral.
 * It waits till the start command to be executed.
 * @pre Pre-conditions:
 *      - \ref sl_si91x_sysrtc_init
 *      - \ref sl_si91x_sysrtc_configure_clock(), keep software trigger disable here
 *      - \ref sl_si91x_sysrtc_configure_group(), keep respective interrupt channel enable
 *      - \ref sl_si91x_sysrtc_register_callback, keep respective interrupt enabled
 *      - \ref sl_si91x_sysrtc_set_count
 * @param none
 * @return none
 * @note This function will send a start command to the SYSRTC peripheral. 
 * @note The SYSRTC peripheral will use some LF clock ticks before the command is executed.
 * @note The rsi_sysrtc_wait_sync() function is used to wait for the start
 * command to be executed.
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_start(void)
{
  rsi_sysrtc_start();
}

/***************************************************************************/ /**
 * @brief Stops the SYSRTC counter.
 * @details This function will send a stop command to the SYSRTC peripheral.
 * It waits till the stop command to be executed.
 *
 * @param    none
 *
 * @return   none
 *
 * @note  This function requires the SYSRTC to be enabled.
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_stop(void)
{
  rsi_sysrtc_stop();
}

/***************************************************************************/ /**
 * @brief Restores the SYSRTC to its reset state.
 *
 * @param    none
 *
 * @return   none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_reset(void)
{
  rsi_sysrtc_reset();
}

/***************************************************************************/ /**
 * @brief Locks the SYSRTC registers.
 * @details When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 * SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written to.
 * @param    none
 * @return   none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_lock(void)
{
  rsi_sysrtc_lock();
}

/***************************************************************************/ /**
 * @brief Unlocks the SYSRTC registers.
 * @details When SYSRTC registers are locked SYSRTC_EN, SYSRTC_CFG, SYSRTC_CMD,
 * SYSRTC_SWRST, SYSRTC_CNT and SYSRTC_TOPCNT registers cannot be written to.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sysrtc_lock
 * @param    none
 * @return   none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_unlock(void)
{
  rsi_sysrtc_unlock();
}

/***************************************************************************/ /**
 * @brief Sets the SYSRTC counter register value of counter
 * This can be used to change the count of SYSRTC, when it is stopped.
 * After calling this API user should start SYSRTC.
 * @pre Pre-condition:
 *      - \ref sl_si91x_sysrtc_stop
 * @param[in] value   The new SYSRTC counter value.
 * @return   none
 ******************************************************************************/
__STATIC_INLINE void sl_si91x_sysrtc_set_count(uint32_t value)
{
  rsi_sysrtc_set_counter(value);
}

/***************************************************************************/ /**
 * @brief Gets the SYSRTC version
 * It returns API version of SYSRTC
 * @param[in] none
 * @return (sl_sysrtc_version_t) type structure.
 ******************************************************************************/
sl_sysrtc_version_t sl_si91x_sysrtc_get_version(void);

/***************************************************************************/ /**
* @brief De-initializes the SYSRTC by disabling its clock.
* @param[in]   none
* @return      none
* @note Unregisters the SYSRTC callback and disables all interrupts.
*******************************************************************************/
void sl_si91x_sysrtc_deinit(void);

/** @} (end addtogroup SYSRTC) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup SYSRTC System RTC
/// @{
///
///   @details
///
///   @n @section SYSRTC_Intro Introduction
///   The System Real-Time Clock (SYSRTC) offers precise timekeeping and timer
///   functionality essential for various applications. It serves as a cornerstone for tasks requiring accurate time measurement,
///   event scheduling, and power management. This SYSRTC driver provides a robust set of
///   functions to configure and use this peripheral effectively, ensuring optimal performance and reliability in time-critical applications.
///
///   @n @section SYSRTC_Config Configuration
///   The @ref sl_si91x_sysrtc_configure_clock function allows setting the clock source frequency,
///   choosing between 32kHz RC, 1kHz RO clock sources. Initialization of the SYSRTC is achieved through
///   the @ref sl_si91x_sysrtc_init function, which not only initializes the peripheral but also enables it. Additionally,
///   SYSRTC can be configured to run during debug halt by specifying the appropriate setting in the initialization function.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document.
///
///   @n @section SYSRTC_Usage Usage
///
///   @li SYSRTC has to be initialised by calling the @ref sl_si91x_sysrtc_configure_clock and @ref sl_si91x_sysrtc_init.
///   @li Once configured, the SYSRTC can be tailored to specific application requirements using the functions in the driver.
///   The @ref sl_si91x_sysrtc_configure_group configures group number and enables its compare & capture channels.
///   To handle timer interrupts and execute actions accordingly, users can register callback functions using the @ref sl_si91x_sysrtc_register_callback function.
///   Furthermore, functions such as @ref sl_si91x_sysrtc_set_compare_value and @ref sl_si91x_sysrtc_get_count facilitate setting compare values and retrieving the
///   current counter value, respectively, allowing for fine-grained control over time-sensitive operations.
///   Functions such as @ref sl_si91x_sysrtc_start and @ref sl_si91x_sysrtc_stop
///   enable starting and stopping the timer, while the @ref sl_si91x_sysrtc_lock locks the SYSRTC registers.
///
///   @li Below functions will initiate and configure the SYSRTC, which is the general flow for implementation:
///
///   1. @ref sl_si91x_sysrtc_configure_clock
///   2. @ref sl_si91x_sysrtc_init
///   3. @ref sl_si91x_sysrtc_configure_group
///   4. @ref sl_si91x_sysrtc_register_callback
///   5. @ref sl_si91x_sysrtc_set_count
///   6. @ref sl_si91x_sysrtc_set_compare_value (If compare channel is enabled)
///   7. @ref sl_si91x_sysrtc_is_running (If required to check timer running status)
///   8. @ref sl_si91x_sysrtc_start
///   9. @ref sl_si91x_sysrtc_get_count (If required to check timer current count)
///   10. @ref sl_si91x_sysrtc_stop
///   11. @ref sl_si91x_sysrtc_deinit
///

///
/// @} end group SYSRTC ********************************************************/

#ifdef __cplusplus
}
#endif

#endif /*SL_SI917_SYSRTC*/
