# Docker (CLI)

Container-based workflow for the Entropy8 CLI. No local C/C++ toolchain or Python required.

## Prerequisites

- Docker Desktop (Windows/macOS) or Docker Engine (Linux)

## .dockerignore

For Docker to apply exclude rules during build, `.dockerignore` must be at the repository root:

```bash
cp docker/.dockerignore .   # one-time setup
```

## Build

From the repository root:

```bash
docker build -f docker/Dockerfile -t entropy8:latest .
```

## Usage

```bash
# Help
docker run --rm entropy8:latest --help

# Create archive
docker run --rm -v "$(pwd):/workspace" -w /workspace entropy8:latest create archive.e8 file1.txt file2.txt

# List contents
docker run --rm -v "$(pwd):/workspace" -w /workspace entropy8:latest list archive.e8

# Extract
docker run --rm -v "$(pwd):/workspace" -w /workspace entropy8:latest extract archive.e8 ./out
```

**Windows (PowerShell):**
```powershell
docker run --rm -v "${PWD}:/workspace" -w /workspace entropy8:latest create archive.e8 file1.txt
```

## Docker Compose

```bash
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm entropy8 create /data/archive.e8 /data/file1.txt
docker compose -f docker/docker-compose.yml run --rm entropy8 list /data/archive.e8
docker compose -f docker/docker-compose.yml run --rm entropy8 extract /data/archive.e8 /data/out
```

Compose mounts the `data/` directory at the repo root as `/data` inside the container.

## Convenience Scripts

Instead of running Docker commands manually, use the wrapper scripts from the repo root:

```powershell
# Windows
.\scripts\run.ps1 create archive.e8 file1.txt

# Linux/macOS
./scripts/run.sh create archive.e8 file1.txt
```

These scripts automatically build the Docker image on first run and handle volume mounting.
