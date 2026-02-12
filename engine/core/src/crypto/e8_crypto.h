/**
 * Entropy8 high-level encryption API.
 * PBKDF2-SHA256 key derivation + AES-256-CBC encrypt/decrypt.
 */
#ifndef E8_CRYPTO_H
#define E8_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define E8_SALT_SIZE 16
#define E8_IV_SIZE   16
#define E8_KEY_SIZE  32
#define E8_PBKDF2_ITERATIONS 100000

/**
 * Derive a 32-byte key from password + salt using PBKDF2-HMAC-SHA256.
 */
void e8_derive_key(const char *password, size_t password_len,
                   const uint8_t salt[E8_SALT_SIZE],
                   uint8_t key_out[E8_KEY_SIZE]);

/**
 * Generate cryptographically random bytes.
 * Uses CryptGenRandom (Windows) or /dev/urandom (Unix).
 */
int e8_random_bytes(uint8_t *buf, size_t len);

/**
 * Encrypt data with AES-256-CBC + PKCS7 padding.
 * Generates random salt and IV, derives key from password.
 * Output layout: [salt 16B][iv 16B][ciphertext...]
 * Returns total output size, or 0 on error.
 * out must have at least (32 + ((in_len / 16) + 1) * 16) bytes.
 */
size_t e8_encrypt(const char *password, size_t password_len,
                  const uint8_t *in, size_t in_len,
                  uint8_t *out);

/**
 * Decrypt data encrypted by e8_encrypt.
 * Input layout: [salt 16B][iv 16B][ciphertext...]
 * Returns plaintext size, or 0 on error.
 */
size_t e8_decrypt(const char *password, size_t password_len,
                  const uint8_t *in, size_t in_len,
                  uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* E8_CRYPTO_H */
