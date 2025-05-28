/***************************************************************************/
/**
 * @file power_manager_test_main.c
 * @brief Power Manager Unity Test Functions
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
#include "sl_si91x_power_manager.h"
#include "sl_si91x_clock_manager.h"
#include "power_manager_m4_wireless_example.h"
#include "rsi_debug.h"
#include "sl_si91x_calendar.h"
#include "unity.h"
#include "cmsis_os2.h"
#include "sl_wifi.h"
#include "rsi_m4.h"
#include "sl_si91x_driver.h"
#include "sl_net.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define TEST_PS_EVENT_MASK                                                                                      \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP) // Ored value of event for which callback is subscribed

/*******************************************************************************
 ************************  Local Function prototypes    ************************
 ******************************************************************************/
static void test_transition_callback(sl_power_state_t from, sl_power_state_t to);
static void on_sec_callback(void);
static void test_application_start(void *argument);
static sl_status_t test_initialize_wireless(void);
static void test_wireless_sleep(boolean_t sleep_with_retention);
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
sl_power_peripheral_t peripheral;
boolean_t add = true;
// Thread attributes which are required for creating a thread
const osThreadAttr_t test_thread_attributes = {
  .name       = "test",
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
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
//Power Manager API's
void test_power_manager_init(void);
void test_power_manager_core_entercritical(void);
void test_power_manager_core_exitcritical(void);
void test_power_manager_add_ps_requirement(void);
void test_power_manager_remove_ps_requirement(void);
void test_power_manager_set_clock_scaling(void);
void test_power_manager_get_clock_scaling(void);
void test_power_manager_add_peripheral_requirement(void);
void test_power_manager_remove_peripheral_requirement(void);
void test_power_manager_subscribe_ps_transition_event(void);
void test_power_manager_unsubscribe_ps_transition_event(void);
void test_power_manager_sleep(void);
void test_power_manager_standby(void);
void test_power_manager_set_wakeup_sources(void);
void test_power_manager_configure_ram_retention(void);
void test_power_manager_get_current_state(void);
void test_power_manager_get_requirement_table(void);
void test_power_manager_deinit(void);
void test_get_lowest_ps(void);
void test_power_manager_is_ok_to_sleep(void);

//Clock Manager API's
void test_clock_manager_init(void);
void test_clock_manager_m4_set_core_clk(void);
void test_clock_manager_set_pll_freq(void);
void test_clock_manager_m4_get_core_clk_src_freq(void);
void test_clock_manager_get_pll_freq(void);
void test_clock_manager_control_pll(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
void test_power_manager_example_init()
{
  // Initialization of thread with the application_start function
  osThreadNew((osThreadFunc_t)test_application_start, NULL, &test_thread_attributes);
}

static void test_application_start(void *argument)
{
  UNUSED_VARIABLE(argument);
  // Initialize the wireless interface and put the NWP in Standby with RAM retention mode.
  test_initialize_wireless();

  UnityBeginGroup("Power Manager");

  RUN_TEST(test_power_manager_init, __LINE__);
  RUN_TEST(test_power_manager_core_entercritical, __LINE__);
  RUN_TEST(test_power_manager_core_exitcritical, __LINE__);
  RUN_TEST(test_power_manager_add_ps_requirement, __LINE__);
  RUN_TEST(test_power_manager_set_clock_scaling, __LINE__);
  RUN_TEST(test_power_manager_get_clock_scaling, __LINE__);
  RUN_TEST(test_power_manager_add_peripheral_requirement, __LINE__);
  RUN_TEST(test_power_manager_subscribe_ps_transition_event, __LINE__);
  RUN_TEST(test_power_manager_get_current_state, __LINE__);
  RUN_TEST(test_power_manager_get_requirement_table, __LINE__);
  RUN_TEST(test_get_lowest_ps, __LINE__);
  RUN_TEST(test_power_manager_is_ok_to_sleep, __LINE__);
  RUN_TEST(test_power_manager_configure_ram_retention, __LINE__);
  RUN_TEST(test_power_manager_set_wakeup_sources, __LINE__);
  RUN_TEST(test_power_manager_sleep, __LINE__);
  RUN_TEST(test_power_manager_standby, __LINE__);
  RUN_TEST(test_power_manager_unsubscribe_ps_transition_event, __LINE__);
  RUN_TEST(test_power_manager_remove_peripheral_requirement, __LINE__);
  RUN_TEST(test_power_manager_remove_ps_requirement, __LINE__);
  RUN_TEST(test_power_manager_deinit, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  UnityPrintf("\n\n");

  UnityBeginGroup("Clock Manager");

  RUN_TEST(test_clock_manager_init, __LINE__);
  RUN_TEST(test_clock_manager_m4_set_core_clk, __LINE__);
  RUN_TEST(test_clock_manager_set_pll_freq, __LINE__);
  RUN_TEST(test_clock_manager_m4_get_core_clk_src_freq, __LINE__);
  RUN_TEST(test_clock_manager_get_pll_freq, __LINE__);
  RUN_TEST(test_clock_manager_control_pll, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Initialization of wireless APIs.
 * M4-NWP handshake is performed.
 * NWP is send to standby with RAM retention mode if SWITCH_TO_PS0 is disabled.
 * NWP is send to standby without RAM retention mode if SWITCH_TO_PS0 is enabled.
 ******************************************************************************/
static sl_status_t test_initialize_wireless(void)
{
  // For M4-sleep wakeup, and to achieve minimum current in powersave application,
  // wifi is initialized, handshake is performed between M4 and NWP, then
  // NWP powersave profile is updated sleep with/without retention as per
  // requirements.
  // Wifi device configuration
  const sl_wifi_device_configuration_t client_init_configuration = {
    .boot_option = LOAD_NWP_FW,
    .mac_address = NULL,
    .band        = SL_WIFI_BAND_MODE_2_4GHZ,
    .region_code = SL_WIFI_REGION_US,
    .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                     .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                     .feature_bit_map =
                       (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_WPS_DISABLE | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE),
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
  // Initialize the wifi interface.
  sl_wifi_init(&client_init_configuration, NULL, NULL);
  DEBUGINIT();

  uint8_t xtal_enable = 1;
  // M4-NWP handshake is required for NWP communication.
  sl_si91x_m4_ta_secure_handshake(SL_SI91X_ENABLE_XTAL, 1, &xtal_enable, 0, NULL);
  // Wireless Sleep with ram retention
  test_wireless_sleep(true);
  // If reaches here, returns SL_STATUS_OK.
  return SL_STATUS_OK;
}

/*******************************************************************************
 * After PS2 to PS4 transition, flash is initialized and to initialize flash
 * wireless processor is set to active mode.
 * This function sends the wireless processor to sleep with retention.
 ******************************************************************************/
static void test_wireless_sleep(boolean_t sleep_with_retention)
{
  // Wifi Profile (NWP Mode) is set to High Performance.
  sl_wifi_performance_profile_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  sl_wifi_set_performance_profile(&ta_performance_profile);

  if (sleep_with_retention) {
    // Wifi Profile (NWP Mode) is set to standby power save with RAM retention.
    ta_performance_profile.profile = DEEP_SLEEP_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  }
  // Wifi Profile (NWP Mode) is set to standby power save with RAM retention.
  sl_wifi_set_performance_profile(&ta_performance_profile);
}

/*******************************************************************************
 * Function to test initialization of Power Manager
 ******************************************************************************/
void test_power_manager_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager Init \n");
  sl_status_t status;

  UnityPrintf("Testing Reinitialized power manager \n");
  status = sl_si91x_power_manager_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_ALREADY_INITIALIZED, status);
  UnityPrintf("Status of API is correct, Power Manager Reinitialized successfully \n");

  UnityPrintf("Power Manager Init test completed \n");
}

/*******************************************************************************
 * Function to test entering critical section
 ******************************************************************************/
void test_power_manager_core_entercritical(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager Core Enter Critical \n");

  UnityPrintf("Testing with correct parameters \n");
  sl_si91x_power_manager_core_entercritical();
  UnityPrintf("Power Manager Core Enter Critical successfully \n");

  UnityPrintf("Power Manager Core Enter Critical test completed \n");
}

/*******************************************************************************
 * Function to test exiting critical section
 ******************************************************************************/
void test_power_manager_core_exitcritical(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager Core Exit Critical \n");

  UnityPrintf("Testing with correct parameters \n");
  sl_si91x_power_manager_core_exitcritical();
  UnityPrintf("Power Manager Core Exit Critical successfully \n");

  UnityPrintf("Power Manager Core Exit Critical test completed \n");
}

/*******************************************************************************
 * Function to test add a requirement on power states.
 ******************************************************************************/
void test_power_manager_add_ps_requirement(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager add ps requirement \n");
  sl_status_t status;
  sl_power_state_t get_current_state;

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(LAST_ENUM_POWER_STATE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_SLEEP);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_STANDBY);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager add ps requirement for PS4 successfully \n");

  get_current_state = sl_si91x_power_manager_get_current_state();
  TEST_ASSERT_EQUAL_HEX(SL_SI91X_POWER_MANAGER_PS4, get_current_state);
  UnityPrintf("Power Manager get current state for PS4 successfully \n");

  sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager add ps requirement for PS3 successfully \n");

  get_current_state = sl_si91x_power_manager_get_current_state();
  TEST_ASSERT_EQUAL_HEX(SL_SI91X_POWER_MANAGER_PS3, get_current_state);
  UnityPrintf("Power Manager get current state for PS3 successfully \n");

  sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager add ps requirement for PS2 successfully \n");

  get_current_state = sl_si91x_power_manager_get_current_state();
  TEST_ASSERT_EQUAL_HEX(SL_SI91X_POWER_MANAGER_PS2, get_current_state);
  UnityPrintf("Power Manager get current state for PS2 successfully \n");

  sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);

  status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);

  UnityPrintf("Power Manager add ps requirement test completed \n");
}

/*******************************************************************************
 * Function to test remove a requirement on power states.
 ******************************************************************************/
void test_power_manager_remove_ps_requirement(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager remove ps requirement \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(LAST_ENUM_POWER_STATE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_SLEEP);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_STANDBY);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Tested with invalid parameter successfully \n");

  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS0);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS0);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove ps requirement successfully \n");

  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS1);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove ps requirement successfully \n");

  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove ps requirement successfully \n");

  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove ps requirement successfully \n");

  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove ps requirement successfully \n");

  UnityPrintf("Power Manager remove ps requirement test completed \n");
}

/*******************************************************************************
 * Function to test configure the clock scaling.
 ******************************************************************************/
void test_power_manager_set_clock_scaling(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager set the clock scaling \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_set_clock_scaling(LAST_ENUM_CLOCK_SCALING);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_PERFORMANCE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager set the clock scaling successfully \n");

  TEST_ASSERT_EQUAL_HEX(180000000UL, SystemCoreClock);
  UnityPrintf("Power Manager system core clock validate successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_POWERSAVE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager set the clock scaling successfully \n");

  TEST_ASSERT_EQUAL_HEX(100000000UL, SystemCoreClock);
  UnityPrintf("Power Manager system core clock validate successfully \n");

  sl_si91x_power_manager_remove_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
  sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS3);

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_PERFORMANCE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager set the clock scaling successfully \n");

  TEST_ASSERT_EQUAL_HEX(80000000UL, SystemCoreClock);
  UnityPrintf("Power Manager system core clock validate successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_set_clock_scaling(SL_SI91X_POWER_MANAGER_POWERSAVE);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager set the clock scaling successfully \n");

  TEST_ASSERT_EQUAL_HEX(40000000UL, SystemCoreClock);
  UnityPrintf("Power Manager system core clock validate successfully \n");

  UnityPrintf("Power Manager set the clock scaling test completed \n");
}

/*******************************************************************************
 * Function to test get the clock scaling.
 ******************************************************************************/
void test_power_manager_get_clock_scaling(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager get the clock scaling \n");
  sl_clock_scaling_t get_clock_scaling;

  UnityPrintf("Testing with correct parameters \n");
  get_clock_scaling = sl_si91x_power_manager_get_clock_scaling();
  TEST_ASSERT_EQUAL_HEX(SL_SI91X_POWER_MANAGER_POWERSAVE, get_clock_scaling);
  UnityPrintf("Power Manager get the clock scaling successfully \n");

  UnityPrintf("Power Manager get the clock scaling test completed \n");
}

/*******************************************************************************
 * Function to test add peripheral requirement.
 ******************************************************************************/
void test_power_manager_add_peripheral_requirement(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager add peripheral requirement \n");
  sl_status_t status;
  sl_power_peripheral_t peripheral;

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_add_peripheral_requirement(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  peripheral.m4ss_peripheral = 0xF;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.npss_peripheral = 0xF;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.ulpss_peripheral = 0xF;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_add_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.m4ss_peripheral  = 0x466A10;
  peripheral.npss_peripheral  = 0x107FE;
  peripheral.ulpss_peripheral = 0x1FEC0000;
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_add_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager add peripheral requirement successfully \n");

  UnityPrintf("Power Manager add peripheral requirement test completed \n");
}

/*******************************************************************************
 * Function to test remove peripheral requirement.
 ******************************************************************************/
void test_power_manager_remove_peripheral_requirement(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager remove peripheral requirement \n");
  sl_status_t status;
  sl_power_peripheral_t peripheral;

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_remove_peripheral_requirement(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  peripheral.m4ss_peripheral = 0xF;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.npss_peripheral = 0x107FE + 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.ulpss_peripheral = 0x1FEC0000 + 1;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_remove_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  peripheral.m4ss_peripheral  = 0;
  peripheral.npss_peripheral  = 0;
  peripheral.ulpss_peripheral = 0;
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_remove_peripheral_requirement(&peripheral);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager remove peripheral requirement successfully \n");

  UnityPrintf("Power Manager remove peripheral requirement test completed \n");
}

/*******************************************************************************
 * Function to test register a callback.
 ******************************************************************************/
void test_power_manager_subscribe_ps_transition_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager register a callback \n");
  sl_status_t status;
  sl_power_manager_ps_transition_event_handle_t test_handle;
  sl_power_manager_ps_transition_event_info_t test_info = { .event_mask = TEST_PS_EVENT_MASK,
                                                            .on_event   = test_transition_callback };

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_subscribe_ps_transition_event(NULL, &test_info);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_subscribe_ps_transition_event(&test_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_subscribe_ps_transition_event(&test_handle, &test_info);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager register a callback successfully \n");

  UnityPrintf("Power Manager register a callback test completed \n");
}

/*******************************************************************************
 * Function to test un-register a callback.
 ******************************************************************************/
void test_power_manager_unsubscribe_ps_transition_event(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager unregister a callback \n");
  sl_status_t status;
  sl_power_manager_ps_transition_event_handle_t test_handle;
  sl_power_manager_ps_transition_event_info_t test_info = { .event_mask = TEST_PS_EVENT_MASK,
                                                            .on_event   = test_transition_callback };

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_unsubscribe_ps_transition_event(NULL, &test_info);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_unsubscribe_ps_transition_event(&test_handle, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_unsubscribe_ps_transition_event(&test_handle, &test_info);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager unregister a callback successfully \n");

  UnityPrintf("Power Manager unregister a callback test completed \n");
}

/*******************************************************************************
 * Function to test move into sleep mode.
 ******************************************************************************/
void test_power_manager_sleep(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager move into sleep mode \n");
  sl_status_t status;

  status = sl_si91x_power_manager_sleep();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager move into sleep mode successfully \n");
  // Calendar clock is stopped as it is not needed.
  sl_si91x_calendar_rtc_stop();
  // Second trigger callback is unregistered and disabled.
  status = sl_si91x_calendar_unregister_sec_trigger_callback();
  //Clear wakeup source;
  sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_SEC_WAKEUP, false);

  UnityPrintf("Power Manager move into sleep mode test completed \n");
}

/*******************************************************************************
 * Function to test move into standby mode.
 ******************************************************************************/
void test_power_manager_standby(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager move into standby mode \n");

  //  sl_si91x_power_manager_standby();
  UnityPrintf("Power Manager move into standby mode successfully \n");

  UnityPrintf("Power Manager move into standby mode test completed \n");
}

/*******************************************************************************
 * Function to test set wakeup source.
 ******************************************************************************/
void test_power_manager_set_wakeup_sources(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager set wakeup source \n");
  sl_status_t status;
  uint32_t src = 0;

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_set_wakeup_sources(src, true);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameters successfully \n");

  sl_si91x_calendar_rtc_stop();
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_set_wakeup_sources(SL_SI91X_POWER_MANAGER_SEC_WAKEUP, true);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager set wakeup source successfully \n");
  // Calendar is initialized.
  sl_si91x_calendar_init();
  // Second trigger callback is configured.
  status = sl_si91x_calendar_register_sec_trigger_callback(on_sec_callback);

  UnityPrintf("Power Manager set wakeup source test completed \n");
}

/*******************************************************************************
 * Function to test set ram retention.
 ******************************************************************************/
void test_power_manager_configure_ram_retention(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager configure ram retention \n");
  sl_status_t status;
  sl_power_ram_retention_config_t config = { 0 };
  config.configure_ram_banks             = false;

  UnityPrintf("Testing with null parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Testing with null parameter successfully \n");

  config.m4ss_ram_size_kb = 193;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  config.m4ss_ram_size_kb  = 170;
  config.ulpss_ram_size_kb = 9;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  config.configure_ram_banks = true;
  config.ulpss_ram_size_kb   = 7;
  config.m4ss_ram_banks      = 0x500;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  config.ulpss_ram_banks = 0xF;
  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  config.m4ss_ram_banks    = 0x0;
  config.ulpss_ram_banks   = 0x0;
  config.ulpss_ram_size_kb = 8;
  config.m4ss_ram_size_kb  = 192;
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager configure ram retention successfully \n");

  config.configure_ram_banks = false;
  config.m4ss_ram_banks      = 0x0;
  config.ulpss_ram_banks     = 0x0;
  config.ulpss_ram_size_kb   = 7;
  config.m4ss_ram_size_kb    = 192;
  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_power_manager_configure_ram_retention(&config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Power Manager configure ram retention successfully \n");

  UnityPrintf("Power Manager configure ram retention test completed \n");
}

/*******************************************************************************
 * Function to test get current state.
 ******************************************************************************/
void test_power_manager_get_current_state(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager get current state \n");
  sl_power_state_t get_current_state;

  get_current_state = sl_si91x_power_manager_get_current_state();
  TEST_ASSERT_EQUAL_HEX(SL_SI91X_POWER_MANAGER_PS3, get_current_state);
  UnityPrintf("Power Manager get current state successfully \n");

  UnityPrintf("Power Manager get current state test completed \n");
}

/*******************************************************************************
 * Function to test get requirement table.
 ******************************************************************************/
void test_power_manager_get_requirement_table(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager get requirement table \n");
  uint8_t *get_table;

  get_table = sl_si91x_power_manager_get_requirement_table();
  UnityPrintf("PS4 = %d \n", get_table[0]);
  UnityPrintf("PS3 = %d \n", get_table[1]);
  UnityPrintf("PS2 = %d \n", get_table[2]);
  UnityPrintf("PS1 = %d \n", get_table[3]);
  UnityPrintf("PS0 = %d \n", get_table[4]);
  UnityPrintf("Power Manager get requirement table successfully \n");

  UnityPrintf("Power Manager get requirement table test completed \n");
}

/*******************************************************************************
 * Function to test get lowest PS.
 ******************************************************************************/
void test_get_lowest_ps(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager get lowest PS \n");
  sl_power_state_t get_lowest_ps;

  get_lowest_ps = sl_si91x_get_lowest_ps();
  UnityPrintf("Lowest PS = %d \n", get_lowest_ps);
  UnityPrintf("Power Manager get lowest PS successfully \n");

  UnityPrintf("Power Manager get lowest PS test completed \n");
}

/*******************************************************************************
 * Function to test is ok to sleep.
 ******************************************************************************/
void test_power_manager_is_ok_to_sleep(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager is ok to sleep \n");
  boolean_t is_sleep_ready;

  is_sleep_ready = sl_si91x_power_manager_is_ok_to_sleep();
  UnityPrintf("Lowest PS = %d \n", is_sleep_ready);
  UnityPrintf("Power Manager is ok to sleep successfully \n");

  UnityPrintf("Power Manager is ok to sleep test completed \n");
}

/*******************************************************************************
 * Function to test de-initialize power manager.
 ******************************************************************************/
void test_power_manager_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Power Manager deinit \n");

  sl_si91x_power_manager_deinit();
  UnityPrintf("Power Manager deinit successfully \n");

  UnityPrintf("Power Manager deinit test completed \n");
}

/*******************************************************************************
 * Function to test initialization of Clock Manager
 ******************************************************************************/
void test_clock_manager_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager Init \n");
  sl_status_t status;

  UnityPrintf("Testing initialized clock manager \n");
  status = sl_si91x_clock_manager_init();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager initialized successfully \n");

  UnityPrintf("Clock Manager Init test completed \n");
}

/*******************************************************************************
 * Function to test configure the M4 core clock source
 ******************************************************************************/
void test_clock_manager_m4_set_core_clk(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager set M4 core clock \n");
  sl_status_t status;

  UnityPrintf("Testing with invalid parameter \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(6, PLL_REF_CLK_VAL_XTAL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, testing invalid parameter successfully \n");

  UnityPrintf("Testing with invalid parameter \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(M4_ULPREFCLK, MAX_PLL_FREQUENCY + 1);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, testing invalid parameter successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(M4_ULPREFCLK, PLL_REF_CLK_VAL_XTAL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set M4 core clock successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(M4_SOCPLLCLK, MAX_PLL_FREQUENCY);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set M4 core clock successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_m4_set_core_clk(M4_INTFPLLCLK, MAX_PLL_FREQUENCY);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set M4 core clock successfully \n");

  // Setting back to default state.
  status = sl_si91x_clock_manager_m4_set_core_clk(M4_ULPREFCLK, PLL_REF_CLK_VAL_XTAL);

  UnityPrintf("Clock Manager set M4 core clock test completed \n");
}

/*******************************************************************************
 * Function to test set pll frequency.
 ******************************************************************************/
void test_clock_manager_set_pll_freq(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager set pll frequency \n");
  sl_status_t status;
  uint32_t pll_ref_clk = XTAL_CLK_FREQ;

  UnityPrintf("Testing with invalid parameter \n");
  status = sl_si91x_clock_manager_set_pll_freq(I2S_PLL, MAX_PLL_FREQUENCY + 1, pll_ref_clk);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, testing invalid parameter successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_set_pll_freq(I2S_PLL, MAX_PLL_FREQUENCY, pll_ref_clk);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set M4 core clock successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_set_pll_freq(SOC_PLL, MAX_PLL_FREQUENCY, pll_ref_clk);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set M4 core clock successfully \n");

  UnityPrintf("Testing correct parameter \n");
  status = sl_si91x_clock_manager_set_pll_freq(INTF_PLL, MAX_PLL_FREQUENCY, pll_ref_clk);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager set pll frequency successfully \n");

  UnityPrintf("Clock Manager set pll frequency test completed \n");
}

/*******************************************************************************
 * Function to test get core clock.
 ******************************************************************************/
void test_clock_manager_m4_get_core_clk_src_freq(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager get core clock \n");
  sl_si91x_m4_soc_clk_src_sel_t get_core_clk;
  uint32_t get_core_clk_freq;

  get_core_clk = sl_si91x_clock_manager_m4_get_core_clk_src_freq(&get_core_clk_freq);
  UnityPrintf("Core clock Frequency = %ld \n", get_core_clk_freq);
  UnityPrintf("Core clock = %ld \n", get_core_clk);
  UnityPrintf("Status of API is correct, Clock Manager get core clock successfully \n");

  UnityPrintf("Clock Manager get core clock test completed \n");
}

/*******************************************************************************
 * Function to test get pll frequency.
 ******************************************************************************/
void test_clock_manager_get_pll_freq(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager get pll frequency \n");
  uint32_t get_pll_freq;

  get_pll_freq = sl_si91x_clock_manager_get_pll_freq(INTF_PLL);
  UnityPrintf("PLL Frequency = %ld \n", get_pll_freq);
  UnityPrintf("Status of API is correct, Clock Manager get pll frequency successfully \n");

  UnityPrintf("Clock Manager get pll frequency test completed \n");
}

/*******************************************************************************
 * Function to test Controls the selected PLL clock.
 ******************************************************************************/
void test_clock_manager_control_pll(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing Clock Manager control PLL \n");
  sl_status_t status;
  boolean_t enable = 1;

  UnityPrintf("Testing with invalid parameter \n");
  status = sl_si91x_clock_manager_control_pll(3, enable);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Testing with invalid parameter successfully \n");

  UnityPrintf("Testing with correct parameter \n");
  status = sl_si91x_clock_manager_control_pll(I2S_PLL, enable);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager control PLL successfully \n");

  UnityPrintf("Testing with correct parameter \n");
  status = sl_si91x_clock_manager_control_pll(SOC_PLL, enable);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager control PLL successfully \n");

  UnityPrintf("Testing with correct parameter \n");
  status = sl_si91x_clock_manager_control_pll(INTF_PLL, enable);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Clock Manager control PLL successfully \n");

  UnityPrintf("Clock Manager control PLL test completed \n");
}

/*******************************************************************************
 * Callback function for state transition.
 * Prints the state transition with the parameters from and to.
 ******************************************************************************/
static void test_transition_callback(sl_power_state_t from, sl_power_state_t to)
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
 * Second trigger callback.
 * Interrupt Clearing is handled in driver layer, so this function is defined
 * to avoid undefined function while registering callback.
 ******************************************************************************/
static void on_sec_callback(void)
{
}
