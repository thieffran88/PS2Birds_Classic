#!/usr/bin/env python3
"""Compares the original camera of each stage with the port's fixed view.

The original levels carry castleCameraData and birdCameraData (px, py, sx, sy for a 1024 x 768 screen): the stage starts on the
"castle" snap point, waits 2 s and pans to the "bird" snap point in 1 s (DefaultLevelStartCameraSequence), then follows the shot bird
(FollowShotBirdCameraSequence). The port draws the whole stage on one 640 x 448 screen, at about half the original zoom, so there is
nothing to pan. This tool measures how much that differs.

Usage: python3 tools/audit_camera.py com.rovio.angrybirds.apk"""
import glob,json,statistics,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from game_profiles import decode_payload
from lua51_data import load
def main(apk):
    z=zipfile.ZipFile(apk);imp={}
    for f in glob.glob(str(ROOT/'ps2/data/level_*.json')):
        j=json.loads(Path(f).read_text());imp[j['stage_script']]=j
    rows=[]
    for n in z.namelist():
        if not (n.startswith('assets/data/levels/') and n.endswith('.lua')):continue
        s=n.split('/')[-1][:-4]
        if s not in imp:continue
        d=load(decode_payload(z.read(n),'classic',None));c=d['castleCameraData']['ipad'];b=d['birdCameraData']['ipad'];j=imp[s]
        pixels=j['physics_to_world']
        rows.append(dict(script=s,zoom_ratio=c['sx']/b['sx'],pan_screens=abs(c['px']-b['px'])*c['sx']/c['screenWidth'],
            original_px_per_unit=b['sx']*pixels*448/768,port_px_per_unit=j['view']['scale']))
    q=lambda v:'min %.2f, median %.2f, max %.2f'%(min(v),statistics.median(v),max(v))
    print('stages:',len(rows))
    print('zoom of the castle view / bird view:',q([r['zoom_ratio'] for r in rows]))
    print('pan between the two views, in screens:',q([r['pan_screens'] for r in rows]),'; stages with no pan (<0.05):',sum(r['pan_screens']<0.05 for r in rows))
    print('port zoom / original zoom (pixels per physics unit):',q([r['port_px_per_unit']/r['original_px_per_unit'] for r in rows]))
    return 0
if __name__=='__main__':sys.exit(main(sys.argv[1]))
