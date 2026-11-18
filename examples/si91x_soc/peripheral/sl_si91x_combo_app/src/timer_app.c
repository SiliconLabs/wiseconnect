/*
 * timer_app.c
 *
 *  Created on: Dec 8, 2023
 *      Author: vebommak
 */

#include "timer_app.h"
#include "app.h"
#include "sl_si91x_ulp_timer.h"
#include "sl_ulp_timer_instances.h"
#include "sl_si91x_ulp_timer_common_config.h"
#include "rsi_debug.h"
#include "sl_si91x_led.h"
#include "i2c_app.h"
#include "cmsis_os2.h"
#include "sl_component_catalog.h"

extern osEventFlagsId_t event_flags_id; // event flags id
static void SL_ULP_TIMER_CALLBACK(void);
static void timer_config_match_reg(uint32_t);

void timer_config_match_reg(uint32_t ms)
{
  sl_status_t status;
  uint32_t match_val;

  status = sl_si91x_ulp_timer_stop(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    return;
  }
  match_val = ms * CLOCKS_PER_MICROSECONDS_32MHZ * TIME_ONE_MS;
  status    = sl_si91x_ulp_timer_set_count(ULP_TIMER_INSTANCE, match_val);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR(

      /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
      "sl_si91x_ulp_timer_set_count : Invalid Parameters Error Code : %lu\n",
      status);
  } else {
    //    DEBUGOUT("Successfully changed the timer match value\n");
  }
  status = sl_si91x_ulp_timer_start(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_start : Invalid Parameters Error Code : %lu\n", status);
    /*      break;*/
  }
}

void timer_init(void)
{
  sl_status_t status = true;
  /* At this stage the micro-controller clock setting is already configured,
   * this is done through SystemInit() function which is called from startup
   * file (startup_rs1xxxx.s) before to branch to application main.
   * To reconfigure the default setting of SystemInit() function, refer to
   * startup_rs1xxxx.c file
   */
  // Switching MCU from PS4 to PS2 state(low power state)
  // In this mode, whatever be the timer clock source value, it will run with 20MHZ only,
  // as it trims higher clock frequencies to 20MHZ.
  DEBUGINIT();
  do {
    // Timer Peripheral input clock source configuration, common for all ulp-timer instances
    status = sl_si91x_ulp_timer_init(&sl_timer_clk_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_init : Invalid Parameters, Error Code : %lu\n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully configured ULP-timer clock input source\n");
    // Updating default timer match-value
    SL_ULP_TIMER_HANDLE.timer_match_value = SL_TIMER_MATCH_VALUE;
    // Configuring timer instance parameters: mode-periodic, type-1us, match-value: 1second
    status = sl_si91x_ulp_timer_set_configuration(&(SL_ULP_TIMER_HANDLE));
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_set_configuration : Invalid "
                            "Parameters Error Code : %lu\n",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully configured ULP-timer parameters with "
                          "default parameters \n");
    // Registering timeout callback for the selected timer instance, which will also enable its interrupt
    status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_INSTANCE, &(SL_ULP_TIMER_CALLBACK));
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_ulp_timer_timeout_callback_register : "
                            "Invalid Parameters Error Code : %lu\n",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("Successfully registered timer instance timeout callback\n");

    // Starting Timer instance with default parameters and setting match value to 5 seconds
    timer_config_match_reg(MS_5000);

    SL_PRINT_STRING_ERROR("\nSuccessfully started ulp-timer for %dsec ticking "
                          "for initiation of i2c leader\n",
                          (MS_5000 / 1000));
  } while (false);
}

static void SL_ULP_TIMER_CALLBACK(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  uint32_t status;

  // set the flag 0x0001U for i2c-timer event
  status = osEventFlagsSet(event_flags_id, EVENT_FLAGS_I2C_TIMER_MASKBIT);
  if (status != EVENT_FLAGS_I2C_TIMER_MASKBIT) {
    SL_PRINT_STRING_ERROR("timer callback failed to set Event flag\n");
  }
#endif // SL_CATALOG_KERNEL_PRESENT
}
