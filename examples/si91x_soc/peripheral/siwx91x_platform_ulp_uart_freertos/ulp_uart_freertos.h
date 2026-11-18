/***************************************************************************/ /**
 * @file ulp_uart_freertos.h
 * @brief ULP USART loopback FreeRTOS example functions
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
#ifndef ULP_UART_FREERTOS_H_
#define ULP_UART_FREERTOS_H_

// IF send and receive should happen continuously then both macros should be enabled
#define SLI_ULP_UART_USE_SEND    DISABLE ///< Used to enable USART send
#define SLI_ULP_UART_USE_RECEIVE ENABLE  ///< Used to enable USART receive

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Creates the ULP USART loopback FreeRTOS task (`ulp_uart_freertos.c`).
 *
 * @details Default thread attributes: stack 10240 bytes, `osPriorityLow1`.
 ******************************************************************************/
void ulp_uart_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ULP_UART_FREERTOS_H_ */
