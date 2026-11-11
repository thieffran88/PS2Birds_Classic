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
assert len(pixels)==sum(p['width']*p['height']*4 for p in meta['pages'])<=meta['vram_budget']==2883584
assert [(p['width'],p['height']) for p in meta['pages']]==[(512,512),(512,512),(512,256),(512,128)]
assert hashlib.sha256(pixels).hexdigest()==meta['atlas_sha256']
assert max(pixels[3::4])==128 and min(pixels[3::4])==0
assert len(meta['sprites'])==152
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
print(f'Sprites OK: endian/channels, alpha, crop, {len(meta["sprites"])} frames, {len(meta["pages"])} pages within {meta["vram_budget"]} bytes, atlas bounds, no overlap, per-frame hashes.')

# PNG sheets use 8-bit alpha, unlike the existing RGBA4444 streams.
import io
from PIL import Image
source=Image.new('RGBA',(2,1));source.putdata([(255,0,0,255),(0,255,0,128)])
png=io.BytesIO();source.save(png,format='PNG')
f={'frame':{'x':0,'y':0,'w':2,'h':1},'rotated':False,'extrude':[0,0,0,0]}
assert module.decode_png_frame(png.getvalue(),f)==bytes([255,0,0,128,0,255,0,64])
f['frame']['w']=3
try:module.decode_png_frame(png.getvalue(),f)
except ValueError:pass
else:raise AssertionError('Out-of-bounds PNG frame accepted')
assert next(s for s in meta['sprites'] if s['sprite']=='DROPPABLE_EGG')['source_format']=='PNG RGBA8'
print('PNG sprites OK: original egg frame, crop bounds and 8-bit-to-GS alpha conversion.')

# Ground masks are stored at half resolution and drawn at SPRITE_MASK_SCALE.
assert module.MASK_SCALE==2 and '#define SPRITE_MASK_SCALE 2' in (a/'sprite_data.h').read_text()
half,w,h=module.halve_mask(bytes([9,9,9,128, 9,9,9,0, 9,9,9,64,  9,9,9,128, 9,9,9,128, 9,9,9,64]),3,2)
assert (w,h)==(2,1) and half==bytes([255,255,255,96, 255,255,255,32])
try:module.halve_mask(b'',3,2)
except ValueError:pass
else:raise AssertionError('Truncated mask accepted')
sizes={s['sprite']:(s['width'],s['height']) for s in meta['sprites']}
assert sizes['GROUND_BLOCK_05']==(97,98) and sizes['GROUND_BLOCK_03']==(321,17) and sizes['GROUND_BLOCK_04']==(49,49)
for s in meta['sprites']:
    if s['sprite'].startswith('GROUND_BLOCK_'):
        page=meta['pages'][s['page']];body=bytearray()
        for row in range(s['atlas_y'],s['atlas_y']+s['height']):
            o=page['offset']+(row*page['width']+s['atlas_x'])*4;body+=pixels[o:o+s['width']*4]
        assert set(body[0::4])=={255} and max(body[3::4])==128
for name in ['BLOCK_EXTRA_TNT','BLOCK_PILLAR','BLOCK_DIAMOND','BLOCK_HOLYGRAIL','BLOCK_BANANA','PIG_MUSTACHE_01','PIG_MUSTACHE_01_BLINK','PIG_MUSTACHE_02','BLOCK_WOOD_1_8','BLOCK_WOOD_4_8']:assert name in sizes
print('Mask/new sprites OK: half-resolution ground masks with box-filtered alpha, TNT, pillar, treasures, mustache pig and round wood present.')

# The king pig appears in a single stage and is stored at half resolution with alpha-weighted colour.
half,w,h=module.halve_rgba(bytes([200,0,0,128, 0,0,0,0, 0,0,0,0, 0,0,0,0]),2,2)
assert (w,h)==(1,1) and half==bytes([200,0,0,32])
for name,size in [('PIG_KING_01',(66,74)),('PIG_KING_02',(66,74))]:assert sizes[name]==size
for name in ['ESTRADE_03','ESTRADE_STAIRS','BLOCK_TIRE_02','BLOCK_EXTRA_DICE_6','BLOCK_STOLEN_EGG','BLOCK_DONUT']:assert name in sizes
assert meta['bytes']<=3047424-160*1024
print('Third page sprites OK: half-resolution king pig, estrade 03, stairs, tire, dice, stolen egg and donut; 160 KiB VRAM margin kept.')
