/***************************************************************************/ /**
 * @file pwm_mode_freertos.h
 * @brief PWM FreeRTOS example
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

#ifndef PWM_MODE_FREERTOS_H_
#define PWM_MODE_FREERTOS_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * PWM example initialization function (FreeRTOS). Creates the PWM task that
 * handles all PWM configuration and event processing.
 *
 * @param none
 * @return none
 ******************************************************************************/
void pwm_mode_example_init(void);

#endif /* PWM_MODE_FREERTOS_H_ */
