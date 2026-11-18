/*******************************************************************************
 * @file  gpio_ulp_example.c
 * @brief GPIO ULP example
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
 * This application demonstrates the toggling of ULP instance GPIO at intervals of 
 * 1 second and configures ULP pin interrupt.
 ============================================================================**/
#include "gpio_ulp_example.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_gpio_board.h"
#include "rsi_debug.h"
#include "sl_si91x_clock_manager.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DELAY                             1000 // Delay for 1sec
#define ULP_INT_CH                        0    // ULP GPIO Pin interrupt 0
#define AVL_INTR_NO                       0    // available interrupt number
#define ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE 0    // Set to 1 to enable ULP peripheral on SOC GPIO 7 and toggle ULP GPIO 1
/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
typedef sl_gpio_t sl_si91x_gpio_t;
// Define a configuration structure for GPIO pins that are required, specifying its port and pin number.
// Below configurations are for ULP GPIO 2. Here SL_SI91X_ULP_GPIO_2_PORT corresponds to ULP GPIO pin 2's port number : 4.
// This is defined to '4' which is ULP_PORT and SL_SI91X_ULP_GPIO_2_PIN refers to ULP GPIO pin number 2.
// Same representation followed for other port and pins.
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config = { { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN },
                                                         GPIO_OUTPUT };

// Define GPIO port and pin for ULP instance. Here SL_SI91X_ULP_GPIO_8_PORT / SL_SI91X_ULP_GPIO_4_PORT refers to ULP GPIO pin 8's port number : 4.
// This is defined to '4' which is ULP_PORT and SL_SI91X_ULP_GPIO_8_PIN refers to GPIO pin number 8 / SL_SI91X_ULP_GPIO_4_PIN refers to GPIO pin number 4.
// Same representation followed for other port and pins.
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config1 = { { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN },
                                                          GPIO_INPUT };
#if (ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE == 1)
// Define HP GPIO pin 7 configuration for output.
static sl_si91x_gpio_pin_config_t sl_gpio_hp_pin_config = { { SL_GPIO_PORT_A, SL_SI91X_GPIO_7_PIN }, GPIO_OUTPUT };
// Define ULP GPIO pin 1 configuration for output.
static sl_si91x_gpio_pin_config_t sl_gpio_ulp_pin_config = { { SL_SI91X_ULP_GPIO_1_PORT, SL_SI91X_ULP_GPIO_1_PIN },
                                                             GPIO_OUTPUT };
#endif
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void gpio_ulp_pin_interrupt_callback(uint32_t pin_intr);
#if (ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE == 1)
static sl_status_t gpio_configure_ulp_peri_on_soc_pin(void);
#endif
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * GPIO example initialization function. It initializes clock, pin mode,
 * direction and configures ULP pin interrupt
 ******************************************************************************/
void gpio_ulp_example_init(void)
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
#if (ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE == 1)
    // Configure ULP peripheral on SOC pin to map ULP GPIO to HP GPIO
    status = gpio_configure_ulp_peri_on_soc_pin();
    if (status != SL_STATUS_OK) {
      break;
    }
#endif
    // Configure ULP GPIO pin 2 using driver pin configuration API.
    // Using this API by default GPIO mode is set as MODE 0. If any other mode is selected for any GPIO use
    // corresponding API sl_gpio_driver_set_pin_mode() is for mode setting.
    status = sl_gpio_set_configuration(sl_gpio_pin_config);
    if (status != SL_STATUS_OK) {
      // Prints if pin configuration fails
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
      break; // breaks if error occurs
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin configuration is successful \r\n");
    // Configure ULP GPIO pin 8 using driver pin configuration API.
    // Using this API by default GPIO mode is set as MODE 0. If any other mode is selected for any GPIO use
    // corresponding API sl_gpio_driver_set_pin_mode() is for mode setting.
    status = sl_gpio_set_configuration(sl_gpio_pin_config1);
    if (status != SL_STATUS_OK) {
      // Prints if pin configuration fails
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
      break; // breaks if error occurs
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin configuration is successful \r\n");
    // Configure ULP GPIO pin interrupts. The pin interrupt in this application is performed using external triggering from button.
    // Connect ULP GPIO8(P15) to low/high for triggering ULP GPIO instance pin interrupt based on event selected.
    status = sl_gpio_driver_configure_interrupt(&sl_gpio_pin_config1.port_pin,
                                                ULP_INT_CH,
                                                (sl_gpio_interrupt_flag_t)SL_GPIO_INTERRUPT_FALL_EDGE,
                                                (sl_gpio_irq_callback_t)&gpio_ulp_pin_interrupt_callback,
                                                AVL_INTR_NO);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_configure_pin_interrupt, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver configure ulp pin interrupt is successful \r\n");
  } while (false);
}
/*******************************************************************************
 * Function will run continuously in while loop and led toggles for every 1sec
 ******************************************************************************/
void gpio_ulp_example_process_action(void)
{
  sl_status_t status;
  // Set ULP GPIO pin high
  status = sl_gpio_driver_set_pin(&sl_gpio_pin_config.port_pin);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_set_pin, Error code: %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("ULP GPIO driver set pin is successful \r\n");
  }
  sl_si91x_delay_ms(DELAY);
  // Clear ULP GPIO pin (set low)
  status = sl_gpio_driver_clear_pin(&sl_gpio_pin_config.port_pin);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_clear_pin, Error code: %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("ULP GPIO driver clear pin is successful \r\n");
  }
  sl_si91x_delay_ms(DELAY);
#if (ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE == 1)
  // Toggle ULP GPIO pin mapped to SOC (HP) GPIO
  status = sl_gpio_driver_toggle_pin(&sl_gpio_ulp_pin_config.port_pin);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_toggle_pin, Error code: %lu\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("ULP GPIO pin %d driver toggle pin is successful \r\n", sl_gpio_ulp_pin_config.port_pin.pin);
  }
#endif
}

#if (ENABLE_ULP_PERI_ON_SOC_PIN_TOGGLE == 1)
/*******************************************************************************
 * Configure ULP peripheral on SOC GPIO pin for ULP GPIO toggle
 ******************************************************************************/
static sl_status_t gpio_configure_ulp_peri_on_soc_pin(void)
{
  sl_status_t status;

  status = sl_gpio_set_configuration(sl_gpio_hp_pin_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
    return status;
  }
  status = sl_si91x_gpio_driver_set_ulp_peri_on_soc_pin_mode(&sl_gpio_hp_pin_config.port_pin, SL_GPIO_MODE_0);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_ulp_peri_on_soc_pin_mode, "
                          "Error code: %lu\r\n",
                          status);
    return status;
  }
  status = sl_gpio_set_configuration(sl_gpio_ulp_pin_config);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
    return status;
  }
  SL_PRINT_STRING_ERROR("ULP GPIO %d is mapped to HP GPIO %d successfully \r\n",
                        sl_gpio_ulp_pin_config.port_pin.pin,
                        sl_gpio_hp_pin_config.port_pin.pin);
  return SL_STATUS_OK;
}
#endif

/*******************************************************************************
 *  This API handles ULP GPIO OR'ed pin interrupt request
 ******************************************************************************/
static void gpio_ulp_pin_interrupt_callback(uint32_t pin_intr)
{
  if (pin_intr == ULP_PIN_INTR_0) {
    // This is with respect to ISR context. Debugout might cause issues sometimes.
    SL_PRINT_STRING_ERROR("gpio ulp pin interrupt0\r\n");
  }
}
