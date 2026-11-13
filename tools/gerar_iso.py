#!/usr/bin/env python3
"""PS2Birds Classic — gera a ISO a partir do APK original do jogador.

O projeto não distribui nada do jogo original: fases, texturas, sons e o ícone do
cartão de memória são convertidos aqui, a partir do APK que você fornece, e gravados
na ISO junto com o programa (PS2BIRDS.ELF). Nenhum script do jogo é executado: as
fases são lidas como dados (tools/lua51_data.py).

Uso:  python gerar_iso.py com.rovio.angrybirds.apk [--saida PS2Birds_Classic.iso] [--pasta DIR]
"""
import argparse,hashlib,json,shutil,sys,tempfile,time,zipfile
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE))
ROOT=HERE.parent
SUPPORTED=json.loads((ROOT/'profiles/supported_apk.json').read_text())

def find_ffmpeg(explicit=None):
    if explicit:return explicit
    try:
        import imageio_ffmpeg;return imageio_ffmpeg.get_ffmpeg_exe()
    except Exception:pass
    found=shutil.which('ffmpeg')
    if not found:raise SystemExit('ffmpeg não encontrado (o kit para Windows já o inclui; no Linux instale o pacote ffmpeg).')
    return found

def check_apk(path):
    digest=hashlib.sha256(Path(path).read_bytes()).hexdigest()
    if digest!=SUPPORTED['sha256']:
        raise SystemExit(f"APK diferente do suportado.\n  esperado: {SUPPORTED['name']} ({SUPPORTED['version']}), SHA-256 {SUPPORTED['sha256']}\n  recebido: SHA-256 {digest}\n"
                         "Use o APK da versão indicada; outras versões têm fases e texturas diferentes. (--ignorar-versao força a tentativa.)")

def generate(apk_path,out_dir,ffmpeg=None,log=print,dev=False):
    """Writes CAMPAIGN.BIN, SPRITES.BIN and SOUNDS.BIN into out_dir. dev=True keeps the
    intermediate files where the developer build and tests expect them (ps2/assets, ps2/data)."""
    import build_campaign,extract_sprites,build_save_icon,pack_sprites,extract_audio
    out_dir=Path(out_dir);out_dir.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='ps2birds-') as work:
        work=Path(work)
        levels=ROOT/'ps2/data' if dev else work/'levels';assets=ROOT/'ps2/assets' if dev else work/'assets'
        levels.mkdir(parents=True,exist_ok=True);assets.mkdir(parents=True,exist_ok=True)
        t=time.time();log('1/3 Fases (330) ...')
        with zipfile.ZipFile(apk_path) as apk:build_campaign.build(apk=apk,workdir=levels,output=out_dir/'CAMPAIGN.BIN',quiet=True)
        log(f'    ok ({time.time()-t:.0f} s)');t=time.time();log('2/3 Texturas ...')
        extract_sprites.extract(apk_path,assets,None,'classic',levels)
        build_save_icon.build(assets,write_sys=False)
        pack_sprites.pack(assets/'manifest.json',assets/'sprites.rgba',assets/'save.icn',ROOT/'ps2/src/sprite_ids.h',out_dir/'SPRITES.BIN')
        log(f'    ok ({time.time()-t:.0f} s)');t=time.time();log('3/3 Sons ...')
        extract_audio.extract(apk_path,assets,find_ffmpeg(ffmpeg),None)
        if Path(assets/'SOUNDS.BIN').resolve()!=Path(out_dir/'SOUNDS.BIN').resolve():shutil.copyfile(assets/'SOUNDS.BIN',out_dir/'SOUNDS.BIN')
        log(f'    ok ({time.time()-t:.0f} s)')

def main():
    p=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('apk',type=Path,help='APK original (com.rovio.angrybirds.apk)')
    p.add_argument('--saida',type=Path,default=Path('PS2Birds_Classic.iso'),help='ISO a criar (padrão: PS2Birds_Classic.iso)')
    p.add_argument('--pasta',type=Path,help='também grava ELF e arquivos de dados nesta pasta (para USB/OPL/HDD)')
    p.add_argument('--elf',type=Path,default=None,help='programa do jogo (padrão: PS2BIRDS.ELF ao lado da ferramenta)')
    p.add_argument('--ffmpeg',default=None);p.add_argument('--ignorar-versao',action='store_true')
    p.add_argument('--dev',action='store_true',help='desenvolvimento: só gera os dados em ps2/assets e ps2/data (para make/make test)')
    a=p.parse_args()
    if not a.apk.is_file():p.error(f'APK não encontrado: {a.apk}')
    if a.dev:
        if not a.ignorar_versao:check_apk(a.apk)
        generate(a.apk,ROOT/'ps2/assets',a.ffmpeg,dev=True);print('Dados de desenvolvimento gerados em ps2/assets e ps2/data.');return
    elf=a.elf or next((c for c in [HERE/'PS2BIRDS.ELF',ROOT/'PS2BIRDS.ELF',ROOT/'ps2/build/PS2Birds_Classic.elf'] if c.exists()),None)
    if not elf or Path(elf).read_bytes()[:4]!=b'\x7fELF':p.error('PS2BIRDS.ELF não encontrado (use --elf).')
    if not a.ignorar_versao:check_apk(a.apk)
    with tempfile.TemporaryDirectory(prefix='ps2birds-data-') as data:
        data=Path(data);generate(a.apk,data,a.ffmpeg)
        files=[data/'SPRITES.BIN',data/'SOUNDS.BIN',data/'CAMPAIGN.BIN']
        import make_iso
        a.saida.parent.mkdir(parents=True,exist_ok=True)
        make_iso.write_with_pycdlib(Path(elf).read_bytes(),a.saida.resolve(),files)
        print(f'ISO pronta: {a.saida.resolve()}')
        if a.pasta:
            a.pasta.mkdir(parents=True,exist_ok=True);shutil.copyfile(elf,a.pasta/'PS2BIRDS.ELF')
            for f in files:shutil.copyfile(f,a.pasta/f.name)
            print(f'Pasta pronta: {a.pasta.resolve()} (copie a pasta inteira; o ELF lê os .BIN ao lado dele)')

if __name__=='__main__':main()
