/***************************************************************************/ /**
 * @file ps2_wireless_event_monitor.c
 * @brief PS2 Wireless Event Monitor functions
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
#include "sl_wifi.h"
#include "sl_net.h"
#include "sl_wifi_callback_framework.h"
#include "sl_net_si91x.h"
#include "ps2_wireless_event_monitor.h"
#include "sl_si91x_power_manager.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_gpio_board.h"
#include "timers.h"
#include "sl_ulp_timer_instances.h"
#include "sl_si91x_ulp_timer_common_config.h"
#include "sl_net_default_values.h"
#include "sl_si91x_clock_manager.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
// Wifi Configuration for the example
#define DHCP_HOST_NAME    NULL
#define TIMEOUT_MS        10000
#define WIFI_SCAN_TIMEOUT 10000
sl_net_ip_configuration_t ip_address        = { 0 };
sl_wifi_client_configuration_t access_point = { 0 };
// Timer match value for generating periodic interrupts to toggle ULP GPIO 1
// ULP timer operates using 32 kHz RC clock (1 second = 32,000 match value)
// For 5 ms interval: (32,000 / 1000) * 5 = 160
#define ULP_TIMER_MATCH_VALUE 160
// ULP timer instance to be used.
#define ULP_TIMER_INSTANCE SL_ULP_TIMER_TIMER0
// OS timer duration in milliseconds (400ms for periodic state transitions)
#define STATE_TRANSITION_TIME 400
// GPIO pin configuration structure for ULP GPIO 1.
static sl_si91x_gpio_pin_config_t sl_gpio_pin_config = { { SL_SI91X_ULP_GPIO_1_PORT, SL_SI91X_ULP_GPIO_1_PIN },
                                                         GPIO_OUTPUT };
// Toggle state variable to track GPIO pin state (1 = high, 0 = low)
static int toggle_state = 1;
// ULP processor clock division factor for power saving (reduces 20MHz to 2MHz)
#define ULP_PROCESSOR_CLOCK_DIVISION 5
// ULP processor clock with no division (restores full 20MHz clock)
#define ULP_PROCESSOR_CLOCK_NO_DIVISION 0
// Dummy function configuration - User can customize ps3_state_application_action() for wireless event processing
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
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
// Main application thread function - handles power state transitions and event polling
static void application_start(void *argument);
// Initializes wireless APIs, performs M4-NWP handshake, and configures NWP power modes
static sl_status_t initialize_wireless(void);
// Puts the wireless interface into sleep mode for power saving
static void wireless_sleep(void);
// Callback handler for WiFi join events (connection status updates)
sl_status_t join_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  char *result,
                                  uint32_t result_length,
                                  void *arg);
// Callback function for periodic software timer events (400ms intervals)
void pm_timer_callback(TimerHandle_t xTimer);
// Configures ULP timer for power state transitions
static void set_ulp_timer_configuration(void);
// Callback function for ULP timer interrupts (5ms intervals)
static void ulp_timer_callback(void);
// Dummy function for user-defined operations during PS3 state transitions
static void ps3_state_application_action(void);
// Monitors the PS2 state transition
static sl_status_t sl_si91x_power_manager_monitor_ps2_state(void);
// Delay function
static void delay(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
* Initializes the thread.
* New thread is created with the application_start function.
******************************************************************************/
void ps2_wireless_event_monitor_init(void)
{
  // Initialization of thread with the application_start function
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}

/***************************************************************************/ /**
 * @brief Main application thread function.
 * This function is executed in the created thread. It performs the following:
 *   - Initializes the timer semaphore
 *   - Adds PS3 state requirement (transitions to PS3)
 *   - Initializes the wireless interface and puts NWP in Standby with RAM retention
 *   - Removes PS3 state requirement
 *   - Adds PS2 state requirement (transitions to PS2)
 *   - Creates and starts a periodic software timer
 *   - Initializes GPIO and ULP timer wakeup source
 *   - Enters main loop to handle power state transitions to process the packets
 ******************************************************************************/
static void application_start(void *argument)
{
  UNUSED_VARIABLE(argument);
  sl_status_t status;
  TimerHandle_t PM_timer_handle;
  osStatus_t timer_status;
  timer_semaphore = osSemaphoreNew(1U, 0U, NULL);

  // Add PS3 state requirement to transition to PS3
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
  }

  // Initialize wireless interface and put NWP in Standby with RAM retention
  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Wireless API initialization failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("Wireless is successfully initialized and NWP is in Sleep \n");

  // Remove PS3 state requirement
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State Transition Failed \n", status);
  }

  // Add PS2 state requirement to transition to PS2
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
  if (status != SL_STATUS_OK) {
    // If status is not OK, return the error info.
    DEBUGOUT("Error Code: 0x%lX, Power State ps2 Transition Failed \n", status);
  }

  // Create and start periodic software timer for state transitions
  PM_timer_handle = xTimerCreate("PM_app_os_timer", STATE_TRANSITION_TIME, 1, NULL, pm_timer_callback);
  timer_status    = xTimerStart(PM_timer_handle, portMAX_DELAY);
  if (timer_status != pdPASS) {
    DEBUGOUT("Failed to start PM_app_os_timer\n");
  }
  // Initialize GPIO
  sl_gpio_driver_init();
  sl_gpio_set_configuration(sl_gpio_pin_config);

  // Configure ULP timer for periodic power state transitions
  set_ulp_timer_configuration();
  DEBUGOUT("\r\n Start Example Demonstration \r\n");
  while (true) {
    ps2_wireless_event_monitor_process_action();
  }
}
/*******************************************************************************
 * Function will run continuously in while loop
 ******************************************************************************/
void ps2_wireless_event_monitor_process_action(void)
{
  sl_status_t status;
  osStatus_t sl_sem_status;
  // Reduce 20MHz M4 ULP Processor clock to 2MHz in PS2 state for power saving
  sl_si91x_clock_manager_ulp_processor_clk_division(ULP_PROCESSOR_CLOCK_DIVISION);
  sl_sem_status = osSemaphoreAcquire(timer_semaphore, osWaitForever);
  if (sl_sem_status != osOK) {
    DEBUGOUT("\r\n osSemaphoreAcquire failed :%d \r\n", sl_sem_status);
  }
  // Restore 20MHz M4 ULP Processor clock.
  sl_si91x_clock_manager_ulp_processor_clk_division(ULP_PROCESSOR_CLOCK_NO_DIVISION);
  // Diable all the interrupts while transitioning from PS2 to PS3 active state
  __disable_irq();
  // Set ULP GPIO pin 1
  sl_gpio_driver_set_pin(&sl_gpio_pin_config.port_pin);
  // Set toggle state to 1
  toggle_state = 1;
  // Remove PS2 state requirement
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State ps3 add Transition Failed \n", status);
  }
  // Add PS3 state requirement to transition to PS3
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State ps3 add Transition Failed \n", status);
  }
  // Enable all the interrupts after transitioning from PS3 to PS2 active state
  __enable_irq();
  //This is a dummy function that user can utilize to perform any operation when transitioned to PS3 state.
  ps3_state_application_action();
  // Monitor and handle PS2 state transition, ensuring no pending TX command before transitioning.
  sl_si91x_power_manager_monitor_ps2_state();
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
    .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                     .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                     .feature_bit_map =
                       (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE),
                     .tcp_ip_feature_bit_map =
                       (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT | SL_SI91X_TCP_IP_FEAT_SSL
                        | SL_SI91X_TCP_IP_FEAT_ICMP | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                     .custom_feature_bit_map = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                     .bt_feature_bit_map     = 0,
                     .ext_tcp_ip_feature_bit_map =
                       (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                        | SL_SI91X_CONFIG_FEAT_EXTENTION_VALID),
                     .ble_feature_bit_map     = 0,
                     .ble_ext_feature_bit_map = 0,
                     .config_feature_bit_map  = SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP }
  };
  sl_status_t status;

  // Initialize the wifi interface.
  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &client_init_configuration, NULL, NULL);

  sl_wifi_set_join_callback_v2(join_callback_handler, NULL);
  sl_wifi_advanced_client_configuration_t config = { .max_retry_attempts      = 1,
                                                     .scan_interval           = 4,
                                                     .beacon_missed_count     = 40,
                                                     .first_time_retry_enable = 0 };

  status = sl_wifi_set_advanced_client_configuration(SL_WIFI_CLIENT_INTERFACE, &config);
  // Initialize the debug uart for printing the debug messages.
  DEBUGINIT();
  if (status != SL_STATUS_OK) {
    // If status is not OK, return with the error code.
    DEBUGOUT("sl_wifi_init failed, Error Code: 0x%lX \n", status);
    return status;
  } else {
    DEBUGOUT("sl_wifi_init success\n");
  }
  // Set the credentials for the wifi connection.
  sl_wifi_credential_t cred  = { 0 };
  sl_wifi_credential_id_t id = SL_NET_DEFAULT_WIFI_CLIENT_CREDENTIAL_ID;
  memset(&access_point, 0, sizeof(sl_wifi_client_configuration_t));

  cred.type = SL_WIFI_PSK_CREDENTIAL;
  memcpy(cred.psk.value, DEFAULT_WIFI_CLIENT_CREDENTIAL, strlen((char *)DEFAULT_WIFI_CLIENT_CREDENTIAL));

  status = sl_net_set_credential(id,
                                 SL_NET_WIFI_PSK,
                                 DEFAULT_WIFI_CLIENT_CREDENTIAL,
                                 strlen((char *)DEFAULT_WIFI_CLIENT_CREDENTIAL));
  if (SL_STATUS_OK == status) {
    DEBUGOUT("Credentials set, id : %lu\n", id);

    access_point.ssid.length = strlen((char *)DEFAULT_WIFI_CLIENT_PROFILE_SSID);
    memcpy(access_point.ssid.value, DEFAULT_WIFI_CLIENT_PROFILE_SSID, access_point.ssid.length);
    access_point.security      = DEFAULT_WIFI_CLIENT_SECURITY_TYPE;
    access_point.encryption    = SL_WIFI_DEFAULT_ENCRYPTION;
    access_point.credential_id = id;

    DEBUGOUT("SSID %s\n", access_point.ssid.value);
    status = sl_wifi_connect(SL_WIFI_CLIENT_2_4GHZ_INTERFACE, &access_point, TIMEOUT_MS);
  }
  if (status != RSI_SUCCESS) {
    DEBUGOUT("WLAN connection failed %lx\r\n", status);
    return status;
  } else {
    DEBUGOUT("WLAN connected\r\n");
  }
  // Configure IP
  ip_address.type      = SL_IPV4;
  ip_address.mode      = SL_IP_MANAGEMENT_DHCP;
  ip_address.host_name = DHCP_HOST_NAME;
  status               = sl_si91x_configure_ip_address(&ip_address, SL_SI91X_WIFI_CLIENT_VAP_ID);
  if (status != RSI_SUCCESS) {
    DEBUGOUT("IP Config failed %lx\r\n", status);
    return status;
  }
  // Print the IP address.
  sl_ip_address_t ip = { 0 };
  ip.type            = ip_address.type;
  ip.ip.v4.value     = ip_address.ip.v4.ip_address.value;
  print_sl_ip_address(&ip);

  sl_wifi_firmware_version_t version = { 0 };
  // Firmware version Prints
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
  } else {
    print_firmware_version(&version);
  }
  // Wireless Sleep
  wireless_sleep();
  // If reaches here, returns SL_STATUS_OK.
  return status;
}

/*******************************************************************************
 * This function sets the wireless processor into power save mode.
 ******************************************************************************/
void wireless_sleep(void)
{
  sl_status_t status;
  sl_wifi_performance_profile_t ta_performance_profile = { .profile = ASSOCIATED_POWER_SAVE_LOW_LATENCY };
  status                                               = sl_wifi_set_performance_profile(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_wifi_set_performance_profile failed, Error Code: 0x%lX \n", status);
    return;
  }
  DEBUGOUT("\r\n sl_wifi_set_performance_profile set \n");
}

/*******************************************************************************
 * Configure and initialize the ulp timer.
 * ULP timer is initialzed and configured for 5 milliseconds.
 * Callback is registered for ULP Timer interrupt.
 ******************************************************************************/
static void set_ulp_timer_configuration(void)
{
  sl_status_t status;
  sl_timer_clk_handle.ulp_timer_clk_input_src = ULP_TIMER_32KHZ_RC_CLK_SRC;
  // ULP Timer initialization, the values are fetched from the UC.
  status = sl_si91x_ulp_timer_init(&sl_timer_clk_handle);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_si91x_ulp_timer_init failed, Error Code: 0x%lX \n", status);
    return;
  }
  // Match value is set to 5  milliseconds.
  sl_timer_handle_timer0.timer_match_value = ULP_TIMER_MATCH_VALUE;
  // ULP timer configurations are set, the values are fetched from the UC.
  status = sl_si91x_ulp_timer_set_configuration(&sl_timer_handle_timer0);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_si91x_ulp_timer_set_configuration failed, Error Code: 0x%lX \n", status);
    return;
  }
  // Callback is registered to enable the timer interrupt.
  status = sl_si91x_ulp_timer_register_timeout_callback(ULP_TIMER_INSTANCE, ulp_timer_callback);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_si91x_ulp_timer_register_timeout_callback failed, Error Code: 0x%lX \n", status);
    return;
  }
  // ULP based wakeup source is selected.
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_ULPSS_WAKEUP, true);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_si91x_power_manager_set_wakeup_sources failed, Error Code: 0x%lX \n", status);
    return;
  }
  // ULP Timer is started for timer 0.
  status = sl_si91x_ulp_timer_start(ULP_TIMER_INSTANCE);
  if (status != SL_STATUS_OK) {
    // If status is not OK, display the error info.
    DEBUGOUT("sl_si91x_ulp_timer_start failed, Error Code: 0x%lX \n", status);
    return;
  }
}

/*******************************************************************************
 * PM_timer_callback is used in periodic state transition.
 ******************************************************************************/
void pm_timer_callback(TimerHandle_t xTimer)
{
  (void)xTimer;
  osStatus_t sl_sem_status;
  // Check if timer_semaphore is acquired (count is 0)
  uint32_t semaphore_count = osSemaphoreGetCount(timer_semaphore);
  if (semaphore_count == 0) {
    sl_sem_status = osSemaphoreRelease(timer_semaphore);
    if (sl_sem_status != osOK) {
      DEBUGOUT("\r\nosSemaphoreRelease failed :%d \r\n", sl_sem_status);
    }
  }
}

/*******************************************************************************
 * ULP Timer callback.
 * Interrupt Clearing is handled in driver layer, so this function is defined
 * to avoid undefined function while registering callback.
 ******************************************************************************/
static void ulp_timer_callback(void)
{
  toggle_state = !toggle_state;
  if (toggle_state) {
    // Set ULP GPIO pin 1
    sl_gpio_driver_set_pin(&sl_gpio_pin_config.port_pin);
  } else {
    // Clear ULP GPIO pin 1
    sl_gpio_driver_clear_pin(&sl_gpio_pin_config.port_pin);
  }
}

/****************************************************************************
* Rejoin failure callback handler in station mode
*****************************************************************************/
sl_status_t join_callback_handler(sl_wifi_event_t event,
                                  sl_status_t status_code,
                                  char *result,
                                  uint32_t result_length,
                                  void *arg)
{
  UNUSED_PARAMETER(result);
  UNUSED_PARAMETER(result_length);
  UNUSED_PARAMETER(arg);

  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    return status_code;
  }

  DEBUGOUT("\r\n[WiFi] Join Failed: wireless event detected in join callback handler\r\n");
  return SL_STATUS_OK;
}

/*******************************************************************************
 * This function monitors the PS2 power state transition by checking for pending TX commands
 * and then managing power state requirements. It first checks if any pending TX command is
 * present. If a pending TX command exists, it waits until the TX command is completed.
 * If no pending TX command is present, it proceeds to transition to PS2 state. After TX
 * completion, it removes the PS3 state requirement and adds the PS2 state requirement to
 * transition the system to PS2 power state.
******************************************************************************/
static sl_status_t sl_si91x_power_manager_monitor_ps2_state(void)
{
  sl_status_t status;
#if SL_WIFI_COMPONENT_INCLUDED
  // It checks if any pending tx command is present. If present, it will wait until the tx command is completed. If no pending tx command is present, it will transition to PS2 state.
  while (sl_si91x_power_manager_is_tx_command_in_progress())
    ;
#endif
  // Remove PS3 state requirement
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State ps3 remove Transition Failed \n", status);
  }
  // Add PS2 state requirement to transition to PS2
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
  if (status != SL_STATUS_OK) {
    DEBUGOUT("Error Code: 0x%lX, Power State ps2 Transition Failed \n", status);
  }
  return status;
}

/********************************************************************************
 * This function implements a brief delay to simulate processing time or workload 
 * typically performed by a user application.
 *******************************************************************************/
static void delay(void)
{
  // Provides the delay of 5ms approximately.
  for (uint32_t x = 0; x < 46000; x++) {
    __NOP();
  }
}

/*******************************************************************************
 * @brief Dummy function for user-defined operations during PS3 state transitions.
 * This function is called when the system transitions from PS2 to PS3 state
 * to process wireless events. Users can add their custom logic here to handle
 * specific wireless events or perform other operations during the PS3 state.
 ******************************************************************************/
static void ps3_state_application_action(void)
{
  // TODO: Add your custom logic here
  // This is a placeholder function where users can implement their specific
  // wireless event handling logic during PS3 state transitions.
  delay();
}
