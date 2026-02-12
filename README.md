# Entropy8

A modern, cross-platform file archiver with multi-format support, AES-256 encryption, pluggable compression codecs, and a native Qt6/QML desktop GUI.

## Features

- **Multi-format archiving** — Create and extract 7Z, ZIP, TAR, GZIP, BZIP2, XZ, LZIP, and the native E8 format
- **AES-256 encryption** — Encrypt and decrypt archives (E8, 7Z, ZIP) with password-based key derivation (PBKDF2-HMAC-SHA256)
- **Pluggable codecs** — Store, LZ4, LZMA, Zstd compression with adjustable speed/ratio
- **Modern dark UI** — Resizable Qt6/QML interface with drag-and-drop, archive viewer, and encryption indicators
- **Multi-language** — English and Turkish UI with one-click switching
- **Cross-platform** — Windows, Linux, macOS

## Architecture

| Layer | Language | Role |
|-------|----------|------|
| **API** | C | ABI-stable public interface, stream-based I/O, codec registry |
| **Engine** | C++ | Archive format, compression dispatch, AES-256 crypto, multi-format dispatcher |
| **GUI** | C++ / QML | Native Qt6 desktop app — dark themed, resizable, drag-and-drop |
| **CLI** | Python | Command-line interface over the C API via ctypes |

### Supported Formats

| Format | Read | Write | Encryption |
|--------|------|-------|------------|
| E8 | Yes | Yes | AES-256-CBC |
| 7Z | Yes | Yes | AES-256 (via libarchive) |
| ZIP | Yes | Yes | AES-256 (via libarchive) |
| TAR | Yes | Yes | — |
| GZIP | Yes | Yes | — |
| BZIP2 | Yes | Yes | — |
| XZ | Yes | Yes | — |
| LZIP | Yes | Yes | — |

### Compression Codecs

| Codec | Description |
|-------|-------------|
| Store | No compression (passthrough) |
| LZ4 | Very fast compression |
| LZMA | High compression ratio |
| Zstd | Balanced speed/ratio (default) |

### Native Archive Format (.e8)

```
Unencrypted:  [Magic: "E8A1" 4B] [Data blocks...] [Directory] [DirSize 4B]
Encrypted:    [Magic: "E8AE" 4B] [Salt 16B] [IV 16B] [AES-256-CBC encrypted payload]
```

Each directory entry stores: path, uncompressed size, compressed size, data offset, codec ID.

## Quick Start

### GUI (recommended)

Build and run the native desktop application:

**Windows (PowerShell):**
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
.\build\bin\entropy8_gui.exe
```

**Linux / macOS:**
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/bin/entropy8_gui
```

The GUI opens a dark-themed settings panel. Drag and drop files onto the window to create archives, or drop archive files to view and extract them. Use the globe button to switch between English and Turkish.

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
- C/C++ compiler (GCC 13+, Clang, or MSVC)
- Qt 6.x with Qt Quick / Qt Quick Controls 2 modules
- Git (for FetchContent dependency download)

All compression libraries (LZ4, Zstd, liblzma) and archive libraries (libarchive) are fetched automatically via CMake FetchContent — no manual installation required.

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
│   ├── include/entropy8/            # C API headers (entropy8.h, io.h, codec.h, multi_format.h)
│   ├── src/
│   │   ├── algorithms/              # lz4.c, lzma.c, zstd.c
│   │   ├── crypto/                  # aes256.c, sha256.c, e8_crypto.c
│   │   ├── engine/                  # archive.cpp, bridge.cpp, thread_pool.cpp
│   │   └── multi_format.cpp         # Multi-format dispatcher (libarchive + E8)
│   └── CMakeLists.txt
├── engine/bindings/python/          # ctypes Python binding
├── apps/
│   ├── gui/                         # Native Qt6/QML GUI
│   │   ├── qml/                     # Main.qml (UI, translations, theming)
│   │   ├── src/                     # main.cpp, backend.hpp/cpp
│   │   ├── resources.qrc            # QML + font resources
│   │   └── CMakeLists.txt
│   └── cli/                         # Python CLI (create/list/extract)
├── fonts/                           # Inter Tight font family
├── docker/                          # Dockerfile, docker-compose.yml
└── scripts/                         # build-gui.ps1, build-gui.sh, run.ps1, run.sh
```

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE) for details.
