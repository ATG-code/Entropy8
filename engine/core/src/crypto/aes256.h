/**
 * AES-256 block cipher + CBC mode.
 * Compact implementation for Entropy8 encryption layer.
 */
#ifndef E8_AES256_H
#define E8_AES256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AES256_KEY_SIZE   32
#define AES256_BLOCK_SIZE 16
#define AES256_ROUNDS     14
#define AES256_EXPKEY_SIZE (4 * (AES256_ROUNDS + 1))  /* 60 uint32_t words */

typedef struct {
    uint32_t rk[AES256_EXPKEY_SIZE];
} Aes256Ctx;

/** Expand 32-byte key into round keys. */
void aes256_init(Aes256Ctx *ctx, const uint8_t key[AES256_KEY_SIZE]);

/** Encrypt a single 16-byte block in-place. */
void aes256_encrypt_block(const Aes256Ctx *ctx, uint8_t block[AES256_BLOCK_SIZE]);

/** Decrypt a single 16-byte block in-place. */
void aes256_decrypt_block(const Aes256Ctx *ctx, uint8_t block[AES256_BLOCK_SIZE]);

/**
 * AES-256-CBC encrypt with PKCS7 padding.
 * out must have space for ((in_len / 16) + 1) * 16 bytes.
 * Returns number of bytes written to out.
 */
size_t aes256_cbc_encrypt(const uint8_t key[AES256_KEY_SIZE],
                          const uint8_t iv[AES256_BLOCK_SIZE],
                          const uint8_t *in, size_t in_len,
                          uint8_t *out);

/**
 * AES-256-CBC decrypt with PKCS7 unpadding.
 * in_len must be a multiple of 16.
 * Returns number of plaintext bytes, or 0 on error (bad padding).
 */
size_t aes256_cbc_decrypt(const uint8_t key[AES256_KEY_SIZE],
                          const uint8_t iv[AES256_BLOCK_SIZE],
                          const uint8_t *in, size_t in_len,
                          uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* E8_AES256_H */
