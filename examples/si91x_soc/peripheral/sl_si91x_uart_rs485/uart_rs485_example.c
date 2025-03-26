/***************************************************************************/
/**
 * @file uart_rs485_example.c
 * @brief UART_RS485 example
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "uart_rs485_example.h"
#include "sl_si91x_clock_manager.h"
#ifdef UART1_RS485_MODE
#define UART_INSTANCE UART_1 // Select UART_1 instance
#endif
#ifdef UART0_RS485_MODE // Select UART_0 instance
#define UART_INSTANCE USART_0
#endif
/*******************************************************************************
    ***************************  Defines / Macros  ********************************
    ******************************************************************************/
#define UART_RS485_BUFFER_SIZE 1024   // Data send and receive length
#define UART_RS485_BAUDRATE    115200 // Baud rate set to 115200
#define NON_UC_DEFAULT_CONFIG  0      // Enable this macro to set the default configurations in non_uc case
#define RS485_SLAVE_ADDRESS    0x10B  // RS485 slave address
#define BIT_POS_8              8      //Bit position 8
/*******************************************************************************
    *************************** LOCAL VARIABLES   *******************************
    ******************************************************************************/
static uint16_t uart_rs485_data_in[UART_RS485_BUFFER_SIZE];
static uint16_t uart_rs485_data_out[UART_RS485_BUFFER_SIZE];
static usart_rs485_config_t rs485_configs;
volatile boolean_t uart_rs485_send_complete = false, uart_rs485_transfer_complete = false,
                   uart_rs485_receive_complete = false;
static boolean_t uart_rs485_begin_transmission = true;
/*******************************************************************************
    **********************  Local Function prototypes   ***************************
    ******************************************************************************/
void uart_rs485_callback_event(uint32_t event);
static void uart_rs485_compare_data(void);

/*******************************************************************************
    **************************   GLOBAL VARIABLES   ******************************
    ******************************************************************************/
sl_usart_handle_t uart_rs485_handle;
uart_rs485_mode_enum_t current_mode = SL_UART_RS485_SEND;

/*******************************************************************************
   **************************   GLOBAL FUNCTIONS   ******************************
   ******************************************************************************/

/*******************************************************************************
    * RS485 Example Initialization function
    *****************************************************************************/
void uart_rs485_example_init(void)
{
  sl_status_t status;
  sl_si91x_usart_control_config_t uart_rs485_config;

#if NON_UC_DEFAULT_CONFIG
  uart_rs485_config.baudrate      = UART_RS485_BAUDRATE;
  uart_rs485_config.mode          = SL_USART_MODE_ASYNCHRONOUS;
  uart_rs485_config.parity        = SL_USART_NO_PARITY;
  uart_rs485_config.stopbits      = SL_USART_STOP_BITS_1;
  uart_rs485_config.hwflowcontrol = SL_USART_FLOW_CONTROL_NONE;
  uart_rs485_config.databits      = SL_USART_DATA_BITS_8;
  uart_rs485_config.misc_control  = SL_USART_MISC_CONTROL_NONE;
  uart_rs485_config.usart_module  = UART_INSTANCE;
  uart_rs485_config.config_enable = ENABLE;
  uart_rs485_config.synch_mode    = DISABLE;
#endif

  sl_si91x_usart_control_config_t get_config;
  for (uint16_t i = 0; i < UART_RS485_BUFFER_SIZE; i++) {
    uart_rs485_data_out[i] = (uint8_t)(i + 1);
  }
  do {
    // Initialize the UART
    status = sl_si91x_usart_init(UART_INSTANCE, &uart_rs485_handle);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_initialize: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("UART initialization is successful \n");

    // Configure the UART configurations
    status = sl_si91x_usart_set_configuration(uart_rs485_handle, &uart_rs485_config);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_set_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("UART configuration is successful \n");
    // Initialize RS485 pins and Enable RS485 module
    status = sl_si91x_uart_rs485_init(UART_INSTANCE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_uart_rs485_init: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("RS485 initialization successful\n");
    // Get RS485 configurations
    sl_si91x_get_uart_rs485_configure(&rs485_configs);
    // Check whether current mode is set to SL_UART_RS485_RECEIVE and RS485 transfer mode is software controlled half-duplex mode
    if ((current_mode == SL_UART_RS485_RECEIVE) && (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE)) {
      // Enable Driver enable
      status = sl_si91x_uart_rs485_de_enable(UART_INSTANCE, DISABLE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_uart_rs485_de_enable: Error Code : %lu \n", status);
        break;
      }
    } else {
      // Enable Driver enable
      status = sl_si91x_uart_rs485_de_enable(UART_INSTANCE, ENABLE);
      if (status != SL_STATUS_OK) {
        DEBUGOUT("sl_si91x_uart_rs485_de_enable: Error Code : %lu \n", status);
        break;
      }
    }
    DEBUGOUT("RS485 DE enable successful\n");
    // Enable Receiver Enable
    status = sl_si91x_uart_rs485_re_enable(UART_INSTANCE, ENABLE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_uart_rs485_re_enable: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("RS485 RE enable successful\n");

    // Configure RS485 configurations
    status = sl_si91x_uart_rs485_set_configuration(UART_INSTANCE);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_uart_rs485_set_configuration: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("RS485 configuration is successful \n");
    // Register user callback function
    status = sl_si91x_usart_multiple_instance_register_event_callback(UART_INSTANCE, uart_rs485_callback_event);
    if (status != SL_STATUS_OK) {
      DEBUGOUT("sl_si91x_usart_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    DEBUGOUT("UART user event callback registered successfully \n");

    sl_si91x_usart_get_configurations(UART_INSTANCE, &get_config);
    DEBUGOUT("Baud Rate = %ld \n", get_config.baudrate);
  } while (false);
}

/*******************************************************************************
    * Example ticking function
    ******************************************************************************/
void uart_rs485_example_process_action(void)
{
  sl_status_t status;

  switch (current_mode) {
    case SL_UART_RS485_SEND:
      if (uart_rs485_begin_transmission == true) {
        // If transfer mode is set to software controlled half-duplex mode, execute if condition
        if (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE) {
          uint16_t uart_rs485_data = RS485_SLAVE_ADDRESS;
          status = sl_si91x_usart_send_data(uart_rs485_handle, &uart_rs485_data, sizeof(uart_rs485_data));
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_send_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          status = sl_si91x_usart_send_data(uart_rs485_handle, uart_rs485_data_out, sizeof(uart_rs485_data_out));
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_send_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
        } else {
          uint8_t uart_rs485_slave_addr;
          sl_si91x_uart_rs485_transfer_hardware_address(UART_INSTANCE, &uart_rs485_slave_addr);
          DEBUGOUT("RS485 Device looking for Slave Address = 0x%X  \n", uart_rs485_slave_addr);
          status = sl_si91x_usart_send_data(uart_rs485_handle, uart_rs485_data_out, sizeof(uart_rs485_data_out));
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_send_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
        }

        uart_rs485_begin_transmission = false;
      }

      if (uart_rs485_send_complete) {
        uart_rs485_send_complete      = false;
        current_mode                  = SL_UART_RS485_COMPLETED;
        uart_rs485_begin_transmission = true;
        DEBUGOUT("RS485 Data transfer completed \n");
      }
      break;

    case SL_UART_RS485_RECEIVE:
      if (uart_rs485_begin_transmission == true) {
        if (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE) {
          uint16_t uart_rs485_rx_addr;
          uart_rs485_receive_complete = false;
          status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          while (!uart_rs485_receive_complete)
            ;
          if (uart_rs485_rx_addr & (1 << BIT_POS_8)) {
            status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
            if (status != SL_STATUS_OK) {
              DEBUGOUT("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
          }
          uart_rs485_receive_complete = false;
          status = sl_si91x_usart_receive_data(uart_rs485_handle, uart_rs485_data_in, sizeof(uart_rs485_data_in) / 2);
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          while (!uart_rs485_receive_complete)
            ;
        } else {
          uint8_t uart_rs485_rx_addr;
          status = sl_si91x_uart_rs485_rx_hardware_address_set(UART_INSTANCE, &uart_rs485_rx_addr);
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_rs485_rx_address_set: Error Code : %lu \n", status);
            break;
          }

          status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }

          status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }

          uart_rs485_receive_complete = false;
          status = sl_si91x_usart_receive_data(uart_rs485_handle, uart_rs485_data_in, sizeof(uart_rs485_data_in) / 2);
          if (status != SL_STATUS_OK) {
            DEBUGOUT("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
        }
        uart_rs485_begin_transmission = false;
      }
      if (uart_rs485_receive_complete) {
        uart_rs485_receive_complete = false;
        uart_rs485_compare_data();
        current_mode = SL_UART_RS485_COMPLETED;
      }

      break;

    case SL_UART_RS485_COMPLETED:
      break;
  }
}

/*******************************************************************************
    * Compare data received buffer via UART with data transfer buffer
    ******************************************************************************/
static void uart_rs485_compare_data(void)
{
  uint16_t data_index;
  for (data_index = 0; data_index < UART_RS485_BUFFER_SIZE; data_index++) {
    if (uart_rs485_data_in[data_index] != uart_rs485_data_out[data_index]) {
      break;
    }
  }

  if (data_index == UART_RS485_BUFFER_SIZE) {
    DEBUGOUT("Received Data comparison successful \n");
  } else {
    DEBUGOUT("Received Data comparison failed \n");
  }
}

/*******************************************************************************
    * Callback function triggered on data Transfer and reception
    ******************************************************************************/
void uart_rs485_callback_event(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      uart_rs485_send_complete = true;
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      uart_rs485_receive_complete = true;
      break;
    case SL_USART_EVENT_TRANSFER_COMPLETE:
      uart_rs485_transfer_complete = true;
      break;
  }
}
