/*
 * sensor_type.h
 *
 *  Created on: Mar 4, 2023
 *      Author: kokuncha
 */

#ifndef SENSOR_TYPE_H_
#define SENSOR_TYPE_H_
#include <stdint.h>
typedef void *sensor_driver_handle_t;

#define SENSOR_ID_MASK            0XF0
#define SENSOR_ID_OFFSET          4
#define SENSOR_DATA_GROUP_MAX_NUM 6

typedef union {
  struct {
    float x;
    float y;
    float z;
  };
  float axis[3];
} axis3_t;
#define AXIS3_T axis3_t
#endif

#ifndef RGBW_T
typedef struct {
  float r;
  float g;
  float b;
  float w;
} rgbw_t;
#define RGBW_T rgbw_t
#endif

#ifndef UV_T
typedef struct {
  float uv;
  float uva;
  float uvb;
} uv_t;
#define UV_T uv_t

typedef enum {
  NULL_ID,
  HUMITURE_ID,
  IMU_ID,
  LIGHT_SENSOR_ID,
  GPIO_SENSOR_ID,
  SENSOR_TYPE_MAX,
} sensor_type_t;

typedef enum {
  COMMAND_SET_MODE,
  COMMAND_SET_RANGE,
  COMMAND_SET_ODR,
  COMMAND_SET_POWER,
  COMMAND_SELF_TEST,
  COMMAND_MAX
} sensor_command_t;

typedef enum { POWER_MODE_WAKEUP, POWER_MODE_SLEEP, POWER_MAX } sensor_power_mode_t;

typedef enum {
  RANGE_DEFAULT,
  RANGE_MIN,
  RANGE_MEDIUM,
  RANGE_MAX,
} sensor_range_t;

typedef struct {
  int64_t timestamp;  /*!< timestamp  */
  uint32_t min_delay; /*!<  minimum delay between two events, unit: ms */
  union {
    axis3_t acce;      /*!< Accelerometer.       unit: G           */
    axis3_t gyro;      /*!< Gyroscope.           unit: dps         */
    axis3_t mag;       /*!< Magnetometer.        unit: Gauss       */
    float temperature; /*!< Temperature.         unit: dCelsius     */
    float humidity;    /*!< Relative humidity.   unit: percentage   */
    float baro;        /*!< Pressure.            unit: pascal (Pa)  */
    float light;       /*!< Light.               unit: lux          */
    rgbw_t rgbw;       /*!< Color.               unit: lux          */
    uv_t uv;           /*!< ultraviole           unit: lux          */
    float proximity;   /*!< Distance.            unit: centimeters  */
    float hr;          /*!< Heat rate.           unit: HZ           */
    float tvoc;        /*!< TVOC.                unit: permillage   */
    float noise;       /*!< Noise Loudness.      unit: HZ           */
    float step;        /*!< Step sensor.         unit: 1            */
    float force;       /*!< Force sensor.        unit: mN           */
    float current;     /*!< Current sensor       unit: mA           */
    float voltage;     /*!< Voltage sensor       unit: mV           */
    float data[4];     /*!< for general use */
  };
} sensor_data_t;

typedef struct {
  uint8_t number;             /*!< effective data number */
  sensor_data_t *sensor_data; /*!< data buffer */
} sensor_data_group_t;

#endif
