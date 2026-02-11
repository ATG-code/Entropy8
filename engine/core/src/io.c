/**
 * Low-level I/O implementation (C). Stream vtable dispatch; no C++ here.
 */
#include <entropy8/io.h>
#include <entropy8/config.h>
#include <string.h>

int e8_stream_create(E8Stream *out, const E8StreamVtable *vtable, void *ctx) {
	if (!out || !vtable || !vtable->read || !vtable->write || !vtable->seek) {
		return -1;
	}
	out->vtable = vtable;
	out->ctx = ctx;
	return 0;
}

void e8_stream_destroy(E8Stream *stream) {
	if (!stream) return;
	if (stream->vtable && stream->vtable->close && stream->ctx) {
		stream->vtable->close(stream->ctx);
	}
	stream->vtable = NULL;
	stream->ctx = NULL;
}

ptrdiff_t e8_read(E8Stream *stream, void *buf, size_t size) {
	if (!stream || !stream->vtable || !stream->vtable->read || !buf) return -1;
	return stream->vtable->read(stream->ctx, buf, size);
}

ptrdiff_t e8_write(E8Stream *stream, const void *buf, size_t size) {
	if (!stream || !stream->vtable || !stream->vtable->write || !buf) return -1;
	return stream->vtable->write(stream->ctx, buf, size);
}

int64_t e8_seek(E8Stream *stream, int64_t offset, int origin) {
	if (!stream || !stream->vtable || !stream->vtable->seek) return -1;
	return stream->vtable->seek(stream->ctx, offset, origin);
}

int e8_flush(E8Stream *stream) {
	if (!stream || !stream->vtable) return 0;
	if (stream->vtable->flush && stream->ctx) return stream->vtable->flush(stream->ctx);
	return 0;
}
