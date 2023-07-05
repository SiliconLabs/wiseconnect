/*******************************************************************************
* @file  rsi_fim.c
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

// Include files

#include "rsi_chip.h"
#ifdef ENHANCED_FIM
#include "rsi_enhanced_fim.h"
#endif

#ifdef ENHANCED_FIM
/*==============================================*/
/**
 *@fn           void RSI_FIM_EnableSaturation(void)
 *@brief        This API  is used to enable saturation .
 *@return       none
 */
void RSI_FIM_EnableSaturation(void)
{
  FIM->FIM_SAT_SHIFT_b.SAT_EN = ENABLE;
}
#endif
/*==============================================*/
/**
 *@fn          static void RSI_FIM_InputData(void *pSrcA,uint32_t bank,uint32_t blockSize,uint8_t data_type) 
 *@brief       This API is used to store data in ulp memories
 *@param[in]   :pSrcA is input vector
 *@param[in]   :bank is in which memory data samples are stored
 *@param[in]   :data_type specifies q7,q15 , q31 formats
 *             \ref FORMAT_F32
 *             \ref FORMAT_Q31 
 *             \ref FORMAT_Q7
 *             \ref FORMAT_Q15
 *@param[in]   :blockSize is size of the input array
 *@return     none
*/
void RSI_FIM_InputData(void *pSrcA, uint32_t bank, uint32_t blockSize, uint8_t data_type)
{
  uint32_t var;
  q31_t *pSrcA32;
  q15_t *pSrcA16;
  q7_t *pSrcA8;

  switch (data_type) {
    case FORMAT_F32:
    case FORMAT_Q31:
      pSrcA32 = (q31_t *)pSrcA;
      for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
        *(volatile uint32_t *)(bank + (var * 4)) = *pSrcA32;
#else
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA32;
#endif
        pSrcA32++;
      }
      break;

    case FORMAT_Q7:
      pSrcA8 = (q7_t *)pSrcA;
      for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
        *(volatile uint32_t *)(bank + (var * 4)) = *pSrcA8;
#else
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA8;
#endif
        pSrcA8++;
      }
      break;

    case FORMAT_Q15:
      pSrcA16 = (q15_t *)pSrcA;

      for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
        *(volatile uint32_t *)(bank + (var * 4)) = *pSrcA16;
#else
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA16;
#endif
        pSrcA16++;
      }
      break;
  }
}
/*==============================================*/
/**
 *@fn          static void RSI_FIM_InputDataCmplx(volatile q15_t *pReal,uint32_t bank,volatile uint32_t var,uint8_t flag)
 *@brief       This API is used to store data in ulp memories
 *@param[in]   :pReal is input vector
 *@param[in]   :bank is in which memory data samples are stored
 *@param[in]   :flag is set depending on complex-real and real-complex values
 *@param[in]   :var is for address increment
**/
static void RSI_FIM_InputDataCmplx(volatile q15_t *pReal, uint32_t bank, volatile uint32_t var, uint8_t flag)
{

  if (flag) {
#ifdef ENHANCED_FIM
    *(volatile uint16_t *)((bank) + ((var + 1) * 2)) = *pReal;
#else
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + ((var + 1) * 2)) = *pReal;
#endif
    pReal++;
#ifdef ENHANCED_FIM
    *(volatile uint16_t *)((bank) + (var)*2) = *pReal;
#else
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + (var)*2) = *pReal;
#endif
    pReal++;
  } else {
#ifdef ENHANCED_FIM
    *(volatile uint16_t *)((bank) + ((var + 1) * 2)) = *pReal;
    *(volatile uint16_t *)((bank) + (var)*2)         = 0x0000;
#else
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + ((var + 1) * 2)) = *pReal;
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + (var)*2) = 0x0000;
#endif
  }
}
/*==============================================*/
/**
 *@fn         static void RSI_FIM_ComplxOutputConvert(uint32_t blockSize, uint32_t bank,volatile q15_t *pDst)
 *@brief      This API is used to set the FIM to convert the complex output
 *@param[in]  :bank is output bank address
 *@param[out] :pDst is required output array
 *@param[in]  :blockSize is size of the input array
 *@return     :none
 */
static void RSI_FIM_ComplxOutputConvert(uint32_t blockSize, uint32_t bank, volatile q15_t *pDst)
{
  volatile uint32_t i, a;

  for (i = 0; i < blockSize; i++) {
    a = i * 2;
#ifdef ENHANCED_FIM
    *pDst = *(volatile uint16_t *)((bank) + ((a + 1)) * 2);
#else
    *pDst = *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + ((a + 1)) * 2);
#endif
    pDst++;
#ifdef ENHANCED_FIM
    *pDst = *(volatile uint16_t *)((bank) + (a)*2);
#else
    *pDst = *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + (a)*2);
#endif
    *pDst++;
  }
}
/*==============================================*/
/**
 *@fn         void rsi_fim_read_data(uint32_t bank, uint32_t blockSize, volatile void *pDst, uint8_t data_type, typ_data_t type_data)
 *@brief      This API is used to set the FIM to read the output
 *@param[in]  bank : is output bank address
 *@param[out] pDst : is required output array
 *@param[in]  data_type : specifies q7,q15 , q31 formats
 *@param[in]  type_data : is to specify real-complex , complex-real or complex-complex data
 *@param[in]  length : is size of the input array
 *@return     none
 */

void rsi_fim_read_data(uint32_t bank, uint32_t blockSize, volatile void *pDst, uint8_t data_type, typ_data_t type_data)
{
  volatile uint32_t var;

  if ((data_type == FORMAT_F32) || (data_type == FORMAT_Q31)) {
    for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
      ((q31_t *)pDst)[var] = *(volatile uint32_t *)(bank + (var * 4));
#else
      ((q31_t *)pDst)[var] = *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4));
#endif
    }
  }

  if (data_type == FORMAT_Q7) {
    for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
      ((q7_t *)pDst)[var] = *(volatile uint32_t *)(bank + (var * 4));
#else
      ((q7_t *)pDst)[var] = *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4));
#endif
    }
  }
  if (data_type == FORMAT_Q15) {
    if (type_data) {
      RSI_FIM_ComplxOutputConvert(blockSize, bank, (q15_t *)pDst);
    } else {
      for (var = 0; var < blockSize; var++) {
#ifdef ENHANCED_FIM
        ((q15_t *)pDst)[var] = *(volatile uint32_t *)(bank) + (var * 4);
#else
        ((q15_t *)pDst)[var] = *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4));
#endif
      }
    }
  }
}

/*==============================================*/
/**
 *@fn         static void RSI_FIM_SetDataLen(uint32_t ColAorfilterLen,uint32_t bufferLen1,uint32_t bufferLen2)
 *@brief      RSI_FIM_SetDataLen API is used to set data blockSize of buffers in the FIM module
 *@param[in]  :ColAorfilterLen is used for configuring number of columns of first matrix or
 *            \n filter coefficients for fir , iir and fir interpolator filters.
 *@param[in]  :bufferLen1 is the blockSize of data used for input1 
 *             \n for scalar only bufferLen1 need to be configured.
 *             \n for filter
 *@param[in]  :bufferLen2 is the blockSize of data used for input2 i.e. only for vector operations this is used.
  *@return     none
 *
 */
static void RSI_FIM_SetDataLen(uint32_t ColAorfilterLen, uint32_t bufferLen1, uint32_t bufferLen2)
{
  FIM->FIM_CONFIG_REG1_b.INP1_LEN = bufferLen1;
  FIM->FIM_CONFIG_REG1_b.INP2_LEN = bufferLen2;
  FIM->FIM_CONFIG_REG1_b.MAT_LEN  = ColAorfilterLen;
}

/*==============================================*/
/**
 *@fn         static void RSI_FIM_SetSatTruncRound(uint32_t SatTruncRoundShift)
 *@brief      RSI_FIM_SetSatTruncRound API is used to set sat trunc values to FIM module for corresponding inputs 
 *@param[in]  :SatTruncRoundShift is saturate truncation and round value that need to be to get appropriate output
 *            saturate - to confine msb to limited value
 *            truncate - to confine lsb to limited value
 *
 *            round    - approximating to near value
 *@return     none
 */
void RSI_FIM_SetSatTruncRound(uint32_t SatTruncRoundShift)
{
  // Shift truncate saturate and round
  FIM->FIM_SAT_SHIFT |= SatTruncRoundShift;
}

/*==============================================*/
/**
 *@fn           :static void RSI_FIM_DatTypTrig(uint32_t numRow1, uint32_t numCol2, typ_data_t typData)
 *@brief        :RSI_FIM_DatTypTrig API is used to trigger the FIM module
 *@param[in]    :numRow1 is number of rows of first matrix 
 *@param[in]    :numCol2 is number of columns of second matrix 
 *@param[in]    :typData is to select which type of data is given as input i.e. real-real , real-complex , complex-real and complex-complex
 *               In case of vectors, for real-complx, data in second memory is considered as real and 1st as cmplx.
 *                                for complx-real, data in 1st memory is considered as real and second as cmplx.
 *               In case of scalar, for real-complx, data in memory is taken as complex and scalar as real.
 *@return      none
*/
static void RSI_FIM_DatTypTrig(uint32_t numRow1, uint32_t numCol2, typ_data_t typData)
{
  uint32_t x;
  x = 0x0FC00000 & FIM->FIM_CONFIG_REG2;
  switch (typData) {
    case ULP_FIM_COP_DATA_REAL_REAL:
      FIM->FIM_CONFIG_REG2 = x | (numRow1 << 16) | (numCol2 << 10) | typData << 8 | ULP_FIM_COP_START_TRIG;
      break;
    case ULP_FIM_COP_DATA_CPLX_REAL:
      FIM->FIM_CONFIG_REG2 = x | (numRow1 << 16) | (numCol2 << 10) | typData << 8 | ULP_FIM_COP_START_TRIG;
      break;
    case ULP_FIM_COP_DATA_REAL_CPLX:
      FIM->FIM_CONFIG_REG2 = x | (numRow1 << 16) | (numCol2 << 10) | typData << 8 | ULP_FIM_COP_START_TRIG;
      break;
    case ULP_FIM_COP_DATA_CPLX_CPLX:
      FIM->FIM_CONFIG_REG2 = x | (numRow1 << 16) | (numCol2 << 10) | typData << 8 | ULP_FIM_COP_START_TRIG;
      break;
  }
}

/*==============================================*/
/**
 *@fn        void rsi_arm_offset_f32_opt(int32_t *pSrc,
                            int32_t scale,
                            int32_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication for real data
 *@param[in]  *pSrc : points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be addedd for each elements of vector array.
 *@param[in]  length : is size of the input array
 *@return      none
 *
 */
void rsi_arm_offset_f32_opt(int32_t *pSrc,
                            int32_t scale,
                            int32_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_offset_q7_opt(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication for real data
 *@param[in]  *pSrc : points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be addedd for each elements of vector array.
 *@param[in]  length : is size of the input array
 *@return      none
 *
 */
void rsi_arm_offset_q7_opt(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}
/*==============================================*/

/**
 *@fn         void rsi_arm_offset_q15_opt(q15_t *pSrc,
 *                           q15_t scale,
 *                           q15_t *pDst,
 *                           uint32_t blockSize,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication for real data
 *@param[in]  *pSrc : points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be addedd for each elements of vector array.
 *@param[in]  length : is size of the input array
 *@return      none
 *
 */

void rsi_arm_offset_q15_opt(q15_t *pSrc,
                            q15_t scale,
                            q15_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_offset_q31_opt(q31_t *pSrc,
 *                           q31_t scale,
 *                           q31_t *pDst,
 *                           uint32_t blockSize,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication for real data
 *@param[in]  *pSrc : points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be addedd for each elements of vector array.
 *@param[in]  length : is size of the input array
 *return      none
 *
 */
void rsi_arm_offset_q31_opt(q31_t *pSrc,
                            q31_t scale,
                            q31_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_scalar_sub_f32(int32_t *pSrc,
 *                          int32_t scale,
 *                           int32_t *pDst,
 *                           uint32_t blockSize,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Subtraction
 *@param[in]  *pSrc :points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be subtracted from each elements of vector array.
 *@param[in]  length :is size of the input array
 *@return       none
 */
void rsi_fim_scalar_sub_f32(int32_t *pSrc,
                            int32_t scale,
                            int32_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/

/**
 *@fn         void rsi_fim_scalar_sub_q7(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Subtraction
 *@param[in]  *pSrc :points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be subtracted from each elements of vector array.
 *@param[in]  length :is size of the input array
 *@return       none
 */

void rsi_fim_scalar_sub_q7(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_scalar_sub_q31(q31_t *pSrc,
 *                           q31_t scale,
 *                           q31_t *pDst,
 *                           uint32_t blockSize,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Subtraction
 *@param[in]  *pSrc :points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be subtracted from each elements of vector array.
 *@param[in]  length :is size of the input array
 *@return       none
 */
void rsi_fim_scalar_sub_q31(q31_t *pSrc,
                            q31_t scale,
                            q31_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_scale_f32_opt(int32_t *pSrc,
 *                         int32_t scale,
 *                          int32_t *pDst,
 *                          uint32_t blockSize,
 *                          uint16_t inBank,
 *                          uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication for real data
 *@param[in]  *pSrc : points to input vector
 *@param[out] *pDst : points to output vector
 *@param[in]  scale : is constant value that need to be multiplied for each elements of vector array.
 *@param[in]  length : is size of the input array
 *@return      none
 *
 */
void rsi_arm_scale_f32_opt(int32_t *pSrc,
                           int32_t scale,
                           int32_t *pDst,
                           uint32_t blockSize,
                           uint16_t inBank,
                           uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scale;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_scale_q7_opt(q7_t *pSrc,
 *                        q7_t scaleFract,
 *                         int8_t shift,
 *                         q7_t *pDst,
 *                         uint32_t blockSize,
 *                         uint16_t inBank,
 *                         uint16_t outBank) 
 *@brief      This API is used to set the FIM Scalar Multiplication
 *@param[in]  pSrc : is input vector A
 *@param[in]  scale : is constant value that need to be multiplied for each elements of vector array.
 *@param[in]  blockSize : is size of the input array
 *@param[in]  typ_data : is to select which type of data is given as input
 *            \n i.e. real complex , complex real and complex complex
 *            \n real complex   real vector, complex  scalar
 *            \n complex real   real scalar, complex vector
 *@return      none
 */
void rsi_arm_scale_q7_opt(q7_t *pSrc,
                          q7_t scaleFract,
                          int8_t shift,
                          q7_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank,
                          uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(shift + 0x19);
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scaleFract;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_scale_q15_opt(q15_t *pSrc,
 *                          q15_t scaleFract,
 *                          int8_t shift,
 *                          q15_t *pDst,
 *                          uint32_t blockSize,
 *                          uint16_t inBank,
 *                          uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication
 *@param[in]  pSrc : is input vector A
 *@param[in]  scale : is constant value that need to be multiplied for each elements of vector array.
 *@param[in]  blockSize : is size of the input array
 *@param[in]  typ_data : is to select which type of data is given as input
 *            \n i.e. real complex , complex real and complex complex
 *            \n real complex   real vector, complex  scalar
 *            \n complex real   real scalar, complex vector
 *@return      none
 */
void rsi_arm_scale_q15_opt(q15_t *pSrc,
                           q15_t scaleFract,
                           int8_t shift,
                           q15_t *pDst,
                           uint32_t blockSize,
                           uint16_t inBank,
                           uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(shift + 0x11);
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scaleFract;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_scale_q31_opt(q31_t *pSrc,
 *                          q31_t scaleFract,
 *                          int8_t shift,
 *                          q31_t *pDst,
 *                          uint32_t blockSize,
 *                          uint16_t inBank,
 *                          uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication
 *@param[in]  pSrc : is input vector A
 *@param[in]  scale : is constant value that need to be multiplied for each elements of vector array.
 *@param[in]  blockSize : is size of the input array
 *@param[in]  typ_data : is to select which type of data is given as input
 *            \n i.e. real complex , complex real and complex complex
 *            \n real complex   real vector, complex  scalar
 *            \n complex real   real scalar, complex vector
 *@return      none
 */

void rsi_arm_scale_q31_opt(q31_t *pSrc,
                           q31_t scaleFract,
                           int8_t shift,
                           q31_t *pDst,
                           uint32_t blockSize,
                           uint16_t inBank,
                           uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, 0);
  RSI_FIM_SetSatTruncRound(shift + 0x1);
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = scaleFract;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_scalar_mul_q15(q15_t *pSrc,
 *                           q15_t *scale,
 *                           q15_t *pDst,
 *                           uint32_t blockSize,
 *                           typ_data_t typ_data,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Scalar Multiplication
 *@param[in]  pSrc : is input vector A
 *@param[in]  scale : is constant value that need to be multiplied for each elements of vector array.
 *@param[in]  blockSize : is size of the input array
 *@param[in]  typ_data : is to select which type of data is given as input
 *            \n i.e. real complex , complex real and complex complex
 *            \n real complex   real vector, complex  scalar
 *            \n complex real   real scalar, complex vector
 *@return      none
 */
void rsi_fim_scalar_mul_q15(q15_t *pSrc,
                            q15_t *scale,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank)
{
  uint32_t i;
  q15_t scalarReal;
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize / 2, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_CPLX_REAL:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
        RSI_FIM_InputDataCmplx(pSrc, ULPSS_RAM_ADDR_SRC, i * 2, 1);
#endif
#else
        // For 9116
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
#endif
        pSrc++;
        pSrc++;
      }
      scalarReal                                     = *scale;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | 0x0000;
      break;

    case ULP_FIM_COP_DATA_REAL_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pSrc, ULPSS_RAM_ADDR_SRC, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 0);
#endif
        pSrc++;
      }
      scalarReal = *scale;
      scale++;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | *scale;
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pSrc, ULPSS_RAM_ADDR_SRC, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
#endif
        pSrc++;
        pSrc++;
      }
      scalarReal = *scale;
      scale++;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | *scale;

      break;

    default:
      break;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, typ_data);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_add_f32_opt(int32_t *pSrcA,
 *                       int32_t *pSrcB,
 *                        int32_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Addition for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return      none
 */
void rsi_arm_add_f32_opt(int32_t *pSrcA,
                         int32_t *pSrcB,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_add_q7_opt(q7_t *pSrcA,
 *                       q7_t *pSrcB,
 *                       q7_t *pDst,
 *                       uint32_t blockSize,
 *                       uint16_t inBank1,
 *                       uint16_t inBank2,
 *                       uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Addition for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return      none
 */
void rsi_arm_add_q7_opt(q7_t *pSrcA,
                        q7_t *pSrcB,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_add_q15_opt(q15_t *pSrcA,
 *                        q15_t *pSrcB,
 *                        q15_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Addition for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return      none
 */
void rsi_arm_add_q15_opt(q15_t *pSrcA,
                         q15_t *pSrcB,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_add_q31_opt(q31_t *pSrcA,
 *                        q31_t *pSrcB,
 *                        q31_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Addition for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return      none
 */
void rsi_arm_add_q31_opt(q31_t *pSrcA,
                         q31_t *pSrcB,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_vector_add_q15(q15_t *pIn1,
 *                          q15_t *pIn2,
 *                           q15_t *pDst,
 *                           uint32_t blockSize,
 *                           typ_data_t typ_data,
 *                           uint16_t inBank1,
 *                           uint16_t inBank2,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Addition
 *@param[in]  pIn1 : points to input vector A
 *@param[in]  pIn2 : points to input vector B
 *@param[in]  blockSize : is size of the input array
 *@param[out] pDst :  points to output vector
 *@param[in]  typ_data : is to select which type of data is given as input
 *            \n i.e. real complex , complex real and complex complex
 *            \n real complex   1st vector is real vector i.e pIn1, 2nd vector is complex i.e pIn2
 *            \n complex real   1st vector is complex vector i.e pIn1, 2nd vector is real i.e pIn2
 *@return     none 
 */
void rsi_fim_vector_add_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize / 2, blockSize / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_CPLX_REAL:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 0);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 0);
#endif
        pIn2++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 1);
#endif
        pIn1++;
        pIn1++;
      }
      break;

    case ULP_FIM_COP_DATA_REAL_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 0);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 0);
#endif
        pIn1++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 1);
#endif
        pIn2++;
        pIn2++;
      }
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 1);
#endif
        pIn1++;
        pIn1++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC2, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 1);
#endif
        pIn2++;
        pIn2++;
      }

      break;

    default:
      break;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pIn1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pIn2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((ADD_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, typ_data);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_sub_f32_opt(int32_t *pSrcA,
 *                        int32_t *pSrcB,
 *                        int32_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Subtraction for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return     none
 *
 */
void rsi_arm_sub_f32_opt(int32_t *pSrcA,
                         int32_t *pSrcB,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_sub_q7_opt(q7_t *pSrcA,
 *                       q7_t *pSrcB,
 *                       q7_t *pDst,
 *                       uint32_t blockSize,
 *                       uint16_t inBank1,
 *                       uint16_t inBank2,
 *                       uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Subtraction for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return     none
 */
void rsi_arm_sub_q7_opt(q7_t *pSrcA,
                        q7_t *pSrcB,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_sub_q15_opt(q15_t *pSrcA,
 *                        q15_t *pSrcB,
 *                        q15_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Subtraction for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return     none
 *
 */
void rsi_arm_sub_q15_opt(q15_t *pSrcA,
                         q15_t *pSrcB,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_sub_q31_opt(q31_t *pSrcA,
 *                        q31_t *pSrcB,
 *                        q31_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Subtraction for real data
 *@param[in]  pSrcA : points to input vector A
 *@param[in]  pSrcB : points to input vector B
 *@param[out] pDst :  points to output vector
 *@param[in]  blockSize : is size of the input array
 *@return      none
 */
void rsi_arm_sub_q31_opt(q31_t *pSrcA,
                         q31_t *pSrcB,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_vector_sub_q15(q15_t *pIn1,
 *                           q15_t *pIn2,
 *                           q15_t *pDst,
 *                           uint32_t blockSize,
 *                           typ_data_t typ_data,
 *                           uint16_t inBank1,
 *                           uint16_t inBank2,
 *                           uint16_t outBank)
 *@brief      This API is used to set the FIM Vector Subtraction
 *@param[in]  pIn1 : is input vector A
 *@param[in]  pIn2 : is input vector B
 *@param[out] pDst : is required output array
 *@param[in]  typ_data : is to specify real-complex , complex-real or complex-complex data
 *@param[in]  blockSize : is size of the input array
 *@return     none
 */
void rsi_fim_vector_sub_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize / 2, blockSize / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_CPLX_REAL:

      for (i = 0; i < (blockSize / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 0);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 0);
#endif
        pIn2++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 1);
#endif
        pIn1++;
        pIn1++;
      }
      break;

    case ULP_FIM_COP_DATA_REAL_CPLX:
      for (i = 0; i < (blockSize / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 0);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 0);
#endif
        pIn1++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 1);
#endif
        pIn2++;
        pIn2++;
      }
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 1);
#endif
#else
        RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 1);
#endif
        pIn1++;
        pIn1++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
        RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 1);
#endif
        RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 1);
#endif
        pIn2++;
        pIn2++;
      }

      break;

    default:
      break;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pIn1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pIn2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((SUB_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, typ_data);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_mult_f32_opt(int32_t *pIn1,
 *                        int32_t *pIn2,
 *                         int32_t *pDst,
 *                         uint32_t SatTruncRound,
 *                         uint32_t blockSize,
 *                         uint16_t inBank1,
 *                         uint16_t inBank2,
 *                         uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Multiplication for real data
 *@param[in]  pIn1 : is input vector A
 *@param[in]  pIn2 : is input vector B
 *@param[in]  length : is size of the input array
 *@param[in]  SatTruncRound : is used to limit the output as required
 *@return     none
 */
void rsi_arm_mult_f32_opt(int32_t *pIn1,
                          int32_t *pIn2,
                          int32_t *pDst,
                          uint32_t SatTruncRound,
                          uint32_t blockSize,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(SatTruncRound);
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pIn1, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pIn2, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pIn1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pIn2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pIn1, inBank1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(pIn2, inBank2, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn           void rsi_arm_mult_q7_opt(q7_t *pSrcA,
 *                        q7_t *pSrcB,
 *                        q7_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief        This API is used for Q7 vector multiplication
 *@param[in]    *pSrcA : points to the first input vector
 *@param[in]    *pSrcB : points to the second input vector
 *@param[out]   *pDst : points to the output vector
 *@param[in]    blockSize : number of samples in each vector
 *@return       none
 */
void rsi_arm_mult_q7_opt(q7_t *pSrcA,
                         q7_t *pSrcB,
                         q7_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x19, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q7);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          void rsi_arm_mult_q15_opt(q15_t *pSrcA,
 *                         q15_t *pSrcB,
 *                         q15_t *pDst,
 *                         uint32_t blockSize,
 *                         uint16_t inBank1,
 *                         uint16_t inBank2,
 *                         uint16_t outBank)
 *@brief       This API is used for Q15 vector multiplication
 *@param[in]   *pSrcA : points to the first input vector
 *@param[in]   *pSrcB : points to the second input vector
 *@param[out]  *pDst : points to the output vector
 *@param[in]   blockSize : number of samples in each vector
 *@return      none
 */

void rsi_arm_mult_q15_opt(q15_t *pSrcA,
                          q15_t *pSrcB,
                          q15_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          void rsi_arm_mult_q31_opt(q31_t *pSrcA,
 *                        q31_t *pSrcB,
 *                         q31_t *pDst,
 *                         uint32_t blockSize,
 *                         uint16_t inBank1,
 *                         uint16_t inBank2,
 *                         uint16_t outBank)
 *@brief       This API is used for Vector Multiplication for complex-real data
 *@param[in]   pIn1 : is input vector A
 *@param[in]   pIn2 : is input vector B
 *@param[out]  *pDst : points to the output vector
 *@param[in]   blockSize : is size of the input array
 *@return       none
 */
void rsi_arm_mult_q31_opt(q31_t *pSrcA,
                          q31_t *pSrcB,
                          q31_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrcA, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrcA, inBank1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(pSrcB, inBank2, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_cmplx_mult_real_q15_opt(q15_t *pSrcCmplx,
 *                                    q15_t *pSrcReal,
 *                                    q15_t *pDst,
 *                                    uint32_t numSamples,
 *                                    uint16_t inBank1,
 *                                    uint16_t inBank2,
 *                                    uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Multiplication for complex-real data
 *@param[in]  pSrcCmplx : is input vector A
 *@param[in]  pSrcReal : is input vector B
 *@param     *pDst : points to the real output vector
 *@param     numSamples : number of complex samples in the input vector
 *@return    none
 */
void rsi_fim_vector_mul_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize / 2, blockSize / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
  for (i = 0; i < (blockSize / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pIn2, ULPSS_RAM_ADDR_SRC2, i * 2, 1);
#endif
#else
    RSI_FIM_InputDataCmplx(pIn2, inBank2, i * 2, 1);
#endif
    pIn2++;
    pIn2++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pIn1, ULPSS_RAM_ADDR_SRC1, i * 2, 0);
#endif
#else
    RSI_FIM_InputDataCmplx(pIn1, inBank1, i * 2, 0);
#endif
    pIn1++;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pIn2;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pIn2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_CPLX);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_cmplx_mult_real_q15_opt(q15_t *pSrcCmplx,
 *                                    q15_t *pSrcReal,
 *                                    q15_t *pDst,
 *                                    uint32_t numSamples,
 *                                    uint16_t inBank1,
 *                                    uint16_t inBank2,
 *                                    uint16_t outBank)
 *@brief     This API is used to set the FIM Vector Multiplication for complex-real data
 *@param[in]  pSrcCmplx : is input vector A
 *@param[in]  pSrcReal : is input vector B
 *@param     *pDst : points to the real output vector
 *@param     numSamples : number of complex samples in the input vector
 *@return    none
 */

void rsi_arm_cmplx_mult_real_q15_opt(q15_t *pSrcCmplx,
                                     q15_t *pSrcReal,
                                     q15_t *pDst,
                                     uint32_t numSamples,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, numSamples / 2, numSamples / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
  for (i = 0; i < (numSamples / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pSrcCmplx, ULPSS_RAM_ADDR_SRC1, i * 2, 1);
#endif
#else
    RSI_FIM_InputDataCmplx(pSrcCmplx, inBank1, i * 2, 1);
#endif
    pSrcCmplx++;
    pSrcCmplx++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pSrcReal, ULPSS_RAM_ADDR_SRC2, i * 2, 0);
#endif
#else
    RSI_FIM_InputDataCmplx(pSrcReal, inBank2, i * 2, 0);
#endif
    pSrcReal++;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcCmplx;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcReal;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_CPLX_REAL);
}

/*==============================================*/
/**
 *@fn          void rsi_arm_cmplx_mult_cmplx_q15_opt(q15_t *pSrcA,
 *                                    q15_t *pSrcB,
 *                                     q15_t *pDst,
 *                                     uint32_t numSamples,
 *                                     uint16_t inBank1,
 *                                     uint16_t inBank2,
 *                                     uint16_t outBank)
 *@brief       This API is used for Q15 complex-by-complex multiplication
 *@param[in]   *pSrcA : points to the first input vector
 *@param[in]   *pSrcB : points to the second input vector
 *@param[out]  *pDst :  points to the output vector
 *@param[in]   numSamples : number of complex samples in each vector
 *@return       none
 */
void rsi_arm_cmplx_mult_cmplx_q15_opt(q15_t *pSrcA,
                                      q15_t *pSrcB,
                                      q15_t *pDst,
                                      uint32_t numSamples,
                                      uint16_t inBank1,
                                      uint16_t inBank2,
                                      uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, numSamples / 2, numSamples / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x10, 0, 0, 1));
  for (i = 0; i < (numSamples / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pSrcA, ULPSS_RAM_ADDR_SRC1, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
#else
    RSI_FIM_InputDataCmplx(pSrcA, inBank1, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
    pSrcA++;
    pSrcA++;
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pSrcB, ULPSS_RAM_ADDR_SRC2, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
#else
    RSI_FIM_InputDataCmplx(pSrcB, inBank2, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
    pSrcB++;
    pSrcB++;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((MUL_VECTOR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_CPLX_CPLX);
}

/*==============================================*/
/**
 *@fn         void rsi_arm_cmplx_mag_squared_q15_opt(q15_t *pSrc, q15_t *pDst, uint32_t numSamples, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Absolute Squaring for real number
 *@param[in]  pSrc : is input for squaring a number
 *@param[in]  length : is size of the input array
 *@return     none
 */

void rsi_arm_cmplx_mag_squared_q15_opt(q15_t *pSrc, q15_t *pDst, uint32_t numSamples, uint16_t inBank, uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetDataLen(NOT_MATRIX, numSamples / 2, numSamples / 2);
  RSI_FIM_SetSatTruncRound(STRS(0x10, 0, 0, 1));
  for (i = 0; i < (numSamples / 2); i++) {
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputDataCmplx(pSrc, ULPSS_RAM_ADDR_SRC, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
#else
    RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, ULP_FIM_COP_DATA_CPLX_CPLX);
#endif
    pSrc++;
    pSrc++;
  }
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((NORM_SQUARE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_CPLX_CPLX);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_absSqr_q7(q7_t *pSrc, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Absolute Squaring for real number
 *@param[in]  pSrc : is input for squaring a number
 *@param[in]  length : is size of the input array
 *@return     none
 */

void rsi_fim_absSqr_q7(q7_t *pSrc, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x19, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q7);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((NORM_SQUARE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_absSqr_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Absolute Squaring for real number
 *@param[in]  pSrc : is input for squaring a number
 *@param[in]  length : is size of the input array
 *@return     none
 */
void rsi_fim_absSqr_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((NORM_SQUARE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_absSqr_q31(q31_t *pSrc, q31_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Absolute Squaring for real number
 *@param[in]  pSrc : is input for squaring a number
 *@param[in]  length : is size of the input array
 *@return     none
 */
void rsi_fim_absSqr_q31(q31_t *pSrc, q31_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((NORM_SQUARE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_absSqr_f32(int32_t *pSrc, int32_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
 *@brief      This API is used to set the FIM Absolute Squaring for real number
 *@param[in]  pSrc : is input for squaring a number
 *@param[in]  length : is size of the input array
 *@return     none
 */
void rsi_fim_absSqr_f32(int32_t *pSrc, int32_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank)
{
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, blockSize);
  RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((NORM_SQUARE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          error_t rsi_arm_mat_mult_q31_opt(const arm_matrix_instance_q31_opt *pSrcA,
 *                                const arm_matrix_instance_q31_opt *pSrcB,
 *                                arm_matrix_instance_q31_opt *pDst,
 *                                uint16_t inBank1,
 *                                uint16_t inBank2,
 *                                uint16_t outBank)
 *@brief       This API is used to set the FIM Matrix Multiplication for real numbers
 *@param[in]  *pSrcA :	points to the first input matrix structure
 *@param[in]  *pSrcB :	points to the second input matrix structure
 *@param[out] *pDst :	points to output matrix structure
 *@return     none
 */

error_t rsi_arm_mat_mult_f32_opt(const arm_matrix_instance_f32_opt *pSrcA,
                                 const arm_matrix_instance_f32_opt *pSrcB,
                                 arm_matrix_instance_f32_opt *pDst,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if (pSrcA->nColumns == pSrcB->nRows) {
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ((uint32_t)pDst->pData);
#endif
#else
      // For 9116
      RSI_FIM_InputData((pSrcA->pData), inBank1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_F32);
      RSI_FIM_InputData((pSrcB->pData), inBank2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_F32);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
      FIM->FIM_MODE_INTERRUPT = ((MUL_MAT << 1) | 1);
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 *@fn          error_t rsi_arm_mat_mult_q31_opt(const arm_matrix_instance_q31_opt *pSrcA,
 *                                const arm_matrix_instance_q31_opt *pSrcB,
 *                                arm_matrix_instance_q31_opt *pDst,
 *                                uint16_t inBank1,
 *                                uint16_t inBank2,
 *                                uint16_t outBank)
 *@brief       This API is used to set the FIM Matrix Multiplication for real numbers
 *@param[in]  *pSrcA :	points to the first input matrix structure
 *@param[in]  *pSrcB :	points to the second input matrix structure
 *@param[out] *pDst :	points to output matrix structure
 *@return     none
 */
error_t rsi_arm_mat_mult_q31_opt(const arm_matrix_instance_q31_opt *pSrcA,
                                 const arm_matrix_instance_q31_opt *pSrcB,
                                 arm_matrix_instance_q31_opt *pDst,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if (pSrcA->nColumns == pSrcB->nRows) {
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ((uint32_t)pDst->pData);
#endif
#else
      // For 9116
      RSI_FIM_InputData((pSrcA->pData), inBank1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      RSI_FIM_InputData((pSrcB->pData), inBank2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
      FIM->FIM_MODE_INTERRUPT = ((MUL_MAT << 1) | 1);
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 *@fn           error_t rsi_arm_mat_mult_q15_opt(const arm_matrix_instance_q15_opt *pSrcA,
 *                               const arm_matrix_instance_q15_opt *pSrcB,
 *                                arm_matrix_instance_q15_opt *pDst,
 *                                q15_t *pState,
 *                                uint16_t inBank1,
 *                                uint16_t inBank2,
 *                                uint16_t outBank)
 *@brief        This API is used to set the FIM Matrix Multiplication for real numbers
 *@param[in]	*pSrcA :	points to the first input matrix structure
 *@param[in]	*pSrcB :	points to the second input matrix structure
 *@param[out]	*pDst :	points to output matrix structure
 *@param[in]	*pState :	points to the array for storing intermediate results (Unused)
 *@return        none
 */
error_t rsi_arm_mat_mult_q15_opt(const arm_matrix_instance_q15_opt *pSrcA,
                                 const arm_matrix_instance_q15_opt *pSrcB,
                                 arm_matrix_instance_q15_opt *pDst,
                                 q15_t *pState,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if (pSrcA->nColumns == pSrcB->nRows) {
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q15);
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q15);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ((uint32_t)pDst->pData);
#endif
#else
      // For 9116
      RSI_FIM_InputData((pSrcA->pData), inBank1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q15);
      RSI_FIM_InputData((pSrcB->pData), inBank2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q15);
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif

      FIM->FIM_MODE_INTERRUPT = ((MUL_MAT << 1) | 1);
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 *@fn           void rsi_arm_fir_init_f32_opt(arm_fir_instance_f32_opt *S,
 *                             uint16_t numTaps,
 *                             int32_t *pCoeffs,
 *                             int32_t *pState,
 *                             uint32_t blockSize)
 *@brief        This API is used to initialize the FIM  filters
 *@param[in,out]	*S :	points to an instance of the floating-point FIR filter structure.
 *@param[in]	numTaps :	Number of filter coefficients in the filter.
 *@param[in]	*pCoeffs :	points to the filter coefficients buffer.
 *@param[in]	*pState :	points to the state buffer.
 *@param[in]	blockSize :	number of samples that are processed per call.
 *@return        none
 */
void rsi_arm_fir_init_f32_opt(arm_fir_instance_f32_opt *S,
                              uint16_t numTaps,
                              int32_t *pCoeffs,
                              int32_t *pState,
                              uint32_t blockSize)
{
  // Assign filter taps
  S->numTaps = numTaps;

  // Assign coefficient pointer
  S->pCoeffs = pCoeffs;
  // Assign state pointer
  S->pState = pState;
}

/*==============================================*/
/**
 *@fn        void rsi_arm_fir_f32_opt(arm_fir_instance_f32_opt *S,
 *                        int32_t *pSrc,
 *                        int32_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief     This API is used to set the FIM FIR Filter
 *@param[in]	*S :	points to an instance of the floating-point FIR filter structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]	*pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of samples to process per call.
 *@return     none
 */
void rsi_arm_fir_f32_opt(arm_fir_instance_f32_opt *S,
                         int32_t *pSrc,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  if (S->numTaps > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numTaps));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, S->numTaps, FORMAT_F32);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pCoeffs, inBank2, S->numTaps, FORMAT_F32);
#endif
  } else {
    RSI_FIM_SetDataLen((S->numTaps), (S->numTaps), blockSize);
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC1, S->numTaps, FORMAT_F32);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pCoeffs, inBank1, S->numTaps, FORMAT_F32);
#endif
  }
  RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((FIR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn            void rsi_arm_fir_init_q31_opt(arm_fir_instance_q31_opt *S,
 *                             uint16_t numTaps,
 *                             q31_t *pCoeffs,
 *                             q31_t *pState,
 *                             uint32_t blockSize)
 *@brief         This API is used to initialize the FIM  filters
 *@param[in,out] *S :	points to an instance of the Q31 FIR filter structure.
 *@param[in]	 numTaps :	Number of filter coefficients in the filter.
 *@param[in]	 *pCoeffs :	points to the filter coefficients buffer.
 *@param[in]	 *pState :	points to the state buffer.
 *@param[in]	 blockSize :	number of samples that are processed per call.
 *@return         none
 */

void rsi_arm_fir_init_q31_opt(arm_fir_instance_q31_opt *S,
                              uint16_t numTaps,
                              q31_t *pCoeffs,
                              q31_t *pState,
                              uint32_t blockSize)
{
  // Assign filter taps
  S->numTaps = numTaps;
  // Assign coefficient pointer
  S->pCoeffs = pCoeffs;
  // Assign state pointer
  S->pState = pState;
}

/*==============================================*/
/**
 *@fn           void rsi_arm_fir_q31_opt(arm_fir_instance_q31_opt *S,
 *                        q31_t *pSrc,
 *                        q31_t *pDst,
 *                        uint32_t blockSize,
 *                        uint16_t inBank1,
 *                        uint16_t inBank2,
 *                        uint16_t outBank)
 *@brief        This API is used to set the FIM FIR Filter
 *@param[in]	*S :	points to an instance of the Q31 FIR filter structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]	*pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of samples to process per call
 *@return     none
 */
void rsi_arm_fir_q31_opt(arm_fir_instance_q31_opt *S,
                         q31_t *pSrc,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{
  if (S->numTaps > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numTaps));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, S->numTaps, FORMAT_Q31);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pCoeffs, inBank2, S->numTaps, FORMAT_Q31);
#endif
  } else {
    RSI_FIM_SetDataLen((S->numTaps), (S->numTaps), blockSize);
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC1, S->numTaps, FORMAT_Q31);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pCoeffs, inBank1, S->numTaps, FORMAT_Q31);
#endif
  }
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((FIR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *fn             void rsi_arm_fir_init_q15_opt(arm_fir_instance_q15_opt *S,
 *                             uint16_t numTaps,
 *                             q15_t *pCoeffs,
 *                             q15_t *pState,
 *                             uint32_t blockSize)
 *@brief         This API is used to initialise the FIM  filters
 *@param[in,out] *S :	points to an instance of the Q15 FIR filter structure.
 *@param[in]	 numTaps :	Number of filter coefficients in the filter. Must be even and greater than or equal to 4.
 *@param[in]	 *pCoeffs :	points to the filter coefficients buffer.
 *@param[in]	 *pState :	points to the state buffer.
 *@param[in]	 blockSize :	is number of samples processed per call.
 *@return        none
 *
 */
void rsi_arm_fir_init_q15_opt(arm_fir_instance_q15_opt *S,
                              uint16_t numTaps,
                              q15_t *pCoeffs,
                              q15_t *pState,
                              uint32_t blockSize)
{
  // Assign filter taps
  S->numTaps = numTaps;

  // Assign coefficient pointer
  S->pCoeffs = pCoeffs;
  // Assign state pointer
  S->pState = pState;
}

/*==============================================*/
/**
  *@fn              void rsi_arm_fir_q15_opt(arm_fir_instance_q15_opt *S,
  *                       q15_t *pSrc,
  *                       q15_t *pDst,
  *                       uint32_t blockSize,
  *                       uint16_t inBank1,
  *                       uint16_t inBank2,
  *                       uint16_t outBank)
 *@brief        This API is used to set the FIM FIR Filter
 *@param[in]	*S :	points to an instance of the Q15 FIR structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]	*pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of samples to process per call.
 *@return       none
 */

void rsi_arm_fir_q15_opt(arm_fir_instance_q15_opt *S,
                         q15_t *pSrc,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank)
{

  if (S->numTaps > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numTaps));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, S->numTaps, FORMAT_Q15);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q15);
    RSI_FIM_InputData(S->pCoeffs, inBank2, S->numTaps, FORMAT_Q15);
#endif
  } else {
    RSI_FIM_SetDataLen((S->numTaps), (S->numTaps), blockSize);
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q15);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC1, S->numTaps, FORMAT_Q15);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_Q15);
    RSI_FIM_InputData(S->pCoeffs, inBank1, S->numTaps, FORMAT_Q15);
#endif
  }
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((FIR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn            void rsi_arm_fir_init_q7_opt(arm_fir_instance_q7_opt *S,
 *                            uint16_t numTaps,
 *                            q7_t *pCoeffs,
 *                            q7_t *pState,
 *                            uint32_t blockSize)
 *@brief         This API is used to initialise the FIM  filters
 *@param[in,out] *S :	points to an instance of the Q7 FIR filter structure.
 *@param[in]	 numTaps :	Number of filter coefficients in the filter.
 *@param[in]	 *pCoeffs :	points to the filter coefficients buffer.
 *@param[in]	 *pState :	points to the state buffer.
 *@param[in]	 blockSize :	number of samples that are processed per call.
 *@return         none
 */
void rsi_arm_fir_init_q7_opt(arm_fir_instance_q7_opt *S,
                             uint16_t numTaps,
                             q7_t *pCoeffs,
                             q7_t *pState,
                             uint32_t blockSize)
{
  // Assign filter taps
  S->numTaps = numTaps;
  // Assign coefficient pointer
  S->pCoeffs = pCoeffs;
  // Assign state pointer
  S->pState = pState;
}

/*==============================================*/
/**
 *@fn           void rsi_arm_fir_q7_opt(arm_fir_instance_q7_opt *S,
 *                       q7_t *pSrc,
 *                       q7_t *pDst,
 *                       uint32_t blockSize,
 *                       uint16_t inBank1,
 *                       uint16_t inBank2,
 *                       uint16_t outBank)
 *@brief        This API is used to set the FIM FIR Filter
 *@param[in]	*S :	points to an instance of the Q7 FIR filter structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]	*pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of samples to process per call.
 *@return        none
 */
void rsi_arm_fir_q7_opt(arm_fir_instance_q7_opt *S,
                        q7_t *pSrc,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank)
{
  if (S->numTaps > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numTaps));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q7);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, S->numTaps, FORMAT_Q7);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q7);
    RSI_FIM_InputData(S->pCoeffs, inBank2, S->numTaps, FORMAT_Q7);
#endif
  } else {
    RSI_FIM_SetDataLen((S->numTaps), (S->numTaps), blockSize);
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC2, blockSize, FORMAT_Q7);
    RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC1, S->numTaps, FORMAT_Q7);
#endif
#else
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_Q7);
    RSI_FIM_InputData(S->pCoeffs, inBank1, S->numTaps, FORMAT_Q7);
#endif
  }
  RSI_FIM_SetSatTruncRound(STRS(0x19, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((FIR << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn           arm_status rsi_arm_fir_interpolate_init_f32_opt(arm_fir_interpolate_instance_f32_opt *S,
 *                                               uint8_t L,
 *                                               uint16_t numTaps,
 *                                               int32_t *pCoeffs,
 *                                               int32_t *pState,
 *                                               uint32_t blockSize)
 *@brief             This API is used to initialization function for the floating-point FIR interpolator.
 *@param[in,out]     *S :	points to an instance of the floating-point FIR interpolator structure.
 *@param[in]	     L	 :upsample factor.
 *@param[in]	     numTaps :	number of filter coefficients in the filter.
 *@param[in]	     *pCoeffs :	points to the filter coefficient buffer.
 *@param[in]	     *pState :	points to the state buffer.
 *@param[in]	     blockSize :	number of input samples to process per call.
 *
 *@return            The function returns \ref ARM_MATH_SUCCESS if initialization was successful or \ref ARM_MATH_LENGTH_ERROR if the filter length numTaps is not a multiple of the interpolation factor L.
 */
arm_status rsi_arm_fir_interpolate_init_f32_opt(arm_fir_interpolate_instance_f32_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                int32_t *pCoeffs,
                                                int32_t *pState,
                                                uint32_t blockSize)
{
  arm_status status;

  // The filter length must be a multiple of the interpolation factor
  if ((numTaps % L) != 0u) {
    // Set status as ARM_MATH_LENGTH_ERROR
    status = ARM_MATH_LENGTH_ERROR;
  } else {
    // Assign coefficient pointer
    S->pCoeffs = pCoeffs;
    // Assign Interpolation factor
    S->L = L;
    // Assign polyPhaseLength
    S->phaseLength = numTaps / L;
    // Assign state pointer
    S->pState = pState;
    status    = ARM_MATH_SUCCESS;
  }
  return (status);
}

/*==============================================*/
/**
 *@fn                void rsi_arm_fir_interpolate_f32_opt(const arm_fir_interpolate_instance_f32_opt *S,
 *                                    int32_t *pSrc,
 *                                    int32_t *pDst,
 *                                    uint32_t blockSize,
 *                                    uint16_t inBank1,
 *                                    uint16_t inBank2,
 *                                    uint16_t outBank)
 *@brief        This API is used Processing function for the floating-point FIR interpolator.
 *@param[in]	*S :	points to an instance of the floating-point FIR interpolator structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]   *pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of input samples to process per call.
 *@return       none
 */
void rsi_arm_fir_interpolate_f32_opt(const arm_fir_interpolate_instance_f32_opt *S,
                                     int32_t *pSrc,
                                     int32_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank)
{

  uint32_t numTaps;
  numTaps = (S->phaseLength) * (S->L);

  RSI_FIM_SetDataLen(numTaps, blockSize, numTaps);
  RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, numTaps, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_F32);
  RSI_FIM_InputData(S->pCoeffs, inBank2, numTaps, FORMAT_F32);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_CONFIG_REG2 |= (S->L) << 22;
  FIM->FIM_MODE_INTERRUPT = ((INTERPOLATE << 1) | 1);
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn                  arm_status rsi_arm_fir_interpolate_init_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
 *                                              uint8_t L,
 *                                              uint16_t numTaps,
 *                                              q15_t *pCoeffs,
 *                                              q15_t *pState,
 *                                              uint32_t blockSize)
 *@brief            This API  is used to  initialization function for the Q31 FIR interpolator.
 *@param[in,out]    *S :	points to an instance of the Q31 FIR interpolator structure.
 *@param[in]	    L :	upsample factor.
 *@param[in]	    numTaps :	number of filter coefficients in the filter.
 *@param[in]	    *pCoeffs :	points to the filter coefficient buffer.
 *@param[in]	    *pState :	points to the state buffer.
 *@param[in]	    blockSize :	number of input samples to process per call.
 *@return           The function returns \ref ARM_MATH_SUCCESS if initialization was successful or \ref ARM_MATH_LENGTH_ERROR if the filter length numTaps is not a multiple of the interpolation factor L.
 *
 */
arm_status rsi_arm_fir_interpolate_init_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                q15_t *pCoeffs,
                                                q15_t *pState,
                                                uint32_t blockSize)
{
  arm_status status;

  // The filter length must be a multiple of the interpolation factor
  if ((numTaps % L) != 0u) {
    // Set status as ARM_MATH_LENGTH_ERROR
    status = ARM_MATH_LENGTH_ERROR;
  } else {
    // Assign coefficient pointer
    S->pCoeffs = pCoeffs;

    // Assign Interpolation factor
    S->L = L;

    // Assign polyPhaseLength
    S->phaseLength = numTaps / L;

    // Assign state pointer
    S->pState = pState;
    status    = ARM_MATH_SUCCESS;
  }
  return (status);
}

/*==============================================*/
/**
 *@fn           void rsi_arm_fir_interpolate_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
 *                                    q15_t *pSrc,
 *                                    q15_t *pDst,
 *                                    uint32_t blockSize,
 *                                    uint16_t inBank1,
 *                                    uint16_t inBank2,
 *                                    uint16_t outBank)
 *@brief        This API is used to Processing function for the Q15 FIR interpolator
 *@param[in]	*S :	points to an instance of the Q15 FIR interpolator structure.
 *@param[in]	*pSrc :	points to the block of input data.
 *@param[out]	*pDst :	points to the block of output data.
 *@param[in]	blockSize :	number of input samples to process per call.
 *@return       none
 */
void rsi_arm_fir_interpolate_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
                                     q15_t *pSrc,
                                     q15_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank)
{

  uint32_t numTaps;
  numTaps = (S->phaseLength) * (S->L);

  RSI_FIM_SetDataLen(numTaps, blockSize, numTaps);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, numTaps, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData(S->pCoeffs, inBank2, numTaps, FORMAT_Q15);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((INTERPOLATE << 1) | 1);
  FIM->FIM_CONFIG_REG2 |= (S->L) << 22;
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          arm_status rsi_arm_fir_interpolate_init_q31_opt(arm_fir_interpolate_instance_q31_opt *S,
 *                                              uint8_t L,
 *                                               uint16_t numTaps,
 *                                               q31_t *pCoeffs,
 *                                               q31_t *pState,
 *                                               uint32_t blockSize)
 *@brief            This API  is used to  initialization function for the Q31 FIR interpolator.
 *@param[in,out]    *S :	points to an instance of the Q31 FIR interpolator structure.
 *@param[in]	    L :	upsample factor.
 *@param[in]	    numTaps :	number of filter coefficients in the filter.
 *@param[in]	    *pCoeffs :	points to the filter coefficient buffer.
 *@param[in]	    *pState :	points to the state buffer.
 *@param[in]	    blockSize :	number of input samples to process per call.
 *@return           The function returns \ref ARM_MATH_SUCCESS if initialization was successful or \ref ARM_MATH_LENGTH_ERROR if the filter length numTaps is not a multiple of the interpolation factor L.
 *
 */
arm_status rsi_arm_fir_interpolate_init_q31_opt(arm_fir_interpolate_instance_q31_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                q31_t *pCoeffs,
                                                q31_t *pState,
                                                uint32_t blockSize)
{
  arm_status status;
  // The filter length must be a multiple of the interpolation factor
  if ((numTaps % L) != 0u) {
    // Set status as ARM_MATH_LENGTH_ERROR
    status = ARM_MATH_LENGTH_ERROR;
  } else {
    // Assign coefficient pointer
    S->pCoeffs = pCoeffs;

    // Assign Interpolation factor
    S->L = L;

    // Assign polyPhaseLength
    S->phaseLength = numTaps / L;

    // Assign state pointer
    S->pState = pState;

    status = ARM_MATH_SUCCESS;
  }

  return (status);
}

/*==============================================*/
/**
 *@fn          rsi_arm_fir_interpolate_q31_opt(const arm_fir_interpolate_instance_q31_opt *S,
                                     q31_t *pSrc,
                                     q31_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank)
 *@brief            This API is used for Q31 FIR interpolator.
 *@param[in,out]    *S :	points to an instance of the Q31 FIR interpolator structure.
 *@param[in]	    *pSrc :	Source Pointer.
 *@param[in]	    *pDst :	Destination pointer
 *@param[in]	    blockSize :	number of input samples to process per call
 *@param[in]	    inBank1
 *@param[in]	    inBank2
  *@param[in]	    outBank
 *@return           None
 *
 */
void rsi_arm_fir_interpolate_q31_opt(const arm_fir_interpolate_instance_q31_opt *S,
                                     q31_t *pSrc,
                                     q31_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank)
{
  uint32_t numTaps;
  numTaps = (S->phaseLength) * (S->L);

  RSI_FIM_SetDataLen(numTaps, blockSize, numTaps);
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#ifdef ENHANCED_FIM
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(S->pCoeffs, ULPSS_RAM_ADDR_SRC2, numTaps, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = ULPSS_RAM_ADDR_DST;
#else
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = (uint32_t)pDst;
#endif
#else
  // For 9116
  RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q31);
  RSI_FIM_InputData(S->pCoeffs, inBank2, numTaps, FORMAT_Q31);
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = outBank;
#endif
  FIM->FIM_MODE_INTERRUPT = ((INTERPOLATE << 1) | 1);
  FIM->FIM_CONFIG_REG2 |= (S->L) << 22;
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_fim_interrupt_handler(volatile FIM_Type *ptFim)
 *@brief     This API Clears interrupt status of fim
 *@param[in]  ptFim is pointer to the FIM register instance
 *@return    none
 */
void rsi_fim_interrupt_handler(volatile FIM_Type *ptFim)
{
  ptFim->FIM_MODE_INTERRUPT_b.INTR_CLEAR = 0x1;
}

#ifdef ENHANCED_FIM

/*==============================================*/
/**   
 * @fn         void rsi_arm_correlate_q15(const q15_t *pSrcA, uint32_t srcALen, const q15_t *pSrcB, uint32_t srcBLen, q15_t *pDst)
 * @brief      This API is used to set FIM Correlation Operation of Q15 sequence. 
 * @param[in]  *pSrcA points to the first input sequence.   
 * @param[in]  srcALen length of the first input sequence.   
 * @param[in]  *pSrcB points to the second input sequence.   
 * @param[in]  srcBLen length of the second input sequence.   
 * @param[out] *pDst points to the location where the output result is written.  Length 2 * max(srcALen, srcBLen) - 1.   
 * @return     none.   
 */
void rsi_arm_correlate_q15(const q15_t *pSrcA, uint32_t srcALen, const q15_t *pSrcB, uint32_t srcBLen, q15_t *pDst)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, srcALen, srcBLen);
  // Set saturation,trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x01, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store Input data1 in ulp memories
  RSI_FIM_InputData((void *)pSrcA, ULPSS_RAM_ADDR_SRC1, srcALen, FORMAT_Q15);
  // Store Input data2 in ulp memories
  RSI_FIM_InputData((void *)pSrcB, ULPSS_RAM_ADDR_SRC2, srcBLen, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.Enable latch mode
  FIM->FIM_MODE_INTERRUPT = ((CORRELATION << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_correlate_q31(const q31_t *pSrcA, uint32_t srcALen, const q31_t *pSrcB, uint32_t srcBLen, q31_t *pDst)
 * @brief      This API is used to set FIM Correlation Operation of Q31 sequence. 
 * @param[in]  *pSrcA points to the first input sequence.   
 * @param[in]  srcALen length of the first input sequence.   
 * @param[in]  *pSrcB points to the second input sequence.   
 * @param[in]  srcBLen length of the second input sequence.   
 * @param[out] *pDst points to the location where the output result is written.  Length 2 * max(srcALen, srcBLen) - 1.   
 * @return     none.   
 */
void rsi_arm_correlate_q31(const q31_t *pSrcA, uint32_t srcALen, const q31_t *pSrcB, uint32_t srcBLen, q31_t *pDst)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, srcALen, srcBLen);
  // Set saturation,trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x01, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store Input data1 in ulp memories
  RSI_FIM_InputData((void *)pSrcA, ULPSS_RAM_ADDR_SRC1, srcALen, FORMAT_Q31);
  // Store Input data1 in ulp memories
  RSI_FIM_InputData((void *)pSrcB, ULPSS_RAM_ADDR_SRC2, srcBLen, FORMAT_Q31);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.Enable latch mode
  FIM->FIM_MODE_INTERRUPT = ((CORRELATION << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_correlate_q7(const q7_t *pSrcA, uint32_t srcALen, const q7_t *pSrcB, uint32_t srcBLen, q7_t *pDst) 
 * @brief      This API is used to set FIM Correlation Operation of Q7 sequence. 
 * @param[in]  *pSrcA points to the first input sequence.   
 * @param[in]  srcALen length of the first input sequence.   
 * @param[in]  *pSrcB points to the second input sequence.   
 * @param[in]  srcBLen length of the second input sequence.   
 * @param[out] *pDst points to the location where the output result is written.  Length 2 * max(srcALen, srcBLen) - 1.   
 * @return     none.   
 */
void rsi_arm_correlate_q7(const q7_t *pSrcA, uint32_t srcALen, const q7_t *pSrcB, uint32_t srcBLen, q7_t *pDst)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, srcALen, srcBLen);
  // Set saturation,trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x01, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store Input data1 in ulp memories
  RSI_FIM_InputData((void *)pSrcA, ULPSS_RAM_ADDR_SRC1, srcALen, FORMAT_Q7);
  // Store Input data2 in ulp memories
  RSI_FIM_InputData((void *)pSrcB, ULPSS_RAM_ADDR_SRC2, srcBLen, FORMAT_Q7);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrcA;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)pSrcB;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.Enable latch mode
  FIM->FIM_MODE_INTERRUPT = ((CORRELATION << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**    
 * @fn     arm_status arm_fir_decimate_init_q31(arm_fir_decimate_instance_q31 *S,
 *                                    uint16_t numTaps,
 *                                    uint8_t M,
 *                                    q31_t *pCoeffs,
 *                                    q31_t *pState,
 *                                    uint32_t blockSize)
 * @brief  Initialization function for the Q31 FIR decimator.    
 * @param[in,out] *S points to an instance of the Q31 FIR decimator structure.    
 * @param[in] numTaps  number of coefficients in the filter.    
 * @param[in] M  decimation factor.    
 * @param[in] *pCoeffs points to the filter coefficients.    
 * @param[in] *pState points to the state buffer.    
 * @param[in] blockSize number of input samples to process per call.    
 * @return    The function returns ARM_MATH_SUCCESS if initialization was successful or ARM_MATH_LENGTH_ERROR if    
 * <code>blockSize</code> is not a multiple of <code>M</code>.        
 */
arm_status arm_fir_decimate_init_q31(arm_fir_decimate_instance_q31 *S,
                                     uint16_t numTaps,
                                     uint8_t M,
                                     q31_t *pCoeffs,
                                     q31_t *pState,
                                     uint32_t blockSize)
{
  arm_status status;

  // The size of the input block must be a multiple of the decimation factor
  if ((blockSize % M) != 0u) {
    // Set status as ARM_MATH_LENGTH_ERROR
    status = ARM_MATH_LENGTH_ERROR;
  } else {
    // Assign filter taps
    S->numTaps = numTaps;

    // Assign coefficient pointer
    S->pCoeffs = pCoeffs;

    // Clear the state buffer.  The size is always (blockSize + numTaps - 1)
    memset(pState, 0, (numTaps + (blockSize - 1)) * sizeof(q31_t));

    // Assign state pointer
    S->pState = pState;

    // Assign Decimation factor
    S->M = M;

    status = ARM_MATH_SUCCESS;
  }
  return (status);
}

/*==============================================*/
/**  
 * @fn      arm_status arm_fir_decimate_init_q15(arm_fir_decimate_instance_q15 *S,
 *                                    uint16_t numTaps,
 *                                    uint8_t M,
 *                                    q15_t *pCoeffs,
 *                                    q15_t *pState,
 *                                    uint32_t blockSize)
 * @brief  Initialization function for the Q15 FIR decimator.    
 * @param[in,out] *S points to an instance of the Q15 FIR decimator structure.    
 * @param[in] numTaps  number of coefficients in the filter.    
 * @param[in] M  decimation factor.    
 * @param[in] *pCoeffs points to the filter coefficients.    
 * @param[in] *pState points to the state buffer.    
 * @param[in] blockSize number of input samples to process per call.    
 * @return    The function returns ARM_MATH_SUCCESS if initialization was successful or ARM_MATH_LENGTH_ERROR if    
 * <code>blockSize</code> is not a multiple of <code>M</code>.    
 */
arm_status arm_fir_decimate_init_q15(arm_fir_decimate_instance_q15 *S,
                                     uint16_t numTaps,
                                     uint8_t M,
                                     q15_t *pCoeffs,
                                     q15_t *pState,
                                     uint32_t blockSize)
{

  arm_status status;

  // The size of the input block must be a multiple of the decimation factor
  if ((blockSize % M) != 0u) {
    // Set status as ARM_MATH_LENGTH_ERROR
    status = ARM_MATH_LENGTH_ERROR;
  } else {
    // Assign filter taps
    S->numTaps = numTaps;

    // Assign coefficient pointer
    S->pCoeffs = pCoeffs;

    // Clear the state buffer.  The size of buffer is always (blockSize + numTaps - 1)
    memset(pState, 0, (numTaps + (blockSize - 1u)) * sizeof(q15_t));

    // Assign state pointer
    S->pState = pState;

    // Assign Decimation factor
    S->M = M;

    status = ARM_MATH_SUCCESS;
  }
  return (status);
}

/*==============================================*/
/**    
 * @fn    void arm_fir_decimate_q15(const arm_fir_decimate_instance_q15 *S, q15_t *pSrc, q15_t *pDst, uint32_t blockSize)  
 * @brief Processing function for the Q15 FIR decimator.    
 * @param[in] *S points to an instance of the Q15 FIR decimator structure.    
 * @param[in] *pSrc points to the block of input data.    
 * @param[out] *pDst points to the location where the output result is written.    
 * @param[in] blockSize number of input samples to process per call.    
 * @return none.        
 */
void arm_fir_decimate_q15(const arm_fir_decimate_instance_q15 *S, q15_t *pSrc, q15_t *pDst, uint32_t blockSize)
{
  uint32_t numTaps;
  numTaps = (S->numTaps);
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, numTaps);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  RSI_FIM_InputData((void *)S->pCoeffs, ULPSS_RAM_ADDR_SRC2, numTaps, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of 2nd Input Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of 1st coefficient for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((DECIMATION << 1) | 1);
  /* Set Decimation Factor */
  FIM->FIM_CONFIG_REG1_b.DECIM_FAC = S->M;
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**  
 * @fn      void arm_fir_decimate_q31(const arm_fir_decimate_instance_q31 *S, q31_t *pSrc, q31_t *pDst, uint32_t blockSize)
 * @brief Processing function for the Q31 FIR decimator.    
 * @param[in] *S points to an instance of the Q31 FIR decimator structure.    
 * @param[in] *pSrc points to the block of input data.    
 * @param[out] *pDst points to the block of output data    
 * @param[in] blockSize number of input samples to process per call.    
 * @return none    
 */
void arm_fir_decimate_q31(const arm_fir_decimate_instance_q31 *S, q31_t *pSrc, q31_t *pDst, uint32_t blockSize)
{
  uint32_t numTaps;
  numTaps = (S->numTaps);
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, numTaps);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q31);
  /* Store pCoeffs in ulp memories */
  RSI_FIM_InputData((void *)S->pCoeffs, ULPSS_RAM_ADDR_SRC2, numTaps, FORMAT_Q31);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of 1st coefficients for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of 1st coefficients for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)S->pCoeffs;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((DECIMATION << 1) | 1);
  // Set Decimation Factor
  FIM->FIM_CONFIG_REG1_b.DECIM_FAC = S->M;
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
* @details   
* @fn       void rsi_arm_cfft_radix2(q31_t *pSrc)
* @brief       Processing function for complex FFT.
* @param[in]      *S    points to an instance of the  CFFT structure.  
* @param[in, out] *pSrc   points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place.  
* @param[in]     ifftFlag       flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform.  
* @param[in]     bitReverseFlag flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output.  
* @return none.  
*/
void rsi_arm_cfft_radix2(q31_t *pSrc)
{
  uint32_t i = 0;
  q31_t pSrc_even[LOOKUP_LENGTH];
  q31_t pSrc_odd[LOOKUP_LENGTH];
  uint32_t pSrc_even_len = LOOKUP_LENGTH;
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  uint32_t pSrc_odd_len = LOOKUP_LENGTH;
#endif
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, FFT_LENGTH, NO_OF_STAGES);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
  for (i = 0; i < pSrc_even_len; i++) {
    pSrc_even[i] = (pSrc[LOOKUP_TABLE_EVEN_BANK[i]]);
    pSrc_odd[i]  = (pSrc[LOOKUP_TABLE_ODD_BANK[i]]);
  }
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store even address data in ulp memories
  RSI_FIM_InputData(pSrc_even, ULPSS_RAM_ADDR_SRC1, pSrc_even_len, FORMAT_Q31);
  // Store odd address data in ulp memories
  RSI_FIM_InputData((void *)pSrc_odd, ULPSS_RAM_ADDR_SRC2, pSrc_odd_len, FORMAT_Q31);
  // Twiddle factor
  RSI_FIM_InputData((void *)TWIDDLE_FACTOR_TABLE, ULPSS_RAM_ADDR_DST, TWIDDLE_FACTOR_LEN, FORMAT_Q31);
  // Start Address of even address  Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of  odd address for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of twiddle factor Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST; //twiddle
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((FFT << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_CPLX_CPLX);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_sin_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric sine function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_sin_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut1, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut1;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;

#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_SINE << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_cos_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric Cosine function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_cos_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut1, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut1;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_COSINE << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/** 
 * @fn         void rsi_arm_Inverse_Tan_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric Inverse Tan function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_Inverse_Tan_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut1, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut1;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_INV_TAN << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_Sinh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric Sinh function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_Sinh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut2, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;

#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_SINH << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_cosh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric cosh function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_cosh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut2, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_COSH << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**   
 * @fn         void rsi_arm_Inverse_Tanh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
 * @brief      Fast approximation to the trigonometric Inverse Tanh function for Q15 data.
 * @param[in]  *pSrc Scaled input value in radians.
 * @param[out] *pDst points to the block of output data   
 * @param[in]  blockSize number of input samples to process per call.    
 * @return     none
 */
void rsi_arm_Inverse_Tanh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blocksize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blocksize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut2, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((COR_INV_TANH << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**    
 * @fn     error_t rsi_arm_mat_add_q15(const arm_matrix_instance_q15_opt *pSrcA,
                            const arm_matrix_instance_q15_opt *pSrcB,
                            arm_matrix_instance_q15_opt *pDst)
 * @brief Q15 matrix addition.    
 * @param[in]       *pSrcA points to the first input matrix structure    
 * @param[in]       *pSrcB points to the second input matrix structure    
 * @param[out]      *pDst points to output matrix structure    
 * @return     		The function returns either    
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.    
 *      
 */
error_t rsi_arm_mat_add_q15(const arm_matrix_instance_q15_opt *pSrcA,
                            const arm_matrix_instance_q15_opt *pSrcB,
                            arm_matrix_instance_q15_opt *pDst)
{

  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q15);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q15);
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((ADD_MAT << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**   
 * @fn        error_t arm_mat_add_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst)
 * @brief Q31 matrix addition.      
 * @param[in]       *pSrcA points to the first input matrix structure      
 * @param[in]       *pSrcB points to the second input matrix structure      
 * @param[out]      *pDst points to output matrix structure      
 * @return     		The function returns either      
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.                 
 */
error_t arm_mat_add_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst)
{

  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif

      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((ADD_MAT << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**  
 * @fn       error_t arm_mat_sub_q15(const arm_matrix_instance_q15_opt *pSrcA,
                        const arm_matrix_instance_q15_opt *pSrcB,
                        arm_matrix_instance_q15_opt *pDst)
 * @brief Q15 matrix subtraction.    
 * @param[in]       *pSrcA points to the first input matrix structure    
 * @param[in]       *pSrcB points to the second input matrix structure    
 * @param[out]      *pDst points to output matrix structure    
 * @return     		The function returns either    
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.        
 */
error_t arm_mat_sub_q15(const arm_matrix_instance_q15_opt *pSrcA,
                        const arm_matrix_instance_q15_opt *pSrcB,
                        arm_matrix_instance_q15_opt *pDst)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q15);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q15);
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((SUB_MAT << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/** 
 * @fn        error_t arm_mat_sub_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst)   
 * @brief Q31 matrix subtraction.        
 * @param[in]       *pSrcA points to the first input matrix structure        
 * @param[in]       *pSrcB points to the second input matrix structure        
 * @param[out]      *pDst points to output matrix structure        
 * @return     		The function returns either        
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.                  
 */
error_t arm_mat_sub_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      /* ULPSS buffers from application */
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((SUB_MAT << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**   
 * @fn     error_t arm_mat_trans_q15(const arm_matrix_instance_q15_opt *pSrc, arm_matrix_instance_q15_opt *pDst)
 * @brief Q15 matrix transpose.    
 * @param[in]  *pSrc points to the input matrix    
 * @param[out] *pDst points to the output matrix    
 * @return 	The function returns either  <code>ERROR_FIM_MATRIX_INVALID_ARG</code>    
 * or <code>RSI_OK</code> based on the outcome of size checking.    
 */
error_t arm_mat_trans_q15(const arm_matrix_instance_q15_opt *pSrc, arm_matrix_instance_q15_opt *pDst)
{
  // Number of rows of input matrix A
  uint16_t numRows = pSrc->nRows;
  // Number of columns of input matrix B
  uint16_t numCols = pSrc->nColumns;
  if (pSrc == NULL) {
    return ERROR_FIM_MATRIX_INVALID_ARG;
  } else {
    // Set column size of buffers in the FIM module
    RSI_FIM_SetDataLen(numCols, 0, 0);
    // Set sat trunc values to FIM module for corresponding inputs
    RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
    // Store data in ulp memories
    RSI_FIM_InputData((pSrc->pData), ULPSS_RAM_ADDR_SRC1, ((pSrc->nRows) * (pSrc->nColumns)), FORMAT_Q15);
    // Start Address of 1st Input Data for FIM Operations
    FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
    // Start Address of Output Data for FIM Operations
    FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
    // ULPSS buffers from application
    // Start Address of 1st Input Data for FIM Operations
    FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrc->pData);
    // Start Address of Output Data for FIM Operations
    FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);

#endif
    // Set the Mode of Operation to be performed.
    FIM->FIM_MODE_INTERRUPT = ((MAT_TRANSPOSE << 1) | 1);
    // Set the data type and  Trigger the FIM module
    RSI_FIM_DatTypTrig(numRows, numCols, ULP_FIM_COP_DATA_REAL_REAL);
    return RSI_OK;
  }
}

/*==============================================*/
/** 
  * @fn       error_t rsi_arm_mat_trans_q31(const arm_matrix_instance_q31_opt *pSrc, arm_matrix_instance_q31_opt *pDst)
  * @brief Q31 matrix transpose.    
  * @param[in]  *pSrc points to the input matrix    
  * @param[out] *pDst points to the output matrix    
  * @return 	The function returns either  <code>ERROR_FIM_MATRIX_INVALID_ARG</code>    
  * or <code>RSI_OK</code> based on the outcome of size checking.    
 */

error_t rsi_arm_mat_trans_q31(const arm_matrix_instance_q31_opt *pSrc, arm_matrix_instance_q31_opt *pDst)
{
  // Number of rows of input matrix A
  uint16_t numRows = pSrc->nRows;
  // Number of columns of input matrix B
  uint16_t numCols = pSrc->nColumns;
  if (pSrc == NULL) {
    return ERROR_FIM_MATRIX_INVALID_ARG;
  } else {
    /* Set column size of buffers in the FIM module */
    RSI_FIM_SetDataLen(numCols, 0, 0);
    // Set sat trunc values to FIM module for corresponding inputs
    RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
    // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
    // Store data in ulp memories
    RSI_FIM_InputData((pSrc->pData), ULPSS_RAM_ADDR_SRC1, ((pSrc->nRows) * (pSrc->nColumns)), FORMAT_Q31);
    /* Start Address of 1st Input Data for FIM Operations */
    FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
    // Start Address of Output Data for FIM Operations
    FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
    /* ULPSS buffers from application */
    /* Start Address of 1st Input Data for FIM Operations */
    FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrc->pData);
    // Start Address of Output Data for FIM Operations
    FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
    // Set the Mode of Operation to be performed.
    FIM->FIM_MODE_INTERRUPT = ((MAT_TRANSPOSE << 1) | 1);
    // Set the data type and  Trigger the FIM module
    RSI_FIM_DatTypTrig(numRows, numCols, ULP_FIM_COP_DATA_REAL_REAL);
    return RSI_OK;
  }
}

/*==============================================*/
/**  
 * @fn       error_t rsi_arm_mat_Hadamard_prod_q15(const arm_matrix_instance_q15_opt *pSrcA,
                                      const arm_matrix_instance_q15_opt *pSrcB,
                                      arm_matrix_instance_q15_opt *pDst)
 * @brief Q15 matrix Hadamard product.    
 * @param[in]       *pSrcA points to the first input matrix structure    
 * @param[in]       *pSrcB points to the second input matrix structure    
 * @param[out]      *pDst points to output matrix structure    
 * @return     		The function returns either    
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.    
 */
error_t rsi_arm_mat_Hadamard_prod_q15(const arm_matrix_instance_q15_opt *pSrcA,
                                      const arm_matrix_instance_q15_opt *pSrcB,
                                      arm_matrix_instance_q15_opt *pDst)
{
  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q15);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q15);
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      // Start Address of 1st Input Data for FIM Operations
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      // Start Address of 2nd Input Data for FIM Operations
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((MAT_HADAMARD << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**    
 * @fn     error_t arm_mat_Hadamard_prod_q31(const arm_matrix_instance_q31_opt *pSrcA,
                                  const arm_matrix_instance_q31_opt *pSrcB,
                                  arm_matrix_instance_q31_opt *pDst)
 * @brief Q31 matrix Hadamard product.    
 * @param[in]       *pSrcA points to the first input matrix structure    
 * @param[in]       *pSrcB points to the second input matrix structure    
 * @param[out]      *pDst points to output matrix structure    
 * @return     		The function returns either    
 * <code>ERROR_FIM_MATRIX_INVALID_ARG</code> or <code>RSI_OK</code> based on the outcome of size checking.    
 */
error_t arm_mat_Hadamard_prod_q31(const arm_matrix_instance_q31_opt *pSrcA,
                                  const arm_matrix_instance_q31_opt *pSrcB,
                                  arm_matrix_instance_q31_opt *pDst)
{

  // Number of rows of input matrix A
  uint16_t numRowsA = pSrcA->nRows;
  // Number of columns of input matrix B
  uint16_t numColsB = pSrcB->nColumns;
  // Number of columns of input matrix A
  uint16_t numColsA = pSrcA->nColumns;
  if ((pSrcA != NULL) && (pSrcB != NULL)) {
    if ((pSrcA->nColumns == pSrcB->nColumns) && (pSrcA->nRows == pSrcB->nRows)) {
      /* Set column size of buffers in the FIM module */
      RSI_FIM_SetDataLen(numColsA, 0, 0);
      // Set sat trunc values to FIM module for corresponding inputs
      RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
      // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcA->pData), ULPSS_RAM_ADDR_SRC1, ((pSrcA->nRows) * (pSrcA->nColumns)), FORMAT_Q31);
      // Store data in ulp memories
      RSI_FIM_InputData((pSrcB->pData), ULPSS_RAM_ADDR_SRC2, ((pSrcB->nRows) * (pSrcB->nColumns)), FORMAT_Q31);
      /* Start Address of 1st Input Data for FIM Operations */
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
      /* Start Address of 2nd Input Data for FIM Operations */
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
      /* Start Address of 1st Input Data for FIM Operations */
      FIM->FIM_INP1_ADDR_b.INP1_ADDR = ((uint32_t)pSrcA->pData);
      /* Start Address of 2nd Input Data for FIM Operations */
      FIM->FIM_INP2_ADDR_b.INP2_ADDR = ((uint32_t)pSrcB->pData);
      // Start Address of Output Data for FIM Operations
      FIM->FIM_OUT_ADDR_b.OUT_ADDR = ((uint32_t)pDst->pData);
#endif
      // Set the Mode of Operation to be performed.
      FIM->FIM_MODE_INTERRUPT = ((MAT_HADAMARD << 1) | 1);
      // Set the data type and  Trigger the FIM module
      RSI_FIM_DatTypTrig(numRowsA, numColsB, ULP_FIM_COP_DATA_REAL_REAL);
    } else {
      return ERROR_FIM_MATRIX_INVALID_ARG;
    }
  }
  return RSI_OK;
}

/*==============================================*/
/**
 *@fn       void rsi_arm_VSqrt_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize)
 *@brief     This API is used to set the FIM Q15 Square root for real number
 *@param[in]  *pSrc  points input for squaring a number
 *@param[in]   *pDst points to the block of output data 
 *@param[in]  blockSize  is size of the input array
 *@return     none
 */
void rsi_arm_VSqrt_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut2, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  /* Start Address of  lookup table Data for FIM Operations  */
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((SQ_ROOT << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_arm_log_q15(q15_t *pSrc, q15_t *pDst, uint16_t blockSize)
 *@brief     This API is used to set the FIM Q15 Natural Log Operation for real number
 *@param[in]  *pSrc  points input for squaring a number
 *@param[in]   *pDst points to the block of output data 
 *@param[in]  blockSize  is size of the input array
 *@return     none
 */
void rsi_arm_log_q15(q15_t *pSrc, q15_t *pDst, uint16_t blockSize)
{
  // Set data blockSize of buffers in the FIM module
  RSI_FIM_SetDataLen(NOT_MATRIX, blockSize, TRIG_LUT_LEN);
  // Set sat trunc values to FIM module for corresponding inputs
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));
#if ULPSS_MEMORY_WITH_M4_MEM_BUFFRS
  // Use application buffer which are in M4 RAM and copy them to ulpss RAM ,point ulpss RAM address to FIM
  // Store data in ulp memories
  RSI_FIM_InputData(pSrc, ULPSS_RAM_ADDR_SRC1, blockSize, FORMAT_Q15);
  // Store data in ulp memories
  RSI_FIM_InputData(Trig_lut2, ULPSS_RAM_ADDR_SRC2, TRIG_LUT_LEN, FORMAT_Q15);
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = ULPSS_RAM_ADDR_SRC1;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = ULPSS_RAM_ADDR_SRC2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = ULPSS_RAM_ADDR_DST;
#else
  // Start Address of 1st Input Data for FIM Operations
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = (uint32_t)pSrc;
  // Start Address of lookup table Data for FIM Operations
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = (uint32_t)Trig_lut2;
  // Start Address of Output Data for FIM Operations
  FIM->FIM_OUT_ADDR_b.OUT_ADDR = (uint32_t)pDst;
#endif
  // Set the Mode of Operation to be performed.
  FIM->FIM_MODE_INTERRUPT = ((NAT_LOG << 1) | 1);
  // Set the data type and  Trigger the FIM module
  RSI_FIM_DatTypTrig(NOT_MATRIX, NOT_MATRIX, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn        void rsi_enable_inst_buff(void)
 *@brief     This API is used to select instruction buffer for performing more than one operations at a time.
 *@return    none
 */
void rsi_enable_inst_buff(void)
{
  // Instruction buffer enable
  FIM->FIM_CONFIG_REG2_b.INSTR_BUFF_ENABLE = ENABLE;
}

#ifdef INST_BUFF_ENABLE
/*==============================================*/
/**
 *@fn        void rsi_fim_copy_fim_reg_to_ulp_memory(void)
 *@brief     This API is used to copy FIM configuration to ulpss memory when  instruction buffer is enabled .
 *@return    none
 */
void rsi_fim_copy_fim_reg_to_ulp_memory(void)
{
  static int reg_copy_count;
  memcpy((void *)(ULP_MEMORY_ADDR + (reg_copy_count * 9 * 4)), (const char *)FIM_BASE, 36);
  memset((void *)(ULP_MEMORY_ADDR + (((reg_copy_count + 1) * 0x24))), 0xFFFFFFFF, sizeof(int));
  reg_copy_count++;
}
#endif
#endif
