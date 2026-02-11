# Docker

All Docker files are in this directory. The build is **always** run from the repository root; the build context must be the repo root.

## .dockerignore

For Docker to apply exclude rules during build, `.dockerignore` must be at the **repository root**. Copy it on first use:

```bash
# From repository root
cp docker/.dockerignore .
```

## Build (from repository root)

```bash
docker build -f docker/Dockerfile -t entropy8:latest .
```

## Run

```bash
docker run --rm entropy8:latest --help
docker run --rm -v "$(pwd)/data:/data" entropy8:latest create /data/archive.e8 /data/file1.txt
docker run --rm -v "$(pwd)/data:/data" entropy8:latest list /data/archive.e8
docker run --rm -v "$(pwd)/data:/data" entropy8:latest extract /data/archive.e8 /data/out
```

On Windows (PowerShell) for volume: `-v "${PWD}/data:/data"`

## Docker Compose (from repository root)

```bash
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm entropy8 create /data/archive.e8 /data/file1.txt
docker compose -f docker/docker-compose.yml run --rm entropy8 list /data/archive.e8
docker compose -f docker/docker-compose.yml run --rm entropy8 extract /data/archive.e8 /data/out
```

Compose mounts the `data` directory at the repo root as `/data` inside the container via `../data`.
