/**
 * Entropy8 public C API – ABI-stable archive create/open/add/extract/list.
 * All I/O is done via E8Stream (io.h); no FILE* or platform-specific types.
 */
#ifndef ENTROPY8_ENTROPY8_H
#define ENTROPY8_ENTROPY8_H

#include "config.h"
#include "io.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct E8Archive E8Archive;
typedef struct E8Entry E8Entry;

enum E8Error {
	E8_OK = 0,
	E8_ERR_IO,
	E8_ERR_FORMAT,
	E8_ERR_MEMORY,
	E8_ERR_INVALID_ARG,
	E8_ERR_NOT_FOUND,
	E8_ERR_UNSUPPORTED,
};

/**
 * Progress callback: current bytes processed, total (0 = unknown). Return 0 to continue, non-zero to abort.
 */
typedef int (*E8ProgressFn)(void *user, uint64_t current, uint64_t total);

/**
 * Create new archive writing to stream. stream must be writable and seekable.
 * If password is non-NULL and non-empty, the archive is AES-256-CBC encrypted.
 * Returns handle or NULL on error; use e8_last_error() for code.
 */
ENTROPY8_API E8Archive *e8_archive_create(E8Stream *stream, const char *password);

/**
 * Open existing archive from stream. stream must be readable and seekable.
 * If the archive is encrypted, password must be provided.
 */
ENTROPY8_API E8Archive *e8_archive_open(E8Stream *stream, const char *password);

/**
 * Release archive and all resources.
 */
ENTROPY8_API void e8_archive_close(E8Archive *archive);

/**
 * Add a file to the archive. path = logical path inside archive; content from content_stream.
 * If progress is non-NULL, called with bytes read. Returns E8_OK or error code.
 */
ENTROPY8_API int e8_archive_add(E8Archive *archive, const char *path, E8Stream *content_stream,
                                E8ProgressFn progress, void *progress_user);

/**
 * Extract entry at index (0-based) to output_stream. progress optional.
 */
ENTROPY8_API int e8_archive_extract(E8Archive *archive, size_t index, E8Stream *output_stream,
                                    E8ProgressFn progress, void *progress_user);

/**
 * Get number of entries in archive.
 */
ENTROPY8_API size_t e8_archive_count(E8Archive *archive);

/**
 * Get entry info at index. Fills path (buffer of path_size) and optional uncompressed size.
 * path may be NULL if path_size is 0. Returns E8_OK or E8_ERR_NOT_FOUND.
 */
ENTROPY8_API int e8_archive_entry(E8Archive *archive, size_t index, char *path, size_t path_size,
                                  uint64_t *uncompressed_size);

/**
 * Last error code (thread-local). Set by create/open/add/extract.
 */
ENTROPY8_API int e8_last_error(void);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_ENTROPY8_H */
