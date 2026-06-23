/***************************************************************************/ /**
 * @file app.c
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2025 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 ******************************************************************************/
#include "app.h"
#include "pcm_primary_freertos.h"

void app_init(void)
{
  pcm_primary_example_init();
}

void app_process_action(void)
{
}
