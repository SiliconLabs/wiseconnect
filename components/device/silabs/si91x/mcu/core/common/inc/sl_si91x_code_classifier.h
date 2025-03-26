/***************************************************************************/ /**
 * @file sl_si91x_code_classifier.h
 * @brief 
 ***********Simple code classifier examples functions********************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _SL_SI91X_CODE_CLASSIFICATION_H_
#define _SL_SI91X_CODE_CLASSIFICATION_H_

/******************************************************************************/
/* Standard Code Classes for Memory Placement                                */
/******************************************************************************/
#define SL_CODE_CLASS_TIME_CRITICAL  timecritical
#define SL_CODE_CLASS_FORCE_RAM      force_ram
#define SL_CODE_CLASS_FORCE_PSRAM    force_psram
#define SL_CODE_CLASS_DMA_ACCESSIBLE dma_accessible

/******************************************************************************/
/* Helper Macros                                                              */
/******************************************************************************/
#define _SL_SI91X_CC_STRINGIZE(X)        #X
#define _SL_SI91X_CC_XSTRINGIZE(X)       _SL_SI91X_CC_STRINGIZE(X)
#define _SL_SI91X_CC_CONCAT3(A, B, C)    A B C
#define _SL_SI91X_CC_CONCAT4(A, B, C, D) A B C D

/******************************************************************************/
/* Compiler-Specific Macros                                                   */
/******************************************************************************/
#define _SL_SI91X_CC_SECTION(section_name, count, line) \
  __attribute__((section(                               \
    _SL_SI91X_CC_XSTRINGIZE(section_name) "_" _SL_SI91X_CC_XSTRINGIZE(count) "_" _SL_SI91X_CC_XSTRINGIZE(line))))
/******************************************************************************/
/* Macro for Assigning Data Variables (Arrays, Buffers, Functions) to Sections */
/******************************************************************************/

#define _SL_SI91X_SECTION_CONCAT1(segment, component, class)          segment##_##component##_##class
#define _SL_SI91X_SECTION_CONCAT2(segment, component, class1, class2) segment##_##component##_##class1##_##class2

#define _SL_SI91X_CLASS1(segment, component, class) \
  _SL_SI91X_CC_SECTION(_SL_SI91X_SECTION_CONCAT1(segment, component, class), __COUNTER__, __LINE__)

#define _SL_SI91X_CLASS2(segment, component, class1, class2) \
  _SL_SI91X_CC_SECTION(_SL_SI91X_SECTION_CONCAT2(segment, component, class1, class2), __COUNTER__, __LINE__)

#define _SL_SI91X_CC_COUNT_N(_1, _2, N, ...) N
#define _SL_SI91X_CC_COUNT(...)              _SL_SI91X_CC_COUNT_N(__VA_ARGS__, 2, 1)
#define _SL_SI91X_CC_IDENTITY(N)             N
#define _SL_SI91X_CC_APPLY(macro, ...)       _SL_SI91X_CC_IDENTITY(macro(__VA_ARGS__))

#define _SL_SI91X_CC_DISPATCH(N) _SL_SI91X_CLASS##N

/* Unified Macro for Classifying Functions and Variables */
#define SL_SI91X_CODE_CLASSIFY(segment, component, ...)                                             \
  _SL_SI91X_CC_IDENTITY(_SL_SI91X_CC_APPLY(_SL_SI91X_CC_DISPATCH, _SL_SI91X_CC_COUNT(__VA_ARGS__))) \
  (segment, component, __VA_ARGS__)

#endif // _SL_SI91X_CODE_CLASSIFICATION_H_