/*******************************************************************************
 * @file  ulp_gpio_transition_freertos.c
 * @brief  ULP GPIO state transition example
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
 * @brief : This file contains example application for GPIO example
 * @section Description :
 * This application demonstrates toggling, pin, group interrupts of GPIO pin
  for HP, ULP and UULP instances.
 ============================================================================**/
#include "ulp_gpio_transition_freertos.h"

#include <stdio.h>
#include <string.h>
#include "rsi_debug.h"
#include "sl_net.h"
#include "sl_si91x_driver.h"
#include "sl_wifi.h"
#include "sl_driver_gpio.h"
#include "sl_gpio_board.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_si91x_power_manager.h"
#include "sl_status.h"
#include "cmsis_os2.h"

/* Aliases used throughout this file; placed after headers that define sl_gpio_t. */
typedef sl_gpio_t sl_si91x_gpio_t;
typedef sl_gpio_mode_t sl_si91x_gpio_mode_t;

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PORT0                0      // PORT 0
#define AVL_INTR_NO          0      // available interrupt number
#define POLARITY             0      // Polarity for GPIO pin
#define AND_EVENT            0      // AND for group interrupt
#define LEVEL_EVENT          0      // level for group interrupt
#define INT_CH               0      // GPIO Pin interrupt 0
#define ULP_INT_CH           0      // ULP GPIO Pin interrupt 0
#define MODE_0               0      // Initializing GPIO MODE_0 value
#define PORT1                1      // PORT 1
#define EDGE_EVENT           1      // Edge for group interrupt
#define OR_EVENT             1      // OR for group interrupt
#define OUTPUT_VALUE         1      // GPIO output value
#define PIN_COUNT            2      // Number of interrupts needed
#define GRP_COUNT            2      // Count of group interrupt pins
#define FALL_EDGE            8      // Fall edge event
#define PAD_SELECT_9         9      // GPIO PAD selection number
#define MAX_PAD_SELECT       34     // Maximum GPIO PAD selection number
#define PINS                 0x0c40 // Pins in a port to mask
#define FIVE_SECOND_DELAY_MS 5000U  // osDelay ms when tick rate is 1 kHz
#define UULP_GPIO_INTR_2     2      // UULP GPIO pin interrupt 2
#define TOGGLE_COUNT         10     // Count for number of times to repeat

/** PS transition events (same mask as tickless-idle / ULP ADC FreeRTOS). */
#define PS_EVENT_MASK                                                                                           \
  (SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS4 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS4   \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS3 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS3 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_ENTERING_PS2 | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_PS2 \
   | SL_SI91X_POWER_MANAGER_EVENT_TRANSITION_LEAVING_SLEEP)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
// Enum for enabling GPIO instances functionality
typedef enum {
  ULP_GPIO_PIN        = 1, // Initialize GPIO ULP instance
  UULP_GPIO_PIN       = 0, // Initialize GPIO UULP instance
  ULP_GPIO_PIN_INTR   = 0, // Configure GPIO ULP instance pin interrupt
  ULP_GPIO_GROUP_INTR = 0, // Configure GPIO ULP instance group interrupt
  UULP_GPIO_PIN_INTR  = 0, // Configure GPIO UULP instance pin interrupt check
} gpio_instance_type_t;

typedef enum {
  SL_ULP_GPIO_PROCESS_ACTION,    // gpio process action mode
  SL_ULP_POWER_STATE_TRANSITION, // gpio power state transition mode
  SL_ULP_GPIO_TERMINATED,        // gpio termination mode
} ulp_gpio_current_mode_t;

static ulp_gpio_current_mode_t ulp_gpio_current_state = SL_ULP_GPIO_PROCESS_ACTION;
static sl_power_state_t current_power_state           = SL_SI91X_POWER_MANAGER_PS4;
/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
static sl_si91x_gpio_group_interrupt_config_t config_grp_int;
static uulp_pad_config_t uulp_pad;
static uint8_t gpio_toggle_count;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static sl_status_t gpio_driver_ulp_initialization(void);
static sl_status_t gpio_driver_uulp_initialization(void);
static void gpio_uulp_pin_interrupt_callback(uint32_t pin_intr);
static void gpio_ulp_pin_interrupt_callback(uint32_t pin_intr);
static void gpio_ulp_group_interrupt_callback(uint32_t pin_intr);
static sl_status_t configuring_ps2_power_state(void);
static void ulp_gpio_pm_transition_callback(sl_power_state_t from, sl_power_state_t to);
static sl_status_t initialize_wireless(void);
static void wireless_sleep(boolean_t sleep_with_retention);
static void ulp_gpio_freertos_task(void *argument);
static sl_status_t ulp_gpio_application_init(void);

/** Must outlive subscribe: PM stores pointers to these in a linked list. */
static sl_power_manager_ps_transition_event_handle_t s_ulp_gpio_pm_event_handle;
static const sl_power_manager_ps_transition_event_info_t s_ulp_gpio_pm_event_info = {
  .event_mask = PS_EVENT_MASK,
  .on_event   = ulp_gpio_pm_transition_callback,
};

static const osThreadAttr_t ulp_gpio_thread_attributes = {
  .name       = "ulp_gpio",
  .stack_size = 6144,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
void gpio_example_init(void)
{
  osThreadId_t tid = osThreadNew((osThreadFunc_t)ulp_gpio_freertos_task, NULL, &ulp_gpio_thread_attributes);
  if (tid == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create ULP GPIO transition thread\r\n");
  }
}

/*******************************************************************************
 * Power Manager PS transition callback (aligned with ULP ADC FreeRTOS / tickless idle).
 ******************************************************************************/
static void ulp_gpio_pm_transition_callback(sl_power_state_t from, sl_power_state_t to)
{
  switch (from) {
    case SL_SI91X_POWER_MANAGER_PS4:
      SL_PRINT_STRING_ERROR("Leaving PS4 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      SL_PRINT_STRING_ERROR("Leaving PS3 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      SL_PRINT_STRING_ERROR("Leaving PS2 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      SL_PRINT_STRING_ERROR("Leaving PS1 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_SLEEP:
      SL_PRINT_STRING_ERROR("Leaving Sleep State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_STANDBY:
      SL_PRINT_STRING_ERROR("Leaving Standby State \r\n");
      break;
    default:
      break;
  }

  switch (to) {
    case SL_SI91X_POWER_MANAGER_PS4:
      SL_PRINT_STRING_ERROR("Entering PS4 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS3:
      SL_PRINT_STRING_ERROR("Entering PS3 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS2:
      SL_PRINT_STRING_ERROR("Entering PS2 State \r\n");
      break;
    case SL_SI91X_POWER_MANAGER_PS1:
      SL_PRINT_STRING_ERROR("Entering PS1 State \r\n");
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * Wireless bring-up (same pattern as `ulp_adc_freertos.c` / tickless idle).
 ******************************************************************************/
static sl_status_t initialize_wireless(void)
{
  static const sl_wifi_device_configuration_t station_init_configuration = {
    .boot_option = LOAD_NWP_FW,
    .mac_address = NULL,
    .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
    .boot_config = { .oper_mode = SL_SI91X_CLIENT_MODE,
                     .coex_mode = SL_SI91X_WLAN_ONLY_MODE,
                     .feature_bit_map =
                       (SL_WIFI_FEAT_SECURITY_OPEN | SL_WIFI_FEAT_AGGREGATION | SL_SI91X_FEAT_ULP_GPIO_BASED_HANDSHAKE
#ifdef SLI_SI91X_MCU_INTERFACE
                        | SL_WIFI_FEAT_WPS_DISABLE
#endif
                        ),
                     .tcp_ip_feature_bit_map =
                       (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID),
                     .custom_feature_bit_map     = (SL_WIFI_SYSTEM_CUSTOM_FEAT_EXTENSION_VALID),
                     .ext_custom_feature_bit_map = (SL_WIFI_SYSTEM_EXT_FEAT_LOW_POWER_MODE | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                    | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#ifdef SLI_SI917
                                                    | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                    ),
                     .bt_feature_bit_map         = 0,
                     .ext_tcp_ip_feature_bit_map = SL_SI91X_CONFIG_FEAT_EXTENSION_VALID,
                     .ble_feature_bit_map        = 0,
                     .ble_ext_feature_bit_map    = 0,
                     .config_feature_bit_map = (SL_SI91X_FEAT_SLEEP_GPIO_SEL_BITMAP | SL_WIFI_ENABLE_ENHANCED_MAX_PSP) }
  };
  sl_status_t status;
  sl_wifi_firmware_version_t version = { 0 };
  sl_mac_address_t mac_addr          = { 0 };

  status = sl_net_init(SL_NET_WIFI_CLIENT_INTERFACE, &station_init_configuration, NULL, NULL);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Failed to start Wi-Fi Client interface: 0x%lx\r\n", status);
    return status;
  }
  status = sl_wifi_get_mac_address(SL_WIFI_CLIENT_INTERFACE, &mac_addr);
  if (status == SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Device MAC address: %x:%x:%x:", mac_addr.octet[0], mac_addr.octet[1], mac_addr.octet[2]);
    SL_PRINT_STRING_ERROR("%x:%x:%x\r\n", mac_addr.octet[3], mac_addr.octet[4], mac_addr.octet[5]);
  } else {
    SL_PRINT_STRING_ERROR("Failed to get mac address: 0x%lx\r\n", status);
  }
  status = sl_wifi_get_firmware_version(&version);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("\r\nFailed to fetch firmware version: 0x%lx\r\n", status);
  } else {
    print_firmware_version(&version);
  }
  wireless_sleep(true);
  return SL_STATUS_OK;
}

static void wireless_sleep(boolean_t sleep_with_retention)
{
  sl_status_t status;
  sl_wifi_performance_profile_v2_t ta_performance_profile = { .profile = HIGH_PERFORMANCE };

  status = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", status);
    return;
  }
  if (sleep_with_retention) {
    ta_performance_profile.profile = DEEP_SLEEP_WITH_RAM_RETENTION;
  } else {
    ta_performance_profile.profile = DEEP_SLEEP_WITHOUT_RAM_RETENTION;
  }
  status = sl_wifi_set_performance_profile_v2(&ta_performance_profile);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("sl_wifi_set_performance_profile_v2 failed, Error Code: 0x%lX \n", status);
  }
}

/*******************************************************************************
 * GPIO example initialization function. It initializes HP/ULP clock, pin mode,
 * direction and configure pin and group interrupts
 ******************************************************************************/
static sl_status_t ulp_gpio_application_init(void)
{
  sl_status_t status;
  uint8_t direction;
  sl_si91x_gpio_version_t version;

  status = SL_STATUS_OK;

  // Version information of gpio
  version = sl_si91x_gpio_driver_get_version();
  SL_PRINT_STRING_ERROR("gpio version is fetched successfully \n");
  SL_PRINT_STRING_ERROR("API version is %d.%d.%d\n", version.release, version.major, version.minor);
  do {
    // Initialize GPIO ULP instance
    if (ULP_GPIO_PIN == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN };
#ifndef SL_SI91X_ACX_MODULE
      sl_si91x_gpio_t port_pin = { SL_SI91X_ULP_GPIO_10_PORT, SL_SI91X_ULP_GPIO_10_PIN };
#else
      sl_si91x_gpio_t port_pin          = { SL_SI91X_ULP_GPIO_4_PORT, SL_SI91X_ULP_GPIO_4_PIN };
#endif
      sl_si91x_gpio_mode_t mode = MODE_0;
      // GPIO initialization function for ULP instance
      status = gpio_driver_ulp_initialization();
      if (status != SL_STATUS_OK) {
        break;
      }
      // Get the pin direction for ULP GPIO pin
      direction = sl_si91x_gpio_driver_get_pin_direction(gpio_port_pin.port, gpio_port_pin.pin);
      SL_PRINT_STRING_ERROR("get_pin_direction = %d\n", direction);
      direction = sl_si91x_gpio_driver_get_pin_direction(port_pin.port, port_pin.pin);
      SL_PRINT_STRING_ERROR("get_pin_direction = %d\n", direction);

      // Get the pin mode for ULP GPIO pin
      status = sl_gpio_driver_get_pin_mode(&gpio_port_pin, &mode);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver get pin mode is successful \n");
      SL_PRINT_STRING_ERROR("get_pin_mode = %d\n", mode);
      status = sl_gpio_driver_get_pin_mode(&port_pin, &mode);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver get pin mode is successful \n");
      SL_PRINT_STRING_ERROR("get_pin_mode = %d\n", mode);
    }

    // Configure GPIO ULP instance pin interrupt
    if (ULP_GPIO_PIN_INTR == SET) {
      // GPIO initialization function for ULP instance
      status = gpio_driver_ulp_initialization();
      if (status != SL_STATUS_OK) {
        break;
      }
      // Configure ULP GPIO pin interrupts
      status = sl_si91x_gpio_driver_configure_ulp_pin_interrupt(
        ULP_INT_CH,
        (sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_FALL_EDGE,
        SL_SI91X_ULP_GPIO_8_PIN,
        (sl_gpio_irq_callback_t)&gpio_ulp_pin_interrupt_callback);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_configure_ulp_pin_interrupt, Error code: %lu", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver configure ulp pin interrupt is successful \n");
    }

    // Configure GPIO ULP instance group interrupt
    if (ULP_GPIO_GROUP_INTR == SET) {
#ifndef SL_SI91X_ACX_MODULE
      uint8_t ulp_group_pins[PIN_COUNT] = { SL_SI91X_ULP_GPIO_8_PIN,
                                            SL_SI91X_ULP_GPIO_10_PIN }; // pins for group interrupt in IC boards
#else
      uint8_t ulp_group_pins[PIN_COUNT] = { SL_SI91X_ULP_GPIO_8_PIN,
                                            SL_SI91X_ULP_GPIO_4_PIN }; // pins for group interrupt in ACX Module boards
#endif
      uint8_t ulp_group_pol[PIN_COUNT] = { POLARITY, POLARITY }; // polarity selected for group interrupt

      // Configure ULP GPIO group parameters
      config_grp_int.grp_interrupt     = ULP_GROUP_INTR_0; // Set ULP group interrupt
      config_grp_int.grp_interrupt_cnt = GRP_COUNT;        // Count of group interrupt pins
      config_grp_int.and_or            = AND_EVENT;        // AND/OR of group interrupt
      config_grp_int.level_edge        = LEVEL_EVENT;      // Level/Edge of group interrupt
      memcpy(config_grp_int.grp_interrupt_pin, ulp_group_pins, PIN_COUNT);
      memcpy(config_grp_int.grp_interrupt_pol, ulp_group_pol, PIN_COUNT);

      // GPIO initialization function for ULP instance
      status = gpio_driver_ulp_initialization();
      if (status != SL_STATUS_OK) {
        break;
      }
      // Configure group interrupt for grp_config_int structure
      status =
        sl_si91x_gpio_driver_configure_ulp_group_interrupt(&config_grp_int,
                                                           (sl_gpio_irq_callback_t)&gpio_ulp_group_interrupt_callback);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_configure_ulp_group_interrupt, Error "
                              "code: %lu",
                              status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver configure ulp group interrupt is successful \n");
    }

    // Initialize GPIO UULP instance
    if (UULP_GPIO_PIN == SET) {
      // GPIO initialization function for UULP instance
      status = gpio_driver_uulp_initialization();
      if (status != SL_STATUS_OK) {
        break;
      }
      // Get NPSS GPIO pin direction
      direction = sl_si91x_gpio_driver_get_uulp_npss_direction(SL_SI91X_UULP_GPIO_0_PIN);
      SL_PRINT_STRING_ERROR("get_direction = %d\n", direction);
    }

    // Configure GPIO UULP instance pin interrupt
    if (UULP_GPIO_PIN_INTR == SET) {
      uulp_pad.gpio_padnum = SL_SI91X_UULP_GPIO_2_PIN; // UULP GPIO pin number 2 is selected
      uulp_pad.pad_select  = SET;                      // UULP GPIO PAD is selected
      uulp_pad.mode        = CLR;                      // UULP GPIO mode 0 is selected
      uulp_pad.direction   = SET;                      // UULP GPIO direction is selected
      uulp_pad.receiver    = SET;                      // UULP GPIO receiver is enabled

      // GPIO initialization function for UULP instance
      status = gpio_driver_uulp_initialization();
      if (status != SL_STATUS_OK) {
        break;
      }
      status = sl_si91x_gpio_driver_set_uulp_pad_configuration(&uulp_pad);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_pad_configuration, Error code: %lu", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver set uulp pad configuration is successful \n");
      // Configure pin interrupt for sl_si91x_gpio_interrupt_config_flag_t
      // structure
      status = sl_si91x_gpio_driver_configure_uulp_interrupt(
        (sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_RISE_EDGE,
        UULP_GPIO_INTR_2,
        (sl_gpio_irq_callback_t)&gpio_uulp_pin_interrupt_callback);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_configure_uulp_interrupt, Error code: %lu", status);
        break;
      }
      SL_PRINT_STRING_ERROR("GPIO driver configure uulp interrupt is successful \n");
    }
  } while (false);
  return status;
}
/*******************************************************************************
 * FreeRTOS task: PS4/PS2 GPIO flow with wireless bring-up and PM subscription,
 * matching `ulp_adc_freertos.c` (tickless-idle style sequencing).
 ******************************************************************************/
static void ulp_gpio_freertos_task(void *argument)
{
  sl_status_t status;

  (void)argument;

  status = initialize_wireless();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Wireless API initialization failed, Error Code: 0x%lX\r\n", status);
    osThreadExit();
  }
  SL_PRINT_STRING_ERROR("Wireless initialized; NWP in sleep with RAM retention\r\n");

  status = sl_si91x_power_manager_subscribe_ps_transition_event(&s_ulp_gpio_pm_event_handle, &s_ulp_gpio_pm_event_info);
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("Power Manager transition event subscription failed, Error Code: 0x%lX\r\n", status);
  } else {
    SL_PRINT_STRING_ERROR("Power Manager transition events subscribed\r\n");
  }

  status = ulp_gpio_application_init();
  if (status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("ULP GPIO application init failed, Error Code: 0x%lX\r\n", (unsigned long)status);
    osThreadExit();
  }

  while (1) {
    switch (ulp_gpio_current_state) {
      case SL_ULP_GPIO_PROCESS_ACTION:
        gpio_toggle_count = 0;
        do {
          if (ULP_GPIO_PIN == SET) {
            sl_si91x_gpio_t port_pin = { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN };
            status                   = sl_gpio_driver_toggle_pin(&port_pin);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_gpio_driver_toggle_pin, Error code: %lu\r\n", (unsigned long)status);
              break;
            }
          }
          if (UULP_GPIO_PIN == SET) {
            status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SL_SI91X_UULP_GPIO_0_PIN, SET);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: %lu\r\n",
                                    (unsigned long)status);
              break;
            }
            status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SL_SI91X_UULP_GPIO_0_PIN, CLR);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: %lu\r\n",
                                    (unsigned long)status);
              break;
            }
          }
        } while (gpio_toggle_count++ < TOGGLE_COUNT);
        ulp_gpio_current_state = SL_ULP_POWER_STATE_TRANSITION;
        break;

      case SL_ULP_POWER_STATE_TRANSITION:
        if (current_power_state == SL_SI91X_POWER_MANAGER_PS4) {
          SL_PRINT_STRING_ERROR("Switching GPIO from PS4->PS2 state \r\n");
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS2);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement (PS2): Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          DEBUGINIT();

          status = configuring_ps2_power_state();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("ULP GPIO: configuring_ps2_power_state failed, Error Code: 0x%lX\r\n",
                                  (unsigned long)status);
            osThreadExit();
          }
          current_power_state    = SL_SI91X_POWER_MANAGER_PS2;
          ulp_gpio_current_state = SL_ULP_GPIO_PROCESS_ACTION;
        } else if (current_power_state == SL_SI91X_POWER_MANAGER_PS2) {
          SL_PRINT_STRING_ERROR("Switching GPIO from PS2->PS4 state \r\n");
          status = sl_si91x_power_manager_add_ps_requirement(SL_SI91X_POWER_MANAGER_PS4);
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_power_manager_add_ps_requirement (PS4): Error Code : %lu \r\n",
                                  (unsigned long)status);
            break;
          }
          DEBUGINIT();

          current_power_state    = LAST_ENUM_POWER_STATE;
          ulp_gpio_current_state = SL_ULP_GPIO_PROCESS_ACTION;
        } else {
          status = sl_gpio_driver_deinit();
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_gpio_driver_deinit, Error code: %lu\r\n", (unsigned long)status);
            break;
          }
          SL_PRINT_STRING_ERROR("GPIO driver de-initialization is successful \r\n");
          ulp_gpio_current_state = SL_ULP_GPIO_TERMINATED;
        }
        break;

      case SL_ULP_GPIO_TERMINATED:
        (void)osDelay(1000);
        break;

      default:
        (void)osDelay(1000);
        break;
    }
  }
}

/*******************************************************************************
 * ULP GPIO initialization function
 ******************************************************************************/
static sl_status_t gpio_driver_ulp_initialization(void)
{
  sl_status_t status;
  sl_gpio_driver_init();
  sl_si91x_gpio_t gpio_port_pin1 = { SL_SI91X_ULP_GPIO_1_PORT, SL_SI91X_ULP_GPIO_1_PIN };
  sl_si91x_gpio_t gpio_port_pin2 = { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN };
  sl_si91x_gpio_t gpio_port_pin8 = { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN };
  sl_gpio_mode_t mode            = MODE_0;

  do {
    SL_PRINT_STRING_ERROR("\r\n ULP_GPIO_PIN test starts \r\n");
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver clock enable is successful \n");
    // Enable pad receiver for ULP GPIO pins
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_1_PIN);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver ulp pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_2_PIN);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver ulp pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_8_PIN);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver ulp pad receiver enable is successful \n");

    // Select pad driver strength for ULP GPIO pins
    status =
      sl_si91x_gpio_driver_select_ulp_pad_driver_strength(SL_SI91X_ULP_GPIO_1_PIN,
                                                          (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_ulp_pad_driver_strength, Error "
                            "code: %lu",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver ulp pad driver strength selection is successful \n");
    // Select pad driver disable state for ULP GPIO pins
    status = sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(SL_SI91X_ULP_GPIO_1_PIN,
                                                                      (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state, "
                            "Error code: %lu",
                            status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver ulp pad driver disable state selection is successful \n");
    // Set the pin mode for ULP GPIO pins.
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin1, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin mode is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin2, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin mode is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin8, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin mode is successful \n");

    // Set the pin direction for ULP GPIO pins.
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_1_PORT,
                                                    SL_SI91X_ULP_GPIO_1_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin direction is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_2_PORT,
                                                    SL_SI91X_ULP_GPIO_2_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin direction is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_8_PORT,
                                                    SL_SI91X_ULP_GPIO_8_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver set pin direction is successful \n");
  } while (false);
  return status;
}

/*******************************************************************************
 * UULP GPIO initialization function
 ******************************************************************************/
static sl_status_t gpio_driver_uulp_initialization(void)
{
  sl_status_t status;
  sl_gpio_driver_init();
  do {
    SL_PRINT_STRING_ERROR("\r\n UULP_GPIO_PIN test starts \r\n");
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_enable_clock, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver clock enable is successful \n");
    // By default making all the interrupts zero.
    GPIO_NPSS_GPIO_CONFIG_REG = CLR;
    // Set NPSS GPIO input buffer
    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(SL_SI91X_UULP_GPIO_0_PIN, SET);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_uulp_npss_receiver, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver uulp receiver selection is successful \n");
    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(SL_SI91X_UULP_GPIO_2_PIN, SET);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_select_uulp_npss_receiver, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver uulp receiver selection is successful \n");
    // Set NPSS GPIO pin MUX
    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SL_SI91X_UULP_GPIO_0_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver uulp pin mux selection is successful \n");
    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SL_SI91X_UULP_GPIO_2_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver uulp pin mux selection is successful \n");
    // Set NPSS GPIO pin direction
    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(SL_SI91X_UULP_GPIO_0_PIN, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO driver uulp pin direction selection is successful \n");
    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(SL_SI91X_UULP_GPIO_2_PIN, (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: %lu", status);
      break;
    }
    SL_PRINT_STRING_ERROR("GPIO uulp driver clear pin direction selection is successful \n");
  } while (false);
  return status;
}

/*******************************************************************************
 *  This API handles UULP GPIO pin interrupt 0 request
 ******************************************************************************/
static void gpio_uulp_pin_interrupt_callback(uint32_t pin_intr)
{
  if (pin_intr == UULP_GPIO_INTR_2) {
    // This is with respect to ISR context. SL_PRINT_STRING_ERROR might cause issues
    // sometimes.
    SL_PRINT_STRING_ERROR("gpio uulp pin interrupt2\n");
  }
}

/*******************************************************************************
 *  This API handles ULP GPIO OR'ed pin interrupt request
 ******************************************************************************/
static void gpio_ulp_pin_interrupt_callback(uint32_t pin_intr)
{
  if (pin_intr == ULP_PIN_INTR_0) {
    // This is with respect to ISR context. SL_PRINT_STRING_ERROR might cause issues
    // sometimes.
    SL_PRINT_STRING_ERROR("gpio ulp pin interrupt0\n");
  }
}

/*******************************************************************************
 *  This API handles ULP GPIO group interrupt request
 ******************************************************************************/
static void gpio_ulp_group_interrupt_callback(uint32_t pin_intr)
{
  if (pin_intr == ULP_GROUP_INTR_0) {
    // This is with respect to ISR context. SL_PRINT_STRING_ERROR might cause issues
    // sometimes.
    SL_PRINT_STRING_ERROR("gpio ulp group interrupt0\n");
  }
}
/*******************************************************************************
 * PS2 peripheral RAM retention (aligned with `ulp_adc_freertos.c`).
 ******************************************************************************/
static sl_status_t configuring_ps2_power_state(void)
{
  sl_power_peripheral_t peri;
  sl_power_ram_retention_config_t config;
  sl_status_t cfg_status;

  cfg_status                 = SL_STATUS_OK;
  config.configure_ram_banks = true;
  /* Same rationale as ULP ADC FreeRTOS: Wi-Fi + FreeRTOS + RAM execution can map code/stack/heap
   * into M4SS banks 8–10; powering those banks down in PS2 can halt the CPU. */
  config.m4ss_ram_banks  = 0;
  config.ulpss_ram_banks = 0;
  peri.m4ss_peripheral   = 0;
  peri.ulpss_peripheral  = SL_SI91X_POWER_MANAGER_ULPSS_PG_MISC | SL_SI91X_POWER_MANAGER_ULPSS_PG_SSI
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2S | SL_SI91X_POWER_MANAGER_ULPSS_PG_I2C
                          | SL_SI91X_POWER_MANAGER_ULPSS_PG_IR | SL_SI91X_POWER_MANAGER_ULPSS_PG_FIM;
  peri.npss_peripheral = SL_SI91X_POWER_MANAGER_NPSS_PG_MCURTC | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUWDT
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUPS | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUTS
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE2 | SL_SI91X_POWER_MANAGER_NPSS_PG_MCUSTORE3
                         | SL_SI91X_POWER_MANAGER_NPSS_PG_TIMEPERIOD;
  do {
    cfg_status = sl_si91x_power_manager_remove_peripheral_requirement(&peri);
    if (cfg_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_remove_peripheral_requirement failed, Error Code: 0x%lX\r\n",
                            (unsigned long)cfg_status);
      break;
    }
    cfg_status = sl_si91x_power_manager_configure_ram_retention(&config);
    if (cfg_status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_power_manager_configure_ram_retention failed, Error Code: 0x%lX\r\n",
                            (unsigned long)cfg_status);

      break;
    }

  } while (false);
  return cfg_status;
}
