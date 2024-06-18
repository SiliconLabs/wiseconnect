/***************************************************************************/ /**
 * @file joystick_example.c
 * @brief Joystick example
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
#include "sl_si91x_joystick.h"
#include "joystick_example.h"
#include "rsi_debug.h"

#include "sl_sleeptimer.h"
#include "sl_si91x_clock_manager.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DELAY_MS1_PERIODIC 400 //sleeptimer1 periodic timeout in ms

#define SOC_PLL_CLK  ((uint32_t)(180000000)) // 180MHz default SoC PLL Clock as source to Processor
#define INTF_PLL_CLK ((uint32_t)(180000000)) // 180MHz default Interface PLL Clock as source to all peripherals
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
sl_sleeptimer_timer_handle_t timer1; //sleeptimer1 handle
boolean_t delay_timeout = false;     //Indicates sleeptimer1 timeout

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
//Sleeptimer timeout callbacks
static void on_timeout_timer1(sl_sleeptimer_timer_handle_t *handle, void *data);
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
 * Joystick example initialization function
 ******************************************************************************/
void joystick_example_init(void)
{
  sl_status_t status = 0;

  // default clock configuration by application common for whole system
  default_clock_configuration();

  do {
    //Start 400 ms periodic timer
    sl_sleeptimer_start_periodic_timer_ms(&timer1,
                                          DELAY_MS1_PERIODIC,
                                          on_timeout_timer1,
                                          NULL,
                                          0,
                                          SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
    //Initialize Joystick
    status = sl_si91x_joystick_init();
    //Due to calling trim_efuse API on ADC init in driver it will change the clock frequency,
    // if we are not initialize the debug again it will print the garbage data in console output.
    DEBUGINIT();
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_joystick_init: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Joystick Initialization Success\n");
    //Start/Enable the Joystick
    status = sl_si91x_joystick_start(SL_JOYSTICK_ENABLED);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_joystick_start: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Joystick started Successfully\n");
  } while (false);
}

/*******************************************************************************
 * Function will run continuously.
 *
 * This function is called in the main while loop. It reads and prints joystick
 * position via VCOM at regular intervals specified by DELAY_MS.
 ******************************************************************************/
void joystick_example_process_action(void)
{
  sl_joystick_position_t pos;
  sl_status_t status;
  /* Check if delay timer timed out */
  if (delay_timeout == true) {
    //Get the Joystick position.
    status = sl_si91x_joystick_get_position(SL_JOYSTICK_ENABLED, &pos);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_joystick_get_position: Error Code : %lu \n", status);
    }
    switch (pos) {
      case SL_JOYSTICK_NONE:
        printf("Not Pressed\n");
        break;
      case SL_JOYSTICK_C:
        printf("Center\n");
        break;
      case SL_JOYSTICK_N:
        printf("North\n");
        break;
      case SL_JOYSTICK_S:
        printf("South\n");
        break;
      case SL_JOYSTICK_E:
        printf("East\n");
        break;
      case SL_JOYSTICK_W:
        printf("West\n");
        break;
    }
    delay_timeout = false;
  }
}

/***************************************************************************/ /**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout_timer1(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)&handle;
  (void)&data;
  delay_timeout = true;
}
