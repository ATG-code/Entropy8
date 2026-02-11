# Entropy8

A modern, cross-platform file archiver with pluggable compression codecs (LZ4, LZMA, Zstd) and a native desktop GUI.

## Architecture

| Layer | Language | Role |
|-------|----------|------|
| **API** | C | ABI-stable public interface, stream-based I/O (no `FILE*`), codec registry |
| **Engine** | C++ | Thread pool, archive format (magic + data blocks + directory), compression dispatch |
| **GUI** | C++ / Dear ImGui | Native desktop app — compact Keka-style dark UI |
| **CLI** | Python | Command-line interface over the C API via ctypes |

### Supported Codecs

| Codec | Description |
|-------|-------------|
| Store | No compression (passthrough) |
| LZ4 | Very fast compression |
| LZMA | High compression ratio |
| Zstd | Balanced speed/ratio (default) |

### Archive Format (.e8)

```
[Magic: "E8AR" 4B] [Data blocks...] [Directory] [DirSize 4B]
```

Each directory entry stores: path, uncompressed size, compressed size, data offset, codec ID.

## Quick Start

### GUI (recommended)

Build and run the native desktop application:

**Windows (PowerShell):**
```powershell
.\scripts\build-gui.ps1 -Run
```

**Linux / macOS:**
```bash
./scripts/build-gui.sh --run
```

The GUI opens a compact settings panel. Drag and drop files onto the window to create archives, or drop `.e8` files to view/extract them.

### CLI (via Docker)

No local toolchain needed — runs in a container:

**Windows:**
```powershell
.\scripts\run.ps1 create archive.e8 file1.txt file2.txt
.\scripts\run.ps1 list archive.e8
.\scripts\run.ps1 extract archive.e8 .\out
```

**Linux / macOS:**
```bash
./scripts/run.sh create archive.e8 file1.txt file2.txt
./scripts/run.sh list archive.e8
./scripts/run.sh extract archive.e8 ./out
```

## Build from Source

### Prerequisites

- CMake 3.14+
- C/C++ compiler (GCC, Clang, or MSVC)
- Git (for FetchContent dependency download)

All compression libraries (LZ4, Zstd, liblzma) and GUI dependencies (GLFW, Dear ImGui, tinyfiledialogs) are fetched automatically via CMake FetchContent — no manual installation required.

### Build Everything

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Outputs:
- `build/bin/entropy8_gui` — GUI application
- `build/bin/libentropy8.dll` (Windows) / `libentropy8.so` (Linux) — shared library

### Engine Only (for Python binding)

```bash
cd engine/core
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Copy the resulting shared library to `engine/bindings/python/entropy8_engine/` for the Python CLI.

## Docker (CLI only)

See [docker/README.md](docker/README.md) for container-based usage.

```bash
docker build -f docker/Dockerfile -t entropy8:latest .
docker run --rm -v "$(pwd):/workspace" -w /workspace entropy8:latest create archive.e8 file1.txt
```

## Project Layout

```
entropy8/
├── CMakeLists.txt                    # Top-level build (engine + GUI)
├── engine/core/
│   ├── include/entropy8/            # C API headers (entropy8.h, io.h, codec.h, config.h)
│   ├── src/                         # C: io.c, entropy8_api.c, codec.c
│   │   ├── algorithms/              # lz4.c, lzma.c, zstd.c
│   │   └── engine/                  # C++: archive.cpp, bridge.cpp, thread_pool.cpp
│   └── CMakeLists.txt
├── engine/bindings/python/          # ctypes Python binding
├── apps/
│   ├── gui/                         # Native C++ GUI (Dear ImGui + GLFW)
│   │   ├── src/                     # main.cpp, ui.cpp, app.cpp, theme.cpp
│   │   └── CMakeLists.txt
│   └── cli/                         # Python CLI (create/list/extract)
├── docker/                          # Dockerfile, docker-compose.yml
└── scripts/                         # build-gui.ps1, build-gui.sh, run.ps1, run.sh
```

## License

See [LICENSE](LICENSE).
