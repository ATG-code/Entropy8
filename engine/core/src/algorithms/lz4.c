/**
 * LZ4 codec – fast compression, moderate ratio.
 */
#include <entropy8/codec.h>
#include <lz4.h>
#include <lz4hc.h>

static size_t lz4_compress(const void *src, size_t src_size,
                           void *dst, size_t dst_cap, int level) {
	if (src_size > (size_t)LZ4_MAX_INPUT_SIZE) return 0;
	int r;
	if (level > 1) {
		/* LZ4 HC for higher levels (1-12 mapped to LZ4HC levels) */
		int hc_level = level < 12 ? level : 12;
		r = LZ4_compress_HC((const char *)src, (char *)dst,
		                    (int)src_size, (int)dst_cap, hc_level);
	} else {
		r = LZ4_compress_default((const char *)src, (char *)dst,
		                         (int)src_size, (int)dst_cap);
	}
	return r > 0 ? (size_t)r : 0;
}

static size_t lz4_decompress(const void *src, size_t src_size,
                             void *dst, size_t dst_cap) {
	int r = LZ4_decompress_safe((const char *)src, (char *)dst,
	                            (int)src_size, (int)dst_cap);
	return r > 0 ? (size_t)r : 0;
}

static size_t lz4_bound(size_t src_size) {
	return (size_t)LZ4_compressBound((int)src_size);
}

static const E8CodecVtable g_lz4_vtable = {
	E8_CODEC_LZ4, "lz4", lz4_compress, lz4_decompress, lz4_bound
};

void e8_codec_lz4_init(void) {
	e8_codec_register(&g_lz4_vtable);
}
