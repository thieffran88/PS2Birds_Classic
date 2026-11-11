#!/usr/bin/env python3
"""Inventory an APK and optionally recover Lua chunks without executing them.

Requires Python 3.9+ and the system OpenSSL command for --extract-lua.
Select declared game keys with --game; only Classic has real-APK validation.
This is a host preparation tool, not the PS2 game engine.
"""
import argparse
import collections
import hashlib
import json
import lzma
from pathlib import Path, PurePosixPath
import struct
import subprocess
import shutil
import tempfile
import zipfile

from game_profiles import PROFILES, decode_payload


def recover_lua(data, sevenzip=None, game='classic'):
    data=decode_payload(data,game,sevenzip)
    if not data.startswith(b"\x1bLua"):
        raise ValueError("Recovered payload is not a Lua bytecode chunk")
    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("apk", type=Path)
    parser.add_argument("--game", choices=sorted(PROFILES), default="classic")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--extract-lua", action="store_true")
    parser.add_argument("--sevenzip", default=shutil.which("7z") or shutil.which("7zz"),
                        help="7-Zip executable for standard 7z payloads")
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=True)
    sha = hashlib.sha256()
    with args.apk.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            sha.update(block)
    inventory, recovered, failures, images, native = [], [], [], [], []
    with zipfile.ZipFile(args.apk) as archive:
        corrupt = archive.testzip()
        if corrupt:
            raise ValueError("Invalid ZIP CRC: " + corrupt)
        for entry in archive.infolist():
            if entry.is_dir():
                continue
            path = PurePosixPath(entry.filename)
            if path.is_absolute() or ".." in path.parts or "\\" in entry.filename:
                raise ValueError("Unsafe archive path: " + entry.filename)
            inventory.append({"path": entry.filename, "size": entry.file_size,
                              "compressed_size": entry.compress_size,
                              "crc32": f"{entry.CRC:08x}"})
            if entry.filename.startswith("lib/") and path.suffix == ".so":
                data = archive.read(entry)
                native.append({"path": entry.filename, "elf_class": data[4],
                               "e_machine": struct.unpack("<H", data[18:20])[0]})
            if entry.filename.startswith("assets/data/") and path.suffix == ".png":
                data = archive.read(entry)
                if data.startswith(b"\x89PNG\r\n\x1a\n"):
                    width, height = struct.unpack(">II", data[16:24])
                    images.append({"path": entry.filename, "width": width, "height": height,
                                   "rgba32_bytes": width * height * 4})
            if args.extract_lua and path.suffix == ".lua":
                try:
                    chunk = recover_lua(archive.read(entry), args.sevenzip, args.game)
                    destination = args.output / "lua_bytecode" / Path(*path.parts)
                    destination = destination.with_suffix(".luac")
                    destination.parent.mkdir(parents=True, exist_ok=True)
                    destination.write_bytes(chunk)
                    recovered.append({"path": entry.filename, "bytes": len(chunk),
                                      "header_hex": chunk[:12].hex()})
                except (ValueError, lzma.LZMAError) as error:
                    failures.append({"path": entry.filename, "error": str(error)})
    summary = {
        "game_profile": args.game, "apk": args.apk.name, "sha256": sha.hexdigest(), "zip_crc": "ok",
        "file_count": len(inventory), "uncompressed_bytes": sum(i["size"] for i in inventory),
        "extensions": dict(collections.Counter(PurePosixPath(i["path"]).suffix for i in inventory)),
        "native_libraries": native,
        "level_directories": dict(collections.Counter(
            i["path"].split("/")[3] for i in inventory
            if i["path"].startswith("assets/data/levels/"))),
        "largest_png_rgba32": sorted(images, key=lambda i: i["rgba32_bytes"], reverse=True)[:10],
        "lua_extraction_requested": args.extract_lua,
        "lua_recovered_count": len(recovered), "lua_failures": failures,
        "lua_headers": dict(collections.Counter(i["header_hex"] for i in recovered)),
    }
    for name, value in [("apk_inventory.json", inventory), ("apk_summary.json", summary),
                        ("lua_recovery.json", recovered)]:
        (args.output / name).write_text(json.dumps(value, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    if failures:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
