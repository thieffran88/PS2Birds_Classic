import sys
import hashlib,importlib.util,json,struct
from pathlib import Path
root=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(root/'tools'))
spec=importlib.util.spec_from_file_location('extract_sprites',root/'tools/extract_sprites.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
# Independent known pixel: verify endian, channel order, extrusion removal, GS alpha.
for value,expected in [(0xf00f,bytes([255,0,0,128])),(0x0f08,bytes([0,255,0,68])),(0x00f0,bytes([0,0,255,0]))]:
    data=bytearray(18);struct.pack_into('<H',data,8,value)
    assert module.decode_rgba4444(data,3,3)==expected
try:module.decode_rgba4444(b'',3,3)
except ValueError:pass
else:raise AssertionError('Truncated input accepted')
a=root/'ps2/assets';meta=json.loads((a/'manifest.json').read_text());pixels=(a/'sprites.rgba').read_bytes()
assert len(pixels)==sum(p['width']*p['height']*4 for p in meta['pages'])==1572864
assert hashlib.sha256(pixels).hexdigest()==meta['atlas_sha256']
assert max(pixels[3::4])==128 and min(pixels[3::4])==0
assert len(meta['sprites'])==85
occupied=set()
for frame in meta['sprites']:
    page=meta['pages'][frame['page']]
    x,y,w,h=[frame[k] for k in ['atlas_x','atlas_y','width','height']]
    assert x>0 and y>0 and x+w<page['width'] and y+h<page['height']
    content=bytearray()
    for row in range(y,y+h):
        indexes=set(range(page['offset']//4+row*page['width']+x,page['offset']//4+row*page['width']+x+w))
        assert not occupied.intersection(indexes);occupied.update(indexes)
        offset=page['offset']+(row*page['width']+x)*4;content+=pixels[offset:offset+w*4]
    assert hashlib.sha256(content).hexdigest()==frame['rgba_sha256']
    if frame['sprite']=='BIRD_RED':assert sum(content[i]>150 and content[i]>content[i+1]*2 and content[i+3]>0 for i in range(0,len(content),4))>100
print('Sprites OK: endian/channels, alpha, crop, 85 frames, 2 pages, atlas bounds, no overlap, per-frame hashes.')
