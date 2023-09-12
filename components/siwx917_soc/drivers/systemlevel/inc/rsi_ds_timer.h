/*******************************************************************************
* @file  rsi_ds_timer.h
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
 *
 */

/**
 * \ingroup   RSI_SPECIFIC_DRIVERS
 * \defgroup  RSI_DS_TIMER_DRIVERS  RSI:RSIxxxx DS TIMER 
 *  @brief    
 *  @{
 *
 */
#ifndef __RSI_DS_TIMER_H__
#define __RSI_DS_TIMER_H__

#include "rsi_error.h"
#include "base_types.h"
#include "rsi_power_save.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	   This API is used to  configure the deep sleep duration of sleep timer
 * @param[in]      dsDuration : sleep duration count in micro seconds
 * @return 	   none
 */
STATIC INLINE void RSI_DST_DurationSet(uint32_t dsDuration)
{
  MCU_FSM->MCU_FSM_DEEP_SLEEP_DURATION_LSB_REG = dsDuration;
}

/**
 * @brief          This API is used to  configure the deep sleep duration of sleep timer
 * @return         none
 */
STATIC INLINE void RSI_DST_IntrUnMask(void)
{
  NPSS_INTR_MASK_CLR_REG = NPSS_TO_MCU_WAKEUP_INTR;
}

/**
 * @brief          This API is used to  mask the Deep Sleep Timer interrupt
 * @return         none
 */
STATIC INLINE void RSI_DST_IntrMask(void)
{
  NPSS_INTR_MASK_SET_REG = NPSS_TO_MCU_WAKEUP_INTR;
}

/**
 * @brief          This API is used to  configure the deep sleep duration of sleep timer
 * @return         none
 */
STATIC INLINE void RSI_DST_TimerIntrClear(void)
{
  NPSS_INTR_CLEAR_REG                = NPSS_TO_MCU_WAKEUP_INTR;
  MCU_FSM->MCU_FSM_WAKEUP_STATUS_REG = 1;
}

#ifdef __cplusplus
}
#endif

#endif /*__RSI_DS_TIMER_H__*/

/*  @} end of RSI_DS_TIMER_DRIVERS */
