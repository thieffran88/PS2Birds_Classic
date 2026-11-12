#!/usr/bin/env python3
"""Convert selected original APK sprites into an embedded PS2 RGBA32 atlas.
Requires Python 3.9+, OpenSSL and 7-Zip. Never executes game scripts.
"""
import argparse,hashlib,json,lzma,re,struct,subprocess,tempfile,zipfile,io
from pathlib import Path
from game_profiles import ROOT, PROFILES, decode_payload, archive_command
SHEETS={
 'INGAME_BIRDS_1':['BIRD_GREEN','BIRD_GREEN_FLYING','BIRD_GREEN_COLLISION','BIRD_GREY','BIRD_GREY_FLYING','BIRD_GREY_1','BIRD_GREY_2','BIRD_GREY_3','BIRD_BLUE','BIRD_BLUE_COLLISION','BIRD_RED','BIRD_YELLOW','BIRD_YELLOW_COLLISION','SLING_SHOT_01_BACK','SLING_SHOT_01_FRONT'],
 'INGAME_PIGS_NEW_SHEET_1_1':['PIG_BASIC_SMALL_01','PIG_BASIC_SMALL_01_BLINK','PIG_BASIC_SMALL_02','PIG_BASIC_MEDIUM_01','PIG_BASIC_MEDIUM_01_BLINK','PIG_BASIC_MEDIUM_02','PIG_KING_01','PIG_KING_02'],
 'INGAME_PIGS_NEW_SHEET_0_1':['PIG_BASIC_BIG_01','PIG_BASIC_BIG_01_BLINK','PIG_BASIC_BIG_02','PIG_HELMET_01','PIG_HELMET_01_BLINK','PIG_HELMET_02','PIG_MUSTACHE_01','PIG_MUSTACHE_01_BLINK','PIG_MUSTACHE_02'],
 'INGAME_BLOCKS_1_1':['ESTRADE_01','ESTRADE_02','BLOCK_TREASURE_CHEST','BLOCK_SMILEY_1','BLOCK_SMILEY_3','BLOCK_BEACHBALL','BLOCK_RUBBERDUCK','BLOCK_STRAWBERRY','BLOCK_TIRE_01','BLOCK_TIRE_03','BLOCK_WATERMELON','BLOCK_EXTRA_TNT','BLOCK_DIAMOND','BLOCK_HOLYGRAIL','BLOCK_PILLAR','BLOCK_BANANA','ESTRADE_03','ESTRADE_STAIRS','BLOCK_TIRE_02','BLOCK_EXTRA_DICE_6','BLOCK_STOLEN_EGG','BLOCK_DONUT','BLOCK_HAM','BLOCK_GOLDEN_EGG','BLOCK_EXTRA_FLAG_02'],
 'INGAME_BLOCKS_3_1':[f'BLOCK_{m}_{state}_{size}' for m,sizes in [('WOOD',[1,2,3,4,5,6,7,8]),('LIGHT',[1,2,3,4,5,6,7]),('ROCK',[1,2,3,4,5,6,7,8])] for state in range(1,5) for size in sizes]+[f'BLOCK_{m}_{shape}_{state}' for m in ['WOOD','LIGHT','ROCK'] for shape in ['4X4','10X1','TRIANGLE_L','TRIANGLE_M'] for state in range(1,5)],
 'PARTICLES_1':['DROPPABLE_EGG'],
 'INGAME_MASKS_1':['GROUND_BLOCK_01','GROUND_BLOCK_02','GROUND_BLOCK_03','GROUND_BLOCK_04','GROUND_BLOCK_05']}
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

HALF_RESOLUTION=('PIG_KING_',)
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

def decode_png_frame(data,frame):
    from PIL import Image
    if frame['rotated'] or any(frame['extrude']):raise ValueError('Unsupported PNG orientation/extrusion')
    f=frame['frame'];x,y,w,h=(f[k] for k in ['x','y','w','h'])
    with Image.open(io.BytesIO(data)) as source:
        if source.format!='PNG' or w<=0 or h<=0 or x<0 or y<0 or x+w>source.width or y+h>source.height:raise ValueError('PNG frame outside sheet')
        rgba=bytearray(source.convert('RGBA').crop((x,y,x+w,y+h)).tobytes())
    for pixel in range(3,len(rgba),4):rgba[pixel]=(rgba[pixel]*128+127)//255
    return rgba

def pack(sprites,base_offset=0):
    """Deterministic skyline packing with a one-pixel gutter into PAGE_LAYOUT; trailing empty pages dropped."""
    ordered=sorted(sprites,key=lambda s:(-s['height'],s['name']))
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
def bank_members(all_names):
    """Bank 0 keeps every sprite of the Poached Eggs build; later banks hold the common
    sprites plus what their stages draw. Banks are swapped when the episode changes."""
    available=set(all_names);profile=json.loads((ROOT/'profiles/classic_level.json').read_text())
    episode=json.loads((ROOT/'profiles/classic_episode.json').read_text())
    common=[n for n in all_names if n.startswith(('BIRD_','SLING_','DROPPABLE_EGG','GROUND_BLOCK_'))]
    bank_of_page={pg['number']:pg.get('bank',0) for pg in episode.get('additional_pages',[])};bank_of_page[1]=0
    banks=[set(common) for _ in range(max(bank_of_page.values())+1)]
    for path in sorted((ROOT/'ps2/data').glob('level_*_*.json')):
        d=json.loads(path.read_text());b=bank_of_page.get(d.get('episode_page',1),0)
        for o in d['objects']:
            if o['definition'] in PIG_SPRITES:
                base=PIG_SPRITES[o['definition']];banks[b]|={f'{base}_01',f'{base}_01_BLINK',f'{base}_02'}&available
                continue
            spec=profile['objects'][o['definition']]
            if 'sprite' in spec:banks[b]|=set(sprite_family(spec['sprite'],available))&available
        if o['definition']=='ExtraBlockTNT':banks[b].add('BLOCK_EXTRA_TNT')
    # Bank 0 also keeps everything not claimed by a later bank, matching earlier builds.
    later=set().union(*banks[1:]) if len(banks)>1 else set()
    banks[0]|={n for n in available if n not in later or n in banks[0]}-{n for n in later if n not in banks[0] and not n.startswith(('BIRD_','SLING_','DROPPABLE_EGG','GROUND_BLOCK_'))}
    missing=[n for b in banks for n in b if n not in available]
    if missing:raise ValueError('Unknown sprites in banks: '+', '.join(sorted(missing)))
    # Later banks reuse neighbouring frames for blinks and middle damage states of
    # the large Mighty Hoax pieces, so the frame tables stay complete without VRAM cost.
    aliases=[{} for _ in banks]
    for b in range(1,len(banks)):
        for n in sorted(banks[b]):
            target=None
            if n.endswith('_BLINK'):target=n[:-6]
            elif re.fullmatch(r'BLOCK_(?:WOOD|LIGHT|ROCK)_(?:4X4|TRIANGLE_L|TRIANGLE_M)_[23]',n):target=n[:-1]+('1' if n.endswith('2') else '4')
            if target and target in banks[b]:aliases[b][n]=target
        banks[b]-=set(aliases[b])
    return [sorted(b) for b in banks],aliases

def extract(apk,out,sevenzip,game="classic"):
    sprites=[];manifest=[]
    with zipfile.ZipFile(apk) as archive,tempfile.TemporaryDirectory(prefix='ps2birds-sprites-') as folder:
        for sheet,names in SHEETS.items():
            base='assets/data/images/base/'+sheet
            encrypted=archive.read(base+'.sheet.json')
            decoded=decode_payload(encrypted,game,sevenzip)
            frames={f['filename']:f for f in json.loads(decoded)['frames']}
            is_png=base+'.stream.7z' not in archive.namelist()
            if is_png:png=archive.read(base+'.png')
            else:
                packed=Path(folder)/'sheet.7z';packed.write_bytes(archive.read(base+'.stream.7z'))
                raw=subprocess.run(archive_command(sevenzip)+['x','-so',str(packed)],capture_output=True,check=True).stdout
            for name in names:
                if name not in frames:raise ValueError('Classic sprite adapter missing '+name+'; this game/version needs its own asset adapter')
                f=frames[name];fw,fh=f['frame']['w'],f['frame']['h']
                if is_png:rgba=decode_png_frame(png,f)
                else:
                    s=f['stream'];p=s['position'];w=s['width'];h=s['height']
                    head=raw[p:p+40]
                    if len(head)!=40 or head[4:6]!=b'\x00\x28' or head[32:40]!=b'RGBA4444':raise ValueError('Unsupported stream header: '+name)
                    if struct.unpack('>HH',head[6:10])!=(w,h) or s['length']!=w*h*2:raise ValueError('Bad dimensions: '+name)
                    if f['rotated']:raise ValueError('Unsupported frame orientation: '+name)
                    left,top,right,bottom=f['extrude']
                    fw,fh=f['frame']['w'],f['frame']['h']
                    if (w,h)!=(fw+2+left+right,fh+2+top+bottom):raise ValueError('Bad extrusion: '+name)
                    data=raw[p+40:p+40+w*h*2]
                    if len(data)!=w*h*2:raise ValueError('Truncated sprite: '+name)
                    rgba=decode_rgba4444(data,w,h)
                    rgba=bytearray().join(rgba[((y+top)*(w-2)+left)*4:((y+top)*(w-2)+left+fw)*4] for y in range(fh))
                pivot=dict(f['pivot'])
                if name.startswith('GROUND_BLOCK_'):
                    rgba,fw,fh=halve_mask(rgba,fw,fh);pivot={'x':(pivot['x']+1)//MASK_SCALE,'y':(pivot['y']+1)//MASK_SCALE}
                elif name.startswith(HALF_RESOLUTION):
                    rgba,fw,fh=halve_rgba(rgba,fw,fh);pivot={'x':(pivot['x']+1)//MASK_SCALE,'y':(pivot['y']+1)//MASK_SCALE}
                sprite={'name':name,'width':fw,'height':fh,'pivot_x':pivot['x'],'pivot_y':pivot['y'],'rgba':rgba}
                sprites.append(sprite)
                manifest.append({'sprite':name,'sheet':base,'source_format':'PNG RGBA8' if is_png else 'RGBA4444 little-endian','processing':'alpha mask at half resolution (2x2 box filter), white RGB for runtime tint; drawn at SPRITE_MASK_SCALE' if name.startswith('GROUND_BLOCK_') else 'half resolution (2x2 alpha-weighted box filter); drawn at SPRITE_MASK_SCALE' if name.startswith(HALF_RESOLUTION) else 'crop extrusion and convert to GS RGBA32','rgba_sha256':hashlib.sha256(rgba).hexdigest(),'width':fw,'height':fh,'pivot':pivot})
    banks,aliases=bank_members([s['name'] for s in sprites])
    by_name={s['name']:s for s in sprites};pixels=bytearray();bank_specs=[];placement={}
    for bank,members in enumerate(banks):
        specs,placed=pack([by_name[n] for n in members],len(pixels))
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
    for entry in manifest:
        entry['banks']=[{'bank':b,'page':placement[(b,entry['sprite'])][0],'atlas_x':placement[(b,entry['sprite'])][1],'atlas_y':placement[(b,entry['sprite'])][2]} for b in range(len(banks)) if (b,entry['sprite']) in placement]
    (out/'manifest.json').write_text(json.dumps({'game_profile':game,'sprite_adapter':'classic_campaign_sprites','apk':Path(apk).name,'vram_budget':VRAM_BUDGET,'slot_bytes':slots,'banks':[{'bank':b,'pages':bank_specs[b],'sprites':banks[b],'aliases':aliases[b]} for b in range(len(banks))],'bytes':len(pixels),'alpha_range':[0,128],'atlas_sha256':hashlib.sha256(pixels).hexdigest(),'sprites':manifest},indent=2)+'\n')
    print(f'{len(sprites)} original sprites, {len(banks)} texture banks ({", ".join(str(len(b)) for b in banks)} sprites), {len(pixels)} bytes in RAM, {sum(slots)} bytes of VRAM')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--sevenzip',default='7z');p.add_argument('--game',choices=sorted(PROFILES),default='classic');a=p.parse_args();extract(a.apk,a.output,a.sevenzip,a.game)
