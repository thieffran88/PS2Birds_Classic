"""Crops named sprites out of the Rio sheets (PVR RGBA4444 from ingame_assets_sd, or PNG) using the
rectangles of the matching .dat files, read by tools/rio/inspect_dat.py's inventory.
usage: crop_sprites.py APK NAME [NAME...]  -> work/rio/sprites/NAME.json (+ .rgba) per sprite"""
import json,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import pvr
from png_decode import decode as decode_png
INV=ROOT/'work/rio/analysis/dat_inventory.json'
def find(names):
    """For each sprite, the first sheet with a rectangle for it that can be read here (PVR before PNG)."""
    inv=json.loads(INV.read_text());hits={}
    for p in inv['parsed']:
        folder=p['path'].split('/')[3]
        if folder not in ('ingame_assets_sd','ingame_assets'):continue
        tex=p.get('texture') or '';ext=tex.rsplit('.',1)[-1].lower()
        for f in p.get('frames') or []:
            if f['name'] in names and ext in ('pvr','png'):
                key=(ext!='pvr',folder!='ingame_assets_sd')   # prefer PVR in the SD set
                if f['name'] not in hits or key<hits[f['name']][0]:hits[f['name']]=(key,folder,tex,f)
    return hits
def sheet(z,folder,tex):
    path='assets/data/images/%s/%s'%(folder,tex)
    b=z.read(path)
    if tex.endswith('.pvr'):return pvr.read_pvr(b)
    w,h,rgba=decode_png(b)[:3];return w,h,rgba
def main(apk,names):
    out=ROOT/'work/rio/sprites';out.mkdir(parents=True,exist_ok=True);hits=find(set(names));cache={};done=[];missing=[]
    with zipfile.ZipFile(apk) as z:
        for n in names:
            if n not in hits:missing.append(n);continue
            _,folder,tex,f=hits[n];k=(folder,tex)
            if k not in cache:cache[k]=sheet(z,folder,tex)
            w,h,px=cache[k];rows=bytearray()
            for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
            (out/(n+'.rgba')).write_bytes(rows)
            (out/(n+'.json')).write_text(json.dumps({'name':n,'sheet':tex,'folder':folder,'w':f['w'],'h':f['h'],'px':f['px'],'py':f['py']}));done.append(n)
    print('recortados',len(done),'| sem folha legível:',missing)
if __name__=='__main__':main(sys.argv[1],sys.argv[2:])
