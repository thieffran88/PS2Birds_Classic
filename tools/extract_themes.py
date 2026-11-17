#!/usr/bin/env python3
"""Stage backgrounds for THEMES.BIN, from themes.lua (data only) and the themes_android sheets:
for each theme the sky colour, the bgLayers behind the stage and, under the ground line,
the ground colour and the fgLayers; every layer is its sprite plus the items of its
composition (THEMEnn.compo.json), scaled to the 640 x 448 screen, anchored on the ground line
and tiled across the width. The result is one opaque 640 x 648 RGBA5551 picture per theme with
the ground line at row 448, followed by the theme's ground texture (`texture`, a 256 x 256
RGB565 PVR in images/base) as RGBA5551 for the fixed terrain pieces; the game takes the rows
around a stage's ground line, paints the terrain pieces with the texture, and copies the
picture into the frame buffer every frame (the camera does not pan, so the parallax factors play
no part). No game script runs."""
import argparse,json,os,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
from lua51_data import load
from extract_sprites import decode_png_frame
from extract_ui import scale
SCREEN=448/768        # the 1024 x 768 layout on the 640 x 448 screen
BASE='assets/data/images/themes_android/'
# Themes whose art is downloaded separately by the original (not in the APK) use a sibling's picture.
FALLBACK={'themeDino2':'themeDino','themeMLExtra':'theme1'}
def num(t,i):return t.get(float(i),t.get(i))
def theme_table(z):
    theme_table.cache={k:v for k,v in load(decode_payload(z.read('assets/data/scripts/themes.lua'),'classic')).items() if isinstance(v,dict) and 'bgLayers' in v}
    return theme_table.cache
def graphics(z,t):
    sets=[v for _,v in sorted(t.get('graphicSetName',{}).items()) if isinstance(v,str)]
    frames={};compo={}
    for gs in sets:
        for n in z.namelist():
            if n.startswith(BASE+gs+'_') and n.endswith('.sheet.json'):
                img=n[:-11]+'.png'
                if img not in z.namelist():continue
                for f in json.loads(decode_payload(z.read(n),'classic'))['frames']:frames.setdefault(f['filename'],(img,f))
        if BASE+gs+'.compo.json' in z.namelist():
            for c in json.loads(decode_payload(z.read(BASE+gs+'.compo.json'),'classic'))['compo']:compo.setdefault(c['name'],c)
    return frames,compo
def compose(z,frames,compo,name,factor):
    """One tile of a layer: RGBA (alpha 0..255), width, height, top relative to the ground line."""
    parts=[(p['name'],p['x'],p['y']) for p in compo[name]['sprites']] if name in compo else [(name,0,0)]
    parts=[p for p in parts if p[0] in frames]
    if not parts:return None
    imgs=[]
    for n,dx,dy in parts:
        png,f=frames[n];rgba=decode_png_frame(z.read(png),f)
        s,w,h=scale(rgba,f['frame']['w'],f['frame']['h'],factor)
        imgs.append((s,w,h,round((dx-f['pivot']['x'])*factor),round((dy-f['pivot']['y'])*factor)))
    tile=round(frames[parts[0][0]][1]['frame']['w']*factor)
    left=min(0,min(x for _,_,_,x,_ in imgs)+tile//2);x0=min(x for *_,x,_ in imgs);y0=min(y for *_,y in imgs)
    x1=max(x+w for _,w,_,x,_ in imgs);y1=max(y+h for _,_,h,_,y in imgs)
    W,H=x1-x0,y1-y0;out=bytearray(W*H*4)
    for s,w,h,ox,oy in reversed(imgs):             # first part (the layer itself) drawn last, on top
        for r in range(h):
            for c in range(w):
                q=(r*w+c)*4;a=min(255,s[q+3]*2)
                if not a:continue
                p=((oy-y0+r)*W+ox-x0+c)*4;b=out[p+3]
                for k in range(3):out[p+k]=round((s[q+k]*a+out[p+k]*(255-a))/255) if b else s[q+k]
                out[p+3]=min(255,a+b*(255-a)//255)
    return out,W,H,y0,x0,tile
_STATE={}
def _init(apk):
    z=zipfile.ZipFile(apk);_STATE.update(z=z,themes=theme_table(z))
def build_theme(name):
    """One theme -> (name, picture bytes + ground texture, True when it has no layers of its own)."""
    z=_STATE['z'];t=_STATE['themes'][name];frames,compo=graphics(z,t);layers=[]
    for kind,key in ((0,'bgLayers'),(1,'fgLayers')):
        for _,L in sorted(t.get(key,{}).items()):
            sprite=num(L,2);factor=(num(L,4) or 1.0)*SCREEN
            if not isinstance(sprite,str) or not sprite:continue
            r=compose(z,frames,compo,sprite,factor)
            if r:layers.append((kind,)+r)
    c=t.get('color',{});g=t.get('groundColor',{})
    col=lambda d,dflt:tuple(int(d.get(k,v)) for k,v in zip('rgb',dflt))
    canvas=compose_canvas(col(c,(146,204,222)),col(g,(10,19,57)),layers)
    return name,(canvas,to5551(canvas)+ground_texture(z,t.get('texture'),col(g,(10,19,57)))),not layers
def extract(apk,out,preview=None,workers=None,log=print):
    """THEMES.BIN. The themes are independent, so they are built in parallel (one worker per core, at most 6);
    the order of the file and the fallbacks of the themes without layers are the same as one after the other."""
    with zipfile.ZipFile(apk) as z:
        themes=theme_table(z);names=sorted(themes,key=lambda n:(themes[n].get('index',999),n))
    if preview:names=[n for n in names if n==preview]
    n=workers if workers is not None else min(6,os.cpu_count() or 1,len(names))
    results=None;pool=None
    if n>1 and not preview:
        try:
            import multiprocessing
            pool=multiprocessing.get_context('spawn').Pool(n,_init,(str(apk),))
            log('    fundos: %d temas em %d processos paralelos'%(len(names),n));results=pool.imap(build_theme,names)
        except Exception as e:
            log('    (processamento em paralelo indisponível: %s; seguindo em um processo)'%e);results=None;pool=None
    if results is None:
        _init(str(apk));results=map(build_theme,names)
    blobs=[];done={}
    for k,(name,(canvas,blob),empty) in enumerate(results,1):
        if empty and FALLBACK.get(name) in done:blobs.append((name,done[FALLBACK[name]]));log('    [%d/%d] tema %s usa o fundo de %s'%(k,len(names),name,FALLBACK[name]));continue
        if preview:Path(out).write_bytes(bytes(canvas[(448-390)*640*3:(448-390+448)*640*3]))
        blobs.append((name,blob));done[name]=blob;log('    [%d/%d] tema %s pronto'%(k,len(names),name))
    if pool:pool.close();pool.join()
    head=struct.pack('<4s4I',b'PBTH',3,len(blobs),640,ABOVE+BELOW);off=len(head)+40*len(blobs);off+=(-off)%2048;pad=off-len(head)-40*len(blobs);table=b''
    for name,blob in blobs:table+=struct.pack('<32s2I',name.encode()[:31],off,len(blob));off+=len(blob)
    if not preview:Path(out).write_bytes(head+table+bytes(pad)+b''.join(b for _,b in blobs))
    return {n:i for i,(n,_) in enumerate(blobs)}
ABOVE,BELOW=448,200
def ground_texture(z,name,fallback):
    """256 x 256 RGBA5551 tile of the theme's ground texture (RGB565 PVR v3), or the ground colour."""
    path=f'assets/data/images/base/{name}.pvr' if isinstance(name,str) else None
    if path and path in z.namelist():
        d=z.read(path);version,flags,fmt,_,_,h,w,depth,surfaces,faces,mips,meta=struct.unpack_from('<IIQIIIIIIIII',d)
        if version==0x03525650 and fmt==0x0005060500626772 and (w,h)==(256,256) and len(d)>=52+meta+w*h*2:
            out=bytearray()
            for i in range(w*h):
                v=struct.unpack_from('<H',d,52+meta+2*i)[0]
                out+=struct.pack('<H',((v>>11)&31)|(((v>>6)&31)<<5)|((v&31)<<10)|0x8000)   # RGB565 -> RGBA5551
            return bytes(out)
    r,g,b=fallback;return struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|0x8000)*(256*256)
def compose_canvas(sky,ground,layers):
    """640 x (ABOVE+BELOW) RGB, ground line at row ABOVE: sky, bgLayers, ground colour, fgLayers."""
    W=640;img=bytearray(bytes(sky)*W*ABOVE+bytes(ground)*W*BELOW)
    for kind,rgba,w,h,top,x0,tile in layers:
        start=x0
        while start>0:start-=tile
        for y in range(h):
            Y=ABOVE+top+y
            if not 0<=Y<ABOVE+BELOW:continue
            sx=start
            while sx<W:
                for x in range(max(0,-sx),min(w,W-sx)):
                    q=(y*w+x)*4;a=rgba[q+3]
                    if a:
                        o=(Y*W+sx+x)*3
                        if a==255:img[o:o+3]=rgba[q:q+3]
                        else:img[o:o+3]=bytes((rgba[q+k]*a+img[o+k]*(255-a))//255 for k in range(3))
                sx+=tile
    return img
def to5551(rgb):
    out=bytearray()
    for i in range(0,len(rgb),3):out+=struct.pack('<H',(rgb[i]>>3)|((rgb[i+1]>>3)<<5)|((rgb[i+2]>>3)<<10)|0x8000)
    return bytes(out)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--preview')
    a=p.parse_args();print(extract(a.apk,a.output,a.preview))
