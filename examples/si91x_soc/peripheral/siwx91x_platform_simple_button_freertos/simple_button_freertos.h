/***************************************************************************/ /**
 * @file simple_button_freertos.h
 * @brief simple button freertos example
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

#ifndef SIMPLE_BUTTON_FREERTOS_H
#define SIMPLE_BUTTON_FREERTOS_H

/***************************************************************************/ /**
 * Initialize example
 ******************************************************************************/
void button_init(void);

/***************************************************************************/ /**
 * Simple Button task
 ******************************************************************************/
void simple_button_task(void *argument);

#endif // SIMPLE_BUTTON_FREERTOS_H
