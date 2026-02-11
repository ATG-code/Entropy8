"""
Entropy8 engine – ctypes binding to the C API (entropy8.h, io.h).
Loads libentropy8 (entropy8.dll / libentropy8.so) and exposes archive create/open/add/extract/list.
"""
import ctypes
import os
import sys
from ctypes import POINTER, byref, c_char_p, c_int, c_int64, c_size_t, c_uint64, c_void_p, Structure

# Errors
E8_OK = 0
E8_ERR_IO = 1
E8_ERR_FORMAT = 2
E8_ERR_MEMORY = 3
E8_ERR_INVALID_ARG = 4
E8_ERR_NOT_FOUND = 5
E8_ERR_UNSUPPORTED = 6

SEEK_SET = 0
SEEK_CUR = 1
SEEK_END = 2


class E8StreamVtable(Structure):
    _fields_ = [
        ("read", c_void_p),
        ("write", c_void_p),
        ("seek", c_void_p),
        ("flush", c_void_p),
        ("close", c_void_p),
    ]


class E8Stream(Structure):
    _fields_ = [
        ("vtable", POINTER(E8StreamVtable)),
        ("ctx", c_void_p),
    ]


# Callback types
E8ReadFn = ctypes.CFUNCTYPE(ctypes.c_ssize_t, c_void_p, c_void_p, c_size_t)
E8WriteFn = ctypes.CFUNCTYPE(ctypes.c_ssize_t, c_void_p, c_void_p, c_size_t)
E8SeekFn = ctypes.CFUNCTYPE(ctypes.c_int64, c_void_p, ctypes.c_int64, c_int)
E8FlushFn = ctypes.CFUNCTYPE(c_int, c_void_p)
E8CloseFn = ctypes.CFUNCTYPE(c_int, c_void_p)
E8ProgressFn = ctypes.CFUNCTYPE(c_int, c_void_p, c_uint64, c_uint64)


def _find_lib():
    """Find and load the entropy8 shared library. Returns a ctypes.CDLL or raises OSError."""
    base = os.path.dirname(os.path.abspath(__file__))
    if sys.platform == "win32":
        names = ["entropy8.dll", "libentropy8.dll"]
    else:
        names = ["libentropy8.so", "libentropy8.so.1", "libentropy8.dylib"]
    # Try local path first (next to this .py file)
    for n in names:
        p = os.path.join(base, n)
        if os.path.isfile(p):
            return ctypes.CDLL(p)
    # Try system library path
    for n in names:
        try:
            return ctypes.CDLL(n)
        except OSError:
            pass
    raise OSError(
        f"libentropy8 not found. Searched {base} for {names}. "
        "Build the engine first or copy the shared library next to this file."
    )


_lib = None


def _lib_entropy8():
    global _lib
    if _lib is None:
        _lib = _find_lib()
    return _lib


def _bind():
    lib = _lib_entropy8()
    lib.e8_stream_create.argtypes = [POINTER(E8Stream), POINTER(E8StreamVtable), c_void_p]
    lib.e8_stream_create.restype = c_int

    lib.e8_stream_destroy.argtypes = [POINTER(E8Stream)]
    lib.e8_stream_destroy.restype = None

    lib.e8_archive_create.argtypes = [POINTER(E8Stream)]
    lib.e8_archive_create.restype = c_void_p

    lib.e8_archive_open.argtypes = [POINTER(E8Stream)]
    lib.e8_archive_open.restype = c_void_p

    lib.e8_archive_close.argtypes = [c_void_p]
    lib.e8_archive_close.restype = None

    lib.e8_archive_add.argtypes = [
        c_void_p, c_char_p, POINTER(E8Stream),
        c_void_p, c_void_p  # progress callback + user data (both nullable)
    ]
    lib.e8_archive_add.restype = c_int

    lib.e8_archive_extract.argtypes = [
        c_void_p, c_size_t, POINTER(E8Stream),
        c_void_p, c_void_p  # progress callback + user data (both nullable)
    ]
    lib.e8_archive_extract.restype = c_int

    lib.e8_archive_count.argtypes = [c_void_p]
    lib.e8_archive_count.restype = c_size_t

    lib.e8_archive_entry.argtypes = [
        c_void_p, c_size_t, c_char_p, c_size_t, POINTER(c_uint64)
    ]
    lib.e8_archive_entry.restype = c_int

    lib.e8_last_error.argtypes = []
    lib.e8_last_error.restype = c_int

    return lib


def stream_from_file(f, mode="rb"):
    """Build E8Stream from a Python file-like (read/write/seek). Keeps reference to f."""
    buf = ctypes.create_string_buffer(65536)

    def read_fn(ctx, buf_p, size):
        try:
            data = f.read(size)
            if not data:
                return 0
            ctypes.memmove(buf_p, data, len(data))
            return len(data)
        except Exception:
            return -1

    def write_fn(ctx, buf_p, size):
        try:
            data = ctypes.string_at(buf_p, size)
            f.write(data)
            return len(data)
        except Exception:
            return -1

    def seek_fn(ctx, offset, origin):
        try:
            f.seek(offset, origin)
            return f.tell()
        except Exception:
            return -1

    def flush_fn(ctx):
        try:
            f.flush()
            return 0
        except Exception:
            return -1

    def close_fn(ctx):
        try:
            f.close()
            return 0
        except Exception:
            return -1

    read_cb = E8ReadFn(read_fn)
    write_cb = E8WriteFn(write_fn)
    seek_cb = E8SeekFn(seek_fn)
    flush_cb = E8FlushFn(flush_fn)
    close_cb = E8CloseFn(close_fn)

    vtable = E8StreamVtable(
        ctypes.cast(read_cb, c_void_p),
        ctypes.cast(write_cb, c_void_p),
        ctypes.cast(seek_cb, c_void_p),
        ctypes.cast(flush_cb, c_void_p),
        ctypes.cast(close_cb, c_void_p),
    )
    stream = E8Stream(ctypes.pointer(vtable), None)
    stream._callbacks = (read_cb, write_cb, seek_cb, flush_cb, close_cb)
    stream._vtable = vtable
    stream._file = f
    lib = _bind()
    if lib.e8_stream_create(byref(stream), byref(vtable), None) != 0:
        raise RuntimeError("e8_stream_create failed")
    return stream


def stream_from_path(path, mode="rb"):
    """E8Stream from file path. Caller must call e8_stream_destroy when done."""
    f = open(path, mode)
    return stream_from_file(f, mode)


class Archive:
    """Python wrapper for E8Archive."""

    def __init__(self, stream, create=True):
        lib = _bind()
        if create:
            self._handle = lib.e8_archive_create(byref(stream))
        else:
            self._handle = lib.e8_archive_open(byref(stream))
        if not self._handle:
            raise RuntimeError(f"Archive open failed: error {lib.e8_last_error()}")
        self._stream = stream
        self._lib = lib

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.close()

    def close(self):
        if self._handle:
            self._lib.e8_archive_close(self._handle)
            self._handle = None
        if hasattr(self._stream, "_file") and self._stream._file:
            self._lib.e8_stream_destroy(byref(self._stream))

    def add(self, path: str, content_stream):
        r = self._lib.e8_archive_add(
            self._handle, path.encode("utf-8"), byref(content_stream),
            None, None
        )
        if r != E8_OK:
            raise RuntimeError(f"e8_archive_add: error {r}")

    def extract(self, index: int, output_stream, progress=None):
        prog_cb = E8ProgressFn(progress) if progress else None
        r = self._lib.e8_archive_extract(
            self._handle, index, byref(output_stream),
            prog_cb, None
        )
        if r != E8_OK:
            raise RuntimeError(f"e8_archive_extract: error {r}")

    def count(self):
        return self._lib.e8_archive_count(self._handle)

    def entry(self, index: int):
        path_buf = ctypes.create_string_buffer(4096)
        size = c_uint64()
        r = self._lib.e8_archive_entry(
            self._handle, index, path_buf, 4096, byref(size)
        )
        if r != E8_OK:
            raise IndexError(index)
        return (path_buf.value.decode("utf-8"), size.value)

    def list_entries(self):
        return [self.entry(i) for i in range(self.count())]
