/***************************************************************************/ /**
 * @file sl_si91x_rgb_led.h
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
#ifndef __SL_SI91X_RGB_LED_H__
#define __SL_SI91X_RGB_LED_H__

#include "sl_si91x_rgb_led_config.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t pin;
  uint8_t port;
  uint8_t led_number;
  uint8_t pad;
} sl_led_t;

#define LED_ON  0
#define LED_OFF 1

#define RGB_LED_PORT 0

/** @addtogroup RGB LED
 *  @ingroup SI91X_HARDWARE_DRIVER_APIS
 *  @brief  Sample API funtions for controlling RGB LEDs.
 *
 * See sl_si91x_led.c for source code.
 *@{
 */

/** @brief Configures GPIOs pertaining to the control of RGB LEDs.
 *
 * @param[in] handle  The pointer to RGB LED structure that has the specific led information.
 */
void sl_si91x_rgb_led_init(const sl_led_t *handle);

/** @brief Turns on (sets) a GPIO pin connected to an LED so that the RGB LED
 *  turns on.
 *
 *  @param[in] handle  The pointer to RGB LED structure that has the specific led information.
 */
void sl_si91x_rgb_led_on(const sl_led_t *handle);

/** @brief Turns off (clears) a GPIO pin connected to an RGB LED, which turns
 *  off the RGB LED.
 *
 *  @param[in] handle  The pointer to RGB LED structure that has the specific RGB led information.
 */
void sl_si91x_rgb_led_off(const sl_led_t *handle);

/** @brief Gets the current status of the RGB LED i.e. On or OFF
 *
 *  @param[in] handle  The pointer to RGB LED structure that has the specific RGB led information.
 */
uint8_t sl_si91x_rgb_led_get_current_state(const sl_led_t *handle);

/** @brief Atomically wraps an XOR or similar operation for a single GPIO
 *  pin attached to an RGB LED.
 *
 *  @param[in] handle  The pointer to RGB LED structure that has the specific led information.
 */
void sl_si91x_rgb_led_toggle(const sl_led_t *handle);

#endif // __SL_SI91X_RGB_LED_H__
