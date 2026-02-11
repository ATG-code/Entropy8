#include "theme.hpp"
#include <imgui.h>

namespace entropy8::gui {

void ApplyDarkTheme() {
	ImGuiStyle& s = ImGui::GetStyle();
	ImVec4* c = s.Colors;

	// Geometry
	s.WindowRounding    = 8.0f;
	s.FrameRounding     = 6.0f;
	s.PopupRounding     = 6.0f;
	s.GrabRounding      = 4.0f;
	s.TabRounding       = 6.0f;
	s.ChildRounding     = 6.0f;
	s.ScrollbarRounding = 6.0f;
	s.WindowPadding     = ImVec2(14, 14);
	s.FramePadding      = ImVec2(10, 6);
	s.ItemSpacing       = ImVec2(10, 8);
	s.ItemInnerSpacing  = ImVec2(8, 6);
	s.ScrollbarSize     = 12.0f;
	s.GrabMinSize       = 10.0f;
	s.WindowBorderSize  = 0.0f;
	s.FrameBorderSize   = 0.0f;
	s.PopupBorderSize   = 1.0f;

	// Base palette
	ImVec4 bg       = ImVec4(0.11f, 0.11f, 0.12f, 1.00f); // #1C1C1F
	ImVec4 surface  = ImVec4(0.16f, 0.16f, 0.18f, 1.00f); // #292930
	ImVec4 panel    = ImVec4(0.20f, 0.20f, 0.22f, 1.00f); // #333338
	ImVec4 border   = ImVec4(0.28f, 0.28f, 0.30f, 0.50f);
	ImVec4 text     = ImVec4(0.92f, 0.92f, 0.94f, 1.00f);
	ImVec4 textDim  = ImVec4(0.55f, 0.55f, 0.58f, 1.00f);
	ImVec4 accent   = ImVec4(0.25f, 0.56f, 0.96f, 1.00f); // #4090F5
	ImVec4 accentHi = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
	ImVec4 green    = ImVec4(0.30f, 0.75f, 0.45f, 1.00f);

	// Window
	c[ImGuiCol_WindowBg]       = bg;
	c[ImGuiCol_ChildBg]        = ImVec4(0, 0, 0, 0);
	c[ImGuiCol_PopupBg]        = ImVec4(0.14f, 0.14f, 0.16f, 0.96f);
	c[ImGuiCol_Border]         = border;
	c[ImGuiCol_BorderShadow]   = ImVec4(0, 0, 0, 0);

	// Text
	c[ImGuiCol_Text]           = text;
	c[ImGuiCol_TextDisabled]   = textDim;

	// Title bar
	c[ImGuiCol_TitleBg]        = surface;
	c[ImGuiCol_TitleBgActive]  = surface;
	c[ImGuiCol_TitleBgCollapsed] = bg;

	// Menu
	c[ImGuiCol_MenuBarBg]      = surface;

	// Frame (input, combo, slider bg)
	c[ImGuiCol_FrameBg]        = panel;
	c[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.27f, 1.00f);
	c[ImGuiCol_FrameBgActive]  = ImVec4(0.28f, 0.28f, 0.32f, 1.00f);

	// Buttons
	c[ImGuiCol_Button]         = accent;
	c[ImGuiCol_ButtonHovered]  = accentHi;
	c[ImGuiCol_ButtonActive]   = ImVec4(0.18f, 0.45f, 0.85f, 1.00f);

	// Header (collapsing header, selectable, menu item)
	c[ImGuiCol_Header]         = ImVec4(0.22f, 0.22f, 0.25f, 1.00f);
	c[ImGuiCol_HeaderHovered]  = ImVec4(0.28f, 0.28f, 0.32f, 1.00f);
	c[ImGuiCol_HeaderActive]   = accent;

	// Separator
	c[ImGuiCol_Separator]      = border;
	c[ImGuiCol_SeparatorHovered] = accent;
	c[ImGuiCol_SeparatorActive]  = accent;

	// Resize grip
	c[ImGuiCol_ResizeGrip]        = ImVec4(0.28f, 0.28f, 0.30f, 0.40f);
	c[ImGuiCol_ResizeGripHovered] = accent;
	c[ImGuiCol_ResizeGripActive]  = accentHi;

	// Tabs
	c[ImGuiCol_Tab]                = surface;
	c[ImGuiCol_TabHovered]         = accent;
	c[ImGuiCol_TabSelected]        = accent;
	c[ImGuiCol_TabDimmed]          = surface;
	c[ImGuiCol_TabDimmedSelected]  = panel;

	// Scrollbar
	c[ImGuiCol_ScrollbarBg]        = bg;
	c[ImGuiCol_ScrollbarGrab]      = panel;
	c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.38f, 1.00f);
	c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.42f, 0.42f, 0.45f, 1.00f);

	// Checkbox / slider grab
	c[ImGuiCol_CheckMark]       = green;
	c[ImGuiCol_SliderGrab]      = accent;
	c[ImGuiCol_SliderGrabActive] = accentHi;

	// Table
	c[ImGuiCol_TableHeaderBg]    = surface;
	c[ImGuiCol_TableBorderStrong] = border;
	c[ImGuiCol_TableBorderLight]  = ImVec4(0.22f, 0.22f, 0.24f, 0.40f);
	c[ImGuiCol_TableRowBg]       = ImVec4(0, 0, 0, 0);
	c[ImGuiCol_TableRowBgAlt]    = ImVec4(1, 1, 1, 0.02f);

	// Misc
	c[ImGuiCol_TextSelectedBg]   = ImVec4(accent.x, accent.y, accent.z, 0.35f);
	c[ImGuiCol_DragDropTarget]   = accent;
	c[ImGuiCol_NavHighlight]     = accent;
	c[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.55f);
}

} // namespace entropy8::gui
