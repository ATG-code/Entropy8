# Entropy8 Architecture (7zip-style)

## Overview

- **API: C** – ABI stability, Python (and other) bindings, low-level I/O only.
- **Engine: C++** – Thread pool, resource management, internal complexity.
- **UI: Python** – CLI and (optional) GUI over the C API.

```
┌─────────────────────────────────────────────────────────┐
│  Python UI (CLI / GUI)                                   │
│  apps/cli, apps/gui                                      │
└─────────────────────────┬───────────────────────────────┘
                          │ ctypes / pybind11
┌─────────────────────────▼───────────────────────────────┐
│  C API (ABI-stable)                                      │
│  include/entropy8/entropy8.h, io.h                       │
│  - e8_archive_create/open/close, add, extract, list      │
│  - E8Stream (read/write/seek callbacks, no FILE*)        │
└─────────────────────────┬───────────────────────────────┘
                          │ engine_bridge.h (internal)
┌─────────────────────────▼───────────────────────────────┐
│  C++ Engine (internal)                                  │
│  src/engine/archive.cpp, thread_pool.cpp, bridge.cpp    │
│  - Archive format (magic + data + directory at end)       │
│  - Thread pool for compression tasks                     │
│  - Resource management (RAII, unique_ptr)               │
└─────────────────────────────────────────────────────────┘
```

## C Layer (ABI, bindings, I/O)

- **entropy8.h** – Archive create/open, add, extract, list; progress callback; `e8_last_error()`.
- **io.h** – `E8Stream` with vtable (read, write, seek, flush, close). No `FILE*` in the API; all I/O via callbacks so Python/other languages can supply their own streams.
- **config.h** – `ENTROPY8_API` (dllexport/dllimport on Windows, visibility on Unix).

Implemented in C:

- **io.c** – `e8_stream_create` / `e8_stream_destroy` and `e8_read` / `e8_write` / `e8_seek` / `e8_flush` (vtable dispatch).
- **entropy8_api.c** – Public API implementation; forwards to engine via `engine_bridge.h` and sets `e8_last_error`.

## C++ Engine (internal)

- **engine_bridge.h** – C-linkage functions implemented in C++: `e8_engine_create`, `e8_engine_open`, `e8_engine_close`, `e8_engine_add`, `e8_engine_extract`, `e8_engine_count`, `e8_engine_entry`; `e8_set_last_error` (implemented in C, called from C++).
- **bridge.cpp** – Implements the bridge; uses `Archive` and calls `e8_set_last_error` for errors.
- **archive.hpp/cpp** – Archive format (magic "E8A1", data blocks, directory at end with `dir_size` prefix), add/extract/load/finalize; holds entries (path, offset, sizes).
- **thread_pool.hpp/cpp** – Worker threads and task queue for future compression/analysis work.

Current format: store-only (uncompressed). Placeholder for entropy-based algorithm selection and compression (e.g. LZ4/LZMA/Zstd) in the engine later.

## Python

- **engine/bindings/python/entropy8_engine/** – ctypes binding to the shared library; `stream_from_file` / `stream_from_path`, `Archive` (create/open, add, extract, list).
- **apps/cli** – CLI: `create`, `list`, `extract` for `.e8` archives.

## Build

- **engine/core/CMakeLists.txt** – Builds shared library `entropy8` (C + C++) from `io.c`, `entropy8_api.c`, and `engine/*.cpp`. Install headers and lib for Python/other consumers.

## Design choices

| Concern            | Choice |
|--------------------|--------|
| ABI stability      | Public API is C-only; no C++ types in headers. |
| Python binding     | ctypes on the C API; no C++ in the binding. |
| Low-level I/O      | Stream = vtable + ctx; no FILE* in API. |
| Thread pool        | C++ only; C API stays single-threaded from the caller’s view. |
| Resource management| C++ RAII (Archive, ThreadPool); C API uses opaque handles. |
