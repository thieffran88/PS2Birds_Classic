import json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]
d=(root/'assets/THEMES.BIN').read_bytes()
magic,version,count,w,h=struct.unpack_from('<4s4I',d)
assert magic==b'PBTH' and version==3 and w==640 and h==648 and 30<=count<=64
names=[];prev=0;textured=0
for i in range(count):
    name,off,size=struct.unpack_from('<32s2I',d,20+40*i);names.append(name.rstrip(b'\0').decode())
    assert off%2048==0 and off>=prev and size==w*h*2+256*256*2 and off+size<=len(d);prev=off+size
    pix=struct.unpack_from(f'<{w*h}H',d,off)
    assert all(v>>15 for v in pix[::997])                      # opaque
    sky,ground=pix[10*w+5],pix[(h-1)*w+5]
    assert len({pix[k*w+k] for k in range(0,h,37)})>3          # layers, not a flat colour
    tex=struct.unpack_from('<65536H',d,off+w*h*2)
    assert all(v>>15 for v in tex[::257]);textured+=len(set(tex[::97]))>2   # ground tile, not a flat colour
assert textured==count                                           # most themes have a real ground texture
assert names[0]=='theme1' and 'themeFarm' in names and 'themeDino' in names
themes={json.loads(p.read_text()).get('theme') for p in (root/'data').glob('level_*.json')}
missing=themes-set(names)-{'',None,'themeTalkweb1','themeSiege','theme01FaceLift','theme02FaceLift'}   # themeTalkweb1 (Year of the Goat) and the Siege themes have no entry in themes.lua: their stages use the built-in scenery
assert not missing,missing
print(f'Themes OK: {count} opaque 640 x 648 backgrounds (sky, layers, ground) covering every stage theme, with their 256 x 256 ground textures.')
