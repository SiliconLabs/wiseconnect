/*******************************************************************************
 * @file    ulp_gpio_example.c
 * @brief   Ulp Gpio Example
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
#include "ulp_gpio_example.h"
#include "rsi_chip.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define RESERVED_IRQ_COUNT   16                                   // Reserved IRQ count
#define EXT_IRQ_COUNT        98                                   // External IRQ count
#define VECTOR_TABLE_ENTRIES (RESERVED_IRQ_COUNT + EXT_IRQ_COUNT) // Vector table entries
#define PORT0                0                                    // GPIO Port number(0 to 4)

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

#define UULP_INTR_1 0x01 // UULP GPIO pin interrupt 1
#define UULP_INTR_2 0x02 // UULP GPIO pin interrupt 2
#define UULP_INTR_3 0x04 // UULP GPIO pin interrupt 3
#define UULP_INTR_4 0x08 // UULP GPIO pin interrupt 4
#define UULP_INTR_5 0x10 // UULP GPIO pin interrupt 5
#define UULP_MASK   0x00

uint32_t ramVector[VECTOR_TABLE_ENTRIES] __attribute__((aligned(256)));
extern void hardware_setup(void);
/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
// Enum for enabling GPIO instances functionality
// for ulp and uulp pins please enable one at a time.
typedef enum {
  ULP_GPIO_PIN        = 1, // Initialize GPIO ULP instance
  UULP_GPIO_PIN       = 0, // Initialize GPIO UULP instance
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
static void sl_gpio_ulp_initialization(void);
static void sl_gpio_uulp_initialization(void);

void UULP_PIN_IRQ_Handler(void);
void ULP_PIN_IRQ_Handler(void);
void ULP_GROUP_IRQ_Handler(void);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * GPIO example initialization function. It initializes HP/ULP clock, pin mode,
 * direction and configure pin and group interrupts
 ******************************************************************************/
void gpio_example_init(void)
{
  /* At this stage the micro-controller clock setting is already configured,
     * this is done through SystemInit() function which is called from startup
     * file (startup_rs1xxxx.s) before to branch to application main.
     * To reconfigure the default setting of SystemInit() function, refer to
     * startup_rs1xxxx.c file
     */
  //copying the vector table from flash to ram
  memcpy(ramVector, (uint32_t *)SCB->VTOR, sizeof(uint32_t) * VECTOR_TABLE_ENTRIES);
  // Assigning the ram vector address to VTOR register
  SCB->VTOR = (uint32_t)ramVector;
  // Switching MCU from PS4 to PS2 state(low power state)
  // In this mode, whatever be the timer clock source value, it will run with 20MHZ only
  // To use gpio in high power mode, don't call hardware_setup()
  hardware_setup();
  DEBUGINIT();

  uint8_t get_pin, direction;
  uint32_t get_port;
  sl_si91x_gpio_version_t version;

  //Version information of gpio
  version = sl_si91x_gpio_get_version();
  DEBUGOUT("gpio version is fetched successfully \n");
  DEBUGOUT("API version is %d.%d.%d\n", version.release, version.major, version.minor);

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

/*******************************************************************************
 *  This API handles UULP GPIO pin interrupt 0 request
 ******************************************************************************/
void UULP_PIN_IRQ_Handler(void)
{
  if ((sl_si91x_gpio_get_uulp_interrupt_status() & UULP_INTR_1) != UULP_MASK) {
    sl_si91x_gpio_clear_uulp_interrupt(UULP_PIN_INTR_1);
  }
  if ((sl_si91x_gpio_get_uulp_interrupt_status() & UULP_INTR_2) != UULP_MASK) {
    sl_si91x_gpio_clear_uulp_interrupt(UULP_PIN_INTR_2);
  }
  if ((sl_si91x_gpio_get_uulp_interrupt_status() & UULP_INTR_3) != UULP_MASK) {
    sl_si91x_gpio_clear_uulp_interrupt(UULP_PIN_INTR_3);
  }
  if ((sl_si91x_gpio_get_uulp_interrupt_status() & UULP_INTR_4) != UULP_MASK) {
    sl_si91x_gpio_clear_uulp_interrupt(UULP_PIN_INTR_4);
  }
  if ((sl_si91x_gpio_get_uulp_interrupt_status() & UULP_INTR_5) != UULP_MASK) {
    sl_si91x_gpio_clear_uulp_interrupt(UULP_PIN_INTR_5);
  }
}

/*******************************************************************************
 *  This API handles ULP GPIO OR'ed pin interrupt request
 ******************************************************************************/
void ULP_PIN_IRQ_Handler(void)
{
  sl_si91x_gpio_clear_ulp_interrupt(ULP_PIN_INT);
}

/*******************************************************************************
 *  This API handles ULP GPIO group interrupt request
 ******************************************************************************/
void ULP_GROUP_IRQ_Handler(void)
{
  sl_si91x_gpio_clear_ulp_group_interrupt(ULP_GROUP_INT);
}
