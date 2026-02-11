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

## Run in dev mode (recommended)

Scripts that build the engine if needed and run the CLI (from repo root or any directory):

**Linux / macOS:**
```bash
./scripts/run.sh --help
./scripts/run.sh create archive.e8 file1.txt file2.txt
./scripts/run.sh list archive.e8
./scripts/run.sh extract archive.e8 ./out
```

**Windows (PowerShell):**
```powershell
.\scripts\run.ps1 --help
.\scripts\run.ps1 create archive.e8 file1.txt file2.txt
.\scripts\run.ps1 list archive.e8
.\scripts\run.ps1 extract archive.e8 .\out
```

On first run, if the library is missing the engine is built automatically; `cmake` and a C/C++ compiler are required.

## CLI (manual)

From repo root, when the library is already in `engine/bindings/python/entropy8_engine/`:

```bash
python apps/cli/entropy8_cli/main.py create archive.e8 file1.txt file2.txt
python apps/cli/entropy8_cli/main.py list archive.e8
python apps/cli/entropy8_cli/main.py extract archive.e8 [output_dir]
```

## Docker

All Docker files live in `docker/`. Build from repo root (see `docker/README.md` for details):

```bash
cp docker/.dockerignore .   # once (for exclude rules)
docker build -f docker/Dockerfile -t entropy8:latest .
docker run --rm entropy8:latest --help
docker run --rm -v "$(pwd)/data:/data" entropy8:latest create /data/archive.e8 /data/file1.txt
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm entropy8 create /data/archive.e8 /data/file1.txt
```

## Layout

- `engine/core/include/entropy8/` – C API: `entropy8.h`, `io.h`, `config.h`.
- `engine/core/src/` – C: `io.c`, `entropy8_api.c`; C++: `engine/` (bridge, archive, thread_pool).
- `engine/bindings/python/entropy8_engine/` – ctypes binding and stream helpers.
- `apps/cli/entropy8_cli/` – CLI (create/list/extract).
- `docs/architecture.md` – Architecture and design choices.
