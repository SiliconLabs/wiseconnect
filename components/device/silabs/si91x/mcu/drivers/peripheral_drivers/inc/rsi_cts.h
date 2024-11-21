/******************************************************************************
* @file  rsi_cts.h
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

// Includes Files

#ifndef RSI_CTS_H
#define RSI_CTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ULPSS_CAP_SENSOR_IRQHandler IRQ006_Handler
#define ACK_FOR_WAKE_UP_INTERRUPT   BIT(8)

#define FIFO_EMPTY_STATUS    BIT(28)
#define RESET_WR_FIFO_STATUS BIT(14)
#define MIN_REF              0
#define MAX_REF              7
#define UPPER_LIMIT          0.75
#define LOWER_LIMIT          0.4

#define NO_OF_SLOW_SAMPLES     30
#define NO_OF_SLOW_AVG_SAMPLES 10

#define NO_OF_SAMPLES     10
#define NO_OF_AVG_SAMPLES 5
#define NO_OF_SENSORS     7
#define BUFFER_SIZE       NO_OF_SENSORS
#define FIFO_THRESHOLD    (NO_OF_SENSORS + 1)
#define LIMIT             4
#define AVG_MULTIPLIER    1.15

#define ON_TIME          256
#define OFF_TIME         4
#define SEED_VALUE       0x10241024
#define POLYNOMIAL       0x68110000
#define SAMPLING_PATTERN 0x1234578
#define WAKEUP_THRESHOLD 300
#define CONTINUOUS_MODE  1
#define ONEHOT_MODE      0
#define CTS_ON_TIME      3000
#define CTS_OFF_TIME     300000

#define SENSOR1 1
#define SENSOR2 2
#define SENSOR3 3
#define SENSOR4 4
#define SENSOR5 5
#define SENSOR6 6
#define SENSOR7 7
#define SENSOR8 8

#define SENSOR1_ENABLE 1
#define SENSOR2_ENABLE 1
#define SENSOR3_ENABLE 1
#define SENSOR4_ENABLE 1
#define SENSOR5_ENABLE 1
#define SENSOR6_ENABLE 0
#define SENSOR7_ENABLE 1
#define SENSOR8_ENABLE 1
/** @addtogroup SOC21
* @{
*/
/*===================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_BypassPRS(CTS_Type *cts,boolean_t enable)
 * @brief       This API is used Bypass the Random number generator output to the Non-overlapping stream generator
 * @param[in]   cts         :pointer to cts config structure
 * @param[in]   enable      
 *                           - 1: bypass the random no generator
 *                           - 0: use random no generator output
 * @return      none               
 */
STATIC INLINE void RSI_CTS_BypassPRS(CTS_Type *cts, boolean_t enable)
{
  (void)cts;
  CTS->CTS_CONFIG_REG_1_1_b.BYPASS = (unsigned int)(enable & 0x01);
}

/*===================================================*/
/**   
 * @fn          STATIC INLINEvoid RSI_CTS_ThresholdProgam(CTS_Type *cts,uint8_t threshold)
 * @brief       This API is used for threshold programming
 * @param[in]   cts           :pointer to cts config structure
 * @param[in]   threshold     :Threshold for fifo aempty            
 * @return      none              
 */
STATIC INLINE void RSI_CTS_ThresholdProgam(CTS_Type *cts, uint8_t threshold)
{
  cts->CTS_CONFIG_REG_0_0_b.FIFO_AEMPTY_THRLD = (unsigned int)(threshold & 0x3F); // Threshold for fifo aempty
}

/*===================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_StaticClkEnable(CTS_Type *cts,boolean_t enable)
 * @brief       This API is used for static clk gating 
 * @param[in]   cts      :pointer to cts config structure
 * @param[in]   enable   :write 0 for static clk gate else 1              
 * @return      none              
 */
STATIC INLINE void RSI_CTS_StaticClkEnable(CTS_Type *cts, boolean_t enable)
{
  cts->CTS_CONFIG_REG_0_0_b.CTS_STATIC_CLK_EN = (unsigned int)(enable & 0x01); // clk are not gated
}

/*===================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_ConfigSampleMode(CTS_Type *cts,boolean_t avg_enable)
 * @brief       This API is used for select averaging mode to apply samples
 * @param[in]   cts           :pointer to cts config structure
 * @param[in]   avg_enable    : 
 *                             - 1: for selecting averaging of samples
 *                             - 0: for directly apply samples
 * @return      none             
 */
STATIC INLINE void RSI_CTS_ConfigSampleMode(CTS_Type *cts, boolean_t avg_enable)
{
  cts->CTS_CONFIG_REG_1_1_b.SAMPLE_MODE = (unsigned int)(avg_enable & 0x03); // selects Averaging
}

/*============================================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_ModeSelect(CTS_Type *cts,boolean_t mode)
 * @brief       This API is used for scanning mode selection 
 * @param[in]   cts    :pointer to cts config structure
 * @param[in]   mode   : 
 *                      - 1: for continious mode select
 *                      - 0: for one hot mode
 * @return      none               
 */
STATIC INLINE void RSI_CTS_ModeSelect(CTS_Type *cts, boolean_t mode)
{
  cts->CTS_CONFIG_REG_1_1_b.CNT_ONEHOT_MODE = (unsigned int)(mode & 0x01);
}

/*============================================================================*/
/**   
 * @fn          STATIC INLINEvoid RSI_CTS_Enable(CTS_Type *cts,boolean_t enable)
 * @brief       This API is used enable/disable the cts
 * @param[in]   cts         :pointer to cts config structure
 * @param[in]   enable      : 
 *                           - 1: enable the CTS
 *                           - 0: disable the cts
 * @return      none              
 */
STATIC INLINE void RSI_CTS_Enable(CTS_Type *cts, boolean_t enable)
{
  cts->CTS_CONFIG_REG_1_1_b.ENABLE1 = (unsigned int)(enable & 0x01); // enable the CTS
}

/*============================================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_BufferDelay(CTS_Type *cts,uint8_t delay)
 * @brief       This API is used set the buffer delay
 * @param[in]   cts         :pointer to cts config structure
 * @param[in]   delay     : delay time (max value is 31)                    
 * @return      none              
 */
STATIC INLINE void RSI_CTS_BufferDelay(CTS_Type *cts, uint8_t delay)
{
  cts->CTS_CONFIG_REG_1_1_b.BUFFER_DELAY = (unsigned int)(delay & 0x1F); // set the buffer delay
}

/*============================================================================*/
/**   
 * @fn          STATIC INLINE void RSI_CTS_FifoInterruptEnable(CTS_Type *cts,boolean_t enable)
 * @brief       This API is used to mask the fifo interrupt
 * @param[in]   cts        :pointer to cts config structure
 * @param[in]   enable     : 
 *                           - 1: fifo afull interrupt will mask
 *                           - 0: unmask
 * @return      none              
 */
STATIC INLINE void RSI_CTS_FifoInterruptEnable(CTS_Type *cts, boolean_t enable)
{
#if defined(SLI_SI917) || defined(SLI_SI915)
  if (enable)
    cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG |= BIT(1); // mask/Uunmask the interrupt
  else
    cts->CTS_CONFIG_REG_1_7_b.REF_VOLT_CONFIG &= (unsigned int)(~BIT(1) & 0x01FF);
#else
  cts->CTS_CONFIG_REG_1_7_b.MASK_FIFO_AFULL_INTR = enable; // mask/Uunmask the interrupt
#endif
}

void RSI_CTS_PinMuxing(void);

void Aux_VddPowerOn(void);

void RSI_CTS_ClkSelection(CTS_Type *cts,
                          uint8_t clk_sel_1,
                          uint8_t clk_sel_2,
                          uint8_t clk_divider_1,
                          uint8_t clk_divider_2);

uint32_t RSI_CTS_GetStatus(CTS_Type *cts);

void RSI_CTS_ConfigPolynomial(CTS_Type *cts, boolean_t enable, uint8_t poly_length, uint32_t seed, uint32_t polynomial);

void RSI_CTS_ConfigOnOffTime(CTS_Type *cts, uint16_t on_time, uint16_t off_time);

void RSI_CTS_ConfigSampling(CTS_Type *cts, uint16_t dealy, uint16_t repetitions);

void RSI_CTS_ConfigSamplingPattern(CTS_Type *cts, uint32_t pattern, uint32_t valid_sensor);

void RSI_CTS_ConfigRefVoltage(CTS_Type *cts,
                              int16_t ref_voltage,
                              boolean_t enable,
                              uint16_t *samples,
                              uint16_t *avg_samples);

void RSI_CTS_ConfigWakeUp(CTS_Type *cts, uint8_t mode, uint16_t threshold);

uint32_t RSI_CTS_ReadRandomData(CTS_Type *cts);

uint32_t RSI_CTS_ReadFifo(CTS_Type *cts);

void RSI_CTS_IntrClear(CTS_Type *cts);

void RSI_CTS_Calibration(uint16_t avg, uint16_t *samples, uint16_t *avg_samples);

void RSI_CTS_SlowCalibration(uint16_t avg, uint16_t *samples, uint16_t *avg_samples);

void RSI_CTS_TouchDetection(uint16_t *fifo_read, uint8_t *sensor_count);
void ULPSS_CAP_SENSOR_IRQHandler(void);
void RSI_CTS_InputSelection(CTS_Type *cts, uint32_t cap_input, uint32_t res_input);
void RSI_SlowCalibration();
void RSI_CTS();
void UDMA_Read();
void udmaTransferComplete(uint32_t event, uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif // RSI_CTS_H
/** @} */
