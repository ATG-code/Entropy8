#include "app.hpp"
#include "ui.hpp"
#include "theme.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace entropy8::gui;

// ── Drag-and-drop callback ───────────────────────────────────────────────────
static AppState* g_state = nullptr;

static void drop_callback(GLFWwindow* /*window*/, int count, const char** paths) {
	if (!g_state) return;
	for (int i = 0; i < count; ++i) {
		const char* path = paths[i];
		size_t len = strlen(path);

		// If it's an .e8 file, open it
		if (len > 3 && strcmp(path + len - 3, ".e8") == 0) {
			OpenArchive(*g_state, path);
		} else if (g_state->show_create_dialog) {
			// Add to file list if create dialog is open
			bool dup = false;
			for (auto& f : g_state->files_to_add)
				if (f == path) { dup = true; break; }
			if (!dup) g_state->files_to_add.emplace_back(path);
		}
	}
}

// ── Error callback ───────────────────────────────────────────────────────────
static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// ── Main ─────────────────────────────────────────────────────────────────────
#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#else
int main(int, char**) {
#endif
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) return 1;

	// OpenGL 3.3 core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

	GLFWwindow* window = glfwCreateWindow(960, 620, "Entropy8", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Fonts – slightly larger for a clean look
	io.Fonts->AddFontDefault();
	// If you want a custom font, add it here:
	// io.Fonts->AddFontFromFileTTF("path/to/font.ttf", 16.0f);

	// Apply dark theme
	ApplyDarkTheme();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// Drag-and-drop
	AppState state;
	g_state = &state;
	glfwSetDropCallback(window, drop_callback);

	// Handle command-line: open archive if passed as argument
	// (Not applicable for WinMain, would need __argc/__argv on Windows)

	// ── Main loop ────────────────────────────────────────────────────────
	ImVec4 clear_color = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Start frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render application UI
		RenderUI(state);

		// Render
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
