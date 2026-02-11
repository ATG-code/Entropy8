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

// ── Globals ──────────────────────────────────────────────────────────────────
static AppState* g_state = nullptr;

// ── Drag-and-drop ────────────────────────────────────────────────────────────
static void drop_callback(GLFWwindow* /*window*/, int count, const char** paths) {
	if (!g_state) return;

	for (int i = 0; i < count; ++i) {
		const char* path = paths[i];
		size_t len = strlen(path);

		// If dropped file is a supported archive, open the viewer
		if (len > 3 && strcmp(path + len - 3, ".e8") == 0) {
			OpenArchive(*g_state, path);
			g_state->show_viewer = true;
			continue;
		}

		// Otherwise add to compression queue
		bool dup = false;
		for (auto& f : g_state->files_to_add)
			if (f == path) { dup = true; break; }
		if (!dup) g_state->files_to_add.push_back(path);
	}

	// Auto-create archive when files are dropped
	if (!g_state->files_to_add.empty()) {
		CreateArchive(*g_state);
		g_state->files_to_add.clear();
	}
}

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
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Fixed size like Keka

	// Compact window size matching Keka style
	GLFWwindow* window = glfwCreateWindow(380, 520, "Entropy8", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Setup Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr; // No imgui.ini for compact app

	// Font: slightly larger for readability
	ImFontConfig fontCfg;
	fontCfg.OversampleH = 2;
	fontCfg.OversampleV = 2;
	io.Fonts->AddFontDefault(&fontCfg);

	ApplyDarkTheme();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// State
	AppState state;
	g_state = &state;
	glfwSetDropCallback(window, drop_callback);

	ImVec4 clear = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderUI(state);

		ImGui::Render();
		int dw, dh;
		glfwGetFramebufferSize(window, &dw, &dh);
		glViewport(0, 0, dw, dh);
		glClearColor(clear.x, clear.y, clear.z, clear.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
