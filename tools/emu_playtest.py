#!/usr/bin/env python3
"""Plays stages in PCSX2 with the game's own autoplay (ps2/src/main.c: an AUTOPLAY.TXT on the disc) and reports what happened.

For each chosen stage: builds a test ISO (ELF + the data files + AUTOPLAY.TXT), runs PCSX2 headless (-batch, -logfile), waits for the
"AUTOPLAY DONE" line of the game's console output, takes a screenshot of the opening view and of the end of the run, and writes
report.md / report.json in the output folder. It is a smoke/play test, not a solver: the shots are a fixed list of angles and powers,
so a stage that is not won only shows that it loads, settles, takes the shots and ends without a crash.

usage: emu_playtest.py --game classic|rio --elf ELF --data DATA_DIR --pcsx2 PCSX2-QT --out OUT_DIR
                       [--levels episodes | 1,181,200] [--shots 3] [--timeout 300] [--display :91]
DATA_DIR holds SPRITES.BIN SOUNDS.BIN MUSIC.BIN UI.BIN FONT.BIN MENU.BIN THEMES.BIN CUTSCENE.BIN CAMPAIGN.BIN (ps2/assets or what the generator made).
Needs: pycdlib (as tools/make_iso.py), an X display (Xvfb :91 works), ImageMagick `import` for the screenshots and, optionally, xdotool (closes PCSX2's audio-device dialog before each screenshot)."""
import argparse,json,os,re,shutil,subprocess,sys,tempfile,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
import make_iso,build_campaign
FILES=['SPRITES.BIN','SOUNDS.BIN','MUSIC.BIN','UI.BIN','FONT.BIN','MENU.BIN','THEMES.BIN','CUTSCENE.BIN','CAMPAIGN.BIN']
DISC={'classic':'SLUS_000.10','rio':'SLUS_000.11'}
def episode_levels(campaign):
    """(1-based stage number, title) of the first stage of each episode: the title without its trailing number."""
    first={}
    for i,l in enumerate(build_campaign.read_binary(campaign)['levels']):
        name=re.sub(r'\s*\d+(-\d+)?$','',l['title']).strip()
        first.setdefault(name,(i+1,l['title']))
    return list(first.values())
def screenshot(display,path,crop,dismiss=False):
    env=dict(os.environ,DISPLAY=display)
    if dismiss and shutil.which('xdotool'):   # PCSX2 without an audio device shows an "audio stream" dialog over the picture: Enter closes it
        subprocess.run(['xdotool','key','Return'],env=env,capture_output=True);time.sleep(0.5)
    cmd=['import','-window','root']+(['-crop',crop] if crop else [])+[str(path)]
    try:subprocess.run(cmd,env=env,check=True,capture_output=True,timeout=30);return True
    except Exception:return False
def wait_for(log,pattern,deadline):
    while time.time()<deadline:
        try:
            text=Path(log).read_text(errors='replace')
            m=re.search(pattern,text)
            if m:return m
        except FileNotFoundError:pass
        time.sleep(1.0)
    return None
def play(args,number,title,out):
    work=Path(tempfile.mkdtemp(prefix='ps2birds-play-'))
    (work/'AUTOPLAY.TXT').write_text('level %d\nshots %d\nwait 6\n'%(number,args.shots))
    extras=[Path(args.data)/f for f in FILES]+[work/'AUTOPLAY.TXT']
    iso=work/'test.iso';make_iso.write_with_pycdlib(Path(args.elf).read_bytes(),iso,extras,DISC[args.game])
    log=work/'emulog.txt';env=dict(os.environ,DISPLAY=args.display)
    proc=subprocess.Popen([args.pcsx2,'-batch','-fastboot','-logfile',str(log),'--',str(iso)],env=env,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    row={'level':number,'title':title,'result':None,'screens':[]}
    try:
        deadline=time.time()+args.timeout
        if wait_for(log,r'AUTOPLAY start level',deadline):
            time.sleep(4);shot=out/('level%04d_open.png'%number)
            if screenshot(args.display,shot,args.crop,dismiss=True):row['screens'].append(shot.name)   # Enter only once: a second one would reach the game (Start = pause)
        m=wait_for(log,r'AUTOPLAY RESULT ([^\n]*)',deadline)
        if m:
            row['result']=m.group(1).strip();time.sleep(0.5);shot=out/('level%04d_end.png'%number)
            if screenshot(args.display,shot,args.crop):row['screens'].append(shot.name)
        else:
            text=Path(log).read_text(errors='replace') if log.exists() else ''
            row['result']='FAIL: '+('could not load' if 'AUTOPLAY FAIL' in text else 'no result before the timeout (crash, hang or too slow)')
        shutil.copy(log,out/('level%04d_log.txt'%number)) if log.exists() else None
    finally:
        proc.terminate()
        try:proc.wait(timeout=15)
        except subprocess.TimeoutExpired:proc.kill()
        shutil.rmtree(work,ignore_errors=True)
    return row
def main():
    p=argparse.ArgumentParser(description=__doc__,formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--game',choices=['classic','rio'],required=True);p.add_argument('--elf',required=True);p.add_argument('--data',required=True)
    p.add_argument('--pcsx2',required=True);p.add_argument('--out',required=True);p.add_argument('--levels',default='episodes')
    p.add_argument('--shots',type=int,default=3);p.add_argument('--timeout',type=int,default=300);p.add_argument('--display',default=':91')
    p.add_argument('--crop',default='830x625+110+20',help="screenshot rectangle (the PCSX2 window in the Xvfb screen); '' for the whole screen")
    args=p.parse_args();out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    camp=Path(args.data)/'CAMPAIGN.BIN';titles={i+1:l['title'] for i,l in enumerate(build_campaign.read_binary(camp)['levels'])}
    targets=episode_levels(camp) if args.levels=='episodes' else [(int(x),titles.get(int(x),'?')) for x in args.levels.split(',')]
    rows=[]
    for number,title in targets:
        print('stage %d (%s) ...'%(number,title),flush=True);row=play(args,number,title,out);rows.append(row);print('   ',row['result'],flush=True)
    (out/'report.json').write_text(json.dumps(rows,indent=1))
    lines=['# Emulator playtest (%s)'%args.game,'','| Stage | Title | Result | Screenshots |','|---|---|---|---|']
    for r in rows:lines.append('| %d | %s | %s | %s |'%(r['level'],r['title'],r['result'],', '.join(r['screens'])))
    ok=sum(1 for r in rows if r['result'] and not r['result'].startswith('FAIL'))
    lines+=['','%d of %d stages ran to the end.'%(ok,len(rows))]
    (out/'report.md').write_text('\n'.join(lines)+'\n');print('\n'.join(lines[-1:]))
    return 0 if ok==len(rows) else 1
if __name__=='__main__':sys.exit(main())
