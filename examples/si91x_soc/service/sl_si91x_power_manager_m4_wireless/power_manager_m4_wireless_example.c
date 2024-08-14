/***************************************************************************/ /**
 * @file power_manager_example.c
 * @brief Power Manager example functions
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
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "sl_si91x_driver.h"
#include "sl_net.h"
#include "sl_si91x_power_manager.h"
#include "power_manager_m4_wireless_example.h"
#include "rsi_debug.h"
#include "rsi_m4.h"
#include "sl_si91x_calendar.h"
#include "sl_ulp_timer_instances.h"
#include "sl_si91x_ulp_timer_common_config.h"
#include "sl_si91x_button_instances.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP) // Ored value of event for which callback is subscribed

#define ULP_TIMER_MATCH_VALUE 40000000 // Timer match value for down-counter type with 20mhz clock for 2 seconds
#define ULP_TIMER_INSTANCE \
  SL_ULP_TIMER_TIMER0 // ulp-timer instance to be used, user can pass selected timer-number in place of '0'

#define BUTTON_INSTANCE_0 button_btn0

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
// This enum is used in the state transition
typedef enum {
  PS4_TO_PS2, // PS4 to PS2 state change
  PS2_TO_PS4, // PS2 to PS4 state change
  PS4_SLEEP,  // PS4 Sleep
  PS4_TO_PS3, // PS4 to PS3 state change
  PS3_SLEEP,  // PS3 Sleep
  PS3_TO_PS2, // PS3 to PS2 state change
  PS2_SLEEP,  // PS2 Sleep
  PS1,        // PS1 state
  PS2_TO_PS3, // PS2 to PS3 state change
  PS3_TO_PS4, // PS3 to PS4 state change
  PS0,        // PS0 State (Sleep without retention)
  TERMINATED, // Last state change
} ps_transition;

static ps_transition transition = PS4_TO_PS2;
static boolean_t change_state   = false;

// Thread attributes which are required for creating a thread
const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void application_start(void *argument);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);
static void transition_callback(sl_power_state_t from, sl_power_state_t to);
static void on_sec_callback(void);
static void ulp_timer_callback(void);

static void set_npss_wakeup_source(uint32_t wakeup_source);
static void clear_npss_wakeup_source(uint32_t wakeup_source);
static void set_ulp_timer_wakeup_source(void);
static void clear_ulp_timer_wakeup_source(void);
static void power_control(sl_power_state_t from, sl_power_state_t to);
static void remove_unused_peripherals(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * Initializes the thread.
 * New thread is created with the application_start function.
 ******************************************************************************/
void power_manager_example_init(void)
{
  // Initialization of thread with the application_start function
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

/*******************************************************************************
 * - This function is called in the thread.
 * - All the activities are performed in this function.
 * - The wireless interface is initialized first and then the power manager service is initialized.
 * - Events are subscribed to get the callbacks.
 * - This function transits between different power states and sleep upon the button press.
 * - The soc transits the power state in the following patter: 
 *    - By default the power state is PS4. After button press it transits to PS2 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS4. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS3 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS3-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS3. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS2 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS2-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS2. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS1 with ulp timer trigger as a wakeup
 *      source and wait for the ulp timer trigger, it triggers after two seconds. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS2. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS3 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4.
 *    - When button trigger is detected, it transits to PS0 deepsleep without retention with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and resets the controller and starts the execution from main(). Now it waits for the button trigger.
 *    - The transition summary is PS4 -> PS2 -> PS4 -> PS4 Sleep -> PS4 -> PS3 -> PS3 Sleep -> PS3
 *      -> PS2 -> PS2 Sleep -> PS2 -> PS1 -> PS2 -> PS3 -> PS4 -> PS0 -> resets the controller.
 ******************************************************************************/
static void application_start(void *argument)
{
  UNUSED_VARIABLE(argument);
  sl_power_manager_ps_transition_event_handle_t handle;
  sl_power_manager_ps_transition_event_info_t info = { .event_mask = PS_EVENT_MASK, .on_event = transition_callback };
  sl_status_t status;
  // Initialize the wireless interface and put the TA in Standby with RAM retention mode.
  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("Wireless API initialization failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("Wireless is successfully initialized and TA is in Sleep \n");

  // Subscribe the state transition callback events, the ored value of flag and function pointer is passed in this API.
  status = sl_si91x_power_manager_subscribe_ps_transition_event(&handle, &info);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("Power Manager transition event subscription failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("Power Manager transition event is subscribed \n");
  change_state = false;

  while (true) {
    power_manager_example_process_action();
  }
}

/*******************************************************************************
 * Initialization of wireless APIs.
 * M4-TA handshake is performed.
 * TA is send to standby with RAM retention mode if SWITCH_TO_PS0 is disabled.
 * TA is send to standby without RAM retention mode if SWITCH_TO_PS0 is enabled.
 ******************************************************************************/
static sl_status_t initialize_wireless(void)
{
  // For M4-sleep wakeup, and to achieve minimum current in powersave application,
  // wifi is initialized, handshake is performed between M4 and TA, then
  // TA powersave profile is updated sleep with/without retention as per
  // requirements.
  // Wifi device configuration
  const sl_wifi_device_configuration_t client_init_configuration = {
    .boot_option = LOAD_NWP_FW,
    .mac_address = NULL,
    .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
    .region_code = US,
    .boot_config = { .oper_mode       = SL_SI91X_CLIENT_MODE,
                     .coex_mode       = SL_SI91X_WLAN_ONLY_MODE,
                     .feature_bit_map = (SL_SI91X_FEAT_SECURITY_OPEN | SL_SI91X_FEAT_WPS_DISABLE
                                         | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE),
                     .tcp_ip_feature_bit_map =
                       (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
                        | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                     .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                     .ext_custom_feature_bit_map = SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0,
                     .bt_feature_bit_map         = 0,
                     .ext_tcp_ip_feature_bit_map =
                       (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                        | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                     .ble_feature_bit_map     = 0,
                     .ble_ext_feature_bit_map = 0,
                     .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP }
  };
  sl_status_t status;
  // Initialize the wifi interface.
  status = sl_wifi_init(&client_init_configuration, NULL, NULL);
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_wifi_init failed, Error Code: 0x%lX \n", status);
    return status;
  }

  uint8_t xtal_enable = 1;
  // M4-TA handshake is required for TA communication.
  status = sl_si91x_m4_ta_secure_handshake(SL_SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with error code.
    DEBUGOUT("sl_si91x_m4_ta_secure_handshake failed, Error Code: 0x%lX \n", status);
    return status;
  }
  // Wireless Sleep with ram retention
  wireless_sleep(true);
  // If reaches here, returns SL_STATUS_OK.
  return SL_STATUS_OK;
}

/*******************************************************************************
 * After PS2 to PS4 transition, flash is initialized and to initialize flash
 * wireless processor is set to active mode.
 * This function sends the wireless processor to sleep with retention.
 ******************************************************************************/
static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_status_t status;
  // Wifi Profile (TA Mode) is set to High Performance.
  sl_wifi_performance_profile_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  status = sl_wifi_set_performance_profile(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with error code.
    DEBUGOUT("sl_wifi_set_performance_profile failed, Error Code: 0x%lX \n", status);
    return;
  }
  if (sleep_with_retention) {
    // Wifi Profile (TA Mode) is set to standby power save with RAM retention.
    ta_performance_profile.profile = STANDBY_POWER_SAVE_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = STANDBY_POWER_SAVE;
  }
  // Wifi Profile (TA Mode) is set to standby power save with RAM retention.
  status = sl_wifi_set_performance_profile(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with error code.
    DEBUGOUT("sl_wifi_set_performance_profile failed, Error Code: 0x%lX \n", status);
    return;
  }
}

/***************************************************************************/ /**
 * Check if the MCU can sleep at that time. This function is called when the system
 * is about to go sleeping, with the interrupts disabled. It allows the software to
 * cancel going to sleep in case of a last-minute event occurred (window between the
 * function call and interrupt disable).
 *
 * @return  True, if the system can go to sleep.
 *          False, otherwise.
 *
 * @note  This function is called with the interrupt disabled and it MUST NOT be
 *        re-enabled.
 ******************************************************************************/
boolean_t app_is_ok_to_sleep(void)
{
  return true;
}

/***************************************************************************/ /**
 * Check if the MCU can sleep after an interrupt. This function is called after an
 * interrupt occured and was processed. It allows the power manger to know if it must
 * go back to sleep or wakeup.
 *
 * @return  SL_SI91X_POWER_MANAGER_IGNORE, if the module did not trigger an ISR and it
 *          won't to contribute to the decision.
 *
 *          SL_SI91X_POWER_MANAGER_SLEEP, The module was the one that caused the system
 *          wakeup and the system SHOULD go back to sleep.
 *
 *          SL_SI91X_POWER_MANAGER_WAKEUP, The module was the one that caused the system
 *          wakeup and the system MUST NOT go back to sleep.
 *
 * @note  This function must not have any side effects. It is not guaranteed to be
 *        called for every ISR. If a prior hook function requires to wakeup, such
 *        as a wireless stack, the application hook function won't be called.
 ******************************************************************************/
sl_si91x_power_manager_on_isr_exit_t app_sleep_on_isr_exit(void)
{
  return SL_SI91X_POWER_MANAGER_ISR_WAKEUP;
}

/*******************************************************************************
 * Function will run continuously in while loop
 * According to the button press it changes the power state.
 ******************************************************************************/
void power_manager_example_process_action(void)
{
  switch (transition) {
    case PS4_TO_PS2:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS2 state.
        power_control(SL_SI91X_POWER_MANAGER_PS4, SL_SI91X_POWER_MANAGER_PS2);
        change_state = false;
        // Next transition is from PS2 to PS4 state.
        transition = PS2_TO_PS4;
      }
      break;
    case PS2_TO_PS4:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS4 state.
        power_control(SL_SI91X_POWER_MANAGER_PS2, SL_SI91X_POWER_MANAGER_PS4);
        // Wireless deepsleep with ram retention
        wireless_sleep(true);
        change_state = false;
        // Next transition is from PS4 to PS4 sleep.
        transition = PS4_SLEEP;
      }
      break;
    case PS4_SLEEP:
      if (change_state) {
        power_control(SL_SI91X_POWER_MANAGER_PS4, SL_SI91X_POWER_MANAGER_SLEEP);
        change_state = false;
        // Next transition is from PS4 to PS3 state.
        transition = PS4_TO_PS3;
      }
      break;
    case PS4_TO_PS3:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS3 state.
        power_control(SL_SI91X_POWER_MANAGER_PS4, SL_SI91X_POWER_MANAGER_PS3);
        change_state = false;
        // Next transition is from PS3 to PS3 sleep.
        transition = PS3_SLEEP;
      }
      break;
    case PS3_SLEEP:
      if (change_state) {
        power_control(SL_SI91X_POWER_MANAGER_PS3, SL_SI91X_POWER_MANAGER_SLEEP);
        change_state = false;
        // Next transition is from PS3 to PS2 state.
        transition = PS3_TO_PS2;
      }
      break;
    case PS3_TO_PS2:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS2 state.
        power_control(SL_SI91X_POWER_MANAGER_PS3, SL_SI91X_POWER_MANAGER_PS2);
        change_state = false;
        // Next transition is from PS2 to PS2 sleep.
        transition = PS2_SLEEP;
      }
      break;
    case PS2_SLEEP:
      if (change_state) {
        power_control(SL_SI91X_POWER_MANAGER_PS2, SL_SI91X_POWER_MANAGER_SLEEP);
        change_state = false;
        // Next transition is from PS2 to PS3 state.
        transition = PS2_TO_PS3;
      }
      break;
    case PS2_TO_PS3:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS3 state.
        power_control(SL_SI91X_POWER_MANAGER_PS2, SL_SI91X_POWER_MANAGER_PS3);
        // Wireless deepsleep with ram retention
        wireless_sleep(true);
        change_state = false;
        // Next transition is from PS3 to PS4 state.
        transition = PS3_TO_PS4;
      }
      break;
    case PS3_TO_PS4:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // If button is pressed, change to PS4 state.
        power_control(SL_SI91X_POWER_MANAGER_PS3, SL_SI91X_POWER_MANAGER_PS4);
        change_state = false;
        // All state transitions are completed.
        transition = PS0;
      }
      break;
    case PS0:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // Wireless deepsleep without ram retention
        wireless_sleep(false);
        // If button is pressed, change to PS0 state.
        power_control(SL_SI91X_POWER_MANAGER_PS4, SL_SI91X_POWER_MANAGER_PS0);
        change_state = false;
        // All state transitions are completed.
        transition = TERMINATED;
      }
      break;
    case TERMINATED:
      // After completion it remains here.
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Callback function for state transition.
 * Prints the state transition with the parameters from and to.
 ******************************************************************************/
static void transition_callback(sl_power_state_t from, sl_power_state_t to)
{
  // DEBUGINIT needs ULP Uart to be enabled in low power states.
  // This is a demonstration and it is not necessary to have debugout in callback function.
  // This callback function can be used to perform any activities after state transitions.
  DEBUGINIT();

  switch (from) {
    case SL_SI91X_POWER_MANAGER_PS4:
      // Previous state was PS4
      DEBUGOUT("Leaving PS4 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      // Previous state was PS3
      DEBUGOUT("Leaving PS3 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      // Previous state was PS2
      DEBUGOUT("Leaving PS2 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      // Wakeup from PS1
      DEBUGOUT("Leaving PS1 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_SLEEP:
      // Wakeup from sleep
      DEBUGOUT("Leaving Sleep State \n");
      break;
    case SL_SI91X_POWER_MANAGER_STANDBY:
      // Wakeup from standby
      DEBUGOUT("Leaving Standby State \n");
      break;
    default:
      break;
  }

  switch (to) {
    case SL_SI91X_POWER_MANAGER_PS4:
      // Current state is PS4
      DEBUGOUT("Entering PS4 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      // Current state is PS3
      DEBUGOUT("Entering PS3 State \n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      // Current state is PS2
      DEBUGOUT("Entering PS2 State \n");
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Changes the power state according to the from and two parameters.
 * It calls the power manager service APIs to change the power state.
 * All the valid power states are handled here.
 ******************************************************************************/
static void power_control(sl_power_state_t from, sl_power_state_t to)
{
  sl_status_t status;
  if (from == SL_SI91X_POWER_MANAGER_PS4) {
    switch (to) {
      case SL_SI91X_POWER_MANAGER_PS3:
        // PS4 state requirement is removed.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        // PS3 state requirement is added, it transits to PS3 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;

      case SL_SI91X_POWER_MANAGER_PS2:
        remove_unused_peripherals();
        // PS2 State requirement is added, it transits to PS2 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;
      default:
        break;
    }
  }
  if (from == SL_SI91X_POWER_MANAGER_PS3) {
    switch (to) {
      case SL_SI91X_POWER_MANAGER_PS4:
        // PS3 state requirement is removed.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }

        // PS4 state requirement is added, it transits to PS4 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;

      case SL_SI91X_POWER_MANAGER_PS2:
        remove_unused_peripherals();
        // PS3 state requirement is removed.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }

        // PS2 State requirement is added, it transits to PS2 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;
      default:
        break;
    }
  }
  if (from == SL_SI91X_POWER_MANAGER_PS2) {
    switch (to) {
      case SL_SI91X_POWER_MANAGER_PS4:
        // PS2 state requirement is removed.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }

        // PS4 state requirement is added, it transits to PS4 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;

      case SL_SI91X_POWER_MANAGER_PS3:
        // PS2 state requirement is removed as it was previously added.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        // PS3 state requirement is added, it transits to PS3 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        break;

      case SL_SI91X_POWER_MANAGER_PS1:
        set_ulp_timer_wakeup_source();
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        // PS2 state requirement is removed as it was previously added.
        status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        // Add requirement for PS0 (sleep without retention), it transits to PS0 state.
        status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS1);
        if (status != SL_STATUS_OK) {
          // If status is not OK, return with the error code.
          DEBUGOUT("sl_si91x_power_manager_add_ps_requirement failed, Error Code: 0x%lX \n", status);
        }

        clear_ulp_timer_wakeup_source();
        break;
      default:
        break;
    }
  }

  if (to == SL_SI91X_POWER_MANAGER_SLEEP) {
    set_npss_wakeup_source(SL_SI91X_POWER_MANAGER_SEC_WAKEUP);

    DEBUGOUT("Entering PS%d Sleep \n", sl_si91x_power_manager_get_current_state());
    // Call the sleep function, it goes to PS4 sleep as current state is PS4.
    status = sl_si91x_power_manager_sleep();
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_sleep failed, Error Code: 0x%lX \n", status);
      return;
    }

    clear_npss_wakeup_source(SL_SI91X_POWER_MANAGER_SEC_WAKEUP);
  }

  if (to == SL_SI91X_POWER_MANAGER_PS0) {
    set_npss_wakeup_source(SL_SI91X_POWER_MANAGER_SEC_WAKEUP);

    DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());

    // PS4 state requirement is removed as it was previously added.
    status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
    }
    // Add requirement for PS0 (sleep without retention), it transits to PS0 state.
    status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS0);
    if (status != SL_STATUS_OK) {
      // If status is not OK, return with the error code.
      DEBUGOUT("sl_si91x_power_manager_add_ps_requirement failed, Error Code: 0x%lX \n", status);
    }

    clear_npss_wakeup_source(SL_SI91X_POWER_MANAGER_SEC_WAKEUP);
  }
}

/*******************************************************************************
 * Turns off the unused peripherals
 ******************************************************************************/
static void remove_unused_peripherals(void)
{
  sl_power_peripheral_t peri;
  peri.m4ss_peripheral  = 0;
  peri.ulpss_peripheral = 0;
  // Ored value for npss peripheral.
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3;
  // Peripherals passed in this API are powered off.
  sl_si91x_power_manager_remove_peripheral_requirement(&peri);
}

/*******************************************************************************
 * Button ISR to change the state of flag, used in state machine.
 ******************************************************************************/
void sl_si91x_button_isr(uint8_t pin, int8_t state)
{
  UNUSED_VARIABLE(state);
  if (pin == BUTTON_INSTANCE_0.pin) {
    change_state = true;
  }
}
/*******************************************************************************
 * Configure and initialize the calendar wakeup source.
 * Different wakeup source can be passed here.
 ******************************************************************************/
static void set_npss_wakeup_source(uint32_t wakeup_source)
{
  sl_status_t status;
  switch (wakeup_source) {
    case SL_SI91X_POWER_MANAGER_SEC_WAKEUP:
      // Calendar clock is stopped to reset the configurations.
      sl_si91x_calendar_rtc_stop();
      // Calendar configuration is set, i.e. clock is set to RC clock.
      status = sl_si91x_calendar_set_configuration(2);
      if (status != SL_STATUS_OK) {
        // If status is not OK, error code is returned.
        DEBUGOUT("sl_si91x_ulp_timer_set_configuration failed, Error Code: 0x%lX \n", status);
        return;
      }
      // Wakeup source is configured as second trigger.
      status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_SEC_WAKEUP, true);
      if (status != SL_STATUS_OK) {
        // If status is not OK, return with the error code.
        DEBUGOUT("sl_si91x_power_manager_set_wakeup_sources failed, Error Code: 0x%lX \n", status);
        return;
      }
      // Calendar is initialized.
      sl_si91x_calendar_init();
      // Second trigger callback is configured.
      status = sl_si91x_calendar_register_sec_trigger_callback(on_sec_callback);
      if (status != SL_STATUS_OK) {
        // If status is not OK, return with the error code.
        DEBUGOUT("sl_si91x_calendar_register_sec_trigger_callback failed, Error Code: 0x%lX \n", status);
        return;
      }
      break;

    case SL_SI91X_POWER_MANAGER_DST_WAKEUP:
      // Initialization of wakeup sources can be added here.
      break;

    case SL_SI91X_POWER_MANAGER_GPIO_WAKEUP:
      // Initialization of wakeup sources can be added here.
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Clears and stops the wakeup source peripheral.
 * Different wakeup source can be cleared here.
 ******************************************************************************/
static void clear_npss_wakeup_source(uint32_t wakeup_source)
{
  sl_status_t status;
  switch (wakeup_source) {
    case SL_SI91X_POWER_MANAGER_SEC_WAKEUP:
      // Calendar clock is stopped as it is not needed.
      sl_si91x_calendar_rtc_stop();
      // Second trigger callback is unregistered and disabled.
      status = sl_si91x_calendar_unregister_sec_trigger_callback();
      if (status != SL_STATUS_OK) {
        // If status is not OK, return with the error code.
        DEBUGOUT("sl_si91x_calendar_register_sec_trigger_callback failed, Error Code: 0x%lX \n", status);
        return;
      }
      // Wakeup source is removed.
      status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_SEC_WAKEUP, false);
      if (status != SL_STATUS_OK) {
        // If status is not OK, return with the error code.
        DEBUGOUT("sl_si91x_power_manager_set_wakeup_sources failed, Error Code: 0x%lX \n", status);
        return;
      }
      break;

    case SL_SI91X_POWER_MANAGER_DST_WAKEUP:
      // Wakeup source can be stopped and cleared here
      break;

    case SL_SI91X_POWER_MANAGER_GPIO_WAKEUP:
      // Wakeup source can be stopped and cleared here
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Configure and initialize the ulp timer wakeup source.
 * ULP timer interrupt is used to wakeup from PS1.
 * ULP timer is initialzed and configured for 2 seconds.
 * Callback is registered for ULP Timer interrupt.
 ******************************************************************************/
static void set_ulp_timer_wakeup_source(void)
{
  sl_status_t status;
  // ULP Timer initialization, the values are fetched from the UC.
  status = sl_si91x_ulp_timer_init(&sl_timer_clk_handle);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_init failed, Error Code: 0x%lX \n", status);
    return;
  }
  // Match value is set to 2 seconds.
  sl_timer_handle_timer0.timer_match_value = ULP_TIMER_MATCH_VALUE;
  // ULP timer configurations are set, the values are fetched from the UC.
  status = sl_si91x_ulp_timer_set_configuration(&sl_timer_handle_timer0);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_set_configuration failed, Error Code: 0x%lX \n", status);
    return;
  }
  // Callback is registered to enable the timer interrupt.
  status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_INSTANCE, ulp_timer_callback);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_register_timeout_callback failed, Error Code: 0x%lX \n", status);
    return;
  }
  // ULP based wakeup source is selected.
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_ULPSS_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_power_manager_set_wakeup_sources failed, Error Code: 0x%lX \n", status);
    return;
  }

  // ULP Timer is started for timer 0.
  status = sl_si91x_ulp_timer_start(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_start failed, Error Code: 0x%lX \n", status);
    return;
  }
}

/*******************************************************************************
 * Clears ulp timer wakeup source.
 * ULP timer is stopped here.
 * Callback is unregistered for ULP Timer interrupt.
 ******************************************************************************/
static void clear_ulp_timer_wakeup_source(void)
{
  sl_status_t status;
  // After waking up, ulp timer is stopped.
  status = sl_si91x_ulp_timer_stop(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_stop failed, Error Code: 0x%lX \n", status);
    return;
  }
  // After waking up, ulp timer callback is unregistered.
  status = sl_si91x_ulp_timer_unregister_timeout_callback(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_ulp_timer_unregister_timeout_callback failed, Error Code: 0x%lX \n", status);
    return;
  }
  // Once the sleep-wakeup is completed, ulp based wakeup source is removed.
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_ULPSS_WAKEUP, false);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_si91x_power_manager_set_wakeup_sources failed, Error Code: 0x%lX \n", status);
    return;
  }
}

/*******************************************************************************
 * ULP Timer callback.
 * Interrupt Clearing is handled in driver layer, so this function is defined
 * to avoid undefined function while registering callback.
 ******************************************************************************/
static void ulp_timer_callback(void)
{
}

/*******************************************************************************
 * Second trigger callback.
 * Interrupt Clearing is handled in driver layer, so this function is defined
 * to avoid undefined function while registering callback.
 ******************************************************************************/
static void on_sec_callback(void)
{
}
