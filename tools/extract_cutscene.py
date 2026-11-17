#!/usr/bin/env python3
"""Comic cutscenes for CUTSCENE.BIN. Everything about a scene comes from the APK itself:
cutscenes.lua is read as data (tools/lua51_data.py, no game code runs) and the pictures are
composed from CUTSCENES.compo.json, as the game's own cutscene player lays them out.

Per scene the script gives (cutscenes.lua actions):
  setreferencesize   width and height of the picture, fitheight: scaled to the screen height;
  createsprite       the picture ("background" = a composition of CUTSCENES.compo.json);
  playsound          the music clip;
  scroll             start time and duration of the cubic_in_out scroll, left to right;
  end                the time the scene ends.
Other actions (the King Pig's eyes, one-shot sounds) are ignored; scenes that need more than a
horizontal scroll (vertical scroll, tweened panels) are not in SCENES.

The picture is composed here (pieces at their pivot, back to front), scaled to the 448-row
screen and stored as opaque RGBA5551 rows: the game copies the visible 640 columns into the
frame each frame, as it does with the stage backgrounds. Nothing of the game is kept in the
repository: the scenes are built from the APK when the data files are generated."""
import argparse,json,os,struct,zipfile
import paralelo
from pathlib import Path
from game_profiles import decode_payload
import lua51_data
from extract_menu import frames_of,read
from extract_ui import scale
from extract_audio import MUSIC_OF_CLIP

# cutscenes.lua scene names that the player can show (see docs/CUTSCENES.md).
SCENES=['pack1_intro','world1_complete','world2_complete','world3_complete',
        'pack2_intro','pack3_intro','world4_complete','world5_complete',
        'world6_complete','world7_complete','world8_complete',
        'pack4_intro','world9_complete','world10_complete','world11_complete',
        'pack5_intro','world12_complete','world13_complete','world14_complete',
        'pack6_intro','world15_complete','world16_complete','world17_complete',
        'pack7_intro','birdday3_complete',
        'pack8_intro','worldS1_complete','worldS2_complete','worldS3_complete',
        # Birdday 2 and 3 worlds, Bad Piggies, Short Fuse, Piggy Farm and Dino (WebP sheets: tools/webp.py)
        'world18_complete','world19_complete','pack9_intro','world20_complete','world21_complete','world22_complete',
        'blackbird_intro','blackbird_middle_1','blackbird_middle_2','blackbird_end',
        'intro_cutscene_farm','outro_cutscene_farm','intro_cutscene_dino','outro_cutscene_dino',
        # Egg Defender and Mighty Feathers: panels that pop in one by one; here they are shown together, in their final places
        'eggdefender_intro','eggdefender_complete','rmf_classic_intro','rmf_complete']
# Not here: the Egg Defender scenes (tweened panels) and the Short Fuse, Piggy Farm and Dino
# stories (WebP sheets, which this generator cannot decode).
COMPOS=['CUTSCENES.compo.json','CUTSCENE_BLACKBIRD.compo.json','CUTSCENES_STORY_FARM.compo.json','CUTSCENES_STORY_DINO.compo.json']
SCREEN_H=448;SCREEN_W=640

def parse(script):
    """The parameters of one scene from its list of actions."""
    acts=[script[k] for k in sorted(script)]
    ref=[a for a in acts if a['action']=='setreferencesize'][0]
    pic=[a for a in acts if a['action']=='createsprite' and a.get('name')=='background'][0]
    scroll=[a for a in acts if a['action']=='scroll']
    fit_width=any(a['action']=='fitwidth' for a in acts)     # fitwidth: the picture is as wide as the screen and scrolls vertically
    bg=[a for a in acts if a['action']=='set_bg_colour']
    start=[a for a in acts if a['action']=='set_scroll_position']
    clips=[a['sound'] for a in acts if a['action']=='playsound' and a.get('sound') in MUSIC_OF_CLIP]
    segments=sorted(((a['time'],a['duration'],a['scroll_target'].get('x',1.0)) for a in scroll),key=lambda t:t[0])
    return {'image':pic['image'],'ref_w':int(ref['width']),'ref_h':int(ref['height']),'bg_x':pic.get('x',0.0),'bg_y':pic.get('y',0.0),'segments':segments,
            'music':MUSIC_OF_CLIP[clips[0]] if clips else -1,
            'scroll_start':scroll[0]['time'] if scroll else 0.0,'scroll':scroll[0]['duration'] if scroll else 0.0,
            'end':[a for a in acts if a['action']=='end'][0]['time'],
            'vertical':fit_width,
            # set_scroll_position at y = 1 starts at the bottom of the picture and scrolls up.
            'reverse':bool(fit_width and start and start[0]['scroll_cursor']['y']==1.0),
            'bg':(int(bg[0]['r']),int(bg[0]['g']),int(bg[0]['b'])) if bg else None}

def extent(where,compo):
    """Right and bottom edge of the pieces of a composition (x, y place each piece's pivot)."""
    right=bottom=0
    for s in compo['sprites']:
        f=where[s['name']][1];fw,fh=f['frame']['w'],f['frame']['h'];pv=f.get('pivot',{'x':fw/2,'y':fh/2})
        right=max(right,s['x']-pv['x']+fw);bottom=max(bottom,s['y']-pv['y']+fh)
    return int(round(right)),int(round(bottom))

def compose(z,where,compo,w,h,bg=None):
    # set_bg_colour: the colour behind the pieces (black when the scene does not set one).
    canvas=bytearray((bg+(255,))*(w*h)) if bg else bytearray(w*h*4)
    # The list runs front to back (the sky and grass gradients come last): drawn in reverse.
    for s in reversed(compo['sprites']):
        rgba,f=read(z,where,s['name']);fw,fh=f['frame']['w'],f['frame']['h']
        px,py=f.get('pivot',{}).get('x',fw/2),f.get('pivot',{}).get('y',fh/2)
        sc=s.get('scale',[1,1]);sx,sy=(sc,sc) if isinstance(sc,(int,float)) else sc
        if (sx,sy)!=(1,1):rgba,fw,fh=scale(rgba,fw,fh,abs(sx));px*=abs(sx);py*=abs(sx)
        x0,y0=int(round(s['x']-px)),int(round(s['y']-py));solid='GRADIENT' in s['name']
        for r in range(fh):
            y=y0+r
            if not 0<=y<h:continue
            for c in range(fw):
                x=x0+c
                if not 0<=x<w:continue
                i=(r*fw+c)*4;a=min(255,(rgba[i+3]*255+64)//128)   # source alpha is on the GS scale, 0..128
                if not a:continue
                if solid:a=255   # the sky/grass gradient tiles overlap with soft edges: drawn opaque, no seams
                o=(y*w+x)*4
                for k in range(3):canvas[o+k]=(rgba[i+k]*a+canvas[o+k]*(255-a))//255
                canvas[o+3]=255
    return canvas

def to5551(rgba):
    out=bytearray()
    for i in range(0,len(rgba),4):
        r,g,b=rgba[i],rgba[i+1],rgba[i+2];out+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15))
    return out

def pad_width(rgba,w,h,target):
    """A picture narrower than the screen is centred on its own edge colours."""
    out=bytearray(target*h*4);left=(target-w)//2
    for y in range(h):
        row=rgba[y*w*4:(y+1)*w*4]
        for x in range(target):
            sx=min(max(x-left,0),w-1);out[(y*target+x)*4:(y*target+x)*4+4]=row[sx*4:sx*4+4]
    return out

_STATE={}
def _setup(apk):
    """Per-process state: the archive, the frame table, the compositions and the cutscene script."""
    z=zipfile.ZipFile(apk);where=frames_of(z);comp={}
    for f in COMPOS:
        comp.update({c['name']:c for c in json.loads(decode_payload(z.read('assets/data/images/1024x768/'+f),'classic'))['compo']})
    script=lua51_data.load(decode_payload(z.read('assets/data/scripts/cutscenes.lua'),'classic'))
    _STATE.update(z=z,where=where,comp=comp,script=script)
def _init(apk):_setup(apk)
EASE={'cubicOut':0,'bounceOut':1,'cubicInOut':2,'linear':3}
def build_panels(name,acts):
    """Panel scenes (fittogrid + tweenPosition/tweenScale/alpha tween): every panel is a layer with its own pop-in
    (scale and position tween) and optional fade, drawn by the game in software (options bit 2). All panels are
    fitted together to the screen, as fittogrid does."""
    z=_STATE['z'];where=_STATE['where'];acts=[acts[k] for k in sorted(acts,key=float)]
    lay={}
    for a in acts:
        if a['action']=='createsprite':
            lay[a['name']]={'img':a['image'],'x0':a['x'],'y0':a['y'],'x1':a['x'],'y1':a['y'],'z':a.get('z',0),'start':a['time'],'s0':a.get('scale_x',1.0),'s1':a.get('scale_x',1.0),
                            'dur':0.0,'ease':0,'ast':0.0,'adur':0.0}
    for a in acts:
        L=lay.get(a.get('name'))
        if not L:continue
        if a['action']=='tweenPosition':L['x0'],L['y0'],L['x1'],L['y1']=a['from']['x'],a['from']['y'],a['to']['x'],a['to']['y'];L['dur']=a['duration'];L['ease']=EASE.get(a['type'],0)
        elif a['action']=='tweenScale':L['s0'],L['s1']=a['from']['x'],a['to']['x'];L['dur']=a['duration'];L['ease']=EASE.get(a['type'],0);L['start']=max(L['start'],a['time']) if False else a['time']
        elif a['action']=='tween' and a.get('property')=='alpha':L['ast']=a['time'];L['adur']=a['duration']
    layers=sorted(lay.values(),key=lambda q:q['z'])
    if any(q['img'] not in where for q in layers):return None,'cena %s ignorada: painel desconhecido'%name
    bases={where[q['img']][0] for q in layers}
    if any(b+'.png' not in z.namelist() and b+'.webp' not in z.namelist() and b+'.pvr' not in z.namelist() for b in bases):return None,'cena %s ignorada: formato de imagem sem suporte'%name
    x0=y0=1e9;x1=y1=-1e9
    for q in layers:   # the layout at the end of the tweens
        f=where[q['img']][1];fw,fh=f['frame']['w'],f['frame']['h'];pv=f.get('pivot',{'x':fw/2,'y':fh/2})
        x0=min(x0,q['x1']-pv['x']);y0=min(y0,q['y1']-pv['y']);x1=max(x1,q['x1']-pv['x']+fw);y1=max(y1,q['y1']-pv['y']+fh)
    w=x1-x0;h=y1-y0;k=min(SCREEN_W/w,SCREEN_H/h);ox=(SCREEN_W-w*k)/2;oy=(SCREEN_H-h*k)/2
    def sx(v):return int(round((v-x0)*k+ox))
    def sy(v):return int(round((v-y0)*k+oy))
    blob=struct.pack('<I',len(layers))
    for q in layers:
        rgba,f=read(z,where,q['img']);fw,fh=f['frame']['w'],f['frame']['h'];pv=f.get('pivot',{'x':fw/2,'y':fh/2})
        small,lw,lh=scale(rgba,fw,fh,k)
        px=bytearray()
        for i in range(0,len(small),4):   # source alpha is 0..128; 1 bit here, edges blend with black
            al=small[i+3]
            if al<64:px+=b'\0\0';continue
            r,g,b=small[i],small[i+1],small[i+2]
            if al<128:r,g,b=r*al//128,g*al//128,b*al//128
            px+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15))
        hdr=struct.pack('<16i',sx(q['x0']),sy(q['y0']),sx(q['x1']),sy(q['y1']),int(q['s0']*1000),int(q['s1']*1000),int(q['start']*1000),int(q['dur']*1000),
                        q['ease'],int(q['ast']*1000),int(q['adur']*1000),lw,lh,int(round(pv['x']*k)),int(round(pv['y']*k)),0)
        blob+=hdr+bytes(px)
    clips=[a['sound'] for a in acts if a['action']=='playsound' and a.get('sound') in MUSIC_OF_CLIP]
    end=[a for a in acts if a['action']=='end'][0]['time']
    return (name,SCREEN_W,SCREEN_H,0,0,int(end*1000),MUSIC_OF_CLIP[clips[0]] if clips else -1,4,blob,len(blob),0,0,0),'cena %s pronta (%d painéis animados)'%(name,len(layers))

def overlays(name,where,z,k):
    """Sprites that the script creates after the background (the king pig's blinking eye): each one replaces the
    previous sprite of the same name and stays until the next create/delete or the end of the scene.
    -> [(start ms, end ms, x, y, width, height, RGBA5551 bytes)] in the scaled picture's pixels."""
    acts=sorted((a for a in _STATE['script'][name].values() if a['action'] in('createsprite','deletesprite') and a.get('name')!='background'),key=lambda a:a['time'])
    end=[a for a in _STATE['script'][name].values() if a['action']=='end'][0]['time']
    out=[]
    for i,a in enumerate(acts):
        if a['action']!='createsprite' or a['image'] not in where:continue
        nxt=[b['time'] for b in acts[i+1:] if b['name']==a['name']]
        t1=nxt[0] if nxt else end
        f=where[a['image']][1];fw,fh=f['frame']['w'],f['frame']['h'];pv=f.get('pivot',{'x':fw/2,'y':fh/2})
        base=where[a['image']][0]
        if base+'.png' not in z.namelist() and base+'.webp' not in z.namelist() and base+'.pvr' not in z.namelist():continue
        rgba,_=read(z,where,a['image']);small,ow,oh=scale(rgba,fw,fh,k)
        px=bytearray()
        for j in range(0,len(small),4):
            al=small[j+3]
            if al<64:px+=b'\0\0';continue
            r,g,b=small[j],small[j+1],small[j+2]
            if al<128:r,g,b=r*al//128,g*al//128,b*al//128
            px+=struct.pack('<H',(r>>3)|((g>>3)<<5)|((b>>3)<<10)|(1<<15))
        out.append((int(a['time']*1000),int(t1*1000),int(round((a['x']-pv['x'])*k)),int(round((a['y']-pv['y'])*k)),ow,oh,bytes(px)))
    return out

def build_scene(name):
    """One scene -> (blob tuple or None, message). Runs in a worker process (or in the main one)."""
    z=_STATE['z'];where=_STATE['where'];comp=_STATE['comp']
    if any(a['action']=='fittogrid' for a in _STATE['script'][name].values()):return build_panels(name,_STATE['script'][name])
    p=parse(_STATE['script'][name])
    if p['image'] in comp:entry=comp[p['image']]
    elif p['image'] in where:entry={'sprites':[{'name':p['image'],'x':p['bg_x'],'y':p['bg_y']}]}   # a single sprite, placed by its centre
    else:return None,'cena %s ignorada: %s não é uma composição nem um sprite'%(name,p['image'])
    # PNG, PVR and WebP sheets can be read here.
    have=set(z.namelist());bases={where[q['name']][0] for q in entry['sprites']}
    bad=sorted(b.split('/')[-1] for b in bases if b+'.png' not in have and b+'.pvr' not in have and b+'.webp' not in have)
    if bad:return None,'cena %s ignorada: formato de imagem sem suporte (%s)'%(name,bad[0])
    right,bottom=extent(where,entry)
    if p['vertical']:
        # fitwidth: scaled to the screen width, scrolling up or down.
        w=p['ref_w'];h=min(p['ref_h'],bottom) if bottom else p['ref_h']
        canvas=compose(z,where,entry,w,h,p['bg'])
        small,sw,sh=scale(canvas,w,h,SCREEN_W/w)
    else:
        # The script's reference width can be a little wider than what the pieces cover:
        # the real extent is used, so no empty strip is scrolled into view.
        w=min(p['ref_w'],right) if right else p['ref_w'];h=p['ref_h']
        canvas=compose(z,where,entry,w,h,p['bg'])
        small,sw,sh=scale(canvas,w,h,SCREEN_H/h)
        if sw<SCREEN_W:small=pad_width(small,sw,sh,SCREEN_W);sw=SCREEN_W
    options=(1 if p['vertical'] else 0)|(2 if p['reverse'] else 0)
    ov=b'';ovs=[]
    if not p['vertical']:ovs=[o for o in overlays(name,where,z,SCREEN_H/h) if o[2]<sw and o[2]+o[4]>0]   # off the picture: not drawn
    if ovs:
        options|=8;ov=struct.pack('<I',len(ovs))
        for t0,t1,x,y,ow,oh,px in ovs:ov+=struct.pack('<8i',t0,t1,x,y,ow,oh,0,0)+px
    # Scroll segments: where the right edge of the screen (the scroll cursor) ends, as a fraction of the picture
    # width, becomes a fraction of the scroll range in thousandths. Vertical scenes scroll over their whole height.
    segs=[(0.0,0.0,1.0)] if not p['segments'] else p['segments']
    if p['vertical']:segs=[(p['scroll_start'],p['scroll'],1.0)]
    if len(segs)>2:return None,'cena %s ignorada: mais de duas rolagens'%name
    rng=sw-SCREEN_W
    def permille(t):return 1000 if p['vertical'] else (0 if rng<=0 else max(0,min(1000,round((t*sw-SCREEN_W)/rng*1000))))
    a=segs[0];b=segs[1] if len(segs)>1 else (0.0,0.0,0.0)
    seg=(int(a[0]*1000),int(a[1]*1000),permille(a[2]),int(b[0]*1000),int(b[1]*1000),permille(b[2]) if len(segs)>1 else 0)
    return (name,sw,sh,seg[0],seg[1],int(p['end']*1000),p['music'],options,to5551(small)+ov,seg[2],seg[3],seg[4],seg[5]),'cena %s pronta (%d x %d)'%(name,sw,sh)

def extract(apk,out,names=None,workers=None,log=print):
    """CUTSCENE.BIN: "PBCS", version 5, scene count, then per scene a 24-byte name (NUL padded),
    width, height, scroll start ms, scroll ms, end ms, data offset, music track (-1: none),
    options (bit 0: vertical scroll, bit 1: starts at the far end), the end of the first scroll in thousandths
    of the scroll range and a second scroll (start ms, ms, end in thousandths) and the RGBA5551 rows. A
    horizontal scene is 448 rows tall and at least 640 wide; a vertical one is 640 wide.
    The scenes are independent, so they are built in parallel (one worker per core, at most 6); the result
    is the same as building them one after the other."""
    names=list(names or SCENES);blobs=[]
    n=workers if workers is not None else paralelo.processos(len(names))
    results=None
    if n>1:
        try:
            import multiprocessing
            pool=multiprocessing.get_context('spawn').Pool(n,_init,(str(apk),))
            log('    quadrinhos: %d cenas em %d processos paralelos'%(len(names),n))
            results=pool.imap(build_scene,names)
        except Exception as e:
            log('    (processamento em paralelo indisponível: %s; seguindo em um processo)'%e);results=None
    if results is None:
        _setup(str(apk));results=map(build_scene,names);pool=None
    for k,(blob,msg) in enumerate(results,1):
        log('    [%d/%d] %s'%(k,len(names),msg))
        if blob:blobs.append(blob)
    if pool:pool.close();pool.join()
    head=struct.pack('<4s2I',b'PBCS',5,len(blobs));offset=len(head)+72*len(blobs);offset+=(-offset)%64
    table=b'';pos=offset
    for name,sw,sh,st,sc,en,mu,op,px,t1,st2,sc2,t2 in blobs:table+=struct.pack('<24s6IiI4i',name.encode()[:23],sw,sh,st,sc,en,pos,mu,op,t1,st2,sc2,t2);pos+=len(px)+(-len(px))%64
    data=bytearray(head+table);data+=bytes(offset-len(data))
    for b in blobs:px=b[8];data+=px+bytes((-len(px))%64)
    Path(out).write_bytes(data)
    return {'scenes':len(blobs),'bytes':len(data),'sizes':sorted({(b[1],b[2]) for b in blobs})}

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk');p.add_argument('output')
    a=p.parse_args();print(extract(a.apk,a.output))
