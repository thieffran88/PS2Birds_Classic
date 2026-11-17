"""Rio stage -> the object list that tools/build_campaign.write_binary stores in CAMPAIGN.BIN.
Everything comes from the APK data read by tools/lua51_data.py and the catalogue built by
tools/rio/build_catalogue.py: no Lua runs. Sizes follow ObjectCreation: a sprite-sized shape is
sprite size / physicsToWorld scaled by the instance scale (and by 0.92 when it is not static).
The world gravity is not in the stage data (see docs/rio/ORDEM_DAS_FASES.md): it is a parameter.
usage: import_level.py LEVEL.lua [--gravity G]  -> prints a summary and writes work/rio/stage_<name>.json"""
import argparse,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
from lua51_data import load
COMP=ROOT/'work/rio/analysis/comp_inventory.json';CAT=ROOT/'work/rio/analysis/definition_catalogue.json';INV=ROOT/'work/rio/analysis/dat_inventory.json'
SIZE_FACTOR=0.92
KIND_OF_MATERIAL={'cage':4,'monkey':4,'monkeyHanging':4,'wood':1,'crate':1,'glass':2,'stone':5,'cardboard':1,'metal':6,'misc':1}
def frame_sizes():
    sizes={}
    for p in json.loads(INV.read_text())['parsed']:
        for f in p.get('frames') or []:sizes.setdefault(f['name'],(f['w'],f['h']))
    return sizes
def composites():
    """name -> sprite of the body of a composition: its last child, drawn at offset 0 (the cage; the freed bird is the child before it)."""
    out={}
    for p in json.loads(COMP.read_text())['parsed']:
        for c in p.get('composites') or []:
            if c.get('children'):out[c['name']]=c['children'][-1]['sprite']
    return out
def convert(path,gravity):
    env=load(Path(path).read_bytes());cat=json.loads(CAT.read_text())['definitions'];sizes=frame_sizes();bodies=composites()
    pixels=float(env['physicsToWorld']);objects=[];birds=[];ground=None;skipped=[]
    for o in env['world'].values():
        d=o['definition']
        if 'startNumber' in o:birds.append(o);continue
        if d=='Ground':ground=o;continue
        e=(cat.get(d) or {}).get('effective')
        if not e:skipped.append(d);continue
        sprite=e.get('sprite');sprite=bodies.get(sprite,sprite);mat=e.get('material')
        if e.get('type')=='box' and e.get('width') and e.get('height'):w,h=float(e['width']),float(e['height'])
        elif sprite in sizes:
            w,h=sizes[sprite][0]/pixels,sizes[sprite][1]/pixels
            if e.get('density',0)!=0:w,h=w*SIZE_FACTOR,h*SIZE_FACTOR
        else:skipped.append(d);continue
        w*=abs(float(o.get('scaleX',1) or 1));h*=abs(float(o.get('scaleY',1) or 1))
        objects.append(dict(name=o['name'],definition=d,x=o['x'],y=o['y'],angle=o['angle'],width=w,height=h,
            shape=e.get('type','box'),kind=4 if e.get('levelGoal') else KIND_OF_MATERIAL.get(mat,1),
            density=e.get('density',0.0),strength=e.get('strength',10.0),defence=e.get('defence',0.0),
            friction=e.get('friction',0.5),restitution=e.get('restitution',0.2),material=mat))
    birds.sort(key=lambda b:b['startNumber'])
    sling=birds[0]   # Slingshot.lua: the slingshot is placed on the first bird of the queue
    return dict(objects=objects,birds=[dict(definition=b['definition'],x=b['x'],y=b['y']) for b in birds],
        sling_x=sling['x'],sling_y=sling['y'],ground_y=ground['y'] if ground else 0.0,
        physics_to_world=pixels,gravity=gravity,skipped=sorted(set(skipped)))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('level',type=Path);p.add_argument('--gravity',type=float,default=-10.0)
    a=p.parse_args();r=convert(a.level,a.gravity)
    out=ROOT/'work/rio';out.mkdir(parents=True,exist_ok=True);(out/('stage_%s.json'%a.level.stem)).write_text(json.dumps(r,indent=1))
    kinds={}
    for o in r['objects']:kinds[o['kind']]=kinds.get(o['kind'],0)+1
    print('objetos',len(r['objects']),'por tipo',kinds,'| pássaros',len(r['birds']),'| estilingue',round(r['sling_x'],2),round(r['sling_y'],2),'| sem adaptador:',r['skipped'])
