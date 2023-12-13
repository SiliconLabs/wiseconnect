/***************************************************************************/ /**
 * @file sensorhub_error_codes.h
 * @brief sensorhub_error_codes example
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef ERROR_CODES_H_
#define ERROR_CODES_H_

/* ERROR CODES */

/****************************ERRORS MASK******************************/

#define SL_SH_SENSORHUB_ERRORS_MASK 0

/****************************TIMER ERRORS******************************/

#define SL_SH_TIMER_CREATION_FAILED \
  (1 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when timer creation failed
#define SL_SH_TIMER_DELETION_FAILED \
  (2 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when timer deletion failed
#define SL_SH_TIMER_START_FAIL \
  (3 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when starting timer failed
#define SL_SH_TIMER_STOP_FAIL \
  (4 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when stopping timer failed

/****************************SENSORHUB ERRORS******************************/

#define SL_SH_MAX_SENSORS_REACHED \
  (5                              \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if tried to create a new sensor after reaching the max number of sensor limit
#define SL_SH_MEMORY_LIMIT_EXCEEDED \
  (6                                \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when allocating memory to store sensor data failed to memory limit reached
#define SL_SH_SENSOR_CREATE_FAIL \
  (7                             \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned when starting sensor API is called for a sensor without creating it
#define SL_SH_COMMAND_SET_POWER_FAIL \
  (8 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if command set power is failed
#define SL_SH_COMMAND_SET_RANGE_FAIL \
  (9 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if command set range is failed
#define SL_SH_COMMAND_SELF_TEST_FAIL \
  (10 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if command self-test is failed
#define SL_SH_INVALID_PARAMETERS \
  (11 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This is a generic error code for any invalid parameters
#define SL_SH_GPIO_OUT_OF_RANGE \
  (12 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if invalid gpio is given
#define SL_SH_SENSOR_IMPLEMENTATION_NOT_FOUND \
  (13                                         \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///<This error code is returned if no hal implementation is found for that sensor type
#define SL_SH_INTERRUPT_TYPE_CONFIG_FAIL \
  (14 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if interrupt type configuration failed
#define SL_SH_MEMORY_ALLOCATION_FAILED \
  (15 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if failed to allocate memory for sensor hal
#define SL_SH_SENSOR_INDEX_NOT_FOUND \
  (0xFF) ///< This error code is returned if the sensor index is not found for a sensor

/****************************SENSOR CONFIGURATION ERRORS******************************/

#define SL_SH_CONFIG_NOT_FOUND \
  (16 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if no configuration is found for a given sensor
#define SL_SH_INVALID_ADDRESS \
  (17 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if an invalid address is given for a sensor
#define SL_SH_WRONG_INTERRUPT_TYPE \
  (18 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if the wrong interrupt type is given for a sensor
#define SL_SH_INVALID_MODE \
  (19                      \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if the wrong mode is given in the sensor configuration
#define SL_SH_INVALID_DELIVERY_MODE \
  (20                               \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if the wrong delivery mode is given in sensor configuration

/****************************HAL ERRORS******************************/

#define SL_SH_HAL_SENSOR_CREATION_FAILED \
  (21 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if HAL creates sensor API failed
#define SL_SH_HAL_SENSOR_DELETION_FAILED \
  (22 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if HAL delete sensor API failed
#define SL_SH_HAL_SENSOR_SAMPLE_FAIL \
  (23 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if HAL sample sensor API failed
#define SL_SH_HAL_SENSOR_CONTROL_FAIL \
  (24 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if HAL control sensor API failed

/******************************Blocking Errors********************************/

#define SL_SH_POWER_TASK_CREATION_FAILED \
  (25 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if power task creation failed
#define SL_SH_SENSOR_TASK_CREATION_FAILED \
  (26 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if sensor task creation failed
#define SL_SH_EM_TASK_CREATION_FAILED \
  (27 << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if EM task creation failed
#define SL_ALL_PERIPHERALS_INIT_FAILED \
  (28                                  \
   << SL_SH_SENSORHUB_ERRORS_MASK) ///< This error code is returned if all(i2c,spi,adc) peripheral's initialization failed

#define SL_DRIVER_OK ARM_DRIVER_OK ///< Operation succeeded

#endif
