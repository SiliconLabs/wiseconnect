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
#include <stdbool.h>
#include <stdint.h>

/** @addtogroup LED LED
 *  @ingroup SI91X_HARDWARE_DRIVER_APIS
 *  @brief  Sample API funtions for controlling LEDs.
 *
 * See sl_si91x_led.c for source code.
 *@{
 */
typedef struct {
  uint8_t pin;
  uint8_t port;
  uint8_t led_number;
} sl_led_t;

/***************************************************************************/ /**
 * Configures GPIOs pertaining to the control of LEDs.  Initializes the selected LED GPIO and mode.
 *
 * @param[in] handle  The pointer to led structure that has the specific 
 * LED information. 
 *
 * @return none
 *
 ******************************************************************************/
void sl_si91x_led_init(const sl_led_t *handle);

/***************************************************************************/ /**
 * Atomically wraps an XOR or a similar operation for a 
 * single GPIO pin connected to an LED to toggle the LED.
 *
 * @param[in] pin  LED pin for the LED to be toggled.
 *
 * @return none
 *
 ******************************************************************************/
void sl_si91x_led_toggle(uint8_t pin);

/***************************************************************************/ /**
 * Sets a GPIO pin connected to an LED to turn the LED on.
 *
 * @param[in] pin  LED pin for the LED to turn on.
 *
 * @return none
 *
 ******************************************************************************/
void sl_si91x_led_set(uint8_t pin);

/***************************************************************************/ /**
 * Clears a GPIO pin connected to an LED to turn the LED off.
 *
 * @param[in] pin  LED pin for the LED to turn off.
 *
 * @return none
 *
 ******************************************************************************/
void sl_si91x_led_clear(uint8_t pin);

/***************************************************************************/ /**
 * Called by the stack to indicate activity over the radio (for 
 * both transmission and reception). It is called once with \c turnOn true and 
 * shortly thereafter with \c turnOn false.
 * 
 * Typically does something interesting, such as change the state of
 * an LED.
 * 
 * @param[in] turnOn  See Usage.
 *
 * @return none
 *
 ******************************************************************************/
void sl_si91x_led_StackIndicateActivity(bool turnOn);

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup LED LED
/// @{
///
///   @details
///
///   @n @section led_intro Introduction
///
///   The SI91x LED driver is a platform-level software module responsible for managing the control of LEDs.
///   All LED functions are called through the generic driver, which then which internally calls functions specific to the LED.
///
///   @n @section led_config Configuration
///
///   All LED instances are configured with an @ref sl_led_t struct. This struct is automatically generated after a LED is set up
///   using Simplicity Studio's wizard.
///
///   @li For more information on configuring available parameters refer to the respective peripheral example readme document and
///   refer here https://docs.silabs.com/wiseconnect/latest/wiseconnect-api-reference-guide-si91x-peripherals/disable-uc-config.
///
///
///   @n @section led_usage Usage
///
///   Once the LED structs are defined, the LED functions can be called being
///   passed an instance of sl_led_t, which will be redirected to calling the type specific
///   version of that function. The common functions include the following:
///
///   @li @ref sl_si91x_led_init
///   @li @ref sl_si91x_led_toggle
///   @li @ref sl_si91x_led_set
///   @li @ref sl_si91x_led_clear
///
///   @ref sl_led_init initialises the specific LED. This function is called automatically when the application is run.
///
///   @ref sl_si91x_led_toggle can be implemented by the application if required. This function is used
///   to toggle the state of the LED. @ref sl_si91x_led_set and @ref sl_si91x_led_clear both can be implemented by the application.
///   These functions are used to turn the LED on and off resectively.
///
/// @} end group LED ********************************************************/

/** @} END addtogroup
 */

#endif // __SL_SI91X_LED_H__
