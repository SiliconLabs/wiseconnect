/***************************************************************************/ /**
* @file  config_timer_example.h
* @brief config-timer examples functions
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

#ifndef SL_SI91X_CONFIG_TIMER_EXAMPLE_H_
#define SL_SI91X_CONFIG_TIMER_EXAMPLE_H_

#define CT_PWM_MODE_USECASE     CLEAR //< To run PWM output code
#define CT_COUNTER_MODE_USECASE SET   //< To run normal counter code
// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
* Config-timer example initialization function
* Clock and Timer Configurations, timeout callback registration
* First use-case: At the configured timeout of timer, CT generates interrupts and toggles GPIO
* After ten CT interrupts, Stopping timer, unregistering timeout callback and de-initialization
* Second use-case: counter0 produces PWM output with varying duty cycle and counter1
* produces square wave output
*
* @param none
* @return none
******************************************************************************/
void config_timer_example_init(void);

/***************************************************************************/ /**
* Function will run continuously and will wait for interrupt
*
* @param none
* @return none
******************************************************************************/
void config_timer_example_process_action(void);

#endif /* SL_SI91X_CONFIG_TIMER_EXAMPLE_H_ */
