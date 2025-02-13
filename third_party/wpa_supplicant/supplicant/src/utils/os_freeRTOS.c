/***************************************************************************/ /**
 * @file os_freeRTOS.c
 * @brief Wrappers for freeRTOS OS functions needed by the supplicant. 
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
#ifdef SUPPLICANT_PORTING
#include "includes.h"
#include "common.h"
#include "supplicant_mgmt_if.h"

uint32_t os_wait_for_thread_stop_flag(os_time_t sec, os_time_t usec)
{
    uint32_t result =
    osEventFlagsWait(supp_eloop_thread_ctx->event_flags_id, SLI_EVENT_SUPPLICANT_LOOP_SIGNAL_THREAD_TERMINATE, osFlagsNoClear, (sec * 1000 + usec / 1000));

    return (((result == (uint32_t)osErrorTimeout) || (result == (uint32_t)osErrorResource)) ? 0 : 1);
}
#endif /*SUPPLICANT_PORTING*/