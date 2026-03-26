/***************************************************************************/ /**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
   ========================================== */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#define UNITY

#include "unity_conf.h"
#include "unity_internals.h"

#ifdef UNITY_SIM
#define printf                   print_info
#define UnityPrintf              print_info
#define UnityPrint               print_info
#define UnityPrintNumber         print_int
#define UnityPrintNumberUnsigned print_int
#define UnityPrintNumberHex      print_hex
void print_info(const char *str, ...);
#endif

#ifdef UNITY_RTL_SIMULATION
#include "unity_rtl_simulation.h"
#define printf                   print_info
#define UnityPrintf              print_info
#define UnityPrint               print_info
#define UnityPrintNumber         print_int
#define UnityPrintNumberUnsigned print_int
#define UnityPrintNumberHex      print_hex_rtl_simulation
void print_info(const char *str, ...);
#endif

#if defined(UNITY_FPRINTF_INCLUDE)
int Unityfprintf(FILE *fp, const char *format, ...);
#endif

//-------------------------------------------------------
// Configuration Options
//-------------------------------------------------------

// Integers
//     - Unity assumes 32 bit integers by default
//     - If your compiler treats ints of a different size, define UNITY_INT_WIDTH

// Floats
//     - define UNITY_EXCLUDE_FLOAT to disallow floating point comparisons
//     - define UNITY_FLOAT_PRECISION to specify the precision to use when doing TEST_ASSERT_EQUAL_FLOAT
//     - define UNITY_FLOAT_TYPE to specify doubles instead of single precision floats
//     - define UNITY_FLOAT_VERBOSE to print floating point values in errors (uses sprintf)

// Output
//     - by default, Unity prints to standard out with putchar.  define UNITY_OUTPUT_CHAR(a) with a different function if desired

// Optimization
//     - by default, line numbers are stored in unsigned shorts.  Define UNITY_LINE_TYPE with a different type if your files are huge
//     - by default, test and failure counters are unsigned shorts.  Define UNITY_COUNTER_TYPE with a different type if you want to save space or have more than 65535 Tests.

//-------------------------------------------------------
// Test Running Macros
//-------------------------------------------------------

#define TEST_PROTECT() (setjmp(UnityGet()->AbortFrame) == 0)

#define TEST_ABORT()                    \
  {                                     \
    longjmp(UnityGet()->AbortFrame, 1); \
  }

#ifndef RUN_TEST
#define RUN_TEST(func, line_num) UnityDefaultTestRun(func, #func, line_num);
#endif

extern struct _Unity *UnityGet(void);

#define UNITY_RUN_TEST_BEGIN(func, line_num)      \
  do {                                            \
    UnityGet()->CurrentTestName       = #func;    \
    UnityGet()->CurrentTestLineNumber = __LINE__; \
    UnityGet()->NumberOfTests++;                  \
  } while (0)

#define UNITY_RUN_TEST_END() UnityConcludeTest()

#define UNITY_RUN_TEST(func, params)              \
  do {                                            \
    UnityGet()->CurrentTestName       = #func;    \
    UnityGet()->CurrentTestLineNumber = __LINE__; \
    UnityGet()->NumberOfTests++;                  \
    if (TEST_PROTECT()) {                         \
      func params;                                \
    }                                             \
    UnityConcludeTest();                          \
  } while (0)

#define TEST_LINE_NUM   (UnityGet()->CurrentTestLineNumber)
#define TEST_IS_IGNORED (UnityGet()->CurrentTestIgnored)

#define TEST_CASE(...)

//-------------------------------------------------------
// Basic Fail and Ignore
//-------------------------------------------------------

#define TEST_FAIL_MESSAGE(message)   UNITY_TEST_FAIL(__LINE__, message)
#define TEST_FAIL()                  UNITY_TEST_FAIL(__LINE__, NULL)
#define TEST_IGNORE_MESSAGE(message) UNITY_TEST_IGNORE(__LINE__, message)
#define TEST_IGNORE()                UNITY_TEST_IGNORE(__LINE__, NULL)
#define TEST_ONLY()

//-------------------------------------------------------
// Test Asserts (simple)
//-------------------------------------------------------

//Boolean
#define TEST_ASSERT(condition)        UNITY_TEST_ASSERT((condition), __LINE__, " Expression Evaluated To FALSE")
#define TEST_ASSERT_TRUE(condition)   UNITY_TEST_ASSERT((condition), __LINE__, " Expected TRUE Was FALSE")
#define TEST_ASSERT_UNLESS(condition) UNITY_TEST_ASSERT(!(condition), __LINE__, " Expression Evaluated To TRUE")
#define TEST_ASSERT_FALSE(condition)  UNITY_TEST_ASSERT(!(condition), __LINE__, " Expected FALSE Was TRUE")
#define TEST_ASSERT_NULL(pointer)     UNITY_TEST_ASSERT_NULL((pointer), __LINE__, " Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer) UNITY_TEST_ASSERT_NOT_NULL((pointer), __LINE__, " Expected Non-NULL")

//Integers (of all sizes)
#define TEST_ASSERT_EQUAL_INT(expected, actual)   UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT8(expected, actual)  UNITY_TEST_ASSERT_EQUAL_INT8((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT16(expected, actual) UNITY_TEST_ASSERT_EQUAL_INT16((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT32(expected, actual) UNITY_TEST_ASSERT_EQUAL_INT32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT64(expected, actual) UNITY_TEST_ASSERT_EQUAL_INT64((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL(expected, actual)       UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_NOT_EQUAL(expected, actual) \
  UNITY_TEST_ASSERT(((expected) != (actual)), __LINE__, " Expected Not-Equal")
#define TEST_ASSERT_EQUAL_UINT(expected, actual)   UNITY_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT8(expected, actual)  UNITY_TEST_ASSERT_EQUAL_UINT8((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT16(expected, actual) UNITY_TEST_ASSERT_EQUAL_UINT16((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT32(expected, actual) UNITY_TEST_ASSERT_EQUAL_UINT32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT64(expected, actual) UNITY_TEST_ASSERT_EQUAL_UINT64((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX(expected, actual)    UNITY_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX8(expected, actual)   UNITY_TEST_ASSERT_EQUAL_HEX8((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX16(expected, actual)  UNITY_TEST_ASSERT_EQUAL_HEX16((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX32(expected, actual)  UNITY_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX64(expected, actual)  UNITY_TEST_ASSERT_EQUAL_HEX64((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS(mask, expected, actual)   UNITY_TEST_ASSERT_BITS((mask), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS_HIGH(mask, actual)        UNITY_TEST_ASSERT_BITS((mask), (_UU32)(-1), (actual), __LINE__, NULL)
#define TEST_ASSERT_BITS_LOW(mask, actual)         UNITY_TEST_ASSERT_BITS((mask), (_UU32)(0), (actual), __LINE__, NULL)
#define TEST_ASSERT_BIT_HIGH(bit, actual) \
  UNITY_TEST_ASSERT_BITS(((_UU32)1 << bit), (_UU32)(-1), (actual), __LINE__, NULL)
#define TEST_ASSERT_BIT_LOW(bit, actual) UNITY_TEST_ASSERT_BITS(((_UU32)1 << bit), (_UU32)(0), (actual), __LINE__, NULL)

// Integer Greater Than/ Less Than (of all sizes)
#define TEST_ASSERT_GREATER_THAN(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_INT64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_UINT64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_THAN_HEX64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_LESS_THAN(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_INT64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_UINT64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_THAN_HEX64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_GREATER_OR_EQUAL(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_INT64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX8(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX16(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX32(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX64(threshold, actual) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, NULL)

#define TEST_ASSERT_LESS_OR_EQUAL(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_INT64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_UINT64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX8(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX16(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX32(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, NULL)
#define TEST_ASSERT_LESS_OR_EQUAL_HEX64(threshold, actual) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, NULL)

//Integer Ranges (of all sizes)
#define TEST_ASSERT_INT_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_INT_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_INT8_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_INT8_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT16_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_INT16_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT32_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_INT32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_INT64_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_INT64_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_UINT_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_UINT8_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_UINT8_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT16_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_UINT16_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT32_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_UINT32_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_UINT64_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_UINT64_WITHIN((delta), (expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_HEX_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_HEX8_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_HEX16_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_HEX32_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_HEX64_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual, __LINE__, NULL)

//Structs and Strings
#define TEST_ASSERT_EQUAL_PTR(expected, actual)    UNITY_TEST_ASSERT_EQUAL_PTR((expected), (actual), __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING(expected, actual) UNITY_TEST_ASSERT_EQUAL_STRING(expected, actual, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) \
  UNITY_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len, __LINE__, NULL)

//Arrays
#define TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements, __LINE__, NULL)

//Arrays Compared To Single Value
#define TEST_ASSERT_EACH_EQUAL_INT(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT8(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT16(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT32(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_INT64(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT8(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT16(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT32(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_UINT64(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX8(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX8((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX16(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX16((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX32(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_HEX64(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX64((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_PTR(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_PTR((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_STRING(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_STRING((expected), (actual), (num_elements), __LINE__, NULL)
#define TEST_ASSERT_EACH_EQUAL_MEMORY(expected, actual, len, num_elements) \
  UNITY_TEST_ASSERT_EACH_EQUAL_MEMORY((expected), (actual), (len), (num_elements), __LINE__, NULL)

//Floating Point (If Enabled)
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) \
  UNITY_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual) UNITY_TEST_ASSERT_EQUAL_FLOAT(expected, actual, __LINE__, NULL)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements) \
  UNITY_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, __LINE__, NULL)

//-------------------------------------------------------
// Test Asserts (with additional messages)
//-------------------------------------------------------

//Boolean
#define TEST_ASSERT_MESSAGE(condition, message)        UNITY_TEST_ASSERT((condition), __LINE__, message)
#define TEST_ASSERT_TRUE_MESSAGE(condition, message)   UNITY_TEST_ASSERT((condition), __LINE__, message)
#define TEST_ASSERT_UNLESS_MESSAGE(condition, message) UNITY_TEST_ASSERT(!(condition), __LINE__, message)
#define TEST_ASSERT_FALSE_MESSAGE(condition, message)  UNITY_TEST_ASSERT(!(condition), __LINE__, message)
#define TEST_ASSERT_NULL_MESSAGE(pointer, message)     UNITY_TEST_ASSERT_NULL((pointer), __LINE__, message)
#define TEST_ASSERT_NOT_NULL_MESSAGE(pointer, message) UNITY_TEST_ASSERT_NOT_NULL((pointer), __LINE__, message)

//Integers (of all sizes)
#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT8((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT16((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT32((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT64((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, message)
#define TEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT(((expected) != (actual)), __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT8((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT16((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT32((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT64((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX8((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX16((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX32((expected), (actual), __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX64((expected), (actual), __LINE__, message)
#define TEST_ASSERT_BITS_MESSAGE(mask, expected, actual, message) \
  UNITY_TEST_ASSERT_BITS((mask), (expected), (actual), __LINE__, message)
#define TEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message) \
  UNITY_TEST_ASSERT_BITS((mask), (_UU32)(-1), (actual), __LINE__, message)
#define TEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, message) \
  UNITY_TEST_ASSERT_BITS((mask), (_UU32)(0), (actual), __LINE__, message)
#define TEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message) \
  UNITY_TEST_ASSERT_BITS(((_UU32)1 << bit), (_UU32)(-1), (actual), __LINE__, message)
#define TEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, message) \
  UNITY_TEST_ASSERT_BITS(((_UU32)1 << bit), (_UU32)(0), (actual), __LINE__, message)

//Integer Greater Than/ Less Than (of all sizes)
#define TEST_ASSERT_GREATER_THAN_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_INT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_UINT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_THAN_HEX64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_THAN_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_LESS_THAN_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_INT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_UINT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_THAN_HEX64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_THAN_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_INT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_GREATER_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, (message))

#define TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_INT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT64((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32((threshold), (actual), __LINE__, (message))
#define TEST_ASSERT_LESS_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message) \
  UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX64((threshold), (actual), __LINE__, (message))

//Integer Ranges (of all sizes)
#define TEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_INT_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_UINT_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_HEX_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual, __LINE__, message)

//Structs and Strings
#define TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_PTR(expected, actual, __LINE__, message)
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_STRING(expected, actual, __LINE__, message)
#define TEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, len, message) \
  UNITY_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len, __LINE__, message)

//Arrays
#define TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_STRING_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements, __LINE__, message)
#define TEST_ASSERT_EQUAL_MEMORY_ARRAY_MESSAGE(expected, actual, len, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements, __LINE__, message)

//Arrays Compared To Single Value
#define TEST_ASSERT_EACH_EQUAL_INT_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT8_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT16_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT32_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_INT64_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_INT64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT16_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT32_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_UINT64_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_UINT64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX8_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX8((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX16_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX16((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX32_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX32((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_HEX64_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_HEX64((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_PTR_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_PTR((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_STRING_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_STRING((expected), (actual), (num_elements), __LINE__, (message))
#define TEST_ASSERT_EACH_EQUAL_MEMORY_MESSAGE(expected, actual, len, num_elements, message) \
  UNITY_TEST_ASSERT_EACH_EQUAL_MEMORY((expected), (actual), (len), (num_elements), __LINE__, (message))

//Floating Point (If Enabled)
#define TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message) \
  UNITY_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, __LINE__, message)
#define TEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message) \
  UNITY_TEST_ASSERT_EQUAL_FLOAT(expected, actual, __LINE__, message)
#define TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, num_elements, message) \
  UNITY_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, __LINE__, message)

#ifdef __cplusplus
}
#endif

#endif
