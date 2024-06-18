/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include "sl_additional_status.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define SL_STATUS_ENUM(prefix, name, value) prefix##_##name = (prefix##_ENUM_OFFSET + value)
#define SL_STATUS_SHARED_ENUM(prefix, name) prefix##_##name = (SL_##name)

#ifdef __CC_ARM
#define BREAKPOINT() __asm__("bkpt #0");
#else
#define BREAKPOINT() __asm__("bkpt");
#endif

#define SL_IPV4_ADDRESS_LENGTH 4
#define SL_IPV6_ADDRESS_LENGTH 16

#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) (void)(x)
#endif // UNUSED_VARIABLE
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) (void)(x)
#endif // UNUSED_PARAMETER

#define ARRAY_COUNT(x) (sizeof(x) / sizeof *(x))

#ifndef FUZZING
#define SL_ASSERT(condition, ...) \
  if (!(condition)) {             \
    BREAKPOINT();                 \
  }
#else
#define SL_ASSERT(condition, ...) \
  if (!(condition)) {             \
  }
#endif

#ifndef ROUND_UP
#define ROUND_UP(x, y) ((x) % (y) ? (x) + (y) - ((x) % (y)) : (x))
#endif /* ifndef ROUND_UP */

#define SL_WAIT_FOREVER    0xFFFFFFFF
#define SL_INVALID_POINTER ((void *)0xEFFFFFFF) // This can point to any location that will trigger an exception

// Defines for log tags
#define ERROR_TAG   "ERROR"
#define WARNING_TAG "WARNING"
#define DEBUG_TAG   "DEBUG"
#define INFO_TAG    "INFO"

// Defines for error logging
#define PRINT_ERROR_LOGS 0

#define PRINT_STATUS(tag, status) printf("\r\n%s %s:%d: 0x%lu \r\n", tag, __FILE__, __LINE__, status);

#define SL_CHECK_STATUS(x)    \
  do {                        \
    if (x != SL_SUCCESS) {    \
      goto status_check_fail; \
    }                         \
  } while (0)

#define SL_CHECK_STATUS_AND_LOG(x, log) \
  do {                                  \
    if (x != SL_SUCCESS) {              \
      SL_LOG(log);                      \
      goto status_check_fail;           \
    }                                   \
  } while (0)

// Macros to help work with pointers / allocations
#define SL_VERIFY_POINTER_OR_EXIT(pointer) \
  do {                                     \
    if ((pointer) == NULL) {               \
      goto exit;                           \
    }                                      \
  } while (0)

#define SL_VERIFY_POINTER_OR_GOTO(pointer, label) \
  do {                                            \
    if ((pointer) == NULL) {                      \
      goto label;                                 \
    }                                             \
  } while (0)

#define SL_VERIFY_POINTER_OR_RETURN(pointer, status) \
  do {                                               \
    if ((pointer) == NULL) {                         \
      if (PRINT_ERROR_LOGS) {                        \
        PRINT_STATUS(ERROR_TAG, status)              \
      }                                              \
      return (status);                               \
    }                                                \
  } while (0)

#define SL_VERIFY_SUCCESS_OR_CONTINUE(x) \
  do {                                   \
    if (x != SL_STATUS_OK) {             \
      continue;                          \
    }                                    \
  } while (0)

#define SL_VERIFY_SUCCESS_OR_EXIT(x) \
  do {                               \
    if (x != SL_STATUS_OK) {         \
      goto exit;                     \
    }                                \
  } while (0)

#define SL_VERIFY_SUCCESS_OR_RETURN(x) \
  do {                                 \
    if ((x) != SL_STATUS_OK) {         \
      return (x);                      \
    }                                  \
  } while (0)

#define SL_CLEANUP_MALLOC(pointer) \
  do {                             \
    if ((pointer) != NULL) {       \
      free(pointer);               \
      pointer = NULL;              \
    }                              \
  } while (0)

#define SL_VERIFY_PARAMETER(condition) \
  do {                                 \
    if (!(condition)) {                \
      return SL_BAD_ARG;               \
    }                                  \
  } while (0)

#define VERIFY_STATUS_AND_RETURN(status) \
  do {                                   \
    if (status != SL_STATUS_OK) {        \
      if (PRINT_ERROR_LOGS) {            \
        PRINT_STATUS(ERROR_TAG, status)  \
      }                                  \
      return status;                     \
    }                                    \
  } while (0)

#define VERIFY_STATUS_AND_GOTO(status, goto_label) \
  do {                                             \
    if (status != SL_STATUS_OK) {                  \
      if (PRINT_ERROR_LOGS) {                      \
        PRINT_STATUS(ERROR_TAG, status)            \
      }                                            \
      goto goto_label;                             \
    }                                              \
  } while (0)

#define PRINT_ERROR_STATUS(tag, status) \
  printf("\r\n%s %s:%d: 0x%x \r\n", tag, __FILE__, __LINE__, (unsigned int)status);

#ifdef PRINT_DEBUG_LOG
extern void sl_debug_log(const char *format, ...);
#define SL_DEBUG_LOG(format, ...)                                                         \
  do {                                                                                    \
    sl_debug_log("%s:%s:%d:" format "\r\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#else
extern void sl_redirect_log(const char *format, ...);
#define SL_DEBUG_LOG(format, ...)                                                            \
  do {                                                                                       \
    sl_redirect_log("%s:%s:%d:" format "\r\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#endif

#define SL_COMPILE_TIME_ASSERT(condition, comment) typedef char assertion_failed__##comment[2 * !!(condition)-1];

typedef uint32_t sl_duration_t;

typedef void (*sl_event_handler_t)(void);
