/***************************************************************************/ /**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface Micrium OS configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef APP_NCP_MICRIUMOS_CONFIG_H
#define APP_NCP_MICRIUMOS_CONFIG_H

/***********************************************************************************************/ /**
 * @addtogroup ncp
 * @{
 **************************************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <o APP_NCP_TASK_PRIO> Priority of the NCP OS task
// <i> Default: 5
// <i> Priority of the NCP OS task
#define APP_NCP_TASK_PRIO 5

// <o APP_NCP_TASK_STACK> Stack size of the NCP OS task in bytes
// <i> Default: 1024
// <i> Stack size of the NCP OS task (bytes)
#define APP_NCP_TASK_STACK 1024

// <s NCP_TASK_NAME> Name of the NCP OS task
// <i> Default: "ncp_task"
// <i> Name of the NCP OS task
#define NCP_TASK_NAME "ncp_task"

// <s NCP_SEMAPHORE_NAME> Name of the NCP OS task semaphore
// <i> Default: "ncp_semaphore"
// <i> Name of the NCP OS task semaphore used for triggering the task
#define NCP_SEMAPHORE_NAME "ncp_semaphore"

// <<< end of configuration section >>>

/** @} (end addtogroup ncp) */
#endif // APP_NCP_MICRIUMOS_CONFIG_H
