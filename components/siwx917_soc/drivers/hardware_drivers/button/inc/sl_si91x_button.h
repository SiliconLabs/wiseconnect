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

#include "sl_si91x_button_config.h"
#include "rsi_board.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup button
 * @brief Sample API functions for using push-buttons.
 *
 * See button.c for source code.
 *@{
 */

/** @name Button State Definitions
 * A set of numerical definitions for use with the button APIs indicating the
 * state of a button.
 *@{
 */

typedef struct {
  uint8_t pin;
  uint8_t port;
  uint8_t button_number;
} sl_button_t;

/** @brief Button state is pressed.
 */
#define BUTTON_PRESSED 1

/** @brief Button state is released.
 */
#define BUTTON_RELEASED 0

/** @brief Button state is invalid.
 */
#define BUTTON_STATE_INVALID -1

/**@} END Button State Definitions */

/** @brief Initializes the buttons. This function is automatically called
 * by ::halInit().
 */
void sl_si91x_button_init(const sl_button_t *handle);

/** @brief Returns the current state (pressed or released) of a button.
 *
 * @note This function is correlated with ::sl_si91x_button_isr() and so returns the
 * shadow state rather than reading the actual state of the pin.
 *
 * @param pin  The button pin being queried, either BUTTON0 pin or BUTTON1 pin as
 * defined.
 *
 * @return ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
 * if the button is not pressed.
 */
int8_t sl_si91x_button_state_get(uint8_t pin);

/** @brief Returns the current state (pressed or released) of the
 * pin associated with a button.
 *
 * This reads the actual state of the pin and can be used on startup to
 * determine the initial position of the buttons.
 *
 * @param pin  The button pin being queried, either BUTTON0 pin or BUTTON1 pin as
 * defined.
 *
 * @return  ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
 * if the button is not pressed.
 */
int8_t sl_si91x_button_pin_state(uint8_t pin);

/** @brief A callback called in interrupt context whenever a button
 * changes its state.
 *
 * @appusage Must be implemented by the application.  This function should
 * contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to the appropriate functionality.
 *
 * @param pin  The button pin which has changed state, either BUTTON0 pin or BUTTON1 pin
 * as defined.
 *
 * @param state   The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED if
 * the button has been released.
 */
void sl_si91x_button_pin_isr(uint8_t pin, uint8_t state);

void sl_si91x_button_state_toggle(uint8_t pin);
void sl_si91x_button_state_set(uint8_t pin, int8_t state);
void sl_si91x_button_isr(uint8_t pin, int8_t state);

/** @} END addtogroup
 */

#ifdef __cplusplus
}
#endif

#endif // SL_SI91X_BUTTON_H