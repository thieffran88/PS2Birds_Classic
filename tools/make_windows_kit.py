#!/usr/bin/env python3
"""Monta o kit "Gerador de ISO" para Windows (uso do desenvolvedor, no Linux ou Windows).

O kit leva o programa do jogo (PS2BIRDS.ELF, sem dados do jogo original), as ferramentas de
conversão, o Python oficial para Windows (pacote NuGet "python") e duas bibliotecas
(pycdlib para gravar a ISO; imageio-ffmpeg, que traz o ffmpeg para ler os sons Ogg).
O jogador só acrescenta o próprio APK.

Uso: python tools/make_windows_kit.py ps2/build/PS2Birds_Classic.elf saida.zip [--cache DIR]
"""
import argparse,io,shutil,subprocess,sys,tempfile,urllib.request,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
PYTHON='3.12.10'
WHEELS=['pycdlib==1.22.0','imageio-ffmpeg==0.8.1']
TOOLS=['gerar_iso.py','build_campaign.py','import_level.py','lua51_data.py','game_profiles.py','aes_cbc.py','mini7z.py',
       'extract_sprites.py','png_decode.py','build_save_icon.py','pack_sprites.py','extract_audio.py','spu_adpcm.py','make_iso.py','extract_ui.py','extract_font.py','extract_menu.py','extract_themes.py','extract_cutscene.py']
PROFILES=['egg_defender.json','games.json','classic_level.json','classic_episode.json','sprite_metrics.json','star_limits.json','supported_apk.json']
BAT=r'''@echo off
chcp 65001 >nul
title PS2Birds Classic - Gerador de ISO
cd /d "%~dp0"
set "APK=%~1"
if "%APK%"=="" for %%f in (*.apk) do set "APK=%%f"
if "%APK%"=="" (
  echo.
  echo  Coloque o APK original nesta pasta ^(ou arraste-o sobre GERAR_ISO.bat^) e execute de novo.
  echo.
  pause
  exit /b 1
)
echo.
echo  APK: %APK%
echo.
"python\python.exe" -I tools\gerar_iso.py "%APK%" --saida "PS2Birds_Classic.iso" --pasta "PS2Birds_Classic_USB"
echo.
pause
'''

def fetch(url,cache):
    target=cache/url.rsplit('/',1)[1]
    if not target.exists():
        print('baixando',url);target.write_bytes(urllib.request.urlopen(url).read())
    return target

def build(elf,out,cache):
    cache.mkdir(parents=True,exist_ok=True)
    if Path(elf).read_bytes()[:4]!=b'\x7fELF':raise SystemExit('ELF inválido')
    nupkg=fetch(f'https://api.nuget.org/v3-flatcontainer/python/{PYTHON}/python.{PYTHON}.nupkg',cache)
    wheels=cache/'wheels';wheels.mkdir(exist_ok=True)
    subprocess.run([sys.executable,'-m','pip','download','--quiet','--only-binary=:all:','--platform','win_amd64','--python-version',PYTHON.rsplit('.',1)[0],'--no-deps','-d',str(wheels)]+WHEELS,check=True)
    with tempfile.TemporaryDirectory() as folder:
        kit=Path(folder)/'PS2Birds_Classic_Gerador'
        with zipfile.ZipFile(nupkg) as z:
            for name in z.namelist():
                if name.startswith('tools/') and not name.endswith('/'):
                    dest=kit/'python'/name[len('tools/'):];dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(z.read(name))
        site=kit/'python/Lib/site-packages';site.mkdir(parents=True,exist_ok=True)
        for w in sorted(wheels.glob('*.whl')):
            if not any(w.name.lower().startswith(x.split('==')[0].replace('-','_')) for x in WHEELS):continue
            with zipfile.ZipFile(w) as z:z.extractall(site)
        (kit/'tools').mkdir();(kit/'profiles').mkdir();(kit/'ps2/src').mkdir(parents=True)
        for t in TOOLS:shutil.copyfile(ROOT/'tools'/t,kit/'tools'/t)
        for p in PROFILES:shutil.copyfile(ROOT/'profiles'/p,kit/'profiles'/p)
        shutil.copyfile(ROOT/'ps2/src/sprite_ids.h',kit/'ps2/src/sprite_ids.h')
        shutil.copyfile(elf,kit/'PS2BIRDS.ELF')
        (kit/'GERAR_ISO.bat').write_bytes(BAT.replace('\n','\r\n').encode('utf-8'))
        (kit/'LEIA-ME.txt').write_bytes((ROOT/'tools/LEIA-ME_GERADOR.txt').read_text(encoding='utf-8').replace('\n','\r\n').encode('utf-8'))
        out=Path(out);out.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for f in sorted(kit.rglob('*')):
                if f.is_file():z.write(f,f.relative_to(kit.parent))
    print(f'Kit pronto: {out} ({out.stat().st_size//1048576} MB)')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('elf');p.add_argument('saida');p.add_argument('--cache',default=str(ROOT/'ps2/build/kit-cache'))
    a=p.parse_args();build(a.elf,a.saida,Path(a.cache))
