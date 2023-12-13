/*******************************************************************************
* @file  rsi_i2c.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
/**============================================================================
 * @brief This files contains functions prototypes related to EGPIO peripheral
 * 
 * @section Description
 * This file contains the list of function prototypes for the usart and low level function definitions
 * Following are list of API's which need to be defined in this file.
============================================================================**/

// Includes Files

#include "I2C.h"

#ifndef RSI_I2C_H
#define RSI_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

//prototypes
void I2C_EnableDisableInterrupt(I2C_RESOURCES *i2c, uint32_t flag, uint32_t intFlags);
int32_t I2Cx_Initialize(ARM_I2C_SignalEvent_t cb_event, I2C_RESOURCES *i2c);
int32_t I2Cx_Uninitialize(I2C_RESOURCES *i2c);
int32_t I2Cx_PowerControl(ARM_POWER_STATE state, I2C_RESOURCES *i2c);
int32_t I2Cx_MasterTransmit(uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending, I2C_RESOURCES *i2c);
int32_t I2Cx_MasterReceive(uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending, I2C_RESOURCES *i2c);

int32_t I2Cx_SlaveTransmit(const uint8_t *data, uint32_t num, I2C_RESOURCES *i2c);
int32_t I2Cx_SlaveReceive(uint8_t *data, uint32_t num, I2C_RESOURCES *i2c);
int32_t I2Cx_GetDataCount(I2C_RESOURCES *i2c);
uint32_t GetI2CClockFreq(I2C_RESOURCES *i2c);
int32_t I2Cx_Control(uint32_t control, uint32_t arg, I2C_RESOURCES *i2c, uint32_t clock);
ARM_I2C_STATUS I2Cx_GetStatus(I2C_RESOURCES *i2c);
uint32_t I2Cx_MasterHandler(I2C_RESOURCES *i2c);
uint32_t I2Cx_SlaveHandler(I2C_RESOURCES *i2c);
void I2Cx_IRQHandler(I2C_RESOURCES *i2c);
int32_t SetClockRate(I2C_RESOURCES *i2c, uint8_t speed, uint32_t icClk);

#ifdef __cplusplus
}
#endif

#endif // RSI_I2C_H
