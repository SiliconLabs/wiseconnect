/***************************************************************************/ /**
 * @file app.c
 * @brief Top level application functions
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
#include "file_operations_example.h"
#include "app.h"

/***************************************************************************/ /**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  file_operations_example_init();
}

/***************************************************************************/ /**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  file_operations_example_process_action();
}
