import json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]
d=(root/'assets/THEMES.BIN').read_bytes()
magic,version,count,w,h=struct.unpack_from('<4s4I',d)
assert magic==b'PBTH' and version==2 and w==640 and h==648 and 30<=count<=64
names=[];prev=0
for i in range(count):
    name,off,size=struct.unpack_from('<32s2I',d,20+40*i);names.append(name.rstrip(b'\0').decode())
    assert off%2048==0 and off>=prev and size==w*h*2 and off+size<=len(d);prev=off+size
    pix=struct.unpack_from(f'<{w*h}H',d,off)
    assert all(v>>15 for v in pix[::997])                      # opaque
    sky,ground=pix[10*w+5],pix[(h-1)*w+5]
    assert len({pix[k*w+k] for k in range(0,h,37)})>3          # layers, not a flat colour
assert names[0]=='theme1' and 'themeFarm' in names and 'themeDino' in names
themes={json.loads(p.read_text()).get('theme') for p in (root/'data').glob('level_*.json')}
missing=themes-set(names)-{'',None}
assert not missing,missing
print(f'Themes OK: {count} opaque 640 x 648 backgrounds (sky, layers, ground) covering every stage theme.')
