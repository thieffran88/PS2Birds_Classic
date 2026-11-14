#!/usr/bin/env python3
"""Convert selected original APK sprites into an embedded PS2 RGBA32 atlas.
Pure Python 3.9+ (built-in AES, 7z and PNG readers). Never executes game scripts.
"""
import argparse,hashlib,json,lzma,re,struct,subprocess,tempfile,zipfile,io
from pathlib import Path
from game_profiles import ROOT, PROFILES, decode_payload, archive_command
SHEETS={
 'INGAME_BIRDS_1':['BIRD_GREY_BLUE','BIRD_GREY_BLUE_FLYING','BIRD_PINK','BIRD_PINK_RELEASE','BIRD_PUFFER_1','BIRD_PUFFER_2','BIRD_BIG_BROTHER','BIRD_BOOMERANG','BIRD_BOOMERANG_SPECIAL','BIRD_BOOMERANG_COLLISION','BIRD_GREEN','BIRD_GREEN_FLYING','BIRD_GREEN_COLLISION','BIRD_GREY','BIRD_GREY_FLYING','BIRD_GREY_1','BIRD_GREY_2','BIRD_GREY_3','BIRD_BLUE','BIRD_BLUE_COLLISION','BIRD_RED','BIRD_YELLOW','BIRD_YELLOW_COLLISION','SLING_SHOT_01_BACK','SLING_SHOT_01_FRONT'],
 'INGAME_PIGS_NEW_SHEET_1_1':['PIG_BASIC_SMALL_01','PIG_BASIC_SMALL_01_BLINK','PIG_BASIC_SMALL_02','PIG_BASIC_MEDIUM_01','PIG_BASIC_MEDIUM_01_BLINK','PIG_BASIC_MEDIUM_02','PIG_KING_01','PIG_KING_02'],
 'INGAME_PIGS_NEW_SHEET_0_1':['PIG_BASIC_BIG_01','PIG_BASIC_BIG_01_BLINK','PIG_BASIC_BIG_02','PIG_HELMET_01','PIG_HELMET_01_BLINK','PIG_HELMET_02','PIG_MUSTACHE_01','PIG_MUSTACHE_01_BLINK','PIG_MUSTACHE_02'],
 'INGAME_BLOCKS_1_1':['ESTRADE_01','ESTRADE_02','BLOCK_TREASURE_CHEST','BLOCK_SMILEY_1','BLOCK_SMILEY_3','BLOCK_BEACHBALL','BLOCK_RUBBERDUCK','BLOCK_STRAWBERRY','BLOCK_APPLE','BLOCK_BALLOON_1','BLOCK_BALLOON_2','BLOCK_BALLOON_3','BLOCK_PIG_BALLOON_1','BLOCK_PIG_BALLOON_2','BLOCK_TRAMPOLINE_01','BLOCK_SUPER_BALL','BLOCK_ROPE_THICK_1','COWBOY_HAT_SMALL_1','COWBOY_HAT_BIG_1','SHERIFF_HAT_1','SHERIFF_HAT_2','BANDITO_HAT_1','SHORT_THICK_ROPE','B_EGG_SUPER_BOWL','BLOCK_CARPET','BLOCK_STEEL_BALL','BLOCK_TIRE_01','BLOCK_TIRE_03','BLOCK_WATERMELON','BLOCK_EXTRA_TNT','BLOCK_EXTRA_FLAG_01','BLOCK_DIAMOND','BLOCK_HOLYGRAIL','BLOCK_PILLAR','BLOCK_BANANA','ESTRADE_03','ESTRADE_STAIRS','BLOCK_TIRE_02','BLOCK_EXTRA_DICE_6','BLOCK_STOLEN_EGG','BLOCK_DONUT','BLOCK_HAM','BLOCK_GOLDEN_EGG','BLOCK_EXTRA_FLAG_02'],
 'INGAME_BLOCKS_3_1':[f'BLOCK_{m}_{state}_{size}' for m,sizes in [('WOOD',[1,2,3,4,5,6,7,8]),('LIGHT',[1,2,3,4,5,6,7,8]),('ROCK',[1,2,3,4,5,6,7,8])] for state in range(1,5) for size in sizes]+[f'BLOCK_{m}_{shape}_{state}' for m in ['WOOD','LIGHT','ROCK'] for shape in ['4X4','10X1','TRIANGLE_L','TRIANGLE_M'] for state in range(1,5)],
 'INGAME_BLOCKS_2_1':['HELMET_SMALL','HELMET_BIG','HELMET_BOSS'],
 'PARTICLES_1':['DROPPABLE_EGG'],
 'INGAME_MASKS_1':['GROUND_BLOCK_01','GROUND_BLOCK_02','GROUND_BLOCK_03','GROUND_BLOCK_04','GROUND_BLOCK_05','GROUND_BLOCK_06']}
# VRAM: 4 MiB minus two 640x448 CT16 frame buffers (2 x 573440 bytes) leaves
# 3047424 bytes for textures. The layout below uses at most 2883584 bytes,
# keeping 160 KiB free; pages 0-2 alone match the 2.5 MiB validated up to 0.15.0.
VRAM_BUDGET=2883584
PAGE_LAYOUT=[(512,512),(512,512),(512,256),(512,128)]

def decode_rgba4444(data,w,h):
    if w<3 or h<3 or len(data)!=w*h*2:raise ValueError('Invalid pixel buffer')
    rgba=bytearray()
    for y in range(1,h-1):
        for x in range(1,w-1):
            v=struct.unpack_from('<H',data,(y*w+x)*2)[0]
            rgba+=bytes(((v>>12&15)*17,(v>>8&15)*17,(v>>4&15)*17,((v&15)*128+7)//15))
    return rgba

MASK_SCALE=2
def halve_mask(rgba,w,h):
    """Static ground masks are flat tinted shapes: keep only a 2x2 box-filtered alpha at half resolution."""
    if len(rgba)!=w*h*4:raise ValueError('Invalid mask buffer')
    ow,oh=(w+1)//2,(h+1)//2;out=bytearray()
    for y in range(oh):
        for x in range(ow):
            total=0
            for dy in (0,1):
                for dx in (0,1):
                    sx,sy=x*2+dx,y*2+dy
                    if sx<w and sy<h:total+=rgba[(sy*w+sx)*4+3]
            out+=bytes([255,255,255,(total+2)//4])
    return out,ow,oh

# Large single-use sprites kept at half resolution (drawn at SPRITE_MASK_SCALE): the king pig,
# the puffed GlobeBird and the big paper/office props of the UGC theme (Birdday 5).
HALF_RESOLUTION=('PIG_KING_','BIRD_PUFFER_2','THEME_UGC_','UGC_')
def half_resolution_sprites():
    """Sprites stored at half resolution: the prefixes above plus pieces marked half_resolution
    in profiles/classic_level.json (the many props of Birdday 5, to fit the texture banks)."""
    prof=json.loads((ROOT/'profiles/classic_level.json').read_text())
    return {v['sprite'] for v in prof['objects'].values() if v.get('half_resolution')}
def is_half(name,_cache=[]):
    if not _cache:_cache.append(half_resolution_sprites())
    return name.startswith(HALF_RESOLUTION) or name in _cache[0]
def halve_rgba(rgba,w,h):
    """Large single-stage sprites: 2x2 box filter with alpha-weighted colour, half resolution."""
    if len(rgba)!=w*h*4:raise ValueError('Invalid sprite buffer')
    ow,oh=(w+1)//2,(h+1)//2;out=bytearray()
    for y in range(oh):
        for x in range(ow):
            acc=[0,0,0];alpha=0;count=0
            for dy in (0,1):
                for dx in (0,1):
                    sx,sy=x*2+dx,y*2+dy
                    if sx<w and sy<h:
                        o=(sy*w+sx)*4;a=rgba[o+3];count+=1;alpha+=a
                        for c in range(3):acc[c]+=rgba[o+c]*a
            out+=bytes([(acc[c]+alpha//2)//alpha if alpha else 0 for c in range(3)])+bytes([(alpha+count//2)//count])
    return out,ow,oh

def decode_png_frame(data,frame,_cache={}):
    """Crop one frame from a PNG sheet (decoded once per sheet by tools/png_decode.py)."""
    from png_decode import decode
    # Theme sheets pad each frame with a 1-pixel extrusion outside the frame rectangle; the
    # rectangle itself is the sprite, so the extrusion is simply not copied.
    if frame['rotated']:raise ValueError('Unsupported PNG orientation')
    key=hash(data)
    if key not in _cache:_cache.clear();_cache[key]=decode(data)
    sw,sh,pixels=_cache[key]
    f=frame['frame'];x,y,w,h=(f[k] for k in ['x','y','w','h'])
    if w<=0 or h<=0 or x<0 or y<0 or x+w>sw or y+h>sh:raise ValueError('PNG frame outside sheet')
    rgba=bytearray().join(pixels[((y+r)*sw+x)*4:((y+r)*sw+x+w)*4] for r in range(h))
    for pixel in range(3,len(rgba),4):rgba[pixel]=(rgba[pixel]*128+127)//255
    return rgba

ORDERS=[lambda s:(-s['height'],s['name']),lambda s:(-s['width'],s['name']),lambda s:(-s['width']*s['height'],s['name']),lambda s:(-max(s['width'],s['height']),s['name'])]
def pack(sprites,base_offset=0):
    """Deterministic skyline packing: tallest first (the layout of every earlier bank); if a bank
    does not fit, the same packer is retried widest first, largest first and longest side first."""
    error=None
    for order in ORDERS:
        try:return _pack(sprites,base_offset,order)
        except ValueError as e:error=error or e
    raise error
def _pack(sprites,base_offset,order):
    """Skyline packing with a one-pixel gutter into PAGE_LAYOUT; trailing empty pages dropped."""
    ordered=sorted(sprites,key=order)
    pages=[{'width':w,'cap':cap,'skyline':[0]*w} for w,cap in PAGE_LAYOUT];placed={}
    for s in ordered:
        span=s['width']+2
        for page,entry in enumerate(pages):
            width=entry['width'];skyline=entry['skyline']
            if span>width:continue
            y,x=min((max(skyline[x:x+span]),x) for x in range(width-span+1))
            if y+s['height']+2<=entry['cap']:
                placed[s['name']]=(page,x+1,y+1);skyline[x:x+span]=[y+s['height']+2]*span;break
        else:raise ValueError('Sprite does not fit the atlas page layout: '+s['name'])
    while pages and max(pages[-1]['skyline'])==0:pages.pop()
    specs=[];offset=base_offset
    for entry in pages:
        height=1
        while height<max(entry['skyline']):height*=2
        specs.append({'width':entry['width'],'height':height,'offset':offset});offset+=entry['width']*height*4
    return specs,placed

PIG_SPRITES={'SmallPiglette':'PIG_BASIC_SMALL','MediumPiglette':'PIG_BASIC_MEDIUM','LargePiglette':'PIG_BASIC_BIG','HelmetPiglette':'PIG_HELMET','GrandpaPiglette':'PIG_MUSTACHE','KingPiglette':'PIG_KING'}
def sprite_family(sprite,available):
    """All damage states of a block sprite name, as drawn by ps2/src/main.c."""
    m=re.fullmatch(r'BLOCK_(WOOD|LIGHT|ROCK)_1_(\d+)',sprite)
    if m:return [f'BLOCK_{m[1]}_{s}_{m[2]}' for s in range(1,5)]
    m=re.fullmatch(r'(BLOCK_(?:WOOD|LIGHT|ROCK)_(?:4X4|10X1|TRIANGLE_L|TRIANGLE_M))_1',sprite)
    if m:return [f'{m[1]}_{s}' for s in range(1,5)]
    if sprite=='BLOCK_SMILEY_1':return ['BLOCK_SMILEY_1','BLOCK_SMILEY_3']
    return [sprite]
def bank_members(all_names,levels_dir=None):
    """Bank 0 keeps every sprite of the Poached Eggs build; later banks hold the common
    sprites plus what their stages draw. Banks are swapped when the episode changes."""
    available=set(all_names);profile=json.loads((ROOT/'profiles/classic_level.json').read_text())
    episode=json.loads((ROOT/'profiles/classic_episode.json').read_text())
    # The green boomerang bird is only stored in the banks of stages that use it.
    common=[n for n in all_names if n.startswith(('BIRD_','SLING_','DROPPABLE_EGG','GROUND_BLOCK_')) and not n.startswith(('BIRD_BOOMERANG','BIRD_BIG_BROTHER','BIRD_PUFFER','BIRD_PINK','BIRD_GREY_BLUE')) and not n.endswith('_CAGE')]
    bank_of_page={pg['number']:pg.get('bank',0) for pg in episode.get('additional_pages',[])};bank_of_page[1]=0
    banks=[set(common) for _ in range(max(bank_of_page.values())+1)]
    for path in sorted(Path(levels_dir or ROOT/'ps2/data').glob('level_*_*.json')):
        d=json.loads(path.read_text());b=bank_of_page.get(d.get('episode_page',1),0)
        for o in d['objects']:
            if o['definition']=='ExtraBlockTNT':banks[b].add('BLOCK_EXTRA_TNT')
            if o['definition']=='ExtraBoomerangBird':banks[b].add('BIRD_BOOMERANG')
            if o['definition'] in PIG_SPRITES:
                base=PIG_SPRITES[o['definition']];banks[b]|={f'{base}_01',f'{base}_01_BLINK',f'{base}_02'}&available
                continue
            spec=profile['objects'][o['definition']]
            if 'sprite' in spec:banks[b]|=set(sprite_family(spec['sprite'],available))&available
        # Birds released from cages (Dino) are drawn in the queue like the level's own birds.
        released={o.get('release_bird') for o in d['objects']}
        if 6 in released:banks[b]|={'BIRD_BOOMERANG','BIRD_BOOMERANG_SPECIAL','BIRD_BOOMERANG_COLLISION'}
        if 7 in released:banks[b].add('BIRD_BIG_BROTHER')
        if 8 in released:banks[b]|={'BIRD_PUFFER_1','BIRD_PUFFER_2'}
        if 9 in released:banks[b]|={'BIRD_PINK','BIRD_PINK_RELEASE'}
        if 10 in released:banks[b]|={'BIRD_GREY_BLUE','BIRD_GREY_BLUE_FLYING'}
        if any(bird['definition']=='BoomerangBird' for bird in d['birds']):banks[b]|={'BIRD_BOOMERANG','BIRD_BOOMERANG_SPECIAL','BIRD_BOOMERANG_COLLISION'}
        if any(bird['definition']=='RedBigBird' for bird in d['birds']):banks[b].add('BIRD_BIG_BROTHER')
        if any(bird['definition']=='GlobeBird' for bird in d['birds']):banks[b]|={'BIRD_PUFFER_1','BIRD_PUFFER_2'}
        if any(bird['definition']=='PinkBird' for bird in d['birds']):banks[b]|={'BIRD_PINK','BIRD_PINK_RELEASE'}
        if any(bird['definition']=='BlackBird_Shockwave' for bird in d['birds']):banks[b]|={'BIRD_GREY_BLUE','BIRD_GREY_BLUE_FLYING'}
    # Bank 0 also keeps everything not claimed by a later bank, matching earlier builds.
    later=set().union(*banks[1:]) if len(banks)>1 else set()
    banks[0]|={n for n in available if n not in later or n in banks[0]}-{n for n in later if n not in banks[0] and (n.startswith(('BIRD_BOOMERANG','BIRD_BIG_BROTHER','BIRD_PUFFER','BIRD_PINK','BIRD_GREY_BLUE')) or not n.startswith(('BIRD_','SLING_','DROPPABLE_EGG','GROUND_BLOCK_')))}
    # Dino cages are pieces: only the banks whose stages place them keep their frames.
    banks[0]-={n for n in later if n.endswith('_CAGE')}
    # Sprites of pieces no stage of the campaign places (e.g. Flock Favorites pieces seen only in
    # its Egg Defender stages) stay out of bank 0.
    used={o['definition'] for path in Path(levels_dir or ROOT/'ps2/data').glob('level_*_*.json') for o in json.loads(path.read_text())['objects']}
    unused={v['sprite'] for k,v in profile['objects'].items() if v.get('auto') and k not in used}-{v['sprite'] for k,v in profile['objects'].items() if k in used and 'sprite' in v}
    banks[0]-={n for n in unused if not any(n in b for b in banks[1:])}
    missing=[n for b in banks for n in b if n not in available]
    if missing:raise ValueError('Unknown sprites in banks: '+', '.join(sorted(missing)))
    # Later banks reuse neighbouring frames for blinks and middle damage states of
    # the large Mighty Hoax pieces, so the frame tables stay complete without VRAM cost.
    aliases=[{} for _ in banks]
    for b in range(1,len(banks)):
        for n in sorted(banks[b]):
            target=None
            if n.endswith('_BLINK'):target=n[:-6]
            # Banks 18+ (Birdday 5) need the room for the big UGC props: every damage state of the
            # numbered and shaped blocks draws the intact frame.
            elif b>=18 and re.fullmatch(r'BIRD_\w+?_(?:COLLISION|FLYING|SPECIAL|[123])',n) and re.sub(r'_(?:COLLISION|FLYING|SPECIAL|[123])$','',n) in banks[b]:target=re.sub(r'_(?:COLLISION|FLYING|SPECIAL|[123])$','',n)
            elif b>=18 and re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_[234]_\d+',n):target=re.sub(r'_[234]_(\d+)$',r'_1_\1',n)
            elif b>=18 and re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_(?:4X4|10X1|TRIANGLE_L|TRIANGLE_M|CIRCLE|[A-Z][A-Z0-9]*)_[234]',n):target=n[:-1]+'1'
            elif re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_(?:4X4|TRIANGLE_L|TRIANGLE_M)_[23]',n):target=n[:-1]+('1' if n.endswith('2') else '4')
            # Banks 2 and 3 (Mighty Hoax page 5, Danger Above) have the largest piece sets: numbered blocks keep
            # two frames, intact (states 1-2) and heavily damaged (states 3-4).
            # Banks 11+ (Mine and Dine) also draw the hurt pig with its normal frame.
            elif b>=11 and n.startswith('PIG_') and n.endswith('_02'):target=n[:-3]+'_01'
            elif b>=11 and re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_10X1_[23]',n):target=n[:-1]+('1' if n.endswith('2') else '4')
            elif b>=2 and re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_[23]_\d+',n):target=re.sub(r'_([23])_(\d+)$',lambda m:('_1_' if m[1]=='2' else '_4_')+m[2],n)
            if target and target in banks[b]:aliases[b][n]=target
        banks[b]-=set(aliases[b])
    return [sorted(b) for b in banks],aliases

def extract(apk,out,sevenzip=None,game="classic",levels_dir=None):
    """levels_dir: the stage JSON made by tools/build_campaign.py (decides which sprites each bank holds)."""
    sprites=[];manifest=[]
    with zipfile.ZipFile(apk) as archive,tempfile.TemporaryDirectory(prefix='ps2birds-sprites-') as folder:
        # Pieces added by tools/add_definitions.py ("auto") bring their sprite and sheet.
        sheets={k:list(v) for k,v in SHEETS.items()}
        profile=json.loads((ROOT/'profiles/classic_level.json').read_text());metrics=json.loads((ROOT/'profiles/sprite_metrics.json').read_text())
        for spec in profile['objects'].values():
            if spec.get('auto') and spec['sprite'] not in sheets.setdefault(metrics[spec['sprite']]['sheet'].replace('.sheet.json',''),[]):
                sheets[metrics[spec['sprite']]['sheet'].replace('.sheet.json','')].append(spec['sprite'])
        for sheet,names in sheets.items():
            base='assets/data/images/'+sheet if '/' in sheet else 'assets/data/images/base/'+sheet
            encrypted=archive.read(base+'.sheet.json')
            decoded=decode_payload(encrypted,game,sevenzip)
            frames={f['filename']:f for f in json.loads(decoded)['frames']}
            is_png=base+'.stream.7z' not in archive.namelist()
            if is_png:png=archive.read(base+'.png')
            else:
                if sevenzip:
                    packed=Path(folder)/'sheet.7z';packed.write_bytes(archive.read(base+'.stream.7z'))
                    raw=subprocess.run(archive_command(sevenzip)+['x','-so',str(packed)],capture_output=True,check=True).stdout
                else:
                    from mini7z import extract as _x7;raw=_x7(archive.read(base+'.stream.7z'))
            for name in names:
                if name not in frames:raise ValueError('Classic sprite adapter missing '+name+'; this game/version needs its own asset adapter')
                f=frames[name];fw,fh=f['frame']['w'],f['frame']['h']
                if is_png:rgba=decode_png_frame(png,f)
                else:
                    s=f['stream'];p=s['position'];w=s['width'];h=s['height']
                    head=raw[p:p+40]
                    if len(head)!=40 or head[4:6]!=b'\x00\x28' or head[32:40] not in (b'RGBA4444',b'RGBA8888'):raise ValueError('Unsupported stream header: '+name)
                    if f['rotated']:raise ValueError('Unsupported frame orientation: '+name)
                    left,top,right,bottom=f['extrude']
                    if head[32:40]==b'RGBA8888':
                        # RGBA8888 sheets (e.g. INGAME_TRANSPARENCY_1_1): no padding border, the
                        # frame sits inside the extrusion margin; alpha scaled to the GS 0..128 range.
                        if struct.unpack('>HH',head[6:10])!=(w,h) or s['length']!=w*h*4:raise ValueError('Bad dimensions: '+name)
                        if (w,h)!=(fw+left+right,fh+top+bottom):
                            # Some sheets (INGAME_DINOS_DECOS_1) declare no extrusion but keep a
                            # padding border, given in the stream header (left, top, right, bottom).
                            left,top,right,bottom=struct.unpack('>HHHH',head[24:32])
                            if (w,h)!=(fw+left+right,fh+top+bottom):raise ValueError('Bad extrusion: '+name)
                        data=raw[p+40:p+40+w*h*4]
                        if len(data)!=w*h*4:raise ValueError('Truncated sprite: '+name)
                        rgba=bytearray().join(data[((y+top)*w+left)*4:((y+top)*w+left+fw)*4] for y in range(fh))
                        for k in range(3,len(rgba),4):rgba[k]=(rgba[k]*128+127)//255
                    else:
                        if struct.unpack('>HH',head[6:10])!=(w,h) or s['length']!=w*h*2:raise ValueError('Bad dimensions: '+name)
                        if (w,h)!=(fw+2+left+right,fh+2+top+bottom):raise ValueError('Bad extrusion: '+name)
                        data=raw[p+40:p+40+w*h*2]
                        if len(data)!=w*h*2:raise ValueError('Truncated sprite: '+name)
                        rgba=decode_rgba4444(data,w,h)
                        rgba=bytearray().join(rgba[((y+top)*(w-2)+left)*4:((y+top)*(w-2)+left+fw)*4] for y in range(fh))
                pivot={k:int(round(v)) for k,v in f['pivot'].items()}  # some sheets store fractional pivots
                if name.startswith('GROUND_BLOCK_'):
                    rgba,fw,fh=halve_mask(rgba,fw,fh);pivot={'x':(pivot['x']+1)//MASK_SCALE,'y':(pivot['y']+1)//MASK_SCALE}
                elif is_half(name):
                    rgba,fw,fh=halve_rgba(rgba,fw,fh);pivot={'x':(pivot['x']+1)//MASK_SCALE,'y':(pivot['y']+1)//MASK_SCALE}
                sprite={'name':name,'width':fw,'height':fh,'pivot_x':pivot['x'],'pivot_y':pivot['y'],'rgba':rgba}
                sprites.append(sprite)
                manifest.append({'sprite':name,'sheet':base,'source_format':'PNG RGBA8' if is_png else 'RGBA4444 little-endian','processing':'alpha mask at half resolution (2x2 box filter), white RGB for runtime tint; drawn at SPRITE_MASK_SCALE' if name.startswith('GROUND_BLOCK_') else 'half resolution (2x2 alpha-weighted box filter); drawn at SPRITE_MASK_SCALE' if is_half(name) else 'crop extrusion and convert to GS RGBA32','rgba_sha256':hashlib.sha256(rgba).hexdigest(),'width':fw,'height':fh,'pivot':pivot})
    banks,aliases=bank_members([s['name'] for s in sprites],levels_dir)
    by_name={s['name']:s for s in sprites};pixels=bytearray();bank_specs=[];placement={}
    for bank,members in enumerate(banks):
        try:specs,placed=pack([by_name[n] for n in members],len(pixels))
        except ValueError as e:
            area=sum((by_name[n]['width']+2)*(by_name[n]['height']+2)*4 for n in members);big=sorted(members,key=lambda n:-by_name[n]['width']*by_name[n]['height'])[:12]
            raise ValueError(f'{e} (bank {bank}: {len(members)} sprites, {area} bytes with gutters, budget {VRAM_BUDGET}; largest {[(n,by_name[n]["width"],by_name[n]["height"]) for n in big]})')
        if sum(page['width']*page['height']*4 for page in specs)>VRAM_BUDGET:raise ValueError(f'Texture bank {bank} exceeds the VRAM budget')
        for page in specs:pixels+=bytes(page['width']*page['height']*4)
        for name,(page,x,y) in placed.items():
            spec=specs[page];sp=by_name[name]
            for row in range(sp['height']):
                start=spec['offset']+((y+row)*spec['width']+x)*4
                pixels[start:start+sp['width']*4]=sp['rgba'][row*sp['width']*4:(row+1)*sp['width']*4]
            placement[(bank,name)]=(page,x,y)
        bank_specs.append(specs)
    # Page slots share VRAM across banks: each slot is sized for its largest page.
    slots=[max((specs[i]['width']*specs[i]['height']*4 for specs in bank_specs if i<len(specs)),default=0) for i in range(max(len(b) for b in bank_specs))]
    if sum(slots)>VRAM_BUDGET:raise ValueError('Texture page slots exceed the VRAM budget')
    names=sorted(by_name,key=lambda n:(-by_name[n]['height'],n))
    out.mkdir(parents=True,exist_ok=True);(out/'sprites.rgba').write_bytes(pixels)
    pages_max=len(slots)
    lines=['/* Generated from the supplied APK by tools/extract_sprites.py. */','#ifndef PS2BIRDS_SPRITE_DATA_H','#define PS2BIRDS_SPRITE_DATA_H',
           f'#define SPRITE_VRAM_BUDGET {VRAM_BUDGET}',f'#define SPRITE_MASK_SCALE {MASK_SCALE}',f'#define SPRITE_BANK_COUNT {len(banks)}',f'#define SPRITE_PAGE_COUNT {pages_max}',
           '/* Bytes of VRAM reserved for each page slot (largest page of any bank). */','static const int sprite_slot_bytes[SPRITE_PAGE_COUNT] = {'+','.join(map(str,slots))+'};',
           'typedef struct { int width,height,offset; } SpritePage;','/* width 0: slot unused by that bank. */','static const SpritePage sprite_pages[SPRITE_BANK_COUNT][SPRITE_PAGE_COUNT] = {']
    for specs in bank_specs:
        lines.append(' {'+','.join('{%d,%d,%d}'%(sp['width'],sp['height'],sp['offset']) for sp in specs+[{'width':0,'height':0,'offset':0}]*(pages_max-len(specs)))+'},')
    lines+=['};','/* page -1: sprite not present in that bank. */','typedef struct { int x,y,w,h,px,py,page; } SpriteFrame;']
    for i,n in enumerate(names):lines.append(f'#define SPR_{n} {i}')
    lines+=['static const SpriteFrame sprite_frames[SPRITE_BANK_COUNT][%d] = {'%len(names)]
    for bank in range(len(banks)):
        lines.append(' {')
        for n in names:
            sp=by_name[aliases[bank].get(n,n)];page,x,y=placement.get((bank,aliases[bank].get(n,n)),(-1,0,0))
            lines.append('  {%d,%d,%d,%d,%d,%d,%d},'%(x,y,sp['width'],sp['height'],sp['pivot_x'],sp['pivot_y'],page))
        lines.append(' },')
    lines+=['};','#define SPRITE_COUNT '+str(len(names)),'#endif','']
    (out/'sprite_data.h').write_text('\n'.join(lines))
    ids=['/* Sprite identifiers and texture-slot sizes for the Classic APK supported by tools/gerar_iso.py.','   Only names and sizes: the frames and pixels are read from SPRITES.BIN at run time.','   Regenerated by tools/extract_sprites.py; tools/pack_sprites.py refuses an APK whose sprite order differs. */','#ifndef PS2BIRDS_SPRITE_IDS_H','#define PS2BIRDS_SPRITE_IDS_H']
    ids+=[l for l in lines if l.startswith(('#define SPR','static const int sprite_slot_bytes','/* Bytes of VRAM'))]+['#endif','']
    (out/'sprite_ids.h').write_text('\n'.join(ids))
    for entry in manifest:
        entry['banks']=[{'bank':b,'page':placement[(b,entry['sprite'])][0],'atlas_x':placement[(b,entry['sprite'])][1],'atlas_y':placement[(b,entry['sprite'])][2]} for b in range(len(banks)) if (b,entry['sprite']) in placement]
    (out/'manifest.json').write_text(json.dumps({'game_profile':game,'sprite_adapter':'classic_campaign_sprites','apk':Path(apk).name,'vram_budget':VRAM_BUDGET,'slot_bytes':slots,'banks':[{'bank':b,'pages':bank_specs[b],'sprites':banks[b],'aliases':aliases[b]} for b in range(len(banks))],'bytes':len(pixels),'alpha_range':[0,128],'atlas_sha256':hashlib.sha256(pixels).hexdigest(),'sprites':manifest},indent=2)+'\n')
    print(f'{len(sprites)} original sprites, {len(banks)} texture banks ({", ".join(str(len(b)) for b in banks)} sprites), {len(pixels)} bytes in RAM, {sum(slots)} bytes of VRAM')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--sevenzip',default=None,help='optional 7-Zip; the built-in reader is used by default');p.add_argument('--game',choices=sorted(PROFILES),default='classic');a=p.parse_args();extract(a.apk,a.output,a.sevenzip,a.game)
