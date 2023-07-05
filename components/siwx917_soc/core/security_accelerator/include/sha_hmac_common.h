/*******************************************************************************
* @file  sha_hmac_common.h
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
/*************************************************************************
 *
 */

#include "stdint.h"

typedef unsigned char byte;
typedef unsigned short word16;
typedef unsigned int word32;
typedef unsigned long long word64;

enum {
  SHA             = 1, /* hash type unique */
  SHA_BLOCK_SIZE  = 64,
  SHA_DIGEST_SIZE = 20,
  SHA_PAD_SIZE    = 56
};

typedef struct Sha {
  word32 buffLen; /* in bytes          */
  word32 loLen;   /* length in bytes   */
  word32 hiLen;   /* length in bytes   */
  word32 buffer[SHA_BLOCK_SIZE / sizeof(word32)];
  word32 digest[SHA_DIGEST_SIZE / sizeof(word32)];
} Sha;

enum {
  SHA256             = 2, /* hash type unique */
  SHA256_BLOCK_SIZE  = 64,
  SHA256_DIGEST_SIZE = 32,
  SHA256_PAD_SIZE    = 56
};

typedef struct Sha256 {
  word32 buffLen; /* in bytes          */
  word32 loLen;   /* length in bytes   */
  word32 hiLen;   /* length in bytes   */
  word32 buffer[SHA_BLOCK_SIZE / sizeof(word32)];
  word32 digest[SHA256_DIGEST_SIZE / sizeof(word32)];
} Sha256;

enum { SHA512 = 4, SHA512_BLOCK_SIZE = 128, SHA512_DIGEST_SIZE = 64, SHA512_PAD_SIZE = 112 };

typedef struct Sha512 {
  word32 buffLen;
  word32 loLen;
  word32 hiLen;
  word64 digest[SHA512_DIGEST_SIZE / sizeof(word64)];
  word64 buffer[SHA512_BLOCK_SIZE / sizeof(word64)];
} Sha512;

enum { SHA384 = 5, SHA384_BLOCK_SIZE = 128, SHA384_DIGEST_SIZE = 48, SHA384_PAD_SIZE = 112 };

typedef struct Sha384 {
  word32 buffLen;
  word32 loLen;
  word32 hiLen;
  word64 digest[SHA512_DIGEST_SIZE / sizeof(word64)]; //FOR TRANSFORM 512
  word64 buffer[SHA384_BLOCK_SIZE / sizeof(word64)];
} Sha384;
