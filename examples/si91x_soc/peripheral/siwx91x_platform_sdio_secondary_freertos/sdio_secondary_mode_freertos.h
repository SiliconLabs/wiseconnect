/***************************************************************************/ /**
 * @file sdio_secondary_mode_freertos.h
 * @brief SDIO secondary FreeRTOS example
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
#ifndef SDIO_SECONDARY_MODE_FREERTOS_H
#define SDIO_SECONDARY_MODE_FREERTOS_H

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * SDIO Secondary example initialization function. Creates a FreeRTOS task
 * that handles SDIO configuration and data transfer.
 * @param none
 * @return none
 ******************************************************************************/
void sdio_secondary_mode_example_init(void);

#endif // SDIO_SECONDARY_MODE_FREERTOS_H
