#!/usr/bin/env python3
"""Pack the converted sprites into SPRITES.BIN, the texture file the game reads at run time.

Layout (little endian):
  "PBSP", version 2, bank count, page count, sprite count, pixel start, icon offset, icon size
  pages  [bank][page] : width, height, offset (u32, offset from the pixel start)
  frames [bank][sprite]: x, y, w, h, pivot x, pivot y, page (i16; page -1 = not in the bank)
  pixels: RGBA32 pages of every bank, then the memory-card icon (save.icn).
The sprite order must be the one compiled into the game (ps2/src/sprite_ids.h)."""
import argparse,json,re,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def pack(manifest,pixels,icon,ids,output):
    meta=json.loads(Path(manifest).read_text());data=Path(pixels).read_bytes();icon=Path(icon).read_bytes()
    order=[n for n,_ in sorted(re.findall(r'#define SPR_(\w+) (\d+)',Path(ids).read_text()),key=lambda x:int(x[1]))]
    banks=meta['banks'];pages=max(len(b['pages']) for b in banks)
    by=lambda: {s['sprite']:s for s in meta['sprites']}
    sprites=by()
    expected=sorted(sprites,key=lambda n:(-sprites[n]['height'],n))
    if expected!=order:raise SystemExit('The sprites of this APK do not match the game (sprite_ids.h): use the APK version the ISO was made for')
    want_banks=int(re.search(r'#define SPRITE_BANK_COUNT (\d+)',Path(ids).read_text())[1])
    if want_banks!=len(banks):raise SystemExit('Texture bank count differs from the game')
    if len(data)!=sum(p['width']*p['height']*4 for b in banks for p in b['pages']):raise SystemExit('Unexpected atlas size')
    head_len=32+len(banks)*pages*12+len(banks)*len(order)*14
    start=(head_len+127)//128*128
    out=bytearray(struct.pack('<4s7I',b'PBSP',2,len(banks),pages,len(order),start,start+len(data),len(icon)))
    for b in banks:
        for i in range(pages):
            p=b['pages'][i] if i<len(b['pages']) else {'width':0,'height':0,'offset':0}
            out+=struct.pack('<3I',p['width'],p['height'],p['offset'])
    for bi,b in enumerate(banks):
        for n in order:
            target=b['aliases'].get(n,n);s=sprites[target]
            place=next((x for x in s['banks'] if x['bank']==bi),None)
            page,x,y=(place['page'],place['atlas_x'],place['atlas_y']) if place else (-1,0,0)
            out+=struct.pack('<7h',x,y,s['width'],s['height'],s['pivot']['x'],s['pivot']['y'],page)
    out+=bytes(start-len(out));out+=data+icon
    Path(output).parent.mkdir(parents=True,exist_ok=True);Path(output).write_bytes(out)
    print(f'SPRITES.BIN: {len(banks)} banks, {len(order)} sprites, {len(out)} bytes')

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('manifest');p.add_argument('pixels');p.add_argument('icon');p.add_argument('output');p.add_argument('--ids',default=str(ROOT/'ps2/src/sprite_ids.h'))
    a=p.parse_args();pack(a.manifest,a.pixels,a.icon,a.ids,a.output)
