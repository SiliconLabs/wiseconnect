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

#include "unity.h"
//#include "em_int.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if !defined(UNITY_FAIL_AND_BAIL)
#define UNITY_FAIL_AND_BAIL             \
  {                                     \
    UnityGet()->CurrentTestFailed = 1;  \
    UNITY_OUTPUT_CHAR('\n');            \
    longjmp(UnityGet()->AbortFrame, 1); \
  }
#endif
#if !defined(UNITY_IGNORE_AND_BAIL)
#define UNITY_IGNORE_AND_BAIL           \
  {                                     \
    UnityGet()->CurrentTestIgnored = 1; \
    UNITY_OUTPUT_CHAR('\n');            \
    longjmp(UnityGet()->AbortFrame, 1); \
  }
#endif
/// return prematurely if we are already in failure or ignore state
#if !defined(UNITY_SKIP_EXECUTION)
#define UNITY_SKIP_EXECUTION                                                             \
  {                                                                                      \
    if ((UnityGet()->CurrentTestFailed != 0) || (UnityGet()->CurrentTestIgnored != 0)) { \
      return;                                                                            \
    }                                                                                    \
  }
#endif
#ifdef UNITY_OUTPUT_TEXT_COLOR
#define UNITY_PRINT_TEXT_COLOR(arg) UnityPrint(arg)
#else
#define UNITY_PRINT_TEXT_COLOR(arg)
#endif

#if defined(UNITY_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

#if defined(UNITY_MICRIUM)
#include "em_assert.h"
#include <kernel/include/os.h>
#endif

struct UNITY_STORAGE_T Unity;
UnityThread_t UnityThreadTbl[UNITY_MULTI_THREADS_MAX];

#if defined(UNITY_FREERTOS)

uint32_t UnityThreadIdGet(void)
{
  return (uint32_t)xTaskGetCurrentTaskHandle();
}
static SemaphoreHandle_t unityPrintMutex = NULL;
static uint32_t unityPrintRegionLockCnt  = 0;
static uint32_t unityPrintRegionOwner    = 0;

void UnityPrintRegionEnter(void)
{
  if (NULL == unityPrintMutex) {
    /* Create mutex which will serve as unity print region lock. */
    unityPrintMutex = xSemaphoreCreateMutex();
  }
  if (unityPrintRegionOwner != UnityThreadIdGet()) {
    xSemaphoreTake(unityPrintMutex, (TickType_t)1000);
  }

  unityPrintRegionOwner = UnityThreadIdGet();

  if (unityPrintRegionLockCnt < UINT32_MAX) {
    unityPrintRegionLockCnt++;
  }
}
void UnityPrintRegionExit(void)
{
  if (unityPrintRegionLockCnt > 0) {
    if (--unityPrintRegionLockCnt == 0) {
      unityPrintRegionOwner = 0;
      xSemaphoreGive(unityPrintMutex);
    }
  }
}

#elif defined(UNITY_MICRIUM)

uint32_t UnityThreadIdGet(void)
{
  return (uint32_t)OSTCBCurPtr;
}
static OS_MUTEX unityPrintMutex;
;
static OS_MUTEX *pUnityPrintRegionMutex = NULL;
static uint32_t unityPrintRegionLockCnt = 0;
static uint32_t unityPrintRegionOwner   = 0;

void UnityPrintRegionEnter(void)
{
  RTOS_ERR err;
  if (NULL == pUnityPrintRegionMutex) {
    /* Create and initialize semaphore value to 1. */
    OSMutexCreate(&unityPrintMutex, "UnityPrintMutex", &err);
    pUnityPrintRegionMutex = &unityPrintMutex;
    EFM_ASSERT(err.Code == RTOS_ERR_NONE);
  }
  if (unityPrintRegionOwner != UnityThreadIdGet()) {
    OSMutexPend(pUnityPrintRegionMutex, (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &err);
    EFM_ASSERT(err.Code == RTOS_ERR_NONE);
  }
  unityPrintRegionOwner = UnityThreadIdGet();

  if (unityPrintRegionLockCnt < UINT32_MAX) {
    unityPrintRegionLockCnt++;
  }
}
void UnityPrintRegionExit(void)
{
  if (unityPrintRegionLockCnt > 0) {
    if (--unityPrintRegionLockCnt == 0) {
      RTOS_ERR err;
      unityPrintRegionOwner = 0;
      OSMutexPost(pUnityPrintRegionMutex, OS_OPT_POST_NONE, &err);
      EFM_ASSERT(err.Code == RTOS_ERR_NONE);
    }
  }
}

#else /* #if defined( UNITY_FREERTOS ) */

uint32_t UnityThreadIdGet(void)
{
  return 1;
}
void UnityPrintRegionEnter(void)
{
}
void UnityPrintRegionExit(void)
{
}

#endif /* #if defined( UNITY_FREERTOS ) */

#if UNITY_MULTI_THREADS_MAX > 1
static UnityThread_t *UnityThreadGet(void)
{
  int i;
  uint32_t threadId = UnityThreadIdGet();

  for (i = 0; i < UNITY_MULTI_THREADS_MAX; i++) {
    if (UnityThreadTbl[i].threadId == threadId) {
      return &UnityThreadTbl[i];
    } else {
      if (UnityThreadTbl[i].threadId == 0) {
        UnityThreadTbl[i].threadId = threadId;
        memset(&UnityThreadTbl[i].Unity, 0, sizeof(struct _Unity));
        return &UnityThreadTbl[i];
      }
    }
  }
  /* We have run out of Unity structs. Consider to increase
     UNITY_MULTI_THREADS_MAX. */
  while (1)
    ;
}

struct _Unity *UnityGet(void)
{
  UnityThread_t *pUnityThread = UnityThreadGet();
  return &pUnityThread->Unity;
}

#else /* UNITY_MULTI_THREADS_MAX > 1 */

struct _Unity *UnityGet(void)
{
  return &UnityThreadTbl[0].Unity;
}

#endif /* UNITY_MULTI_THREADS_MAX > 1 */

const char *UnityStrNull                   = "NULL";
const char *UnityStrSpacer                 = ". ";
const char *UnityStrExpected               = " Expected ";
const char *UnityStrWas                    = " Was ";
const char *UnityStrGt                     = " to be greater than ";
const char *UnityStrLt                     = " to be less than ";
const char *UnityStrOrEqual                = "or equal to ";
const char *UnityStrTo                     = " To ";
const char *UnityStrElement                = " Element ";
const char *UnityStrMemory                 = " Memory Mismatch";
const char *UnityStrDelta                  = " Values Not Within Delta ";
const char *UnityStrPointless              = " You Asked Me To Compare Nothing, Which Was Pointless.";
const char *UnityStrNullPointerForExpected = " Expected pointer to be NULL";
const char *UnityStrNullPointerForActual   = " Actual pointer was NULL";

void UnityTestResultsBegin(const char *file, const UNITY_LINE_TYPE line);
void UnityTestResultsFailBegin(const UNITY_LINE_TYPE line);
void UnityAddMsgIfSpecified(const char *msg);
void UnityPrintExpectedAndActualStrings(const char *expected, const char *actual);
int UnityCheckArraysForNull(const void *expected,
                            const void *actual,
                            const UNITY_LINE_TYPE lineNumber,
                            const char *msg);

//-----------------------------------------------
// Pretty Printers & Test Result Output Handlers
//-----------------------------------------------
#if !defined(UNITY_SIM) && !defined(UNITY_RTL_SIMULATION)
void UnityPrint(const char *string)
{
  const char *pch = string;

  UnityPrintRegionEnter();

  if (pch != NULL) {
    while (*pch) {
      // printable characters plus CR & LF are printed
      if ((*pch <= 126) && (*pch >= 32)) {
        UNITY_OUTPUT_CHAR(*pch);
      }
      //write escaped carriage returns
      else if (*pch == 13) {
        UNITY_OUTPUT_CHAR('\\');
        UNITY_OUTPUT_CHAR('r');
      }
      //write escaped line feeds
      else if (*pch == 10) {
        UNITY_OUTPUT_CHAR('\\');
        UNITY_OUTPUT_CHAR('n');
      }
      //write escape character for text formating
      else if (*pch == 27) {
        UNITY_OUTPUT_CHAR(*pch);
      }
      // unprintable characters are shown as codes
      else {
        UNITY_OUTPUT_CHAR('\\');
        UnityPrintNumberHex((_U_SINT)*pch, 2);
      }
      pch++;
    }
  }

  UnityPrintRegionExit();
}
#endif

#if !defined(UNITY_SIM) && !defined(UNITY_RTL_SIMULATION)
int UnityPrintf(const char *format, ...)
{
#define LINELEN 200
  int i;
  va_list argptr;
  char line[LINELEN], *c;

  va_start(argptr, format);
  vsnprintf(line, sizeof(line), format, argptr);
  va_end(argptr);

  UnityPrintRegionEnter();

  i = 0;
  c = line;
  while (*c && (i < LINELEN)) {
    UNITY_OUTPUT_CHAR(*c);
    c++;
    i++;
  }

  UnityPrintRegionExit();

  return i;
#undef LINELEN
}

#if defined(UNITY_FPRINTF_INCLUDE)
int Unityfprintf(FILE *fp, const char *format, ...)
{
#define LINELEN 200
  int i;
  va_list argptr;
  char line[LINELEN], *c;
  (void)fp;

  va_start(argptr, format);
  vsnprintf(line, sizeof(line), format, argptr);
  va_end(argptr);

  UnityPrintRegionEnter();

  i = 0;
  c = line;
  while (*c && (i < LINELEN)) {
    UNITY_OUTPUT_CHAR(*c);
    c++;
    i++;
  }

  UnityPrintRegionExit();

  return i;
#undef LINELEN
}
#endif
#endif

//-----------------------------------------------
void UnityPrintNumberByStyle(const _U_SINT number, const UNITY_DISPLAY_STYLE_T style)
{
  if ((style & UNITY_DISPLAY_RANGE_INT) == UNITY_DISPLAY_RANGE_INT) {
    UnityPrintNumber(number);
  } else if ((style & UNITY_DISPLAY_RANGE_UINT) == UNITY_DISPLAY_RANGE_UINT) {
    UnityPrintNumberUnsigned((_U_UINT)number);
  } else {
#if defined(UNITY_RTL_SIMULATION)
    print_hex_rtl_simulation(number);
#else
    UnityPrintNumberHex((_U_UINT)number, (style & 0x000F) << 1);
#endif
  }
}
#if !defined(UNITY_SIM) && !defined(UNITY_RTL_SIMULATION)
//-----------------------------------------------
/// basically do an itoa using as little ram as possible
void UnityPrintNumber(const _U_SINT number_to_print)
{
  _U_SINT divisor = 1;
  _U_SINT next_divisor;
  _U_SINT number = number_to_print;

  UnityPrintRegionEnter();

  if (number < 0) {
    UNITY_OUTPUT_CHAR('-');
    number = -number;
  }

  // figure out initial divisor
  while (number / divisor > 9) {
    next_divisor = divisor * 10;
    if (next_divisor > divisor) {
      divisor = next_divisor;
    } else {
      break;
    }
  }

  // now mod and print, then divide divisor
  do {
    UNITY_OUTPUT_CHAR((char)('0' + (number / divisor % 10)));
    divisor /= 10;
  } while (divisor > 0);

  UnityPrintRegionExit();
}

//-----------------------------------------------
/// basically do an itoa using as little ram as possible
void UnityPrintNumberUnsigned(const _U_UINT number)
{
  _U_UINT divisor = 1;
  _U_UINT next_divisor;

  // figure out initial divisor
  while (number / divisor > 9) {
    next_divisor = divisor * 10;
    if (next_divisor > divisor) {
      divisor = next_divisor;
    } else {
      break;
    }
  }

  UnityPrintRegionEnter();

  // now mod and print, then divide divisor
  do {
    UNITY_OUTPUT_CHAR((char)('0' + (number / divisor % 10)));
    divisor /= 10;
  } while (divisor > 0);

  UnityPrintRegionExit();
}

//-----------------------------------------------
void UnityPrintNumberHex(const _U_UINT number, const char nibbles_to_print)
{
  _U_UINT nibble;
  char nibbles = nibbles_to_print;

  UnityPrintRegionEnter();

  UNITY_OUTPUT_CHAR('0');
  UNITY_OUTPUT_CHAR('x');

  while (nibbles > 0) {
    nibble = (number >> (--nibbles << 2)) & 0x0000000F;
    if (nibble <= 9) {
      UNITY_OUTPUT_CHAR((char)('0' + nibble));
    } else {
      UNITY_OUTPUT_CHAR((char)('A' - 10 + nibble));
    }
  }

  UnityPrintRegionExit();
}
#endif

//-----------------------------------------------
void UnityPrintMask(const _U_UINT mask, const _U_UINT number)
{
  _U_UINT current_bit = (_U_UINT)1 << (UNITY_INT_WIDTH - 1);
  _US32 i;

  UnityPrintRegionEnter();

  for (i = 0; i < UNITY_INT_WIDTH; i++) {
    if (current_bit & mask) {
      if (current_bit & number) {
        UNITY_OUTPUT_CHAR('1');
      } else {
        UNITY_OUTPUT_CHAR('0');
      }
    } else {
      UNITY_OUTPUT_CHAR('X');
    }
    current_bit = current_bit >> 1;
  }

  UnityPrintRegionExit();
}

//-----------------------------------------------
#ifdef UNITY_FLOAT_VERBOSE
void UnityPrintFloat(_UF number)
{
  char TempBuffer[32];
  sprintf(TempBuffer, "%.6f", number);
  UnityPrint(TempBuffer);
}
#endif

//-----------------------------------------------
void UnityTestResultsBegin(const char *file, const UNITY_LINE_TYPE line)
{
  UnityPrint(file);
  UNITY_OUTPUT_CHAR(':');
  UnityPrintNumber(line);
  UNITY_OUTPUT_CHAR(':');
  UnityPrint(UnityGet()->CurrentTestName);
  UNITY_OUTPUT_CHAR(':');
}

//-----------------------------------------------
void UnityTestResultsFailBegin(const UNITY_LINE_TYPE line)
{
  UnityPrintRegionEnter();
  UnityTestResultsBegin(UnityGet()->TestFile, line);
  UNITY_PRINT_TEXT_COLOR("\033[1;31m");
  UnityPrint("FAIL:");
  UNITY_PRINT_TEXT_COLOR("\033[0m");
  UnityPrintRegionExit();
}

//-----------------------------------------------
void UnityConcludeTest(void)
{
  if (UnityGet()->CurrentTestIgnored) {
    UnityGet()->TestIgnores++;
  } else if (!UnityGet()->CurrentTestFailed) {
    UnityPrintRegionEnter();
    UnityTestResultsBegin(UnityGet()->TestFile, UnityGet()->CurrentTestLineNumber);
    UNITY_PRINT_TEXT_COLOR("\033[1;32m");
    UnityPrint("PASS");
    UNITY_PRINT_TEXT_COLOR("\033[0m");
    UNITY_PRINT_EOL;
    UnityPrintRegionExit();
  } else {
    UnityGet()->TestFailures++;
  }

  UnityGet()->CurrentTestFailed  = 0;
  UnityGet()->CurrentTestIgnored = 0;
}

//-----------------------------------------------
void UnityAddMsgIfSpecified(const char *msg)
{
  if (msg) {
    UnityPrint(UnityStrSpacer);
    UnityPrint(msg);
  }
}

//-----------------------------------------------
void UnityPrintExpectedAndActualStrings(const char *expected, const char *actual)
{
  UnityPrint(UnityStrExpected);
  if (expected != NULL) {
    UNITY_OUTPUT_CHAR('\'');
    UnityPrint(expected);
    UNITY_OUTPUT_CHAR('\'');
  } else {
    UnityPrint(UnityStrNull);
  }
  UnityPrint(UnityStrWas);
  if (actual != NULL) {
    UNITY_OUTPUT_CHAR('\'');
    UnityPrint(actual);
    UNITY_OUTPUT_CHAR('\'');
  } else {
    UnityPrint(UnityStrNull);
  }
}

//-----------------------------------------------
// Assertion & Control Helpers
//-----------------------------------------------

int UnityCheckArraysForNull(const void *expected, const void *actual, const UNITY_LINE_TYPE lineNumber, const char *msg)
{
  //return true if they are both NULL
  if ((expected == NULL) && (actual == NULL)) {
    return 1;
  }

  //throw error if just expected is NULL
  if (expected == NULL) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrNullPointerForExpected);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }

  //throw error if just actual is NULL
  if (actual == NULL) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrNullPointerForActual);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }

  //return false if neither is NULL
  return 0;
}

//-----------------------------------------------
// Assertion Functions
//-----------------------------------------------

void UnityAssertBits(const _U_SINT mask,
                     const _U_SINT expected,
                     const _U_SINT actual,
                     const char *msg,
                     const UNITY_LINE_TYPE lineNumber)
{
  UNITY_SKIP_EXECUTION;

  if ((mask & expected) != (mask & actual)) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrExpected);
    UnityPrintMask(mask, expected);
    UnityPrint(UnityStrWas);
    UnityPrintMask(mask, actual);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

//-----------------------------------------------
void UnityAssertEqualNumber(const _U_SINT expected,
                            const _U_SINT actual,
                            const char *msg,
                            const UNITY_LINE_TYPE lineNumber,
                            const UNITY_DISPLAY_STYLE_T style)
{
  UNITY_SKIP_EXECUTION;

  if (expected != actual) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrExpected);
    UnityPrintNumberByStyle(expected, style);
    UnityPrint(UnityStrWas);
    UnityPrintNumberByStyle(actual, style);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

//-----------------------------------------------
void UnityAssertGreaterOrLessOrEqualNumber(const UNITY_INT threshold,
                                           const UNITY_INT actual,
                                           const UNITY_COMPARISON_T compare,
                                           const char *msg,
                                           const UNITY_LINE_TYPE lineNumber,
                                           const UNITY_DISPLAY_STYLE_T style)
{
  int failed = 0;

  UNITY_SKIP_EXECUTION;

  if ((threshold == actual) && (compare & UNITY_EQUAL_TO)) {
    return;
  }
  if ((threshold == actual)) {
    failed = 1;
  }

  if ((style & UNITY_DISPLAY_RANGE_INT) == UNITY_DISPLAY_RANGE_INT) {
    if ((actual > threshold) && (compare & UNITY_SMALLER_THAN)) {
      failed = 1;
    }
    if ((actual < threshold) && (compare & UNITY_GREATER_THAN)) {
      failed = 1;
    }
  } else /* UINT or HEX */
  {
    if (((UNITY_UINT)actual > (UNITY_UINT)threshold) && (compare & UNITY_SMALLER_THAN)) {
      failed = 1;
    }
    if (((UNITY_UINT)actual < (UNITY_UINT)threshold) && (compare & UNITY_GREATER_THAN)) {
      failed = 1;
    }
  }

  if (failed) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrExpected);
    UnityPrintNumberByStyle(actual, style);
    if (compare & UNITY_GREATER_THAN) {
      UnityPrint(UnityStrGt);
    }
    if (compare & UNITY_SMALLER_THAN) {
      UnityPrint(UnityStrLt);
    }
    if (compare & UNITY_EQUAL_TO) {
      UnityPrint(UnityStrOrEqual);
    }
    UnityPrintNumberByStyle(threshold, style);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

//-----------------------------------------------
void UnityAssertEqualIntArray(const _U_SINT *expected,
                              const _U_SINT *actual,
                              const _UU32 num_elements,
                              const char *msg,
                              const UNITY_LINE_TYPE lineNumber,
                              const UNITY_DISPLAY_STYLE_T style,
                              const UNITY_FLAGS_T flags)
{
  _UU32 elements      = num_elements;
  const _US8 *ptr_exp = (_US8 *)expected;
  const _US8 *ptr_act = (_US8 *)actual;

  UNITY_SKIP_EXECUTION;

  if (elements == 0) {
    // If we compare 0 elements, the comparison is always true
    return;

    /* This is the original Unity behaviour, which is pointless :-)
       UnityTestResultsFailBegin(lineNumber);
       UnityPrint(UnityStrPointless);
       UnityAddMsgIfSpecified(msg);
       UNITY_FAIL_AND_BAIL; */
  }

  if (UnityCheckArraysForNull((void *)expected, (void *)actual, lineNumber, msg) == 1) {
    return;
  }

  switch (style) {
    case UNITY_DISPLAY_STYLE_HEX8:
    case UNITY_DISPLAY_STYLE_INT8:
    case UNITY_DISPLAY_STYLE_UINT8:
      while (elements--) {
        if (*ptr_exp != *ptr_act) {
          UnityTestResultsFailBegin(lineNumber);
          UnityPrint(UnityStrElement);
          UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
          UnityPrint(UnityStrExpected);
          UnityPrintNumberByStyle(*ptr_exp, style);
          UnityPrint(UnityStrWas);
          UnityPrintNumberByStyle(*ptr_act, style);
          UnityAddMsgIfSpecified(msg);
          UNITY_FAIL_AND_BAIL;
        }
        if (flags == UNITY_ARRAY_TO_ARRAY) {
          ptr_exp += 1;
        }
        ptr_act += 1;
      }
      break;
    case UNITY_DISPLAY_STYLE_HEX16:
    case UNITY_DISPLAY_STYLE_INT16:
    case UNITY_DISPLAY_STYLE_UINT16:
      while (elements--) {
        if (*(_US16 *)ptr_exp != *(_US16 *)ptr_act) {
          UnityTestResultsFailBegin(lineNumber);
          UnityPrint(UnityStrElement);
          UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
          UnityPrint(UnityStrExpected);
          UnityPrintNumberByStyle(*(_US16 *)ptr_exp, style);
          UnityPrint(UnityStrWas);
          UnityPrintNumberByStyle(*(_US16 *)ptr_act, style);
          UnityAddMsgIfSpecified(msg);
          UNITY_FAIL_AND_BAIL;
        }
        if (flags == UNITY_ARRAY_TO_ARRAY) {
          ptr_exp += 2;
        }
        ptr_act += 2;
      }
      break;
#ifdef UNITY_SUPPORT_64
    case UNITY_DISPLAY_STYLE_HEX64:
    case UNITY_DISPLAY_STYLE_INT64:
    case UNITY_DISPLAY_STYLE_UINT64:
      while (elements--) {
        if (*(_US64 *)ptr_exp != *(_US64 *)ptr_act) {
          UnityTestResultsFailBegin(lineNumber);
          UnityPrint(UnityStrElement);
          UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
          UnityPrint(UnityStrExpected);
          UnityPrintNumberByStyle(*(_US64 *)ptr_exp, style);
          UnityPrint(UnityStrWas);
          UnityPrintNumberByStyle(*(_US64 *)ptr_act, style);
          UnityAddMsgIfSpecified(msg);
          UNITY_FAIL_AND_BAIL;
        }
        if (flags == UNITY_ARRAY_TO_ARRAY) {
          ptr_exp += 8;
        }
        ptr_act += 8;
      }
      break;
#endif
    default:
      while (elements--) {
        if (*(_US32 *)ptr_exp != *(_US32 *)ptr_act) {
          UnityTestResultsFailBegin(lineNumber);
          UnityPrint(UnityStrElement);
          UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
          UnityPrint(UnityStrExpected);
          UnityPrintNumberByStyle(*(_US32 *)ptr_exp, style);
          UnityPrint(UnityStrWas);
          UnityPrintNumberByStyle(*(_US32 *)ptr_act, style);
          UnityAddMsgIfSpecified(msg);
          UNITY_FAIL_AND_BAIL;
        }
        if (flags == UNITY_ARRAY_TO_ARRAY) {
          ptr_exp += 4;
        }
        ptr_act += 4;
      }
      break;
  }
}

//-----------------------------------------------
#ifndef UNITY_EXCLUDE_FLOAT
void UnityAssertEqualFloatArray(const _UF *expected,
                                const _UF *actual,
                                const _UU32 num_elements,
                                const char *msg,
                                const UNITY_LINE_TYPE lineNumber)
{
  _UU32 elements          = num_elements;
  const _UF *ptr_expected = expected;
  const _UF *ptr_actual   = actual;
  _UF diff, tol;

  UNITY_SKIP_EXECUTION;

  if (elements == 0) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrPointless);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }

  if (UnityCheckArraysForNull((void *)expected, (void *)actual, lineNumber, msg) == 1) {
    return;
  }

  while (elements--) {
    diff = *ptr_expected - *ptr_actual;
    if (diff < 0.0) {
      diff = 0.0 - diff;
    }
    tol = UNITY_FLOAT_PRECISION * *ptr_expected;
    if (tol < 0.0) {
      tol = 0.0 - tol;
    }
    if (diff > tol) {
      UnityTestResultsFailBegin(lineNumber);
      UnityPrint(UnityStrElement);
      UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
#ifdef UNITY_FLOAT_VERBOSE
      UnityPrint(UnityStrExpected);
      UnityPrintFloat(*ptr_expected);
      UnityPrint(UnityStrWas);
      UnityPrintFloat(*ptr_actual);
#else
      UnityPrint(UnityStrDelta);
#endif
      UnityAddMsgIfSpecified(msg);
      UNITY_FAIL_AND_BAIL;
    }
    ptr_expected++;
    ptr_actual++;
  }
}

//-----------------------------------------------
void UnityAssertFloatsWithin(const _UF delta,
                             const _UF expected,
                             const _UF actual,
                             const char *msg,
                             const UNITY_LINE_TYPE lineNumber)
{
  _UF diff      = actual - expected;
  _UF pos_delta = delta;

  UNITY_SKIP_EXECUTION;

  if (diff < 0) {
    diff = 0.0f - diff;
  }
  if (pos_delta < 0) {
    pos_delta = 0.0f - pos_delta;
  }

  if (pos_delta < diff) {
    UnityTestResultsFailBegin(lineNumber);
#ifdef UNITY_FLOAT_VERBOSE
    UnityPrint(UnityStrExpected);
    UnityPrintFloat(expected);
    UnityPrint(UnityStrWas);
    UnityPrintFloat(actual);
#else
    UnityPrint(UnityStrDelta);
#endif
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}
#endif

//-----------------------------------------------
void UnityAssertNumbersWithin(const _U_SINT delta,
                              const _U_SINT expected,
                              const _U_SINT actual,
                              const char *msg,
                              const UNITY_LINE_TYPE lineNumber,
                              const UNITY_DISPLAY_STYLE_T style)
{
  UNITY_SKIP_EXECUTION;

  if ((style & UNITY_DISPLAY_RANGE_INT) == UNITY_DISPLAY_RANGE_INT) {
    if (actual > expected) {
      UnityGet()->CurrentTestFailed = ((actual - expected) > delta);
    } else {
      UnityGet()->CurrentTestFailed = ((expected - actual) > delta);
    }
  } else {
    if ((_U_UINT)actual > (_U_UINT)expected) {
      UnityGet()->CurrentTestFailed = ((_U_UINT)(actual - expected) > (_U_UINT)delta);
    } else {
      UnityGet()->CurrentTestFailed = ((_U_UINT)(expected - actual) > (_U_UINT)delta);
    }
  }

  if (UnityGet()->CurrentTestFailed) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrDelta);
    UnityPrintNumberByStyle(delta, style);
    UnityPrint(UnityStrExpected);
    UnityPrintNumberByStyle(expected, style);
    UnityPrint(UnityStrWas);
    UnityPrintNumberByStyle(actual, style);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

//-----------------------------------------------
void UnityAssertEqualString(const char *expected, const char *actual, const char *msg, const UNITY_LINE_TYPE lineNumber)
{
  _UU32 i;

  UNITY_SKIP_EXECUTION;

  // if both pointers not null compare the strings
  if (expected && actual) {
    for (i = 0; expected[i] || actual[i]; i++) {
      if (expected[i] != actual[i]) {
        UnityGet()->CurrentTestFailed = 1;
        break;
      }
    }
  } else { // handle case of one pointers being null (if both null, test should pass)
    if (expected != actual) {
      UnityGet()->CurrentTestFailed = 1;
    }
  }

  if (UnityGet()->CurrentTestFailed) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrintExpectedAndActualStrings(expected, actual);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

//-----------------------------------------------
void UnityAssertEqualStringArray(const char **expected,
                                 const char **actual,
                                 const _UU32 num_elements,
                                 const char *msg,
                                 const UNITY_LINE_TYPE lineNumber)
{
  _UU32 i, j = 0;

  UNITY_SKIP_EXECUTION;

  // if no elements, it's an error
  if (num_elements == 0) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrPointless);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }

  if (UnityCheckArraysForNull((void *)expected, (void *)actual, lineNumber, msg) == 1) {
    return;
  }

  do {
    // if both pointers not null compare the strings
    if (expected[j] && actual[j]) {
      for (i = 0; expected[j][i] || actual[j][i]; i++) {
        if (expected[j][i] != actual[j][i]) {
          UnityGet()->CurrentTestFailed = 1;
          break;
        }
      }
    } else { // handle case of one pointers being null (if both null, test should pass)
      if (expected[j] != actual[j]) {
        UnityGet()->CurrentTestFailed = 1;
      }
    }

    if (UnityGet()->CurrentTestFailed) {
      UnityTestResultsFailBegin(lineNumber);
      if (num_elements > 1) {
        UnityPrint(UnityStrElement);
        UnityPrintNumberByStyle((num_elements - j - 1), UNITY_DISPLAY_STYLE_UINT);
      }
      UnityPrintExpectedAndActualStrings((const char *)(expected[j]), (const char *)(actual[j]));
      UnityAddMsgIfSpecified(msg);
      UNITY_FAIL_AND_BAIL;
    }
  } while (++j < num_elements);
}

//-----------------------------------------------
void UnityAssertEqualMemory(const void *expected,
                            const void *actual,
                            _UU32 length,
                            _UU32 num_elements,
                            const char *msg,
                            const UNITY_LINE_TYPE lineNumber)
{
  unsigned char *expected_ptr = (unsigned char *)expected;
  unsigned char *actual_ptr   = (unsigned char *)actual;
  _UU32 elements              = num_elements;

  UNITY_SKIP_EXECUTION;

  if ((elements == 0) || (length == 0)) {
    UnityTestResultsFailBegin(lineNumber);
    UnityPrint(UnityStrPointless);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }

  if (UnityCheckArraysForNull((void *)expected, (void *)actual, lineNumber, msg) == 1) {
    return;
  }

  while (elements--) {
    if (memcmp((const void *)expected_ptr, (const void *)actual_ptr, length) != 0) {
      UnityGet()->CurrentTestFailed = 1;
      break;
    }
    expected_ptr += length;
    actual_ptr += length;
  }

  if (UnityGet()->CurrentTestFailed) {
    UnityTestResultsFailBegin(lineNumber);
    if (num_elements > 1) {
      UnityPrint(UnityStrElement);
      UnityPrintNumberByStyle((num_elements - elements - 1), UNITY_DISPLAY_STYLE_UINT);
    }
    UnityPrint(UnityStrMemory);
    UnityAddMsgIfSpecified(msg);
    UNITY_FAIL_AND_BAIL;
  }
}

/*-----------------------------------------------*/

static union {
  UNITY_INT8 i8;
  UNITY_INT16 i16;
  UNITY_INT32 i32;
#ifdef UNITY_SUPPORT_64
  UNITY_INT64 i64;
#endif
#ifndef UNITY_EXCLUDE_FLOAT
  float f;
#endif
#ifndef UNITY_EXCLUDE_DOUBLE
  double d;
#endif
} UnityQuickCompare;

UNITY_INTERNAL_PTR UnityNumToPtr(const UNITY_INT num, const UNITY_UINT8 size)
{
  switch (size) {
    case 1:
      UnityQuickCompare.i8 = (UNITY_INT8)num;
      return (UNITY_INTERNAL_PTR)(&UnityQuickCompare.i8);

    case 2:
      UnityQuickCompare.i16 = (UNITY_INT16)num;
      return (UNITY_INTERNAL_PTR)(&UnityQuickCompare.i16);

#ifdef UNITY_SUPPORT_64
    case 8:
      UnityQuickCompare.i64 = (UNITY_INT64)num;
      return (UNITY_INTERNAL_PTR)(&UnityQuickCompare.i64);
#endif

    default: /* 4 bytes */
      UnityQuickCompare.i32 = (UNITY_INT32)num;
      return (UNITY_INTERNAL_PTR)(&UnityQuickCompare.i32);
  }
}

//-----------------------------------------------
// Control Functions
//-----------------------------------------------

void UnityFail(const char *msg, const UNITY_LINE_TYPE line)
{
  UNITY_SKIP_EXECUTION;

  UnityPrintRegionEnter();
  UnityTestResultsBegin(UnityGet()->TestFile, line);
  UNITY_PRINT_TEXT_COLOR("\033[1;31m");
  UnityPrint("FAIL");
  UNITY_PRINT_TEXT_COLOR("\033[0m");

  if (msg != NULL) {
    UNITY_OUTPUT_CHAR(':');
    if (msg[0] != ' ') {
      UNITY_OUTPUT_CHAR(' ');
    }
    UnityPrint(msg);
  }
  UnityPrintRegionExit();
  UNITY_FAIL_AND_BAIL;
}

//-----------------------------------------------
void UnityIgnore(const char *msg, const UNITY_LINE_TYPE line)
{
  UNITY_SKIP_EXECUTION;

  UnityPrintRegionEnter();
  UnityTestResultsBegin(UnityGet()->TestFile, line);
  UNITY_PRINT_TEXT_COLOR("\033[1;33m");
  UnityPrint("IGNORE");
  UNITY_PRINT_TEXT_COLOR("\033[0m");
  if (msg != NULL) {
    UNITY_OUTPUT_CHAR(':');
    UNITY_OUTPUT_CHAR(' ');
    UnityPrint(msg);
  }
  UnityPrintRegionExit();
  UNITY_IGNORE_AND_BAIL;
}

//-----------------------------------------------
void UnityDefaultTestRun(UnityTestFunction Func, const char *FuncName, const int FuncLineNum)
{
  UnityGet()->CurrentTestName       = FuncName;
  UnityGet()->CurrentTestLineNumber = FuncLineNum;
  UnityGet()->NumberOfTests++;
  if (TEST_PROTECT()) {
    setUp();
    Func();
  }
  if (TEST_PROTECT() && !(UnityGet()->CurrentTestIgnored)) {
    tearDown();
  }
  UnityConcludeTest();
}

void UnityBeginGroup(const char *TestFile)
{
  UnityGet()->TestFile = TestFile;
  UnityPrintRegionEnter();
  UNITY_PRINT_EOL;
  UnityPrint("Starting test group ");
  UnityPrint(TestFile);
  UNITY_PRINT_EOL;
  UnityPrintRegionExit();
  UnityBegin();
}

//-----------------------------------------------
void UnityBegin(void)
{
  UnityGet()->NumberOfTests = 0;
  UnityGet()->TestFailures  = 0;
  UnityGet()->TestIgnores   = 0;
}

//-----------------------------------------------
int UnityEnd(void)
{
  UnityPrintRegionEnter();
  UnityPrint("-----------------------");
  UNITY_PRINT_EOL;
  UnityPrintNumber(UnityGet()->NumberOfTests);
  UnityPrint(" Tests ");
  if (UnityGet()->TestFailures > 0) {
    UNITY_PRINT_TEXT_COLOR("\033[1;31m");
  } else {
    UNITY_PRINT_TEXT_COLOR("\033[1;32m");
  }

  UnityPrintNumber(UnityGet()->TestFailures);
  UnityPrint(" Failures ");

  UNITY_PRINT_TEXT_COLOR("\033[0m");

  UnityPrintNumber(UnityGet()->TestIgnores);
  UnityPrint(" Ignored");
  UNITY_PRINT_EOL;
  if (UnityGet()->TestFailures == 0U) {
    UnityPrint("OK");
  } else {
    UnityPrint("FAIL");
#ifdef UNITY_SIM
    print_error("Error detected in test suite");
#endif
  }
  UNITY_PRINT_EOL;
  UnityPrintRegionExit();
  return UnityGet()->TestFailures;
}
