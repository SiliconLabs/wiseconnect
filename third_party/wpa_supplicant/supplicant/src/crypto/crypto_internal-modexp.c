/*
 * Crypto wrapper for internal crypto implementation - modexp
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "tls/bignum.h"
#include "crypto.h"
#include "supplicant_mgmt_if.h"

//TODO: Implementatinn may change as dh operations may move to hw. 
extern int crypto_mod_exp_hw(const NN_DIGIT *base,
                      uint32 base_len,
                      const NN_DIGIT *power,
                      uint32 power_len,
                      const NN_DIGIT *modulus,
                      uint32 modulus_len,
                      NN_DIGIT *result,
                      uint32 *result_len);

int crypto_dh_init(u8 generator, const u8 *prime, size_t prime_len, u8 *privkey,
		   u8 *pubkey)
{
	size_t pubkey_len, pad;

	if (os_get_random(privkey, prime_len) < 0)
		return -1;
	if (os_memcmp(privkey, prime, prime_len) > 0) {
		/* Make sure private value is smaller than prime */
		privkey[0] = 0;
	}

	pubkey_len = prime_len;
	if (crypto_mod_exp(&generator, 1, privkey, prime_len, prime, prime_len,
			   pubkey, &pubkey_len) < 0)
		return -1;
	if (pubkey_len < prime_len) {
		pad = prime_len - pubkey_len;
		os_memmove(pubkey + pad, pubkey, pubkey_len);
		os_memset(pubkey, 0, pad);
	}

	return 0;
}


int crypto_dh_derive_secret(u8 generator, const u8 *prime, size_t prime_len,
			    const u8 *order, size_t order_len,
			    const u8 *privkey, size_t privkey_len,
			    const u8 *pubkey, size_t pubkey_len,
			    u8 *secret, size_t *len)
{
#if UNUSED_FEAT_IN_SUPP_29	
	struct bignum *pub;
	int res = -1;

	if (pubkey_len > prime_len ||
	    (pubkey_len == prime_len &&
	     os_memcmp(pubkey, prime, prime_len) >= 0))
		return -1;

	pub = bignum_init();
	if (!pub || bignum_set_unsigned_bin(pub, pubkey, pubkey_len) < 0 ||
	    bignum_cmp_d(pub, 1) <= 0)
		goto fail;

	if (order) {
		struct bignum *p, *q, *tmp;
		int failed;

		/* verify: pubkey^q == 1 mod p */
		p = bignum_init();
		q = bignum_init();
		tmp = bignum_init();
		failed = !p || !q || !tmp ||
			bignum_set_unsigned_bin(p, prime, prime_len) < 0 ||
			bignum_set_unsigned_bin(q, order, order_len) < 0 ||
			bignum_exptmod(pub, q, p, tmp) < 0 ||
			bignum_cmp_d(tmp, 1) != 0;
		bignum_deinit(p);
		bignum_deinit(q);
		bignum_deinit(tmp);
		if (failed)
			goto fail;
	}

	res = crypto_mod_exp(pubkey, pubkey_len, privkey, privkey_len,
			     prime, prime_len, secret, len);
fail:
	bignum_deinit(pub);
	return res;
#else
#ifndef ENABLE_UMAC_ROM_PTR 
	if (crypto_mod_exp_hw((NN_DIGIT *)pubkey, pubkey_len, (NN_DIGIT *)privkey, privkey_len,
				(NN_DIGIT *)prime, prime_len, (NN_DIGIT *)secret, (uint32_t *)len) < 0)
#else
	if (crypto_rom_bss_ptr->crypto_mod_exp_hw((NN_DIGIT *)pubkey, pubkey_len, (NN_DIGIT *)privkey, privkey_len,
				(NN_DIGIT *)prime, prime_len, (NN_DIGIT *)secret, (uint32_t *)len) < 0) 
#endif
     {
		wpa_printf(MSG_INFO, "DH: crypto_mod_exp failed");
		return -1;
	}
	return 0;
#endif	
}


int crypto_mod_exp(const u8 *base, size_t base_len,
		   const u8 *power, size_t power_len,
		   const u8 *modulus, size_t modulus_len,
		   u8 *result, size_t *result_len)
{
#if 1
#ifndef ENABLE_UMAC_ROM_PTR
		if (crypto_mod_exp_hw((const NN_DIGIT *)base, base_len,
			   (const NN_DIGIT *)power, power_len,
			   (const NN_DIGIT *)modulus, modulus_len, (NN_DIGIT *)result,
			   (uint32_t *)result_len) < 0)
#else
	if (crypto_rom_bss_ptr->crypto_mod_exp_hw((const NN_DIGIT *)base, base_len,
			   (const NN_DIGIT *)power, power_len,
			   (const NN_DIGIT *)modulus, modulus_len, (NN_DIGIT *)result,
			   (uint32_t *)result_len) < 0) 
#endif
     {
		wpa_printf(MSG_INFO, "DH: crypto_mod_exp failed");
		return 0;
	}
  return 0;
#else
	struct bignum *bn_base, *bn_exp, *bn_modulus, *bn_result;
	int ret = -1;

	bn_base = bignum_init();
	bn_exp = bignum_init();
	bn_modulus = bignum_init();
	bn_result = bignum_init();

	if (bn_base == NULL || bn_exp == NULL || bn_modulus == NULL ||
	    bn_result == NULL)
		goto error;

	if (bignum_set_unsigned_bin(bn_base, base, base_len) < 0 ||
	    bignum_set_unsigned_bin(bn_exp, power, power_len) < 0 ||
	    bignum_set_unsigned_bin(bn_modulus, modulus, modulus_len) < 0)
		goto error;

	if (bignum_exptmod(bn_base, bn_exp, bn_modulus, bn_result) < 0)
		goto error;

	ret = bignum_get_unsigned_bin(bn_result, result, result_len);

error:
	bignum_deinit(bn_base);
	bignum_deinit(bn_exp);
	bignum_deinit(bn_modulus);
	bignum_deinit(bn_result);
	return ret;
#endif
}
