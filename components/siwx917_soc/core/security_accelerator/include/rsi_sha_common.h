/*******************************************************************************
* @file  rsi_sha_common.h
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
typedef unsigned char byte;
typedef unsigned short word16;
typedef unsigned int word32;
typedef unsigned long long word64;

#define W64LIT(x) x##LL

static word32 rotlFixed(word32 x, word32 y)
{
  return (x << y) | (x >> (sizeof(y) * 8 - y));
}

static word32 rotrFixed(word32 x, word32 y)
{
  return (x >> y) | (x << (sizeof(y) * 8 - y));
}

static word64 rotlFixed64(word64 x, word64 y)
{
  return (x << y) | (x >> (sizeof(y) * 8 - y));
}

static word64 rotrFixed64(word64 x, word64 y)
{
  return (x >> y) | (x << (sizeof(y) * 8 - y));
}

static word64 ByteReverseWord64(word64 value)
{
  value = ((value & W64LIT(0xFF00FF00FF00FF00)) >> 8) | ((value & W64LIT(0x00FF00FF00FF00FF)) << 8);
  value = ((value & W64LIT(0xFFFF0000FFFF0000)) >> 16) | ((value & W64LIT(0x0000FFFF0000FFFF)) << 16);
  return rotlFixed64(value, 32U);
}

static void ByteReverseWords64(word64 *out, const word64 *in, word32 byteCount)
{
  word32 count = byteCount / (word32)sizeof(word64), i;

  for (i = 0; i < count; i++)
    out[i] = ByteReverseWord64(in[i]);
}

static word32 ByteReverseWord32(word32 value)
{
  /* 6 instructions with rotate instruction, 8 without */
  value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
  return rotlFixed(value, 16U);
}

static void ByteReverseWords(word32 *out, const word32 *in, word32 byteCount)
{
  word32 count = byteCount / (word32)sizeof(word32), i;

  for (i = 0; i < count; i++)
    out[i] = ByteReverseWord32(in[i]);
}
