"""THEMES.BIN for the Rio stages ("PBTH" v3, the format the runtime already reads), built from the player's
own APK: themes.lua (data only), the PVR sheets of themes_android and the .dat frames. Each layer of a
theme is one sprite (not a composition): scaled by field 4 of the layer times the 448/768 screen factor, its
pivot on the ground line, tiled across 640 columns, exactly as tools/extract_themes.py does for the Classic.
The ground texture (a 32 x 32 tile) is repeated to the 256 x 256 the runtime expects.
usage: extract_themes_rio.py APK OUT.BIN [--themes theme2 ...]"""
import argparse,json,struct,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import pvr
from extract_themes import compose_canvas,to5551,SCREEN,ABOVE,BELOW
from extract_ui import scale
from lua51_data import load
from game_profiles import decode_payload
BASE='assets/data/images/themes_android/'
def frames_of(z):
    """sprite name -> (sheet pvr name, x, y, w, h, pivot x, pivot y) from the .dat files of themes_android."""
    inv=json.loads((ROOT/'work/rio/analysis/dat_inventory.json').read_text());out={}
    for p in inv['parsed']:
        if '/themes_android/' not in p['path']:continue
        for f in p.get('frames') or []:out[f['name']]=(p['texture'],f['x'],f['y'],f['w'],f['h'],f['px'],f['py'])
    return out
def layer(z,frames,cache,name,factor):
    if name not in frames:return None
    tex,x,y,w,h,px,py=frames[name]
    if not tex.endswith('.pvr'):return None   # WebP/PNG sheets: not readable here
    if tex not in cache:cache[tex]=pvr.read_pvr(z.read(BASE+tex))
    sw,sh,rgba=cache[tex];crop=bytearray()
    for r in range(y,y+h):crop+=rgba[(r*sw+x)*4:(r*sw+x+w)*4]
    s,ww,hh=scale(bytes(crop),w,h,factor)
    return bytes(s),ww,hh,round(-py*factor),round(-px*factor),round(w*factor)   # top, x0, tile (as the Classic)
def ground(z,name):
    d=z.read(BASE+name+'.pvr');sw,sh,rgba=pvr.read_pvr(d);out=bytearray()
    for y in range(256):
        for x in range(256):
            i=((y%sh)*sw+(x%sw))*4;out+=struct.pack('<H',(rgba[i]>>3)|((rgba[i+1]>>3)<<5)|((rgba[i+2]>>3)<<10)|0x8000)
    return bytes(out)
def num(t,i):return t.get(float(i),t.get(i))
def extract(apk,out,themes):
    blobs=[];cache={}
    with zipfile.ZipFile(apk) as z:
        table=load(decode_payload(z.read('assets/data/scripts/themes.lua'),'rio'))
        frames=frames_of(z)
        for name in themes:
            t=table[name];layers=[]
            for kind,key in ((0,'bgLayers'),(1,'fgLayers')):
                for _,L in sorted((t.get(key) or {}).items()):
                    sprite=num(L,2);factor=(num(L,4) or 1.0)*SCREEN
                    r=layer(z,frames,cache,sprite,factor) if isinstance(sprite,str) else None
                    if r:layers.append((kind,)+r)
                    else:print('camada sem folha legível:',name,sprite)
            sky=tuple(int(t.get('skyColor',{}).get(k,0)) for k in 'rgb');gr=tuple(int(t.get('groundColor',{}).get(k,0)) for k in 'rgb')
            canvas=compose_canvas(sky,gr,layers)
            tex=t.get('texture');blob=to5551(canvas)+(ground(z,tex) if isinstance(tex,str) else struct.pack('<H',0x8000)*(256*256))
            blobs.append((name,blob))
    head=struct.pack('<4s4I',b'PBTH',3,len(blobs),640,ABOVE+BELOW);off=len(head)+40*len(blobs);off+=(-off)%2048;pad=off-len(head)-40*len(blobs);tab=b''
    for n,b in blobs:tab+=struct.pack('<32s2I',n.encode()[:31],off,len(b));off+=len(b)
    Path(out).parent.mkdir(parents=True,exist_ok=True);Path(out).write_bytes(head+tab+bytes(pad)+b''.join(b for _,b in blobs))
    print('THEMES.BIN do Rio:',len(blobs),'tema(s),',sum(len(b) for _,b in blobs),'bytes')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--themes',nargs='+',default=['theme2'])
    a=p.parse_args();extract(a.apk,a.output,a.themes)
