# -----------------------------------------------------------------------------
# Stage 1: Build entropy8 engine (C/C++)
# -----------------------------------------------------------------------------
FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY engine/core/CMakeLists.txt engine/core/
COPY engine/core/include engine/core/include
COPY engine/core/src engine/core/src

RUN cd engine/core \
    && mkdir -p build && cd build \
    && cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build .

# -----------------------------------------------------------------------------
# Stage 2: Runtime image with Python + CLI
# -----------------------------------------------------------------------------
FROM python:3.12-slim-bookworm

WORKDIR /app

# Copy built shared library (Linux: libentropy8.so or libentropy8.so.1)
COPY --from=builder /build/engine/core/build/libentropy8.so* engine/bindings/python/entropy8_engine/

# Copy Python binding and CLI
COPY engine/bindings/python/ engine/bindings/python/
COPY apps/cli/ apps/cli/

# Ensure library is found: prefer local dir, then LD_LIBRARY_PATH
ENV PYTHONPATH=/app/engine/bindings/python
ENV LD_LIBRARY_PATH=/app/engine/bindings/python/entropy8_engine

# Default: run CLI
ENTRYPOINT ["python", "apps/cli/entropy8_cli/main.py"]
CMD ["--help"]
