"""Rio stage -> the object list that tools/build_campaign.write_binary stores in CAMPAIGN.BIN.
Everything comes from the APK data read by tools/lua51_data.py and the catalogue built by
tools/rio/build_catalogue.py: no Lua runs. Sizes follow ObjectCreation: a sprite-sized shape is
sprite size / physicsToWorld scaled by the instance scale (and by 0.92 when it is not static).
The world gravity is not in the stage data (see docs/rio/ORDEM_DAS_FASES.md): it is a parameter.
usage: rio_import_level.py LEVEL.lua [--gravity G]  -> prints a summary and writes work/rio/stage_<name>.json"""
import argparse,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
from lua51_data import load
COMP=ROOT/'work/rio/analysis/comp_inventory.json';CAT=ROOT/'work/rio/analysis/definition_catalogue.json';INV=ROOT/'work/rio/analysis/dat_inventory.json'
SIZE_FACTOR=0.92   # ObjectCreation (gamelogic.lua): a sprite-sized shape is the sprite's size x 0.92
GROUND_HALF_HEIGHT=5.0   # the Ground object is 10 units tall (as in tools/import_level.py): its top, where every stage's pieces rest, is y - 5; checked on the 45 stages: the lowest pieces end at y = 0
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
def convert_joints(env,objects):
    """The stage's joints in the form tools/build_campaign.write_binary stores (the same fields as tools/import_level.py: the Rio data
    uses the same joint tables). Ends are object indices (-1: the ground); coordType 2 is relative to the object. Joints to something
    that is not an object of the stage (a bird, a skipped piece) are left out."""
    import math
    index={o['name']:i for i,o in enumerate(objects)};out=[]
    for j in (env.get('joints') or {}).values():
        if not isinstance(j,dict):continue
        kind=int(j.get('type',0))
        if kind not in (1,2,3,4,5):continue
        ends=[]
        for end,xk,yk in (('end1','x1','y1'),('end2','x2','y2')):
            name=j[end]
            if name not in index and name not in ('ground','Ground'):break
            i=index.get(name,-1);x,y=float(j[xk]),float(j[yk])
            if int(j.get('coordType',0))==2 and i>=0:
                o=objects[i];c,sn=math.cos(o['angle']),math.sin(o['angle']);x,y=o['x']+x*c-y*sn,o['y']+x*sn+y*c
            elif int(j.get('coordType',0)) not in (0,2):break
            ends.append((i,x,y))
        if len(ends)<2:continue
        out.append({'name':j.get('name',''),'vehicle':False,'type':kind,'a':ends[0][0],'b':ends[1][0],'ax':ends[0][1],'ay':ends[0][2],'bx':ends[1][1],'by':ends[1][2],
            'frequency':float(j.get('frequency',0)),'damping':float(j.get('dampingRatio',0)),'break_force':float(j.get('breakForce',0)) if j.get('breakable') else 0.0,
            'destroy_timer':float(j.get('destroyTimer',0)) if kind==5 else 0.0,'collide':bool(j.get('collideConnected',False)),
            'axis_x':float(j.get('worldAxisX',1.0)) if kind==4 else 0.0,'axis_y':float(j.get('worldAxisY',0.0)) if kind==4 else 0.0,
            'lower':float(j.get('lowerLimit',0)),'upper':float(j.get('upperLimit',0)),'motor_speed':float(j.get('motorSpeed',0)),'max_force':float(j.get('maxTorque',0)),
            'limit':bool(j.get('limit',False)) and kind in (3,4),'motor':bool(j.get('motor',False)) and kind in (3,4),'back_and_forth':bool(j.get('backAndForth',False)) and kind in (3,4)})
    return out[:512]
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
            if e.get("density",0)!=0:w,h=w*SIZE_FACTOR,h*SIZE_FACTOR
        else:skipped.append(d);continue
        sx=float(o.get('scaleX',1) or 1);sy=float(o.get('scaleY',1) or 1)
        w*=abs(sx);h*=abs(sy)
        mirror=(2 if sx<0 else 0)|(4 if sy<0 else 0)   # OBJECT_MIRROR_X / _Y
        dens=e.get('density',0.0)
        objects.append(dict(name=o['name'],definition=d,x=o['x'],y=o['y'],angle=o['angle'],width=w,height=h,
            shape={'block':'box'}.get(e.get('type','box'),e.get('type','box')),kind=4 if e.get('levelGoal') else 3 if not dens else KIND_OF_MATERIAL.get(mat,1),mirror=mirror,
            **({'no_collision':True} if e.get('collision') is False else {}),
            density=e.get('density',0.0),strength=e.get('strength',10.0),defence=e.get('defence',0.0),
            friction=e.get('friction',0.5),restitution=e.get('restitution',0.2),material=mat))
    birds.sort(key=lambda b:b['startNumber'])
    sling=birds[0]   # Slingshot.lua: the slingshot is placed on the first bird of the queue
    joints=convert_joints(env,objects)
    return dict(objects=objects,joints=joints,birds=[dict(definition=b['definition'],x=b['x'],y=b['y']) for b in birds],
        sling_x=sling['x'],sling_y=sling['y'],ground_y=(ground['y']-GROUND_HALF_HEIGHT) if ground else 0.0,
        physics_to_world=pixels,gravity=gravity,skipped=sorted(set(skipped)))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('level',type=Path);p.add_argument('--gravity',type=float,default=20.0)
    a=p.parse_args();r=convert(a.level,a.gravity)
    out=ROOT/'work/rio';out.mkdir(parents=True,exist_ok=True);(out/('stage_%s.json'%a.level.stem)).write_text(json.dumps(r,indent=1))
    kinds={}
    for o in r['objects']:kinds[o['kind']]=kinds.get(o['kind'],0)+1
    print('objetos',len(r['objects']),'por tipo',kinds,'| pássaros',len(r['birds']),'| estilingue',round(r['sling_x'],2),round(r['sling_y'],2),'| sem adaptador:',r['skipped'])
