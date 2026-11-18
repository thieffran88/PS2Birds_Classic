"""Sprite bank for the Rio stages, built from the user's own APK (nothing of the game is stored in
the repository). Sprite list: every sprite used by the pieces of the stages in scope (the leaf
sprites of the definitions, from the catalogue of tools/rio/build_catalogue.py); pixels: the PVR
RGBA4444 sheets of ingame_assets_sd (or PNG), cropped with the .dat rectangles and packed with the
same skyline packer as the Classic. WebP sheets are read with tools/webp.py.
usage: extract_sprites_rio.py APK OUTDIR [--levels warehouse warehouse2 ...]"""
import argparse,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import crop_sprites
from extract_sprites import pack,VRAM_BUDGET
from lua51_data import load
import zipfile
# Sprites the runtime draws by name, not through a piece definition: the slingshot of the first chapter.
EXTRA=['SLING_SHOT_01_BACK','SLING_SHOT_01_FRONT']
# The runtime's bird types (tools/rio/rio_campaign.py BIRD_TYPE) and the sprite states it draws.
BIRDS={0:'RedBird',1:'SmallBlueBird',2:'YellowBird',3:'BlackBird',4:'WhiteBird',5:'EggGranade',6:'BoomerangBird',7:'RedBigBird',8:'GlobeBird',9:'PinkBird'}
BIRD_STATES=('default','flying','collision','action')   # + the black bird's bomb timer and the globe bird's inflated sprite
PUFFED='BIRD_PUFFER_2'
def bird_names(sp):
    """Every sprite a bird definition's `sprites` table can show in the game."""
    out={sp[k] for k in BIRD_STATES if isinstance(sp.get(k),str)}
    if isinstance(sp.get('timer'),dict):out|=set(sp['timer'].values())
    return out
CAT=ROOT/'work/rio/analysis/definition_catalogue.json';LEVELS=ROOT/'work/rio/analysis/chunks/assets/data/levels'
def invisible(definition):
    """Static and dummy blocks are physics only (their sprite is a flat white placeholder): the theme draws the scenery."""
    return 'STATIC' in definition or 'DUMMY' in definition
def wanted(packs):
    cat=json.loads(CAT.read_text())['definitions'];comps=json.loads(crop_sprites.INV.parent.joinpath('comp_inventory.json').read_text())
    composite={c['name']:[x['sprite'] for x in c['children']] for p in comps['parsed'] for c in p.get('composites') or []}
    names=set();used=set()
    for pk in packs:
        for f in sorted((LEVELS/pk).glob('*.lua')):
            used|={o['definition'] for o in load(f.read_bytes())['world'].values()}
    for d in used:
        if invisible(d):continue
        e=(cat.get(d) or {}).get('effective') or {}
        s=e.get('sprite')
        if not s:continue
        names|=set(composite.get(s,[s]))
    for b in BIRDS.values():
        sp=((cat.get(b) or {}).get('effective') or {}).get('sprites') or {}
        names|=bird_names(sp)
    names.add(PUFFED)
    return sorted(names|set(EXTRA)),sorted(used)
def main(apk,out,packs):
    names,used=wanted(packs);hits=crop_sprites.find(set(names));cache={};sprites=[];pixels={};missing=[]
    with zipfile.ZipFile(apk) as z:
        for n in names:
            if n not in hits:missing.append(n);continue
            _,folder,tex,f=hits[n];k=(folder,tex)
            if k not in cache:cache[k]=crop_sprites.sheet(z,folder,tex)
            w,h,px=cache[k];rows=bytearray()
            for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
            for q in range(3,len(rows),4):rows[q]=(rows[q]*128+127)//255   # alpha on the GS scale, 0..128, like the Classic bank
            sprites.append({'name':n,'width':f['w'],'height':f['h'],'pivot_x':f['px'],'pivot_y':f['py']});pixels[n]=bytes(rows)
    specs,placed=pack(sprites)
    sheets=[bytearray(s['width']*s['height']*4) for s in specs]
    for s in sprites:
        page,x0,y0=placed[s['name']];sp=specs[page];d=pixels[s['name']]
        for r in range(s['height']):
            o=((y0+r)*sp['width']+x0)*4;sheets[page][o:o+s['width']*4]=d[r*s['width']*4:(r+1)*s['width']*4]
    out=Path(out);out.mkdir(parents=True,exist_ok=True);(out/'sprites.rgba').write_bytes(b''.join(sheets))
    slots=[s['width']*s['height']*4 for s in specs]
    (out/'manifest.json').write_text(json.dumps({'game':'rio','apk':Path(apk).name,'vram_budget':VRAM_BUDGET,'slot_bytes':slots,'pages':specs,
        'sprites':[dict(s,page=placed[s['name']][0],atlas_x=placed[s['name']][1],atlas_y=placed[s['name']][2]) for s in sprites],'missing_webp_or_unreadable':missing},indent=1))
    print('definições usadas',len(used),'| sprites pedidos',len(names),'| empacotados',len(sprites),'| páginas',len(specs),'| bytes de VRAM',sum(slots),'de',VRAM_BUDGET,'| sem folha legível:',missing)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--levels',nargs='+',default=['warehouse','warehouse2'])
    a=p.parse_args();main(a.apk,a.output,a.levels)
