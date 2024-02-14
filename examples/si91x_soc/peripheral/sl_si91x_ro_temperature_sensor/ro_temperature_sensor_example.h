/***************************************************************************/ /**
 * @file ro_temperature_sensor_example
 * @brief ro temperature sensor example
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

#ifndef SL_RO_TEMPERATURE_SENSOR_EXAMPLE_H_
#define SL_RO_TEMPERATURE_SENSOR_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * ro temperature sensor example initialization function
 * This API initializes the ro temperature sensor required APIs
 * @param none
 * @return none
 ******************************************************************************/
void ro_temperature_sensor_example_init(void);

/***************************************************************************/ /**
 * Function will run continuously
 * This API will reads the RO sensor based temperature and prints on console
 * @param none
 * @return none
 ******************************************************************************/
void ro_temperature_sensor_example_process_action(void);

#endif /* SL_RO_TEMPERATURE_SENSOR_EXAMPLE_H_ */
