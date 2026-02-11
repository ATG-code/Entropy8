cmake_minimum_required(VERSION 3.14)
project(entropy8_gui LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ── GLFW ──────────────────────────────────────────────────────────────────────
FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
  GIT_SHALLOW    ON
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ── Dear ImGui ────────────────────────────────────────────────────────────────
FetchContent_Declare(imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.91.8
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_lib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_lib PUBLIC glfw)

# ── tinyfiledialogs ──────────────────────────────────────────────────────────
FetchContent_Declare(tinyfd
  GIT_REPOSITORY https://git.code.sf.net/p/tinyfiledialogs/code
  GIT_TAG        master
  GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(tinyfd)
add_library(tinyfd_lib STATIC ${tinyfd_SOURCE_DIR}/tinyfiledialogs.c)
target_include_directories(tinyfd_lib PUBLIC ${tinyfd_SOURCE_DIR})

# ── OpenGL ────────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ── GUI executable ────────────────────────────────────────────────────────────
add_executable(entropy8_gui
  src/main.cpp
  src/app.cpp
  src/ui.cpp
  src/theme.cpp
)

target_include_directories(entropy8_gui PRIVATE src)

# Link against engine from parent build (target "entropy8") or as imported lib
target_link_libraries(entropy8_gui PRIVATE
  imgui_lib
  tinyfd_lib
  OpenGL::GL
  entropy8
)

if(WIN32)
  # Use WinMain entry point (no console window)
  set_target_properties(entropy8_gui PROPERTIES WIN32_EXECUTABLE TRUE)
  # comdlg32 / ole32 needed by tinyfiledialogs on Windows
  target_link_libraries(entropy8_gui PRIVATE comdlg32 ole32)
endif()
