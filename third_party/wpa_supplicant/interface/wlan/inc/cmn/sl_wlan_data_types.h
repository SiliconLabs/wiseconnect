/*******************************************************************************
* @file  sl_wlan_data_types.h
* @brief This file contains data types definitions
*******************************************************************************
* # License
* <b>Copyright 2020-2024 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_WLAN_DATA_TYPES_H
#define SL_WLAN_DATA_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "ctype.h"

//! unsigned char is mapped to uint8
typedef uint8_t uint8;
//! char is mapped to int8
typedef int8_t int8;
//! unsigned short int is mapped as uint16
typedef uint16_t uint16;
//! short int is mapped as int16
typedef int16_t int16;
//! unsigned int is mapped as uint32
typedef uint32_t uint32;
//! int is mapped as uint32
typedef int32_t int32;

typedef uint8_t UINT8;
typedef uint64_t uint64;
typedef int INT;
typedef unsigned int uint; /* System V compatibility */

#define BIT(x) ((uint32)1 << (x))

#define __PACKED__ __attribute__((packed))

#define _OBE_LOW_POWER_     ;
#define _OBE_LOW_POWER_RAM_ ;
#define _OBE_SSL_TO_RAM_    ;

#define WPA_SUPPLICANT_SCAN wpa_supplicant_scan

#endif /* SL_WLAN_DATA_TYPES_H */
