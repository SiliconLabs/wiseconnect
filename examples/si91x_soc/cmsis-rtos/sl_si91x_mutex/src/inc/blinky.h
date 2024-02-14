/***************************************************************************/ /**
 * @file
 * @brief Simple button baremetal examples functions
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

#ifndef BLINKY_H
#define BLINKY_H

/***************************************************************************/ /**
 * Initialize example
 ******************************************************************************/
void blinky_init(void);

/***************************************************************************/ /**
 * Blink ticking function
 ******************************************************************************/
void led_on_process_action(void);
void led_off_process_action(void);

#endif // BLINK_H