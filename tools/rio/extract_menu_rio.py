"""MENU.BIN for the Rio build, from the player's own APK: same layout and identifiers as the Classic's (ps2/src/menu_gfx.h),
filled with the Rio's own frames (main logo, button slices, level buttons, stars, episode cards). The about text is the Rio's TEXT_ABOUT_RIO. Sprites the Rio has no
counterpart for (menu birds, rewards, score boxes) are empty entries, which the menu draws as nothing.
usage: extract_menu_rio.py APK OUT.BIN"""
import json,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import crop_sprites,extract_menu,extract_ui
K=1024/864          # the Classic factors are for 1024-wide frames
# (identifier, Rio frame or None, factor, mirrored): identifiers in the order of extract_menu.SPRITES.
S=[('LOGO','MAIN_LOGO',0.62),('PLAY_LEFT','BTN_POPUP_LEFT',0.62),('PLAY_MIDDLE','BTN_POPUP_MIDDLE',0.62),('PLAY_RIGHT','BTN_POPUP_RIGHT',0.62),
   ('SCOREBOX',None,0),('SCOREBOX_TEXT',None,0),('STARBOX',None,0),
   ('ARROW_LEFT','ICON_NEXT',1.2,True),('ARROW_RIGHT','ICON_NEXT',1.2),('BACK','ICON_BACK',0.55),
   ('LEVEL_CLOSED','LS_BUTTON_LOCKED',0.58),('LEVEL_LOCK','LS_LOCK',0.58)]+\
  [('LEVEL_OPEN_%d'%i,'LS_BUTTON_BG_%d'%i,0.58) for i in range(1,8)]+[('STARS_%d'%i,'LS_STARS_%d'%i,0.58) for i in range(1,4)]+\
  [('DOT_WHITE','LS_DOT_WHITE',0.8),('DOT_BLACK','LS_DOT_BLACK',0.8),('OPTIONS','ICON_OPTIONS',0.62),('SOUNDS','ICON_SFX',0.62),('ABOUT','ICON_ABOUT',0.62),
   ('OFF','ICON_DISABLED_SETTINGS',0.62),('ABOUT_BIRDS','IMG_CREDITS_2',1.2)]+[('FLY_'+n,None,0) for n in extract_menu.MENU_BIRDS]+[('YELL_'+n,None,0) for n in extract_menu.MENU_BIRDS]
CARD_FRAMES=['BTN_EP_%d'%i for i in range(1,8)]
def mirror(rgba,w,h):
    o=bytearray(len(rgba))
    for y in range(h):
        for x in range(w):o[(y*w+x)*4:(y*w+x)*4+4]=rgba[(y*w+w-1-x)*4:(y*w+w-x)*4]
    return o
def main(apk,out):
    inv=json.loads(crop_sprites.INV.read_text());where={}
    for p in inv['parsed']:
        folder=p['path'].split('/')[3];tex=p.get('texture') or ''
        if folder!='864x480' or tex.rsplit('.',1)[-1].lower() not in ('pvr','png','webp'):continue
        for f in p.get('frames') or []:where.setdefault(f['name'],(folder,tex,f))
    images=[];cache={};miss=[]
    with zipfile.ZipFile(apk) as z:
        def add(ident,name,factor,flip=False):
            if name is None:images.append((ident,0,0,0,0,b''));return
            if name not in where:miss.append(name);images.append((ident,0,0,0,0,b''));return
            folder,tex,f=where[name]
            if (folder,tex) not in cache:cache[(folder,tex)]=crop_sprites.sheet(z,folder,tex)
            w,h,px=cache[(folder,tex)];rows=bytearray()
            for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
            for q in range(3,len(rows),4):rows[q]=(rows[q]*128+127)//255
            if flip:rows=mirror(rows,f['w'],f['h'])
            r,fw,fh=extract_ui.scale(rows,f['w'],f['h'],factor*K);images.append((ident,fw,fh,0,0,r))
        for e in S:add(e[0],e[1],e[2],len(e)>3)
        for i in range(len(extract_menu.CARDS)):
            add('CARD_%d'%i,CARD_FRAMES[i] if i<len(CARD_FRAMES) else None,extract_menu.CARD);add('TITLE_%d'%i,None,0)
        for i in range(len(extract_menu.REWARDS)):add('REWARD_%d'%i,None,0);add('REWARD_STAR_%d'%i,None,0)
    about=extract_menu.texts(apk)['TEXT_ABOUT_RIO']   # credits: the Rio's own about text (pt_BR, English where not translated)
    r=extract_menu.pack_menu(images,apk,out,about=about)
    print('MENU.BIN do Rio:',r,'| sem quadro:',miss);return r
if __name__=='__main__':main(*sys.argv[1:3])
