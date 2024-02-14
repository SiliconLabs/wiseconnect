/***************************************************************************/ /**
 * @file ulp_uart_example.c
 * @brief Ulp UART examples functions
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
#include "sl_si91x_usart.h"
#include "rsi_debug.h"
#include "ulp_uart_example.h"
#include "rsi_egpio.h"
#include "rsi_rom_egpio.h"
#include "sl_si91x_driver_gpio.h"
/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/

#define ULP_UART_BUFFER_SIZE 1024   // Data send and receive length
#define USART_BAUDRATE       115200 // Baud rate <9600-7372800>
#define ULP_GPIO_PIN         2      // ULP GPIO to receive
#define OUTPUT_VALUE         1      // GPIO output value
#define ULP_GPIO_PORT        4      // GPIO Port no
#define ULP_GPIO_TOGGLE      10     // ULP GPIO to toggle
#define SET                  1      // Macro to set
#define CLR                  0      // Macro to clear
#define NON_UC_DEFAULT_CONFIG \
  0 //  Enable this macro to set the default configurations in non_uc case, this is useful when someone don't want to use UC configuration
#define ULP_BANK_OFFSET 0x800                                         // ULP Memory bank offset value.
#define TX_BUF_MEMORY   (ULP_SRAM_START_ADDR + (1 * ULP_BANK_OFFSET)) // Transfer buffer start address at 2K
#define RX_BUF_MEMORY   (ULP_SRAM_START_ADDR + (2 * ULP_BANK_OFFSET)) // Rx buffer start address at 4k

/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
static uint8_t ulp_uart_data_in[ULP_UART_BUFFER_SIZE];
static uint8_t ulp_uart_data_out[ULP_UART_BUFFER_SIZE];

volatile boolean_t ulp_uart_send_complete = false, ulp_uart_transfer_complete = false,
                   ulp_uart_receive_complete = false;
static boolean_t ulp_uart_begin_transmission = true;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
void ulp_uart_callback_event(uint32_t event);
static void compare_loop_back_data(void);
/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/
sl_usart_handle_t usart_handle;
usart_mode_enum_t current_mode = SL_ULP_UART_SEND_DATA;
sl_gpio_t ulp_gpio_rx          = { ULP_GPIO_PORT, ULP_GPIO_PIN };
sl_gpio_t ulp_gpio_toggle      = { ULP_GPIO_PORT, ULP_GPIO_TOGGLE };

extern void hardware_setup(void);
/*******************************************************************************
 * USART Example Initialization function
 ******************************************************************************/
void usart_example_init(void)
{
  /* At this stage the micro-controller clock setting is already configured,
     * this is done through SystemInit() function which is called from startup
     * file (startup_rs1xxxx.s) before to branch to application main.
     * To reconfigure the default setting of SystemInit() function, refer to
     * startup_rs1xxxx.c file
     */
  // Switching MCU from PS4 to PS2 state(low power state)
  // In this mode, whatever be the timer clock source value, it will run with 20MHZ only
  // To use usart in high power mode, don't call hardware_setup()

  hardware_setup();
  sl_status_t status;
  sl_si91x_usart_control_config_t usart_config;
  sl_si91x_usart_control_config_t get_config;
#if NON_UC_DEFAULT_CONFIG
  usart_config.baudrate      = USART_BAUDRATE;
  usart_config.mode          = SL_USART_MODE_ASYNCHRONOUS;
  usart_config.parity        = SL_USART_NO_PARITY;
  usart_config.stopbits      = SL_USART_STOP_BITS_1;
  usart_config.hwflowcontrol = SL_USART_FLOW_CONTROL_NONE;
  usart_config.databits      = SL_USART_DATA_BITS_8;
  usart_config.misc_control  = SL_USART_MISC_CONTROL_NONE;
  usart_config.usart_module  = ULPUART;
  usart_config.config_enable = ENABLE;
  usart_config.synch_mode    = DISABLE;
#endif

  //Set ulp pin 10 in GPIO mode
  sl_gpio_driver_set_pin_mode(&ulp_gpio_toggle, EGPIO_PIN_MUX_MODE0, OUTPUT_VALUE);
  //Set output direction
  sl_si91x_gpio_driver_set_pin_direction(ULP_GPIO_PORT, ULP_GPIO_TOGGLE, (sl_si91x_gpio_direction_t)GPIO_OUTPUT);

  do {
    // Initialize the UART
    status = sl_si91x_usart_init(ULPUART, &usart_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_initialize: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("USART initialization is successful \n");
    // Configure the USART configurations
    status = sl_si91x_usart_set_configuration(usart_handle, &usart_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_set_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("USART configuration is successful \n");
#if USE_SEND
    // Enable ULP clk
    sl_si91x_gpio_driver_enable_clock((sl_si91x_gpio_select_clock_t)ULPCLK_GPIO);
    // Enable ULP pad receiver
    sl_si91x_gpio_driver_enable_ulp_pad_receiver(ULP_GPIO_PIN);
    // Pull up the ULP GPIO
    sl_si91x_gpio_driver_select_ulp_pad_driver_disable_state(ULP_GPIO_PIN,
                                                             (sl_si91x_gpio_driver_disable_state_t)GPIO_PULLUP);
    //configure RX pin
    sl_gpio_driver_set_pin_mode(&ulp_gpio_rx, EGPIO_PIN_MUX_MODE3, OUTPUT_VALUE);
#endif
    // Register user callback function
    status = sl_si91x_usart_multiple_instance_register_event_callback(ULPUART, ulp_uart_callback_event);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("USART user event callback registered successfully \n");
    sl_si91x_usart_get_configurations(ULPUART, &get_config);
    DEBUGOUT("Baud Rate = %ld \n", get_config.baudrate);
  } while (false);
}

/*******************************************************************************
 * Example ticking function
 ******************************************************************************/
void usart_example_process_action(void)
{
  sl_status_t status;
  // In this switch case, according to the macros enabled in header file, it starts to execute the APIs
  switch (current_mode) {
    case SL_ULP_UART_SEND_DATA:
      // Validation for executing the API only once
      if (ulp_uart_begin_transmission == true) {
        // Fill the data buffer to be send
        for (uint16_t i = 0; i < ULP_UART_BUFFER_SIZE; i++) {
          ulp_uart_data_out[i] = (uint8_t)(i + 1);
        }

        memcpy((uint8_t *)TX_BUF_MEMORY, &ulp_uart_data_out, sizeof(ulp_uart_data_out[0]) * ULP_UART_BUFFER_SIZE);

        status = sl_si91x_usart_send_data(usart_handle, (uint8_t *)TX_BUF_MEMORY, sizeof(ulp_uart_data_out));

        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          DEBUGOUT("sl_si91x_usart_send_data: Error Code : %lu \n", status);
          current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
          break;
        }
        ulp_uart_begin_transmission = false;
      }

      ulp_uart_send_complete = false;

      if (USE_RECEIVE) {
        // If receive macro is enabled, current mode is set to receive
        current_mode                = SL_ULP_UART_RECEIVE_DATA;
        ulp_uart_begin_transmission = true;
        break;
      }
      DEBUGOUT("USART send completed successfully \n");
      // Current mode is set to complete
      current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
      break;

    case SL_ULP_UART_RECEIVE_DATA:
      if (ulp_uart_begin_transmission == true) {
        // Validation for executing the API only once
        status = sl_si91x_usart_receive_data(usart_handle, (uint8_t *)RX_BUF_MEMORY, sizeof(ulp_uart_data_in));
        if (status != SL_STATUS_OK) {
          // If it fails to execute the API, it will not execute rest of the things
          DEBUGOUT("sl_si91x_usart_receive_data: Error Code : %lu \n", status);
          current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
          break;
        }
        DEBUGOUT("USART receive begin successfully \n");
        ulp_uart_begin_transmission = false;
      }

      if (ulp_uart_receive_complete) {

        //Waiting till the receive is completed
        memcpy(&ulp_uart_data_in, (uint8_t *)RX_BUF_MEMORY, sizeof(ulp_uart_data_in[0]) * ULP_UART_BUFFER_SIZE);
        // Update the receive compelete flag with 0.
        ulp_uart_receive_complete = false;
        if (USE_SEND) {
          // If send macro is enabled, current mode is set to send
          current_mode                = SL_ULP_UART_SEND_DATA;
          ulp_uart_begin_transmission = true;
          compare_loop_back_data();
          DEBUGOUT("USART receive completed \n");
          break;
        }

        DEBUGOUT("USART send completed successfully \n");
        DEBUGOUT("USART receive completed \n");
        compare_loop_back_data();
        // If send macro is not enabled, current mode is set to completed.
        current_mode = SL_ULP_UART_TRANSMISSION_COMPLETED;
      }
      break;

    case SL_ULP_UART_TRANSMISSION_COMPLETED:
      break;
    case SL_ULP_UART_TRANSFER_DATA: // This case is not required in ULP_UART mode
      break;
  }
}

/*******************************************************************************
 * Compares data received buffer via USART with data transfer buffer and print
 * the loopback test passed or failed
 ******************************************************************************/
static void compare_loop_back_data(void)
{
  uint16_t data_index = 0;
  // Check for data in and data out are same, if same then comparision
  // will continue till end of the buffer
  for (data_index = 0; data_index < ULP_UART_BUFFER_SIZE; data_index++) {
    if (ulp_uart_data_in[data_index] != ulp_uart_data_out[data_index]) {
      break;
    }
  }
  if (data_index == ULP_UART_BUFFER_SIZE) {
    // Set the pin
    sl_gpio_driver_set_pin(&ulp_gpio_toggle);
    // Clear the pin
    sl_gpio_driver_clear_pin(&ulp_gpio_toggle);
    DEBUGOUT("Data comparison successful, Loop Back Test Passed \n");
  } else {
    DEBUGOUT("Data comparison failed, Loop Back Test failed \n");
  }
}

/*******************************************************************************
 * Callback function triggered on data Transfer and reception
 ******************************************************************************/
void ulp_uart_callback_event(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      ulp_uart_send_complete = true;
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      ulp_uart_receive_complete = true;
      break;
  }
}
