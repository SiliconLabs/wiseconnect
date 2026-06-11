/***************************************************************************/ /**
 * @file power_manager_example.h
 * @brief Power Manager example functions
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

#ifndef POWER_MANAGER_EXAMPLE_H_
#define POWER_MANAGER_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * Initializes the thread.
 * Button is initialized here.
 * New thread is created with the application_start function.
 * - Application start function is called in the thread.
 * - All the activities are performed in this function.
 * - Wireless interface is initialized first and then power manager service is initialized.
 * - Events are subscribed to get the callbacks.
 * - This function is called in the thread.
 * - This function transits between different power states and sleep upon the button press.
 * - The soc transits the power state in the following patter: 
 *    - By default the power state is PS4. After button press it transits to PS2 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS4. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS3 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS3-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS3. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS2 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS2-Sleep with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS2. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS1 with ulp timer trigger as a wakeup
 *      source and wait for the ulp timer trigger, it triggers after two seconds. When calendar trigger is detected, it wakes up the
 *      soc and the current power state is PS2. Now it waits for the button trigger.
 *    - When button trigger is detected, it transits to PS3 and waits for button trigger.
 *    - When button trigger is detected, it transits to PS4.
 *    - When button trigger is detected, it transits to PS0 deepsleep without retention with calendar one-second trigger as a wakeup
 *      source and wait for the calendar one second trigger, it triggers after one second. When calendar trigger is detected, it wakes up the
 *      soc and resets the controller and starts the execution from main(). Now it waits for the button trigger.
 *    - The transition summary is PS4 -> PS2 -> PS4 -> PS4 Sleep -> PS4 -> PS3 -> PS3 Sleep -> PS3
 *      -> PS2 -> PS2 Sleep -> PS2 -> PS1 -> PS2 -> PS3 -> PS4 -> PS0 -> resets the controller.
 ******************************************************************************/
void power_manager_example_init(void);

/***************************************************************************/ /**
 * This Function is called inside the thread in while loop.
 ******************************************************************************/
void power_manager_example_process_action(void);

#endif /* POWER_MANAGER_EXAMPLE_H_ */
