/*
 * Simultaneous authentication of equals
 * Copyright (c) 2012-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef SAE_H
#define SAE_H

#define SAE_KCK_LEN 32
#define SAE_PMK_LEN 32
#define SAE_PMKID_LEN 16
#define SAE_KEYSEED_KEY_LEN 32
#define SAE_MAX_PRIME_LEN 512
#define SAE_MAX_ECC_PRIME_LEN 66
#define SAE_MAX_HASH_LEN 64
#define SAE_COMMIT_MAX_LEN (2 + 3 * SAE_MAX_PRIME_LEN)
#define SAE_CONFIRM_MAX_LEN (2 + SAE_MAX_PRIME_LEN)

/* Special value returned by sae_parse_commit() */
#define SAE_SILENTLY_DISCARD 65535

struct sae_pk {
	struct wpabuf *m;
	struct crypto_ec_key *key;
	int group;
	struct wpabuf *pubkey; /* DER encoded subjectPublicKey */
#ifdef CONFIG_TESTING_OPTIONS
	struct crypto_ec_key *sign_key_override;
#endif /* CONFIG_TESTING_OPTIONS */
};


struct sae_temporary_data {
	u8 kck[SAE_KCK_LEN];
	size_t kck_len;
	struct crypto_bignum *own_commit_scalar;
	struct crypto_bignum *own_commit_element_ffc;
	struct crypto_ec_point *own_commit_element_ecc;
	struct crypto_bignum *peer_commit_element_ffc;
	struct crypto_ec_point *peer_commit_element_ecc;
	struct crypto_ec_point *pwe_ecc;
	struct crypto_bignum *pwe_ffc;
	struct crypto_bignum *sae_rand;
	struct crypto_ec *ec;
	int prime_len;
	int order_len;
	const struct dh_group *dh;
	const struct crypto_bignum *prime;
	const struct crypto_bignum *order;
	struct crypto_bignum *prime_buf;
	struct crypto_bignum *order_buf;
	struct wpabuf *anti_clogging_token;
	char *pw_id;
	int vlan_id;
	u8 bssid[ETH_ALEN];
	struct wpabuf *own_rejected_groups;
	struct wpabuf *peer_rejected_groups;
	unsigned int own_addr_higher:1;
};

struct sae_pt {
	struct sae_pt *next;
	int group;
	struct crypto_ec *ec;
	struct crypto_ec_point *ecc_pt;

	const struct dh_group *dh;
	struct crypto_bignum *ffc_pt;
#ifdef CONFIG_SAE_PK
	u8 ssid[32];
	size_t ssid_len;
#endif /* CONFIG_SAE_PK */
};

enum sae_state {
	SAE_NOTHING, SAE_COMMITTED, SAE_CONFIRMED, SAE_ACCEPTED
};

struct sae_data {
	enum sae_state state;
	u16 send_confirm;
	u8 pmk[SAE_PMK_LEN];
	u8 pmkid[SAE_PMKID_LEN];
	struct crypto_bignum *peer_commit_scalar;
	struct crypto_bignum *peer_commit_scalar_accepted;
	int group;
	unsigned int sync; /* protocol instance variable: Sync */
	u16 rc; /* protocol instance variable: Rc (received send-confirm) */
	unsigned int h2e:1;
	unsigned int pk:1;
	struct sae_temporary_data *tmp;
};

int sae_set_group(struct sae_data *sae, int group);
void sae_clear_temp_data(struct sae_data *sae);
void sae_clear_data(struct sae_data *sae);

int sae_prepare_commit(const u8 *addr1, const u8 *addr2,
		       const u8 *password, size_t password_len,
		       const char *identifier, struct sae_data *sae);
int sae_prepare_commit_pt(struct sae_data *sae, const struct sae_pt *pt,
			  const u8 *addr1, const u8 *addr2,
			  int *rejected_groups, const struct sae_pk *pk);
int sae_process_commit(struct sae_data *sae);
void sae_write_commit(struct sae_data *sae, struct wpabuf *buf,
		      const struct wpabuf *token, const char *identifier);
u16 sae_parse_commit(struct sae_data *sae, const u8 *data, size_t len,
		     const u8 **token, size_t *token_len, int *allowed_groups,
		     int h2e);
void sae_write_confirm(struct sae_data *sae, struct wpabuf *buf);
int sae_check_confirm(struct sae_data *sae, const u8 *data, size_t len);
u16 sae_group_allowed(struct sae_data *sae, int *allowed_groups, u16 group);
const char * sae_state_txt(enum sae_state state);
void sae_deinit_pt(struct sae_pt *pt);
struct sae_pt * sae_derive_pt(int *groups, const u8 *ssid, size_t ssid_len,
			      const u8 *password, size_t password_len,
			      const char *identifier);

#endif /* SAE_H */
