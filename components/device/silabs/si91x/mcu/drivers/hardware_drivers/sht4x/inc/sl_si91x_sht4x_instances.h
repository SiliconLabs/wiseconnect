/***************************************************************************/ /**
 * @file sl_si91x_sht4x_instances.h
 * @brief I2CSPM instance declarations for WiseConnect (Si91x) SHT4x
 *
 * The application must define the I2CSPM handle in one .c file, for example:
 *
 *   #include "sl_si91x_sht4x_instances.h"
 *   static sl_i2c_instance_t _i2c_sensor = SL_I2C0;  // or SL_I2C1, SL_ULP_I2C
 *   sl_i2cspm_t *sl_si91x_sht4x_sensor = (sl_i2cspm_t *)&_i2c_sensor;
 *
 * Then use with the platform SHT4x driver:
 *   sl_sht4x_init(sl_si91x_sht4x_sensor, SHT4X_ADDR);
 *   sl_sht4x_measure_rh_and_temp(sl_si91x_sht4x_sensor, SHT4X_ADDR, &rh, &temp);
 *
 * Copyright 2026 Silicon Laboratories Inc. www.silabs.com
 * SPDX-License-Identifier: Zlib
 ******************************************************************************/

#ifndef SL_SI91X_SHT4X_INSTANCES_H
#define SL_SI91X_SHT4X_INSTANCES_H

#include "sl_i2cspm.h"

#ifdef __cplusplus
extern "C" {
#endif

/** I2CSPM instance used by platform drivers (e.g. SHT4x). App must define this. */
extern sl_i2cspm_t *sl_si91x_sht4x_sensor;

#ifdef __cplusplus
}
#endif

#endif /* SL_SI91X_SHT4X_INSTANCES_H */
