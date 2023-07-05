/*******************************************************************************
* @file  dh.h
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

#include "stdint.h"

/* NN.H - header file for NN.C
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/* Type definitions.
 */
#ifndef __NN_H_
#define __NN_H_

typedef unsigned long int NN_DIGIT;

int calc_dh_key(uint8_t *prime,
                uint32_t prime_len,
                NN_DIGIT *base,
                uint32_t base_len,
                uint8_t *x_orig,
                uint32_t x_length,
                NN_DIGIT *yy,
                uint8_t interrupt);

#define dh_clk_enable BIT(14)

typedef unsigned short int UINT2;
typedef unsigned long int UINT4;

typedef UINT4 NN_DIGIT;
typedef UINT2 NN_HALF_DIGIT;

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

/* Constants.

   Note: MAX_NN_DIGITS is long enough to hold any RSA modulus, plus
   one more digit as required by R_GeneratePEMKeys (for n and phiN,
   whose lengths must be even). All natural numbers have at most
   MAX_NN_DIGITS digits, except for double-length intermediate values
   in NN_Mult (t), NN_ModMult (t), NN_ModInv (w), and NN_Div (c).
 */
/* Length of digit in bits */
#define NN_DIGIT_BITS      32
#define NN_HALF_DIGIT_BITS 16
/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS 64
/* Maximum digits */
#define MAX_NN_DIGIT      0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

/* Macros.
 */
#define LOW_HALF(x)     ((x)&MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x)    (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x)    (unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x)   (unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

/* CONVERSIONS
   NN_Decode (a, digits, b, len)   Decodes character string b into a.
   NN_Encode (a, len, b, digits)   Encodes a into character string b.

   ASSIGNMENTS
   NN_Assign (a, b, digits)        Assigns a = b.
   NN_ASSIGN_DIGIT (a, b, digits)  Assigns a = b, where b is a digit.
   NN_AssignZero (a, b, digits)    Assigns a = 0.
   NN_Assign2Exp (a, b, digits)    Assigns a = 2^b.
     
   ARITHMETIC OPERATIONS
   NN_Add (a, b, c, digits)        Computes a = b + c.
   NN_Sub (a, b, c, digits)        Computes a = b - c.
   NN_Mult (a, b, c, digits)       Computes a = b * c.
   NN_LShift (a, b, c, digits)     Computes a = b * 2^c.
   NN_RShift (a, b, c, digits)     Computes a = b / 2^c.
   NN_Div (a, b, c, cDigits, d, dDigits)  Computes a = c div d and b = c mod d.

   NUMBER THEORY
   NN_Mod (a, b, bDigits, c, cDigits)  Computes a = b mod c.
   NN_ModMult (a, b, c, d, digits) Computes a = b * c mod d.
   NN_ModExp (a, b, c, cDigits, d, dDigits)  Computes a = b^c mod d.
   NN_ModInv (a, b, c, digits)     Computes a = 1/b mod c.
   NN_Gcd (a, b, c, digits)        Computes a = gcd (b, c).

   OTHER OPERATIONS
   NN_EVEN (a, digits)             Returns 1 iff a is even.
   NN_Cmp (a, b, digits)           Returns sign of a - b.
   NN_EQUAL (a, digits)            Returns 1 iff a = b.
   NN_Zero (a, digits)             Returns 1 iff a = 0.
   NN_Digits (a, digits)           Returns significant length of a in digits.
   NN_Bits (a, digits)             Returns significant length of a in bits.
 */
void NN_Decode PROTO_LIST((NN_DIGIT *, unsigned int, unsigned char *, unsigned int));
void NN_Encode PROTO_LIST((unsigned char *, unsigned int, NN_DIGIT *, unsigned int));

void NN_Assign PROTO_LIST((NN_DIGIT *, NN_DIGIT *, unsigned int));
void NN_AssignZero PROTO_LIST((NN_DIGIT *, unsigned int));
void NN_Assign2Exp PROTO_LIST((NN_DIGIT *, unsigned int, unsigned int));

NN_DIGIT NN_Add PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));
NN_DIGIT NN_Sub PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));
void NN_Mult PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));
void NN_Div PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int));
NN_DIGIT NN_LShift PROTO_LIST((NN_DIGIT *, NN_DIGIT *, unsigned int, unsigned int));
NN_DIGIT NN_RShift PROTO_LIST((NN_DIGIT *, NN_DIGIT *, unsigned int, unsigned int));

void NN_Mod PROTO_LIST((NN_DIGIT *, NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int));
void NN_ModMult PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));
void NN_ModExp PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int));
void NN_ModInv PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));
void NN_Gcd PROTO_LIST((NN_DIGIT *, NN_DIGIT *, NN_DIGIT *, unsigned int));

int NN_Cmp PROTO_LIST((NN_DIGIT *, NN_DIGIT *, unsigned int));
int NN_Zero PROTO_LIST((NN_DIGIT *, unsigned int));
unsigned int NN_Bits PROTO_LIST((NN_DIGIT *, unsigned int));
unsigned int NN_Digits PROTO_LIST((NN_DIGIT *, unsigned int));

#define NN_ASSIGN_DIGIT(a, b, digits) \
  {                                   \
    NN_AssignZero(a, digits);         \
    a[0] = b;                         \
  }
#define NN_EQUAL(a, b, digits) (!NN_Cmp(a, b, digits))
#define NN_EVEN(a, digits)     (((digits) == 0) || !(a[0] & 1))
#endif
