import struct
from pathlib import Path
d=(Path(__file__).resolve().parents[1]/'assets/FONT.BIN').read_bytes()
magic,version,count,w,h,fonts=struct.unpack_from('<4s5I',d)
assert magic==b"PBFT" and version==1 and fonts==2 and w==256 and 0<h<=128   # one row of PSMT4 GS pages (16 KiB)
p=24+4*fonts;pals=[struct.unpack_from('<16I',d,p+64*i) for i in range(fonts)];p+=64*fonts
glyphs=[struct.unpack_from('<IB3x4Hh2x',d,p+20*i) for i in range(count)];p+=20*count;p+=(-p)%64
assert len(d)==p+w*h//2 and len(d)<=24576           # PSMT4 page: fits the VRAM left after UI.BIN
assert all(pal[0]==0 and all((c>>24)<=128 for c in pal) for pal in pals)   # index 0 transparent, GS alpha
basic={chr(c) for c,f,*_ in glyphs if f==0};score={chr(c) for c,f,*_ in glyphs if f==1}
assert set('Pontuação: Recorde0123456789NÍVELCONCLUÍDO!ÃÇ') <= basic|{' '} and set('0123456789') <= score
boxes=[(x,y,gw,gh) for _,_,x,y,gw,gh,_ in glyphs]
for i,(x,y,gw,gh) in enumerate(boxes):
    assert x+gw<=w and y+gh<=h
    for X,Y,W,H in boxes[:i]:assert x+gw<=X or X+W<=x or y+gh<=Y or Y+H<=y
print(f'Font assets OK: FONT_BASIC ({len(basic)} glyphs, accents for the pt_BR texts) and FONT_SCORE ({len(score)}) on a {w} x {h} PSMT4 page, 16-colour palettes.')
