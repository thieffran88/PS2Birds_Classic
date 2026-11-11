#!/usr/bin/env python3
"""Build a PS2 DVD ISO containing a self-contained ELF and SYSTEM.CNF."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("elf", type=Path)
    parser.add_argument("iso", type=Path)
    args = parser.parse_args()
    tool = shutil.which("genisoimage") or shutil.which("mkisofs")
    if not tool:
        parser.error("Instale genisoimage ou mkisofs e adicione ao PATH")
    if args.elf.read_bytes()[:4] != b"\x7fELF":
        parser.error("Entrada nao e ELF")
    destination = args.iso.resolve()
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="ps2birds-iso-") as folder:
        root = Path(folder)
        shutil.copyfile(args.elf, root / "PS2BIRDS.ELF")
        (root / "SYSTEM.CNF").write_bytes(
            b"BOOT2 = cdrom0:\\PS2BIRDS.ELF;1\r\nVER = 1.00\r\nVMODE = NTSC\r\n")
        subprocess.run([tool, "-quiet", "-iso-level", "1", "-V", "PS2BIRDS_CLASSIC",
                        "-o", str(destination), str(root)], check=True)
    print(destination)


if __name__ == "__main__":
    main()
