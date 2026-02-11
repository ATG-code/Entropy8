/**
 * C++ implementation of engine_bridge.h – C linkage, uses Archive + ThreadPool.
 */
#include "../include/entropy8/engine_bridge.h"
#include "../include/entropy8/entropy8.h"
#include "archive.hpp"
#include <cstdlib>
#include <memory>

using namespace entropy8;

void *e8_engine_create(E8Stream *stream) {
	if (!stream) {
		e8_set_last_error((int)E8_ERR_INVALID_ARG);
		return nullptr;
	}
	try {
		std::unique_ptr<Archive> ar(new Archive(stream, Archive::Mode::Write));
		return ar.release();
	} catch (...) {
		e8_set_last_error((int)E8_ERR_MEMORY);
		return nullptr;
	}
}

void *e8_engine_open(E8Stream *stream) {
	if (!stream) {
		e8_set_last_error((int)E8_ERR_INVALID_ARG);
		return nullptr;
	}
	try {
		std::unique_ptr<Archive> ar(new Archive(stream, Archive::Mode::Read));
		if (ar->load() != 0) {
			e8_set_last_error((int)E8_ERR_FORMAT);
			return nullptr;
		}
		return ar.release();
	} catch (...) {
		e8_set_last_error((int)E8_ERR_MEMORY);
		return nullptr;
	}
}

void e8_engine_close(void *engine) {
	auto *ar = static_cast<Archive*>(engine);
	if (ar && ar->mode() == Archive::Mode::Write) {
		ar->finalize();
	}
	delete ar;
}

int e8_engine_add(void *engine, const char *path, E8Stream *content_stream,
                  int (*progress)(void *, uint64_t, uint64_t), void *progress_user) {
	auto *ar = static_cast<Archive*>(engine);
	if (!ar) return (int)E8_ERR_INVALID_ARG;
	int r = ar->add(path, content_stream, progress, progress_user);
	if (r == -1) e8_set_last_error((int)E8_ERR_IO);
	if (r == -2) e8_set_last_error((int)E8_ERR_IO);
	return r == 0 ? (int)E8_OK : (int)E8_ERR_IO;
}

int e8_engine_extract(void *engine, size_t index, E8Stream *output_stream,
                      int (*progress)(void *, uint64_t, uint64_t), void *progress_user) {
	auto *ar = static_cast<Archive*>(engine);
	if (!ar) return (int)E8_ERR_INVALID_ARG;
	int r = ar->extract(index, output_stream, progress, progress_user);
	if (r == -1) e8_set_last_error(index >= ar->count() ? (int)E8_ERR_NOT_FOUND : (int)E8_ERR_IO);
	if (r == -2) e8_set_last_error((int)E8_ERR_IO);
	return r == 0 ? (int)E8_OK : (r == -2 ? (int)E8_ERR_IO : (index >= ar->count() ? (int)E8_ERR_NOT_FOUND : (int)E8_ERR_IO));
}

size_t e8_engine_count(void *engine) {
	auto *ar = static_cast<Archive*>(engine);
	return ar ? ar->count() : 0;
}

int e8_engine_entry(void *engine, size_t index, char *path, size_t path_size,
                    uint64_t *uncompressed_size) {
	auto *ar = static_cast<Archive*>(engine);
	if (!ar) return (int)E8_ERR_INVALID_ARG;
	int r = ar->entry(index, path, path_size, uncompressed_size);
	if (r != 0) e8_set_last_error((int)E8_ERR_NOT_FOUND);
	return r == 0 ? (int)E8_OK : (int)E8_ERR_NOT_FOUND;
}
