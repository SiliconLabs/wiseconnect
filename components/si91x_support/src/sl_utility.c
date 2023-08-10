#include <stdbool.h>
#include <stdlib.h>
#include "sl_utility.h"
#include "sl_constants.h"
#include "string.h"
#include "cmsis_compiler.h"
#include <stdio.h>

#ifdef SPRINTF_CHAR
#define SPRINTF(x) strlen(sprintf /**/ x)
#else
#define SPRINTF(x) ((size_t)sprintf x)
#endif

extern char *strtok_r(char *, const char *, char **);

void convert_uint32_to_bytestream(uint16_t data, uint8_t *buffer)
{
  buffer[0] = (uint8_t)(data & 0xFF);
  buffer[1] = (uint8_t)((data >> 8) & 0xFF);
  buffer[2] = (uint8_t)((data >> 16) & 0xFF);
  buffer[3] = (uint8_t)((data >> 24) & 0xFF);
  return;
}

void print_sl_ip_address(sl_ip_address_t *sl_ip_address)
{
  if (sl_ip_address == NULL) {
    return;
  }

  uint8_t *ip_address;
  uint8_t ip_address_size;
  ip_address_size = sl_ip_address->type == SL_IPV4 ? SL_IPV4_ADDRESS_LENGTH : SL_IPV6_ADDRESS_LENGTH;
  ip_address      = sl_ip_address->type == SL_IPV4 ? sl_ip_address->ip.v4.bytes : sl_ip_address->ip.v6.bytes;
  
  if(sl_ip_address->type == SL_IPV4) {
    for (uint8_t ip_address_index = 0; ip_address_index < ip_address_size; ip_address_index++) {
      printf((ip_address_index != 0) ? ".%d" : "\r\nIP address is %d", ip_address[ip_address_index]);
    }
  printf("\r\n");
  }
  else if(sl_ip_address->type == SL_IPV6){
    char temp_buffer[46] = { 0 };
    sl_inet_ntop6((const unsigned char *)&(sl_ip_address->ip.v6),
                  (char *)temp_buffer,
                  sizeof(temp_buffer));
    printf("%s\r\n", temp_buffer);
  }
}

void print_mac_address(sl_mac_address_t *mac_address)
{
  if (mac_address == NULL) {
      return;
    }
  printf("%2X:%2X:%2X:%2X:%2X:%2X", mac_address->octet[0], mac_address->octet[1], mac_address->octet[2], mac_address->octet[3], mac_address->octet[4], mac_address->octet[5]);
}

char *sl_inet_ntop6(const unsigned char *input, char *dst, uint32_t size)
{
  char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
  struct {
    int base, len;
  } best, cur;
  unsigned int words[SL_IPV6_ADDRESS_LENGTH / 2];
  int i;
  unsigned int ip_big_endian[4];
  unsigned char *src;
  src = (unsigned char *)&ip_big_endian;

  little_to_big_endian((unsigned int *)input, (unsigned char *)ip_big_endian, SL_IPV6_ADDRESS_LENGTH);

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
      if (cur.base == -1)
        cur.base = i, cur.len = 1;
      else
        cur.len++;
    } else {
      if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
          best = cur;
        cur.base = -1;
      }
    }
  }
  if (cur.base != -1) {
    if (best.base == -1 || cur.len > best.len)
      best = cur;
  }
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
  return strcpy(dst, tmp);
}

static int hex_digit_value(char ch)
{
  if ('0' <= ch && ch <= '9')
    return ch - '0';
  if ('a' <= ch && ch <= 'f')
    return ch - 'a' + 10;
  if ('A' <= ch && ch <= 'F')
    return ch - 'A' + 10;
  return -1;
}

void little_to_big_endian(unsigned int *source, unsigned char *result, unsigned int length)
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
  unsigned char tmp[SL_IPV6_ADDRESS_LENGTH], *tp, *endp, *colonp;
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
    int digit = hex_digit_value(ch);
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
  little_to_big_endian((unsigned int *)dst, (unsigned char *)ptr_result, SL_IPV6_ADDRESS_LENGTH);

  return 1;
}

__WEAK void sl_debug_log(const char* format, ...) {
  UNUSED_PARAMETER(format);
}
