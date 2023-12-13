/*******************************************************************************
* @file  data_types.h
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
/** @file data_types.h
 * 
 *  @brief This file contains data types defines
 *
 */

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdint.h>

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

#ifdef SINGLE_IMAGE

typedef unsigned char UINT8;
typedef char INT8;
typedef unsigned short UINT16;

typedef char STR;                  /*  8-bit  character                                                 */
typedef unsigned short WSTR;       /*  16-bit character                                                */
typedef unsigned char BYTE;        /*  8-bit unsigned integer                                          */
typedef unsigned char UINT08;      /*  8-bit unsigned integer                                          */
typedef signed char INT08;         /*  8-bit   signed integer                                          */
typedef signed short INT16;        /* 16-bit   signed integer                                          */
typedef unsigned int UINT32;       /* 32-bit unsigned integer                                          */
typedef signed int INT32;          /* 32-bit   signed integer                                          */
typedef unsigned long long UINT64; /* 64-bit unsigned integer                                          */
typedef signed long long INT64;    /* 64-bit   signed integer                                          */
typedef unsigned long ULONG;       /* 32-bit long unsigned.                                            */
typedef long LONG;                 /* 32-bit long signed.                                              */
typedef int INTSTK;                /* Defines CPU stack word size (in octets).                         */
typedef int INTBOOL;
typedef unsigned long SIZE_T;
typedef float FLOAT32;  /* 32-bit floating point                                            */
typedef double FLOAT64; /* 64-bit floating point                                            */
typedef int INTERR;

typedef volatile BYTE REG_BYTE;     /*  8-bit register                                                  */
typedef volatile UINT16 REG_WORD16; /*  8-bit register                                                  */
typedef volatile UINT32 REG_WORD32;

typedef char CHAR;
typedef unsigned char UCHAR;
typedef int INT;
typedef unsigned int UINT;
typedef short SHORT;
typedef unsigned short USHORT;
#endif /* SINGLE_IMAGE */

#endif
