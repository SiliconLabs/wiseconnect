/***************************************************************************/ /**
 * @file app.h
 * @brief Top level application functions for the Multithreading Application
 *        example (STA + SoftAP concurrent mode with MQTT and throughput).
 *
 * Thread-safety:
 *   printf_mutex serializes console output from the MQTT thread and the
 *   throughput thread.  The mutex must be created before any LOG_PRINT call.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

/// Thread-safe printf wrapper.
/// Falls back to raw printf if the mutex has not been created yet.
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
 * Initialize application.
 ******************************************************************************/
void app_init(void);

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void);

#endif // APP_H
