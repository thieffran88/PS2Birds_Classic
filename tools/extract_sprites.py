#!/usr/bin/env python3
"""Convert selected original APK sprites into an embedded PS2 RGBA32 atlas.
Requires Python 3.9+, OpenSSL and 7-Zip. Never executes game scripts.
"""
import argparse,hashlib,json,lzma,struct,subprocess,tempfile,zipfile
from pathlib import Path
from game_profiles import PROFILES, decode_payload
SHEETS={
 'INGAME_BIRDS_1':['BIRD_BLUE','BIRD_BLUE_COLLISION','BIRD_RED','BIRD_YELLOW','BIRD_YELLOW_COLLISION','SLING_SHOT_01_BACK','SLING_SHOT_01_FRONT'],
 'INGAME_PIGS_NEW_SHEET_1_1':['PIG_BASIC_SMALL_01','PIG_BASIC_SMALL_01_BLINK','PIG_BASIC_SMALL_02','PIG_BASIC_MEDIUM_01','PIG_BASIC_MEDIUM_01_BLINK','PIG_BASIC_MEDIUM_02'],
 'INGAME_PIGS_NEW_SHEET_0_1':['PIG_BASIC_BIG_01','PIG_BASIC_BIG_01_BLINK','PIG_BASIC_BIG_02','PIG_HELMET_01','PIG_HELMET_01_BLINK','PIG_HELMET_02'],
 'INGAME_BLOCKS_1_1':['ESTRADE_01','ESTRADE_02','BLOCK_TREASURE_CHEST','BLOCK_SMILEY_1','BLOCK_SMILEY_3'],
 'INGAME_BLOCKS_3_1':[f'BLOCK_{m}_{state}_{size}' for m,sizes in [('WOOD',[2,3,4,5,6,7]),('LIGHT',[1,2,3,4,5,6]),('ROCK',[1,2,3,4,5,6,7,8])] for state in range(1,5) for size in sizes],
 'INGAME_MASKS_1':['GROUND_BLOCK_01','GROUND_BLOCK_02','GROUND_BLOCK_03','GROUND_BLOCK_04','GROUND_BLOCK_05']}
# VRAM budget: 4 MiB minus two 640x448 CT16 frame buffers (1.09 MiB) leaves
# about 2.9 MiB; keep a margin below that for the embedded texture pages.
VRAM_BUDGET=2621440

def decode_rgba4444(data,w,h):
    if w<3 or h<3 or len(data)!=w*h*2:raise ValueError('Invalid pixel buffer')
    rgba=bytearray()
    for y in range(1,h-1):
        for x in range(1,w-1):
            v=struct.unpack_from('<H',data,(y*w+x)*2)[0]
            rgba+=bytes(((v>>12&15)*17,(v>>8&15)*17,(v>>4&15)*17,((v&15)*128+7)//15))
    return rgba

def extract(apk,out,sevenzip,game="classic"):
    sprites=[];manifest=[]
    with zipfile.ZipFile(apk) as archive,tempfile.TemporaryDirectory(prefix='ps2birds-sprites-') as folder:
        for sheet,names in SHEETS.items():
            base='assets/data/images/base/'+sheet
            encrypted=archive.read(base+'.sheet.json')
            decoded=decode_payload(encrypted,game,sevenzip)
            frames={f['filename']:f for f in json.loads(decoded)['frames']}
            packed=Path(folder)/'sheet.7z';packed.write_bytes(archive.read(base+'.stream.7z'))
            raw=subprocess.run([sevenzip,'x','-so',str(packed)],capture_output=True,check=True).stdout
            for name in names:
                if name not in frames:raise ValueError('Classic sprite adapter missing '+name+'; this game/version needs its own asset adapter')
                f=frames[name];s=f['stream'];p=s['position'];w=s['width'];h=s['height']
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
                if name.startswith('GROUND_BLOCK_'):
                    for pixel in range(0,len(rgba),4):rgba[pixel:pixel+3]=bytes([255,255,255])
                sprite={'name':name,'width':fw,'height':fh,'pivot_x':f['pivot']['x'],'pivot_y':f['pivot']['y'],'rgba':rgba}
                sprites.append(sprite)
                manifest.append({'sprite':name,'sheet':base,'source_format':'RGBA4444 little-endian','processing':'alpha mask, white RGB for runtime tint' if name.startswith('GROUND_BLOCK_') else 'crop extrusion and convert to GS RGBA32','rgba_sha256':hashlib.sha256(rgba).hexdigest(),'width':fw,'height':fh,'pivot':f['pivot']})
    # Deterministic skyline packing with a transparent one-pixel gutter.
    # Pages are 512 wide; a page is widened to 1024 only for sprites wider than 510.
    sprites.sort(key=lambda s:(-s['height'],s['name']))
    pages=[]
    for s in sprites:
        span=s['width']+2
        if span>1024 or s['height']+2>512:raise ValueError('Sprite exceeds page limits')
        for page,entry in enumerate(pages+[None]):
            if entry is None:entry={'width':1024 if span>512 else 512};entry['skyline']=[0]*entry['width']
            width=entry['width'];skyline=entry['skyline']
            if span>width:continue
            y,x=min((max(skyline[x:x+span]),x) for x in range(width-span+1))
            if y+s['height']+2<=512:
                if page==len(pages):pages.append(entry)
                s['page']=page;s['atlas_x']=x+1;s['atlas_y']=y+1
                skyline[x:x+span]=[y+s['height']+2]*span
                break
    specs=[];pixels=bytearray()
    for entry in pages:
        height=1
        while height<max(entry['skyline']):height*=2
        specs.append({'width':entry['width'],'height':height,'offset':len(pixels)})
        pixels+=bytes(entry['width']*height*4)
    if len(pixels)>VRAM_BUDGET:raise ValueError('Atlas pages exceed the VRAM budget')
    for s in sprites:
        page=specs[s['page']]
        for row in range(s['height']):
            start=page['offset']+((s['atlas_y']+row)*page['width']+s['atlas_x'])*4
            pixels[start:start+s['width']*4]=s['rgba'][row*s['width']*4:(row+1)*s['width']*4]
    out.mkdir(parents=True,exist_ok=True);(out/'sprites.rgba').write_bytes(pixels)
    lines=['/* Generated from the supplied APK by tools/extract_sprites.py. */','#ifndef PS2BIRDS_SPRITE_DATA_H','#define PS2BIRDS_SPRITE_DATA_H',f'#define SPRITE_ATLAS_WIDTH {specs[0]["width"]}',f'#define SPRITE_ATLAS_HEIGHT {specs[0]["height"]}',f'#define SPRITE_VRAM_BUDGET {VRAM_BUDGET}',f'#define SPRITE_PAGE_COUNT {len(specs)}','typedef struct { int width,height,offset; } SpritePage;','static const SpritePage sprite_pages[] = {']
    for page in specs:lines.append(' {'+','.join(str(page[k]) for k in ['width','height','offset'])+'},')
    lines+=['};','typedef struct { int x,y,w,h,px,py,page; } SpriteFrame;']
    for i,s in enumerate(sprites):lines.append('#define SPR_'+s['name']+' '+str(i))
    lines+=['static const SpriteFrame sprite_frames[] = {']
    for s in sprites:lines.append(' {'+','.join(str(s[k]) for k in ['atlas_x','atlas_y','width','height','pivot_x','pivot_y','page'])+'},')
    lines+=['};','#define SPRITE_COUNT '+str(len(sprites)),'#endif','']
    (out/'sprite_data.h').write_text('\n'.join(lines))
    by_name={s['name']:s for s in sprites}
    for entry in manifest:
        s=by_name[entry['sprite']];entry['atlas_x']=s['atlas_x'];entry['atlas_y']=s['atlas_y'];entry['page']=s['page']
    (out/'manifest.json').write_text(json.dumps({'game_profile':game,'sprite_adapter':'classic_campaign_sprites','apk':Path(apk).name,'width':specs[0]['width'],'height':specs[0]['height'],'vram_budget':VRAM_BUDGET,'pages':specs,'bytes':len(pixels),'alpha_range':[0,128],'atlas_sha256':hashlib.sha256(pixels).hexdigest(),'sprites':manifest},indent=2)+'\n')
    print(f'{len(sprites)} original sprites, {len(specs)} atlas pages, {len(pixels)} bytes')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--sevenzip',default='7z');p.add_argument('--game',choices=sorted(PROFILES),default='classic');a=p.parse_args();extract(a.apk,a.output,a.sevenzip,a.game)
