/*******************************************************************************
 * @file  gpio_detailed_mode_freertos.c
 * @brief  gpio detailed example (FreeRTOS)
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
 * This application demonstrates the toggling of HP instance GPIO at intervals
 * of 1 second and provides usage of additional APIs, running as a dedicated
 * FreeRTOS task.
 ============================================================================**/
#include "gpio_detailed_mode_freertos.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_gpio_board.h"
#include "rsi_debug.h"
#include "sl_si91x_clock_manager.h"
#include "cmsis_os2.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DELAY                1000 // Delay for 1sec
#define GPIO_PORT_GROUP_ABCD 0    // Set to 1 to enable GPIO Ports A/B/C/D configuration and control
/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
typedef sl_gpio_t sl_si91x_gpio_t;

static sl_si91x_gpio_pin_config_t gpio_pin_config = { { SL_SI91X_GPIO_10_PORT, SL_SI91X_GPIO_10_PIN }, GPIO_OUTPUT };
#if (GPIO_PORT_GROUP_ABCD == 1)
static sl_si91x_gpio_pin_config_t gpio_multi_port_configs[] = { { { SL_GPIO_PORT_A, GPIO_PIN_NUMBER6 }, GPIO_OUTPUT },
                                                                { { SL_GPIO_PORT_B, GPIO_PIN_NUMBER11 }, GPIO_OUTPUT },
                                                                { { SL_GPIO_PORT_C, GPIO_PIN_NUMBER15 }, GPIO_OUTPUT },
                                                                { { SL_GPIO_PORT_D, GPIO_PIN_NUMBER1 }, GPIO_OUTPUT } };

#define GPIO_MULTI_PORT_COUNT (sizeof(gpio_multi_port_configs) / sizeof(gpio_multi_port_configs[0]))

static const char gpio_port_names[] = { 'A', 'B', 'C', 'D' };
#endif
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void gpio_detailed_mode_task(void *argument);
static sl_status_t gpio_detailed_init_function(void);

static const osThreadAttr_t gpio_detailed_mode_thread_attributes = {
  .name       = "gpio_task",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the GPIO detailed FreeRTOS task.
 * @return None
 ******************************************************************************/
void gpio_detailed_mode_example_init(void)
{
  osThreadId_t thread_id =
    osThreadNew((osThreadFunc_t)gpio_detailed_mode_task, NULL, &gpio_detailed_mode_thread_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create gpio thread\n");
    return;
  }
}
/*******************************************************************************
 * @brief  One-time GPIO hardware init: driver init, pin config, pad strength,
 *         pad disable state.
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t gpio_detailed_init_function(void)
{
  sl_status_t status;

  do {
    status = sl_gpio_driver_init();
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_driver_init, Error code: %lu\r\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver initialization is successful \r\n");

    status = sl_gpio_set_configuration(gpio_pin_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_set_configuration, Error code: %lu\r\n", status);
      break;
    }
#if (GPIO_PORT_GROUP_ABCD == 1)
    for (uint8_t i = 0; i < GPIO_MULTI_PORT_COUNT; i++) {
      status = sl_gpio_set_configuration(gpio_multi_port_configs[i]);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_set_configuration Port %c Pin %d, Error code: %lu\r\n",
                              gpio_port_names[i],
                              gpio_multi_port_configs[i].port_pin.pin,
                              status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver Port %c Pin %d configuration is successful \r\n",
                            gpio_port_names[i],
                            gpio_multi_port_configs[i].port_pin.pin);
    }
    if (status != SL_STATUS_OK) {
      break;
    }
#endif
    SL_PRINT_STRING_ERROR("GPIO driver set pin configuration is successful \r\n");

    status =
      sl_si91x_gpio_driver_select_pad_driver_strength(gpio_pin_config.port_pin.pin,
                                                      (sl_si91x_gpio_driver_strength_select_t)GPIO_FOUR_MILLI_AMPS);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_pad_driver_strength, "
                            "Error code: %lu\r\n",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver pad driver strength select is successful \r\n");

    status = sl_si91x_gpio_driver_select_pad_driver_disable_state(gpio_pin_config.port_pin.pin,
                                                                  (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_pad_driver_disable_"
                            "state, Error code: %lu\r\n",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver pad driver disable state select is successful \r\n");
  } while (false);

  return status;
}

/*******************************************************************************
 * @brief  GPIO FreeRTOS task. Initialises GPIO hardware, then loops forever
 *         toggling pin using osDelay for 1-second interval.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void gpio_detailed_mode_task(void *argument)
{
  (void)argument;

  sl_status_t status = gpio_detailed_init_function();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("GPIO init failed, exiting task\r\n");
    osThreadExit();
  }

  while (1) {
    status = sl_gpio_driver_toggle_pin(&gpio_pin_config.port_pin);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_toggle_pin, Error code: %lu\r\n", status);
    }
#if (GPIO_PORT_GROUP_ABCD == 1)
    for (uint8_t i = 0; i < GPIO_MULTI_PORT_COUNT; i++) {
      status = sl_gpio_driver_toggle_pin(&gpio_multi_port_configs[i].port_pin);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_toggle_pin Port %c Pin %d, Error code: %lu\r\n",
                              gpio_port_names[i],
                              gpio_multi_port_configs[i].port_pin.pin,
                              status);
      }
    }
#endif
    SL_PRINT_STRING_ERROR("HP GPIO driver toggle pin is successful \r\n");
    osDelay(DELAY);
  }
}
