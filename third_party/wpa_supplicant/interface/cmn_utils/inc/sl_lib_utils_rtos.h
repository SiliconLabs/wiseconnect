/*******************************************************************************
* @file  sl_lib_utils_rtos.h
* @brief This file contains function declarations of packet util wrapper functions
*******************************************************************************
* # License
* <b>Copyright 2020-2023 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_LIB_UTILS_RTOS_H
#define SL_LIB_UTILS_RTOS_H

#include "sl_wlan_data_types.h"
#include "sl_wlan_logger.h"
#include "string.h"
#include "stddef.h"
#include <stdlib.h>
#include "sl_string.h"

extern char *itoa(int value, char *str, int base);

#define sl_memcpy(dst, src, len)          memcpy((dst), (src), (len))
#define sl_memset(dst, c, n)              memset((dst), (c), (n))
#define sl_memzero(dst, n)                memset((dst), 0, (n))
#define sl_memcmp(string1, string2, len)  memcmp((string1), (string2), (len))
#define sl_strncmp(string1, string2, len) strncmp((const char *)(string1), (const char *)(string2), (size_t)(len))
#define sl_strcmp(dst, src)               strcmp((const char *)(dst), (const char *)(src))
#define sl_malloc(size)                   malloc(size)
#define sl_free(ptr)                      free(ptr)
#define sl_zalloc(size)                   (uint8 *)calloc(1, size)
#define sl_atoi(str)                      (uint32) atoi((const char *)(str))
#define sl_itoa(val, str)                 (uint8 *)itoa((val), (char *)(str), 10)
#define sl_strcpy(dst, src)               strcpy((char *)(dst), (const char *)(src))
#define sli_mgmt_realloc(x, y)            realloc(x, y)
#define sl_strcat(dst, src)               strcat((int8_t *)(dst), (const int8_t *)(src))

#define SL_READ_6(b0, b1, b2, b3, b4, b5) \
  ((((uint64_t)((b4) | ((b5) << 8))) << 32) | ((uint32_t)((b0) | ((b1) << 8) | ((b2) << 16) | ((b3) << 24))))

#endif /* SL_LIB_UTILS_RTOS_H */
