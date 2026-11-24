#!/usr/bin/env python3
"""Menu graphics for MENU.BIN: main menu logo and PLAY button, episode cards (background +
title, compositions from LEVELSELECTION.compo.json), score/star boxes, arrows, level buttons
(LS_LEVEL_BG_NORMAL_OPEN_1..7, closed, lock), LS_STARS_1..3 and page dots. While a menu is
open these pages replace the stage textures in the four VRAM slots (512x512, 512x512,
512x256, 512x128 CT32). Frames are scaled from the 1024 x 768 originals. No game script runs."""
import argparse,json,struct,zipfile
from pathlib import Path
from game_profiles import decode_payload
from extract_sprites import decode_png_frame
from extract_ui import decode_pvr_frame,scale
PAGES=[(512,512),(512,512),(512,256),(512,128)]
CARD=0.5;LEVEL=0.58
# main_menu.lua animateBirds: birds thrown across the main menu (INGAME_BIRDS_1 stream sheet).
MENU_BIRDS=['BIRD_RED','BIRD_BLUE','BIRD_YELLOW','BIRD_GREY','BIRD_GREEN','BIRD_BIG_BROTHER','BIRD_BOOMERANG']
# (identifier = order in ps2/src/menu_gfx.h, sprite, scale)
SPRITES=[('LOGO','MENU_LOGO',0.62),('PLAY_LEFT','PLAY_BUTTON_BG_LEFT',0.62),('PLAY_MIDDLE','PLAY_BUTTON_BG_MIDDLE',0.62),('PLAY_RIGHT','PLAY_BUTTON_BG_RIGHT',0.62),
 ('SCOREBOX','EP_BTN_SCOREBOX_BACKGOUND',CARD),('SCOREBOX_TEXT','EP_BTN_SCOREBOX_TEXT_PT_BR',CARD),('STARBOX','EP_BTN_STARBOX',CARD),
 ('ARROW_LEFT','BUTTON_ARROW_LEFT',0.5),('ARROW_RIGHT','BUTTON_ARROW_RIGHT',0.5),('BACK','ARROW_LEFT',0.55),
 ('LEVEL_CLOSED','LS_LEVEL_BG_NORMAL_CLOSED',LEVEL),('LEVEL_LOCK','LS_LEVEL_BG_LOCK',LEVEL)]+\
 [(f'LEVEL_OPEN_{i}',f'LS_LEVEL_BG_NORMAL_OPEN_{i}',LEVEL) for i in range(1,8)]+\
 [(f'STARS_{i}',f'LS_STARS_{i}',LEVEL) for i in range(1,4)]+[('DOT_WHITE','LS_DOT_WHITE',0.8),('DOT_BLACK','LS_DOT_BLACK',0.8)]+\
 [('OPTIONS','BUTTON_OPTIONS',0.62),('SOUNDS','BUTTON_SOUNDS_SMALL',0.62),('ABOUT','BUTTON_ABOUT_SMALL',0.62),('OFF','BUTTON_OFF_SMALL',0.62),('ABOUT_BIRDS','ABOUT_BIRDS_3',0.62)]+\
 [(f'FLY_{n}',n,1.0) for n in MENU_BIRDS]+[(f'YELL_{n}',n+'_YELL',0.8) for n in MENU_BIRDS]
# main_menu.lua options (gear, sound, about) and credits.lua (ABOUT_BIRDS_3, TEXT_ABOUT).
# Episode cards in the port's episode order (menu.c): composition name, or None for the generic card.
CARDS=['BTN_EP1','BTN_EP2','BTN_EP3','BTN_EP4','BTN_EP5','BTN_EP6','BTN_EP7','BTN_SURFTURF','BTN_BADPIGGIES',
       'BTN_BOMB_BIRD','BTN_BIRDISLAND','BTN_FARM','BTN_DINO','BTN_EP5','BTN_FLOCKFAVORITES',
       'BTN_GOLDENEGGS',('BTN_TUTORIAL_BACKGROUND','BTN_TUTORIAL_TEXT'),('BTN_EP1_BACKGROUND',None),'BTN_EGGDEFENDER']
# main_menu.lua buildBirdList: each episode's reward_completion / reward_three_stars sprite flies
# with the birds once the episode is completed / three-starred (episodes.lua). Port episode
# names -> reward sprite (completion, three stars).
REWARDS=[('POACHED EGGS','REWARD_1'),('MIGHTY HOAX','REWARD_2'),('DANGER ABOVE','REWARD_3'),('BIG SETUP','REWARD_4'),
         ('HAM EM HIGH','REWARD_5'),('MINE AND DINE','REWARD_6'),('BIRDDAY','REWARD_7'),('BAD PIGGIES','REWARD_8'),
         ('SHORT FUSE','REWARD_9'),('SURF AND TURF','REWARD_SURFTURF'),('PIGGY FARM','REWARD_FARM'),('DINO','REWARD_DINO')]
REWARD_SCALE=0.45
# Mighty Feathers has no card of its own in this APK: the Poached Eggs art, its title drawn with FONT_BASIC.
def texts(apk,language='pt_BR'):
    """TEXTS_BASIC.dat (KA3D TEXT): LDAT language list, LIDS identifiers, one TXGP per language."""
    with zipfile.ZipFile(apk) as z:d=z.read('assets/data/localization/TEXTS_BASIC.dat')
    i=d.index(b'LDAT');count=struct.unpack_from('>H',d,i+8)[0];p=i+10;langs=[]
    for _ in range(count):l=struct.unpack_from('>H',d,p)[0];langs.append(d[p+2:p+2+l].decode());p+=2+l
    i=d.index(b'LIDS');n=struct.unpack_from('>H',d,i+8)[0];p=i+10;ids=[]
    for _ in range(n):l=struct.unpack_from('>H',d,p)[0];ids.append(d[p+2:p+2+l].decode());p+=2+l
    groups=[]
    while p<len(d) and d[p:p+4]==b'TXGP':
        size=struct.unpack_from('>I',d,p+4)[0];q=p+8;strings=[]
        for _ in range(n):l=struct.unpack_from('>H',d,q)[0];strings.append(d[q+2:q+2+l].decode('utf-8','replace'));q+=2+l
        groups.append(strings);p+=8+size
    table=groups[langs.index(language)];english=groups[0]
    return {k:(table[i] or english[i]) for i,k in enumerate(ids)}
def about_text(apk):return texts(apk)['TEXT_ABOUT']
def frames_of(z):
    """sprite name -> (sheet path, frame) over the 1024x768 sheets."""
    out={}
    for n in z.namelist():
        if n.startswith('assets/data/images/1024x768/') and n.endswith('.sheet.json'):
            try:fr=json.loads(decode_payload(z.read(n),'classic'))['frames']
            except Exception:continue
            for f in fr:out.setdefault(f['filename'],(n[:-len('.sheet.json')],f))
    return out
_WEBP={}
def decode_webp_frame(z,base,frame):
    """One frame of a WebP sheet (tools/webp.py, no library): RGBA with the alpha on the GS scale (0..128)."""
    import webp
    if base not in _WEBP:_WEBP.clear();_WEBP[base]=webp.decode_webp(z.read(base+'.webp'))
    sw,sh,px=_WEBP[base]
    if frame.get('rotated'):raise ValueError('Unsupported WebP orientation')
    f=frame['frame'];x,y,w,h=(f[k] for k in ('x','y','w','h'))
    if w<=0 or h<=0 or x<0 or y<0 or x+w>sw or y+h>sh:raise ValueError('WebP frame outside sheet')
    rgba=bytearray().join(px[((y+r)*sw+x)*4:((y+r)*sw+x+w)*4] for r in range(h))
    for i in range(3,len(rgba),4):rgba[i]=(rgba[i]*128+127)//255
    return rgba
def read(z,where,name,cache={}):
    base,f=where[name]
    if base+'.png' in z.namelist():return decode_png_frame(z.read(base+'.png'),f),f
    if base+'.webp' in z.namelist():return decode_webp_frame(z,base,f),f
    if base not in cache:cache.clear();cache[base]=z.read(base+'.pvr')
    return decode_pvr_frame(cache[base],f),f
def extract(apk,out):
    with zipfile.ZipFile(apk) as z:
        where=frames_of(z);images=[]
        compo={c['name']:c for c in json.loads(decode_payload(z.read('assets/data/images/1024x768/LEVELSELECTION.compo.json'),'classic'))['compo']}
        def add(ident,name,factor,dx=0,dy=0):
            if name in MENU_BIRDS or name.endswith('_YELL'):
                from extract_sprites import sheet_sprite
                rgba,fw,fh,_=sheet_sprite(z,'INGAME_BIRDS_1',name)
                if factor!=1.0:rgba,fw,fh=scale(rgba,fw,fh,factor)
                images.append((ident,fw,fh,0,0,rgba));return
            rgba,f=read(z,where,name);small,w,h=scale(rgba,f['frame']['w'],f['frame']['h'],factor)
            images.append((ident,w,h,round(dx*factor),round(dy*factor),small))
        for ident,name,factor in SPRITES:add(ident,name,factor)
        for i,card in enumerate(CARDS):
            if isinstance(card,tuple):
                add(f'CARD_{i}',card[0],CARD)
                if card[1]:add(f'TITLE_{i}',card[1],CARD,0,-150)
                else:images.append((f'TITLE_{i}',0,0,0,0,b''))
            else:
                parts={s['name']:s for s in compo[card]['sprites']}
                tx=[n for n in parts if n.endswith('_TEXT')][0];bg=[n for n in parts if n!=tx][0]
                add(f'CARD_{i}',bg,CARD,parts[bg]['x'],parts[bg]['y']);add(f'TITLE_{i}',tx,CARD,parts[tx]['x'],parts[tx]['y'])
        for i,(_,name) in enumerate(REWARDS):add(f'REWARD_{i}',name,REWARD_SCALE);add(f'REWARD_STAR_{i}',name+'_STAR',REWARD_SCALE)
    return pack_menu(images,apk,out)
def pack_menu(images,apk,out,about=None):
    """images: (identifier, w, h, dx, dy, RGBA); about: the TEXT_ABOUT text (default: read from the APK). Shared with the Rio."""
    # Shelf packing into the four slots, tallest first, 1-pixel gutter.
    pages=[bytearray(w*h*4) for w,h in PAGES];cursor=[[0,0,0] for _ in PAGES];place={}
    for ident,w,h,dx,dy,rgba in sorted(images,key=lambda s:-s[2]):
        if not w:place[ident]=(0,0,0,0,0);continue
        for p,(pw,ph) in enumerate(PAGES):
            x,y,shelf=cursor[p]
            if x+w>pw:x,y,shelf=0,y+shelf+1,0
            if y+h<=ph and w<=pw:break
        else:raise ValueError('Menu sprites do not fit the VRAM slots')
        pw=PAGES[p][0]
        for r in range(h):pages[p][((y+r)*pw+x)*4:((y+r)*pw+x+w)*4]=rgba[r*w*4:(r+1)*w*4]
        place[ident]=(p,x,y,w,h);cursor[p]=[x+w+1,y,max(shelf,h)]
    order=[i for i,*_ in SPRITES]+[f'{k}_{i}' for i in range(len(CARDS)) for k in ('CARD','TITLE')]+[f'{k}_{i}' for i in range(len(REWARDS)) for k in ('REWARD','REWARD_STAR')]
    offs={i:(dx,dy) for i,w,h,dx,dy,_ in images}
    head=struct.pack('<4s3I',b'PBMN',2,len(order),len(PAGES))+b''.join(struct.pack('<2I',w,h) for w,h in PAGES)
    head+=b''.join(struct.pack('<5H2h2x',*place[i],*offs[i]) for i in order)
    head+=bytes((-len(head))%64)
    # Version 2: after the pages, the pt_BR TEXT_ABOUT of TEXTS_BASIC.dat (credits.lua), UTF-8.
    about=(about_text(apk) if about is None else about).encode()
    Path(out).write_bytes(head+b''.join(bytes(p) for p in pages)+struct.pack('<I',len(about))+about)
    return {'sprites':len(order),'used':[c[1]+c[2] for c in cursor],'bytes':len(head)+sum(len(p) for p in pages)}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);a=p.parse_args()
    print(extract(a.apk,a.output))
