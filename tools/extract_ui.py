#!/usr/bin/env python3
"""Interface sprites for UI.BIN, one 128 x 1024 page of 8-bit indices with a shared 256-colour
palette (PSMT8 + CT32 CLUT), kept in VRAM next to the stage texture banks:
  - level end (level_end.lua): BIG_STAR_*, NEW_HIGHSCORE_BADGE, BUTTON_MENU/RESTART/NEXTLEVEL,
    LEVEL_FAILED_PIG; pause (PausePage.lua): BUTTON_RESUME, BUTTON_SFX, BUTTON_OFF;
    scaled to 40 % of the 1024 x 768 originals;
  - particles (PARTICLES_1): wood, glass and rock fragments, the feathers of each bird, smoke
    puffs and the coloured score sprites (5K_GREEN for pigs, 10K_<bird> for birds left);
  - P_STAR_1-5 (levelCompleteStars1-3) and EXPLOSION_CLOUD_1-6 (explosionBuff).
The page takes the same 128 KiB as the former 256 x 256 RGBA5551 one; the palette is reduced by
The page is one PSMT8 GS page wide (128 texels), so each 64-row block is one 8 KiB page in a row.
median cut in plain Python (the Windows kit has no image library). Never executes game scripts."""
import argparse,json,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
from extract_sprites import decode_png_frame
SCALE=0.4;PAGE=128;PAGE_H=1024;PARTICLE_SCALE=0.75
# Order = identifiers in ps2/src/ui.h.
UI=[('STAR_1','MENU_GRADIENTS_1','BIG_STAR_1'),('STAR_2','MENU_GRADIENTS_1','BIG_STAR_2'),('STAR_3','MENU_GRADIENTS_1','BIG_STAR_3'),
    ('NEW_HIGHSCORE','MENU_ELEMENTS_1_1','NEW_HIGHSCORE_BADGE'),('MENU','BUTTONS_SHEET_1_1','BUTTON_MENU'),
    ('RESTART','BUTTONS_SHEET_1_1','BUTTON_RESTART'),('NEXT','BUTTONS_SHEET_1_1','BUTTON_NEXTLEVEL'),('FAILED_PIG','MENU_ELEMENTS_2_1','LEVEL_FAILED_PIG'),
    # PausePage.lua: resume and the sound toggle (restart and menu are shared with the level end)
    ('RESUME','BUTTONS_SHEET_1_1','BUTTON_RESUME'),('SOUND','BUTTONS_SHEET_1_1','BUTTON_SFX'),('SOUND_OFF','BUTTONS_SHEET_1_1','BUTTON_OFF')]
# Particles (base/PARTICLES_1.png); order = identifiers in ps2/src/ui.h after the interface ones.
FEATHERS=['RED','BLUE','YELLOW','BLACK','GREEN','BIGBROTHER','BOOMERANG']
PARTICLES=[f'WOOD_PARTICLE_{i}' for i in (1,2,3)]+[f'LIGHT_PARTICLE_{i}' for i in (1,2,3,4,5)]+[f'ROCK_PARTICLE_{i}' for i in (1,2,3)]+\
    [f'{c}_FEATHER_{i}' for c in FEATHERS for i in (1,2,3)]+[f'SMOKE_BUFF_{i}' for i in (1,2,3)]+\
    ['5K_GREEN','10K_RED','10K_BLUE','10K_YELLOW','10K_BLACK','10K_WHITE','10K_BOOMERANG']+\
    [f'P_STAR_{i}' for i in (1,2,3,4,5)]+[f'EXPLOSION_CLOUD_{i}' for i in (1,2,3,4,5,6)]
# Explosion clouds are large; they are stored smaller and drawn scaled up.
CLOUD_SCALE=0.35
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
def median_cut(colours,count):
    """colours: {rgba tuple: pixel count}. Boxes split on their widest channel at the weighted
    median until count boxes; each box gives its weighted mean."""
    boxes=[list(colours.items())]
    while len(boxes)<count:
        best=None
        for i,b in enumerate(boxes):
            if len(b)<2:continue
            spans=[max(c[k] for c,_ in b)-min(c[k] for c,_ in b) for k in range(4)]
            k=max(range(4),key=lambda j:spans[j]);score=spans[k]*sum(n for _,n in b)**0.5
            if best is None or score>best[0]:best=(score,i,k)
        if best is None:break
        _,i,k=best;b=sorted(boxes.pop(i),key=lambda e:e[0][k]);total=sum(n for _,n in b);acc=0;cut=1
        for j,(_,n) in enumerate(b):
            acc+=n
            if acc>=total/2:cut=max(1,min(len(b)-1,j+1));break
        boxes+= [b[:cut],b[cut:]]
    palette=[]
    for b in boxes:
        total=sum(n for _,n in b);palette.append(tuple(round(sum(c[k]*n for c,n in b)/total) for k in range(4)))
    return palette
def clut_slot(i):
    """PSMT8 CLUT in CSM1 storage: entries 8-15 and 16-23 of every 32 are swapped."""
    return (i&~0x18)|((i&0x08)<<1)|((i&0x10)>>1)
def extract(apk,out):
    from extract_sprites import sheet_sprite
    sprites=[]
    with zipfile.ZipFile(apk) as z:
        for ident,sheet,name in UI:
            base='assets/data/images/1024x768/'+sheet
            frames={f['filename']:f for f in json.loads(decode_payload(z.read(base+'.sheet.json'),'classic'))['frames']}
            f=frames[name]
            rgba=decode_png_frame(z.read(base+'.png'),f) if base+'.png' in z.namelist() else decode_pvr_frame(z.read(base+'.pvr'),f)
            rgba,w,h=scale(rgba,f['frame']['w'],f['frame']['h'],SCALE);sprites.append((ident,w,h,rgba))
        for name in PARTICLES:
            rgba,w,h,_=sheet_sprite(z,'PARTICLES_1',name)
            rgba,w,h=scale(rgba,w,h,CLOUD_SCALE if name.startswith('EXPLOSION') else PARTICLE_SCALE);sprites.append((name,w,h,rgba))
    # Shelf packing, tallest first, 1-pixel gutter.
    page=bytearray(PAGE*PAGE_H*4);place={};x=y=shelf=0
    for ident,w,h,rgba in sorted(sprites,key=lambda s:-s[2]):
        if x+w>PAGE:x=0;y+=shelf+1;shelf=0
        if y+h>PAGE_H:raise ValueError('UI sprites do not fit the 128 x 1024 page')
        for r in range(h):page[((y+r)*PAGE+x)*4:((y+r)*PAGE+x+w)*4]=rgba[r*w*4:(r+1)*w*4]
        place[ident]=(x,y,w,h);x+=w+1;shelf=max(shelf,h)
    # 255 colours + transparent (index 0); colours quantised to 5 bits first to keep the table small.
    colours={}
    for i in range(0,len(page),4):
        if page[i+3]>=4:
            c=(page[i]>>2,page[i+1]>>2,page[i+2]>>2,page[i+3]>>2);colours[c]=colours.get(c,0)+1
    palette=[(0,0,0,0)]+median_cut(colours,255)
    nearest={}
    def index(c):
        if c not in nearest:nearest[c]=1+min(range(len(palette)-1),key=lambda j:sum((c[k]-palette[j+1][k])**2 for k in range(4)))
        return nearest[c]
    idx=bytes(0 if page[i+3]<4 else index((page[i]>>2,page[i+1]>>2,page[i+2]>>2,page[i+3]>>2)) for i in range(0,len(page),4))
    clut=[0]*256
    for i,(r,g,b,a) in enumerate(palette):
        clut[clut_slot(i)]=0 if a==0 else min(255,r*4+2)|(min(255,g*4+2)<<8)|(min(255,b*4+2)<<16)|(min(128,a*4)<<24)
    names=[i for i,_,_ in UI]+PARTICLES
    head=struct.pack('<4s4I',b'PBUI',2,len(names),PAGE,PAGE_H)+b''.join(struct.pack('<4H',*place[i]) for i in names)
    head+=bytes((-len(head))%64)
    Path(out).write_bytes(head+struct.pack('<256I',*clut)+idx)
    return {'sprites':len(names),'colours':len(colours),'used_rows':y+shelf}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
    print(extract(a.apk,a.output))
