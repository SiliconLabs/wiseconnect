/***************************************************************************/ /**
 * @file sl_i2cspm.h
 * @brief I2CSPM compatibility layer for WiseConnect (Si91x)
 *
 * This layer is intended for Si91x/WiseConnect. It implements the Gecko Platform
 * I2CSPM API on top of sl_si91x_i2c so that unchanged platform drivers (e.g. SHT4x)
 * can be used on WiseConnect.
 *
 * I2C must be initialized by the application (e.g. sl_i2c_driver_init() and pin
 * configuration such as sl_si91x_i2c_pin_init()) before using this layer.
 *
 * The application must:
 *   - Init I2C with sl_i2c_driver_init() and pin config (sl_si91x_i2c_pin_init).
 *   - Define the I2CSPM handle in one .c file (see sl_si91x_sht4x_instances.h for SHT4x).
 *
 * Copyright 2026 Silicon Laboratories Inc. www.silabs.com
 * SPDX-License-Identifier: Zlib
 ******************************************************************************/

#ifndef SL_I2CSPM_H
#define SL_I2CSPM_H

#include <stdint.h>
#include "sl_si91x_i2c.h"
#include "sl_log_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define I2C_FLAG_WRITE       0x0001
#define I2C_FLAG_READ        0x0002
#define I2C_FLAG_WRITE_READ  0x0004
#define I2C_FLAG_WRITE_WRITE 0x0008

/*******************************************************************************
 * Enums (compat for platform I2C_TransferReturn_TypeDef)
 *
 * Values and order match the Gecko platform I2CSPM header (Series 2/3) so that
 * unchanged platform drivers (e.g. SHT4x) using symbolic checks
 * (e.g. ret == i2cTransferDone) work correctly. Do not rely on (ret == 0) for success; use (ret == i2cTransferDone).
 * i2cTransferTimeout is an extension so that SL_I2C_TIMEOUT from the Si91x driver
 * is reported distinctly from i2cTransferBusErr.
 ******************************************************************************/
typedef enum {
  i2cTransferInProgress,
  i2cTransferDone,
  i2cTransferNack,
  i2cTransferBusErr,
  i2cTransferArbLost,
  i2cTransferUsageFault,
  i2cTransferTimeout, /**< I2C timeout (Si91x); distinct from bus error. */
} I2C_TransferReturn_TypeDef;

/*******************************************************************************
 * Structs (compat for platform I2C_TransferSeq_TypeDef)
 ******************************************************************************/
typedef struct {
  uint16_t addr;
  uint16_t flags;
  struct {
    uint8_t *data;
    uint16_t len;
  } buf[2];
} I2C_TransferSeq_TypeDef;

/*******************************************************************************
 * I2CSPM handle type
 * On WiseConnect, sl_i2cspm_t is the same size as sl_i2c_instance_t so that
 * a pointer (sl_i2cspm_t *) can point to an sl_i2c_instance_t variable.
 ******************************************************************************/
typedef sl_i2c_instance_t sl_i2cspm_t;

/*******************************************************************************
 * Init type (opaque for compat; I2CSPM_Init is a no-op, real init is via
 * sl_i2c_driver_init).
 ******************************************************************************/
typedef struct {
  void *port;
  uint8_t sclPort;
  uint8_t sclPin;
  uint8_t sdaPort;
  uint8_t sdaPin;
  uint32_t i2cRefFreq;
  uint32_t i2cMaxFreq;
  uint32_t i2cClhr;
} I2CSPM_Init_TypeDef;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/**
 * @brief Initialize I2C peripheral (no-op in compat layer).
 * I2C must be initialized with sl_i2c_driver_init() and pin config in the app.
 */
void I2CSPM_Init(I2CSPM_Init_TypeDef *init);

/**
 * @brief Perform I2C transfer.
 * @param i2c  Pointer to sl_i2c_instance_t (cast from sl_i2cspm_t *).
 * @param seq  Transfer sequence (addr = 7-bit addr << 1; flags; buf[0], buf[1]).
 * @return     i2cTransferDone on success, or error code.
 */
I2C_TransferReturn_TypeDef I2CSPM_Transfer(sl_i2cspm_t *i2c, I2C_TransferSeq_TypeDef *seq);

#ifdef __cplusplus
}
#endif

#endif /* SL_I2CSPM_H */
