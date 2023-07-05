/*******************************************************************************
* @file  rsi_sha.h
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
#include "rsi_crypto.h"
#include "stdlib.h"

#define NULL 0

#define InitSha   wc_InitSha
#define ShaUpdate wc_ShaUpdate
#define ShaFinal  wc_ShaFinal

#define InitSha256   wc_InitSha256
#define Sha256Update wc_Sha256Update
#define Sha256Final  wc_Sha256Final
/////////////hash

#define InitSha384   wc_InitSha384
#define Sha384Update wc_Sha384Update
#define Sha384Final  wc_Sha384Final

#define InitSha512   wc_InitSha512
#define Sha512Update wc_Sha512Update
#define Sha512Final  wc_Sha512Final

int sha1_vector(unsigned int num_elem, const uint8_t *addr[], const unsigned int *len, uint8_t *mac);
int sha256_vector(unsigned int num_elem, const uint8_t *addr[], const unsigned int *len, uint8_t *mac);
int sha384_vector(unsigned int num_elem, const uint8_t *addr[], const unsigned int *len, uint8_t *mac);
int sha512_vector(unsigned int num_elem, const uint8_t *addr[], const unsigned int *len, uint8_t *mac);

int16_t rsi_crypto_SHA(rsi_sha_req_t *crypto, uint16_t *output_length, uint8_t *output, uint8_t *sha_data);
