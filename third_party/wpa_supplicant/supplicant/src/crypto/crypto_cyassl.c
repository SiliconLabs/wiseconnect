/*
 * WPA Supplicant / Empty template functions for crypto wrapper
 * Copyright (c) 2005, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "crypto.h"

/* CyaSSL headers */
#include <cyassl/ctaocrypt/md4.h>
#include <cyassl/ctaocrypt/md5.h>
#include <cyassl/ctaocrypt/sha.h>
#include <cyassl/ctaocrypt/sha256.h>
#ifdef WOLFSSL_4_6_0
#include <cyassl/ctaocrypt/sha512.h>
#endif
#include <cyassl/ctaocrypt/arc4.h>
#include <cyassl/ctaocrypt/des3.h>
#include <cyassl/ctaocrypt/aes.h>

#ifdef CYASSL_SUPP_PORTING
#include "hmac.h"
#endif

#ifdef NO_DES3
#include "rsi_debug_prints.h"
#endif

#include "sha1.h"
#include "md5.h"
#ifdef ROM_PATCH_FRAMEWORK
#include "wrappers_wlan.h"
#include "wrappers_bootloader_common.h"
#include "wrappers_ram_bt_wlan.h"
#endif 
#ifdef SUPPLICANT_NON_ROM
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    Md4    md4;
    size_t i;

    InitMd4(&md4);

    for (i = 0; i < num_elem; i++)
        Md4Update(&md4, addr[i], len[i]);

    Md4Final(&md4, mac);

    return 0;
}

int des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
#if defined(SSL_PORTING) && !defined(NO_DES3)
	u8  pkey[8], next, tmp;
	int i;
    Des des;

	/* Add parity bits to the key */
	next = 0;
	for (i = 0; i < 7; i++) {
		tmp = key[i];
		pkey[i] = (tmp >> i) | next | 1;
		next = tmp << (7 - i);
	}
	pkey[i] = next | 1;

	Des_SetKey(&des, pkey, NULL, DES_ENCRYPTION);
	Des_EcbEncrypt(&des, cypher, clear, DES_BLOCK_SIZE);
#else
  UMAC_ASSERTION(UMAC_ASSERT_DES_IS_NOT_ENABLED);
#endif
return 0;

}


int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    Md5    md5;
    size_t i;

    InitMd5(&md5);

    for (i = 0; i < num_elem; i++)
        Md5Update(&md5, addr[i], len[i]);

    Md5Final(&md5, mac);

    return 0;
}


int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		u8 *mac)
{
    Sha    sha;
    size_t i;

    InitSha(&sha);

    for (i = 0; i < num_elem; i++)
        ShaUpdate(&sha, addr[i], len[i]);

    ShaFinal(&sha, mac);

    return 0;
}


int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
    Sha256 sha256;
    size_t i;

    InitSha256(&sha256);

    for (i = 0; i < num_elem; i++)
        Sha256Update(&sha256, addr[i], len[i]);

    Sha256Final(&sha256, mac);

    return 0;
}

#ifdef SSL_PORTING
int sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
    Sha384 sha384;
    size_t i;

    InitSha384(&sha384);

    for (i = 0; i < num_elem; i++)
        Sha384Update(&sha384, addr[i], len[i]);

    Sha384Final(&sha384, mac);

    return 0;
}
int sha512_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
    Sha512 sha512;
    size_t i;

    InitSha512(&sha512);

    for (i = 0; i < num_elem; i++)
        Sha512Update(&sha512, addr[i], len[i]);

    Sha512Final(&sha512, mac);

    return 0;
}
#endif
#endif //SUPPLICANT_NON_ROM


//#ifdef SUPPLICANT_ROM
void * aes_encrypt_init(const u8 *key, size_t len)
{
    Aes* aes = os_malloc(sizeof(Aes));

    if (aes == NULL)
        return NULL;

    if (AesSetKey(aes, key, len, NULL, AES_ENCRYPTION) < 0) {
        os_free(aes);
        return NULL;
    }

    return aes;
}


int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
    AesEncryptDirect((Aes*)ctx, crypt, plain);
    return 0;
}


void aes_encrypt_deinit(void *ctx)
{
    os_free(ctx);
}


void* aes_decrypt_init(const u8 *key, size_t len)
{
    Aes* aes = os_malloc(sizeof(Aes));

    if (aes == NULL)
        return NULL;

    if (AesSetKey(aes, key, len, NULL, AES_DECRYPTION) < 0) {
        os_free(aes);
        return NULL;
    }

    return aes;
}


int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
    AesDecryptDirect((Aes*)ctx, plain, crypt);
    return 0;
}


void aes_decrypt_deinit(void *ctx)
{
    os_free(ctx);
}
//#endif //SUPPLICANT_ROM

#ifdef SUPPLICANT_NON_ROM
int rc4_skip(const u8 *key, size_t keylen, size_t skip, u8 *data,
             size_t data_len)
{
    Arc4          arc4;
	unsigned char skip_buf[16];

    Arc4SetKey(&arc4, key, keylen);

	while (skip >= sizeof(skip_buf)) {
		size_t len = skip;
		if (len > sizeof(skip_buf))
			len = sizeof(skip_buf);
        Arc4Process(&arc4, skip_buf, skip_buf, len);
		skip -= len;
	}

    Arc4Process(&arc4, data, data, data_len);

    return 0;
}

#endif //SUPPLICANT_NON_ROM

//#ifdef SUPPLICANT_ROM
/**
 * hmac_vector - HMAC-SHA1 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_vector(int type, const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
  Hmac        hmac;  
  int ret  = 0, i;
  if (num_elem > 5) {
    /*
     * Fixed limit on the number of fragments to avoid having to
     * allocate memory (which could fail).
     */
    return -1;
  }

  ret = HmacSetKey(&hmac, type, key, key_len);
  if (ret != 0)
  {
    return -1;
  }
  for (i = 0; i < num_elem; i++)
  {
    ret = HmacUpdate(&hmac, addr[i], len[i]);
    if (ret != 0)
    {
      return -1;
    }
  }
  ret = HmacFinal(&hmac, mac);
  if (ret != 0)
  {
    return -1;
  }
  return 0;
}

/**
 * hmac_sha1_vector - HMAC-SHA1 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(SHA, key, key_len, num_elem, addr, len, mac);
}

/**
 * hmac_sha1 - HMAC-SHA1 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 of failure
 */
int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	       u8 *mac)
{
	return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}
//#endif //SUPPLICANT_ROM

#ifdef SUPPLICANT_NON_ROM
/**
 * hmac_sha256_vector - HMAC-SHA256 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (32 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
		       const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(SHA256, key, key_len, num_elem, addr, len, mac);
}


/**
 * hmac_sha256 - HMAC-SHA256 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (32 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

/**
 * tls_prf_sha1_md5 - Pseudo-Random Function for TLS (TLS-PRF, RFC 2246)
 * @secret: Key for PRF
 * @secret_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @seed: Seed value to bind into the key
 * @seed_len: Length of the seed
 * @out: Buffer for the generated pseudo-random key
 * @outlen: Number of bytes of key to generate
 * Returns: 0 on success, -1 on failure.
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key in TLS. This PRF is defined in RFC 2246, Chapter 5.
 */
int tls_prf_sha1_md5(const u8 *secret, size_t secret_len, const char *label,
		     const u8 *seed, size_t seed_len, u8 *out, size_t outlen)
{
#ifndef WOLFSSL_4_6_0
  return PRF(out, outlen, secret,secret_len, label, strlen(label), seed, seed_len,0,0);
#else
  return wc_PRF_TLS(out, outlen, secret,secret_len, (const byte *)label, strlen(label), seed, seed_len,0,0,NULL,INVALID_DEVID);
#endif
}
/**
 * hmac_sha384_vector - HMAC-SHA384 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (48 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(SHA384, key, key_len, num_elem, addr, len, mac);
}


/**
 * hmac_sha512_vector - HMAC-SHA512 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (64 bytes)
 * Returns: 0 on success, -1 on failure
 */
int hmac_sha512_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(SHA512, key, key_len, num_elem, addr, len, mac);
}
int tls_prf_sha256(const u8 *secret, size_t secret_len, const char *label,
    const u8 *seed, size_t seed_len, u8 *out, size_t outlen)
{
#ifndef WOLFSSL_4_6_0
  return PRF(out, outlen, secret,secret_len, label, strlen(label), seed, seed_len,1,0);
#else
  return wc_PRF_TLS(out, outlen, secret,secret_len, (const byte *)label, strlen(label), seed, seed_len,1,0, NULL, INVALID_DEVID);
#endif
}   

#endif //SUPPLICANT_NON_ROM
