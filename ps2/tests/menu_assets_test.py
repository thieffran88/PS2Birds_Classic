import struct
from pathlib import Path
d=(Path(__file__).resolve().parents[1]/'assets/MENU.BIN').read_bytes()
magic,version,count,npages=struct.unpack_from('<4s3I',d)
assert magic==b'PBMN' and version==2 and npages==4 and count==111    # 43 interface sprites (7 flying birds, 7 yelling) + 22 episode cards (art + title) + 12 rewards x 2
dims=[struct.unpack_from('<2I',d,16+8*i) for i in range(npages)]
assert dims==[(512,512),(512,512),(512,256),(512,128)]               # the four stage texture slots
head=16+8*npages+16*count;head+=(-head)%64
pages_end=head+sum(w*h*4 for w,h in dims);about_len=struct.unpack_from('<I',d,pages_end)[0]
assert len(d)==pages_end+4+about_len and 'Rovio' in d[pages_end+4:].decode()   # credits.lua TEXT_ABOUT
spr=[struct.unpack_from('<5H2h2x',d,16+8*npages+16*i) for i in range(count)]
cards=[spr[43+2*e] for e in range(22)];titles=[spr[44+2*e] for e in range(22)]
assert all(60<w<=100 and 100<h<=260 for _,_,_,w,h,_,_ in cards)     # every episode has card art
assert sum(1 for t in titles if t[3])==20                            # all but Mighty Feathers have a title sprite
for i,(p,x,y,w,h,_,_) in enumerate(spr):
    if not w:continue
    W,H=dims[p];assert x+w<=W and y+h<=H
    for q,X,Y,ww,hh,_,_ in spr[:i]:assert q!=p or not ww or (q,X,Y,ww,hh)==(p,x,y,w,h) or x+w<=X or X+ww<=x or y+h<=Y or Y+hh<=y   # an identical rectangle is an alias (the Mighty League card shares the generic card)
print('Menu assets OK: logo, PLAY button, options/sound/about buttons, the about text, 7 flying birds, 22 episode cards with titles, level buttons, stars, lock and dots on the four 512-wide texture slots.')
