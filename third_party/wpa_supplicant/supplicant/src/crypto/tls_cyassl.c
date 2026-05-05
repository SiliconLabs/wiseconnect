/*
 * SSL/TLS interface functions for CyaSSL TLS case
 * Copyright (c) 2004-2009, Jouni Malinen <j@w1.fi>
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
#include "common.h"
#ifndef EXCLUDE_EAP
#include "includes.h"

#include "tls.h"
#include "tlsv1_common.h"
#include "rsi_wsc.h"
#include "sha256.h"
#include "sha1.h"
/* CyaSSL includes */
#include <cyassl/ssl.h>

#ifdef SSL_PORTING
#include <cyassl/internal.h>
#endif
#include "rsi_global.h"
#include "rsi_wsc_non_rom.h"
#ifdef ROM_PATCH_FRAMEWORK
#include "wrappers_wlan.h"
#include "wrappers_ram_bt_wlan.h"
#endif 

#ifdef SUPPLICANT_PORTING
uint8 reason_code_eap_failure;
#endif

static int tlsRefCount = 0;
#ifdef SSL_PORTING
uint8 *private_cert_start;
uint32 private_cert_length;
uint8 *client_cert1_start;
uint32 client_cert_length;
struct eap_ssl_data;
typedef int (*tlsv1_client_session_ticket_cb)
(void *ctx, const u8 *ticket, size_t len, const u8 *client_random,
 const u8 *server_random, u8 *master_secret);
static int tls_sess_sec_cb(CYASSL *s, void *secret, int *secretSz, void *ctx);
void eap_peer_tls_clear_input_buffer(struct eap_ssl_data *data, int clear);
#endif
int tls_connection_prf(void *tls_ctx, struct tls_connection *conn,
		       const char *label, int server_random_first,
		       u8 *out, size_t out_len);
int tls_connection_get_keys(void *tls_ctx, struct tls_connection *conn,
			    struct tls_keys *keys);
int rsi_get_eap_tls_version(struct tls_connection *conn);
int tls_connection_get_keyblock_size(void *tls_ctx,
				     struct tls_connection *conn);

/* tls input data for CyaSSL Read Callback */
struct tls_in_data {
    const struct wpabuf* in_data;
    size_t consumed;                 /* how many bytes have we used already */
};


/* tls output data for CyaSSL Write Callback */
struct tls_out_data {
    struct wpabuf* out_data;
};


/* cyassl tls_connection */
struct tls_connection {
    CYASSL*             ssl;
    int                 readAlerts;
    int                 writeAlerts;
    int                 failed;
    struct tls_in_data  input;
    struct tls_out_data output;
    char*               subjectMatch;
    char*               altSubjectMatch;
#ifdef SSL_PORTING
    tlsv1_client_session_ticket_cb session_ticket_cb;
    void*               session_ticket_cb_ctx;
    void*               session_ticket;
    int                 session_ticket_len;
#endif
};

int rsi_wolfSSL_get_ssl_version(WOLFSSL* ssl);
static void ResetInData(struct tls_in_data* in, const struct wpabuf* buf)
{
    /* old one not owned by us so don't free */
    in->in_data  = buf;
    in->consumed = 0;
}


static void ResetOutData(struct tls_out_data* out)
{
    /* old one not owned by us so don't free */
    out->out_data = NULL;
}


/* CyaSSL I/O Receive CallBack */
static int CyasslReceiveCb(CYASSL* ssl, char* buf, int sz, void* ctx)
{
    size_t get = sz;
    struct tls_in_data* data = (struct tls_in_data*)ctx;

    if (data == NULL)
        return -1;

#ifdef SSL_PORTING	
  if (data->in_data == NULL)
    return -2; /* WANT_READ */
#endif

    if (get > (wpabuf_len(data->in_data) - data->consumed))
        get =  wpabuf_len(data->in_data) - data->consumed;

    os_memcpy(buf, wpabuf_head(data->in_data) + data->consumed, get);
    data->consumed += get;

#ifdef SSL_PORTING
  if (wpabuf_len(data->in_data) == (data->consumed))
  {
    /* clear wpabuf as the data is copied completely */
    eap_peer_tls_clear_input_buffer(NULL, 1);
    data->in_data = NULL ;
  }
#endif	
    if (get == 0)
        return -2; /* WANT_READ */

    return (int)get;
}


/* CyaSSL I/O Send CallBack */
static int CyasslSendCb(CYASSL* ssl, char* buf, int sz, void* ctx)
{
    struct wpabuf* tmp;
    struct tls_out_data* data = (struct tls_out_data*)ctx;

    if (data == NULL)
        return -1;

    tmp = wpabuf_alloc_copy(buf, sz);
    data->out_data = wpabuf_concat(data->out_data, tmp);

    if (data->out_data == NULL)
            return -1;

    return sz;
}


void* tls_init(const struct tls_config *conf)
{
    CYASSL_CTX* sslCtx;

    if (tlsRefCount == 0) {
        if (CyaSSL_Init() < 0)
            return NULL;
        /* CyaSSL_Debugging_ON(); */
    }

    tlsRefCount++;
    if (rsi_wsc_cb_non_rom.sc_params_non_rom.tls_version == 2)
    {
	    sslCtx = CyaSSL_CTX_new(CyaTLSv1_2_client_method());  /* start as client */
    }
    else if (rsi_wsc_cb_non_rom.sc_params_non_rom.tls_version == 1)
    {
	    sslCtx = CyaSSL_CTX_new(CyaTLSv1_client_method());  /* start as client */
    }
    else
    {
	    rsi_wsc_cb_non_rom.sc_params_non_rom.tls_version = 0 ;
	    sslCtx = CyaSSL_CTX_new(CyaSSLv23_client_method());
    }
    if (sslCtx) {
        CyaSSL_SetIORecv(sslCtx, CyasslReceiveCb);
        CyaSSL_SetIOSend(sslCtx, CyasslSendCb);
#ifdef SSL_PORTING
    CyaSSL_CTX_allow_anon_cipher(sslCtx);
#endif
    }

    return (void *)sslCtx;
}


void tls_deinit(void *ssl_ctx)
{
    CyaSSL_CTX_free((CYASSL_CTX*)ssl_ctx);

    tlsRefCount--;
    if (tlsRefCount == 0) {
        CyaSSL_Cleanup();
    }
}


int tls_get_errors(void *tls_ctx)
{
    /* all but openssl do this */
	return 0;
}

void eap_cipher_suit_selection(char *cipher) {

    if (rsi_wsc_cb.okc & DHE_RSA_AES256_SHA256){
        strcat(cipher, ":DHE-RSA-AES256-SHA256");
    }
    if (rsi_wsc_cb.okc & DHE_RSA_AES128_SHA256){
        strcat(cipher, ":DHE-RSA-AES128-SHA256");
    }
    if (rsi_wsc_cb.okc & DHE_RSA_AES256_SHA){
        strcat(cipher, ":DHE-RSA-AES256-SHA");
    }
    if (rsi_wsc_cb.okc & DHE_RSA_AES128_SHA){
        strcat(cipher, ":DHE-RSA-AES128-SHA");
    }
    if (rsi_wsc_cb.okc & AES256_SHA256){
        strcat(cipher, ":AES256-SHA256");
    }
    if (rsi_wsc_cb.okc & AES128_SHA256){
        strcat(cipher, ":AES128-SHA256");
    }
    if (rsi_wsc_cb.okc & AES256_SHA){
        strcat(cipher, ":AES256-SHA");
    }
    if (rsi_wsc_cb.okc & AES128_SHA){
        strcat(cipher, ":AES128-SHA");
    }
    if (rsi_wsc_cb.okc & RC4_SHA){
        strcat(cipher, ":RC4-SHA");
    }
    if (rsi_wsc_cb.okc & DES_CBC3_SHA){
        strcat(cipher, ":DES-CBC3-SHA");
    }
    if (rsi_wsc_cb.okc & RC4_MD5){
        strcat(cipher, ":RC4-MD5");
    }
    if(strlen(cipher) == 0) {
        strcat(cipher, ":DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:AES256-SHA256:AES128-SHA256:AES256-SHA:AES128-SHA:RC4-SHA:DES-CBC3-SHA:RC4-MD5");
    }
}

struct tls_connection * tls_connection_init(void *tls_ctx)
{
    CYASSL_CTX* sslCtx = (CYASSL_CTX*)tls_ctx;
    struct tls_connection* conn;
    char cipher[170] = {0};

    conn = os_zalloc(sizeof(*conn));
    if (conn == NULL)
        return NULL;
    conn->ssl = CyaSSL_new(sslCtx);
    if (conn->ssl == NULL) {
        os_free(conn);
        return NULL;
    }

#ifdef SSL_PORTING
    conn->session_ticket = NULL;
    conn->session_ticket_len = 0;
    ((CYASSL *)conn->ssl)->nxCtx.buf      = NULL;
    ((CYASSL *)conn->ssl)->nxCtx.pkt_len  = 0;
#endif

    eap_cipher_suit_selection(cipher);

    CyaSSL_SetIOReadCtx(conn->ssl,  &conn->input);
    CyaSSL_SetIOWriteCtx(conn->ssl, &conn->output);
    CyaSSL_set_cipher_list(conn->ssl, (const char*)(cipher+1));

#ifdef FIPS_CODE_ENABLE
    if(rsi_wsc_cb.fips.fips_mode_enable){
      int  status = 0;
      status = CyaSSL_set_cipher_list(conn->ssl, 
          "DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:AES256-SHA256:AES128-SHA256:AES256-SHA:AES128-SHA");
      if (status != SSL_SUCCESS)
      {
        CYASSL_MSG("OBE: Cipher Suite not set");
      }
    }
#endif

    return conn;
}


void tls_connection_deinit(void *tls_ctx, struct tls_connection *conn)
{
    if (conn == NULL)
        return;

    /* parts */
    CyaSSL_free(conn->ssl);
    os_free(conn->subjectMatch);
    os_free(conn->altSubjectMatch);

    /* self */
    os_free(conn);
}


int tls_connection_established(void *tls_ctx, struct tls_connection *conn)
{
    return conn ? CyaSSL_is_init_finished(conn->ssl) : 0;
}


int tls_connection_shutdown(void *tls_ctx, struct tls_connection *conn)
{
  CYASSL_CTX* ctx = (CYASSL_CTX*)tls_ctx;
  char private_key_passwd[82] = {0}; 
  char cipher[170] = {0};

  if (ctx == NULL || conn == NULL)
    return -1;

  /* Set quiet as OpenSSL does */
  CyaSSL_set_quiet_shutdown(conn->ssl, 1);
  CyaSSL_shutdown(conn->ssl);

  /* free old session, create new for use with reconnection */
  CyaSSL_free(conn->ssl);
  conn->ssl = CyaSSL_new(ctx);
  if (conn->ssl == NULL) {
    wpa_printf(MSG_INFO, "SSL: failed to create new SSL object for reconnect");
    return -1;
  }

  ((CYASSL *)conn->ssl)->nxCtx.buf      = NULL;
  ((CYASSL *)conn->ssl)->nxCtx.pkt_len  = 0;

  CyaSSL_SetIOReadCtx(conn->ssl,  &conn->input);
  CyaSSL_SetIOWriteCtx(conn->ssl, &conn->output);

  eap_cipher_suit_selection(cipher);
  
  CyaSSL_set_cipher_list(conn->ssl, (const char*)(cipher+1));

#ifdef FIPS_CODE_ENABLE
  if(rsi_wsc_cb.fips.fips_mode_enable){
    int  status = 0;
    status = CyaSSL_set_cipher_list(conn->ssl, 
        "DHE-RSA-AES256-SHA256:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:AES256-SHA256:AES128-SHA256:AES256-SHA:AES128-SHA");
    if (status != SSL_SUCCESS)
    {
      CYASSL_MSG("OBE: Cipher Suite not set");
    }
  }
#endif

#ifdef SSL_PORTING

  uint8 eap_cert_type = 1;

  if(*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_pem_file_pattern())
  {
    eap_cert_type = 0;
  }

  if(((eap_cert_type == 0) && (*(uint32 *)cpf_get_eap_cert_len_addr() != 0xFFFFFFFF)) || \
      ((eap_cert_type == 1) && (*(uint32 *)cpf_get_eap_public_key_len_addr() != 0xFFFFFFFF) && (*(uint32 *)cpf_get_eap_priv_key_len_addr() != 0xFFFFFFFF) && (*(uint32 *)cpf_get_eap_ca_cert_len_addr() != 0xFFFFFFFF)))
  {
    if (CyaSSL_use_certificate_buffer(conn->ssl, client_cert1_start, client_cert_length, SSL_FILETYPE_PEM) < 0) 
    {
      wpa_printf(MSG_INFO, "SSL: use client cert der blob failed");
      return -1;
    }
    else
    {
      wpa_printf(MSG_DEBUG, "SSL: use client cert OK");
    }
	
    if (strlen((char *)rsi_wsc_cb.sc_params.private_key_passwd) != 0)
    {
	  memcpy(private_key_passwd, rsi_wsc_cb.sc_params.private_key_passwd + 1, (strlen((char *)rsi_wsc_cb.sc_params.private_key_passwd) - 2));
	  CyaSSL_CTX_set_default_passwd_cb_userdata(conn->ssl->ctx, private_key_passwd);
    }

    if (CyaSSL_use_PrivateKey_buffer(conn->ssl, private_cert_start, private_cert_length, SSL_FILETYPE_PEM) < 0) {
      wpa_printf(MSG_INFO, "SSL: use private der blob failed");
      return -1;
    }
    else {
      wpa_printf(MSG_DEBUG, "SSL: use private key blob OK");
    }
  }

  
  /* workaround for PEAP & TTLS without certificate */
  if(
      (sl_strcmp((uint8 *)rsi_wsc_cb.sc_params.eap_method, (uint8 *)"PEAP") == 0) || 
      (sl_strcmp((uint8 *)rsi_wsc_cb.sc_params.eap_method, (uint8 *)"TTLS") == 0)
    )
  {
    if (((eap_cert_type == 0) && (*(uint32 *)cpf_get_eap_cert_len_addr() == 0xFFFFFFFF))|| 
        ((eap_cert_type == 1) && (*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) != cpf_get_ca_key_pattern())))
    {
      conn->ssl->options.verifyNone = 1;
    }
  }
#endif


  return 0;
}

int tls_connection_get_random(void *ssl_ctx, struct tls_connection *conn, struct tls_random *keys)
{
#if 0  
	keys->client_random = conn->client_random;
	keys->server_random = conn->server_random;
	keys->client_random_len = RAN_LEN;
	keys->server_random_len = RAN_LEN;
	*sr = ssl->arrays->serverRandom;
	*cr = ssl->arrays->clientRandom;

	*srLen = RAN_LEN;                                                                                                                                                                                      *crLen = RAN_LEN;    
	os_memset(keys, 0, sizeof(*keys));
	keys->client_random = conn->client_random;
	keys->client_random_len = SSL_get_client_random(
			ssl, conn->client_random, sizeof(conn->client_random));
	keys->server_random = conn->server_random;
	keys->server_random_len = SSL_get_server_random(
			ssl, conn->server_random, sizeof(conn->server_random));

#endif
    keys->client_random = conn->ssl->arrays->clientRandom;
	keys->server_random = conn->ssl->arrays->serverRandom;
 	keys->client_random_len = RAN_LEN;
 	keys->server_random_len = RAN_LEN;
	return 0;
}                  

int tls_connection_export_key(void *ssl_ctx, struct tls_connection *conn,
		const char *label, const u8 *context,
		size_t context_len, u8 *out, size_t len)
{
#ifndef CONFIG_FIPS
	struct tls_keys keys;
#endif /* CONFIG_FIPS */
	u8 *rnd = NULL;

	/* First, try to use TLS library function for PRF, if available. */
	if (tls_connection_prf(ssl_ctx, conn, label, 0, out, len) // commented for debug purpose
			== 0)
		return 0;

#ifndef CONFIG_FIPS
	/*
	 * 	 * TLS library did not support key generation, so get the needed TLS
	 * 	 	 * session parameters and use an internal implementation of TLS PRF to
	 * 	 	 	 * derive the key.
	 * 	 	 	 	 */

	if (tls_connection_get_keys(ssl_ctx, conn, &keys))
		goto fail;

	if (keys.client_random == NULL || keys.server_random == NULL ||
			keys.master_key == NULL)
		goto fail;

	rnd = os_malloc(keys.client_random_len + keys.server_random_len);
	if (rnd == NULL)
		goto fail;
	os_memcpy(rnd, keys.client_random, keys.client_random_len);
	os_memcpy(rnd + keys.client_random_len, keys.server_random,
			keys.server_random_len);
	if (rsi_get_eap_tls_version(conn) == 3 /* TLSv1_2_MINOR */)
	{
		if (tls_prf_sha256(keys.master_key, keys.master_key_len,
					label, rnd, keys.client_random_len +
					keys.server_random_len, out, len))
			goto fail;
	}
	else
	{
		if (tls_prf_sha1_md5(keys.master_key, keys.master_key_len,
					label, rnd, keys.client_random_len +
					keys.server_random_len, out, len))
			goto fail;
	}
	os_free(rnd);


	return 0;

fail:


#endif /* CONFIG_FIPS */
	os_free(out);
	os_free(rnd);
	return -1;

}

int tls_connection_get_eap_fast_key(void *ssl_ctx, struct tls_connection *conn, u8 *out, size_t len)
{
	struct tls_keys keys;
	u8 *rnd = NULL, *_out;
	int block_size;
    char label[] = "key expansion";

	block_size = tls_connection_get_keyblock_size(ssl_ctx, conn);
	if (block_size < 0)
		return -1;

	_out = os_malloc(block_size + len);
	if (_out == NULL)
		return -1;

	if (tls_connection_prf(ssl_ctx, conn, (const char *)label, 1, _out, block_size + len)
			== 0) {
		os_memmove(out, _out + block_size, len);
		return 0;
	}

	if (tls_connection_get_keys(ssl_ctx, conn, &keys))
		goto fail;

	rnd = os_malloc(keys.client_random_len + keys.server_random_len);
	if (rnd == NULL)
		goto fail;

	os_memcpy(rnd, keys.server_random, keys.server_random_len);
	os_memcpy(rnd + keys.server_random_len, keys.client_random,
			keys.client_random_len);

	wpa_hexdump_key(MSG_MSGDUMP, "EAP-FAST: master_secret for key "
			"expansion", keys.master_key, keys.master_key_len);
	if (rsi_get_eap_tls_version(conn) == 3 /* TLSv1_2_MINOR */)
	{
		if (tls_prf_sha256(keys.master_key, keys.master_key_len,
					(const char *)label, rnd, keys.client_random_len +
					keys.server_random_len, _out, block_size + len))
			goto fail;
	}
	else
	{
		if (tls_prf_sha1_md5(keys.master_key, keys.master_key_len,
					(const char *)label, rnd, keys.client_random_len +
					keys.server_random_len, _out, block_size + len))
			goto fail;
	}
	os_free(rnd);
	os_memmove(out, _out + block_size, len);
	return 0;

fail:
	os_free(rnd);
	os_free(_out);
	return -1;

}

static int tls_connection_set_subject_match(struct tls_connection *conn,
                                            const char *subject_match,
                                            const char *altsubject_match)
{
	os_free(conn->subjectMatch);
	conn->subjectMatch = NULL;
	if (subject_match) {
		conn->subjectMatch = os_strdup(subject_match);
		if (conn->subjectMatch == NULL)
			return -1;
	}

	os_free(conn->altSubjectMatch);
	conn->altSubjectMatch = NULL;
	if (altsubject_match) {
		conn->altSubjectMatch = os_strdup(altsubject_match);
		if (conn->altSubjectMatch == NULL)
			return -1;
	}

	return 0;
}


static int tls_connection_dh(struct tls_connection* conn, const char* dh_file,
                             const u8* dh_blob, size_t blob_len)
{
    if (dh_file == NULL && dh_blob == NULL)
        return 0;
#ifndef SSL_PORTING
    if (dh_blob) {
        if (CyaSSL_SetTmpDH_buffer(conn->ssl, dh_blob, blob_len,
                                   SSL_FILETYPE_ASN1) < 0) {
            wpa_printf(MSG_INFO, "SSL: use dh der blob failed");
            return -1;
        }
        wpa_printf(MSG_DEBUG, "SSL: use dh blob OK");
        return 0;
    }

    if (dh_file) {
        if (CyaSSL_SetTmpDH_file(conn->ssl, dh_file, SSL_FILETYPE_PEM) < 0) {
            wpa_printf(MSG_INFO, "SSL: use dh pem file failed");
            if (CyaSSL_SetTmpDH_file(conn->ssl, dh_file,
                                            SSL_FILETYPE_ASN1) < 0) {
                wpa_printf(MSG_INFO, "SSL: use dh der file failed");
                return -1;
            }
        }
        wpa_printf(MSG_DEBUG, "SSL: use dh file OK");
        return 0;
    }
#endif

    return 0;
}


static int tls_connection_client_cert(struct tls_connection* conn,
                                      const char* client_cert,
                                      const u8*   client_cert_blob,
                                      size_t      blob_len)
{
    if (client_cert == NULL && client_cert_blob == NULL)
        return 0;

    if (client_cert_blob) {
        if (CyaSSL_use_certificate_buffer(conn->ssl, client_cert_blob, blob_len,
#ifdef SSL_PORTING
                                          SSL_FILETYPE_PEM) < 0) 
#else
										  SSL_FILETYPE_ASN1) < 0) 
#endif
                      {
            wpa_printf(MSG_INFO, "SSL: use client cert der blob failed");
            return -1;
        }
        wpa_printf(MSG_DEBUG, "SSL: use client cert blob OK");
        return 0;
    }

#ifndef SSL_PORTING
    if (client_cert) {
        if (CyaSSL_use_certificate_file(conn->ssl, client_cert,
                                        SSL_FILETYPE_PEM) < 0) {
            wpa_printf(MSG_INFO, "SSL: use client cert pem file failed");
            if (CyaSSL_use_certificate_file(conn->ssl, client_cert,
                                            SSL_FILETYPE_ASN1) < 0) {
                wpa_printf(MSG_INFO, "SSL: use client cert der file failed");
                return -1;
            }
        }
        wpa_printf(MSG_DEBUG, "SSL: use client cert file OK");
        return 0;
    }
#endif

    return 0;
}
#endif

#ifdef RSI_PUF_PRIVATE
int tls_passwd_cb(char *buf, int size, int rwflag, void *password)
#else
static int tls_passwd_cb(char *buf, int size, int rwflag, void *password)
#endif
{
	if (password == NULL) {
		return 0;
	}
	os_strlcpy(buf, (char *) password, size);
	return os_strlen(buf);
}

#ifndef EXCLUDE_EAP
static int tls_connection_private_key(void* tls_ctx,
                                      struct tls_connection *conn,
                                      const char* private_key,
                                      const char* private_key_passwd,
                                      const u8*   private_key_blob,
                                      size_t      blob_len)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)tls_ctx;
    char* passwd = NULL;
    int   ok = 0;

    if (private_key == NULL && private_key_blob == NULL)
        return 0;

    if (private_key_passwd) {
        passwd = os_strdup(private_key_passwd);
        if (passwd == NULL)
            return -1;
    }

    CyaSSL_CTX_set_default_passwd_cb(ctx, tls_passwd_cb);
    CyaSSL_CTX_set_default_passwd_cb_userdata(ctx, passwd);

    if (private_key_blob) {
        if (CyaSSL_use_PrivateKey_buffer(conn->ssl, private_key_blob, blob_len,
#ifdef SSL_PORTING
                                          SSL_FILETYPE_PEM) < 0) {
#else
										  SSL_FILETYPE_ASN1) < 0) {
#endif
            wpa_printf(MSG_INFO, "SSL: use private der blob failed");
        }
        else {
            wpa_printf(MSG_DEBUG, "SSL: use private key blob OK");
            ok = 1;
        }
    }

#ifndef SSL_PORTING
    if (!ok && private_key) {
        if (CyaSSL_use_PrivateKey_file(conn->ssl, private_key,
                                       SSL_FILETYPE_PEM) < 0) {
            wpa_printf(MSG_INFO, "SSL: use private key pem file failed");
            if (CyaSSL_use_PrivateKey_file(conn->ssl, private_key,
                                           SSL_FILETYPE_ASN1) < 0) {
                wpa_printf(MSG_INFO, "SSL: use private key der file failed");
            }
            else
                ok = 1;
        }
        else
            ok = 1;

        if (ok)
            wpa_printf(MSG_DEBUG, "SSL: use private key file OK");
    }
#endif

   // CyaSSL_CTX_set_default_passwd_cb(ctx, NULL);
    os_free(passwd);

    if (!ok)
        return -1;

    return 0;
}


static int tls_connection_ca_cert(void* tls_ctx, struct tls_connection *conn,
                                  const char* ca_cert,
                                  const u8*   ca_cert_blob, size_t blob_len,
                                  const char *ca_path)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)tls_ctx;

    if (ca_cert_blob) {
        if (CyaSSL_CTX_load_verify_buffer(ctx, ca_cert_blob, blob_len, 
#ifdef SSL_PORTING
                                          SSL_FILETYPE_PEM) != SSL_SUCCESS) {
#else
                                          SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
#endif
            wpa_printf(MSG_INFO, "SSL: failed to load ca blob"); 
            return -1;
        }
        wpa_printf(MSG_DEBUG, "SSL: use ca cert blob OK");
        return 0;

    }

#ifndef SSL_PORTING
    if (ca_cert || ca_path) {
        if (CyaSSL_CTX_load_verify_locations(ctx, ca_cert, ca_path)
                                            != SSL_SUCCESS) {
            wpa_printf(MSG_INFO, "SSL: failed to load ca_cert as pem");

            if (ca_cert) {
                if (CyaSSL_CTX_der_load_verify_locations(ctx, ca_cert, 
                                           SSL_FILETYPE_ASN1) != SSL_SUCCESS) {
                    wpa_printf(MSG_INFO, "SSL: failed to load ca_cert as der");
                    return -1;
                }
            }
            else
                return -1;
        }
    }
#endif

    return 0;
}


int tls_connection_set_params(void *tls_ctx, struct tls_connection *conn,
			      const struct tls_connection_params *params)
{
    if (tls_connection_set_subject_match(conn, params->subject_match,
                                               params->altsubject_match) < 0) {
        wpa_printf(MSG_INFO, "Error setting subject match");
        return -1;
    }

#ifdef SSL_PORTING
    uint8 *ca_cert_start  = NULL;
    uint32 ca_cert_length = 0;
    uint8 eap_cert_type = 0;
    uint8 TLS_certificate = 0;

     if((*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) == cpf_get_ca_key_pattern()) || (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_priv_key_pattern()) 
        ||(*(uint32 *)(cpf_get_eap_public_key_len_addr() + 4) == cpf_get_public_key_pattern()) || (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_pem_file_pattern()))
    {
      /* New certificate format */
      eap_cert_type = 1; 
    }
    else
    {
      /* Old certificate format*/
      eap_cert_type = 0;
    }


    if((eap_cert_type == 1)  && ((*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_pem_file_pattern()) ||
         ((*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) == cpf_get_ca_key_pattern()) && 
          (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_priv_key_pattern()) &&
        (*(uint32 *)(cpf_get_eap_public_key_len_addr() + 4) == cpf_get_public_key_pattern()))))
    {
      /* 12K certificate */
      TLS_certificate = 1;
    }
   
    if(((eap_cert_type == 0) && (*(uint32 *)cpf_get_eap_cert_len_addr() != 0xFFFFFFFF)) || \
       ((eap_cert_type == 1) && (TLS_certificate == 1)))
    {
      if(eap_cert_type == 0 || ((eap_cert_type == 1) &&  (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) == cpf_get_pem_file_pattern() )))
      {
        private_cert_start = (uint8 *) strstr((const char *)cpf_get_eap_cert_addr(),"-----BEGIN");
        if(private_cert_start == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        uint8 *private_cert_end  = (uint8 *) strstr((const char *)private_cert_start,"-----END");
        if(private_cert_end == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        uint8 *temp_ptr = (uint8 *) strstr((const char *)private_cert_end+5, "-----");
        if(temp_ptr == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        private_cert_end += (temp_ptr+5 - private_cert_end);
        private_cert_length = (private_cert_end - private_cert_start);

        client_cert1_start = (uint8 *) strstr((const char *)private_cert_end,"-----BEGIN CERTIFICATE-----");
        if(client_cert1_start == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        uint8 *client_cert1_end  = (uint8 *) strstr((const char *)client_cert1_start,"-----END CERTIFICATE-----");
        if(client_cert1_end == NULL)
        {
#ifdef SUPPLICANT_PORTING
        reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        client_cert1_end   += sizeof("-----END CERTIFICATE-----");
        uint8 *client_cert2_start = (uint8 *) strstr((const char *)client_cert1_end,"-----BEGIN CERTIFICATE-----");
        if(client_cert2_start == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        uint8 *client_cert2_end = (uint8 *) strstr((const char *)client_cert2_start,"-----END CERTIFICATE-----");
        if(client_cert2_end == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        client_cert2_end += sizeof("-----END CERTIFICATE-----");
        client_cert_length = (client_cert1_end - client_cert1_start);

        ca_cert_start = (uint8 *) strstr((const char *)client_cert2_end,"-----BEGIN CERTIFICATE-----");
        if(ca_cert_start == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        uint8 *ca_cert_end = (uint8 *) strstr((const char *)ca_cert_start,"-----END CERTIFICATE-----");
        if(ca_cert_end == NULL)
        {
#ifdef SUPPLICANT_PORTING
          reason_code_eap_failure = PEM_ERR;
#endif
          return -1;
        }
        ca_cert_end += sizeof("-----END CERTIFICATE-----");
        ca_cert_length = (ca_cert_end - ca_cert_start);
      }
      else
      {
        //! EAP public key
        client_cert1_start = (uint8 *)cpf_get_eap_public_key_addr(); 
        client_cert_length = *(uint32 *)cpf_get_eap_public_key_len_addr();

        //! EAP private key
        private_cert_start  = (uint8 *)cpf_get_eap_private_key_addr();
        private_cert_length = *(uint32 *)cpf_get_eap_priv_key_len_addr();

        //! EAP CA cert
        ca_cert_start  = (uint8 *)cpf_get_eap_ca_cert_addr();
        ca_cert_length = *(uint32 *)cpf_get_eap_ca_cert_len_addr(); 
      }

#endif
      if (tls_connection_ca_cert(tls_ctx, conn, params->ca_cert,
#ifndef SSL_PORTING
            params->ca_cert_blob, params->ca_cert_blob_len,
#else
            ca_cert_start, ca_cert_length,
#endif
            params->ca_path) < 0) {
        wpa_printf(MSG_INFO, "Error setting ca cert");
        return -1;
      }

      if (tls_connection_client_cert(conn, params->client_cert,
#ifndef SSL_PORTING
            params->client_cert_blob,
            params->client_cert_blob_len
#else
            client_cert1_start,
            client_cert_length
#endif
            ) < 0) {
        wpa_printf(MSG_INFO,"Error setting client cert");
        return -1;
      }

      if (tls_connection_private_key(tls_ctx, conn, params->private_key,
            params->private_key_passwd,
#ifndef SSL_PORTING
            params->private_key_blob,
            params->private_key_blob_len
#else
            private_cert_start,
            private_cert_length
#endif
            ) < 0) {

        wpa_printf(MSG_INFO,"Error setting private key");
        return -1;
      }

      if (tls_connection_dh(conn, params->dh_file, 
#ifdef SSL_PORTING
            (const u8*)cpf_get_eap_cert_addr(), 
            *(long*)(cpf_get_eap_cert_len_addr())) < 0) 
#else
        params->dh_blob,
          params->dh_blob_len) < 0) 
#endif
          {
            wpa_printf(MSG_INFO,"Error setting dh");
            return -1;
          }
#ifdef SSL_PORTING
    }
#endif
    else
    {
      if(sl_strcmp((uint8 *)rsi_wsc_cb.sc_params.eap_method, (uint8 *)"TLS") == 0)
      {		  
	    /* CA cert is not loaded into module */
        if (*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) != cpf_get_ca_key_pattern()) 
        {
#ifdef SUPPLICANT_PORTING
            reason_code_eap_failure = TLS_CA_CERT_NOT_PRESENT;
#endif
        }
	    /* private key is not loaded into module */
        else if (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) != cpf_get_priv_key_pattern()) 
        {
#ifdef SUPPLICANT_PORTING
            reason_code_eap_failure = TLS_PRIV_KEY_NOT_PRESENT;
#endif
        }
        /* client cert is not loaded into module */
            else if(*(uint32 *)(cpf_get_eap_public_key_len_addr() + 4) != cpf_get_public_key_pattern())
        {
#ifdef SUPPLICANT_PORTING
            reason_code_eap_failure = TLS_CLIENT_CERT_NOT_PRESENT;
#endif
        }

        /* PEM file and also CA/client/key file not loaded into module */	
        if ((*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) != cpf_get_pem_file_pattern()) &&
                (*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) != cpf_get_ca_key_pattern()) &&
            (*(uint32 *)(cpf_get_eap_priv_key_len_addr() + 4) != cpf_get_priv_key_pattern()) &&
                (*(uint32 *)(cpf_get_eap_public_key_len_addr() + 4) != cpf_get_public_key_pattern()))
        {
#ifdef SUPPLICANT_PORTING
            reason_code_eap_failure = TLS_NO_CERT_PRESENT;
#endif
        }
            /*If it is TLS without Client/Private/CA cert retrun error*/
            return -1;
     }

      /* workaround for PEAP & TTLS without certificate */
      if(
          (sl_strcmp((uint8 *)rsi_wsc_cb.sc_params.eap_method, (uint8 *)"PEAP") == 0) || 
          (sl_strcmp((uint8 *)rsi_wsc_cb.sc_params.eap_method, (uint8 *)"TTLS") == 0)
        )
      {
        if(((eap_cert_type == 1) && (*(uint32 *)(cpf_get_eap_ca_cert_len_addr() + 4) == cpf_get_ca_key_pattern())))
        {
          //! EAP CA cert
          ca_cert_start  = (uint8 *)cpf_get_eap_ca_cert_addr();
          ca_cert_length = *(uint32 *)cpf_get_eap_ca_cert_len_addr(); 

          if (tls_connection_ca_cert(tls_ctx, conn, params->ca_cert,
#ifndef SSL_PORTING
                params->ca_cert_blob, params->ca_cert_blob_len,
#else
                ca_cert_start, ca_cert_length,
#endif
                params->ca_path) < 0) {
            wpa_printf(MSG_INFO, "Error setting ca cert");
            if ((rsi_wsc_cb.okc & BIT(1)) && (sl_strcmp(rsi_wsc_cb.sc_params.eap_method, (uint8 *)"PEAP") == 0)){
#ifdef SUPPLICANT_PORTING
                reason_code_eap_failure = PEAP_CA_CERT_NOT_PRESENT;
#endif
            }
            return -1;
          }
        }
        else
        {
            if ((rsi_wsc_cb.okc & BIT(1)) && (sl_strcmp(rsi_wsc_cb.sc_params.eap_method, (uint8 *)"PEAP") == 0)){
#ifdef SUPPLICANT_PORTING
                reason_code_eap_failure = PEAP_CA_CERT_NOT_PRESENT;
#endif
                return -1;
            }
          conn->ssl->options.verifyNone = 1;
        }
      }
    }

	return 0;
}


#if UNUSED_FEAT_IN_SUPP_29
static int tls_global_ca_cert(void* ssl_ctx, const char* ca_cert)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)ssl_ctx;

	if (ca_cert) {
		if (CyaSSL_CTX_load_verify_locations(ctx, ca_cert, NULL) != 1)
		{
			wpa_printf(MSG_WARNING, "Failed to load root certificates");
			return -1;
		}

		wpa_printf(MSG_DEBUG, "TLS: Trusted root certificate(s) loaded");
	}

	return 0;
}


static int tls_global_client_cert(void* ssl_ctx, const char* client_cert)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)ssl_ctx;

	if (client_cert == NULL)
		return 0;

	if (CyaSSL_CTX_use_certificate_file(ctx, client_cert,
					 SSL_FILETYPE_ASN1) != SSL_SUCCESS &&
	    CyaSSL_CTX_use_certificate_file(ctx, client_cert,
					 SSL_FILETYPE_PEM) != SSL_SUCCESS) {
		wpa_printf(MSG_INFO, "Failed to load client certificate");
		return -1;
	}

	return 0;
}


static int tls_global_private_key(void* ssl_ctx, const char* private_key,
				                  const char* private_key_passwd)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)ssl_ctx;
	char* passwd = NULL;
    int   ret = 0;

	if (private_key == NULL)
		return 0;

	if (private_key_passwd) {
		passwd = os_strdup(private_key_passwd);
		if (passwd == NULL)
			return -1;
    }

	CyaSSL_CTX_set_default_passwd_cb(ctx, tls_passwd_cb);
	CyaSSL_CTX_set_default_passwd_cb_userdata(ctx, passwd);

	if (CyaSSL_CTX_use_PrivateKey_file(ctx, private_key,
					                SSL_FILETYPE_ASN1) != 1 &&
	    CyaSSL_CTX_use_PrivateKey_file(ctx, private_key,
					SSL_FILETYPE_PEM) != 1 ) {
		wpa_printf(MSG_INFO, "Failed to load private key");
		ret = -1;
	}

	os_free(passwd);
	CyaSSL_CTX_set_default_passwd_cb(ctx, NULL);
	
	return ret;
}


static int tls_global_dh(void* ssl_ctx, const char* dh_file,
                         const u8* dh_blob, size_t blob_len)
{
    CYASSL_CTX* ctx = (CYASSL_CTX*)ssl_ctx;

    if (dh_file == NULL && dh_blob == NULL)
        return 0;

    if (dh_blob) {
        if (CyaSSL_CTX_SetTmpDH_buffer(ctx, dh_blob, blob_len,
                                       SSL_FILETYPE_ASN1) < 0) {
            wpa_printf(MSG_INFO, "SSL: global use dh der blob failed");
            return -1;
        }
        wpa_printf(MSG_DEBUG, "SSL: global use dh blob OK");
        return 0;
    }

    if (dh_file) {
        if (CyaSSL_CTX_SetTmpDH_file(ctx, dh_file, SSL_FILETYPE_PEM) < 0) {
            wpa_printf(MSG_INFO, "SSL: global use dh pem file failed");
            if (CyaSSL_CTX_SetTmpDH_file(ctx, dh_file,
                                            SSL_FILETYPE_ASN1) < 0) {
                wpa_printf(MSG_INFO, "SSL: global use dh der file failed");
                return -1;
            }
        }
        wpa_printf(MSG_DEBUG, "SSL: global use dh file OK");
        return 0;
    }

    return 0;
}
#endif


int tls_global_set_params(void* tls_ctx,
			             const struct tls_connection_params* params)
{
#ifndef SSL_PORTING
	if (tls_global_ca_cert(tls_ctx, params->ca_cert) < 0) {
		wpa_printf(MSG_INFO, "SSL: Failed to load ca cert file '%s'",
			                  params->ca_cert);
		return -1;
    }

	if (tls_global_client_cert(tls_ctx, params->client_cert) < 0) {
		wpa_printf(MSG_INFO, "SSL: Failed to load client cert file '%s'",
			                  params->client_cert);
		return -1;
    }

	if (tls_global_private_key(tls_ctx, params->private_key,
				               params->private_key_passwd) < 0) {
		wpa_printf(MSG_INFO, "SSL: Failed to load private key file '%s'",
			                  params->private_key);
		return -1;
    }

	if (tls_global_dh(tls_ctx, params->dh_file, params->dh_blob,
                      params->dh_blob_len) < 0) {
		wpa_printf(MSG_INFO, "SSL: Failed to load DH file '%s'",
			                  params->dh_file);
		return -1;
	}

	return 0;
#else
    return -1;
#endif
}

int tls_global_set_verify(void *tls_ctx, int check_crl, int strict)
{
    /* CyaSSL internal */
	return 0;
}


int tls_connection_set_verify(void *tls_ctx, struct tls_connection *conn,
		int verify_peer, unsigned int flags,
		const u8 *session_ctx, size_t session_ctx_len)
{
    if (conn == NULL)
	    return -1;

    if (verify_peer)
        CyaSSL_set_verify(conn->ssl, SSL_VERIFY_PEER | 
                                     SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    else
        CyaSSL_set_verify(conn->ssl, SSL_VERIFY_NONE, NULL);

    CyaSSL_set_accept_state(conn->ssl);

    /* do we need to fake a session like OpenSSL does here ? */

    return 0;
}



#if UNUSED_FEAT_IN_SUPP_29
int tls_connection_set_ia(void *tls_ctx, struct tls_connection *conn,
			  int tls_ia)
{
    /* same as OpenSSL */
	return -1;
}
#endif


int tls_connection_get_keys(void *tls_ctx, struct tls_connection *conn,
			    struct tls_keys *keys)
{
#ifdef SSL_PORTING
    int status = 0;
#endif
    if (conn == NULL || conn->ssl == NULL || keys == NULL)
	    return -1;

    os_memset(keys, 0, sizeof(*keys));

#ifdef SSL_PORTING
    status = CyaSSL_get_keys(conn->ssl,
                           (unsigned char**)&keys->master_key,
                           (unsigned int*)&keys->master_key_len,
                           (unsigned char**)&keys->server_random,
                           (unsigned int*)&keys->server_random_len,
                           (unsigned char**)&keys->client_random,
                           (unsigned int*)&keys->client_random_len);
    
    if(status == SSL_SUCCESS)
      return 0;
    else
      return status;
#else
    return CyaSSL_get_keys(conn->ssl,
                           (unsigned char**)&keys->master_key,
                           (unsigned int*)&keys->master_key_len,
                           (unsigned char**)&keys->server_random,
                           (unsigned int*)&keys->server_random_len,
                           (unsigned char**)&keys->client_random,
                           (unsigned int*)&keys->client_random_len);
#endif
}


int tls_connection_prf(void *tls_ctx, struct tls_connection *conn,
		       const char *label, int server_random_first,
		       u8 *out, size_t out_len)
{
    /* same as others */
	return -1;
}

#define ASN_NO_SIGNER_E -188
#ifdef SUPPLICANT_PORTING
#define ASN_SIG_CONFIRM_E -155
#endif

u8 * tlsv1_client_send_alert(struct tls_connection *conn, u8 level,
		u8 description, size_t *out_len)
{                                               
	u8 *alert, *pos, *length;                                                                 
	wpa_printf(MSG_DEBUG, "TLSv1: Send Alert(%d:%d)", level, description);
	*out_len = 0; 
	alert = os_malloc(10);       
	if (alert == NULL)   
		return NULL;  
	pos = alert;   
	/* TLSPlaintext */ 
	/* ContentType type */
	*pos++ = 21;//TLS_CONTENT_TYPE_ALERT;
	/* ProtocolVersion version */
	if (rsi_get_eap_tls_version(conn) == 3 /* TLSv1_2_MINOR */)
	{
		WPA_PUT_BE16(pos,TLS_VERSION_1_2);
	}
	else if (rsi_get_eap_tls_version(conn) == 2 /* TLSv1_1_MINOR */)
	{
		WPA_PUT_BE16(pos,TLS_VERSION_1_1);
	}
	else
	{
		WPA_PUT_BE16(pos,TLS_VERSION_1);
	}

	pos += 2;    
	/* uint16 length (to be filled) */
	length = pos;    
	pos += 2;
	/* opaque fragment[TLSPlaintext.length] */
	/* Alert */
	/* AlertLevel level */
	*pos++ = level;
	/* AlertDescription description */
	*pos++ = description; 
	WPA_PUT_BE16(length, pos - length - 2);  
	*out_len = pos - alert;
	return alert;                  
}


static struct wpabuf* Cyassl_handshake(struct tls_connection *conn,
                                       const struct wpabuf *in_data,
		                               int server)
{
	int res;
    size_t out_len;

    ResetOutData(&conn->output);

	/* Initiate TLS handshake or continue the existing handshake */
	if (server)
		res = CyaSSL_accept(conn->ssl);
	else
		res = CyaSSL_connect(conn->ssl);


	if (res != 1) {
		int err = CyaSSL_get_error(conn->ssl, res);
		if (err == SSL_ERROR_WANT_READ)
			wpa_printf(MSG_DEBUG, "SSL: CyaSSL_connect - want "
				   "more data");
		else if (err == SSL_ERROR_WANT_WRITE)
			wpa_printf(MSG_DEBUG, "SSL: CyaSSL_connect - want to "
				   "write");
		else {
            #if UNUSED_FEAT_IN_SUPP_29
            char msg[80];
            #endif
			wpa_printf(MSG_DEBUG, "SSL: CyaSSL_connect - failed %s",
                       CyaSSL_ERR_error_string(err, msg));
			conn->failed++;
		}
	}
	/* For now, only for UNKONWN_CA alert is sending. If for any other alerts required to send, need to add code here */
	if (conn->ssl->error == ASN_NO_SIGNER_E)
	{
		u8 *resp;
		resp = tlsv1_client_send_alert(conn, TLS_ALERT_LEVEL_FATAL,
			TLS_ALERT_UNKNOWN_CA,
			&out_len);
		if (resp == NULL)
			return NULL;
		conn->output.out_data = wpabuf_alloc_ext_data(resp, out_len); 
		if (conn->output.out_data == NULL) {
			os_free(resp);
			return NULL; 
		}             
	}

#ifdef SUPPLICANT_PORTING
	if (conn->ssl->error == ASN_SIG_CONFIRM_E) {
		SendAlert(conn->ssl, alert_fatal, bad_certificate);
	}
#endif /* SUPPLICANT_PORTING */

	return conn->output.out_data;
}


static struct wpabuf* cyassl_get_appl_data(struct tls_connection *conn,
                                           size_t max_len)
{
	int res;
	struct wpabuf* appl_data = wpabuf_alloc(max_len + 100);

	if (appl_data == NULL)
		return NULL;

	res = CyaSSL_read(conn->ssl, wpabuf_mhead(appl_data),
                                 wpabuf_size(appl_data));
	if (res < 0) {
		int err = CyaSSL_get_error(conn->ssl, res);
		if (err == SSL_ERROR_WANT_READ ||
		    err == SSL_ERROR_WANT_WRITE) {
			wpa_printf(MSG_DEBUG, "SSL: No Application Data "
				   "included");
		} else {
            #if UNUSED_FEAT_IN_SUPP_29
            char msg[80];
            #endif
			wpa_printf(MSG_DEBUG, "Failed to read possible Application Data %s",
                                  CyaSSL_ERR_error_string(err, msg));
        }

		wpabuf_free(appl_data);
		return NULL;
	}

	wpabuf_put(appl_data, res);
	wpa_hexdump_buf_key(MSG_MSGDUMP, "SSL: Application Data in Finished "
			                         "message", appl_data);
	return appl_data;
}




static struct wpabuf* cyassl_connection_handshake(struct tls_connection* conn,
            const struct wpabuf* in_data, struct wpabuf** appl_data, int server)
{
    struct wpabuf* out_data;

    ResetInData(&conn->input, in_data);

    if (appl_data)
        *appl_data = NULL;

    out_data = Cyassl_handshake(conn, in_data, server);
    if (out_data == NULL)
        return NULL;

    if (CyaSSL_is_init_finished(conn->ssl) && appl_data && in_data)
        *appl_data = cyassl_get_appl_data(conn, wpabuf_len(in_data));

    return out_data;
}


struct wpabuf * tls_connection_handshake(void *tls_ctx,
					 struct tls_connection *conn,
					 const struct wpabuf *in_data,
					 struct wpabuf **appl_data)
{
    return cyassl_connection_handshake(conn, in_data, appl_data, 0);
}


struct wpabuf * tls_connection_server_handshake(void *tls_ctx,
						struct tls_connection *conn,
						const struct wpabuf *in_data,
						struct wpabuf **appl_data)
{
    return cyassl_connection_handshake(conn, in_data, appl_data, 1);
}


struct wpabuf * tls_connection_encrypt(void *tls_ctx,
				       struct tls_connection *conn,
				       const struct wpabuf *in_data)
{
	int res;

	if (conn == NULL)
		return NULL;

    ResetOutData(&conn->output);

	res = CyaSSL_write(conn->ssl, wpabuf_head(in_data), wpabuf_len(in_data));
	if (res < 0) {
        #if UNUSED_FEAT_IN_SUPP_29
        int  err = CyaSSL_get_error(conn->ssl, res);
        char msg[80];
        #endif
		wpa_printf(MSG_INFO, "Encryption failed - SSL_write: %s",
                             CyaSSL_ERR_error_string(err, msg));
		return NULL;
	}

    return conn->output.out_data;
}


struct wpabuf * tls_connection_decrypt(void *tls_ctx,
				       struct tls_connection *conn,
				       const struct wpabuf *in_data)
{
	int res;
	struct wpabuf *buf;

    ResetInData(&conn->input, in_data);

	/* Read decrypted data for further processing */
	/*
	 * Even though we try to disable TLS compression, it is possible that
	 * this cannot be done with all TLS libraries. Add extra buffer space
	 * to handle the possibility of the decrypted data being longer than
	 * input data.
	 */
	buf = wpabuf_alloc((wpabuf_len(in_data) + 500) * 3);
	if (buf == NULL)
		return NULL;
	res = CyaSSL_read(conn->ssl, wpabuf_mhead(buf), wpabuf_size(buf));
	if (res < 0) {
		wpa_printf(MSG_INFO, "Decryption failed - SSL_read");
		wpabuf_free(buf);
		return NULL;
	}
	wpabuf_put(buf, res);

	return buf;
}


int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn)
{
    return conn ? CyaSSL_session_reused(conn->ssl) : 0;
}


int tls_connection_set_cipher_list(void *tls_ctx, struct tls_connection *conn,
				   u8 *ciphers)
{
	char buf[128], *pos, *end;
	u8 *c;
	int ret;

	if (conn == NULL || conn->ssl == NULL || ciphers == NULL)
		return -1;

	buf[0] = '\0';
	pos = buf;
	end = pos + sizeof(buf);

	c = ciphers;
	while (*c != TLS_CIPHER_NONE) {
		const char *suite;

		switch (*c) {
		case TLS_CIPHER_RC4_SHA:
			suite = "RC4-SHA";
			break;
		case TLS_CIPHER_AES128_SHA:
			suite = "AES128-SHA";
			break;
		case TLS_CIPHER_RSA_DHE_AES128_SHA:
			suite = "DHE-RSA-AES128-SHA";
			break;
#ifdef SSL_PORTING
    case TLS_CIPHER_ANON_DH_AES128_SHA:
      suite = "ADH-AES128-SHA"; 
      break;
#endif
		default:
			wpa_printf(MSG_DEBUG, "TLS: Unsupported "
				   "cipher selection: %d", *c);
			return -1;
		}
		ret = os_snprintf(pos, end - pos, ":%s", suite);
		if (ret < 0 || ret >= end - pos)
			break;
		pos += ret;

		c++;
	}

	wpa_printf(MSG_DEBUG, "CyaSSL: cipher suites: %s", buf + 1);

	if (CyaSSL_set_cipher_list(conn->ssl, buf + 1) != 1) {
	    wpa_printf(MSG_DEBUG, "Cipher suite configuration failed");
		return -1;
	}

	return 0;
}


int tls_get_cipher(void *tls_ctx, struct tls_connection *conn,
		   char *buf, size_t buflen)
{
    CYASSL_CIPHER* cipher;
    const char*    name;

    if (conn == NULL || conn->ssl == NULL)
        return -1;

    cipher = CyaSSL_get_current_cipher(conn->ssl);
    if (cipher == NULL)
        return -1;

    name = CyaSSL_CIPHER_get_name(cipher);
    if (name == NULL)
        return -1;

    os_strlcpy(buf, name, buflen);
    return 0;
}


int tls_connection_enable_workaround(void *tls_ctx,
				     struct tls_connection *conn)
{
    /* no empty fragments in CyaSSL for now */
	return 0;
}


int tls_connection_client_hello_ext(void *tls_ctx, struct tls_connection *conn,
				    int ext_type, const u8 *data,
				    size_t data_len)
{
    /* internal default for now */
	if (conn == NULL || conn->ssl == NULL || ext_type != 35)
		return -1;

	/* If the data is NULL, the caller is trying to clear the
	 * session ticket. If we aren't using the session ticket
	 * the secret callback needs to be cleared so the default
	 * shared secret processing happens. */
	if (data != NULL && data_len > 0) {
		if (CyaSSL_set_session_secret_cb(conn->ssl, tls_sess_sec_cb, conn) != 1) {
    		wpa_printf(2,"OBE: tls_connection_client_hello_ext() failed to set cb");
			return -1;
		}
	}

  if(CyaSSL_set_SessionTicket(conn->ssl, data, data_len) < 0){
    wpa_printf(2,"OBE: tls_connection_client_hello_ext() failed");
	return -1;
  }
  else{
    wpa_printf(2,"OBE: tls_connection_client_hello_ext()  succeeded");
	return 0;
  }
}


/* START NEW CODE */

#if UNUSED_FEAT_IN_SUPP_29
static int tls_session_ticket_ext_cb(CYASSL *s, const unsigned char *data,
				     int len, void *arg)
{
	struct tls_connection *conn = arg;

	if (conn == NULL || conn->session_ticket_cb == NULL)
		return 0;

	wpa_printf(MSG_DEBUG, "CyaSSL: %s: length=%d", __func__, len);

	os_free(conn->session_ticket);
	conn->session_ticket = NULL;

	wpa_hexdump(MSG_DEBUG, "CyaSSL: ClientHello SessionTicket "
		    "extension", data, len);

	conn->session_ticket = os_malloc(len);
	if (conn->session_ticket == NULL)
		return 0;

	os_memcpy(conn->session_ticket, data, len);
	conn->session_ticket_len = len;

	return 1;
}
#endif


static int tls_sess_sec_cb(CYASSL *s, void *secret, int *secretSz, void *ctx)
{
	struct tls_connection *conn = ctx;
	unsigned char *masterSecret, *serverRandom, *clientRandom;
    unsigned int serverRandomSz, clientRandomSz, masterSecretSz;
	int ret;

	if (conn == NULL || conn->session_ticket_cb == NULL)
		return -1;

	ret = CyaSSL_get_keys(s,
					&masterSecret, &masterSecretSz,
					&serverRandom, &serverRandomSz,
					&clientRandom, &clientRandomSz);
	if (ret != SSL_SUCCESS)
		return -1;

	ret = conn->session_ticket_cb(conn->session_ticket_cb_ctx,
				conn->session_ticket, conn->session_ticket_len,
				clientRandom, serverRandom, secret);

	os_free(conn->session_ticket);
	conn->session_ticket = NULL;

	if (ret <= 0)
		return -1;

	*secretSz = masterSecretSz;

	return 0;
}


int tls_connection_set_session_ticket_cb(void *tls_ctx,
					 struct tls_connection *conn,
					 tls_session_ticket_cb cb,
					 void *ctx)
{
	conn->session_ticket_cb = cb;
	conn->session_ticket_cb_ctx = ctx;

	if (cb) {
#ifndef SSL_PORTING /*FIXME*/
		CyaSSL_set_SessionTicket_cb(conn->ssl, tls_session_ticket_ext_cb, conn);
#endif
	}
	else {
		CyaSSL_set_SessionTicket_cb(conn->ssl, NULL, NULL);
	}

	return 0;
}


/* END NEW CODE */

int tls_connection_get_failed(void *tls_ctx, struct tls_connection *conn)
{
    if (conn == NULL)
        return -1;

    return conn->failed;
}


#if UNUSED_FEAT_IN_SUPP_29
int tls_connection_get_read_alerts(void *tls_ctx, struct tls_connection *conn)
{
    if (conn == NULL)
        return -1;

    return conn->readAlerts;
}


int tls_connection_get_write_alerts(void *tls_ctx,
				    struct tls_connection *conn)
{
    if (conn == NULL)
        return -1;

    return conn->writeAlerts;
}
#endif


int tls_connection_get_keyblock_size(void *tls_ctx,
				     struct tls_connection *conn)
{
    if (conn == NULL || conn->ssl == NULL)
	    return -1;

    return CyaSSL_get_keyblock_size(conn->ssl);
}


#if UNUSED_FEAT_IN_SUPP_29
unsigned int tls_capabilities(void *tls_ctx)
{
    /* same as OpenSSL */
	return 0;
}


struct wpabuf * tls_connection_ia_send_phase_finished(
	void *tls_ctx, struct tls_connection *conn, int final)
{
    /* same as OpenSSL */
	return NULL;
}


int tls_connection_ia_final_phase_finished(void *tls_ctx,
					   struct tls_connection *conn)
{
    /* same as OpenSSL */
	return -1;
}


int tls_connection_ia_permute_inner_secret(void *tls_ctx,
					   struct tls_connection *conn,
					   const u8 *key, size_t key_len)
{
    /* same as OpenSSL */
	return -1;
}
#endif
int rsi_get_eap_tls_version(struct tls_connection *conn)
{                                                                                                                                                                                                          if (conn == NULL || conn->ssl == NULL)
	return -1;
	return rsi_wolfSSL_get_ssl_version(conn->ssl);
}   
#endif
