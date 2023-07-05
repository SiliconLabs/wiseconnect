/*******************************************************************************
* @file  rsi_processor_sensor.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * Includes
 */

#include "rsi_chip.h"

/*==============================================*/
/** 
 * @fn        error_t RSI_ProSense_Enable(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
 * @brief     This API is used to Enable / Disable the process sensor
 * @param[in] pstcProcessSensor : pointer to the processor sensor register instance
 * @param[in] bEN : to Enable / Disble of process sensor
 *            0: Disable process sensor
 *            1: Enable process sensor
 * @return    RSI_OK on success
 *            Error code on failure
 */

error_t RSI_ProSense_Enable(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
{
  pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.PROCESS_SENSOR_EN = bEN;
  return RSI_OK;
}

/*==============================================*/
/** 
 * @fn        uint32_t RSI_ProSense_Read(MCU_ProcessSensor_Type *pstcProcessSensor)
 * @brief     This API is used to read the PS count from the process sensor
 * @param[in] pstcProcessSensor : pointer to the processor sensor register instance
 * @return    returns the PS count values
 */

uint32_t RSI_ProSense_Read(MCU_ProcessSensor_Type *pstcProcessSensor)
{
  return pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.PS_COUNT;
}

/*==============================================*/
/** 
 * @fn        uint32_t RSI_ProSense_GetNumCycles(MCU_ProcessSensor_Type *pstcProcessSensor)
 * @brief     This API is used to read the Num cycle count from the process sensor
 * @param[in] pstcProcessSensor : pointer to the processor sensor register instance
 * @return    returns the PS count values
 */

uint32_t RSI_ProSense_GetNumCycles(MCU_ProcessSensor_Type *pstcProcessSensor)
{
  return pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.NUM_CYCLES;
}

/*==============================================*/
/** 
 * @fn         error_t RSI_ProSense_ClkEnable(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
 * @brief      This API is used to Enable / Disable the process sensor clock
 * @param[in]  pstcProcessSensor : pointer to the processor sensor register instance
 * @param[in]  bEN : to Enable / Disble of process sensor
 *             0: Disable process sensor clock
 *             1: Enable process sensor clock
 * @return     RSI_OK on success
 *             Error code on failure
 */

error_t RSI_ProSense_ClkEnable(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
{
  if (bEN) {
    pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.PS_CLK_SW_ON = 1;
  } else {
    pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.PS_CLK_SW_OFF = 1;
  }
  return RSI_OK;
}

/*==============================================*/
/** 
 * @fn        error_t RSI_ProSense_RingClkStart(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
 * @brief     This API is used to start / Stop the Ring-Oscillator clock for estimating process corner.
 * @param[in] pstcProcessSensor  : pointer to the processor sensor register instance
 * @param[in] bEN  : to Enable / Disble of process sensor
 *            0: STOP  Ring-Oscillator clock for estimating process corner.
 *            1: START Ring-Oscillator clock for estimating process corner.
 * @return    RSI_OK on success
 *            Error code on failure
 */

error_t RSI_ProSense_RingClkStart(MCU_ProcessSensor_Type *pstcProcessSensor, boolean_t bEN)
{
  pstcProcessSensor->PROCESS_SENSOR_ENABLE_AND_READ_b.PS_RING_CLK_START = bEN;
  return RSI_OK;
}
