/***************************************************************************/ /**
 * @file sl_si91x_led.h
 * @brief See @ref led for documentation.
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
#ifndef __SL_SI91X_LED_H__
#define __SL_SI91X_LED_H__

#include "sl_si91x_led_config.h"
#include "rsi_board.h"
#include <stdbool.h>

typedef struct {
  uint8_t pin;
  uint8_t port;
  uint8_t led_number;
} sl_led_t;

/** @addtogroup led
 *  @brief  Sample API funtions for controlling LEDs.
 *
 * When specifying an LED to use, always use the BOARDLEDx definitions that
 * are defined within the BOARD_HEADER.
 *
 * See led.c for source code.
 *@{
 */

/** @brief Configures GPIOs pertaining to the control of LEDs.
 */
void sl_si91x_led_init(const sl_led_t *handle);

/** @brief Atomically wraps an XOR or similar operation for a single GPIO
 *  pin attached to an LED.
 *
 *  @param pin  LED pin for the LED to be toggled.
 */
void sl_si91x_led_toggle(uint8_t pin);

/** @brief Turns on (sets) a GPIO pin connected to an LED so that the LED
 *  turns on.
 *
 *  @param pin  LED pin for the LED to turn on.
 */
void sl_si91x_led_set(uint8_t pin);

/** @brief Turns off (clears) a GPIO pin connected to an LED, which turns
 *  off the LED.
 *
 *  @param pin  LED pin for the LED to turn off.
 */
void sl_si91x_led_clear(uint8_t pin);

/** @brief Called by the stack to indicate activity over the radio (for
 *  both transmission and reception). It is called once with \c turnOn true and
 *  shortly thereafter with \c turnOn false.
 *
 *  Typically does something interesting, such as change the state of
 *  an LED.
 *
 *  @param turnOn  See Usage.
 */
void sl_si91x_led_StackIndicateActivity(bool turnOn);

/** @} END addtogroup
 */

#endif // __SL_SI91X_LED_H__
