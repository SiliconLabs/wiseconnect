/***************************************************************************/ /**
 * @file
 * @brief Calendar Unity Test Functions
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
 ******************************************************************************
 ******************************************************************************/

#include "sl_si91x_usart.h"
#include "rsi_board.h"
#include "unity.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define MAX_BAUDRATE   7372800
#define API_VERSION    514
#define DRIVER_VERSION 522
#define BUFFER_SIZE    1024

/*******************************************************************************
 ***************************  Global Variables   *******************************
 ******************************************************************************/
//extern void Driver_USART0;
sl_usart_handle_t usart_handle;

/*******************************************************************************
 ***************************  Local Variables   *******************************
 ******************************************************************************/
volatile boolean_t send_complete = false, transfer_complete = false, receive_complete = false;

/*******************************************************************************
 ************************  Static Function Prototypes   ************************
 ******************************************************************************/
static void default_usart_config_struct(sl_si91x_usart_control_config_t *config);
void callback_event(uint32_t event);

/*******************************************************************************
 ************************  Test Function Prototypes  ****************************
 ******************************************************************************/
void test_usart_init(void);
void test_usart_power_control(void);
void test_usart_control(void);
void test_usart_get_configurations(void);
void test_register_and_unregister_callback(void);
void test_usart_loopback_data(void);
void test_usart_receive_data(void);
void test_usart_send_data(void);
void test_usart_transfer_data(void);
void test_usart_deinit(void);

/***************************************************************************/ /**
 * Main function in which all the test cases are tested using unity framework
 ******************************************************************************/
int main()
{
  sl_system_init();
  UnityBeginGroup("USART");

  RUN_TEST(test_usart_init, __LINE__);
  RUN_TEST(test_usart_power_control, __LINE__);
  RUN_TEST(test_usart_control, __LINE__);
  RUN_TEST(test_usart_get_configurations, __LINE__);
  RUN_TEST(test_register_and_unregister_callback, __LINE__);
  RUN_TEST(test_usart_receive_data, __LINE__);
  RUN_TEST(test_usart_send_data, __LINE__);
  RUN_TEST(test_usart_deinit, __LINE__);
  UnityEnd();
  UnityPrintf("END");
  while (1) {
  }
}

void callback_event(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      send_complete = true;
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      receive_complete = true;
      break;
    case SL_USART_EVENT_TRANSFER_COMPLETE:
      transfer_complete = true;
      break;
  }
}

void test_usart_init(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Initialization \n");
  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_init(USART_0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with correct parameters \n");
  status = sl_si91x_usart_init(USART_0, &usart_handle);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, USART is initialized successfully \n");

  UnityPrintf("USART Initialization test completed \n");
}

void test_usart_deinit(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Un-initialization \n");

  sl_status_t status;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_deinit(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  status = sl_si91x_usart_deinit(usart_handle);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("\nStatus of API is correct, USART is deinitd successfully \n");
  UnityPrintf("USART Un-initialization test completed \n");
}

void test_usart_power_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Power Control \n");
  sl_status_t status;
  sl_si91x_usart_control_config_t usart_config;

  UnityPrintf("Testing Power Off Mode \n");
  status = sl_si91x_usart_set_power_mode(usart_handle, SL_POWER_OFF);
  DEBUGINIT();
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, USART power mode is configured successfully \n");

  UnityPrintf("Testing Low Power Mode \n");
  status = sl_si91x_usart_set_power_mode(usart_handle, SL_POWER_LOW);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_SUPPORTED, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing with invalid parameters \n");
  status = sl_si91x_usart_set_power_mode(usart_handle, SL_POWER_MODE_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing Full Power mode \n");
  status = sl_si91x_usart_set_power_mode(usart_handle, SL_POWER_FULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, usart speed is configured successfully \n");
  UnityPrintf("usart Power Control test completed \n ");
}

void test_usart_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Control \n");
  sl_status_t status;
  sl_si91x_usart_control_config_t usart_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_set_configuration(usart_handle, NULL, 115200);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with false baud rate \n");
  usart_config.baudrate = MAX_BAUDRATE + 1;
  status                = sl_si91x_usart_set_configuration(usart_handle, &usart_config, usart_config.baudrate);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_INVALID_PARAMETER, status);
  UnityPrintf("Status of API is correct, it returns expected error \n");

  UnityPrintf("Testing Asynchronous mode \n");
  usart_config.mode = SL_USART_MODE_ASYNCHRONOUS;
  default_usart_config_struct(&usart_config);
  status = sl_si91x_usart_set_configuration(usart_handle, &usart_config, 115200);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, USART_MODE_ASYNCHRONOUS  is set successfully \n");

  UnityPrintf("Testing with all the correct parameters \n");
  default_usart_config_struct(&usart_config);
  status = sl_si91x_usart_set_configuration(usart_handle, &usart_config, 115200);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, usart configurations are set successfully \n");

  UnityPrintf("usart Control test completed \n ");
}

void test_usart_get_configurations(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART get configurations \n");
  sl_status_t status;
  sl_si91x_usart_control_config_t usart_config;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_get_configurations(0, NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  /*  USART_0           0
    UART_1            1
    ULPUART         2 */
  status = sl_si91x_usart_get_configurations(0, &usart_config);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("\nStatus of API is correct, USART configurations retrived successfully \n");
  UnityPrintf("USART Get usart configurations test completed \n");
}

void test_usart_receive_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Receive Data \n");
  sl_status_t status;
  uint8_t data_in[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_receive_data(usart_handle, NULL, sizeof(data_in));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  //TODO: This test cases tested as part of example code

  UnityPrintf("usart Receive Data test completed \n ");
}

void test_usart_send_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Send Data \n");
  sl_status_t status;
  uint8_t data_out[BUFFER_SIZE];

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_send_data(usart_handle, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  //TODO: This test case tested as part of example code

  UnityPrintf("usart Send Data test completed \n ");
}

void test_register_and_unregister_callback(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART register and unregister callback \n");
  sl_status_t status;

  UnityPrintf("Registering callback \n");
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");
  status = sl_si91x_usart_register_event_callback(NULL);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");
  status = sl_si91x_usart_register_event_callback(callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is registered successfully \n");

  status = sl_si91x_usart_register_event_callback(callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_BUSY, status);
  UnityPrintf("Status of API is correct, Callback is already registered, cannot register another\n");
  UnityPrintf("Unregistering the callback \n");
  sl_si91x_usart_unregister_event_callback();
  UnityPrintf("Callback is unregistered successfully \n");
  status = sl_si91x_usart_register_event_callback(callback_event);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK, status);
  UnityPrintf("Status of API is correct, Callback is again registered successfully \n");
}

void test_usart_set_modem_control(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing USART Set Modem Control \n");
  sl_status_t status;

  UnityPrintf("Testing with Invalid option \n");
  status = sl_si91x_usart_set_modem_control(usart_handle, SL_USART_MODEM_CONTROL_LAST);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_SUPPORTED, status);
  UnityPrintf("Status of API is correct, Invalid option Test Passed successfully \n");

  sl_usart_modem_control_t modem_control;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_set_modem_control(usart_handle, SL_USART_RTS_CLEAR);
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NOT_SUPPORTED, status);
  UnityPrintf("Status of API is correct, sl_si91x_usart_set_modem_control Test Passed successfully \n");

  UnityPrintf("USART set Modem Control test completed \n ");
}

void test_usart_transfer_data(void)
{
  UnityPrintf("\n");
  UnityPrintf("Testing usart Transfer \n");
  sl_status_t status;
  uint8_t data_in[BUFFER_SIZE];
  uint8_t data_out[BUFFER_SIZE];
  uint16_t data_index = 0;

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_transfer_data(usart_handle, NULL, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  UnityPrintf("Testing with NULL pointer \n");
  status = sl_si91x_usart_transfer_data(usart_handle, data_out, NULL, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_NULL_POINTER, status);
  UnityPrintf("Status of API is correct, Null Pointer Test Passed successfully \n");

  // This test case need to be test in synchronous master and slave mode
  /*
  UnityPrintf("Testing API with valid parameter \n");
  UnityPrintf("Loopback Test started \n");
  for (int i=0; i<BUFFER_SIZE; i++) {
      data_out[i] = i+1;
  }
  //Supports only in synchronous master and slave
  status = sl_si91x_usart_transfer_data(data_out, data_in, sizeof(data_out));
  TEST_ASSERT_EQUAL_HEX(SL_STATUS_OK,status);
  UnityPrintf("Status of API is correct, usart data transfer begins successfully \n");

  while(!event_stat.send_complete)
    ;
  event_stat.send_complete = 0;

  UnityPrintf("usart data transfer complete \n");
  UnityPrintf("Comparing the values of send data and receive \n");
  for (data_index = 0; data_index < BUFFER_SIZE; data_index++) {
      if (data_in[data_index] == data_out[data_index]) {
          continue;
      }
      else {
          break;
      }
  }
  TEST_ASSERT_EQUAL_INT(BUFFER_SIZE,data_index);
  UnityPrintf("Output and Input values are same \n");
  UnityPrintf("Loopback Test passed \n");
   */
  UnityPrintf("usart transfer data test completed \n ");
}

static void default_usart_config_struct(sl_si91x_usart_control_config_t *config)
{
  config->baudrate      = 115200;
  config->mode          = SL_USART_MODE_ASYNCHRONOUS;
  config->parity        = SL_USART_NO_PARITY;
  config->stopbits      = SL_USART_STOP_BITS_2;
  config->hwflowcontrol = SL_USART_FLOW_CONTROL_NONE;
  config->databits      = SL_USART_DATA_BITS_8;
  config->misc_control  = SL_USART_MISC_CONTROL_NONE;
}
