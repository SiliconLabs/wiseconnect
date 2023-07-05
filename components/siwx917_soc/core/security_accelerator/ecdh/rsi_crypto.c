/*******************************************************************************
* @file  rsi_crypto.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "stdint.h"
#include "ecdh.h"
#include "rsi_sha_hw_9116.h"
#include "rsi_crypto.h"
//#include "rsi_hmac.h"
#include "rsi_aes.h"
#include "string.h"
#include "stdlib.h"

#if 0
/*==============================================*/
/**
 * @fn         int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
 * @brief      THis API Decides whether the sha message can be sent once or requires multiple calls to send
 * @param[in]  sha_mode : 1 – For SHA1 2 – For SHA256 3 – For SHA384 4 – For SHA512 
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[out] digest:  Output parameter to hold computed digest from SHA
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
	int32_t  status = RSI_SUCCESS;
	uint16_t output_length;
	uint8_t *output=NULL;
	rsi_sha_req_t *crypto;	

	crypto->total_msg_length=msg_length;
	crypto->algorithm_sub_type=sha_mode;

	if(sha_mode==SHA_1){
		output_length=SHA_1_DIGEST_LEN;
	}
	else if(sha_mode==SHA_256){
		output_length=SHA_256_DIGEST_LEN;
	}
	else if(sha_mode==SHA_384){
		output_length=SHA_384_DIGEST_LEN;
	}
	else{
		output_length=SHA_512_DIGEST_LEN;
	}

	output=(uint8_t *)malloc(output_length);

//	rsi_crypto_SHA(crypto,&output_length,output,msg);
	memcpy(digest,output,output_length);
	free(output);
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_hmac_sha(uint8_t hmac_sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *key, uint16_t key_length, uint8_t *digest)
 * @brief      This API Decides whether the message can be sent once or requires multiple calls to send
 * @param[in]  hmac_sha_mode : 1 – For HMAC-SHA1 2 – For HMAC-SHA256 3 – For HMAC-SHA384 4 – For HMAC-SHA512 
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  key: Pointer to HMAC key
 * @param[in]  key_length: HMAC key length in bytes
 * @param[out] digest:  Output parameter to hold computed digest from HMAC-SHA
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *key, uint16_t key_length, uint8_t *digest)
{
	rsi_hmac_sha_req_t hmac;
	int32_t  status = RSI_SUCCESS;
	uint8_t  *data = NULL;
	uint8_t *output=NULL;
	uint16_t output_length;

	hmac.total_msg_length = msg_length + key_length;
	hmac.algorithm_sub_type=hmac_sha_mode;
	hmac.key_length=key_length;

	data = (uint8_t *)malloc(hmac.total_msg_length);
	if(data == NULL)
	{
		//! return packet allocation failure error
		return -1;

	}
	//! Copy key in to data
	memcpy(data, key, key_length);

	//! Copy message in to data
	memcpy((data+key_length), msg, msg_length);
	hmac.algorithm_sub_type=hmac_sha_mode;

	if(hmac_sha_mode==SHA_1){
		output_length=SHA_1_DIGEST_LEN;
	}
	else if(hmac_sha_mode==SHA_256){
		output_length=SHA_256_DIGEST_LEN;
	}
	else if(hmac_sha_mode==SHA_384){
		output_length=SHA_384_DIGEST_LEN;
	}
	else{
		output_length=SHA_512_DIGEST_LEN;
	}

	output=(uint8_t *)malloc(output_length);

	//rsi_crypto_HMAC(&hmac,&output_length,output,data);
	memcpy(digest,output,output_length);
	if(data != 0 || output != 0)
	{
		free(data);
		data =NULL;
		free(output);
		output =NULL;
	}	
	return status;
}
#endif
/*==============================================*/
/**
 * @fn         int32_t rsi_aes(uint8_t aes_mode,
                uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output)
 * @brief      This API calculates encrypted and decrypted data for different AES modes
 * @param[in]  aes_mode : 1 – For AES CBC mode 2 – For AES ECB mode 3 – For AES CTR mode 
 * @param[in]  enc_dec: 1 – For AES Encryption 2 – For AES Decryption 
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  key: Pointer to AES key
 * @param[in]  key_length: AES key length in bytes
 * @param[in]  iv: Pointer to AES iv
 * @param[out]  output:  Output parameter to hold encrypted/decrypted from AES
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_aes(uint8_t aes_mode,
                uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output)
{
  int32_t status = RSI_SUCCESS;
  uint16_t output_length;

  rsi_aes_req_t aes;
  aes.algorithm_sub_type = aes_mode;
  aes.total_msg_length   = msg_length;
  aes.encrypt_decryption = enc_dec;
  aes.key_length         = key_length;
  memcpy(aes.IV, iv, sizeof(aes.IV));
  memcpy(aes.key, key, sizeof(aes.key));

  output_length = msg_length;

  status = rsi_crypto_aes(&aes, &output_length, output, msg);

  return status;
}

#if 0
/*==============================================*/
/**
 * @fn         int32_t rsi_exponentiation(uint8_t *prime, uint32_t prime_length, uint8_t *base, uint32_t base_length, uint8_t *exponent, uint32_t exponent_length, uint8_t *exp_result)
 * @brief      This API is used to DH key calculation 
 * @param[in]  prime: pointer to the prime 
 * @param[in]  prime_length: Length of the prime
 * @param[in]  base: pointer to base 
 * @param[in]  base_length: Length of the base
 * @param[in]  exponent: pointer to exponent
 * @param[in]  exponent_length: Length of the exponent 
 * @param[out] exp_result: Output exponentiation result 
 * @return     Non zero - If fails
 *             Zero - If success
 */
rsi_exp_req_t *exponentiation;
int32_t rsi_exponentiation(uint8_t *prime, uint32_t prime_length, uint8_t *base, uint32_t base_length, uint8_t *exponent, uint32_t exponent_length, uint8_t *exp_result)
{
	int32_t status = RSI_SUCCESS;

	exponentiation = malloc(sizeof(rsi_exp_req_t));
	if(exponentiation==NULL)
		return -1;

	memset(exponentiation,0,sizeof(rsi_exp_req_t));

	memcpy(exponentiation->prime,prime,prime_length);

	memcpy(exponentiation->base,base,base_length);

	memcpy(exponentiation->exponent,exponent,exponent_length);

//	status = calc_dh_key(exponentiation->prime,prime_length,(NN_DIGIT *)exponentiation->base,base_length,exponentiation->exponent,exponent_length,(NN_DIGIT *)exp_result,0);

	if(exponentiation)
	{
		free(exponentiation);
		exponentiation = NULL;
	}
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ecdh_point_multiplication(uint8_t ecdh_mode, uint8_t *d, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief      THis API Computes the ecdh point multiplication vector 
 * @param[in]  ecdh_mode: 1 – For ECDH 192 2 – For ECDH 224 3 – For ECDH 256    
 * @param[in]  d: Pointer to scalar value that needs to be multiplied 
 * @param[in]  sx, sy, sz: Pointers to x, y, z coordinates of the point to be multiplied with scalar ‘d’  
 * @param[out] rx, ry, rz: Pointers to x, y, z coordinates of the result point  
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_ecdh_point_multiplication(uint8_t ecdh_mode, uint8_t *d, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
	int32_t 		status = RSI_SUCCESS;
//	ecdh_point_multiplication_hw(ecdh_mode,ECDH_CURVE_P,0,d,sx,sy,sz,0,0,0,rx,ry,rz);
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ecdh_point_addition(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *tx, uint8_t *ty, uint8_t *tz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief      THis API Computes the ecdh point addition vector 
 * @param[in]  ecdh_mode: 1 – For ECDH 192 2 – For ECDH 224 3 – For ECDH 256    
 * @param[in]  sx, sy, sz: Pointers to x, y, z coordinates of the point1 that needs to be added  
 * @param[in]  tx, ty, tz: Pointers to x, y, z coordinates of the point2 that needs to be added  
 * @param[out] rx, ry, rz: Pointers to x, y, z coordinates of the result point  
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_ecdh_point_addition(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *tx, uint8_t *ty, uint8_t *tz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
	int32_t status=RSI_SUCCESS; 
//	ecdh_point_addition_or_subtraction_hw(ecdh_mode,0,sx,sy,sz,tx,ty,tz,0,0,0,rx,ry,rz);
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ecdh_point_substraction(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *tx, uint8_t *ty, uint8_t *tz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief      This API Computes the ecdh point subtraction vector 
 * @param[in]  ecdh_mode: 1 – For ECDH 192 2 – For ECDH 224 3 – For ECDH 256    
 * @param[in]  sx, sy, sz: Pointers to x, y, z coordinates of the point1 that needs to be subtracted  
 * @param[in]  tx, ty, tz: Pointers to x, y, z coordinates of the point2 that needs to be subtracted  
 * @param[out] rx, ry, rz: Pointers to x, y, z coordinates of the result point   
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_ecdh_point_substraction(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *tx, uint8_t *ty, uint8_t *tz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
	int32_t 		status       = RSI_SUCCESS;
	//ecdh_point_addition_or_subtraction_hw(ecdh_mode,1,sx,sy,sz,tx,ty,tz,0,0,0,rx,ry,rz);
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ecdh_point_double(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief      This API Computes the ecdh point double vector 
 * @param[in]  ecdh_mode: 1 – For ECDH 192 2 – For ECDH 224 3 – For ECDH 256    
 * @param[in]  sx, sy, sz: Pointers to x, y, z coordinates of the point1 that needs to be doubled  
 * @param[out] rx, ry, rz: Pointers to x, y, z coordinates of the result point  
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_ecdh_point_double(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
	int32_t     status     = RSI_SUCCESS;
//	ecdh_point_doubling_hw(ecdh_mode,sx,sy,sz,0,rx,ry,rz);
	return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_ecdh_point_affine(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
 * @brief      This API Computes the ecdh point affinity vector 
 * @param[in]  ecdh_mode: 1 – For ECDH 192 2 – For ECDH 224 3 – For ECDH 256    
 * @param[in]  sx, sy, sz: Pointers to x, y, z coordinates of the point1 that needs to be affinified  
 * @param[out] rx, ry, rz: Pointers to x, y, z coordinates of the result point   
 * @return      Non zero - If fails
 *              Zero - If success
 */
int32_t rsi_ecdh_point_affine(uint8_t ecdh_mode, uint8_t *sx, uint8_t *sy, uint8_t *sz, uint8_t *rx, uint8_t *ry, uint8_t *rz)
{
	int32_t     status = RSI_SUCCESS;
//	affinify(ecdh_mode,sx,sy,sz);
	return status;
}
#endif