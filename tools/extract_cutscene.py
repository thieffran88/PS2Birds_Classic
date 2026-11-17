#!/usr/bin/env python3
"""Comic cutscene for CUTSCENE.BIN, from cutscenes.lua (read as data, translated by hand) and the
CUTSCENES.compo.json composition of the APK. First scene: pack1_intro, the opening of Poached
Eggs. cutscenes.lua: image STORY_1_BEGINNING, setreferencesize 2048 x 659, fitheight, music
birds_intro, then a 9.6 s cubic_in_out scroll from the left edge to the right edge; the scene
ends at 15.6 s. The picture is composed here (pieces at their pivot, back to front, unrotated in
this scene), scaled to the 448-row screen and stored as opaque RGBA5551 rows: the game copies the
visible 640 columns into the frame each frame, as it does with the stage backgrounds.
No game script runs; nothing of the game is kept in the repository."""
import argparse,json,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
from extract_menu import frames_of,read
from extract_ui import scale
from extract_audio import MUSIC_OF_CLIP

# One entry per scene: (cutscenes.lua scene name, CUTSCENES.compo.json image, setreferencesize
# width and height, scroll duration s, end time s, music clip). The game looks scenes up by name
# and plays each one where cutscenes.lua does; a scene missing here is simply skipped. To add a
# scene, read its block in cutscenes.lua (image, setreferencesize, the playsound clip, the two
# times) and add a line: see docs/CUTSCENES.md. The music clip must be in extract_audio.MUSIC.
SCENES=[('pack1_intro','STORY_1_BEGINNING',2048,659,9.6,15.6,'birds_intro'),
        ('world1_complete','STORY_1_MIDDLE_1',1368,659,2.0,7.5,'birds_boss'),
        ('world2_complete','STORY_1_MIDDLE_2',1368,659,2.0,7.5,'birds_boss')]
SCREEN_H=448

def compose(z,where,compo,w,h):
    canvas=bytearray(w*h*4)
    # The list runs front to back (the sky and grass gradients come last): drawn in reverse.
    # x, y place the sprite's pivot (sheet.json pivot), as the game's drawSprite does.
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

def extract(apk,out):
    """CUTSCENE.BIN: "PBCS", version 2, scene count, then per scene a 24-byte name (NUL padded),
    width, height, scroll ms, end ms, data offset, music track, and the RGBA5551 rows."""
    with zipfile.ZipFile(apk) as z:
        where=frames_of(z)
        comp={c['name']:c for c in json.loads(decode_payload(z.read('assets/data/images/1024x768/CUTSCENES.compo.json'),'classic'))['compo']}
        blobs=[]
        for name,image,w,h,scroll_s,end_s,clip in SCENES:
            canvas=compose(z,where,comp[image],w,h)
            small,sw,sh=scale(canvas,w,h,SCREEN_H/h)
            blobs.append((name,sw,sh,int(scroll_s*1000),int(end_s*1000),MUSIC_OF_CLIP[clip],to5551(small)))
    head=struct.pack('<4s2I',b'PBCS',2,len(blobs));offset=len(head)+48*len(blobs);offset+=(-offset)%64
    table=b'';pos=offset
    for name,sw,sh,sc,en,mu,px in blobs:table+=struct.pack('<24s6I',name.encode()[:23],sw,sh,sc,en,pos,mu);pos+=len(px)+(-len(px))%64
    data=bytearray(head+table);data+=bytes(offset-len(data))
    for *_,px in blobs:data+=px+bytes((-len(px))%64)
    Path(out).write_bytes(data)
    return {'scenes':[b[0] for b in blobs],'sizes':[(b[1],b[2]) for b in blobs],'bytes':len(data)}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk');p.add_argument('output')
    a=p.parse_args();print(extract(a.apk,a.output))
