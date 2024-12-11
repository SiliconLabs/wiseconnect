/*******************************************************************************
* @file  rsi_time_period.h
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
uint32_t RSI_TIMEPERIOD_RCCalibTimePeriodRead(const TIME_PERIOD_Type *pstcTimePeriod);
uint32_t RSI_TIMEPERIOD_ROCalibTimePeriodRead(const TIME_PERIOD_Type *pstcTimePeriod);
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
