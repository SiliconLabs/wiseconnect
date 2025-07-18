/*
 * Universally Unique IDentifier (UUID)
 * Copyright (c) 2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EXCLUDE_WPS
#include "includes.h"

#include "common.h"
#include "crypto/sha256.h"
#include "uuid.h"

int uuid_str2bin(const char *str, u8 *bin)
{
	const char *pos;
	u8 *opos;

	pos = str;
	opos = bin;

	if (hexstr2bin(pos, opos, 4))
		return -1;
	pos += 8;
	opos += 4;

	if (*pos++ != '-' || hexstr2bin(pos, opos, 2))
		return -1;
	pos += 4;
	opos += 2;

	if (*pos++ != '-' || hexstr2bin(pos, opos, 2))
		return -1;
	pos += 4;
	opos += 2;

	if (*pos++ != '-' || hexstr2bin(pos, opos, 2))
		return -1;
	pos += 4;
	opos += 2;

	if (*pos++ != '-' || hexstr2bin(pos, opos, 6))
		return -1;

	return 0;
}


int uuid_bin2str(const u8 *bin, char *str, size_t max_len)
{
	int len;
	len = os_snprintf(str, max_len, "%02x%02x%02x%02x-%02x%02x-%02x%02x-"
			  "%02x%02x-%02x%02x%02x%02x%02x%02x",
			  bin[0], bin[1], bin[2], bin[3],
			  bin[4], bin[5], bin[6], bin[7],
			  bin[8], bin[9], bin[10], bin[11],
			  bin[12], bin[13], bin[14], bin[15]);
	if (os_snprintf_error(max_len, len))
		return -1;
	return 0;
}


int is_nil_uuid(const u8 *uuid)
{
	int i;
	for (i = 0; i < UUID_LEN; i++)
		if (uuid[i])
			return 0;
	return 1;
}


int uuid_random(u8 *uuid)
{
	struct os_time t;
	u8 hash[SHA256_MAC_LEN];

	/* Use HMAC-SHA256 and timestamp as context to avoid exposing direct
	 * os_get_random() output in the UUID field. */
	os_get_time(&t);
	if (os_get_random(uuid, UUID_LEN) < 0 ||
	    hmac_sha256(uuid, UUID_LEN, (const u8 *) &t, sizeof(t), hash) < 0)
		return -1;

	os_memcpy(uuid, hash, UUID_LEN);

	/* Version: 4 = random */
	uuid[6] = (4 << 4) | (uuid[6] & 0x0f);

	/* Variant specified in RFC 4122 */
	uuid[8] = 0x80 | (uuid[8] & 0x3f);

	return 0;
}
#endif
