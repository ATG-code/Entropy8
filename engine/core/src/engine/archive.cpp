#include "archive.hpp"
#include "thread_pool.hpp"
#include "../crypto/e8_crypto.h"
#include <cstring>
#include <vector>
#include <algorithm>

namespace entropy8 {

static const uint8_t MAGIC_V1[] = { 'E', '8', 'A', '1' };
static const uint8_t MAGIC_ENC[] = { 'E', '8', 'A', 'E' };
static const size_t BUF_SIZE = 256 * 1024;

/* ─── Memory-stream helpers for encrypted write mode ──────────────────────── */

static void mem_write(std::vector<uint8_t>& buf, size_t& pos, const void* data, size_t len) {
	if (pos + len > buf.size()) buf.resize(pos + len);
	std::memcpy(buf.data() + pos, data, len);
	pos += len;
}

static void mem_read(const std::vector<uint8_t>& buf, size_t& pos, void* out, size_t len) {
	if (pos + len <= buf.size()) {
		std::memcpy(out, buf.data() + pos, len);
	}
	pos += len;
}

/* ─── Constructor ─────────────────────────────────────────────────────────── */

Archive::Archive(E8Stream* stream, Mode mode, const char* password)
    : stream_(stream), mode_(mode)
{
	e8_codecs_init();

	if (password && password[0] != '\0') {
		encrypted_ = true;
		password_ = password;
	}

	if (mode == Mode::Write) {
		pool_ = std::make_unique<ThreadPool>(0);

		if (encrypted_) {
			/* Encrypted: write magic to file, payload goes to in-memory buffer */
			e8_write(stream_, MAGIC_ENC, 4);
			/* Data will accumulate in mem_buf_, encrypted on finalize() */
		} else {
			e8_write(stream_, MAGIC_V1, 4);
		}
	}
}

Archive::~Archive() = default;

/* ─── Add ─────────────────────────────────────────────────────────────────── */

int Archive::add(const char* path, E8Stream* content_stream,
                 int (*progress)(void*, uint64_t, uint64_t), void* progress_user,
                 enum E8Codec codec, int level) {
	if (mode_ != Mode::Write || !path || !content_stream) return -1;

	/* Read entire input into memory */
	std::vector<uint8_t> raw;
	{
		std::vector<uint8_t> buf(BUF_SIZE);
		while (true) {
			ptrdiff_t n = e8_read(content_stream, buf.data(), buf.size());
			if (n < 0) return -1;
			if (n == 0) break;
			raw.insert(raw.end(), buf.data(), buf.data() + n);
		}
	}
	uint64_t raw_size = raw.size();

	/* Compress */
	const E8CodecVtable* cv = e8_codec_get(codec);
	if (!cv) cv = e8_codec_get(E8_CODEC_STORE);

	std::vector<uint8_t> comp;
	uint32_t comp_size;
	uint8_t used_codec;

	if (cv->id == E8_CODEC_STORE || !cv->compress) {
		comp = std::move(raw);
		comp_size = static_cast<uint32_t>(raw_size);
		used_codec = E8_CODEC_STORE;
	} else {
		size_t bound = cv->bound ? cv->bound(raw.size()) : raw.size() + raw.size() / 3 + 1024;
		comp.resize(bound);
		size_t csize = cv->compress(raw.data(), raw.size(), comp.data(), comp.size(), level);
		if (csize == 0 || csize >= raw_size) {
			comp = std::move(raw);
			comp_size = static_cast<uint32_t>(raw_size);
			used_codec = E8_CODEC_STORE;
		} else {
			comp.resize(csize);
			comp_size = static_cast<uint32_t>(csize);
			used_codec = static_cast<uint8_t>(cv->id);
		}
	}

	if (encrypted_) {
		/* Write to in-memory buffer */
		uint64_t start = mem_pos_;
		mem_write(mem_buf_, mem_pos_, comp.data(), comp_size);

		if (progress && progress(progress_user, raw_size, raw_size) != 0) return -2;

		Entry e;
		e.path = path;
		e.data_offset = start;
		e.uncompressed_size = raw_size;
		e.compressed_size = comp_size;
		e.codec_id = used_codec;
		entries_.push_back(std::move(e));
	} else {
		/* Write to archive stream */
		int64_t start = e8_seek(stream_, 0, 1);
		if (start < 0) return -1;

		ptrdiff_t w = e8_write(stream_, comp.data(), comp_size);
		if (w != static_cast<ptrdiff_t>(comp_size)) return -1;

		if (progress && progress(progress_user, raw_size, raw_size) != 0) return -2;

		Entry e;
		e.path = path;
		e.data_offset = static_cast<uint64_t>(start);
		e.uncompressed_size = raw_size;
		e.compressed_size = comp_size;
		e.codec_id = used_codec;
		entries_.push_back(std::move(e));
	}
	return 0;
}

/* ─── Extract ─────────────────────────────────────────────────────────────── */

int Archive::extract(size_t index, E8Stream* output_stream,
                     int (*progress)(void*, uint64_t, uint64_t), void* progress_user) {
	if (mode_ != Mode::Read || index >= entries_.size() || !output_stream) return -1;

	const Entry& e = entries_[index];

	/* Read compressed block */
	std::vector<uint8_t> comp(e.compressed_size);

	if (encrypted_ && !mem_buf_.empty()) {
		/* Read from decrypted in-memory buffer */
		if (e.data_offset + e.compressed_size > mem_buf_.size()) return -1;
		std::memcpy(comp.data(), mem_buf_.data() + e.data_offset, e.compressed_size);
	} else {
		/* Read from stream */
		int64_t pos = e8_seek(stream_, static_cast<int64_t>(e.data_offset), 0);
		if (pos < 0) return -1;

		size_t read_total = 0;
		while (read_total < e.compressed_size) {
			ptrdiff_t n = e8_read(stream_, comp.data() + read_total,
			                      e.compressed_size - read_total);
			if (n <= 0) return -1;
			read_total += static_cast<size_t>(n);
		}
	}

	/* Decompress */
	const E8CodecVtable* cv = e8_codec_get(static_cast<E8Codec>(e.codec_id));
	std::vector<uint8_t> out;

	if (!cv || cv->id == E8_CODEC_STORE || !cv->decompress) {
		out = std::move(comp);
	} else {
		out.resize(e.uncompressed_size);
		size_t dsize = cv->decompress(comp.data(), comp.size(), out.data(), out.size());
		if (dsize != e.uncompressed_size) return -1;
	}

	/* Write to output stream */
	ptrdiff_t w = e8_write(output_stream, out.data(), out.size());
	if (w != static_cast<ptrdiff_t>(out.size())) return -1;

	if (progress && progress(progress_user, e.uncompressed_size, e.uncompressed_size) != 0) return -2;
	return 0;
}

/* ─── Entry info ──────────────────────────────────────────────────────────── */

int Archive::entry(size_t index, char* path, size_t path_size, uint64_t* uncompressed_size) const {
	if (index >= entries_.size()) return -1;
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

/* ─── Finalize ────────────────────────────────────────────────────────────── */

int Archive::finalize() {
	if (mode_ != Mode::Write) return -1;

	if (encrypted_) {
		/*
		 * Build directory into mem_buf_, then encrypt the whole payload
		 * and write: [magic(4)][salt(16)][iv(16)][encrypted_payload]
		 */

		/* Write directory to mem_buf_ */
		size_t dir_start = mem_pos_;
		uint32_t num = static_cast<uint32_t>(entries_.size());
		mem_write(mem_buf_, mem_pos_, &num, 4);

		for (const Entry& e : entries_) {
			uint16_t plen = static_cast<uint16_t>(e.path.size() > 65535 ? 65535 : e.path.size());
			mem_write(mem_buf_, mem_pos_, &plen, 2);
			if (plen) mem_write(mem_buf_, mem_pos_, e.path.data(), plen);
			mem_write(mem_buf_, mem_pos_, &e.uncompressed_size, 8);
			uint64_t off = e.data_offset;
			mem_write(mem_buf_, mem_pos_, &off, 8);
			mem_write(mem_buf_, mem_pos_, &e.compressed_size, 4);
			uint8_t cid = e.codec_id;
			mem_write(mem_buf_, mem_pos_, &cid, 1);
		}

		uint32_t dir_size = static_cast<uint32_t>(mem_pos_ - dir_start);
		mem_write(mem_buf_, mem_pos_, &dir_size, 4);

		/* Encrypt the entire payload */
		size_t payload_len = mem_pos_;
		/* e8_encrypt output: salt(16) + iv(16) + ciphertext(padded) */
		size_t enc_max = 32 + ((payload_len / 16) + 1) * 16;
		std::vector<uint8_t> enc_buf(enc_max);

		size_t enc_len = e8_encrypt(password_.c_str(), password_.size(),
		                            mem_buf_.data(), payload_len,
		                            enc_buf.data());
		if (enc_len == 0) return -1;

		/* Write encrypted payload to stream (magic already written) */
		ptrdiff_t w = e8_write(stream_, enc_buf.data(), enc_len);
		if (w != static_cast<ptrdiff_t>(enc_len)) return -1;

		e8_flush(stream_);
		return 0;
	}

	/* Unencrypted finalize (original behavior) */
	int64_t dir_start = e8_seek(stream_, 0, 2);
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
		uint8_t cid = e.codec_id;
		if (e8_write(stream_, &cid, 1) != 1) return -1;
	}

	int64_t dir_end = e8_seek(stream_, 0, 2);
	if (dir_end < 0) return -1;
	uint32_t dir_size = static_cast<uint32_t>(dir_end - dir_start);
	if (e8_write(stream_, &dir_size, 4) != 4) return -1;

	e8_flush(stream_);
	return 0;
}

/* ─── Load ────────────────────────────────────────────────────────────────── */

int Archive::load() {
	if (mode_ != Mode::Read) return -1;

	/* Read magic at offset 0 */
	uint8_t mag[4];
	if (e8_seek(stream_, 0, 0) != 0 || e8_read(stream_, mag, 4) != 4) return -1;

	bool is_encrypted = (std::memcmp(mag, MAGIC_ENC, 4) == 0);
	bool is_v1 = (std::memcmp(mag, MAGIC_V1, 4) == 0);

	if (!is_encrypted && !is_v1) return -1;

	if (is_encrypted) {
		encrypted_ = true;

		if (password_.empty()) return -1; /* need password */

		/* Read the rest of the file (salt + iv + ciphertext) */
		int64_t file_end = e8_seek(stream_, 0, 2);
		if (file_end < 0 || file_end < 36) return -1; /* magic(4) + salt(16) + iv(16) + at least 1 block */

		size_t enc_len = static_cast<size_t>(file_end - 4);
		std::vector<uint8_t> enc_data(enc_len);
		e8_seek(stream_, 4, 0);
		size_t read_total = 0;
		while (read_total < enc_len) {
			ptrdiff_t n = e8_read(stream_, enc_data.data() + read_total, enc_len - read_total);
			if (n <= 0) return -1;
			read_total += static_cast<size_t>(n);
		}

		/* Decrypt */
		std::vector<uint8_t> dec_buf(enc_len); /* plaintext <= ciphertext */
		size_t dec_len = e8_decrypt(password_.c_str(), password_.size(),
		                            enc_data.data(), enc_len,
		                            dec_buf.data());
		if (dec_len == 0) return -1; /* wrong password or corrupt */

		/* Store decrypted payload in mem_buf_ for extract() */
		mem_buf_.assign(dec_buf.begin(), dec_buf.begin() + dec_len);

		/* Parse directory from decrypted buffer */
		if (dec_len < 8) return -1; /* at least dir_size(4) + num(4) */

		/* Read dir_size from last 4 bytes */
		size_t pos = dec_len - 4;
		uint32_t dir_size;
		std::memcpy(&dir_size, mem_buf_.data() + pos, 4);
		if (dir_size == 0 || dir_size > dec_len - 4) return -1;

		/* Directory starts at dec_len - 4 - dir_size */
		size_t dir_start = dec_len - 4 - dir_size;
		pos = dir_start;

		uint32_t num;
		std::memcpy(&num, mem_buf_.data() + pos, 4);
		pos += 4;

		entries_.clear();
		entries_.reserve(num);

		for (uint32_t i = 0; i < num; ++i) {
			Entry e;
			uint16_t plen;
			std::memcpy(&plen, mem_buf_.data() + pos, 2); pos += 2;
			if (plen > 0) {
				e.path.assign(reinterpret_cast<const char*>(mem_buf_.data() + pos), plen);
				pos += plen;
			}
			std::memcpy(&e.uncompressed_size, mem_buf_.data() + pos, 8); pos += 8;
			std::memcpy(&e.data_offset, mem_buf_.data() + pos, 8); pos += 8;
			std::memcpy(&e.compressed_size, mem_buf_.data() + pos, 4); pos += 4;
			std::memcpy(&e.codec_id, mem_buf_.data() + pos, 1); pos += 1;
			entries_.push_back(std::move(e));
		}
		return 0;
	}

	/* Unencrypted v1 load (original behavior) */
	int64_t file_end = e8_seek(stream_, 0, 2);
	if (file_end < 0 || file_end < 12) return -1;
	e8_seek(stream_, file_end - 4, 0);
	uint32_t dir_size;
	if (e8_read(stream_, &dir_size, 4) != 4) return -1;
	if (dir_size == 0 || static_cast<int64_t>(dir_size) > file_end - 8) return -1;

	int64_t dir_start = file_end - 4 - static_cast<int64_t>(dir_size);
	if (dir_start < 4) return -1;
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
		if (e8_read(stream_, &e.codec_id, 1) != 1) return -1;
		entries_.push_back(std::move(e));
	}
	return 0;
}

} // namespace entropy8
