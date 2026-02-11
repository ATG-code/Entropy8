#include "theme.hpp"
#include <imgui.h>

namespace entropy8::gui {

void ApplyDarkTheme() {
	ImGuiStyle& s = ImGui::GetStyle();
	ImVec4* c = s.Colors;

	// ── Geometry (compact, rounded, Keka-inspired) ───────────────────────
	s.WindowRounding    = 10.0f;
	s.FrameRounding     = 6.0f;
	s.PopupRounding     = 8.0f;
	s.GrabRounding      = 10.0f;  // Pill-shaped slider grab
	s.TabRounding       = 6.0f;
	s.ChildRounding     = 6.0f;
	s.ScrollbarRounding = 6.0f;

	s.WindowPadding     = ImVec2(18, 18);
	s.FramePadding      = ImVec2(10, 5);
	s.ItemSpacing       = ImVec2(8, 6);
	s.ItemInnerSpacing  = ImVec2(6, 4);
	s.ScrollbarSize     = 10.0f;
	s.GrabMinSize       = 14.0f;
	s.WindowBorderSize  = 0.0f;
	s.FrameBorderSize   = 0.0f;
	s.PopupBorderSize   = 1.0f;
	s.SeparatorTextPadding = ImVec2(8, 4);

	// ── Palette ──────────────────────────────────────────────────────────
	ImVec4 bg       = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);  // deep dark
	ImVec4 surface  = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
	ImVec4 panel    = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
	ImVec4 border   = ImVec4(0.25f, 0.25f, 0.28f, 0.40f);
	ImVec4 text     = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
	ImVec4 textDim  = ImVec4(0.50f, 0.50f, 0.54f, 1.00f);
	ImVec4 accent   = ImVec4(0.25f, 0.56f, 0.96f, 1.00f);
	ImVec4 accentHi = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
	ImVec4 green    = ImVec4(0.25f, 0.78f, 0.48f, 1.00f);

	// ── Window ───────────────────────────────────────────────────────────
	c[ImGuiCol_WindowBg]       = bg;
	c[ImGuiCol_ChildBg]        = ImVec4(0, 0, 0, 0);
	c[ImGuiCol_PopupBg]        = ImVec4(0.12f, 0.12f, 0.14f, 0.97f);
	c[ImGuiCol_Border]         = border;
	c[ImGuiCol_BorderShadow]   = ImVec4(0, 0, 0, 0);

	// ── Text ─────────────────────────────────────────────────────────────
	c[ImGuiCol_Text]           = text;
	c[ImGuiCol_TextDisabled]   = textDim;

	// ── Title ────────────────────────────────────────────────────────────
	c[ImGuiCol_TitleBg]        = surface;
	c[ImGuiCol_TitleBgActive]  = surface;
	c[ImGuiCol_TitleBgCollapsed] = bg;
	c[ImGuiCol_MenuBarBg]      = surface;

	// ── Frame (inputs, slider track) ─────────────────────────────────────
	c[ImGuiCol_FrameBg]        = panel;
	c[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);
	c[ImGuiCol_FrameBgActive]  = ImVec4(0.26f, 0.26f, 0.30f, 1.00f);

	// ── Buttons ──────────────────────────────────────────────────────────
	c[ImGuiCol_Button]         = panel;
	c[ImGuiCol_ButtonHovered]  = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
	c[ImGuiCol_ButtonActive]   = ImVec4(0.30f, 0.30f, 0.34f, 1.00f);

	// ── Header (selectable, menu item) ───────────────────────────────────
	c[ImGuiCol_Header]         = ImVec4(0.20f, 0.20f, 0.23f, 1.00f);
	c[ImGuiCol_HeaderHovered]  = ImVec4(0.26f, 0.26f, 0.30f, 1.00f);
	c[ImGuiCol_HeaderActive]   = accent;

	// ── Separator ────────────────────────────────────────────────────────
	c[ImGuiCol_Separator]      = ImVec4(0.22f, 0.22f, 0.25f, 0.50f);
	c[ImGuiCol_SeparatorHovered] = accent;
	c[ImGuiCol_SeparatorActive]  = accent;

	// ── Resize ───────────────────────────────────────────────────────────
	c[ImGuiCol_ResizeGrip]        = ImVec4(0.25f, 0.25f, 0.28f, 0.30f);
	c[ImGuiCol_ResizeGripHovered] = accent;
	c[ImGuiCol_ResizeGripActive]  = accentHi;

	// ── Tabs ─────────────────────────────────────────────────────────────
	c[ImGuiCol_Tab]                = surface;
	c[ImGuiCol_TabHovered]         = accent;
	c[ImGuiCol_TabSelected]        = accent;
	c[ImGuiCol_TabDimmed]          = surface;
	c[ImGuiCol_TabDimmedSelected]  = panel;

	// ── Scrollbar ────────────────────────────────────────────────────────
	c[ImGuiCol_ScrollbarBg]          = bg;
	c[ImGuiCol_ScrollbarGrab]        = panel;
	c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.32f, 0.32f, 0.36f, 1.00f);
	c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.40f, 0.40f, 0.44f, 1.00f);

	// ── Slider / checkbox ────────────────────────────────────────────────
	c[ImGuiCol_CheckMark]        = green;
	c[ImGuiCol_SliderGrab]       = accent;
	c[ImGuiCol_SliderGrabActive] = accentHi;

	// ── Table ────────────────────────────────────────────────────────────
	c[ImGuiCol_TableHeaderBg]     = surface;
	c[ImGuiCol_TableBorderStrong] = border;
	c[ImGuiCol_TableBorderLight]  = ImVec4(0.20f, 0.20f, 0.22f, 0.30f);
	c[ImGuiCol_TableRowBg]        = ImVec4(0, 0, 0, 0);
	c[ImGuiCol_TableRowBgAlt]     = ImVec4(1, 1, 1, 0.015f);

	// ── Misc ─────────────────────────────────────────────────────────────
	c[ImGuiCol_TextSelectedBg]   = ImVec4(accent.x, accent.y, accent.z, 0.30f);
	c[ImGuiCol_DragDropTarget]   = accent;
	c[ImGuiCol_NavHighlight]     = accent;
	c[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.55f);
}

} // namespace entropy8::gui
