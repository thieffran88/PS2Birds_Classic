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
from extract_sprites import pack,VRAM_BUDGET,halve_mask
from lua51_data import load
import zipfile
# Sprites the runtime draws by name, not through a piece definition: the slingshot of the first chapter.
EXTRA=['SLING_SHOT_01_BACK','SLING_SHOT_01_FRONT']
# The runtime's bird types (tools/rio/rio_campaign.py BIRD_TYPE) and the sprite states it draws.
BIRDS={0:'RedBird',1:'SmallBlueBird',2:'YellowBird',3:'BlackBird',4:'WhiteBird',5:'EggGranade',6:'BoomerangBird',7:'RedBigBird',8:'GlobeBird',9:'PinkBird',11:'BIRD_RIO_BLUE_ALONE',12:'RioBirds'}
BIRD_STATES=('default','flying','collision','action')   # + the black bird's bomb timer and the globe bird's inflated sprite
PUFFED='BIRD_PUFFER_2'
def bird_names(sp):
    """Every sprite a bird definition's `sprites` table can show in the game."""
    out={sp[k] for k in BIRD_STATES if isinstance(sp.get(k),str)}
    if isinstance(sp.get('action'),dict):out|=set(sp['action'].get('frames',{}).values())   # Blu and Jewel: the wing-beat frames (the first is the one drawn)
    if isinstance(sp.get('timer'),dict):out|=set(sp['timer'].values())
    return out
CAT=ROOT/'work/rio/analysis/definition_catalogue.json';LEVELS=ROOT/'work/rio/analysis/chunks/assets/data/levels'
def invisible(effective):
    """Pieces drawn by nothing: the dummy anchors (collision = false), the empty-sprite pieces and the fixed terrain (`themed`: only physics, the theme's art is the scenery)."""
    return effective.get('collision') is False or effective.get('themed') or (effective.get('sprite') in ('EMPTY_1X1',None) and not effective.get('sprites'))
def mask_sprites(cat):
    """The pieces whose sprite is a ground mask ("themed": the theme's ground texture is painted through it, as in the Classic)."""
    return {e['sprite'] for d in cat.values() for e in [d.get('effective') or {}] if e.get('themed') and e.get('sprite')}
BAKED={}   # composition name -> [(child sprite, dx, dy)] of the compositions drawn by the stages, filled by wanted()
def sd_compositions():
    """Compositions of the SD set (ingame_assets_sd). The same name appears in several files, some with the HD offsets (twice the SD
    ones): the entry with the smallest offsets is the SD one. dx, dy: where the child's pivot sits in the composition."""
    comps=json.loads(crop_sprites.INV.parent.joinpath('comp_inventory.json').read_text());best={}
    for p in comps['parsed']:
        if p['path'].split('/')[3]!='ingame_assets_sd':continue
        for c in p.get('composites') or []:
            ch=[(x['sprite'],x['dx_raw'],x['dy_raw']) for x in c['children']];cost=sum(abs(a)+abs(b) for _,a,b in ch)
            if c['name'] not in best or cost<best[c['name']][0]:best[c['name']]=(cost,ch)
    return {n:ch for n,(_,ch) in best.items()}
def wanted(packs):
    cat=json.loads(CAT.read_text())['definitions'];comps=json.loads(crop_sprites.INV.parent.joinpath('comp_inventory.json').read_text())
    composite={c['name']:[x['sprite'] for x in c['children']] for p in comps['parsed'] for c in p.get('composites') or []}
    sdc=sd_compositions();BAKED.clear()
    names=set();used=set()
    for pk in packs:
        for f in sorted((LEVELS/pk).glob('*.lua')):
            used|={o['definition'] for o in load(f.read_bytes())['world'].values()}
    for d in used:
        e=(cat.get(d) or {}).get('effective') or {}
        if invisible(e):continue
        s=e.get('sprite')
        if not s:continue
        if s in sdc:BAKED[s]=sdc[s];names|={c for c,_,_ in sdc[s]}|{s}
        else:names|=set(composite.get(s,[s]))
    for b in BIRDS.values():
        sp=((cat.get(b) or {}).get('effective') or {}).get('sprites') or {}
        names|=bird_names(sp)
    names.add(PUFFED)
    return sorted(names|set(EXTRA)),sorted(used)
def main(apk,out,packs):
    names,used=wanted(packs);masks=mask_sprites(json.loads(CAT.read_text())['definitions']);hits=crop_sprites.find(set(names));cache={};sprites=[];pixels={};missing=[]
    with zipfile.ZipFile(apk) as z:
        for n in names:
            if n in BAKED:continue   # drawn below from its children
            if n not in hits:missing.append(n);continue
            _,folder,tex,f=hits[n];k=(folder,tex)
            if k not in cache:cache[k]=crop_sprites.sheet(z,folder,tex)
            w,h,px=cache[k];rows=bytearray()
            for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
            for q in range(3,len(rows),4):rows[q]=(rows[q]*128+127)//255   # alpha on the GS scale, 0..128, like the Classic bank
            fw,fh,px_,py_=f['w'],f['h'],f['px'],f['py']
            if n in masks:   # ground masks: half resolution, drawn at SPRITE_MASK_SCALE
                rows,fw,fh=halve_mask(bytes(rows),fw,fh);px_=(px_+1)//2;py_=(py_+1)//2
            sprites.append({'name':n,'width':fw,'height':fh,'pivot_x':px_,'pivot_y':py_});pixels[n]=bytes(rows)
    # Compositions (monkeys: tail, body, head, face; cages with their birds...): each is drawn into one sprite, children in order, pivot at the
    # composition's origin. Children no piece uses on its own are dropped from the bank.
    alone=set(names)-{c for ch in BAKED.values() for c,_,_ in ch}-set(BAKED)
    by={sp['name']:sp for sp in sprites}
    for cname,ch in BAKED.items():
        parts=[(by[c],dx-by[c]['pivot_x'],dy-by[c]['pivot_y']) for c,dx,dy in ch if c in by]
        if not parts:missing.append(cname);continue
        left=min(l for _,l,_ in parts);top=min(t for _,_,t in parts);right=max(l+sp['width'] for sp,l,_ in parts);bottom=max(t+sp['height'] for sp,_,t in parts)
        W,H=right-left,bottom-top;canvas=bytearray(W*H*4)
        for sp,l,t in parts:
            d=pixels[sp['name']]
            for r in range(sp['height']):
                for c in range(sp['width']):
                    q=(r*sp['width']+c)*4;al=d[q+3]
                    if not al:continue
                    o=((t-top+r)*W+(l-left+c))*4;ba=canvas[o+3]
                    if al>=128 or not ba:canvas[o:o+4]=d[q:q+4]
                    else:   # source over destination, alpha 0..128
                        oa=al+ba*(128-al)//128
                        for k in range(3):canvas[o+k]=(d[q+k]*al+canvas[o+k]*ba*(128-al)//128)//oa
                        canvas[o+3]=oa
        sprites.append({'name':cname,'width':W,'height':H,'pivot_x':-left,'pivot_y':-top});pixels[cname]=bytes(canvas)
    keep=alone|set(BAKED);sprites=[sp for sp in sprites if sp['name'] in keep]
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
