"""SPRITES.BIN (format "PBSP" v2, the one the runtime reads) and sprite_ids.h for the Rio bank made by
tools/rio/extract_sprites_rio.py. One texture bank; sprite order = the Classic's (tallest first, then name),
which is the order compiled into the runtime. sprite_ids.h holds only names and slot sizes, like the Classic's.
usage: pack_sprites_rio.py BANKDIR OUT_SPRITES.BIN OUT_sprite_ids.h"""
import json,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SLOT_BYTES=[1048576,1048576,524288,262144];SLOTS_N=len(SLOT_BYTES)   # the Classic's four texture slots (extract_sprites.PAGE_LAYOUT)
def pack(bank,out_bin,out_ids):
    """One texture bank (kept for the single-bank callers)."""
    return pack_banks([bank],out_bin,out_ids)
def pack_banks(banks,out_bin,out_ids):
    """One texture bank per entry of `banks` (a directory made by extract_sprites_rio.main). Sprite ids are global (sorted by name,
    the order compiled into the runtime); each bank has its own pages and frames, and a sprite the bank does not hold has an empty frame."""
    ms=[];datas=[];by_bank=[]
    for bank in banks:
        bank=Path(bank);m=json.loads((bank/'manifest.json').read_text());d=(bank/'sprites.rgba').read_bytes()
        if len(d)!=sum(p['width']*p['height']*4 for p in m['pages']):raise SystemExit('tamanho do atlas inesperado')
        ms.append(m);datas.append(d);by_bank.append({s['name']:s for s in m['sprites']})
    by={}
    for bb in by_bank:
        for n,sp in bb.items():by.setdefault(n,sp)
    order=sorted(by);m=ms[0]
    # Pages of each bank, offsets absolute in the data section; the menus upload into the same four texture slots as the Classic's,
    # so every bank declares four slots (unused ones are width 0).
    page_tables=[];base=0
    for mm,d in zip(ms,datas):
        pg=[dict(p,offset=p['offset']+base) for p in mm['pages']]
        pg+=[{'width':0,'height':0,'offset':0}]*(SLOTS_N-len(pg));page_tables.append(pg);base+=len(d)
    head=32+len(banks)*SLOTS_N*12+len(banks)*len(order)*14;start=(head+127)//128*128
    data=b''.join(datas)
    out=bytearray(struct.pack('<4s7I',b'PBSP',2,len(banks),SLOTS_N,len(order),start,start+len(data),0))
    for pg in page_tables:
        for p in pg:out+=struct.pack('<3I',p['width'],p['height'],p['offset'])
    for bb in by_bank:
        for n in order:
            sp=bb.get(n)
            out+=struct.pack('<7h',sp['atlas_x'],sp['atlas_y'],sp['width'],sp['height'],sp['pivot_x'],sp['pivot_y'],sp['page']) if sp else struct.pack('<7h',0,0,0,0,0,0,0)
    out+=bytes(start-len(out));out+=data   # no memory-card icon yet (the save works with a blank one)
    Path(out_bin).parent.mkdir(parents=True,exist_ok=True);Path(out_bin).write_bytes(out)
    pages=page_tables[0]
    slots=SLOT_BYTES
    ids=['/* Sprite identifiers and texture-slot sizes for the Rio APK supported by tools/gerar_iso.py --jogo rio.',
         '   Only names and sizes: the frames and pixels are read from SPRITES.BIN at run time. */',
         '#ifndef PS2BIRDS_RIO_SPRITE_IDS_H','#define PS2BIRDS_RIO_SPRITE_IDS_H',
         '#define SPRITE_VRAM_BUDGET %d'%m['vram_budget'],'#define SPRITE_MASK_SCALE 2','#define SPRITE_BANK_COUNT %d'%len(banks),'#define SPRITE_PAGE_COUNT %d'%len(pages),
         'static const int sprite_slot_bytes[SPRITE_PAGE_COUNT] = {%s};'%','.join(map(str,slots))]
    ids+=['#define SPR_%s %d'%(n,i) for i,n in enumerate(order)]+['#define SPRITE_COUNT %d'%len(order),'#endif','']
    Path(out_ids).parent.mkdir(parents=True,exist_ok=True);Path(out_ids).write_text('\n'.join(ids))
    # Piece-to-sprite table (the Classic's definition_sprites.h): a piece is drawn with its sprite, or with
    # the body of its composition (the cage; the freed bird is not drawn yet). Pieces whose sprite is not
    # in the bank (WebP) are left out and are not drawn.
    import extract_sprites_rio as xr
    cat=json.loads((ROOT/'work/rio/analysis/definition_catalogue.json').read_text())['definitions']
    comps=json.loads((ROOT/'work/rio/analysis/comp_inventory.json').read_text())
    body={c['name']:c['children'][-1]['sprite'] for p in comps['parsed'] for c in p.get('composites') or [] if c.get('children')}
    rows=[]
    for d in sorted(cat):
        e=cat[d].get('effective') or {};sp=e.get('sprite');sp=sp if sp in by else body.get(sp,sp)   # a baked composition is a sprite of the bank by its own name
        if sp in by and e.get('type') in ('box','circle','polygon') and not xr.invisible(e):rows.append(' {"%s",SPR_%s,1},'%(d,sp))
    import extract_sprites_rio as xr
    brows=[];mrows=[]
    for d in sorted(cat):
        e=cat[d].get('effective') or {}
        if e.get('themed') and e.get('sprite') in by and e.get('type') in ('box','circle','polygon'):mrows.append(' {"%s",SPR_%s},'%(d,e['sprite']))
    def sid(n):return 'SPR_%s'%n if n in by else '-1'
    for t in range(13):
        b=xr.BIRDS.get(t,'')
        sp=((cat.get(b) or {}).get('effective') or {}).get('sprites') or {}
        timer=sp.get('timer') if isinstance(sp.get('timer'),dict) else {}
        tm=[timer[k] for k in sorted(timer,key=float)][:3];tm+=[None]*(3-len(tm))
        extra=xr.PUFFED if t==8 else None
        act=sp.get('action')
        if isinstance(act,dict):act=(act.get('frames') or {}).get('1.0')   # animated action sprite: the first frame
        fields=[sp.get('default'),sp.get('flying'),sp.get('collision'),act]+tm+[extra]
        ids_=[sid(n) for n in fields]
        brows.append(' {"%s",%s,%s,%s,%s,{%s},%s},   /* type %d */'%(b,*ids_[:4],','.join(ids_[4:7]),ids_[7],t) if False else ' {"%s",%s,%s,%s,%s,{%s},%s},   /* type %d */'%(b,ids_[0],ids_[1],ids_[2],ids_[3],','.join(ids_[4:7]),ids_[7],t))
    defs=['/* Generated by tools/rio/pack_sprites_rio.py: sprite of each Rio piece in the bank. */','#ifndef PS2BIRDS_RIO_DEFINITION_SPRITES_H','#define PS2BIRDS_RIO_DEFINITION_SPRITES_H',
          'typedef struct {const char *definition;int sprite,scale;} DefinitionSprite;','static const DefinitionSprite definition_sprites[]={']+rows+[' {0,0,0}','};','/* Birds by runtime type (BirdType in game.h; empty rows draw as the red bird): normal, flying, collision, special-ability sprite, the black bird\'s three bomb-timer sprites (-1: none) and an extra (globe bird: the inflated sprite). */','typedef struct {const char *definition;int normal,flying,collision,action,timer[3],extra;} BirdSprites;','static const BirdSprites bird_sprites[]={']+brows+['};','/* Fixed terrain pieces drawn through the theme\'s ground texture (half-resolution masks, SPRITE_MASK_SCALE). */','typedef struct {const char *definition;int sprite;} GroundMask;','static const GroundMask ground_masks[]={']+mrows+[' {0,0}','};','#endif','']
    Path(out_ids).with_name('definition_sprites.h').write_text('\n'.join(defs))
    print('definition_sprites.h do Rio: %d peças'%len(rows))
    print('SPRITES.BIN do Rio: %d banco(s), %d sprites, %d bytes; sprite_ids.h com %d nomes'%(len(banks),len(order),len(out),len(order)))
if __name__=='__main__':pack(*sys.argv[1:4])
