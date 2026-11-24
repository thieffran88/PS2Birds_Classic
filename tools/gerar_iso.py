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
PROJECTS=json.loads((ROOT/'profiles/projects.json').read_text())

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

STEPS=['Fases (campanha)','Texturas (sprites)','Sons e música','Interface','Letras','Menu','Fundos (temas)','Quadrinhos (cenas)','Montar a ISO']

STEPS_RIO=['Análise do APK','Fases (campanha)','Texturas (sprites)','Fundos (temas)','Sons, música e interface','Montar a ISO']

def generate_rio(apk_path,out_dir,ffmpeg=None,log=print,progress=None):
    """Rio: analyses the APK (recovers the scripts as data, catalogues pieces and sprites in work/rio, kept for the next run), then writes
    CAMPAIGN.BIN, SPRITES.BIN, THEMES.BIN, SOUNDS.BIN, MUSIC.BIN, FONT.BIN, UI.BIN and MENU.BIN (the Rio's own font, interface and menu
    graphics). The Rio has no CUTSCENE.BIN yet. Returns the list of warnings (empty when the sprite tables match the ELF)."""
    sys.path.insert(0,str(HERE/'rio'))
    from rio.investigate_rio import investigate
    import build_rio_data
    def etapa(i):
        if progress:progress.step(i)
        else:log(f'[{i+1}/{len(STEPS_RIO)}] {STEPS_RIO[i]} ...')
    out_dir=Path(out_dir);out_dir.mkdir(parents=True,exist_ok=True);warnings=[]
    etapa(0);t=time.time()
    done=ROOT/'work/rio/analysis/recovery.json'
    mine=hashlib.sha256(Path(apk_path).read_bytes()).hexdigest()
    try:ready=done.is_file() and json.loads(done.read_text()).get('sha256')==mine and (ROOT/'work/rio/analysis/definition_catalogue.json').is_file()
    except Exception:ready=False
    if ready:log('    análise já feita antes para este APK: reaproveitada')
    else:investigate(apk_path)
    log(f'    ok ({time.time()-t:.0f} s)')
    with tempfile.TemporaryDirectory(prefix='ps2birds-ids-') as ids:
        build_rio_data.build(apk_path,out_dir,build_rio_data.DEFAULT_PACKS,find_ffmpeg(ffmpeg),etapa=lambda i:etapa(i),ids_dir=ids)
        shipped=ROOT/'ps2/src/rio'
        for name in ('sprite_ids.h','definition_sprites.h'):
            a,b=Path(ids)/name,shipped/name
            if b.is_file() and a.read_bytes()!=b.read_bytes():warnings.append(f'{name} gerado difere do compilado no ELF: os sprites podem aparecer trocados (use o ELF do mesmo pacote).')
    return warnings

def generate(apk_path,out_dir,ffmpeg=None,log=print,dev=False,progress=None):
    """Writes CAMPAIGN.BIN, SPRITES.BIN, SOUNDS.BIN, MUSIC.BIN, UI.BIN, FONT.BIN, MENU.BIN and THEMES.BIN into out_dir. dev=True keeps the
    intermediate files where the developer build and tests expect them (ps2/assets, ps2/data)."""
    import build_campaign,extract_cutscene,extract_sprites,build_save_icon,pack_sprites,extract_audio,extract_ui,extract_font,extract_menu,extract_themes
    out_dir=Path(out_dir);out_dir.mkdir(parents=True,exist_ok=True)
    def etapa(i):
        if progress:progress.step(i)
        else:log(f'[{i+1}/{len(STEPS)}] {STEPS[i]} ...')
    with tempfile.TemporaryDirectory(prefix='ps2birds-') as work:
        work=Path(work)
        levels=ROOT/'ps2/data' if dev else work/'levels';assets=ROOT/'ps2/assets' if dev else work/'assets'
        levels.mkdir(parents=True,exist_ok=True);assets.mkdir(parents=True,exist_ok=True)
        t=time.time();etapa(0)
        with zipfile.ZipFile(apk_path) as apk:build_campaign.build(apk=apk,workdir=levels,output=out_dir/'CAMPAIGN.BIN',quiet=True)
        log(f'    ok ({time.time()-t:.0f} s)');t=time.time();etapa(1)
        extract_sprites.extract(apk_path,assets,None,'classic',levels)
        build_save_icon.build(assets,write_sys=False)
        if dev:
            # A developer adding pieces or episodes regenerates the identifiers compiled into the game.
            shutil.copyfile(assets/'sprite_ids.h',ROOT/'ps2/src/sprite_ids.h');build_campaign.write_definition_sprites()
        pack_sprites.pack(assets/'manifest.json',assets/'sprites.rgba',assets/'save.icn',ROOT/'ps2/src/sprite_ids.h',out_dir/'SPRITES.BIN')
        log(f'    ok ({time.time()-t:.0f} s)');t=time.time();etapa(2)
        t0=time.time();extract_audio.extract(apk_path,assets,find_ffmpeg(ffmpeg),None);log(f'    ok ({time.time()-t0:.0f} s)')
        def parte(i,fn,*args):
            etapa(i);t0=time.time();fn(*args);log(f'    ok ({time.time()-t0:.0f} s)')
        parte(3,extract_ui.extract,apk_path,assets/'UI.BIN');parte(4,extract_font.extract,apk_path,assets/'FONT.BIN');parte(5,extract_menu.extract,apk_path,assets/'MENU.BIN')
        parte(6,extract_themes.extract,apk_path,assets/'THEMES.BIN');parte(7,extract_cutscene.extract,apk_path,assets/'CUTSCENE.BIN')
        for name in ['SOUNDS.BIN','MUSIC.BIN','UI.BIN','FONT.BIN','MENU.BIN','THEMES.BIN','CUTSCENE.BIN']:
            if Path(assets/name).resolve()!=Path(out_dir/name).resolve():shutil.copyfile(assets/name,out_dir/name)

def main_rio(a,p,disc_id):
    """ISO of the Rio (experimental: first episode and the first Jungle world; no menu art yet)."""
    if a.dev:p.error('--dev não está disponível para o Rio.')
    if a.saida==Path(f'{disc_id}.PS2Birds_Classic.iso'):a.saida=Path(f'{disc_id}.PS2Birds_Rio.iso')
    elf=a.elf or next((c for c in [HERE/'PS2BIRDS_RIO.ELF',ROOT/'PS2BIRDS_RIO.ELF',ROOT/'ps2/build/PS2Birds_Rio.elf'] if c.exists()),None)
    if not elf or Path(elf).read_bytes()[:4]!=b'\x7fELF':p.error('PS2BIRDS_RIO.ELF não encontrado (use --elf com o ELF do Rio).')
    import progresso
    real_out=sys.stdout
    def to_console(line):
        real_out.write(line+'\n');real_out.flush()
    prog=progresso.Progress(STEPS_RIO,logfile=a.log,panel=a.painel,echo=to_console);sys.stdout=progresso.Tee(prog,real_out)
    prog.write(f'Registro completo: {a.log.resolve()}')
    try:
        with tempfile.TemporaryDirectory(prefix='ps2birds-rio-') as data:
            data=Path(data);warnings=generate_rio(a.apk,data,a.ffmpeg,progress=prog,log=prog.write)
            for w in warnings:prog.write('AVISO: '+w)
            files=[data/n for n in ('SPRITES.BIN','SOUNDS.BIN','MUSIC.BIN','THEMES.BIN','CAMPAIGN.BIN','FONT.BIN','UI.BIN','MENU.BIN') if (data/n).is_file()]
            import make_iso
            prog.step(5);a.saida.parent.mkdir(parents=True,exist_ok=True)
            make_iso.write_with_pycdlib(Path(elf).read_bytes(),a.saida.resolve(),files,disc_id)
            prog.write(f'ISO pronta: {a.saida.resolve()}')
            if a.pasta:
                a.pasta.mkdir(parents=True,exist_ok=True);shutil.copyfile(elf,a.pasta/'PS2BIRDS.ELF')
                (a.pasta/'title.cfg').write_text('title=PS2Birds Rio\nboot=PS2BIRDS.ELF\n',encoding='ascii')
                for f in files:shutil.copyfile(f,a.pasta/f.name)
                prog.write(f'Pasta pronta: {a.pasta.resolve()}')
        prog.finish(f'ISO: {a.saida.resolve()}')
    except BaseException as e:
        prog.fail(f'{type(e).__name__}: {e}');raise
    finally:
        sys.stdout=real_out;prog.close()

def main():
    p=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('apk',type=Path,help='APK original do jogo selecionado')
    p.add_argument('--jogo',choices=['auto','classic','rio'],default='auto',help='perfil do jogo (padrão: detectar pelo SHA-256)')
    p.add_argument('--investigar',action='store_true',help='auditar dados do Rio; não gera ELF/ISO')
    p.add_argument('--saida',type=Path,default=None,help='ISO a criar (padrão: <ID do disco>.PS2Birds_Classic.iso, o nome que o OPL espera)')
    p.add_argument('--id',default=None,help='ID do disco, no formato SLUS_000.10 (padrão: o do jogo escolhido)')
    p.add_argument('--pasta',type=Path,default=Path('apps')/'PS2Birds',help='pasta onde também são gravados o ELF e os arquivos de dados, para USB/OPL/HDD (padrão: apps/PS2Birds)')
    p.add_argument('--elf',type=Path,default=None,help='programa do jogo (padrão: PS2BIRDS.ELF ao lado da ferramenta)')
    p.add_argument('--ffmpeg',default=None);p.add_argument('--ignorar-versao',action='store_true')
    p.add_argument('--painel',action='store_true',help='abre no navegador um painel com as etapas, o tempo e o registro detalhado')
    p.add_argument('--log',type=Path,default=Path('GERAR_ISO_LOG.txt'),help='arquivo de registro (padrão: GERAR_ISO_LOG.txt)')
    p.add_argument('--dev',action='store_true',help='desenvolvimento: só gera os dados em ps2/assets e ps2/data (para make/make test)')
    a=p.parse_args()
    if not a.apk.is_file():p.error(f'APK não encontrado: {a.apk}')
    selected=a.jogo
    digest=hashlib.sha256(a.apk.read_bytes()).hexdigest()
    real=next((name for name,profile in PROJECTS.items() if profile['sha256']==digest),None)
    if selected=='auto':selected=real or 'classic'
    elif real and real!=selected and not a.ignorar_versao:
        p.error(f"Você escolheu {PROJECTS[selected]['title']}, mas este APK é de {PROJECTS[real]['title']}. Use o APK do jogo escolhido ou troque a escolha.")
    disc_id=a.id or PROJECTS[selected].get('disc_id','SLUS_000.10')
    if a.saida is None:a.saida=Path(f'{disc_id}.PS2Birds_Classic.iso')
    if selected=='rio' and not a.investigar:
        main_rio(a,p,disc_id);return
    if selected=='rio':
        if a.dev:p.error('--dev não está disponível para a investigação Rio.')
        from rio.investigate_rio import investigate
        investigate(a.apk);return
    if a.investigar:p.error('--investigar está disponível para o perfil Rio.')
    if a.dev:
        if not a.ignorar_versao:check_apk(a.apk)
        generate(a.apk,ROOT/'ps2/assets',a.ffmpeg,dev=True);print('Dados de desenvolvimento gerados em ps2/assets e ps2/data.');return
    elf=a.elf or next((c for c in [HERE/'PS2BIRDS.ELF',ROOT/'PS2BIRDS.ELF',ROOT/'ps2/build/PS2Birds_Classic.elf'] if c.exists()),None)
    if not elf or Path(elf).read_bytes()[:4]!=b'\x7fELF':p.error('PS2BIRDS.ELF não encontrado (use --elf).')
    if not a.ignorar_versao:check_apk(a.apk)
    import progresso
    real_out=sys.stdout   # o console recebe as linhas direto; a saída dos extratores passa pelo Tee só para o registro e o painel
    def to_console(line):
        real_out.write(line+'\n');real_out.flush()
    prog=progresso.Progress(STEPS,logfile=a.log,panel=a.painel,echo=to_console);sys.stdout=progresso.Tee(prog,real_out)
    prog.write(f'Registro completo: {a.log.resolve()}');prog.write('Se uma etapa demorar, o programa continua escrevendo "ainda trabalhando" a cada poucos segundos.')
    try:
        with tempfile.TemporaryDirectory(prefix='ps2birds-data-') as data:
            data=Path(data);generate(a.apk,data,a.ffmpeg,progress=prog,log=prog.write)
            files=[data/'SPRITES.BIN',data/'SOUNDS.BIN',data/'MUSIC.BIN',data/'UI.BIN',data/'FONT.BIN',data/'MENU.BIN',data/'THEMES.BIN',data/'CUTSCENE.BIN',data/'CAMPAIGN.BIN']
            import make_iso
            prog.step(8);a.saida.parent.mkdir(parents=True,exist_ok=True)
            make_iso.write_with_pycdlib(Path(elf).read_bytes(),a.saida.resolve(),files,disc_id)
            prog.write(f'ISO pronta: {a.saida.resolve()}')
            if a.pasta:
                a.pasta.mkdir(parents=True,exist_ok=True);shutil.copyfile(elf,a.pasta/'PS2BIRDS.ELF')
                # title.cfg: o OPL lista a pasta como aplicativo (menu Apps) quando ela tem este arquivo
                (a.pasta/'title.cfg').write_text('title=PS2Birds Classic\nboot=PS2BIRDS.ELF\n',encoding='ascii')
                for f in files:shutil.copyfile(f,a.pasta/f.name)
                prog.write(f'Pasta pronta: {a.pasta.resolve()} (copie a pasta inteira; o ELF lê os .BIN ao lado dele)')
        prog.finish(f'ISO: {a.saida.resolve()}')
    except BaseException as e:
        prog.fail(f'{type(e).__name__}: {e}');raise
    finally:
        sys.stdout=real_out;prog.close()

if __name__=='__main__':main()
