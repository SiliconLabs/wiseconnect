/*******************************************************************************
 * @file  gpio_driver_example.c
 * @brief  gpio driver example
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "rsi_board.h"
#include "gpio_driver_example.h"
#include "sl_si91x_driver_gpio.h"
#include "sl_gpio_board.h"
#include "sl_driver_gpio.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PORT0            0      // PORT 0
#define AVL_INTR_NO      0      // available interrupt number
#define POLARITY         0      // Polarity for GPIO pin
#define AND_EVENT        0      // AND for group interrupt
#define LEVEL_EVENT      0      // level for group interrupt
#define INT_CH           0      // GPIO Pin interrupt 0
#define MODE_0           0      // Initializing GPIO MODE_0 value
#define PORT1            1      // PORT 1
#define EDGE_EVENT       1      // Edge for group interrupt
#define OR_EVENT         1      // OR for group interrupt
#define OUTPUT_VALUE     1      // GPIO output value
#define PIN_COUNT        2      // Number of interrupts needed
#define GRP_COUNT        2      // Count of group interrupt pins
#define FALL_EDGE        8      // Fall edge event
#define PAD_SELECT_9     9      // GPIO PAD selection number
#define MAX_PAD_SELECT   34     // Maximum GPIO PAD selection number
#define PINS             0x0c40 // Pins in a port to mask
#define DELAY            1000   // Delay for 1sec
#define MS_DELAY_COUNTER 4600   // Delay count

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
// Enum for enabling GPIO instances functionality
typedef enum {
  M4_GPIO_PIN         = 1, // Initialize GPIO HP instance
  ULP_GPIO_PIN        = 0, // Initialize GPIO ULP instance
  UULP_GPIO_PIN       = 0, // Initialize GPIO UULP instance
  M4_GPIO_PIN_INTR    = 0, // Configure GPIO HP instance pin interrupt
  M4_GPIO_GROUP_INTR  = 0, // Configure GPIO HP instance group interrupt
  M4_GPIO_PORT        = 0, // Set GPIO port output
  ULP_GPIO_PIN_INTR   = 0, // Configure GPIO ULP instance pin interrupt
  ULP_GPIO_GROUP_INTR = 0, // Configure GPIO ULP instance group interrupt
  UULP_GPIO_PIN_INTR  = 0, // Configure GPIO UULP instance pin interrupt
} gpio_instance_type_t;

// Enum for UULP GPIO group interrupt
typedef enum {
  UULP_GPIO_PIN0 = (1 << 0), // UULP GPIO pin 1 interrupt
  UULP_GPIO_PIN1 = (1 << 1), // UULP GPIO pin 2 interrupt
  UULP_GPIO_PIN2 = (1 << 2), // UULP GPIO pin 3 interrupt
  UULP_GPIO_PIN3 = (1 << 3), // UULP GPIO pin 4 interrupt
} sl_si91x_uulp_gpio_grp_intr_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
static sl_si91x_gpio_group_interrupt_config_t config_grp_int;
uint8_t pad_sel = 1;
uint32_t i;
static uulp_pad_config_t uulp_pad;
typedef sl_gpio_t sl_si91x_gpio_t;
typedef sl_gpio_mode_t sl_si91x_gpio_mode_t;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void delay(uint32_t idelay);
static void sl_gpio_driver_initialization(void);
static void sl_gpio_driver_ulp_initialization(void);
static void sl_gpio_driver_uulp_initialization(void);
static void gpio_pin_interrupt0_callback(void);
static void gpio_group_interrupt0_callback(void);
static void gpio_uulp_pin_interrupt_callback(void);
static void gpio_ulp_pin_interrupt_callback(void);
static void gpio_ulp_group_interrupt_callback(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * GPIO example initialization function. It initializes HP/ULP clock, pin mode,
 * direction and configure pin and group interrupts
 ******************************************************************************/
void gpio_example_init(void)
{
  sl_status_t status;
  uint8_t get_pin_output, direction;
  sl_si91x_gpio_version_t version;

  // Version information of gpio
  version = sl_si91x_gpio_driver_get_version();
  DEBUGOUT("gpio version is fetched successfully \n");
  DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);
  do {
    // Initialize GPIO HP instance
    if (M4_GPIO_PIN == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_GPIO_10_PORT, SL_SI91X_GPIO_10_PIN };
      sl_si91x_gpio_t port_pin      = { SL_SI91X_GPIO_7_PORT, SL_SI91X_GPIO_7_PIN };
      sl_si91x_gpio_mode_t mode     = MODE_0;
      // GPIO initialization function for HP instance
      sl_gpio_driver_initialization();
      // Get the pin direction for GPIO pin
      direction = sl_si91x_gpio_driver_get_pin_direction(gpio_port_pin.port, gpio_port_pin.pin);
      DEBUGOUT("get_pin_direction = %d\n", direction);
      direction = sl_si91x_gpio_driver_get_pin_direction(port_pin.port, port_pin.pin);
      DEBUGOUT("get_pin_direction = %d\n", direction);

      // Get the pin mode for GPIO pin
      status = sl_gpio_driver_get_pin_mode(&port_pin, &mode);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get pin mode is successful \n");
      DEBUGOUT("get_pin_mode = %d\n", mode);
      status = sl_gpio_driver_get_pin_mode(&port_pin, &mode);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get pin mode is successful \n");
      DEBUGOUT("get_pin_mode = %d\n", mode);

      // Set single GPIO pin to '1'
      status = sl_gpio_driver_set_pin(&gpio_port_pin);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_set_pin, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver set pin is successful \n");
      // Get single GPIO pin value '0' or '1'
      get_pin_output = sl_gpio_driver_get_pin_output(&gpio_port_pin);
      DEBUGOUT("get_pin_output = %d\n", get_pin_output);
      // Clear GPIO pin value
      status = sl_gpio_driver_clear_pin(&gpio_port_pin);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_clear_pin, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver clear pin is successful \n");
    }

    // Configure GPIO HP instance pin interrupt
    if (M4_GPIO_PIN_INTR == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_GPIO_11_PORT, SL_SI91X_GPIO_11_PIN };
      // GPIO initialization function for HP instance
      sl_gpio_driver_initialization();
      // Configure pin interrupt for GPIO pin
      status = sl_gpio_driver_configure_interrupt(&gpio_port_pin,
                                                  INT_CH,
                                                  (sl_gpio_interrupt_flag_t)SL_GPIO_INTERRUPT_RISE_EDGE,
                                                  &gpio_pin_interrupt0_callback,
                                                  AVL_INTR_NO);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_configure_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver interrupt configure is successful \n");
    }

    // Set GPIO port output
    if (M4_GPIO_PORT == SET) {
      sl_si91x_gpio_t gpio_port_pin;
      gpio_port_pin.port  = PORT0;
      uint32_t port_value = PINS;
      // GPIO initialization function for HP instance
      sl_gpio_driver_initialization();
      // Set port pins to 1's
      sl_gpio_driver_set_port(gpio_port_pin.port, PINS);
      __NOP();
      // Get GPIO port value
      status = sl_gpio_driver_get_port_output(gpio_port_pin.port, &port_value);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_port_output, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get port output is successful \n");
      DEBUGOUT("get_port_output = %ld\n", port_value);
      // Clear GPIO port value
      sl_gpio_driver_clear_port(gpio_port_pin.port, PINS);
      __NOP();
      status = sl_gpio_driver_get_port_output(gpio_port_pin.port, &port_value);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_port_output, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get port output is successful \n");
      DEBUGOUT("get_port_output = %ld\n", port_value);
      // Set and get bits by masking which bits to modify
      status = sl_gpio_driver_set_port_output_value(gpio_port_pin.port, PINS, PORT_MASK);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_set_port_output_value, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver set port output value is successful \n");
      __NOP();
      status = sl_gpio_driver_get_port_output(gpio_port_pin.port, &port_value);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_port_output, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get port output is successful \n");
      DEBUGOUT("get_port_output = %ld\n", port_value);
      status = sl_gpio_driver_clear_port(gpio_port_pin.port, PINS);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_clear_port, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver clear port is successful \n");
    }

    // Configure GPIO HP instance group interrupt
    if (M4_GPIO_GROUP_INTR == SET) {
      sl_si91x_gpio_t gpio_port_pin;
      gpio_port_pin.port            = PORT0;
      uint8_t group_pins[PIN_COUNT] = { SL_SI91X_GPIO_8_PIN, SL_SI91X_GPIO_9_PIN }; // pins for group interrupt
      uint8_t group_port[PIN_COUNT] = { gpio_port_pin.port, gpio_port_pin.port };   // port selected for group interrupt
      uint8_t group_pol[PIN_COUNT]  = { POLARITY, POLARITY }; // polarity selected for group interrupt

      // Configure group interrupt parameters
      config_grp_int.grp_interrupt     = GROUP_INT_1; // Set group interrupt 1
      config_grp_int.grp_interrupt_cnt = GRP_COUNT;   // Count of group interrupt pins
      config_grp_int.and_or            = AND_EVENT;   // AND/OR of group interrupt
      config_grp_int.level_edge        = LEVEL_EVENT; // Level/Edge of group interrupt
      memcpy(config_grp_int.grp_interrupt_pin, group_pins, PIN_COUNT);
      memcpy(config_grp_int.grp_interrupt_port, group_port, PIN_COUNT);
      memcpy(config_grp_int.grp_interrupt_pol, group_pol, PIN_COUNT);

      // GPIO initialization function for HP instance
      sl_gpio_driver_initialization();
      // Configure group interrupt for grp_config_int structure
      status = sl_si91x_gpio_driver_configure_group_interrupt(&config_grp_int, &gpio_group_interrupt0_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_configure_group_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver configure group interrupt is successful \n");
    }

    // Initialize GPIO ULP instance
    if (ULP_GPIO_PIN == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN };
      sl_si91x_gpio_t port_pin      = { SL_SI91X_ULP_GPIO_10_PORT, SL_SI91X_ULP_GPIO_10_PIN };
      sl_si91x_gpio_mode_t mode     = MODE_0;
      // GPIO initialization function for ULP instance
      sl_gpio_driver_ulp_initialization();
      // Get the pin direction for ULP GPIO pin
      direction = sl_si91x_gpio_driver_get_pin_direction(gpio_port_pin.port, gpio_port_pin.pin);
      DEBUGOUT("get_pin_direction = %d\n", direction);
      direction = sl_si91x_gpio_driver_get_pin_direction(port_pin.port, port_pin.pin);
      DEBUGOUT("get_pin_direction = %d\n", direction);

      // Get the pin mode for ULP GPIO pin
      status = sl_gpio_driver_get_pin_mode(&gpio_port_pin, &mode);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get pin mode is successful \n");
      DEBUGOUT("get_pin_mode = %d\n", mode);
      status = sl_gpio_driver_get_pin_mode(&port_pin, &mode);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_pin_mode, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get pin mode is successful \n");
      DEBUGOUT("get_pin_mode = %d\n", mode);
    }

    // Configure GPIO ULP instance pin interrupt
    if (ULP_GPIO_PIN_INTR == SET) {
      // GPIO initialization function for ULP instance
      sl_gpio_driver_ulp_initialization();
      // Configure ULP GPIO pin interrupts
      status = sl_si91x_gpio_driver_configure_ulp_pin_interrupt(
        INT_CH,
        (sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_FALL_EDGE,
        SL_SI91X_ULP_GPIO_8_PIN,
        &gpio_ulp_pin_interrupt_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_configure_ulp_pin_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver configure ulp pin interrupt is successful \n");
    }

    // Configure GPIO ULP instance group interrupt
    if (ULP_GPIO_GROUP_INTR == SET) {
      uint8_t ulp_group_pins[PIN_COUNT] = { SL_SI91X_ULP_GPIO_8_PIN,
                                            SL_SI91X_ULP_GPIO_10_PIN }; // pins for group interrupt
      uint8_t ulp_group_pol[PIN_COUNT]  = { POLARITY, POLARITY };       // polarity selected for group interrupt

      // Configure ULP GPIO group parameters
      config_grp_int.grp_interrupt     = ULP_GROUP_INT; // Set ULP group interrupt
      config_grp_int.grp_interrupt_cnt = GRP_COUNT;     // Count of group interrupt pins
      config_grp_int.and_or            = AND_EVENT;     // AND/OR of group interrupt
      config_grp_int.level_edge        = LEVEL_EVENT;   // Level/Edge of group interrupt
      memcpy(config_grp_int.grp_interrupt_pin, ulp_group_pins, PIN_COUNT);
      memcpy(config_grp_int.grp_interrupt_pol, ulp_group_pol, PIN_COUNT);

      // GPIO initialization function for ULP instance
      sl_gpio_driver_ulp_initialization();
      // Configure group interrupt for grp_config_int structure
      status = sl_si91x_gpio_driver_configure_ulp_group_interrupt(&config_grp_int, &gpio_ulp_group_interrupt_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_configure_ulp_group_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver configure ulp group interrupt is successful \n");
    }

    // Initialize GPIO UULP instance
    if (UULP_GPIO_PIN == SET) {
      // GPIO initialization function for UULP instance
      sl_gpio_driver_uulp_initialization();
      // Get NPSS GPIO pin direction
      direction = sl_si91x_gpio_driver_get_uulp_npss_direction(SL_SI91X_UULP_GPIO_0_PIN);
      DEBUGOUT("get_direction = %d\n", direction);
    }

    // Configure GPIO UULP instance pin interrupt
    if (UULP_GPIO_PIN_INTR == SET) {
      uulp_pad.gpio_padnum = SL_SI91X_UULP_GPIO_2_PIN; // UULP GPIO pin number 2 is selected
      uulp_pad.pad_select  = SET;                      // UULP GPIO PAD is selected
      uulp_pad.mode        = CLR;                      // UULP GPIO mode 0 is selected
      uulp_pad.direction   = SET;                      // UULP GPIO direction is selected
      uulp_pad.receiver    = SET;                      // UULP GPIO receiver is enabled

      // GPIO initialization function for UULP instance
      sl_gpio_driver_uulp_initialization();
      status = sl_si91x_gpio_driver_set_uulp_pad_configuration(&uulp_pad);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_set_uulp_pad_configuration, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver set uulp pad configuration is successful \n");
      // Unmask NPSS interrupt
      status = sl_si91x_gpio_driver_unmask_uulp_npss_interrupt(SL_SI91X_UULP_GPIO_2_PIN);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_unmask_uulp_npss_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver unmask uulp interrupt is successful \n");
      // Configure pin interrupt for sl_si91x_gpio_interrupt_config_flag_t structure
      status = sl_si91x_gpio_driver_configure_uulp_interrupt(
        (sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_FALL_EDGE,
        UULP_GPIO_PIN2,
        &gpio_uulp_pin_interrupt_callback);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_configure_uulp_interrupt, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver configure uulp interrupt is successful \n");
    }
  } while (false);
}
/*******************************************************************************
 * Function will run continuously in while loop and led toggles for every 1sec
 ******************************************************************************/
void gpio_example_process_action(void)
{
  sl_status_t status;
  while (TRUE) {
    // Initialize GPIO HP instance
    if (M4_GPIO_PIN == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_GPIO_10_PORT, SL_SI91X_GPIO_10_PIN };
      status                        = sl_gpio_driver_toggle_pin(&gpio_port_pin); // Toggle GPIO pin
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_toggle_pin, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver toggle pin is successful \n");
      delay(DELAY); // Delay of 1sec
    }

    // Initialize GPIO ULP instance
    if (ULP_GPIO_PIN == SET) {
      sl_si91x_gpio_t gpio_port_pin = { SL_SI91X_ULP_GPIO_1_PORT, SL_SI91X_ULP_GPIO_1_PIN };
      sl_si91x_gpio_t port_pin      = { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN };
      uint8_t pin_value;
      // Get GPIO pin input
      status = sl_gpio_driver_get_pin(&gpio_port_pin, &pin_value);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_gpio_driver_get_pin, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver get pin is successful \n");
      if (pin_value == CLR) {
        status = sl_gpio_driver_set_pin(&port_pin); // Set ULP GPIO pin
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_gpio_driver_set_pin, Error code: %lu", status);
          break;
        }
        DEBUGOUT("GPIO driver set pin is successful \n");
      } else {
        status = sl_gpio_driver_clear_pin(&port_pin); // Clear ULP GPIO pin
        if (status != SL_STATUS_OK) {
          DEBUGOUT("sl_gpio_driver_set_pin, Error code: %lu", status);
          break;
        }
        DEBUGOUT("GPIO driver set pin is successful \n");
      }
    }

    // Initialize GPIO UULP instance
    if (UULP_GPIO_PIN == SET) {
      // Set UULP GPIO pin
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SL_SI91X_UULP_GPIO_0_PIN, SET);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver set uulp pin value is successful \n");
      delay(DELAY); // Delay of 1sec
                    // Clear UULP GPIO pin
      status = sl_si91x_gpio_driver_set_uulp_npss_pin_value(SL_SI91X_UULP_GPIO_0_PIN, CLR);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_pin_value, Error code: %lu", status);
        break;
      }
      DEBUGOUT("GPIO driver clear uulp pin value is successful \n");
      delay(DELAY); // Delay of 1sec
    }
  }
}

/*******************************************************************************
 * Delay function for 1ms
 ******************************************************************************/
static void delay(uint32_t idelay)
{
  for (uint32_t x = 0; x < MS_DELAY_COUNTER * idelay; x++) {
    __NOP();
  }
}

/*******************************************************************************
 * GPIO initialization function
 ******************************************************************************/
static void sl_gpio_driver_initialization(void)
{
  sl_status_t status;
  sl_gpio_driver_init();
  sl_si91x_gpio_t gpio_port_pin6  = { SL_SI91X_GPIO_6_PORT, SL_SI91X_GPIO_6_PIN };
  sl_si91x_gpio_t gpio_port_pin7  = { SL_SI91X_GPIO_7_PORT, SL_SI91X_GPIO_7_PIN };
  sl_si91x_gpio_t gpio_port_pin8  = { SL_SI91X_GPIO_8_PORT, SL_SI91X_GPIO_8_PIN };
  sl_si91x_gpio_t gpio_port_pin10 = { SL_SI91X_GPIO_10_PORT, SL_SI91X_GPIO_10_PIN };
  sl_si91x_gpio_t gpio_port_pin11 = { SL_SI91X_GPIO_11_PORT, SL_SI91X_GPIO_11_PIN };

  do {
    sl_gpio_mode_t mode = MODE_0;
    DEBUGOUT("\r\n M4_GPIO_PIN test starts \r\n");
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO); // Enable GPIO M4_CLK
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_clock, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver enable clock is successful \n");

    // Enable pad selection for GPIO pins
    // PadSelection-9 related GPIOS are pre-configured for Other Functions
    for (pad_sel = 1; pad_sel < MAX_PAD_SELECT; pad_sel++) {
      if (pad_sel != PAD_SELECT_9) {
        sl_si91x_gpio_driver_enable_pad_selection(pad_sel);
      }
    }
    // Enable pad receiver for GPIO pins
    status = sl_si91x_gpio_driver_enable_pad_receiver(SL_SI91X_GPIO_6_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_pad_receiver(SL_SI91X_GPIO_7_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_pad_receiver(SL_SI91X_GPIO_8_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_pad_receiver(SL_SI91X_GPIO_10_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_pad_receiver(SL_SI91X_GPIO_11_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad receiver enable is successful \n");

    // Select pad driver strength for GPIO pins
    status =
      sl_si91x_gpio_driver_select_pad_driver_strength(SL_SI91X_GPIO_8_PIN,
                                                      (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_pad_driver_strength, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad driver strength select is successful \n");
    // Select pad driver disable state for GPIO pins
    status = sl_si91x_gpio_driver_select_pad_driver_disable_state(SL_SI91X_GPIO_8_PIN,
                                                                  (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_pad_driver_disable_state, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pad driver disable state select is successful \n");
    // Set the pin mode for GPIO pins.
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin6, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin mode select is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin7, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin mode select is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin8, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin mode select is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin10, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin mode select is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin11, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin mode select is successful \n");

    // Select the direction of GPIO pin whether Input/ Output
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_GPIO_6_PORT,
                                                    SL_SI91X_GPIO_6_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin direction select is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_GPIO_7_PORT,
                                                    SL_SI91X_GPIO_7_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin direction select is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_GPIO_8_PORT,
                                                    SL_SI91X_GPIO_8_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin direction select is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_GPIO_10_PORT,
                                                    SL_SI91X_GPIO_10_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin direction select is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_GPIO_11_PORT,
                                                    SL_SI91X_GPIO_11_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver pin direction select is successful \n");
  } while (false);
}

/*******************************************************************************
 * ULP GPIO initialization function
 ******************************************************************************/
static void sl_gpio_driver_ulp_initialization(void)
{
  sl_status_t status;
  sl_gpio_driver_init();
  sl_si91x_gpio_t gpio_port_pin1 = { SL_SI91X_ULP_GPIO_1_PORT, SL_SI91X_ULP_GPIO_1_PIN };
  sl_si91x_gpio_t gpio_port_pin2 = { SL_SI91X_ULP_GPIO_2_PORT, SL_SI91X_ULP_GPIO_2_PIN };
  sl_si91x_gpio_t gpio_port_pin8 = { SL_SI91X_ULP_GPIO_8_PORT, SL_SI91X_ULP_GPIO_8_PIN };
  sl_gpio_mode_t mode            = MODE_0;

  do {
    DEBUGOUT("\r\n ULP_GPIO_PIN test starts \r\n");
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_clock, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver clock enable is successful \n");
    // Enable pad receiver for ULP GPIO pins
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_1_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver ulp pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_2_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver ulp pad receiver enable is successful \n");
    status = sl_si91x_gpio_driver_enable_ulp_pad_receiver(SL_SI91X_ULP_GPIO_8_PIN);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_ulp_pad_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver ulp pad receiver enable is successful \n");

    // Select pad driver strength for ULP GPIO pins
    status =
      sl_si91x_gpio_driver_select_ulp_pad_driver_strength(SL_SI91X_ULP_GPIO_1_PIN,
                                                          (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_ulp_pad_driver_strength, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver ulp pad driver strength selection is successful \n");
    // Select pad driver disable state for ULP GPIO pins
    status = sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(SL_SI91X_ULP_GPIO_1_PIN,
                                                                      (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver ulp pad driver disable state selection is successful \n");
    // Set the pin mode for ULP GPIO pins.
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin1, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin mode is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin2, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin mode is successful \n");
    status = sl_gpio_driver_set_pin_mode(&gpio_port_pin8, mode, OUTPUT_VALUE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_gpio_driver_set_pin_mode, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin mode is successful \n");

    // Set the pin direction for ULP GPIO pins.
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_1_PORT,
                                                    SL_SI91X_ULP_GPIO_1_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin direction is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_2_PORT,
                                                    SL_SI91X_ULP_GPIO_2_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin direction is successful \n");
    status = sl_si91x_gpio_driver_set_pin_direction(SL_SI91X_ULP_GPIO_8_PORT,
                                                    SL_SI91X_ULP_GPIO_8_PIN,
                                                    (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_pin_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver set pin direction is successful \n");
  } while (false);
}

/*******************************************************************************
 * UULP GPIO initialization function
 ******************************************************************************/
static void sl_gpio_driver_uulp_initialization(void)
{
  sl_status_t status;
  sl_gpio_driver_init();
  do {
    DEBUGOUT("\r\n UULP_GPIO_PIN test starts \r\n");
    status = sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_enable_clock, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver clock enable is successful \n");
    // Set NPSS GPIO input buffer
    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(SL_SI91X_UULP_GPIO_0_PIN, SET);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_uulp_npss_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp receiver selection is successful \n");
    status = sl_si91x_gpio_driver_select_uulp_npss_receiver(SL_SI91X_UULP_GPIO_2_PIN, SET);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_select_uulp_npss_receiver, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp receiver selection is successful \n");
    // Set NPSS GPIO pin MUX
    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SL_SI91X_UULP_GPIO_0_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp pin mux selection is successful \n");
    status = sl_si91x_gpio_driver_set_uulp_npss_pin_mux(SL_SI91X_UULP_GPIO_2_PIN, NPSS_GPIO_PIN_MUX_MODE0);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_pin_mux, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp pin mux selection is successful \n");
    // Set NPSS GPIO pin direction
    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(SL_SI91X_UULP_GPIO_0_PIN, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp pin direction selection is successful \n");
    status =
      sl_si91x_gpio_driver_set_uulp_npss_direction(SL_SI91X_UULP_GPIO_2_PIN, (sl_si91x_gpio_direction_t)GPIO_INPUT);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_gpio_driver_set_uulp_npss_direction, Error code: %lu", status);
      break;
    }
    DEBUGOUT("GPIO driver uulp pin direction selection is successful \n");
  } while (false);
}

/*******************************************************************************
 *   This API handles GPIO pin interrupt 0 request
 ******************************************************************************/
static void gpio_pin_interrupt0_callback(void)
{
}

/*******************************************************************************
 *  This API handles GPIO Group interrupt 0 request
 ******************************************************************************/
static void gpio_group_interrupt0_callback(void)
{
}

/*******************************************************************************
 *  This API handles UULP GPIO pin interrupt 0 request
 ******************************************************************************/
static void gpio_uulp_pin_interrupt_callback(void)
{
}

/*******************************************************************************
 *  This API handles ULP GPIO OR'ed pin interrupt request
 ******************************************************************************/
static void gpio_ulp_pin_interrupt_callback(void)
{
}

/*******************************************************************************
 *  This API handles ULP GPIO group interrupt request
 ******************************************************************************/
static void gpio_ulp_group_interrupt_callback(void)
{
}
