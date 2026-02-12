/**
 * Entropy8 high-level encryption: PBKDF2-SHA256 + AES-256-CBC.
 */
#include "e8_crypto.h"
#include "sha256.h"
#include "aes256.h"
#include <string.h>

#ifdef _WIN32
#  include <windows.h>
#  include <wincrypt.h>
#else
#  include <stdio.h>
#endif

/* ─── PBKDF2-HMAC-SHA256 ─────────────────────────────────────────────────── */

void e8_derive_key(const char *password, size_t password_len,
                   const uint8_t salt[E8_SALT_SIZE],
                   uint8_t key_out[E8_KEY_SIZE]) {
    /*
     * PBKDF2-HMAC-SHA256 with dkLen=32.
     * Since dkLen <= hLen (32), we only need one block (i=1).
     */
    uint8_t U[SHA256_DIGEST_SIZE];
    uint8_t T[SHA256_DIGEST_SIZE];
    int j;
    uint32_t iter;

    /* First iteration: HMAC(password, salt || INT(1)) */
    uint8_t salt_block[E8_SALT_SIZE + 4];
    memcpy(salt_block, salt, E8_SALT_SIZE);
    salt_block[E8_SALT_SIZE]     = 0;
    salt_block[E8_SALT_SIZE + 1] = 0;
    salt_block[E8_SALT_SIZE + 2] = 0;
    salt_block[E8_SALT_SIZE + 3] = 1;  /* block index = 1 (big-endian) */

    hmac_sha256(password, password_len, salt_block, E8_SALT_SIZE + 4, U);
    memcpy(T, U, SHA256_DIGEST_SIZE);

    /* Remaining iterations */
    for (iter = 1; iter < E8_PBKDF2_ITERATIONS; iter++) {
        hmac_sha256(password, password_len, U, SHA256_DIGEST_SIZE, U);
        for (j = 0; j < SHA256_DIGEST_SIZE; j++) {
            T[j] ^= U[j];
        }
    }

    memcpy(key_out, T, E8_KEY_SIZE);
}

/* ─── Random bytes ────────────────────────────────────────────────────────── */

int e8_random_bytes(uint8_t *buf, size_t len) {
#ifdef _WIN32
    HCRYPTPROV prov;
    if (!CryptAcquireContextW(&prov, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
        return -1;
    BOOL ok = CryptGenRandom(prov, (DWORD)len, buf);
    CryptReleaseContext(prov, 0);
    return ok ? 0 : -1;
#else
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return -1;
    size_t n = fread(buf, 1, len, f);
    fclose(f);
    return (n == len) ? 0 : -1;
#endif
}

/* ─── Encrypt ─────────────────────────────────────────────────────────────── */

size_t e8_encrypt(const char *password, size_t password_len,
                  const uint8_t *in, size_t in_len,
                  uint8_t *out) {
    uint8_t salt[E8_SALT_SIZE];
    uint8_t iv[E8_IV_SIZE];
    uint8_t key[E8_KEY_SIZE];

    if (e8_random_bytes(salt, E8_SALT_SIZE) != 0) return 0;
    if (e8_random_bytes(iv, E8_IV_SIZE) != 0) return 0;

    e8_derive_key(password, password_len, salt, key);

    /* Output: salt || iv || ciphertext */
    memcpy(out, salt, E8_SALT_SIZE);
    memcpy(out + E8_SALT_SIZE, iv, E8_IV_SIZE);

    size_t ct_len = aes256_cbc_encrypt(key, iv, in, in_len,
                                       out + E8_SALT_SIZE + E8_IV_SIZE);

    /* Zero sensitive data */
    memset(key, 0, E8_KEY_SIZE);

    return E8_SALT_SIZE + E8_IV_SIZE + ct_len;
}

/* ─── Decrypt ─────────────────────────────────────────────────────────────── */

size_t e8_decrypt(const char *password, size_t password_len,
                  const uint8_t *in, size_t in_len,
                  uint8_t *out) {
    if (in_len < E8_SALT_SIZE + E8_IV_SIZE + AES256_BLOCK_SIZE) return 0;

    const uint8_t *salt = in;
    const uint8_t *iv   = in + E8_SALT_SIZE;
    const uint8_t *ct   = in + E8_SALT_SIZE + E8_IV_SIZE;
    size_t ct_len       = in_len - E8_SALT_SIZE - E8_IV_SIZE;

    uint8_t key[E8_KEY_SIZE];
    e8_derive_key(password, password_len, salt, key);

    size_t pt_len = aes256_cbc_decrypt(key, iv, ct, ct_len, out);

    /* Zero sensitive data */
    memset(key, 0, E8_KEY_SIZE);

    return pt_len;
}
