/**
 * Codec registry – compress/decompress interface for pluggable algorithms.
 * Each algorithm (store, lz4, lzma, zstd) registers itself here.
 * The archive stores codec_id per entry so extract knows which decompressor to use.
 */
#ifndef ENTROPY8_CODEC_H
#define ENTROPY8_CODEC_H

#include "config.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum E8Codec {
	E8_CODEC_STORE = 0,  /* no compression */
	E8_CODEC_LZ4   = 1,
	E8_CODEC_LZMA  = 2,
	E8_CODEC_ZSTD  = 3,
	E8_CODEC_COUNT
};

/**
 * Compress src_size bytes from src into dst (pre-allocated, dst_cap bytes).
 * Returns compressed size on success, 0 on failure.
 */
typedef size_t (*E8CompressFn)(const void *src, size_t src_size,
                               void *dst, size_t dst_cap, int level);

/**
 * Decompress src_size bytes from src into dst (pre-allocated, dst_cap bytes).
 * Returns decompressed size on success, 0 on failure.
 */
typedef size_t (*E8DecompressFn)(const void *src, size_t src_size,
                                 void *dst, size_t dst_cap);

/**
 * Returns worst-case (maximum) compressed size for a given input size.
 */
typedef size_t (*E8CompressBoundFn)(size_t src_size);

typedef struct E8CodecVtable {
	enum E8Codec       id;
	const char        *name;       /* "store", "lz4", "lzma", "zstd" */
	E8CompressFn       compress;   /* NULL for store */
	E8DecompressFn     decompress; /* NULL for store */
	E8CompressBoundFn  bound;      /* NULL for store (bound = src_size) */
} E8CodecVtable;

/**
 * Get codec vtable by id. Returns NULL if not registered.
 */
ENTROPY8_API const E8CodecVtable *e8_codec_get(enum E8Codec id);

/**
 * Get codec vtable by name ("lz4", "lzma", "zstd", "store"). Case-insensitive.
 */
ENTROPY8_API const E8CodecVtable *e8_codec_find(const char *name);

/**
 * Register a codec. Called by each algorithm's init function.
 * Returns 0 on success, -1 on error.
 */
ENTROPY8_API int e8_codec_register(const E8CodecVtable *vtable);

/**
 * Initialize all built-in codecs (store, lz4, lzma, zstd).
 * Called once at library load time.
 */
ENTROPY8_API void e8_codecs_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_CODEC_H */
