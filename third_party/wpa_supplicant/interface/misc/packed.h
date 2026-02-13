/*******************************************************************************
* @file  packed.h
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

#ifndef __RSI_PACKED_H_
#define __RSI_PACKED_H_

#ifdef ENABLE_WC_PLUS
#define __PACKED__
#define RSI_PACK __packed
#else
#define __PACKED__ __attribute__((packed))
#endif
#define __PACKED_ALLIGNED__ __attribute__((packed, aligned(4)))
#endif
