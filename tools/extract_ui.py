#!/usr/bin/env python3
"""Level-end interface sprites (level_end.lua: BIG_STAR_*, NEW_HIGHSCORE_BADGE, BUTTON_MENU/
RESTART/NEXTLEVEL, LEVEL_FAILED_PIG) for UI.BIN: one 256 x 256 RGBA5551 page that stays in
VRAM next to the stage texture banks. Frames are scaled to 40 % (the originals are drawn for
1024 x 768). Reads the PNG sheets of the APK; never executes game scripts."""
import argparse,json,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
from extract_sprites import decode_png_frame
SCALE=0.4;PAGE=256
# Order = identifiers in ps2/src/ui.h.
UI=[('STAR_1','MENU_GRADIENTS_1','BIG_STAR_1'),('STAR_2','MENU_GRADIENTS_1','BIG_STAR_2'),('STAR_3','MENU_GRADIENTS_1','BIG_STAR_3'),
    ('NEW_HIGHSCORE','MENU_ELEMENTS_1_1','NEW_HIGHSCORE_BADGE'),('MENU','BUTTONS_SHEET_1_1','BUTTON_MENU'),
    ('RESTART','BUTTONS_SHEET_1_1','BUTTON_RESTART'),('NEXT','BUTTONS_SHEET_1_1','BUTTON_NEXTLEVEL'),('FAILED_PIG','MENU_ELEMENTS_2_1','LEVEL_FAILED_PIG')]
def scale(rgba,w,h,f):
    """Area-average resample (alpha-weighted colour) to round(w*f) x round(h*f)."""
    W,H=max(1,round(w*f)),max(1,round(h*f));out=bytearray(W*H*4)
    for Y in range(H):
        y0,y1=Y/f,min(h,(Y+1)/f)
        for X in range(W):
            x0,x1=X/f,min(w,(X+1)/f);acc=[0.0]*4;area=0.0
            for y in range(int(y0),min(h,int(y1+0.999))):
                wy=min(y+1,y1)-max(y,y0)
                if wy<=0:continue
                for x in range(int(x0),min(w,int(x1+0.999))):
                    wx=min(x+1,x1)-max(x,x0)
                    if wx<=0:continue
                    a=wx*wy;p=(y*w+x)*4;al=rgba[p+3]
                    acc[0]+=rgba[p]*al*a;acc[1]+=rgba[p+1]*al*a;acc[2]+=rgba[p+2]*al*a;acc[3]+=al*a;area+=a
            q=(Y*W+X)*4;al=acc[3]
            if al>0:out[q:q+3]=bytes(min(255,round(c/al)) for c in acc[:3])
            out[q+3]=round(al/area) if area else 0
    return out,W,H
def decode_pvr_frame(data,frame):
    """Crop a frame from a PVR v3 sheet stored as packed RGBA4444 (r in the high nibble),
    premultiplied alpha (flag 2): un-premultiplied, alpha scaled to the GS 0..128 range."""
    version,flags,fmt,_,_,height,width,depth,surfaces,faces,mips,meta=struct.unpack_from('<IIQIIIIIIIII',data)
    if version!=0x03525650 or fmt!=0x0404040461626772 or depth!=1 or surfaces!=1 or faces!=1:raise ValueError('Unsupported PVR sheet')
    base=52+meta;f=frame['frame'];x,y,w,h=(f[k] for k in ['x','y','w','h'])
    if frame.get('rotated') or x+w>width or y+h>height or len(data)<base+width*height*2:raise ValueError('PVR frame outside sheet')
    out=bytearray()
    for r in range(h):
        for c in range(w):
            v=struct.unpack_from('<H',data,base+((y+r)*width+x+c)*2)[0]
            R,G,B,A=(v>>12)*17,((v>>8)&15)*17,((v>>4)&15)*17,(v&15)*17
            if flags&2 and A:R,G,B=(min(255,round(ch*255/A)) for ch in (R,G,B))
            out+=bytes((R,G,B,(A*128+127)//255))
    return out
def to5551(rgba):
    """GS CT16: 5 bits per colour, alpha bit set when the (0..128) alpha is at least half."""
    o=bytearray()
    for i in range(0,len(rgba),4):
        r,g,b,a=rgba[i:i+4]
        # Transparent texels are black: with TEXA.AEM the GS then draws them fully transparent.
        o+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15) if a>=64 else 0)
    return o
def extract(apk,out):
    sprites=[]
    with zipfile.ZipFile(apk) as z:
        for ident,sheet,name in UI:
            base='assets/data/images/1024x768/'+sheet
            frames={f['filename']:f for f in json.loads(decode_payload(z.read(base+'.sheet.json'),'classic'))['frames']}
            f=frames[name]
            rgba=decode_png_frame(z.read(base+'.png'),f) if base+'.png' in z.namelist() else decode_pvr_frame(z.read(base+'.pvr'),f)
            rgba,w,h=scale(rgba,f['frame']['w'],f['frame']['h'],SCALE);sprites.append((ident,w,h,rgba))
    # Shelf packing, tallest first, 1-pixel gutter.
    page=bytearray(PAGE*PAGE*4);place={};x=y=shelf=0
    for ident,w,h,rgba in sorted(sprites,key=lambda s:-s[2]):
        if x+w>PAGE:x=0;y+=shelf+1;shelf=0
        if y+h>PAGE:raise ValueError('UI sprites do not fit the 256 x 256 page')
        for r in range(h):page[((y+r)*PAGE+x)*4:((y+r)*PAGE+x+w)*4]=rgba[r*w*4:(r+1)*w*4]
        place[ident]=(x,y,w,h);x+=w+1;shelf=max(shelf,h)
    head=struct.pack('<4s3I',b'PBUI',1,len(UI),PAGE)+b''.join(struct.pack('<4H',*place[i]) for i,_,_ in UI)
    head+=bytes((-len(head))%64)
    Path(out).write_bytes(head+to5551(page))
    return place
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
    print(extract(a.apk,a.output))
