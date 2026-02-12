#ifndef ENTROPY8_ENGINE_ARCHIVE_HPP
#define ENTROPY8_ENGINE_ARCHIVE_HPP

#include "../include/entropy8/io.h"
#include "../include/entropy8/codec.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace entropy8 {

struct Entry {
	std::string path;
	uint64_t uncompressed_size{0};
	uint64_t data_offset{0};   /* offset in archive stream */
	uint32_t compressed_size{0};
	uint8_t  codec_id{0};      /* E8Codec enum value */
};

/**
 * Archive format v1 (unencrypted):
 *   Magic "E8A1" (4 bytes) | compressed data blocks | directory | dir_size(4)
 *
 * Archive format v2 (encrypted, AES-256-CBC):
 *   Magic "E8AE" (4 bytes) | salt(16) | iv(16) |
 *   encrypted { compressed data blocks | directory | dir_size(4) }
 *
 * The entire payload after the magic+salt+iv is encrypted as a single blob.
 * Key derivation: PBKDF2-HMAC-SHA256 (100k iterations).
 */
class Archive {
public:
	enum class Mode { Read, Write };

	explicit Archive(E8Stream* stream, Mode mode,
	                 const char* password = nullptr);
	~Archive();

	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;

	/** Write mode: add file from content stream with compression. */
	int add(const char* path, E8Stream* content_stream,
	        int (*progress)(void*, uint64_t, uint64_t), void* progress_user,
	        enum E8Codec codec = E8_CODEC_ZSTD, int level = 3);

	/** Read mode: extract entry at index to output stream. */
	int extract(size_t index, E8Stream* output_stream,
	            int (*progress)(void*, uint64_t, uint64_t), void* progress_user);

	size_t count() const { return entries_.size(); }

	int entry(size_t index, char* path, size_t path_size, uint64_t* uncompressed_size) const;

	/** Write mode: finalize and write directory to stream. */
	int finalize();

	/** Read mode: load directory from stream. */
	int load();

	E8Stream* stream() const { return stream_; }
	Mode mode() const { return mode_; }
	bool encrypted() const { return encrypted_; }

	class ThreadPool* pool() { return pool_.get(); }

private:
	E8Stream* stream_;
	Mode mode_;
	bool encrypted_{false};
	std::string password_;
	std::vector<Entry> entries_;
	uint64_t data_start_{0};  /* offset where first data block begins */
	std::unique_ptr<class ThreadPool> pool_;

	/* In-memory buffer used for encrypted archives (write mode) */
	std::vector<uint8_t> mem_buf_;
	size_t mem_pos_{0};
};

} // namespace entropy8

#endif
