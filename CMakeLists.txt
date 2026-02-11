cmake_minimum_required(VERSION 3.14)
project(entropy8_project LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Output all binaries to same directory so DLLs are found
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build the engine (shared library)
add_subdirectory(engine/core)

# Build the GUI
add_subdirectory(apps/gui)
