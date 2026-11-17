#!/usr/bin/env python3
"""Comic cutscenes for CUTSCENE.BIN. Everything about a scene comes from the APK itself:
cutscenes.lua is read as data (tools/lua51_data.py, no game code runs) and the pictures are
composed from CUTSCENES.compo.json, as the game's own cutscene player lays them out.

Per scene the script gives (cutscenes.lua actions):
  setreferencesize   width and height of the picture, fitheight: scaled to the screen height;
  createsprite       the picture ("background" = a composition of CUTSCENES.compo.json);
  playsound          the music clip;
  scroll             start time and duration of the cubic_in_out scroll, left to right;
  end                the time the scene ends.
Other actions (the King Pig's eyes, one-shot sounds) are ignored; scenes that need more than a
horizontal scroll (vertical scroll, tweened panels) are not in SCENES.

The picture is composed here (pieces at their pivot, back to front), scaled to the 448-row
screen and stored as opaque RGBA5551 rows: the game copies the visible 640 columns into the
frame each frame, as it does with the stage backgrounds. Nothing of the game is kept in the
repository: the scenes are built from the APK when the data files are generated."""
import argparse,json,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
import lua51_data
from extract_menu import frames_of,read
from extract_ui import scale
from extract_audio import MUSIC_OF_CLIP

# cutscenes.lua scene names that the player can show (see docs/CUTSCENES.md).
SCENES=['pack1_intro','world1_complete','world2_complete','world3_complete',
        'pack2_intro','world4_complete','world5_complete',
        'world6_complete','world7_complete','world8_complete',
        'pack4_intro','world9_complete','world10_complete','world11_complete',
        'pack5_intro','world12_complete','world13_complete','world14_complete',
        'pack6_intro','world15_complete','world16_complete','world17_complete',
        'pack7_intro','birdday3_complete',
        'pack8_intro','worldS1_complete','worldS2_complete','worldS3_complete']
# Not here: pack3_intro (vertical scroll), the Egg Defender scenes (tweened panels) and the
# Short Fuse, Piggy Farm and Dino stories (WebP sheets, which this generator cannot decode).
COMPOS=['CUTSCENES.compo.json']
SCREEN_H=448;SCREEN_W=640

def parse(script):
    """The parameters of one scene from its list of actions."""
    acts=[script[k] for k in sorted(script)]
    ref=[a for a in acts if a['action']=='setreferencesize'][0]
    pic=[a for a in acts if a['action']=='createsprite' and a.get('name')=='background'][0]
    scroll=[a for a in acts if a['action']=='scroll']
    clips=[a['sound'] for a in acts if a['action']=='playsound' and a.get('sound') in MUSIC_OF_CLIP]
    return {'image':pic['image'],'ref_w':int(ref['width']),'ref_h':int(ref['height']),
            'music':MUSIC_OF_CLIP[clips[0]] if clips else -1,
            'scroll_start':scroll[0]['time'] if scroll else 0.0,'scroll':scroll[0]['duration'] if scroll else 0.0,
            'end':[a for a in acts if a['action']=='end'][0]['time']}

def extent(where,compo):
    """Right edge of the pieces of a composition (x, y place each piece's pivot)."""
    right=0
    for s in compo['sprites']:
        f=where[s['name']][1];fw=f['frame']['w'];pv=f.get('pivot',{'x':fw/2})
        right=max(right,s['x']-pv['x']+fw)
    return int(round(right))

def compose(z,where,compo,w,h):
    canvas=bytearray(w*h*4)
    # The list runs front to back (the sky and grass gradients come last): drawn in reverse.
    for s in reversed(compo['sprites']):
        rgba,f=read(z,where,s['name']);fw,fh=f['frame']['w'],f['frame']['h']
        px,py=f.get('pivot',{}).get('x',fw/2),f.get('pivot',{}).get('y',fh/2)
        sx,sy=s.get('scale',[1,1])
        if (sx,sy)!=(1,1):rgba,fw,fh=scale(rgba,fw,fh,abs(sx));px*=abs(sx);py*=abs(sx)
        x0,y0=int(round(s['x']-px)),int(round(s['y']-py));solid='GRADIENT' in s['name']
        for r in range(fh):
            y=y0+r
            if not 0<=y<h:continue
            for c in range(fw):
                x=x0+c
                if not 0<=x<w:continue
                i=(r*fw+c)*4;a=min(255,(rgba[i+3]*255+64)//128)   # source alpha is on the GS scale, 0..128
                if not a:continue
                if solid:a=255   # the sky/grass gradient tiles overlap with soft edges: drawn opaque, no seams
                o=(y*w+x)*4
                for k in range(3):canvas[o+k]=(rgba[i+k]*a+canvas[o+k]*(255-a))//255
                canvas[o+3]=255
    return canvas

def to5551(rgba):
    out=bytearray()
    for i in range(0,len(rgba),4):
        r,g,b=rgba[i],rgba[i+1],rgba[i+2];out+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15))
    return out

def pad_width(rgba,w,h,target):
    """A picture narrower than the screen is centred on its own edge colours."""
    out=bytearray(target*h*4);left=(target-w)//2
    for y in range(h):
        row=rgba[y*w*4:(y+1)*w*4]
        for x in range(target):
            sx=min(max(x-left,0),w-1);out[(y*target+x)*4:(y*target+x)*4+4]=row[sx*4:sx*4+4]
    return out

def extract(apk,out,names=None):
    """CUTSCENE.BIN: "PBCS", version 3, scene count, then per scene a 24-byte name (NUL padded),
    width, height, scroll start ms, scroll ms, end ms, data offset, music track (-1: none), and
    the RGBA5551 rows."""
    names=list(names or SCENES)
    with zipfile.ZipFile(apk) as z:
        where=frames_of(z)
        comp={}
        for f in COMPOS:
            comp.update({c['name']:c for c in json.loads(decode_payload(z.read('assets/data/images/1024x768/'+f),'classic'))['compo']})
        script=lua51_data.load(decode_payload(z.read('assets/data/scripts/cutscenes.lua'),'classic'))
        blobs=[]
        for name in names:
            p=parse(script[name])
            if p['image'] not in comp:print('cena %s ignorada: %s não é uma composição'%(name,p['image']));continue
            # Only PNG and PVR sheets can be read here (the Short Fuse story is WebP).
            have=set(z.namelist());bases={where[q['name']][0] for q in comp[p['image']]['sprites']}
            bad=sorted(b.split('/')[-1] for b in bases if b+'.png' not in have and b+'.pvr' not in have)
            if bad:print('cena %s ignorada: formato de imagem sem suporte (%s)'%(name,bad[0]));continue
            right=extent(where,comp[p['image']])
            # The script's reference width can be a little wider than what the pieces cover:
            # the real extent is used, so no empty strip is scrolled into view.
            w=min(p['ref_w'],right) if right else p['ref_w'];h=p['ref_h']
            canvas=compose(z,where,comp[p['image']],w,h)
            small,sw,sh=scale(canvas,w,h,SCREEN_H/h)
            if sw<SCREEN_W:small=pad_width(small,sw,sh,SCREEN_W);sw=SCREEN_W
            blobs.append((name,sw,sh,int(p['scroll_start']*1000),int(p['scroll']*1000),int(p['end']*1000),p['music'],to5551(small)))
    head=struct.pack('<4s2I',b'PBCS',3,len(blobs));offset=len(head)+52*len(blobs);offset+=(-offset)%64
    table=b'';pos=offset
    for name,sw,sh,st,sc,en,mu,px in blobs:table+=struct.pack('<24s6Ii',name.encode()[:23],sw,sh,st,sc,en,pos,mu);pos+=len(px)+(-len(px))%64
    data=bytearray(head+table);data+=bytes(offset-len(data))
    for *_,px in blobs:data+=px+bytes((-len(px))%64)
    Path(out).write_bytes(data)
    return {'scenes':len(blobs),'bytes':len(data),'sizes':sorted({(b[1],b[2]) for b in blobs})}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk');p.add_argument('output')
    a=p.parse_args();print(extract(a.apk,a.output))
