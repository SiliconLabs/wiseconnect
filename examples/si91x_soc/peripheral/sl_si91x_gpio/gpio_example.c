/*******************************************************************************
 * @file    gpio example.c
 * @brief
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
#include "sl_si91x_peripheral_gpio.h"
#include "gpio_example.h"
#include "rsi_chip.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define PORT0 0 // GPIO Port 0

#define PAD_SELECT_9   9  // GPIO PAD selection number
#define MAX_PAD_SELECT 34 // Maximum GPIO PAD selection number

#define PIN6  6  // GPIO Pin number 6
#define PIN7  7  // Pin number 7 port 0 for button
#define PIN8  8  // Pin number 8 port 0 for pin interrupt
#define PIN10 10 // Pin number 10 port 0 for led
#define PIN11 11 // GPIO Pin number 11
#define PIN25 25 // Pin number 25 for interrupt
#define PIN28 28 // Pin number 28 for interrupt

#define ULP_PIN_0  0  // ULP GPIO Pin number 0
#define ULP_PIN_4  4  // ULP GPIO Pin number 4
#define ULP_PIN_5  5  // ULP GPIO Pin number 5
#define ULP_PIN_8  8  // ULP GPIO Pin number 8
#define ULP_PIN_10 10 // ULP GPIO Pin number 10

#define UULP_PIN0 0 // UULP Pin number 0
#define UULP_PIN1 1 // UULP Pin number 1
#define UULP_PIN2 2 // UULP Pin number 2

#define POLARITY  0 // Polarity for GPIO pin
#define PIN_COUNT 2 // Number of interrupts needed

#define AND_EVENT   0 // AND for group interrupt
#define OR_EVENT    1 // OR for group interrupt
#define LEVEL_EVENT 0 // level for group interrupt
#define EDGE_EVENT  1 // Edge for group interrupt
#define GRP_COUNT   2 // Count of group interrupt pins

#define PRIORITY_7 7      // Priority 7 for interrupt
#define PINS       0x0c40 // Pins in a port to mask

#define INT_CH       0 // GPIO Pin interrupt 0
#define OUTPUT_VALUE 1 // GPIO output value
#define NPSS_INTR    4 // NPSS GPIO interrupt number

#define MS_DELAY_COUNTER 4600 // Delay count
#define DELAY            1000 // Delay for 1sec
#define FALL_EDGE        8    // Fall edge event

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
  UULP_GPIO_PIN4 = (1 << 4), // UULP GPIO pin 5 interrupt
} sl_si91x_uulp_gpio_grp_intr_t;

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
static sl_si91x_gpio_group_interrupt_config_t config_grp_int;
uint8_t pad_sel = 1;
uint32_t i;
static uulp_pad_config_t uulp_pad;
/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void delay(uint32_t idelay);
static void sl_gpio_initialization(void);
static void sl_gpio_ulp_initialization(void);
static void sl_gpio_uulp_initialization(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * GPIO example initialization function. It initializes HP/ULP clock, pin mode,
 * direction and configure pin and group interrupts
 ******************************************************************************/
void gpio_example_init(void)
{
  uint8_t get_pin, direction;
  uint32_t get_port;
  sl_si91x_gpio_version_t version;

  //Version information of gpio
  version = sl_si91x_gpio_get_version();
  DEBUGOUT("gpio version is fetched successfully \n");
  DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);

  // Initialize GPIO HP instance
  if (M4_GPIO_PIN == SET) {
    // GPIO initialization function for HP instance
    sl_gpio_initialization();
    // Get the pin direction for GPIO pin
    direction = sl_si91x_gpio_get_pin_direction(PORT0, PIN10);
    DEBUGOUT("get_pin_direction = %d\n", direction);
    direction = sl_si91x_gpio_get_pin_direction(PORT0, PIN7);
    DEBUGOUT("get_pin_direction = %d\n", direction);

    // Get the pin mode for GPIO pin
    get_pin = sl_gpio_get_pin_mode(PORT0, PIN10);
    DEBUGOUT("get_pin_mode = %d\n", get_pin);
    get_pin = sl_gpio_get_pin_mode(PORT0, PIN7);
    DEBUGOUT("get_pin_mode = %d\n", get_pin);

    // Set single GPIO pin to '1'
    sl_gpio_set_pin_output(PORT0, PIN10);
    // Get single GPIO pin value '0' or '1'
    get_pin = sl_gpio_get_pin_output(PORT0, PIN10);
    DEBUGOUT("get_pin_output = %d\n", get_pin);
    // Clear GPIO pin value
    sl_gpio_clear_pin_output(PORT0, PIN10);
  }

  // Configure GPIO HP instance pin interrupt
  if (M4_GPIO_PIN_INTR == SET) {
    // GPIO initialization function for HP instance
    sl_gpio_initialization();
    // Configure pin interrupt for GPIO pin
    sl_gpio_configure_interrupt(PORT0, PIN8, INT_CH, FALL_EDGE);
    NVIC_EnableIRQ(PININT0_NVIC_NAME);
    NVIC_SetPriority(PININT0_NVIC_NAME, PRIORITY_7);
  }

  // Set GPIO port output
  if (M4_GPIO_PORT == SET) {
    // GPIO initialization function for HP instance
    sl_gpio_initialization();
    // Set port pins to 1's
    sl_gpio_set_port_output(PORT0, PINS);
    __NOP();
    // Get GPIO port value
    get_port = sl_gpio_get_port_output(PORT0);
    DEBUGOUT("get_port_output = %ld\n", get_port);
    // Clear GPIO port value
    sl_gpio_clear_port_output(PORT0, PINS);
    __NOP();
    get_port = sl_gpio_get_port_output(PORT0);
    DEBUGOUT("get_port_output = %ld\n", get_port);
    // Set and get bits by masking which bits to modify
    sl_gpio_set_port_output_value(PORT0, PINS, PORT_MASK);
    __NOP();
    get_port = sl_gpio_get_port_output(PORT0);
    DEBUGOUT("get_port_output = %ld\n", get_port);
    sl_gpio_clear_port_output(PORT0, PINS);
  }

  // Configure GPIO HP instance group interrupt
  if (M4_GPIO_GROUP_INTR == SET) {
    uint8_t group_pins[PIN_COUNT] = { PIN25, PIN28 };       // pins for group interrupt
    uint8_t group_port[PIN_COUNT] = { PORT0, PORT0 };       // port selected for group interrupt
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
    sl_gpio_initialization();
    // Configure group interrupt for grp_config_int structure
    sl_si91x_gpio_configure_group_interrupt(&config_grp_int);
  }

  // Initialize GPIO ULP instance
  if (ULP_GPIO_PIN == SET) {
    // GPIO initialization function for ULP instance
    sl_gpio_ulp_initialization();
    // Get the pin direction for ULP GPIO pin
    direction = sl_si91x_gpio_get_pin_direction(SL_ULP_GPIO_PORT, ULP_PIN_8);
    DEBUGOUT("get_pin_direction = %d\n", direction);
    direction = sl_si91x_gpio_get_pin_direction(SL_ULP_GPIO_PORT, ULP_PIN_4);
    DEBUGOUT("get_pin_direction = %d\n", direction);

    // Get the pin mode for ULP GPIO pin
    get_pin = sl_gpio_get_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_8);
    DEBUGOUT("get_pin_mode = %d\n", get_pin);
    get_pin = sl_gpio_get_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_4);
    DEBUGOUT("get_pin_mode = %d\n", get_pin);
  }

  // Configure GPIO ULP instance pin interrupt
  if (ULP_GPIO_PIN_INTR == SET) {
    // GPIO initialization function for ULP instance
    sl_gpio_ulp_initialization();
    // Configure ULP GPIO pin interrupts
    sl_si91x_gpio_configure_ulp_pin_interrupt(INT_CH,
                                              (sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_LEVEL_HIGH,
                                              ULP_PIN_4);
    NVIC_EnableIRQ(ULP_PININT0_NVIC_NAME);
    NVIC_SetPriority(ULP_PININT0_NVIC_NAME, PRIORITY_7);
  }

  // Configure GPIO ULP instance group interrupt
  if (ULP_GPIO_GROUP_INTR == SET) {
    uint8_t ulp_group_pins[PIN_COUNT] = { ULP_PIN_0, ULP_PIN_10 }; // pins for group interrupt
    uint8_t ulp_group_pol[PIN_COUNT]  = { POLARITY, POLARITY };    // polarity selected for group interrupt

    // Configure ULP GPIO group parameters
    config_grp_int.grp_interrupt     = ULP_GROUP_INT; // Set ULP group interrupt
    config_grp_int.grp_interrupt_cnt = GRP_COUNT;     // Count of group interrupt pins
    config_grp_int.and_or            = AND_EVENT;     // AND/OR of group interrupt
    config_grp_int.level_edge        = LEVEL_EVENT;   // Level/Edge of group interrupt
    memcpy(config_grp_int.grp_interrupt_pin, ulp_group_pins, PIN_COUNT);
    memcpy(config_grp_int.grp_interrupt_pol, ulp_group_pol, PIN_COUNT);

    // GPIO initialization function for ULP instance
    sl_gpio_ulp_initialization();
    // Configure group interrupt for grp_config_int structure
    sl_si91x_gpio_configure_ulp_group_interrupt(&config_grp_int);
  }

  // Initialize GPIO UULP instance
  if (UULP_GPIO_PIN == SET) {
    // GPIO initialization function for UULP instance
    sl_gpio_uulp_initialization();
    // Get NPSS GPIO pin direction
    direction = sl_si91x_gpio_get_uulp_npss_direction(UULP_PIN2);
    DEBUGOUT("get_direction = %d\n", direction);
  }

  // Configure GPIO UULP instance pin interrupt
  if (UULP_GPIO_PIN_INTR == SET) {
    uulp_pad.gpio_padnum = UULP_PIN2; // UULP GPIO pin number 2 is selected
    uulp_pad.pad_select  = SET;       // UULP GPIO PAD is selected
    uulp_pad.mode        = CLR;       // UULP GPIO mode 0 is selected
    uulp_pad.direction   = SET;       // UULP GPIO direction is selected
    uulp_pad.receiver    = SET;       // UULP GPIO receiver is enabled

    // GPIO initialization function for UULP instance
    sl_gpio_uulp_initialization();
    sl_si91x_gpio_set_uulp_pad_configuration(&uulp_pad);
    // Unmask NPSS interrupt
    sl_si91x_gpio_unmask_uulp_npss_interrupt(UULP_PIN2);

    // Configure pin interrupt for sl_si91x_gpio_interrupt_config_flag_t structure
    sl_si91x_gpio_configure_uulp_interrupt((sl_si91x_gpio_interrupt_config_flag_t)SL_GPIO_INTERRUPT_LEVEL_LOW,
                                           UULP_GPIO_PIN2);
  }
}
/*******************************************************************************
 * Function will run continuously in while loop and led toggles for every 1sec
 ******************************************************************************/
void gpio_example_process_action(void)
{
  // Initialize GPIO HP instance
  if (M4_GPIO_PIN == SET) {
    sl_gpio_toggle_pin_output(PORT0, PIN10); // Toggle GPIO pin
    delay(DELAY);                            // Delay of 1sec
  }

  // Initialize GPIO ULP instance
  if (ULP_GPIO_PIN == SET) {
    uint8_t input;
    // Get GPIO pin input
    input = sl_gpio_get_pin_input(SL_ULP_GPIO_PORT, ULP_PIN_5);
    if (input == CLR) {
      sl_gpio_set_pin_output(SL_ULP_GPIO_PORT, ULP_PIN_8); // Set ULP GPIO pin
    } else {
      sl_gpio_clear_pin_output(SL_ULP_GPIO_PORT, ULP_PIN_8); // Clear ULP GPIO pin
    }
  }

  // Initialize GPIO UULP instance
  if (UULP_GPIO_PIN == SET) {
    // Set UULP GPIO pin
    sl_si91x_gpio_set_uulp_npss_pin_value(UULP_PIN0, SET);
    delay(DELAY); // Delay of 1sec
                  // Clear UULP GPIO pin
    sl_si91x_gpio_set_uulp_npss_pin_value(UULP_PIN0, CLR);
    delay(DELAY); // Delay of 1sec
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
static void sl_gpio_initialization(void)
{
  DEBUGOUT("\r\n M4_GPIO_PIN test starts \r\n");
  sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)M4CLK_GPIO); // Enable GPIO M4_CLK

  // Enable pad selection for GPIO pins
  // PadSelection-9 related GPIOS are pre-configured for Other Functions
  for (pad_sel = 1; pad_sel < MAX_PAD_SELECT; pad_sel++) {
    if (pad_sel != PAD_SELECT_9) {
      sl_si91x_gpio_enable_pad_selection(pad_sel);
    }
  }
  // Enable pad receiver for GPIO pins
  sl_si91x_gpio_enable_pad_receiver(PIN6);
  sl_si91x_gpio_enable_pad_receiver(PIN7);
  sl_si91x_gpio_enable_pad_receiver(PIN8);
  sl_si91x_gpio_enable_pad_receiver(PIN10);
  sl_si91x_gpio_enable_pad_receiver(PIN11);

  // Select pad driver strength for GPIO pins
  sl_si91x_gpio_select_pad_driver_strength(PIN8, (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);

  // Select pad driver disable state for GPIO pins
  sl_si91x_gpio_select_pad_driver_disable_state(PIN8, (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);

  // Set the pin mode for GPIO pins.
  sl_gpio_set_pin_mode(PORT0, PIN6, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(PORT0, PIN7, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(PORT0, PIN8, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(PORT0, PIN10, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(PORT0, PIN11, _MODE0, OUTPUT_VALUE);

  // Select the direction of GPIO pin whether Input/ Output
  sl_si91x_gpio_set_pin_direction(PORT0, PIN6, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
  sl_si91x_gpio_set_pin_direction(PORT0, PIN7, (sl_si91x_gpio_direction_t)GPIO_INPUT);
  sl_si91x_gpio_set_pin_direction(PORT0, PIN8, (sl_si91x_gpio_direction_t)GPIO_INPUT);
  sl_si91x_gpio_set_pin_direction(PORT0, PIN10, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
  sl_si91x_gpio_set_pin_direction(PORT0, PIN11, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
}

/*******************************************************************************
 * ULP GPIO initialization function
 ******************************************************************************/
static void sl_gpio_ulp_initialization(void)
{
  DEBUGOUT("\r\n ULP_GPIO_PIN test starts \r\n");
  sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK

  // Enable pad receiver for ULP GPIO pins
  sl_si91x_gpio_enable_ulp_pad_receiver(ULP_PIN_4);
  sl_si91x_gpio_enable_ulp_pad_receiver(ULP_PIN_5);

  // Select pad driver strength for ULP GPIO pins
  sl_si91x_gpio_select_ulp_pad_driver_strength(ULP_PIN_4, (sl_si91x_gpio_driver_strength_select_t)GPIO_TWO_MILLI_AMPS);

  // Select pad driver disable state for ULP GPIO pins
  sl_si91x_gpio_select_ulp_pad_driver_disable_state(ULP_PIN_4, (sl_si91x_gpio_driver_disable_state_t)GPIO_HZ);

  // Set the pin mode for ULP GPIO pins.
  sl_gpio_set_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_8, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_4, _MODE0, OUTPUT_VALUE);
  sl_gpio_set_pin_mode(SL_ULP_GPIO_PORT, ULP_PIN_5, _MODE0, OUTPUT_VALUE);

  // Set the pin direction for ULP GPIO pins.
  sl_si91x_gpio_set_pin_direction(SL_ULP_GPIO_PORT, ULP_PIN_8, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
  sl_si91x_gpio_set_pin_direction(SL_ULP_GPIO_PORT, ULP_PIN_4, (sl_si91x_gpio_direction_t)GPIO_INPUT);
  sl_si91x_gpio_set_pin_direction(SL_ULP_GPIO_PORT, ULP_PIN_5, (sl_si91x_gpio_direction_t)GPIO_INPUT);
}

/*******************************************************************************
 * UULP GPIO initialization function
 ******************************************************************************/
static void sl_gpio_uulp_initialization(void)
{
  DEBUGOUT("\r\n UULP_GPIO_PIN test starts \r\n");
  sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO); // Enable GPIO ULP_CLK
  // Set NPSS GPIO input buffer
  sl_si91x_gpio_select_uulp_npss_receiver(UULP_PIN0, SET);
  sl_si91x_gpio_select_uulp_npss_receiver(UULP_PIN2, SET);

  // Set NPSS GPIO pin MUX
  sl_si91x_gpio_set_uulp_npss_pin_mux(UULP_PIN0, NPSS_GPIO_PIN_MUX_MODE0);
  sl_si91x_gpio_set_uulp_npss_pin_mux(UULP_PIN2, NPSS_GPIO_PIN_MUX_MODE0);

  // Set NPSS GPIO pin direction
  sl_si91x_gpio_set_uulp_npss_direction(UULP_PIN0, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);
  sl_si91x_gpio_set_uulp_npss_direction(UULP_PIN2, (sl_si91x_gpio_direction_t)GPIO_INPUT);
}
