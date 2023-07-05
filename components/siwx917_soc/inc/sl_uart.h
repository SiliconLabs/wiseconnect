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
