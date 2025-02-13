#ifndef _CAU2_DEFINES_H
#define _CAU2_DEFINES_H
#include "includes.h"
#include "common.h"
#define		TL		0
#define		TS		0
#define		CASR	0
#define		CAA		1
#define		CA0		2
#define		CA1		3
#define		CA2		4
#define		CA3		5
#define		CA4		6
#define		CA5		7
#define		CA6		8
#define		CA7		9
#define		CA8		10
#define		CNOP	0x000
#define		LDR		0x010
#define		STR		0x020
#define		ADR		0x030
#define		RADR	0x040
#define		ADRA	0x050
#define		XOR		0x060
#define		ROTL	0x070
#define		MVRA	0x080
#define		MVAR	0x090
#define		AESS	0x0a0
#define		AESIS	0x0b0
#define		AESC	0x0c0
#define		AESIC	0x0d0
#define		AESR	0x0e0
#define		AESIR	0x0f0
#define		DESR	0x100
#define		DESK	0x110
#define		HASH	0x120
#define		SHS		0x130
#define		MDS		0x140
#define		SHS2	0x150
#define		ILL		0x1f0
#define		IP		8
#define		FP		4
#define		DC		1
#define		CP		2
#define		KSL1	0
#define		KSL2	1
#define		KSR1	2
#define		KSR2	3
#define		HFF		0
#define		HFG		1
#define		HFH		2
#define		HFI		3
#define		HFP		2
#define		HFC		4
#define		HFM		5
#define		HF2C	6
#define		HF2M	7
#define		HF2S	8
#define		HF2T	9
#define		HF2U	10
#define		HF2V	11

/* SHA1 */
void mmcau_sha1_initialize_output (u32 sha1_state[]);
void mmcau_sha1_hash_n (u32 msg_data[], int num_blks,
                        u32 sha1_state[]);
void mmcau_sha1_update(u32 msg_data[], int num_blks,
                       u32 sha1_state[]);
void mmcau_sha1_hash (u32 msg_data[], u32 sha1_state[]);

#define ENABLE_INTERNAL_AES 1
/* AES */
int mmcau_aes_set_key(u32 key[], int key_size, u32 key_sch[]);
void mmcau_aes_encrypt(u32 in[], u32 key_sch[], int nr,
                       u32 out[]);
void mmcau_aes_decrypt(u32 in[], u32 key_sch[], int nr,
                       u32 out[]);
/* MD5 */
void mmcau_md5_hash (u32 msg_data[], u32 md_state[]);

//#define ENABLE_SHA256_INTERNAL 1
/* SHA256 */
void mmcau_sha256_hash (u32 input[], u32 output[]);

/* DES */
void mmcau_des_encrypt(u32 in[], u32 key[],
                       u32 out[]);
void mmcau_des_decrypt(u32 in[], u32 key[],
                       u32 out[]);
#endif
