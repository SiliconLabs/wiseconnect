/***************************************************************************/ /**
 * @file psa_driver_aead_test_data.h
 * @brief
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

#ifndef PSA_DRIVER_AEAD_TEST_DATA_H
#define PSA_DRIVER_AEAD_TEST_DATA_H

/****************************************************************
* AES-CCM
****************************************************************/
typedef struct {
  char *key;
  uint32_t key_len;
  char *plain_text;
  uint32_t text_len;
  char *nonce;
  uint32_t nonce_len;
  char *cipher_text;
  uint32_t cipher_len;
  char *mac_data;
  uint32_t mac_data_len;
  uint32_t *add_data;
  uint32_t add_data_len;
} sli_aes_test_vector_t;

typedef struct {
  char *key;
  uint32_t key_len;
  char *plain_text;
  uint32_t text_len;
  char *nonce;
  uint32_t nonce_len;
  char *cipher_text;
  uint32_t cipher_len;
  char *mac_data;
  uint32_t mac_data_len;
  char *add_data;
  uint32_t add_data_len;
} sli_ccm_stream_test_vector_t;

#define AES_TEXT_MAX_LEN 512

static const uint32_t add_data_128_short[] = { 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110 };

static const uint32_t add_data_256_short_1[] = { 0x6932f8b4, 0x955da444, 0xc28718f9, 0xb636aca6,
                                                 0xde5eea0e, 0x351c4cf8, 0x66a64681, 0x358387b6 };

static const uint32_t add_data_256_short_2[] = { 0x00148b5f, 0xe8910892, 0x61397605, 0xc8c98381,
                                                 0x1a1c8247, 0xa9f279ae, 0x14f1750d, 0x75e921db };

const sli_ccm_stream_test_vector_t aes_ccm_stream_test_vector[] = {
#if !defined(CRYPTO_PRESENT)
  { "7588da147dd0431bb81795dd377c7b8c181059dc200dab7e",
    24,
    "1adad4f43c59c0e7a7ca1c01970074c2e719f6ff4d37cfd8f59bc0a847c6a556e70c6291e46803261a926c580e0631739072082b60772b645f"
    "f6a995e21139ce9e8f2858b8737e8f939f3c3787b6fa30715b588c9abd17a7f269a0ddf67758f6181a58a64a86f1e7d3384a59bda47757f4c8"
    "13310ffc9e2bbf91dd51c81b78a1",
    128,
    "8f0d34c57b2fcd36",
    8,
    "a07f9b5b29e5daa18f79c006389b6560fc64ebe8ac418a73712d751b6326421905cad596ceab80b2cc12e0dae7737a65e21900477317e9b75f"
    "d87fa932dd0ce81ed58e64eeb60809c8f0bfaa1dbdca58782c386a557fa5cf3997d5b17e6211a27a134be50715bdc88efdb90c141ab18b8ef4"
    "27140d75dca67af5a812e4f64a62",
    128,
    "e51c4a1e8cb069e16c050c2254858a60",
    16,
    "1ea13d99821390c10628e17ccfa9dbae24369f607829e5d23cf24b169736b126",
    32 },
  { "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    24,
    "",
    0,
    "5a8aa485c316e9",
    7,
    "",
    0,
    "17223038fa99d53681ca1beabe78d1b4",
    16,
    "",
    0 },
  { "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    24,
    "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    24,
    "5a8aa485c316e9",
    7,
    "411986d04d6463100bff03f7d0bde7ea2c3488784378138c",
    24,
    "ddc93a54",
    4,
    "",
    0 },
  { "4bb3c4a4f893ad8c9bdc833c325d62b3d3ad1bccf9282a65",
    24,
    "",
    0,
    "5a8aa485c316e9",
    7,
    "",
    0,
    "17223038fa99d53681ca1beabe78d1b4",
    16,
    "",
    0 },
  { "19ebfde2d5468ba0a3031bde629b11fd4094afcb205393fa",
    24,
    "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    24,
    "5a8aa485c316e9",
    7,
    "411986d04d6463100bff03f7d0bde7ea2c3488784378138c",
    24,
    "ddc93a54",
    4,
    "",
    0 },
#endif //CRYPTO_PRESENT
  { "a7aa635ea51b0bb20a092bd5573e728c",
    16,
    "a265480ca88d5f536db0dc6abc40faf0d05be7a966977768",
    24,
    "5a8aa485c316e9",
    7,
    "b351ab96b2e45515254558d5212673ee6c776d42dbca3b51",
    24,
    "2cf3a20b7fd7c49e6e79bef475c2906f",
    16,
    "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22ec22b1a268f88e2c",
    32 },
  { "4bb3c4a4f893ad8c9bdc833c325d62b3",
    16,
    "",
    0,
    "5a8aa485c316e9",
    7,
    "",
    0,
    "75d582db43ce9b13ab4b6f7f14341330",
    16,
    "",
    0 },
  { "19ebfde2d5468ba0a3031bde629b11fd",
    16,
    "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    24,
    "5a8aa485c316e9",
    7,
    "a90e8ea44085ced791b2fdb7fd44b5cf0bd7d27718029bb7",
    24,
    "03e1fa6b",
    4,
    "",
    0 },
  { "4ae701103c63deca5b5a3939d7d05992", 16, "", 0, "5a8aa485c316e9", 7, "", 0, "02209f55", 4, "", 0 },
  { "4bb3c4a4f893ad8c9bdc833c325d62b3",
    16,
    "",
    0,
    "5a8aa485c316e9",
    7,
    "",
    0,
    "75d582db43ce9b13ab4b6f7f14341330",
    16,
    "",
    0 },
  { "8117bc99951f24b623cdfff4a0650275",
    16,
    "fefcf4317ca92bd2e4592b601a96c1ec781a589850aa64fae98d41025eb8152446e5684c5cdb7233f884c392685a2d0bbc26e257b41df2ab10"
    "80e067a0be4a85b00ffeb7a86b0dfdf0134d42eec4c60d8181825de4b3a60907f64a38e287ba965005274e4e694b96a18c64d91a5bd8f5d6ed"
    "1c899876e29144b43fc0e2b354cf",
    128,
    "2c2c88c72d73a03fea69",
    10,
    "e5b6aee9023fe14fc3206299f9ddaa998c32f9b3583e708ea8aa22ab112c4dd6e9cc8125fe9132a4b0bffc1a193110a2b7531c3e56419b1dc5"
    "db3b6f0e9870ecbc4ac707758c04e921cd27638c7088950d4f56180c5943a8924f9109c79de3697ccff059d159ef337b49bd5e9d652e0e7bc9"
    "39bba4ab00c674a294a545b1acad",
    128,
    "8c5896c726c56812f2ea53ebe4de4006",
    16,
    "d94be0fd531651dc42be83a5cc6768ea73fc9de744bfdf36957c249e0c8c3871",
    32 },
  { "8ddf78b49ce0225856f48986c66c28b9ad8b19dd018e997165dca9cf55cbf4e1",
    32,
    "f787a8a75928c52b8c7482fab36b0901e35c787a2c63b6efcc9f8e0742448b3076a7b9474ae26ca08cb3ca69704fd743016b5d46e6230fafdd"
    "007f6ae5861b5f230ed75d5ef05004c57f617445caffd50b7a83c18063854759898a670befcab91b2f7860aea9970059dcfed14479a0d99f04"
    "33bb6a61d319e1fc10a526bdc7f4",
    128,
    "967cecf2ae703fccdcfdf3",
    11,
    "b0e817d486a46b8ad7f869a1e6a226e1a3ffbfe0496e9581b7497f5aab4d722957b0b2309d3f495f05516639af7e40b85e3ca751ec6ec3dd74"
    "80f5885a6647ba4617f169e89affdf1c126b26fdc1bcba9261d3ba7bb03eea5a09a26eedc4bd5b225959b772d06da4c6ab48db4bf858e013ac"
    "0257b6fd8f434e86b45c212bc1bb",
    128,
    "fc76fcdff4627af31fed79b577a92bf5",
    16,
    "8d5a5a629be6d0893d2f8e55008c7432f73efc11ae2601bdab14ee3c15cb2b58",
    32 },
  { "2cb6dcb0ba6e43fd4fa44134a7c78ac5ec30df58c306e17b443dd2a68a3dab72",
    32,
    "cc68688663391e25b5bd3de71d012b905049125e9e7b9947c900b55226cb7e0f2bb828905e9ac2f33152295cb117b9e45d860deeba292d9657"
    "2c34298df4fa0f4e7300b76924a975b378e11fe8fba34406f805a6cc0ea2783f768151437ffde7f4df5b9f9eae9aa511dab5e5905f0286d9a5"
    "712986aefcde04021828c1ad212a",
    128,
    "ba09d6ff2f0a74552fcb134abd",
    13,
    "10702f83ff85f95436cd997f8a98a524116f5f6c02ef301f7cdcca993137da2c2e74114209792bf3eccc6e3e55042f7eb23907bf8ab01bacf7"
    "4df14a90a9660af9e8bd7ed4224cd171e9734fb1dc1444829a249368cfd8eacb16b40fde22a2cdbfbf1a7e3a82dfebc20f74bf4d62e3e94f97"
    "257b386e781cff83b9f4ffa3eedc",
    128,
    "47bbd18300eb2d303c9e624f55dc3f94",
    16,
    "0f35088d78cb72faae8544b657d7dd8a9806b5e726ce2e81ff0a2be755e854fdc007ec27c717c217ddbb96edc242d631606e7e67d30eee933e"
    "155e00799a08a7",
    64 },
  { "4189351b5caea375a0299e81c621bf43",
    16,
    "4535d12b4377928a7c0a61c9f825a48671ea05910748c8ef",
    24,
    "48c0906930561e0ab0ef4cd972",
    13,
    "26c56961c035a7e452cce61bc6ee220d77b3f94d18fd10b6",
    24,
    "d80e8bf80f4a46cab06d4313f0db9be9",
    16,
    "40a27c1d1e23ea3dbe8056b2774861a4a201cce49f19997d19206d8c8a343951",
    32 },
  { "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    16,
    "08090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20",
    25,
    "00000005040302a0a1a2a3a4a5",
    13,
    "51b1e5f44a197d1da46b0f8e2d282ae871e838bb64da859657",
    25,
    "4adaa76fbd9fb0c5",
    8,
    "0001020304050607",
    8 },
  { "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf",
    16,
    "",
    0,
    "acde4800000000010000000502",
    13,
    "",
    0,
    "223bc1ec841ab553",
    8,
    "08d0842143010000000048deac020500000055cf000051525354",
    26 }
};

// Python generated test-vectors
const sli_aes_test_vector_t aes_ccm_test_vector[] = {
// CCM implementation on Series-1 doesn't support add_len > 0xFF00
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  {
    "da68abeca171e04f1044d081b198c86cbf3da803b282c3c0570b9a0fcbcb77c9",
    32,
    "058d34cdec07c8018185205b88ad4898",
    16,
    "eb887a4c35a3ad6d",
    8,
    "beb629403d8f26bd567dc0c3ca1d6122",
    16,
    "a395857c97e1b26c31dae7209a8aadd6",
    16,
    (uint32_t *)add_data_256_long,
    100000,
  },
  {
    "7af62e6bf700a049807cecc8e6a06010",
    16,
    "84f396d6995de4ec450fc42c83fcbb3b",
    16,
    "9ce7e8f73e2cbc93",
    8,
    "60928960e1f33a0ff4c90fbc93a2b2c3",
    16,
    "d6350fbe8ab134cbc1e5e3ded414b920",
    16,
    (uint32_t *)add_data_128_long,
    90000,
  },
  {
    "8c808d6927504ebdc2c16b9326f7a0cb24901aaf6b42cade",
    24,
    "ba813d2802d4f67444b14b41ce6a1528",
    16,
    "3d50b54fe9f24c32",
    8,
    "54a63b5f69c9e23993850060ee409042",
    16,
    "f67b255c9215d2d36e94ded5ccfa844e",
    16,
    (uint32_t *)add_data_192,
    110000,
  },
#endif
  // NIST SP800-38C test-vectors
  { "4bb3c4a4f893ad8c9bdc833c325d62b3",
    16,
    "",
    0,
    "5a8aa485c316e9",
    7,
    "",
    0,
    "75d582db43ce9b13ab4b6f7f14341330",
    16,
    NULL,
    0 },
  { "19ebfde2d5468ba0a3031bde629b11fd",
    16,
    "3796cf51b8726652a4204733b8fbb047cf00fb91a9837e22",
    24,
    "5a8aa485c316e9",
    7,
    "a90e8ea44085ced791b2fdb7fd44b5cf0bd7d27718029bb7",
    24,
    "03e1fa6b",
    4,
    NULL,
    0 },
  {
    "404142434445464748494a4b4c4d4e4f", // key
    16,
    "20212223", // plaintext
    4,
    "10111213141516", // nonce
    7,
    "7162015b", // ciphertext
    4,
    "4dac255d", // mac data
    4,
    (uint32_t *)add_data_128_short, // add data
    8,
  },
  {
    "404142434445464748494a4b4c4d4e4f", // key
    16,
    "202122232425262728292a2b2c2d2e2f", // plaintext
    16,
    "1011121314151617", // nonce
    8,
    "d2a1f0e051ea5f62081a7792073d593d", // ciphertext
    16,
    "1fc64fbfaccd", // mac data
    6,
    (uint32_t *)add_data_128_short, // add data
    16,
  },
  {
    "404142434445464748494a4b4c4d4e4f", // key
    16,
    "202122232425262728292a2b2c2d2e2f3031323334353637", // plaintext
    24,
    "101112131415161718191a1b", // nonce
    12,
    "e3b201a9f5b71a7a9b1ceaeccd97e70b6176aad9a4428aa5484392fbc1b09951", // ciphertext
    24,
    "484392fbc1b09951", // mac data
    8,
    (uint32_t *)add_data_128_short, // add data
    20,
  },
  {
    "9cdebaeee8690b68751070691f49593668a6de12d3a948b38ddbd3f75218b2d4", // key
    32,
    "3cbb08f133270e4454bcaaa0f20f6d63c38b6572e766", // plaintext
    22,
    "af1a97d43151f5ea9c48ad36a3", // nonce
    13,
    "3966930a2ae8fdd8f40e7007f3fde0bd6eb48a46e6d2", // ciphertext
    22,
    "7c0c1ba4bfd2bd215b0cd921f06a8f3b", // mac data
    16,
    (uint32_t *)add_data_128_short, // add data
    20,
  },
  // NIST CCM VPT256 Count=0
  {
    "c6c14c655e52c8a4c7e8d54e974d698e1f21ee3ba717a0adfa6136d02668c476", // key
    32,
    "", // plaintext
    0,
    "291e91b19de518cd7806de44f6", // nonce
    13,
    "", // ciphertext
    0,
    "ca482c674b599046cc7d7ee0d00eec1e", // mac data
    16,
    (uint32_t *)add_data_256_short_1, // add data
    32,
  },
  // Python-generated vector
  {
    "c6c14c655e52c8a4c7e8d54e974d698e1f21ee3ba717a0adfa6136d02668c476", // key
    32,
    "bd6b40c87b9ca2eaae7bdc95c11636ceee11eeeb6fce39ea5f596ebb7d7a866d"
    "56492360945ca3b61de9701c6098e88540d2ecdf6867e6e12273e8d29f866e2e"
    "e6c0f681cc10b6e252827a76b51fd27d69cd6f399ff771e74a8e2736b3f4cdd0"
    "ede6e4f82981401efc5e9ff8656f5f45143bf4b284b90990127fcee0dc024eb3"
    "7ff7c5dd733e659e234ad2150421cc2fbaa24fccefddb50442713b7ce7e0a1ae"
    "41f2c10205c2ebeef1348d8978d6d5412b5bebebd7b3612f52449e30fc736706"
    "d8798640cecd5ebdcba5a26adc6aef3aba54a6499730933c9fc0abe800e9e1ee"
    "bbece1ecde230b1ca286615d81fb21a6e7f048f8363991194c5bd3f7bbb4d68d", // plaintext
    256,
    "291e91b19de518cd7806de44f6", // nonce
    13,
    "0798ff78f228e51df02e96976d38fce65d80d027a0dcabc603326b82eb2d3dee"
    "16083fae637ea9bc78207708cd47facd4e7f3814c5bf5ece9f1e498f465abbf4"
    "03c5823903e32774af9c83764daf08379e7c0a0b89fc92f42ca21827eb6173c8"
    "c77327b0a5a6763ea4d11632c67089698ec1cc808ccfec790b1564a381a101b9"
    "390150e20079d9872784a3e8c99bea059d2b1a9d53a090cf27feb339662a7e8d"
    "fb264f4d9d4bce6abb0408b1026786b627f626c09c7b10d8cf7725dc2400438e"
    "19e8df285919475e67234b2caea49c162d44c5d230fe1c05f4938988304ac621"
    "4659650780120a94bb585928088c0a8e8c259ffbda852fc9d3d517afaf3e06f2", // ciphertext
    256,
    "fab7113e11d841ee6c662fa7e367706a", // mac data
    16,
    (uint32_t *)add_data_256_short_2, // add data
    32,
  }
};

/****************************************************************
* AES-GCM
****************************************************************/

typedef enum { SLI_CIPHER_ENCRYPT, SLI_CIPHER_DECRYPT } sli_cipher_operation_t;

typedef struct {
  size_t key_len;
  size_t iv_len;
  size_t in_len;
  size_t aad_len;
  size_t tag_len;
  const char *key;
  const char *iv;
  const char *in; // PT for encrypt, CT for decrypt
  const char *aad;
  const char *out; // CT for encrypt, PT for decrypt
  const char *tag;
} gcm_block_vector_t;

typedef struct {
  size_t key_size;
  sli_cipher_operation_t mode;
  const char *key;
  const char *in; // PT for encrypt, CT for decrypt
  const char *iv;
  const char *aad;
  size_t tag_len;
  const char *tag;
  const char *out; // CT for encrypt, PT for decrypt
} gcm_multipart_vector_t;

/*
 * # CAVS 14.0
 * # GCM Encrypt with keysize 128 test information
 * # Generated on Fri Aug 31 11:23:06 2012
 */

static const gcm_block_vector_t gcm_enc_128_vec[] = {
  { 128 / 8,
    96 / 8,
    104 / 8,
    (256 + 128) / 8,
    32 / 8,
    "aa6e9a7cf708f19de082eebc28d63343",
    "539d2492b706cdf5e03b4468",
    "c5838fe240d4f90276fbdb65ad",
    "b44f259fa13d100a2caa43858e7bf3811dbb85002d613ce4f7baa3f73c72dad0"
    "0ac5e84023c1af776b6920161aaee8ee",
    "f83227f67ec8b2bf7acd6d3afd",
    "f75bfed4" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "11754cd72aec309bf52f7687212e8957",
    "3c819d9a9bed087615030b65",
    "",
    "",
    "",
    "250327c674aaf477aef2675748cf6971" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    128 / 8,
    128 / 8,
    "a82bb1edc7c01a3689006f34bfed783e",
    "963836b67b188becf9ba1411",
    "",
    "9d115bb9bbd119fb777b6316065a9ac8",
    "",
    "c491889fa3eca4544ba0d51b8e0f3837" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    128 / 8,
    "94fd0269a0ce813133626f93c4af7e6f",
    "11fc3928028dfa34db06a1bc",
    "a1aefec976cd87cf8a4c21bbe902f7b4",
    "",
    "b1baf8c58cdec88238b1b0ab0b40337d",
    "882f865df7da529f768d4944e8387f69" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    128 / 8,
    "9db91a40020cdb07f88769309a6ac40b",
    "f89e1b7e598cc2535a5c8659",
    "f4a5003db4a4ebbc2fdb8c6756830391",
    "70910598e7abd4f0503ecd9e21bdafb5",
    "40d7fc4ccc8147581f40655a07f23ee9",
    "243331b48404859c66af4d7b2ee44109" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    128 / 8,
    "d4a22488f8dd1d5c6c19a7d6ca17964c",
    "f3d5837f22ac1a0425e0d1d5",
    "7b43016a16896497fb457be6d2a54122",
    "f1c5d424b83f96c6ad8cb28ca0d20e475e023b5a",
    "c2bd67eef5e95cac27e3b06e3031d0a8",
    "f23eacf9d1cdf8737726c58648826e9c" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    128 / 8,
    "89850dd398e1f1e28443a33d40162664",
    "e462c58482fe8264aeeb7231",
    "2805cdefb3ef6cc35cd1f169f98da81a",
    "d74e99d1bdaa712864eec422ac507bddbe2b0d4633cd3dff29ce5059b49fe868526c59a2a3a604457bc2afea866e7606",
    "ba80e244b7fc9025cd031d0f63677e06",
    "d84a8c3eac57d1bb0e890a8f461d1065" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 128 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    120 / 8,
    "a3efc33605d227cd35ae04fe4378f852",
    "5ae351cfcac892f5c7bfd1f2",
    "",
    "",
    "",
    "e07b4152a7cdd6ae4fa57a2d1df4d3" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    96 / 8,
    "562ae8aadb8d23e0f271a99a7d1bd4d1",
    "f7a5e2399413b89b6ad31aff",
    "bbdc3504d803682aa08a773cde5f231a",
    "2b9680b886b3efb7c6354b38c63b5373",
    "e2b7e5ed5ff27fc8664148f5a628a46d",
    "cbf2015184fffb82f2651c36" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    64 / 8,
    "af2904e234458af8ce0d616866c981fc",
    "ef6381fdeb7877845f46edcd",
    "13836338abcfc03b89dd93f1dd691b01",
    "41946f4a8304875ab3db0dec08d6c990",
    "b13b49e06b9e615a86d4c17ac10da212",
    "ac8af4dc584da9a6" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    96 / 8,
    "90bf5dc928e91cc2f05f8c55dc1ffbc0",
    "db658c4362e7d6a03faa8492",
    "99eac872b13b623f255bcdc0eeb1c529",
    "ee192a6b71e57f3f2b27efc9b1cb385d6c1be0c4",
    "93d95389595d76131b8b763e50a5fe79",
    "0c37dd797b78bdecceb9c2a7" },
  { 0 }
};

/*
 *  CAVS 14.0
 * # GCM Decrypt with keysize 128 test information
 * # Generated on Fri Aug 31 11:28:04 2012
 */

static const gcm_block_vector_t gcm_dec_128_vec[] = {
  { 128 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "cf063a34d4a9a76c2c86787d3f96db71",
    "113b9785971864c83b01c787",
    "",
    "",
    "",
    "72ac8493e3a5228b5d130a69d2510e42" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    128 / 8,
    128 / 8,
    "2370e320d4344208e0ff5683f243b213",
    "04dbb82f044d30831c441228",
    "",
    "d43a8e5089eea0d026c03a85178b27da",
    "",
    "2a049c049d25aa95969b451d93c31c6e" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    128 / 8,
    "5211242698bed4774a090620a6ca56f3",
    "b1e1349120b6e832ef976f5d",
    "",
    "b6d729aab8e6416d7002b9faa794c410d8d2f193",
    "",
    "0f247e7f9c2505de374006738018493b" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    128 / 8,
    "762da58bb000f5d499818bc859989a30",
    "584c291ff1aa388a5112521e",
    "",
    "91f92e8bbda7b5ec967ade766f4f26e9189eaafad416f37b4891d3e37d70cb9a267aa843dd202858ade020261223dfce",
    "",
    "9ac7eb2d762facae06086c957959880e" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    128 / 8,
    "e98b72a9881a84ca6b76e0f43e68647a",
    "8b23299fde174053f3d652ba",
    "5a3c1cf1985dbb8bed818036fdd5ab42",
    "",
    "28286a321293253c3e0aa2704a278032",
    "23c7ab0f952b7091cd324835043b5eb5" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    128 / 8,
    "816e39070410cf2184904da03ea5075a",
    "32c367a3362613b27fc3e67e",
    "552ebe012e7bcf90fcef712f8344e8f1",
    "f2a30728ed874ee02983c294435d3c16",
    "ecafe96c67a1646744f1c891f5e69427",
    "ecaae9fc68276a45ab0ca3cb9dd9539f" },
  { 128 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    128 / 8,
    "abbc49ee0bbe3d81afc2b6b84f70b748",
    "f11db9f7b99a59ed59ade66f",
    "ce2d76f834942c022044eebc91b461c0",
    "d533cf7644a48da46fcdec47ae5c77b9b52db775d6c886896e4f4e00c51affd59499a0e572f324989df511c4ea5f93cd",
    "5135ba1354cbb80478ecaf3db38a443f",
    "62df4b04f219554cd3e69d3c870032d2" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 128 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    120 / 8,
    "eac258e99c55e6ae8ef1da26640613d7",
    "4e8df20faaf2c8eebe922902",
    "",
    "",
    "",
    "e39aeaebe86aa309a4d062d6274339" },
  { 128 / 8,
    96 / 8,
    0 / 8,
    128 / 8,
    120 / 8,
    "6dfa1a07c14f978020ace450ad663d18",
    "34edfa462a14c6969a680ec1",
    "",
    "2a35c7f5f8578e919a581c60500c04f6",
    "",
    "751f3098d59cf4ea1d2fb0853bde1c" },

  { 128 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    104 / 8,
    "a011a0343271a46294dff492e9fbbc95",
    "b10f521d7f1326c260dca5ab",
    "",
    "50acf3bbbae829404c27124aae6d79d7e15e7445",
    "",
    "238b5908fa6f91c435d2efa0aa" },
  { 0 }
};

#if !defined(CRYPTO_PRESENT)
/*
 * # CAVS 14.0
 * # GCM Encrypt with keysize 192 test information
 * # Generated on Fri Aug 31 11:24:46 2012
 */

static const gcm_block_vector_t gcm_enc_192_vec[] = {
  { 192 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "aa740abfadcda779220d3b406c5d7ec09a77fe9d94104539",
    "ab2265b4c168955561f04315",
    "",
    "",
    "",
    "f149e2b5f0adaa9842ca5f45b768a8fc" },
  { 192 / 8,
    96 / 8,
    0 / 8,
    128 / 8,
    128 / 8,
    "41c5da8667ef725220ffe39ae0ac590ac9fca729ab60ada0",
    "05ad13a5e2c2ab667e1a6fbc",
    "",
    "8b5c124bef6e2f0fe4d8c95cd5fa4cf1",
    "",
    "204bdb1bd62154bf08922aaa54eed705" },
  { 192 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    128 / 8,
    "093ef7551ebbff8eb0c0a8a4a62b198f0c2e838de10eeeee",
    "e656e93930ed5210ba3f0322",
    "",
    "3da22dacfd11b21b0a713157f60aec0cd22f1add",
    "",
    "1b2d2764573e20ae640bf29d48e5fe05" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    128 / 8,
    "fbc0b4c56a714c83217b2d1bcadd2ed2e9efb0dcac6cc19f",
    "5f4b43e811da9c470d6a9b01",
    "d2ae38c4375954835d75b8e4c2f9bbb4",
    "",
    "69482957e6be5c54882d00314e0259cf",
    "191e9f29bef63a26860c1e020a21137e" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 192 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    120 / 8,
    "31389612d244c9792a510eca3f9c94f9f48c97ed67ae965a",
    "df6b54ec8b58114df5b09279",
    "",
    "0863bec42ee93385efbec665adfc46dafcd793f29e859e3b531c15b168f1888dd13e905cd7d5bc03f9f1f6495717df62",
    "",
    "77e5682a49243d5b9016eb1adafa2d" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    64 / 8,
    "c50ac59e50556e47b834380018c0dc0380af9df3bf6714e6",
    "f303bf4b6cfbba7104cd9436",
    "d3f3f57033df30c22860231334b099cb",
    "",
    "2269c72d77f2b6f9d57da1820ec5a5d3",
    "d62d4491e3e4e9e7" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    112 / 8,
    "8ef391e4b7a2fe05b959be27823357080f963ed2f64b9e59",
    "0080052a2a5bb0e95222a419",
    "e7fb0631eebf9bdba87045b33650c4ce",
    "290322092d57479e20f6281e331d95a9",
    "88d674044031414af7ba9da8b89dd68e",
    "69897d99d8e1706f38c613896c18" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    32 / 8,
    "1cb5a0db778d3eb430b2816ceef9e455f519a8977b074183",
    "c1df5e9e2e3165c54242a306",
    "636871d4c0aae3da7b55abd8b5f21297",
    "7134e5ddc396c2a8a7da23906c8f7b40",
    "14eb02562aa1d963d0033626cdc8a5c8",
    "972f4bdf" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    104 / 8,
    "45148f42669f8ab8fad689d9b9180e39d7ea8fc95696297e",
    "5afcb134acc78b4eb9d11e79",
    "3d952be11deb421b56e0ce9d7ce99553",
    "aec409e5fd82e50b824ebc1f45e75188d80615c6",
    "077c0d53869869e191df116fd7baa8a2",
    "93d2b577a29b0953c91b5d3b9d" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    120 / 8,
    "5255428457fe75e64447971ec5af0d13c5b60a07ee2d07b0",
    "40cb6ebafc202f82223db097",
    "fdd8a462c86d4365c8bfee0e25fc8a62",
    "b2da2bd05ab1f3e39613efc8d80c5d0f240ee08f6abad5791649e9c1d0f48fa3dc59c1e535d1db1a4d3fa2263f5a1117",
    "9ca4a6d08267038f6f7999c84105bb5e",
    "af8f7b3b9310ec688e033088a03482" },
  { 0 }
};

/*
 *  CAVS 14.0
 * # GCM Decrypt with keysize 192 test information
 * # Generated on Fri Aug 31 11:29:44 2012
 */

static const gcm_block_vector_t gcm_dec_192_vec[] = {
  { 192 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "0e5d6e68f82f32bea3f0b69498c1a31ef6d955cd3d27a2a8",
    "caf72ee1e62e1001e8cfbc63",
    "",
    "",
    "",
    "db1a74ffb5f7de26f5742e0942b1b9cb" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    128 / 8,
    "48a67f942305b2cb104c5764a0f1657bd223961b2f117b32",
    "2b0c587a86f9283efe66359f",
    "2237e94b9b78b70359864bae60ecece2",
    "776488814300e9627987d9c5d3274298709ac81c",
    "090bd2109185855ae1f5eed7c1f03747",
    "d1cea1b00693074f541da9bdf6278430" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 192 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    112 / 8,
    "e79fb7defce4f650402e6b521170686d3eb2a0b9514f3a64",
    "40e0d2d836c0519e7042419b",
    "",
    "41c5b5d971c0723bc1b63a259fe7e06c2961de1241bc34c13965f43636e4da3da8c75ed5956abe3a42f3039af005925a",
    "",
    "434ff68f2436f48418fd69f52158" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    120 / 8,
    "e41d1f533d5b342ffe434b94b1372683bfd5d9d8cb79f9ee",
    "5fe11a596dfcd3a305c1d711",
    "1847f64fff986476d1d2f758692f856d",
    "",
    "b03c2c20f758a93a8d1220232ad87098",
    "a4a0ff98c0c1101694c84fd86680c9" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    120 / 8,
    "8e7da473c057a2a4669a0d22bf9b7c9913fba48930ca0c9b",
    "f9ff8ef80d76c50f9ca0e9ff",
    "5deb093b6e7c766a64bb9d5170af1ff8",
    "f141bae18a1b54f065554fd34aa02c91c90f505c",
    "b22b2dcdcc18adc30d16297b84b459d8",
    "bf130b64eebdce06a9bdb2cf1da15a" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    96 / 8,
    "01bf150add51bb11623e3bfbebd62a7ea81c5b192b8eb6de",
    "dfacbc6791b785d324c646b7",
    "77579db3c6da769e17731faac4732d7c",
    "e35412a625324257bef35399a7eacca34fec2d2d24166e6bb3e94d96f5c57599ded45e2a74503f07116caa1692398a07",
    "7e5fd8b595ddc4753676107951d900e2",
    "ce65d960a49f94f6b583e54a" },
  { 192 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    64 / 8,
    "551266c4ed166fe1c43761927801ed50cb9c0b3864fc97df",
    "e711afbeccd567f866340abb",
    "2b54cc27f6ee71882e8b1ead207d2b04",
    "562d1697237ebc563941076d459727dfa094eb9ac00d30ed5836825d163dd27517c7660a01056b2d868c7fc5d0343830",
    "37245449db8f72b1ecdb420f629d3d80",
    "2d262e87eac97b58" },
  { 192 / 8,
    96 / 8,
    104 / 8,
    0 / 8,
    120 / 8,
    "78fa4a2a5b5b1b1d9580ea527f2e1653e9336e15cc5462f5",
    "98b774f7110e0bea624b487f",
    "a642aabed8b99e15e297ee705a",
    "",
    "496909523f574b205d757659c5",
    "40c3e2e506cb889727b327b7e044a8" },
  { 192 / 8,
    96 / 8,
    104 / 8,
    128 / 8,
    120 / 8,
    "61f4c2e55d729c4657e503dfe2b604e2853675dbdeb0982a",
    "0c4d5548aa2d8d54964e1e63",
    "0d4eacc3db304f46cb7a9eba6e",
    "5affdf8886dabb14790aff3dbfcbdd80",
    "b6e056de521a27266dffbc0d96",
    "c105bf86d9dc0639b7cebbd5260f47" },
  { 192 / 8,
    96 / 8,
    104 / 8,
    160 / 8,
    104 / 8,
    "73245c4f115a74fe71d6fefb9094c57c75f28033a3c7372b",
    "536a82485999b93b0bb7ef24",
    "29333e87bfe65d0e37da2936f6",
    "64dcad870a42eeec0730fd7a7e4154638a85d739",
    "f6d56f8c86f27d957fa63aea22",
    "95824d4e3f37fab3b8e2b868f6" },
  { 192 / 8,
    96 / 8,
    104 / 8,
    384 / 8,
    32 / 8,
    "9002e74638e09dd1f091439518e1460cdd5905bd9e1a37ae",
    "76c81a95d24be5c8bac63b50",
    "31bf37acbc53ca3fdbc9e5eaae",
    "aa3ae4531aaac8f3eb07f748712c55a680bc8df5cf845edc66d09049500b41688b8023f5746879b45bdd586af29c4ede",
    "bd94b34511bc65ae47684805cb",
    "bbb85a7f" },
  { 0 }
};
#endif // !defined(CRYPTO_PRESENT)

/*
 * # CAVS 14.0
 * # GCM Encrypt with keysize 256 test information
 * # Generated on Fri Aug 31 11:26:26 2012
 */

static const gcm_block_vector_t gcm_enc_256_vec[] = {
  { 256 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "b52c505a37d78eda5dd34f20c22540ea1b58963cf8e5bf8ffa85f9f2492505b4",
    "516c33929df5a3284ff463d7",
    "",
    "",
    "",
    "bdc1ac884d332457a1d2664f168c76f0" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    128 / 8,
    "886cff5f3e6b8d0e1ad0a38fcdb26de97e8acbe79f6bed66959a598fa5047d65",
    "3a8efa1cd74bbab5448f9945",
    "",
    "519fee519d25c7a304d6c6aa1897ee1eb8c59655",
    "",
    "f6d47505ec96c98a42dc3ae719877b87" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    128 / 8,
    "92e11dcdaa866f5ce790fd24501f92509aacf4cb8b1339d50c9c1240935dd08b",
    "ac93a1a6145299bde902f21a",
    "2d71bcfa914e4ac045b2aa60955fad24",
    "1e0889016f67601c8ebea4943bc23ad6",
    "8995ae2e6df3dbf96fac7b7137bae67f",
    "eca5aa77d51d4a0a14d9c51e1da474ab" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 256 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    96 / 8,
    "98ebf7a58db8b8371d9069171190063cc1fdc1927e49a3385f890d41a838619c",
    "3e6db953bd4e641de644e50a",
    "",
    "",
    "",
    "2fb9c3e41fff24ef07437c47" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    104 / 8,
    "a7c928738b89c3258b910ac31bc465338b2e133b143fd52d9c9859eb1d01f2a0",
    "a483a7e94fbb2d694d3c4a8d",
    "",
    "bdb613cd3c2f0edd37b3ed43041bacb949ee51fa",
    "",
    "5233f95bdcf5d666fb957acdcb" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    120 / 8,
    "16a5b58a1dbb273a8fc6a4af722d46dbb898dd86ab128cb93d8388a8647a80a3",
    "59e0c40d6675923cf5e004d5",
    "",
    "5b4b4ffc9c66bd394abeed3f03b695b949b3b69a42198cc3bfad971174915df913b967ccf36ee1f001f54efbcd117b68",
    "",
    "d57e27914ecb4a764359d3c0f8d4d6" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    32 / 8,
    "5dd13092dd695b90ab835ed6343031c4cdb710d32f4d3804d72b46d921fcfa18",
    "1de4bd816c8ec6bffc1e6453",
    "",
    "1b63d6278702abacf8b6c2faf542a808659fd5da03cdc1061a8593ea8ce9fc8ff54ffef6ebf3e15f7a832b4ae750a6ce",
    "",
    "72901467" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    120 / 8,
    "ef9f9284cf599eac3b119905a7d18851e7e374cf63aea04358586b0f757670f8",
    "b6ac8e4963f49207ffd6374c",
    "722ee47da4b77424733546c2d400c4e5",
    "",
    "1224dfefb72a20d49e09256908874979",
    "882eafea22adf8dbed06a2265f907b" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    96 / 8,
    "b33b0e4c5b9f7ef77cec1a29ed5844bda3853238bdf7766e7645029931f169f0",
    "f226d65e8654fdf5193ed721",
    "bcf48ddcfe9d011a1003973d68d2d78a",
    "",
    "d2eb20898a301b5d8e69e99262720213",
    "93af01abb6a970047a7fc010" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    112 / 8,
    "c6e126a65faec77ab62318e30d8a50c39a664670039a66ae5a6874201bc68f9f",
    "0ba5193b2d3a8378d67163ce",
    "c37aada3d4408e880d47e41df77da9b9",
    "5844b289dc74327f9fd93f7aae1c3d39",
    "b5cd7563989b460a2fe187e90c41fc31",
    "79c73d0d1e3a4484909969de93b0" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    384 / 8,
    120 / 8,
    "2e6942d537f1a98444c2f9dbdb5d8db42a503a00a17b57d516399569e044a703",
    "7eb67721581ed52cfcfc2c4d",
    "e5f410fe939e79b7ad33fbd3aaf5856f",
    "a96cc73451502c7278b467ac85d5fc14fc1a2f51bc685645b173f0cd9af02d383095de063e6eaa50374ce9bc951e9e61",
    "727f5e19a5582e5782bbbe73517f0c04",
    "c492319abf12b03b380724ff1483a3" },
  { 0 }
};

/*
 * # CAVS 14.0
 * # GCM Decrypt with keysize 256 test information
 * # Generated on Fri Aug 31 11:31:25 2012
 */

static const gcm_block_vector_t gcm_dec_256_vec[] = {
  { 256 / 8,
    96 / 8,
    0 / 8,
    0 / 8,
    128 / 8,
    "f5a2b27c74355872eb3ef6c5feafaa740e6ae990d9d48c3bd9bb8235e589f010",
    "58d2240f580a31c1d24948e9",
    "",
    "",
    "",
    "15e051a5e4a5f5da6cea92e2ebee5bac" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    128 / 8,
    "4c8ebfe1444ec1b2d503c6986659af2c94fafe945f72c1e8486a5acfedb8a0f8",
    "473360e0ad24889959858995",
    "d2c78110ac7e8f107c0df0570bd7c90c",
    "",
    "7789b41cb3ee548814ca0b388c10b343",
    "c26a379b6d98ef2852ead8ce83a833a7" },
  // Libcryptosoc only supports 16 bytes long tags.
  { 256 / 8,
    96 / 8,
    0 / 8,
    128 / 8,
    120 / 8,
    "3395a1485315c5b5e6353acb05ae9499c440a2e9f5c57494662f827235ea314c",
    "3b7e632571602456b49880f0",
    "",
    "f283f80226dacb69c8af089ec6b59e81",
    "",
    "84c8beff4b0d160ee68ac613097f51" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    160 / 8,
    120 / 8,
    "4dc46ca55c1c1fcb4720c274c0e675c2ac5bf93d8dd5e951ca9f6b61f884edc9",
    "6473ab77dc885127422f5594",
    "",
    "e2cf8172ab4cf77eba45cd2c8ff939b938080a90",
    "",
    "8d6351f18d873242204c20144e2b83" },
  { 256 / 8,
    96 / 8,
    0 / 8,
    384 / 8,
    112 / 8,
    "a7f95798434f9a0fe6fd8acd30b8bad96dbdcfacee4594f01cbf26479be7d154",
    "9ef5a77b02137b46e8461d09",
    "",
    "5595a16fa12d4dcdba6b128480dce2d39c1211c3fb6068cde6013f6a80dfcda5eb92af8879e40ee9c177fd0e446fc8ca",
    "",
    "3bfd3d99fe2063e8ef8255519fe0" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    0 / 8,
    32 / 8,
    "57805f98aae1b8b64bb49756529ab8181b3ada674a90c55422e9eb26c48bcd7b",
    "9735945d8ca161777206632a",
    "58375442ab1c0e6a8952c83d128d9fc5",
    "",
    "4860116a6d2deb9bf794bfd6ac5bbbd6",
    "f45bb315" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    128 / 8,
    64 / 8,
    "f913bb823a1d0c10b0b72d56866907b893f2266f15de1abc17f93600824db55a",
    "d4fe686a14592b6ca1bd6b42",
    "35af9b502ea6b56269f896bf98affdd5",
    "e35d880c1c53688eb83869de9dd8a473",
    "ff426dd751190ff826e8b4a0792d746e",
    "9c2aa418b38bc7fd" },
  { 256 / 8,
    96 / 8,
    128 / 8,
    160 / 8,
    112 / 8,
    "74e9d9d7cd0728cea94e169af485f21f9d2447e022f16008f803dcf5c4f7cc0c",
    "ecba39edc96667da726122c0",
    "e5745ce0e02dbba05363b548c3ac7047",
    "ae9ab021f86f5b81bb2e0fcbd4b855e1501e9f82",
    "0a0b284515694188b6b6c15bc8a09036",
    "eacca7e61db6f72fc9b9e5bdb2bb" },
  { 256 / 8,
    96 / 8,
    104 / 8,
    0 / 8,
    112 / 8,
    "60667fce64b8c7169ddf45f335e46951248f69abc4e0f4f292d0ffe3dfd5219f",
    "1057322a39f08ef761c3c8fc",
    "501b033c841acb430c52d88fe9",
    "",
    "f386b28e7eb4c2fb8eb5dc66a2",
    "cb44c751f2f1641d1e801a534ac8" },
  { 256 / 8,
    96 / 8,
    104 / 8,
    160 / 8,
    120 / 8,
    "e67590da399cbcdcddcc56110562ade8665b50287a8ab38e8b9ee7520531b560",
    "2c36ab6b686a66fba1805196",
    "cff6b6f03c67152f3ce1030653",
    "823493d42f4f60b2d1433ad75eccaafd7e7c7d12",
    "da1c61fbfcdb73445ad4c7d889",
    "d9bd9a6559f5b04b48d77c2a1fc364" },
  { 0 }
};

// Long test vector used for in/out offset testing
static const gcm_block_vector_t gcm_enc_256_vec_long[] = {
  { 32,
    12,
    64,
    64,
    16,
    "b52c505a37d78eda5dd34f20c22540ea1b58963cf8e5bf8ffa85f9f2492505b4",
    "516c33929df5a3284ff463d7",
    "d7d6eb9e4130e787765183b2f69c4c9ee2c74f7c1362f7e9e1fca741da38610cd5392c6a69a8bec583f4e500fac9d2ebd5a689e12497375c8c"
    "610946ed119a97",
    "966d3eb0ed9e30f68381be714ac3aba29c824c33a1a218206464258f6c2109e7b54e783f3dfe37e21b1b57899c5196a264deaece753f830449"
    "a8a5340bfc529d",
    "314b67f5debb57b81dad4a94fb4d888e75388b67c0a29aa4503229753fe62471ea198e2e5f151a646624159ba16cbb72512bb4bd31c3730386"
    "bf6ea093781aaf",
    "bbdc3fb81e4c387d596a5931e0d11450" },
  { 32,
    12,
    256,
    0,
    16,
    "b52c505a37d78eda5dd34f20c22540ea1b58963cf8e5bf8ffa85f9f2492505b4",
    "516c33929df5a3284ff463d7",
    "a5c2bbf04f1bf1b15632c0a2eab81c7ba39a391e8460f758891116276eb1b4962ffd9abd9456fe5806733d5a06a70520a3182e9edad34bf44d"
    "b97264bbc8f65af4b54d7a523cf7a297968c5730d296a86bfc029a5280580e79ca34629b396c71fbb5e4c8e540dccdc21d6f3ee895b6641c6c"
    "725267aa4b7f42b28aed7d37f15ee7e559727958764da117637a7e55025905d95cad64d9f77af509c180d2c1939595f93333be2da014e12d6d"
    "b1501a06b415d39d519ab449e71d84493f42fdaa155015023ae4144f3e78466c438180a65f67e429ad89cd393be7b836d714219c1dd48404e2"
    "9b86b95002ded9ad1dbb1a1a15d8d9332c143001a1156815ff209fa4",
    "",
    "435f379bd090418e3dce0984e769d86b3465fd0557a09a1538df98138b6ff1eb10dd38f9a2eb5af9e3a3cdc15d026cb9279513c2cf870fab47"
    "671582c5a176629fcf11312c9b2581a6975a7f9836e6dfa2bd392746debac0842f7f8db6af254c7776dd02b4915bd8271431acac40b3946951"
    "f2da6576d5a4e24455b1bb61600dc5e8b0dd434944bb024ced54b800490a7df089fbb5729e21010526ab51cdc0d804c7e9de8c40358baa2fe8"
    "4ec178d0641d1fa97876bd12625b3082a30d2f549ef6f500fd2a726dc9bda89156e6383a0a20d7c16c0e6b1f5fe57ddd8286a6cf6abd84a4e5"
    "00def137f83091c61fbee6477e9cfd9163e28f6310815e1a1df590e4",
    "6248abb87bf1f9eb932aab7bff88acb0" },
  { 0 }
};

static const gcm_block_vector_t gcm_dec_256_vec_long[] = {
  { 32,
    12,
    64,
    64,
    16,
    "b52c505a37d78eda5dd34f20c22540ea1b58963cf8e5bf8ffa85f9f2492505b4",
    "516c33929df5a3284ff463d7",
    "314b67f5debb57b81dad4a94fb4d888e75388b67c0a29aa4503229753fe62471ea198e2e5f151a646624159ba16cbb72512bb4bd31c3730386"
    "bf6ea093781aaf",
    "966d3eb0ed9e30f68381be714ac3aba29c824c33a1a218206464258f6c2109e7b54e783f3dfe37e21b1b57899c5196a264deaece753f830449"
    "a8a5340bfc529d",
    "d7d6eb9e4130e787765183b2f69c4c9ee2c74f7c1362f7e9e1fca741da38610cd5392c6a69a8bec583f4e500fac9d2ebd5a689e12497375c8c"
    "610946ed119a97",
    "bbdc3fb81e4c387d596a5931e0d11450" },
  { 32,
    12,
    256,
    0,
    16,
    "b52c505a37d78eda5dd34f20c22540ea1b58963cf8e5bf8ffa85f9f2492505b4",
    "516c33929df5a3284ff463d7",
    "435f379bd090418e3dce0984e769d86b3465fd0557a09a1538df98138b6ff1eb10dd38f9a2eb5af9e3a3cdc15d026cb9279513c2cf870fab47"
    "671582c5a176629fcf11312c9b2581a6975a7f9836e6dfa2bd392746debac0842f7f8db6af254c7776dd02b4915bd8271431acac40b3946951"
    "f2da6576d5a4e24455b1bb61600dc5e8b0dd434944bb024ced54b800490a7df089fbb5729e21010526ab51cdc0d804c7e9de8c40358baa2fe8"
    "4ec178d0641d1fa97876bd12625b3082a30d2f549ef6f500fd2a726dc9bda89156e6383a0a20d7c16c0e6b1f5fe57ddd8286a6cf6abd84a4e5"
    "00def137f83091c61fbee6477e9cfd9163e28f6310815e1a1df590e4",
    "",
    "a5c2bbf04f1bf1b15632c0a2eab81c7ba39a391e8460f758891116276eb1b4962ffd9abd9456fe5806733d5a06a70520a3182e9edad34bf44d"
    "b97264bbc8f65af4b54d7a523cf7a297968c5730d296a86bfc029a5280580e79ca34629b396c71fbb5e4c8e540dccdc21d6f3ee895b6641c6c"
    "725267aa4b7f42b28aed7d37f15ee7e559727958764da117637a7e55025905d95cad64d9f77af509c180d2c1939595f93333be2da014e12d6d"
    "b1501a06b415d39d519ab449e71d84493f42fdaa155015023ae4144f3e78466c438180a65f67e429ad89cd393be7b836d714219c1dd48404e2"
    "9b86b95002ded9ad1dbb1a1a15d8d9332c143001a1156815ff209fa4",
    "6248abb87bf1f9eb932aab7bff88acb0" },
  { 0 }
};

#if defined(SEMAILBOX_PRESENT)
static const gcm_multipart_vector_t gcm_multipart_invalid_param_vector[] = {
  { 128,
    SLI_CIPHER_DECRYPT,
    "feffe9928665731c6d6a8f9467308308",
    "42831ec2217774244b7221b784d0d49ce3aa212f2c02a4e035c17e2329aca12e"
    "21d514b25466931c7d8f6a5aac84aa051ba30b396a0aac973d58e091",
    "cafebabefacedbaddecaf888",
    "feedfacedeadbeeffeedfacedeadbeefabaddad2",
    128 / 8,
    "5bc94fbc3221a5db94fae95ae7121a47",
    "d9313225f88406e5a55909c5aff5269a86a7a9531534f7da2e4c303d8a318a72"
    "1c3c0c95956809532fcf0e2449a6b525b16aedf5aa0de657ba637b39" },
  { 0 }
};
#endif

/****************************************************************
* CHACHA20-POLY1305
****************************************************************/

typedef struct {
  char *key;
  uint32_t key_len;
  char *plain_text;
  uint32_t text_len;
  char *add_data;
  uint32_t add_data_len;
  char *iv;
  uint32_t iv_len;
  char *cipher_text;
  uint32_t cipher_len;
  char *tag;
  uint32_t tag_len;
} sli_chachapoly_test_vector_t;

#define CHAPOL_TEXT_MAX_LEN 512

const sli_chachapoly_test_vector_t chachapoly_test_vector[] = {
  // RFC8439 2.8.2
  { "80818283 84858687 88898a8b 8c8d8e8f"
    "90919293 94959697 98999a9b 9c9d9e9f",
    32,
    "4c616469 65732061 6e642047 656e746c"
    "656d656e 206f6620 74686520 636c6173"
    "73206f66 20273939 3a204966 20492063"
    "6f756c64 206f6666 65722079 6f75206f"
    "6e6c7920 6f6e6520 74697020 666f7220"
    "74686520 66757475 72652c20 73756e73"
    "63726565 6e20776f 756c6420 62652069"
    "742e",
    114,
    "50515253 c0c1c2c3 c4c5c6c7",
    12,
    "07000000 40414243 44454647",
    12,
    "d31a8d34 648e60db 7b86afbc 53ef7ec2"
    "a4aded51 296e08fe a9e2b5a7 36ee62d6"
    "3dbea45e 8ca96712 82fafb69 da92728b"
    "1a71de0a 9e060b29 05d6a5b6 7ecd3b36"
    "92ddbd7f 2d778b8c 9803aee3 28091b58"
    "fab324e4 fad67594 5585808b 4831d7bc"
    "3ff4def0 8e4b7a9d e576d265 86cec64b"
    "6116",
    114,
    "1ae10b59 4f09e26a 7e902ecb d0600691",
    16 },
  // RFC8439 A.5
  { "1c9240a5 eb55d38a f3338886 04f6b5f0"
    "473917c1 402b8009 9dca5cbc 207075c0",
    32,
    "496e7465 726e6574 2d447261 66747320"
    "61726520 64726166 7420646f 63756d65"
    "6e747320 76616c69 6420666f 72206120"
    "6d617869 6d756d20 6f662073 6978206d"
    "6f6e7468 7320616e 64206d61 79206265"
    "20757064 61746564 2c207265 706c6163"
    "65642c20 6f72206f 62736f6c 65746564"
    "20627920 6f746865 7220646f 63756d65"
    "6e747320 61742061 6e792074 696d652e"
    "20497420 69732069 6e617070 726f7072"
    "69617465 20746f20 75736520 496e7465"
    "726e6574 2d447261 66747320 61732072"
    "65666572 656e6365 206d6174 65726961"
    "6c206f72 20746f20 63697465 20746865"
    "6d206f74 68657220 7468616e 20617320"
    "2fe2809c 776f726b 20696e20 70726f67"
    "72657373 2e2fe280 9d",
    265,
    "f3338886 00000000 00004e91",
    12,
    "00000000 01020304 05060708",
    12,
    "64a08615 75861af4 60f062c7 9be643bd"
    "5e805cfd 345cf389 f108670a c76c8cb2"
    "4c6cfc18 755d43ee a09ee94e 382d26b0"
    "bdb7b73c 321b0100 d4f03b7f 355894cf"
    "332f830e 710b97ce 98c8a84a bd0b9481"
    "14ad176e 008d33bd 60f982b1 ff37c855"
    "9797a06e f4f0ef61 c186324e 2b350638"
    "3606907b 6a7c02b0 f9f6157b 53c867e4"
    "b9166c76 7b804d46 a59b5216 cde7a4e9"
    "9040c5a4 0433225e e282a1b0 a06c523e"
    "af4534d7 f83fa115 5b004771 8cbc546a"
    "0d072b04 b3564eea 1b422273 f548271a"
    "0bb23160 53fa7699 1955ebd6 3159434e"
    "cebb4e46 6dae5a10 73a67276 27097a10"
    "49e617d9 1d361094 fa68f0ff 77987130"
    "305beaba 2eda04df 997b714d 6c6f2c29"
    "a6ad5cb4 022b0270 9b",
    265,
    "eead9d67 890cbb22 392336fe a1851f38",
    16 }
};

// Test vector with zero-length plaintext and additional data
const sli_chachapoly_test_vector_t chachapoly_test_null_vector = { "00000000 00000000 00000000 00000000"
                                                                   "00000000 00000000 00000000 00000000", // key
                                                                   32,
                                                                   "", // plaintext
                                                                   0,
                                                                   "", // addition data
                                                                   0,
                                                                   "000000000000000000000000", // nonce
                                                                   12,
                                                                   "", // ciphertext
                                                                   0,
                                                                   "4eb972c9 a8fb3a1b 382bb4d3 6f5ffad1", // tag
                                                                   16 };

#endif // PSA_DRIVER_AEAD_TEST_DATA_H
