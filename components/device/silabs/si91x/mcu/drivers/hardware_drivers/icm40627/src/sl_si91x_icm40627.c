/***************************************************************************/ /**
 * @file
 * @brief Driver for the Invensense ICM40627 6-axis motion sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_si91x_icm40627.h"
#include <stdint.h>
#include <stdio.h>
#include "sl_sleeptimer.h"

/*******************************************************************************
 ***************************  Defines / Macros  ********************************
 ******************************************************************************/
#define DUMMY_DATA 0xA5 // Dummy data to be written to receive data
/*******************************************************************************
 *************************** LOCAL VARIABLES   *******************************
 ******************************************************************************/
/*******************************************************************************
 ***********************  Local function Prototypes ***************************
 ******************************************************************************/
static void wait_till_ssi_gets_idle(sl_ssi_handle_t ssi_driver_handle);
static sl_status_t icm40627_write_register(sl_ssi_handle_t ssi_driver_handle,
                                           uint16_t reg_addr,
                                           uint8_t *data,
                                           uint32_t data_length);
static sl_status_t icm40627_read_register(sl_ssi_handle_t ssi_driver_handle,
                                          uint16_t reg_addr,
                                          uint8_t *data_in,
                                          uint32_t num_bytes);

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Concatenate preprocessor tokens A and B. */
#define SL_CONCAT(A, B) A##B

/* Generate the cmu clock symbol based on instance. */
#define SL_ICM40627_REG_SPI_CLK(N) SL_CONCAT(cmuClock_EUSART, N)
/** @endcond */

/***************************************************************************/ /**
 *    Initializes the ICM40627 sensor. Enables the power supply and SPI lines,
 *    sets up the host SPI controller, configures the chip control interface,
 *    clock generator and interrupt line.
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_init(sl_ssi_handle_t ssi_driver_handle)
{
  sl_status_t status;
  uint8_t ssi_data;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data_in[ssi_data_length];
  uint8_t temp = 0;

  /* Disable I2C interface, use SPI */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INTF_CONFIG0, ssi_data_in, ssi_data_length);
  if (status != SL_STATUS_OK) {
    // If it fails to execute the API, it will not execute rest of the things
    return status;
  }
  temp = ssi_data_in[ssi_data_length - 1];
  temp |= SL_ICM40627_INTF_CONFIG0_BIT_UI_SIFS_CFG_DISABLE_I2C;
  status = icm40627_write_register(ssi_driver_handle, (uint8_t)SL_ICM40627_REG_INTF_CONFIG0, &temp, 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Set clock select to automatic clock source selection and clear the sleep bit */
  ssi_data = (uint8_t)SL_ICM40627_INT_SOURCE6_BIT_CLKSEL_SEL_PLL;
  status   = icm40627_write_register(ssi_driver_handle, (uint8_t)SL_ICM40627_REG_INT_SOURCE6, &ssi_data, 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Calibration data */
  float accel[3] = { 0.01, 0.01, 0.01 };
  float gyro[3]  = { 0.01, 0.01, 0.01 };

  /* calibrate sensor */
  sl_si91x_icm40627_calibrate_accel_and_gyro(ssi_driver_handle, accel, gyro);

  /* enable the sensor */
  sl_si91x_icm40627_enable_sensor(ssi_driver_handle, true, true, true);

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    De-initializes the ICM40627 sensor.
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_deinit(void)
{
  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Performs software reset on the ICM40627 chip
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_software_reset(sl_ssi_handle_t ssi_driver_handle)
{
  sl_status_t status;
  uint32_t ssi_data_length = 1;
  uint16_t soft_reset_reg_addr;
  uint8_t soft_reset_data;

  /* Select the appropriate reset register and value */
  soft_reset_reg_addr = (uint16_t)SL_ICM40627_REG_DEVICE_CONFIG;
  soft_reset_data     = (uint8_t)SL_ICM40627_BIT_SOFT_RESET_CONFIG_EN_RESET;

  /* Set SOFT_RESET_CONFIG bit to initiate Software reset */
  status = icm40627_write_register(ssi_driver_handle, soft_reset_reg_addr, &soft_reset_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Wait 100ms to complete the reset sequence */
  sl_sleeptimer_delay_millisecond(100);

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Reads register from the ICM40627 device
 ******************************************************************************/
static sl_status_t icm40627_read_register(sl_ssi_handle_t ssi_driver_handle,
                                          uint16_t reg_addr,
                                          uint8_t *data_in,
                                          uint32_t num_bytes)
{
  sl_status_t status;
  uint8_t ssi_data_out[num_bytes];
  uint32_t ssi_data_length = 0;
  uint8_t bank;

  /* Select appropriate register bank for current register */
  bank   = (reg_addr >> 8);
  status = icm40627_select_register_bank(ssi_driver_handle, bank);
  if (status != SL_STATUS_OK) {
    return status;
  }

  ssi_data_out[ssi_data_length++] = (uint8_t)(reg_addr | 0x80); // Set R/W bit
  for (; ssi_data_length < (num_bytes + 1); ssi_data_length++) {
    ssi_data_out[ssi_data_length] = DUMMY_DATA;
  }

  status = sl_si91x_ssi_transfer_data(ssi_driver_handle, ssi_data_out, data_in, num_bytes);

  // wait till the read operation is completed
  wait_till_ssi_gets_idle(ssi_driver_handle);

  return status;
}

/***************************************************************************/ /**
 *    Writes a register in the ICM40627 device
 ******************************************************************************/
static sl_status_t icm40627_write_register(sl_ssi_handle_t ssi_driver_handle,
                                           uint16_t reg_addr,
                                           uint8_t *data,
                                           uint32_t data_length)
{
  sl_status_t status;
  uint8_t ssi_data_out[data_length + 1];
  uint8_t ssi_data_length = 0;
  uint8_t bank;

  /* Select appropriate register bank for current register */
  bank   = (reg_addr >> 8);
  status = icm40627_select_register_bank(ssi_driver_handle, bank);
  if (status != SL_STATUS_OK) {
    return status;
  }

  ssi_data_out[ssi_data_length++] = (uint8_t)(reg_addr & 0x7F); // Clear R/W bit
  for (; ssi_data_length < (data_length + 1); ssi_data_length++) {
    ssi_data_out[ssi_data_length] = data[ssi_data_length - 1];
  }

  status = sl_si91x_ssi_send_data(ssi_driver_handle, ssi_data_out, ssi_data_length);
  if (status != SL_STATUS_OK) {
    // If it fails to execute the API, it will not execute rest of the things
    return status;
  }

  // wait till the write operation is completed
  wait_till_ssi_gets_idle(ssi_driver_handle);

  return status;
}

/***************************************************************************/ /**
  * @brief
 *    Select the desired register bank.
 *
 * @param[in] bank
 *    The address of the register bank (0..3)
 ******************************************************************************/
sl_status_t icm40627_select_register_bank(sl_ssi_handle_t ssi_driver_handle, uint8_t bank)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data_out[ssi_data_length];
  uint8_t index = 0;

  if (index < ssi_data_length) {
    ssi_data_out[index++] = SL_ICM40627_REG_BANK_SEL;
  }
  if (index < ssi_data_length) {
    ssi_data_out[index++] = bank;
  }

  status = sl_si91x_ssi_send_data(ssi_driver_handle, ssi_data_out, ssi_data_length);
  if (status != SL_STATUS_OK) {
    // If it fails to execute the API, it will not execute rest of the things
    return status;
  }

  // wait till the write operation is completed
  wait_till_ssi_gets_idle(ssi_driver_handle);

  return status;
}

/***************************************************************************/ /**
 *    Reads the device ID of the ICM40627
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_device_id(sl_ssi_handle_t ssi_driver_handle, uint8_t *dev_id)
{
  sl_status_t status;
  uint16_t reg_address     = SL_ICM40627_REG_WHO_AM_I;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data_in[ssi_data_length];

  status = icm40627_read_register(ssi_driver_handle, reg_address, ssi_data_in, ssi_data_length);
  if (status != SL_STATUS_OK) {
    // If it fails to execute the API, it will not execute rest of the things
    return status;
  }

  // Full-duplex fills the read buffer by same data length,
  // we take only the actual data response anticipated
  *dev_id = ssi_data_in[ssi_data_length - 1];

  return status;
}

/***************************************************************************/ /**
 *    Reads the temperature sensor raw value and converts to Celsius.
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_temperature_data(sl_ssi_handle_t ssi_driver_handle, float *temperature)
{
  uint32_t ssi_data_length = 2;
  uint8_t data0_in[ssi_data_length];
  uint8_t data1_in[ssi_data_length];
  int16_t raw_temp_data;

  /* Read temperature registers */
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_TEMP_DATA0, data0_in, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_TEMP_DATA1, data1_in, ssi_data_length);

  /* Convert to int16 */
  raw_temp_data = (int16_t)((data1_in[ssi_data_length - 1] << 8) + data0_in[ssi_data_length - 1]);

  /* Calculate the Centigrade value from the raw reading */
  *temperature = ((float)raw_temp_data / 132.48f) + 25.0f;

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Reads the raw acceleration value and converts to g value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_accel_data(sl_ssi_handle_t ssi_driver_handle, float accel[3])
{
  uint32_t ssi_data_length = 2;
  uint8_t raw_data_x0[ssi_data_length];
  uint8_t raw_data_x1[ssi_data_length];
  uint8_t raw_data_y0[ssi_data_length];
  uint8_t raw_data_y1[ssi_data_length];
  uint8_t raw_data_z0[ssi_data_length];
  uint8_t raw_data_z1[ssi_data_length];
  float accel_res;
  int16_t temp;

  /* Retrieve the current resolution */
  sl_si91x_icm40627_get_accel_resolution(ssi_driver_handle, &accel_res);

  /* Read the six raw data registers into data array */
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_X0, raw_data_x0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_X1, raw_data_x1, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_Y0, raw_data_y0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_Y1, raw_data_y1, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_Z0, raw_data_z0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_DATA_Z1, raw_data_z1, ssi_data_length);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp     = ((int16_t)raw_data_x1[ssi_data_length - 1] << 8) | raw_data_x0[ssi_data_length - 1];
  accel[0] = (float)temp * accel_res;

  temp     = ((int16_t)raw_data_y1[ssi_data_length - 1] << 8) | raw_data_y0[ssi_data_length - 1];
  accel[1] = (float)temp * accel_res;

  temp     = ((int16_t)raw_data_z1[ssi_data_length - 1] << 8) | raw_data_z0[ssi_data_length - 1];
  accel[2] = (float)temp * accel_res;

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Reads the raw gyroscope value and converts to deg/sec value based on
 *    the actual resolution
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_gyro_data(sl_ssi_handle_t ssi_driver_handle, float gyro[3])
{
  uint32_t ssi_data_length = 2;
  uint8_t raw_data_x0[ssi_data_length];
  uint8_t raw_data_x1[ssi_data_length];
  uint8_t raw_data_y0[ssi_data_length];
  uint8_t raw_data_y1[ssi_data_length];
  uint8_t raw_data_z0[ssi_data_length];
  uint8_t raw_data_z1[ssi_data_length];
  float gyro_res;
  int16_t temp;

  /* Set the current resolution */
  sl_si91x_icm40627_set_gyro_full_scale(ssi_driver_handle, SL_ICM40627_GYRO_FULLSCALE_250DPS);

  /* Retrieve the current resolution */
  sl_si91x_icm40627_get_gyro_resolution(ssi_driver_handle, &gyro_res);

  /* Read the six raw data registers into data array */
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_X0, raw_data_x0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_X1, raw_data_x1, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_Y0, raw_data_y0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_Y1, raw_data_y1, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_Z0, raw_data_z0, ssi_data_length);
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_DATA_Z1, raw_data_z1, ssi_data_length);

  /* Convert the MSB and LSB into a signed 16-bit value and multiply by the resolution to get the dps value */
  temp    = ((int16_t)raw_data_x1[ssi_data_length - 1] << 8) | raw_data_x0[ssi_data_length - 1];
  gyro[0] = (float)temp * gyro_res;

  temp    = ((int16_t)raw_data_y1[ssi_data_length - 1] << 8) | raw_data_y0[ssi_data_length - 1];
  gyro[1] = (float)temp * gyro_res;

  temp    = ((int16_t)raw_data_z1[ssi_data_length - 1] << 8) | raw_data_z0[ssi_data_length - 1];
  gyro[2] = (float)temp * gyro_res;

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Gets the actual resolution of the accelerometer
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_accel_resolution(sl_ssi_handle_t ssi_driver_handle, float *accel_res)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  /* Read the actual acceleration full scale setting */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  ssi_data[ssi_data_length - 1] &= SL_ICM40627_MASK_ACCEL_FS_SEL;

  /* Calculate the resolution */
  switch (ssi_data[ssi_data_length - 1]) {
    case SL_ICM40627_ACCEL_FULLSCALE_2G:
      *accel_res = 2.0 / 32768.0;
      break;

    case SL_ICM40627_ACCEL_FULLSCALE_4G:
      *accel_res = 4.0 / 32768.0;
      break;

    case SL_ICM40627_ACCEL_FULLSCALE_8G:
      *accel_res = 8.0 / 32768.0;
      break;

    case SL_ICM40627_ACCEL_FULLSCALE_16G:
      *accel_res = 16.0 / 32768.0;
      break;

    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Gets the actual resolution of the gyroscope
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_get_gyro_resolution(sl_ssi_handle_t ssi_driver_handle, float *gyro_res)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  /* Read the actual gyroscope full scale setting */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  ssi_data[ssi_data_length - 1] &= 0xF0;

  /* Calculate the resolution */
  switch (ssi_data[ssi_data_length - 1]) {
    case SL_ICM40627_GYRO_FULLSCALE_250DPS:
      *gyro_res = 250.0 / 32768.0;
      break;

    case SL_ICM40627_GYRO_FULLSCALE_500DPS:
      *gyro_res = 500.0 / 32768.0;
      break;

    case SL_ICM40627_GYRO_FULLSCALE_1000DPS:
      *gyro_res = 1000.0 / 32768.0;
      break;

    case SL_ICM40627_GYRO_FULLSCALE_2000DPS:
      *gyro_res = 2000.0 / 32768.0;
      break;

    default:
      EFM_ASSERT(false);
      return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

///***************************************************************************/ /**
// *    Enables or disables the sensors in the ICM40627 chip
// ******************************************************************************/
sl_status_t sl_si91x_icm40627_enable_sensor(sl_ssi_handle_t ssi_driver_handle, bool accel, bool gyro, bool temp)
{
  sl_status_t status;
  uint8_t pwrManagement    = 0x00;
  uint8_t gyroOn           = 0x40;
  uint8_t gyroOff          = 0x00;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_PWR_MGMT0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  pwrManagement = ssi_data[ssi_data_length - 1];

  /* To enable the accelerometer */
  if (accel) {
    pwrManagement |= SL_ICM40627_PWR_MGMT0_ACCEL_MODE_LN;
  } else {
    pwrManagement &= SL_ICM40627_PWR_MGMT0_ACCEL_MODE_DIS;
  }

  /* To enable gyro */
  if (gyro) {
    pwrManagement |= SL_ICM40627_PWR_MGMT0_GYRO_MODE_LN;

    /*Set value to 0x40 when turning on gyroscope.*/
    status =
      icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_ON_OFF_CONFIG, &gyroOn, ssi_data_length - 1);
    if (status != SL_STATUS_OK) {
      return status;
    }
  } else {
    pwrManagement &= SL_ICM40627_PWR_MGMT0_GYRO_MODE_DIS;
    /*Set value to 0 when turning off gyroscope.*/
    status =
      icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_ON_OFF_CONFIG, &gyroOff, ssi_data_length - 1);
    if (status != SL_STATUS_OK) {
      return status;
    }
  }

  /* To enable the temperature sensor */
  if (temp) {
    pwrManagement &= SL_ICM40627_PWR_MGMT0_TEMP_MODE_EN;
  } else {
    pwrManagement |= SL_ICM40627_PWR_MGMT0_TEMP_MODE_DIS;
  }

  /* Write back the modified values */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_PWR_MGMT0, &pwrManagement, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Wait 200ms to complete the write */
  sl_sleeptimer_delay_millisecond(200);

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Enables or disables the sleep mode of the device
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_enable_sleep_mode(sl_ssi_handle_t ssi_driver_handle, bool enable)
{
  sl_status_t status;
  uint8_t reg;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS3, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg = ssi_data[ssi_data_length - 1];

  if (enable) {
    /* Sleep: Set the SLEEP interrupt bit and clear the WAKE interrupt bit */
    reg |= SL_ICM40627_BIT_MASK_SLEEP_INT;
    reg &= ~(SL_ICM40627_BIT_MASK_WAKE_INT);
  } else {
    /* Wake Up: Clear the SLEEP interrupt bit and set the wake interrupt bit */
    reg &= ~(SL_ICM40627_BIT_MASK_SLEEP_INT);
    reg |= SL_ICM40627_BIT_MASK_WAKE_INT;
  }

  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS3, &reg, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the sample rate for both the accelerometer and the gyroscope.
 ******************************************************************************/
float sl_si91x_icm40627_set_sample_rate(sl_ssi_handle_t ssi_driver_handle, float sample_rate)
{
  uint8_t accel_sample_rate_setting;
  uint8_t gyro_sample_rate_setting;

  /* Map sample_rate to corresponding register values using range checks */
  if (sample_rate > 6000.0f && sample_rate <= 8000.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_8000;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_8000;
    sample_rate               = 8000.0f;
  } else if (sample_rate > 3000.0f && sample_rate <= 6000.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_4000;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_4000;
    sample_rate               = 4000.0f;
  } else if (sample_rate > 1500.0f && sample_rate <= 3000.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_2000;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_2000;
    sample_rate               = 2000.0f;
  } else if (sample_rate > 750.0f && sample_rate <= 1500.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_1000;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_1000;
    sample_rate               = 1000.0f;
  } else if (sample_rate > 375.0f && sample_rate <= 750.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_500;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_500;
    sample_rate               = 500.0f;
  } else if (sample_rate > 100.0f && sample_rate <= 375.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_200;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_200;
    sample_rate               = 200.0f;
  } else if (sample_rate > 75.0f && sample_rate <= 100.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_100;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_100;
    sample_rate               = 100.0f;
  } else if (sample_rate > 37.0f && sample_rate <= 75.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_50;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_50;
    sample_rate               = 50.0f;
  } else if (sample_rate > 18.0f && sample_rate <= 37.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_25;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_25;
    sample_rate               = 25.0f;
  } else if (sample_rate > 9.0f && sample_rate <= 18.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_12_5;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_12_5;
    sample_rate               = 12.5f;
  } else if (sample_rate > 6.0f && sample_rate <= 9.0f) {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_6_25;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_12_5;
    sample_rate               = 6.25f;
  } else {
    accel_sample_rate_setting = SL_ICM40627_ACCEL_ODR_VALUE_3_125;
    gyro_sample_rate_setting  = SL_ICM40627_GYRO_ODR_VALUE_12_5;
    sample_rate               = 3.125f;
  }

  /* Set the sample rate for gyroscope and accelerometer */
  sl_si91x_icm40627_gyro_set_sample_rate(ssi_driver_handle, gyro_sample_rate_setting);
  sl_si91x_icm40627_accel_set_sample_rate(ssi_driver_handle, accel_sample_rate_setting);

  /* Return the actual sample rate set */
  return sample_rate;
}

/***************************************************************************/ /**
 *    Sets the Output Data Rate of the gyroscope
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_gyro_set_sample_rate(sl_ssi_handle_t ssi_driver_handle, uint8_t gyro_ODR)
{
  sl_status_t status;
  uint8_t reg;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  /* Validate gyro_ODR */
  switch (gyro_ODR) {
    case SL_ICM40627_GYRO_ODR_VALUE_8000:
    case SL_ICM40627_GYRO_ODR_VALUE_4000:
    case SL_ICM40627_GYRO_ODR_VALUE_2000:
    case SL_ICM40627_GYRO_ODR_VALUE_1000:
    case SL_ICM40627_GYRO_ODR_VALUE_500:
    case SL_ICM40627_GYRO_ODR_VALUE_200:
    case SL_ICM40627_GYRO_ODR_VALUE_100:
    case SL_ICM40627_GYRO_ODR_VALUE_50:
    case SL_ICM40627_GYRO_ODR_VALUE_25:
    case SL_ICM40627_GYRO_ODR_VALUE_12_5:
      /* Valid ODR - proceed */
      break;
    default:
      /* Invalid ODR */
      return SL_STATUS_INVALID_PARAMETER;
  }

  /* Read the ICM40627_REG_GYRO_CONFIG0 register */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg = ssi_data[ssi_data_length - 1];
  reg &= ~(SL_ICM40627_MASK_GYRO_ODR_VALUE);

  /* Write the new bandwidth value to the gyro config register */
  reg |= (gyro_ODR & SL_ICM40627_MASK_GYRO_ODR_VALUE);
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, &reg, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the Output Data Rate of the accelerometer
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_accel_set_sample_rate(sl_ssi_handle_t ssi_driver_handle, uint8_t accel_ODR)
{
  sl_status_t status;
  uint8_t reg;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  /* Validate accel_ODR */
  switch (accel_ODR) {
    case SL_ICM40627_ACCEL_ODR_VALUE_8000:
    case SL_ICM40627_ACCEL_ODR_VALUE_4000:
    case SL_ICM40627_ACCEL_ODR_VALUE_2000:
    case SL_ICM40627_ACCEL_ODR_VALUE_1000:
    case SL_ICM40627_ACCEL_ODR_VALUE_500:
    case SL_ICM40627_ACCEL_ODR_VALUE_200:
    case SL_ICM40627_ACCEL_ODR_VALUE_100:
    case SL_ICM40627_ACCEL_ODR_VALUE_50:
    case SL_ICM40627_ACCEL_ODR_VALUE_25:
    case SL_ICM40627_ACCEL_ODR_VALUE_12_5:
    case SL_ICM40627_ACCEL_ODR_VALUE_6_25:
    case SL_ICM40627_ACCEL_ODR_VALUE_3_125:
      /* Valid ODR - proceed */
      break;
    default:
      /* Invalid ODR */
      return SL_STATUS_INVALID_PARAMETER;
  }

  /* Read the ICM40627_REG_ACCEL_CONFIG0 register */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg = ssi_data[ssi_data_length - 1];
  reg &= ~(SL_ICM40627_MASK_ACCEL_ODR_VALUE);

  /* Write the new bandwidth value to the accel config register */
  reg |= (accel_ODR & SL_ICM40627_MASK_ACCEL_ODR_VALUE);
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, &reg, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the bandwidth of the gyroscope
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_set_gyro_bandwidth(sl_ssi_handle_t ssi_driver_handle, uint8_t gyro_ODR)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];
  uint8_t temp;

  /* Read the GYRO_CONFIG_1 register */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  temp = ssi_data[ssi_data_length - 1];
  temp &= ~(SL_ICM40627_MASK_GYRO_UI_FILT_BW);
  temp |= (gyro_ODR & SL_ICM40627_MASK_GYRO_UI_FILT_BW);

  /* Write the new bandwidth value to the gyro config register */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, &temp, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the bandwidth of the accelerometer
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_set_accel_bandwidth(sl_ssi_handle_t ssi_driver_handle, uint8_t accel_ODR)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];
  uint8_t temp;

  /* Read the GYRO_CONFIG_1 register */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  temp = ssi_data[ssi_data_length - 1];
  temp &= ~(SL_ICM40627_MASK_ACCEL_UI_FILT_BW);
  temp |= (accel_ODR & SL_ICM40627_MASK_ACCEL_UI_FILT_BW);

  /* Write the new bandwidth value to the accel config register */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, &temp, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the full scale value of the accelerometer
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_set_accel_full_scale(sl_ssi_handle_t ssi_driver_handle, uint8_t accelFs)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];
  uint8_t temp;

  accelFs &= SL_ICM40627_MASK_ACCEL_FS_SEL;

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  temp = ssi_data[ssi_data_length - 1];
  temp &= ~(SL_ICM40627_MASK_ACCEL_FS_SEL);
  temp |= accelFs;

  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_ACCEL_CONFIG0, &temp, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Sets the full scale value of the accelerometer
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_set_gyro_full_scale(sl_ssi_handle_t ssi_driver_handle, uint8_t gyroFs)
{
  sl_status_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];
  uint8_t temp;

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  temp = ssi_data[ssi_data_length - 1];
  temp &= ~(SL_ICM40627_MASK_GYRO_FS_SEL);
  temp |= gyroFs;

  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_GYRO_CONFIG0, &temp, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Enables or disables the interrupts in the ICM40627 chip
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_enable_interrupt(sl_ssi_handle_t ssi_driver_handle, bool dataReadyEnable, bool womEnable)
{
  sl_status_t status;
  uint8_t intEnable        = 0;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_SOURCE1, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Enable one or both of the interrupt sources if required */
  if (womEnable) {
    intEnable = (ssi_data[ssi_data_length - 1] | SL_ICM40627_BIT_MASK_WOM_X_INT1_EN | SL_ICM40627_BIT_MASK_WOM_Y_INT1_EN
                 | SL_ICM40627_BIT_MASK_WOM_Z_INT1_EN);
  }
  /* Write value to register */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_INT_SOURCE1, &intEnable, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* All interrupts are disabled by default */
  intEnable = 0;

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_SOURCE0, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  if (dataReadyEnable) {
    intEnable = ssi_data[ssi_data_length - 1] | SL_ICM40627_MASK_UI_DRDY_INT1_EN;
  }
  /* Write value to register */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_INT_SOURCE0, &intEnable, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Reads the interrupt status registers of the ICM40627 chip
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_read_interrupt_status(sl_ssi_handle_t ssi_driver_handle, uint32_t *intStatus)
{
  sl_status_t status;
  uint8_t reg_status;
  uint8_t reg_status2;
  uint8_t reg_status3;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg_status = ssi_data[ssi_data_length - 1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS2, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg_status2 = ssi_data[ssi_data_length - 1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS3, ssi_data, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  reg_status3 = ssi_data[ssi_data_length - 1];

  *intStatus = 0;                              // Clear all bits
  *intStatus |= (uint32_t)reg_status;          // Bits 0-7 from reg_status
  *intStatus |= ((uint32_t)reg_status2 << 8);  // Bits 8-15 from reg_status2
  *intStatus |= ((uint32_t)reg_status3 << 16); // Bits 16-23 from reg_status3

  return SL_STATUS_OK;
}

/***************************************************************************/ /**
 *    Checks if new data is available for read
 ******************************************************************************/
bool sl_si91x_icm40627_is_data_ready(sl_ssi_handle_t ssi_driver_handle)
{
  bool ret;
  uint8_t status;
  uint32_t ssi_data_length = 2;
  uint8_t ssi_data[ssi_data_length];

  ret = false;
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_INT_STATUS, ssi_data, ssi_data_length);

  status = ssi_data[ssi_data_length - 1];
  if (status & SL_ICM40627_MASK_DATA_RDY_INT) {
    ret = true;
  }

  return ret;
}

/***************************************************************************/ /**
 *    Accelerometer and gyroscope calibration function. Reads the gyroscope
 *    and accelerometer values while the device is at rest and in level. The
 *    resulting values are loaded to the accel and gyro bias registers to cancel
 *    the static offset error.
 ******************************************************************************/
sl_status_t sl_si91x_icm40627_calibrate_accel_and_gyro(sl_ssi_handle_t ssi_driver_handle,
                                                       float *accel_bias_scaled,
                                                       float *gyro_bias_scaled)
{
  sl_status_t status;
  uint8_t data_length = 13;
  uint8_t data[data_length];
  uint16_t i, packet_count, fifo_count;
  int32_t gyro_bias[3]  = { 0, 0, 0 };
  int32_t accel_bias[3] = { 0, 0, 0 };
  int32_t accel_temp[3];
  int32_t gyro_temp[3];
  int32_t accel_bias_factory[3] = { 0, 0, 0 };
  int32_t gyro_bias_stored[3]   = { 0, 0, 0 };
  float gyro_res, accel_res;
  uint8_t disable_fifo    = 0x00;
  uint8_t reset_fifo      = 0x40;
  uint8_t ssi_data_length = 2;
  uint8_t countl[ssi_data_length], counth[ssi_data_length];
  uint8_t sensor_write_fifo         = 0x0F;
  fifo_count                        = 0;
  uint8_t sensor_disable_write_fifo = 0x00;

  /* Disable all sensors */
  sl_si91x_icm40627_enable_sensor(ssi_driver_handle, false, false, false);

  /* 246Hz BW for the accelerometer and 200Hz for the gyroscope */
  sl_si91x_icm40627_set_accel_bandwidth(ssi_driver_handle, 0xF0);
  sl_si91x_icm40627_set_gyro_bandwidth(ssi_driver_handle, 0x0F);

  /* Set the most sensitive range: 2G full scale and 250dps full scale */
  sl_si91x_icm40627_set_accel_full_scale(ssi_driver_handle, SL_ICM40627_ACCEL_FULLSCALE_2G);
  sl_si91x_icm40627_set_gyro_full_scale(ssi_driver_handle, SL_ICM40627_GYRO_FULLSCALE_250DPS);

  /*Set the output data rate as 1kHz*/
  sl_si91x_icm40627_accel_set_sample_rate(ssi_driver_handle, SL_ICM40627_ACCEL_ODR_VALUE_1000);
  sl_si91x_icm40627_gyro_set_sample_rate(ssi_driver_handle, SL_ICM40627_GYRO_ODR_VALUE_1000);

  /* Retrieve the resolution per bit */
  sl_si91x_icm40627_get_accel_resolution(ssi_driver_handle, &accel_res);
  sl_si91x_icm40627_get_gyro_resolution(ssi_driver_handle, &gyro_res);

  /* Disable the FIFO */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_CONFIG, &disable_fifo, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Enable the accelerometer and the gyro */
  sl_si91x_icm40627_enable_sensor(ssi_driver_handle, true, true, false);

  /* The accel sensor needs max 30ms, the gyro max 35ms to fully start */
  /* Experiments show that the gyro needs more time to get reliable results */
  sl_sleeptimer_delay_millisecond(50);

  /* Reset the FIFO */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_CONFIG, &reset_fifo, ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Enable the different sensors to write to the FIFO
  status =
    icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_CONFIG1, &sensor_write_fifo, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* The max FIFO size is 4096 bytes, but is limited to 512. */
  while (fifo_count < FIFO_SAMPLE_COUNT) {
    sl_sleeptimer_delay_millisecond(5);
    icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_COUNTH, counth, ssi_data_length);
    icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_COUNTL, countl, ssi_data_length);

    /* Convert to a 16 bit value */
    fifo_count = ((uint16_t)(counth[ssi_data_length - 1] << 8) | countl[ssi_data_length - 1]);
  }

  /* Disable accelerometer and gyro to store the data in FIFO */
  status = icm40627_write_register(ssi_driver_handle,
                                   SL_ICM40627_REG_FIFO_CONFIG1,
                                   &sensor_disable_write_fifo,
                                   ssi_data_length - 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Read FIFO sample count */
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_COUNTH, counth, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_COUNTL, countl, ssi_data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Convert to a 16 bit value */
  fifo_count = ((uint16_t)(counth[ssi_data_length - 1] << 8) | countl[ssi_data_length - 1]);

  /* Number of data sets (3 axis of accel an gyro, two bytes each = 12 bytes) */
  packet_count = fifo_count / 12;

  /* Retrieve the data from the FIFO */
  for (i = 0; i < packet_count; i++) {
    icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_DATA, data, data_length);
    /* Convert to 16 bit signed accel and gyro x,y and z values */
    accel_temp[0] = ((int16_t)(data[1] << 8) | data[2]);
    accel_temp[1] = ((int16_t)(data[3] << 8) | data[4]);
    accel_temp[2] = ((int16_t)(data[5] << 8) | data[6]);

    gyro_temp[0] = ((int16_t)(data[7] << 8) | data[8]);
    gyro_temp[1] = ((int16_t)(data[9] << 8) | data[10]);
    gyro_temp[2] = ((int16_t)(data[11] << 8) | data[12]);

    /* Sum the values */
    accel_bias[0] += accel_temp[0];
    accel_bias[1] += accel_temp[1];
    accel_bias[2] += accel_temp[2];

    gyro_bias[0] += gyro_temp[0];
    gyro_bias[1] += gyro_temp[1];
    gyro_bias[2] += gyro_temp[2];
  }

  /* Divide by packet count to get the average */
  accel_bias[0] /= packet_count;
  accel_bias[1] /= packet_count;
  accel_bias[2] /= packet_count;
  gyro_bias[0] /= packet_count;
  gyro_bias[1] /= packet_count;
  gyro_bias[2] /= packet_count;

  /* Accelerometer: add or remove (depending on the orientation of the chip) 1G (gravity) from the Z axis value */
  if (accel_bias[ssi_data_length] > 0L) {
    accel_bias[ssi_data_length] -= (int32_t)(1.0f / accel_res);
  } else {
    accel_bias[ssi_data_length] += (int32_t)(1.0f / accel_res);
  }

  /* Convert the values to degrees per sec for displaying */
  gyro_bias_scaled[0] = (float)gyro_bias[0] * gyro_res;
  gyro_bias_scaled[1] = (float)gyro_bias[1] * gyro_res;
  gyro_bias_scaled[2] = (float)gyro_bias[2] * gyro_res;

  /* Read stored gyro trim values. After reset these values are all 0 */
  icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER0, data, data_length);
  gyro_bias_stored[0] = (int16_t)data[1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER1, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  uint8_t temp = data[1];
  gyro_bias_stored[0] |= (int16_t)((temp & 0x0F) << 8);
  gyro_bias_stored[1] = (int16_t)((temp & 0xF0) << 4);

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER2, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  gyro_bias_stored[1] |= (int16_t)data[1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER3, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  gyro_bias_stored[2] = (int16_t)data[1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER4, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  temp = data[1];
  gyro_bias_stored[2] |= (int16_t)((temp & 0x0F) << 8);

  /* The gyro bias should be stored in 1000dps full scaled format. We measured in 250dps to get */
  /* the best sensitivity, so need to divide by 4 */
  /* Substract from the stored calibration value */
  gyro_bias_stored[0] -= gyro_bias[0] / 4;
  gyro_bias_stored[1] -= gyro_bias[1] / 4;
  gyro_bias_stored[2] -= gyro_bias[2] / 4;

  /* Calculate the accelerometer bias values to store in the hardware accelerometer bias registers. These registers contain */
  /* factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold */
  /* non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature */
  /* compensation calculations(? the datasheet is not clear). Accelerometer bias registers expect bias input */
  /* as 2048 LSB per g, so that the accelerometer biases calculated above must be divided by 8. */

  /* Construct total accelerometer bias, including calculated average accelerometer bias from above */
  /* Scale the 2g full scale (most sensitive range) results to 16g full scale - divide by 8 */
  /* Subtract from the factory calibration value */

  /* Read factory accelerometer trim values */
  accel_bias_factory[0] = (int16_t)((temp & 0xF0) << 4);
  status                = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER5, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  accel_bias_factory[0] |= (int16_t)data[1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER6, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  accel_bias_factory[1] |= (int16_t)data[1];

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER7, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  temp = data[1];
  accel_bias_factory[1] |= (int16_t)((temp & 0x0F) << 8);
  accel_bias_factory[2] = (int16_t)((temp & 0xF0) << 4);

  status = icm40627_read_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER8, data, data_length);
  if (status != SL_STATUS_OK) {
    return status;
  }
  accel_bias_factory[2] |= (int16_t)data[1];

  accel_bias_factory[0] -= ((accel_bias[0] / 8) & ~1);
  accel_bias_factory[1] -= ((accel_bias[1] / 8) & ~1);
  accel_bias_factory[2] -= ((accel_bias[2] / 8) & ~1);

  /* Split the values into two bytes */
  data[0] = gyro_bias_stored[0] & 0xFF;
  data[1] = ((gyro_bias_stored[0]) >> 8) | ((gyro_bias_stored[1] >> 4) & 0xF0);
  data[2] = gyro_bias_stored[1] & 0xFF;
  data[3] = gyro_bias_stored[2] & 0xFF;
  data[4] = ((gyro_bias_stored[2]) >> 8) | ((accel_bias_factory[0] >> 4) & 0xF0);
  data[5] = accel_bias_factory[0] & 0xFF;
  data[6] = accel_bias_factory[1] & 0xFF;
  data[7] = (((accel_bias_factory[1])) >> 8) | ((accel_bias_factory[2] >> 4) & 0xF0);
  data[8] = accel_bias_factory[2] & 0xFF;

  /* Write the  gyro and accel bias values to the chip */
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER0, &data[0], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER1, &data[1], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER2, &data[2], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER3, &data[3], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER4, &data[4], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER5, &data[5], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER6, &data[6], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER7, &data[7], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }
  status = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_OFFSET_USER8, &data[8], 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Convert the values to G for displaying */
  accel_bias_scaled[0] = (float)accel_bias[0] * accel_res;
  accel_bias_scaled[1] = (float)accel_bias[1] * accel_res;
  accel_bias_scaled[2] = (float)accel_bias[2] * accel_res;

  /* Disable the FIFO */
  disable_fifo = 0x00;
  status       = icm40627_write_register(ssi_driver_handle, SL_ICM40627_REG_FIFO_CONFIG, &disable_fifo, 1);
  if (status != SL_STATUS_OK) {
    return status;
  }

  /* Disable all sensors */
  sl_si91x_icm40627_enable_sensor(ssi_driver_handle, false, false, false);

  return SL_STATUS_OK;
}

/*******************************************************************************
 * Function to wait till GSPI communication completes
 *
 * @param gspi (SSI_TypeDef) Pointer to the GSPI base address.
 * @return none
 ******************************************************************************/
static void wait_till_ssi_gets_idle(sl_ssi_handle_t ssi_driver_handle)
{
  volatile ARM_SPI_STATUS ssi_status;

  do {
    // waiting for GSPI to be in idle state
    ssi_status = ((sl_ssi_driver_t *)ssi_driver_handle)->GetStatus();
  } while (ssi_status.busy);
}

/** @endcond */
