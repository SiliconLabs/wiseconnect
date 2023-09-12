/*******************************************************************************
 * @file sl_si91x_driver_gpio.c
 * @brief General Purpose IO (GPIO) driver API.
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
#include "sl_si91x_driver_gpio.h"
#include <stdio.h>
/*******************************************************************************
 ***************************  DEFINES / MACROS ********************************
 ******************************************************************************/
#define GPIO_RELEASE_VERSION                    0  // GPIO Release version
#define GPIO_MAJOR_VERSION                      0  // GPIO SQA version
#define GPIO_MINOR_VERSION                      2  // GPIO Developer version
#define GPIO_DIRECTION_MAX_VALUE                1  // GPIO maximum direction set value
#define GPIO_CLOCK_MAX_VAL                      1  // Validating clock for GPIO instance
#define GPIO_GROUP_INTERRUPT_MAX_VALUE          1  // Maximum number of GPIO group interrupts
#define GPIO_LEVEL_EDGE_MAX_VALUE               1  // GPIO maximum level edge value
#define GPIO_POLARITY_MAX_VALUE                 1  // GPIO maximum polarity value
#define GPIO_AND_OR_MAX_VALUE                   1  // GPIO maximum AND_OR value
#define GPIO_SLEW_RATE_MAX_VALUE                1  // GPIO maximum SLEW RATE value
#define GPIO_RECEIVER_MAX_VALUE                 1  // GPIO maximum receiver value
#define GPIO_PIN_VALUE_MAX_VALUE                1  // GPIO maximum pin value
#define GPIO_STRENGTH_MAX_VAL                   3  // GPIO maximum strength value
#define GPIO_DISABLE_STATE_MAX_VAL              3  // GPIO maximum disable state value
#define GPIO_UULP_MAX_PIN_NUM                   5  // GPIO UULP maximum pin number
#define GPIO_MODE_MAX_VALUE                     7  // GPIO maximum MODE value
#define GPIO_INTERRUPT_MAX_VALUE                8  // GPIO interrupt maximum value
#define GPIO_ULP_MAX_PIN_NUM                    11 // GPIO ULP maximum pin number
#define GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE 16 // NPSSGPIO maximum value
#define GPIO_MAX_PAD_NUM                        34 // GPIO maximum pad number
#define GPIO_MAX_PIN_NUM                        57 // Maximum number of GPIO pins in m4 instance

#define UULP_MASK   0x00 // UULP GPIO pin mask
#define UULP_INTR_1 0x02 // UULP GPIO pin interrupt 1
#define UULP_INTR_2 0x04 // UULP GPIO pin interrupt 2
#define UULP_INTR_3 0x08 // UULP GPIO pin interrupt 3
#define UULP_INTR_4 0x10 // UULP GPIO pin interrupt 4
#define UULP_INTR_5 0x20 // UULP GPIO pin interrupt 5

/*******************************************************************************
 ***********************  Global function Prototypes ***************************
 ******************************************************************************/
void PIN_IRQ0_Handler(void);
void PIN_IRQ1_Handler(void);
void PIN_IRQ2_Handler(void);
void PIN_IRQ3_Handler(void);
void PIN_IRQ4_Handler(void);
void PIN_IRQ5_Handler(void);
void PIN_IRQ6_Handler(void);
void PIN_IRQ7_Handler(void);
void GRP_IRQ0_Handler(void);
void GRP_IRQ1_Handler(void);
void UULP_PIN_IRQ_Handler(void);
void ULP_PIN_IRQ_Handler(void);
void ULP_GROUP_IRQ_Handler(void);

/*******************************************************************************
 ************************       GLOBAL FUNCTIONS      **************************
 ******************************************************************************/
sl_gpio_irq_callback_t gpio_callback_function_pointer = NULL;

/*******************************************************************************
 * @brief This API is used to configure the pin interrupt in GPIO HP instance.
 * There are total 8 pin interrupts in this instance.
 * To configure the interrupt, first GPIO initialization must be done.
 * The actions to be performed in GPIO initialization are:
 *  - Enable the M4 clock of GPIO HP instance.
 *  - Select PAD selection of the GPIO HP instance.
 *  - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *    output/input.
 *  - Set pin mode and direction of the GPIO pin.
 ******************************************************************************/
sl_status_t sl_gpio_driver_configure_interrupt(sl_gpio_t *gpio,
                                               uint32_t int_no,
                                               sl_gpio_interrupt_flag_t flags,
                                               sl_gpio_irq_callback_t gpio_callback,
                                               uint32_t *avl_intr_no)
{
  (void)avl_intr_no;
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback == NULL) {
    // Returns null pointer status code if gpio_callback == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == PORTA) || (gpio->port == PORTB) || (gpio->port == PORTC)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTD) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTE) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (flags > GPIO_FLAGS_MAX_VALUE) {
    // Returns invalid parameter status code if flags > GPIO_FLAGS_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (int_no > GPIO_MAX_INTR_VALUE) {
    // Returns invalid parameter status code if int_no > GPIO_MAX_INTR_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (int_no == 0) {
    NVIC_EnableIRQ(PININT0_NVIC_NAME);
    NVIC_SetPriority(PININT0_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 1) {
    NVIC_EnableIRQ(PININT1_NVIC_NAME);
    NVIC_SetPriority(PININT1_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 2) {
    NVIC_EnableIRQ(PININT2_NVIC_NAME);
    NVIC_SetPriority(PININT2_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 3) {
    NVIC_EnableIRQ(PININT3_NVIC_NAME);
    NVIC_SetPriority(PININT3_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 4) {
    NVIC_EnableIRQ(PININT4_NVIC_NAME);
    NVIC_SetPriority(PININT4_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 5) {
    NVIC_EnableIRQ(PININT5_NVIC_NAME);
    NVIC_SetPriority(PININT5_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 6) {
    NVIC_EnableIRQ(PININT6_NVIC_NAME);
    NVIC_SetPriority(PININT6_NVIC_NAME, PRIORITY_7);
  }
  if (int_no == 7) {
    NVIC_EnableIRQ(PININT7_NVIC_NAME);
    NVIC_SetPriority(PININT7_NVIC_NAME, PRIORITY_7);
  }
  gpio_callback_function_pointer = gpio_callback;
  sl_gpio_configure_interrupt(gpio->port, gpio->pin, int_no, flags);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  This API is used for GPIO HP, ULP instances to set pin mode.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To set the pin mode in GPIO HP instance, GPIO initialization needs to
 *     be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *        selected as output/input.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other
 *      functionality.
 * - If GPIO ULP instance is considered, the following actions are performed:
 *   - To set the pin mode in GPIO ULP instance, GPIO initialization needs to
 *     be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *        selected as output/input.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
sl_status_t sl_gpio_driver_set_pin_mode(sl_gpio_t *gpio, sl_gpio_mode_t mode, uint32_t output_value)
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
  if ((gpio->port == PORTA) || (gpio->port == PORTB) || (gpio->port == PORTC)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTD) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTE) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
    if (mode > ULP_MAX_MODE) {
      // Returns invalid parameter status code if mode > ULP_MAX_MODE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (mode > MAX_MODE) {
    // Returns invalid parameter status code if mode > MAX_MODE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (output_value > GPIO_MAX_OUTPUT_VALUE) {
    // Returns invalid parameter status code if output_value > GPIO_MAX_OUTPUT_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_gpio_set_pin_mode(gpio->port, gpio->pin, mode, output_value);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  This API is used for GPIO HP, ULP instances to get pin mode.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To get the pin status in GPIO HP instance, GPIO initialization needs
 *     to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *        selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin mode of GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *         use GPIO pin number(0 to 5) in HP instance as these are used for other
 *         functionality.
 * - If GPIO ULP instance is considered, the following actions are
 *   performed:
 *   - To get the pin mode in GPIO ULP instance, GPIO initialization needs
 *     to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *        selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin mode of GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to
 *     ULP_GPIO_11).
 ******************************************************************************/
sl_status_t sl_gpio_driver_get_pin_mode(sl_gpio_t *gpio, sl_gpio_mode_t *mode)
{
  // Check for valid parameters
  if (gpio == NULL) {
    // Returns null pointer status code if gpio == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (mode == NULL) {
    // Returns null pointer status code if mode == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio->port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if gpio->port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((gpio->port == PORTA) || (gpio->port == PORTB) || (gpio->port == PORTC)) {
    if (gpio->pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTD) {
    if (gpio->pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (gpio->port == PORTE) {
    if (gpio->pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if gpio->pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  *mode = sl_gpio_get_pin_mode(gpio->port, gpio->pin);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Initialization of GPIO driver.
 ******************************************************************************/
sl_status_t sl_gpio_driver_init(void)
{
  uint32_t flag;
  for (flag = 0; flag < GPIO_MAX_INTR_VALUE; flag++) {
    sl_gpio_driver_clear_interrupts(flag);
  }
  for (flag = 0; flag < MAX_GROUP_INT; flag++) {
    sl_si91x_gpio_driver_clear_group_interrupt(flag);
  }
  for (flag = 0; flag < MAX_UULP_INT; flag++) {
    sl_si91x_gpio_driver_clear_uulp_interrupt((uint8_t)flag);
  }
  for (flag = 0; flag < MAX_ULP_INTR; flag++) {
    sl_si91x_gpio_driver_clear_ulp_interrupt((uint8_t)flag);
  }
  for (flag = 0; flag < MAX_GROUP_INT; flag++) {
    sl_si91x_gpio_driver_clear_ulp_group_interrupt(flag);
  }
  return SL_STATUS_OK;
}

/*******************************************************************************
* Get the release, SQA, and development version numbers of the GPIO peripheral.
*******************************************************************************/
sl_si91x_gpio_version_t sl_si91x_gpio_driver_get_version(void)
{
  sl_si91x_gpio_version_t version;
  version = sl_si91x_gpio_get_version();
  return version;
}

/*******************************************************************************
 * Indicate UULP GPIO PAD configuration.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_pad_configuration(uulp_pad_config_t *pad_config)
{
  if (pad_config == NULL) {
    // Returns null pointer status code if pad_config == NULL
    return SL_STATUS_NULL_POINTER;
  }
  sl_si91x_gpio_set_uulp_pad_configuration(pad_config);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to set pin direction.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To set the pin direction in GPIO HP instance, GPIO initialization
 *		needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *			selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 * - If GPIO ULP instance is considered, the following actions are
 *		performed:
 *   - To set the pin direction in GPIO ULP instance, GPIO initialization
 *		needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *			selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_pin_direction(uint8_t port, uint8_t pin, sl_si91x_gpio_direction_t direction)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (direction > GPIO_DIRECTION_MAX_VALUE) {
    // Returns invalid parameter status code if direction > GPIO_DIRECTION_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_pin_direction(port, pin, direction);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to get pin direction.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To get the pin direction in GPIO HP instance, GPIO initialization
 *			needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *			selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin direction of the GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *		use GPIO pin number(0 to 5) in HP instance as these are used for other
 *		functionality.
 * - If GPIO ULP instance is considered, the following actions are
 *		performed:
 *   - To get the pin direction in GPIO ULP instance, GPIO initialization
 *		needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *			selected as output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin direction of the GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_pin_direction(uint8_t port, uint8_t pin)
{
  uint8_t direction;
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  direction = sl_si91x_gpio_get_pin_direction(port, pin);
  return direction;
}

/*******************************************************************************
 * This API is used to enable PAD receiver in GPIO HP instance.
 * The actions to be performed for enabling PAD are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_receiver(uint8_t gpio_num)
{
  // Check for valid parameters
  if (gpio_num > GPIO_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_enable_pad_receiver(gpio_num);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to disable PAD receiver in GPIO HP instance.
 * The actions to be performed for disabling PAD are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Disable PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_pad_receiver(uint8_t gpio_num)
{
  // Check for valid parameters
  if (gpio_num > GPIO_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_disable_pad_receiver(gpio_num);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to enable PAD selection in GPIO HP instance.
 * The actions to be performed for enable PAD selection are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *  @note: PAD number(25 to 30) are used for HOST PAD selection.
 *      Do not use PAD number-9 as it is used for other functionality.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_selection(uint8_t gpio_padnum)
{
  // Check for valid parameters
  if (gpio_padnum > GPIO_MAX_PAD_NUM) {
    // Returns invalid parameter status code if gpio_padnum > GPIO_MAX_PAD_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_enable_pad_selection(gpio_padnum);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To select the PAD driver strength in GPIO HP instance, GPIO
 *initialization needs to be done first.
 * - The actions to be performed in GPIO initialization are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver strength of type @ref
 *		sl_si91x_gpio_driver_strength_select_t.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_strength(uint8_t gpio_num,
                                                            sl_si91x_gpio_driver_strength_select_t strength)
{
  // Check for valid parameters
  if (gpio_num > GPIO_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (strength > GPIO_STRENGTH_MAX_VAL) {
    // Returns invalid parameter status code if strength > GPIO_STRENGTH_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_pad_driver_strength(gpio_num, strength);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To select the PAD driver disable state in GPIO HP instance, GPIO
 *initialization needs to be done first.
 * - The actions to be performed in GPIO initialization are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver disable state of type @ref
 *		sl_si91x_gpio_driver_disable_state_t.
 *  @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 *	use GPIO pin number(0 to 5) in HP instance as these are used for other
 *	functionality.
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_disable_state(uint8_t gpio_num,
                                                                 sl_si91x_gpio_driver_disable_state_t disable_state)
{
  // Check for valid parameters
  if (gpio_num > GPIO_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (disable_state > GPIO_DISABLE_STATE_MAX_VAL) {
    // Returns invalid parameter status code if disable_state > GPIO_DISABLE_STATE_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_pad_driver_disable_state(gpio_num, disable_state);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * The GPIO pins to work in different instances, requires this clock.
 * For GPIO HP instance, enable M4 clock of type @ref
 * sl_si91x_gpio_select_clock_t. For GPIO ULP/UULP instances, enable ULP
 * clock of type
 *  @ref sl_si91x_gpio_select_clock_t
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_clock(sl_si91x_gpio_select_clock_t clock)
{
  // Check for valid parameters
  if (clock > GPIO_CLOCK_MAX_VAL) {
    // Returns invalid parameter status code if clock > GPIO_CLOCK_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_enable_clock(clock);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API disables the M4/ ULP clock of GPIO instances.
 * For GPIO HP instance, disable M4 clock of type @ref
 * sl_si91x_gpio_select_clock_t. For GPIO ULP/UULP instances, disable ULP
 * clock of type
 *  @ref sl_si91x_gpio_select_clock_t
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_clock(sl_si91x_gpio_select_clock_t clock)
{
  // Check for valid parameters
  if (clock > GPIO_CLOCK_MAX_VAL) {
    // Returns invalid parameter status code if clock > GPIO_CLOCK_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_disable_clock(clock);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to enable the group interrupts.
 * @note: We are calling this API, inside the group interrupt
 * configuration API's
 *      @ref sl_si91x_gpio_driver_configure_group_interrupt(), used for HP
 *		instance,
 *      @ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), used for ULP
 *		instance.
 * @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 * use GPIO pin number(0 to 5) in HP instance as these are used for other
 * functionality.
 * @note: Select ULP GPIO pins for ULP instances(0 to 11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                        uint8_t port,
                                                        uint8_t pin)
{
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_si91x_gpio_enable_group_interrupt(group_interrupt, port, pin);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to disable the group interrupts.
 * @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 * use GPIO pin number(0 to 5) in HP instance as these are used for other
 * functionality.
 * @note: Select ULP GPIO pins for ULP instances(0 to 11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                         uint8_t port,
                                                         uint8_t pin)
{
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_si91x_gpio_disable_group_interrupt(group_interrupt, port, pin);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to mask the group interrupts.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_mask_group_interrupt(port, group_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to unmask the group interrupts.
 * @note: We are calling this API, inside the group interrupt
 * configuration API's
 *      @ref sl_si91x_gpio_driver_configure_group_interrupt(), used for HP
 *		instance,
 *      @ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), used for ULP
 *  	instance.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port >
    // GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_unmask_group_interrupt(port, group_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to set level/edge event of group interrupt.
 * @note: We are calling this API, inside the group interrupt
 * configuration API's
 *      @ref sl_si91x_gpio_driver_configure_group_interrupt(), used for HP
 *		instance,
 *      @ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), used for ULP
 *		instance.
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_level_edge(uint8_t port,
                                                                sl_si91x_group_interrupt_t group_interrupt,
                                                                sl_si91x_gpio_level_edge_t level_edge)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port >
    // GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (level_edge > GPIO_LEVEL_EDGE_MAX_VALUE) {
    // Returns invalid parameter status code if level_edge >
    // GPIO_LEVEL_EDGE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_group_interrupt_level_edge(port, group_interrupt, level_edge);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to get level/edge event of group interrupt.
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_level_edge(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  uint8_t level_edge;
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port >
    // GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  level_edge = sl_si91x_gpio_get_group_interrupt_level_edge(port, group_interrupt);
  return level_edge;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to set polarity of group interrupt.
 * @note: We are calling this API, inside the group interrupt
 * configuration API's
 *      @ref sl_si91x_gpio_driver_configure_group_interrupt(), used for HP
 *		instance,
 *      @ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), used for ULP
 *		instance.
 * @note: Select HP GPIO pins for HP instances(6 to 57). Do not
 * use GPIO pin number(0 to 5) in HP instance as these are used for other
 * functionality.
 * @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                              uint8_t port,
                                                              uint8_t pin,
                                                              sl_si91x_gpio_polarity_t polarity)
{
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (polarity > GPIO_POLARITY_MAX_VALUE) {
    // Returns invalid parameter status code if polarity >
    // GPIO_POLARITY_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  sl_si91x_gpio_set_group_interrupt_polarity(group_interrupt, port, pin, polarity);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to get polarity of group interrupt .
 * @note: Select HP GPIO pins for HP instances(6 to 57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                          uint8_t port,
                                                          uint8_t pin)
{
  uint8_t polarity = 0;
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if ((port == PORTA) || (port == PORTB) || (port == PORTC)) {
    if (pin > PORT_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORT_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTD) {
    if (pin > PORTD_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTD_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  if (port == PORTE) {
    if (pin > PORTE_PIN_MAX_VALUE) {
      // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
      return SL_STATUS_INVALID_PARAMETER;
    }
  }
  polarity = sl_si91x_gpio_get_group_interrupt_polarity(group_interrupt, port, pin);
  return polarity;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to select and/or event of group interrupt.
 * @example: Consider two GPIO pins for group interrupts.
 *    - If AND event is selected then both GPIO pins, interrupt should be
 *        generated to do specific task.
 *    - If OR event is selected then any one GPIO pin, interrupt
 * 		  generation should be enough to do specific task.
 * @note: We are calling this API, inside the group interrupt
 * configuration API's
 *      @ref sl_si91x_gpio_driver_configure_group_interrupt(), used for HP
 *		instance,
 *      @ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), used for ULP
 *		instance.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_and_or(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_and_or_t and_or)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port >
    // GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt >
    // GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (and_or > GPIO_AND_OR_MAX_VALUE) {
    // Returns invalid parameter status code if and_or >
    // GPIO_AND_OR_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_group_interrupt_and_or(port, group_interrupt, and_or);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used in GPIO HP instance to configure group interrupts.
 * It has configuration pointer of type @ref
 * sl_si91x_gpio_group_interrupt_config_t structure. GPIO HP instance has
 * total 4 ports. Port-0, 1, 2 has 16 pins each. Port-3 has 9 pins. While
 * configuring group interrupts, one can select random ports and pins for
 *  group interrupt.
 * @example 1:
 * - If port 1 is selected, any group of pins(0 to 16) can be selected for
 * group interrupt.
 * - Same applied for other ports also.
 * @example 2:
 * - Once can select port 1, pin 7 and port 2, pin 3 as a group for
 * interrupt generation.
 * - One should assign group count of how many pins are passed.
 * For more clarification look into group interrupt configuration
 * structure
 *       @ref sl_si91x_gpio_driver_group_interrupt_config_t.
 *  @note: Do not use Port 0, GPIO pin number(0 to 5) in HP instance
 *     as these are used for other functionality.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                           sl_gpio_irq_callback_t gpio_callback)
{
  // Check for valid parameters
  if (configuration == NULL) {
    // Returns null pointer status code if configuration == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback == NULL) {
    // Returns null pointer status code if gpio_callback == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback_function_pointer != NULL) {
    return SL_STATUS_BUSY;
  }
  gpio_callback_function_pointer = gpio_callback;
  sl_si91x_gpio_configure_group_interrupt(configuration);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to configure the pin interrupt in GPIO ULP instance.
 * There are total 12 pin interrupts in this instance.
 * To configure the interrupt, first ULP GPIO initialization must be done.
 * The actions to be performed in ULP GPIO initialization are:
 *  - Enable the ULP clock of GPIO ULP instance.
 *  - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *   selected as output/input.
 *  - Set pin mode and direction of the GPIO pin.
 * @note: Select ULP GPIO pins for ULP instances(0 to 11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_pin_interrupt(uint8_t int_no,
                                                             sl_si91x_gpio_interrupt_config_flag_t flags,
                                                             sl_si91x_gpio_pin_ulp_t pin,
                                                             sl_gpio_irq_callback_t gpio_callback)
{
  // Check for valid parameters
  if (gpio_callback == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (int_no > GPIO_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if int_no > GPIO_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (flags > GPIO_FLAGS_MAX_VALUE) {
    // Returns invalid parameter status code if flags > GPIO_FLAGS_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (pin > PORTE_PIN_MAX_VALUE) {
    // Returns invalid parameter status code if pin > PORTE_PIN_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (gpio_callback_function_pointer != NULL) {
    return SL_STATUS_BUSY;
  }
  NVIC_EnableIRQ(ULP_PININT0_NVIC_NAME);
  NVIC_SetPriority(ULP_PININT0_NVIC_NAME, PRIORITY_7);
  gpio_callback_function_pointer = gpio_callback;
  sl_si91x_gpio_configure_ulp_pin_interrupt(int_no, flags, pin);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used in GPIO ULP instance to configure group interrupts.
 * It has configuration pointer of type @ref
 * sl_si91x_gpio_group_interrupt_config_t structure. ULP GPIO domain has
 * only one port and calling as Port 4 in program which has maximum of 12
 * pins. While configuring group interrupts, one can select random pins
 * which are allocated for ULP port.
 * - One should assign group count of how many pins are passed.
 * For more clarification look into group interrupt configuration
 * structure
 *       @ref sl_si91x_gpio_group_interrupt_config_t.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                               sl_gpio_irq_callback_t gpio_callback)
{
  // Check for valid parameters
  if (configuration == NULL) {
    // Returns null pointer status code if configuration == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback == NULL) {
    // Returns null pointer status code if gpio_callback == NULL
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback_function_pointer != NULL) {
    return SL_STATUS_BUSY;
  }
  gpio_callback_function_pointer = gpio_callback;
  sl_si91x_gpio_configure_ulp_group_interrupt(configuration);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP instance.
 * It is used to clear group interrupt.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_group_interrupt(sl_si91x_group_interrupt_t group_interrupt)
{
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_clear_group_interrupt(group_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instance.
 * It is used to get status of group interrupt generated.
 ******************************************************************************/
uint32_t sl_si91x_gpio_driver_get_group_interrupt_status(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  uint32_t status;
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  status = sl_si91x_gpio_get_group_interrupt_status(port, group_interrupt);
  return status;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instance.
 * It is used to select group interrupt wakeup.
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_wakeup(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_wakeup_t flags)
{
  // Check for valid parameters
  if (port > GPIO_PORT_MAX_VALUE) {
    // Returns invalid parameter status code if port > GPIO_PORT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (flags > GPIO_FLAGS_MAX_VALUE) {
    // Returns invalid parameter status code if flags > GPIO_FLAGS_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_group_interrupt_wakeup(port, group_interrupt, flags);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To enable ULP PAD receiver in GPIO ULP instance, ULP GPIO
 * initialization needs to be done first.
 *   - The actions to be performed in ULP GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *  selected as output/input.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to
 *  ULP_GPIO_11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_ulp_pad_receiver(uint8_t gpio_num)
{
  // Check for valid parameters
  if (gpio_num > GPIO_ULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_ULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_enable_ulp_pad_receiver(gpio_num);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to disable the ULP PAD receiver.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to
 * ULP_GPIO_11).
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_ulp_pad_receiver(uint8_t gpio_num)
{
  // Check for valid parameters
  if (gpio_num > GPIO_ULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_ULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_disable_ulp_pad_receiver(gpio_num);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To select the ULP PAD driver disable state in GPIO ULP instance, ULP
 * GPIO initialization needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver disable state of type @ref
 * 		sl_si91x_gpio_driver_disable_state_t.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(uint8_t gpio_num,
                                                                     sl_si91x_gpio_driver_disable_state_t disable_state)
{
  // Check for valid parameters
  if (gpio_num > GPIO_ULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_ULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (disable_state > GPIO_DISABLE_STATE_MAX_VAL) {
    // Returns invalid parameter status code if disable_state > GPIO_DISABLE_STATE_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_ulp_pad_driver_disable_state(gpio_num, disable_state);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To select the ULP PAD driver strength in GPIO ULP instance, ULP GPIO
 *initialization needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver strength of type @ref
 *		sl_si91x_gpio_driver_strength_select_t.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_strength(uint8_t gpio_num,
                                                                sl_si91x_gpio_driver_strength_select_t strength)
{
  // Check for valid parameters
  if (gpio_num > GPIO_ULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_ULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (strength > GPIO_STRENGTH_MAX_VAL) {
    // Returns invalid parameter status code if strength > GPIO_STRENGTH_MAX_VAL
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_ulp_pad_driver_strength(gpio_num, strength);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * To select the ULP PAD slew rate in GPIO ULP instance, ULP GPIO
 * initialization needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is
 *		selected as output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD slew rate of type @ref sl_si91x_gpio_slew_rate_t.
 *  @note: Select ULP GPIO pins for ULP instances(0 to 11).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_slew_rate(uint8_t gpio_num, sl_si91x_gpio_slew_rate_t slew_rate)
{
  // Check for valid parameters
  if (gpio_num > GPIO_ULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if gpio_num > GPIO_ULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (slew_rate > GPIO_SLEW_RATE_MAX_VALUE) {
    // Returns invalid parameter status code if slew_rate > GPIO_SLEW_RATE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_ulp_pad_slew_rate(gpio_num, slew_rate);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to select the UULP mode in NPSS GPIO control register.
 * Few actions are required to be performed before setting the mode,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_mux(uint8_t pin, sl_si91x_uulp_npss_mode_t mode)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (mode > GPIO_MODE_MAX_VALUE) {
    // Returns invalid parameter status code if mode > GPIO_MODE_MAX_VALUE)
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_uulp_npss_pin_mux(pin, mode);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to enable receiver bit in NPSS GPIO control register.
 * Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API,
 * before using this API.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_receiver(uint8_t pin, sl_si91x_gpio_receiver_t receiver)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (receiver > GPIO_RECEIVER_MAX_VALUE) {
    // Returns invalid parameter status code if receiver > GPIO_RECEIVER_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_uulp_npss_receiver(pin, receiver);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to select the UULP direction in NPSS GPIO control
 * register. Few actions are required to be performed before setting the
 * direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Select UULP NPSS direction for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_direction(uint8_t pin, sl_si91x_gpio_direction_t direction)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (direction > GPIO_DIRECTION_MAX_VALUE) {
    // Returns invalid parameter status code if direction > GPIO_DIRECTION_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_uulp_npss_direction(pin, direction);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to get the UULP direction in NPSS GPIO control
 * register. Few actions are required to be performed before setting the
 * direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Get the direction of the GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 *******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_direction(uint8_t pin)
{
  uint8_t direction;
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  direction = sl_si91x_gpio_get_uulp_npss_direction(pin);
  return direction;
}

/*******************************************************************************
 * This API is used to select the UULP pin value in NPSS GPIO control
 * register. Few actions are required to be performed before setting the
 * direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Select the GPIO pin value.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_value(uint8_t pin, sl_si91x_gpio_pin_value_t pin_value)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (pin_value > GPIO_PIN_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if pin_value > GPIO_PIN_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_uulp_npss_pin_value(pin, pin_value);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to toggle the UULP pin.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Toggle the UULP GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_toggle_uulp_npss_pin(uint8_t pin)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_toggle_uulp_npss_pin(pin);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to get the UULP pin value in NPSS GPIO control
 * register. Few actions are required to be performed before setting the
 * direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Select the GPIO pin value.
 *  - Get the GPIO pin value.
 * @note: Select UULP GPIO pins for UULP instances(0 to 4).
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_pin(uint8_t pin)
{
  // Check for valid parameters
  uint8_t uulp_pin;
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  uulp_pin = sl_si91x_gpio_get_uulp_npss_pin(pin);
  return uulp_pin;
}

/*******************************************************************************
 * This API is used to select polarity of the UULP GPIO to be considered
      when used as a wakeup source from any of the Sleep States.
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_polarity(uint8_t pin, sl_si91x_gpio_polarity_t polarity)
{
  // Check for valid parameters
  if (pin > GPIO_UULP_MAX_PIN_NUM) {
    // Returns invalid parameter status code if pin > GPIO_UULP_MAX_PIN_NUM
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (polarity > GPIO_POLARITY_MAX_VALUE) {
    // Returns invalid parameter status code if polarity > GPIO_POLARITY_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_select_uulp_npss_polarity(pin, polarity);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to set the UULP NPSS GPIO to wakeup interrupt
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt)
{
  // Check for valid parameters
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_set_uulp_npss_wakeup_interrupt(npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to clear the UULP NPSS GPIO to wakeup interrupt
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt)
{
  // Check for valid parameters
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_clear_uulp_npss_wakeup_interrupt(npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to mask the UULP NPSS GPIO interrupt.
 * Few actions are required to be performed before interrupt mask is
 * performed,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Select the .
 *  @note: All the UULP interrupts are masked by default.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_uulp_npss_interrupt(uint8_t npssgpio_interrupt)
{
  // Check for valid parameters
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_mask_uulp_npss_interrupt(npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * Get the NPSS GPIO interrupt status.
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_interrupt_status(void)
{
  uint8_t status;
  status = sl_si91x_gpio_get_uulp_interrupt_status();
  return status;
}

/*******************************************************************************
 * This API is used to un-mask the UULP NPSS GPIO interrupt.
 * Few actions are required to be performed before interrupt un-mask is
 * performed,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Set UULP PAD configuration register.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Un-mask interrupt by setting corresponding bit in register.
 *  @note: All the UULP interrupts are masked by default.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_uulp_npss_interrupt(uint8_t npssgpio_interrupt)
{
  // Check for valid parameters
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_unmask_uulp_npss_interrupt(npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to clear the UULP interrupt.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_interrupt(uint8_t npssgpio_interrupt)
{
  // Check for valid parameters
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_clear_uulp_interrupt(npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to configure the pin interrupt in GPIO UULP instance.
 * There are total 5 pin interrupts in this instance.
 * To configure the interrupt, first UULP GPIO initialization must be
 * done. The actions to be performed in UULP GPIO initialization are:
 *  - Enable the ULP clock using @ref sl_si91x_gpio_driver_enable_clock() API.
 *  - Set UULP PAD configuration register.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Configure the UULP pin interrupt.
 * Enable the IRQ handler.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_uulp_interrupt(sl_si91x_gpio_interrupt_config_flag_t flags,
                                                          uint8_t npssgpio_interrupt,
                                                          sl_gpio_irq_callback_t gpio_callback)
{
  // Check for valid parameters
  if (gpio_callback == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (gpio_callback_function_pointer != NULL) {
    return SL_STATUS_BUSY;
  }
  if (flags > GPIO_FLAGS_MAX_VALUE) {
    // Returns invalid parameter status code if flags > GPIO_FLAGS_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE) {
    // Returns invalid parameter status code if npssgpio_interrupt > GPIO_NPSSGPIO_INTERRUPT_VALUE_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  gpio_callback_function_pointer = gpio_callback;
  sl_si91x_gpio_configure_uulp_interrupt(flags, npssgpio_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to clear one (or) more pending ULP GPIO interrupts.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_interrupt(uint32_t flags)
{
  // Check for valid parameters
  if (flags > GPIO_FLAGS_MAX_VALUE) {
    // Returns invalid parameter status code if flags > GPIO_FLAGS_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_clear_ulp_interrupt(flags);
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This API is used to clear the ULP group interrupts.
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_group_interrupt(sl_si91x_group_interrupt_t group_interrupt)
{
  // Check for valid parameters
  if (group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE) {
    // Returns invalid parameter status code if group_interrupt > GPIO_GROUP_INTERRUPT_MAX_VALUE
    return SL_STATUS_INVALID_PARAMETER;
  }
  sl_si91x_gpio_clear_ulp_group_interrupt(group_interrupt);
  return SL_STATUS_OK;
}

/*******************************************************************************
 *   This API handles GPIO pin interrupt 0 request
 ******************************************************************************/
void PIN_IRQ0_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_0);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 1 request
 ******************************************************************************/
void PIN_IRQ1_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_1);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 2 request
 ******************************************************************************/
void PIN_IRQ2_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_2);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 3 request
 ******************************************************************************/
void PIN_IRQ3_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_3);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 4 request
 ******************************************************************************/
void PIN_IRQ4_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_4);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 5 request
 ******************************************************************************/
void PIN_IRQ5_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_5);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 6 request
 ******************************************************************************/
void PIN_IRQ6_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_6);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO pin interrupt 7 request
 ******************************************************************************/
void PIN_IRQ7_Handler(void)
{
  sl_gpio_driver_clear_interrupts(PIN_INTR_7);
  gpio_callback_function_pointer();
}
/*******************************************************************************
 *  This API handles GPIO Group interrupt 0 request
 ******************************************************************************/
void GRP_IRQ0_Handler(void)
{
  sl_si91x_gpio_driver_clear_group_interrupt(GROUP_INT_1);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles GPIO Group interrupt 1 request
 ******************************************************************************/
void GRP_IRQ1_Handler(void)
{
  sl_si91x_gpio_driver_clear_group_interrupt(GROUP_INT_2);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles UULP GPIO pin interrupt 0 request
 ******************************************************************************/
void UULP_PIN_IRQ_Handler(void)
{
  if ((sl_si91x_gpio_driver_get_uulp_interrupt_status() & UULP_INTR_1) != UULP_MASK) {
    sl_si91x_gpio_driver_clear_uulp_interrupt(UULP_PIN_INTR_1);
  }
  if ((sl_si91x_gpio_driver_get_uulp_interrupt_status() & UULP_INTR_2) != UULP_MASK) {
    sl_si91x_gpio_driver_clear_uulp_interrupt(UULP_PIN_INTR_2);
  }
  if ((sl_si91x_gpio_driver_get_uulp_interrupt_status() & UULP_INTR_3) != UULP_MASK) {
    sl_si91x_gpio_driver_clear_uulp_interrupt(UULP_PIN_INTR_3);
  }
  if ((sl_si91x_gpio_driver_get_uulp_interrupt_status() & UULP_INTR_4) != UULP_MASK) {
    sl_si91x_gpio_driver_clear_uulp_interrupt(UULP_PIN_INTR_4);
  }
  if ((sl_si91x_gpio_driver_get_uulp_interrupt_status() & UULP_INTR_5) != UULP_MASK) {
    sl_si91x_gpio_driver_clear_uulp_interrupt(UULP_PIN_INTR_5);
  }
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles ULP GPIO OR'ed pin interrupt request
 ******************************************************************************/
void ULP_PIN_IRQ_Handler(void)
{
  sl_si91x_gpio_driver_clear_ulp_interrupt(ULP_PIN_INT);
  gpio_callback_function_pointer();
}

/*******************************************************************************
 *  This API handles ULP GPIO group interrupt request
 ******************************************************************************/
void ULP_GROUP_IRQ_Handler(void)
{
  sl_si91x_gpio_driver_clear_ulp_group_interrupt(ULP_GROUP_INT);
  gpio_callback_function_pointer();
}
