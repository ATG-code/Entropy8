/**
 * SHA-256 hash (FIPS 180-4).
 * Compact implementation for Entropy8 PBKDF2 key derivation.
 */
#ifndef E8_SHA256_H
#define E8_SHA256_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_BLOCK_SIZE  64
#define SHA256_DIGEST_SIZE 32

typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t  buf[SHA256_BLOCK_SIZE];
} Sha256Ctx;

void sha256_init(Sha256Ctx *ctx);
void sha256_update(Sha256Ctx *ctx, const void *data, size_t len);
void sha256_final(Sha256Ctx *ctx, uint8_t digest[SHA256_DIGEST_SIZE]);

/** One-shot hash. */
void sha256(const void *data, size_t len, uint8_t digest[SHA256_DIGEST_SIZE]);

/** HMAC-SHA256. */
void hmac_sha256(const void *key, size_t key_len,
                 const void *data, size_t data_len,
                 uint8_t out[SHA256_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* E8_SHA256_H */
