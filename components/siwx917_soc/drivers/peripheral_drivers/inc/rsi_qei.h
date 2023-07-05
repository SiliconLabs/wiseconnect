/*******************************************************************************
* @file  rsi_qei.h
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

// Include Files

#include "base_types.h"
#include "rsi_ccp_common.h"

#ifndef RSI_QEI_H
#define RSI_QEI_H

#ifdef __cplusplus
extern "C" {
#endif

// QEI status flags
#define QEI_IDX          BIT(0) // This is a direct value from the position signal generator
#define QEI_POS_B        BIT(1) // This is a direct value from the position signal generator.Value
#define QEI_POS_A        BIT(2) // This is a direct value from the position signal generator.Value
#define QEI_POS_CTRL_ERR BIT(3) // Count Error Status Flag bit
#define QEI_POS_CTRL_DIR BIT(4) // Position Counter Direction Status bit

// QEI interrupt status flags

// Note : Same flags are used to clear the interrupts

// Interrupt status bits
#define QEI_POS_CNT_RST_INTR_LVL BIT(0) // This is raised when the position counter reaches it?s extremes.
#define QEI_IDX_CNT_MAT_INT_LVL \
  BIT(1) //	This is raised when index counter reaches max value loaded in to index_max_cnt register.
#define QEI_POS_CNT_ERR_INTR_LVL \
  BIT(2) /* Whenever number of possible positions are mismatched 
                                                     with actual positions are received between two index pulses this will raised.*/
#define QEI_VELO_LESS_THAN_INTR_LVL \
  BIT(3) // When velocity count is less than the value given in velocity_value_to_compare register, interrupt is raised.
#define QEI_POS_CNT_MAT_INTR_LVL \
  BIT(4) //  This is raised when the position counter reaches position match value, which is programmable
#define VELOCITY_COMPUTATION_OVER_INTR_LVL BIT(5) // QEI control register set

// Configuration settings
#define QEI_SFT_RST BIT(0) // Quadrature encoder soft reset. It is self reset signal
#define QEI_SWAP_PHASE_AB_B \
  BIT(1) // Phase A and Phase B Input Swap Select bit 1 = Phase A and Phase B inputs are swapped
#define QEI_POS_CNT_RST_WITH_IDX_EN \
  BIT(2) // 1 = position counter is getting reset for every index pulse 0 = position counter is getting
#define QEI_POS_CNT_DIR_CTRL    BIT(4) // 0 = position B pin defines the direction of position counter
#define QEI_POS_CNT_DIR_FRM_REG BIT(5) // Position Counter Direction indication from user
#define QEI_IDX_CNT_RST_EN      BIT(8) // 1= index counter is going to reset after reaching max count, which is mentioned in
#define QEI_DIGITAL_FILTER_BYPASS \
  BIT(9) // 1= digital filter is bypassed for all input signals (position A, position B and Index)
#define QEI_TIMER_MODE \
  BIT(10) /* 1 = timer mode. In this mode, decoded timer pulse and direction are taken from position A and 
                                               position B pins respectively */
#define QEI_START_VELOCITY_CNTR BIT(11) // Starting the velocity counter. It is self reset bit
#define QEI_STOP_IN_IDLE        BIT(12) // QEI stop in idle
#define QEI_POS_CNT_16_BIT_MDE  BIT(13) // QEI position counter 16 bit mode enable
#define QEI_POS_CNT_RST         BIT(14) // 1= position counter is going to reset
#define QEI_IDX_CNT_RST         BIT(15) // 1= index counter is going to reset

// QEI control bits
#define QEI_INDEX_CNT_INIT          BIT(12)
#define QEI_UNIDIRECTIONAL_INDEX    BIT(11)
#define QEI_UNIDIRECTIONAL_VELOCITY BIT(10)

// Interrupt un mask
#define QEI_POS_CNT_RESET_INTR_UNMASK      BIT(0)
#define QEI_IDX_CNT_MATCH_INTR_UNMASK      BIT(1)
#define QEI_POS_CNTR_ERR_INTR_UNMASK       BIT(2)
#define QEI_VEL_LESS_THAN_INTR_UNMASK      BIT(3)
#define QEI_IDX_POS_MATCH_INTR_UNMASK      BIT(4)
#define QEI_VEL_COMPUTATION_OVER_INTR_MASK BIT(5)

// Clock divider
// Digital Filter Clock Divide Select bits
#define QEI_CLK_DIV_1    0x00
#define QEI_CLK_DIV_2    0x01
#define QEI_CLK_DIV_4    0x02
#define QEI_CLK_DIV_8    0x03
#define QEI_CLK_DIV_16   0x04
#define QEI_CLK_DIV_32   0x05
#define QEI_CLK_DIV_64   0x06
#define QEI_CLK_DIV_128  0x07
#define QEI_CLK_DIV_256  0x08
#define QEI_CLK_DIV_512  0x09
#define QEI_CLK_DIV_1024 0x0A

// QEI Encoding mode
#define QEI_ENCODING_MODE_1X 0x00
#define QEI_ENCODING_MODE_2X 0x01
#define QEI_ENCODING_MODE_4X 0x02
/** @addtogroup SOC25
* @{
*/
/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetConfiguration(volatile QEI_Type  *pstcQei, uint32_t configParms)   
 * @brief	    Sets the QEI module configurations
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   configParms : OR'ed values configurations needs to be passed to this variable
 * @return 		none
 */
STATIC INLINE void RSI_QEI_SetConfiguration(volatile QEI_Type *pstcQei, uint32_t configParms)
{
  // QEI configuration set
  pstcQei->QEI_CTRL_REG_SET = configParms;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_ClrConfiguration(volatile QEI_Type  *pstcQei, uint32_t configParms)
 * @brief	    clears the QEI module configurations
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   configParms : OR'ed values configurations needs to be passed to this variable
 * @return 		none
 */
STATIC INLINE void RSI_QEI_ClrConfiguration(volatile QEI_Type *pstcQei, uint32_t configParms)
{
  // QEI configuration set
  pstcQei->QEI_CTRL_REG_RESET = configParms;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetControls(volatile QEI_Type  *pstcQei, uint32_t cntrlParms)
 * @brief	    Sets the QEI control configuration
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   cntrlParms : OR'ed values control field bits.
 * @return 		none
 */
STATIC INLINE void RSI_QEI_SetControls(volatile QEI_Type *pstcQei, uint32_t cntrlParms)
{
  // QEI configuration set
  pstcQei->QEI_CNTLR_INIT_REG |= cntrlParms;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_ClrControls(volatile QEI_Type  *pstcQei, uint32_t cntrlParms)
 * @brief		Clears the QEI control configuration
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   cntrlParms : OR'ed values control field bits.
 *              \n 
 *              \ref QEI_CNTLR_INIT_REG
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_ClrControls(volatile QEI_Type *pstcQei, uint32_t cntrlParms)
{
  // QEI configuration set
  pstcQei->QEI_CNTLR_INIT_REG &= ~cntrlParms;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetMaxPosCnt(volatile QEI_Type  *pstcQei,uint32_t maxPosition)
 * @brief       Sets the MAX value for position counter. This is a maximum count value that is
 *              allowed to increment in the position counter.If position counter reaches this value in
 *              positive direction, will get set to zero and if reaches zero in negative direction, will get
 *              set to this max count.\ref Pos_cnt_rst_with_index_en,
 *              \ref qei_ctrl_reg[2] should be reset (0) for setting the position
 *              counter when reaches max/zero value.
 *              \ref QEI_POSITION_MAX_CNT_LSW_REG
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   maxPosition :Max position value
 * @return 		none
 */
STATIC INLINE void RSI_QEI_SetMaxPosCnt(volatile QEI_Type *pstcQei, uint32_t maxPosition)
{
  // configure least significant word (LSW)
  pstcQei->QEI_POSITION_MAX_CNT_LSW_REG = (0xFFFF & maxPosition);
  pstcQei->QEI_POSITION_MAX_CNT_LSW_REG |= (0xFFFF & (maxPosition >> 16)) << 16;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetMaxPosCnt(volatile QEI_Type  *pstcQei)
 * @brief 	    Gets the maximum position count
 * @param[in]   pstcQei : Pointer to the QEI register instance
 *             \n 
 *             \ref QEI_POSITION_MAX_CNT_LSW_REG
 * @return 	returns the maximum position count
 */
STATIC INLINE uint32_t RSI_QEI_GetMaxPosCnt(volatile QEI_Type *pstcQei)
{
  return pstcQei->QEI_POSITION_MAX_CNT_LSW_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetPosMatch(volatile QEI_Type  *pstcQei, uint32_t matchPosition)    
 * @brief	    Sets the position match value to compare the position counter. When it is matched with position
 *              counter, interrupt is raised.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   matchPosition : position match value.
 *              \ref QEI_POSITION_MATCH_REG
 * @return 		none
 */
STATIC INLINE void RSI_QEI_SetPosMatch(volatile QEI_Type *pstcQei, uint32_t matchPosition)
{
  // configure least significant word (LSW)
  pstcQei->QEI_POSITION_MATCH_REG = matchPosition;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetPosMatch(volatile QEI_Type  *pstcQei)
 * @brief	    Gets the position match value.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 *              \ref STATIC INLINE uint32_t RSI_QEI_GetPosMatch(volatile QEI_Type  *pstcQei)
 * @return 	    returns the position match value
 */
STATIC INLINE uint32_t RSI_QEI_GetPosMatch(volatile QEI_Type *pstcQei)
{
  // configure least significant word (LSW)
  return pstcQei->QEI_POSITION_MATCH_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetPosition(volatile QEI_Type  *pstcQei)
 * @brief       Gets the current position increment counter value to know the position of the motor.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    position current position counter value
 *              \n
 *              \ref QEI_POSITION_CNT_REG 
 */
STATIC INLINE uint32_t RSI_QEI_GetPosition(volatile QEI_Type *pstcQei)
{
  // Return the position count register
  return pstcQei->QEI_POSITION_CNT_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetIndex(volatile QEI_Type  *pstcQei)   
 * @brief	    Gets the current index increment counter value to know the index count of the motor.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    index current counter value
 *              \n
 *              \ref QEI_INDEX_CNT_REG
 */
STATIC INLINE uint32_t RSI_QEI_GetIndex(volatile QEI_Type *pstcQei)
{
  // Return the position count register
  return pstcQei->QEI_INDEX_CNT_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetMaxIndex(volatile QEI_Type  *pstcQei , uint16_t maxIdxCntVal)
 * @brief	    configures the QEI index maximum count.This is a maximum count value that is allowed to increment in the index counter
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   maxIdxCntVal : maximum allowed index value to be programmed.
 *              \n
 *              \ref QEI_INDEX_MAX_CNT_REG
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_SetMaxIndex(volatile QEI_Type *pstcQei, uint16_t maxIdxCntVal)
{
  pstcQei->QEI_INDEX_MAX_CNT_REG = maxIdxCntVal;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint16_t RSI_QEI_GetMaxIndex(volatile QEI_Type  *pstcQei)
 * @brief       Gets the maximum index value.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    returns the MAX index value programmed by user.
 *              \n
 *              \ref  QEI_INDEX_MAX_CNT_REG 
 */
STATIC INLINE uint16_t RSI_QEI_GetMaxIndex(volatile QEI_Type *pstcQei)
{
  return (uint16_t)(pstcQei->QEI_INDEX_MAX_CNT_REG);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE boolean_t RSI_QEI_GetDirection(volatile QEI_Type  *pstcQei) 
 * @brief	    Gets the direction of the QEI interfaced motor
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    returns 1: (+) Positive direction \ref QEI_STATUS_REG_b 
 *                      0: (-) Negative direction \ref QEI_STATUS_REG_b
 */
STATIC INLINE boolean_t RSI_QEI_GetDirection(volatile QEI_Type *pstcQei)
{
  // return direction
  return pstcQei->QEI_STATUS_REG_b.POSITION_CNTR_DIRECTION;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE int32_t RSI_QEI_GetStatus(volatile QEI_Type  *pstcQei)
 * @brief	    Gets the status of the QEI module
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    returns the status of the QEI. \ref QEI_STATUS_REG
 */
STATIC INLINE int32_t RSI_QEI_GetStatus(volatile QEI_Type *pstcQei)
{
  return (int32_t)(pstcQei->QEI_STATUS_REG);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_StartVelocityCounter(volatile QEI_Type  *pstcQei)
 * @brief       Starts the velocity counter.
 * @param[in]   pstcQei : Pointer to the QEI register instance.\ref QEI_CTRL_REG_SET 
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_StartVelocityCounter(volatile QEI_Type *pstcQei)
{
  // Enable velocity counter
  pstcQei->QEI_CTRL_REG_SET = BIT(11);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_StopVelocityCounter(volatile QEI_Type  *pstcQei)
 * @brief       stops the velocity counter.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_StopVelocityCounter(volatile QEI_Type *pstcQei)
{
  // Disable velocity counter
  pstcQei->QEI_CTRL_REG_RESET = BIT(11);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetDigitalFilterClkDiv(volatile QEI_Type  *pstcQei , uint32_t div)
 * @brief       Configures the digital filter clock division selects
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   div     : Digital filter clock division selects
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_SetDigitalFilterClkDiv(volatile QEI_Type *pstcQei, uint32_t div)
{
  pstcQei->QEI_CNTLR_INIT_REG_b.DF_CLK_DIVIDE_SLT = (unsigned int)(div & 0x0F);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetDigitalFilterClkDiv(volatile QEI_Type  *pstcQei) 
 * @brief	    Gets the digital filter clock division selects
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 		returns the digital filter clock division bits
 */
STATIC INLINE uint32_t RSI_QEI_GetDigitalFilterClkDiv(volatile QEI_Type *pstcQei)
{
  return pstcQei->QEI_CNTLR_INIT_REG_b.DF_CLK_DIVIDE_SLT;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void  RSI_QEI_ConfigureDeltaTimeAndFreq(volatile QEI_Type  *pstcQei, uint32_t freq, uint32_t PeriodInUs)
 * @brief       Configures the delta time and QEI module input frequency for velocity measurements
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   freq    : QEI module input frequency(typically soc clock) in hertz.
 * @param[in]   PeriodInUs    : Velocity measurement time in micro seconds (ex: for sec program 1000000)
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_ConfigureDeltaTimeAndFreq(volatile QEI_Type *pstcQei, uint32_t freq, uint32_t PeriodInUs)
{
  // Operating frequency
  pstcQei->QEI_CLK_FREQ_REG = ((freq / 1000000) - 1);
  // configure the clock divider value
  pstcQei->QEI_DELTA_TIME_REG = PeriodInUs;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void  RSI_QEI_SetModuleFreq(volatile QEI_Type  *pstcQei, uint32_t freq)
 * @brief       Sets the QEI module frequency for velocity measurements computations.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   freq    : QEI module input frequency(typically soc clock) in hertz.
 *              \ref  QEI_CLK_FREQ_REG
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_SetModuleFreq(volatile QEI_Type *pstcQei, uint32_t freq)
{
  // Operating frequency
  pstcQei->QEI_CLK_FREQ_REG = ((freq / 1000000) - 1);
}

/**
 * @fn          STATIC INLINE uint32_t  RSI_QEI_GetModuleFreq(volatile QEI_Type  *pstcQei)
 * @brief	Gets the QEI module frequency for velocity measurements computations.
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	returns the module frequency in hertz
 *              \ref QEI_CLK_FREQ_REG
 */
STATIC INLINE uint32_t RSI_QEI_GetModuleFreq(volatile QEI_Type *pstcQei)
{
  // Operating frequency
  return (((pstcQei->QEI_CLK_FREQ_REG) + 1) * 1000000);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void  RSI_QEI_SetDeltaTime(volatile QEI_Type  *pstcQei, uint32_t PeriodInUs)
 * @brief	    Gets the QEI module delta time for velocity computation
 * @param[in]   pstcQei : Pointer to the QEI register instance.\ref QEI_DELTA_TIME_REG 
 * @param[in]   PeriodInUs    : Velocity measurement time in micro seconds (ex: for sec program 1000000)
 * @return      none
 */
STATIC INLINE void RSI_QEI_SetDeltaTime(volatile QEI_Type *pstcQei, uint32_t PeriodInUs)
{
  // Operating frequency
  pstcQei->QEI_DELTA_TIME_REG = PeriodInUs;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t  RSI_QEI_GetDeltaTime(volatile QEI_Type  *pstcQei)
 * @brief       Gets the QEI module delta time configured for velocity computation
 * @param[in]   pstcQei : Pointer to the QEI register instance.\ref  QEI_DELTA_TIME_REG
 * @return 	    returns the configured delta time.
 */
STATIC INLINE uint32_t RSI_QEI_GetDeltaTime(volatile QEI_Type *pstcQei)
{
  // Operating frequency
  return pstcQei->QEI_DELTA_TIME_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetVelocity(volatile QEI_Type  *pstcQei)
 * @brief       Get the velocity of QEI module for programmed delta time
 * @param[in]   pstcQei : Pointer to the QEI register instance.\ref QEI_VELOCITY_REG
 * @return 	    number of position counts for configured delta time from start of velocity trigger.
 */
STATIC INLINE uint32_t RSI_QEI_GetVelocity(volatile QEI_Type *pstcQei)
{
  return pstcQei->QEI_VELOCITY_REG;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_IntrUnMask(volatile QEI_Type  *pstcQei, uint32_t intrMask)
 * @brief       Un masks the QEI interrupts
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   intrMask : OR'ed values of interrupt mask bits to be un masked
 * @return 	none
 */
STATIC INLINE void RSI_QEI_IntrUnMask(volatile QEI_Type *pstcQei, uint32_t intrMask)
{
  pstcQei->QEI_INTR_UNMASK_REg = intrMask;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_IntrMask(volatile QEI_Type  *pstcQei, uint32_t intrMask)
 * @brief	    Masks the QEI interrupts
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   intrMask : OR'ed values of interrupt mask bits to be masked
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_IntrMask(volatile QEI_Type *pstcQei, uint32_t intrMask)
{
  pstcQei->QEI_INTR_MASK_REG = intrMask;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetIntrStatus(volatile QEI_Type  *pstcQei)
 * @brief       Get the interrupt status of QEI module
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    returns the interrupt status bits
 */
STATIC INLINE uint32_t RSI_QEI_GetIntrStatus(volatile QEI_Type *pstcQei)
{
  return (pstcQei->QEI_INTR_STS_REG);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_ClrIntrStatus(volatile QEI_Type  *pstcQei, uint32_t intrMask) 
 * @brief	    Clears the QEI interrupt status
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @param[in]   intrMask : OR'ed values of interrupt mask bits to be cleared
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_ClrIntrStatus(volatile QEI_Type *pstcQei, uint32_t intrMask)
{
  pstcQei->QEI_INTR_ACK_REG = intrMask;
}

/*===================================================*/
/**
 * @fn          STATIC INLINE void RSI_QEI_SetMode(volatile QEI_Type  *pstcQei , uint8_t encMode)
 * @brief       Sets the QEI encoding modes (eg:1X , 2X , 4X)
 * @param[in]   pstcQei : Pointer to the QEI register instance.\ref QEI_CNTLR_INIT_REG_b
 * @param[in]   encMode : Encoding mode
 * @return 	    none
 */
STATIC INLINE void RSI_QEI_SetMode(volatile QEI_Type *pstcQei, uint8_t encMode)
{
  pstcQei->QEI_CNTLR_INIT_REG_b.QEI_ENCODING_MODE = (unsigned int)(encMode & 0x03);
}

/*===================================================*/
/**
 * @fn          STATIC INLINE uint32_t RSI_QEI_GetMode(volatile QEI_Type  *pstcQei)
 * @brief       Gets the QEI encoding modes (eg:1X , 2X , 4X)
 * @param[in]   pstcQei : Pointer to the QEI register instance
 * @return 	    Returns the Encoding mode
 */
STATIC INLINE uint32_t RSI_QEI_GetMode(volatile QEI_Type *pstcQei)
{
  return pstcQei->QEI_CNTLR_INIT_REG_b.QEI_ENCODING_MODE;
}

// Function prototypes
void RSI_QEI_Enable(volatile QEI_Type *pstcQei);
void RSI_QEI_Disable(volatile QEI_Type *pstcQei);

#ifdef __cplusplus
}
#endif

#endif // RSI_QEI_H
/** @} */
