/******************************************************************************
* @file  rsi_ccp_user_config.h
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

/*rsi_ccp_user_config.h --> user configurations w.r.t CCP*/

/* To enable 917 chip  --> Add #define SLI_SI917 */
/* To enable 9116 chip -->  Add #define CHIP_9118 */
/* To enable 9115 chip -->  Add #define CHIP_915 */

/* #define CHIP_9118 */
/* #define CHIP_915 */
//#define SLI_SI917

#ifdef CHIP_915
#if !defined(SLI_SI917) && !defined(SLI_SI915)
#define SLI_SI917
#endif
#endif

/*1. By default A10 ROM is enabled */
/*2. To enable A11 ROM      --> Add #define A11_ROM (this disables A10 ROM) */
/*3. To enable OS           --> Add SLI_SI91X_ENABLE_OS in preprocessor */
/*4. To enable ROM Wireless --> Add #define ROM_WIRELESS */
#define A11_ROM

#ifndef SLI_SI91X_MCU_MOV_ROM_API_TO_FLASH
#define ROMDRIVER_PRESENT
#endif

#if defined(SLI_SI917B0) || defined(SLI_SI915)
#define SI91X_SYSRTC_COUNT 2
#endif
