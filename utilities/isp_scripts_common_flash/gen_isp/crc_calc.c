#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define CRC_ALGO_BIT_BY_BIT 1

typedef unsigned int crc_t;
typedef struct {
  unsigned int width;       /*!< The width of the polynomial */
  crc_t poly;               /*!< The CRC polynomial */
  unsigned int reflect_in;  /*!< Whether the input shall be reflected or not */
  crc_t xor_in;             /*!< The initial value of the algorithm */
  unsigned int reflect_out; /*!< Wether the output shall be reflected or not */
  crc_t xor_out;            /*!< The value which shall be XOR-ed to the final CRC value */

  /* internal parameters */
  crc_t msb_mask;         /*!< a bitmask with the Most Significant Bit set to 1
                                     initialise as 1UL << (width - 1) */
  crc_t crc_mask;         /*!< a bitmask with all width bits set to 1
                                     initialise as (cfg->msb_mask - 1) | cfg->msb_mask */
  unsigned int crc_shift; /*!< a shift count that is used when width < 8
                                     initialise as cfg->width < 8 ? 8 - cfg->width : 0 */
} crc_cfg_t;

static crc_t crc_reflect(crc_t data, size_t data_len);
crc_t crc_init(const crc_cfg_t *cfg);
crc_t crc_update(const crc_cfg_t *cfg, crc_t crc, const unsigned char *data, size_t data_len);
crc_t crc_finalize(const crc_cfg_t *cfg, crc_t crc);
crc_t crc_calc(unsigned char *data_in, unsigned int num);

crc_t crc_calc(unsigned char *data_in, unsigned int num)
{
  crc_cfg_t cfg;
  crc_t crc;
  int i;

  cfg.width = 32;
  cfg.poly  = 0xd95eaae5;
  //cfg.poly = 0xd83e2927;
  cfg.reflect_in  = 1;
  cfg.xor_in      = 0;
  cfg.reflect_out = 1;
  cfg.xor_out     = 0;

  cfg.msb_mask  = 1UL << (cfg.width - 1);
  cfg.crc_mask  = (cfg.msb_mask - 1) | cfg.msb_mask;
  cfg.crc_shift = cfg.width < 8 ? 8 - cfg.width : 0;

  cfg.poly &= cfg.crc_mask;
  cfg.xor_in &= cfg.crc_mask;
  cfg.xor_out &= cfg.crc_mask;

  crc = crc_init(&cfg);
  crc = crc_update(&cfg, crc, (unsigned char *)data_in, num);
  crc = crc_finalize(&cfg, crc);

  return crc;
}

/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param data         The data word to be reflected.
 * \param data_len     The width of \a data expressed in number of bits.
 * \return             The reflected data.
 *****************************************************************************/
crc_t crc_reflect(crc_t data, size_t data_len)
{
  unsigned int i;
  crc_t ret;

  ret = data & 0x01;
  for (i = 1; i < data_len; i++) {
    data >>= 1;
    ret = (ret << 1) | (data & 0x01);
  }
  return ret;
}

/**
 * Calculate the initial crc value.
 *
 * \param cfg  A pointer to a initialised crc_cfg_t structure.
 * \return     The initial crc value.
 *****************************************************************************/
crc_t crc_init(const crc_cfg_t *cfg)
{
  unsigned int i;
  bool bit;
  crc_t crc = cfg->xor_in;
  for (i = 0; i < cfg->width; i++) {
    bit = crc & 0x01;
    if (bit) {
      crc = ((crc ^ cfg->poly) >> 1) | cfg->msb_mask;
    } else {
      crc >>= 1;
    }
  }
  return crc & cfg->crc_mask;
}

/**
 * Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param cfg      A pointer to a initialised crc_cfg_t structure.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 *****************************************************************************/
crc_t crc_update(const crc_cfg_t *cfg, crc_t crc, const unsigned char *data, size_t data_len)
{
  unsigned int i;
  bool bit;
  unsigned char c;

  while (data_len--) {
    if (cfg->reflect_in) {
      c = crc_reflect(*data++, 8);
    } else {
      c = *data++;
    }
    for (i = 0; i < 8; i++) {
      bit = crc & cfg->msb_mask;
      crc = (crc << 1) | ((c >> (7 - i)) & 0x01);
      if (bit) {
        crc ^= cfg->poly;
      }
    }
    crc &= cfg->crc_mask;
  }
  return crc & cfg->crc_mask;
}

/**
 * Calculate the final crc value.
 *
 * \param cfg  A pointer to a initialised crc_cfg_t structure.
 * \param crc  The current crc value.
 * \return     The final crc value.
 *****************************************************************************/
crc_t crc_finalize(const crc_cfg_t *cfg, crc_t crc)
{
  unsigned int i;
  bool bit;

  for (i = 0; i < cfg->width; i++) {
    bit = crc & cfg->msb_mask;
    crc = (crc << 1) | 0x00;
    if (bit) {
      crc ^= cfg->poly;
    }
  }
  if (cfg->reflect_out) {
    crc = crc_reflect(crc, cfg->width);
  }
  return (crc ^ cfg->xor_out) & cfg->crc_mask;
}

unsigned int checksum_addition(unsigned char *buf, unsigned int size, unsigned int prev_sum)
{
  unsigned int sum = prev_sum;
  unsigned int cnt;
  unsigned int cnt_limit;
  unsigned int dword;

  if (size == 0) {
    return sum;
  }

  cnt_limit = (size & (~0x3));
  /* Accumulate checksum */
  for (cnt = 0; cnt < cnt_limit; cnt += 4) {
    dword = *(unsigned int *)&buf[cnt];
    sum += dword;
    if (sum < dword) {
      /* In addition operation, if result is lesser than any one of the operand
			 * it means carry is generated. 
			 * Incrementing the sum to get ones compliment addition */

      sum++;
    }
  }

  /* Handle non dword-sized case */
  if (size & 0x3) {
    dword = 0xffffffff;
    dword = ~(dword << (8 * (size & 0x3)));
    /* Keeping only valid bytes and making upper bytes zeroes. */
    dword = (*(unsigned int *)&buf[cnt]) & dword;
    sum += dword;
    if (sum < dword) {
      sum++;
    }
  }

  return ~sum;
}
