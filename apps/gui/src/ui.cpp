#include "ui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <tinyfiledialogs.h>
#include <algorithm>
#include <cstring>
#include <cstdio>

namespace entropy8::gui {

// ── Constants ────────────────────────────────────────────────────────────────
static const char* kMethodLabels[] = {"Store", "Fast", "Normal", "Slow"};
static constexpr int kMethodCount = 4;
static constexpr float kLabelCol = 90.0f;  // Left column for labels

// ── Helper: draw a colored format badge ──────────────────────────────────────
static void DrawFormatBadge(const ArchiveFormat& fmt, float height = 28.0f) {
	ImVec4 col = ImGui::ColorConvertU32ToFloat4(fmt.color);
	ImVec4 colDark = ImVec4(col.x * 0.6f, col.y * 0.6f, col.z * 0.6f, 1.0f);

	ImGui::PushStyleColor(ImGuiCol_Button, col);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col.x * 1.15f, col.y * 1.15f, col.z * 1.15f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colDark);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

	char label[32];
	snprintf(label, sizeof(label), "  %s  ", fmt.name);
	ImGui::Button(label, ImVec2(0, height));

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
}

// ── Format selection popup ───────────────────────────────────────────────────
static void DrawFormatPopup(AppState& state) {
	if (!state.show_format_popup) return;

	ImGui::SetNextWindowSize(ImVec2(200, 0));

	if (ImGui::Begin("##format_popup", &state.show_format_popup,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {

		for (int i = 0; i < kFormatCount; ++i) {
			const auto& fmt = kFormats[i];
			bool selected = (state.format_index == i);

			// Colored dot
			ImVec4 col = ImGui::ColorConvertU32ToFloat4(fmt.color);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::Text("*");
			ImGui::PopStyleColor();
			ImGui::SameLine();

			// Selectable name
			if (selected) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
			} else if (!fmt.supported) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
			} else {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.87f, 1.0f));
			}

			char selLabel[64];
			snprintf(selLabel, sizeof(selLabel), "%s##fmt%d", fmt.name, i);
			if (ImGui::Selectable(selLabel, selected)) {
				state.format_index = i;
				state.show_format_popup = false;
			}
			ImGui::PopStyleColor();

			// Show checkmark for selected
			if (selected) {
				ImGui::SameLine(170);
				ImGui::TextColored(ImVec4(0.3f, 0.8f, 0.5f, 1.0f), "ok");
			}
		}
	}
	ImGui::End();
}

// ── Tick-mark slider for Method ──────────────────────────────────────────────
static bool MethodSlider(const char* id, int* value, int v_min, int v_max) {
	bool changed = false;

	float avail = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(avail);
	changed = ImGui::SliderInt(id, value, v_min, v_max, "");

	// Draw tick labels below
	ImVec2 cursor = ImGui::GetCursorPos();
	float pad = ImGui::GetStyle().FramePadding.x;
	float sliderW = avail - pad * 2;

	for (int i = v_min; i <= v_max; ++i) {
		float frac = static_cast<float>(i - v_min) / static_cast<float>(v_max - v_min);
		float textW = ImGui::CalcTextSize(kMethodLabels[i]).x;
		float x;
		if (i == v_min) x = pad;
		else if (i == v_max) x = avail - textW;
		else x = pad + frac * sliderW - textW * 0.5f;

		ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y));
		if (i == *value)
			ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.92f, 1.0f), "%s", kMethodLabels[i]);
		else
			ImGui::TextDisabled("%s", kMethodLabels[i]);

		if (i < v_max) ImGui::SameLine();
	}
	ImGui::SetCursorPos(ImVec2(cursor.x, cursor.y + ImGui::GetTextLineHeight() + 4));

	return changed;
}

// ── Main settings panel (Keka-style compact window) ──────────────────────────
static void DrawSettingsPanel(AppState& state) {
	const ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(vp->WorkPos);
	ImGui::SetNextWindowSize(vp->WorkSize);

	ImGuiWindowFlags wf =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::Begin("##main", nullptr, wf);

	// ═══════════════════════════════════════════════════════════════════════
	// Top: Format badge (right-aligned) with dropdown
	// ═══════════════════════════════════════════════════════════════════════
	{
		const auto& fmt = kFormats[state.format_index];
		float badgeW = ImGui::CalcTextSize(fmt.name).x + 32;
		ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - badgeW + ImGui::GetCursorPosX());

		// Badge as a button that opens the format popup
		ImVec4 col = ImGui::ColorConvertU32ToFloat4(fmt.color);
		ImGui::PushStyleColor(ImGuiCol_Button, col);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col.x*1.15f, col.y*1.15f, col.z*1.15f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(col.x*0.7f, col.y*0.7f, col.z*0.7f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));

		char badgeLbl[32];
		snprintf(badgeLbl, sizeof(badgeLbl), "  %s  ##badge", fmt.name);
		if (ImGui::Button(badgeLbl, ImVec2(0, 30))) {
			state.show_format_popup = !state.show_format_popup;
		}

		ImGui::PopStyleColor(4);
		ImGui::PopStyleVar();
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Method slider
	// ═══════════════════════════════════════════════════════════════════════
	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("Method:");
		ImGui::SameLine(kLabelCol);

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		MethodSlider("##method", &state.method_index, 0, kMethodCount - 1);
		ImGui::PopItemWidth();
	}

	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Split field
	// ═══════════════════════════════════════════════════════════════════════
	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("Split:");
		ImGui::SameLine(kLabelCol);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextWithHint("##split", "Example: 5 MB", state.split_buf, sizeof(state.split_buf));
	}

	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Password
	// ═══════════════════════════════════════════════════════════════════════
	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("Password:");
		ImGui::SameLine(kLabelCol);

		float eyeW = 30;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - eyeW - 6);
		ImGuiInputTextFlags flags = state.show_password ? 0 : ImGuiInputTextFlags_Password;
		ImGui::InputText("##pw", state.password, sizeof(state.password), flags);
		ImGui::SameLine();
		if (ImGui::Button(state.show_password ? "o##epw" : "*##epw", ImVec2(eyeW, 0))) {
			state.show_password = !state.show_password;
		}
	}

	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextDisabled("Repeat:");
		ImGui::SameLine(kLabelCol);

		float eyeW = 30;
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - eyeW - 6);
		ImGuiInputTextFlags flags = state.show_password_repeat ? 0 : ImGuiInputTextFlags_Password;
		ImGui::InputText("##pw2", state.password_repeat, sizeof(state.password_repeat), flags);
		ImGui::SameLine();
		if (ImGui::Button(state.show_password_repeat ? "o##epw2" : "*##epw2", ImVec2(eyeW, 0))) {
			state.show_password_repeat = !state.show_password_repeat;
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Checkboxes - group 1
	// ═══════════════════════════════════════════════════════════════════════
	ImGui::Checkbox("Encrypt filenames", &state.opt_encrypt_filenames);
	ImGui::Checkbox("Solid archive", &state.opt_solid_archive);
	ImGui::Checkbox("Self-extracting archive for Windows", &state.opt_self_extracting);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Checkboxes - group 2
	// ═══════════════════════════════════════════════════════════════════════
	ImGui::Checkbox("Verify compression integrity", &state.opt_verify_integrity);
	ImGui::Checkbox("Delete file(s) after compression", &state.opt_delete_after);
	ImGui::Checkbox("Archive items separately", &state.opt_archive_separately);

	ImGui::Spacing();
	ImGui::Spacing();

	// ═══════════════════════════════════════════════════════════════════════
	// Status message at the bottom
	// ═══════════════════════════════════════════════════════════════════════
	if (!state.status_msg.empty()) {
		ImGui::Separator();
		ImGui::Spacing();
		if (state.status_error)
			ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", state.status_msg.c_str());
		else
			ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.5f, 1.0f), "%s", state.status_msg.c_str());
	}

	// ═══════════════════════════════════════════════════════════════════════
	// Drop zone hint (at very bottom, centered)
	// ═══════════════════════════════════════════════════════════════════════
	{
		float avail = ImGui::GetContentRegionAvail().y;
		if (avail > 40) {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + avail - 30);
			const char* hint = "Drop files here to compress";
			float tw = ImGui::CalcTextSize(hint).x;
			ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - tw) * 0.5f + ImGui::GetCursorPosX());
			ImGui::TextDisabled("%s", hint);
		}
	}

	ImGui::End();
}

// ── Format selection popup (floating window) ─────────────────────────────────
static void DrawFormatSelector(AppState& state) {
	if (!state.show_format_popup) return;

	// Position next to the badge button
	const ImGuiViewport* vp = ImGui::GetMainViewport();
	float popW = 180;
	float popH = kFormatCount * 28.0f + 16;
	ImGui::SetNextWindowPos(
		ImVec2(vp->WorkPos.x + vp->WorkSize.x - popW - 10, vp->WorkPos.y + 50),
		ImGuiCond_Always
	);
	ImGui::SetNextWindowSize(ImVec2(popW, popH));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.13f, 0.15f, 0.97f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.32f, 0.6f));

	if (ImGui::Begin("##fmtsel", &state.show_format_popup,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {

		for (int i = 0; i < kFormatCount; ++i) {
			const auto& fmt = kFormats[i];
			bool selected = (state.format_index == i);

			ImGui::PushID(i);

			// Row: checkmark | color dot | name
			if (selected) {
				ImGui::TextColored(ImVec4(0.3f, 0.85f, 0.5f, 1.0f), " ok");
			} else {
				ImGui::TextDisabled("   ");
			}
			ImGui::SameLine(35);

			// Colored square
			ImVec4 col = ImGui::ColorConvertU32ToFloat4(fmt.color);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddRectFilled(
				p, ImVec2(p.x + 14, p.y + 14), fmt.color, 3.0f
			);
			ImGui::Dummy(ImVec2(14, 14));
			ImGui::SameLine();

			// Name
			if (!fmt.supported && !selected) {
				ImGui::TextDisabled("%s", fmt.name);
			} else {
				ImGui::Text("%s", fmt.name);
			}

			// Make the whole row clickable
			ImVec2 rowMin = ImVec2(ImGui::GetWindowPos().x, p.y - 2);
			ImVec2 rowMax = ImVec2(ImGui::GetWindowPos().x + popW, p.y + 20);
			if (ImGui::IsMouseHoveringRect(rowMin, rowMax) && ImGui::IsMouseClicked(0)) {
				state.format_index = i;
				state.show_format_popup = false;
			}

			ImGui::PopID();
		}
	}
	ImGui::End();

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar(2);

	// Close on click outside
	if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
		state.show_format_popup = false;
	}
}

// ── Archive viewer window ────────────────────────────────────────────────────
static void DrawViewer(AppState& state) {
	if (!state.show_viewer || !state.archive_open) return;

	ImGui::SetNextWindowSize(ImVec2(540, 380), ImGuiCond_FirstUseEver);

	char title[256];
	snprintf(title, sizeof(title), "Archive: %s###viewer", state.archive_path.c_str());

	if (ImGui::Begin(title, &state.show_viewer)) {
		// Toolbar
		if (ImGui::Button("Extract All")) {
			const char* dir = tinyfd_selectFolderDialog("Extract To", "");
			if (dir) ExtractAll(state, dir);
		}
		ImGui::SameLine();
		ImGui::TextDisabled("%zu entries  |  %s",
			state.entries.size(), FormatSize(state.total_uncompressed).c_str());

		ImGui::Separator();

		// Table
		const ImGuiTableFlags tf =
			ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
			ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_SizingStretchProp;

		if (ImGui::BeginTable("##entries", 3, tf)) {
			ImGui::TableSetupColumn("Name", 0, 3.0f);
			ImGui::TableSetupColumn("Size", 0, 1.0f);
			ImGui::TableSetupColumn("Codec", 0, 0.8f);
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableHeadersRow();

			for (auto& e : state.entries) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%s", e.path.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%s", FormatSize(e.uncompressed_size).c_str());
				ImGui::TableNextColumn();
				ImGui::TextDisabled("%s", e.codec_name());
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

// ── Public entry point ───────────────────────────────────────────────────────
void RenderUI(AppState& state) {
	DrawSettingsPanel(state);
	DrawFormatSelector(state);
	DrawViewer(state);
}

} // namespace entropy8::gui
