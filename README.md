# Entropy8
Entropy8 is a smart, cross-platform file archiver that analyzes data entropy to choose the most efficient 
compression algorithm automatically.

- **API: C** – ABI-stable, Python (and other) bindings, low-level I/O only (stream callbacks, no `FILE*`).
- **Engine: C++** – Thread pool, resource management, archive format (magic + data + directory at end).
- **UI: Python** – CLI (and optional GUI) over the C API.

## Build (engine)

```bash
cd engine/core
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Output: `libentropy8.dll` (Windows) or `libentropy8.so` (Unix). Copy it next to `engine/bindings/python/entropy8_engine/` so the Python binding can load it.

## CLI (Python)

From the repo root, with the shared library in place:

```bash
python apps/cli/entropy8_cli/main.py create archive.e8 file1.txt file2.txt
python apps/cli/entropy8_cli/main.py list archive.e8
python apps/cli/entropy8_cli/main.py extract archive.e8 [output_dir]
```

## Layout

- `engine/core/include/entropy8/` – C API: `entropy8.h`, `io.h`, `config.h`.
- `engine/core/src/` – C: `io.c`, `entropy8_api.c`; C++: `engine/` (bridge, archive, thread_pool).
- `engine/bindings/python/entropy8_engine/` – ctypes binding and stream helpers.
- `apps/cli/entropy8_cli/` – CLI (create/list/extract).
- `docs/architecture.md` – Architecture and design choices.
