"""UI.BIN for the Rio build, from the player's own APK: the same 128 x 1024 PSMT8 page and the same sprite names (ui.h)
as the Classic, filled with the Rio's own frames (level end, pause, particles, score sprites). The Rio's .dat sheets
are read through the inventory of tools/rio (the 864x480 menu sheets and ingame_assets_sd).
usage: extract_ui_rio.py APK OUT.BIN"""
import json,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import crop_sprites,extract_ui
UI_SCALE=0.4*1024/864     # the Classic draws its 1024-wide frames at 0.4; the Rio's are 864 wide
# ui.h name -> (Rio frame, scale). Names the Rio does not have borrow the nearest one (feathers: only red and blue exist).
RIO_UI={'STAR_1':'BIG_STAR_1','STAR_2':'BIG_STAR_2','STAR_3':'BIG_STAR_3','NEW_HIGHSCORE':'NEW_HIGHSCORE_BADGE','MENU':'ICON_MENU','RESTART':'ICON_RESTART',
        'NEXT':'ICON_NEXT','FAILED_PIG':'LEVEL_FAILED_MONKEY','RESUME':'ICON_RESUME_SMALL','SOUND':'ICON_SFX','SOUND_OFF':'ICON_SFX_SETTINGS'}
def particle(n):
    for a,b in (('WOOD_PARTICLE_','PARTICLE_WOOD_'),('ROCK_PARTICLE_','PARTICLE_STONE_')):
        if n.startswith(a):return b+n[len(a):]
    if n.startswith('LIGHT_PARTICLE_'):return 'PARTICLE_GLASS_'+n[-1]
    if n.endswith(tuple('_FEATHER_%d'%i for i in (1,2,3))):
        c=n.split('_')[0];return 'P_%s_FEATHER_%s'%('BLUE' if c in ('BLUE','GREEN','BLACK') else 'RED',n[-1])
    if n[:3] in ('10K',):return 'SCORE_'+n
    return n
def main(apk,out):
    inv=json.loads(crop_sprites.INV.read_text());where={}
    for p in inv['parsed']:
        folder=p['path'].split('/')[3];tex=p.get('texture') or ''
        if folder not in ('864x480','ingame_assets_sd') or tex.rsplit('.',1)[-1].lower() not in ('pvr','png','webp'):continue
        for f in p.get('frames') or []:where.setdefault(f['name'],(folder,tex,f))
    sprites=[];cache={};missing=[]
    with zipfile.ZipFile(apk) as z:
        for name in [i for i in RIO_UI]+extract_ui.PARTICLES:
            src=RIO_UI.get(name) or particle(name)
            if src not in where:missing.append((name,src));continue
            folder,tex,f=where[src]
            if (folder,tex) not in cache:cache[(folder,tex)]=crop_sprites.sheet(z,folder,tex)
            w,h,px=cache[(folder,tex)];rows=bytearray()
            for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
            for q in range(3,len(rows),4):rows[q]=(rows[q]*128+127)//255
            if name in RIO_UI:fac=UI_SCALE
            else:fac=0.5 if name.startswith('EXPLOSION') else 1.0
            r,fw,fh=extract_ui.scale(rows,f['w'],f['h'],fac);sprites.append((name,fw,fh,r))
    if missing:raise SystemExit('Rio UI sprites not found: %s'%missing)
    r=extract_ui.write_ui(sprites,list(RIO_UI)+extract_ui.PARTICLES,out);print('UI.BIN do Rio:',r);return r
if __name__=='__main__':main(*sys.argv[1:3])
