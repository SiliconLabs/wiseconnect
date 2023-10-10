/***************************************************************************/ /**
* @file  watchdog_timer_example.c
* @brief Watchdog-Timer examples functions
*******************************************************************************
* # License
* <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licenser of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/*==================================================================================
 * @brief :
 * In this example system restarts watchdog-timer on interrupts,
 * after ten times restarts of WDT, WDT resets system.
 *
 * @section Description :
 * First Initializing WDT, configuring clock Sources & parameters, registering callback.
 * Then starting WDT & system restarts(kicks) WDT on every timeout interrupt & toggles LED0
 * After six times restart of WDT, system not kicks it so WDT resets the system.
 * Then WDT is again initialized, configured clock Sources & parameters and registered callback
 * Then changing WDT interrupt time, system reset time and window time to new values and started again.
 * After six LED toggles, stopping WDT, unregistering callback and de-initialized it.
 ===================================================================================*/

#include "watchdog_timer_example.h"
#include "sl_si91x_watchdog_timer.h"
#include "rsi_board.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define WDT_RESTART_CNT          5 // Watchdog-timer restart(kick) count value, after which system will not restart it.
#define LED0                     0 // for on board LED-0
#define ZERO_INTERRUPT_CNT       0 // for zero interrupt count
#define NEW_INTERRUPT_TIME       TIME_DELAY_16 // for 2 seconds interrupt time
#define NEW_SYS_RST_TIME         TIME_DELAY_18 // for 8 seconds system-reset time
#define NEW_WINDOW_TIME          TIME_DELAY_10 // for 32 milliseconds window time
#define SL_WDT_INTERRUPT_TIME    15            // WDT Interrupt Time for 1 seconds
#define SL_WDT_SYSTEM_RESET_TIME 17            // WDT System Reset Time for 4 seconds
#define SL_WDT_WINDOW_TIME       0             // WDT Window Time for 0.0325 milli-seconds

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
void on_timeout_callback(void);

/*******************************************************************************
 **********************  Local variables   *************************************
 ******************************************************************************/
static uint8_t wdt_interrupt_count = ZERO_INTERRUPT_CNT;
static bool wdt_system_reset_flag  = false;
static bool wdt_stop_flag          = false;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void watchdog_timer_example_init(void)
{
  sl_status_t status;
  sl_watchdog_timer_version_t version;
  watchdog_timer_clock_config_t wdt_clock_config;
  wdt_clock_config.low_freq_fsm_clock_src  = KHZ_RC_CLK_SEL;
  wdt_clock_config.high_freq_fsm_clock_src = FSM_32MHZ_RC;
  wdt_clock_config.bg_pmu_clock_source     = RO_32KHZ_CLOCK;
  watchdog_timer_config_t wdt_config;
  wdt_config.interrupt_time    = SL_WDT_INTERRUPT_TIME;
  wdt_config.system_reset_time = SL_WDT_SYSTEM_RESET_TIME;
  wdt_config.window_time       = SL_WDT_WINDOW_TIME;
  uint8_t new_interrupt_time;
  uint8_t new_sys_rst_time;
  uint8_t new_window_time;
  // Toggles LED0 once
  RSI_Board_LED_Toggle(LED0);
  DEBUGOUT("In Main..!\r\n");
  // Checking system-reset status if true means system-reset done by watchdog-timer
  // else it is a power-on system-reset.
  if (sl_si91x_watchdog_get_timer_system_reset_status()) {
    // Assigning TRUE to system-reset flag, if reset done by watchdog-timer
    wdt_system_reset_flag = true;
    DEBUGOUT("Watchdog-timer system-reset occurred \r\n");
  } else {
    DEBUGOUT("Power on system-reset occurred..\r\n");
  }
  do {
    //Reading Version information of watchdog-timer
    version = sl_si91x_watchdog_get_version();
    DEBUGOUT("Watchdog-timer version is fetched successfully \n");
    DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
    // Initializing watchdog-timer
    sl_si91x_watchdog_init_timer();
    DEBUGOUT("Successfully initialized watchdog-timer \n");
    // Configuring watchdog-timer
    status = sl_si91x_watchdog_configure_clock(&wdt_clock_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_watchdog_timer_config : Invalid Parameters, Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully Configured watchdog-timer with default clock sources\n");
    // Configuring watchdog-timer
    status = sl_si91x_watchdog_set_configuration(&wdt_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_watchdog_timer_config : Invalid Parameters, Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully Configured watchdog-timer with default parameters\n");
    // Registering timeout callback
    status = sl_si91x_watchdog_register_timeout_callback(on_timeout_callback);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_watchdog_timer_register_timeout_callback : Invalid Parameters, Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("Successfully registered watchdog-timer timeout callback\n");
    // Checking WDT system-reset flag
    if (!(wdt_system_reset_flag)) {
      // Starting watchdog-timer with changed parameters
      sl_si91x_watchdog_start_timer();
      DEBUGOUT("Successfully started watchdog-timer with uc parameters \n");
    } else {
      // Changing WDT parameters
      // Changing system-reset time to 8s
      status = sl_si91x_watchdog_set_system_reset_time(NEW_SYS_RST_TIME);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_watchdog_timer_set_system_reset_time : Invalid Parameters, Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("Successfully changed watchdog-timer system reset time \n");
      //Changing interrupt time to 2s
      status = sl_si91x_watchdog_set_interrupt_time(NEW_INTERRUPT_TIME);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_watchdog_timer_set_interrupt_time : Invalid Parameters, Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("Successfully changed watchdog-timer interrupt time \n");
      // Changing window time to 32ms
      status = sl_si91x_watchdog_set_window_time(NEW_WINDOW_TIME);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_watchdog_timer_set_interrupt_time : Invalid Parameters, Error Code : %lu \n", status);
        break;
      }
      DEBUGOUT("Successfully changed watchdog-timer window time \n");
      // Reading interrupt time value
      new_interrupt_time = sl_si91x_watchdog_get_interrupt_time();
      DEBUGOUT("New interrupt time value : %d \n", new_interrupt_time);
      // Reading system-reset time value
      new_sys_rst_time = sl_si91x_watchdog_get_system_reset_time();
      DEBUGOUT("New system-reset time value : %d \n", new_sys_rst_time);
      // Reading window time value
      new_window_time = sl_si91x_watchdog_get_window_time();
      DEBUGOUT("New window time value : %d \n", new_window_time);
      DEBUGOUT("Successfully changed watchdog-timer system reset time \n");
      // making stop flag true
      wdt_stop_flag = true;
      // Starting watchdog-timer with changed parameters
      sl_si91x_watchdog_start_timer();
      DEBUGOUT("Successfully started watchdog-timer again with new parameters \n");
    }
  } while (false);
}

void watchdog_timer_example_process_action(void)
{
}

/*******************************************************************************
* Callback function of watchdog-timer
*
* @param data (void pointer)
* @return none
******************************************************************************/
void on_timeout_callback(void)
{
  // Note: For lower values of interrupt time (0 to 6) caution to remove all Debugs and
  // other instructions except watchdog-restart call to get exact delay, otherwise there will
  // be interrupt latency
  // Incrementing watchdog-timer interrupt count
  wdt_interrupt_count++;
  // Toggle on-board LED0 on every WDT restart
  RSI_Board_LED_Toggle(LED0);
  // To not re-start(kick) WDT at 6th timeout interrupts so that WDT generates the System-reset
  if (wdt_interrupt_count <= WDT_RESTART_CNT) {
    // System restarting(kicking) watchdog-timer on interrupts
    sl_si91x_watchdog_restart_timer();
    DEBUGOUT("In handler : WDT restarted \r\n");
  }
  if ((wdt_interrupt_count > WDT_RESTART_CNT) && (wdt_stop_flag)) {
    // Stopping the watchdog-timer after watchdog-timer system reset
    sl_si91x_watchdog_stop_timer();
    // Unregistering the timeout_callback
    sl_si91x_watchdog_unregister_timeout_callback();
    // De-initializing watchdog-timer
    sl_si91x_watchdog_deinit_timer();
    DEBUGOUT("Successfully stopped, unregistered & De-initialized watchdog-timer\n");
  }
  return;
}
