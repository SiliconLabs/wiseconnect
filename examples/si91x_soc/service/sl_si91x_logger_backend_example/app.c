/***************************************************************************/ /**
 * @file app.c
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_si91x_logger_example.h"
#include "app.h"

/*******************************************************************************
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  logger_example_init();
}

/*******************************************************************************
 * App ticking function.
 *
 * Intentionally empty: all work runs in the FreeRTOS application_start task
 * created by logger_example_init(), and the FreeRTOS idle hook handles
 * sl_log_flush(). This hook is kept only to satisfy the sl_main entry
 * contract.
 ******************************************************************************/
void app_process_action(void)
{
}
