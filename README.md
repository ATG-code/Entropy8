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

## Docker

Build and run the CLI in a container (Linux image, builds `libentropy8.so`):

```bash
docker build -t entropy8:latest .
docker run --rm entropy8:latest --help
docker run --rm -v "$(pwd)/data:/data" entropy8:latest create /data/archive.e8 /data/file1.txt
docker run --rm -v "$(pwd)/data:/data" entropy8:latest list /data/archive.e8
docker run --rm -v "$(pwd)/data:/data" entropy8:latest extract /data/archive.e8 /data/out
```

With Docker Compose (mount `./data` as `/data`):

```bash
docker compose build
docker compose run --rm entropy8 create /data/archive.e8 /data/file1.txt
docker compose run --rm entropy8 list /data/archive.e8
docker compose run --rm entropy8 extract /data/archive.e8 /data/out
```

## Layout

- `engine/core/include/entropy8/` – C API: `entropy8.h`, `io.h`, `config.h`.
- `engine/core/src/` – C: `io.c`, `entropy8_api.c`; C++: `engine/` (bridge, archive, thread_pool).
- `engine/bindings/python/entropy8_engine/` – ctypes binding and stream helpers.
- `apps/cli/entropy8_cli/` – CLI (create/list/extract).
- `docs/architecture.md` – Architecture and design choices.
