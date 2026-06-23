/***************************************************************************/ /**
 * @file sl_si91x_sht4x.c
 * @brief I2CSPM compatibility layer implementation for WiseConnect (Si91x)
 *
 * Implements I2CSPM_Transfer() and I2CSPM_Init() on top of sl_si91x_i2c so that
 * unchanged platform drivers (e.g. SHT4x) can be used on WiseConnect.
 *
 * Copyright 2026 Silicon Laboratories Inc. www.silabs.com
 * SPDX-License-Identifier: Zlib
 ******************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include "sl_i2cspm.h"
#include "sl_si91x_i2c.h"
/* Sleeptimer required by SHT4x platform driver dependency (e.g. for measurement delays). */
#include "sl_sleeptimer.h"

/*******************************************************************************
  * I2CSPM_Init - no-op; app initializes I2C via sl_i2c_driver_init()
  ******************************************************************************/
void I2CSPM_Init(I2CSPM_Init_TypeDef *init)
{
  (void)init;
  /* I2C is initialized by the application using sl_i2c_driver_init() and
    * sl_si91x_i2c_pin_init(). No-op here. */
}

/*******************************************************************************
  * Map sl_i2c_status_t (Si91x driver) to I2C_TransferReturn_TypeDef (platform).
  * Required because I2CSPM_Transfer() is called by platform drivers that expect
  * the platform return type and (ret == 0) / (ret < 0) semantics.
  ******************************************************************************/
static I2C_TransferReturn_TypeDef i2c_status_to_transfer_return(sl_i2c_status_t status)
{
  switch (status) {
    case SL_I2C_SUCCESS:
      return i2cTransferDone;
    case SL_I2C_NACK:
      return i2cTransferNack;
    case SL_I2C_TIMEOUT:
      return i2cTransferTimeout;
    case SL_I2C_ARIBITRATION_LOST:
      return i2cTransferArbLost;
    case SL_I2C_BUS_ERROR:
      return i2cTransferBusErr;
    case SL_I2C_INVALID_PARAMETER:
    default:
      return i2cTransferUsageFault;
  }
}

/*******************************************************************************
  * If status indicates failure: disable repeated-start and store the converted
  * status in *result. Returns true if the caller should return *result;
  * otherwise false to continue to the second phase.
  ******************************************************************************/
static bool i2c_stop_repeated_start_on_error(sl_i2c_instance_t instance,
                                             sl_i2c_status_t status,
                                             I2C_TransferReturn_TypeDef *result)
{
  if (status != SL_I2C_SUCCESS) {
    sl_i2c_driver_enable_repeated_start(instance, false);
    *result = i2c_status_to_transfer_return(status);
    return true;
  }
  return false;
}

/*******************************************************************************
  * I2CSPM_Transfer - perform I2C transfer using sl_si91x_i2c
  *
  * The handle (i2c) is a pointer to sl_i2c_instance_t (app defines it as
  * sl_i2cspm_t *sl_si91x_sht4x_sensor = (sl_i2cspm_t *)&_i2c_sensor).
  * seq->addr is 7-bit address << 1 (platform convention); we pass addr >> 1
  * to the Si91x driver (7-bit).
  ******************************************************************************/
I2C_TransferReturn_TypeDef I2CSPM_Transfer(sl_i2cspm_t *i2c, I2C_TransferSeq_TypeDef *seq)
{
  sl_i2c_status_t i2c_status = SL_I2C_SUCCESS;
  sl_i2c_instance_t instance;
  uint16_t addr_7bit = 0;
  I2C_TransferReturn_TypeDef phase_ret;

  if (i2c == NULL || seq == NULL) {
    SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid parameters, NULL handle or sequence");
    return i2cTransferUsageFault;
  }

  /* Handle is pointer to sl_i2c_instance_t (same size as sl_i2cspm_t) */
  instance = *(sl_i2c_instance_t *)i2c;

  if (instance >= SL_I2C_LAST) {
    SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid instance, out of range");
    return i2cTransferUsageFault;
  }

  /*
   * Platform (Gecko) uses seq->addr = 7-bit address << 1 (8-bit style).
   * Si91x driver expects raw 7-bit address; shift right to convert.
   */
  addr_7bit = seq->addr >> 1;

  switch (seq->flags) {
    /* Write only: send buf[0] to the device. */
    case I2C_FLAG_WRITE:
      if (seq->buf[0].len == 0 || seq->buf[0].data == NULL) {
        SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid write buffer, length 0 or NULL");
        return i2cTransferUsageFault;
      }
      i2c_status = sl_i2c_driver_send_data_blocking(instance, addr_7bit, seq->buf[0].data, (uint32_t)seq->buf[0].len);
      return i2c_status_to_transfer_return(i2c_status);

    /* Read only: receive into buf[0]. */
    case I2C_FLAG_READ:
      if (seq->buf[0].len == 0 || seq->buf[0].data == NULL) {
        SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid read buffer, length 0 or NULL");
        return i2cTransferUsageFault;
      }
      i2c_status =
        sl_i2c_driver_receive_data_blocking(instance, addr_7bit, seq->buf[0].data, (uint32_t)seq->buf[0].len);
      return i2c_status_to_transfer_return(i2c_status);

    /* Write then read (repeated start): send buf[0], then receive into buf[1]. */
    case I2C_FLAG_WRITE_READ: {
      if (seq->buf[0].len == 0 || seq->buf[0].data == NULL || seq->buf[1].len == 0 || seq->buf[1].data == NULL) {
        SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid write/read buffer, length 0 or NULL");
        return i2cTransferUsageFault;
      }
      sl_i2c_driver_enable_repeated_start(instance, true);
      i2c_status = sl_i2c_driver_send_data_blocking(instance, addr_7bit, seq->buf[0].data, (uint32_t)seq->buf[0].len);
      if (i2c_stop_repeated_start_on_error(instance, i2c_status, &phase_ret)) {
        return phase_ret;
      }
      /* Disable repeated-start before read so the driver emits STOP after the read. */
      sl_i2c_driver_enable_repeated_start(instance, false);
      i2c_status =
        sl_i2c_driver_receive_data_blocking(instance, addr_7bit, seq->buf[1].data, (uint32_t)seq->buf[1].len);
      return i2c_status_to_transfer_return(i2c_status);
    }

    /* Write then write: send buf[0], then buf[1] in one transaction (repeated start, no STOP between). */
    case I2C_FLAG_WRITE_WRITE: {
      if (seq->buf[0].len == 0 || seq->buf[0].data == NULL || seq->buf[1].len == 0 || seq->buf[1].data == NULL) {
        SL_PRINT_STRING_ERROR("I2CSPM_Transfer: invalid write/write buffer, length 0 or NULL");
        return i2cTransferUsageFault;
      }
      sl_i2c_driver_enable_repeated_start(instance, true);
      i2c_status = sl_i2c_driver_send_data_blocking(instance, addr_7bit, seq->buf[0].data, (uint32_t)seq->buf[0].len);
      if (i2c_stop_repeated_start_on_error(instance, i2c_status, &phase_ret)) {
        SL_PRINT_STRING_ERROR("I2CSPM_Transfer: stop repeated start on error");
        return phase_ret;
      }
      /* Disable repeated-start before second write so the driver emits STOP after the second segment. */
      sl_i2c_driver_enable_repeated_start(instance, false);
      i2c_status = sl_i2c_driver_send_data_blocking(instance, addr_7bit, seq->buf[1].data, (uint32_t)seq->buf[1].len);
      return i2c_status_to_transfer_return(i2c_status);
    }

    /* Unsupported transfer type. */
    default:
      SL_PRINT_STRING_ERROR("I2CSPM_Transfer: unsupported transfer type");
      return i2cTransferUsageFault;
  }
}
