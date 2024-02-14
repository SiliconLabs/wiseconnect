#include "console.h"
#include "sl_si91x_aes.h"
#include "sl_si91x_wrap.h"
#include "sl_si91x_crypto_utility.h"
#include "sl_string.h"
#include "sl_net.h"
#include "sl_wifi.h"
#include "sl_si91x_sha.h"
#include "sl_si91x_hmac.h"
#include "sl_si91x_ecdh.h"

#define BUFFER_SIZE                   256
#define SL_SI91X_ECDH_MAX_VECTOR_SIZE 32

uint8_t *encrypted_buffer_ptr = NULL;
uint8_t *decrypted_buffer_ptr = NULL;

sl_status_t aes_encryption_command_handler(console_args_t *arguments)
{
  uint16_t ii                  = 0;
  sl_status_t status           = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint8_t key[32]              = { 0 };
  uint8_t *key_ptr;
  encrypted_buffer_ptr = malloc(BUFFER_SIZE);
  memset(encrypted_buffer_ptr, '\0', BUFFER_SIZE);
  config.aes_mode                   = sl_cli_get_argument_int8(arguments, 0);
  config.encrypt_decrypt            = sl_cli_get_argument_int8(arguments, 1);
  config.msg                        = sl_cli_get_argument_hex(arguments, 2);
  config.key_config.b0.key_size     = sl_cli_get_argument_int8(arguments, 3);
  config.key_config.b0.wrap_iv_mode = sl_cli_get_argument_int8(arguments, 4);
  config.key_config.b0.key_type     = sl_cli_get_argument_int8(arguments, 5);
  key_ptr                           = sl_cli_get_argument_hex(arguments, 6);
  config.msg_length                 = strlen((const char *)config.msg);
  config.key_config.b0.key_slot     = 0;
  config.iv                         = NULL;
  memcpy(key, key_ptr, strlen((const char *)key_ptr));
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);
  status = sl_si91x_aes(&config, encrypted_buffer_ptr);
  if (status == SL_STATUS_OK) {
    printf("encrypted data:");
    while (encrypted_buffer_ptr[ii]) {
      printf("%02x", encrypted_buffer_ptr[ii]);
      ii++;
    }
  }
  return status;
}

sl_status_t aes_description_command_handler(console_args_t *arguments)
{
  sl_status_t status           = SL_STATUS_OK;
  sl_si91x_aes_config_t config = { 0 };
  uint8_t key[32]              = { 0 };
  uint8_t *key_ptr;
  decrypted_buffer_ptr = malloc(BUFFER_SIZE);
  memset(decrypted_buffer_ptr, '\0', BUFFER_SIZE);
  config.aes_mode                   = sl_cli_get_argument_int8(arguments, 0);
  config.encrypt_decrypt            = sl_cli_get_argument_int8(arguments, 1);
  config.key_config.b0.key_size     = sl_cli_get_argument_int8(arguments, 2);
  config.key_config.b0.wrap_iv_mode = sl_cli_get_argument_int8(arguments, 3);
  config.key_config.b0.key_type     = sl_cli_get_argument_int8(arguments, 4);
  key_ptr                           = sl_cli_get_argument_hex(arguments, 5);
  config.msg_length                 = sl_cli_get_argument_int8(arguments, 6);
  config.msg                        = encrypted_buffer_ptr;
  config.key_config.b0.key_slot     = 0;
  config.iv                         = NULL;
  memcpy(key, key_ptr, strlen((const char *)key_ptr));
  memcpy(config.key_config.b0.key_buffer, &key, config.key_config.b0.key_size);
  status = sl_si91x_aes(&config, decrypted_buffer_ptr);
  if (status == SL_STATUS_OK) {
    printf("decrypted data:");
    printf("%s", decrypted_buffer_ptr);
  }
  free(encrypted_buffer_ptr);
  free(decrypted_buffer_ptr);
  return status;
}

sl_status_t compute_digest_with_sha_algo_command_handler(console_args_t *arguments)
{
  uint8_t ii             = 0;
  sl_status_t status     = SL_STATUS_OK;
  uint8_t digest_out[64] = { 0 };
  uint8_t digest_len     = 0;
  uint8_t sha_type       = sl_cli_get_argument_int8(arguments, 0);
  uint8_t *msg           = sl_cli_get_argument_hex(arguments, 1);
  if (sha_type == SL_SI91x_SHA_1) {
    digest_len = 20;
  } else if (sha_type == SL_SI91x_SHA_256) {
    digest_len = 32;
  } else if (sha_type == SL_SI91x_SHA_384) {
    digest_len = 48;
  } else if (sha_type == SL_SI91x_SHA_512) {
    digest_len = 64;
  } else {
    digest_len = 28;
  }

  status = sl_si91x_sha(sha_type, (uint8_t *)msg, strlen((const char *)msg), digest_out);
  if (status == SL_STATUS_OK) {
    printf("digest:");
    for (ii = 0; ii < digest_len; ii++) {
      printf("%02x", digest_out[ii]);
    }
  }
  return status;
}

sl_status_t compute_digest_with_hmac_command_handler(console_args_t *arguments)
{
  sl_status_t status            = SL_STATUS_OK;
  sl_si91x_hmac_config_t config = { 0 };
  uint8_t ii                    = 0;
  uint8_t digest_len            = 0;
  uint8_t digest_out[64]        = { 0 };

  config.hmac_mode              = sl_cli_get_argument_int8(arguments, 0);
  config.key_config.B0.key_type = sl_cli_get_argument_int8(arguments, 1);
  uint8_t *key_ptr              = (uint8_t *)GET_COMMAND_ARG(arguments, 2);
  config.msg                    = sl_cli_get_argument_hex(arguments, 3);
  config.key_config.B0.key_size = strlen((const char *)key_ptr);
  config.msg_length             = strlen((const char *)config.msg);
  config.key_config.B0.key      = key_ptr;
  if (config.hmac_mode == SL_SI91X_HMAC_SHA_1) {
    digest_len = 20;
  } else if (config.hmac_mode == SL_SI91X_HMAC_SHA_256) {
    digest_len = 32;
  } else if (config.hmac_mode == SL_SI91X_HMAC_SHA_384) {
    digest_len = 48;
  } else {
    digest_len = 64;
  }
  status = sl_si91x_hmac(&config, digest_out);
  if (status == SL_STATUS_OK) {
    printf("digest:");
    for (ii = 0; ii < digest_len; ii++) {
      printf("%02x", digest_out[ii]);
    }
  }
  return status;
}

void convert_string_to_hex(uint8_t *p1_add_sub_x_string, uint8_t *p1_add_sub_x, int length)
{
  int ii       = 0;
  int j        = 0;
  char arg1[3] = { 0 };
  for (ii = 0, j = 0; ii < length; ii++) {
    memcpy(arg1, &p1_add_sub_x_string[ii], 2);
    if ((ii % 2 == 0) && (ii != 0)) {
      j++;
    }
    ii = ii + 1;
    sscanf(arg1, "%02x", (unsigned int *)&p1_add_sub_x[j]);
  }
}

sl_status_t ecdh_point_operation_command_handler(console_args_t *arguments)
{
  sl_status_t status    = SL_STATUS_OK;
  uint8_t ii            = 0;
  uint8_t *p1_add_sub_x = NULL;
  uint8_t *p1_add_sub_y = NULL;
  uint8_t *p1_add_sub_z = NULL;
  uint8_t *p2_add_sub_x = NULL;
  uint8_t *p2_add_sub_y = NULL;
  uint8_t *p2_add_sub_z = NULL;

  uint8_t ecdh_mode            = sl_cli_get_argument_int8(arguments, 0);
  uint8_t ecdh_operation       = sl_cli_get_argument_int8(arguments, 1);
  uint8_t *p1_add_sub_x_string = GET_OPTIONAL_COMMAND_ARG(arguments, 2, NULL, uint8_t *);
  uint8_t *p1_add_sub_y_string = GET_OPTIONAL_COMMAND_ARG(arguments, 3, NULL, uint8_t *);
  uint8_t *p1_add_sub_z_string = GET_OPTIONAL_COMMAND_ARG(arguments, 4, NULL, uint8_t *);
  uint8_t *p2_add_sub_x_string = GET_OPTIONAL_COMMAND_ARG(arguments, 5, NULL, uint8_t *);
  uint8_t *p2_add_sub_y_string = GET_OPTIONAL_COMMAND_ARG(arguments, 6, NULL, uint8_t *);
  uint8_t *p2_add_sub_z_string = GET_OPTIONAL_COMMAND_ARG(arguments, 7, NULL, uint8_t *);
  uint32_t affinity            = GET_OPTIONAL_COMMAND_ARG(arguments, 8, 0, uint32_t);
  uint8_t reverse              = GET_OPTIONAL_COMMAND_ARG(arguments, 9, 0, uint8_t);

  uint16_t msg_len = 0;

  if (p1_add_sub_x_string != NULL) {
    msg_len      = strlen((const char *)p1_add_sub_x_string);
    p1_add_sub_x = malloc(msg_len + 1);
    memset(p1_add_sub_x, '\0', msg_len + 1);
    convert_string_to_hex(p1_add_sub_x_string, p1_add_sub_x, msg_len);
  }

  if (p1_add_sub_y_string != NULL) {
    msg_len      = strlen((const char *)p1_add_sub_y_string);
    p1_add_sub_y = malloc(msg_len + 1);
    memset(p1_add_sub_y, '\0', msg_len + 1);
    convert_string_to_hex(p1_add_sub_y_string, p1_add_sub_y, msg_len);
  }

  if (p1_add_sub_z_string != NULL) {
    msg_len      = strlen((const char *)p1_add_sub_z_string);
    p1_add_sub_z = malloc(msg_len + 1);
    memset(p1_add_sub_z, '\0', msg_len + 1);
    convert_string_to_hex(p1_add_sub_z_string, p1_add_sub_z, msg_len);
  }

  if (p2_add_sub_x_string != NULL) {
    msg_len      = strlen((const char *)p2_add_sub_x_string);
    p2_add_sub_x = malloc(msg_len + 1);
    memset(p2_add_sub_x, '\0', msg_len + 1);
    convert_string_to_hex(p2_add_sub_x_string, p2_add_sub_x, msg_len);
  }

  if (p2_add_sub_y_string != NULL) {
    msg_len      = strlen((const char *)p2_add_sub_y_string);
    p2_add_sub_y = malloc(msg_len + 1);
    memset(p2_add_sub_y, '\0', msg_len + 1);
    convert_string_to_hex(p2_add_sub_y_string, p2_add_sub_y, msg_len);
  }

  if (p2_add_sub_z_string != NULL) {
    msg_len      = strlen((const char *)p2_add_sub_z_string);
    p2_add_sub_z = malloc(msg_len + 1);
    memset(p2_add_sub_z, '\0', msg_len + 1);
    convert_string_to_hex(p2_add_sub_z_string, p2_add_sub_z, msg_len);
  }

  uint8_t *output_x = malloc(SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);
  memset(output_x, '\0', SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);

  uint8_t *output_y = malloc(SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);
  memset(output_y, '\0', SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);

  uint8_t *output_z = malloc(SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);
  memset(output_z, '\0', SL_SI91X_ECDH_MAX_VECTOR_SIZE + 1);

  switch (ecdh_operation) {
    case SL_SI91X_ECDH_ADD: {
      status = sl_si91x_ecdh_point_addition((sl_si91x_ecdh_mode_t)ecdh_mode,
                                            p1_add_sub_x,
                                            p1_add_sub_y,
                                            p1_add_sub_z,
                                            p2_add_sub_x,
                                            p2_add_sub_y,
                                            p2_add_sub_z,
                                            output_x,
                                            output_y,
                                            output_z);

    } break;
    case SL_SI91X_ECDH_SUB: {
      status = sl_si91x_ecdh_point_subtraction((sl_si91x_ecdh_mode_t)ecdh_mode,
                                               p1_add_sub_x,
                                               p1_add_sub_y,
                                               p1_add_sub_z,
                                               p2_add_sub_x,
                                               p2_add_sub_y,
                                               p2_add_sub_z,
                                               output_x,
                                               output_y,
                                               output_z);

    } break;
    case SL_SI91X_ECDH_MUL: {
      status = sl_si91x_ecdh_point_multiplication((sl_si91x_ecdh_mode_t)ecdh_mode,
                                                  p1_add_sub_x,
                                                  p1_add_sub_y,
                                                  p1_add_sub_z,
                                                  p2_add_sub_x,
                                                  affinity,
                                                  output_x,
                                                  output_y,
                                                  output_z,
                                                  reverse);
    } break;
    case SL_SI91X_ECDH_DOUBLE: {
      status = sl_si91x_ecdh_point_double((sl_si91x_ecdh_mode_t)ecdh_mode,
                                          p1_add_sub_x,
                                          p1_add_sub_y,
                                          p1_add_sub_z,
                                          output_x,
                                          output_y,
                                          output_z);
    } break;
    case SL_SI91X_ECDH_AFFINITY: {
      status = sl_si91x_ecdh_point_affine((sl_si91x_ecdh_mode_t)ecdh_mode,
                                          p1_add_sub_x,
                                          p1_add_sub_y,
                                          p1_add_sub_z,
                                          output_x,
                                          output_y,
                                          output_z);
    } break;
    default: {
      printf("ecdh operation not supported\n");
      return SL_STATUS_NOT_SUPPORTED;
    }
  }

  if (status == SL_STATUS_OK) {
    printf("output_x:");
    for (ii = 0; ii < SL_SI91X_ECDH_MAX_VECTOR_SIZE; ii++) {
      printf("%02x", output_x[ii]);
    }
    printf("\noutput_y:");
    for (ii = 0; ii < SL_SI91X_ECDH_MAX_VECTOR_SIZE; ii++) {
      printf("%02x", output_y[ii]);
    }
    printf("\noutput_z:");
    for (ii = 0; ii < SL_SI91X_ECDH_MAX_VECTOR_SIZE; ii++) {
      printf("%02x", output_z[ii]);
    }
  }
  if (p1_add_sub_x) {
    free(p1_add_sub_x);
  }
  if (p1_add_sub_y) {
    free(p1_add_sub_y);
  }
  if (p1_add_sub_z) {
    free(p1_add_sub_z);
  }
  if (p2_add_sub_x) {
    free(p2_add_sub_x);
  }
  if (p2_add_sub_y) {
    free(p2_add_sub_y);
  }
  if (p2_add_sub_z) {
    free(p2_add_sub_z);
  }
  free(output_x);
  free(output_y);
  free(output_z);

  return status;
}