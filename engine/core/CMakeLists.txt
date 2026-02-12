cmake_minimum_required(VERSION 3.14)
project(entropy8_core LANGUAGES C CXX)

set(ENTROPY8_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(ENTROPY8_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

include(FetchContent)

# ── LZ4 ──────────────────────────────────────────────────────────────────────
find_package(lz4 CONFIG QUIET)
if(NOT lz4_FOUND)
  message(STATUS "LZ4 not found – fetching from source")
  FetchContent_Declare(lz4
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG        v1.10.0
    GIT_SHALLOW    ON
    SOURCE_SUBDIR  build/cmake
  )
  set(LZ4_BUILD_CLI OFF CACHE BOOL "" FORCE)
  set(LZ4_BUILD_LEGACY_LZ4C OFF CACHE BOOL "" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(lz4)
  # lz4 cmake creates target: lz4_static
endif()

# ── Zstd ─────────────────────────────────────────────────────────────────────
find_package(zstd CONFIG QUIET)
if(NOT zstd_FOUND)
  message(STATUS "Zstd not found – fetching from source")
  FetchContent_Declare(zstd
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG        v1.5.6
    GIT_SHALLOW    ON
    SOURCE_SUBDIR  build/cmake
  )
  set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
  set(ZSTD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(ZSTD_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(ZSTD_BUILD_STATIC ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(zstd)
  # zstd cmake creates target: libzstd_static
endif()

# ── XZ / liblzma ────────────────────────────────────────────────────────────
find_package(liblzma CONFIG QUIET)
if(NOT liblzma_FOUND)
  find_package(LibLZMA QUIET)
endif()
if(NOT liblzma_FOUND AND NOT LIBLZMA_FOUND)
  message(STATUS "liblzma not found – fetching from source")
  FetchContent_Declare(xz
    GIT_REPOSITORY https://github.com/tukaani-project/xz.git
    GIT_TAG        v5.6.4
    GIT_SHALLOW    ON
  )
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(XZ_BUILD_XZ OFF CACHE BOOL "" FORCE)
  set(XZ_BUILD_XZDEC OFF CACHE BOOL "" FORCE)
  set(XZ_BUILD_LZMADEC OFF CACHE BOOL "" FORCE)
  set(XZ_BUILD_LZMAINFO OFF CACHE BOOL "" FORCE)
  set(CREATE_XZ_SYMLINKS OFF CACHE BOOL "" FORCE)
  set(CREATE_LZMA_SYMLINKS OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(xz)
  # xz cmake creates target: liblzma
endif()

# ── libarchive (multi-format archive support) ────────────────────────────────
find_package(LibArchive QUIET)
if(NOT LibArchive_FOUND)
  message(STATUS "libarchive not found – fetching from source")
  FetchContent_Declare(libarchive
    GIT_REPOSITORY https://github.com/libarchive/libarchive.git
    GIT_TAG        v3.7.7
    GIT_SHALLOW    ON
  )
  set(ENABLE_TEST OFF CACHE BOOL "" FORCE)
  set(ENABLE_TAR OFF CACHE BOOL "" FORCE)
  set(ENABLE_CPIO OFF CACHE BOOL "" FORCE)
  set(ENABLE_CAT OFF CACHE BOOL "" FORCE)
  set(ENABLE_UNZIP OFF CACHE BOOL "" FORCE)
  set(ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(libarchive)
  # Creates target: archive_static (or archive)
endif()

# ── entropy8 shared library ─────────────────────────────────────────────────
add_library(entropy8 SHARED
  ${ENTROPY8_SRC_DIR}/io.c
  ${ENTROPY8_SRC_DIR}/entropy8_api.c
  ${ENTROPY8_SRC_DIR}/codec.c
  ${ENTROPY8_SRC_DIR}/algorithms/lz4.c
  ${ENTROPY8_SRC_DIR}/algorithms/lzma.c
  ${ENTROPY8_SRC_DIR}/algorithms/zstd.c
  ${ENTROPY8_SRC_DIR}/crypto/aes256.c
  ${ENTROPY8_SRC_DIR}/crypto/sha256.c
  ${ENTROPY8_SRC_DIR}/crypto/e8_crypto.c
  ${ENTROPY8_SRC_DIR}/engine/thread_pool.cpp
  ${ENTROPY8_SRC_DIR}/engine/archive.cpp
  ${ENTROPY8_SRC_DIR}/engine/bridge.cpp
  ${ENTROPY8_SRC_DIR}/multi_format.cpp
)

target_include_directories(entropy8
  PUBLIC $<BUILD_INTERFACE:${ENTROPY8_INCLUDE_DIR}> $<INSTALL_INTERFACE:include>
  PRIVATE ${ENTROPY8_SRC_DIR} ${ENTROPY8_SRC_DIR}/engine
)

target_compile_features(entropy8 PUBLIC c_std_11 PRIVATE cxx_std_17)

# ── Link compression libraries ───────────────────────────────────────────────
# LZ4
if(TARGET lz4::lz4)
  target_link_libraries(entropy8 PRIVATE lz4::lz4)
elseif(TARGET lz4_static)
  target_link_libraries(entropy8 PRIVATE lz4_static)
  target_include_directories(entropy8 PRIVATE ${lz4_SOURCE_DIR}/lib)
endif()

# Zstd
if(TARGET zstd::libzstd_shared)
  target_link_libraries(entropy8 PRIVATE zstd::libzstd_shared)
elseif(TARGET zstd::libzstd_static)
  target_link_libraries(entropy8 PRIVATE zstd::libzstd_static)
elseif(TARGET libzstd_static)
  target_link_libraries(entropy8 PRIVATE libzstd_static)
  target_include_directories(entropy8 PRIVATE ${zstd_SOURCE_DIR}/lib)
endif()

# liblzma
if(TARGET liblzma::liblzma)
  target_link_libraries(entropy8 PRIVATE liblzma::liblzma)
elseif(TARGET liblzma)
  target_link_libraries(entropy8 PRIVATE liblzma)
  # FetchContent xz: public API headers live in src/liblzma/api/
  if(xz_SOURCE_DIR)
    target_include_directories(entropy8 PRIVATE ${xz_SOURCE_DIR}/src/liblzma/api)
  endif()
elseif(LIBLZMA_FOUND)
  target_include_directories(entropy8 PRIVATE ${LIBLZMA_INCLUDE_DIRS})
  target_link_libraries(entropy8 PRIVATE ${LIBLZMA_LIBRARIES})
endif()

# libarchive
if(LibArchive_FOUND)
  target_include_directories(entropy8 PRIVATE ${LibArchive_INCLUDE_DIRS})
  target_link_libraries(entropy8 PRIVATE ${LibArchive_LIBRARIES})
elseif(TARGET archive_static)
  target_link_libraries(entropy8 PRIVATE archive_static)
  target_include_directories(entropy8 PRIVATE ${libarchive_SOURCE_DIR}/libarchive)
elseif(TARGET archive)
  target_link_libraries(entropy8 PRIVATE archive)
  if(libarchive_SOURCE_DIR)
    target_include_directories(entropy8 PRIVATE ${libarchive_SOURCE_DIR}/libarchive)
  endif()
endif()

# Windows crypto (for CryptGenRandom used in e8_crypto.c)
if(WIN32)
  target_link_libraries(entropy8 PRIVATE advapi32)
endif()

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
