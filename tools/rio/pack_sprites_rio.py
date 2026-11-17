"""SPRITES.BIN (format "PBSP" v2, the one the runtime reads) and sprite_ids.h for the Rio bank made by
tools/rio/extract_sprites_rio.py. One texture bank; sprite order = the Classic's (tallest first, then name),
which is the order compiled into the runtime. sprite_ids.h holds only names and slot sizes, like the Classic's.
usage: pack_sprites_rio.py BANKDIR OUT_SPRITES.BIN OUT_sprite_ids.h"""
import json,struct,sys
from pathlib import Path
def pack(bank,out_bin,out_ids):
    bank=Path(bank);m=json.loads((bank/'manifest.json').read_text());data=(bank/'sprites.rgba').read_bytes()
    by={s['name']:s for s in m['sprites']};order=sorted(by,key=lambda n:(-by[n]['height'],n));pages=m['pages']
    if len(data)!=sum(p['width']*p['height']*4 for p in pages):raise SystemExit('tamanho do atlas inesperado')
    head=32+len(pages)*12+len(order)*14;start=(head+127)//128*128
    out=bytearray(struct.pack('<4s7I',b'PBSP',2,1,len(pages),len(order),start,start+len(data),0))
    for p in pages:out+=struct.pack('<3I',p['width'],p['height'],p['offset'])
    for n in order:
        s=by[n];out+=struct.pack('<7h',s['atlas_x'],s['atlas_y'],s['width'],s['height'],s['pivot_x'],s['pivot_y'],s['page'])
    out+=bytes(start-len(out));out+=data   # no memory-card icon yet (the save works with a blank one)
    Path(out_bin).parent.mkdir(parents=True,exist_ok=True);Path(out_bin).write_bytes(out)
    slots=[p['width']*p['height']*4 for p in pages]
    ids=['/* Sprite identifiers and texture-slot sizes for the Rio APK supported by tools/gerar_iso.py --jogo rio.',
         '   Only names and sizes: the frames and pixels are read from SPRITES.BIN at run time. */',
         '#ifndef PS2BIRDS_RIO_SPRITE_IDS_H','#define PS2BIRDS_RIO_SPRITE_IDS_H',
         '#define SPRITE_VRAM_BUDGET %d'%m['vram_budget'],'#define SPRITE_MASK_SCALE 2','#define SPRITE_BANK_COUNT 1','#define SPRITE_PAGE_COUNT %d'%len(pages),
         'static const int sprite_slot_bytes[SPRITE_PAGE_COUNT] = {%s};'%','.join(map(str,slots))]
    ids+=['#define SPR_%s %d'%(n,i) for i,n in enumerate(order)]+['#define SPRITE_COUNT %d'%len(order),'#endif','']
    Path(out_ids).parent.mkdir(parents=True,exist_ok=True);Path(out_ids).write_text('\n'.join(ids))
    print('SPRITES.BIN do Rio: 1 banco, %d sprites, %d bytes; sprite_ids.h com %d nomes'%(len(order),len(out),len(order)))
if __name__=='__main__':pack(*sys.argv[1:4])
