#!/usr/bin/env python3
"""Pacote do gerador para Android (Termux): scripts do gerador, perfis, o ELF e o GERAR_ISO_ANDROID.sh.
Não leva Python nem ffmpeg (o Termux instala) e nada do jogo original.
Uso: python tools/make_android_kit.py ps2/build/PS2Birds_Classic.elf saida.zip"""
import shutil,sys,tempfile,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from make_windows_kit import TOOLS,PROFILES
def build(elf,out):
    with tempfile.TemporaryDirectory() as folder:
        kit=Path(folder)/'PS2Birds_Gerador'
        (kit/'tools').mkdir(parents=True);(kit/'profiles').mkdir();(kit/'ps2/src').mkdir(parents=True)
        for t in TOOLS:shutil.copyfile(ROOT/'tools'/t,kit/'tools'/t)
        for p in PROFILES:shutil.copyfile(ROOT/'profiles'/p,kit/'profiles'/p)
        shutil.copyfile(ROOT/'ps2/src/sprite_ids.h',kit/'ps2/src/sprite_ids.h')
        shutil.copyfile(elf,kit/'PS2BIRDS.ELF')
        sh=(ROOT/'tools/GERAR_ISO_ANDROID.sh').read_bytes().replace(b'\r\n',b'\n');(kit/'GERAR_ISO_ANDROID.sh').write_bytes(sh)
        (kit/'LEIA-ME_ANDROID.txt').write_bytes((ROOT/'tools/LEIA-ME_ANDROID.txt').read_bytes())
        out=Path(out);out.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for f in sorted(kit.rglob('*')):
                if f.is_file():
                    info=zipfile.ZipInfo.from_file(f,f.relative_to(folder).as_posix())
                    info.external_attr=(0o755 if f.suffix=='.sh' else 0o644)<<16;info.compress_type=zipfile.ZIP_DEFLATED
                    z.writestr(info,f.read_bytes())
    print('Pacote Android pronto:',out,'(%.1f MB)'%(Path(out).stat().st_size/1e6))
if __name__=='__main__':build(sys.argv[1],sys.argv[2])
