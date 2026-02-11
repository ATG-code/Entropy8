"""
Entropy8 Python binding – C API (ABI-stable) wrapped for Python.
Use core.Archive, core.stream_from_path, core.stream_from_file for create/extract/list.
"""
from .core import (
    E8_OK,
    E8_ERR_IO,
    E8_ERR_FORMAT,
    E8_ERR_MEMORY,
    E8_ERR_INVALID_ARG,
    E8_ERR_NOT_FOUND,
    E8_ERR_UNSUPPORTED,
    Archive,
    stream_from_file,
    stream_from_path,
)

__all__ = [
    "Archive",
    "stream_from_file",
    "stream_from_path",
    "E8_OK",
    "E8_ERR_IO",
    "E8_ERR_FORMAT",
    "E8_ERR_MEMORY",
    "E8_ERR_INVALID_ARG",
    "E8_ERR_NOT_FOUND",
    "E8_ERR_UNSUPPORTED",
]
