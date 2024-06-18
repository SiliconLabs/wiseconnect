/***************************************************************************/ /**
 * @file sl_si91x_rgb_led.c
 * @brief RGB LED manipulation routines; stack and example APIs
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
#include "sl_si91x_rgb_led.h"
#include "si91x_device.h"
#include "sl_driver_gpio.h"
#include "sl_si91x_driver_gpio.h"

sl_status_t status;

void sl_si91x_rgb_led_init(const sl_led_t *handle)
{
  /*Enable clock*/
  sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO);

  /*Set the GPIO pin MUX */
  sl_gpio_t rgb_gpio_port_pin = { handle->port, handle->pin };
  status                      = sl_gpio_driver_set_pin_mode(&rgb_gpio_port_pin, 0, 1);

  /*PAD selection*/
  sl_si91x_gpio_driver_enable_pad_selection(handle->pad);

  /*Set pin Direction*/
  sl_si91x_gpio_driver_set_pin_direction(handle->port, handle->pin, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);

  /*REN enable */
  sl_si91x_gpio_driver_enable_pad_receiver(handle->pin);
}

void sl_si91x_rgb_led_on(const sl_led_t *handle)
{
  sl_gpio_t rgb_gpio_port_pin = { handle->port, handle->pin };
  sl_gpio_driver_clear_pin(&rgb_gpio_port_pin);
}

void sl_si91x_rgb_led_off(const sl_led_t *handle)
{
  sl_gpio_t rgb_gpio_port_pin = { handle->port, handle->pin };
  sl_gpio_driver_set_pin(&rgb_gpio_port_pin);
}

uint8_t sl_si91x_rgb_led_get_current_state(const sl_led_t *handle)
{
  sl_gpio_t rgb_gpio_port_pin = { handle->port, handle->pin };
  return sl_gpio_driver_get_pin_output(&rgb_gpio_port_pin);
}

void sl_si91x_rgb_led_toggle(const sl_led_t *handle)
{
  sl_gpio_t rgb_gpio_port_pin = { handle->port, handle->pin };
  sl_gpio_driver_toggle_pin(&rgb_gpio_port_pin);
}
