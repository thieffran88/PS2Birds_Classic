#!/usr/bin/env python3
"""Convert the supported unluac assignment layout without executing Lua.
Game-specific names and shapes live in an explicit level adapter JSON.
Other Lua layouts/abilities require new adapters; they are not auto-supported.
"""
import argparse,hashlib,json,math,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
DEFAULT_PROFILE=ROOT/'profiles/classic_level.json'
GROUND_HALF_HEIGHT=5.0
SIZE_FACTOR=0.92
SLING_BACK_HEIGHT=199
SLING_BACK_PIVOT_Y=30
BIRD_RADIUS={'RedBird':0.75,'SmallBlueBird':0.6,'YellowBird':0.8,'BlackBird':1.0,'BasicBird2':1.3,'BoomerangBird':1.5}

def read_assignments(text):
    """Read only the literal/table assignment subset emitted by unluac. No Lua execution."""
    env={}
    def value(expression):
        expression=expression.strip()
        if expression=='{}':return {}
        if expression=='true':return True
        if expression=='false':return False
        if expression=='nil':return None
        if expression.startswith('"'):return json.loads(expression)
        if re.fullmatch(r'[-+\d.eE]+',expression):
            number=float(expression)
            if not math.isfinite(number):raise ValueError('Non-finite scalar')
            return number
        if re.fullmatch(r'\w+',expression):
            if expression not in env:raise ValueError('Unknown register: '+expression)
            return env[expression]
        match=re.fullmatch(r'(\w+)\.(\w+)',expression)
        if match and isinstance(env.get(match[1]),dict):return env[match[1]][match[2]]
        raise ValueError('Unsupported value expression: '+expression)
    for line_number,line in enumerate(text.splitlines(),1):
        line=line.strip()
        if not line or line.startswith('--') or re.fullmatch(r'local [\w, ]+',line):continue
        assignment=re.fullmatch(r'(.+?) = (.+)',line)
        if not assignment:raise ValueError(f'Unsupported statement at line {line_number}')
        target,expression=assignment.groups();resolved=value(expression)
        if re.fullmatch(r'\w+',target):env[target]=resolved;continue
        match=re.fullmatch(r'(\w+)(?:\.(\w+)|\[(.+)\])',target)
        if not match or not isinstance(env.get(match[1]),dict):raise ValueError(f'Unsupported target at line {line_number}')
        key=match[2] if match[2] else value(match[3])
        if not isinstance(key,(str,int,float)):raise ValueError('Unsupported table key')
        env[match[1]][key]=resolved
    return env

def convert(source,output,header,profile_path=DEFAULT_PROFILE,source_name=None):
    profile=json.loads(Path(profile_path).read_text());metrics=json.loads((ROOT/profile['metrics']).read_text())
    text=source.read_text();env=read_assignments(text);world=env.get('world')
    if not isinstance(world,dict):raise ValueError('Missing world table')
    objects=[dict(o) for o in world.values() if isinstance(o,dict) and 'definition' in o and 'name' in o]
    def scalar(key):
        val=env.get(key)
        if type(val) not in (int,float) or not math.isfinite(val):raise ValueError('Missing or invalid scalar '+key)
        return val
    view=profile.get('view',{'x':210,'y':333,'scale':7})
    if any(not isinstance(view.get(k),(int,float)) or not math.isfinite(view[k]) for k in ['x','y','scale']) or view['scale']<=0:
        raise ValueError('Invalid view configuration')
    pixels=scalar('physicsToWorld');gravity=scalar('worldGravity')
    if pixels<=0 or gravity<=0:raise ValueError('Adapter supports positive scale and uniform downward gravity only')
    birds=sorted((o for o in objects if o['definition'] in profile['bird_definitions']),key=lambda o:o['startNumber'])
    if not 1<=len(birds)<=16:raise ValueError('Adapter needs 1..16 supported birds')
    for bird in birds:
        bird['type']=profile.get('bird_types',{'RedBird':0}).get(bird['definition'],-1)
        if bird['type'] not in [0,1,2,3,4,6]:raise ValueError('Unsupported runtime bird type')
    if len({b['startNumber'] for b in birds})!=len(birds):raise ValueError('Duplicate bird queue position')
    grounds=[o for o in objects if o['definition']=='Ground']
    if len(grounds)!=1:raise ValueError('Adapter requires exactly one Ground object')
    # The Ground definition in blocks.lua declares a box with no size of its own; the
    # stage data place every resting object on y = Ground.y - 5, so its top surface sits
    # 5 units above the stored centre (a 10-unit-tall slab).
    ground=grounds[0]['y']-GROUND_HALF_HEIGHT;level=[]
    for o in objects:
        if o['definition'] in profile['bird_definitions'] or o['definition']=='Ground':continue
        shape=profile['objects'].get(o['definition'])
        if not shape:raise ValueError('Unsupported definition; add a verified adapter: '+o['definition'])
        kind=shape['kind']
        if kind not in [1,2,3,4,5,6,7,8]:raise ValueError('Unsupported runtime object kind')
        if 'sprite' in shape:
            metric=metrics[shape['sprite']];w,h=metric['width']/pixels,metric['height']/pixels
            # ObjectCreation.lua: sprite-sized shapes are scaled by sizeFactorX/Y (default
            # 0.92) unless the object is static (density 0), which keeps the full sprite.
            if o.get('density',0)!=0:w,h=w*SIZE_FACTOR,h*SIZE_FACTOR
        else:w=h=float(shape['diameter'])
        if w<=0 or h<=0:raise ValueError('Invalid shape dimensions')
        for key in ['x','y','angle','density','strength','defence']:
            if key not in o or type(o[key]) not in (int,float):raise ValueError('Missing or invalid object property '+key)
        if o['density']<0 or o['strength']<=0 or o['defence']<0:raise ValueError('Invalid material properties')
        collision=shape.get('shape','circle' if kind==4 else 'box')
        if collision not in ['box','circle','triangle_l','triangle_m']:raise ValueError('Unsupported collision shape')
        material=profile['materials']['by_kind'][str(kind)]
        friction=profile['materials']['friction_overrides'].get(o['definition'],material['friction'])
        restitution=profile['materials'].get('restitution_overrides',{}).get(o['definition'],material['restitution'])
        level.append(dict(o,width=w,height=h,kind=kind,shape=collision,friction=friction,restitution=restitution))
    if not 1<=len(level)<=192:raise ValueError('Adapter supports 1..192 scene objects')
    # SlingshotSystem.setPositionByStartObject (Slingshot.lua) places the slingshot on the
    # first bird of the queue: rest point = (x - 0.1, y - (h - pivot_y) * physicsScale +
    # radius + 0.2 - 0.1) with h/pivot_y from SLING_SHOT_01_BACK (199 px, pivot 30).
    # birdZeroPos only positions the waiting queue.
    first=birds[0];radius=BIRD_RADIUS[first['definition']]
    sling_x=first['x']-0.1
    sling_y=first['y']-(SLING_BACK_HEIGHT-SLING_BACK_PIVOT_Y)/pixels+radius+0.2-0.1
    # Joints: coordType 0 stores world anchors, coordType 2 stores anchors relative to
    # each body (rotated with it). Unsupported types are rejected rather than dropped.
    index={o['name']:i for i,o in enumerate(level)};joints=[]
    for j in (env.get('joints') or {}).values():
        if not isinstance(j,dict):continue
        kind=int(j.get('type',0))
        if kind not in (1,2,3):raise ValueError('Unsupported joint type: '+str(kind))
        ends=[]
        for end,xk,yk in [('end1','x1','y1'),('end2','x2','y2')]:
            name=j[end]
            if name not in index and name not in ('ground','Ground'):raise ValueError('Joint end not in scene: '+name)
            i=index.get(name,-1);x,y=float(j[xk]),float(j[yk])
            if int(j.get('coordType',0))==2 and i>=0:
                o=level[i];c,s=math.cos(o['angle']),math.sin(o['angle']);x,y=o['x']+x*c-y*s,o['y']+x*s+y*c
            elif int(j.get('coordType',0)) not in (0,2):raise ValueError('Unsupported joint coordType')
            ends.append((i,x,y))
        joints.append({'name':j.get('name',''),'type':kind,'a':ends[0][0],'b':ends[1][0],'ax':ends[0][1],'ay':ends[0][2],'bx':ends[1][1],'by':ends[1][2],
            'frequency':float(j.get('frequency',0)),'damping':float(j.get('dampingRatio',0)),'break_force':float(j.get('breakForce',0)) if j.get('breakable') else 0.0,
            'collide':bool(j.get('collideConnected',False))})
    if len(joints)>64:raise ValueError('Adapter supports up to 64 joints')
    doc={'adapter':profile['id'],'joints':joints,'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
         'source':source_name or source.name,'collision_dimensions':'sprite extents / physicsToWorld x 0.92 for dynamic objects (ObjectCreation.lua sizeFactor); static objects use the full sprite',
         'gravity':gravity,'physics_to_world':pixels,'ground_y':ground,'ground_object_y':grounds[0]['y'],'view':view,
         'sling_x':sling_x,'sling_y':sling_y,'bird_queue_x':scalar('birdZeroPosX')/pixels,'bird_queue_y':scalar('birdZeroPosY')/pixels,'birds':birds,'objects':level}
    def f(v):return f'{v:.8f}f'
    config=['/* Generated by tools/import_level.py. */','#ifndef PS2BIRDS_LEVEL_CONFIG_H','#define PS2BIRDS_LEVEL_CONFIG_H',
            '#define LEVEL_BIRDS '+str(len(birds)),'#define LEVEL_GRAVITY '+f(gravity),
            '#define LEVEL_SLING_X '+f(doc['sling_x']),'#define LEVEL_SLING_Y '+f(doc['sling_y']),
            '#define LEVEL_GROUND_Y '+f(ground),'#define LEVEL_PIXELS_PER_UNIT '+f(pixels),
            '#define LEVEL_VIEW_X '+f(view['x']),'#define LEVEL_VIEW_Y '+f(view['y']),'#define LEVEL_VIEW_SCALE '+f(view['scale']),'#endif','']
    lines=['/* Generated level data; box collision approximation. */','#ifndef PS2BIRDS_LEVEL_DATA_H','#define PS2BIRDS_LEVEL_DATA_H',
           '#include "level_config.h"','typedef struct { float x,y,angle,width,height,density,strength,defence; int kind; } LevelObject;',
           'static const LevelObject level_objects[] = {']
    for o in level:
        values=[o[k] for k in ['x','y','angle','width','height','density','strength','defence']]
        lines.append(' {'+','.join(f(v) for v in values)+','+str(o['kind'])+'}, /* '+o['name']+' */')
    lines+=['};','#define LEVEL_OBJECTS (sizeof(level_objects)/sizeof(level_objects[0]))','#endif','']
    # Validate fully before emitting any generated files.
    output.write_text(json.dumps(doc,indent=2)+'\n');header.write_text('\n'.join(lines))
    header.with_name('level_config.h').write_text('\n'.join(config))
    print(f'Imported {len(level)} scene objects and {len(birds)} birds with adapter {profile["id"]}')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('source',type=Path);p.add_argument('output',type=Path);p.add_argument('header',type=Path)
    p.add_argument('--profile',type=Path,default=DEFAULT_PROFILE);p.add_argument('--source-name');a=p.parse_args();convert(a.source,a.output,a.header,a.profile,a.source_name)
