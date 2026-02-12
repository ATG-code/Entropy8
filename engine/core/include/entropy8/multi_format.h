/**
 * Entropy8 Multi-Format Archive API.
 * Unified C API for creating/extracting archives in multiple formats:
 *   - E8 (native, with optional AES-256 encryption)
 *   - ZIP, TAR, GZIP, BZIP2, XZ, LZIP (via libarchive)
 *   - 7Z (via libarchive, with encryption support)
 *
 * All I/O is file-path based for simplicity at the multi-format level.
 */
#ifndef ENTROPY8_MULTI_FORMAT_H
#define ENTROPY8_MULTI_FORMAT_H

#include "config.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*E8MFProgressFn)(void *user, uint64_t current, uint64_t total);

/**
 * Supported archive formats.
 */
enum E8Format {
    E8_FMT_7Z    = 0,
    E8_FMT_BZIP2 = 1,
    E8_FMT_E8    = 2,
    E8_FMT_GZIP  = 3,
    E8_FMT_LZIP  = 4,
    E8_FMT_TAR   = 5,
    E8_FMT_XZ    = 6,
    E8_FMT_ZIP   = 7,
    E8_FMT_COUNT
};

/**
 * Create a new archive at output_path in the specified format.
 *
 * @param output_path   Path for the new archive file.
 * @param format        One of E8Format values.
 * @param files         Array of file paths to add.
 * @param file_count    Number of files.
 * @param password      Optional password (NULL for no encryption).
 *                      Encryption supported for: E8, 7Z, ZIP.
 * @param level         Compression level (0=store, 1=fast, 2=normal, 3=max).
 * @param progress      Optional progress callback.
 * @param progress_user User data for progress callback.
 * @return 0 on success, negative on error.
 */
ENTROPY8_API int e8_mf_create(const char *output_path, enum E8Format format,
                               const char **files, size_t file_count,
                               const char *password, int level,
                               E8MFProgressFn progress, void *progress_user);

/**
 * Extract all entries from an archive to output_dir.
 *
 * @param archive_path  Path to the archive file.
 * @param output_dir    Directory to extract files into.
 * @param password      Optional password for encrypted archives.
 * @param progress      Optional progress callback.
 * @param progress_user User data for progress callback.
 * @return Number of files extracted on success, negative on error.
 */
ENTROPY8_API int e8_mf_extract(const char *archive_path, const char *output_dir,
                                const char *password,
                                E8MFProgressFn progress, void *progress_user);

/**
 * List entries in an archive.
 *
 * @param archive_path  Path to the archive file.
 * @param password      Optional password for encrypted archives.
 * @param paths         Output array of entry paths (caller frees with e8_mf_free_list).
 * @param sizes         Output array of uncompressed sizes (may be NULL).
 * @param count         Output: number of entries.
 * @return 0 on success, negative on error.
 */
ENTROPY8_API int e8_mf_list(const char *archive_path, const char *password,
                             char ***paths, uint64_t **sizes, size_t *count);

/**
 * Free entry list returned by e8_mf_list.
 */
ENTROPY8_API void e8_mf_free_list(char **paths, uint64_t *sizes, size_t count);

/**
 * Detect the format of an archive from its file extension.
 * Returns E8_FMT_COUNT if unknown.
 */
ENTROPY8_API enum E8Format e8_mf_detect_format(const char *path);

/**
 * Check if a format supports encryption.
 */
ENTROPY8_API int e8_mf_supports_encryption(enum E8Format format);

/**
 * Get file extension for a format (e.g. ".e8", ".7z").
 */
ENTROPY8_API const char *e8_mf_extension(enum E8Format format);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY8_MULTI_FORMAT_H */
