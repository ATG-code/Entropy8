cmake_minimum_required(VERSION 3.14)
project(entropy8_core LANGUAGES C CXX)

set(ENTROPY8_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ENTROPY8_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(entropy8 SHARED
  ${ENTROPY8_SRC_DIR}/io.c
  ${ENTROPY8_SRC_DIR}/entropy8_api.c
  ${ENTROPY8_SRC_DIR}/engine/thread_pool.cpp
  ${ENTROPY8_SRC_DIR}/engine/archive.cpp
  ${ENTROPY8_SRC_DIR}/engine/bridge.cpp
)

target_include_directories(entropy8
  PUBLIC $<BUILD_INTERFACE:${ENTROPY8_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>
  PRIVATE ${ENTROPY8_SRC_DIR} ${ENTROPY8_SRC_DIR}/engine
)

target_compile_features(entropy8 PUBLIC c_std_11 PRIVATE cxx_std_17)

set_target_properties(entropy8 PROPERTIES
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  OUTPUT_NAME entropy8
  SOVERSION 1
)

if(WIN32)
  target_compile_definitions(entropy8 PRIVATE ENTROPY8_EXPORTS)
endif()

install(TARGETS entropy8
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY ${ENTROPY8_INCLUDE_DIR}/entropy8 DESTINATION include)
