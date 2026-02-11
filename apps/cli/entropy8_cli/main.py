#!/usr/bin/env python3
"""
Entropy8 CLI – create / list / extract .e8 archives.
Usage:
  entropy8 create archive.e8 file1 [file2 ...]
  entropy8 list archive.e8
  entropy8 extract archive.e8 [output_dir]
"""
import argparse
import os
import sys

# Bindings from engine; ensure path is set so entropy8_engine can be imported
_ENGINE_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "engine", "bindings", "python"))
if _ENGINE_ROOT not in sys.path:
    sys.path.insert(0, _ENGINE_ROOT)

from entropy8_engine import Archive, stream_from_file, stream_from_path


def cmd_create(args):
    archive_path = args.archive
    paths = args.files
    if not paths:
        print("create: at least one file required", file=sys.stderr)
        return 1
    out = open(archive_path, "wb")
    out_stream = stream_from_file(out, "wb")
    try:
        with Archive(out_stream, create=True) as ar:
            for path in paths:
                if not os.path.isfile(path):
                    print(f"skip (not file): {path}", file=sys.stderr)
                    continue
                name = os.path.basename(path)
                with open(path, "rb") as f:
                    content = stream_from_file(f, "rb")
                    ar.add(name, content)
    finally:
        # Archive.close() already destroys out_stream and closes the file
        pass
    print(f"Created {archive_path} with {len(paths)} file(s).")
    return 0


def cmd_list(args):
    archive_path = args.archive
    if not os.path.isfile(archive_path):
        print(f"Not found: {archive_path}", file=sys.stderr)
        return 1
    stream = stream_from_path(archive_path, "rb")
    try:
        with Archive(stream, create=False) as ar:
            n = ar.count()
            print(f"Entries: {n}")
            for i in range(n):
                path, size = ar.entry(i)
                print(f"  {path}  ({size} bytes)")
    finally:
        from ctypes import byref
        from entropy8_engine.core import _bind
        _bind().e8_stream_destroy(byref(stream))
    return 0


def cmd_extract(args):
    archive_path = args.archive
    out_dir = args.output_dir or "."
    if not os.path.isfile(archive_path):
        print(f"Not found: {archive_path}", file=sys.stderr)
        return 1
    os.makedirs(out_dir, exist_ok=True)
    stream = stream_from_path(archive_path, "rb")
    try:
        with Archive(stream, create=False) as ar:
            for i in range(ar.count()):
                path, _ = ar.entry(i)
                safe = os.path.join(out_dir, os.path.basename(path))
                with open(safe, "wb") as f:
                    out_stream = stream_from_file(f, "wb")
                    ar.extract(i, out_stream)
                print(f"Extracted: {safe}")
    finally:
        from ctypes import byref
        from entropy8_engine.core import _bind
        _bind().e8_stream_destroy(byref(stream))
    return 0


def main():
    p = argparse.ArgumentParser(description="Entropy8 – create/list/extract archives")
    sub = p.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("create", help="Create archive")
    c.add_argument("archive", help="Output .e8 path")
    c.add_argument("files", nargs="+", help="Files to add")
    l = sub.add_parser("list", help="List archive contents")
    l.add_argument("archive", help="Archive path")
    e = sub.add_parser("extract", help="Extract archive")
    e.add_argument("archive", help="Archive path")
    e.add_argument("output_dir", nargs="?", help="Output directory (default: .)")
    args = p.parse_args()
    if args.cmd == "create":
        return cmd_create(args)
    if args.cmd == "list":
        return cmd_list(args)
    if args.cmd == "extract":
        return cmd_extract(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
