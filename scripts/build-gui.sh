#!/usr/bin/env bash
# Entropy8 GUI – Native Build Script (Linux / macOS)
# Requires: CMake, C/C++ toolchain, liblz4-dev liblzma-dev libzstd-dev libglfw3-dev
#
# Usage:
#   ./scripts/build-gui.sh           # Build Release
#   ./scripts/build-gui.sh --debug   # Build Debug
#   ./scripts/build-gui.sh --run     # Build + Run

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT/build"
BUILD_TYPE="Release"
RUN=0

for arg in "$@"; do
  case "$arg" in
    --debug) BUILD_TYPE="Debug" ;;
    --run)   RUN=1 ;;
    --clean) rm -rf "$BUILD_DIR" ;;
  esac
done

# Check cmake
if ! command -v cmake &>/dev/null; then
  echo "ERROR: CMake not found. Install with: sudo apt install cmake"
  exit 1
fi

mkdir -p "$BUILD_DIR"

echo "==> Configuring ($BUILD_TYPE)..."
cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

echo "==> Building..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j "$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

echo "==> Build successful!"

if [ "$RUN" -eq 1 ]; then
  exe="$BUILD_DIR/bin/entropy8_gui"
  if [ -x "$exe" ]; then
    echo "==> Running $exe..."
    "$exe"
  else
    echo "ERROR: entropy8_gui not found at $exe"
    exit 1
  fi
fi
