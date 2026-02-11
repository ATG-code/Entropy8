#!/usr/bin/env bash
# Entropy8 – run in dev mode via Docker. No local Python or C++ toolchain needed.
# Usage: scripts/run.sh [create|list|extract] [arg ...]
# Example: ./scripts/run.sh create archive.e8 file1.txt
#          ./scripts/run.sh list archive.e8
#          ./scripts/run.sh extract archive.e8 ./out

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(dirname "$SCRIPT_DIR")"
cd "$ROOT"

IMAGE_NAME="entropy8:dev"
DOCKERFILE="$ROOT/docker/Dockerfile"

# ── Check Docker ──────────────────────────────────────────────────────────────
if ! command -v docker &>/dev/null; then
  echo ""
  echo "ERROR: Docker not found."
  echo "Install Docker from https://docs.docker.com/get-docker/"
  echo ""
  exit 1
fi

# ── Check Docker daemon is running ────────────────────────────────────────────
if ! docker info &>/dev/null; then
  echo ""
  echo "ERROR: Docker daemon is not running."
  echo "Start Docker Desktop (or the docker service), then try again."
  echo ""
  exit 1
fi

# ── Build image if it doesn't exist ──────────────────────────────────────────
if [ -z "$(docker images -q "$IMAGE_NAME" 2>/dev/null)" ]; then
  echo "Building Docker image ($IMAGE_NAME)..."
  docker build -f "$DOCKERFILE" -t "$IMAGE_NAME" "$ROOT"
  echo "Image built."
fi

# ── Run ──────────────────────────────────────────────────────────────────────
# Mount current directory as /workspace so files are read/written locally.
WORK_DIR="$(pwd)"
exec docker run --rm -v "$WORK_DIR:/workspace" -w /workspace "$IMAGE_NAME" "$@"
