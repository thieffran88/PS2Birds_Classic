import struct
from pathlib import Path
d=(Path(__file__).resolve().parents[1]/'assets/UI.BIN').read_bytes()
magic,version,count,size=struct.unpack_from('<4s3I',d)
assert magic==b'PBUI' and version==1 and count==11 and size==256
head=(16+8*count+63)//64*64;assert len(d)==head+size*size*2   # 128 KiB CT16 page: fits the 160 KiB of spare VRAM
rects=[struct.unpack_from('<4H',d,16+8*i) for i in range(count)]
for i,(x,y,w,h) in enumerate(rects):
    assert 30<w<=120 and 30<h<=120 and x+w<=size and y+h<=size
    for j in range(i):
        X,Y,W,H=rects[j];assert x+w<=X or X+W<=x or y+h<=Y or Y+H<=y   # no overlap
    opaque=sum(1 for r in range(h) for c in range(w) if struct.unpack_from('<H',d,head+((y+r)*size+x+c)*2)[0]>>15)
    assert opaque>w*h//5   # stars fill about a quarter of their box
assert all(v==0 or v>>15 for v in struct.unpack_from(f'<{size*size}H',d,head))  # transparent texels are black (TEXA.AEM)
print('UI assets OK: 11 level-end and pause sprites (big stars, new highscore badge, menu/restart/next/resume/sound buttons, failed pig) on one 256 x 256 CT16 page.')
