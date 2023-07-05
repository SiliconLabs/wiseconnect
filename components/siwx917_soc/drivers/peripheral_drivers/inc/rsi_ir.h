/*******************************************************************************
* @file  rsi_ir.h
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

// Includes Files

#include "rsi_ccp_common.h"

#ifndef RSI_IR_H
#define RSI_IR_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_SREST_IR_CORE    (1U << 16) // Soft reset IR core block
#define CONFIG_EN_CONT_IR_DET   (1U << 8)  // Continuous IR detection enable
#define CONFIG_EN_CLK_IR_CORE   (1U << 2)  // Enable 32KHz clock to IR Core
#define CONFIG_EN_IR_DET_RSTART (1U << 1)  // Enable IR detection Re-start Logic
#define CONFIG_EN_IR_DET        (1U << 0)  // Enable IR detection Logic
#define MAX_MEMORY_ADDRESS      128
#define MAX_OFF_DURATION        131072
#define MAX_ON_DURATION         4096
#define MAX_FRAMEDONE_THRESHOLD 32768
#define MAX_DETECTION_THRESHOLD 128

/*===================================================*/
/**
 * @fn           error_t RSI_IR_OffDuration(IR_Type* pIr , uint32_t off_duration)
 * @brief        This API is used to configure the off duration of IR decoder
 * @param[in]    pIr  : IR type pointer
 * @param[in]    off_duration : IR Sleep duration timer value. Programmable value for OFF duration 
                                for power cycling on External IR Sensor.
                                Count to be programmed write to clock ticks of 32KHz clock.
						        Programmed value is (1/32K)*off_duration
 * @return       return zero \ref RSI_OK on success and return error code on failure.
 * @par          Example
                 RSI_IR_OffDuration(IR , 20); 
 *               \n in the above parameter we get off time of (1/32K)*20 = 0.625ms
 */
STATIC INLINE error_t RSI_IR_OffDuration(IR_Type *pIr, uint32_t off_duration)
{
  if (off_duration > MAX_OFF_DURATION) {
    return INVALID_PARAMETERS;
  }
  pIr->IR_OFF_TIME_DURATION_b.IR_OFF_TIME_DURATION = off_duration;
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn           error_t RSI_IR_OnDuration(IR_Type* pIr , uint16_t on_duration)
 * @brief        This API is used to configure the off duration of IR decoder
 * @param[in]    pIr  : IR type pointer
 * @param[in]    on_duration : IR Sleep duration timer value. Programmable value for ON duration 
 *                             for power cycling on External IR Sensor.
 *                             Count to be programmed write to clock ticks of 32KHz clock.
 *							   Programmed value is (1/32K)*on_duration
 * @return       return zero \ref RSI_OK on success and return error code on failure.
 * @par          Example      
 *               RSI_IR_OnDuration(IR , 20); 
 *               \n in the above parameter we get off time of (1/32K)*20 = 0.625ms
 */
STATIC INLINE error_t RSI_IR_OnDuration(IR_Type *pIr, uint16_t on_duration)
{
  if (on_duration > MAX_ON_DURATION) {
    return INVALID_PARAMETERS;
  }
  pIr->IR_ON_TIME_DURATION_b.IR_ON_TIME_DURATION = on_duration;
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn           void RSI_IR_SetConfiguration(IR_Type* pIr , uint32_t flags)
 * @brief        This API is used set the configure the IR modes 
 * @param[in]    pIr  : IR type pointer
 * @param[in]    flags        : Ored values of IR configuration flags
 *                             - \ref CONFIG_SREST_IR_CORE 
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 * @return       none
 * @par          Example
 *               RSI_IR_SetConfiguration(IR , (CONFIG_SREST_IR_CORE | CONFIG_EN_CONT_IR_DET)); 
 */
STATIC INLINE void RSI_IR_SetConfiguration(IR_Type *pIr, uint32_t flags)
{
  pIr->IR_CONFIG |= flags;
}

/*===================================================*/
/**
 * @fn           void RSI_IR_ClrConfiguration(IR_Type* pIr , uint32_t flags)
 * @brief        This API is used clear configure the IR modes 
 * @param[in]    pIr  : IR type pointer
 * @param[in]    flags        : Ored values of IR configuration flags
 *                             - \ref CONFIG_SREST_IR_CORE 
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 *                             - \ref CONFIG_EN_CONT_IR_DET
 * @return       none
 * @par          Example
 *               RSI_IR_SetConfiguration(IR , (CONFIG_SREST_IR_CORE | CONFIG_EN_CONT_IR_DET)); 
 */
STATIC INLINE void RSI_IR_ClrConfiguration(IR_Type *pIr, uint32_t flags)
{
  pIr->IR_CONFIG &= ~flags;
}

/*===================================================*/
/**
 * @fn           void RSI_IR_Restart(IR_Type* pIr)
 * @brief        This API is used clear configure the IR modes 
 * @param[in]    pIr  : IR type pointer
 * @return       none
 * @par          Example
 *               RSI_IR_SetConfiguration(IR);
 */
STATIC INLINE void RSI_IR_Restart(IR_Type *pIr)
{
  pIr->IR_CONFIG_b.IR_DET_RSTART = 1U;
}

/*===================================================*/
/**
 * @fn           error_t RSI_IR_Framedonethreshold(IR_Type* pIr,uint16_t frame_threshold)
 * @brief        This API is used count with respect to 32KHz clock after not more toggle are expected to a
 *               given pattern.
 * @param[in]    pIr  : IR type pointer
 * @param[in]    frame_threshold        : frame done threshold value.
 * @return       return zero \ref RSI_OK on success and return error code on failure.
 * @par          Example
 *               RSI_IR_Framedonethreshold(IR,20);
 */
STATIC INLINE error_t RSI_IR_Framedonethreshold(IR_Type *pIr, uint16_t frame_threshold)
{
  if (frame_threshold > MAX_FRAMEDONE_THRESHOLD) {
    return INVALID_PARAMETERS;
  }
  pIr->IR_FRAME_DONE_THRESHOLD_b.IR_FRAME_DONE_THRESHOLD = frame_threshold;
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn           error_t RSI_IR_Detectionthreshold(IR_Type* pIr,uint16_t detection_threshold)
 * @brief        This API is used minimum number of edges to detected during on-time failing which 
 *               IR detection is re-stated.
 * @param[in]    pIr  : IR type pointer
 * @param[in]    detection_threshold       : detection threshold value.
 * @return       return zero \ref RSI_OK on success and return error code on failure.
 * @par          Example
 *               RSI_IR_Detectionthreshold(IR,20);
 */
STATIC INLINE error_t RSI_IR_Detectionthreshold(IR_Type *pIr, uint16_t detection_threshold)
{
  if (detection_threshold > MAX_DETECTION_THRESHOLD) {
    return INVALID_PARAMETERS;
  }
  pIr->IR_DET_THRESHOLD_b.IR_DET_THRESHOLD = detection_threshold;
  return RSI_OK;
}

/*===================================================*/
/**
 * @fn           void RSI_IR_MemoryReadEnable(IR_Type* pIr)
 * @brief        This API is used enable the memory read option.
 * @param[in]    pIr  : IR type pointer
 * @return       return zero \ref RSI_OK on success and return error code on failure.
 * @par          Example
 *               RSI_IR_MemoryReadEnable(IR);
 */
STATIC INLINE void RSI_IR_MemoryReadEnable(IR_Type *pIr)
{
  pIr->IR_MEM_ADDR_ACCESS_b.IR_MEM_RD_EN = 1U;
}

/*===================================================*/
/**
 * @fn           uint32_t RSI_IR_GetMemoryDepth(IR_Type* pIr)
 * @brief        This API returns the IR data samples depth 
 * @param[in]    pIr  : IR type pointer
 * @return       number samples received
 * @par          Example
 *               memory_depth = RSI_IR_GetMemoryDepth(IR);
 */
STATIC INLINE uint32_t RSI_IR_GetMemoryDepth(IR_Type *pIr)
{
  return pIr->IR_MEM_READ_b.IR_DATA_MEM_DEPTH;
}

uint16_t RSI_IR_ReadData(IR_Type *pIr, uint16_t memory_address);
void RSI_IR_SoftwareRestart(IR_Type *pIr);
#ifdef __cplusplus
}
#endif

#endif //RSI_IR_H
