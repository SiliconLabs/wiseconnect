/***************************************************************************/ /**
 * @file uart_rs485_mode_freertos.c
 * @brief UART RS485 FreeRTOS example: multi-drop send/receive in a task,
 *        blocking on semaphores for USART send/receive completion events.
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
#include "uart_rs485_mode_freertos.h"
#include "sl_si91x_clock_manager.h"
#include "cmsis_os2.h"

#ifdef UART1_RS485_MODE
#define UART_INSTANCE UART_1
#elif defined(UART0_RS485_MODE)
#define UART_INSTANCE USART_0
#else
#define UART_INSTANCE UART_1
#endif

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define UART_RS485_BUFFER_SIZE  1024   // Data send and receive length
#define UART_RS485_BAUDRATE     115200 // Baud rate set to 115200
#define RS485_SW_SLAVE1_ADDRESS 0x10B  // RS485 software controlled slave address
#define RS485_SW_SLAVE2_ADDRESS 0x178  // RS485 software controlled slave address
#define BIT_POS_8               8      // Bit position 8

#define RS485_SLAVE1_ADDRESS       120 // RS485 hardware controlled slave1 address
#define RS485_SLAVE2_ADDRESS       11  // RS485 hardware controlled slave2 address
#define RS485_SLAVE1               1   // RS485 slave1
#define RS485_SLAVE2               2   // RS485 slave2
#define TRANSMISSION_COUNT_TRIGGER 3   // Defines the number of send/receive cycles
#define DELAY_MS                   30  // 30ms delay added between transitions

/*******************************************************************************
 *************************** LOCAL VARIABLES   *********************************
 ******************************************************************************/
static uint16_t uart_rs485_data_in[UART_RS485_BUFFER_SIZE];
static uint16_t uart_rs485_data_out[UART_RS485_BUFFER_SIZE];

static usart_rs485_config_t rs485_configs;
volatile boolean_t uart_rs485_send_complete = false, uart_rs485_transfer_complete = false,
                   uart_rs485_receive_complete = false;
static boolean_t uart_rs485_begin_transmission = true;
static uint8_t send_flag                       = 0;
static uint8_t receive_flag                    = 0;
static uint8_t current_slave                   = RS485_SLAVE1;

static osSemaphoreId_t rs485_send_complete_sem    = NULL;
static osSemaphoreId_t rs485_receive_complete_sem = NULL;

/*******************************************************************************
 **********************  Local Function prototypes   ***************************
 ******************************************************************************/
static void uart_rs485_mode_task(void *argument);
static sl_status_t uart_rs485_init_function(void);
static void uart_rs485_compare_data(void);
static void uart_rs485_event_callback_handler(uint32_t event);

static const osThreadAttr_t uart_rs485_mode_task_attributes = {
  .name       = "uart_rs485",
  .stack_size = 2048,
  .priority   = osPriorityLow1,
};

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/
sl_usart_handle_t uart_rs485_handle;
uart_rs485_mode_enum_t current_mode = SL_UART_RS485_SEND;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
/*******************************************************************************
 * @brief  Entry point: creates the UART RS485 FreeRTOS task.
 * @return None
 ******************************************************************************/
void uart_rs485_mode_example_init(void)
{
  osThreadId_t thread_id = osThreadNew((osThreadFunc_t)uart_rs485_mode_task, NULL, &uart_rs485_mode_task_attributes);
  if (thread_id == NULL) {
    /* Note: All status messages in this example — both success and failure — are
 * intentionally emitted via SL_PRINT_STRING_ERROR so that they remain visible
 * on the console at the default log level. This is a demonstration choice, not
 * a recommendation: in production code, ERROR severity should be reserved for
 * actual failures, with successful operations logged via SL_PRINT_STRING_INFO
 * (or SL_PRINT_STRING_DEBUG for verbose trace). */
    SL_PRINT_STRING_ERROR("Failed to create uart_rs485 thread\r\n");
    return;
  }
}

/*******************************************************************************
 * @brief  One-time RS485 hardware init: UART init, RS485 config, DE/RE enable,
 *         callback registration, and semaphore creation.
 * @return SL_STATUS_OK on success, error code on failure
 ******************************************************************************/
static sl_status_t uart_rs485_init_function(void)
{
  sl_status_t status = SL_STATUS_OK;
  sl_si91x_usart_control_config_t uart_rs485_config;

  /* Initialize before set_configuration: when UART_UC/USART_UC is enabled the driver
   * still applies UC-backed settings via the out-parameter; this avoids an uninitialized
   * stack struct when those paths are off or for static analysis. */
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

  for (uint16_t i = 0; i < UART_RS485_BUFFER_SIZE; i++) {
    uart_rs485_data_out[i] = (uint8_t)(i + 1);
  }

  do {
    status = sl_si91x_usart_init(UART_INSTANCE, &uart_rs485_handle);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_initialize: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("UART initialization is successful \n");

    status = sl_si91x_usart_set_configuration(uart_rs485_handle, &uart_rs485_config);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_set_configuration: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("UART configuration is successful \n");

    status = sl_si91x_uart_rs485_init(UART_INSTANCE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_init: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("RS485 initialization successful\n");

    sl_si91x_get_uart_rs485_configure(&rs485_configs);

    if ((current_mode == SL_UART_RS485_RECEIVE) && (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE)) {
      status = sl_si91x_uart_rs485_de_enable(UART_INSTANCE, DISABLE);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_de_enable: Error Code : %lu \n", status);
        break;
      }
    } else {
      status = sl_si91x_uart_rs485_de_enable(UART_INSTANCE, ENABLE);
      if (status != SL_STATUS_OK) {
        SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_de_enable: Error Code : %lu \n", status);
        break;
      }
    }
    SL_PRINT_STRING_ERROR("RS485 DE enable successful\n");

    status = sl_si91x_uart_rs485_re_enable(UART_INSTANCE, ENABLE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_re_enable: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("RS485 RE enable successful\n");

    status = sl_si91x_uart_rs485_set_configuration(UART_INSTANCE);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_set_configuration: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("RS485 configuration is successful \n");

    status = sl_si91x_usart_multiple_instance_register_event_callback(UART_INSTANCE, uart_rs485_event_callback_handler);
    if (status != SL_STATUS_OK) {
      SL_PRINT_STRING_ERROR("sl_si91x_usart_multiple_instance_register_event_callback: Error Code : %lu \n", status);
      break;
    }
    SL_PRINT_STRING_ERROR("UART user event callback registered successfully \n");

  } while (false);

  if (status != SL_STATUS_OK) {
    return status;
  }

  rs485_send_complete_sem = osSemaphoreNew(1U, 0U, NULL);
  if (rs485_send_complete_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create RS485 send semaphore\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }
  rs485_receive_complete_sem = osSemaphoreNew(1U, 0U, NULL);
  if (rs485_receive_complete_sem == NULL) {
    SL_PRINT_STRING_ERROR("Failed to create RS485 receive semaphore\r\n");
    return SL_STATUS_ALLOCATION_FAILED;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************
 * @brief  UART RS485 FreeRTOS task. Initialises RS485 hardware, then loops
 *         forever running the send/receive state machine.
 * @param  argument  Unused (NULL)
 * @return None
 ******************************************************************************/
static void uart_rs485_mode_task(void *argument)
{
  (void)argument;

  sl_status_t init_status = uart_rs485_init_function();
  if (init_status != SL_STATUS_OK) {
    SL_PRINT_STRING_ERROR("UART RS485 init failed: Error Code : %lu, exiting task\r\n", init_status);
    osThreadExit();
  }

  while (1) {
    sl_status_t status;

    switch (current_mode) {
      case SL_UART_RS485_SEND:
        if (uart_rs485_begin_transmission == true) {
          if (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE) {
            uint16_t uart_rs485_data = 0;
            if (current_slave == RS485_SLAVE1) {
              uart_rs485_data = RS485_SW_SLAVE1_ADDRESS;
              SL_PRINT_STRING_ERROR("RS485 Device looking for Slave Address = 0x%X  \n", uart_rs485_data);
            } else {
              uart_rs485_data = RS485_SW_SLAVE2_ADDRESS;
              SL_PRINT_STRING_ERROR("RS485 Device looking for Slave Address = 0x%X  \n", uart_rs485_data);
            }
            status = sl_si91x_usart_send_data(uart_rs485_handle, &uart_rs485_data, sizeof(uart_rs485_data));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
            if (rs485_send_complete_sem != NULL) {
              (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
            }
            uart_rs485_send_complete = false;
            osDelay(DELAY_MS);

            status = sl_si91x_usart_send_data(uart_rs485_handle,
                                              uart_rs485_data_out,
                                              (sizeof(uart_rs485_data_out) / sizeof(uart_rs485_data_out[0])));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
            if (rs485_send_complete_sem != NULL) {
              (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
            }
          } else {
            uint8_t uart_rs485_slave_addr = 0;
            if (current_slave == RS485_SLAVE1) {
              uart_rs485_slave_addr = RS485_SLAVE1_ADDRESS;
            } else {
              uart_rs485_slave_addr = RS485_SLAVE2_ADDRESS;
            }
            sl_si91x_uart_rs485_transfer_hardware_address(UART_INSTANCE, &uart_rs485_slave_addr);
            SL_PRINT_STRING_ERROR("RS485 Device looking for Slave Address = 0x%X  \n", uart_rs485_slave_addr);
            status = sl_si91x_usart_send_data(uart_rs485_handle,
                                              uart_rs485_data_out,
                                              (sizeof(uart_rs485_data_out) / sizeof(uart_rs485_data_out[0])));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
          }

          uart_rs485_begin_transmission = false;
        }

        if (uart_rs485_send_complete) {
          uart_rs485_send_complete = false;
          if (rs485_configs.transfer_mode == SL_UART_HW_CTRL_HALF_DUPLEX_MODE) {
            current_mode                  = SL_UART_RS485_RECEIVE;
            uart_rs485_begin_transmission = true;
            SL_PRINT_STRING_ERROR("RS485 Data send completed \n");
          } else {
            if (current_slave == RS485_SLAVE1) {
              current_mode  = SL_UART_RS485_SEND;
              current_slave = RS485_SLAVE2;
            } else {
              current_mode                  = SL_UART_RS485_COMPLETED;
              uart_rs485_begin_transmission = false;
              SL_PRINT_STRING_ERROR("RS485 Data send completed \n");
            }
          }
          if (current_slave == RS485_SLAVE2) {
            if (rs485_configs.transfer_mode == SL_UART_HW_CTRL_HALF_DUPLEX_MODE) {
              current_mode                  = SL_UART_RS485_COMPLETED;
              uart_rs485_begin_transmission = false;
            } else {
              uart_rs485_begin_transmission = true;
            }
          }
          if (send_flag == TRANSMISSION_COUNT_TRIGGER) {
            current_mode = SL_UART_RS485_COMPLETED;
            if (current_slave == RS485_SLAVE1) {
              current_slave = RS485_SLAVE2;
              current_mode  = SL_UART_RS485_SEND;
            }
          }
          osDelay(DELAY_MS);
        }
        break;

      case SL_UART_RS485_RECEIVE:
        if (uart_rs485_begin_transmission == true) {
          if (rs485_configs.transfer_mode == SL_UART_SW_CTRL_HALF_DUPLEX_MODE) {
            uint16_t uart_rs485_rx_addr = 0;
            uart_rs485_receive_complete = false;
            status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
            if (rs485_receive_complete_sem != NULL) {
              (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
            }
            uint16_t target_hw_address = (current_slave == 1) ? RS485_SLAVE2_ADDRESS : RS485_SLAVE1_ADDRESS;
            if ((uart_rs485_rx_addr & (1 << BIT_POS_8)) && ((uart_rs485_rx_addr & 0xFF) == target_hw_address)) {
              status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
              if (status != SL_STATUS_OK) {
                SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
                current_mode = SL_UART_RS485_COMPLETED;
                break;
              }

              uart_rs485_receive_complete = false;
              status                      = sl_si91x_usart_receive_data(uart_rs485_handle,
                                                   uart_rs485_data_in,
                                                   (sizeof(uart_rs485_data_in) / sizeof(uart_rs485_data_in[0])));
              if (status != SL_STATUS_OK) {
                SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
                current_mode = SL_UART_RS485_COMPLETED;
                break;
              }
              if (rs485_receive_complete_sem != NULL) {
                (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
              }
              uart_rs485_begin_transmission = false;
            } else {
              uart_rs485_begin_transmission = true;
              uart_rs485_receive_complete   = false;
            }
          } else {
            uint8_t uart_rs485_rx_addr = 0;
            if (current_slave == RS485_SLAVE1) {
              uart_rs485_rx_addr = RS485_SLAVE1_ADDRESS;
            } else {
              uart_rs485_rx_addr = RS485_SLAVE2_ADDRESS;
            }
            status = sl_si91x_uart_rs485_rx_hardware_address_set(UART_INSTANCE, &uart_rs485_rx_addr);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_rx_address_set: Error Code : %lu \n", status);
              break;
            }

            status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }

            status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }

            uart_rs485_receive_complete = false;
            status                      = sl_si91x_usart_receive_data(uart_rs485_handle,
                                                 uart_rs485_data_in,
                                                 (sizeof(uart_rs485_data_in) / sizeof(uart_rs485_data_in[0])));
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
            uart_rs485_begin_transmission = false;
          }
        }
        if (uart_rs485_receive_complete) {
          uart_rs485_receive_complete = false;
          uart_rs485_compare_data();
          if (rs485_configs.transfer_mode == SL_UART_HW_CTRL_HALF_DUPLEX_MODE) {
            current_mode                  = SL_UART_RS485_SEND;
            uart_rs485_begin_transmission = true;
          } else {
            current_mode                  = SL_UART_RS485_COMPLETED;
            uart_rs485_begin_transmission = false;
          }
          if ((current_slave == RS485_SLAVE2) || (receive_flag == TRANSMISSION_COUNT_TRIGGER)) {
            current_mode                  = SL_UART_RS485_COMPLETED;
            uart_rs485_begin_transmission = false;
          }
          osDelay(DELAY_MS);
        }
        break;

      case SL_UART_RS485_FULL_DUPLEX_SEND_RECEIVE:
        if (uart_rs485_begin_transmission == true) {
          uint16_t uart_rs485_rx_addr = 0;
          uint16_t uart_rs485_data    = RS485_SW_SLAVE1_ADDRESS;
          SL_PRINT_STRING_ERROR("RS485 Device looking for Address = 0x%X  \n", uart_rs485_data);
          status = sl_si91x_usart_send_data(uart_rs485_handle, &uart_rs485_data, sizeof(uart_rs485_data));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data (address): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_send_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
          }
          uart_rs485_send_complete = false;
          osDelay(DELAY_MS);

          status = sl_si91x_usart_send_data(uart_rs485_handle,
                                            uart_rs485_data_out,
                                            (sizeof(uart_rs485_data_out) / sizeof(uart_rs485_data_out[0])));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data (data): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_send_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
          }
          uart_rs485_send_complete = false;
          SL_PRINT_STRING_ERROR("RS485 Data send completed \n");

          status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data (address): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_receive_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
          }
          uart_rs485_receive_complete = false;

          if (uart_rs485_rx_addr & (1 << BIT_POS_8)) {
            status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
          }

          uart_rs485_receive_complete = false;
          status                      = sl_si91x_usart_receive_data(uart_rs485_handle,
                                               uart_rs485_data_in,
                                               (sizeof(uart_rs485_data_in) / sizeof(uart_rs485_data_in[0])));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data (data): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_receive_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
          }
          uart_rs485_receive_complete = false;

          uart_rs485_compare_data();
          uart_rs485_begin_transmission = false;
          current_mode                  = SL_UART_RS485_COMPLETED;
        }
        break;

      case SL_UART_RS485_FULL_DUPLEX_RECEIVE_SEND:
        if (uart_rs485_begin_transmission == true) {
          uint16_t uart_rs485_rx_addr = 0;
          status = sl_si91x_usart_receive_data(uart_rs485_handle, &uart_rs485_rx_addr, sizeof(uart_rs485_rx_addr));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data (address): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_receive_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
          }
          uart_rs485_receive_complete = false;

          if (uart_rs485_rx_addr & (1 << BIT_POS_8)) {
            status = sl_si91x_uart_rs485_address_received(UART_INSTANCE);
            if (status != SL_STATUS_OK) {
              SL_PRINT_STRING_ERROR("sl_si91x_uart_rs485_address_received: Error Code : %lu \n", status);
              current_mode = SL_UART_RS485_COMPLETED;
              break;
            }
          }

          uart_rs485_receive_complete = false;
          status                      = sl_si91x_usart_receive_data(uart_rs485_handle,
                                               uart_rs485_data_in,
                                               (sizeof(uart_rs485_data_in) / sizeof(uart_rs485_data_in[0])));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_receive_data (data): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_receive_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_receive_complete_sem, osWaitForever);
          }
          uart_rs485_receive_complete = false;

          uart_rs485_compare_data();

          uint16_t uart_rs485_data = RS485_SW_SLAVE1_ADDRESS;
          SL_PRINT_STRING_ERROR("RS485 Device looking for Address = 0x%X  \n", uart_rs485_data);
          status = sl_si91x_usart_send_data(uart_rs485_handle, &uart_rs485_data, sizeof(uart_rs485_data));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data (address): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_send_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
          }
          uart_rs485_send_complete = false;
          osDelay(DELAY_MS);

          status = sl_si91x_usart_send_data(uart_rs485_handle,
                                            uart_rs485_data_out,
                                            (sizeof(uart_rs485_data_out) / sizeof(uart_rs485_data_out[0])));
          if (status != SL_STATUS_OK) {
            SL_PRINT_STRING_ERROR("sl_si91x_uart_send_data (data): Error Code : %lu \n", status);
            current_mode = SL_UART_RS485_COMPLETED;
            break;
          }
          if (rs485_send_complete_sem != NULL) {
            (void)osSemaphoreAcquire(rs485_send_complete_sem, osWaitForever);
          }
          uart_rs485_send_complete = false;
          SL_PRINT_STRING_ERROR("RS485 Data send completed \n");

          uart_rs485_begin_transmission = false;
          current_mode                  = SL_UART_RS485_COMPLETED;
        }
        break;

      case SL_UART_RS485_COMPLETED:
        osThreadExit();
        break;
    }
  }
}

/*******************************************************************************
 * @brief  Compare data received buffer via UART with data transfer buffer.
 * @return None
 ******************************************************************************/
static void uart_rs485_compare_data(void)
{
  uint16_t data_index = 0;
  for (data_index = 0; data_index < UART_RS485_BUFFER_SIZE; data_index++) {
    if (uart_rs485_data_in[data_index] != uart_rs485_data_out[data_index]) {
      break;
    }
  }

  if (data_index == UART_RS485_BUFFER_SIZE) {
    SL_PRINT_STRING_ERROR("Received Data comparison successful \n");
  } else {
    SL_PRINT_STRING_ERROR("Received Data comparison failed \n");
  }
}

/*******************************************************************************
 * @brief    USART callback: releases appropriate semaphore on send/receive done.
 * @param[in] event  USART interrupt event
 * @return   None
 ******************************************************************************/
static void uart_rs485_event_callback_handler(uint32_t event)
{
  switch (event) {
    case SL_USART_EVENT_SEND_COMPLETE:
      uart_rs485_send_complete = true;
      if (send_flag == TRANSMISSION_COUNT_TRIGGER) {
        send_flag = 0;
      }
      send_flag++;
      if (rs485_send_complete_sem != NULL) {
        (void)osSemaphoreRelease(rs485_send_complete_sem);
      }
      break;
    case SL_USART_EVENT_RECEIVE_COMPLETE:
      uart_rs485_receive_complete = true;
      if (receive_flag == TRANSMISSION_COUNT_TRIGGER) {
        receive_flag = 0;
      }
      receive_flag++;
      if (rs485_receive_complete_sem != NULL) {
        (void)osSemaphoreRelease(rs485_receive_complete_sem);
      }
      break;
    case SL_USART_EVENT_TRANSFER_COMPLETE:
      uart_rs485_transfer_complete = true;
      break;
  }
}
