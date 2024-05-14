/***************************************************************************/ /**
 * @file sl_si91x_driver_gpio.h
 * @brief sl si91x driver gpio header file
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
#ifndef SL_SI91X_DRIVER_GPIO_H
#define SL_SI91X_DRIVER_GPIO_H

#if !defined(GPIO_PRESENT)
#include "sl_status.h"
#include "sl_driver_gpio.h"

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
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief   Set the direction for a GPIO pin.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @param[in]  direction  -  pin direction of type \ref sl_si91x_gpio_direction_t
 *  -              '0' - Output
 *  -              '1' - Input
 * 
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -     \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_pin_direction(uint8_t port, uint8_t pin, sl_si91x_gpio_direction_t direction);

/***************************************************************************/ /**
 * @brief       Get the direction of a selected GPIO pin.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance. 
 * -     \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -     \ref sl_si91x_gpio_driver_get_pin_direction() 
 * 
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @return      Returns the direction of the pin.
 * -                '0' - Output
 * -                '1' - Input
 *
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_pin_direction(uint8_t port, uint8_t pin);

/***************************************************************************/ /**
 * @brief      Enable the receiver enable bit in the PAD configuration register for reading the GPIO pin status.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 *
 * @param[in]  gpio_num - GPIO pin number to be use.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief      Disable the receiver enable bit in the PAD configuration register.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance 
 *
 * @param[in]  gpio_num - GPIO pin number to be use.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief    Enable the pad selection bit in the PAD selection register.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]   gpio_padnum - PAD number to be use
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_selection(uint8_t gpio_padnum);

/***************************************************************************/ /**
 * @brief     Select the drive strength for an HP instance GPIO pin.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver()
 * @param[in]    gpio_num - GPIO pin number to be use
 * @param[in]    strength    - Drive strength selector(E1,E2) of type
 *                  \ref sl_si91x_gpio_driver_strength_select_t
 *                    possible values are
 * -                   0, for two_milli_amps   (E1=0,E2=0)
 * -                   1, for four_milli_amps  (E1=0,E2=1)
 * -                   2, for eight_milli_amps (E1=1,E2=0)
 * -                   3, for twelve_milli_amps(E1=1,E2=1)
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_strength(uint8_t gpio_num,
                                                            sl_si91x_gpio_driver_strength_select_t strength);

/***************************************************************************/ /**
 * @brief    Select the Driver disabled state control for an HP instance GPIO pin.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver()
 * @param[in]    gpio_num     -  GPIO pin number to be use
 * @param[in]    disable_state    -  driver disable state of type
 *                  \ref sl_si91x_gpio_driver_disable_state_t
 *                 possible values are
 * 
 *  -                  0, for HiZ       (P1=0,P2=0)
 *  -                  1, for Pull-up   (P1=0,P2=1)
 *  -                  2, for Pull-down (P1=1,P2=0)
 *  -                  3, for Repeater  (P1=1,P2=1)
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *        \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_disable_state(uint8_t gpio_num,
                                                                 sl_si91x_gpio_driver_disable_state_t disable_state);

/***************************************************************************/ /**
 * @brief Select AND/OR type of the group interrupt. If multiple interrupts
 *    on same port (or) different are to be generated, then use this API.
 *   Example: Consider port 0: pin 2,3 and port 3: pin 1,2 for interrupt generation.
 *      Choose OR, any of the selected pin condition triggers the group interrupt generation
 *      Choose AND, all the selected pin conditions should match to trigger group interrupt generation
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance 
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode() 

 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance 
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - Group interrupt number of type
 *                            \ref sl_si91x_group_interrupt_t
 * @param[in]  and_or  - AND/OR of GPIO group interrupts of type
 *                    \ref sl_si91x_gpio_and_or_t
 * -                       '0'  - AND
 * -                        '1'  - OR
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_and_or(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_and_or_t and_or);

/***************************************************************************/ /**
 * @brief        Clear the selected group interrupt status value.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver()
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt()
 * @param[in]    group_interrupt - Group interrupt number of type
 *                        \ref sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_group_interrupt(sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Get the current status of the selected group interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode(),
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]   port - The port to associate with the pin.
 *                  HP instance  - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]    group_interrupt - Group interrupt number of type (0 to 1)
 *                        \ref sl_si91x_group_interrupt_t
 * @return       returns the group interrupt status register
 * -                      1, when interrupt is enabled
 * -                      0, when interrupt is disabled
 *
 ******************************************************************************/
uint32_t sl_si91x_gpio_driver_get_group_interrupt_status(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Configure the group interrupt as a wake up source across sleep wakeups.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]    group_interrupt - Group interrupt number of type
 *                            \ref sl_si91x_group_interrupt_t
 * @param[in]    flags   - GPIO group interrupt wake up flag of type
 *                              \ref sl_si91x_gpio_wakeup_t
 * -                      '1'  - enable
 * -                      '0'  - disable
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_wakeup(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_wakeup_t flags);

/***************************************************************************/ /**
 * @brief      Configure the MCU HP instance group interrupts with trigger type (level/edge), polarity (high/low), interrupt type (and/or)
 *             and register the callback function for interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver()
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * @param[in]  configuration - configuration pointer to
 *                      \ref sl_si91x_gpio_group_interrupt_config_t structure
 * @param[in]  gpio_callback -  IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                           sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief      Get the configured polarity of group interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                  \ref  sl_si91x_group_interrupt_t
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @return     returns group interrupt polarity
 *  -                    1, when GPIO pin status is HIGH
 *  -                    0, when GPIO pin status is LOW
 *
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                          uint8_t port,
                                                          uint8_t pin);

/***************************************************************************/ /**
 * @brief      Configure the polarity to a selected group interrupt which decides the active value of the pin to be 
 *             considered for group interrupt generation.
 *             '0' – group interrupt gets generated when gpio input pin status is LOW
 *             '1' – group interrupt gets generated when gpio input pin status is HIGH
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                  \ref  sl_si91x_group_interrupt_t
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @param[in]  polarity   -  polarity of GPIO group interrupt of type
 *                      \ref  sl_si91x_gpio_polarity_t
 * -             1, group interrupt gets generated when GPIO pin status is '1'
 * -             0, group interrupt gets generated when GPIO pin status is '0'
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                              uint8_t port,
                                                              uint8_t pin,
                                                              sl_si91x_gpio_polarity_t polarity);

/***************************************************************************/ /**
 * @brief      Get the configured level/edge event for the selected group interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode(),
 * -   \ref sl_si91x_gpio_driver_set_pin_direction(),
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @return     returns group interrupt level_edge
 * -                    1, for Edge
 * -                    0, for Level
 *
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_level_edge(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief       Set the level/edge event for the selected group interrupt.
 *             '1' Triggers interrupt generation when configured edge is detected on pin
 *             '0' Triggers interrupt generation when configured level is detected on pin
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *  -                 HP instance - PORT 0,1,2,3
 *  -                 ULP instance - PORT 4
 * @param[in]  group_interrupt    - GPIO group interrupt number of type
 *                  \ref  sl_si91x_group_interrupt_t
 * @param[in]  level_edge - GPIO level edge group interrupt of type
 *                  \ref  sl_si91x_gpio_level_edge_t
 *  -                 1, for Edge
 *  -                 0, for Level
 *
  * @return returns status 0 if successful,
 *               else error code as follow.
 *  -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_level_edge(uint8_t port,
                                                                sl_si91x_group_interrupt_t group_interrupt,
                                                                sl_si91x_gpio_level_edge_t level_edge);

/***************************************************************************/ /**
 * @brief      Unmask the selected group interrupt to enable interrupt clearing upon generation.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance  - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief      Mask the selected group interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * @param[in]  port - The port to associate with the pin.
 *  -                HP instance - PORT 0,1,2,3
 *  -                ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                      \ref sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief        Disable the clock for either the HP or ULP instance of the GPIO Peripheral.
 * @param[in]    clock  - Selects M4 clock or ULP clock of type
 *                    \ref  sl_si91x_gpio_select_clock_t
 * -                       0, for M4 GPIO CLK
 * -                       1, for ULP GPIO CLK
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_clock(sl_si91x_gpio_select_clock_t clock);

/***************************************************************************/ /**
 * @brief    Enable the clock for either the HP or ULP instance of the GPIO peripheral.
 * @param[in]    clock  - Selects M4 clock or ULP clock of type
 *                    \ref  sl_si91x_gpio_select_clock_t
 * -                       0, for M4 GPIO CLK
 * -                       1, for ULP GPIO CLK
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_clock(sl_si91x_gpio_select_clock_t clock);

/***************************************************************************/ /**
 * @brief   Enable the selected group interrupts for either the HP or ULP instance of the GPIO peripheral.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance
 *  Use the corresponding pad receiver API for the corresponding GPIO instance.
 * -   \ref sl_gpio_driver_set_pin_mode()
 * -   \ref sl_si91x_gpio_driver_set_pin_direction()
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  group_interrupt  - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @param[in]  port - The port to associate with the pin.
 *  -                HP instance - PORT 0,1,2,3
 *  -                ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -    \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -    \ref SL_STATUS _OK (0X000)  - Success 
 *
*******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                        uint8_t port,
                                                        uint8_t pin);

/***************************************************************************/ /**
 * @brief      Disable the selected group interrupts for either the HP or ULP instance of the GPIO peripheral.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_clock, for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance 
 *  Use the corresponding pad receiver API for the corresponding GPIO instance. 
 *
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance 
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance 
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]  group_interrupt  - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -      \ref SL_STATUS _OK (0X000)  - Success 
 *
*******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                         uint8_t port,
                                                         uint8_t pin);

/***************************************************************************/ /**
 * @brief      Select the slew rate for the ULP instance of the GPIO peripheral.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 *
 * @param[in]  gpio_num - GPIO pin number to be use
 * @param[in]  slew_rate   -   slew rate of type \ref sl_si91x_gpio_slew_rate_t
 * -               '0' - Slow
 * -               '1' - Fast
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_slew_rate(uint8_t gpio_num, sl_si91x_gpio_slew_rate_t slew_rate);

/***************************************************************************/ /**
 * @brief         Select the drive strength for the ULP instance of the GPIO peripheral.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 *
 * @param[in]    gpio_num -  GPIO pin number to be use
 * @param[in]    strength    -  Drive strength selector(E1,E2) of type
 *                    \ref  sl_si91x_gpio_driver_strength_select_t
 *  -                  0, for two_milli_amps   (E1=0,E2=0)
 *  -                  1, for four_milli_amps  (E1=0,E2=1)
 *  -                  2, for eight_milli_amps (E1=1,E2=0)
 *  -                  3, for twelve_milli_amps(E1=1,E2=1)
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_strength(uint8_t gpio_num,
                                                                sl_si91x_gpio_driver_strength_select_t strength);

/***************************************************************************/ /**
 * @brief        Select the driver-disabled state control for the ULP instance of the GPIO peripheral.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 *
 * @param[in]    gpio_num    -  GPIO pin number to be use
 * @param[in]    disable_state  -  driver disable state of type
 *                    \ref sl_si91x_gpio_driver_disable_state_t
 * -                0, for HiZ       (P1=0,P2=0)
 * -                1, for Pull up   (P1=0,P2=1)
 * -                2, for Pull down (P1=1,P2=0)
 * -                3, for Repeater  (P1=1,P2=1)
 * 
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(
  uint8_t gpio_num,
  sl_si91x_gpio_driver_disable_state_t disable_state);

/***************************************************************************/ /**
 * @brief     Disable the receiver enable bit for the ULP instance of the GPIO peripheral.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]    gpio_num  - GPIO pin number to be used
 * @return returns status 0 if successful,
 *               else error code as follow.
 *  -     \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 *  -     \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_ulp_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief        Enable the receiver enable bit for the ULP instance of the GPIO peripheral.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]    gpio_num  - GPIO pin number to be used
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_ulp_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief      Configure the MCU ULP instance pin interrupts with the trigger type (level/edge)
 *             and register the callback function for interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * @param[in]  int_no - The interrupt number to trigger.
 * @param[in]  flags  - Interrupt configuration flags of type
 *                   \ref  sl_si91x_gpio_interrupt_config_flag_t
 * @param[in]  pin - GPIO pin number (0 to 11)
 * @param[in]  gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_pin_interrupt(uint8_t int_no,
                                                             sl_si91x_gpio_interrupt_config_flag_t flags,
                                                             sl_si91x_gpio_pin_ulp_t pin,
                                                             sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief     Set the NPSS GPIO pin MUX (mode) to the selected mode.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 *
 * @param[in]  pin  -  NPSS GPIO pin number(0...4) of type
 *                      \ref sl_si91x_uulp_npss_mode_t
 * @param[in]  mode  -  NPSS GPIO  MUX value (0 to 10)
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_mux(uint8_t pin, sl_si91x_uulp_npss_mode_t mode);

/***************************************************************************/ /**
 * @brief     Enable/disable the NPSS GPIO Input Buffer.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]  pin    - is NPSS GPIO pin number (0...4)
 * @param[in]  receiver - is enable/disable NPSS GPIO receiver of type
 *                  \ref  sl_si91x_gpio_receiver_t
 * -               '1' - Enable
 * -               '0' - Disable
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_receiver(uint8_t pin, sl_si91x_gpio_receiver_t receiver);

/***************************************************************************/ /**
 * @brief     Set the direction for the selected NPSS GPIO.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * @param[in]  pin  - is NPSS GPIO pin number (0...4)
 * @param[in]  direction  - is direction value (Input / Output) of type
 *                \ref  sl_si91x_gpio_direction_t
 * -                 '1' - Input Direction
 * -                 '0' - Output Direction
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 * 
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_direction(uint8_t pin, sl_si91x_gpio_direction_t direction);

/***************************************************************************/ /**
 * @brief      Get the direction of the selected NPSS GPIO.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * 
 * @param[in]  pin - is NPSS GPIO pin number(0...4)
 * @return     returns the GPIO pin direction
 *  -              1, Input Direction 
 *  -              0, Output Direction 
 *******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_direction(uint8_t pin);

/***************************************************************************/ /**
 * @brief      Control(set or clear) the NPSS GPIO pin value.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * @param[in]  pin - is NPSS GPIO pin number (0...4) of type
 *                    \ref  sl_si91x_gpio_pin_value_t
 * @param[in]  pin_value - is NPSS GPIO pin value
 * -                     '0' - Output
 * -                     '1' - Input
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_value(uint8_t pin, sl_si91x_gpio_pin_value_t pin_value);

/***************************************************************************/ /**
 * @brief     Read the status of the selected NPSS GPIO pin value.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_value() 
 * @param[in]  pin  - is NPSS GPIO pin number (0...4)
 * @return     returns the pin logical state of pin
 * -                       '0' - Output
 * -                       '1' - Input
 *
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_pin(uint8_t pin);

/***************************************************************************/ /**
 * @brief     Select the NPSS GPIO polarity for generating the interrupt.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in] pin   - is NPSS GPIO pin number (0...4)
 * @param[in] polarity - GPIO polarity
 *                 \ref sl_si91x_gpio_polarity_t
 * -              '1' - High
 * -              '0' - Low
 *
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_polarity(uint8_t pin, sl_si91x_gpio_polarity_t polarity);

/***************************************************************************/ /**
 * @brief       Set the NPSS GPIO interrupt as a wake up source across sleep wakeups.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Clear the UULP NPSS GPIO Interrupt as a wake up source.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Mask the selected NPSS GPIO interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_set_uulp_pad_configuration()
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_uulp_npss_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Unmask the selected NPSS GPIO interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_set_uulp_pad_configuration() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -      \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -      \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_uulp_npss_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief    Clear the selected NPSS GPIO interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_set_uulp_pad_configuration()
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction()
 * -   \ref sl_si91x_gpio_configure_uulp_interrupt() 
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief     Get the current status of all the NPSS GPIO interrupt status.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock()
 * -   \ref sl_si91x_gpio_set_uulp_pad_configuration()
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * -   \ref sl_si91x_gpio_configure_uulp_interrupt() 
 *
 * @param  none
 * @return    returns the UULP INTR status.
 * -                  1, interrupt has been raised
 * -                  0, interrupt is masked or not raised
 *
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_interrupt_status(void);

/***************************************************************************/ /**
 * @brief     Get the selected ULP instance GPIO pin interrupt status.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_configure_ulp_pin_interrupt() 
 * @param[in]  flags : ULP GPIO interrupt sources status.
 * @return    returns the ULP INTR status.
 *                   1, interrupt has been raised
 *                   0, interrupt is masked or not raised
 ******************************************************************************/
uint32_t sl_si91x_gpio_driver_get_ulp_interrupt_status(uint32_t flags);

/***************************************************************************/ /**
 * @brief     Clear the selected ULP instance GPIO pin interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_configure_ulp_pin_interrupt() 
 * @param[in] flags : ULP GPIO interrupt sources to clear.
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_interrupt(uint32_t flags);

/***************************************************************************/ /**
 * @brief     Clear the selected ULP instance group interrupt.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * -   \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt() 
 *
 *  Use the corresponding group interrupt configuration API for the corresponding GPIO instance.
 * @param[in]    group_interrupt - Group interrupt number of type
 *                     \ref   sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_group_interrupt(sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Configure the UULP GPIO pin interrupt with interrupt type level or edge and registers callback function for interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_set_uulp_pad_configuration() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * @param[in] flags  -  Interrupt configuration flags of type
 *                     \ref  sl_si91x_gpio_interrupt_config_flag_t
 * @param[in] npssgpio_interrupt - NPSS GPIO input number (0 to 4) 
 * @param[in] gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_uulp_interrupt(sl_si91x_gpio_interrupt_config_flag_t flags,
                                                          uint8_t npssgpio_interrupt,
                                                          sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief      Configure the MCU ULP instance group interrupts with trigger type (level/edge), polarity (high/low), interrupt type (and/or)
 *             and register the callback function for interrupts.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() 
 * -   \ref sl_gpio_driver_set_pin_mode() 
 * -   \ref sl_si91x_gpio_driver_set_pin_direction() 
 * @param[in]  configuration - configuration pointer to
 *                  \ref sl_si91x_gpio_group_interrupt_config_t structure
 * @param[in] gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                               sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief  Toggle the selected UULP pin status.
 * @pre Pre-conditions:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * -   \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() 
 * -   \ref sl_si91x_gpio_driver_set_uulp_npss_direction() 
 * @param[in]   pin  -  UULP pin number to toggle
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is an invalid argument 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_toggle_uulp_npss_pin(uint8_t pin);

/***************************************************************************/ /**
 * @brief        Configure the UULP GPIO pin mode, receiver enable, direction and polarity settings.
 * @pre Pre-condition:
 * -   \ref sl_si91x_gpio_driver_enable_clock() 
 * @param[in]    pad_config : PAD configuration pointer to \ref uulp_pad_config_t structure
 * @return returns status 0 if successful,
 *               else error code as follow.
 * -       \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is a null pointer 
 * -       \ref SL_STATUS _OK (0X000)  - Success 
 *
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_pad_configuration(uulp_pad_config_t *pad_config);

/***************************************************************************/ /**
* @brief    Get the release, SQA, and development version numbers of the GPIO peripheral.
* @param    none
* @return       returns structure of type \ref sl_si91x_gpio_version_t
*******************************************************************************/
sl_si91x_gpio_version_t sl_si91x_gpio_driver_get_version(void);

/** @} (end addtogroup GPIO) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup GPIO General-Purpose Input-Output
/// @{
///
///   @details
///
///   @n @section GPIO_Intro Introduction
///   The GPIO (General Purpose Input/Output) driver provides a comprehensive set of functions to
///   configure and control the GPIO pins. The GPIO pins can be used for various purposes such as reading
///   digital signals, controlling external devices, generating interrupts, etc. The GPIO driver simplifies the process of interacting
///   with GPIO pins, making it easier to integrate GPIO functionality into the application.
///
///   @n @section GPIO_Config Configuration
///
///   @li To configure the GPIO driver, identify the GPIO instance and pins you want to configure. Set the direction of each pin
///   as either input or output. Then, if needed, set the drive strength for the pins to ensure they can
///   handle the required current. Set the GPIO mux and pad selection if needed. Configure any interrupt settings, such as trigger type (level or edge) and
///   interrupt polarity (high or low). If interrupts are used, register the appropriate callback functions to handle them.
///
///   @li Additionally, make sure to enable the GPIO clock for the selected instance to ensure the GPIO peripheral operates correctly.
///
///   @li These steps are necessary for the GPIO pins to function correctly and trigger interrupts as required by the application.
///   The respective APIs for the same functionality will change depending on the GPIO in use i.e. Ultra Ultra Low Power (UULP), Ultra Low Power (ULP), or High Power (HP).
///
///   @n @section GPIO_Usage Usage
///
///   @li For UULP GPIOs, common functions include:
///
///      1.    @ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux
///      2.    @ref sl_si91x_gpio_driver_select_uulp_npss_receiver
///      3.    @ref sl_si91x_gpio_driver_set_uulp_npss_direction
///      4.    @ref sl_si91x_gpio_driver_set_uulp_npss_pin_value
///      5.    @ref sl_si91x_gpio_driver_get_uulp_npss_pin
///
///   @li To set the pin MUX mode,  @ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux can be used.
///   For enabling or disabling the input buffer, use @ref sl_si91x_gpio_driver_select_uulp_npss_receiver. Additionally,
///   set pin direction with @ref sl_si91x_gpio_driver_set_uulp_npss_direction. Also, use @ref sl_si91x_gpio_driver_set_uulp_npss_pin_value to set the pin value.
///
///   @li For ULP GPIOs, common functions include:
///
///      1.    @ref sl_si91x_gpio_driver_select_ulp_pad_slew_rate
///      2.    @ref sl_si91x_gpio_driver_select_ulp_pad_driver_strength
///      3.    @ref sl_si91x_gpio_driver_disable_ulp_pad_receiver
///      4.    @ref sl_si91x_gpio_driver_enable_ulp_pad_receiver
///      5.    @ref sl_si91x_gpio_driver_configure_ulp_pin_interrupt
///
///   @li With @ref sl_si91x_gpio_driver_select_ulp_pad_slew_rate, set the slew rate for ULP GPIO pins. With @ref sl_si91x_gpio_driver_select_ulp_pad_driver_strength
///   choose the drive strength. To manage the receiver, @ref sl_si91x_gpio_driver_enable_ulp_pad_receiver and for disabling and enabling the receiver
///   @ref sl_si91x_gpio_driver_enable_ulp_pad_receiver can be used, respectively. For configuring interrupts, @ref sl_si91x_gpio_driver_configure_ulp_pin_interrupt
///   can be used to set up pin interrupts with defined trigger types (level/edge).
///
///   @li For HP GPIOs, common functions include:
///
///      1.    @ref sl_si91x_gpio_driver_set_pin_direction
///      2.    @ref sl_si91x_gpio_driver_enable_pad_receiver
///      3.    @ref sl_si91x_gpio_driver_enable_pad_selection
///      4.    @ref sl_si91x_gpio_driver_configure_group_interrupt
///      5.    @ref sl_si91x_gpio_driver_enable_clock
///
///   @li To configure a GPIO pin, first, use @ref sl_si91x_gpio_driver_set_pin_direction to set its direction.
///   To enable the receiver for reading the pin status, use @ref sl_si91x_gpio_driver_enable_pad_receiver.
///   For selecting the pad, use @ref sl_si91x_gpio_driver_enable_pad_selection. To configure group interrupts for the MCU HP instance,
///   use @ref sl_si91x_gpio_driver_configure_group_interrupt. Make sure to enable the clock for HP or ULP instances,
///   using @ref sl_si91x_gpio_driver_enable_clock.
///
///   @li Other APIs can be referred to in the API Documentation.
///
/// @} end group GPIO ********************************************************/

#ifdef __cplusplus
}
#endif

#endif ///< GPIO_PRESENT
#endif ///< SL_SI91X_DRIVER_GPIO_H
