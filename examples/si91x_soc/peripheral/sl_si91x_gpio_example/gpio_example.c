/*******************************************************************************
 * @file  gpio_example.c
 * @brief GPIO example
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
/**============================================================================
 * @brief : This file contains example application for GPIO example
 * @section Description :
 * This application demonstrates the HP pin interrupt and toggling of HP GPIO pin.
 ============================================================================**/
#include "gpio_example.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_gpio_board.h"
#include "rsi_debug.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define AVL_INTR_NO                       0 // available interrupt number
#define INT_CH                            0 // GPIO Pin interrupt 0
#define ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE 0 // Set to 1 to enable HP GPIO 66 toggle via ULP GPIO 2
/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
// Define GPIO port and pin for HP instance. Here SL_SI91X_GPIO_6_PORT corresponds to GPIO pin 6's port number : 0.
// This is defined to '0' which is PORT_A and SL_SI91X_GPIO_6_PIN refers to GPIO pin number 6.
// Same representation followed for other port and pins.
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config = { { SL_SI91X_GPIO_6_PORT, SL_SI91X_GPIO_6_PIN }, GPIO_OUTPUT };
// Define GPIO port and pin for HP instance. Here SL_SI91X_GPIO_11_PORT corresponds to GPIO pin 11's port number : 0.
// This is defined to '0' which is PORT_A and SL_SI91X_GPIO_11_PIN refers to GPIO pin number 11.
// Same representation followed for other port and pins.
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config1 = { { SL_SI91X_GPIO_11_PORT, SL_SI91X_GPIO_11_PIN }, GPIO_INPUT };
#if (ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE == 1)
// Define ULP GPIO pin 2 configuration for output.
static sl_si91x_gpio_pin_config_t sl_gpio_ulp_pin_config = { { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN },
                                                             GPIO_OUTPUT };
// Define HP GPIO pin 66 (SOC peripheral mapped from ULP GPIO 2) configuration for output.
static sl_si91x_gpio_pin_config_t sl_gpio_hp_pin_config = { { SL_SI91X_HP_GPIO_66_PORT, SL_SI91X_HP_GPIO_66_PIN },
                                                            GPIO_OUTPUT };
#endif
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void gpio_pin_interrupt0_callback(uint32_t pin_intr);
#if (ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE == 1)
static sl_status_t gpio_configure_soc_peri_on_ulp_pin(void);
#endif
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * GPIO example initialization function. It initializes clock, pin mode,
 * direction and configures pin interrupt
 ******************************************************************************/
void gpio_example_init(void)
{
  sl_status_t status;

  do {
    // Initialize the GPIOs by clearing all interrupts initially
    status = sl_gpio_driver_init();
    if (status != SL_STATUS_OK) {
      // Prints GPIO initialization fails
      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      SL_PRINT_STRING_ERROR("sl_gpio_driver_init, Error code: %lu\r\n", status);
      break; // breaks if error occurs
    }
    SL_PRINT_STRING_ERROR("GPIO driver initialization is successful \r\n");
#if (ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE == 1)
    // Configure SOC peripheral on ULP pin to map HP GPIO to ULP GPIO
    status = gpio_configure_soc_peri_on_ulp_pin();
    if (status != SL_STATUS_OK) {
      break;
    }
#endif
    // Configure GPIO pin 6 using pin configuration API.
    // Using this API by default GPIO mode is set as MODE 0. If any other mode is selected for any GPIO use
    // corresponding API sl_gpio_driver_set_pin_mode() is for mode setting.
    status = sl_gpio_set_configuration(sl_gpio_pin_config);
    if (status != SL_STATUS_OK) {
      // Prints if pin configuration fails
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
      break; // breaks if error occurs
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin configuration is successful \r\n");
    // Configure GPIO pin 11 using pin configuration API.
    // Using this API by default GPIO mode is set as MODE 0. If any other mode is selected for any GPIO use
    // corresponding API sl_gpio_driver_set_pin_mode() is for mode setting.
    status = sl_gpio_set_configuration(sl_gpio_pin_config1);
    if (status != SL_STATUS_OK) {
      // Prints if pin configuration fails
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
      break; // breaks if error occurs
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin configuration is successful \r\n");
    // Configure pin interrupt for GPIO pin. The pin interrupt in this application is performed using external triggering from button.
    // Press button1 for triggering HP GPIO instance pin interrupt
    status = sl_gpio_driver_configure_interrupt(&sl_gpio_pin_config1.port_pin,
                                                INT_CH,
                                                (sl_gpio_interrupt_flag_t)SL_GPIO_INTERRUPT_RISE_EDGE,
                                                (sl_gpio_irq_callback_t)&gpio_pin_interrupt0_callback,
                                                AVL_INTR_NO);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_configure_pin_interrupt, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver interrupt configure is successful \r\n");
  } while (false);
}
/*******************************************************************************
 * Function will run continuously in while loop and toggles GPIO pin 6
 ******************************************************************************/
void gpio_example_process_action(void)
{
  sl_status_t status;
  status = sl_gpio_driver_toggle_pin(&sl_gpio_pin_config.port_pin); // Toggle HP GPIO pin 6
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_toggle_pin, Error code: %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("HP GPIO pin %d driver toggle pin is successful \r\n", sl_gpio_pin_config.port_pin.pin);
  }
#if (ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE == 1)
  // Toggle HP GPIO pin mapped via SOC peripheral on ULP pin
  status = sl_gpio_driver_toggle_pin(&sl_gpio_hp_pin_config.port_pin);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_toggle_pin, Error code: %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("HP GPIO pin %d driver toggle pin is successful \r\n", sl_gpio_hp_pin_config.port_pin.pin);
  }
#endif
}

#if (ENABLE_SOC_PERI_ON_ULP_PIN_TOGGLE == 1)
/*******************************************************************************
 * Configure SOC peripheral on ULP GPIO pin for HP GPIO toggle
 ******************************************************************************/
static sl_status_t gpio_configure_soc_peri_on_ulp_pin(void)
{
  sl_status_t status;

  status = sl_gpio_set_configuration(sl_gpio_ulp_pin_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
    return status;
  }
  status = sl_si91x_gpio_driver_set_soc_peri_on_ulp_pin_mode(&sl_gpio_ulp_pin_config.port_pin, SL_GPIO_MODE_0);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_soc_peri_on_ulp_pin_mode, "
                          "Error code: %lu\r\n",
                          status);
    return status;
  }
  status = sl_gpio_set_configuration(sl_gpio_hp_pin_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("HP GPIO pin %d is mapped to ULP GPIO %d successfully \r\n",
                        sl_gpio_hp_pin_config.port_pin.pin,
                        sl_gpio_ulp_pin_config.port_pin.pin);
  return SL_STATUS_OK;
}
#endif

/*******************************************************************************
 *   This API handles GPIO pin interrupt 0 request
 ******************************************************************************/
static void gpio_pin_interrupt0_callback(uint32_t pin_intr)
{
  if (pin_intr == PIN_INTR_0) {
    // This is with respect to ISR context. Debugout might cause issues sometimes.
    SL_PRINT_STRING_ERROR("gpio pin interrupt0\r\n");
  }
}
