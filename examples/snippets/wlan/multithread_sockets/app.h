/***************************************************************************/ /**
 * @file app.h
 * @brief Top level application functions for the Multithread Sockets example.
 *
 * Thread-safety:
 *   printf_mutex must be created (via osMutexNew) before any thread calls
 *   LOG_PRINT. The macro acquires the mutex with an infinite timeout so
 *   concurrent threads never interleave log output.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_H
#define APP_H

#include "cmsis_os2.h"
#include <stdio.h>

extern osMutexId_t printf_mutex;

/***************************************************************************/ /**
 * Initialize application.
 ******************************************************************************/
void app_init(void);

/// Thread-safe printf wrapper. Guards console output with printf_mutex so that
/// log lines from different RTOS threads are never interleaved.
/// The mutex must have been created before first use.
#define LOG_PRINT(...)                                           \
  do {                                                           \
    if (printf_mutex != 0) {                                     \
      if (osMutexAcquire(printf_mutex, osWaitForever) == osOK) { \
        printf(__VA_ARGS__);                                     \
        osMutexRelease(printf_mutex);                            \
      }                                                          \
    } else {                                                     \
      printf(__VA_ARGS__);                                       \
    }                                                            \
  } while (0)

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void);

#endif // APP_H
