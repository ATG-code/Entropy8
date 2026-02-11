#ifndef ENTROPY8_ENGINE_ARCHIVE_HPP
#define ENTROPY8_ENGINE_ARCHIVE_HPP

#include "../include/entropy8/io.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace entropy8 {

struct Entry {
	std::string path;
	uint64_t uncompressed_size{0};
	uint64_t data_offset{0};  /* offset in archive stream */
	uint32_t compressed_size{0};
};

/**
 * Archive format (simple):
 * - Magic "E8A1" (4 bytes)
 * - Version 1 (2 bytes)
 * - Reserved (2 bytes)
 * - Num entries (4 bytes)
 * - For each entry: path len (2), path (UTF-8), uncompressed_size (8), data_offset (8), compressed_size (4)
 * - Then raw compressed data blocks
 */
class Archive {
public:
	enum class Mode { Read, Write };

	explicit Archive(E8Stream* stream, Mode mode);
	~Archive();

	Archive(const Archive&) = delete;
	Archive& operator=(const Archive&) = delete;

	/** Write mode: add file from content stream; uses thread pool for compression. */
	int add(const char* path, E8Stream* content_stream,
	        int (*progress)(void*, uint64_t, uint64_t), void* progress_user);

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

	class ThreadPool* pool() { return pool_.get(); }

private:
	E8Stream* stream_;
	Mode mode_;
	std::vector<Entry> entries_;
	uint64_t data_start_{0};  /* offset where first data block begins */
	std::unique_ptr<class ThreadPool> pool_;
};

} // namespace entropy8

#endif
