/***************************************************************************/ /**
 * @file sl_driver_gpio.h
 * @brief sl driver gpio header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licenser of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be miss represented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    miss represented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_DRIVER_GPIO_H
#define SL_DRIVER_GPIO_H

#if !defined(GPIO_PRESENT)
#include "sl_status.h"
#include "sl_si91x_peripheral_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/ /**
 * @addtogroup GPIO General-Purpose Input-Output
 * @ingroup SI91X_PERIPHERAL_APIS
 * @{
 *
 ******************************************************************************/
/*******************************************************************************
 ***************************  DEFINES / MACROS ********************************
 ******************************************************************************/
#define GPIO_MAX_OUTPUT_VALUE 1  // GPIO output maximum value
#define MAX_GROUP_INT         2  // Maximum number of group interrupts
#define GPIO_PORT_MAX_VALUE   5  // Maximum number of GPIO ports
#define MAX_UULP_INT          5  // Maximum number of UULP interrupts
#define ULP_MAX_MODE          10 // Maximum ULP mode

#define GPIO_MAX_INTR_VALUE  8    // Maximum number of M4 GPIO pin interrupts
#define PORTD_PIN_MAX_VALUE  8    // Port D maximum(0-8) number of GPIO pins
#define PORTE_PIN_MAX_VALUE  11   // Port E maximum(0-11) number of GPIO pins
#define PORTF_PIN_MAX_VALUE  5    // Port F maximum(0-4) number of GPIO pins
#define MAX_ULP_INTR         8    // Maximum number of ULP interrupts
#define MAX_MODE             15   // Maximum M4 GPIO mode
#define PORT_PIN_MAX_VALUE   15   // GPIO pin maximum(0-15) value for ports
#define GPIO_FLAGS_MAX_VALUE 0x0F // GPIO flags maximum value

#define PORTF 5 // Initializing port F value

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///@brief structure to hold parameters of GPIO port and pin numbers.
typedef struct {
  sl_gpio_port_t port;
  uint8_t pin;
} sl_gpio_t;

/*******************************************************************************
 ********************************   Local Variables   **************************
 ******************************************************************************/
///@brief GPIO interrupt callback function pointer.
typedef void (*sl_gpio_irq_callback_t)(uint32_t flag);

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/*******************************************************************************/ /**
 * @brief  Clear one or more pending GPIO interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  flags  -  Bitwise logic OR of GPIO interrupt sources to clear.
 * @return returns status 0
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_clear_interrupts(uint32_t flags)
{
  sl_gpio_clear_interrupts(flags);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief     Configure the GPIO pin interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @param[in] int_no - Specifies the interrupt number to trigger (0 to 7).
 * @param[in] flags  - Interrupt configuration flags \ref sl_gpio_interrupt_flag_t
 * @param[in] gpio_callback  - IRQ callback function pointer \ref sl_gpio_irq_callback_t
 * @param[out] avl_intr_no - Pointer to the available interrupt number.
          SL_GPIO_INTERRUPT_UNAVAILABLE (0xFF) no available interrupt
 * @return The following values are returned:
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 * -   \ref SL_STATUS_BUSY (0x0004) - Interrupt is busy and cannot carry out the requested operation
 ******************************************************************************/
sl_status_t sl_gpio_driver_configure_interrupt(sl_gpio_t *gpio,
                                               uint32_t int_no,
                                               sl_gpio_interrupt_flag_t flags,
                                               sl_gpio_irq_callback_t gpio_callback,
                                               uint32_t *avl_intr_no);

/***************************************************************************/ /**
 * @brief      Set the pin mode for a GPIO pin.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for corresponding GPIO instance.
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t.
 * @param[in]  mode - The desired pin mode.
 * @param[in]  output_value - A value to set for the pin in the GPIO register.
 *                The GPIO setting is important for some input mode configurations.
 * @return The following values are returned:
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 ******************************************************************************/
sl_status_t sl_gpio_driver_set_pin_mode(sl_gpio_t *gpio, sl_gpio_mode_t mode, uint32_t output_value);

/***************************************************************************/ /**
 * @brief      Get the pin mode (alternate function) of a GPIO for either HP instance (or) ULP instance as per the port number.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 *     \ref sl_gpio_driver_set_pin_mode();
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @param[out]  mode - The desired pin mode.
 * @return The following values are returned:
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_gpio_driver_get_pin_mode(sl_gpio_t *gpio, sl_gpio_mode_t *mode);

/*******************************************************************************
 * @brief Initializes the GPIO driver by clearing all the interrupts (HP, ULP, and NPSS instance) and callback function pointers with NULL.
 * @param none
 * @return returns status 0,
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_gpio_driver_init(void);

/*******************************************************************************
 * @brief De-Initializes the GPIO driver by disabling clocks (HP and ULP instance) and callback function pointers.
 * @param none
 * @return returns status 0,
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_gpio_driver_deinit(void);

/*******************************************************************************/ /**
 * @brief Unregister GPIO driver.
 * @param[in] gpio_instance - Instances of type \ref sl_si91x_gpio_instances_t
 * @param[in] gpio_intr - GPIO interrupts of type \ref sl_si91x_gpio_intr_t
 * @param[in] flag - GPIO interrupt flag Max range is 8.
 * @return The following values are returned:
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_gpio_driver_unregister(sl_si91x_gpio_instances_t gpio_instance,
                                      sl_si91x_gpio_intr_t gpio_intr,
                                      uint8_t flag);

/***************************************************************************/ /**
 * @brief Set a single pin in GPIO configuration register to 1. 
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode(); 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction(); 
 *
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @return The following values are returned:
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_set_pin(sl_gpio_t *gpio)
{
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == SL_GPIO_PORT_A) || (gpio->port == SL_GPIO_PORT_B) || (gpio->port == SL_GPIO_PORT_C)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_GPIO_PORT_D) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_ULP_GPIO_PORT) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_gpio_set_pin_output(gpio->port, gpio->pin);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Clear a single pin in the GPIO configuration register to 0.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(); for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode(); 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction(); 
 *
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_clear_pin(sl_gpio_t *gpio)
{
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == SL_GPIO_PORT_A) || (gpio->port == SL_GPIO_PORT_B) || (gpio->port == SL_GPIO_PORT_C)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_GPIO_PORT_D) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_ULP_GPIO_PORT) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_gpio_clear_pin_output(gpio->port, gpio->pin);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Toggle a single pin in GPIO port register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_toggle_pin(sl_gpio_t *gpio)
{
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == SL_GPIO_PORT_A) || (gpio->port == SL_GPIO_PORT_B) || (gpio->port == SL_GPIO_PORT_C)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_GPIO_PORT_D) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_ULP_GPIO_PORT) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_gpio_toggle_pin_output(gpio->port, gpio->pin);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Read the pin value for a single pin in a GPIO port.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @param[out] pin_value - Gets the gpio pin value
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_get_pin(sl_gpio_t *gpio, uint8_t *pin_value)
{
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == SL_GPIO_PORT_A) || (gpio->port == SL_GPIO_PORT_B) || (gpio->port == SL_GPIO_PORT_C)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_GPIO_PORT_D) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_ULP_GPIO_PORT) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  *pin_value = sl_gpio_get_pin_input(gpio->port, gpio->pin);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Set bits GPIO data out register to 1.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode(); 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction(); 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in] pins - The GPIO pins in a Port that are set to 1 (1 to 65535).
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_set_port(sl_gpio_port_t port, uint32_t pins)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_set_port_output(port, pins);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Set bits in configuration register for a port to 0.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pins - The GPIO pins in a Port that are set to 0 (1 to 65535).
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_clear_port(sl_gpio_port_t port, uint32_t pins)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_clear_port_output(port, pins);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Get the current setting for a GPIO configuration register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[out] port_value - Gets the gpio port value
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_get_port_output(sl_gpio_port_t port, uint32_t *port_value)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  *port_value = sl_gpio_get_port_output(port);
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 * @brief  Get the current setting for a pin in a GPIO configuration register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in] gpio - Pointer to the structure of type \ref sl_gpio_t
 * @return The GPIO pin value
 *             '0' - Low\n
 *             '1' - High\n
 ******************************************************************************/
STATIC __INLINE uint8_t sl_gpio_driver_get_pin_output(sl_gpio_t *gpio)
{
  uint8_t pin_output = 0;
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == SL_GPIO_PORT_A) || (gpio->port == SL_GPIO_PORT_B) || (gpio->port == SL_GPIO_PORT_C)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_GPIO_PORT_D) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == SL_ULP_GPIO_PORT) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  pin_output = sl_gpio_get_pin_output(gpio->port, gpio->pin);
  return pin_output;
}

/*******************************************************************************/ /**
 * @brief  Set the GPIO port configuration register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance 
 * -
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode(); 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction(); 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  val -  Value to write to the port configuration  register.
 * @param[in]  mask - Mask indicating which bits to modify.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_set_port_output_value(sl_gpio_port_t port, uint32_t val, uint32_t mask)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_set_port_output_value(port, val, mask);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  Set slewrate for pins on an HP instance GPIO port.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 *
 * @param[in]  port - The GPIO port to configure.
 * @param[in]  slewrate - The slewrate to configure for the pins on this GPIO port.
 * @param[in]  slewrate_alt - The slewrate to configure for the pins using alternate modes on this GPIO port.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_set_slew_rate(sl_gpio_port_t port, uint32_t slewrate, uint32_t slewrate_alt)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_set_slew_rate(port, slewrate, slewrate_alt);
  return SL_STATUS_OK;
}

/*******************************************************************************/ /**
 * @brief  Read the port value for GPIO.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance 
 * -
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @return The GPIO pin value
 *             '0' - Low\n
 *             '1' - High\n
 ******************************************************************************/
STATIC __INLINE uint32_t sl_gpio_driver_get_port_input(sl_gpio_port_t port)
{
  uint32_t port_input = 0;
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  port_input = sl_gpio_get_port_input(port);
  return port_input;
}

/*******************************************************************************
 * @brief  Toggle the selected pin values in the GPIO port register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance 
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pins - Port pins to toggle.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -   \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_toggle_port_output(sl_gpio_port_t port, uint32_t pins)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_toggle_port_output(port, pins);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  Enable the selected GPIO pin interrupt with the configured interrupt type (Level or Edge).
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  flags  - GPIO interrupt sources to enable.
 * @return returns status 0
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_enable_interrupts(uint32_t flags)
{
  sl_gpio_enable_interrupts(flags);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  Disable the selected GPIO pin interrupt with the configured interrupt type (Level or Edge).
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  flags  -  GPIO interrupt sources to disable.
 * @return returns status 0
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_disable_interrupts(uint32_t flags)
{
  sl_gpio_disable_interrupts(flags);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  Set the selected GPIO pin interrupt with the configured interrupt type (Level or Edge).
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param[in]  flags  -  GPIO interrupt sources to set to pending.
 * @return returns status 0
 * -   \ref SL_STATUS_OK (0X000)  - Success 
 *
 ******************************************************************************/
STATIC __INLINE sl_status_t sl_gpio_driver_set_interrupts(uint32_t flags)
{
  sl_gpio_set_interrupts(flags);
  return SL_STATUS_OK;
}

/*******************************************************************************/ /**
 * @brief  Get the pending GPIO interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param  none
 * @return Pending GPIO Pin interrupt numbers.
 ******************************************************************************/
STATIC __INLINE uint32_t sl_gpio_driver_get_pending_interrupts(void)
{
  uint32_t status;
  status = sl_gpio_get_pending_interrupts();
  return status;
}

/*******************************************************************************/ /**
 * @brief  Get the enabled GPIO interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 *
 * @param  none
 * @return Enabled GPIO pin interrupt numbers.
 ******************************************************************************/
STATIC __INLINE uint32_t sl_gpio_driver_get_enabled_interrupts(void)
{
  uint32_t status;
  status = sl_gpio_get_enabled_interrupts();
  return status;
}

/*******************************************************************************/ /**
 * @brief  Get the enabled and pending GPIO interrupt flags.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * @param  none
 * @return Enabled GPIO pending interrupts.
 ******************************************************************************/
STATIC __INLINE uint32_t sl_gpio_driver_get_enabled_pending_interrupts(void)
{
  uint32_t status;
  status = sl_gpio_get_enabled_pending_interrupts();
  return status;
}

/** @} (end addtogroup GPIO) */

#ifdef __cplusplus
}
#endif

#endif ///< GPIO_PRESENT
#endif ///< SL_DRIVER_GPIO_H
