#pragma once

#include <entropy8/entropy8.h>
#include <entropy8/codec.h>
#include <string>
#include <vector>

namespace entropy8::gui {

// ── Entry info shown in the UI table ─────────────────────────────────────────
struct FileEntry {
	std::string path;
	uint64_t    uncompressed_size{0};
	uint64_t    compressed_size{0};
	uint8_t     codec_id{0};

	float ratio() const {
		if (uncompressed_size == 0) return 0;
		return 1.0f - static_cast<float>(compressed_size) / static_cast<float>(uncompressed_size);
	}

	const char* codec_name() const {
		switch (codec_id) {
			case E8_CODEC_STORE: return "Store";
			case E8_CODEC_LZ4:   return "LZ4";
			case E8_CODEC_LZMA:  return "LZMA";
			case E8_CODEC_ZSTD:  return "Zstd";
			default:             return "?";
		}
	}
};

// ── Compression method presets ───────────────────────────────────────────────
enum class Method { Store = 0, Fast = 1, Normal = 2, Best = 3 };

inline const char* MethodLabel(Method m) {
	switch (m) {
		case Method::Store:  return "Store";
		case Method::Fast:   return "Fast";
		case Method::Normal: return "Normal";
		case Method::Best:   return "Best";
	}
	return "?";
}

// ── Application state ────────────────────────────────────────────────────────
struct AppState {
	// Current archive
	std::string              archive_path;
	std::vector<FileEntry>   entries;
	bool                     archive_open{false};
	uint64_t                 total_uncompressed{0};
	uint64_t                 total_compressed{0};

	// Create dialog
	bool          show_create_dialog{false};
	std::vector<std::string> files_to_add;
	int           codec_index{3};   // 0=Store, 1=LZ4, 2=LZMA, 3=Zstd
	int           method_index{2};  // 0=Store, 1=Fast, 2=Normal, 3=Best
	char          output_path[512]{"archive.e8"};

	// Status
	std::string   status_msg;
	bool          status_error{false};
};

// ── Operations ───────────────────────────────────────────────────────────────

/// Open an existing .e8 archive and populate state.entries.
bool OpenArchive(AppState& state, const std::string& path);

/// Create a new .e8 archive from state.files_to_add with selected codec/method.
bool CreateArchive(AppState& state);

/// Extract all entries from the open archive to output_dir.
bool ExtractAll(AppState& state, const std::string& output_dir);

/// Format byte size as human-readable string.
std::string FormatSize(uint64_t bytes);

} // namespace entropy8::gui
