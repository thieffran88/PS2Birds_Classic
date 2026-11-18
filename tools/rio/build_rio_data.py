"""Builds the data files of the Rio build in one folder, from the player's own APK: CAMPAIGN.BIN (the stages of the
first episode that the runtime can play), SPRITES.BIN, THEMES.BIN, SOUNDS.BIN and MUSIC.BIN. Experimental: UI.BIN,
FONT.BIN, MENU.BIN and CUTSCENE.BIN of the Rio do not exist yet, so this is not a complete ISO.
Stages whose birds the runtime does not have (only the red bird exists) are listed and left out.
usage: build_rio_data.py APK OUTDIR [--packs warehouse warehouse2] [--ffmpeg ffmpeg]"""
import argparse,json,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
# Episode titles by pack folder (the first episode is the only one whose name is known from the game's menu).
EPISODE={'warehouse':"SMUGGLERS DEN",'warehouse2':"SMUGGLERS DEN"}
LEVELS=ROOT/'work/rio/analysis/chunks/assets/data/levels'
DEFAULT_PACKS=['warehouse','warehouse2','jungle1']   # the stages the shipped ELF's sprite and piece tables (ps2/src/rio) were made for
def build(apk,out,packs,ffmpeg,etapa=None,ids_dir=None):
    """etapa(i): called as each part starts (1 stages, 2 sprites, 3 themes, 4 sounds); ids_dir: where sprite_ids.h and
    definition_sprites.h are written (default ps2/src/rio, which the ELF is compiled with)."""
    etapa=etapa or (lambda i:None);ids_dir=Path(ids_dir) if ids_dir else ROOT/'ps2/src/rio'
    import build_campaign as classic,rio_campaign,extract_sprites_rio,pack_sprites_rio,extract_themes_rio,extract_audio_rio,extract_audio_rio_music,level_order
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    etapa(1)
    order,_=level_order.level_order();levels=[];skipped=[];pack_of=[];k_in_pack=[]
    for pk in packs:
        for name in order[pk]:
            f=LEVELS/pk/(name+'.lua')
            try:rio_campaign.stage(f,20.0,len(levels)+1);levels.append(f);pack_of.append(pk);k_in_pack.append(sum(1 for q in pack_of if q==pk) if pk not in('warehouse','warehouse2') else len(levels))
            except ValueError as e:skipped.append((pk+'/'+name,str(e)))
    themes=[]
    for f in levels:
        t=rio_campaign.level_theme(f)
        if t not in themes:themes.append(t)
    stages=[rio_campaign.stage(f,20.0,i+1,themes) for i,f in enumerate(levels)]
    for i,f in enumerate(levels):stages[i]['stage_script']=f.stem;stages[i]['title']='%s %d'%(EPISODE.get(pack_of[i],pack_of[i].upper()),k_in_pack[i])
    classic.write_binary(stages,{'objects':{}},[],[],{},out/'CAMPAIGN.BIN')
    etapa(2)
    bank=out/'_bank';extract_sprites_rio.main(apk,bank,packs);pack_sprites_rio.pack(bank,out/'SPRITES.BIN',ids_dir/'sprite_ids.h')
    etapa(3)
    extract_themes_rio.extract(apk,out/'THEMES.BIN',themes)
    etapa(4)
    extract_audio_rio.main(apk,out,ffmpeg);extract_audio_rio_music.main(apk,out,ffmpeg)
    for f in ('sprites.rgba','manifest.json'):(bank/f).unlink(missing_ok=True)
    try:bank.rmdir()
    except OSError:pass
    print('fases no CAMPAIGN.BIN:',len(levels),'| fora (%d):'%len(skipped),[s[0] for s in skipped][:8])
    return levels,skipped
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--packs',nargs='+',default=DEFAULT_PACKS);p.add_argument('--ffmpeg',default='ffmpeg')
    a=p.parse_args();build(a.apk,a.output,a.packs,a.ffmpeg)
