/**
 * Internal engine bridge – C-linkage API implemented in C++.
 * Only used by entropy8_api.c; not part of public ABI.
 */
#ifndef ENTROPY8_ENGINE_BRIDGE_H
#define ENTROPY8_ENGINE_BRIDGE_H

#include "io.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct E8Stream E8Stream;

void *e8_engine_create(E8Stream *stream, const char *password);
void *e8_engine_open(E8Stream *stream, const char *password);
void e8_engine_close(void *engine);

int e8_engine_add(void *engine, const char *path, E8Stream *content_stream,
                  int (*progress)(void *, uint64_t, uint64_t), void *progress_user);

int e8_engine_extract(void *engine, size_t index, E8Stream *output_stream,
                      int (*progress)(void *, uint64_t, uint64_t), void *progress_user);

size_t e8_engine_count(void *engine);

int e8_engine_entry(void *engine, size_t index, char *path, size_t path_size,
                    uint64_t *uncompressed_size);

/** Set thread-local last error (implemented in C, called from C++ bridge). */
void e8_set_last_error(int code);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_ENGINE_BRIDGE_H */
