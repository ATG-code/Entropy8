# Contributing to Entropy8

Thank you for your interest in contributing to Entropy8! This document provides guidelines and information for contributors.

## Getting Started

1. **Fork** the repository and clone your fork locally.
2. **Create a branch** from `main` for your changes: `git checkout -b feature/my-feature`
3. **Build** the project to ensure everything works before making changes (see [README.md](README.md)).

## Development Setup

### Prerequisites

- CMake 3.14+
- C/C++ compiler with C++17 support (GCC 13+, Clang 15+, MSVC 2019+)
- Qt 6.x with Qt Quick modules (for GUI development)
- Git

### Building

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

All third-party dependencies (LZ4, Zstd, liblzma, libarchive) are fetched automatically via CMake FetchContent.

## Project Structure

| Directory | Description |
|-----------|-------------|
| `engine/core/include/` | Public C API headers |
| `engine/core/src/` | Engine implementation (C/C++) |
| `engine/core/src/crypto/` | AES-256, SHA-256, PBKDF2 implementations |
| `engine/core/src/algorithms/` | Compression codec wrappers |
| `apps/gui/` | Qt6/QML desktop application |
| `apps/gui/qml/` | QML UI files and translations |
| `apps/cli/` | Python command-line interface |
| `engine/bindings/python/` | Python ctypes bindings |

## How to Contribute

### Reporting Bugs

- Use GitHub Issues to report bugs.
- Include steps to reproduce the issue, expected behavior, and actual behavior.
- Include your OS, compiler version, and Qt version.

### Suggesting Features

- Open a GitHub Issue with the "enhancement" label.
- Describe the feature, its use case, and any proposed implementation ideas.

### Submitting Changes

1. Ensure your code compiles without warnings.
2. Follow the existing code style:
   - C code: `snake_case` for functions and variables, `UPPER_CASE` for macros
   - C++ code: `camelCase` for methods, `m_` prefix for member variables
   - QML: standard Qt Quick conventions
3. Add or update comments for public APIs.
4. Test your changes with different archive formats and encryption on/off.
5. Commit with clear, descriptive messages.
6. Push to your fork and open a Pull Request against `main`.

### Adding a New Archive Format

1. Add a new entry to the `E8Format` enum in `engine/core/include/entropy8/multi_format.h`.
2. Implement create/extract/list handlers in `engine/core/src/multi_format.cpp`.
3. Add the format to `kFormats[]` in `apps/gui/src/backend.cpp`.
4. Update translations in `apps/gui/qml/Main.qml` if UI text is involved.

### Adding a New Language

1. Add translation strings to the `tr` QtObject in `apps/gui/qml/Main.qml`.
2. Add a new condition for your language code (e.g., `root.lang === "de"`).
3. Update the language toggle button to cycle through all supported languages.

### Adding a New Compression Codec

1. Create the codec wrapper in `engine/core/src/algorithms/`.
2. Register it in `engine/core/src/codec.c`.
3. Add the FetchContent declaration in `engine/core/CMakeLists.txt` if an external library is required.

## Code Review

All submissions require review before merging. Maintainers may request changes, improvements, or additional tests.

## License

By contributing to Entropy8, you agree that your contributions will be licensed under the [Apache License 2.0](LICENSE).
