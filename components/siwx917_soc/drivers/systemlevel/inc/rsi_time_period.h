/*******************************************************************************
* @file  rsi_time_period.h
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

#ifndef __RSI_TIME_PERIOD_H__
#define __RSI_TIME_PERIOD_H__
#include "rsi_ccp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

rsi_error_t RSI_TIMEPERIOD_TimerClkSel(TIME_PERIOD_Type *pstcTimePeriod, uint32_t u32TimePeriod);
rsi_error_t RSI_TIMEPERIOD_RCCalibration(TIME_PERIOD_Type *pstcTimePeriod,
                                         uint32_t u32TimePeriodRefClk,
                                         uint32_t u32XtalSettle,
                                         uint16_t u16RcClkCnt,
                                         boolean_t bPeriodicCalibEn,
                                         uint8_t u8PeriodicCalibRate,
                                         boolean_t bTemperatureCalibEn,
                                         uint8_t u8TemperatureVal,
                                         uint8_t u8AverageFactor);
uint32_t RSI_TIMEPERIOD_RCCalibTimePeriodRead(TIME_PERIOD_Type *pstcTimePeriod);
uint32_t RSI_TIMEPERIOD_ROCalibTimePeriodRead(TIME_PERIOD_Type *pstcTimePeriod);
rsi_error_t RSI_TIMEPERIOD_XTAL32KHzCalibration(TIME_PERIOD_Type *pstcTimePeriod,
                                                uint32_t u32TimePeriodRefClk,
                                                uint32_t u32XtalSettle,
                                                uint16_t u16RcClkCnt,
                                                boolean_t bPeriodicCalibEn,
                                                uint8_t u8PeriodicCalibRate,
                                                boolean_t bTemperatureCalibEn,
                                                uint8_t u8TemperatureVal,
                                                uint8_t u8AverageFactor);
rsi_error_t RSI_TIMEPERIOD_ROCalibration(TIME_PERIOD_Type *pstcTimePeriod,
                                         uint8_t u8RefClkSrc,
                                         uint32_t u32XtalSettle,
                                         uint16_t u16RoClkCnt,
                                         boolean_t bPeriodicCalibEn,
                                         uint8_t u8PeriodicCalibRate,
                                         uint8_t u8AverageFactor

);
rsi_error_t RSI_TIMEPERIOD_LowPwrTrigSelEn(TIME_PERIOD_Type *pstcTimePeriod, boolean_t bEn);
rsi_error_t RSI_TIMEPERIOD_VbatTrigSel(TIME_PERIOD_Type *pstcTimePeriod, uint8_t u8Time);

#ifdef __cplusplus
}
#endif

/*End of file not truncated*/
#endif /*__RSI_TIME_PERIOD_H__*/
