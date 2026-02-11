# Entropy8 Engine

The core C/C++ library that implements the archive format and compression pipeline.

## Architecture

```
include/entropy8/
  entropy8.h    — Public C API: create, open, close, add, extract, count, entry
  io.h          — Stream abstraction (E8Stream + vtable callbacks)
  codec.h       — Codec registry: register/get compress/decompress functions
  config.h      — Platform macros (ENTROPY8_API for DLL export/import)

src/
  io.c              — Stream dispatch (read, write, seek, flush, close)
  entropy8_api.c    — Thin C wrapper calling engine bridge + thread-local error
  codec.c           — Codec registry (init, register, get)
  algorithms/
    lz4.c           — LZ4 compress/decompress via liblz4
    lzma.c          — LZMA compress/decompress via liblzma
    zstd.c          — Zstd compress/decompress via libzstd
  engine/
    bridge.cpp      — C-linkage bridge to C++ Archive class
    archive.cpp     — Archive format: read/write, compress/decompress per entry
    archive.hpp     — Archive class + Entry struct
    thread_pool.cpp — Basic thread pool for async tasks
    thread_pool.hpp
    engine_bridge.h — Internal C-linkage declarations
```

## Build

All compression libraries are fetched automatically via CMake FetchContent:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Output: `libentropy8.dll` (Windows) or `libentropy8.so` (Linux).

## Archive Format (.e8)

```
[Magic "E8AR" 4B]
[Data block 0] [Data block 1] ... [Data block N]
[Directory: count(4) + entries...]
[DirSize 4B]
```

Each entry: `pathlen(2) + path + uncompressed_size(8) + data_offset(8) + compressed_size(4) + codec_id(1)`

The directory is always at the end. To read: seek to EOF-4, read dir_size, seek back, parse entries.

## Codec Pipeline

1. `e8_codecs_init()` registers all built-in codecs (Store, LZ4, LZMA, Zstd)
2. `Archive::add()` compresses data using the selected codec, falls back to Store if compression doesn't reduce size
3. `Archive::extract()` reads codec_id from the entry and decompresses accordingly
