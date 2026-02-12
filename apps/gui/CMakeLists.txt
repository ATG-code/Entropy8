cmake_minimum_required(VERSION 3.16)
project(entropy8_gui LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── Qt6 ───────────────────────────────────────────────────────────────────────
find_package(Qt6 6.3 REQUIRED COMPONENTS Quick QuickControls2)

# ── GUI executable ────────────────────────────────────────────────────────────
qt_add_executable(entropy8_gui
    src/main.cpp
    src/backend.cpp
    resources.qrc
)

target_include_directories(entropy8_gui PRIVATE src)

# Link against Qt6, engine from parent build (target "entropy8")
target_link_libraries(entropy8_gui PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    entropy8
)

if(WIN32)
    # Use WinMain entry point (no console window)
    set_target_properties(entropy8_gui PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
