#!/usr/bin/env bash
# Entropy8 – run in dev mode: build engine if needed, then run CLI.
# Usage: scripts/run.sh [create|list|extract] [arg ...]
# Example: scripts/run.sh create archive.e8 file1.txt
#          scripts/run.sh list archive.e8

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(dirname "$SCRIPT_DIR")"
cd "$ROOT"

BINDINGS_LIB="$ROOT/engine/bindings/python/entropy8_engine"
CORE_BUILD="$ROOT/engine/core/build"
CORE_SRC="$ROOT/engine/core"

# Check if library exists (libentropy8.so, libentropy8.so.1, libentropy8.dylib, libentropy8.dll, etc.)
has_lib() {
  for pat in "$BINDINGS_LIB"/libentropy8.so* "$BINDINGS_LIB"/libentropy8.dylib "$BINDINGS_LIB"/entropy8.dll "$BINDINGS_LIB"/libentropy8.dll; do
    for f in $pat; do
      [ -f "$f" ] && return 0
    done
  done
  return 1
}

# Build engine if library is missing
if ! has_lib; then
  echo "Engine library not found, building..."
  mkdir -p "$CORE_BUILD"
  (cd "$CORE_BUILD" && cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .)
  if [ -f "$CORE_BUILD/libentropy8.so" ]; then
    cp -f "$CORE_BUILD"/libentropy8.so* "$BINDINGS_LIB/"
  elif [ -f "$CORE_BUILD/libentropy8.so.1" ]; then
    cp -f "$CORE_BUILD"/libentropy8.so* "$BINDINGS_LIB/"
  elif [ -f "$CORE_BUILD/libentropy8.dll" ]; then
    cp -f "$CORE_BUILD/libentropy8.dll" "$BINDINGS_LIB/"
  else
    echo "Error: Build finished but library was not found." >&2
    exit 1
  fi
  echo "Engine built."
fi

export PYTHONPATH="$ROOT/engine/bindings/python${PYTHONPATH:+:$PYTHONPATH}"
# Linux: LD_LIBRARY_PATH; macOS: DYLD_LIBRARY_PATH
case "$(uname -s 2>/dev/null)" in
  Linux)  export LD_LIBRARY_PATH="$BINDINGS_LIB${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ;;
  Darwin) export DYLD_LIBRARY_PATH="$BINDINGS_LIB${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}" ;;
esac

exec python3 "$ROOT/apps/cli/entropy8_cli/main.py" "$@"
