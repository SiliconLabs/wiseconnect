/***************************************************************************/ /**
* @file bjt_temperature_Sensor_example.h
* @brief BJT Temperature example
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

#ifndef BJT_TEMPERATURE_SENSOR_EXAMPLE_H_
#define BJT_TEMPERATURE_SENSOR_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
* BJT Temperature example initialization function
* BJT Temperature channel is configured by the BJT initialization
*
* @param none
* @return none
******************************************************************************/
void bjt_temperature_sensor_example_init(void);

/***************************************************************************/ /**
* Function will run continuously and provides equivalent output of 12 bit ADC data
* @param none
* @return none
******************************************************************************/
void bjt_temperature_sensor_example_process_action(void);

#endif /* BJT_TEMPERATURE_SENSOR_EXAMPLE_H_ */
