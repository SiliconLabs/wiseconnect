/***************************************************************************/ /**
 * @file
 * @brief SL USART Config.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_DMA_CONFIG_H
#define SL_DMA_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#ifdef __cplusplus
extern "C" {
#endif

// <o SL_DMA_INSTANCE> DMA instance <0-1>
// <i> Default: 0
#define SL_DMA_INSTANCE 0

// <o SL_DMA_CHANNEL> DMA channel <1-32>
// <i> Default: 32
#define SL_DMA_CHANNEL 32

// <o SL_DMA_TRANSFER_SIZE> DMA transfer size in bytes <1-10000>
// <i> Default: 10000
#define SL_DMA_TRANSFER_SIZE 2048

// <q SL_DMA_SIMPLE_TRANSFER>
// <i> Default: 1
#define SL_DMA_SIMPLE_TRANSFER 1

#ifdef __cplusplus
}
#endif
// <<< end of configuration section >>>
#endif //SL_DMA_CONFIG_H
