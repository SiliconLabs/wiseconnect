/***************************************************************************/ /**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#pragma once

#include <stdint.h>
#include "cmsis_os2.h"

#ifndef BIT
#define BIT(a) ((uint32_t)1U << a)
#endif

/*=========================================================================*/

// SSL features
/*=========================================================================*/

//Release 2.0 default ciphers
#define SSL_DEFAULT_CIPHERS                                                                    \
  (BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256           \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256     \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA           \
   | BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA               \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA          \
   | BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA | BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA)
//Release 2.0 all configurable ciphers
#define SSL_RELEASE_2_0_ALL_CIPHERS                                                                       \
  (SSL_DEFAULT_CIPHERS | BIT_TLS_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_RSA_WITH_AES_128_CBC_SHA256        \
   | BIT_TLS_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CCM_8 \
   | BIT_TLS_RSA_WITH_AES_256_CCM_8)

#ifdef CHIP_917
#define SSL_EXT_CIPHERS SSL_TLSV1_3_ALL_CIPHERS
#endif

#define SSL_ALL_CIPHERS SSL_RELEASE_2_0_ALL_CIPHERS

//TLSv1.3 configurable ciphers
#ifdef CHIP_917
#define SSL_TLSV1_3_ALL_CIPHERS                                                                     \
  (BIT_TLS13_AES_128_GCM_SHA256 | BIT_TLS13_AES_256_GCM_SHA384 | BIT_TLS13_CHACHA20_POLY1305_SHA256 \
   | BIT_TLS13_AES_128_CCM_SHA256 | BIT_TLS13_AES_128_CCM_8_SHA256)
#endif

#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA256         BIT(0)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA256         BIT(1)
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA            BIT(2)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA            BIT(3)
#define BIT_TLS_RSA_WITH_AES_128_CCM_8              BIT(4)
#define BIT_TLS_RSA_WITH_AES_256_CCM_8              BIT(5)
#define BIT_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256     BIT(8)
#define BIT_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384     BIT(9)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256   BIT(10)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384   BIT(11)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256     BIT(14)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256     BIT(15)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA        BIT(16)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA        BIT(17)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384   BIT(18)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256   BIT(19)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA      BIT(20)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA      BIT(21)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 BIT(22)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 BIT(23)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA    BIT(24)
#define BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA       BIT(25)
#define BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA     BIT(26)
#define BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA   BIT(27)
#define SSL_NEW_CIPHERS                             BIT(31)

// TLSv1.3 supported ciphers
#ifdef CHIP_917
#define BIT_TLS13_AES_128_GCM_SHA256       BIT(0)
#define BIT_TLS13_AES_256_GCM_SHA384       BIT(1)
#define BIT_TLS13_CHACHA20_POLY1305_SHA256 BIT(2)
#define BIT_TLS13_AES_128_CCM_SHA256       BIT(3)
#define BIT_TLS13_AES_128_CCM_8_SHA256     BIT(4)
#endif
