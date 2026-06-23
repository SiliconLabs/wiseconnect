/***************************************************************************/ /**
 * @file sl_si91x_logger_example.h
 * @brief Logger backend example functions
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

#ifndef SL_SI91X_LOGGER_EXAMPLE_H_
#define SL_SI91X_LOGGER_EXAMPLE_H_

// -----------------------------------------------------------------------------
// Prototypes
/***************************************************************************/ /**
 * @brief Initialize the logger backend example.
 *
 * Spawns the FreeRTOS @c application_start task, which drives the
 * demonstration loop (button-driven log-level cycling and per-backend log
 * output). Call once from @c app_init() during system startup.
 *
 * @note This routine returns as soon as the task is created; all log work
 *       happens asynchronously in the spawned task and the FreeRTOS idle
 *       hook (which calls @c sl_log_flush()).
 ******************************************************************************/
void logger_example_init(void);

#endif /* SL_SI91X_LOGGER_EXAMPLE_H_ */
