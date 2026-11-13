#!/usr/bin/env python3
"""Create an own double-sided PS2 icon using the already converted RedBird.
Format reference: ticky/ps2iconsys, src/ps2_ps2icon.cpp (no code copied).
Uses only the Python standard library and project assets.
"""
import json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def build(assets=None,write_sys=True):
    """assets: folder with manifest.json and sprites.rgba; writes save.icn there (and icon.sys if asked)."""
    assets=Path(assets) if assets else ROOT/'ps2/assets';meta=json.loads((assets/'manifest.json').read_text())
    frame=next(s for s in meta['sprites'] if s['sprite']=='BIRD_RED');raw=(assets/'sprites.rgba').read_bytes()
    place=next(b for b in frame['banks'] if b['bank']==0);frame=dict(frame,page=place['page'],atlas_x=place['atlas_x'],atlas_y=place['atlas_y']);pages=meta['banks'][0]['pages']
    tex=bytearray()
    for y in range(128):
        for x in range(128):
            bg=(23,53,58);left,top,width,height=15,16,98,96
            if left<=x<left+width and top<=y<top+height:
                sx=(x-left)*frame['width']//width;sy=(y-top)*frame['height']//height
                page=pages[frame['page']];p=page['offset']+((frame['atlas_y']+sy)*page['width']+frame['atlas_x']+sx)*4
                r,g,b,a=raw[p:p+4];bg=tuple((c*a+d*(128-a))//128 for c,d in zip((r,g,b),bg))
            r,g,b=bg;tex+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10))
    corners=[(-1,1,0,0),(1,1,1,0),(1,-1,1,1),(-1,-1,0,1)]
    icon=bytearray(struct.pack('<5I',0x10000,1,7,0x3f800000,12))
    for back,order in [(False,[0,1,2,0,2,3]),(True,[2,1,0,3,2,0])]:
        for i in order:
            x,y,u,v=corners[i]
            icon+=struct.pack('<4h4h2hI',int(x*4096),int(y*4096),-100 if back else 100,0,0,0,-4096 if back else 4096,0,u*4096,v*4096,0xffffffff)
    icon+=struct.pack('<IIfII',1,31,1.0,0,1)+struct.pack('<IIff',0,1,0.0,1.0)+tex
    (assets/'save.icn').write_bytes(icon)
    if not write_sys:return
    descriptor=bytearray(964);descriptor[:4]=b'PS2D';struct.pack_into('<HHII',descriptor,4,0,16,0,0x60)
    for i in range(4):struct.pack_into('<4i',descriptor,16+i*16,23,53,58,128)
    for i in range(3):struct.pack_into('<4f',descriptor,80+i*16,0,0,1,0);struct.pack_into('<4f',descriptor,128+i*16,0.5,0.5,0.5,0)
    struct.pack_into('<4f',descriptor,176,0.5,0.5,0.5,0)
    title='ＰＳ２ＢｉｒｄｓＣｌａｓｓｉｃ'.encode('shift_jis');descriptor[192:192+len(title)]=title
    for offset in [260,324,388]:descriptor[offset:offset+9]=b'save.icn\0'
    (assets/'save_icon.sys').write_bytes(descriptor)
    (assets/'save_assets.h').write_text(f'#define SAVE_ICON_BYTES {len(icon)}\n#define SAVE_SYS_BYTES {len(descriptor)}\n')
    print(f'Save icon: {len(icon)} bytes; icon.sys: {len(descriptor)} bytes')
if __name__=='__main__':build()
