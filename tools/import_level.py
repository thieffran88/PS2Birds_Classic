#!/usr/bin/env python3
"""Convert the supported unluac assignment layout without executing Lua.
Game-specific names and shapes live in an explicit level adapter JSON.
Other Lua layouts/abilities require new adapters; they are not auto-supported.
"""
import argparse,hashlib,json,math,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
DEFAULT_PROFILE=ROOT/'profiles/classic_level.json'

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
        if bird['type'] not in [0,1,2,3,4]:raise ValueError('Unsupported runtime bird type')
    if len({b['startNumber'] for b in birds})!=len(birds):raise ValueError('Duplicate bird queue position')
    grounds=[o for o in objects if o['definition']=='Ground']
    if len(grounds)!=1:raise ValueError('Adapter requires exactly one Ground object')
    ground=grounds[0]['y'];level=[]
    for o in objects:
        if o['definition'] in profile['bird_definitions'] or o['definition']=='Ground':continue
        shape=profile['objects'].get(o['definition'])
        if not shape:raise ValueError('Unsupported definition; add a verified adapter: '+o['definition'])
        kind=shape['kind']
        if kind not in [1,2,3,4,5,6,7]:raise ValueError('Unsupported runtime object kind')
        if 'sprite' in shape:
            metric=metrics[shape['sprite']];w,h=metric['width']/pixels,metric['height']/pixels
        else:w=h=float(shape['diameter'])
        if w<=0 or h<=0:raise ValueError('Invalid shape dimensions')
        for key in ['x','y','angle','density','strength','defence']:
            if key not in o or type(o[key]) not in (int,float):raise ValueError('Missing or invalid object property '+key)
        if o['density']<0 or o['strength']<=0 or o['defence']<0:raise ValueError('Invalid material properties')
        collision=shape.get('shape','circle' if kind==4 else 'box')
        if collision not in ['box','circle','triangle_l','triangle_m']:raise ValueError('Unsupported collision shape')
        level.append(dict(o,width=w,height=h,kind=kind,shape=collision))
    if not 1<=len(level)<=160:raise ValueError('Adapter supports 1..160 scene objects')
    doc={'adapter':profile['id'],'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),
         'source':source_name or source.name,'collision_dimensions':'atlas extents / physicsToWorld; native hulls not recovered',
         'gravity':gravity,'physics_to_world':pixels,'ground_y':ground,'view':view,
         'sling_x':scalar('birdZeroPosX')/pixels,'sling_y':scalar('birdZeroPosY')/pixels,'birds':birds,'objects':level}
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
