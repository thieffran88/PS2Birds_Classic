#!/usr/bin/env python3
"""Original bitmap fonts for FONT.BIN: FONT_BASIC (HUD texts, white with a black outline) and
FONT_SCORE (floating scores, white with an orange outline), read from the APK's KA3D glyph
tables (.dat) and RGBA4444 PVR sheets. Both go on one 4-bit (PSMT4) page with a 16-colour
palette per font, small enough for the VRAM left beside the stage textures and UI.BIN.
Never executes game scripts."""
import argparse,struct,zipfile
from pathlib import Path
from extract_ui import decode_pvr_frame,scale
PAGE_W=256
CHARSET=''.join(chr(c) for c in range(32,127))+'ÀÁÂÃÇÉÊÍÓÔÕÚàáâãçéêíóôõú©'
# (name in the APK, scale from the 1024 x 768 originals, characters kept)
FONTS=[('FONT_BASIC',0.44,CHARSET),('FONT_SCORE',0.32,None)]
def glyph_table(dat):
    """KA3D FONT chunk: name, line height (u8), u16, count (u16), then per glyph six big-endian
    u16: character, x, y, width, height, ascent above the baseline."""
    i=dat.index(b'.pvr\x00')+5;line=dat[i];count=struct.unpack_from('>H',dat,i+3)[0];p=i+5;g={}
    for _ in range(count):
        c,x,y,w,h,asc=struct.unpack_from('>6H',dat,p);g[chr(c)]=(x,y,w,h,asc);p+=12
    return line,g
def palette_index(r,g,b,a,outline,pal):
    if a<16:return 0
    level=min(2,max(0,round(a/128*3)-1))            # alpha 1/3, 2/3, 1
    fill=(255,255,255);d=[f-o for f,o in zip(fill,outline)];n=sum(v*v for v in d) or 1
    t=sum((c-o)*v for c,o,v in zip((r,g,b),outline,d))/n   # 1 = fill colour, 0 = outline
    step=min(4,max(0,round((1-t)*4)))
    return 1+level*5+step
def make_palette(outline):
    pal=[0]
    for level in range(3):
        a=round(128*(level+1)/3)
        for step in range(5):
            t=1-step/4;c=[round(255*t+o*(1-t)) for o in outline];pal.append(c[0]|(c[1]<<8)|(c[2]<<16)|(a<<24))
    return pal
def extract(apk,out):
    fonts=[];glyphs=[]
    with zipfile.ZipFile(apk) as z:
        for fid,(name,factor,keep) in enumerate(FONTS):
            line,table=glyph_table(z.read(f'assets/data/fonts/1024x768/{name}.dat'))
            sheet=z.read(f'assets/data/fonts/1024x768/{name}.pvr')
            chars=[c for c in (keep or sorted(table)) if c in table]
            dark=[0,0,0];count=0;images=[]
            for c in chars:
                x,y,w,h,asc=table[c]
                rgba=decode_pvr_frame(sheet,{'frame':{'x':x,'y':y,'w':w,'h':h}})
                for i in range(0,len(rgba),4):
                    if rgba[i+3]>100 and sum(rgba[i:i+3])<450:dark=[d+v for d,v in zip(dark,rgba[i:i+3])];count+=1
                small,sw,sh=scale(rgba,w,h,factor);images.append((c,sw,sh,round(asc*factor),small))
            outline=[round(v/max(1,count)) for v in dark]
            fonts.append((round(line*factor),outline));glyphs+=[(fid,)+img for img in images]
    # Shelf packing on a 256-wide page, tallest first, 1-pixel gutter.
    place={};x=y=shelf=0
    for g in sorted(glyphs,key=lambda g:-g[3]):
        fid,c,w,h,asc,_=g
        if x+w>PAGE_W:x=0;y+=shelf+1;shelf=0
        place[(fid,c)]=(x,y);x+=w+1;shelf=max(shelf,h)
    height=(y+shelf+15)//16*16
    # A PSMT4 GS page is 128 x 128: 256 x 128 takes 16 KiB, anything taller 32 KiB (no room for it).
    if height>128:raise ValueError('Font page taller than 128 rows')
    idx=bytearray(PAGE_W*height);pals=[make_palette(o) for _,o in fonts]
    for fid,c,w,h,asc,rgba in glyphs:
        px,py=place[(fid,c)]
        for r in range(h):
            for k in range(w):
                q=(r*w+k)*4;idx[(py+r)*PAGE_W+px+k]=palette_index(*rgba[q:q+4],fonts[fid][1],pals[fid])
    page=bytes(idx[i]|(idx[i+1]<<4) for i in range(0,len(idx),2))   # PSMT4: first pixel in the low nibble
    head=struct.pack('<4s5I',b'PBFT',1,len(glyphs),PAGE_W,height,len(fonts))
    head+=b''.join(struct.pack('<I',line) for line,_ in fonts)+b''.join(struct.pack('<16I',*p) for p in pals)
    head+=b''.join(struct.pack('<IB3x4Hh2x',ord(c),fid,*place[(fid,c)],w,h,asc) for fid,c,w,h,asc,_ in glyphs)
    head+=bytes((-len(head))%64)
    Path(out).write_bytes(head+page)
    return {'glyphs':len(glyphs),'page':(PAGE_W,height),'bytes':len(head)+len(page),'lines':[l for l,_ in fonts],'outlines':[o for _,o in fonts]}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
    print(extract(a.apk,a.output))
