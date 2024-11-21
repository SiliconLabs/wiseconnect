/***************************************************************************//**
 * @file
 * @brief Core interrupt handling API
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef EM_CORE_H
#define EM_CORE_H

/***************************************************************************//**
 * @addtogroup core
 * @{
 ******************************************************************************/
#include "stdint.h"
/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ************************   MACRO API   ***************************************
 ******************************************************************************/

//
//  CRITICAL section macro API.
//

 
/** Storage for PRIMASK or BASEPRI value. */
typedef uint32_t CORE_irqState_t;

#define CORE_DECLARE_IRQ_STATE      CORE_irqState_t irqState

/// Enter CRITICAL section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
/// scope.
#define CORE_ENTER_CRITICAL() irqState = CORE_EnterCritical()
 
/// Exit CRITICAL section. Assumes that a @ref CORE_DECLARE_IRQ_STATE exist in
/// scope.
#define CORE_EXIT_CRITICAL()  CORE_ExitCritical(irqState)
 
CORE_irqState_t CORE_EnterAtomic(void);

void CORE_ExitAtomic(CORE_irqState_t irqState);

CORE_irqState_t CORE_EnterCritical(void);

void CORE_ExitCritical(CORE_irqState_t irqState);

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup core) */

#endif /* EM_CORE_H */
