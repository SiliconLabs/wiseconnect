/*******************************************************************************
* @file  rsi_fim.h
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

#include "base_types.h"
#include "rsi_error.h"
#include "rsi_ccp_common.h"

#ifdef ARM_MATH_DEF
#include "arm_math.h"
#endif

#ifndef RSI_FIM_H
#define RSI_FIM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef float float32_t;

// brief 8-bit fractional data type in 1.7 format.

typedef int8_t q7_t;

// brief 16-bit fractional data type in 1.15 format.

typedef int16_t q15_t;

// brief 32-bit fractional data type in 1.31 format.
typedef int32_t q31_t;

typedef enum {
  ULP_FIM_COP_DATA_REAL_REAL = 0,
  ULP_FIM_COP_DATA_CPLX_REAL,
  ULP_FIM_COP_DATA_REAL_CPLX,
  ULP_FIM_COP_DATA_CPLX_CPLX
} typ_data_t;

#define ULP_FIM_COP_START_TRIG 0x01
#define XMAX(x, y)             (((x) > (y)) ? (x) : (y))
#define MEM_BANK               0x24060000

// For 9117 FIM
#define ENHANCED_FIM_BANK0 0x24060000
#define ENHANCED_FIM_BANK1 0x24060800
#define ENHANCED_FIM_BANK2 0x24061000
#define ENHANCED_FIM_BANK3 0x24061800

#define ULPSS_RAM_ADDR_SRC ENHANCED_FIM_BANK0
#define ULPSS_RAM_ADDR_DST ENHANCED_FIM_BANK2

#define ULPSS_RAM_ADDR_SRC1 ENHANCED_FIM_BANK0
#define ULPSS_RAM_ADDR_SRC2 ENHANCED_FIM_BANK1
#define ULP_MEMORY_ADDR     0x24060000

// For 9116 FIM
#define BANK0                          0x00
#define BANK1                          (0x0800 >> 2)
#define BANK2                          (0x1000 >> 2)
#define BANK3                          (0x1800 >> 2)
#define BANK4                          (0x2000 >> 2)
#define BANK5                          (0x2800 >> 2)
#define BANK6                          (0x3000 >> 2)
#define BANK7                          (0x3800 >> 2)
#define STRS(sat, trunc, round, shift) ((round << 16) | (shift << 10) | (trunc << 5) | sat)

typedef enum mode {
  FIR         = 0x01,
  IIR         = 0x02,
  INTERPOLATE = 0x63,
  ADD_SCALAR  = 0x44,
  SUB_SCALAR  = 0x45,
  MUL_SCALAR  = 0x46,
  ADD_VECTOR  = 0x47,
  SUB_VECTOR  = 0x49,
  MUL_VECTOR  = 0x4A,
  NORM_SQUARE = 0xAB,
  MUL_MAT     = 0x4C,
#ifdef ENHANCED_FIM
  CORRELATION   = 0x3,
  DECIMATION    = 0x4,
  FFT           = 0x8,
  ADD_MAT       = 0x4D,
  SUB_MAT       = 0x4E,
  MAT_HADAMARD  = 0x4F,
  MAT_TRANSPOSE = 0x50,
  COR_SINE      = 0x51,
  COR_COSINE    = 0x52,
  COR_INV_TAN   = 0x54,
  COR_SINH      = 0x55,
  COR_COSH      = 0x56,
  COR_INV_TANH  = 0x57,
  SQ_ROOT       = 0x58,
  NAT_LOG       = 0x59,
#endif
} present_mode;

#define FORMAT_Q7  2
#define FORMAT_Q15 3
#define FORMAT_Q31 1
#define FORMAT_F32 0

#define NOT_MATRIX          0
#define M4SS_ADDR_SHIFT_VAL 2

// list of variables for matrix multiplication
typedef struct {
  int16_t nRows;
  int16_t nColumns;
  int32_t *pData;
} arm_matrix_instance_f32_opt;

typedef struct {
  uint16_t nRows;
  uint16_t nColumns;
  q31_t *pData;
} arm_matrix_instance_q31_opt;

typedef struct {
  int16_t nRows;
  int16_t nColumns;
  q15_t *pData;
} arm_matrix_instance_q15_opt;

// list of variables for fir filter
typedef struct {
  uint16_t numTaps;
  int32_t *pState;
  int32_t *pCoeffs;
} arm_fir_instance_f32_opt;

typedef struct {
  uint16_t numTaps;
  q31_t *pState;
  q31_t *pCoeffs;
} arm_fir_instance_q31_opt;

typedef struct {
  uint16_t numTaps;
  q15_t *pState;
  q15_t *pCoeffs;
} arm_fir_instance_q15_opt;

typedef struct {
  uint16_t numTaps;
  q7_t *pState;
  q7_t *pCoeffs;
} arm_fir_instance_q7_opt;

// list of variables for Iir filter
typedef struct {
  uint16_t numStages;
  int32_t *pState;
  int32_t *pkCoeffs;
  int32_t *pvCoeffs;
} fim_iir_instance_f32;

typedef struct {
  uint16_t numStages;
  q31_t *pState;
  q31_t *pkCoeffs;
  q31_t *pvCoeffs;
} fim_iir_instance_q31;

typedef struct {
  uint16_t numStages;
  q15_t *pState;
  q15_t *pkCoeffs;
  q15_t *pvCoeffs;
} fim_iir_instance_q15;

typedef struct {
  uint8_t L;            // upsample factor.
  uint16_t phaseLength; // length of each polyphase filter component.
  int32_t *pCoeffs;     // points to the coefficient array. The array is of length L*phaseLength.
  int32_t *pState;      // points to the state variable array. The array is of length phaseLength+numTaps-1.
} arm_fir_interpolate_instance_f32_opt;

#ifndef ARM_MATH_DEF
typedef enum {
  ARM_MATH_SUCCESS        = 0,  // No error
  ARM_MATH_ARGUMENT_ERROR = -1, // One or more arguments are incorrect
  ARM_MATH_LENGTH_ERROR   = -2, // Length of data buffer is incorrect
  ARM_MATH_SIZE_MISMATCH  = -3, // Size of matrices is not compatible with the operation.
  ARM_MATH_NANINF         = -4, // Not-a-number (NaN) or infinity is generated
  ARM_MATH_SINGULAR       = -5, // Generated by matrix inversion if the input matrix is singular and cannot be inverted.
  ARM_MATH_TEST_FAILURE   = -6  // Test Failed
} arm_status;
#endif

// brief Instance structure for the Q15 FIR interpolator.
typedef struct {
  uint8_t L;            // upsample factor.
  uint16_t phaseLength; // length of each polyphase filter component
  q15_t *pCoeffs;       // points to the coefficient array. The array is of length L*phaseLength.
  q15_t *pState;        // points to the state variable array. The array is of length blockSize+phaseLength-1.
} arm_fir_interpolate_instance_q15_opt;

// brief Instance structure for the Q31 FIR interpolator.
typedef struct {
  uint8_t L;            // upsample factor.
  uint16_t phaseLength; // length of each polyphase filter component.
  q31_t *pCoeffs;       // points to the coefficient array. The array is of length L*phaseLength.
  q31_t *pState;        // points to the state variable array. The array is of length blockSize+phaseLength-1.
} arm_fir_interpolate_instance_q31_opt;

#ifdef ENHANCED_FIM

// brief Instance structure for the Q15 FIR decimate.
typedef struct {
  uint8_t M;            // decimation factor..
  uint16_t numTaps;     // number of coefficients in the filter.
  const q15_t *pCoeffs; // points to the coefficient array. The array is of length numTaps.
  q15_t *pState;        // points to the state variable array. The array is of length numTaps+blockSize-1.
} arm_fir_decimate_instance_q15;

typedef struct {
  uint8_t M;            // decimation factor.
  uint16_t numTaps;     // number of coefficients in the filter.
  const q31_t *pCoeffs; // points to the coefficient array. The array is of length numTaps.
  q31_t *pState;        // points to the state variable array. The array is of length numTaps+blockSize-1.
} arm_fir_decimate_instance_q31;

typedef struct {
  uint16_t fftLen;              // length of the FFT.
  const q15_t *pTwiddle;        // points to the Twiddle factor table.
  const uint16_t *pBitRevTable; // points to the bit reversal table.
  uint16_t bitRevLength;        // bit reversal table length.
} arm_cfft_radix2_instance_q15;
#endif

// FIM Function Prototypes
void rsi_arm_offset_f32_opt(int32_t *pSrc,
                            int32_t scale,
                            int32_t *pDst,
                            uint32_t length,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_arm_offset_q7_opt(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t length, uint16_t inBank, uint16_t outBank);

void rsi_arm_offset_q15_opt(q15_t *pSrc, q15_t scale, q15_t *pDst, uint32_t length, uint16_t inBank, uint16_t outBank);

void rsi_arm_offset_q31_opt(q31_t *pSrc, q31_t scale, q31_t *pDst, uint32_t length, uint16_t inBank, uint16_t outBank);

void rsi_fim_scalar_add_q15(q15_t *pSrc,
                            q15_t *scale,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_fim_scalar_sub_q7(q7_t *pSrc, q7_t scale, q7_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank);

void rsi_fim_scalar_sub_q15(q15_t *pSrc,
                            q15_t *scale,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_fim_scalar_sub_q31(q31_t *pSrc,
                            q31_t scale,
                            q31_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_fim_scalar_sub_f32(int32_t *pSrc,
                            int32_t scale,
                            int32_t *pDst,
                            uint32_t length,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_arm_scale_f32_opt(int32_t *pSrc,
                           int32_t scale,
                           int32_t *pDst,
                           uint32_t length,
                           uint16_t inBank,
                           uint16_t outBank);

void rsi_arm_scale_q7_opt(q7_t *pSrc,
                          q7_t scaleFract,
                          int8_t shift,
                          q7_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank,
                          uint16_t outBank);

void rsi_arm_scale_q15_opt(q15_t *pSrc,
                           q15_t scaleFract,
                           int8_t shift,
                           q15_t *pDst,
                           uint32_t blockSize,
                           uint16_t inBank,
                           uint16_t outBank);

void rsi_arm_scale_q31_opt(q31_t *pSrc,
                           q31_t scaleFract,
                           int8_t shift,
                           q31_t *pDst,
                           uint32_t blockSize,
                           uint16_t inBank,
                           uint16_t outBank);

void rsi_fim_scalar_mul_q15(q15_t *pSrc,
                            q15_t *scale,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank,
                            uint16_t outBank);

void rsi_fim_interrupt_handler(volatile FIM_Type *ptFim);

void rsi_arm_add_f32_opt(int32_t *pSrcA,
                         int32_t *pSrcB,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_add_q7_opt(q7_t *pSrcA,
                        q7_t *pSrcB,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank);

void rsi_arm_add_q15_opt(q15_t *pSrcA,
                         q15_t *pSrcB,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_add_q31_opt(q31_t *pSrcA,
                         q31_t *pSrcB,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_fim_vector_add_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank);

void rsi_arm_sub_f32_opt(int32_t *pSrcA,
                         int32_t *pSrcB,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_sub_q7_opt(q7_t *pSrcA,
                        q7_t *pSrcB,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank);

void rsi_arm_sub_q15_opt(q15_t *pSrcA,
                         q15_t *pSrcB,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_sub_q31_opt(q31_t *pSrcA,
                         q31_t *pSrcB,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_fim_read_data(uint32_t bank, uint32_t length, volatile void *pDst, uint8_t data_type, typ_data_t type_data);

void rsi_fim_vector_sub_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            typ_data_t typ_data,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank);

void rsi_arm_mult_f32_opt(int32_t *pIn1,
                          int32_t *pIn2,
                          int32_t *pDst,
                          uint32_t SatTruncRound,
                          uint32_t length,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank);

void rsi_arm_mult_q7_opt(q7_t *pSrcA,
                         q7_t *pSrcB,
                         q7_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_mult_q15_opt(q15_t *pSrcA,
                          q15_t *pSrcB,
                          q15_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank);

void rsi_arm_mult_q31_opt(q31_t *pSrcA,
                          q31_t *pSrcB,
                          q31_t *pDst,
                          uint32_t blockSize,
                          uint16_t inBank1,
                          uint16_t inBank2,
                          uint16_t outBank);

void rsi_fim_vector_mul_q15(q15_t *pIn1,
                            q15_t *pIn2,
                            q15_t *pDst,
                            uint32_t blockSize,
                            uint16_t inBank1,
                            uint16_t inBank2,
                            uint16_t outBank);

void rsi_arm_cmplx_mult_real_q15_opt(q15_t *pSrcCmplx,
                                     q15_t *pSrcReal,
                                     q15_t *pDst,
                                     uint32_t numSamples,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank);

void rsi_arm_cmplx_mult_cmplx_q15_opt(q15_t *pSrcA,
                                      q15_t *pSrcB,
                                      q15_t *pDst,
                                      uint32_t numSamples,
                                      uint16_t inBank1,
                                      uint16_t inBank2,
                                      uint16_t outBank);

void rsi_arm_cmplx_mag_squared_q15_opt(q15_t *pSrc,
                                       q15_t *pDst,
                                       uint32_t numSamples,
                                       uint16_t inBank,
                                       uint16_t outBank);

void rsi_fim_absSqr_q7(q7_t *pSrc, q7_t *pDst1, uint32_t blockSize, uint16_t inBank, uint16_t outBank);
void rsi_fim_absSqr_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize, uint16_t inBank, uint16_t outBank);
void rsi_fim_absSqr_q31(q31_t *pSrc, q31_t *pDst, uint32_t length, uint16_t inBank, uint16_t outBank);
void rsi_fim_absSqr_f32(int32_t *pSrc, int32_t *pDst, uint32_t length, uint16_t inBank, uint16_t outBank);

error_t rsi_arm_mat_mult_f32_opt(const arm_matrix_instance_f32_opt *pSrcA,
                                 const arm_matrix_instance_f32_opt *pSrcB,
                                 arm_matrix_instance_f32_opt *pDst,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank);

error_t rsi_arm_mat_mult_q31_opt(const arm_matrix_instance_q31_opt *pSrcA,
                                 const arm_matrix_instance_q31_opt *pSrcB,
                                 arm_matrix_instance_q31_opt *pDst,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank);

error_t rsi_arm_mat_mult_q15_opt(const arm_matrix_instance_q15_opt *pSrcA,
                                 const arm_matrix_instance_q15_opt *pSrcB,
                                 arm_matrix_instance_q15_opt *pDst,
                                 q15_t *pState,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank);

void rsi_arm_fir_init_f32_opt(arm_fir_instance_f32_opt *S,
                              uint16_t numTaps,
                              int32_t *pCoeffs,
                              int32_t *pState,
                              uint32_t blockSize);

void rsi_arm_fir_f32_opt(arm_fir_instance_f32_opt *S,
                         int32_t *pSrc,
                         int32_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_fir_init_q31_opt(arm_fir_instance_q31_opt *S,
                              uint16_t numTaps,
                              q31_t *pCoeffs,
                              q31_t *pState,
                              uint32_t blockSize);

void rsi_arm_fir_q31_opt(arm_fir_instance_q31_opt *S,
                         q31_t *pSrc,
                         q31_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_arm_fir_init_q15_opt(arm_fir_instance_q15_opt *S,
                              uint16_t numTaps,
                              q15_t *pCoeffs,
                              q15_t *pState,
                              uint32_t blockSize);

void rsi_arm_fir_q15_opt(arm_fir_instance_q15_opt *S,
                         q15_t *pSrc,
                         q15_t *pDst,
                         uint32_t blockSize,
                         uint16_t inBank1,
                         uint16_t inBank2,
                         uint16_t outBank);

void rsi_fim_fir_q15(arm_fir_instance_q15_opt *S,
                     q15_t *pSrc,
                     q15_t *pDst,
                     uint32_t blockSize,
                     typ_data_t typ_data,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank);

void rsi_arm_fir_init_q7_opt(arm_fir_instance_q7_opt *S,
                             uint16_t numTaps,
                             q7_t *pCoeffs,
                             q7_t *pState,
                             uint32_t blockSize);

void rsi_arm_fir_q7_opt(arm_fir_instance_q7_opt *S,
                        q7_t *pSrc,
                        q7_t *pDst,
                        uint32_t blockSize,
                        uint16_t inBank1,
                        uint16_t inBank2,
                        uint16_t outBank);

void rsi_fim_Iir_init_f32(fim_iir_instance_f32 *S,
                          uint16_t numStages,
                          int32_t *pCoeffs,
                          int32_t *pvCoeffs,
                          int32_t *pState);

void rsi_fim_Iir_init_q31(fim_iir_instance_q31 *S,
                          uint16_t numStages,
                          q31_t *pCoeffs,
                          q31_t *pvCoeffs,
                          uint32_t *pState);

void rsi_fim_Iir_q31(fim_iir_instance_q31 *S,
                     int32_t *pSrc,
                     q31_t *pDst,
                     uint32_t blockSize,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank);

void rsi_fim_Iir_init_q15(fim_iir_instance_q15 *S, uint16_t numStages, q15_t *pCoeffs, q15_t *pvCoeffs, q15_t *pState);

void rsi_fim_Iir_q15(fim_iir_instance_q15 *S,
                     q15_t *pSrc,
                     q15_t *pDst,
                     uint32_t blockSize,
                     typ_data_t typ_data,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank);

void rsi_fim_Iir_f32(fim_iir_instance_f32 *S,
                     int32_t *pSrc,
                     int32_t *pDst,
                     uint32_t length,
                     uint16_t inBank1,
                     uint16_t inBank2,
                     uint16_t outBank);

void rsi_arm_fir_interpolate_f32_opt(const arm_fir_interpolate_instance_f32_opt *S,
                                     int32_t *pSrc,
                                     int32_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank);

arm_status rsi_arm_fir_interpolate_init_f32_opt(arm_fir_interpolate_instance_f32_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                int32_t *pCoeffs,
                                                int32_t *pState,
                                                uint32_t blockSize);

arm_status rsi_arm_fir_interpolate_init_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                q15_t *pCoeffs,
                                                q15_t *pState,
                                                uint32_t blockSize);

arm_status rsi_arm_fir_interpolate_init_q31_opt(arm_fir_interpolate_instance_q31_opt *S,
                                                uint8_t L,
                                                uint16_t numTaps,
                                                q31_t *pCoeffs,
                                                q31_t *pState,
                                                uint32_t blockSize);

void rsi_arm_fir_interpolate_q15_opt(arm_fir_interpolate_instance_q15_opt *S,
                                     q15_t *pSrc,
                                     q15_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank);

void rsi_fim_fir_interpolate_q15(arm_fir_interpolate_instance_q15_opt *S,
                                 q15_t *pSrc,
                                 q15_t *pDst,
                                 uint32_t blockSize,
                                 typ_data_t typ_data,
                                 uint16_t inBank1,
                                 uint16_t inBank2,
                                 uint16_t outBank);

void rsi_arm_fir_interpolate_q31_opt(const arm_fir_interpolate_instance_q31_opt *S,
                                     q31_t *pSrc,
                                     q31_t *pDst,
                                     uint32_t blockSize,
                                     uint16_t inBank1,
                                     uint16_t inBank2,
                                     uint16_t outBank);

void RSI_FIM_EnableSaturation(void);
void RSI_FIM_SetSatTruncRound(uint32_t SatTruncRoundShift);

// New feature
#ifdef ENHANCED_FIM
void rsi_arm_correlate_q15(const q15_t *pSrcA, uint32_t srcALen, const q15_t *pSrcB, uint32_t srcBLen, q15_t *pDst);
void rsi_arm_correlate_q31(const q31_t *pSrcA, uint32_t srcALen, const q31_t *pSrcB, uint32_t srcBLen, q31_t *pDst);
void rsi_arm_correlate_q7(const q7_t *pSrcA, uint32_t srcALen, const q7_t *pSrcB, uint32_t srcBLen, q7_t *pDst);
arm_status arm_fir_decimate_init_q31(arm_fir_decimate_instance_q31 *S,
                                     uint16_t numTaps,
                                     uint8_t M,
                                     q31_t *pCoeffs,
                                     q31_t *pState,
                                     uint32_t blockSize);
arm_status arm_fir_decimate_init_q15(arm_fir_decimate_instance_q15 *S,
                                     uint16_t numTaps,
                                     uint8_t M,
                                     q15_t *pCoeffs,
                                     q15_t *pState,
                                     uint32_t blockSize);
void arm_fir_decimate_q15(const arm_fir_decimate_instance_q15 *S, q15_t *pSrc, q15_t *pDst, uint32_t blockSize);
void arm_fir_decimate_q31(const arm_fir_decimate_instance_q31 *S, q31_t *pSrc, q31_t *pDst, uint32_t blockSize);
void rsi_arm_cfft_radix2(q31_t *pSrc);
void rsi_arm_sin_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
void rsi_arm_cos_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
void rsi_arm_Inverse_Tan_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
void rsi_arm_Sinh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
void rsi_arm_cosh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
void rsi_arm_Inverse_Tanh_q15(q15_t *pSrc, q15_t *pDst, uint32_t blocksize);
error_t rsi_arm_mat_add_q15(const arm_matrix_instance_q15_opt *pSrcA,
                            const arm_matrix_instance_q15_opt *pSrcB,
                            arm_matrix_instance_q15_opt *pDst);
error_t arm_mat_add_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst);
error_t arm_mat_sub_q15(const arm_matrix_instance_q15_opt *pSrcA,
                        const arm_matrix_instance_q15_opt *pSrcB,
                        arm_matrix_instance_q15_opt *pDst);
error_t arm_mat_sub_q31(const arm_matrix_instance_q31_opt *pSrcA,
                        const arm_matrix_instance_q31_opt *pSrcB,
                        arm_matrix_instance_q31_opt *pDst);
error_t arm_mat_trans_q15(const arm_matrix_instance_q15_opt *pSrc, arm_matrix_instance_q15_opt *pDst);
error_t rsi_arm_mat_trans_q31(const arm_matrix_instance_q31_opt *pSrc, arm_matrix_instance_q31_opt *pDst);
error_t rsi_arm_mat_Hadamard_prod_q15(const arm_matrix_instance_q15_opt *pSrcA,
                                      const arm_matrix_instance_q15_opt *pSrcB,
                                      arm_matrix_instance_q15_opt *pDst);
error_t arm_mat_Hadamard_prod_q31(const arm_matrix_instance_q31_opt *pSrcA,
                                  const arm_matrix_instance_q31_opt *pSrcB,
                                  arm_matrix_instance_q31_opt *pDst);
void rsi_arm_VSqrt_q15(q15_t *pSrc, q15_t *pDst, uint32_t blockSize);
void rsi_arm_log_q15(q15_t *pSrc, q15_t *pDst, uint16_t blockSize);
void rsi_enable_inst_buff(void);
void RSI_FIM_InputData(void *pSrcA, uint32_t bank, uint32_t blockSize, uint8_t data_type);
void rsi_fim_copy_fim_reg_to_ulp_memory(void);
#endif

#ifdef __cplusplus
}
#endif
#endif // RSI_FIM_H
