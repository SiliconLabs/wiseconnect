#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
// #include "crypto_mbedtls.h"
#include <assert.h>

struct crypto_ec {
  mbedtls_mpi *A;
  mbedtls_ecp_group *grp;
};

void print_hex(const uint8_t *out, const size_t out_length)
{
  printf("------------------- len: %zu -------------------\n", out_length);
  for (size_t i = 0; i < out_length; i++) {
    if (i % 16 == 0) {
      printf("\n");
    }
    // printf("%02x ", out[i]);
    printf("%02x", out[i]);
  }
  printf("\n--------------------- end ---------------------\n");
}
void print_bignum_data(const struct crypto_bignum *big_num)
{
  int n = ((mbedtls_mpi *)big_num)->private_n;
  for (int i = 0; i < n; i++) {
    print_hex((const uint8_t *)(((mbedtls_mpi *)big_num)->private_p) + i, 8);
  }
}
uint8_t *hexStringToArray(const char *hexString, size_t *length, int null_flag)
{
  size_t len = strlen(hexString);
  if (len % 2 != 0) {
    printf("length of hexstring is not even\n");
    return NULL; // Hex string should have an even length
  }
  unsigned char *result;
  if (!len) {
    *length = 0;
    return NULL;
  }
  size_t numBytes = len / 2;
  if (null_flag) {
    result           = (unsigned char *)malloc(numBytes + 1);
    result[numBytes] = '\0';
  } else {
    result = (unsigned char *)malloc(numBytes);
  }

  if (result == NULL) {
    printf("allocation failed");
    return NULL; // Memory allocation failed
  }

  for (size_t i = 0; i < numBytes; i++) {
    unsigned int byte;
    if (sscanf(hexString + 2 * i, "%2x", &byte) != 1) {
      free(result);
      return NULL; // Failed to parse hex
    }
    result[i] = (unsigned char)byte;
  }

  *length = numBytes;
  // print_hex(result, numBytes);
  return result;
}

int compare_hex_arrays(const unsigned char *array1, const unsigned char *array2, size_t length)
{
  for (size_t i = 0; i < length; i++) {
    if (array1[i] != array2[i]) {
      return 0;
    }
  }
  return 1;
}

int test_key_destroy()
{
  psa_status_t status                 = psa_crypto_init();
  psa_key_attributes_t key_attributes = psa_key_attributes_init();
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DERIVE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_HKDF_EXPAND(PSA_ALG_SHA_256));
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
  psa_key_id_t key_id;
  status = psa_destroy_key(key_id);
  printf("--%d--", status);
  return 0;
}

void test_md5()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[16];
  const uint8_t *message_array[1];
  size_t len[1];
  char s[]         = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  length           = 62;
  message_array[0] = s;
  len[0]           = length;
  md5_vector(num_elem, (const uint8_t **)message_array, len, digest);
  printf("MD5 digest:\n");
  print_hex(digest, 16);
  uint8_t *hash = hexStringToArray("d174ab98d277d9f5a5611c2c9f419d9f", &length, 0);
  assert(compare_hex_arrays(digest, hash, length));
  printf("expected:\n");
  print_hex(hash, length);
}

void test_sha_1()
{
  size_t length;
  size_t num_elem = 2;
  uint8_t digest[20];
  const uint8_t *message_array[2];
  size_t len[2];
  uint8_t *hex1    = hexStringToArray("f6c237fb3cfe95ec8414cc16d203b4874e644cc9", &length, 0);
  message_array[0] = hex1;
  len[0]           = length;
  hex1             = hexStringToArray("a543465cad2dc563488a659e8a2e7c981e2a9f22e5e868ffe1", &length, 0);
  message_array[1] = hex1;
  len[1]           = length;
  sha1_vector(num_elem, (const uint8_t **)message_array, len, digest);
  printf("SHA-1 digest:\n");
  print_hex(digest, 20);
  uint8_t *hash = hexStringToArray("199d986ed991b99a071f450c6b1121a727e8c735", &length, 0);
  assert(compare_hex_arrays(digest, hash, length));
  printf("expected:\n");
  print_hex(hash, length);
}

void test_sha_256()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[32];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 =
    hexStringToArray("e68cb6d8c1866c0a71e7313f83dc11a5809cf5cfbeed1a587ce9c2c92e022abc1644bb", &length, 0);
  message_array[0] = hex1;
  len[0]           = length;
  sha256_vector(num_elem, (const uint8_t **)message_array, len, digest);
  printf("SHA-256 digest:\n");
  print_hex(digest, 32);
  uint8_t *hash = hexStringToArray("c2684c0dbb85c232b6da4fb5147dd0624429ec7e657991edd95eda37a587269e", &length, 0);
  assert(compare_hex_arrays(digest, hash, length));
  printf("expected:\n");
  print_hex(hash, length);
}

void test_sha_384()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[48];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray("345c3c022e20144e135604078762ef5c8a8f038cf1b1d6a91709b59dd068396a9e971ab628f74886e76"
                                   "5384a23607c1a1e6e973f8fbb0ff55104c46f5db3",
                                   &length,
                                   0);
  message_array[0] = hex1;
  len[0]           = length;
  sha384_vector(num_elem, (const uint8_t **)message_array, len, digest);
  printf("SHA-384 digest:\n");
  print_hex(digest, 48);
  uint8_t *hash =
    hexStringToArray("bfb1d5ee3a0b629058ecc521c706c2f9241c48cda3dcfdba660a2601d832a7a872a2bb840f3b98d21c37e28f9041a5b2",
                     &length,
                     0);
  assert(compare_hex_arrays(digest, hash, length));
  printf("expected:\n");
  print_hex(hash, length);
}

void test_sha_512()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[64];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1    = hexStringToArray("", &length, 0);
  message_array[0] = hex1;
  len[0]           = length;
  sha512_vector(num_elem, (const uint8_t **)message_array, len, digest);
  printf("SHA-512 digest:\n");
  print_hex(digest, 64);
  uint8_t *hash = hexStringToArray("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8"
                                   "318d2877eec2f63b931bd47417a81a538327af927da3e",
                                   &length,
                                   0);
  assert(compare_hex_arrays(digest, hash, length));
  printf("expected:\n");
  print_hex(hash, length);
}

void test_hmac_md5()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[16];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray(
    "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    &length,
    0);
  message_array[0]  = hex1;
  len[0]            = length;
  uint8_t *hmac_key = hexStringToArray("0102030405060708090a0b0c0d0e0f10111213141516171819", &length, 0);
  hmac_md5_vector(hmac_key, length, num_elem, message_array, len, digest);
  printf("HMAC MD5:\n");
  print_hex(digest, 16);
  uint8_t *hmac = hexStringToArray("697eaf0aca3a3aea3a75164746ffaa79", &length, 0);
  assert(compare_hex_arrays(digest, hmac, length));
  printf("expected:\n");
  print_hex(hmac, length);
}

void test_hmac_sha1()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[20];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray(
    "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    &length,
    0);
  message_array[0]  = hex1;
  len[0]            = length;
  uint8_t *hmac_key = hexStringToArray("0102030405060708090a0b0c0d0e0f10111213141516171819", &length, 0);
  hmac_sha1_vector(hmac_key, length, num_elem, message_array, len, digest);
  printf("HMAC SHA1:\n");
  print_hex(digest, 20);
  uint8_t *hmac = hexStringToArray("4c9007f4026250c6bc8414f9bf50c86c2d7235da", &length, 0);
  assert(compare_hex_arrays(digest, hmac, length));
  printf("expected:\n");
  print_hex(hmac, length);
}

void test_hmac_sha256()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[32];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray(
    "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
    &length,
    0);
  message_array[0]  = hex1;
  len[0]            = length;
  uint8_t *hmac_key = hexStringToArray("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", &length, 0);
  hmac_sha256_vector(hmac_key, length, num_elem, message_array, len, digest);
  printf("HMAC SHA256:\n");
  print_hex(digest, 32);
  uint8_t *hmac = hexStringToArray("773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe", &length, 0);
  assert(compare_hex_arrays(digest, hmac, length));
  printf("expected:\n");
  print_hex(hmac, length);
}

void test_hmac_sha384()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[48];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray(
    "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
    &length,
    0);
  message_array[0]  = hex1;
  len[0]            = length;
  uint8_t *hmac_key = hexStringToArray("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", &length, 0);
  hmac_sha384_vector(hmac_key, length, num_elem, message_array, len, digest);
  printf("HMAC SHA384:\n");
  print_hex(digest, 48);
  uint8_t *hmac =
    hexStringToArray("88062608d3e6ad8a0aa2ace014c8a86f0aa635d947ac9febe83ef4e55966144b2a5ab39dc13814b94e3ab6e101a34f27",
                     &length,
                     0);
  assert(compare_hex_arrays(digest, hmac, length));
  printf("expected:\n");
  print_hex(hmac, length);
}

void test_hmac_sha512()
{
  size_t length;
  size_t num_elem = 1;
  uint8_t digest[64];
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1 = hexStringToArray(
    "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd",
    &length,
    0);
  message_array[0]  = hex1;
  len[0]            = length;
  uint8_t *hmac_key = hexStringToArray("0102030405060708090a0b0c0d0e0f10111213141516171819", &length, 0);
  hmac_sha512_vector(hmac_key, length, num_elem, message_array, len, digest);
  printf("HMAC SHA 512:\n");
  print_hex(digest, 64);
  uint8_t *hmac = hexStringToArray("b0ba465637458c6990e5a8c5f61d4af7e576d97ff94b872de76f8050361ee3dba91ca5c11aa25eb4d67"
                                   "9275cc5788063a5f19741120c4f2de2adebeb10a298dd",
                                   &length,
                                   0);
  assert(compare_hex_arrays(digest, hmac, length));
  printf("expected:\n");
  print_hex(hmac, length);
}

void test_pbkdf2()
{
  size_t length;
  char pwd[]        = "passwordPASSWORDpassword";
  const char ssid[] = "saltSALTsaltSALTsaltSALTsaltSALTsalt";
  uint8_t derived_key[25];
  pbkdf2_sha1(pwd, ssid, strlen(ssid), 4096, derived_key, 25);
  printf("pbkdf2 sha1 \n");
  print_hex(derived_key, 25);
  uint8_t *dk = hexStringToArray("3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038", &length, 0);
  assert(compare_hex_arrays(derived_key, dk, length));
  printf("expected\n");
  print_hex(dk, length);
}

void test_aes_128_cbc()
{
  size_t length;
  uint8_t *key  = hexStringToArray("625eefa18a4756454e218d8bfed56e36", &length, 0);
  uint8_t *iv   = hexStringToArray("73d9d0e27c2ec568fbc11f6a0998d7c8", &length, 0);
  uint8_t *data = hexStringToArray("360dc1896ce601dfb2a949250067aad96737847a4580ede2654a329b842fe81e", &length, 0);
  printf("input msg:\n");
  print_hex(data, length);
  aes_128_cbc_encrypt(key, iv, data, length);
  printf("expected:\n");
  uint8_t *expected = hexStringToArray("5d6fed86f0c4fe59a078d6361a142812514b295dc62ff5d608a42ea37614e6a1", &length, 0);
  print_hex(expected, length);
  printf("now decrypt\n");
  aes_128_cbc_decrypt(key, iv, data, length);
  uint8_t *data1 = hexStringToArray("360dc1896ce601dfb2a949250067aad96737847a4580ede2654a329b842fe81e", &length, 0);
  assert(compare_hex_arrays(data, data1, length));
  printf("expected decryption\n");
  print_hex(data1, length);
}

void test_des_encryption()
{
  uint8_t pkey[8], next, tmp;
  int i;
  size_t length;
  uint8_t *key    = hexStringToArray("057055D43FED5B", &length, 0);
  uint8_t *clear  = hexStringToArray("42FD443059577FA2", &length, 0);
  uint8_t *cipher = (uint8_t *)malloc(length);
  printf("key passed\n");
  print_hex(key, 7);
  des_encrypt(clear, key, cipher);
  printf("cipher:\n");
  print_hex(cipher, 8);
  uint8_t *expected = hexStringToArray("AF37FB421F8C4095", &length, 0);
  assert(compare_hex_arrays(cipher, expected, length));
  printf("expected\n");
  print_hex(expected, length);
}

void test_multi_part_aes_encryption()
{
  size_t length;
  uint8_t *key   = hexStringToArray("edfdb257cb37cdf182c5455b0c0efebb", &length, 0);
  void *p        = aes_encrypt_init(key, length);
  uint8_t *plain = hexStringToArray("1695fe475421cace3557daca01f445ff", &length, 0);
  uint8_t *crypt = (uint8_t *)malloc(length);
  aes_encrypt(p, plain, crypt);
  printf("encypted:\n");
  print_hex(crypt, 16);
  uint8_t *expected = hexStringToArray("7888beae6e7a426332a7eaa2f808e637", &length, 0);
  assert(compare_hex_arrays(expected, crypt, length));
  printf("expected\n");
  print_hex(expected, length);
  aes_encrypt_deinit(p);
}

void test_multi_part_aes_decryption()
{
  size_t length;
  uint8_t *key   = hexStringToArray("54b760dd2968f079ac1d5dd20626445d", &length, 0);
  void *p        = aes_decrypt_init(key, length);
  uint8_t *crypt = hexStringToArray("065bd5a9540d22d5d7b0f75d66cb8b30", &length, 0);
  uint8_t *plain = (uint8_t *)malloc(length);
  aes_decrypt(p, crypt, plain);
  printf("plain text:\n");
  print_hex(plain, 16);
  uint8_t *expected = hexStringToArray("46f2c98932349c338e9d67f744a1c988", &length, 0);
  assert(compare_hex_arrays(expected, plain, length));
  printf("expected\n");
  print_hex(expected, length);
  aes_decrypt_deinit(p);
}

void test_omac1_aes()
{
  size_t length;
  uint8_t digest[48];
  size_t num_elem = 1;
  const uint8_t *message_array[1];
  size_t len[1];
  uint8_t *hex1    = hexStringToArray("6bc1bee22e409f96e93d7e117393172a", &length, 0);
  message_array[0] = hex1;
  len[0]           = length;
  uint8_t *key     = hexStringToArray("2b7e151628aed2a6abf7158809cf4f3c", &length, 0);
  omac1_aes_vector(key, 16, 1, message_array, len, digest);
  printf("MAC:\n");
  print_hex(digest, 16);
  uint8_t *omac1 = hexStringToArray("070a16b46b4d4144f79bdd9dd04a287c", &length, 0);
  assert(compare_hex_arrays(omac1, digest, length));
  printf("expected:\n");
  print_hex(omac1, length);
}

void test_bignum_init()
{
  struct crypto_bignum *p = crypto_bignum_init();
  printf("\n number of limbs = %d\n", ((mbedtls_mpi *)p)->private_n);
}

void test_bignum_deinit()
{
  struct crypto_bignum *p = crypto_bignum_init();
  crypto_bignum_deinit(p, 1);
  printf("\n number of limbs = %d\n", ((mbedtls_mpi *)p)->private_n);
}

void test_bignum_add()
{
  int n;
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "987654321");
  struct crypto_bignum *b = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "123456789");
  struct crypto_bignum *c = crypto_bignum_init();
  int ret                 = crypto_bignum_add(a, b, c);
  printf("return value = %d\n", ret);
  print_bignum_data(c);
}

void test_bignum_rshift()
{
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "9a123315567b0");
  struct crypto_bignum *r = crypto_bignum_init();
  int n                   = 4;
  int ret                 = crypto_bignum_rshift(a, n, r);
  print_bignum_data(r);
  printf("return val = %d\n", ret);
}

void test_bignum_expmod()
{
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "13");
  struct crypto_bignum *b = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "7");
  struct crypto_bignum *c = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)c, 16, "37");
  struct crypto_bignum *d = crypto_bignum_init();
  int ret                 = crypto_bignum_exptmod(a, b, c, d);
  printf("return val = %d\n", ret);
  print_bignum_data(d);
}

void test_bignum_mod()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "100000000");
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "379C");
  int ret = crypto_bignum_mod(a, b, c);
  print_bignum_data(c);
  printf("return val = %d\n", ret);
}

void test_bignum_is_zero()
{
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "-0");
  int n = ((mbedtls_mpi *)a)->private_n;
  for (int i = 0; i < n; i++) {
    print_hex((const uint8_t *)(((mbedtls_mpi *)a)->private_p) + i, 8);
  }
  printf("sign = %d\n", ((mbedtls_mpi *)a)->private_s);
  assert(crypto_bignum_is_zero(a));
  if (crypto_bignum_is_zero(a))
    printf("zero");
  else
    printf("non zero");
}

void test_bignum_is_one()
{
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "1");
  print_bignum_data(a);
  printf("sign = %d\n", ((mbedtls_mpi *)a)->private_s);
  assert(crypto_bignum_is_zero(a));
  if (crypto_bignum_is_one(a))
    printf("one");
  else
    printf("not one");
}

void test_bignum_inverse()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "3d");
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "13");
  int ret = crypto_bignum_inverse(a, b, c);
  print_bignum_data(c);
  printf("return value = %d\n", ret);
}

void test_bignum_mulmod()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  struct crypto_bignum *d = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "2");
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "3");
  mbedtls_mpi_read_string((mbedtls_mpi *)c, 16, "9");
  int ret = crypto_bignum_mulmod(a, b, c, d);
  print_bignum_data(d);
  printf("%d\n", ret);
}

void test_bignum_init_set()
{
  size_t length;
  uint8_t *buf            = hexStringToArray("1209ab070ba117", &length, 0);
  struct crypto_bignum *p = crypto_bignum_init_set(buf, length);
  print_bignum_data(p);
}

void test_bignum_comp()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "123456789101112");
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "9123456789101112");
  int ret = crypto_bignum_cmp(a, b);
  assert(ret == -1);
  printf("return val = %d\n", ret);
}

void test_bignum_sub()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "123456789101112140");
  print_bignum_data(a);
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "1100224466");
  print_bignum_data(b);
  int ret = crypto_bignum_sub(a, b, c);
  print_bignum_data(c);
  printf("ret val = %d\n", ret);
}

void test_bignum_init_uint()
{
  struct crypto_bignum *x;
  printf("how -ve value stored?\n");
  int a = -10;
  print_hex((uint8_t *)&a, 4);
  x = crypto_bignum_init_uint(-10);
  print_hex((uint8_t *)((mbedtls_mpi *)x)->private_p, 8);
  printf("%d--\n", ((mbedtls_mpi *)x)->private_n);
  printf("size of limb %ld", sizeof(mbedtls_mpi_uint));
  printf("\nsign: %d\n", ((mbedtls_mpi *)x)->private_s);
}

void test_bignum_bits()
{
  struct crypto_bignum *a = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 10, "123456789101112140");
  printf("\nnumber of bits: %d", crypto_bignum_bits(a));
}

void test_bignum_addmod()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  struct crypto_bignum *d = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "100000999999999aaff6611");
  print_bignum_data(a);
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "110022dd");
  print_bignum_data(b);
  mbedtls_mpi_read_string((mbedtls_mpi *)c, 16, "25");
  int ret = crypto_bignum_addmod(a, b, c, d);
  printf("ret = %d\n", ret);
  print_bignum_data(d);
}

void test_bignum_sqrmod()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *b = crypto_bignum_init();
  struct crypto_bignum *c = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 16, "07192b95ffc8da78631011ed6b24cdd573f977a11e794811");
  // print_bignum_data(a);
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 16, "fffffffffffffffffffffffffffffffeffffffffffffffff");
  // print_bignum_data(b);
  int ret = crypto_bignum_sqrmod(a, b, c);
  printf("ret = %d\n", ret);
  print_bignum_data(c);
}

void test_bignum_legendre()
{
  struct crypto_bignum *a = crypto_bignum_init();
  struct crypto_bignum *p = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)a, 10, "0");
  mbedtls_mpi_read_string((mbedtls_mpi *)p, 10, "41");
  int ret = crypto_bignum_legendre(a, p);
  printf("ret val = %d\n", ret);
}

void test_bignum_to_bin()
{
  int ret;
  uint8_t *buf   = (uint8_t *)malloc(20);
  size_t buf_len = 20;
  size_t pad_len = 10;
  mbedtls_mpi *p;
  mbedtls_mpi_read_string((mbedtls_mpi *)p, 16, "0930200000000000000002");
  printf("mpi size = %zu\n", mbedtls_mpi_size((const mbedtls_mpi *)p));
  ret = crypto_bignum_to_bin((const struct crypto_bignum *)p, buf, buf_len, pad_len);
  print_hex(buf, ret);
}

void test_ec_prime_len_bits()
{
  struct crypto_ec *ec = crypto_ec_init(19);
  printf("bits of prime bits %zu \n", crypto_ec_prime_len_bits((struct crypto_ec *)ec));
}

void test_ec_get_order()
{
  struct crypto_ec *ec              = crypto_ec_init(19);
  const struct crypto_bignum *order = crypto_ec_get_order(ec);
  print_bignum_data(order);
}

void test_ec_prime_len()
{
  struct crypto_ec *ec = crypto_ec_init(19);
  printf("bits of prime %zu \n", crypto_ec_prime_len((struct crypto_ec *)ec));
}

void test_ec_get_prime()
{
  struct crypto_ec *ec              = crypto_ec_init(30);
  const struct crypto_bignum *prime = crypto_ec_get_prime(ec);
  print_bignum_data(prime);
}

void test_ec_get_b()
{
  struct crypto_ec *ec          = crypto_ec_init(19);
  const struct crypto_bignum *b = crypto_ec_get_b(ec);
  print_bignum_data(b);
}

void test_ec_get_a()
{
  struct crypto_ec *ec          = crypto_ec_init(19);
  const struct crypto_bignum *a = crypto_ec_get_a(ec);
  print_bignum_data(a);
}

void test_ec_get_order_len()
{
  struct crypto_ec *ec = crypto_ec_init(19);
  printf("length of order %zu \n", crypto_ec_order_len((struct crypto_ec *)ec));
}

void test_ec_get_point_from_bin()
{
  struct crypto_ec *ec          = crypto_ec_init(19);
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  uint8_t *s = "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c2964fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce335"
               "76b315ececbb6406837bf51f5";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  print_hex(val, length);
  point = crypto_ec_point_from_bin(ec, val);
  printf("X:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)point)->private_X);
  printf("Y:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)point)->private_Y);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)point)->private_Z);
  printf("done\n");
}

void test_ec_point_on_curve()
{
  struct crypto_ec *ec          = crypto_ec_init(21);
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  uint8_t *s = "00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de334"
               "8b3c1856a429bf97e7e31c2e5bd66011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72"
               "995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  // print_hex(val, length);
  point = crypto_ec_point_from_bin(ec, val);
  printf("z value of new point\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)point)->private_Z);
  int ret;
  ret = crypto_ec_point_is_on_curve(ec, point);
  // point is on the curve hence ret value will be 1
  assert(ret == 0);
  if (ret == 1)
    printf("on the curve\n");
  else
    printf("not on the curve\n");
}

void test_ec_point_to_bin()
{
  struct crypto_ec *ec          = crypto_ec_init(21);
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  uint8_t *s = "00c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de334"
               "8b3c1856a429bf97e7e31c2e5bd66011839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72"
               "995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  point        = crypto_ec_point_from_bin(ec, val);
  uint8_t *x   = (uint8_t *)malloc(66);
  uint8_t *y   = (uint8_t *)malloc(66);
  crypto_ec_point_to_bin(ec, point, x, y);
  printf("x:");
  print_hex(x, 66);
  printf("Y:");
  print_hex(y, 66);
}

void test_ec_solve_y()
{
  // change the group id and give x coordinate of a point on the curve for testing with parity of Y
  struct crypto_ec *ec    = crypto_ec_init(20);
  struct crypto_bignum *x = crypto_bignum_init();
  // struct crypto_bignum * expected = crypto_bignum_init();
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  mbedtls_mpi_read_string(
    (mbedtls_mpi *)x,
    16,
    "(aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7");
  // mbedtls_mpi_read_string((mbedtls_mpi*)expected, 16, "BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E3");
  int ret = crypto_ec_point_solve_y_coord(ec, point, x, 1);
  // printf("%d\n",ret);
  uint8_t *x1 = (uint8_t *)malloc(66);
  uint8_t *y1 = (uint8_t *)malloc(66);
  ret         = crypto_ec_point_to_bin(ec, point, x1, y1);
  // printf("%d",ret);
  printf("x:");
  print_hex(x1, 48);
  printf("Y:");
  print_hex(y1, 48);
}

void test_ec_point_mul()
{
  struct crypto_ec *ec        = crypto_ec_init(25);
  struct crypto_ec_point *p1  = crypto_ec_point_init(ec);
  struct crypto_ec_point *res = crypto_ec_point_init(ec);
  struct crypto_bignum *b     = crypto_bignum_init();
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 10, "4");
  uint8_t *s = "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff101207192b95ffc8da78631011ed6b24cdd573f977a11e794811";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  p1           = crypto_ec_point_from_bin(ec, val);

  //case1:
  crypto_ec_point_mul(ec, p1, b, res);
  uint8_t *x = (uint8_t *)malloc(24);
  uint8_t *y = (uint8_t *)malloc(24);
  int r      = crypto_ec_point_to_bin(ec, res, x, y);
  printf("r = %d\n", r); // r =0 means point is a affine point
  printf("x:");
  print_hex(x, 24);
  printf("Y:");
  print_hex(y, 24);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_Z);

  // case 2:
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 10, "0");
  crypto_ec_point_mul(ec, p1, b, res);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_Z);

  // case 3:
  mbedtls_ecp_set_zero((mbedtls_ecp_point *)p1);
  mbedtls_mpi_read_string((mbedtls_mpi *)b, 10, "3");
  crypto_ec_point_mul(ec, p1, b, res);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_Z);
}

void test_ec_point_comp()
{
  struct crypto_ec *ec       = crypto_ec_init(25);
  struct crypto_ec_point *p1 = crypto_ec_point_init(ec);
  struct crypto_ec_point *p2 = crypto_ec_point_init(ec);
  uint8_t *s = "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff101207192b95ffc8da78631011ed6b24cdd573f977a11e794811";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  int ret1, ret2;
  //case 1:
  p1   = crypto_ec_point_from_bin(ec, val);
  p2   = crypto_ec_point_from_bin(ec, val);
  ret1 = crypto_ec_point_cmp(ec, p1, p2);
  if (ret1 == 0)
    printf("same points\n");
  else
    printf("not same\n");
  //case 2:
  p1   = crypto_ec_point_from_bin(ec, val);
  s    = "76e32a2557599e6edcd283201fb2b9aadfd0d359cbb263da782c37e372ba4520aa62e0fed121d49ef3b543660cfd05fd";
  val  = hexStringToArray(s, &length, 0);
  p2   = crypto_ec_point_from_bin(ec, val);
  ret2 = crypto_ec_point_cmp(ec, p1, p2);
  if (ret2 == 0)
    printf("same points\n");
  else
    printf("not same\n");
}

void test_ec_point_add()
{
  struct crypto_ec *ec         = crypto_ec_init(25);
  struct crypto_ec_point *p1   = crypto_ec_point_init(ec);
  struct crypto_ec_point *p2   = crypto_ec_point_init(ec);
  struct crypto_ec_point *res1 = crypto_ec_point_init(ec);
  struct crypto_ec_point *res2 = crypto_ec_point_init(ec);
  uint8_t *s = "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff101207192b95ffc8da78631011ed6b24cdd573f977a11e794811";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  uint8_t *x   = (uint8_t *)malloc(24);
  uint8_t *y   = (uint8_t *)malloc(24);
  p1           = crypto_ec_point_from_bin(ec, val);
  // case 1:
  p2 = crypto_ec_point_from_bin(ec, val);
  crypto_ec_point_add(ec, p1, p2, res1);
  crypto_ec_point_to_bin(ec, res1, x, y);
  printf("x:");
  print_hex(x, 24);
  printf("Y:");
  print_hex(y, 24);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res1)->private_Z);

  // case 2:
  crypto_ec_point_add(ec, p1, res1, res2);
  crypto_ec_point_to_bin(ec, res2, x, y);
  printf("x:");
  print_hex(x, 24);
  printf("Y:");
  print_hex(y, 24);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res2)->private_Z);

  // case 3:
  mbedtls_ecp_set_zero((mbedtls_ecp_point *)res1);
  crypto_ec_point_add(ec, p1, res1, res2);
  crypto_ec_point_to_bin(ec, res2, x, y);
  printf("x:");
  print_hex(x, 24);
  printf("Y:");
  print_hex(y, 24);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res2)->private_Z);

  // case 4:
  mbedtls_ecp_set_zero((mbedtls_ecp_point *)res1);
  mbedtls_ecp_set_zero((mbedtls_ecp_point *)p1);
  crypto_ec_point_add(ec, p1, res1, res2);
  int r = crypto_ec_point_to_bin(ec, res2, x, y); // return -1 if the point is at infinity
  printf("return value %d \n", r);
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res2)->private_Z);
}

void test_ec_point_inv()
{
  struct crypto_ec *ec        = crypto_ec_init(25);
  struct crypto_ec_point *p1  = crypto_ec_point_init(ec);
  struct crypto_ec_point *p2  = crypto_ec_point_init(ec);
  struct crypto_ec_point *res = crypto_ec_point_init(ec);
  uint8_t *s = "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff101207192b95ffc8da78631011ed6b24cdd573f977a11e794811";
  size_t length;
  uint8_t *val = hexStringToArray(s, &length, 0);
  p1           = crypto_ec_point_from_bin(ec, val);
  p2           = crypto_ec_point_from_bin(ec, val);
  crypto_ec_point_invert(ec, p2);

  crypto_ec_point_add(ec, p1, p2, res);
  printf(
    "print coordinates of point at infinity\n"); // point at infinity will have (1,1,0) in Jacobian cordinates system
  printf("X:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_X);
  printf("Y:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_Y);
  printf("Z:\n");
  print_bignum_data((struct crypto_bignum *)&((mbedtls_ecp_point *)res)->private_Z);
}

void test_ec_init()
{
  struct crypto_ec *ec = crypto_ec_init(19);
  printf("%d", ec->grp->id);
}

void test_ec_deinit()
{
  struct crypto_ec *ec = crypto_ec_init(19);
  crypto_ec_deinit(ec);
  printf("%d", ec->grp->id);
}

void test_ec_point_init()
{
  struct crypto_ec *ec          = crypto_ec_init(19);
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  printf("%d\n", ((mbedtls_ecp_point *)point)->private_X.private_s);
  printf("%d\n", ((mbedtls_ecp_point *)point)->private_Y.private_n);
}

void test_ec_point_deinit()
{
  struct crypto_ec *ec          = crypto_ec_init(19);
  struct crypto_ec_point *point = crypto_ec_point_init(ec);
  crypto_ec_point_deinit(point, 1);
  printf("%d\n", ((mbedtls_ecp_point *)point)->private_X.private_s);
  printf("%d\n", ((mbedtls_ecp_point *)point)->private_Y.private_n);
}

int main()
{

  /*hash SHA512*/
  // test_md5();
  /*hash SHA512*/
  // test_sha_1();
  /*hash SHA512*/
  // test_sha_256();
  /*hash SHA512*/
  // test_sha_384();
  /*hash SHA512*/
  // test_sha_512();

  /*hmac SHA512*/
  // test_hmac_md5();
  /*hmac SHA512*/
  // test_hmac_sha1();
  /*hmac SHA512*/
  // test_hmac_sha256();
  /*hmac SHA384*/
  // test_hmac_sha384();
  /*hmac SHA512*/
  // test_hmac_sha512();

  /*random no. generator*/
  // uint8_t random_no[2];
  // crypto_get_random(random_no,2);
  // printf("generated random:\n");
  // print_hex(random_no, 2 );

  /*pbkdf2*/
  // test_pbkdf2();

  /*aes 128 cbc*/
  // test_aes_128_cbc();

  /*DES encryption*/
  // test_des_encryption();

  /*aes encryption multi-part*/
  //    test_multi_part_aes_encryption();

  /*aes decryption multipart*/
  // test_multi_part_aes_decryption();

  /*omac1 aes*/
  // test_omac1_aes();

  /*mbedtls APIs*/
  /*crypto_bignum_init*/
  // test_bignum_init();

  /*crypto_bignum_deinit*/
  // test_bignum_deinit();

  /*crypto_bignum_to_bin*/
  // test_bignum_to_bin();

  /*crypto_bignum_add*/ //c=a+b
  // test_bignum_add();

  /*crypto_bignum_rshift*/
  //    test_bignum_rshift();

  /*crypto_bignum_exptmod*/
  //    test_bignum_expmod();

  /*crypto_bignum_mod*/
  // test_bignum_mod();

  /*crypto_bignum_is_zero*/
  // test_bignum_is_zero();

  /*crypto_bignum_is_one*/
  // test_bignum_is_one();

  /*crypto_bignum_inverse*/
  // test_bignum_inverse();

  /*crypto_bignum_mulmod*/
  // test_bignum_mulmod();

  /*crypto_bignum_init_set*/
  // test_bignum_init_set();

  /*crypto_bignum_cmp*/
  // test_bignum_comp();

  /*crypto_bignum_sub*/
  // test_bignum_sub();

  /*crypto_bignum_init_uint*/
  // test_bignum_init_uint();

  /*crypto_bignum_bits*/
  //    test_bignum_bits();

  /*crypto_bignum_addmod*/
  //    test_bignum_addmod();

  /*crypto_bignum_sqrmod*/
  // test_bignum_sqrmod();

  /*crypto_bignum_legendre*/
  // test_bignum_legendre();

  /*ECC init*/
  // test_ec_init();
  /*crypto_ec_deinit*/
  // test_ec_deinit();
  /*ec point init*/
  // test_ec_point_init();
  /*crypto_ec_point_deinit*/
  // test_ec_point_deinit();
  /*crypto_ec_prime_len_bits*/
  // test_ec_prime_len_bits();
  /*crypto_ec_get_order*/
  // test_ec_get_order();

  /*crypto_ec_prime_len*/
  // test_ec_prime_len();

  /*crypto_ec_get_prime*/
  // test_ec_get_prime();

  /*crypto_ec_get_b*/
  // test_ec_get_b();
  /*crypto_ec_get_a*/
  // test_ec_get_a();
  /*crypto_ec_order_len*/
  // test_ec_get_order_len();

  /*crypto_ec_point_from_bin*/
  // test_ec_get_point_from_bin();

  /*crypto_ec_point_is_on_curve*/
  test_ec_point_on_curve();

  /*crypto_ec_point_to_bin*/
  //    test_ec_point_to_bin();

  /*crypto_ec_point_solve_y_coord*/
  // test_ec_solve_y();

  /*crypto_ec_point_mul*/
  // test_ec_point_mul();

  /*crypto_ec_point_cmp*/
  // test_ec_point_comp();

  /*crypto_ec_point_add*/
  // test_ec_point_add();

  /*crypto_ec_point_invert*/
  // test_ec_point_inv();

  return 0;
}