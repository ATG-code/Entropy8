/**
 * Low-level I/O API (C) – ABI-stable, for Python binding and streaming.
 * All stream operations go through these callbacks; no FILE* in the public API.
 */
#ifndef ENTROPY8_IO_H
#define ENTROPY8_IO_H

#include "config.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct E8Stream E8Stream;

/**
 * Read up to size bytes into buf. Returns number of bytes read, 0 on EOF, <0 on error.
 */
typedef ptrdiff_t (*E8ReadFn)(void *ctx, void *buf, size_t size);

/**
 * Write size bytes from buf. Returns number of bytes written, <0 on error.
 */
typedef ptrdiff_t (*E8WriteFn)(void *ctx, const void *buf, size_t size);

/**
 * Seek to offset from origin. origin: 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END.
 * Returns new position in bytes, or <0 on error.
 */
typedef int64_t (*E8SeekFn)(void *ctx, int64_t offset, int origin);

/**
 * Optional flush. Return 0 on success, <0 on error.
 */
typedef int (*E8FlushFn)(void *ctx);

/**
 * Optional close. Called when stream is released. Return 0 on success.
 */
typedef int (*E8CloseFn)(void *ctx);

typedef struct E8StreamVtable {
	E8ReadFn  read;
	E8WriteFn write;
	E8SeekFn  seek;
	E8FlushFn flush;  /* may be NULL */
	E8CloseFn close;  /* may be NULL */
} E8StreamVtable;

/**
 * Opaque stream handle. Create with e8_stream_create, release with e8_stream_destroy.
 */
struct E8Stream {
	const E8StreamVtable *vtable;
	void *ctx;
};

/**
 * Create a stream from callbacks. ctx is passed to all callbacks; not owned by E8Stream.
 * Returns 0 on success, <0 on error (e.g. invalid vtable).
 */
ENTROPY8_API int e8_stream_create(E8Stream *out, const E8StreamVtable *vtable, void *ctx);

/**
 * Call close callback if present and clear stream. Safe to call on zero-initialized stream.
 */
ENTROPY8_API void e8_stream_destroy(E8Stream *stream);

ENTROPY8_API ptrdiff_t e8_read(E8Stream *stream, void *buf, size_t size);
ENTROPY8_API ptrdiff_t e8_write(E8Stream *stream, const void *buf, size_t size);
ENTROPY8_API int64_t e8_seek(E8Stream *stream, int64_t offset, int origin);
ENTROPY8_API int e8_flush(E8Stream *stream);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_IO_H */
