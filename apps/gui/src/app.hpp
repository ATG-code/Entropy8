#pragma once

#include <entropy8/entropy8.h>
#include <entropy8/codec.h>
#include <cstdint>
#include <string>
#include <vector>

namespace entropy8::gui {

// ── Archive format definitions ───────────────────────────────────────────────
struct ArchiveFormat {
	const char* name;        // Display name
	const char* extension;   // File extension
	uint32_t    color;       // Badge color (ABGR packed for ImGui)
	bool        supported;   // Currently implemented?
};

// Alphabetically sorted format list
inline const ArchiveFormat kFormats[] = {
	{"7Z",    ".7z",    0xFF2070E0, false},
	{"BZIP2", ".bz2",   0xFFE08030, false},
	{"E8",    ".e8",    0xFFF59040, true },
	{"GZIP",  ".gz",    0xFF40B040, false},
	{"LZIP",  ".lz",    0xFFE04070, false},
	{"TAR",   ".tar",   0xFF60A0C0, false},
	{"XZ",    ".xz",    0xFF5060D0, false},
	{"ZIP",   ".zip",   0xFF3090E0, false},
};
inline constexpr int kFormatCount = sizeof(kFormats) / sizeof(kFormats[0]);

// ── Entry info shown when viewing an archive ─────────────────────────────────
struct FileEntry {
	std::string path;
	uint64_t    uncompressed_size{0};
	uint64_t    compressed_size{0};
	uint8_t     codec_id{0};

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

// ── Application state ────────────────────────────────────────────────────────
struct AppState {
	// Format & compression
	int  format_index{2};          // Default: E8 (index into kFormats)
	int  method_index{2};          // 0=Store, 1=Fast, 2=Normal, 3=Slow
	int  codec_index{3};           // 0=Store, 1=LZ4, 2=LZMA, 3=Zstd (for E8)
	bool show_format_popup{false};

	// Split
	char split_buf[64]{""};

	// Password
	char password[256]{""};
	char password_repeat[256]{""};
	bool show_password{false};
	bool show_password_repeat{false};

	// Options
	bool opt_encrypt_filenames{false};
	bool opt_solid_archive{true};
	bool opt_self_extracting{false};
	bool opt_verify_integrity{false};
	bool opt_delete_after{false};
	bool opt_archive_separately{false};

	// Files queued for compression (via drag-and-drop)
	std::vector<std::string> files_to_add;

	// Archive viewer state
	std::string              archive_path;
	std::vector<FileEntry>   entries;
	bool                     archive_open{false};
	uint64_t                 total_uncompressed{0};
	uint64_t                 total_compressed{0};
	bool                     show_viewer{false};

	// Status
	std::string   status_msg;
	bool          status_error{false};
	float         status_timer{0};
};

// ── Operations ───────────────────────────────────────────────────────────────
bool OpenArchive(AppState& state, const std::string& path);
bool CreateArchive(AppState& state);
bool ExtractAll(AppState& state, const std::string& output_dir);
std::string FormatSize(uint64_t bytes);

} // namespace entropy8::gui
