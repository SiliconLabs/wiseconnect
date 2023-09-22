/*******************************************************************************
* @file  sl_uart.h
* @brief 
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

#pragma once

#include "sl_status.h"
#include <stdint.h>

typedef void sl_usart_peripheral_t;
typedef void sl_uart_configuration_t;
typedef void sl_uart_pin_configuration_t;

sl_status_t sl_uart_init(sl_usart_peripheral_t *uart, uint32_t baud_rate, const sl_uart_pin_configuration_t *config);
sl_status_t sl_uart_print(sl_usart_peripheral_t *uart, const char *string);
sl_status_t sl_uart_tx_byte(sl_usart_peripheral_t *uart, uint8_t byte);
sl_status_t sl_uart_rx_byte(sl_usart_peripheral_t *uart, uint8_t *byte);
