/*******************************************************************************
* @file  rsi_timers.h
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

//Include Files

#include "rsi_ccp_common.h"
#include "rsi_error.h"

#ifndef RSI_TIMERS_H
#define RSI_TIMERS_H

#ifdef __cplusplus
extern "C" {
#endif

// LOCAL OR GLOBAL DEFINES

#define RSI_TIMERS_API_VERSION RSI_DRIVER_VERSION_MAJOR_MINOR(2, 00)  // API version 0.1
#define RSI_TIMERS_DRV_VERSION RSI_DRIVER_VERSION_MAJOR_MINOR(00, 01) // driver version 0.1

#define MICRO_SEC_MODE      1
#define _256_MICRO_SEC_MODE 2
#define COUNTER_DOWN_MODE   0

// TIMERS Events
#define RSI_TIMERS_EVENT_TIMER0_TIMEOUT (1UL << 0) // Timer0 timeout interrupt
#define RSI_TIMERS_EVENT_TIMER1_TIMEOUT (1UL << 1) // Timer1 timeout interrupt
#define RSI_TIMERS_EVENT_TIMER2_TIMEOUT (1UL << 2) // Timer2 timeout interrupt
#define RSI_TIMERS_EVENT_TIMER3_TIMEOUT (1UL << 3) // Timer3 timeout interrupt

#define TIMER_MODE 0x18

// Example defines
#define TIMER_0 0
#define TIMER_1 1
#define TIMER_2 2
#define TIMER_3 3

#define ULP_TIMER_RF_REF_CLK         0
#define ULP_TIMER_ULP_32KHZ_RO_CLK   1
#define ULP_TIMER_ULP_32KHZ_RC_CLK   2
#define ULP_TIMER_ULP_32KHZ_XTAL_CLK 3
#define ULP_TIMER_ULP_32MHZ_RC_CLK   4
#define ULP_TIMER_ULP_20MHZ_RO_CLK   5
#define ULP_TIMER_SOC_CLK            6

#define ULP_TIMER_CLK_DIV_FACT 0

#define PERIODIC_TIMER 1
#define ONESHOT_TIMER  0

typedef TIMERS_Type RSI_TIMERS_T;

// brief  TIMERS Driver Capabilities.
typedef struct {
  uint32_t timerCount : 4;   // Number of Timers
  uint32_t microSecMode : 1; // supports Micro second mode
} RSI_TIMERS_CAPABILITIES_T;

/// @brief Enumeration to represent ulp-timer direction
typedef enum {
  DOWN_COUNTER,   ///< For ULP Timer up-counting direction
  UP_COUNTER,     ///< For ULP Timer down-counting direction
  LAST_DIRECTION, ///< Last member of enum for validation
} ulp_timer_dir_t;

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_SetDirection(RSI_TIMERS_T *pTIMER, uint8_t timerNum, boolean_t countDir)
 * @brief   This API is used to set direction of the timer
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)
 * @param[in]   countDir   :  counter direction
 *                          - \ref UP_COUNTER
 *                          - \ref DOWN_COUNTER
 * @return    return the timer error code
 */
STATIC INLINE rsi_error_t RSI_TIMERS_SetDirection(RSI_TIMERS_T *pTIMER, uint8_t timerNum, ulp_timer_dir_t countDir)
{
  if (timerNum <= TIMER_3) {
    if (countDir == UP_COUNTER) {
      pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.COUNTER_UP = ENABLE;
    } else if (countDir == DOWN_COUNTER) {
      pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.COUNTER_UP = DISABLE;
    } else {
      return ERROR_INVAL_COUNTER_DIR;
    }
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          uint32_t RSI_TIMERS_getDirection(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
 * @brief   This API is used to get direction of the timer
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)
 *
 * @return   countDir   :  counter direction
 *                        -  1 for UP_COUNTER
 *                        -  0 for DOWN_COUNTER
 */
STATIC INLINE uint32_t RSI_TIMERS_getDirection(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  uint8_t counterDir;
  if (timerNum <= TIMER_3) {
    counterDir = pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.COUNTER_UP;
    return counterDir;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
}

/*===================================================*/
/**
 * @fn          uint32_t RSI_TIMERS_GetTimerMode(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
 * @brief   This API is used to get the mode of timer
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)
 * @return    return the type of timer if valid timer else error code
 */
STATIC INLINE uint32_t RSI_TIMERS_GetTimerMode(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    return (pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_MODE);
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
}
/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_TimerStart(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
 * @brief		This API is used to start the timer 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code
 */
STATIC INLINE rsi_error_t RSI_TIMERS_TimerStart(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_START = ENABLE;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_TimerStop(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
 * @brief		This API is used to stop the timer 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_TimerStop(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_STOP = ENABLE;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_InterruptEnable(RSI_TIMERS_T *pTIMER , uint8_t timerNum)
 * @brief		This API is used to enable the timer interrupt 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_InterruptEnable(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_INTR_ENABLE = ENABLE;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_InterruptDisable(RSI_TIMERS_T *pTIMER , uint8_t timerNum)
 * @brief		This API is used to disable the timer interrupt 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_InterruptDisable(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_INTR_ENABLE = DISABLE;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_InterruptClear(RSI_TIMERS_T *pTIMER , uint8_t timerNum)
 * @brief		This API is used to clear the timer interrupt 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_InterruptClear(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_INTR_CLR = ENABLE;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_SetMatch( RSI_TIMERS_T *pTIMER, uint8_t timerNum, uint32_t match)
 * @brief		This API is used to disable the timer interrupt 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)
 * @param[in]   match      : delay time
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_SetMatch(RSI_TIMERS_T *pTIMER, uint8_t timerNum, uint32_t match)
{
  if (timerNum <= TIMER_3) {
    pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_MATCH = match;
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_InterruptStatus(RSI_TIMERS_T *pTIMER , uint8_t timerNum)
 * @brief		This API is used to get the timer interrupt status 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer interrupt status if valid timer else 0. 
 */
STATIC INLINE uint8_t RSI_TIMERS_InterruptStatus(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    return (uint8_t)(pTIMER->MCUULP_TMR_INTR_STAT & (1 << timerNum));
  } else {
    return 0;
  }
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_SetTimerType( RSI_TIMERS_T *pTIMER, uint8_t timerType,  uint8_t timerNum)
 * @brief		This API is used to set the timer type 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerType  : timer type
 *	                          - \ref MICRO_SEC_MODE
 *                            - \ref _256_MICRO_SEC_MODE
 *                            - \ref COUNTER_DOWN_MODE
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_SetTimerType(RSI_TIMERS_T *pTIMER, uint8_t timerType, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    if ((timerType == MICRO_SEC_MODE) || (timerType == _256_MICRO_SEC_MODE) || (timerType == COUNTER_DOWN_MODE)) {
      pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_TYPE = (unsigned int)(timerType & 0x03);
    } else {
      return ERROR_INVAL_TIMERTYPE;
    }
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          rsi_error_t RSI_TIMERS_SetTimerMode(RSI_TIMERS_T *pTIMER, boolean_t mode,uint8_t timerNum)
 * @brief		This API is used to set the timer mode 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   mode       : in which mode timer run
 *                           - \ref PERIODIC_TIMER
 *                           - \ref ONESHOT_TIMER
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the timer error code 
 */
STATIC INLINE rsi_error_t RSI_TIMERS_SetTimerMode(RSI_TIMERS_T *pTIMER, boolean_t mode, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    if ((mode == PERIODIC_TIMER) || (mode == ONESHOT_TIMER)) {
      pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_MODE = (unsigned int)(mode & 0x01);
    } else {
      return ERROR_INVAL_TIMER_MODE;
    }
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn          uint32_t RSI_TIMERS_GetTimerType(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
 * @brief		This API is used to get the type of timer 
 * @param[in]   pTIMER     : Pointer to the TIMERS instance register area
 * @param[in]   timerNum   : Timer number(0 to 3)                          
 * @return 		return the type of timer if valid timer else error code 
 */
STATIC INLINE uint32_t RSI_TIMERS_GetTimerType(RSI_TIMERS_T *pTIMER, uint8_t timerNum)
{
  if (timerNum <= TIMER_3) {
    return (pTIMER->MATCH_CTRL[timerNum].MCUULP_TMR_CNTRL_b.TMR_TYPE);
  } else {
    return ERROR_INVAL_TIMER_NUM;
  }
}

// TIMERS FUNCTION PROTOTYPES
RSI_DRIVER_VERSION_M4 RSI_TIMERS_GetVersion(void);

RSI_TIMERS_CAPABILITIES_T RSI_TIMERS_GetCapabilities(void);

rsi_error_t timers_microsec_timer_config(RSI_TIMERS_T *pTIMER,
                                         uint8_t timerNum,
                                         uint16_t integer,
                                         uint8_t fractional,
                                         uint8_t mode);
uint32_t timers_read_timer(RSI_TIMERS_T *pTIMER, uint8_t timerNum, boolean_t countDir);
void IRQ002_Handler();

#ifdef __cplusplus
}
#endif

#endif // RSI_TIMERS_H
