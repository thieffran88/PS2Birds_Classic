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
# The Rio birds (and their R2_ variants, same radius and density as the Classic's) run as the runtime's BirdType values
# (game.h: 0 red, 1 blue, 2 yellow, 3 black, 4 white, 6 boomerang, 7 big red, 8 globe, 9 pink). Same definition names as the Classic's runtime types (profiles/classic_level.json bird_types). The Rio birds have
# their own radius/density (e.g. the yellow bird 0.8, the small blue bird 0.6); the runtime still uses the
# Classic's values for each type, so these are approximations until the bird parameters are imported.
BIRD_TYPE={'RedBird':0,'R2_RedBird':0,'SmallBlueBird':1,'R2_SmallBlueBird':1,'YellowBird':2,'R2_YellowBird':2,'BlackBird':3,'R2_BlackBird':3,
    'WhiteBird':4,'R2_WhiteBird':4,'BoomerangBird':6,'R2_BoomerangBird':6,'RedBigBird':7,'R2_RedBigBird':7,'GlobeBird':8,'R2_GlobeBird':8,'PinkBird':9,'R2_PinkBird':9}
def fit_view(r,zoom=None):
    """The Classic's camera rule (tools/build_campaign.py): the whole stage in the 576 x 336 play area, scale at most 7."""
    import math
    left=r['sling_x']-4;right=r['sling_x']+4;top=r['sling_y']-5
    for o in r['objects']:
        cs=abs(math.cos(o['angle']));sn=abs(math.sin(o['angle']))
        ex=(o['width']*cs+o['height']*sn)/2;ey=(o['width']*sn+o['height']*cs)/2
        left=min(left,o['x']-ex);right=max(right,o['x']+ex);top=min(top,o['y']-ey)
    scale=min(7,576/(right-left),210/max(1,r['ground_y']-top))
    if zoom and scale<zoom:   # wider than the screen: the original camera's zoom, the game scrolls to follow the bird (game_camera)
        scale=min(max(zoom,3.0),8.0,290/max(1,r['ground_y']-top))
        left=r['sling_x']-4
    return {'scale':scale,'x':32-left*scale,'y':368-r['ground_y']*scale}
def level_zoom(path):
    """Zoom of the original iPhone camera (screen pixels per world unit, scaled from 480 to the 576-pixel play area)."""
    from lua51_data import load
    d=load(Path(path).read_bytes());cam=(d.get('castleCameraData') or {}).get('iphone') or {}
    return cam.get('sx',0)*d.get('physicsToWorld',20.0)*576/480
def level_theme(path):
    from lua51_data import load
    return load(Path(path).read_bytes()).get('theme','theme2')
def stage(path,gravity,index,themes=None):
    r=convert(path,gravity)
    th=level_theme(path)
    unknown=[b['definition'] for b in r['birds'] if b['definition'] not in BIRD_TYPE]
    if unknown:raise ValueError('pássaro sem tipo no runtime: %s'%sorted(set(unknown)))
    return dict(objects=r['objects'],joints=r['joints'],birds=[{'type':BIRD_TYPE[b['definition']]} for b in r['birds']],gravity=r['gravity'],
        sling_x=r['sling_x'],sling_y=r['sling_y'],ground_y=r['ground_y'],physics_to_world=r['physics_to_world'],
        view=fit_view(r,level_zoom(path)),theme_index=(themes.index(th) if themes and th in themes else 0),title='RIO %d'%index,stage_script=Path(path).stem)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('out',type=Path);p.add_argument('levels',nargs='+',type=Path);p.add_argument('--gravity',type=float,default=20.0)
    a=p.parse_args();levels=[stage(f,a.gravity,i+1) for i,f in enumerate(a.levels)]
    classic.write_binary(levels,{'objects':{}},[],[],{},a.out)
    r=classic.read_binary(a.out);print('gravado',a.out,'| fases',len(r['levels']),'| objetos',len(r['objects']),'|',r['levels'][0])
