/***************************************************************************/ /**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface FreeRTOS adaptation
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"
#include "em_common.h"
#include "app_ncp.h"
#include "app_ncp_freertos_config.h"

/// Task stack definition and identifier
static TaskHandle_t ncp_task_handle;

/// Binary semaphore to run the task only if its really necessary
static SemaphoreHandle_t ncp_semaphore;

/// Task function-prototype
static void ncp_task(void *p_arg);

/**************************************************************************/ /**
 * Function to trigger the OS task to proceed
 *****************************************************************************/
void app_ncp_os_task_proceed(void)
{
  (void)xSemaphoreGive(ncp_semaphore);
}

/**************************************************************************/ /**
 * OS initialization function
 *****************************************************************************/
void app_ncp_os_task_init(void)
{
  ncp_semaphore = xSemaphoreCreateBinary();

  xTaskCreate(ncp_task, NCP_TASK_NAME, APP_NCP_TASK_STACK, NULL, APP_NCP_TASK_PRIO, &ncp_task_handle);
}

/**************************************************************************/ /**
 * NCP OS task
 *
 * @param[in] p_arg FreeRTOS task arguments
 *****************************************************************************/
static void ncp_task(void *p_arg)
{
  (void)p_arg;
  app_ncp_os_task_proceed();

  while (1) {
    if (xSemaphoreTake(ncp_semaphore, portMAX_DELAY) == pdTRUE) {
      app_ncp_step();
    }
  }
}
