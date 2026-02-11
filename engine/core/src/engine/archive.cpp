#include "archive.hpp"
#include "thread_pool.hpp"
#include <cstring>
#include <vector>

namespace entropy8 {

static const uint8_t MAGIC[] = { 'E', '8', 'A', '1' };
static const uint16_t VERSION = 1;
static const size_t BUF_SIZE = 256 * 1024;

Archive::Archive(E8Stream* stream, Mode mode) : stream_(stream), mode_(mode) {
	if (mode == Mode::Write) {
		pool_ = std::make_unique<ThreadPool>(0);
		if (e8_write(stream_, MAGIC, 4) != 4) { /* caller will see failed state */ }
	}
}

Archive::~Archive() = default;

int Archive::add(const char* path, E8Stream* content_stream,
                 int (*progress)(void*, uint64_t, uint64_t), void* progress_user) {
	if (mode_ != Mode::Write || !path || !content_stream) return -1; /* E8_ERR_INVALID_ARG */

	int64_t start = e8_seek(stream_, 0, 1); /* SEEK_CUR = current position */
	if (start < 0) return -1;

	Entry e;
	e.path = path;
	e.data_offset = static_cast<uint64_t>(start);
	e.compressed_size = 0;

	std::vector<uint8_t> buf(BUF_SIZE);
	uint64_t total = 0;

	/* For minimal implementation: store uncompressed (compressed_size = uncompressed_size). */
	while (true) {
		ptrdiff_t n = e8_read(content_stream, buf.data(), buf.size());
		if (n < 0) return -1; /* E8_ERR_IO */
		if (n == 0) break;
		ptrdiff_t w = e8_write(stream_, buf.data(), static_cast<size_t>(n));
		if (w != n) return -1;
		e.uncompressed_size += static_cast<uint64_t>(n);
		total += static_cast<uint64_t>(n);
		if (progress && progress(progress_user, total, 0) != 0) return -2; /* abort */
	}

	e.compressed_size = static_cast<uint32_t>(e.uncompressed_size);
	entries_.push_back(std::move(e));
	return 0; /* E8_OK */
}

int Archive::extract(size_t index, E8Stream* output_stream,
                     int (*progress)(void*, uint64_t, uint64_t), void* progress_user) {
	if (mode_ != Mode::Read || index >= entries_.size() || !output_stream) return -1;

	const Entry& e = entries_[index];
	int64_t pos = e8_seek(stream_, static_cast<int64_t>(e.data_offset), 0);
	if (pos < 0) return -1;

	std::vector<uint8_t> buf(BUF_SIZE);
	uint64_t remaining = e.uncompressed_size;
	uint64_t done = 0;

	while (remaining > 0) {
		size_t to_read = static_cast<size_t>(remaining < buf.size() ? remaining : buf.size());
		ptrdiff_t n = e8_read(stream_, buf.data(), to_read);
		if (n <= 0) return -1;
		ptrdiff_t w = e8_write(output_stream, buf.data(), static_cast<size_t>(n));
		if (w != n) return -1;
		remaining -= static_cast<uint64_t>(n);
		done += static_cast<uint64_t>(n);
		if (progress && progress(progress_user, done, e.uncompressed_size) != 0) return -2;
	}
	return 0;
}

int Archive::entry(size_t index, char* path, size_t path_size, uint64_t* uncompressed_size) const {
	if (index >= entries_.size()) return -1; /* E8_ERR_NOT_FOUND */
	const Entry& e = entries_[index];
	if (uncompressed_size) *uncompressed_size = e.uncompressed_size;
	if (path && path_size > 0) {
		size_t len = e.path.size();
		if (len >= path_size) len = path_size - 1;
		std::memcpy(path, e.path.c_str(), len);
		path[len] = '\0';
	}
	return 0;
}

int Archive::finalize() {
	if (mode_ != Mode::Write) return -1;

	/*
	 * Directory layout (appended after data blocks):
	 *   [num(4)] [entries...] [dir_size(4)]
	 * dir_size = bytes of [num(4)] + [entries...] (excludes trailing dir_size itself).
	 * load() reads dir_size from last 4 bytes, then seeks back to read directory.
	 */
	int64_t dir_start = e8_seek(stream_, 0, 2); /* SEEK_END */
	if (dir_start < 0) return -1;

	uint32_t num = static_cast<uint32_t>(entries_.size());
	if (e8_write(stream_, &num, 4) != 4) return -1;

	for (const Entry& e : entries_) {
		uint16_t plen = static_cast<uint16_t>(e.path.size() > 65535 ? 65535 : e.path.size());
		if (e8_write(stream_, &plen, 2) != 2) return -1;
		if (plen && e8_write(stream_, e.path.data(), plen) != static_cast<ptrdiff_t>(plen)) return -1;
		if (e8_write(stream_, &e.uncompressed_size, 8) != 8) return -1;
		uint64_t off = e.data_offset;
		if (e8_write(stream_, &off, 8) != 8) return -1;
		if (e8_write(stream_, &e.compressed_size, 4) != 4) return -1;
	}

	/* Write dir_size as trailer (last 4 bytes of file) */
	int64_t dir_end = e8_seek(stream_, 0, 2);
	if (dir_end < 0) return -1;
	uint32_t dir_size = static_cast<uint32_t>(dir_end - dir_start);
	if (e8_write(stream_, &dir_size, 4) != 4) return -1;

	/* Flush to ensure all data hits disk before close */
	e8_flush(stream_);
	return 0;
}

int Archive::load() {
	if (mode_ != Mode::Read) return -1;

	/* Verify magic at offset 0 */
	uint8_t mag[4];
	if (e8_seek(stream_, 0, 0) != 0 || e8_read(stream_, mag, 4) != 4) return -1;
	if (std::memcmp(mag, MAGIC, 4) != 0) return -1;

	/* Read dir_size from last 4 bytes (trailer) */
	int64_t file_end = e8_seek(stream_, 0, 2);
	if (file_end < 0 || file_end < 12) return -1; /* magic(4) + dir_size trailer(4) + at least num(4) */
	e8_seek(stream_, file_end - 4, 0);
	uint32_t dir_size;
	if (e8_read(stream_, &dir_size, 4) != 4) return -1;
	if (dir_size == 0 || static_cast<int64_t>(dir_size) > file_end - 8) return -1;

	/* Seek to start of directory: [num(4)][entries...] */
	int64_t dir_start = file_end - 4 - static_cast<int64_t>(dir_size);
	if (dir_start < 4) return -1; /* must be after magic */
	e8_seek(stream_, dir_start, 0);

	uint32_t num;
	if (e8_read(stream_, &num, 4) != 4) return -1;

	entries_.clear();
	entries_.reserve(num);

	for (uint32_t i = 0; i < num; ++i) {
		Entry e;
		uint16_t plen;
		if (e8_read(stream_, &plen, 2) != 2) return -1;
		if (plen > 0) {
			std::string p(plen, '\0');
			if (e8_read(stream_, &p[0], plen) != static_cast<ptrdiff_t>(plen)) return -1;
			e.path = p;
		}
		if (e8_read(stream_, &e.uncompressed_size, 8) != 8) return -1;
		if (e8_read(stream_, &e.data_offset, 8) != 8) return -1;
		if (e8_read(stream_, &e.compressed_size, 4) != 4) return -1;
		entries_.push_back(std::move(e));
	}
	return 0;
}

} // namespace entropy8
