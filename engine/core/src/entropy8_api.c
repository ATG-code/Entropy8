/**
 * Public C API implementation – thin layer over engine bridge (C++).
 */
#include <entropy8/entropy8.h>
#include <entropy8/engine_bridge.h>
#include <stdlib.h>
#include <string.h>

static int g_e8_last_error;

int e8_last_error(void) {
	return g_e8_last_error;
}

void e8_set_last_error(int code) {
	g_e8_last_error = code;
}

E8Archive *e8_archive_create(E8Stream *stream) {
	if (!stream) {
		g_e8_last_error = E8_ERR_INVALID_ARG;
		return NULL;
	}
	void *engine = e8_engine_create(stream);
	if (!engine) {
		return NULL;
	}
	return (E8Archive *)engine;
}

E8Archive *e8_archive_open(E8Stream *stream) {
	if (!stream) {
		g_e8_last_error = E8_ERR_INVALID_ARG;
		return NULL;
	}
	void *engine = e8_engine_open(stream);
	if (!engine) {
		return NULL;
	}
	return (E8Archive *)engine;
}

void e8_archive_close(E8Archive *archive) {
	if (archive) {
		e8_engine_close((void *)archive);
	}
}

int e8_archive_add(E8Archive *archive, const char *path, E8Stream *content_stream,
                   E8ProgressFn progress, void *progress_user) {
	if (!archive || !path || !content_stream) {
		g_e8_last_error = E8_ERR_INVALID_ARG;
		return E8_ERR_INVALID_ARG;
	}
	int r = e8_engine_add((void *)archive, path, content_stream, progress, progress_user);
	if (r != E8_OK) g_e8_last_error = r;
	return r;
}

int e8_archive_extract(E8Archive *archive, size_t index, E8Stream *output_stream,
                       E8ProgressFn progress, void *progress_user) {
	if (!archive || !output_stream) {
		g_e8_last_error = E8_ERR_INVALID_ARG;
		return E8_ERR_INVALID_ARG;
	}
	int r = e8_engine_extract((void *)archive, index, output_stream, progress, progress_user);
	if (r != E8_OK) g_e8_last_error = r;
	return r;
}

size_t e8_archive_count(E8Archive *archive) {
	if (!archive) return 0;
	return e8_engine_count((void *)archive);
}

int e8_archive_entry(E8Archive *archive, size_t index, char *path, size_t path_size,
                     uint64_t *uncompressed_size) {
	if (!archive) {
		g_e8_last_error = E8_ERR_INVALID_ARG;
		return E8_ERR_INVALID_ARG;
	}
	int r = e8_engine_entry((void *)archive, index, path, path_size, uncompressed_size);
	if (r != E8_OK) g_e8_last_error = r;
	return r;
}
