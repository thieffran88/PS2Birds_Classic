"""CUTSCENE.BIN for the Rio build, from the player's own APK: the comic panels of CutsceneScene.lua (33 pictures,
CUTSCENE_IMAGE_n in the 864x480 sheets CUTSCENES_SHEET_1..4), as the panel scenes the runtime already plays (options bit 2,
tools/extract_cutscene.py). In the original each animation is a list of states of 2 s; entering a state shows its picture
and the earlier ones stay. Every picture is drawn at the centre of the screen shifted by its pivot (the frame's own place
in the page) and all pictures of an animation are scaled together to fit 85 % x 95 % of the screen (layout()).
The animation table below was read from the decompiled CutsceneScene.lua (the script builds it in code, which the data
reader cannot execute): theme1Start and the six theme<N>End animations.
usage: extract_cutscene_rio.py APK OUT.BIN"""
import json,struct,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'));sys.path.insert(0,str(ROOT/'tools/rio'))
import crop_sprites,extract_ui
SCREEN_W,SCREEN_H=640,448;NATIVE_W,NATIVE_H=864,480;STATE_S=2.0;HOLD_S=3.0;MUSIC_INTRO=13   # story_music is the INTRO track of the Rio's MUSIC.BIN
# scene name in CUTSCENE.BIN -> (animation of the script, CUTSCENE_IMAGE numbers in order)
SCENES=[('rio_intro','theme1Start',[1,2,3,4,5,6,7]),('rio_end1','theme1End',[12,13,14,15,16]),('rio_end2','theme2End',[8,9,10,11]),
        ('rio_end3','theme3End',[17,18,19]),('rio_end4','theme4End',[20,21,22,23]),('rio_end5','theme5End',[24,25,26]),
        ('rio_end6','theme6End',[27,28,29,30,31,32,33])]
def main(apk,out):
    inv=json.loads(crop_sprites.INV.read_text());where={}
    for p in inv['parsed']:
        if p['path'].split('/')[3]!='864x480' or 'CUTSCENES_SHEET' not in p['path']:continue
        for f in p.get('frames') or []:where[f['name']]=(p['texture'],f)
    blobs=[];cache={}
    with zipfile.ZipFile(apk) as z:
        for name,_,images in SCENES:
            frames=[where['CUTSCENE_IMAGE_%d'%n] for n in images]
            # layout(): the box is 85 % x 95 % of the screen; a picture that would leave it shrinks the whole scene
            m=1.0;lim=(NATIVE_W*0.425,NATIVE_H*0.475)
            for _,f in frames:
                l,t,r,b=-f['px'],-f['py'],f['w']-f['px'],f['h']-f['py']
                if l<-lim[0]:m=min(m,lim[0]/-l)
                if r>lim[0]:m=min(m,lim[0]/r)
                if t<-lim[1]:m=min(m,lim[1]/-t)
                if b>lim[1]:m=min(m,lim[1]/b)
            k=m*SCREEN_W/NATIVE_W
            blob=struct.pack('<I',len(frames))
            for i,(tex,f) in enumerate(frames):
                if tex not in cache:cache[tex]=crop_sprites.sheet(z,'864x480',tex)
                w,h,px=cache[tex];rows=bytearray()
                for y in range(f['y'],f['y']+f['h']):rows+=px[(y*w+f['x'])*4:(y*w+f['x']+f['w'])*4]
                for q in range(3,len(rows),4):rows[q]=(rows[q]*128+127)//255
                small,lw,lh=extract_ui.scale(rows,f['w'],f['h'],k);pix=bytearray()
                for q in range(0,len(small),4):   # alpha 0..128; one bit here, soft edges blend with black
                    al=small[q+3]
                    if al<64:pix+=b'\0\0';continue
                    r,g,b=small[q],small[q+1],small[q+2]
                    if al<128:r,g,b=r*al//128,g*al//128,b*al//128
                    pix+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15))
                cx,cy=SCREEN_W//2,SCREEN_H//2
                blob+=struct.pack('<16i',cx,cy,cx,cy,1000,1000,int(i*STATE_S*1000),0,0,0,0,lw,lh,int(round(f['px']*k)),int(round(f['py']*k)),0)+bytes(pix)
            end=int((len(frames)*STATE_S+HOLD_S)*1000)
            blobs.append((name,SCREEN_W,SCREEN_H,0,0,end,MUSIC_INTRO,4,blob,len(blob),0,0,0))
    head=struct.pack('<4s2I',b'PBCS',5,len(blobs));offset=len(head)+72*len(blobs);offset+=(-offset)%64
    table=b'';pos=offset
    for name,sw,sh,st,sc,en,mu,op,px,t1,st2,sc2,t2 in blobs:table+=struct.pack('<24s6IiI4i',name.encode()[:23],sw,sh,st,sc,en,pos,mu,op,t1,st2,sc2,t2);pos+=len(px)+(-len(px))%64
    data=bytearray(head+table);data+=bytes(offset-len(data))
    for b in blobs:data+=b[8]+bytes((-len(b[8]))%64)
    Path(out).write_bytes(data);print('CUTSCENE.BIN do Rio:',len(blobs),'cenas,',len(data),'bytes');return len(data)
if __name__=='__main__':main(*sys.argv[1:3])
