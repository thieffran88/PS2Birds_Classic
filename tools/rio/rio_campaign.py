"""Writes a CAMPAIGN.BIN with Rio stages (experimental, not part of the ISO yet).
Each stage is converted by tools/rio/rio_import_level.py and stored with build_campaign.write_binary,
the same writer the Classic uses. usage: rio_campaign.py OUT.BIN LEVEL.lua [LEVEL.lua ...] [--gravity G]
The Rio stages carry no gravity: the default (20, the value of the Classic files, whose y axis also
points down) is only a placeholder until it is checked against the APK."""
import argparse,json,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import build_campaign as classic
from rio_import_level import convert
BIRD_TYPE={'RedBird':0}
def stage(path,gravity,index):
    r=convert(path,gravity)
    unknown=[b['definition'] for b in r['birds'] if b['definition'] not in BIRD_TYPE]
    if unknown:raise ValueError('pássaro sem tipo no runtime: %s'%sorted(set(unknown)))
    return dict(objects=r['objects'],joints=[],birds=[{'type':BIRD_TYPE[b['definition']]} for b in r['birds']],gravity=r['gravity'],
        sling_x=r['sling_x'],sling_y=r['sling_y'],ground_y=r['ground_y'],physics_to_world=r['physics_to_world'],
        view={'x':210,'y':333,'scale':7},title='RIO %d'%index,stage_script=Path(path).stem)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('out',type=Path);p.add_argument('levels',nargs='+',type=Path);p.add_argument('--gravity',type=float,default=20.0)
    a=p.parse_args();levels=[stage(f,a.gravity,i+1) for i,f in enumerate(a.levels)]
    classic.write_binary(levels,{'objects':{}},[],[],{},a.out)
    r=classic.read_binary(a.out);print('gravado',a.out,'| fases',len(r['levels']),'| objetos',len(r['objects']),'|',r['levels'][0])
