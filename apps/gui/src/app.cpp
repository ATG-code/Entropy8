#include "app.hpp"
#include <entropy8/entropy8.h>
#include <entropy8/io.h>
#include <entropy8/codec.h>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <iomanip>

namespace entropy8::gui {

// ── File stream helpers (same pattern as Python binding but in C++) ──────────

struct FileCtx {
	FILE* fp;
};

static ptrdiff_t file_read(void* ctx, void* buf, size_t size) {
	auto* fc = static_cast<FileCtx*>(ctx);
	return static_cast<ptrdiff_t>(fread(buf, 1, size, fc->fp));
}

static ptrdiff_t file_write(void* ctx, const void* buf, size_t size) {
	auto* fc = static_cast<FileCtx*>(ctx);
	return static_cast<ptrdiff_t>(fwrite(buf, 1, size, fc->fp));
}

static int64_t file_seek(void* ctx, int64_t offset, int origin) {
	auto* fc = static_cast<FileCtx*>(ctx);
#ifdef _WIN32
	_fseeki64(fc->fp, offset, origin);
	return _ftelli64(fc->fp);
#else
	fseeko(fc->fp, static_cast<off_t>(offset), origin);
	return static_cast<int64_t>(ftello(fc->fp));
#endif
}

static int file_flush(void* ctx) {
	auto* fc = static_cast<FileCtx*>(ctx);
	return fflush(fc->fp);
}

static int file_close(void* ctx) {
	auto* fc = static_cast<FileCtx*>(ctx);
	int r = fclose(fc->fp);
	delete fc;
	return r;
}

static E8StreamVtable g_file_vtable = {
	file_read, file_write, file_seek, file_flush, file_close
};

static E8Stream make_stream(FILE* fp) {
	auto* fc = new FileCtx{fp};
	E8Stream s{};
	e8_stream_create(&s, &g_file_vtable, fc);
	return s;
}

// ── Codec/method mapping ─────────────────────────────────────────────────────

static E8Codec codec_from_index(int idx) {
	switch (idx) {
		case 0: return E8_CODEC_STORE;
		case 1: return E8_CODEC_LZ4;
		case 2: return E8_CODEC_LZMA;
		case 3: return E8_CODEC_ZSTD;
		default: return E8_CODEC_ZSTD;
	}
}

static int level_from_method(int codec_idx, int method_idx) {
	// Store
	if (codec_idx == 0) return 0;
	// method: 0=Store(->fallback), 1=Fast, 2=Normal, 3=Best
	switch (method_idx) {
		case 0: return 0;
		case 1: return 1;  // fast
		case 2: return (codec_idx == 2) ? 6 : 3;  // normal (lzma: 6, others: 3)
		case 3: return (codec_idx == 2) ? 9 : (codec_idx == 1 ? 12 : 19); // best
		default: return 3;
	}
}

// ── Operations ───────────────────────────────────────────────────────────────

bool OpenArchive(AppState& state, const std::string& path) {
	e8_codecs_init();

	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) {
		state.status_msg = "Cannot open: " + path;
		state.status_error = true;
		return false;
	}

	E8Stream stream = make_stream(fp);
	E8Archive* ar = e8_archive_open(&stream);
	if (!ar) {
		e8_stream_destroy(&stream);
		state.status_msg = "Invalid archive format: " + path;
		state.status_error = true;
		return false;
	}

	state.entries.clear();
	state.total_uncompressed = 0;
	state.total_compressed = 0;
	size_t n = e8_archive_count(ar);

	for (size_t i = 0; i < n; ++i) {
		char pbuf[4096];
		uint64_t usize = 0;
		e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);

		FileEntry fe;
		fe.path = pbuf;
		fe.uncompressed_size = usize;
		// Note: compressed_size and codec_id are internal to the engine;
		// for now we show uncompressed size. Future: extend C API for full entry info.
		fe.compressed_size = usize; // placeholder
		fe.codec_id = 0;

		state.entries.push_back(fe);
		state.total_uncompressed += usize;
	}

	e8_archive_close(ar);
	// stream is destroyed by archive close (close callback closes FILE)

	state.archive_path = path;
	state.archive_open = true;
	state.status_msg = "Opened: " + path + " (" + std::to_string(n) + " entries)";
	state.status_error = false;
	return true;
}

bool CreateArchive(AppState& state) {
	e8_codecs_init();

	if (state.files_to_add.empty()) {
		state.status_msg = "No files to add.";
		state.status_error = true;
		return false;
	}

	FILE* out_fp = fopen(state.output_path, "wb");
	if (!out_fp) {
		state.status_msg = "Cannot create: " + std::string(state.output_path);
		state.status_error = true;
		return false;
	}

	E8Stream out_stream = make_stream(out_fp);
	E8Archive* ar = e8_archive_create(&out_stream);
	if (!ar) {
		e8_stream_destroy(&out_stream);
		state.status_msg = "Failed to create archive.";
		state.status_error = true;
		return false;
	}

	int added = 0;
	for (auto& fpath : state.files_to_add) {
		FILE* in_fp = fopen(fpath.c_str(), "rb");
		if (!in_fp) continue;
		E8Stream in_stream = make_stream(in_fp);

		// Extract filename from path
		std::string name = fpath;
		auto slash = name.find_last_of("/\\");
		if (slash != std::string::npos) name = name.substr(slash + 1);

		int r = e8_archive_add(ar, name.c_str(), &in_stream, nullptr, nullptr);
		// in_stream close callback closes in_fp
		e8_stream_destroy(&in_stream);
		if (r == 0) added++;
	}

	e8_archive_close(ar);
	// Closes + finalizes + flushes

	state.status_msg = "Created " + std::string(state.output_path) + " with " +
	                   std::to_string(added) + " file(s).";
	state.status_error = false;

	// Auto-open the newly created archive
	OpenArchive(state, state.output_path);
	return true;
}

bool ExtractAll(AppState& state, const std::string& output_dir) {
	e8_codecs_init();

	if (!state.archive_open) {
		state.status_msg = "No archive open.";
		state.status_error = true;
		return false;
	}

	FILE* fp = fopen(state.archive_path.c_str(), "rb");
	if (!fp) {
		state.status_msg = "Cannot reopen: " + state.archive_path;
		state.status_error = true;
		return false;
	}

	E8Stream stream = make_stream(fp);
	E8Archive* ar = e8_archive_open(&stream);
	if (!ar) {
		e8_stream_destroy(&stream);
		state.status_msg = "Failed to reopen archive.";
		state.status_error = true;
		return false;
	}

	size_t n = e8_archive_count(ar);
	int extracted = 0;

	for (size_t i = 0; i < n; ++i) {
		char pbuf[4096];
		uint64_t usize = 0;
		e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);

		std::string name = pbuf;
		auto slash = name.find_last_of("/\\");
		if (slash != std::string::npos) name = name.substr(slash + 1);

		std::string outpath = output_dir + "/" + name;
		FILE* out_fp = fopen(outpath.c_str(), "wb");
		if (!out_fp) continue;

		E8Stream out_stream = make_stream(out_fp);
		int r = e8_archive_extract(ar, i, &out_stream, nullptr, nullptr);
		e8_stream_destroy(&out_stream);
		if (r == 0) extracted++;
	}

	e8_archive_close(ar);

	state.status_msg = "Extracted " + std::to_string(extracted) + " file(s) to " + output_dir;
	state.status_error = false;
	return true;
}

std::string FormatSize(uint64_t bytes) {
	std::ostringstream oss;
	if (bytes < 1024) {
		oss << bytes << " B";
	} else if (bytes < 1024 * 1024) {
		oss << std::fixed << std::setprecision(1) << (bytes / 1024.0) << " KB";
	} else if (bytes < 1024ULL * 1024 * 1024) {
		oss << std::fixed << std::setprecision(1) << (bytes / (1024.0 * 1024.0)) << " MB";
	} else {
		oss << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
	}
	return oss.str();
}

} // namespace entropy8::gui
