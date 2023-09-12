/***************************************************************************/ /**
 * @file sio_example.h
 * @brief sio example
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

#ifndef SIO_EXAMPLE_H_
#define SIO_EXAMPLE_H_

#define SL_USE_RECEIVE ENABLE

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/
///< Enum for different transmission scenarios
typedef enum {
  SL_SEND_DATA,              ///< SIO UART send data
  SL_TRANSFER_DATA,          ///< SIO UART transfer data
  SL_RECEIVE_DATA,           ///< SIO UART received data
  SL_TRANSMISSION_COMPLETED, ///< SIO UART transfer completed
} sio_uart_mode_enum_t;
// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * SIO example initialization function ,this function initializes the SIO pins
 * and clock
 * @param none
 * @return none
 ******************************************************************************/
void sio_example_init(void);

/***************************************************************************/ /**
 * Function will run continuously in while loop. In this SIO send and receive
 * data is implemented
 * @param none
 * @return none
 ******************************************************************************/
void sio_example_process_action(void);

#endif /* SIO_EXAMPLE_H_ */
