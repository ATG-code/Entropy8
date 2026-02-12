/**
 * AES-256 block cipher + CBC mode implementation.
 * Based on FIPS-197 (AES standard).
 */
#include "aes256.h"
#include <string.h>

/* ─── S-box and inverse S-box ─────────────────────────────────────────────── */

static const uint8_t sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t inv_sbox[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

/* Round constants */
static const uint8_t rcon[11] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

/* ─── Helpers ─────────────────────────────────────────────────────────────── */

static uint32_t get_u32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |  (uint32_t)p[3];
}

static void put_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static uint32_t sub_word(uint32_t w) {
    return ((uint32_t)sbox[(w >> 24) & 0xff] << 24) |
           ((uint32_t)sbox[(w >> 16) & 0xff] << 16) |
           ((uint32_t)sbox[(w >>  8) & 0xff] <<  8) |
           ((uint32_t)sbox[ w        & 0xff]);
}

static uint32_t rot_word(uint32_t w) {
    return (w << 8) | (w >> 24);
}

/* GF(2^8) multiply by 2 */
static uint8_t xtime(uint8_t x) {
    return (uint8_t)((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

/* ─── Key expansion ───────────────────────────────────────────────────────── */

void aes256_init(Aes256Ctx *ctx, const uint8_t key[AES256_KEY_SIZE]) {
    int i;
    /* First 8 words come directly from the key */
    for (i = 0; i < 8; i++) {
        ctx->rk[i] = get_u32(key + 4 * i);
    }
    /* Expand remaining words */
    for (i = 8; i < AES256_EXPKEY_SIZE; i++) {
        uint32_t tmp = ctx->rk[i - 1];
        if (i % 8 == 0) {
            tmp = sub_word(rot_word(tmp)) ^ ((uint32_t)rcon[i / 8] << 24);
        } else if (i % 8 == 4) {
            tmp = sub_word(tmp);
        }
        ctx->rk[i] = ctx->rk[i - 8] ^ tmp;
    }
}

/* ─── Encrypt block ───────────────────────────────────────────────────────── */

void aes256_encrypt_block(const Aes256Ctx *ctx, uint8_t block[AES256_BLOCK_SIZE]) {
    uint8_t s[16];
    int r, i;
    memcpy(s, block, 16);

    /* Initial round key addition */
    for (i = 0; i < 4; i++) {
        uint32_t rk = ctx->rk[i];
        s[4*i]   ^= (uint8_t)(rk >> 24);
        s[4*i+1] ^= (uint8_t)(rk >> 16);
        s[4*i+2] ^= (uint8_t)(rk >> 8);
        s[4*i+3] ^= (uint8_t)(rk);
    }

    for (r = 1; r <= AES256_ROUNDS; r++) {
        uint8_t t[16];

        /* SubBytes */
        for (i = 0; i < 16; i++) s[i] = sbox[s[i]];

        /* ShiftRows */
        t[0]=s[0]; t[1]=s[5]; t[2]=s[10]; t[3]=s[15];
        t[4]=s[4]; t[5]=s[9]; t[6]=s[14]; t[7]=s[3];
        t[8]=s[8]; t[9]=s[13]; t[10]=s[2]; t[11]=s[7];
        t[12]=s[12]; t[13]=s[1]; t[14]=s[6]; t[15]=s[11];
        memcpy(s, t, 16);

        /* MixColumns (skip for last round) */
        if (r < AES256_ROUNDS) {
            for (i = 0; i < 4; i++) {
                uint8_t a = s[4*i], b = s[4*i+1], c = s[4*i+2], d = s[4*i+3];
                uint8_t xa = xtime(a), xb = xtime(b), xc = xtime(c), xd = xtime(d);
                s[4*i]   = xa ^ xb ^ b ^ c ^ d;
                s[4*i+1] = a ^ xb ^ xc ^ c ^ d;
                s[4*i+2] = a ^ b ^ xc ^ xd ^ d;
                s[4*i+3] = xa ^ a ^ b ^ c ^ xd;
            }
        }

        /* AddRoundKey */
        for (i = 0; i < 4; i++) {
            uint32_t rk = ctx->rk[r * 4 + i];
            s[4*i]   ^= (uint8_t)(rk >> 24);
            s[4*i+1] ^= (uint8_t)(rk >> 16);
            s[4*i+2] ^= (uint8_t)(rk >> 8);
            s[4*i+3] ^= (uint8_t)(rk);
        }
    }

    memcpy(block, s, 16);
}

/* ─── Decrypt block ───────────────────────────────────────────────────────── */

void aes256_decrypt_block(const Aes256Ctx *ctx, uint8_t block[AES256_BLOCK_SIZE]) {
    uint8_t s[16];
    int r, i;
    memcpy(s, block, 16);

    /* Initial round key addition (last round key) */
    for (i = 0; i < 4; i++) {
        uint32_t rk = ctx->rk[AES256_ROUNDS * 4 + i];
        s[4*i]   ^= (uint8_t)(rk >> 24);
        s[4*i+1] ^= (uint8_t)(rk >> 16);
        s[4*i+2] ^= (uint8_t)(rk >> 8);
        s[4*i+3] ^= (uint8_t)(rk);
    }

    for (r = AES256_ROUNDS - 1; r >= 0; r--) {
        uint8_t t[16];

        /* InvShiftRows */
        t[0]=s[0]; t[5]=s[1]; t[10]=s[2]; t[15]=s[3];
        t[4]=s[4]; t[9]=s[5]; t[14]=s[6]; t[3]=s[7];
        t[8]=s[8]; t[13]=s[9]; t[2]=s[10]; t[7]=s[11];
        t[12]=s[12]; t[1]=s[13]; t[6]=s[14]; t[11]=s[15];
        memcpy(s, t, 16);

        /* InvSubBytes */
        for (i = 0; i < 16; i++) s[i] = inv_sbox[s[i]];

        /* AddRoundKey */
        for (i = 0; i < 4; i++) {
            uint32_t rk = ctx->rk[r * 4 + i];
            s[4*i]   ^= (uint8_t)(rk >> 24);
            s[4*i+1] ^= (uint8_t)(rk >> 16);
            s[4*i+2] ^= (uint8_t)(rk >> 8);
            s[4*i+3] ^= (uint8_t)(rk);
        }

        /* InvMixColumns (skip for round 0) */
        if (r > 0) {
            for (i = 0; i < 4; i++) {
                uint8_t a = s[4*i], b = s[4*i+1], c = s[4*i+2], d = s[4*i+3];
                /* Multiply by inverse MixColumns matrix: [14,11,13,9] */
                uint8_t xa = xtime(a), x2a = xtime(xa), x3a = xtime(x2a);
                uint8_t xb = xtime(b), x2b = xtime(xb), x3b = xtime(x2b);
                uint8_t xc = xtime(c), x2c = xtime(xc), x3c = xtime(x2c);
                uint8_t xd = xtime(d), x2d = xtime(xd), x3d = xtime(x2d);
                /* 14 = 8+4+2, 11 = 8+2+1, 13 = 8+4+1, 9 = 8+1 */
                s[4*i]   = (x3a ^ x2a ^ xa) ^ (x3b ^ xb ^ b) ^ (x3c ^ x2c ^ c) ^ (x3d ^ d);
                s[4*i+1] = (x3a ^ a) ^ (x3b ^ x2b ^ xb) ^ (x3c ^ xc ^ c) ^ (x3d ^ x2d ^ d);
                s[4*i+2] = (x3a ^ x2a ^ a) ^ (x3b ^ b) ^ (x3c ^ x2c ^ xc) ^ (x3d ^ xd ^ d);
                s[4*i+3] = (x3a ^ xa ^ a) ^ (x3b ^ x2b ^ b) ^ (x3c ^ c) ^ (x3d ^ x2d ^ xd);
            }
        }
    }

    memcpy(block, s, 16);
}

/* ─── CBC encrypt with PKCS7 ─────────────────────────────────────────────── */

size_t aes256_cbc_encrypt(const uint8_t key[AES256_KEY_SIZE],
                          const uint8_t iv[AES256_BLOCK_SIZE],
                          const uint8_t *in, size_t in_len,
                          uint8_t *out) {
    Aes256Ctx ctx;
    uint8_t prev[AES256_BLOCK_SIZE];
    size_t nblocks, i, j, out_len;

    aes256_init(&ctx, key);
    memcpy(prev, iv, AES256_BLOCK_SIZE);

    /* PKCS7 pad: add 1..16 bytes */
    uint8_t pad_val = (uint8_t)(AES256_BLOCK_SIZE - (in_len % AES256_BLOCK_SIZE));
    nblocks = (in_len / AES256_BLOCK_SIZE) + 1;
    out_len = nblocks * AES256_BLOCK_SIZE;

    for (i = 0; i < nblocks; i++) {
        uint8_t block[AES256_BLOCK_SIZE];
        for (j = 0; j < AES256_BLOCK_SIZE; j++) {
            size_t idx = i * AES256_BLOCK_SIZE + j;
            uint8_t byte = (idx < in_len) ? in[idx] : pad_val;
            block[j] = byte ^ prev[j];
        }
        aes256_encrypt_block(&ctx, block);
        memcpy(out + i * AES256_BLOCK_SIZE, block, AES256_BLOCK_SIZE);
        memcpy(prev, block, AES256_BLOCK_SIZE);
    }

    return out_len;
}

/* ─── CBC decrypt with PKCS7 ─────────────────────────────────────────────── */

size_t aes256_cbc_decrypt(const uint8_t key[AES256_KEY_SIZE],
                          const uint8_t iv[AES256_BLOCK_SIZE],
                          const uint8_t *in, size_t in_len,
                          uint8_t *out) {
    Aes256Ctx ctx;
    uint8_t prev[AES256_BLOCK_SIZE];
    size_t nblocks, i, j;

    if (in_len == 0 || in_len % AES256_BLOCK_SIZE != 0) return 0;

    aes256_init(&ctx, key);
    memcpy(prev, iv, AES256_BLOCK_SIZE);
    nblocks = in_len / AES256_BLOCK_SIZE;

    for (i = 0; i < nblocks; i++) {
        uint8_t block[AES256_BLOCK_SIZE];
        memcpy(block, in + i * AES256_BLOCK_SIZE, AES256_BLOCK_SIZE);
        uint8_t cipher_copy[AES256_BLOCK_SIZE];
        memcpy(cipher_copy, block, AES256_BLOCK_SIZE);

        aes256_decrypt_block(&ctx, block);

        for (j = 0; j < AES256_BLOCK_SIZE; j++) {
            out[i * AES256_BLOCK_SIZE + j] = block[j] ^ prev[j];
        }
        memcpy(prev, cipher_copy, AES256_BLOCK_SIZE);
    }

    /* Verify and remove PKCS7 padding */
    uint8_t pad_val = out[in_len - 1];
    if (pad_val == 0 || pad_val > AES256_BLOCK_SIZE) return 0;

    for (i = 0; i < pad_val; i++) {
        if (out[in_len - 1 - i] != pad_val) return 0;
    }

    return in_len - pad_val;
}
