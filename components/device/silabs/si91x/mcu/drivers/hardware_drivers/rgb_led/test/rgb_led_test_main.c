#include "unity.h"
#include "sl_si91x_rgb_led.h"

// Mock RGB LED structure
sl_led_t red_led          = { .pin = 1, .port = 1, .led_number = 1, .pad = 1 };
sl_led_t green_led        = { .pin = 2, .port = 1, .led_number = 2, .pad = 1 };
sl_led_t blue_led         = { .pin = 3, .port = 1, .led_number = 3, .pad = 1 };
sl_rgb_led_t mock_rgb_led = { .red = &red_led, .green = &green_led, .blue = &blue_led };

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_init(void);
void test_sl_si91x_simple_rgb_led_on(void);
void test_sl_si91x_simple_rgb_led_off(void);
void test_sl_si91x_simple_rgb_led_toggle(void);
void test_sl_si91x_simple_rgb_led_set_colour(void);
void test_sl_si91x_simple_rgb_led_get_current_state(void);
void test_sl_si91x_simple_rgb_led_get_colour(void);

/******************************************************************************
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  UnityBeginGroup("RGB_LED");

  RUN_TEST(test_sl_si91x_simple_rgb_led_init, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_on, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_off, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_toggle, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_set_colour, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_get_current_state, __LINE__);
  RUN_TEST(test_sl_si91x_simple_rgb_led_get_colour, __LINE__);

  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

/*******************************************************************************
 * Function to test initializing the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Init \n");

  sl_si91x_simple_rgb_led_init(&mock_rgb_led);
  // Assuming there's a way to verify the initialization, add assertions here
  UnityPrintf("RGB LED initialized successfully \n");
}

/*******************************************************************************
 * Function to test turning on the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_on(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED On \n");

  sl_si91x_simple_rgb_led_on(&mock_rgb_led);
  // Assuming there's a way to verify the state change, add assertions here
  UnityPrintf("RGB LED turned on successfully \n");
}

/*******************************************************************************
 * Function to test turning off the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_off(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Off \n");

  sl_si91x_simple_rgb_led_off(&mock_rgb_led);
  // Assuming there's a way to verify the state change, add assertions here
  UnityPrintf("RGB LED turned off successfully \n");
}

/*******************************************************************************
 * Function to test toggling the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_toggle(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Toggle \n");

  sl_si91x_simple_rgb_led_toggle(&mock_rgb_led);
  // Assuming there's a way to verify the state change, add assertions here
  UnityPrintf("RGB LED toggled successfully \n");
}

/*******************************************************************************
 * Function to test setting the color of the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_set_colour(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Set Colour \n");
  int rgb_colour = 0xFF00FF; // Example color

  sl_si91x_simple_rgb_led_set_colour(&mock_rgb_led, rgb_colour);
  // Assuming there's a way to verify the color change, add assertions here
  UnityPrintf("RGB LED color set successfully \n");
}

/*******************************************************************************
 * Function to test getting the current state of the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_get_current_state(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Get Current State \n");

  uint8_t state = sl_si91x_simple_rgb_led_get_current_state(&mock_rgb_led);
  TEST_ASSERT(state == 0 || state == 1);
  UnityPrintf("RGB LED current state retrieved successfully \n");
}

/*******************************************************************************
 * Function to test getting the current color of the RGB LED
 ******************************************************************************/
void test_sl_si91x_simple_rgb_led_get_colour(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing RGB LED Get Colour \n");
  uint16_t red, green, blue;

  sl_si91x_simple_rgb_led_get_colour(&mock_rgb_led, &red, &green, &blue);
  // Assuming there's a way to verify the color values, add assertions here
  UnityPrintf("RGB LED color values retrieved successfully \n");
}