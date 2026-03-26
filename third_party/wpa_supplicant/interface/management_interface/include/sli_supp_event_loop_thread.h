/***************************************************************************/ /**
 * @file sli_supp_event_loop_thread.h
 * @brief Header file for supplicant eloop thread management. 
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SLI_SUPP_EVENT_LOOP_THREAD
#define SLI_SUPP_EVENT_LOOP_THREAD

#include "cmsis_os2.h"
#include "sl_status.h"

#define SLI_EVENT_SUPPLICANT_LOOP_TRIGGER                   BIT(1)
#define SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE   BIT(2)
#define SLI_EVENT_SUPPLICANT_LOOP_NOTIFY_READY_TO_TERMINATE BIT(3)

#define SLI_SUPP_LOOP_EVENTS SLI_EVENT_SUPPLICANT_LOOP_TRIGGER | SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE

typedef struct {
  osEventFlagsId_t event_flags_id;
} sl_supp_eloop_thread_ctx_t;

extern sl_supp_eloop_thread_ctx_t *supp_eloop_thread_ctx;

sl_status_t sli_supp_eloop_thread_init();

sl_status_t sli_supp_eloop_thread_deinit();
#endif /*SLI_SUPP_EVENT_LOOP_THREAD*/
