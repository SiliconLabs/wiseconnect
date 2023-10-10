/*******************************************************************************
* @file  base_types.h
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

#ifndef __BASE_TYPES_H__
#define __BASE_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include <stddef.h>
#include "stdint.h"
#include <stdbool.h>
#include <string.h>
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                           */
/*****************************************************************************/
#ifndef TRUE
/** Value is true (boolean_t type) */
#define TRUE 1
#endif

#ifndef FALSE
/** Value is false (boolean_t type) */
#define FALSE 0
#endif

#ifndef MIN
/** Returns the minimum value out of two values */
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#endif
#ifndef MAX
/** Returns the maximum value out of two values */
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#endif
/** Returns the dimension of an array */
#define DIM(X) (sizeof(X) / sizeof(X[0]))

#ifndef BIT
#define BIT(x) ((uint32_t)1U << (x))
#endif
typedef enum en { Enable = 1, Disable = 0 } en_t;
/******************************************************************************
 * Global type definitions
 ******************************************************************************/

/** logical datatype (only values are TRUE and FALSE) */
typedef uint8_t boolean_t;

/** single precision floating point number (4 byte) */
typedef float float32_t;

/** double precision floating point number (8 byte) */
typedef double float64_t;

/** ASCCI character for string generation (8 bit) */
typedef char char_t;

/** function pointer type to void/void function */
typedef void (*func_ptr_t)(void);

/** function pointer type to void/uint8_t function */
typedef void (*func_ptr_arg1_t)(uint8_t);

#define RSI_DRIVER_VERSION_MAJOR_MINOR(major, minor) (((major) << 8) | (minor))

/**
\brief Driver Version
 */
typedef struct _RSI_DRIVER_VERSION {
  uint16_t api; ///< API version
  uint16_t drv; ///< Driver version
} RSI_DRIVER_VERSION_M4;

/* General return codes */
#define RSI_DRIVER_OK                0  ///< Operation succeeded
#define RSI_DRIVER_ERROR             -1 ///< Unspecified error
#define RSI_DRIVER_ERROR_BUSY        -2 ///< Driver is busy
#define RSI_DRIVER_ERROR_TIMEOUT     -3 ///< Timeout occurred
#define RSI_DRIVER_ERROR_UNSUPPORTED -4 ///< Operation not supported
#define RSI_DRIVER_ERROR_PARAMETER   -5 ///< Parameter error
#define RSI_DRIVER_ERROR_SPECIFIC    -6 ///< Start of driver specific errors

#define SET_BIT(n) BIT(n)  //((uint32_t)1 << n)
#define CLR_BIT(n) ~BIT(n) //(~((uint32_t)1 << n))

#ifndef STATIC
#define STATIC static
#endif

#ifndef INLINE
#ifdef __CC_ARM
#define INLINE __inline
#else
#define INLINE inline
#endif
#endif

#define ENABLE  1
#define DISABLE 0

#ifdef __cplusplus
}
#endif

#endif /* __BASE_TYPES_H__ */

/******************************************************************************/
/* EOF                                                                        */
/******************************************************************************/
