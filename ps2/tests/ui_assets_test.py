import struct
from pathlib import Path
d=(Path(__file__).resolve().parents[1]/'assets/UI.BIN').read_bytes()
magic,version,count,w,h=struct.unpack_from('<4s4I',d)
assert magic==b'PBUI' and version==2 and count==53 and (w,h)==(128,1024)
head=(20+8*count+63)//64*64;assert len(d)==head+1024+w*h   # PSMT8 page: the same 128 KiB as before + the palette
clut=struct.unpack_from('<256I',d,head);idx=d[head+1024:]
assert clut[0]==0 and all((c>>24)<=128 for c in clut) and len(set(clut))>200          # transparent index 0, GS alpha
rects=[struct.unpack_from('<4H',d,20+8*i) for i in range(count)]
for i,(x,y,rw,rh) in enumerate(rects):
    assert 2<=rw<=120 and 2<=rh<=120 and x+rw<=w and y+rh<=h
    for j in range(i):
        X,Y,W,H=rects[j];assert x+rw<=X or X+W<=x or y+rh<=Y or Y+H<=y   # no overlap
    opaque=sum(1 for r in range(rh) for c in range(rw) if idx[(y+r)*w+x+c])
    assert opaque>rw*rh//6
print('UI assets OK: 53 sprites (11 level-end and pause, 42 particles and score sprites) on one 128 x 1024 PSMT8 page with a 256-colour palette.')
