/**
 * Zstandard codec – good balance of speed and compression ratio.
 */
#include <entropy8/codec.h>
#include <zstd.h>

static size_t zstd_compress(const void *src, size_t src_size,
                            void *dst, size_t dst_cap, int level) {
	int clevel = (level >= 1 && level <= 22) ? level : 3;
	size_t r = ZSTD_compress(dst, dst_cap, src, src_size, clevel);
	return ZSTD_isError(r) ? 0 : r;
}

static size_t zstd_decompress(const void *src, size_t src_size,
                              void *dst, size_t dst_cap) {
	size_t r = ZSTD_decompress(dst, dst_cap, src, src_size);
	return ZSTD_isError(r) ? 0 : r;
}

static size_t zstd_bound(size_t src_size) {
	return ZSTD_compressBound(src_size);
}

static const E8CodecVtable g_zstd_vtable = {
	E8_CODEC_ZSTD, "zstd", zstd_compress, zstd_decompress, zstd_bound
};

void e8_codec_zstd_init(void) {
	e8_codec_register(&g_zstd_vtable);
}
