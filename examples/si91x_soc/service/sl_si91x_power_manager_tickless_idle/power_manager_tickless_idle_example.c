/***************************************************************************/ /**
 * @file power_manager_tickless_idle_example.h
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
#include "power_manager_tickless_idle_example.h"
#include "rsi_debug.h"
#include "rsi_m4.h"
#include "sl_si91x_button_instances.h"
#include "FreeRTOS.h"
#include "timers.h"

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
  ADD_REQ,
  REM_REQ,
} ps_transition;

static ps_transition transition = ADD_REQ;
static boolean_t change_state   = false;
osSemaphoreId_t timer_semaphore;
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
void pm_timer_callback(TimerHandle_t xTimer);

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
 * - This function transits between different power states and sleep.
 ******************************************************************************/
static void application_start(void *argument)
{
  UNUSED_VARIABLE(argument);
  timer_semaphore = osSemaphoreNew(1U, 0U, NULL);
  sl_power_manager_ps_transition_event_handle_t handle;
  sl_power_manager_ps_transition_event_info_t info = { .event_mask = PS_EVENT_MASK, .on_event = transition_callback };
  sl_status_t status;
  TimerHandle_t PM_timer_handle;
  osStatus_t timer_status;
  // Initialize the wireless interface and put the NWP in Standby with RAM retention mode.
  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("Wireless API initialization failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("Wireless is successfully initialized and NWP is in Sleep \n");

  // Subscribe the state transition callback events, the ored value of flag and function pointer is passed in this API.
  status = sl_si91x_power_manager_subscribe_ps_transition_event(&handle, &info);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("Power Manager transition event subscription failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("Power Manager transition event is subscribed \n");

  change_state = false;

  // Create software timer
  PM_timer_handle = xTimerCreate("PM_app_os_timer", 500, 1, NULL, pm_timer_callback);
  // start software timer
  timer_status = xTimerStart(PM_timer_handle, portMAX_DELAY);

  if (timer_status != pdPASS) {
  }
  while (true) {
    power_manager_example_process_action();
  }
}

/*******************************************************************************
 * Initialization of wireless APIs.
 * M4-NWP handshake is performed.
 * NWP is send to standby with RAM retention mode if SWITCH_TO_PS0 is disabled.
 * NWP is send to standby without RAM retention mode if SWITCH_TO_PS0 is enabled.
 ******************************************************************************/
static sl_status_t initialize_wireless(void)
{
  // For M4-sleep wakeup, and to achieve minimum current in powersave application,
  // wifi is initialized, handshake is performed between M4 and NWP, then
  // NWP powersave profile is updated sleep with/without retention as per
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
  // M4-NWP handshake is required for NWP communication.
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
  // Wifi Profile (NWP Mode) is set to High Performance.
  sl_wifi_performance_profile_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  status = sl_wifi_set_performance_profile(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_wifi_set_performance_profile failed, Error Code: 0x%lX \n", status);
    return;
  }
  if (sleep_with_retention) {
    // Wifi Profile (NWP Mode) is set to standby power save with RAM retention.
    ta_performance_profile.profile = DEEP_SLEEP_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  }
  // Wifi Profile (NWP Mode) is set to standby power save with RAM retention.
  status = sl_wifi_set_performance_profile(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_wifi_set_performance_profile failed, Error Code: 0x%lX \n", status);
    return;
  }
}
/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void power_manager_example_process_action(void)
{
  osStatus_t sl_sem_status;
  sl_status_t status;
  switch (transition) {
    case ADD_REQ:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        if (sl_si91x_power_manager_get_current_state() == SL_SI91X_POWER_MANAGER_PS2) {
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            // If status is not OK, display the error info.
            DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
          }
        } else if (sl_si91x_power_manager_get_current_state() == SL_SI91X_POWER_MANAGER_PS4) {
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
          if (status != SL_STATUS_OK) {
            // If status is not OK, display the error info.
            DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
          }
        } else if (sl_si91x_power_manager_get_current_state() == SL_SI91X_POWER_MANAGER_PS3) {
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            // If status is not OK, display the error info.
            DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
          }
        }
        // Next transition is from PS3 to PS4 state.
        transition   = REM_REQ;
        change_state = false;
      }
      break;
    case REM_REQ:
      if (change_state) {
        DEBUGOUT("Current State: PS%d \n", sl_si91x_power_manager_get_current_state());
        status = sl_si91x_power_manager_remove_ps_requirement(sl_si91x_power_manager_get_current_state());
        if (status != SL_STATUS_OK) {
          // If status is not OK, display the error info.
          DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
        }
        change_state = false;
        // Next transition is from PS4 to PS3 state.
        transition = ADD_REQ;
      }
      break;
    default:
      break;
  }
  sl_sem_status = osSemaphoreAcquire(timer_semaphore, osWaitForever);
  if (sl_sem_status != osOK) {
    DEBUGOUT("\r\n osSemaphoreAcquire failed :%d \r\n", sl_sem_status);
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
 * PM_timer_callback to change the state of flag, used in state machine.
 ******************************************************************************/
void pm_timer_callback(TimerHandle_t xTimer)
{
  (void)xTimer;
  osStatus_t sl_sem_status;
  change_state = true;

  sl_sem_status = osSemaphoreRelease(timer_semaphore);
  if (sl_sem_status != osOK) {
    DEBUGOUT("\r\nosSemaphoreRelease failed :%d \r\n", sl_sem_status);
  }
}
