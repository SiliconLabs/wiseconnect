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
 * @ingroup SL_SI91X_DRIVER_APIS
 * @{
 *
 ******************************************************************************/
/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************/ /**
 * @brief   Set the direction for a GPIO pin.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @param[in]  direction  -  pin direction of type \ref sl_si91x_gpio_direction_t
 *                 '0' - Output\n
 *                 '1' - Input\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_pin_direction(uint8_t port, uint8_t pin, sl_si91x_gpio_direction_t direction);

/***************************************************************************/ /**
 * @brief       Get the direction GPIO.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_get_pin_direction() \n
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  pin  - The pin number on the port.
 *                HP instance has total 57 GPIO pins. Port 0, 1, 2 has 16 pins each.
 *                Port 3 has 9 pins.
 *                ULP instance has total 12 pins.
 * @return      Returns the direction of the pin.
 *                  '0' - Output\n
 *                  '1' - Input\n
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_pin_direction(uint8_t port, uint8_t pin);

/***************************************************************************/ /**
 * @brief      Enable the receiver bit in the PAD configuration register.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance \n
 * @param[in]  gpio_num - GPIO number to be use.
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief      Disable the receiver bit in the PAD configuration register.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_pad_selection(), for HP instance \n
 * @param[in]  gpio_num - GPIO number to be use.
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief   Select the pad(0 to 21).
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]   gpio_padnum - PAD number to be use
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_pad_selection(uint8_t gpio_padnum);

/***************************************************************************/ /**
 * @brief     Select drive strength of a GPIO pin.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver() \n
 * @param[in]    gpio_num - GPIO number to be use
 * @param[in]    strength    - Drive strength selector(E1,E2) of type
 *                  \ref sl_si91x_gpio_driver_strength_select_t
 *                    possible values are
 *                      0, for two_milli_amps   (E1=0,E2=0)\n
 *                      1, for four_milli_amps  (E1=0,E2=1)\n
 *                      2, for eight_milli_amps (E1=1,E2=0)\n
 *                      3, for twelve_milli_amps(E1=1,E2=1)\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_strength(uint8_t gpio_num,
                                                            sl_si91x_gpio_driver_strength_select_t strength);

/***************************************************************************/ /**
 * @brief    Select the Driver disabled state control.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver() \n
 * @param[in]    gpio_num     -  GPIO number to be use
 * @param[in]    disable_state    -  driver disable state of type
 *                  \ref sl_si91x_gpio_driver_disable_state_t
 *                 possible values are
 *                    0, for HiZ       (P1=0,P2=0)\n
 *                    1, for Pull-up   (P1=0,P2=1)\n
 *                    2, for Pull-down (P1=1,P2=0)\n
 *                    3, for Repeater  (P1=1,P2=1)\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_pad_driver_disable_state(uint8_t gpio_num,
                                                                 sl_si91x_gpio_driver_disable_state_t disable_state);

/***************************************************************************/ /**
 * @brief Select AND/OR of the group interrupt. If multiple interrupts
 *    on same port (or) different are to be generated, then use this API.
 *   Example: Consider port 0: pin 2,3 and port 3: pin 1,2 for interrupt generation.
 *      Choose OR, any of the selected pin is fine for group interrupt generation
 *      Choose AND, all the selected pins are necessary for group interrupt generation
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance.
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - Group interrupt number of type
 *                            \ref sl_si91x_group_interrupt_t
 * @param[in]  and_or  - AND/OR of GPIO group interrupts of type
 *                    \ref sl_si91x_gpio_and_or_t
 *                          '0'  - AND\n
 *                          '1'  - OR\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
*******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_and_or(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_and_or_t and_or);

/***************************************************************************/ /**
 * @brief        Clear the group interrupt status.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt() \n
 * @param[in]    group_interrupt - Group interrupt number of type
 *                        \ref sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_group_interrupt(sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Get the group interrupt status.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode(),
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]   port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]    group_interrupt - Group interrupt number of type
 *                        \ref sl_si91x_group_interrupt_t
 * @return       returns the group interrupt status register
 *                        1, when interrupt is enabled\n
 *                        0, when interrupt is disabled\n
 ******************************************************************************/
uint32_t sl_si91x_gpio_driver_get_group_interrupt_status(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Configure the group interrupt wake up the interrupt.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]    group_interrupt - Group interrupt number of type
 *                            \ref sl_si91x_group_interrupt_t
 * @param[in]    flags   - GPIO group interrupt wake up flag of type
 *                              \ref sl_si91x_gpio_wakeup_t
 *                        '1'  - enable\n
 *                        '0'  - disable\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_group_interrupt_wakeup(uint8_t port,
                                                               sl_si91x_group_interrupt_t group_interrupt,
                                                               sl_si91x_gpio_wakeup_t flags);

/***************************************************************************/ /**
 * @brief      Configure the MCU HP group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  configuration - configuration pointer to
 *                      \ref sl_si91x_gpio_group_interrupt_config_t structure
 * @param[in]  gpio_callback -  IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                           sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief      Get the polarity of group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
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
 *                      1, when GPIO pin status is '1'\n
 *                      0, when GPIO pin status is '0'\n
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                          uint8_t port,
                                                          uint8_t pin);

/***************************************************************************/ /**
 * @brief      Configure the polarity of group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
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
 *               1, group interrupt gets generated when GPIO pin status is '1'\n
 *               0, group interrupt gets generated when GPIO pin status is '0'\n
  * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_polarity(sl_si91x_group_interrupt_t group_interrupt,
                                                              uint8_t port,
                                                              uint8_t pin,
                                                              sl_si91x_gpio_polarity_t polarity);

/***************************************************************************/ /**
 * @brief      Get the level/edge event of group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode(), \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction(), \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @return     returns group interrupt level_edge
 *                     1, for Edge\n
 *                     0, for Level\n
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_group_interrupt_level_edge(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief      Set the level/edge event of group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt    - GPIO group interrupt number of type
 *                  \ref  sl_si91x_group_interrupt_t
 * @param[in]  level_edge - GPIO level edge group interrupt of type
 *                  \ref  sl_si91x_gpio_level_edge_t
 *                     1, for Edge\n
 *                     0, for Level\n
  * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_group_interrupt_level_edge(uint8_t port,
                                                                sl_si91x_group_interrupt_t group_interrupt,
                                                                sl_si91x_gpio_level_edge_t level_edge);

/***************************************************************************/ /**
 * @brief      Unmask the group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                    \ref  sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief      Mask the group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 * @param[in]  port - The port to associate with the pin.
 *                  HP instance - PORT 0,1,2,3
 *                  ULP instance - PORT 4
 * @param[in]  group_interrupt - GPIO group interrupt number of type
 *                      \ref sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_group_interrupt(uint8_t port, sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief        Disable HP/ULP GPIO clock.
 * @param[in]    clock  - Selects M4 clock or ULP clock of type
 *                    \ref  sl_si91x_gpio_select_clock_t
 *                         0, for M4 GPIO CLK\n
 *                         1, for ULP GPIO CLK\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_clock(sl_si91x_gpio_select_clock_t clock);

/***************************************************************************/ /**
 * @brief    Enable HP/ULP GPIO clock.
 * @param[in]    clock  - Selects M4 clock or ULP clock of type
 *                    \ref  sl_si91x_gpio_select_clock_t
 *                         0, for M4 GPIO CLK\n
 *                         1, for ULP GPIO CLK\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_clock(sl_si91x_gpio_select_clock_t clock);

/***************************************************************************/ /**
 * @brief      Configure the ULP group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]  configuration  - Pointer to the structure of type
 *                  \ref sl_si91x_gpio_group_interrupt_config_t
 * @param[in]  gpio_callback - IRQ function pointer.
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                               sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief   Enable the group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
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
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
*******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                        uint8_t port,
                                                        uint8_t pin);

/***************************************************************************/ /**
 * @brief      Disable the group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_clock, for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_pad_receiver(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver(), for ULP instance \n
 *  Use corresponding pad receiver API for corresponding GPIO instance. \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt(), for HP instance \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt, for ULP instance \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
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
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
*******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_group_interrupt(sl_si91x_group_interrupt_t group_interrupt,
                                                         uint8_t port,
                                                         uint8_t pin);

/***************************************************************************/ /**
 * @brief      Select the slew rate.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 * @param[in]  gpio_num - GPIO number to be use
 * @param[in]  slew_rate   -   slew rate of type \ref sl_si91x_gpio_slew_rate_t
 *                  '0' - Slow\n
 *                  '1' - Fast\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_slew_rate(uint8_t gpio_num, sl_si91x_gpio_slew_rate_t slew_rate);

/***************************************************************************/ /**
 * @brief        Select the drive strength.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 * @param[in]    gpio_padnum -  GPIO number to be use
 * @param[in]    strength    -  Drive strength selector(E1,E2) of type
 *                    \ref  sl_si91x_gpio_driver_strength_select_t
 *                    0, for two_milli_amps   (E1=0,E2=0)\n
 *                    1, for four_milli_amps  (E1=0,E2=1)\n
 *                    2, for eight_milli_amps (E1=1,E2=0)\n
 *                    3, for twelve_milli_amps(E1=1,E2=1)\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_strength(uint8_t gpio_num,
                                                                sl_si91x_gpio_driver_strength_select_t strength);

/***************************************************************************/ /**
 * @brief        Select the driver-disabled state control.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 * @param[in]    gpio_num    -  GPIO number to be use
 * @param[in]    disable_state  -  driver disable state of type
 *                    \ref sl_si91x_gpio_driver_disable_state_t
 *                  0, for HiZ       (P1=0,P2=0)\n
 *                  1, for Pull up   (P1=0,P2=1)\n
 *                  2, for Pull down (P1=1,P2=0)\n
 *                  3, for Repeater  (P1=1,P2=1)\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(
  uint8_t gpio_num,
  sl_si91x_gpio_driver_disable_state_t disable_state);

/***************************************************************************/ /**
 * @brief     Disable the receiver bit for ULP.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]    gpio_num  - GPIO number to be used
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_disable_ulp_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief        Enable the receiver bit for ULP.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]    gpio_num  - GPIO number to be used
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_enable_ulp_pad_receiver(uint8_t gpio_num);

/***************************************************************************/ /**
 * @brief      Configure the MCU ULP GPIO pin interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 * @param[in]  int_no - The interrupt number to trigger.
 * @param[in]  flags  - Interrupt configuration flags of type
 *                   \ref  sl_si91x_gpio_interrupt_config_flag_t
 * @param[in]  pin - GPIO pin number
 * @param[in]  gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_pin_interrupt(uint8_t int_no,
                                                             sl_si91x_gpio_interrupt_config_flag_t flags,
                                                             sl_si91x_gpio_pin_ulp_t pin,
                                                             sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief     Set the NPSS GPIO pin MUX(mode).
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 * @param[in]  pin  -  NPSS GPIO pin number(0...4) of type
 *                      \ref sl_si91x_uulp_npss_mode_t
 * @param[in]  mode  -  NPSS GPIO  MUX value
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_mux(uint8_t pin, sl_si91x_uulp_npss_mode_t mode);

/***************************************************************************/ /**
 * @brief     Enable/disable NPSS GPIO receiver.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]  pin    - is NPSS GPIO pin number (0...4)
 * @param[in]  receiver - is enable/disable NPSS GPIO receiver of type
 *                  \ref  sl_si91x_gpio_receiver_t
 *                 '1' - Enable\n
 *                 '0' - Disable\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_receiver(uint8_t pin, sl_si91x_gpio_receiver_t receiver);

/***************************************************************************/ /**
 * @brief     Set the direction of the NPSS GPIO.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 * @param[in]  pin  - is NPSS GPIO pin number (0...4)
 * @param[in]  direction  - is direction value (Input / Output) of type
 *                \ref  sl_si91x_gpio_direction_t
 *                  '1' - Input Direction\n
 *                  '0' - Output Direction\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_direction(uint8_t pin, sl_si91x_gpio_direction_t direction);

/***************************************************************************/ /**
 * @brief      Get the direction of the NPSS GPIO.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in]  pin - is NPSS GPIO pin number(0...4)
 * @return     returns the GPIO pin direction
 *                - 1, Input Direction
 *                - 0, Output Direction
 *******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_direction(uint8_t pin);

/***************************************************************************/ /**
 * @brief      Control the NPSS GPIO pin value.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in]  pin - is NPSS GPIO pin number (0...4) of type
 *                    \ref  sl_si91x_gpio_pin_value_t
 * @param[in]  pin_value - is NPSS GPIO pin value
 *                      '0' - Output\n
 *                      '1' - Input\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_pin_value(uint8_t pin, sl_si91x_gpio_pin_value_t pin_value);

/***************************************************************************/ /**
 * @brief     Get the NPSS GPIO pin value.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_value() \n
 * @param[in]  pin  - is NPSS GPIO pin number (0...4)
 * @return     returns the pin logical state of pin
 *                        '0' - Output\n
 *                        '1' - Input\n
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_npss_pin(uint8_t pin);

/***************************************************************************/ /**
 * @brief     Select the NPSS GPIO polarity.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in] pin   - is NPSS GPIO pin number (0...4)
 * @param[in] polarity - GPIO polarity
 *                 \ref sl_si91x_gpio_polarity_t
 *                 '1' - High\n
 *                 '0' - Low\n
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_select_uulp_npss_polarity(uint8_t pin, sl_si91x_gpio_polarity_t polarity);

/***************************************************************************/ /**
 * @brief       Set the UULP NPSS GPIO to wake up interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Clear the UULP NPSS GPIO to wake up interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_npss_wakeup_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Mask the NPSS GPIO interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_set_uulp_pad_configuration() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_mask_uulp_npss_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief       Unmask the NPSS GPIO interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_set_uulp_pad_configuration() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_unmask_uulp_npss_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief    Clear the NPSS GPIO interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_set_uulp_pad_configuration() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 *        \ref sl_si91x_gpio_configure_uulp_interrupt() \n
 * @param[in]   npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_uulp_interrupt(uint8_t npssgpio_interrupt);

/***************************************************************************/ /**
 * @brief     Get the NPSS GPIO interrupt status.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_set_uulp_pad_configuration() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 *        \ref sl_si91x_gpio_configure_uulp_interrupt() \n
 * @param[in]  None
 * @return    returns the UULP INTR status.
 *                   1, interrupt has been raised\n
 *                   0, interrupt is masked or not raised\n
 ******************************************************************************/
uint8_t sl_si91x_gpio_driver_get_uulp_interrupt_status(void);

/***************************************************************************/ /**
 * @brief     Clear one or more pending ULP GPIO interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_configure_ulp_pin_interrupt() \n
 * @param[in] flags : ULP GPIO interrupt sources to clear.
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_interrupt(uint32_t flags);

/***************************************************************************/ /**
 * @brief     Clear the ULP group interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 *        \ref sl_si91x_gpio_driver_configure_ulp_group_interrupt() \n
 *  Use corresponding group interrupt configuration API for corresponding GPIO instance.
 * @param[in]    group_interrupt - Group interrupt number of type
 *                     \ref   sl_si91x_group_interrupt_t
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_clear_ulp_group_interrupt(sl_si91x_group_interrupt_t group_interrupt);

/***************************************************************************/ /**
 * @brief     Configure the UULP GPIO pin interrupt.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_set_uulp_pad_configuration() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in] flags  -  Interrupt configuration flags of type
 *                     \ref  sl_si91x_gpio_interrupt_config_flag_t
 * @param[in] npssgpio_interrupt - OR'ed values of the NPSS GPIO interrupts
 * @param[in] gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_uulp_interrupt(sl_si91x_gpio_interrupt_config_flag_t flags,
                                                          uint8_t npssgpio_interrupt,
                                                          sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief      Configure ULP GPIO group interrupts.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_enable_ulp_pad_receiver() \n
 *        \ref sl_gpio_driver_set_pin_mode() \n
 *        \ref sl_si91x_gpio_driver_set_pin_direction() \n
 * @param[in]  configuration - configuration pointer to
 *                  \ref sl_si91x_gpio_group_interrupt_config_t structure
 * @param[in] gpio_callback - IRQ function pointer
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_configure_ulp_group_interrupt(sl_si91x_gpio_group_interrupt_config_t *configuration,
                                                               sl_gpio_irq_callback_t gpio_callback);

/***************************************************************************/ /**
 * @brief  Toggle the UULP pin.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 *        \ref sl_si91x_gpio_driver_select_uulp_npss_receiver() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_pin_mux() \n
 *        \ref sl_si91x_gpio_driver_set_uulp_npss_direction() \n
 * @param[in]   pin  -  UULP pin number to toggle
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_INVALID_PARAMETER (0x0021) - The parameter is invalid argument \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 ******************************************************************************/
sl_status_t sl_si91x_gpio_driver_toggle_uulp_npss_pin(uint8_t pin);

/***************************************************************************/ /**
 * @brief        Indicate UULP GPIO PAD configuration.
 * @pre   \ref sl_si91x_gpio_driver_enable_clock() \n
 * @param[in]    pad_config : PAD configuration pointer to \ref uulp_pad_config_t structure
 * @return returns status 0 if successful,
 *               else error code.
 *        \ref SL_STATUS_NULL_POINTER (0x0022) - The parameter is null pointer \n
 *        \ref SL_STATUS _OK (0X000)  - Success \n
 *******************************************************************************/
sl_status_t sl_si91x_gpio_driver_set_uulp_pad_configuration(uulp_pad_config_t *pad_config);

/***************************************************************************/ /**
* @brief    Get the release, SQA, and development version numbers of the GPIO peripheral.
* @param[in]    None
* @return       returns structure of type \ref sl_si91x_gpio_version_t
*******************************************************************************/
sl_si91x_gpio_version_t sl_si91x_gpio_driver_get_version(void);

/** @} (end addtogroup GPIO) */

#ifdef __cplusplus
}
#endif

#endif ///< GPIO_PRESENT
#endif ///< SL_SI91X_DRIVER_GPIO_H
