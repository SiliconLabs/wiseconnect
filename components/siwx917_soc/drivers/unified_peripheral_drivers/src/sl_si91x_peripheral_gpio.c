/*******************************************************************************
 * @file sl_si91x_peripheral_gpio.c
 * @brief General Purpose IO (GPIO) peripheral API.
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
#include "sl_si91x_peripheral_gpio.h"
#include "rsi_board.h"

/*******************************************************************************
 ***************************  DEFINES / MACROS   ********************************
 ******************************************************************************/
#define GPIO_RELEASE_VERSION 0 // gpio Release version
#define GPIO_MAJOR_VERSION   0 // gpio SQA version
#define GPIO_MINOR_VERSION   2 // gpio Developer version

/*******************************************************************************
 ************************       GLOBAL FUNCTIONS      **************************
 ******************************************************************************/
extern __INLINE void sl_gpio_set_pin_output(sl_gpio_port_t port, uint8_t pin);
extern __INLINE void sl_gpio_clear_pin_output(sl_gpio_port_t port, uint8_t pin);
/*******************************************************************************
 * This API is used to configure the pin interrupt in GPIO HP instance.
 * There are total 8 pin interrupts in this instance.
 * To configure the interrupt, first GPIO initialization must be done.
 * The actions to be performed in GPIO initialization are:
 *  - Enable the M4 clock of GPIO HP instance.
 *  - Select PAD selection of the GPIO HP instance.
 *  - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *    output/input.
 *  - Set pin mode and direction of the GPIO pin.
 * Configuring the pin interrupt requires port number, pin number, interrupt number,
 *  and interrupt flag to be generated.
 * Enable the IRQ handler.
 ******************************************************************************/
void sl_gpio_configure_interrupt(sl_gpio_port_t port, uint8_t pin, uint32_t int_no, sl_gpio_interrupt_flag_t flags)
{
  // Pin interrupt configuration in HP GPIO instance
  SL_GPIO_ASSERT(SL_GPIO_PORT_PIN_VALID(port, pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_FLAG(flags));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_INTR(int_no));
  GPIO->INTR[int_no].GPIO_INTR_CTRL_b.PORT_NUMBER = port;
  GPIO->INTR[int_no].GPIO_INTR_CTRL_b.PIN_NUMBER  = (sl_si91x_gpio_pin_t)pin;
  // Enable GPIO interrupt falling edge
  if ((flags & SL_GPIO_INTERRUPT_FALL_EDGE) == SL_GPIO_INTERRUPT_FALL_EDGE) {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  }
  // Disable GPIO interrupt falling edge
  else {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable GPIO interrupt rising edge
  if ((flags & SL_GPIO_INTERRUPT_RISE_EDGE) == SL_GPIO_INTERRUPT_RISE_EDGE) {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  }
  // Disable GPIO interrupt rising edge
  else {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable GPIO interrupt level high
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_HIGH) == SL_GPIO_INTERRUPT_LEVEL_HIGH) {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  }
  // Disable GPIO interrupt level high
  else {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable GPIO interrupt level low
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_LOW) == SL_GPIO_INTERRUPT_LEVEL_LOW) {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  }
  // Disable GPIO interrupt level low
  else {
    GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Un-mask the interrupt
  GPIO->INTR[int_no].GPIO_INTR_CTRL_b.MASK = CLR;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to set pin mode.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To set the pin mode in GPIO HP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * - If GPIO ULP instance is considered, the following actions are performed:
 *   - To set the pin mode in GPIO ULP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
void sl_gpio_set_pin_mode(sl_gpio_port_t port, uint8_t pin, sl_gpio_mode_t mode, uint32_t output_value)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(output_value));
  /* If disabling a pin, do not modify register to reduce the chance of */
  /* a glitch/spike(may not be sufficient precaution in all use cases). */
  if (mode != SL_GPIO_MODE_DISABLED) {
    if (output_value) {
      sl_gpio_set_pin_output(port, pin); // Set the GPIO pin
    } else {
      sl_gpio_clear_pin_output(port, pin); // Clear the GPIO pin
    }
  }
  // if condition is satisfied when ULP GPIO instance occurs
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_MODE_PARAMETER(mode));
    ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.MODE = mode; // Set mode in ULP GPIO instance
  }
  // else condition is satisfied when HP GPIO instance occurs
  else {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_MODE(mode));
    // Set mode in HP GPIO instance
    GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE = mode;
  }
  if (mode == SL_GPIO_MODE_DISABLED) {
    if (output_value) {
      sl_gpio_set_pin_output(port, pin); // Set the GPIO pin
    } else {
      sl_gpio_clear_pin_output(port, pin); // Clear the GPIO pin
    }
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to get pin mode.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To get the pin status in GPIO HP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin mode of GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * - If GPIO ULP instance is considered, the following actions are performed:
 *   - To get the pin mode in GPIO ULP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin mode of GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
sl_gpio_mode_t sl_gpio_get_pin_mode(sl_gpio_port_t port, uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    // Read status of the pin in ULP GPIO instance
    return (sl_gpio_mode_t)(ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.MODE);
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Read status of the pin in HP GPIO instance
    return (sl_gpio_mode_t)(GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE);
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to set pin direction.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To set the pin direction in GPIO HP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * - If GPIO ULP instance is considered, the following actions are performed:
 *   - To set the pin direction in GPIO ULP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_set_pin_direction(uint8_t port, uint8_t pin, sl_si91x_gpio_direction_t direction)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(direction));
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    // Set the pin direction in ULP GPIO instance
    ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.DIRECTION = direction;
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Set the pin direction in HP GPIO instance
    GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.DIRECTION = direction;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances to get pin direction.
 * - If GPIO HP instance is considered, the following actions are performed:
 *   - To get the pin direction in GPIO HP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the M4 clock of GPIO HP instance.
 *      - Select PAD selection of the GPIO HP instance.
 *      - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin direction of the GPIO pin.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * - If GPIO ULP instance is considered, the following actions are performed:
 *   - To get the pin direction in GPIO ULP instance, GPIO initialization needs to be done first.
 *   - The actions to be performed in GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *      - Set pin mode and direction of the GPIO pin.
 *      - Get the pin direction of the GPIO pin.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
uint8_t sl_si91x_gpio_get_pin_direction(uint8_t port, uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    // Get the pin direction in ULP GPIO instance
    return ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.DIRECTION;
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Get the pin direction in HP GPIO instance
    return GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.DIRECTION;
  }
}

/*******************************************************************************
 * This API is used to enable PAD receiver in GPIO HP instance.
 * The actions to be performed for enabling PAD are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *     output/input.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
*******************************************************************************/
void sl_si91x_gpio_enable_pad_receiver(uint8_t gpio_padnum)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PIN(gpio_padnum));
  // Set the REN(receiver bit) in PAD configuration register
  PAD_REG(gpio_padnum)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_REN = SET;
}

/*******************************************************************************
 * This API is used to disable PAD receiver in GPIO HP instance.
 * The actions to be performed for disabling PAD are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Disable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *     output/input.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
*******************************************************************************/
void sl_si91x_gpio_disable_pad_receiver(uint8_t gpio_padnum)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PIN(gpio_padnum));
  // Clear the REN(receiver bit) in PAD configuration register
  PAD_REG(gpio_padnum)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_REN = CLR;
}

/*******************************************************************************
 * This API is used to enable PAD selection in GPIO HP instance.
 * The actions to be performed for enable PAD selection are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *  @note: PAD number(25 to 30) are used for HOST PAD selection.
 *      Do not use PAD number-9 as it is used for other functionality.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
*******************************************************************************/
void sl_si91x_gpio_enable_pad_selection(uint8_t gpio_padnum)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PAD(gpio_padnum));
  if (gpio_padnum < PAD_SELECT) {
    // (tass_m4ss_gpio_sel)PAD selection (0 to 21)
    // A value of 1 on this gives control to M4SS(by default it is 0)
    PADSELECTION |= BIT(gpio_padnum);
  } else if (gpio_padnum >= HOST_PAD_MIN && gpio_padnum <= HOST_PAD_MAX) {
    // (tass_m4ss_gpio_sel)PAD selection (25 to 30)
    // A value of 1 on this gives control to M4SS(by default it is 0)
    HOST_PADS_GPIO_MODE |= BIT(gpio_padnum - 12);
    // (tass_m4ss_gpio_sel)PAD selection (22 t0 33)
    // A value of 1 on this gives control to M4SS(by default it is 0 means ta control)
  } else {
    PADSELECTION_1 |= BIT(gpio_padnum - 22);
  }
}

/*******************************************************************************
 * To select the PAD driver strength in GPIO HP instance, GPIO initialization
 *   needs to be done first.
 * - The actions to be performed in GPIO initialization are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver strength of type @ref sl_si91x_gpio_driver_strength_select_t.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 ******************************************************************************/
void sl_si91x_gpio_select_pad_driver_strength(uint8_t gpio_padnum, sl_si91x_gpio_driver_strength_select_t strength)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PIN(gpio_padnum));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_STRENGTH(strength));
  // Select the required drive strength in HP GPIO instance
  PAD_REG(gpio_padnum)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_E1_E2 = strength;
}

/*******************************************************************************
 * To select the PAD driver disable state in GPIO HP instance, GPIO initialization
 *   needs to be done first.
 * - The actions to be performed in GPIO initialization are:
 *   - Enable the M4 clock of GPIO HP instance.
 *   - Select PAD selection of the GPIO HP instance.
 *   - Enable PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver disable state of type @ref sl_si91x_gpio_driver_disable_state_t.
 *  @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 ******************************************************************************/
void sl_si91x_gpio_select_pad_driver_disable_state(uint8_t gpio_padnum,
                                                   sl_si91x_gpio_driver_disable_state_t disable_state)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PIN(gpio_padnum));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_DISABLE_STATE(disable_state));
  // Select the required driver disable state in HP GPIO instance
  PAD_REG(gpio_padnum)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_P1_P2 = disable_state;
}

/*******************************************************************************
 * The GPIO pins to work in different instances, requires this clock.
 * For GPIO HP instance, enable M4 clock of type @ref sl_si91x_gpio_select_clock_t.
 * For GPIO ULP/UULP instances, enable ULP clock of type
 *  @ref sl_si91x_gpio_select_clock_t
 ******************************************************************************/
void sl_si91x_gpio_enable_clock(sl_si91x_gpio_select_clock_t clock)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(clock));
  // Set M4 clock
  if (clock == M4CLK_GPIO) {
    SL_PERIPHERAL_CLK->CLK_ENABLE_SET_REG3_b.EGPIO_CLK_ENABLE_b  = SET;
    SL_PERIPHERAL_CLK->CLK_ENABLE_SET_REG2_b.EGPIO_PCLK_ENABLE_b = SET;
  }
  // Set ULP clock
  else {
    ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_CLK_EN_b      = SET;
    ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_PCLK_ENABLE_b = SET;
  }
}

/*******************************************************************************
 * This API disables the M4/ ULP clock of GPIO instances.
 * For GPIO HP instance, disable M4 clock of type @ref sl_si91x_gpio_select_clock_t.
 * For GPIO ULP/UULP instances, disable ULP clock of type
 *  @ref sl_si91x_gpio_select_clock_t
 ******************************************************************************/
void sl_si91x_gpio_disable_clock(sl_si91x_gpio_select_clock_t clock)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(clock));
  // Clear M4 clock
  if (clock == M4CLK_GPIO) {
    SL_PERIPHERAL_CLK->CLK_ENABLE_SET_REG3_b.EGPIO_CLK_ENABLE_b  = CLR;
    SL_PERIPHERAL_CLK->CLK_ENABLE_SET_REG2_b.EGPIO_PCLK_ENABLE_b = CLR;
  }
  // Clear ULP clock
  else {
    ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_CLK_EN_b      = CLR;
    ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_PCLK_ENABLE_b = CLR;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to enable the group interrupts.
 * @note: We are calling this API, inside the group interrupt configuration API's
 *      @ref sl_si91x_gpio_configure_group_interrupt(), used for HP instance,
 *      @ref sl_si91x_gpio_configure_ulp_group_interrupt(), used for ULP instance.
 * @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_enable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt, uint8_t port, uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  // Enable group interrupt in ULP GPIO instance
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT = ENABLE;
    // Enable group interrupt 1 in ULP GPIO instance
    if (group_interrupt == GROUP_INT_1) {
      ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_ENABLE = ENABLE;
    }
  }
  // Enable group interrupt in HP GPIO instance
  else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT = ENABLE;
    if (group_interrupt == GROUP_INT_1) {
      // Enable group interrupt 1 in HP GPIO instance
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_ENABLE = ENABLE;
    } else {
      // Enable group interrupt 2 in HP GPIO instance
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT2_ENABLE = ENABLE;
    }
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to disable the group interrupts.
 * @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_disable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt, uint8_t port, uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  // Disable group interrupt in ULP GPIO instance
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT = DISABLE;
    // Disable group interrupt 1 in ULP GPIO instance
    ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_ENABLE = DISABLE;
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Disable group interrupt in HP GPIO instance
    GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT = DISABLE;
    if (group_interrupt == GROUP_INT_1) {
      // Disable group interrupt 1 in HP GPIO instance
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_ENABLE = DISABLE;
    } else {
      // Disable group interrupt 2 in HP GPIO instance
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT2_ENABLE = DISABLE;
    }
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to mask the group interrupts.
 *******************************************************************************/
void sl_si91x_gpio_mask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  if (port == SL_ULP_GPIO_PORT) {
    // Enable group interrupt mask in ULP GPIO instance
    ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK = ENABLE;
  } else {
    // Enable group interrupt mask in HP GPIO instance
    GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK = ENABLE;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to unmask the group interrupts.
 * @note: We are calling this API, inside the group interrupt configuration API's
 *      @ref sl_si91x_gpio_configure_group_interrupt(), used for HP instance,
 *      @ref sl_si91x_gpio_configure_ulp_group_interrupt(), used for ULP instance.
*******************************************************************************/
void sl_si91x_gpio_unmask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  if (port == SL_ULP_GPIO_PORT) {
    // Disable group interrupt mask in ULP GPIO instance
    ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK = DISABLE;
  } else {
    // Disable group interrupt mask in HP GPIO instance
    GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK = DISABLE;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to set level/edge event of group interrupt.
 * @note: We are calling this API, inside the group interrupt configuration API's
 *      @ref sl_si91x_gpio_configure_group_interrupt(), used for HP instance,
 *      @ref sl_si91x_gpio_configure_ulp_group_interrupt(), used for ULP instance.
 ******************************************************************************/
void sl_si91x_gpio_set_group_interrupt_level_edge(uint8_t port,
                                                  sl_si91x_group_interrupt_t group_interrupt,
                                                  sl_si91x_gpio_level_edge_t level_edge)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(level_edge));
  if (port == SL_ULP_GPIO_PORT) {
    // Enable group level edge interrupt in ULP GPIO instance
    ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.LEVEL_EDGE = level_edge;
  } else {
    // Enable group level edge interrupt in HP GPIO instance
    GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.LEVEL_EDGE = level_edge;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to get level/edge event of group interrupt.
 ******************************************************************************/
uint8_t sl_si91x_gpio_get_group_interrupt_level_edge(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  if (port == SL_ULP_GPIO_PORT) {
    // Get group level edge interrupt in ULP GPIO instance
    return ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.LEVEL_EDGE;
  } else {
    // Get group level edge interrupt in HP GPIO instance
    return GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.LEVEL_EDGE;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to set polarity of group interrupt.
 * @note: We are calling this API, inside the group interrupt configuration API's
 *      @ref sl_si91x_gpio_configure_group_interrupt(), used for HP instance,
 *      @ref sl_si91x_gpio_configure_ulp_group_interrupt(), used for ULP instance.
 * @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
void sl_si91x_gpio_set_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                uint8_t port,
                                                uint8_t pin,
                                                sl_si91x_gpio_polarity_t polarity)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(polarity));
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    // Set group interrupt polarity in ULP GPIO instance
    if (group_interrupt == GROUP_INT_1) {
      ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY = polarity;
    }
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Set group interrupt polarity in HP GPIO instance
    if (group_interrupt == GROUP_INT_1) {
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY = polarity;
    } else {
      GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT2_POLARITY = polarity;
    }
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to get polarity of group interrupt .
 * @note: Select HP GPIO pins for HP instances(GPIO_6 to GPIO_57). Do not use
 *      GPIO pin number(0 to 5) in HP instance as these are used for other functionality.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
uint8_t sl_si91x_gpio_get_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                   uint8_t port,
                                                   uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  if (port == SL_ULP_GPIO_PORT) {
    SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PORT_PIN(port, pin));
    // Get group interrupt polarity in ULP GPIO instance
    return ULP_GPIO->PIN_CONFIG[pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY;
  } else {
    SL_GPIO_ASSERT(SL_GPIO_NDEBUG_PORT_PIN(port, pin));
    // Get group interrupt polarity in HP GPIO instance
    if (group_interrupt == GROUP_INT_1) {
      return GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY;
    } else {
      return GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.GROUP_INTERRUPT2_POLARITY;
    }
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instances.
 * It is used to select and/or event of group interrupt.
 * @example: Consider two GPIO pins for group interrupts.
 *    - If AND event is selected then both GPIO pins, interrupt should be
 *        generated to do specific task.
 *    - If OR event is selected then any one GPIO pin, interrupt generation
 *       should be enough to do specific task.
 * @note: We are calling this API, inside the group interrupt configuration API's
 *      @ref sl_si91x_gpio_configure_group_interrupt(), used for HP instance,
 *      @ref sl_si91x_gpio_configure_ulp_group_interrupt(), used for ULP instance.
*******************************************************************************/
void sl_si91x_gpio_select_group_interrupt_and_or(uint8_t port,
                                                 sl_si91x_group_interrupt_t group_interrupt,
                                                 sl_si91x_gpio_and_or_t and_or)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(and_or));
  if (port == SL_ULP_GPIO_PORT) {
    // Set group interrupt OR in ULP GPIO instance
    if ((and_or & SL_GPIO_GROUP_INTERRUPT_OR) == (SL_GPIO_GROUP_INTERRUPT_OR)) {
      ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR = SET;
    }
    // Set group interrupt AND in ULP GPIO instance
    else {
      ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR = CLR;
    }
  } else {
    // Set group interrupt OR in HP GPIO instance
    if ((and_or & SL_GPIO_GROUP_INTERRUPT_OR) == (SL_GPIO_GROUP_INTERRUPT_OR)) {
      GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR = SET;
    }
    // Set group interrupt AND in HP GPIO instance
    else {
      GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR = CLR;
    }
  }
}

/*******************************************************************************
 * This API is used in GPIO HP instance to configure group interrupts.
 * It has configuration pointer of type @ref sl_si91x_gpio_group_interrupt_config_t
 *    structure.
 * GPIO HP instance has total 4 ports. Port-0, 1, 2 has 16 pins each. Port-3 has 9 pins.
 * While configuring group interrupts, one can select random ports and pins for
 *  group interrupt.
 * @example 1:
 * - If port 1 is selected, any group of pins(0 to 16) can be selected for group interrupt.
 * - Same applied for other ports also.
 * @example 2:
 * - Once can select port 1, pin 7 and port 2, pin 3 as a group for interrupt generation.
 * - One should assign group count of how many pins are passed.
 * For more clarification look into group interrupt configuration structure
 *       @ref sl_si91x_gpio_group_interrupt_config_t.
 *  @note: Do not use Port 0, GPIO pin number(0 to 5) in HP instance
 *     as these are used for other functionality.
*******************************************************************************/
void sl_si91x_gpio_configure_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration)
{
  uint8_t int_pin_count;
  // Group interrupt pin configuration in HP GPIO instance
  for (int_pin_count = 0; int_pin_count < configuration->grp_interrupt_cnt; int_pin_count++) {
    sl_gpio_set_pin_mode(configuration->grp_interrupt_port[int_pin_count],
                         configuration->grp_interrupt_pin[int_pin_count],
                         SL_GPIO_MODE_0,
                         SET);
    sl_si91x_gpio_set_pin_direction(configuration->grp_interrupt_port[int_pin_count],
                                    configuration->grp_interrupt_pin[int_pin_count],
                                    GPIO_INPUT);
    sl_si91x_gpio_enable_pad_receiver(configuration->grp_interrupt_pin[int_pin_count]);
    sl_si91x_gpio_set_group_interrupt_polarity(configuration->grp_interrupt,
                                               configuration->grp_interrupt_port[int_pin_count],
                                               configuration->grp_interrupt_pin[int_pin_count],
                                               configuration->grp_interrupt_pol[int_pin_count]);
    sl_si91x_gpio_set_group_interrupt_level_edge(configuration->grp_interrupt_port[int_pin_count],
                                                 configuration->grp_interrupt,
                                                 configuration->level_edge);
    sl_si91x_gpio_select_group_interrupt_and_or(configuration->grp_interrupt_port[int_pin_count],
                                                configuration->grp_interrupt,
                                                configuration->and_or);
    sl_si91x_gpio_enable_group_interrupt(configuration->grp_interrupt,
                                         configuration->grp_interrupt_port[int_pin_count],
                                         configuration->grp_interrupt_pin[int_pin_count]);
    sl_si91x_gpio_unmask_group_interrupt(configuration->grp_interrupt_port[int_pin_count],
                                         configuration->grp_interrupt);
  }
  // NVIC enable for group interrupt 1
  if (configuration->grp_interrupt == GROUP_INT_1) {
    NVIC_EnableIRQ(GROUP_0_INTERRUPT_NAME);
    NVIC_SetPriority(GROUP_0_INTERRUPT_NAME, PRIORITY_15);
  }
  // NVIC enable for group interrupt 2
  else if (configuration->grp_interrupt == GROUP_INT_2) {
    NVIC_EnableIRQ(GROUP_1_INTERRUPT_NAME);
    NVIC_SetPriority(GROUP_1_INTERRUPT_NAME, PRIORITY_15);
  }
}

/*******************************************************************************
 * This API is used to configure the pin interrupt in GPIO ULP instance.
 * There are total 12 pin interrupts in this instance.
 * To configure the interrupt, first ULP GPIO initialization must be done.
 * The actions to be performed in ULP GPIO initialization are:
 *  - Enable the ULP clock of GPIO ULP instance.
 *  - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *    output/input.
 *  - Set pin mode and direction of the GPIO pin.
 * Configuring the pin interrupt requires pin number, interrupt number,
 *  and interrupt flag to be generated.
 * Enable the IRQ handler.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_configure_ulp_pin_interrupt(uint8_t int_no,
                                               sl_si91x_gpio_interrupt_config_flag_t flags,
                                               sl_si91x_gpio_pin_ulp_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_FLAG(flags));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_INTR(int_no));
  // Pin interrupt configuration in ULP GPIO instance
  ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.PORT_NUMBER = ULP_PORT_NUM;
  ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.PIN_NUMBER  = pin;
  // Enable or disable GPIO interrupt falling edge
  if ((flags & SL_GPIO_INTERRUPT_FALL_EDGE) == SL_GPIO_INTERRUPT_FALL_EDGE) {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  } else {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable or disable GPIO interrupt rising edge
  if ((flags & SL_GPIO_INTERRUPT_RISE_EDGE) == SL_GPIO_INTERRUPT_RISE_EDGE) {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  } else {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable or disable GPIO interrupt level high
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_HIGH) == SL_GPIO_INTERRUPT_LEVEL_HIGH) {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  } else {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Enable or disable GPIO interrupt level low
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_LOW) == SL_GPIO_INTERRUPT_LEVEL_LOW) {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE = SL_GPIO_INTERRUPT_ENABLE;
  } else {
    ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE = SL_GPIO_INTERRUPT_DISABLE;
  }
  // Un-mask the interrupt
  ULP_GPIO->INTR[int_no].GPIO_INTR_CTRL_b.MASK = CLR;
}

/*******************************************************************************
 * This API is used in GPIO ULP instance to configure group interrupts.
 * It has configuration pointer of type @ref sl_si91x_gpio_group_interrupt_config_t
 *    structure.
 * ULP GPIO domain has only one port and calling as Port 4 in program which has maximum of 12 pins.
 * While configuring group interrupts, one can select random pins which are allocated for
 *  ULP port.
 * - One should assign group count of how many pins are passed.
 * For more clarification look into group interrupt configuration structure
 *       @ref sl_si91x_gpio_group_interrupt_config_t.
*******************************************************************************/
void sl_si91x_gpio_configure_ulp_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration)
{
  uint8_t int_pin_count;
  // Group interrupt pin configuration in ULP GPIO instance
  for (int_pin_count = 0; int_pin_count < configuration->grp_interrupt_cnt; int_pin_count++) {
    sl_gpio_set_pin_mode(SL_ULP_GPIO_PORT, configuration->grp_interrupt_pin[int_pin_count], SL_GPIO_MODE_0, SET);
    sl_si91x_gpio_set_pin_direction(SL_ULP_GPIO_PORT, configuration->grp_interrupt_pin[int_pin_count], GPIO_INPUT);
    sl_si91x_gpio_enable_ulp_pad_receiver(configuration->grp_interrupt_pin[int_pin_count]);
    sl_si91x_gpio_set_group_interrupt_polarity(configuration->grp_interrupt,
                                               SL_ULP_GPIO_PORT,
                                               configuration->grp_interrupt_pin[int_pin_count],
                                               configuration->grp_interrupt_pol[int_pin_count]);
    sl_si91x_gpio_set_group_interrupt_level_edge(SL_ULP_GPIO_PORT,
                                                 configuration->grp_interrupt,
                                                 configuration->level_edge);
    sl_si91x_gpio_select_group_interrupt_and_or(SL_ULP_GPIO_PORT, configuration->grp_interrupt, configuration->and_or);
    sl_si91x_gpio_enable_group_interrupt(configuration->grp_interrupt,
                                         SL_ULP_GPIO_PORT,
                                         configuration->grp_interrupt_pin[int_pin_count]);
    sl_si91x_gpio_unmask_group_interrupt(SL_ULP_GPIO_PORT, configuration->grp_interrupt);
  }
  // NVIC enable for ULP group interrupt
  NVIC_EnableIRQ(ULP_GROUP_INTERRUPT_NAME);
  NVIC_SetPriority(ULP_GROUP_INTERRUPT_NAME, PRIORITY_14);
}

/*******************************************************************************
 * This API is used for GPIO HP instance.
 * It is used to clear group interrupt.
*******************************************************************************/
void sl_si91x_gpio_clear_group_interrupt(sl_si91x_group_interrupt_t group_interrupt)
{
  // Clear group interrupt in HP GPIO instance
  GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_STS_b.INTERRUPT_STATUS = SET;
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instance.
 * It is used to get status of group interrupt generated.
 ******************************************************************************/
uint32_t sl_si91x_gpio_get_group_interrupt_status(uint8_t port, sl_si91x_group_interrupt_t group_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  if (port == SL_ULP_GPIO_PORT) {
    // Get group interrupt status in ULP GPIO instance
    return ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_STS;
  } else {
    // Get group interrupt status in HP GPIO instance
    return GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_STS;
  }
}

/*******************************************************************************
 * This API is used for GPIO HP, ULP instance.
 * It is used to select group interrupt wakeup.
 ******************************************************************************/
void sl_si91x_gpio_select_group_interrupt_wakeup(uint8_t port,
                                                 sl_si91x_group_interrupt_t group_interrupt,
                                                 uint8_t flags)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PORT(port));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(group_interrupt));
  // Enables or Disables wakeup group interrupt in ULP GPIO instance
  if (port == SL_ULP_GPIO_PORT) {
    if ((flags & SL_GPIO_GROUP_INTERRUPT_WAKEUP) == (SL_GPIO_GROUP_INTERRUPT_WAKEUP)) {
      ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_WAKEUP = SET;
    } else {
      ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_WAKEUP = CLR;
    }
  } else {
    // Enables or Disables wakeup group interrupt in HP GPIO instance
    if ((flags & SL_GPIO_GROUP_INTERRUPT_WAKEUP) == (SL_GPIO_GROUP_INTERRUPT_WAKEUP)) {
      GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_WAKEUP = SET;
    } else {
      GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_WAKEUP = CLR;
    }
  }
}

/*******************************************************************************
 * To enable ULP PAD receiver in GPIO ULP instance, ULP GPIO initialization needs to be done first.
 *   - The actions to be performed in ULP GPIO initialization are:
 *      - Enable the ULP clock of GPIO ULP instance.
 *      - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_enable_ulp_pad_receiver(uint8_t gpio_padnum)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(gpio_padnum));
  // Enable receiver bit in PAD configuration register in ULP instance
  ULP_PAD_CONFIG_REG |= BIT(gpio_padnum);
}

/*******************************************************************************
 * This API is used to disable the ULP PAD receiver.
 * @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
*******************************************************************************/
void sl_si91x_gpio_disable_ulp_pad_receiver(uint32_t gpio_padnum)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(gpio_padnum));
  // Disable receiver bit in PAD configuration register in ULP instance
  ULP_PAD_CONFIG_REG &= ~BIT(gpio_padnum);
}

/*******************************************************************************
 * To select the ULP PAD driver disable state in GPIO ULP instance, ULP GPIO initialization
 *   needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver disable state of type @ref sl_si91x_gpio_driver_disable_state_t.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
void sl_si91x_gpio_select_ulp_pad_driver_disable_state(uint8_t gpio_padnum,
                                                       sl_si91x_gpio_driver_disable_state_t disable_state)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(gpio_padnum));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_DISABLE_STATE(disable_state));
  // Select driver disable state in ULP PAD configuration registers
  if (gpio_padnum <= GPIO_PAD_3) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_P1_P2_1 = disable_state;
  } else if ((gpio_padnum >= GPIO_PAD_4) && (gpio_padnum <= GPIO_PAD_7)) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_P1_P2_2 = disable_state;
  } else {
    ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_P1_P2_1 = disable_state;
  }
}

/*******************************************************************************
 * To select the ULP PAD driver strength in GPIO ULP instance, ULP GPIO initialization
 *   needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD driver strength of type @ref sl_si91x_gpio_driver_strength_select_t.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
void sl_si91x_gpio_select_ulp_pad_driver_strength(uint8_t gpio_padnum, sl_si91x_gpio_driver_strength_select_t strength)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(gpio_padnum));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_STRENGTH(strength));
  // Select drive strength in ULP PAD configuration registers
  if (gpio_padnum <= GPIO_PAD_3) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_E1_E2_1 = strength;
  } else if ((gpio_padnum >= GPIO_PAD_4) && (gpio_padnum <= GPIO_PAD_7)) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_E1_E2_2 = strength;
  } else {
    ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_E1_E2_1 = strength;
  }
}

/*******************************************************************************
 * To select the ULP PAD slew rate in GPIO ULP instance, ULP GPIO initialization
 *   needs to be done first.
 * - The actions to be performed in ULP GPIO initialization are:
 *   - Enable the ULP clock of GPIO ULP instance.
 *   - Enable ULP PAD receiver for GPIO pin number, whether GPIO pin is selected as
 *        output/input.
 *   - Set pin mode and direction of the GPIO pin.
 *   - Select the PAD slew rate of type @ref sl_si91x_gpio_slew_rate_t.
 *  @note: Select ULP GPIO pins for ULP instances(ULP_GPIO_0 to ULP_GPIO_11).
 ******************************************************************************/
void sl_si91x_gpio_select_ulp_pad_slew_rate(uint8_t gpio_padnum, sl_si91x_gpio_slew_rate_t slew_rate)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_ULP_PIN(gpio_padnum));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(slew_rate));
  // Select slew rate in ULP PAD configuration registers
  if (gpio_padnum <= GPIO_PAD_3) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_SR_1 = slew_rate;
  } else if ((gpio_padnum >= GPIO_PAD_4) && (gpio_padnum <= GPIO_PAD_7)) {
    ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_SR_2 = slew_rate;
  } else {
    ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_SR_1 = slew_rate;
  }
}

/*******************************************************************************
 * This API is used to select the UULP mode in NPSS GPIO control register.
 * Few actions are required to be performed before setting the mode,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
void sl_si91x_gpio_set_uulp_npss_pin_mux(uint8_t pin, sl_si91x_uulp_npss_mode_t mode)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_MODE_PARAMETER(mode));
  // Select pin mode in UULP GPIO instance
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE = mode;
}

/*******************************************************************************
 * This API is used to enable receiver bit in NPSS GPIO control register.
 * Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API, before
 *    using this API.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
void sl_si91x_gpio_select_uulp_npss_receiver(uint8_t pin, sl_si91x_gpio_receiver_t receiver)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(receiver));
  // Select input buffer in UULP GPIO instance
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_REN = receiver;
}

/*******************************************************************************
 * This API is used to select the UULP direction in NPSS GPIO control register.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Select UULP NPSS direction for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
void sl_si91x_gpio_set_uulp_npss_direction(uint8_t pin, sl_si91x_gpio_direction_t direction)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(direction));
  // Set direction(input/output) in UULP GPIO instance
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OEN = direction;
}

/*******************************************************************************
 * This API is used to get the UULP direction in NPSS GPIO control register.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Get the direction of the GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
*******************************************************************************/
uint8_t sl_si91x_gpio_get_uulp_npss_direction(uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  // Get direction(input/output) in UULP GPIO instance
  return UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OEN;
}

/*******************************************************************************
 * This API is used to select the UULP pin value in NPSS GPIO control register.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Select the GPIO pin value.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
void sl_si91x_gpio_set_uulp_npss_pin_value(uint8_t pin, sl_si91x_gpio_pin_value_t pin_value)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(pin_value));
  // Set or Clear pin in UULP GPIO instance by controlling pin value
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OUT = pin_value;
}

/*******************************************************************************
 * This API is used to toggle the UULP pin.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Toggle the UULP GPIO pin.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
void sl_si91x_gpio_toggle_uulp_npss_pin(uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  // Set or Clear pin in UULP GPIO instance by controlling pin value
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OUT ^= SET;
}

/*******************************************************************************
 * This API is used to get the UULP pin value in NPSS GPIO control register.
 * Few actions are required to be performed before setting the direction,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Select the GPIO pin value.
 *  - Get the GPIO pin value.
 * @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
 ******************************************************************************/
uint8_t sl_si91x_gpio_get_uulp_npss_pin(uint8_t pin)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  // Read pin status in UULP GPIO instance
  return (UULP_GPIO_STATUS >> pin) & MASK_INTR;
}

/*******************************************************************************
 * This API is used to select polarity of the UULP GPIO to be considered
      when used as a wakeup source from any of the Sleep States.
 ******************************************************************************/
void sl_si91x_gpio_select_uulp_npss_polarity(uint8_t pin, sl_si91x_gpio_polarity_t polarity)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_PIN(pin));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_PARAMETER(polarity));
  UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_POLARITY = polarity;
}

/*******************************************************************************
 * This API is used to set the UULP NPSS GPIO to wakeup interrupt
*******************************************************************************/
void sl_si91x_gpio_set_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt)
{
  UULP_GPIO_FSM->GPIO_WAKEUP_REGISTER |= (BIT(npssgpio_interrupt) << 1);
}

/*******************************************************************************
 * This API is used to clear the UULP NPSS GPIO to wakeup interrupt
*******************************************************************************/
void sl_si91x_gpio_clear_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt)
{
  UULP_GPIO_FSM->GPIO_WAKEUP_REGISTER |= (BIT(npssgpio_interrupt) << 1);
}

/*******************************************************************************
 * This API is used to mask the UULP NPSS GPIO interrupt.
 * Few actions are required to be performed before interrupt mask is performed,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select the .
 *  @note: All the UULP interrupts are masked by default.
*******************************************************************************/
void sl_si91x_gpio_mask_uulp_npss_interrupt(uint8_t npssgpio_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_INTR(npssgpio_interrupt));
  GPIO_NPSS_INTERRUPT_MASK_SET_REG |= (BIT(npssgpio_interrupt) << 1);
}

/*******************************************************************************
 * This API is used to un-mask the UULP NPSS GPIO interrupt.
 * Few actions are required to be performed before interrupt un-mask is performed,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Set UULP PAD configuration register.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Un-mask interrupt by setting corresponding bit in register.
 *  @note: All the UULP interrupts are masked by default.
*******************************************************************************/
void sl_si91x_gpio_unmask_uulp_npss_interrupt(uint8_t npssgpio_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_UULP_INTR(npssgpio_interrupt));
  GPIO_NPSS_INTERRUPT_MASK_CLR_REG |= (BIT(npssgpio_interrupt) << 1);
}

/*******************************************************************************
 * This API is used to clear the UULP interrupt.
*******************************************************************************/
void sl_si91x_gpio_clear_uulp_interrupt(uint8_t npssgpio_interrupt)
{
  GPIO_NPSS_INTERRUPT_CLEAR_REG |= (BIT(npssgpio_interrupt) << 1);
}

/*******************************************************************************
 * This API is used to get the UULP interrupt status.
 ******************************************************************************/
uint8_t sl_si91x_gpio_get_uulp_interrupt_status(void)
{
  return (GPIO_NPSS_INTERRUPT_STATUS_REG >> 1) & UULP_PIN_MASK;
}

/*******************************************************************************
 * This API is used to configure the pin interrupt in GPIO UULP instance.
 * There are total 5 pin interrupts in this instance.
 * To configure the interrupt, first UULP GPIO initialization must be done.
 * The actions to be performed in UULP GPIO initialization are:
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Set UULP PAD configuration register.
 *  - Select UULP NPSS receiver for UULP GPIO pin.
 *  - Set the mode of the GPIO pin.
 *  - Set the direction of the GPIO pin.
 *  - Configure the UULP pin interrupt.
 * Enable the IRQ handler.
*******************************************************************************/
void sl_si91x_gpio_configure_uulp_interrupt(sl_si91x_gpio_interrupt_config_flag_t flags, uint8_t npssgpio_interrupt)
{
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_FLAG(flags));
  SL_GPIO_ASSERT(SL_GPIO_VALIDATE_INTR(npssgpio_interrupt));
  //By default making all the interrupts zero.
  GPIO_NPSS_GPIO_CONFIG_REG = CLR;
  // Enable or disable interrupt rising edge in UULP GPIO instance
  if ((flags & SL_GPIO_INTERRUPT_RISE_EDGE) == SL_GPIO_INTERRUPT_RISE_EDGE) {
    GPIO_NPSS_GPIO_CONFIG_REG |= (npssgpio_interrupt << BIT_0);
  } else {
    GPIO_NPSS_GPIO_CONFIG_REG &= ~((uint32_t)npssgpio_interrupt << BIT_0);
  }
  // Enable or disable GPIO interrupt falling edge in UULP GPIO instance
  if ((flags & SL_GPIO_INTERRUPT_FALL_EDGE) == SL_GPIO_INTERRUPT_FALL_EDGE) {
    GPIO_NPSS_GPIO_CONFIG_REG |= (npssgpio_interrupt << BIT_8);
  } else {
    GPIO_NPSS_GPIO_CONFIG_REG &= ~((uint32_t)npssgpio_interrupt << BIT_8);
  }
  // Enable or disable interrupt level low in UULP GPIO instance
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_LOW) == SL_GPIO_INTERRUPT_LEVEL_LOW) {
    GPIO_NPSS_GPIO_CONFIG_REG |= (npssgpio_interrupt << BIT_16);
  } else {
    GPIO_NPSS_GPIO_CONFIG_REG &= ~((uint32_t)npssgpio_interrupt << BIT_16);
  }
  // Enable or disable interrupt level high in UULP GPIO instance
  if ((flags & SL_GPIO_INTERRUPT_LEVEL_HIGH) == SL_GPIO_INTERRUPT_LEVEL_HIGH) {
    GPIO_NPSS_GPIO_CONFIG_REG |= (npssgpio_interrupt << BIT_24);
  } else {
    GPIO_NPSS_GPIO_CONFIG_REG &= ~((uint32_t)npssgpio_interrupt << BIT_24);
  }

  NVIC_EnableIRQ(UULP_PININT_NVIC_NAME);
  NVIC_SetPriority(UULP_PININT_NVIC_NAME, PRIORITY_14);
}

/*******************************************************************************
 * This API is used to clear one (or) more pending ULP GPIO interrupts.
*******************************************************************************/
void sl_si91x_gpio_clear_ulp_interrupt(uint32_t flags)
{
  ULP_GPIO->INTR[flags].GPIO_INTR_STATUS_b.INTERRUPT_STATUS = SET;
}

/*******************************************************************************
 * This API is used to clear the ULP group interrupts.
*******************************************************************************/
void sl_si91x_gpio_clear_ulp_group_interrupt(sl_si91x_group_interrupt_t group_interrupt)
{
  ULP_GPIO->GPIO_GRP_INTR[group_interrupt].GPIO_GRP_INTR_STS_b.INTERRUPT_STATUS = SET;
}

/*******************************************************************************
 * This API is used to verify assumptions and print message if the assumption is false.
 ******************************************************************************/
void sl_assert_failed(uint8_t *file, uint32_t line)
{
  DEBUGOUT("Assert failed: file %s on line %lu \r\n", file, line);
}

/*******************************************************************************
 * This API is used to select the UULP PAD configuration register.
 * It has pad_config pointer of type @ref uulp_pad_config_t.
 * It selects the mode, direction, polarity, enables receiver etc in the register.
 * To set the UULP PAD configuration,
 *  - Enable the ULP clock using @ref sl_si91x_gpio_enable_clock() API.
 *  - Select UULP NPSS input buffer for UULP GPIO pin.
 *  @note: Select UULP GPIO pins for UULP instances(UULP_GPIO_0 to UULP_GPIO_4).
*******************************************************************************/
void sl_si91x_gpio_set_uulp_pad_configuration(uulp_pad_config_t *pad_config)
{
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_MODE =
    (sl_si91x_uulp_npss_mode_t)pad_config->mode;
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_REN =
    (sl_si91x_gpio_receiver_t)pad_config->receiver;
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_OEN =
    (sl_si91x_gpio_direction_t)pad_config->direction;
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_OUTPUT =
    (sl_si91x_gpio_pin_value_t)pad_config->output;
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_PAD_SELECT =
    (sl_si91x_gpio_uulp_pad_t)pad_config->pad_select;
  UULP_PAD_CONFIG_REG(pad_config->gpio_padnum)->UULP_GPIO_PAD_CONFIG_REG_b.GPIO_POLARITY =
    (sl_si91x_gpio_polarity_t)pad_config->polarity;
}

/*******************************************************************************
 * To get the release, sqa and dev version of gpio peripheral
 * It returns the structure for gpio version.
 * Structure includes three members:
 * - Release version
 * - Major version (SQA version)
 * - Minor version (Dev version)
 ******************************************************************************/
sl_si91x_gpio_version_t sl_si91x_gpio_get_version(void)
{
  sl_si91x_gpio_version_t version;
  version.release = GPIO_RELEASE_VERSION;
  version.major   = GPIO_MAJOR_VERSION;
  version.minor   = GPIO_MINOR_VERSION;
  return version;
}
