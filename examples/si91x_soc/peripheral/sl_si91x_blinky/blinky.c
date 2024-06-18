/*******************************************************************************
 * @file  blinky.c
 * @brief
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
/**============================================================================
 * @section Description
 * This files contains example code to demonstrate the GPIO/LED toggle functionality.
 ============================================================================**/

// Include Files
#include "rsi_ccp_user_config.h"
#include "sl_sleeptimer.h"
#include "sl_si91x_led_instances.h"
#include "rsi_debug.h"
#include "sl_si91x_led.h"
#include "blinky.h"
#include "sl_si91x_clock_manager.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef LED_INSTANCE
#define LED_INSTANCE led_led0
#endif

#ifndef TOOGLE_DELAY_MS
#define TOOGLE_DELAY_MS 500
#endif

#define SOC_PLL_CLK  ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor
#define INTF_PLL_CLK ((uint32_t)(180000000)) // 180MHz default Interface PLL Clock as source to all peripherals
/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

sl_sleeptimer_timer_handle_t timer;
bool toggle_timeout = false;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data);
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
/***************************************************************************/ /**
 * Initialize blinky example.
 ******************************************************************************/
void blinky_init(void)
{
  // default clock configuration by application common for whole system
  default_clock_configuration();

  // Create timer for waking up the system periodically.
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        TOOGLE_DELAY_MS,
                                        on_timeout,
                                        NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

/***************************************************************************/ /**
 * Blinky ticking function.
 ******************************************************************************/
void blinky_process_action(void)
{
  if (toggle_timeout == true) {
    sl_si91x_led_toggle(LED_INSTANCE.pin);
    toggle_timeout = false;
  }
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  toggle_timeout = true;
}
