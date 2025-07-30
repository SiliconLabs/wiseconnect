/***************************************************************************/ /**
 * @file app_iostream_usart.h
 * @brief iostream usart example
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

#ifndef APP_IOSTREAM_USART_H
#define APP_IOSTREAM_USART_H

/***************************************************************************/ /**
 * Initialize iostream usart
 ******************************************************************************/
void app_iostream_usart_init(void);

/***************************************************************************/ /**
 * iostream usart ticking function
 ******************************************************************************/
void app_iostream_usart_process_action(void);

#endif // APP_IOSTREAM_USART_H
