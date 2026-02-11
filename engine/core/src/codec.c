/**
 * Codec registry implementation + store (no-op) codec.
 */
#include <entropy8/codec.h>
#include <string.h>
#include <ctype.h>

static const E8CodecVtable *g_codecs[E8_CODEC_COUNT] = {0};
static int g_initialized = 0;

/* ── Store codec (no compression) ─────────────────────────────────────────── */
static size_t store_compress(const void *src, size_t src_size,
                             void *dst, size_t dst_cap, int level) {
	(void)level;
	if (dst_cap < src_size) return 0;
	memcpy(dst, src, src_size);
	return src_size;
}

static size_t store_decompress(const void *src, size_t src_size,
                               void *dst, size_t dst_cap) {
	if (dst_cap < src_size) return 0;
	memcpy(dst, src, src_size);
	return src_size;
}

static size_t store_bound(size_t src_size) { return src_size; }

static const E8CodecVtable g_store_vtable = {
	E8_CODEC_STORE, "store", store_compress, store_decompress, store_bound
};

/* ── External init functions (implemented in lz4.c, lzma.c, zstd.c) ────── */
extern void e8_codec_lz4_init(void);
extern void e8_codec_lzma_init(void);
extern void e8_codec_zstd_init(void);

/* ── Public API ───────────────────────────────────────────────────────────── */
int e8_codec_register(const E8CodecVtable *vtable) {
	if (!vtable || vtable->id < 0 || vtable->id >= E8_CODEC_COUNT) return -1;
	g_codecs[vtable->id] = vtable;
	return 0;
}

const E8CodecVtable *e8_codec_get(enum E8Codec id) {
	if (!g_initialized) e8_codecs_init();
	if (id < 0 || id >= E8_CODEC_COUNT) return NULL;
	return g_codecs[id];
}

const E8CodecVtable *e8_codec_find(const char *name) {
	if (!g_initialized) e8_codecs_init();
	if (!name) return NULL;
	for (int i = 0; i < E8_CODEC_COUNT; ++i) {
		if (!g_codecs[i]) continue;
		const char *a = g_codecs[i]->name;
		const char *b = name;
		int match = 1;
		for (; *a && *b; ++a, ++b) {
			if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) { match = 0; break; }
		}
		if (match && !*a && !*b) return g_codecs[i];
	}
	return NULL;
}

void e8_codecs_init(void) {
	if (g_initialized) return;
	g_initialized = 1;
	e8_codec_register(&g_store_vtable);
	e8_codec_lz4_init();
	e8_codec_lzma_init();
	e8_codec_zstd_init();
}
