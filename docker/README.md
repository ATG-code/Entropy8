# Docker

Tüm Docker dosyaları bu klasördedir. Build **her zaman proje kökünden** (repository root) alınır; context proje kökü olmalı.

## .dockerignore

Docker build sırasında exclude kurallarının uygulanması için `.dockerignore` **proje kökünde** olmalı. İlk kullanımda kopyalayın:

```bash
# Proje kökünden
cp docker/.dockerignore .
```

## Build (proje kökünden)

```bash
docker build -f docker/Dockerfile -t entropy8:latest .
```

## Çalıştırma

```bash
docker run --rm entropy8:latest --help
docker run --rm -v "$(pwd)/data:/data" entropy8:latest create /data/archive.e8 /data/file1.txt
docker run --rm -v "$(pwd)/data:/data" entropy8:latest list /data/archive.e8
docker run --rm -v "$(pwd)/data:/data" entropy8:latest extract /data/archive.e8 /data/out
```

Windows (PowerShell) için volume: `-v "${PWD}/data:/data"`

## Docker Compose (proje kökünden)

```bash
docker compose -f docker/docker-compose.yml build
docker compose -f docker/docker-compose.yml run --rm entropy8 create /data/archive.e8 /data/file1.txt
docker compose -f docker/docker-compose.yml run --rm entropy8 list /data/archive.e8
docker compose -f docker/docker-compose.yml run --rm entropy8 extract /data/archive.e8 /data/out
```

Compose, `../data` ile proje kökündeki `data` klasörünü container içinde `/data` olarak bağlar.
