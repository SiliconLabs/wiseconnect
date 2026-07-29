/***************************************************************************/ /**
 * @file uart_rs485_mode_freertos.h
 * @brief UART RS485 FreeRTOS example functions
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

#ifndef UART_RS485_MODE_FREERTOS_H_
#define UART_RS485_MODE_FREERTOS_H_

/*******************************************************************************
 *************************** Macros   *******************************
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
//Enum for different transmission scenarios
typedef enum {
  SL_UART_RS485_SEND,
  SL_UART_RS485_RECEIVE,
  SL_UART_RS485_FULL_DUPLEX_SEND_RECEIVE,
  SL_UART_RS485_FULL_DUPLEX_RECEIVE_SEND,
  SL_UART_RS485_COMPLETED,
} uart_rs485_mode_enum_t;

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * UART RS485 example initialization function. Creates a FreeRTOS task that
 * handles RS485 configuration and send/receive operations.
 *
 * @param none
 * @return none
 ******************************************************************************/
void uart_rs485_mode_example_init(void);

#endif /* UART_RS485_MODE_FREERTOS_H_ */
