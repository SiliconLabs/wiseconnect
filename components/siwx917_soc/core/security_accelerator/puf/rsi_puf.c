/*******************************************************************************
* @file  rsi_puf.c
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

#include "rsi_puf.h"
#include "puf_api.h"
#include "rsi_common.h"
#include "rsi_error.h"
#include "rsi_board.h"

uint32_t actv_code[298];

uint32_t activation_code[] = {
  0x8f027e24, 0xc01878e5, 0x721417a1, 0x8f2697e8, 0x8c052524, 0xb30172e9, 0x50c20206, 0x8a8a44ed, 0xf8da7b62,
  0xa03f5b9c, 0xcf8ce48b, 0x52db676,  0x13df5778, 0x9e3863c9, 0xbdacc07d, 0x8a17e49c, 0x327d85d9, 0x2efeef80,
  0x950c48f7, 0xf696f9f3, 0xb0c74bf8, 0xd9338398, 0xba219dc3, 0x8e19b1a6, 0xcb020ad1, 0xb8d046a0, 0x81bcf773,
  0x6ede8af8, 0x42faeec2, 0xbb77d155, 0x85488da1, 0xdfdc95ca, 0xe01a1d99, 0x62fd05da, 0x5f3dd61d, 0x45b74acd,
  0xb127a1c1, 0x76988295, 0xc49593ff, 0xd2551ebb, 0x9bf93994, 0x144c37c5, 0xb87d66b0, 0xcb50a9da, 0xb283383a,
  0x20fc6fbf, 0xe71b5e3a, 0x778bd522, 0x3c9d9ab9, 0xc1cf310f, 0xf44a792b, 0xc119f5bd, 0xa63fdbcb, 0x9515ceca,
  0x599b4286, 0xbb1a84ed, 0x27d3fe11, 0x84b163a6, 0xe6caaa64, 0x45af742e, 0xc30ef33a, 0x485895a6, 0xd9ff9b6f,
  0xd519726c, 0xb4c51ea3, 0x217cf2a2, 0x3e3cbefa, 0x7ddf9e84, 0x8431a1e4, 0xd8780593, 0x6b6a0bbe, 0x1a46714,
  0xb575d2a,  0xd407903e, 0xa1a472e7, 0xcfb7d3f1, 0x8d9d170e, 0xdffe009,  0xf001eff2, 0xa4e4b3dc, 0x489ab1e4,
  0x1d297f52, 0xefb5098d, 0x3b6a7ea1, 0xca44552e, 0xb75ac50d, 0x4e9da3a3, 0x8db55c71, 0xbb6d51a6, 0x11ece074,
  0x768bcf48, 0x935d163e, 0xd389f2bd, 0xb3ca4bf9, 0xa10a5ac3, 0x923eebc,  0xde4135e7, 0xb9c5e1c7, 0xb2ff2ea,
  0x189bf0bd, 0x2872870b, 0x723fe2d4, 0x4da1d354, 0x348022b1, 0x67a5c8ea, 0x52f85b40, 0xc0ff8c60, 0x86e2bbc4,
  0x5dd5e2e3, 0x9971050,  0x8c9c93e1, 0x47c79b1e, 0x34cab1a,  0x5d476b0,  0x24835a5c, 0xe1c94dd1, 0x6599c95e,
  0x1a6ec55e, 0xb01022ca, 0x738e3070, 0x8d0a2b17, 0xa31455b7, 0xc1046523, 0x616a2498, 0x864a42bf, 0x9d4988a4,
  0x1d73d3e4, 0x375c89d4, 0x4c0ed0ab, 0xd16b8756, 0x99c338e4, 0xca3a1915, 0x1f5fade8, 0x49bf7f72, 0x8806d442,
  0x1cda4723, 0x1dc15774, 0x7c43ec1e, 0x909be13b, 0xd2f18709, 0x2171c67b, 0xf9863212, 0x6844380,  0xa9affcd2,
  0x34fb376d, 0xe40e3c3,  0x1a9ffe1a, 0x90b0e02d, 0x47517b39, 0x8493ed5c, 0xbc2839d1, 0xd7593350, 0xc1934b45,
  0xd52db55d, 0xe7e9b0bd, 0x4bcb835,  0xde2919b3, 0x7a70f143, 0x478c71d0, 0x431172c6, 0x43bbeef9, 0x633eafc6,
  0x67a90038, 0xa3924e98, 0x3b537d5b, 0x2f6b9e1d, 0xc066f5dc, 0x6c301de1, 0xf70b78b6, 0x5baab8e5, 0x9a3cd2c6,
  0x376f372f, 0x23ce7e02, 0x23eb7a3e, 0xb7982660, 0xfef5c49a, 0xeac853e9, 0x8a1deba4, 0xf160b5cd, 0x5f7786e7,
  0x41abf21,  0xd8edd0a8, 0xdad373ec, 0x63aa35fb, 0x72306dd0, 0x443bd8aa, 0x2687c73,  0xa48d2f45, 0x74a40765,
  0xa5917aac, 0x310427f6, 0x442c0e96, 0xf5facdf8, 0xa1d547be, 0x7ebe85c,  0xf2c5fbec, 0xaca7d454, 0x4830ffab,
  0x4b6be50a, 0x5fb32323, 0x50d218bb, 0xf3969540, 0xe4f823fb, 0x9670a9d4, 0x449fea,   0xab476574, 0xd5b6ff1c,
  0xc37514f2, 0x1d61b12,  0xfd5726c0, 0xa9ae3c4a, 0x194c1396, 0x838d0854, 0x466f879f, 0x424bcb07, 0xd02c3496,
  0x6df12fa0, 0x721e3ffa, 0xc09b94c9, 0xe08d87bf, 0x974d8755, 0x34205aa6, 0x7bb01233, 0x1a9713c8, 0xf40a9747,
  0x1c58db8a, 0x9a35fa6,  0xae11ec6,  0x6b63553b, 0xec5c50bc, 0x5e4eb830, 0xb6ee968d, 0x5c095819, 0xbdbe387c,
  0x822cb4fd, 0xa2868cf4, 0xf4870f8e, 0x4d8b0320, 0xc3ec2ee7, 0xb385ae41, 0x63541d3b, 0xb264ba3c, 0x8c718e87,
  0x949cb5f8, 0xe62e8437, 0x9043814c, 0x29d3a67a, 0x55d9fb9,  0x95028ce0, 0x42b6ec1b, 0x5a92f3f5, 0xd5e29a87,
  0x4d020c27, 0xf0a12992, 0x9ff08cd4, 0x4d48872c, 0xb2ddfdf5, 0x282b3174, 0x3685d923, 0x9e088338, 0xf80999ff,
  0x64c059b2, 0x82da99c5, 0x4d32ac1d, 0xc4111936, 0x41bf11ce, 0x3a567f50, 0xdcf51099, 0xc4582e4c, 0xde6bec2c,
  0x65609bd5, 0xb9977e67, 0xe19ddd71, 0xab6cf5dc, 0xae3569e3, 0xfb0b87a8, 0xe192b1eb, 0x75022236, 0x9006a88d,
  0xcd8d2080, 0x60aa9b9d, 0x8f813dcd, 0x2f56af5c, 0x3ee22a31, 0xc3963a13, 0x8720105e, 0x65fe1bef, 0xf1f006ef,
  0xdcabbca0, 0x9304834c, 0x2361f7de, 0x3b8509b1, 0x7352292,  0xa51ff118, 0xfe95f84e, 0x2f20948e, 0x877f92f1,
  0x1bab4681

};
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_enroll_req(void)
 * @brief      This API used to Enroll PUF if not blocked earlier
 * @return     Non zero - If fails
 *             Zero - If success          
 */
int32_t rsi_puf_enroll_req(void)
{

  int32_t status = RSI_SUCCESS;
  uint32_t i;
  puf_enroll(actv_code);

  //! Return the status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_enroll_disable_req(void)
 * @brief      This API is used to Blocks further Enrolls of PUF
 * @return     Non zero - If fails
 *             Zero - If success
 */
int32_t rsi_puf_enroll_disable_req(void)
{
  uint32_t status = 0;

  status = puf_block(BLOCK_ENROLL);

  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_start_req(void)
 * @brief      This API is used to Start PUF if earlier enrolled
 * @return     Non zero - If fails
 *             Zero - If success
 */
int32_t rsi_puf_start_req(void)
{
  int32_t status = RSI_SUCCESS;

  status = puf_start(activation_code);
  //! Return the status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_set_key_req(uint8_t key_index,
                            uint16_t key_size,
                            uint8_t *key_ptr,
                            uint8_t *set_key_resp,
                            uint16_t length)
 * @brief      Set Key for PUF, This API Does set key operation on key provided
 * @param[in]  key_index, Key Index
 * @param[in]  key_size, Key Size in byte : 1 - 256bit, 0 - 128bit
 * @param[in]  key_ptr, Pointer to Key
 * @param[out] set_key_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return     Non zero - If fails
 *             Zero - If success
 */
int32_t rsi_puf_set_key_req(uint8_t key_index,
                            uint16_t key_size,
                            uint8_t *key_ptr,
                            uint8_t *set_key_resp,
                            uint16_t length)
{
  int32_t status = RSI_SUCCESS; //! Get wlan cb structure pointer
  uint16_t keysize;

  //! Parameter check
  if ((key_index > 15) || (key_size > 1) || (set_key_resp == NULL)) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (key_size == 1) {
    keysize = BITS_256;
  } else if (key_size == 0) {
    keysize = BITS_128;
  } else {
    keysize = INVALID_KEYSIZE;
  }

  status = puf_set_uk((uint32_t *)key_ptr, key_index, keysize, (uint32_t *)set_key_resp);

  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_set_key_disable_req(void)
 * @brief      This API is used to Blocks Set key operation on PUF
 * @return     Non zero - If fails
 *             Zero - If success    
 */
int32_t rsi_puf_set_key_disable_req(void)
{
  uint32_t status = 0;

  status = puf_block(BLOCK_SET_KEY);

  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_get_key_req(uint8_t *keycode_ptr, uint8_t *get_key_resp, uint16_t length)
 * @brief      Get Key using PUF, This API Does Get key operation with keycode provided
 * @param[in]  keycode_ptr, Pointer to KeyCode
 * @param[out] get_key_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_get_key_req(uint8_t *keycode_ptr, uint8_t *get_key_resp, uint16_t length)
{
  puf_get_key((uint32_t *)keycode_ptr, (uint32_t *)get_key_resp, 0x01, (uint32_t *)&length);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_puf_get_key_disable_req(void)
 * @brief      This API Blocks further get key operation on PUF
 * @return     Non zero - If fails
 *             Zero - If success
 */
int32_t rsi_puf_get_key_disable_req(void)
{
  uint32_t status = 0;

  status = puf_block(BLOCK_GET_KEY_REG);

  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_load_key_req(uint8_t *keycode_ptr)
 * @brief      This API Loads Key using PUF to AES engine
 * @param[in]  keycode_ptr, Pointer to KeyCode
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_load_key_req(uint8_t *keycode_ptr)
{
  int32_t status        = RSI_SUCCESS;
  uint32_t send_size    = 0;
  uint32_t key_pass[44] = { 0 };
  //! Parameter check
  if (keycode_ptr == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  puf_get_key((uint32_t *)keycode_ptr, NULL, 0x00, &send_size);

  //! Return the status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_aes_encrypt_req(uint8_t mode,
                                uint8_t key_source,
                                uint16_t key_size,
                                uint8_t *key_ptr,
                                uint16_t data_size,
                                uint8_t *data_ptr,
                                uint16_t iv_size,
                                uint8_t *iv_ptr,
                                uint8_t *aes_encry_resp,
                                uint16_t length)
 * @brief      Encrypt using AES, This API is used to Encrypts data using IV, Key provided depending on mode
 * @param[in]  mode, AES Mode : 1 - CBC, 0 - EBC
 * @param[in]  key_source,Key Source : 0 - PUF, 1 - AES
 * @param[in]  key_size, Key Size : 1 - 256bit, 0 - 128bit
 * @param[in]  key_ptr, Pointer to Key
 * @param[in]  data_size, Data Size : size of data in bytes
 * @param[in]  data_ptr, Pointer to Data
 * @param[in]  iv_size,IV Size : 1 - 256bit, 0 - 128bit
 * @param[in]  iv, Pointer to IV
 * @param[out] aes_encry_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_aes_encrypt_req(uint8_t mode,
                                uint8_t key_source,
                                uint16_t key_size,
                                uint8_t *key_ptr,
                                uint16_t data_size,
                                uint8_t *data_ptr,
                                uint16_t iv_size,
                                uint8_t *iv_ptr,
                                uint8_t *aes_encry_resp,
                                uint16_t length)
{
  uint8_t Mode   = 0;
  int32_t status = RSI_SUCCESS;
  Mode           = (mode | (key_size << 1) | (key_source << 2) | (iv_size << 3));

  data_aes_encry_decry((Mode = Mode | MODE_ENCRYPT),
                       (uint32_t *)data_ptr,
                       0,
                       (uint32_t *)iv_ptr,
                       (uint32_t *)key_ptr,
                       data_size,
                       (uint32_t *)aes_encry_resp,
                       NULL,
                       0);

  //! Return the status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_aes_decrypt_req(uint8_t mode,
                                uint8_t key_source,
                                uint16_t key_size,
                                uint8_t *key_ptr,
                                uint16_t data_size,
                                uint8_t *data_ptr,
                                uint16_t iv_size,
                                uint8_t *iv_ptr,
                                uint8_t *aes_decry_resp,
                                uint16_t length)
 * @brief      Decrypt using AES, This API Decrypts data using IV, Key provided depending on mode
 * @param[in]  mode, AES Mode : 1 - CBC, 0 - EBC
 * @param[in]  key_source, Key Source : 0 - PUF, 1 - AES
 * @param[in]  key_size, Key Size : 1 - 256bit, 0 - 128bit
 * @param[in]  key_ptr, Pointer to Key
 * @param[in]  data_size, Data Size : size of data in bytes
 * @param[in]  data_ptr, Pointer to Encrypted Data
 * @param[in]  iv_size, IV Size : 1 - 256bit, 0 - 128bit
 * @param[in]  in_ptr, Pointer to IV
 * @param[out] aes_decry_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_aes_decrypt_req(uint8_t mode,
                                uint8_t key_source,
                                uint16_t key_size,
                                uint8_t *key_ptr,
                                uint16_t data_size,
                                uint8_t *data_ptr,
                                uint16_t iv_size,
                                uint8_t *iv_ptr,
                                uint8_t *aes_decry_resp,
                                uint16_t length)
{
  uint8_t Mode             = 0;
  int32_t status           = RSI_SUCCESS;
  uint32_t offset_in_flash = 0;
  uint32_t nonce           = 0;
  uint32_t type            = 0;
  Mode                     = (mode | (key_size << 1) | (key_source << 2) | (iv_size << 3));
  data_aes_encry_decry((Mode = Mode & 0x0f),
                       (uint32_t *)data_ptr,
                       offset_in_flash,
                       (uint32_t *)iv_ptr,
                       (uint32_t *)key_ptr,
                       data_size,
                       (uint32_t *)aes_decry_resp,
                       &nonce,
                       type);
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_puf_aes_mac_req(uint8_t key_source,
                            uint16_t key_size,
                            uint8_t *key_ptr,
                            uint16_t data_size,
                            uint8_t *data_ptr,
                            uint16_t iv_size,
                            uint8_t *iv_ptr,
                            uint8_t *aes_mac_resp,
                            uint16_t length)
 * @brief      Calculate AES CBC mode MAC, This API Cacluates MAC for given data using AES-CBC mode
 * @param[in]  key_source, Key Source : 0 - PUF, 1 - AES
 * @param[in]  key_size,Key Size : 1 - 256bit, 0 - 128bit
 * @param[in]  key_ptr,Pointer to Key
 * @param[in]  data_size, size of data in bytes
 * @param[in]  data_pte, Pointer to Data
 * @param[in]  iv_size, IV Size : 1 - 256bit, 0 - 128bit
 * @param[in]  iv_ptr, Pointer to IV
 * @param[out] aes_mac_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_aes_mac_req(uint8_t key_source,
                            uint16_t key_size,
                            uint8_t *key_ptr,
                            uint16_t data_size,
                            uint8_t *data_ptr,
                            uint16_t iv_size,
                            uint8_t *iv_ptr,
                            uint8_t *aes_mac_resp,
                            uint16_t length)
{
  uint8_t mode = 0;
  mode         = ((key_size << 1) | (key_source << 2) | (iv_size << 3));
  data_aes_mac(mode,
               (uint32_t *)data_ptr,
               (uint32_t *)iv_ptr,
               (uint32_t *)key_ptr,
               data_size,
               (uint32_t *)aes_mac_resp);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_puf_intr_key_req(uint8_t key_index, uint8_t key_size, uint8_t *intr_key_resp, uint16_t length)
 * @brief      Intrinsic Key for PUF, This API Does set intrinsic key operation
 * @param[in]  key_index, Key Index
 * @param[in]  key_size, Key Size : 1 - 256bit, 0 - 128bit
 * @param[out] intr_key_resp, Pointer to response
 * @param[out] length, Length of response buffer
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_puf_intr_key_req(uint8_t key_index, uint8_t key_size, uint8_t *intr_key_resp, uint16_t length)
{
  int32_t status = RSI_SUCCESS;

  //! Parameter check
  if ((key_index > 15) || (key_size > 1) || (intr_key_resp == NULL)) {
    return RSI_ERROR_INVALID_PARAM;
  }

  puf_set_intrensic_key(key_index, key_size, (uint32_t *)intr_key_resp);
  //! Return the status
  return status;
}
