/**
 * LZMA codec – high compression ratio, slower.
 * Uses liblzma (xz-utils / lzma-sdk).
 */
#include <entropy8/codec.h>
#include <lzma.h>
#include <stdlib.h>
#include <string.h>

static size_t lzma_codec_compress(const void *src, size_t src_size,
                                  void *dst, size_t dst_cap, int level) {
	uint32_t preset = (level >= 0 && level <= 9) ? (uint32_t)level : 6;
	size_t out_pos = 0;
	lzma_ret ret = lzma_easy_buffer_encode(
		preset, LZMA_CHECK_CRC64,
		NULL, /* allocator */
		(const uint8_t *)src, src_size,
		(uint8_t *)dst, &out_pos, dst_cap);
	return (ret == LZMA_OK) ? out_pos : 0;
}

static size_t lzma_codec_decompress(const void *src, size_t src_size,
                                    void *dst, size_t dst_cap) {
	uint64_t memlimit = 128 * 1024 * 1024; /* 128 MB */
	size_t in_pos = 0;
	size_t out_pos = 0;
	lzma_ret ret = lzma_stream_buffer_decode(
		&memlimit, 0, NULL,
		(const uint8_t *)src, &in_pos, src_size,
		(uint8_t *)dst, &out_pos, dst_cap);
	return (ret == LZMA_OK) ? out_pos : 0;
}

static size_t lzma_codec_bound(size_t src_size) {
	/* LZMA worst case: input + ~1KB overhead */
	return src_size + src_size / 3 + 1024;
}

static const E8CodecVtable g_lzma_vtable = {
	E8_CODEC_LZMA, "lzma", lzma_codec_compress, lzma_codec_decompress, lzma_codec_bound
};

void e8_codec_lzma_init(void) {
	e8_codec_register(&g_lzma_vtable);
}
