/***************************************************************************/ /**
 * @file app.c
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2026 Silicon Laboratories Inc. www.silabs.com</b>
 * SPDX-License-Identifier: Zlib
 ******************************************************************************/
#include "app.h"
#include "sht4x_example.h"

void app_init(void)
{
  sht4x_example_init();
}

void app_process_action(void)
{
  sht4x_example_process_action();
}
