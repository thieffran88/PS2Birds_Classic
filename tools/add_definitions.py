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
    b=blocks.split(f'L3_1 = "{name}"\n',1)[1].split('\nL3_1 = "',1)[0]
    t=[x or y for x,y in re.findall(r'(?:L5_1 = "(polygon|box|circle)"\nL4_1.type|L4_1.type = "(polygon|box|circle)")',b)]
    s=re.findall(r'L5_1 = "(\w+)"\nL4_1.sprite',b)+re.findall(r'L4_1.sprite = "(\w+)"',b)+re.findall(r'L9_1 = "(\w+)"\nL8_1.sprite',b)
    nums=re.findall(r'"([xy])"\nL\d+_1 = (-?[\d.]+)',b)
    props={k:float(v) for v,k in re.findall(r'L5_1 = (-?[\d.]+)\nL4_1\.(\w+) = L5_1',b)}
    m=re.findall(r'materialName = "(\w+)"',b)
    return dict(type=t[0],sprite=s[0],material=m[0] if m else 'extras',vertices=[[float(nums[i][1]),float(nums[i+1][1])] for i in range(0,len(nums),2)],props=props)
def material(blocks,name):
    n=blocks.index(f'\nL2_1.{name} = L3_1\n');start=blocks.rindex('L3_1 = {}',0,n)
    return {k:float(v) for k,v in re.findall(r'L3_1\.(\w+) = (-?[\d.]+)',blocks[start:n])}
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('blocks',type=Path);p.add_argument('frames',type=Path,help='JSON {sprite:[sheet,width,height]}');p.add_argument('names',nargs='+')
    a=p.parse_args();blocks=a.blocks.read_text().replace('\r','');frames=json.loads(a.frames.read_text())
    prof_path=ROOT/'profiles/classic_level.json';prof=json.loads(prof_path.read_text())
    met_path=ROOT/'ps2/data/sprite_metrics.json';met=json.loads(met_path.read_text())
    for name in a.names:
        d=definition(blocks,name);mat=material(blocks,d['material']);sheet,w,h=frames[d['sprite']]
        spec={'sprite':d['sprite'],'kind':6,'shape':d['type'],'auto':True}
        if d['type']=='polygon':spec['vertices']=d['vertices']
        prof['objects'][name]=spec
        prof['materials']['friction_overrides'][name]=d['props'].get('friction',mat.get('friction',0.7))
        prof['materials']['restitution_overrides'][name]=d['props'].get('restitution',mat.get('restitution',0.4))
        met[d['sprite']]={'width':w,'height':h,'sheet':sheet+'.sheet.json'}
        print(name,spec,prof['materials']['friction_overrides'][name],prof['materials']['restitution_overrides'][name])
    prof_path.write_text(json.dumps(prof,indent=2)+'\n');met_path.write_text(json.dumps(met,indent=2)+'\n')
if __name__=='__main__':main()
