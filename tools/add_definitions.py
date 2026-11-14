#!/usr/bin/env python3
"""Add block definitions recovered from blocks.lua to profiles/classic_level.json.

Reads each definition (type, sprite, polygon vertices, material, own friction/restitution)
from a decompiled blocks.lua, the material table for friction/restitution, and the sprite
frame size from the APK sheets (already decoded to a JSON cache by --cache). Entries are
marked "auto": the game draws them through ps2/src/definition_sprites.h, which
tools/build_campaign.py generates. Lua is only read, never executed.
"""
import argparse,json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def definition(blocks,name):
    # A definition assigned twice keeps the last assignment, as Lua does; entries of other
    # tables that reuse the name (e.g. bird hat offsets) have no shape type and are skipped.
    parts=[p.split('\nL3_1 = "',1)[0] for p in blocks.split(f'L3_1 = "{name}"\nL4_1 = {{}}\n')[1:]]
    parts=[p for p in parts if re.search(r'L4_1.type = (L5_1|"(polygon|box|circle)")',p)]
    if not parts:raise ValueError('Definition not found: '+name)
    b=parts[-1]
    t=[x or y for x,y in re.findall(r'(?:L5_1 = "(polygon|box|circle)"\nL4_1.type|L4_1.type = "(polygon|box|circle)")',b)]
    s=re.findall(r'L5_1 = "(\w+)"\nL4_1.sprite = L5_1',b)+re.findall(r'L4_1.sprite = "(\w+)"',b)+re.findall(r'L9_1 = "(\w+)"\nL8_1.sprite = L9_1',b)
    nums=re.findall(r'"([xy])"\nL\d+_1 = (-?[\d.]+)',b)
    props={k:float(v) for v,k in re.findall(r'L5_1 = (-?[\d.]+)\nL4_1\.(\w+) = L5_1',b)}
    props.update({k:float(v) for k,v in re.findall(r'L5_1 = "(width|height|targetVelocity)"\nL6_1 = (-?[\d.]+)',b)})
    if re.search(r'L5_1 = false\nL4_1.collision = L5_1',b):props['no_collision']=True
    m=re.findall(r'materialName = "(\w+)"',b)
    return dict(type=t[0],sprite=s[0],material=m[0] if m else 'extras',vertices=[[float(nums[i][1]),float(nums[i+1][1])] for i in range(0,len(nums),2)],props=props)
def material(blocks,name):
    n=blocks.index(f'\nL2_1.{name} = L3_1\n');start=blocks.rindex('L3_1 = {}',0,n)
    return {k:float(v) for k,v in re.findall(r'L3_1\.(\w+) = (-?[\d.]+)',blocks[start:n])}
def material_has_no_collider(blocks,name):
    """Materials such as decoration_new declare collider = collider_types.none: pure scenery."""
    n=blocks.index(f'\nL2_1.{name} = L3_1\n');start=blocks.rindex('L3_1 = {}',0,n)
    return 'L4_1 = L4_1.none\nL3_1.collider = L4_1' in blocks[start:n]
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('blocks',type=Path);p.add_argument('frames',type=Path,help='JSON {sprite:[sheet,width,height]}');p.add_argument('names',nargs='+');p.add_argument('--material-kinds',action='store_true',help='kind from the material (wood 1, light 2, rock 5, immovableFragile 9) instead of extras')
    a=p.parse_args();blocks=a.blocks.read_text().replace('\r','');frames=json.loads(a.frames.read_text())
    prof_path=ROOT/'profiles/classic_level.json';prof=json.loads(prof_path.read_text())
    met_path=ROOT/'profiles/sprite_metrics.json';met=json.loads(met_path.read_text())
    for name in a.names:
        d=definition(blocks,name);mat=material(blocks,d['material']);sheet,w,h=frames[d['sprite']]
        spec={'sprite':d['sprite'],'kind':6,'shape':d['type'],'auto':True}
        if d['type']=='polygon':spec['vertices']=d['vertices']
        # Boxes with their own width/height (ObjectCreation.lua uses them instead of the sprite bounds).
        if d['type']=='box' and 'width' in d['props'] and 'height' in d['props']:spec['size']=[d['props']['width'],d['props']['height']]
        if d['type']=='circle' and 'radius' in d['props']:spec['size']=[2*d['props']['radius'],2*d['props']['radius']]
        # Materials with forceX/forceY (balloon) push the piece every step (setObjectAsForceAdder).
        # The definition's own forceX/forceY take precedence (e.g. BP_SINGLE_BALLOON, -80).
        fx=d['props'].get('forceX',mat.get('forceX'));fy=d['props'].get('forceY',mat.get('forceY'))
        if fx is not None or fy is not None:spec['force']=[fx or 0.0,fy or 0.0]
        if 'targetVelocity' in d['props'] and 'force' in spec:spec['force_target']=d['props']['targetVelocity']
        # collision = false: drawn but not touched (a Box2D sensor in the port).
        if d['props'].get('no_collision') or material_has_no_collider(blocks,d['material']):spec['no_collision']=True
        if material_has_no_collider(blocks,d['material']):spec['kind']=3
        # immovable material: static scenery, drawn like the ground pieces and not counted as a block.
        if d['material']=='immovable':spec['kind']=3
        # Birdday 7 onwards: themed copies of the wood/glass/stone blocks and fragile scenery keep
        # their material's kind (damage factors, sounds, score), as in the original.
        if a.material_kinds and d['material'] in ('wood','light','rock','immovableFragile','piglette'):spec['kind']={'wood':1,'light':2,'rock':5,'immovableFragile':9,'piglette':4}[d['material']]
        prof['objects'][name]=spec
        prof['materials']['friction_overrides'][name]=d['props'].get('friction',mat.get('friction',0.7))
        prof['materials']['restitution_overrides'][name]=d['props'].get('restitution',mat.get('restitution',0.4))
        met[d['sprite']]={'width':w,'height':h,'sheet':sheet+'.sheet.json'}
        print(name,spec,prof['materials']['friction_overrides'][name],prof['materials']['restitution_overrides'][name])
    prof_path.write_text(json.dumps(prof,indent=2)+'\n');met_path.write_text(json.dumps(met,indent=2)+'\n')
if __name__=='__main__':main()
