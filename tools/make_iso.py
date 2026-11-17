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

# The executable on the disc is named like a PS2 disc ID (SLUS_000.10): that is what the OPL loader (and other
# launchers) read from SYSTEM.CNF and from the name of the .iso file to list and start the game.
DEFAULT_DISC_ID = "SLUS_000.10"


def system_cnf(disc_id=DEFAULT_DISC_ID):
    return ("BOOT2 = cdrom0:\\%s;1\r\nVER = 1.00\r\nVMODE = NTSC\r\n" % disc_id).encode("ascii")


def write_with_pycdlib(elf_bytes, destination, extras=(), disc_id=DEFAULT_DISC_ID):
    import pycdlib
    cnf = system_cnf(disc_id)
    iso = pycdlib.PyCdlib()
    iso.new(interchange_level=1, vol_ident="PS2BIRDS_CLASSIC")
    iso.add_fp(io.BytesIO(cnf), len(cnf), "/SYSTEM.CNF;1")
    iso.add_fp(io.BytesIO(elf_bytes), len(elf_bytes), "/%s;1" % disc_id)
    handles = []
    try:
        for extra in extras:
            # the file is read while the ISO is written, not loaded beforehand (phones have little memory)
            fp = open(extra, "rb")
            handles.append(fp)
            iso.add_fp(fp, extra.stat().st_size, "/" + extra.name.upper() + ";1")
        iso.write(str(destination))
    finally:
        iso.close()
        for fp in handles:
            fp.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("iso", type=Path)
    parser.add_argument("--id", default=DEFAULT_DISC_ID, help="ID do disco (padrão: %s)" % DEFAULT_DISC_ID)
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
        write_with_pycdlib(elf_bytes, destination, args.extra, args.id)
    else:
        if not tool:
            parser.error("Instale genisoimage ou mkisofs e adicione ao PATH")
        with tempfile.TemporaryDirectory(prefix="ps2birds-iso-") as folder:
            root = Path(folder)
            shutil.copyfile(args.elf, root / args.id)
            (root / "SYSTEM.CNF").write_bytes(system_cnf(args.id))
            for extra in args.extra:
                shutil.copyfile(extra, root / extra.name.upper())
            subprocess.run([tool, "-quiet", "-iso-level", "1", "-V", "PS2BIRDS_CLASSIC",
                            "-o", str(destination), str(root)], check=True)
    print(destination)


if __name__ == "__main__":
    main()
