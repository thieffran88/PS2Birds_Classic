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
banks=meta['banks'];assert len(banks)==5
assert len(pixels)==sum(p['width']*p['height']*4 for b in banks for p in b['pages'])
for b in banks:
    sizes=[(p['width'],p['height']) for p in b['pages']];assert len(sizes)==4 and all(w==512 and h<=cap for (w,h),cap in zip(sizes,[512,512,256,128]))
assert [(p['width'],p['height']) for p in banks[0]['pages']]==[(512,512),(512,512),(512,256),(512,128)]
assert sum(meta['slot_bytes'])<=meta['vram_budget']==2883584 and meta['slot_bytes']==[1048576,1048576,524288,262144]
assert hashlib.sha256(pixels).hexdigest()==meta['atlas_sha256']
assert max(pixels[3::4])==128 and min(pixels[3::4])==0
assert len(meta['sprites'])==222 and len(banks[0]['sprites'])==153 and len(banks[1]['sprites'])==151 and len(banks[2]['sprites'])==127 and len(banks[3]['sprites'])==128 and len(banks[4]['sprites'])==119
occupied=set();checked=0
for frame in meta['sprites']:
    for place in frame['banks']:
        page=banks[place['bank']]['pages'][place['page']]
        x,y,w,h=place['atlas_x'],place['atlas_y'],frame['width'],frame['height']
        assert x>0 and y>0 and x+w<page['width'] and y+h<page['height']
        content=bytearray()
        for row in range(y,y+h):
            indexes=set(range(page['offset']//4+row*page['width']+x,page['offset']//4+row*page['width']+x+w))
            assert not occupied.intersection(indexes);occupied.update(indexes)
            offset=page['offset']+(row*page['width']+x)*4;content+=pixels[offset:offset+w*4]
        assert hashlib.sha256(content).hexdigest()==frame['rgba_sha256'];checked+=1
        if frame['sprite']=='BIRD_RED':assert sum(content[i]>150 and content[i]>content[i+1]*2 and content[i+3]>0 for i in range(0,len(content),4))>100
# Aliased frames in the Mighty Hoax bank point at sprites present in that bank.
for name,target in banks[1]['aliases'].items():assert target in banks[1]['sprites'] and name not in banks[1]['sprites'] and (name.endswith('_BLINK') or name[-1] in '23')
header=(a/'sprite_data.h').read_text();assert '#define SPRITE_BANK_COUNT 5' in header and '#define SPRITE_COUNT 222' in header
print(f'Sprites OK: endian/channels, alpha, crop, {len(meta["sprites"])} frames in {len(banks)} texture banks ({checked} placements), shared slots within {meta["vram_budget"]} bytes, bounds, no overlap, per-frame hashes, aliases.')

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
        place=s['banks'][0];page=banks[place['bank']]['pages'][place['page']];body=bytearray()
        for row in range(place['atlas_y'],place['atlas_y']+s['height']):
            o=page['offset']+(row*page['width']+place['atlas_x'])*4;body+=pixels[o:o+s['width']*4]
        # GROUND_BLOCK_06 is a faint 20x20 mask in the APK (the original fills it with a theme texture).
        assert set(body[0::4])=={255} and (max(body[3::4])==128 if s['sprite']!='GROUND_BLOCK_06' else 0<max(body[3::4])<128)
for name in ['BLOCK_EXTRA_TNT','BLOCK_PILLAR','BLOCK_DIAMOND','BLOCK_HOLYGRAIL','BLOCK_BANANA','PIG_MUSTACHE_01','PIG_MUSTACHE_01_BLINK','PIG_MUSTACHE_02','BLOCK_WOOD_1_8','BLOCK_WOOD_4_8']:assert name in sizes
print('Mask/new sprites OK: half-resolution ground masks with box-filtered alpha, TNT, pillar, treasures, mustache pig and round wood present.')

# The king pig appears in a single stage and is stored at half resolution with alpha-weighted colour.
half,w,h=module.halve_rgba(bytes([200,0,0,128, 0,0,0,0, 0,0,0,0, 0,0,0,0]),2,2)
assert (w,h)==(1,1) and half==bytes([200,0,0,32])
for name,size in [('PIG_KING_01',(66,74)),('PIG_KING_02',(66,74))]:assert sizes[name]==size
for name in ['ESTRADE_03','ESTRADE_STAIRS','BLOCK_TIRE_02','BLOCK_EXTRA_DICE_6','BLOCK_STOLEN_EGG','BLOCK_DONUT']:assert name in sizes
assert sum(meta['slot_bytes'])<=3047424-160*1024
print('Third page sprites OK: half-resolution king pig, estrade 03, stairs, tire, dice, stolen egg and donut; 160 KiB VRAM margin kept.')

for mat in ['WOOD','LIGHT','ROCK']:
    for shape in ['4X4','10X1','TRIANGLE_L','TRIANGLE_M']:
        assert f'BLOCK_{mat}_{shape}_1' in banks[1]['sprites'] and f'BLOCK_{mat}_{shape}_1' not in banks[0]['sprites']
for name in ['BLOCK_HAM','BLOCK_GOLDEN_EGG','BLOCK_EXTRA_FLAG_02','BIRD_RED','SLING_SHOT_01_BACK']:assert name in banks[1]['sprites']
import re as _re
for name,target in list(banks[2]['aliases'].items())+list(banks[3]['aliases'].items())+list(banks[4]['aliases'].items()):
    assert target in banks[2]['sprites']+banks[3]['sprites']+banks[4]['sprites']
    assert name.endswith('_BLINK') or name[-1] in '23' or _re.fullmatch(r'BLOCK_(WOOD|LIGHT|ROCK)_[23]_\d+',name)
for name in ['BLOCK_LIGHT_1_8','BLOCK_LIGHT_4_8','BLOCK_APPLE','BLOCK_EXTRA_FLAG_01','BIRD_RED','SLING_SHOT_01_BACK']:assert name in banks[2]['sprites']
assert banks[2]['aliases']['BLOCK_LIGHT_2_8']=='BLOCK_LIGHT_1_8' and banks[2]['aliases']['BLOCK_LIGHT_3_8']=='BLOCK_LIGHT_4_8'
for name in ['BLOCK_LIGHT_1_8','BLOCK_APPLE','BLOCK_EXTRA_FLAG_01']:assert name not in banks[0]['sprites'] and name not in banks[1]['sprites']
print('Mighty Hoax page 5 bank OK: LightBlock8, apple and flag only in bank 2; numbered blocks keep intact and heavily damaged frames.')
print('Mighty Hoax bank OK: 12 new block families and 3 extras only in bank 1; birds and slingshot in both banks.')

# Danger Above bank 3: the green boomerang bird and the new pieces live only where they are used.
for name in ['GROUND_BLOCK_06','BLOCK_PIG_BALLOON_1','BIRD_BOOMERANG','BIRD_BOOMERANG_SPECIAL','BIRD_BOOMERANG_COLLISION','BLOCK_BALLOON_1','BLOCK_BALLOON_2','BLOCK_BALLOON_3','BLOCK_PIG_BALLOON_2','BLOCK_TRAMPOLINE_01','BIRD_RED','SLING_SHOT_01_BACK']:assert name in banks[3]['sprites'],name
for b in range(3):
    for name in ['BIRD_BOOMERANG','BIRD_BOOMERANG_SPECIAL','BLOCK_BALLOON_1','BLOCK_TRAMPOLINE_01']:assert name not in banks[b]['sprites'],(b,name)
assert sum(meta['slot_bytes'])<=meta['vram_budget']
print('Danger Above bank OK: boomerang bird, balloons, trampoline and ground block 06 in bank 3; boomerang and balloons absent from earlier banks.')

# Danger Above page 8 (bank 4): super ball, carpet and steel ball only there; boomerang bird present.
for name in ['BLOCK_SUPER_BALL','BLOCK_CARPET','BLOCK_STEEL_BALL','BIRD_BOOMERANG','BIRD_RED','SLING_SHOT_01_BACK']:assert name in banks[4]['sprites'],name
for b in range(4):
    for name in ['BLOCK_SUPER_BALL','BLOCK_CARPET','BLOCK_STEEL_BALL']:assert name not in banks[b]['sprites'],(b,name)
print('Danger Above page 8 bank OK: super ball, carpet and steel ball only in bank 4.')
