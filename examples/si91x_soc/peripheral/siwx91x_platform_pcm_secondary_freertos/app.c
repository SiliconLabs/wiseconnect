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
#include "pcm_secondary_freertos.h"

void app_init(void)
{
  pcm_secondary_example_init();
}

void app_process_action(void)
{
}
