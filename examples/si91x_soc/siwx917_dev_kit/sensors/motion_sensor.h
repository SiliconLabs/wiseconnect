/***************************************************************************/ /**
 * @file motion_sensor.h
 * @brief Motion sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef MOTION_SENSOR_H_
#define MOTION_SENSOR_H_

void motion_sensor_init(void);

sl_status_t accelerometer_read(float *x, float *y, float *z);

sl_status_t gyro_read(float *x, float *y, float *z);

sl_status_t temperature_read(float *temperature);

#endif /* MOTION_SENSOR_H_ */
