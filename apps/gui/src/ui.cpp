#include "ui.hpp"
#include <imgui.h>
#include <tinyfiledialogs.h>
#include <algorithm>
#include <cstring>

namespace entropy8::gui {

// ── Icons (Unicode) ──────────────────────────────────────────────────────────
// Using simple ASCII art / text since we don't bundle icon fonts.
static const char* ICON_FOLDER   = "[D]";
static const char* ICON_FILE     = " - ";

// ── Codec names for combo ────────────────────────────────────────────────────
static const char* kCodecNames[] = {"Store", "LZ4", "LZMA", "Zstd"};
static const int kCodecCount = 4;

static const char* kMethodNames[] = {"Store", "Fast", "Normal", "Best"};
static const int kMethodCount = 4;

// ── Color helpers ────────────────────────────────────────────────────────────
static ImVec4 CodecColor(int idx) {
	switch (idx) {
		case 0: return ImVec4(0.55f, 0.55f, 0.58f, 1.0f); // Store: gray
		case 1: return ImVec4(0.40f, 0.80f, 0.40f, 1.0f); // LZ4: green
		case 2: return ImVec4(0.90f, 0.65f, 0.20f, 1.0f); // LZMA: orange
		case 3: return ImVec4(0.25f, 0.56f, 0.96f, 1.0f); // Zstd: blue
		default: return ImVec4(1, 1, 1, 1);
	}
}

// ── Create Archive Dialog ────────────────────────────────────────────────────
static void DrawCreateDialog(AppState& state) {
	if (!state.show_create_dialog) return;

	ImGui::SetNextWindowSize(ImVec2(520, 560), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(
		ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
		ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f)
	);

	if (ImGui::Begin("Create Archive", &state.show_create_dialog,
			ImGuiWindowFlags_NoCollapse)) {

		// ── Output path ──────────────────────────────────────────────────
		ImGui::Text("Output:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(-80);
		ImGui::InputText("##out", state.output_path, sizeof(state.output_path));
		ImGui::SameLine();
		if (ImGui::Button("Browse##out")) {
			const char* filters[] = {"*.e8"};
			const char* sel = tinyfd_saveFileDialog(
				"Save Archive", state.output_path, 1, filters, "Entropy8 Archive (*.e8)"
			);
			if (sel) {
				strncpy(state.output_path, sel, sizeof(state.output_path) - 1);
				state.output_path[sizeof(state.output_path) - 1] = '\0';
			}
		}

		ImGui::Separator();

		// ── Codec selection (color badges like screenshot) ───────────────
		ImGui::Text("Codec:");
		ImGui::SameLine(100);

		for (int i = 0; i < kCodecCount; ++i) {
			if (i > 0) ImGui::SameLine();
			bool selected = (state.codec_index == i);

			ImVec4 col = CodecColor(i);
			if (selected) {
				ImGui::PushStyleColor(ImGuiCol_Button, col);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			} else {
				ImVec4 dim = ImVec4(col.x * 0.3f, col.y * 0.3f, col.z * 0.3f, 1.0f);
				ImGui::PushStyleColor(ImGuiCol_Button, dim);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
					ImVec4(col.x * 0.5f, col.y * 0.5f, col.z * 0.5f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			}

			char label[32];
			snprintf(label, sizeof(label), " %s ##codec%d", kCodecNames[i], i);
			if (ImGui::Button(label, ImVec2(80, 0))) {
				state.codec_index = i;
				if (i == 0) state.method_index = 0; // Store -> Store method
			}
			ImGui::PopStyleColor(4);
		}

		ImGui::Spacing();

		// ── Method slider ────────────────────────────────────────────────
		bool is_store = (state.codec_index == 0);
		if (is_store) ImGui::BeginDisabled();

		ImGui::Text("Method:");
		ImGui::SameLine(100);
		ImGui::SetNextItemWidth(280);
		ImGui::SliderInt("##method", &state.method_index, 0, kMethodCount - 1, kMethodNames[state.method_index]);

		// Draw labels below slider
		{
			float slider_x = 100;
			float slider_w = 280;
			ImVec2 cur = ImGui::GetCursorPos();
			for (int i = 0; i < kMethodCount; ++i) {
				float frac = static_cast<float>(i) / static_cast<float>(kMethodCount - 1);
				float x = slider_x + frac * slider_w;
				ImGui::SetCursorPos(ImVec2(x - 15, cur.y));
				ImGui::TextDisabled("%s", kMethodNames[i]);
			}
			ImGui::SetCursorPos(ImVec2(cur.x, cur.y + 20));
		}

		if (is_store) ImGui::EndDisabled();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ── Files to add ─────────────────────────────────────────────────
		ImGui::Text("Files to add:");
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60);
		if (ImGui::Button("+ Add")) {
			const char* multi = tinyfd_openFileDialog(
				"Select Files", "", 0, nullptr, nullptr, 1 /* multi */
			);
			if (multi) {
				// tinyfiledialogs returns pipe-separated paths for multi-select
				std::string s(multi);
				size_t pos = 0;
				while (pos < s.size()) {
					size_t pipe = s.find('|', pos);
					std::string part = (pipe == std::string::npos) ? s.substr(pos) : s.substr(pos, pipe - pos);
					if (!part.empty()) {
						// Avoid duplicates
						bool dup = false;
						for (auto& f : state.files_to_add) {
							if (f == part) { dup = true; break; }
						}
						if (!dup) state.files_to_add.push_back(part);
					}
					if (pipe == std::string::npos) break;
					pos = pipe + 1;
				}
			}
		}

		ImGui::BeginChild("##filelist", ImVec2(0, 180), ImGuiChildFlags_Borders);
		int remove_idx = -1;
		for (int i = 0; i < static_cast<int>(state.files_to_add.size()); ++i) {
			ImGui::PushID(i);
			// Just show filename
			std::string& full = state.files_to_add[i];
			auto slash = full.find_last_of("/\\");
			const char* name = (slash != std::string::npos) ? full.c_str() + slash + 1 : full.c_str();

			ImGui::TextColored(ImVec4(0.7f, 0.8f, 1.0f, 1.0f), "%s", name);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 10);
			if (ImGui::SmallButton("X")) remove_idx = i;

			// Tooltip with full path
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
				ImGui::SetTooltip("%s", full.c_str());
			}
			ImGui::PopID();
		}
		if (remove_idx >= 0) {
			state.files_to_add.erase(state.files_to_add.begin() + remove_idx);
		}
		if (state.files_to_add.empty()) {
			ImGui::TextDisabled("  Drag & drop or click '+ Add' to select files.");
		}
		ImGui::EndChild();

		ImGui::Spacing();

		// ── Create button ────────────────────────────────────────────────
		float bw = 140;
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - bw) * 0.5f + ImGui::GetCursorPosX());
		bool can_create = !state.files_to_add.empty();
		if (!can_create) ImGui::BeginDisabled();
		if (ImGui::Button("Create Archive", ImVec2(bw, 36))) {
			CreateArchive(state);
			state.show_create_dialog = false;
		}
		if (!can_create) ImGui::EndDisabled();
	}
	ImGui::End();
}

// ── Main toolbar ─────────────────────────────────────────────────────────────
static void DrawToolbar(AppState& state) {
	float h = 44;
	ImGui::BeginChild("##toolbar", ImVec2(0, h), ImGuiChildFlags_None);
	ImGui::SetCursorPos(ImVec2(8, 6));

	if (ImGui::Button("New Archive", ImVec2(120, 30))) {
		state.show_create_dialog = true;
		state.files_to_add.clear();
	}
	ImGui::SameLine();
	if (ImGui::Button("Open Archive", ImVec2(120, 30))) {
		const char* filters[] = {"*.e8"};
		const char* sel = tinyfd_openFileDialog(
			"Open Archive", "", 1, filters, "Entropy8 Archive (*.e8)", 0
		);
		if (sel) OpenArchive(state, sel);
	}
	ImGui::SameLine();

	bool has_archive = state.archive_open;
	if (!has_archive) ImGui::BeginDisabled();
	if (ImGui::Button("Extract All", ImVec2(120, 30))) {
		const char* dir = tinyfd_selectFolderDialog("Extract To", "");
		if (dir) ExtractAll(state, dir);
	}
	if (!has_archive) ImGui::EndDisabled();

	// Right-aligned info
	if (state.archive_open) {
		std::string info = state.archive_path + "  |  " +
			std::to_string(state.entries.size()) + " file(s)  |  " +
			FormatSize(state.total_uncompressed);
		float tw = ImGui::CalcTextSize(info.c_str()).x;
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - tw + ImGui::GetCursorPosX() - 8);
		ImGui::SetCursorPosY(12);
		ImGui::TextDisabled("%s", info.c_str());
	}

	ImGui::EndChild();
	ImGui::Separator();
}

// ── File entry table ─────────────────────────────────────────────────────────
static void DrawTable(AppState& state) {
	if (!state.archive_open) {
		// Empty state
		float avail_h = ImGui::GetContentRegionAvail().y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + avail_h * 0.35f);
		const char* msg = "No archive open.\n\nCreate a new archive or open an existing .e8 file.";
		float tw = ImGui::CalcTextSize(msg).x;
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - tw) * 0.5f);
		ImGui::TextDisabled("%s", msg);
		return;
	}

	const ImGuiTableFlags tflags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingStretchProp;

	if (ImGui::BeginTable("##entries", 4, tflags)) {
		ImGui::TableSetupColumn("Name",       ImGuiTableColumnFlags_DefaultSort, 3.0f);
		ImGui::TableSetupColumn("Size",        ImGuiTableColumnFlags_None, 1.0f);
		ImGui::TableSetupColumn("Compressed",  ImGuiTableColumnFlags_None, 1.0f);
		ImGui::TableSetupColumn("Codec",       ImGuiTableColumnFlags_None, 0.6f);
		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();

		for (auto& e : state.entries) {
			ImGui::TableNextRow();

			// Name
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(0.7f, 0.82f, 1.0f, 1.0f), "%s %s", ICON_FILE, e.path.c_str());

			// Size
			ImGui::TableNextColumn();
			ImGui::Text("%s", FormatSize(e.uncompressed_size).c_str());

			// Compressed
			ImGui::TableNextColumn();
			ImGui::Text("%s", FormatSize(e.compressed_size).c_str());

			// Codec
			ImGui::TableNextColumn();
			ImGui::TextColored(CodecColor(e.codec_id), "%s", e.codec_name());
		}

		ImGui::EndTable();
	}
}

// ── Status bar ───────────────────────────────────────────────────────────────
static void DrawStatusBar(AppState& state) {
	ImGui::Separator();
	ImGui::BeginChild("##status", ImVec2(0, 28), ImGuiChildFlags_None);
	ImGui::SetCursorPos(ImVec2(10, 5));

	if (state.status_error)
		ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", state.status_msg.c_str());
	else
		ImGui::TextColored(ImVec4(0.55f, 0.75f, 0.55f, 1.0f), "%s", state.status_msg.c_str());

	ImGui::EndChild();
}

// ── Public entry point ───────────────────────────────────────────────────────
void RenderUI(AppState& state) {
	// Full-screen window
	const ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(vp->WorkPos);
	ImGui::SetNextWindowSize(vp->WorkSize);

	ImGuiWindowFlags wflags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("##main", nullptr, wflags);

	DrawToolbar(state);
	DrawTable(state);
	DrawStatusBar(state);

	ImGui::End();

	// Floating dialogs
	DrawCreateDialog(state);
}

} // namespace entropy8::gui
