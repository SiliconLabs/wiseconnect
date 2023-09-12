/*******************************************************************************
* @file  gpio_test.c
* @brief Test program for the peripheral_gpio module of PERIPHERAL.
* @details
*   This file implements the test functions for the peripheral_gpio module of PERIPHERAL.
*   peripheral_gpio implements the PERIPHERAL API for the General Purpose I/O (GPIO)
*   module.
*******************************************************************************/
#include "sl_si91x_peripheral_gpio.h"
#include "rsi_board.h"
#include "unity.h"
#include "sl_system_init.h"
#include "rsi_power_save.h"
#include "rsi_rom_clks.h"
#include "rsi_rom_ulpss_clk.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
// Returns true if there are debug pins associated to this port.
#define DEBUG_PORT(PORT) ((PORT) == 0)

// Mask of all pins which are NOT debug pins.
#define _GPIO_NDEBUG_PIN_MASK   0x9FC0
#define _GPIO_NDEBUG_PIN_B_MASK 0x3E00
#define _GPIO_NDEBUG_PIN_C_MASK 0xC000
#define _GPIO_NDEBUG_PIN_D_MASK 0xFFFF

// Mask of all pins which are debug pins.
#define _GPIO_DEBUG_PIN_MASK ~(_GPIO_NDEBUG_PIN_MASK)

// Mask of all pins which are NOT debug pins for each individual port.
#define _GPIO_NDEBUG_PORT(i)                                  \
  (i == 0   ? (_GPIO_PORT_A_PIN_MASK & _GPIO_NDEBUG_PIN_MASK) \
   : i == 1 ? _GPIO_PORT_B_PIN_MASK & _GPIO_NDEBUG_PIN_B_MASK \
   : i == 2 ? _GPIO_PORT_C_PIN_MASK & _GPIO_NDEBUG_PIN_C_MASK \
   : i == 3 ? _GPIO_PORT_D_PIN_MASK & _GPIO_NDEBUG_PIN_D_MASK \
            : 0)

#define MAX_STRENGTH             3 // GPIO max strength
#define MAX_DRIVER_DISABLE_STATE 3 // GPIO max driver disable state

#define PAD_SELECTION  9  // GPIO pad number 9
#define MAX_PAD_SELECT 34 // GPIO max pad selection

#define ALL_PIN_INTERRUPTS 0x0F // Bit mask for GPIO_DOUT
#define MS_DELAY_COUNTER   4600 // delay count

#define CLEAR_OUTPUT 0 // Clear output
#define OUTPUT_VALUE 1 // Set output

#define LOW  0
#define HIGH 1

#define RAND_MASK 0xF4   // Random mask value
#define PORT_MASK 0xFFFF // GPIO port mask
#define MASK1     0xFF0B

#define PIN_0  0  // GPIO pin number 0
#define PIN_1  1  // GPIO pin number 1
#define PIN_4  4  // GPIO pin number 4
#define PIN_6  6  // GPIO pin number 6
#define PIN_8  8  // GPIO pin number 8
#define PIN_10 10 // GPIO pin number 10
#define PIN_11 11 // GPIO pin number 11
#define PIN_12 12 // GPIO pin number 12
#define PIN_57 57 // GPIO pin number 57

#define MAX_CLK  2  // GPIO instance clock selection
#define MAX_MODE 14 // GPIO Pin maximum mode
#define MAX_PIN  16 // GPIO maximum pins

#define MAX_SLEW_RATE 2 // GPIO maximum slew rate

#define PORT0 0 // GPIO port 0 number

#define INT_NO_52 52 // GPIO pin interrupt 52
#define INT_NO_59 59 // GPIO pin interrupt 59

#define SHIFT_16 16

#define MAX_FLAG 6 // GPIO maximum interrupts

#define MASK_1 0x01
#define MASK_2 0x02

#define INTR_4 4 // Maximum UULP pin interrupts
#define INTR_6 6 // Maximum ULP pin interrupts

#define AND_EVENT   0 // AND for group interrupt
#define OR_EVENT    1 // OR for group interrupt
#define LEVEL_EVENT 0 // level for group interrupt
#define EDGE_EVENT  1 // edge for group interrupt
#define GRP_COUNT   2 // Count of group interrupt pins

#define ULP_PORT         4  // ULP GPIO port
#define ULP_GPIO_PIN_MAX 13 // ULP GPIO maximum pins

#define PIN_COUNT     2 // Number of HP group interrupts needed
#define ULP_PIN_COUNT 2 // Number of ULP group interrupts needed

#define ULP_PIN_0 0 // ULP GPIO pin number 0
#define ULP_PIN_4 4 // ULP GPIO Pin number 4
#define ULP_PIN_5 5 // ULP GPIO Pin number 5

#define PIN25 25 // Pin number 25 for interrupt
#define PIN26 26 // Pin number 28 for interrupt

#define MODE_7       7  // GPIO mode 7
#define ULP_GPIO_MAX 11 // ULP GPIO maximum pins
#define POLARITY     0  // Polarity for GPIO pin

#define RISE_EDGE_EVENT 4 // GPIO rise edge event
#define FALL_EDGE_EVENT 8 // GPIO fall edge event
/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
uint8_t port1_pin_valid_array[16] = { 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 };
uint8_t port2_pin_valid_array[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1 };
uint8_t port3_pin_valid_array[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
uint8_t port4_pin_valid_array[9]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sl_si91x_gpio_set_pin_direction(void);
void test_sl_si91x_gpio_get_pin_direction(void);
void test_sl_si91x_gpio_enable_pad_receiver(void);
void test_sl_si91x_gpio_disable_pad_receiver(void);
void test_sl_si91x_gpio_enable_pad_selection(void);
void test_sl_si91x_gpio_select_pad_driver_strength(void);
void test_sl_si91x_gpio_select_pad_driver_disable_state(void);
void test_sl_gpio_pin_functions(void);
void test_sl_gpio_port_functions(void);
void test_sl_si91x_gpio_enable_ulp_pad_receiver(void);
void test_sl_si91x_gpio_disable_ulp_pad_receiver(void);
void test_sl_si91x_gpio_select_ulp_pad_slew_rate(void);
void test_sl_si91x_gpio_select_ulp_pad_driver_strength(void);
void test_sl_si91x_gpio_select_ulp_pad_driver_disable_state(void);
void test_sl_interrupt_functions(void);
void test_sl_interrupt_configure(void);
void test_sl_gpio_pin_mode_int_glitch(void);
void test_sl_gpio_slewrate(void);
void test_sl_si91x_gpio_configure_group_interrupt(void);
void test_sl_si91x_gpio_uulp_pin_functions(void);
void test_sl_si91x_gpio_select_uulp_npss_receiver(void);
void test_sl_si91x_gpio_get_uulp_npss_direction(void);
void test_sl_si91x_gpio_set_uulp_npss_direction(void);
void test_sl_si91x_gpio_configure_uulp_interrupt(void);
void test_sl_si91x_gpio_configure_ulp_interrupt(void);
void test_sl_si91x_gpio_enable_clock(void);
void test_sl_si91x_gpio_disable_clock(void);
void test_sl_si91x_gpio_set_uulp_npss_pin_mux(void);
void test_sl_si91x_gpio_configure_ulp_group_interrupt(void);
uint8_t validate_port_pins(uint8_t port, uint8_t pin);
void delay(uint32_t idelay);
void test_gpio_get_version(void);
/*******************************************************************************
* Function to Validate the GPIO Pin
*******************************************************************************/
uint8_t validate_port_pins(uint8_t port, uint8_t pin)
{
  switch (port) {
    case 0:
      return port1_pin_valid_array[pin];
    case 1:
      return port2_pin_valid_array[pin];
    case 2:
      return port3_pin_valid_array[pin];
    case 3:
      return port4_pin_valid_array[pin];
    default:
      break;
  }
  return 0;
}

/*******************************************************************************
* Function to provide 1ms Delay
*******************************************************************************/
void delay(uint32_t idelay)
{
  for (uint32_t x = 0; x < MS_DELAY_COUNTER * idelay; x++) {
    __NOP();
  }
}

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("GPIO Test Starts \n");

  RUN_TEST(test_gpio_get_version, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_disable_clock, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_enable_clock, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_enable_pad_selection, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_disable_pad_receiver, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_enable_pad_receiver, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_pad_driver_disable_state, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_pad_driver_strength, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_set_pin_direction, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_get_pin_direction, __LINE__);
  RUN_TEST(test_sl_gpio_pin_mode_int_glitch, __LINE__);
  RUN_TEST(test_sl_gpio_slewrate, __LINE__);
  RUN_TEST(test_sl_gpio_pin_functions, __LINE__);
  RUN_TEST(test_sl_gpio_port_functions, __LINE__);
  RUN_TEST(test_sl_interrupt_configure, __LINE__);
  RUN_TEST(test_sl_interrupt_functions, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_configure_group_interrupt, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_disable_ulp_pad_receiver, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_enable_ulp_pad_receiver, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_ulp_pad_driver_disable_state, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_ulp_pad_driver_strength, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_ulp_pad_slew_rate, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_configure_ulp_interrupt, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_configure_ulp_group_interrupt, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_select_uulp_npss_receiver, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_set_uulp_npss_pin_mux, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_set_uulp_npss_direction, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_get_uulp_npss_direction, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_uulp_pin_functions, __LINE__);
  RUN_TEST(test_sl_si91x_gpio_configure_uulp_interrupt, __LINE__);

  UnityEnd();
  UnityPrintf("END GPIO Test \n");
  while (1)
    ;
}

/*******************************************************************************
* Function to test the ULP GPIO disable PAD receiver function
*******************************************************************************/
void test_sl_si91x_gpio_disable_ulp_pad_receiver(void)
{
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO disable PAD receiver API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= ULP_GPIO_MAX; pin++) {
    sl_si91x_gpio_disable_ulp_pad_receiver(pin);
    TEST_ASSERT(((ULP_PAD_CONFIG2_REG->ULP_GPIO_PAD_CONFIG_REG_2.PADCONFIG_REN) & ((BIT(pin)))) == CLR);
  }
  UnityPrintf("Status of API is correct, ULP GPIO disable PAD receiver is fetched successfully \n");

  UnityPrintf("ULP GPIO disable PAD receiver API test completed \n ");
}

/*******************************************************************************
* Function to test the ULP GPIO enable PAD receiver function
*******************************************************************************/
void test_sl_si91x_gpio_enable_ulp_pad_receiver(void)
{
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO enable PAD receiver API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= ULP_GPIO_MAX; pin++) {
    sl_si91x_gpio_enable_ulp_pad_receiver(pin);
    TEST_ASSERT(((ULP_PAD_CONFIG2_REG->ULP_GPIO_PAD_CONFIG_REG_2.PADCONFIG_REN) & (BIT(pin))) == BIT(pin));
  }
  UnityPrintf("Status of API is correct, ULP GPIO enable PAD receiver is fetched successfully \n");

  UnityPrintf("ULP GPIO enable PAD receiver API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO disable clock function
*******************************************************************************/
void test_sl_si91x_gpio_disable_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GPIO disable clock API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (uint8_t clk = 0; clk < MAX_CLK; clk++) {
    sl_si91x_gpio_disable_clock((sl_si91x_gpio_select_clock_t)clk);
    if (clk == M4CLK_GPIO) {
      TEST_ASSERT(SL_PERIPHERAL_CLK->CLK_ENABLE_CLEAR_REG3_b.EGPIO_CLK_ENABLE_b == CLR);
      TEST_ASSERT(SL_PERIPHERAL_CLK->CLK_ENABLE_CLEAR_REG2_b.EGPIO_PCLK_ENABLE_b == CLR);
    } else {
      TEST_ASSERT(ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_CLK_EN_b == CLR);
      TEST_ASSERT(ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_PCLK_ENABLE_b == CLR);
    }
  }
  UnityPrintf("Status of API is correct, GPIO disable clock is fetched successfully \n");

  UnityPrintf("GPIO disable clock API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO enable clock function
*******************************************************************************/
void test_sl_si91x_gpio_enable_clock(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing GPIO enable clock API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (uint8_t clk = 0; clk < MAX_CLK; clk++) {
    sl_si91x_gpio_enable_clock((sl_si91x_gpio_select_clock_t)clk);
    if (clk == M4CLK_GPIO) {
      TEST_ASSERT(SL_PERIPHERAL_CLK->CLK_ENABLE_CLEAR_REG3_b.EGPIO_CLK_ENABLE_b == SET);
      TEST_ASSERT(SL_PERIPHERAL_CLK->CLK_ENABLE_CLEAR_REG2_b.EGPIO_PCLK_ENABLE_b == SET);
    } else {
      TEST_ASSERT(ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_CLK_EN_b == SET);
      TEST_ASSERT(ULPCLK->ULP_MISC_SOFT_SET_REG_b.EGPIO_PCLK_ENABLE_b == SET);
    }
  }
  UnityPrintf("Status of API is correct, GPIO enable clock is fetched successfully \n");

  UnityPrintf("GPIO enable clock API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO PAD selection enable function
*******************************************************************************/
void test_sl_si91x_gpio_enable_pad_selection(void)
{
  uint8_t pad_num;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO enable PAD selection API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pad_num = 1; pad_num < MAX_PAD_SELECT; pad_num++) {
    // Enable pad selection for GPIO pins
    // PadSelection-9 related GPIOS are pre-configured for Other Functions
    if (pad_num != PAD_SELECTION) {
      sl_si91x_gpio_enable_pad_selection(pad_num);
    }
  }
  UnityPrintf("Status of API is correct, GPIO enable PAD selection is fetched successfully \n");

  UnityPrintf("GPIO enable PAD selection API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO disable PAD receiver function
*******************************************************************************/
void test_sl_si91x_gpio_disable_pad_receiver(void)
{
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO disable PAD receiver API \n");

  UnityPrintf("Testing API with valid parameter \n");
  /*
   * NOTE: Do not use GPIO pins 0 to 5 as these are used for flashing.
  It may cause hard fault if these GPIO pins are used for functionality.
  Please avoid using them.
  *
  */
  for (pin = PIN_6; pin <= PIN_57; pin++) {
    sl_si91x_gpio_disable_pad_receiver(pin);
    TEST_ASSERT(PAD_REG(pin)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_REN == CLR);
  }
  UnityPrintf("Status of API is correct, GPIO disable PAD receiver is fetched successfully \n");

  UnityPrintf("GPIO disable PAD receiver API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO enable PAD receiver function
*******************************************************************************/
void test_sl_si91x_gpio_enable_pad_receiver(void)
{
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO enable PAD receiver API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_6; pin <= PIN_57; pin++) {
    sl_si91x_gpio_enable_pad_receiver(pin);
    TEST_ASSERT(PAD_REG(pin)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_REN == SET);
  }
  UnityPrintf("Status of API is correct, GPIO enable PAD receiver is fetched successfully \n");

  UnityPrintf("GPIO enable PAD receiver API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO PAD driver disable state function
*******************************************************************************/
void test_sl_si91x_gpio_select_pad_driver_disable_state(void)
{
  uint8_t driver_disable_state, pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO PAD driver disable state API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_6; pin <= PIN_57; pin++) {
    for (driver_disable_state = 0; driver_disable_state <= MAX_DRIVER_DISABLE_STATE; driver_disable_state++) {
      sl_si91x_gpio_select_pad_driver_disable_state(pin, driver_disable_state);
      TEST_ASSERT(PAD_REG(pin)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_P1_P2 == driver_disable_state);
    }
  }
  UnityPrintf("Status of API is correct, GPIO PAD driver disable state is fetched successfully \n");

  UnityPrintf("GPIO PAD driver disable state API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO select pad driver strength function
*******************************************************************************/
void test_sl_si91x_gpio_select_pad_driver_strength(void)
{
  uint8_t strength, pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO select pad driver strength API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_6; pin <= PIN_57; pin++) {
    for (strength = 0; strength <= MAX_STRENGTH; strength++) {
      sl_si91x_gpio_select_pad_driver_strength(pin, strength);
      TEST_ASSERT(PAD_REG(pin)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_E1_E2 == strength);
    }
  }
  UnityPrintf("Status of API is correct, GPIO select pad driver strength is fetched successfully \n");

  UnityPrintf("GPIO select pad driver strength API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO set pin direction function
*******************************************************************************/
void test_sl_si91x_gpio_set_pin_direction(void)
{
  uint8_t port;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO set pin direction API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (port = 0; SL_GPIO_PORT_VALID(port); port++) {
    for (pin = 0; pin <= GPIO_PIN_MAX; pin++) {
      if (!validate_port_pins(port, pin) && SL_GPIO_PORT_PIN_VALID(port, pin)) {
        sl_si91x_gpio_set_pin_direction(port, pin, CLEAR_OUTPUT);
        TEST_ASSERT(GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.DIRECTION == CLR);
      }
    }
  }
  UnityPrintf("Status of API is correct, GPIO set pin direction is fetched successfully \n");

  UnityPrintf("GPIO set pin direction API test completed \n ");
}

/*******************************************************************************
* Function to test the GPIO get pin direction function of GPIO
*******************************************************************************/
void test_sl_si91x_gpio_get_pin_direction(void)
{
  uint8_t port;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO get pin direction API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (port = 0; SL_GPIO_PORT_VALID(port); port++) {
    for (pin = 0; pin <= GPIO_PIN_MAX; pin++) {
      if (!validate_port_pins(port, pin) && SL_GPIO_PORT_PIN_VALID(port, pin)) {
        sl_si91x_gpio_get_pin_direction(port, pin);
        TEST_ASSERT(GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.DIRECTION == CLR);
      }
    }
  }
  UnityPrintf("Status of API is correct, GPIO get pin direction is fetched successfully \n");

  UnityPrintf("GPIO get pin direction API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO pin function.
*******************************************************************************/
void test_sl_gpio_pin_functions(void)
{
  uint32_t regval, expected;
  uint8_t mode, firstmode = 0, port;
  sl_gpio_mode_t modeval;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO pin function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (port = 0; SL_GPIO_PORT_VALID(port); port++) {
    for (pin = 0; pin <= GPIO_PIN_MAX; pin++) {
      if (!validate_port_pins(port, pin) && SL_GPIO_PORT_PIN_VALID(port, pin)) {
        // Testing GPIO get pin verifying it returns the correct register value
        regval   = sl_gpio_get_pin_input(port, pin);
        expected = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].BIT_LOAD_REG;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Setting pins to this mode 0
        sl_gpio_set_pin_mode(port, pin, (sl_gpio_mode_t)firstmode, OUTPUT_VALUE);
        delay(1); // 1ms delay
        // Testing GPIO get pin verifying it returns the correct register value
        regval   = sl_gpio_get_pin_input(port, pin);
        expected = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].BIT_LOAD_REG;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Initial test of sl_gpio_get_pin_output verifying it returns the correct register value
        regval   = sl_gpio_get_pin_output(port, pin);
        expected = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].BIT_LOAD_REG;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Tests the set, clear and toggle functions
        // Bit is set, value is verified to be 1
        sl_gpio_set_pin_output(port, pin);
        regval   = sl_gpio_get_pin_output(port, pin);
        expected = HIGH;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Bit is cleared, value is verified to be 0
        sl_gpio_clear_pin_output(port, pin);
        regval   = sl_gpio_get_pin_output(port, pin);
        expected = LOW;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Bit is toggled, value is verified to be changed to 1
        sl_gpio_toggle_pin_output(port, pin);
        regval   = sl_gpio_get_pin_output(port, pin);
        expected = HIGH;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
        sl_gpio_toggle_pin_output(port, pin);
        regval   = sl_gpio_get_pin_output(port, pin);
        expected = LOW;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Tests for all modes
        for (mode = 0; mode <= MAX_MODE; mode++) {
          // Run function, set out-bit
          sl_gpio_set_pin_mode(port, pin, (sl_gpio_mode_t)mode, OUTPUT_VALUE);
          expected = mode; // Verify mode is set correctly
          // Read and verify written register values.
          regval = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE;
          TEST_ASSERT_EQUAL_HEX32(expected, regval);
          modeval = sl_gpio_get_pin_mode(port, pin);
          TEST_ASSERT_EQUAL_HEX32(expected, modeval);
          if (mode != SL_GPIO_MODE_DISABLED) { // out-bit is not set if disabled.
            expected = mode;                   // Verify out-bit is set
            regval   = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE;
            TEST_ASSERT_EQUAL_HEX32(expected, regval);
          }
          // Run function, clear out-bit
          sl_gpio_set_pin_mode(port, pin, (sl_gpio_mode_t)firstmode, CLEAR_OUTPUT);
          expected = LOW; // Verify mode is set correctly
          // Read and verify written register values.
          regval = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE;
          TEST_ASSERT_EQUAL_HEX32(expected, regval);
          expected = LOW; // Verify out-bit is cleared
          regval   = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].BIT_LOAD_REG;
          TEST_ASSERT_EQUAL_HEX32(expected, regval);
        } // mode for-loop
        // sets mode back to original
        sl_gpio_set_pin_mode(port, pin, (sl_gpio_mode_t)firstmode, CLEAR_OUTPUT);
        // Read and verify written register values.
        expected = LOW; // Verify mode is set correctly
        regval   = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
      } // if not DEBUG_PORT
    }
  }
  UnityPrintf("Status of API is correct, GPIO pin function is fetched successfully \n");

  UnityPrintf("GPIO pin function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO port functions.
*******************************************************************************/
void test_sl_gpio_port_functions(void)
{
  uint32_t regval, expected, validmask, pins, firstval, port;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO port function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (port = 0; SL_GPIO_PORT_VALID(port); port++) {
    if (!DEBUG_PORT(port)) {
      validmask = _GPIO_NDEBUG_PORT(port);
      // Testing GPIO get port_input verifying it returns the correct register value
      regval   = sl_gpio_get_port_input((sl_gpio_port_t)port) & validmask;
      expected = GPIO->PORT_CONFIG[port].PORT_READ_REG & validmask;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);

      // Testing GPIO get port verifying it returns the correct register value
      regval   = sl_gpio_get_port_output(port) & validmask;
      expected = GPIO->PORT_CONFIG[port].PORT_READ_REG & validmask;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);

      firstval = expected;
      for (pins = 0x0; pins <= validmask; pins++) {
        // Selected bits is set, unlocked values is verified to be 1
        sl_gpio_set_port_output((sl_gpio_port_t)port, pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & pins;
        expected = validmask & pins;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Selected bits is cleared, unlocked values is verified to be 0
        sl_gpio_clear_port_output((sl_gpio_port_t)port, pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & pins;
        expected = LOW;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Bits is toggled, unlocked values is verified to be changed to 1
        sl_gpio_toggle_port_output((sl_gpio_port_t)port, pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & pins;
        expected = validmask & pins;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Bits is toggled, unlocked values is verified to be 0
        sl_gpio_toggle_port_output((sl_gpio_port_t)port, pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & pins;
        expected = LOW;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
        // Selected bits is set, unlocked values is verified to be 1
        // Chooses a random mask (0xf4) to & with pins
        sl_gpio_set_port_output_value((sl_gpio_port_t)port, (pins & RAND_MASK), pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & pins & RAND_MASK;
        expected = validmask & pins & RAND_MASK;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
        // Selected bits is set, unlocked values is verified to be 0
        // Chooses a "random" mask (0xf4)
        sl_gpio_set_port_output_value((sl_gpio_port_t)port, (pins & MASK1), pins);
        regval   = sl_gpio_get_port_output((sl_gpio_port_t)port) & validmask & (MASK1 & pins);
        expected = validmask & (MASK1 & pins);
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
      }
      // Sets port out values back to original ones.
      sl_gpio_set_port_output_value((sl_gpio_port_t)port, firstval, PORT_MASK);
      regval = GPIO->PORT_CONFIG[port].PORT_SET_REG;
      TEST_ASSERT_EQUAL_HEX32(firstval, regval);
    }
  }
  UnityPrintf("Status of API is correct, GPIO port function is fetched successfully \n");

  UnityPrintf("GPIO port function API test completed \n ");
}

/*******************************************************************************
* Function to test ULP GPIO select pad driver disable state.
*******************************************************************************/
void test_sl_si91x_gpio_select_ulp_pad_driver_disable_state(void)
{
  uint8_t driver_disable_state, pin;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO pad driver disable state function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= ULP_GPIO_MAX; pin++) {
    for (driver_disable_state = 0; driver_disable_state < MAX_DRIVER_DISABLE_STATE; driver_disable_state++) {
      sl_si91x_gpio_select_ulp_pad_driver_disable_state(pin,
                                                        (sl_si91x_gpio_driver_disable_state_t)driver_disable_state);
      if (pin <= GPIO_PAD_3) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_P1_P2_1 == driver_disable_state);
      }
      if ((pin >= GPIO_PAD_4) && (pin <= GPIO_PAD_7)) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_P1_P2_2 == driver_disable_state);
      }
      if (pin >= GPIO_PAD_8) {
        TEST_ASSERT(ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_P1_P2_1 == driver_disable_state);
      }
    }
    sl_si91x_gpio_select_ulp_pad_driver_disable_state(pin, (sl_si91x_gpio_driver_disable_state_t)LOW);
  }
  UnityPrintf("Status of API is correct, ULP GPIO pad driver disable state is fetched successfully \n");

  UnityPrintf("ULP GPIO pad driver disable state API test completed \n ");
}

/*******************************************************************************
* Function to test ULP GPIO select pad driver strength.
*******************************************************************************/
void test_sl_si91x_gpio_select_ulp_pad_driver_strength(void)
{
  uint8_t strength, pin;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO pad driver strength function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= ULP_GPIO_MAX; pin++) {
    for (strength = 0; strength < MAX_STRENGTH; strength++) {
      sl_si91x_gpio_select_ulp_pad_driver_strength(pin, (sl_si91x_gpio_driver_strength_select_t)strength);
      if (pin <= GPIO_PAD_3) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_E1_E2_1 == strength);
      }
      if ((pin >= GPIO_PAD_4) && (pin <= GPIO_PAD_7)) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_E1_E2_2 == strength);
      }
      if (pin >= GPIO_PAD_8) {
        TEST_ASSERT(ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_E1_E2_1 == strength);
      }
    }
    sl_si91x_gpio_select_ulp_pad_driver_strength(pin, (sl_si91x_gpio_driver_strength_select_t)LOW);
  }
  UnityPrintf("Status of API is correct, ULP GPIO pad driver strength is fetched successfully \n");

  UnityPrintf("ULP GPIO pad driver strength API test completed \n ");
}

/*******************************************************************************
* Function to test ULP GPIO select pad slew rate.
*******************************************************************************/
void test_sl_si91x_gpio_select_ulp_pad_slew_rate(void)
{
  uint8_t slew_rate, pin;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO pad slew rate function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= ULP_GPIO_MAX; pin++) {
    for (slew_rate = 0; slew_rate < MAX_SLEW_RATE; slew_rate++) {
      sl_si91x_gpio_select_ulp_pad_slew_rate(pin, slew_rate);
      if (pin <= GPIO_PAD_3) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_SR_1 == slew_rate);
      }
      if ((pin >= GPIO_PAD_4) && (pin <= GPIO_PAD_7)) {
        TEST_ASSERT(ULP_PAD_CONFIG0_REG->ULP_GPIO_PAD_CONFIG_REG_0.PADCONFIG_SR_2 == slew_rate);
      }
      if (pin >= GPIO_PAD_8) {
        TEST_ASSERT(ULP_PAD_CONFIG1_REG->ULP_GPIO_PAD_CONFIG_REG_1.PADCONFIG_SR_1 == slew_rate);
      }
    }
    sl_si91x_gpio_select_ulp_pad_slew_rate(pin, CLEAR_OUTPUT);
  }
  UnityPrintf("Status of API is correct, ULP GPIO pad slew rate is fetched successfully \n");

  UnityPrintf("ULP GPIO pad slew rate API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO interrupt.
*******************************************************************************/
void test_sl_interrupt_functions(void)
{
  uint32_t regval, expected, flag, int_var, int_flag;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO interrupts function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (uint8_t interrupt_no = INT_NO_52; interrupt_no <= INT_NO_59; interrupt_no++) {
    NVIC_ClearPendingIRQ(interrupt_no);
    NVIC_DisableIRQ(interrupt_no);
  }
  // Start by disabling interrupts.
  for (flag = 0; flag < 6; flag++) {
    sl_gpio_disable_interrupts((flag << WORD_SHIFT) | ALL_PIN_INTERRUPTS);
    regval   = (GPIO->INTR[flag].GPIO_INTR_CTRL & ALL_PIN_INTERRUPTS); // Interrupt Enable Register
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  // make sure the interrupts are cleared.
  for (flag = 0; flag < MAX_FLAG; flag++) {
    sl_gpio_clear_interrupts(flag);
    regval = ((sl_gpio_get_pending_interrupts() >> (flag * NIBBLE_SHIFT)) & ALL_PIN_INTERRUPTS);
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }

  // Test all combinations of the 8 pin interrupts.
  for (flag = 0; flag < MAX_FLAG; flag++) {
    for (int_flag = 1; int_flag <= ALL_PIN_INTERRUPTS; int_flag = (int_flag << 1)) {
      GPIO->INTR[flag].GPIO_INTR_CTRL_b.PIN_NUMBER  = PIN_12;
      GPIO->INTR[flag].GPIO_INTR_CTRL_b.PORT_NUMBER = PORT0;
      int_var                                       = LOW;
      expected                                      = (flag << SHIFT_16) | int_flag;
      // Test sl_gpio_enable_interrupts
      sl_gpio_enable_interrupts(expected);
      regval = (GPIO->INTR[flag].GPIO_INTR_CTRL & ALL_PIN_INTERRUPTS); // Interrupt Enable Register
      regval = (flag << SHIFT_16) | regval;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
      // Test GPIO set interrupt
      sl_gpio_set_interrupts(expected); // There is no interrupt set register for Si91x
      /*
          * Cortex-M0+ has single cycle GPIO register access. We need a NOP
          * to make sure the change propagate to the IF register.
          */
      __NOP();
      regval = (GPIO->INTR[flag].GPIO_INTR_CTRL & ALL_PIN_INTERRUPTS); // Interrupt Control Register
      regval = (flag << SHIFT_16) | regval;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);

      // Test GPIO get pending register
      regval = ((sl_gpio_get_pending_interrupts() >> (flag * NIBBLE_SHIFT)) & ALL_PIN_INTERRUPTS);
      TEST_ASSERT_EQUAL_HEX32(int_var, regval);

      // Test GPIO get enabled interrupts
      regval =
        ((flag << SHIFT_16) | ((sl_gpio_get_enabled_interrupts() >> (flag * NIBBLE_SHIFT)) & ALL_PIN_INTERRUPTS));
      TEST_ASSERT_EQUAL_HEX32(expected, regval);

      // Test sl_gpio_get_enabled_pending_interrupts
      regval = ((sl_gpio_get_enabled_pending_interrupts() >> (flag * NIBBLE_SHIFT)) & ALL_PIN_INTERRUPTS);
      TEST_ASSERT_EQUAL_HEX32(int_var, regval);

      // Test GPIO Clear interrupt
      sl_gpio_clear_interrupts(flag);
      /*
          * Cortex-M0+ has single cycle GPIO register access. We need a NOP
          * to make sure the change propagate to the IF register.
          */
      __NOP();
      regval = GPIO->INTR[flag].GPIO_INTR_STATUS;
      TEST_ASSERT_EQUAL_HEX32(int_var, regval);
      regval = sl_gpio_get_pending_interrupts();
      TEST_ASSERT_EQUAL_HEX32(int_var, regval);

      // Test GPIO disable interrupt
      sl_gpio_disable_interrupts(expected);
      regval = GPIO->INTR[flag].GPIO_INTR_CTRL & ALL_PIN_INTERRUPTS; // Interrupt Enable Register
      TEST_ASSERT_EQUAL_HEX32(LOW, regval);

      if (int_flag & MASK_1)
        sl_gpio_set_pin_output(GPIO->INTR[flag].GPIO_INTR_CTRL_b.PORT_NUMBER,
                               GPIO->INTR[flag].GPIO_INTR_CTRL_b.PIN_NUMBER);
      if (int_flag & MASK_2)
        sl_gpio_clear_pin_output(GPIO->INTR[flag].GPIO_INTR_CTRL_b.PORT_NUMBER,
                                 GPIO->INTR[flag].GPIO_INTR_CTRL_b.PIN_NUMBER);
    }
  }
  UnityPrintf("Status of API is correct, GPIO interrupt function is fetched successfully \n");

  UnityPrintf("GPIO interrupt function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO interrupt configure.
*******************************************************************************/
void test_sl_interrupt_configure(void)
{
  uint32_t regval, expected;
  uint8_t intno, flag, port;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO interrupt configure function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  // Testing function for different permutations of all parameters
  for (port = 0; SL_GPIO_PORT_VALID(port); port++) {
    for (pin = 0; pin <= GPIO_PIN_MAX; pin++) {
      if (!validate_port_pins(port, pin) && SL_GPIO_PORT_PIN_VALID(port, pin)) {
        for (intno = 0; intno < MAX_FLAG; intno++) {
          for (flag = 0; flag <= MAX_FLAG; flag++) {
            sl_gpio_configure_interrupt(port, pin, intno, flag);
            regval = GPIO->INTR[intno].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE;
            if (flag & SL_GPIO_INTERRUPT_RISE_EDGE) {
              expected = HIGH;
            } else {
              expected = LOW;
            }
            TEST_ASSERT_EQUAL_HEX32(expected, regval);

            // Set regval to written register value, then verify it
            regval = GPIO->INTR[intno].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE;
            if (flag & SL_GPIO_INTERRUPT_FALL_EDGE) {
              expected = HIGH;
            } else {
              expected = LOW;
            }
            TEST_ASSERT_EQUAL_HEX32(expected, regval);

            // Set regval to written register value, then verify it
            regval = GPIO->INTR[intno].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE;
            if (flag & SL_GPIO_INTERRUPT_LEVEL_HIGH) {
              expected = HIGH;
            } else {
              expected = LOW;
            }
            TEST_ASSERT_EQUAL_HEX32(expected, regval);

            // Set regval to written register value, then verify it
            regval = GPIO->INTR[intno].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE;
            if (flag & SL_GPIO_INTERRUPT_LEVEL_LOW) {
              expected = HIGH;
            } else {
              expected = LOW;
            }
            TEST_ASSERT_EQUAL_HEX32(expected, regval);
          }
        }
      }
    }
  }
  UnityPrintf("Status of API is correct, GPIO interrupt configure function is fetched successfully \n");

  UnityPrintf("GPIO interrupt configure function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO pin mode glitch.
*******************************************************************************/
void test_sl_gpio_pin_mode_int_glitch(void)
{
  uint32_t regval, originalmode;
  uint8_t pin, port;
  port = SL_GPIO_PORT_D;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO interrupt glitch function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  if (!SL_GPIO_PORT_VALID(port)) {
    UnityPrintf("Warning: Port D not available for test on device\n");
    return;
  }
  for (pin = 0; pin < PIN_8; pin++) {
    if (validate_port_pins(port, pin) || !SL_GPIO_PORT_PIN_VALID(port, pin)) {
      continue;
    }
    // Store original mode for this port pin
    originalmode = GPIO->PIN_CONFIG[(port * MAX_GPIO_PORT_PIN) + pin].GPIO_CONFIG_REG_b.MODE;

    // Set pin high
    sl_gpio_set_pin_mode(port, pin, _MODE0, OUTPUT_VALUE);
    // Give some time for mode to be set
    delay(1);
    // Enable rising edge interrupt on pin
    sl_gpio_configure_interrupt(port, pin, pin, RISE_EDGE_EVENT);
    // Clear interrupt
    sl_gpio_clear_interrupts(pin);

    // Set pin low, this should not cause rising edge interrupt flag
    sl_gpio_set_pin_mode(port, pin, _MODE0, CLEAR_OUTPUT);
    // Give some time for mode to be set
    delay(1);
    regval = GPIO->INTR[pin].GPIO_INTR_STATUS_b.INTERRUPT_STATUS;
    TEST_ASSERT_EQUAL_HEX32(LOW, regval);

    // Enable falling edge interrupt on pin
    sl_gpio_configure_interrupt(port, pin, pin, FALL_EDGE_EVENT);
    // Clear interrupt
    sl_gpio_clear_interrupts(pin);

    // Set pin high; this should not cause falling edge interrupt flag
    sl_gpio_set_pin_mode(port, pin, _MODE0, OUTPUT_VALUE);
    regval = GPIO->INTR[pin].GPIO_INTR_STATUS_b.INTERRUPT_STATUS;
    TEST_ASSERT_EQUAL_HEX32(LOW, regval);

    // Disable interrupt on pin
    sl_gpio_configure_interrupt(port, pin, _MODE2, SL_GPIO_INTERRUPT_DISABLE);
    sl_gpio_set_pin_mode(port, pin, (sl_gpio_mode_t)originalmode, CLEAR_OUTPUT);
  }
  UnityPrintf("Status of API is correct, GPIO interrupt glitch function is fetched successfully \n");

  UnityPrintf("GPIO interrupt glitch function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO slew rate.
*******************************************************************************/
void test_sl_gpio_slewrate(void)
{
  uint8_t regval, expected, initial_val, port, slewrate;

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO slew rate function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (port = 0; port <= GPIO_PORT_MAX; port++) {
    for (uint8_t i = (port * MAX_GPIO_PORT_PIN); i < (MAX_GPIO_PORT_PIN * (port + 1)); i++) {
      if (!SL_GPIO_PORT_VALID(port)) {
        continue;
      }
      // Store initial slewrate
      initial_val = PAD_REG(i)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_SR;

      // Test all slew rates
      for (slewrate = 0; slewrate < MAX_SLEW_RATE; slewrate++) {
        sl_gpio_set_slew_rate((sl_gpio_port_t)port, slewrate, CLEAR_OUTPUT);
        if (slewrate == CLEAR_OUTPUT) {
          regval   = PAD_REG(i)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_SR;
          expected = LOW;
          TEST_ASSERT_EQUAL(expected, regval);
        } else {
          regval   = PAD_REG(i)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_SR;
          expected = HIGH;
          TEST_ASSERT_EQUAL(expected, regval);
        }
      }
      // Reset slew rate
      PAD_REG(i)->GPIO_PAD_CONFIG_REG_b.PADCONFIG_SR = (sl_si91x_gpio_slew_rate_t)initial_val;
    }
  }
  UnityPrintf("Status of API is correct, GPIO slew rate is fetched successfully \n");

  UnityPrintf("GPIO slew rate function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO interrupt configure.
*******************************************************************************/
void test_sl_si91x_gpio_configure_ulp_interrupt(void)
{
  uint32_t regval, expected;
  uint8_t port = ULP_PORT, intno, flag, pin = ULP_PIN_0;

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO interrupt configure function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  // Testing function for different permutations
  if (!validate_port_pins(port, pin) && SL_GPIO_PORT_PIN_VALID(port, pin)) {
    for (intno = 0; intno < INTR_6; intno++) {
      for (flag = 0; flag <= MAX_FLAG; flag++) {
        sl_si91x_gpio_configure_ulp_pin_interrupt(flag, intno, pin);
        // Set regval to written register value, then verify it
        regval = ULP_GPIO->INTR[intno].GPIO_INTR_CTRL_b.RISE_EDGE_ENABLE;
        if (flag & SL_GPIO_INTERRUPT_RISE_EDGE) {
          expected = HIGH;
        } else {
          expected = LOW;
        }
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Set regval to written register value, then verify it
        regval = ULP_GPIO->INTR[intno].GPIO_INTR_CTRL_b.FALL_EDGE_ENABLE;
        if (flag & SL_GPIO_INTERRUPT_FALL_EDGE) {
          expected = HIGH;
        } else {
          expected = LOW;
        }
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Set regval to written register value, then verify it
        regval = GPIO->INTR[intno].GPIO_INTR_CTRL_b.LEVEL_HIGH_ENABLE;
        if (flag & SL_GPIO_INTERRUPT_LEVEL_HIGH) {
          expected = HIGH;
        } else {
          expected = LOW;
        }
        TEST_ASSERT_EQUAL_HEX32(expected, regval);

        // Set regval to written register value, then verify it
        regval = ULP_GPIO->INTR[intno].GPIO_INTR_CTRL_b.LEVEL_LOW_ENABLE;
        if (flag & SL_GPIO_INTERRUPT_LEVEL_LOW) {
          expected = HIGH;
        } else {
          expected = LOW;
        }
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
        // Clear ULP interrupt
        sl_si91x_gpio_clear_ulp_interrupt(flag);
        regval   = ULP_GPIO->INTR[flag].GPIO_INTR_STATUS_b.INTERRUPT_STATUS;
        expected = HIGH;
        TEST_ASSERT_EQUAL_HEX32(expected, regval);
      }
    }
  }
  UnityPrintf("Status of API is correct, ULP GPIO interrupt configure is fetched successfully \n");

  UnityPrintf("ULP GPIO interrupt configure function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO configure group interrupt.
*******************************************************************************/
void test_sl_si91x_gpio_configure_group_interrupt(void)
{
  uint32_t regval, expected;
  sl_si91x_gpio_group_interrupt_config_t grp_config_int;
  uint8_t group_pins[PIN_COUNT] = { PIN25, PIN26 };
  uint8_t group_port[PIN_COUNT] = { PORT0, PORT0 };
  uint8_t group_pol[PIN_COUNT]  = { POLARITY, POLARITY };

  grp_config_int.grp_interrupt     = GROUP_INT_1;
  grp_config_int.grp_interrupt_cnt = GRP_COUNT;
  grp_config_int.and_or            = OR_EVENT;
  grp_config_int.level_edge        = LEVEL_EVENT;
  memcpy(grp_config_int.grp_interrupt_pin, group_pins, PIN_COUNT);
  memcpy(grp_config_int.grp_interrupt_port, group_port, PIN_COUNT);
  memcpy(grp_config_int.grp_interrupt_pol, group_pol, PIN_COUNT);

  UnityPrintf("\n");
  UnityPrintf("Testing GPIO configure group interrupt function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  sl_si91x_gpio_configure_group_interrupt(&grp_config_int);
  for (uint8_t int_pin_count = 0; int_pin_count < grp_config_int.grp_interrupt_cnt; int_pin_count++) {
    // Set regval to written register value, then verify it
    regval =
      GPIO->PIN_CONFIG[grp_config_int.grp_interrupt_pin[int_pin_count]].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Get group interrupt level edge
    regval   = sl_si91x_gpio_get_group_interrupt_level_edge(ULP_PORT, grp_config_int.grp_interrupt);
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR;
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT;
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Clear group interrupt
    sl_si91x_gpio_clear_group_interrupt(grp_config_int.grp_interrupt);
    regval   = GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_STS_b.INTERRUPT_STATUS;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Get group interrupt status
    sl_si91x_gpio_get_group_interrupt_status(SL_ULP_GPIO_PORT, grp_config_int.grp_interrupt);
    regval   = GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_STS;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct, GPIO configure group interrupt is fetched successfully \n");

  UnityPrintf("GPIO configure group interrupt function API test completed \n ");
}

/*******************************************************************************
* Function to test GPIO configure group interrupt.
*******************************************************************************/
void test_sl_si91x_gpio_configure_ulp_group_interrupt(void)
{
  uint32_t regval, expected;
  sl_si91x_gpio_group_interrupt_config_t grp_config_int;
  uint8_t group_pins[ULP_PIN_COUNT] = { ULP_PIN_4, ULP_PIN_5 };
  uint8_t group_pol[ULP_PIN_COUNT]  = { POLARITY, POLARITY };

  UnityPrintf("\n");
  UnityPrintf("Testing ULP GPIO configure group interrupt function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  grp_config_int.grp_interrupt     = ULP_GROUP_INT;
  grp_config_int.grp_interrupt_cnt = ULP_PIN_COUNT;
  grp_config_int.and_or            = OR_EVENT;
  grp_config_int.level_edge        = LEVEL_EVENT;
  memcpy(grp_config_int.grp_interrupt_pin, group_pins, PIN_COUNT);
  memcpy(grp_config_int.grp_interrupt_pol, group_pol, PIN_COUNT);

  sl_si91x_gpio_configure_ulp_group_interrupt(&grp_config_int);

  for (uint8_t int_pin_count = 0; int_pin_count < grp_config_int.grp_interrupt_cnt; int_pin_count++) {
    // Set regval to written register value, then verify it
    regval =
      ULP_GPIO->PIN_CONFIG[grp_config_int.grp_interrupt_pin[int_pin_count]].GPIO_CONFIG_REG_b.GROUP_INTERRUPT1_POLARITY;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Get group interrupt level edge
    regval   = sl_si91x_gpio_get_group_interrupt_level_edge(ULP_PORT, grp_config_int.grp_interrupt);
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.AND_OR;
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.ENABLE_INTERRUPT;
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Set regval to written register value, then verify it
    regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Clear group interrupt
    sl_si91x_gpio_clear_ulp_group_interrupt(grp_config_int.grp_interrupt);
    regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_STS_b.INTERRUPT_STATUS;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
    // Get group interrupt status
    sl_si91x_gpio_get_group_interrupt_status(SL_ULP_GPIO_PORT, grp_config_int.grp_interrupt);
    regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_STS;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  // Mask group interrupt
  sl_si91x_gpio_mask_group_interrupt(SL_ULP_GPIO_PORT, grp_config_int.grp_interrupt);
  regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_CTRL_REG_b.MASK;
  expected = HIGH;
  TEST_ASSERT_EQUAL_HEX32(expected, regval);
  // Clear group interrupt
  sl_si91x_gpio_clear_ulp_group_interrupt(grp_config_int.grp_interrupt);
  regval   = ULP_GPIO->GPIO_GRP_INTR[grp_config_int.grp_interrupt].GPIO_GRP_INTR_STS_b.INTERRUPT_STATUS;
  expected = LOW;
  TEST_ASSERT_EQUAL_HEX32(expected, regval);
  UnityPrintf("Status of API is correct, ULP GPIO configure group interrupt function is fetched successfully \n");

  UnityPrintf("ULP GPIO configure group interrupt function API test completed \n ");
}

/*******************************************************************************
* Function to test UULP GPIO receiver.
*******************************************************************************/
void test_sl_si91x_gpio_select_uulp_npss_receiver(void)
{
  uint32_t regval, expected;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing UULP GPIO NPSS receiver function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_0; pin <= PIN_4; pin++) {
    sl_si91x_gpio_select_uulp_npss_receiver(pin, HIGH);
    regval   = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_REN;
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct, UULP GPIO NPSS receiver function is fetched successfully \n");

  UnityPrintf("UULP GPIO NPSS receiver function API test completed \n ");
}

/*******************************************************************************
* Function to test Set UULP GPIO direction.
*******************************************************************************/
void test_sl_si91x_gpio_set_uulp_npss_direction(void)
{
  uint32_t regval, expected;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing UULP GPIO NPSS direction function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_0; pin <= PIN_4; pin++) {
    sl_si91x_gpio_set_uulp_npss_direction(pin, LOW);
    regval   = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OEN;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct, UULP GPIO NPSS direction function is fetched successfully \n");

  UnityPrintf(" UULP GPIO NPSS direction function API test completed \n ");
}

/*******************************************************************************
* Function to test Get UULP GPIO direction.
*******************************************************************************/
void test_sl_si91x_gpio_get_uulp_npss_direction(void)
{
  uint32_t regval, expected;
  uint8_t pin;

  UnityPrintf("\n");
  UnityPrintf("Testing  UULP GPIO NPSS get direction function API \n");

  UnityPrintf("Testing API with valid parameter \n");

  for (pin = PIN_0; pin <= PIN_4; pin++) {
    sl_si91x_gpio_get_uulp_npss_direction(pin);
    regval   = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_OEN;
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct,UULP GPIO NPSS get direction function is fetched successfully \n");

  UnityPrintf("UULP GPIO NPSS get direction function API test completed \n ");
}

/*******************************************************************************
* Function to test UULP GPIO interrupt configure.
*******************************************************************************/
void test_sl_si91x_gpio_configure_uulp_interrupt(void)
{
  uint32_t regval, expected;
  uint8_t intno, flag;

  UnityPrintf("\n");
  UnityPrintf("Testing UULP GPIO interrupt configure API \n");

  UnityPrintf("Testing API with valid parameter \n");
  // Testing function for different permutations
  for (intno = 1; intno <= INTR_4; intno++) {
    for (flag = 1; flag <= MAX_FLAG; flag = (flag << 1)) {
      sl_si91x_gpio_configure_uulp_interrupt(flag, intno);
      // Set regval to written register value, then verify it
      if (flag & SL_GPIO_INTERRUPT_RISE_EDGE) {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG & (intno << BIT_0));
        expected = (intno << BIT_0);
      } else {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG) & (intno << BIT_0);
        expected = LOW;
      }
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
      // Set regval to written register value, then verify it
      if (flag & SL_GPIO_INTERRUPT_FALL_EDGE) {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG & (intno << BIT_8));
        expected = (intno << BIT_8);
      } else {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG) & (intno << BIT_8);
        expected = LOW;
      }
      TEST_ASSERT_EQUAL_HEX32(expected, regval);

      // Set regval to written register value, then verify it
      if (flag & SL_GPIO_INTERRUPT_LEVEL_LOW) {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG & (intno << BIT_16));
        expected = (intno << BIT_16);
      } else {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG) & (intno << BIT_16);
        expected = LOW;
      }
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
      // Set regval to written register value, then verify it
      if (flag & SL_GPIO_INTERRUPT_LEVEL_HIGH) {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG & (intno << BIT_24));
        expected = (intno << BIT_24);
      } else {
        regval   = (GPIO_NPSS_GPIO_CONFIG_REG) & (intno << BIT_24);
        expected = LOW;
      }
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
    }
    // Mask NPSS interrupt
    sl_si91x_gpio_mask_uulp_npss_interrupt(intno);
    regval   = GPIO_NPSS_INTERRUPT_MASK_SET_REG & BIT(intno);
    expected = BIT(intno);
    TEST_ASSERT_EQUAL_HEX32(expected, regval);

    // Clear NPSS interrupt
    sl_si91x_gpio_clear_uulp_interrupt(intno);
    regval   = (GPIO_NPSS_INTERRUPT_CLEAR_REG)&BIT(intno);
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct, UULP GPIO pin interrupt configure is fetched successfully \n");

  UnityPrintf("UULP GPIO pin function API test completed \n ");
}

/*******************************************************************************
* Function to test UULP GPIO pin function.
*******************************************************************************/
void test_sl_si91x_gpio_uulp_pin_functions(void)
{
  uint8_t pin, mode;
  uint32_t regval, expected;

  UnityPrintf("\n");
  UnityPrintf("Testing UULP GPIO pin function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = 0; pin <= PIN_4; pin++) {
    // Testing GPIO get pin verifying it returns the correct register value
    sl_si91x_gpio_set_uulp_npss_pin_mux(pin, NPSS_GPIO_PIN_MUX_MODE0);
    regval   = sl_si91x_gpio_get_uulp_npss_pin(pin);
    expected = ((UULP_GPIO_STATUS >> pin) & MASK_1);
    TEST_ASSERT_EQUAL_HEX32(expected, regval);

    // Bit is set, value is verified to be 1
    sl_si91x_gpio_set_uulp_npss_pin_value(pin, OUTPUT_VALUE);
    regval   = sl_si91x_gpio_get_uulp_npss_pin(pin);
    expected = HIGH;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);

    sl_si91x_gpio_set_uulp_npss_pin_value(pin, CLEAR_OUTPUT);
    delay(1);
    // Test GPIO get pin
    regval   = sl_si91x_gpio_get_uulp_npss_pin(pin);
    expected = LOW;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);

    // Tests the sl_gpio_set_pin_mode and sl_gpio_get_pin_mode function
    // Tests for all modes
    for (mode = 0; mode <= MODE_7; mode++) {
      // Run function, set out-bit
      sl_si91x_gpio_set_uulp_npss_pin_mux(pin, (sl_si91x_uulp_npss_mode_t)mode);
      expected = mode; // Verify mode is set correctly

      // Read and verify written register values.
      regval = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
    } // mode for-loop

    // sets mode back to original
    sl_si91x_gpio_set_uulp_npss_pin_mux(pin, (sl_si91x_uulp_npss_mode_t)CLEAR_OUTPUT);

    // Read and verify written register values.
    expected = LOW; // Verify mode is set correctly
    regval   = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE;
    TEST_ASSERT_EQUAL_HEX32(expected, regval);
  }
  UnityPrintf("Status of API is correct, UULP GPIO pin function is fetched successfully \n");

  UnityPrintf("UULP GPIO pin function API test completed \n ");
}

/*******************************************************************************
* Function to test UULP GPIO set pin mux function.
*******************************************************************************/
void test_sl_si91x_gpio_set_uulp_npss_pin_mux(void)
{
  uint32_t regval, expected;
  uint8_t pin, mode;
  UnityPrintf("\n");
  UnityPrintf("Testing UULP GPIO NPSS pin mux function API \n");

  UnityPrintf("Testing API with valid parameter \n");
  for (pin = PIN_0; pin <= PIN_4; pin++) {
    for (mode = 0; mode <= MODE_7; mode++) {
      sl_si91x_gpio_set_uulp_npss_pin_mux(pin, mode);
      regval   = UULP_GPIO->NPSS_GPIO_CNTRL[pin].NPSS_GPIO_CTRLS_b.NPSS_GPIO_MODE;
      expected = mode;
      TEST_ASSERT_EQUAL_HEX32(expected, regval);
    }
    sl_si91x_gpio_set_uulp_npss_pin_mux(pin, NPSS_GPIO_PIN_MUX_MODE0);
  }
  UnityPrintf("Status of API is correct, UULP GPIO NPSS pin mux function is fetched successfully \n");

  UnityPrintf("UULP GPIO NPSS pin mux function API test completed \n ");
}

/*******************************************************************************
 * Function to test the get version API
 ******************************************************************************/
void test_gpio_get_version(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing gpio get version  \n");
  sl_si91x_gpio_version_t version;

  UnityPrintf("Testing API with valid parameter \n");
  version = sl_si91x_gpio_get_version();
  UnityPrintf("Verifying gpio API version \n");
  TEST_ASSERT_EQUAL_INT(0, version.major);
  TEST_ASSERT_EQUAL_INT(0, version.release);
  TEST_ASSERT_EQUAL_INT(2, version.minor);
  UnityPrintf("API version is correct\n");
  UnityPrintf("gpio get version test completed \n ");
}
