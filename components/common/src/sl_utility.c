/***************************************************************************/ /**
 * @file  sl_utility.c
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include <stdlib.h>
#include "sl_utility.h"
#include "sl_constants.h"
#include "sl_string.h"
#include "cmsis_compiler.h"
#include <string.h>
#include <stdio.h>

#ifdef SPRINTF_CHAR
#define SPRINTF(x) strlen(sprintf /**/ x)
#else
#define SPRINTF(x) ((size_t)sprintf x)
#endif

#define DIGIT_VAL(c)                             \
  (((c >= '0') && (c <= '9'))   ? (c - '0')      \
   : ((c >= 'a') && (c <= 'z')) ? (c - 'a' + 10) \
   : ((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 10) \
                                : -1)
typedef struct {
  int base;
  int len;
} data_t;

extern char *strtok_r(char *, const char *, char **);

void sli_convert_uint32_to_bytestream(uint16_t data, uint8_t *buffer)
{
  buffer[0] = (uint8_t)(data & 0xFF);
  buffer[1] = (uint8_t)((data >> 8) & 0xFF);
  buffer[2] = (uint8_t)((data >> 16) & 0xFF);
  buffer[3] = (uint8_t)((data >> 24) & 0xFF);
  return;
}

sl_status_t convert_string_to_sl_ipv4_address(char *line, sl_ipv4_address_t *ip)
{
  char *lasts       = NULL;
  const char *token = strtok_r(line, ".", &lasts);

  for (uint8_t i = 0; i < 4; i++, token = strtok_r(NULL, ".", &lasts)) {
    if (token == NULL) {
      return SL_STATUS_COMMAND_IS_INVALID;
    }
    ip->bytes[i] = (uint8_t)strtoul(token, 0, 0);
  }
  return SL_STATUS_OK;
}

void print_sl_ip_address(const sl_ip_address_t *sl_ip_address)
{
  if (sl_ip_address == NULL) {
    return;
  }

  if (sl_ip_address->type == SL_IPV4) {
    print_sl_ipv4_address(&sl_ip_address->ip.v4);
  } else if (sl_ip_address->type == SL_IPV6) {
    print_sl_ipv6_address(&sl_ip_address->ip.v6);
  }
}

void print_sl_ipv4_address(const sl_ipv4_address_t *ip_address)
{
  printf("%d.%d.%d.%d", ip_address->bytes[0], ip_address->bytes[1], ip_address->bytes[2], ip_address->bytes[3]);
}

void print_sl_ipv6_address(const sl_ipv6_address_t *ip_address)
{
  char temp_buffer[46] = { 0 };
  sl_inet_ntop6((const unsigned char *)(ip_address), (char *)temp_buffer, sizeof(temp_buffer));
  printf("%s\r\n", temp_buffer);
}

void print_mac_address(const sl_mac_address_t *mac_address)
{
  if (mac_address == NULL) {
    return;
  }
  printf("%2X:%2X:%2X:%2X:%2X:%2X",
         mac_address->octet[0],
         mac_address->octet[1],
         mac_address->octet[2],
         mac_address->octet[3],
         mac_address->octet[4],
         mac_address->octet[5]);
}

char *sl_inet_ntop6(const unsigned char *input, char *dst, uint32_t size)
{
  char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"];
  char *tp;
  data_t best;
  data_t cur;
  unsigned int words[SL_IPV6_ADDRESS_LENGTH / 2];
  int i;
  unsigned int ip_big_endian[4];
  const unsigned char *src;
  src = (unsigned char *)&ip_big_endian;

  sli_little_to_big_endian((const unsigned int *)input, (unsigned char *)ip_big_endian, SL_IPV6_ADDRESS_LENGTH);

  memset(words, '\0', sizeof words);
  for (i = 0; i < SL_IPV6_ADDRESS_LENGTH; i += 2) {
    int temp     = src[i];
    words[i / 2] = (temp << 8) | src[i + 1];
  }
  best.base = -1;
  cur.base  = -1;
  best.len  = 0;
  cur.len   = 0;
  for (i = 0; i < (SL_IPV6_ADDRESS_LENGTH / 2); i++) {
    if (words[i] == 0) {
      if (cur.base == -1) {
        cur.base = i;
        cur.len  = 1;
      } else
        cur.len++;
    } else {
      if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
          best = cur;
        cur.base = -1;
      }
    }
  }
  if ((cur.base != -1) && (best.base == -1 || cur.len > best.len))
    best = cur;
  if (best.base != -1 && best.len < 2)
    best.base = -1;
  /*
   * Format the result.
   */
  tp = tmp;
  for (i = 0; i < (SL_IPV6_ADDRESS_LENGTH / 2); i++) {
    /* Are we inside the best run of 0x00's? */
    if (best.base != -1 && i >= best.base && i < (best.base + best.len)) {
      if (i == best.base)
        *tp++ = ':';
      continue;
    }
    /* Are we following an initial run of 0x00s or any real hex? */
    if (i != 0)
      *tp++ = ':';
    tp += SPRINTF((tp, "%x", words[i]));
  }
  /* Was it a trailing run of 0x00's? */
  if (best.base != -1 && (best.base + best.len) == (SL_IPV6_ADDRESS_LENGTH / 2))
    *tp++ = ':';
  *tp++ = '\0';
  /*
   * Check for overflow, copy, and we're done.
   */
  if ((uint32_t)(tp - tmp) > size) {
    printf("\r\n Error \r\n");
    return NULL;
  }
  return memcpy(dst, tmp, size);
}

static int sli_hex_digit_value(char ch)
{
  if ('0' <= ch && ch <= '9')
    return ch - '0';
  if ('a' <= ch && ch <= 'f')
    return ch - 'a' + 10;
  if ('A' <= ch && ch <= 'F')
    return ch - 'A' + 10;
  return -1;
}

void sli_little_to_big_endian(const unsigned int *source, unsigned char *result, unsigned int length)
{
  unsigned char *temp;
  unsigned int curr = 0;
  length /= 4;

  for (unsigned int i = 0; i < length; i++) {
    curr    = source[i];
    temp    = &result[i * 4];
    temp[3] = (curr & 0xFF);
    temp[2] = ((curr >> 8) & 0xFF);
    temp[1] = ((curr >> 16) & 0xFF);
    temp[0] = ((curr >> 24) & 0xFF);
  }
}

int sl_inet_pton6(const char *src, const char *src_endp, unsigned char *dst, unsigned int *ptr_result)
{
  unsigned char tmp[SL_IPV6_ADDRESS_LENGTH];
  unsigned char *tp;
  unsigned char *endp;
  unsigned char *colonp;
  const char *curtok __attribute__((__unused__));
  int ch;
  size_t xdigits_seen; /* Number of hex digits since colon.  */
  unsigned int val;
  tp     = memset(tmp, '\0', SL_IPV6_ADDRESS_LENGTH);
  endp   = tp + SL_IPV6_ADDRESS_LENGTH;
  colonp = NULL;

  /* Leading :: requires some special handling.  */
  if (src == src_endp)
    return 0;
  if (*src == ':') {
    ++src;
    if (src == src_endp || *src != ':')
      return 0;
  }

  curtok       = src;
  xdigits_seen = 0;
  val          = 0;

  while (src < src_endp) {
    ch        = *src++;
    int digit = sli_hex_digit_value((char)ch);
    //printf(" digit :%d ",digit);
    if (digit >= 0) {
      if (xdigits_seen == 4)
        return 0;
      val <<= 4;
      val |= digit;
      if (val > 0xffff)
        return 0;
      ++xdigits_seen;
      continue;
    }
    if (ch == ':') {
      curtok = src;

      if (xdigits_seen == 0) {
        if (colonp)
          return 0;
        colonp = tp;
        continue;
      } else if (src == src_endp)
        return 0;
      if (tp + 2 > endp)
        return 0;

      *tp++ = (unsigned char)(val >> 8) & 0xff;
      *tp++ = (unsigned char)val & 0xff;

      xdigits_seen = 0;
      val          = 0;
      continue;
    }
  }

  if (xdigits_seen > 0) {
    if (tp + 2 > endp)
      return 0;
    *tp++ = (unsigned char)(val >> 8) & 0xff;
    *tp++ = (unsigned char)val & 0xff;
  }
  if (colonp != NULL) {
    /* Replace :: with zeros.  */
    if (tp == endp)
      /* :: would expand to a zero-width field.  */
      return 0;
    size_t n = tp - colonp;
    memmove(endp - n, colonp, n);
    memset(colonp, 0, endp - n - colonp);
    tp = endp;
  }
  if (tp != endp)
    return 0;

  memcpy(dst, tmp, SL_IPV6_ADDRESS_LENGTH);
  sli_little_to_big_endian((const unsigned int *)dst, (unsigned char *)ptr_result, SL_IPV6_ADDRESS_LENGTH);

  return 1;
}

sl_status_t convert_string_to_mac_address(const char *line, sl_mac_address_t *mac)
{
  // Verify we have the exact number of characters. Basic argument verification
  if (sl_strnlen((char *)line, 18) != 17) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t index = 0;
  while (index < 6) {
    // Read all the data and verify validity
    int char1 = DIGIT_VAL(line[0]);
    int char2 = DIGIT_VAL(line[1]);
    if (char1 == -1 || char2 == -1 || (line[2] != '\0' && line[2] != ':')) {
      return SL_STATUS_INVALID_PARAMETER;
    }

    // Store value
    mac->octet[index++] = (uint8_t)((uint8_t)char1 << 4) + (uint8_t)char2;
    line += 3;
  }

  return SL_STATUS_OK;
}

void sli_reverse_digits(unsigned char *xx, int no_digits)
{
  uint8_t temp;
  for (int count = 0; count < (no_digits / 2); count++) {
    temp                      = xx[count];
    xx[count]                 = xx[no_digits - count - 1];
    xx[no_digits - count - 1] = temp;
  }
}

void print_firmware_version(const sl_wifi_firmware_version_t *firmware_version)
{
  printf("\r\nFirmware version is: %x%x.%d.%d.%d.%d.%d.%d\r\n",
         firmware_version->chip_id,
         firmware_version->rom_id,
         firmware_version->major,
         firmware_version->minor,
         firmware_version->security_version,
         firmware_version->patch_num,
         firmware_version->customer_id,
         firmware_version->build_num);
}

__WEAK void sl_debug_log(const char *format, ...)
{
  UNUSED_PARAMETER(format);
}

void sl_redirect_log(const char *format, ...)
{
  UNUSED_PARAMETER(format);
}
