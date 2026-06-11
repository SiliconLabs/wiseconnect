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

#ifndef UNITY_INTERNALS_H
#define UNITY_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(UNITY_APP_CONF)
#include UNITY_APP_CONF
#endif

#ifndef UNITY_EXCLUDE_STDIO_H
#include <stdio.h>
#endif

#ifndef UNITY_EXCLUDE_SETJMP_H
#include <setjmp.h>
#endif

#define UNITY_SUPPORT_64
//-------------------------------------------------------
// Int Support
//-------------------------------------------------------

#ifndef UNITY_INT_WIDTH
#define UNITY_INT_WIDTH (32)
#endif

#ifndef UNITY_LONG_WIDTH
#define UNITY_LONG_WIDTH (32)
#endif

#if (UNITY_INT_WIDTH == 32)
typedef unsigned char _UU8;
typedef unsigned short _UU16;
typedef unsigned int _UU32;
typedef signed char _US8;
typedef signed short _US16;
typedef signed int _US32;
#elif (UNITY_INT_WIDTH == 16)
typedef unsigned char _UU8;
typedef unsigned int _UU16;
typedef unsigned long _UU32;
typedef signed char _US8;
typedef signed int _US16;
typedef signed long _US32;
#else
#error Invalid UNITY_INT_WIDTH specified! (16 or 32 are supported)
#endif

#if (UNITY_INT_WIDTH == 32)
typedef unsigned char UNITY_UINT8;
typedef unsigned short UNITY_UINT16;
typedef unsigned int UNITY_UINT32;
typedef signed char UNITY_INT8;
typedef signed short UNITY_INT16;
typedef signed int UNITY_INT32;
#elif (UNITY_INT_WIDTH == 16)
typedef unsigned char UNITY_UINT8;
typedef unsigned int UNITY_UINT16;
typedef unsigned long UNITY_UINT32;
typedef signed char UNITY_INT8;
typedef signed int UNITY_INT16;
typedef signed long UNITY_INT32;
#else
#error Invalid UNITY_INT_WIDTH specified! (16 or 32 are supported)
#endif

//-------------------------------------------------------
// 64-bit Support
//-------------------------------------------------------

#ifndef UNITY_SUPPORT_64

//No 64-bit Support
typedef _UU32 _U_UINT;
typedef _US32 _U_SINT;
typedef UNITY_UINT32 UNITY_UINT;
typedef UNITY_INT32 UNITY_INT;

#else

//64-bit Support
#if (UNITY_LONG_WIDTH == 32)
typedef unsigned long long UNITY_UINT64;
typedef signed long long UNITY_INT64;
typedef unsigned long long _UU64;
typedef signed long long _US64;
#elif (UNITY_LONG_WIDTH == 64)
typedef unsigned long UNITY_UINT64;
typedef signed long UNITY_INT64;
typedef unsigned long _UU64;
typedef signed long _US64;
#else
#error Invalid UNITY_LONG_WIDTH specified! (32 or 64 are supported)
#endif
typedef UNITY_UINT64 UNITY_UINT;
typedef UNITY_INT64 UNITY_INT;
typedef _UU64 _U_UINT;
typedef _US64 _U_SINT;

#endif

//-------------------------------------------------------
// Pointer Support
//-------------------------------------------------------

#ifndef UNITY_POINTER_WIDTH
#define UNITY_POINTER_WIDTH (32)
#endif

#if (UNITY_POINTER_WIDTH == 32)
typedef _UU32 _UP;
#define UNITY_DISPLAY_STYLE_POINTER UNITY_DISPLAY_STYLE_HEX32
#elif (UNITY_POINTER_WIDTH == 64)
typedef _UU64 _UP;
#define UNITY_DISPLAY_STYLE_POINTER UNITY_DISPLAY_STYLE_HEX64
#elif (UNITY_POINTER_WIDTH == 16)
typedef _UU16 _UP;
#define UNITY_DISPLAY_STYLE_POINTER UNITY_DISPLAY_STYLE_HEX16
#else
#error Invalid UNITY_POINTER_WIDTH specified! (16, 32 or 64 are supported)
#endif

#ifndef UNITY_PTR_ATTRIBUTE
#define UNITY_PTR_ATTRIBUTE
#endif

#ifndef UNITY_INTERNAL_PTR
#define UNITY_INTERNAL_PTR UNITY_PTR_ATTRIBUTE const void *
#endif

//-------------------------------------------------------
// Float Support
//-------------------------------------------------------

#ifdef UNITY_EXCLUDE_FLOAT

//No Floating Point Support
#undef UNITY_FLOAT_PRECISION
#undef UNITY_FLOAT_TYPE
#undef UNITY_FLOAT_VERBOSE

#else

//Floating Point Support
#ifndef UNITY_FLOAT_PRECISION
#define UNITY_FLOAT_PRECISION (0.00001f)
#endif
#ifndef UNITY_FLOAT_TYPE
#define UNITY_FLOAT_TYPE float
#endif
typedef UNITY_FLOAT_TYPE _UF;

#endif

//-------------------------------------------------------
// Output Method
//-------------------------------------------------------

#ifndef UNITY_OUTPUT_CHAR
//Default to using putchar, which is defined in stdio.h above
#define UNITY_OUTPUT_CHAR(a) putchar(a)
#else
/* Commented out by Energy Micro.
 * (If defined as something else, make sure we declare it here so it's ready for
 * use)
 * extern int UNITY_OUTPUT_CHAR(int); <- This isn't compatible with LEOS! */
#endif

//-------------------------------------------------------
// Footprint
//-------------------------------------------------------

#ifndef UNITY_LINE_TYPE
#define UNITY_LINE_TYPE unsigned short
#endif

#ifndef UNITY_COUNTER_TYPE
#define UNITY_COUNTER_TYPE unsigned short
#endif

//-------------------------------------------------------
// Internal Structs Needed
//-------------------------------------------------------

typedef void (*UnityTestFunction)(void);

#define UNITY_DISPLAY_RANGE_INT  (0x10)
#define UNITY_DISPLAY_RANGE_UINT (0x20)
#define UNITY_DISPLAY_RANGE_HEX  (0x40)
#define UNITY_DISPLAY_RANGE_AUTO (0x80)

typedef enum {
  UNITY_DISPLAY_STYLE_INT   = 4 + UNITY_DISPLAY_RANGE_INT + UNITY_DISPLAY_RANGE_AUTO,
  UNITY_DISPLAY_STYLE_INT8  = 1 + UNITY_DISPLAY_RANGE_INT,
  UNITY_DISPLAY_STYLE_INT16 = 2 + UNITY_DISPLAY_RANGE_INT,
  UNITY_DISPLAY_STYLE_INT32 = 4 + UNITY_DISPLAY_RANGE_INT,
#ifdef UNITY_SUPPORT_64
  UNITY_DISPLAY_STYLE_INT64 = 8 + UNITY_DISPLAY_RANGE_INT,
#endif
  UNITY_DISPLAY_STYLE_UINT   = 4 + UNITY_DISPLAY_RANGE_UINT + UNITY_DISPLAY_RANGE_AUTO,
  UNITY_DISPLAY_STYLE_UINT8  = 1 + UNITY_DISPLAY_RANGE_UINT,
  UNITY_DISPLAY_STYLE_UINT16 = 2 + UNITY_DISPLAY_RANGE_UINT,
  UNITY_DISPLAY_STYLE_UINT32 = 4 + UNITY_DISPLAY_RANGE_UINT,
#ifdef UNITY_SUPPORT_64
  UNITY_DISPLAY_STYLE_UINT64 = 8 + UNITY_DISPLAY_RANGE_UINT,
#endif
  UNITY_DISPLAY_STYLE_HEX8  = 1 + UNITY_DISPLAY_RANGE_HEX,
  UNITY_DISPLAY_STYLE_HEX16 = 2 + UNITY_DISPLAY_RANGE_HEX,
  UNITY_DISPLAY_STYLE_HEX32 = 4 + UNITY_DISPLAY_RANGE_HEX,
#ifdef UNITY_SUPPORT_64
  UNITY_DISPLAY_STYLE_HEX64 = 8 + UNITY_DISPLAY_RANGE_HEX,
#endif
} UNITY_DISPLAY_STYLE_T;

typedef enum {
  UNITY_WITHIN           = 0x0,
  UNITY_EQUAL_TO         = 0x1,
  UNITY_GREATER_THAN     = 0x2,
  UNITY_GREATER_OR_EQUAL = 0x2 + UNITY_EQUAL_TO,
  UNITY_SMALLER_THAN     = 0x4,
  UNITY_SMALLER_OR_EQUAL = 0x4 + UNITY_EQUAL_TO,
  UNITY_UNKNOWN
} UNITY_COMPARISON_T;

typedef enum { UNITY_ARRAY_TO_VAL = 0, UNITY_ARRAY_TO_ARRAY, UNITY_ARRAY_UNKNOWN } UNITY_FLAGS_T;

struct _Unity {
  const char *TestFile;
  const char *CurrentTestName;
  _UU32 CurrentTestLineNumber;
  UNITY_COUNTER_TYPE NumberOfTests;
  UNITY_COUNTER_TYPE TestFailures;
  UNITY_COUNTER_TYPE TestIgnores;
  UNITY_COUNTER_TYPE CurrentTestFailed;
  UNITY_COUNTER_TYPE CurrentTestIgnored;
  jmp_buf AbortFrame;
};

typedef struct UnityThread_t {
  uint32_t threadId;
  struct _Unity Unity;
} UnityThread_t;

#ifndef UNITY_MULTI_THREADS_MAX
#define UNITY_MULTI_THREADS_MAX (1)
#endif

extern UnityThread_t UnityThreadTbl[UNITY_MULTI_THREADS_MAX];

struct UNITY_STORAGE_T {
  const char *TestFile;
  const char *CurrentTestName;
#ifndef UNITY_EXCLUDE_DETAILS
  const char *CurrentDetail1;
  const char *CurrentDetail2;
#endif
  UNITY_LINE_TYPE CurrentTestLineNumber;
  UNITY_COUNTER_TYPE NumberOfTests;
  UNITY_COUNTER_TYPE TestFailures;
  UNITY_COUNTER_TYPE TestIgnores;
  UNITY_COUNTER_TYPE CurrentTestFailed;
  UNITY_COUNTER_TYPE CurrentTestIgnored;
#ifdef UNITY_INCLUDE_EXEC_TIME
  UNITY_TIME_TYPE CurrentTestStartTime;
  UNITY_TIME_TYPE CurrentTestStopTime;
#endif
#ifndef UNITY_EXCLUDE_SETJMP_H
  jmp_buf AbortFrame;
#endif
};

extern struct UNITY_STORAGE_T Unity;

//-------------------------------------------------------
// Test Suite Management
//-------------------------------------------------------

void UnityBeginGroup(const char *TestFile);
void UnityBegin(void);
int UnityEnd(void);
void UnityConcludeTest(void);
void UnityDefaultTestRun(UnityTestFunction Func, const char *FuncName, const int FuncLineNum);

//-------------------------------------------------------
// Test Output
//-------------------------------------------------------

#define UNITY_PRINT_EOL      \
  {                          \
    UNITY_OUTPUT_CHAR('\n'); \
  }
void UnityPrint(const char *string);
int UnityPrintf(const char *format, ...);
void UnityPrintMask(const _U_UINT mask, const _U_UINT number);
void UnityPrintNumberByStyle(const _U_SINT number, const UNITY_DISPLAY_STYLE_T style);
void UnityPrintNumber(const _U_SINT number);
void UnityPrintNumberUnsigned(const _U_UINT number);
void UnityPrintNumberHex(const _U_UINT number, const char nibbles);

#ifdef UNITY_FLOAT_VERBOSE
void UnityPrintFloat(const _UF number);
#endif

//-------------------------------------------------------
// Test Assertion Fuctions
//-------------------------------------------------------
//  Use the macros below this section instead of calling
//  these directly. The macros have a consistent naming
//  convention and will pull in file and line information
//  for you.

void UnityAssertEqualNumber(const _U_SINT expected,
                            const _U_SINT actual,
                            const char *msg,
                            const UNITY_LINE_TYPE lineNumber,
                            const UNITY_DISPLAY_STYLE_T style);

void UnityAssertGreaterOrLessOrEqualNumber(const UNITY_INT threshold,
                                           const UNITY_INT actual,
                                           const UNITY_COMPARISON_T compare,
                                           const char *msg,
                                           const UNITY_LINE_TYPE lineNumber,
                                           const UNITY_DISPLAY_STYLE_T style);

void UnityAssertEqualIntArray(const _U_SINT *expected,
                              const _U_SINT *actual,
                              const _UU32 num_elements,
                              const char *msg,
                              const UNITY_LINE_TYPE lineNumber,
                              const UNITY_DISPLAY_STYLE_T style,
                              const UNITY_FLAGS_T flags);

void UnityAssertBits(const _U_SINT mask,
                     const _U_SINT expected,
                     const _U_SINT actual,
                     const char *msg,
                     const UNITY_LINE_TYPE lineNumber);

void UnityAssertEqualString(const char *expected,
                            const char *actual,
                            const char *msg,
                            const UNITY_LINE_TYPE lineNumber);

void UnityAssertEqualStringArray(const char **expected,
                                 const char **actual,
                                 const _UU32 num_elements,
                                 const char *msg,
                                 const UNITY_LINE_TYPE lineNumber);

void UnityAssertEqualMemory(const void *expected,
                            const void *actual,
                            const _UU32 length,
                            const _UU32 num_elements,
                            const char *msg,
                            const UNITY_LINE_TYPE lineNumber);

void UnityAssertNumbersWithin(const _U_SINT delta,
                              const _U_SINT expected,
                              const _U_SINT actual,
                              const char *msg,
                              const UNITY_LINE_TYPE lineNumber,
                              const UNITY_DISPLAY_STYLE_T style);

void UnityFail(const char *message, const UNITY_LINE_TYPE line);

void UnityIgnore(const char *message, const UNITY_LINE_TYPE line);

#ifndef UNITY_EXCLUDE_FLOAT
void UnityAssertFloatsWithin(const _UF delta,
                             const _UF expected,
                             const _UF actual,
                             const char *msg,
                             const UNITY_LINE_TYPE lineNumber);

void UnityAssertEqualFloatArray(const _UF *expected,
                                const _UF *actual,
                                const _UU32 num_elements,
                                const char *msg,
                                const UNITY_LINE_TYPE lineNumber);
#endif

/*-------------------------------------------------------
 * Helpers
 *-------------------------------------------------------*/

UNITY_INTERNAL_PTR UnityNumToPtr(const UNITY_INT num, const UNITY_UINT8 size);
#ifndef UNITY_EXCLUDE_FLOAT
UNITY_INTERNAL_PTR UnityFloatToPtr(const float num);
#endif
#ifndef UNITY_EXCLUDE_DOUBLE
UNITY_INTERNAL_PTR UnityDoubleToPtr(const double num);
#endif

/*-------------------------------------------------------
 * Test Running Macros
 *-------------------------------------------------------*/

#ifndef UNITY_BEGIN
#define UNITY_BEGIN() UnityBegin()
#endif

#ifndef UNITY_END
#define UNITY_END() UnityEnd()
#endif

//-------------------------------------------------------
// Basic Fail and Ignore
//-------------------------------------------------------

#if !defined(UNITY_TEST_FAIL)
#define UNITY_TEST_FAIL(line, message) UnityFail((message), (UNITY_LINE_TYPE)line);
#endif
#if !defined(UNITY_TEST_IGNORE)
#define UNITY_TEST_IGNORE(line, message) UnityIgnore((message), (UNITY_LINE_TYPE)line);
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_NUMBER)
#define UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, style) \
  UnityAssertEqualNumber((_U_SINT)(expected), (_U_SINT)(actual), (message), (UNITY_LINE_TYPE)line, style)
#endif

//-------------------------------------------------------
// Test Asserts
//-------------------------------------------------------

#define UNITY_TEST_ASSERT(condition, line, message)  \
  if (condition) {                                   \
  } else {                                           \
    UNITY_TEST_FAIL((UNITY_LINE_TYPE)line, message); \
  }
#define UNITY_TEST_ASSERT_NULL(pointer, line, message) \
  UNITY_TEST_ASSERT(((pointer) == NULL), (UNITY_LINE_TYPE)line, message)
#define UNITY_TEST_ASSERT_NOT_NULL(pointer, line, message) \
  UNITY_TEST_ASSERT(((pointer) != NULL), (UNITY_LINE_TYPE)line, message)

#if !defined(UNITY_TEST_ASSERT_EQUAL_INT)
#define UNITY_TEST_ASSERT_EQUAL_INT(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_INT)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_INT8)
#define UNITY_TEST_ASSERT_EQUAL_INT8(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_INT8)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_INT16)
#define UNITY_TEST_ASSERT_EQUAL_INT16(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_INT16)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_INT32)
#define UNITY_TEST_ASSERT_EQUAL_INT32(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_INT32)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_UINT)
#define UNITY_TEST_ASSERT_EQUAL_UINT(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_UINT)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_UINT8)
#define UNITY_TEST_ASSERT_EQUAL_UINT8(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_UINT8)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_UINT16)
#define UNITY_TEST_ASSERT_EQUAL_UINT16(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_UINT16)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_UINT32)
#define UNITY_TEST_ASSERT_EQUAL_UINT32(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_UINT32)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_HEX8)
#define UNITY_TEST_ASSERT_EQUAL_HEX8(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_HEX8)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_HEX16)
#define UNITY_TEST_ASSERT_EQUAL_HEX16(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_HEX16)
#endif
#if !defined(UNITY_TEST_ASSERT_EQUAL_HEX32)
#define UNITY_TEST_ASSERT_EQUAL_HEX32(expected, actual, line, message) \
  UNITY_TEST_ASSERT_EQUAL_NUMBER(expected, actual, line, message, UNITY_DISPLAY_STYLE_HEX32)
#endif

#define UNITY_TEST_ASSERT_BITS(mask, expected, actual, line, message) \
  UnityAssertBits((_U_SINT)(mask), (_U_SINT)(expected), (_U_SINT)(actual), (message), (UNITY_LINE_TYPE)line)

#define UNITY_TEST_ASSERT_GREATER_THAN_INT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),              \
                                        (UNITY_INT)(actual),                 \
                                        UNITY_GREATER_THAN,                  \
                                        (message),                           \
                                        (UNITY_LINE_TYPE)(line),             \
                                        UNITY_DISPLAY_STYLE_INT)
#define UNITY_TEST_ASSERT_GREATER_THAN_INT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT8)(threshold),   \
                                        (UNITY_INT)(UNITY_INT8)(actual),      \
                                        UNITY_GREATER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_INT8)
#define UNITY_TEST_ASSERT_GREATER_THAN_INT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT16)(threshold),   \
                                        (UNITY_INT)(UNITY_INT16)(actual),      \
                                        UNITY_GREATER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_INT16)
#define UNITY_TEST_ASSERT_GREATER_THAN_INT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT32)(threshold),   \
                                        (UNITY_INT)(UNITY_INT32)(actual),      \
                                        UNITY_GREATER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_INT32)
#define UNITY_TEST_ASSERT_GREATER_THAN_UINT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),               \
                                        (UNITY_INT)(actual),                  \
                                        UNITY_GREATER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_UINT)
#define UNITY_TEST_ASSERT_GREATER_THAN_UINT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT8)(actual),      \
                                        UNITY_GREATER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_UINT8)
#define UNITY_TEST_ASSERT_GREATER_THAN_UINT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT16)(actual),      \
                                        UNITY_GREATER_THAN,                     \
                                        (message),                              \
                                        (UNITY_LINE_TYPE)(line),                \
                                        UNITY_DISPLAY_STYLE_UINT16)
#define UNITY_TEST_ASSERT_GREATER_THAN_UINT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT32)(actual),      \
                                        UNITY_GREATER_THAN,                     \
                                        (message),                              \
                                        (UNITY_LINE_TYPE)(line),                \
                                        UNITY_DISPLAY_STYLE_UINT32)
#define UNITY_TEST_ASSERT_GREATER_THAN_HEX8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT8)(actual),     \
                                        UNITY_GREATER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_HEX8)
#define UNITY_TEST_ASSERT_GREATER_THAN_HEX16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT16)(actual),     \
                                        UNITY_GREATER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_HEX16)
#define UNITY_TEST_ASSERT_GREATER_THAN_HEX32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT32)(actual),     \
                                        UNITY_GREATER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_HEX32)

#define UNITY_TEST_ASSERT_SMALLER_THAN_INT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),              \
                                        (UNITY_INT)(actual),                 \
                                        UNITY_SMALLER_THAN,                  \
                                        (message),                           \
                                        (UNITY_LINE_TYPE)(line),             \
                                        UNITY_DISPLAY_STYLE_INT)
#define UNITY_TEST_ASSERT_SMALLER_THAN_INT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT8)(threshold),   \
                                        (UNITY_INT)(UNITY_INT8)(actual),      \
                                        UNITY_SMALLER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_INT8)
#define UNITY_TEST_ASSERT_SMALLER_THAN_INT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT16)(threshold),   \
                                        (UNITY_INT)(UNITY_INT16)(actual),      \
                                        UNITY_SMALLER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_INT16)
#define UNITY_TEST_ASSERT_SMALLER_THAN_INT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT32)(threshold),   \
                                        (UNITY_INT)(UNITY_INT32)(actual),      \
                                        UNITY_SMALLER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_INT32)
#define UNITY_TEST_ASSERT_SMALLER_THAN_UINT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),               \
                                        (UNITY_INT)(actual),                  \
                                        UNITY_SMALLER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_UINT)
#define UNITY_TEST_ASSERT_SMALLER_THAN_UINT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT8)(actual),      \
                                        UNITY_SMALLER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_UINT8)
#define UNITY_TEST_ASSERT_SMALLER_THAN_UINT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT16)(actual),      \
                                        UNITY_SMALLER_THAN,                     \
                                        (message),                              \
                                        (UNITY_LINE_TYPE)(line),                \
                                        UNITY_DISPLAY_STYLE_UINT16)
#define UNITY_TEST_ASSERT_SMALLER_THAN_UINT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),   \
                                        (UNITY_INT)(UNITY_UINT32)(actual),      \
                                        UNITY_SMALLER_THAN,                     \
                                        (message),                              \
                                        (UNITY_LINE_TYPE)(line),                \
                                        UNITY_DISPLAY_STYLE_UINT32)
#define UNITY_TEST_ASSERT_SMALLER_THAN_HEX8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT8)(actual),     \
                                        UNITY_SMALLER_THAN,                   \
                                        (message),                            \
                                        (UNITY_LINE_TYPE)(line),              \
                                        UNITY_DISPLAY_STYLE_HEX8)
#define UNITY_TEST_ASSERT_SMALLER_THAN_HEX16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT16)(actual),     \
                                        UNITY_SMALLER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_HEX16)
#define UNITY_TEST_ASSERT_SMALLER_THAN_HEX32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),  \
                                        (UNITY_INT)(UNITY_UINT32)(actual),     \
                                        UNITY_SMALLER_THAN,                    \
                                        (message),                             \
                                        (UNITY_LINE_TYPE)(line),               \
                                        UNITY_DISPLAY_STYLE_HEX32)

#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),                  \
                                        (UNITY_INT)(actual),                     \
                                        UNITY_GREATER_OR_EQUAL,                  \
                                        (message),                               \
                                        (UNITY_LINE_TYPE)(line),                 \
                                        UNITY_DISPLAY_STYLE_INT)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT8)(threshold),       \
                                        (UNITY_INT)(UNITY_INT8)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_INT8)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT16)(threshold),       \
                                        (UNITY_INT)(UNITY_INT16)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_INT16)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_INT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT32)(threshold),       \
                                        (UNITY_INT)(UNITY_INT32)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_INT32)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),                   \
                                        (UNITY_INT)(actual),                      \
                                        UNITY_GREATER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_UINT)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT8)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_UINT8)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT16)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                     \
                                        (message),                                  \
                                        (UNITY_LINE_TYPE)(line),                    \
                                        UNITY_DISPLAY_STYLE_UINT16)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_UINT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT32)(actual),          \
                                        UNITY_GREATER_OR_EQUAL,                     \
                                        (message),                                  \
                                        (UNITY_LINE_TYPE)(line),                    \
                                        UNITY_DISPLAY_STYLE_UINT32)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT8)(actual),         \
                                        UNITY_GREATER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_HEX8)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT16)(actual),         \
                                        UNITY_GREATER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_HEX16)
#define UNITY_TEST_ASSERT_GREATER_OR_EQUAL_HEX32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT32)(actual),         \
                                        UNITY_GREATER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_HEX32)

#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),                  \
                                        (UNITY_INT)(actual),                     \
                                        UNITY_SMALLER_OR_EQUAL,                  \
                                        (message),                               \
                                        (UNITY_LINE_TYPE)(line),                 \
                                        UNITY_DISPLAY_STYLE_INT)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT8)(threshold),       \
                                        (UNITY_INT)(UNITY_INT8)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_INT8)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT16)(threshold),       \
                                        (UNITY_INT)(UNITY_INT16)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_INT16)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_INT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_INT32)(threshold),       \
                                        (UNITY_INT)(UNITY_INT32)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_INT32)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(threshold),                   \
                                        (UNITY_INT)(actual),                      \
                                        UNITY_SMALLER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_UINT)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT8)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_UINT8)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT16)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                     \
                                        (message),                                  \
                                        (UNITY_LINE_TYPE)(line),                    \
                                        UNITY_DISPLAY_STYLE_UINT16)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_UINT32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),       \
                                        (UNITY_INT)(UNITY_UINT32)(actual),          \
                                        UNITY_SMALLER_OR_EQUAL,                     \
                                        (message),                                  \
                                        (UNITY_LINE_TYPE)(line),                    \
                                        UNITY_DISPLAY_STYLE_UINT32)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX8(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT8)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT8)(actual),         \
                                        UNITY_SMALLER_OR_EQUAL,                   \
                                        (message),                                \
                                        (UNITY_LINE_TYPE)(line),                  \
                                        UNITY_DISPLAY_STYLE_HEX8)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX16(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT16)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT16)(actual),         \
                                        UNITY_SMALLER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_HEX16)
#define UNITY_TEST_ASSERT_SMALLER_OR_EQUAL_HEX32(threshold, actual, line, message) \
  UnityAssertGreaterOrLessOrEqualNumber((UNITY_INT)(UNITY_UINT32)(threshold),      \
                                        (UNITY_INT)(UNITY_UINT32)(actual),         \
                                        UNITY_SMALLER_OR_EQUAL,                    \
                                        (message),                                 \
                                        (UNITY_LINE_TYPE)(line),                   \
                                        UNITY_DISPLAY_STYLE_HEX32)

#define UNITY_TEST_ASSERT_INT_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_U_SINT)(delta),                                 \
                           (_U_SINT)(expected),                              \
                           (_U_SINT)(actual),                                \
                           NULL,                                             \
                           (UNITY_LINE_TYPE)line,                            \
                           UNITY_DISPLAY_STYLE_INT)
#define UNITY_TEST_ASSERT_INT8_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU8)(delta),                                     \
                           (_U_SINT)(_US8)(expected),                         \
                           (_U_SINT)(_US8)(actual),                           \
                           (message),                                         \
                           (UNITY_LINE_TYPE)(line),                           \
                           UNITY_DISPLAY_STYLE_INT8)
#define UNITY_TEST_ASSERT_INT16_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU16)(delta),                                     \
                           (_U_SINT)(_US16)(expected),                         \
                           (_U_SINT)(_US16)(actual),                           \
                           (message),                                          \
                           (UNITY_LINE_TYPE)(line),                            \
                           UNITY_DISPLAY_STYLE_INT16)
#define UNITY_TEST_ASSERT_INT32_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU32)(delta),                                     \
                           (_U_SINT)(_US32)(expected),                         \
                           (_U_SINT)(_US32)(actual),                           \
                           (message),                                          \
                           (UNITY_LINE_TYPE)(line),                            \
                           UNITY_DISPLAY_STYLE_INT32)
#define UNITY_TEST_ASSERT_UINT_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_U_SINT)(delta),                                  \
                           (_U_SINT)(expected),                               \
                           (_U_SINT)(actual),                                 \
                           NULL,                                              \
                           (UNITY_LINE_TYPE)line,                             \
                           UNITY_DISPLAY_STYLE_UINT)
#define UNITY_TEST_ASSERT_UINT8_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU8)(delta),                                      \
                           (_U_SINT)(_U_UINT)(_UU8)(expected),                 \
                           (_U_SINT)(_U_UINT)(_UU8)(actual),                   \
                           (message),                                          \
                           (UNITY_LINE_TYPE)(line),                            \
                           UNITY_DISPLAY_STYLE_UINT8)
#define UNITY_TEST_ASSERT_UINT16_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU16)(delta),                                      \
                           (_U_SINT)(_U_UINT)(_UU16)(expected),                 \
                           (_U_SINT)(_U_UINT)(_UU16)(actual),                   \
                           (message),                                           \
                           (UNITY_LINE_TYPE)(line),                             \
                           UNITY_DISPLAY_STYLE_UINT16)
#define UNITY_TEST_ASSERT_UINT32_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_UU32)(delta),                                      \
                           (_U_SINT)(_U_UINT)(_UU32)(expected),                 \
                           (_U_SINT)(_U_UINT)(_UU32)(actual),                   \
                           (message),                                           \
                           (UNITY_LINE_TYPE)(line),                             \
                           UNITY_DISPLAY_STYLE_UINT32)
#define UNITY_TEST_ASSERT_HEX8_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_U_SINT)(delta),                                  \
                           (_U_SINT)(expected),                               \
                           (_U_SINT)(actual),                                 \
                           NULL,                                              \
                           (UNITY_LINE_TYPE)line,                             \
                           UNITY_DISPLAY_STYLE_HEX8)
#define UNITY_TEST_ASSERT_HEX16_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_U_SINT)(delta),                                   \
                           (_U_SINT)(expected),                                \
                           (_U_SINT)(actual),                                  \
                           NULL,                                               \
                           (UNITY_LINE_TYPE)line,                              \
                           UNITY_DISPLAY_STYLE_HEX16)
#define UNITY_TEST_ASSERT_HEX32_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((_U_SINT)(delta),                                   \
                           (_U_SINT)(expected),                                \
                           (_U_SINT)(actual),                                  \
                           NULL,                                               \
                           (UNITY_LINE_TYPE)line,                              \
                           UNITY_DISPLAY_STYLE_HEX32)

#define UNITY_TEST_ASSERT_EQUAL_PTR(expected, actual, line, message) \
  UnityAssertEqualNumber((_U_SINT)(_UP)(expected),                   \
                         (_U_SINT)(_UP)(actual),                     \
                         (message),                                  \
                         (UNITY_LINE_TYPE)line,                      \
                         UNITY_DISPLAY_STYLE_POINTER)
#define UNITY_TEST_ASSERT_EQUAL_STRING(expected, actual, line, message) \
  UnityAssertEqualString((const char *)(expected), (const char *)(actual), (message), (UNITY_LINE_TYPE)line)
#define UNITY_TEST_ASSERT_EQUAL_MEMORY(expected, actual, len, line, message) \
  UnityAssertEqualMemory((void *)(expected), (void *)(actual), (_UU32)(len), 1, (message), (UNITY_LINE_TYPE)line)

#define UNITY_TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                               \
                           (UNITY_INTERNAL_PTR)(actual),                                 \
                           (UNITY_UINT32)(num_elements),                                 \
                           (message),                                                    \
                           (UNITY_LINE_TYPE)(line),                                      \
                           UNITY_DISPLAY_STYLE_INT,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_INT8,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_INT16,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_INT32,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_UINT,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_UINT8,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                  \
                           (UNITY_INTERNAL_PTR)(actual),                                    \
                           (UNITY_UINT32)(num_elements),                                    \
                           (message),                                                       \
                           (UNITY_LINE_TYPE)(line),                                         \
                           UNITY_DISPLAY_STYLE_UINT16,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                  \
                           (UNITY_INTERNAL_PTR)(actual),                                    \
                           (UNITY_UINT32)(num_elements),                                    \
                           (message),                                                       \
                           (UNITY_LINE_TYPE)(line),                                         \
                           UNITY_DISPLAY_STYLE_UINT32,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_HEX8,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_HEX16,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_HEX32,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_PTR_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                               \
                           (UNITY_INTERNAL_PTR)(actual),                                 \
                           (UNITY_UINT32)(num_elements),                                 \
                           (message),                                                    \
                           (UNITY_LINE_TYPE)(line),                                      \
                           UNITY_DISPLAY_STYLE_POINTER,                                  \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualStringArray((UNITY_INTERNAL_PTR)(expected),                               \
                              (const char **)(actual),                                      \
                              (UNITY_UINT32)(num_elements),                                 \
                              (message),                                                    \
                              (UNITY_LINE_TYPE)(line))
#define UNITY_TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements, line, message) \
  UnityAssertEqualMemory((UNITY_INTERNAL_PTR)(expected),                                         \
                         (UNITY_INTERNAL_PTR)(actual),                                           \
                         (UNITY_UINT32)(len),                                                    \
                         (UNITY_UINT32)(num_elements),                                           \
                         (message),                                                              \
                         (UNITY_LINE_TYPE)(line))

#define UNITY_TEST_ASSERT_EACH_EQUAL_INT(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(expected), (UNITY_INT_WIDTH / 8)), \
                           (UNITY_INTERNAL_PTR)(actual),                                \
                           (UNITY_UINT32)(num_elements),                                \
                           (message),                                                   \
                           (UNITY_LINE_TYPE)(line),                                     \
                           UNITY_DISPLAY_STYLE_INT,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_INT8(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT8)(expected), 1),          \
                           (UNITY_INTERNAL_PTR)(actual),                                 \
                           (UNITY_UINT32)(num_elements),                                 \
                           (message),                                                    \
                           (UNITY_LINE_TYPE)(line),                                      \
                           UNITY_DISPLAY_STYLE_INT8,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_INT16(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT16)(expected), 2),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_INT16,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_INT32(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT32)(expected), 4),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_INT32,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_UINT(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(expected), (UNITY_INT_WIDTH / 8)),  \
                           (UNITY_INTERNAL_PTR)(actual),                                 \
                           (UNITY_UINT32)(num_elements),                                 \
                           (message),                                                    \
                           (UNITY_LINE_TYPE)(line),                                      \
                           UNITY_DISPLAY_STYLE_UINT,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_UINT8(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_UINT8)(expected), 1),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_UINT8,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_UINT16(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_UINT16)(expected), 2),          \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_UINT16,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_UINT32(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_UINT32)(expected), 4),          \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_UINT32,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_HEX8(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT8)(expected), 1),          \
                           (UNITY_INTERNAL_PTR)(actual),                                 \
                           (UNITY_UINT32)(num_elements),                                 \
                           (message),                                                    \
                           (UNITY_LINE_TYPE)(line),                                      \
                           UNITY_DISPLAY_STYLE_HEX8,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_HEX16(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT16)(expected), 2),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_HEX16,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_HEX32(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT32)(expected), 4),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_HEX32,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_PTR(expected, actual, num_elements, line, message)            \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_PTR_TO_INT)(expected), (UNITY_POINTER_WIDTH / 8)), \
                           (UNITY_INTERNAL_PTR)(actual),                                           \
                           (UNITY_UINT32)(num_elements),                                           \
                           (message),                                                              \
                           (UNITY_LINE_TYPE)(line),                                                \
                           UNITY_DISPLAY_STYLE_POINTER,                                            \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_STRING(expected, actual, num_elements, line, message) \
  UnityAssertEqualStringArray((UNITY_INTERNAL_PTR)(expected),                              \
                              (const char **)(actual),                                     \
                              (UNITY_UINT32)(num_elements),                                \
                              (message),                                                   \
                              (UNITY_LINE_TYPE)(line))
#define UNITY_TEST_ASSERT_EACH_EQUAL_MEMORY(expected, actual, len, num_elements, line, message) \
  UnityAssertEqualMemory((UNITY_INTERNAL_PTR)(expected),                                        \
                         (UNITY_INTERNAL_PTR)(actual),                                          \
                         (UNITY_UINT32)(len),                                                   \
                         (UNITY_UINT32)(num_elements),                                          \
                         (message),                                                             \
                         (UNITY_LINE_TYPE)(line))

#ifdef UNITY_SUPPORT_64
#define UNITY_TEST_ASSERT_EQUAL_INT64(expected, actual, line, message) \
  UnityAssertEqualNumber((_U_SINT)(expected),                          \
                         (_U_SINT)(actual),                            \
                         (message),                                    \
                         (UNITY_LINE_TYPE)line,                        \
                         UNITY_DISPLAY_STYLE_INT64)
#define UNITY_TEST_ASSERT_EQUAL_UINT64(expected, actual, line, message) \
  UnityAssertEqualNumber((_U_SINT)(expected),                           \
                         (_U_SINT)(actual),                             \
                         (message),                                     \
                         (UNITY_LINE_TYPE)line,                         \
                         UNITY_DISPLAY_STYLE_UINT64)
#define UNITY_TEST_ASSERT_EQUAL_HEX64(expected, actual, line, message) \
  UnityAssertEqualNumber((_U_SINT)(expected),                          \
                         (_U_SINT)(actual),                            \
                         (message),                                    \
                         (UNITY_LINE_TYPE)line,                        \
                         UNITY_DISPLAY_STYLE_HEX64)
#define UNITY_TEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_INT64,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                  \
                           (UNITY_INTERNAL_PTR)(actual),                                    \
                           (UNITY_UINT32)(num_elements),                                    \
                           (message),                                                       \
                           (UNITY_LINE_TYPE)(line),                                         \
                           UNITY_DISPLAY_STYLE_UINT64,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray((UNITY_INTERNAL_PTR)(expected),                                 \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_HEX64,                                      \
                           UNITY_ARRAY_TO_ARRAY)
#define UNITY_TEST_ASSERT_EACH_EQUAL_INT64(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT64)(expected), 8),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_INT64,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_UINT64(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_UINT64)(expected), 8),          \
                           (UNITY_INTERNAL_PTR)(actual),                                   \
                           (UNITY_UINT32)(num_elements),                                   \
                           (message),                                                      \
                           (UNITY_LINE_TYPE)(line),                                        \
                           UNITY_DISPLAY_STYLE_UINT64,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_EACH_EQUAL_HEX64(expected, actual, num_elements, line, message) \
  UnityAssertEqualIntArray(UnityNumToPtr((UNITY_INT)(UNITY_INT64)(expected), 8),          \
                           (UNITY_INTERNAL_PTR)(actual),                                  \
                           (UNITY_UINT32)(num_elements),                                  \
                           (message),                                                     \
                           (UNITY_LINE_TYPE)(line),                                       \
                           UNITY_DISPLAY_STYLE_HEX64,                                     \
                           UNITY_ARRAY_TO_VAL)
#define UNITY_TEST_ASSERT_UINT64_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((delta),                                             \
                           (_U_SINT)(expected),                                 \
                           (_U_SINT)(actual),                                   \
                           (message),                                           \
                           (UNITY_LINE_TYPE)(line),                             \
                           UNITY_DISPLAY_STYLE_UINT64)
#define UNITY_TEST_ASSERT_HEX64_WITHIN(delta, expected, actual, line, message) \
  UnityAssertNumbersWithin((delta),                                            \
                           (_U_SINT)(expected),                                \
                           (_U_SINT)(actual),                                  \
                           (message),                                          \
                           (UNITY_LINE_TYPE)(line),                            \
                           UNITY_DISPLAY_STYLE_HEX64)
#endif

#ifdef UNITY_EXCLUDE_FLOAT
#define UNITY_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, line, message) \
  UNITY_TEST_FAIL((UNITY_LINE_TYPE)line, "Unity Floating Point Disabled")
#define UNITY_TEST_ASSERT_EQUAL_FLOAT(expected, actual, line, message) \
  UNITY_TEST_FAIL((UNITY_LINE_TYPE)line, "Unity Floating Point Disabled")
#define UNITY_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, line, message) \
  UNITY_TEST_FAIL((UNITY_LINE_TYPE)line, "Unity Floating Point Disabled")
#else
#define UNITY_TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual, line, message) \
  UnityAssertFloatsWithin((_UF)(delta), (_UF)(expected), (_UF)(actual), (message), (UNITY_LINE_TYPE)line)
#define UNITY_TEST_ASSERT_EQUAL_FLOAT(expected, actual, line, message)         \
  UNITY_TEST_ASSERT_FLOAT_WITHIN((_UF)(expected) * (_UF)UNITY_FLOAT_PRECISION, \
                                 (_UF)expected,                                \
                                 (_UF)actual,                                  \
                                 (UNITY_LINE_TYPE)line,                        \
                                 message)
#define UNITY_TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements, line, message) \
  UnityAssertEqualFloatArray((_UF *)(expected),                                            \
                             (_UF *)(actual),                                              \
                             (_UU32)(num_elements),                                        \
                             (message),                                                    \
                             (UNITY_LINE_TYPE)line)
#endif

#ifdef __cplusplus
}
#endif

#endif
