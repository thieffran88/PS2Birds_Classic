#!/usr/bin/env python3
"""Build a PS2 DVD ISO containing a self-contained ELF and SYSTEM.CNF.

Uses genisoimage/mkisofs when available; otherwise falls back to the pure
Python pycdlib writer so the build does not depend on system packages.
"""
import argparse
from pathlib import Path
import io
import shutil
import subprocess
import tempfile

SYSTEM_CNF = b"BOOT2 = cdrom0:\\PS2BIRDS.ELF;1\r\nVER = 1.00\r\nVMODE = NTSC\r\n"


def write_with_pycdlib(elf_bytes, destination, extras=()):
    import pycdlib
    iso = pycdlib.PyCdlib()
    iso.new(interchange_level=1, vol_ident="PS2BIRDS_CLASSIC")
    iso.add_fp(io.BytesIO(SYSTEM_CNF), len(SYSTEM_CNF), "/SYSTEM.CNF;1")
    iso.add_fp(io.BytesIO(elf_bytes), len(elf_bytes), "/PS2BIRDS.ELF;1")
    for extra in extras:
        data = extra.read_bytes()
        iso.add_fp(io.BytesIO(data), len(data), "/" + extra.name.upper() + ";1")
    iso.write(str(destination))
    iso.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("iso", type=Path)
    parser.add_argument("--extra", type=Path, action="append", default=[], help="Arquivo adicional na raiz do disco (ex.: SPRITES.BIN)")
    parser.add_argument("--writer", choices=["auto", "genisoimage", "pycdlib"], default="auto")
    args = parser.parse_args()
    elf_bytes = args.elf.read_bytes()
    if elf_bytes[:4] != b"\x7fELF":
        parser.error("Entrada nao e ELF")
    destination = args.iso.resolve()
    destination.parent.mkdir(parents=True, exist_ok=True)
    tool = shutil.which("genisoimage") or shutil.which("mkisofs")
    if args.writer == "pycdlib" or (args.writer == "auto" and not tool):
        write_with_pycdlib(elf_bytes, destination, args.extra)
    else:
        if not tool:
            parser.error("Instale genisoimage ou mkisofs e adicione ao PATH")
        with tempfile.TemporaryDirectory(prefix="ps2birds-iso-") as folder:
            root = Path(folder)
            shutil.copyfile(args.elf, root / "PS2BIRDS.ELF")
            (root / "SYSTEM.CNF").write_bytes(SYSTEM_CNF)
            for extra in args.extra:
                shutil.copyfile(extra, root / extra.name.upper())
            subprocess.run([tool, "-quiet", "-iso-level", "1", "-V", "PS2BIRDS_CLASSIC",
                            "-o", str(destination), str(root)], check=True)
    print(destination)


if __name__ == "__main__":
    main()
