/*******************************************************************************
* @file  rsi_fim_filterScalar.c
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

#include "rsi_fim.h"

/*==============================================*/
/**
 *@fn          static void RSI_FIM_InputData(void *pSrcA, uint32_t bank, uint32_t blockSize, uint8_t data_type)
 *@brief       This API is used to store data in ulp memories
 *@param[in]   pReal is input vector
 *@param[in]   bank is in which memory data samples are stored
 *@param[in]   flag is set depending on complex-real and real-complex values
 *@param[in]   var is for address increment
 *@return      none
 **/
static void RSI_FIM_InputData(void *pSrcA, uint32_t bank, uint32_t blockSize, uint8_t data_type)
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
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA32;
        pSrcA32++;
      }
      break;

    case FORMAT_Q7:
      pSrcA8 = (q7_t *)pSrcA;
      for (var = 0; var < blockSize; var++) {
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA8;
        pSrcA8++;
      }
      break;

    case FORMAT_Q15:
      pSrcA16 = (q15_t *)pSrcA;

      for (var = 0; var < blockSize; var++) {
        *(volatile uint32_t *)((MEM_BANK + (bank << 2)) + (var * 4)) = *pSrcA16;

        pSrcA16++;
      }
      break;
  }
}

/*==============================================*/
/**
 *@fn          static void RSI_FIM_InputDataCmplx(volatile q15_t *pReal, uint32_t bank, volatile uint32_t var, uint8_t flag)
 *@brief       This API is used to store data in ulp memories
 *@param[in]   pReal is input vector
 *@param[in]   bank is in which memory data samples are stored
 *@param[in]   flag is set depending on complex-real and real-complex values
 *@param[in]   var is for address increment
 **/
static void RSI_FIM_InputDataCmplx(volatile q15_t *pReal, uint32_t bank, volatile uint32_t var, uint8_t flag)
{

  if (flag) {
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + ((var + 1) * 2)) = *pReal;
    pReal++;
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + (var)*2) = *pReal;
    pReal++;
  } else {
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + ((var + 1) * 2)) = *pReal;
    *(volatile uint16_t *)((MEM_BANK + (bank << 2)) + (var)*2)         = 0x0000;
  }
}

/*==============================================*/
/**
 *@fn         static void RSI_FIM_SetDataLen(uint32_t ColAorfilterLen, uint32_t bufferLen1, uint32_t bufferLen2)
 *@brief      This API is used to set data blockSize of buffers in the FIM module
 *@param[in]  ColAorfilterLen is used for configuring number of columns of first matrix or
 *            filter coefficients for fir , iir and fir interpolator filters.
 *@param[in]  bufferLen1 is the blockSize of data used for input1 
 *            for scalar only bufferLen1 need to be configured.
 *            for filter
 *@param[in]  bufferLen2 is the blockSize of data used for input2 i.e. only for vector operations this is used.
 **/
static void RSI_FIM_SetDataLen(uint32_t ColAorfilterLen, uint32_t bufferLen1, uint32_t bufferLen2)
{
  /* buf_len 1,2, filter len */
  FIM->FIM_CONFIG_REG1 = (bufferLen2 << 16) | (bufferLen1 << 6) | ColAorfilterLen;
}

/*==============================================*/
/**
 *@fn         static void RSI_FIM_DatTypTrig(uint32_t numRow1, uint32_t numCol2, typ_data_t typData)
 *@brief      This API is used to trigger the FIM module
 *@param[in]  numRow1 is number of rows of first matrix 
 *@param[in]  numCol2 is number of columns of second matrix 
 *@param[in]  typData is to select which type of data is given as input i.e. real-real , real-complex , complex-real and complex-complex
 *            In case of vectors, for real-complx, data in second memory is considered as real and 1st as cmplx.
 *                                 for complx-real, data in 1st memory is considered as real and second as cmplx.
 *             In case of scalar, for real-complx, data in memory is taken as complex and scalar as real.
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
 *@fn         static void rsi_fim_Iir_init_f32(fim_iir_instance_f32 *S,
 *                         uint16_t numStages,
 *                         int32_t *pCoeffs,
 *                         int32_t *pvCoeffs,
 *                         int32_t *pState)
 *@brief      This API is used to initialization function for the floating-point IIR lattice filter.
 *@param[in]	*S 	        points to an instance of the floating-point IIR lattice structure.
 *@param[in]	numStages	number of stages in the filter.
 *@param[in]	*pCoeffs	points to the reflection coefficient buffer. The array is of length numStages.
 *@param[in]	*pvCoeffs	points to the ladder coefficient buffer. The array is of length numStages+1.
 *@param[in]	*pState 	points to the state buffer. The array is of length numStages+blockSize.
 *@param[in]	blockSize	number of samples to process per call. 
 *@return       none
 */
void rsi_fim_Iir_init_f32(fim_iir_instance_f32 *S,
                          uint16_t numStages,
                          int32_t *pCoeffs,
                          int32_t *pvCoeffs,
                          int32_t *pState)
{
  // Assign filter taps
  S->numStages = numStages;
  // Assign coefficient pointer
  S->pkCoeffs = pCoeffs;
  // Assign coefficient pointer
  S->pvCoeffs                                    = pvCoeffs;
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = pvCoeffs[0];
  FIM->FIM_POLE_DATA2_b.POLE_DATA2               = pvCoeffs[1];
}

/*==============================================*/
/**
 *@fn         void rsi_fim_Iir_f32(fim_iir_instance_f32 *S,
 *                    int32_t *pSrc,
 *                    int32_t *pDst,
 *                    uint32_t blockSize,
 *                    uint16_t inBank1,
 *                    uint16_t inBank2,
 *                    uint16_t outBank)
 *@brief      This API is used to processing function for the floating-point IIR lattice filter
 *@param[in]	*S	points to an instance of the floating-point IIR lattice structure
 *@param[in]	*pSrc	points to the block of input data.
 *@param[out]	*pDst	points to the block of output data.
 *@param[in]	blockSize	number of input samples to process per call. 
 *@param[in]  inBank1 is to store input data samples
 *@param[in]  typ_data is to specify real-complex , complex-real or complex-complex data
 *@param[in]  inBank2 is to store input data samples
 *@param[in]  outBank stores output data samples
 *@return     none
 */
void rsi_fim_Iir_f32(fim_iir_instance_f32 *S,
                     int32_t *pSrc,
                     int32_t *pDst,
                     uint32_t blockSize,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank)
{
  RSI_FIM_SetSatTruncRound(STRS(0x9, 0, 0, 0));
  if (S->numStages > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numStages));
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pkCoeffs, inBank2, S->numStages, FORMAT_F32);
  } else {
    RSI_FIM_SetDataLen((S->numStages), (S->numStages), blockSize);
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_F32);
    RSI_FIM_InputData(S->pkCoeffs, inBank1, S->numStages, FORMAT_F32);
  }
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((IIR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          void rsi_fim_Iir_init_q31(fim_iir_instance_q31 *S,
 *                         uint16_t numStages,
 *                         q31_t *pCoeffs,
 *                         q31_t *pvCoeffs,
 *                         uint32_t *pState)
 *@brief         This API is used to initialization function for the Q31 IIR lattice filter
 *@param[in]	*S 	        points to an instance of the Q31 IIR lattice structure
 *@param[in]	numStages	number of stages in the filter.
 *@param[in]	*pCoeffs	points to the reflection coefficient buffer. The array is of length numStages.
 *@param[in]	*pvCoeffs	points to the ladder coefficient buffer. The array is of length numStages+1.
 *@param[in]	*pState 	points to the state buffer. The array is of length numStages+blockSize.
 *@param[in]	blockSize	number of samples to process per call. 
 *@return       none
 **/
void rsi_fim_Iir_init_q31(fim_iir_instance_q31 *S,
                          uint16_t numStages,
                          q31_t *pCoeffs,
                          q31_t *pvCoeffs,
                          uint32_t *pState)
{
  // Assign filter taps
  S->numStages = numStages;
  // Assign coefficient pointer
  S->pkCoeffs = pCoeffs;
  // Assign coefficient pointer
  S->pvCoeffs                                    = pvCoeffs;
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = pvCoeffs[0];
  FIM->FIM_POLE_DATA2_b.POLE_DATA2               = pvCoeffs[1];
}

/*==============================================*/
/**
 *@fn        void rsi_fim_Iir_q31(fim_iir_instance_q31 *S,
 *                    int32_t *pSrc,
 *                    q31_t *pDst,
 *                    uint32_t blockSize,
 *                    uint16_t inBank1,
 *                    uint16_t inBank2,
 *                    uint16_t outBank)
 *@brief      This API is used to processing function for the floating-point IIR lattice filter
 *@param[in]  *S 	points to an instance of the Q31 IIR lattice structure
 *@param[in]  *pSrc	points to the block of input data.
 *@param[out] *pDst	points to the block of output data.
 *@param[in]  blockSize	number of samples to process per call.
 *@param[in]  inBank1 is to store input data samples
 *@param[in]  inBank2 is to store input data samples
 *@param[in]  outBank stores output data samples
 *@return     none
 */
void rsi_fim_Iir_q31(fim_iir_instance_q31 *S,
                     int32_t *pSrc,
                     q31_t *pDst,
                     uint32_t blockSize,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank)
{

  RSI_FIM_SetSatTruncRound(STRS(0x1, 0, 0, 0));
  if (S->numStages > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numStages));
    RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pkCoeffs, inBank2, S->numStages, FORMAT_Q31);
  } else {
    RSI_FIM_SetDataLen((S->numStages), (S->numStages), blockSize);
    RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_Q31);
    RSI_FIM_InputData(S->pkCoeffs, inBank1, S->numStages, FORMAT_Q31);
  }
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((IIR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, ULP_FIM_COP_DATA_REAL_REAL);
}

/*==============================================*/
/**
 *@fn          void rsi_fim_Iir_init_q15(fim_iir_instance_q15 *S, uint16_t numStages, q15_t *pCoeffs, q15_t *pvCoeffs, q15_t *pState)
 *@brief        This API is used initialization function for the Q15 IIR lattice filter
 *@param[in]	*S  	        points to an instance of the Q15 IIR lattice structure. 
 *@param[in]	numStages	number of stages in the filter.
 *@param[in]	*pCoeffs	points to the reflection coefficient buffer. The array is of length numStages.
 *@param[in]	*pvCoeffs	points to the ladder coefficient buffer. The array is of length numStages+1.
 *@param[in]	*pState 	points to the state buffer. The array is of length numStages+blockSize.
 *@return       none
 */
void rsi_fim_Iir_init_q15(fim_iir_instance_q15 *S, uint16_t numStages, q15_t *pCoeffs, q15_t *pvCoeffs, q15_t *pState)
{
  // Assign filter taps
  S->numStages = numStages;
  // Assign coefficient pointer
  S->pkCoeffs = pCoeffs;
  // Assign coefficient pointer
  S->pvCoeffs                                    = pvCoeffs;
  FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = pvCoeffs[0];
  FIM->FIM_POLE_DATA2_b.POLE_DATA2               = pvCoeffs[1];
}

/*==============================================*/
/**
 *@fn          rsi_fim_Iir_q15(fim_iir_instance_q15 *S,
                     q15_t *pSrc,
                     q15_t *pDst,
                     uint32_t blockSize,
                     typ_data_t typ_data,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank)
 *@brief        This API is interrupt handler for  Q15 IIR lattice filter
 *@param[in]	*S  	    points to an instance of the Q15 IIR lattice structure. 
 *@param[in]	*pSrc	    Source pointer.
 *@param[in]	*pDst    	Destination pointer.
 *@param[in]	blocksize	No of blocks.
 *@param[in]	inBank1
 *@param[in]	inBank2
 *@outBank      outBank
 *@return       none
 */
void rsi_fim_Iir_q15(fim_iir_instance_q15 *S,
                     q15_t *pSrc,
                     q15_t *pDst,
                     uint32_t blockSize,
                     typ_data_t typ_data,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank)
{
  uint32_t i;
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));

  if (S->numStages > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numStages));

    switch (typ_data) {
      case ULP_FIM_COP_DATA_REAL_REAL:
        RSI_FIM_InputData(pSrc, inBank1, blockSize, FORMAT_Q15);
        RSI_FIM_InputData(S->pkCoeffs, inBank2, S->numStages, FORMAT_Q15);
        break;

      case ULP_FIM_COP_DATA_REAL_CPLX:

        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 0);
          pSrc++;
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank2, i * 2, 1);
          S->pkCoeffs++;
          S->pkCoeffs++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_REAL:
        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank2, i * 2, 0);
          S->pkCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
          pSrc++;
          pSrc++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_CPLX:
        for (i = 0; i < blockSize; i++) {
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank2, i * 2, 1);
          S->pkCoeffs++;
          S->pkCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
          pSrc++;
          pSrc++;
        }

        break;
    }

  } else {
    RSI_FIM_SetDataLen((S->numStages), (S->numStages), blockSize);

    switch (typ_data) {
      case ULP_FIM_COP_DATA_REAL_REAL:
        RSI_FIM_InputData(pSrc, inBank2, blockSize, FORMAT_Q15);
        RSI_FIM_InputData(S->pkCoeffs, inBank1, S->numStages, FORMAT_Q15);
        break;
      case ULP_FIM_COP_DATA_CPLX_REAL:

        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 0);
          pSrc++;
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank1, i * 2, 1);
          S->pkCoeffs++;
          S->pkCoeffs++;
        }
        break;

      case ULP_FIM_COP_DATA_REAL_CPLX:
        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank1, i * 2, 0);
          S->pkCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 1);
          pSrc++;
          pSrc++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_CPLX:
        for (i = 0; i < blockSize; i++) {
          RSI_FIM_InputDataCmplx(S->pkCoeffs, inBank1, i * 2, 1);
          S->pkCoeffs++;
          S->pkCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 1);
          pSrc++;
          pSrc++;
        }
    }
  }

  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((IIR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, typ_data);
}

/*==============================================*/
/**  
 *@fn          void rsi_fim_scalar_sub_q15(q15_t *pSrc,
 *                           q15_t *scale,
 *                           q15_t *pDst,
 *                           uint32_t blockSize,
 *                           typ_data_t typ_data,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief       This API is used to set the FIM Scalar substraction for real data
 *@param[in]   pSrc is input vector A
 *@param[in]   scale is constant value that need to be added to each elements of vector array.
 *@param[in]   blockSize is size of the input array
 *@param[in]   typ_data is to specify real-complex , complex-real or complex-complex data
 *@param[in]   inBank is to store input data samples
 *@param[in]   outBank stores output data samples
 *@param[out]  pDst  points to output vector
 *@return      none
 **/
void rsi_fim_scalar_sub_q15(q15_t *pSrc,
                            q15_t *scale,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank)
{
  uint32_t i;
  q15_t scalarReal;

  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_REAL_REAL:
      RSI_FIM_SetDataLen(0, blockSize, 0);
      RSI_FIM_InputData(pSrc, inBank, blockSize, FORMAT_Q15);
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = *scale;
      break;

    case ULP_FIM_COP_DATA_CPLX_REAL:
      RSI_FIM_SetDataLen(0, blockSize / 2, 0);
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
        pSrc++;
        pSrc++;
      }
      scalarReal                                     = *scale;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | 0x0000;
      break;

    case ULP_FIM_COP_DATA_REAL_CPLX:
      RSI_FIM_SetDataLen(0, blockSize / 2, 0);
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 0);
        pSrc++;
      }
      scalarReal = *scale;
      scale++;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | *scale;
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      RSI_FIM_SetDataLen(0, blockSize / 2, 0);
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
        pSrc++;
        pSrc++;
      }
      scalarReal = *scale;
      scale++;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | *scale;

      break;
  }

  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((SUB_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, typ_data);
}

/*==============================================*/
/**
 *@fn         void rsi_fim_scalar_add_q15(q15_t *pSrc,
 *                           q15_t *scale,
 *                           uint32_t blockSize,
 *                           typ_data_t typ_data,
 *                           uint16_t inBank,
 *                           uint16_t outBank)
 *@brief       This API is used to set the FIM Scalar substraction for real data
 *@param[in]   pSrc is input vector A
 *@param[in]   scale is constant value that need to be added to each elements of vector array.
 *@param[in]   blockSize is size of the input array
 *@param[in]   typ_data is to specify real-complex , complex-real or complex-complex data
 *@param[in]   inBank is to store input data samples
 *@param[in]   outBank stores output data samples
 *@param[out]  pDst  points to output vector
 *@return      none
 **/
void rsi_fim_scalar_add_q15(q15_t *pSrc,
                            q15_t *scale,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank)
{
  uint32_t i;
  q15_t scalarReal;

  RSI_FIM_SetDataLen(0, blockSize / 2, 0);
  RSI_FIM_SetSatTruncRound(STRS(0x1F, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_CPLX_REAL:
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
        pSrc++;
        pSrc++;
      }
      scalarReal                                     = *scale;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | 0x0000;
      break;

    case ULP_FIM_COP_DATA_REAL_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 0);
        pSrc++;
      }
      scalarReal = *scale;
      scale++;
      FIM->FIM_SCALAR_POLE_DATA1_b.SCALAR_POLE_DATA1 = (scalarReal << 16) | *scale;
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      for (i = 0; i < blockSize / 2; i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank, i * 2, 1);
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

  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((ADD_SCALAR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, typ_data);
}

/*==============================================*/
/**
 *@fn          void rsi_fim_fir_interpolate_q15(arm_fir_interpolate_instance_q15_opt *S,
 *                                q15_t *pSrc,
 *                                q15_t *pDst,
 *                                uint32_t blockSize,
 *                                typ_data_t typ_data,
 *                                uint16_t inBank1,
 *                                uint16_t inBank2,
 *                                uint16_t outBank)
 *@brief        This API is used to Processing function for the Q15 FIR interpolator
 *@param[in]	*S	points to an instance of the Q15 FIR interpolator structure.
 *@param[in]	*pSrc	points to the block of input data.
 *@param[out]	*pDst	points to the block of output data.
 *@param[in]	blockSize	number of input samples to process per call. 
 *@param[in]    inBank1 is to store input data samples
 *@param[in]    inBank2 is to store input data samples
 *@param[in]    typ_data is to specify real-complex , complex-real or complex-complex data 
 *@param[in]    outBank stores output data samples
 *@return       none
 *
 *
 */
void rsi_fim_fir_interpolate_q15(arm_fir_interpolate_instance_q15_opt *S,
                                 q15_t *pSrc,
                                 q15_t *pDst,
                                 uint32_t blockSize,
                                 typ_data_t typ_data,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank)
{

  uint32_t numTaps, i;
  numTaps = (S->phaseLength) * (S->L);

  RSI_FIM_SetDataLen(numTaps, blockSize, numTaps);
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));

  switch (typ_data) {
    case ULP_FIM_COP_DATA_REAL_CPLX:

      for (i = 0; i < (blockSize); i++) {
        RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 0);
        pSrc++;
        RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 1);
        S->pCoeffs++;
        S->pCoeffs++;
      }
      break;

    case ULP_FIM_COP_DATA_CPLX_REAL:
      for (i = 0; i < (blockSize); i++) {
        RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 0);
        S->pCoeffs++;
        RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
        pSrc++;
        pSrc++;
      }
      break;

    case ULP_FIM_COP_DATA_CPLX_CPLX:
      for (i = 0; i < blockSize; i++) {
        RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 1);
        S->pCoeffs++;
        S->pCoeffs++;
        RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
        pSrc++;
        pSrc++;
      }
    default:
      break;
  }
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((INTERPOLATE << 1) | 1);
  FIM->FIM_CONFIG_REG2 |= (S->L) << 22;
  RSI_FIM_DatTypTrig(0, 0, typ_data);
}

/*==============================================*/
/**
 *@fn           void rsi_fim_fir_q15(arm_fir_instance_q15_opt *S,
 *                    q15_t *pSrc,
 *                    q15_t *pDst,
 *                    uint32_t blockSize,
 *                    typ_data_t typ_data,
 *                    uint16_t inBank1,
 *                    uint16_t inBank2,
 *                    uint16_t outBank)
 *@brief        This API is used to Processing function for the Q15 FIR interpolator
 *@param[in]	*S	points to an instance of the Q15 FIR interpolator structure.
 *@param[in]	*pSrc	points to the block of input data.
 *@param[out]	*pDst	points to the block of output data.
 *@param[in]	blockSize	number of input samples to process per call. 
 *@param[in]    inBank1 is to store input data samples
 *@param[in]    inBank2 is to store input data samples
 *@param[in]    typ_data is to specify real-complex , complex-real or complex-complex data 
 *@param[in]    outBank stores output data samples
 *@return       none
 */
void rsi_fim_fir_q15(arm_fir_instance_q15_opt *S,
                     q15_t *pSrc,
                     q15_t *pDst,
                     uint32_t blockSize,
                     typ_data_t typ_data,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank)
{
  uint32_t i;
  if (S->numTaps > blockSize) {
    RSI_FIM_SetDataLen(blockSize, blockSize, (S->numTaps));

    switch (typ_data) {
      case ULP_FIM_COP_DATA_REAL_CPLX:

        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 0);
          pSrc++;
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 1);
          S->pCoeffs++;
          S->pCoeffs++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_REAL:
        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 0);
          S->pCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
          pSrc++;
          pSrc++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_CPLX:
        for (i = 0; i < blockSize; i++) {
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank2, i * 2, 1);
          S->pCoeffs++;
          S->pCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank1, i * 2, 1);
          pSrc++;
          pSrc++;
        }

        break;
      default:
        break;
    }

  } else {
    RSI_FIM_SetDataLen((S->numTaps), (S->numTaps), blockSize);

    switch (typ_data) {
      case ULP_FIM_COP_DATA_CPLX_REAL:

        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 0);
          pSrc++;
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank1, i * 2, 1);
          S->pCoeffs++;
          S->pCoeffs++;
        }
        break;

      case ULP_FIM_COP_DATA_REAL_CPLX:
        for (i = 0; i < (blockSize); i++) {
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank1, i * 2, 0);
          S->pCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 1);
          pSrc++;
          pSrc++;
        }
        break;

      case ULP_FIM_COP_DATA_CPLX_CPLX:
        for (i = 0; i < blockSize; i++) {
          RSI_FIM_InputDataCmplx(S->pCoeffs, inBank1, i * 2, 1);
          S->pCoeffs++;
          S->pCoeffs++;
          RSI_FIM_InputDataCmplx(pSrc, inBank2, i * 2, 1);
          pSrc++;
          pSrc++;
        }
      default:
        break;
    }
  }
  RSI_FIM_SetSatTruncRound(STRS(0x11, 0, 0, 0));
  FIM->FIM_INP1_ADDR_b.INP1_ADDR = inBank1;
  FIM->FIM_INP2_ADDR_b.INP2_ADDR = inBank2;
  FIM->FIM_OUT_ADDR_b.OUT_ADDR   = outBank;
  FIM->FIM_MODE_INTERRUPT        = ((FIR << 1) | 1);
  RSI_FIM_DatTypTrig(0, 0, typ_data);
}
