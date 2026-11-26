"""Builds the data files of the Rio build in one folder, from the player's own APK: CAMPAIGN.BIN (the stages of the
first episode that the runtime can play), SPRITES.BIN, THEMES.BIN, SOUNDS.BIN, MUSIC.BIN, FONT.BIN, UI.BIN, MENU.BIN and CUTSCENE.BIN.
Stages whose birds the runtime does not have (only the red bird exists) are listed and left out.
usage: build_rio_data.py APK OUTDIR [--packs warehouse warehouse2] [--ffmpeg ffmpeg]"""
import argparse,json,subprocess,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
# Episodes of the first game, in the order of CutsceneScene.lua (theme1End .. theme6End): name and the level packs of each one.
EPISODES=[("SMUGGLERS DEN",['warehouse','warehouse2']),("JUNGLE ESCAPE",['jungle1','jungle2']),("BEACH VOLLEY",['beach1','beach2']),
          ("CARNIVAL UPHEAVAL",['carnival1','carnival2']),("AIRFIELD CHASE",['airport1','airport2']),("SMUGGLERS PLANE",['plane1','plane2']),
          ("MARKET MAYHEM",['fruitmarket1','fruitmarket2']),("GOLDEN BEACHBALL",['goldenbeachball','goldenbeachball2']),("ROCKET RUMBLE",['newyear1','newyear2']),
          ("HIGH DIVE",['blossomdive']),("BLOSSOM RIVER",['river1','river2']),("TIMBER TUMBLE",['paradise1','paradise2']),("HIDDEN HARBOR",['manaus1','manaus2']),
          ("TREASURE HUNT",['treasurehunt1','treasurehunt2','treasurehunt3','treasurehunt4']),
          ("PLAYGROUND",['tutorial']),("REWARD LEVELS",['rewardlevels']),
          ("BONUS LEVELS",['bonus'+n for n in ('Warehouse','Jungle','Beach','Carnival','Airport','Plane','Fruitmarket','Goldenbeachball','Newyear','Manaus','Paradise','River','Treasurehunt')])]   # names from the texts (EPISODE_SELECTION_EPISODE_NAME_7.._13, _GBB); which pack is which is inferred from the theme ambience and the card names (newyear = BTN_EP_NYE with the rocketrumble ambience; manaus and blossomdive share the harbor ambience)
EPISODE={pk:name for name,packs in EPISODES for pk in packs}
SPLIT_BANKS={'BONUS LEVELS'}   # one texture bank per pack inside these episodes (the 13 themes of the bonus stages would not fit one bank)
def bank_key(pk):
    e=EPISODE.get(pk,pk.upper());return pk if e in SPLIT_BANKS else e
LEVELS=ROOT/'work/rio/analysis/chunks/assets/data/levels'
DEFAULT_PACKS=[pk for _,packs in EPISODES for pk in packs]   # the stages the shipped ELF's sprite and piece tables (ps2/src/rio) were made for
def build(apk,out,packs,ffmpeg,etapa=None,ids_dir=None):
    """etapa(i): called as each part starts (1 stages, 2 sprites, 3 themes, 4 sounds); ids_dir: where sprite_ids.h and
    definition_sprites.h are written (default ps2/src/rio, which the ELF is compiled with)."""
    etapa=etapa or (lambda i:None);ids_dir=Path(ids_dir) if ids_dir else ROOT/'ps2/src/rio'
    import build_campaign as classic,rio_campaign,extract_sprites_rio,pack_sprites_rio,extract_themes_rio,extract_audio_rio,extract_audio_rio_music,extract_font_rio,extract_ui_rio,extract_menu_rio,extract_cutscene_rio,level_order
    out=Path(out);out.mkdir(parents=True,exist_ok=True)
    etapa(1)
    order,_=level_order.level_order();levels=[];skipped=[];pack_of=[]
    for pk in packs:
        for name in order[pk]:
            f=LEVELS/pk/(name+'.lua')
            try:rio_campaign.stage(f,20.0,len(levels)+1);levels.append(f);pack_of.append(pk)
            except ValueError as e:skipped.append((pk+'/'+name,str(e)))
    themes=[]
    for f in levels:
        t=rio_campaign.level_theme(f)
        if t not in themes:themes.append(t)
    stages=[rio_campaign.stage(f,20.0,i+1,themes) for i,f in enumerate(levels)]
    banks=[];count={}   # one texture bank per episode; titles count the stages inside the episode ("JUNGLE ESCAPE 7")
    for pk in packs:
        e=bank_key(pk)
        if e not in banks:banks.append(e)
    for i,f in enumerate(levels):
        e=EPISODE.get(pack_of[i],pack_of[i].upper());count[e]=count.get(e,0)+1
        stages[i]['stage_script']=f.stem;stages[i]['title']='%s %d'%(e,count[e]);stages[i]['texture_bank']=banks.index(bank_key(pack_of[i]))
    # floatability of the pieces the water lifts (blocks.lua `floatability`: barrels 2.6, eggs 4.2...), one force record per definition (no force of their own)
    cat=json.loads((ROOT/'work/rio/analysis/definition_catalogue.json').read_text())['definitions']
    used={o['definition'] for s in stages for o in s['objects']}
    float_of={d:(cat.get(d) or {}).get('effective',{}).get('floatability') for d in used}
    float_of={d:v for d,v in float_of.items() if v}
    classic.write_binary(stages,{'objects':{d:{'floatability':v} for d,v in float_of.items()}},[],sorted(float_of),{},out/'CAMPAIGN.BIN')
    etapa(2)
    bank_dirs=[]
    for k,e in enumerate(banks):
        d=out/('_bank%d'%k);extract_sprites_rio.main(apk,d,[pk for pk in packs if bank_key(pk)==e]);bank_dirs.append(d)
    pack_sprites_rio.pack_banks(bank_dirs,out/'SPRITES.BIN',ids_dir/'sprite_ids.h')
    etapa(3)
    extract_themes_rio.extract(apk,out/'THEMES.BIN',themes)
    etapa(4)
    extract_audio_rio.main(apk,out,ffmpeg);extract_audio_rio_music.main(apk,out,ffmpeg)
    extract_font_rio.main(apk,out/'FONT.BIN')   # the Rio's own HUD and score fonts (with the Portuguese accents)
    extract_ui_rio.main(apk,out/'UI.BIN');extract_menu_rio.main(apk,out/'MENU.BIN');extract_cutscene_rio.main(apk,out/'CUTSCENE.BIN')   # comic scenes; level end / pause / particles and the menu graphics
    for d in bank_dirs:
        for f in ('sprites.rgba','manifest.json'):(d/f).unlink(missing_ok=True)
        try:d.rmdir()
        except OSError:pass
    print('fases no CAMPAIGN.BIN:',len(levels),'| fora (%d):'%len(skipped),[s[0] for s in skipped][:8])
    return levels,skipped
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--packs',nargs='+',default=DEFAULT_PACKS);p.add_argument('--ffmpeg',default='ffmpeg')
    a=p.parse_args();build(a.apk,a.output,a.packs,a.ffmpeg)
