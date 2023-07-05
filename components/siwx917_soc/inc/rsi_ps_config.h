/*******************************************************************************
* @file  rsi_ps_config.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * Includes
 */

/*Flash based or RAM based execution selection for IVT*/
#ifdef RSI_M4_INTERFACE
#if defined(CHIP_917B0) && defined(PSRAM)
#define IVT_OFFSET_ADDR 0xA001000 /*<!Application IVT location VTOR offset>        */
#else
#ifdef COMMON_FLASH_EN
#ifdef CHIP_917B0
#define IVT_OFFSET_ADDR 0x81C2000 /*<!Application IVT location VTOR offset>        */
#else
#define IVT_OFFSET_ADDR 0x8212000 /*<!Application IVT location VTOR offset>        */
#endif
#else
#define IVT_OFFSET_ADDR 0x8012000 /*<!Application IVT location VTOR offset>        */
#endif
#endif
#endif

#ifdef CHIP_917B0
#define WKP_RAM_USAGE_LOCATION 0x24061EFC /*<!Bootloader RAM usage location upon wake up  */
#else
#define WKP_RAM_USAGE_LOCATION 0x24061000 /*<!Bootloader RAM usage location upon wake up  */
#endif
#define WKP_STACK_USAGE_LOCATION 0x1b000 /*<!Bootloader stack usage location upon wake up> */
#define WKP_CB_ADDRESS           0x1e000 /*<!Bootloader Control block usage loacation upon wake up>*/

#define WIRELESS_WAKEUP_IRQHandler NPSS_TO_MCU_WIRELESS_INTR_IRQn