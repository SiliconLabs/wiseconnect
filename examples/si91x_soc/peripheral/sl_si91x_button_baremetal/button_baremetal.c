/***************************************************************************/ /**
 * @file button_baremetal.c
 * @brief button baremetal examples functions
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

#include "button_baremetal.h"
#include "sl_si91x_led.h"
#include "sl_si91x_button.h"
#include "sl_si91x_button_pin_config.h"
#include "sl_si91x_led_config.h"
#include "sl_si91x_button_instances.h"
#include "sl_si91x_led_instances.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/
#ifndef BUTTON_INSTANCE
#define BUTTON_INSTANCE button_btn0
#endif

#ifndef LED_INSTANCE
#ifndef SI917_DEVKIT
#define LED_INSTANCE led_led0
#else
#define LED_INSTANCE led_ledb
#endif
#endif

#define SOC_PLL_CLK  ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor
#define INTF_PLL_CLK ((uint32_t)(180000000)) // 180MHz default Interface PLL Clock as source to all peripherals

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
/*******************************************************************************
**********************  Local Function prototypes   ***************************
******************************************************************************/
static void default_clock_configuration(void);
/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
// Function to configure clock on powerup
static void default_clock_configuration(void)
{
  // Core Clock runs at 180MHz SOC PLL Clock
  sl_si91x_clock_manager_m4_set_core_clk(M4_SOCPLLCLK, SOC_PLL_CLK);

  // All peripherals' source to be set to Interface PLL Clock
  // and it runs at 180MHz
  sl_si91x_clock_manager_set_pll_freq(INFT_PLL, INTF_PLL_CLK, PLL_REF_CLK_VAL_XTAL);
}
/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
 * Initialize example.
 ******************************************************************************/
void button_init(void)
{
  // default clock configuration by application common for whole system
  default_clock_configuration();

  return;
}

/***************************************************************************/ /**
 * Call back on button change.
 ******************************************************************************/
void button_process_action(void)
{
  return;
}

void sl_si91x_button_isr(uint8_t pin, int8_t state)
{
  if (pin == BUTTON_INSTANCE.pin) {
    if (state == BUTTON_PRESSED) {
      sl_si91x_led_toggle(LED_INSTANCE.pin);
    }
  }
}
