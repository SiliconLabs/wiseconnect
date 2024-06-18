/***************************************************************************/ /**
 * @file psa_driver_cipher_test_data.h
 * @brief Test data present for PSA Cipher.
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
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

typedef struct {
  char *key;
  uint32_t key_len;
  char *plain_text;
  uint32_t text_len;
  char *iv;
  uint32_t iv_len;
  char *cipher_text;
  uint32_t cipher_len;
  char *add_data;
  uint32_t add_data_len;
} sl_aes_test_vector_t;

#define AES_TEXT_MAX_LEN 512

const sl_aes_test_vector_t aes_cbc_no_padding_test_vector[] = { { "2F423F45 28482B4D 62516553 68566D59",
                                                                  16,
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                                  64,
                                                                  "00010203 04050607 08090A0B 0C0D0E0F",
                                                                  16,
                                                                  "05bc1289 c106d407 eb8d00c3 66ef896e"
                                                                  "e1c95af8 20a1ec99 d15eb30d 8388fcea"
                                                                  "ff395565 5152f403 8fef4b7e 3baac87a"
                                                                  "306db46e 49944d3e 2c82f83f 83076215",
                                                                  64,
                                                                  "",
                                                                  0 },
                                                                // AES-192-CBC Encrypt NIST KAT #4
                                                                { "25a39dbfd8034f71a81f9ceb55026e4037f8f6aa30ab44ce",
                                                                  24,
                                                                  "00000000000000000000000000000000",
                                                                  16,
                                                                  "00000000000000000000000000000000",
                                                                  16,
                                                                  "3608c344868e94555d23a120f8a5502d",
                                                                  16,
                                                                  "",
                                                                  0 },
                                                                // AES-192-CBC Encrypt NIST KAT #7
                                                                { "000000000000000000000000000000000000000000000000",
                                                                  24,
                                                                  "51719783d3185a535bd75adc65071ce1",
                                                                  16,
                                                                  "00000000000000000000000000000000",
                                                                  16,
                                                                  "4f354592ff7c8847d2d0870ca9481b7c",
                                                                  16,
                                                                  "",
                                                                  0 },
                                                                { "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                                                  "1F352C07 3B6108D7 2D9810A3 0914DFF4",
                                                                  32,
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                                  64,
                                                                  "00010203 04050607 08090A0B 0C0D0E0F",
                                                                  16,
                                                                  "F58C4C04 D6E5F1BA 779EABFB 5F7BFBD6"
                                                                  "9CFC4E96 7EDB808D 679F777B C6702C7D"
                                                                  "39F23369 A9D9BACF A530E263 04231461"
                                                                  "B2EB05E2 C39BE9FC DA6C1907 8C6A9D1B",
                                                                  64,
                                                                  "",
                                                                  0 },
                                                                // Long vector used for in/out offset testing
                                                                { "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                                                  "1F352C07 3B6108D7 2D9810A3 0914DFF4",
                                                                  32,
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710"
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710"
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710"
                                                                  "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                                  "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                                  "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                                  "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                                  256,
                                                                  "00010203 04050607 08090A0B 0C0D0E0F",
                                                                  16,
                                                                  "F58C4C04 D6E5F1BA 779EABFB 5F7BFBD6"
                                                                  "9CFC4E96 7EDB808D 679F777B C6702C7D"
                                                                  "39F23369 A9D9BACF A530E263 04231461"
                                                                  "B2EB05E2 C39BE9FC DA6C1907 8C6A9D1B"
                                                                  "B34E62FC 55E1CCF6 674CF57F 3AC8F282"
                                                                  "9E44A898 A93F7359 2513986A 0D5A2A71"
                                                                  "EB440582 FFF1C1A5 F695697F 53D9BAB4"
                                                                  "97DB2586 3AEF3A58 01DBF1E1 D43B6A6B"
                                                                  "B8D11B12 F41ACBC0 D2145F7A EBDEEEE2"
                                                                  "81EBF423 C3423186 88FF6187 9E701EB9"
                                                                  "3792FDD7 FB93DECA A6B2E5D7 89AFE237"
                                                                  "B049F913 1E450CA7 4E778EA5 7AB7D8FB"
                                                                  "DC2E65FE B8C204D4 B15D7AE1 A7856CC8"
                                                                  "F3632F33 835B0CB8 5DD9D04F CE25D05D"
                                                                  "1EB84F5A 16FF9172 D4785A81 A75319DB"
                                                                  "6911190A 8DF739B3 2F0A258A 0A625032",
                                                                  256,
                                                                  "",
                                                                  0 } };

const sl_aes_test_vector_t aes_cbc_pkcs7_test_vector[] = { { "2F423F45 28482B4D 62516553 68566D59",
                                                             16,
                                                             "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                             "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                             "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                             "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                             64,
                                                             "00010203 04050607 08090A0B 0C0D0E0F",
                                                             16,
                                                             "05bc1289 c106d407 eb8d00c3 66ef896e"
                                                             "e1c95af8 20a1ec99 d15eb30d 8388fcea"
                                                             "ff395565 5152f403 8fef4b7e 3baac87a"
                                                             "306db46e 49944d3e 2c82f83f 83076215"
                                                             "4bf0120b cda4e376 208aff09 115af2b0",
                                                             80,
                                                             "",
                                                             0 },
                                                           { "2F423F45 28482B4D 62516553 68566D59",
                                                             16,
                                                             "6BC1BEE2 2E409F96 E93D7E11 739317",
                                                             15,
                                                             "2A2A2A2A 2A2A2A2A 2A2A2A2A 2A2A2A2A",
                                                             16,
                                                             "13387399 be9df963 1f5bd3fa 5c3b9f05",
                                                             16,
                                                             "",
                                                             0 },
                                                           { "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                                             "1F352C07 3B6108D7 2D9810A3 0914DFF4",
                                                             32,
                                                             "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                             "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                             "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                             "F69F2445 DF4F9B17 AD2B417B",
                                                             60,
                                                             "00010203 04050607 08090A0B 0C0D0E0F",
                                                             16,
                                                             "f58c4c04 d6e5f1ba 779eabfb 5f7bfbd6"
                                                             "9cfc4e96 7edb808d 679f777b c6702c7d"
                                                             "39f23369 a9d9bacf a530e263 04231461"
                                                             "3c247a56 237c135b 757c11fd 34d5ef70",
                                                             64,
                                                             "",
                                                             0 } };

const sl_aes_test_vector_t aes_cfb_test_vector[] = {
  { "2F423F45 28482B4D 62516553 68566D59",
    16,
    "6BC1BEE2 2E409F96 E93D7E11 7393172A"
    "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
    "30C81C46 A35CE411 E5FBC119 1A0A52EF"
    "F69F2445 DF4F9B17 AD2B417B E66C3710",
    64,
    "00010203 04050607 08090A0B 0C0D0E0F",
    16,
    "155f1fd4 7c0ae3ab be6037c1 ac8f5adb"
    "17dbba89 1ea6011b f838aada f16a9c25"
    "ddc4bf4f 8b0549d6 334c78cb e23103f6"
    "5aebf309 6aac97af cd4760ba 73765a89",
    64,
    "",
    0 },
  { "2F423F45 28482B4D 62516553 68566D59",
    16,
    "6BC1BEE2 2E409F96 E93D7E11 7393172A"
    "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
    "30C81C46 A35CE411 E5FBC119 1A0A52EF"
    "F69F2445 DF4F9B17 AD2B",
    58,
    "00010203 04050607 08090A0B 0C0D0E0F",
    16,
    "155f1fd4 7c0ae3ab be6037c1 ac8f5adb"
    "17dbba89 1ea6011b f838aada f16a9c25"
    "ddc4bf4f 8b0549d6 334c78cb e23103f6"
    "5aebf309 6aac97af cd47",
    58,
    "",
    0 },
  // AES-192-CFB128 Encrypt NIST KAT #6
  { "c88f5b00a4ef9a6840e2acaf33f00a3bdc4e25895303fa72",
    24,
    "00000000000000000000000000000000",
    16,
    "00000000000000000000000000000000",
    16,
    "a67cf333b314d411d3c0ae6e1cfcd8f5",
    16,
    "",
    0 },
  // AES-192-CFB128 Encrypt NIST KAT #10
  { "000000000000000000000000000000000000000000000000",
    24,
    "00000000000000000000000000000000",
    16,
    "ffffffffffffffe00000000000000000",
    16,
    "f34e4a6324ea4a5c39a661c8fe5ada8f",
    16,
    "",
    0 },
  // AES-192-CFB128 Encrypt NIST KAT #12
  { "000000000000000000000000000000000000000000000000",
    24,
    "00000000000000000000000000000000",
    16,
    "fffffffffffffff80000000000000000",
    16,
    "3a3c15bfc11a9537c130687004e136ee",
    16,
    "",
    0 },
  // AES-256-CFB128 Encrypt NIST KAT #0
  { "0000000000000000000000000000000000000000000000000000000000000000",
    32,
    "00000000000000000000000000000000",
    16,
    "80000000000000000000000000000000",
    16,
    "ddc6bf790c15760d8d9aeb6f9a75fd4e",
    16,
    "",
    0 },
  { "603DEB10 15CA71BE 2B73AEF0 857D7781"
    "1F352C07 3B6108D7 2D9810A3 0914DFF4",
    32,
    "6BC1BEE2 2E409F96 E93D7E11 7393172A"
    "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
    "30C81C46 A35CE411 E5FBC119 1A0A52EF"
    "F69F2445 DF4F9B17 AD2B417B E66C3710",
    64,
    "00010203 04050607 08090A0B 0C0D0E0F",
    16,
    "DC7E84BF DA79164B 7ECD8486 985D3860"
    "39FFED14 3B28B1C8 32113C63 31E5407B"
    "DF101324 15E54B92 A13ED0A8 267AE2F9"
    "75A38574 1AB9CEF8 2031623D 55B1E471",
    64,
    "",
    0 }
};

const sl_aes_test_vector_t aes_ctr_test_vector[] = { { "2F423F45 28482B4D 62516553 68566D59",
                                                       16,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                       64,
                                                       "F0F1F2F3 F4F5F6F7 F8F9FAFB FCFDFEFF",
                                                       16,
                                                       "46457f09 e5bd8051 429970ca 05824cee"
                                                       "0e7df145 ceb36ce1 8504103d 53760a5a"
                                                       "07d202cf 09331880 6b65aca5 b25ccaea"
                                                       "cbc8dc5a 45f51b18 5cf299fd 819a318e",
                                                       64,
                                                       "",
                                                       0 },
#if !defined(SLI_CRYPTOACC_PRESENT_SI91X) //The plain_text length should be a multiple of 16
                                                     { "2F423F45 28482B4D 62516553 68566D59",
                                                       16,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF",
                                                       53,
                                                       "F0F1F2F3 F4F5F6F7 F8F9FAFB FCFDFEFF",
                                                       16,
                                                       "46457f09 e5bd8051 429970ca 05824cee"
                                                       "0e7df145 ceb36ce1 8504103d 53760a5a"
                                                       "07d202cf 09331880 6b65aca5 b25ccaea"
                                                       "cbc8dc5a 45",
                                                       53,
                                                       "",
                                                       0 },
#endif // SLI_CRYPTOACC_PRESENT_SI91X
                                                     { "8E73B0F7 DA0E6452 C810F32B 809079E5 62F8EAD2 522C6B7B",
                                                       24,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                       64,
                                                       "F0F1F2F3 F4F5F6F7 F8F9FAFB FCFDFEFF",
                                                       16,
                                                       "1ABC9324 17521CA2 4F2B0459 FE7E6E0B"
                                                       "090339EC 0AA6FAEF D5CCC2C6 F4CE8E94"
                                                       "1E36B26B D1EBC670 D1BD1D66 5620ABF7"
                                                       "4F78A7F6 D2980958 5A97DAEC 58C6B050",
                                                       64,
                                                       "",
                                                       0 },
                                                     { "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                                       "1F352C07 3B6108D7 2D9810A3 0914DFF4",
                                                       32,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                       64,
                                                       "F0F1F2F3 F4F5F6F7 F8F9FAFB FCFDFEFF",
                                                       16,
                                                       "601EC313 775789A5 B7A7F504 BBF3D228"
                                                       "F443E3CA 4D62B59A CA84E990 CACAF5C5"
                                                       "2B0930DA A23DE94C E87017BA 2D84988D"
                                                       "DFC9C58D B67AADA6 13C2DD08 457941A6",
                                                       64,
                                                       "",
                                                       0 },
#if !defined(SLI_CRYPTOACC_PRESENT_SI91X)              //The plain_text length should be a multiple of 16
                                                     { // AES CTR BUG Regression Test PSEC-4071
                                                       "D24A3D3D DE8C8483 0280CB87 ABAD0BB3",
                                                       16,
                                                       "6BC1BEE2 2E409F96 E93D7E11 739317",
                                                       15,
                                                       "01C65408 9E304AAB D137210C AFB20001",
                                                       16,
                                                       "F5DB715B EF6FF378 BC0AD461 F14AF4",
                                                       15,
                                                       "",
                                                       0 }
#endif // SLI_CRYPTOACC_PRESENT_SI91X
};

const sl_aes_test_vector_t aes_ecb_test_vector[] = { { "2F423F45 28482B4D 62516553 68566D59",
                                                       16,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                       64,
                                                       "00010203 04050607 08090A0B 0C0D0E0F",
                                                       16,
                                                       "50756d6b 159e71be 12748ef0 e3efbb62"
                                                       "ea5573e0 e2c15bc7 2e58ee6a 2d4f875c"
                                                       "61ea6c58 55ff1623 ed3d13fc ae9fe399"
                                                       "956533c8 c96d0b57 67a369d7 ac81df9e",
                                                       64,
                                                       "",
                                                       0 },
                                                     // AES-192-ECB Encrypt NIST KAT #7
                                                     { "d2926527e0aa9f37b45e2ec2ade5853ef807576104c7ace3",
                                                       24,
                                                       "00000000000000000000000000000000",
                                                       16,
                                                       "",
                                                       0,
                                                       "dd619e1cf204446112e0af2b9afa8f8c",
                                                       16,
                                                       "",
                                                       0 },
                                                     // AES-192-ECB Encrypt NIST KAT #11
                                                     { "fffffffffffffffffffffffffffc00000000000000000000",
                                                       24,
                                                       "00000000000000000000000000000000",
                                                       16,
                                                       "",
                                                       0,
                                                       "9d6bdc8f4ce5feb0f3bed2e4b9a9bb0b",
                                                       16,
                                                       "",
                                                       0 },
                                                     { "603DEB10 15CA71BE 2B73AEF0 857D7781"
                                                       "1F352C07 3B6108D7 2D9810A3 0914DFF4",
                                                       32,
                                                       "6BC1BEE2 2E409F96 E93D7E11 7393172A"
                                                       "AE2D8A57 1E03AC9C 9EB76FAC 45AF8E51"
                                                       "30C81C46 A35CE411 E5FBC119 1A0A52EF"
                                                       "F69F2445 DF4F9B17 AD2B417B E66C3710",
                                                       64,
                                                       "00010203 04050607 08090A0B 0C0D0E0F",
                                                       16,
                                                       "F3EED1BD B5D2A03C 064B5A7E 3DB181F8"
                                                       "591CCB10 D410ED26 DC5BA74A 31362870"
                                                       "B6ED21B9 9CA6F4F9 F153E7B1 BEAFED1D"
                                                       "23304B7A 39F9F3FF 067D8D8F 9E24ECC7",
                                                       64,
                                                       "",
                                                       0 } };