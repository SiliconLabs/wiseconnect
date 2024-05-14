/***************************************************************************/ /**
* @file sl_si91x_button.h
* @brief See @ref button for documentation.
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef SL_SI91X_BUTTON_H
#define SL_SI91X_BUTTON_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup BUTTON Button
 * @ingroup SI91X_HARDWARE_DRIVER_APIS
 * @brief Sample API functions for using push-buttons.
 *
 * See sl_si91x_button.c for source code.
 *@{
 */

/** @name Macros
 * A set of numerical macros for use with the button APIs.
 *@{
 */

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/** @brief Interrupt on low/pressed button state can be configured
 */
#define HIGH_LEVEL_INTERRUPT 1U ///< Interrupt on high/released button state

#define LOW_LEVEL_INTERRUPT 2U ///< Interrupt on low/pressed button state can be configured

#define HIGH_LEVEL_AND_LOW_LEVEL_INTERRUPT \
  3U ///< Interrupt on low/pressed and high/released button state can be configured

#define RISE_EDGE_INTERRUPT 4U ///< Interrupt on rising edge of the button press

#define FALL_EDGE_INTERRUPT 8U ///< Interrupt on falling edge of the button press

#define RISE_EDGE_AND_FALL_EDGE_INTERRUPT 12U ///< Interrupt on rising edge and falling edge of the button press

#define BUTTON_PRESSED 1 ///< Button state is pressed

#define BUTTON_RELEASED 0 ///< Button state is released

#define BUTTON_STATE_INVALID -1 ///< Button state is invalid

/**@} END Macros */

typedef struct {
  uint8_t pin;
  uint8_t port;
  uint8_t button_number;
  uint8_t pad;
  uint8_t interrupt_config;
} sl_button_t;

/***************************************************************************/ /**
* Initializes the button, a process automatically triggered upon the creation of a button instance, sets the mode of operation, and configures interrupts based on the selected mode.
*
* @param[in] handle  The pointer to button structure that has the specific button information.
*
* @return none
*
******************************************************************************/
void sl_si91x_button_init(const sl_button_t *handle);

/***************************************************************************/ /**
* Returns the current state (pressed or released) of a button.
*
* @note This function is correlated with ::sl_si91x_button_isr() and so returns the
* shadow state rather than reading the actual state of the pin.
*
* @param[in] pin  The button pin being queried, either BUTTON0 pin or BUTTON1 pin as
* defined.
*
* @return ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
* if the button is not pressed.
*
******************************************************************************/
int8_t sl_si91x_button_state_get(uint8_t pin);

/***************************************************************************/ /**
* Returns the current state (pressed or released) of the
* pin associated with a button.
*
* This reads the actual state of the pin and can be used on startup to
* determine the initial position of the buttons.
*
* @param[in] pin  The button pin being queried, either BUTTON0 pin or BUTTON1 pin as
* defined.
*
* @return  ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
* if the button is not pressed.
*
******************************************************************************/
int8_t sl_si91x_button_pin_state(uint8_t pin);

/***************************************************************************/ /**
* A callback called in interrupt context whenever a button
* changes its state.
*
* @note Must be implemented by the application.  This function should
* contain the functionality to be executed in response to changes of state
* in each of the buttons, or callbacks to the appropriate functionality.
*
* @param[in] pin  The button pin which has changed state, either BUTTON0 pin or BUTTON1 pin
* as defined.
*
* @param[out] state   The new state of the button referenced by the button parameter,
* either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED if
* the button has been released.
*
* @return  none
*
******************************************************************************/
void sl_si91x_button_pin_isr(uint8_t pin, uint8_t state);

/***************************************************************************/ /**
* Toggles the value of a local variable associated with the state of button
*
* @note Must be implemented by the application.  This function should
* contain the functionality to be executed in response to changes of state
* in each of the buttons, or callbacks to the appropriate functionality.
*
* @param[in] pin  The button pin which has changed state, either BUTTON0 pin or BUTTON1 pin
* as defined.
*
* @return  none
*
******************************************************************************/
void sl_si91x_button_state_toggle(uint8_t pin);

/***************************************************************************/ /**
* Sets the state (pressed or released) of the
* pin associated with a button.
*
* @note Must be implemented by the application.  This function should
* contain the functionality to be executed in response to changes of state
* in each of the buttons, or callbacks to the appropriate functionality.
*
* @param[in] pin  The button pin which has changed state, either BUTTON0 pin or BUTTON1 pin
* as defined.
*
* @param[in] state   The state of the button to be set,
* either ::BUTTON_PRESSED if the button state is pressed or ::BUTTON_RELEASED if
* the button state is released.
*
* @return  none
*
******************************************************************************/
void sl_si91x_button_state_set(uint8_t pin, int8_t state);

/***************************************************************************/ /**
* A callback that user can modify for the application.
*
* @note Must be implemented by the application.  This function should
* contain the functionality to be executed in response to changes of state
* in each of the buttons, or callbacks to the appropriate functionality.
*
* @param[in] pin  The button pin which has changed state, either BUTTON0 pin or BUTTON1 pin
* as defined.
*
* @param[in] state   The new state of the button referenced by the button parameter,
* either ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED if
* the button is released.
*
* @return  none
*
******************************************************************************/
void sl_si91x_button_isr(uint8_t pin, int8_t state);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup BUTTON Button
/// @{
///
///   @details
///
///   @n @section buttondrv_intro Introduction
///
///   The SI91x button driver is a platform-level software module responsible for managing the initialization and reading buttons.
///   It offers a comprehensive set of functions for button operations, including initialization, state monitoring,
///   and callback handling for state changes. The driver supports flexible interrupt configurations,
///   enabling detection based on button state, edge transitions, or both. All button functions are called through the generic driver,
///   which internally triggers specific functions to the respective button.
///
///   @n @section buttondrv_config Configuration
///
///   All button instances are configured with an @ref sl_button_t struct. This struct is automatically generated after a button is set up
///   using Simplicity Studio's wizard.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document and
///   refer here https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-si91x-peripherals/disable-uc-config.
///
///
///   @n @section buttondrv_usage Usage
///
///   Once the button structures are defined, the button functions can be invoked by passing an instance of sl_button_t,
///   which will be redirected to call the type-specific version of that function. The common functions include the following:
///
///   @li @ref sl_si91x_button_init
///   @li @ref sl_si91x_button_pin_state
///   @li @ref sl_si91x_button_state_toggle
///   @li @ref sl_si91x_button_isr
///
///   @ref sl_button_init is called automatically when the application is run.
///
///   The button driver can be used with interrupt mode, with or without debounce. @ref sl_si91x_button_pin_state can be implemented by the application if required. This function is used
///   to determine the current state of the button. @ref sl_si91x_button_state_toggle can be implemented by the application.
///   This function toggles the "soft" state so that it gives the interrupts and their callbacks. @ref sl_si91x_button_isr is a callback function
///   that is called when the button state changes. This function can be implemented in the application to specify what the user wants to do
///   when the button state changes.
///
/// @} end group button ********************************************************/

/** @} END addtogroup
 */

#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_BUTTON_H