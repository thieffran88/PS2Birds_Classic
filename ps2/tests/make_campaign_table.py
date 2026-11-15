#!/usr/bin/env python3
"""Host helper: rebuild the stage table of tests/campaign_test.cpp from solver output.

Reads ps2/build/all_*.txt (single-shot wins) and ps2/build/nb*_<index>.txt (beam wins);
stages without a win get an empty script and are exercised as unverified.
"""
import json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];B=ROOT/'ps2/build'
shots={}
for f in B.glob('all_*.txt'):
    for m in re.finditer(r'level (\d+) .*?: WIN angle=(-?\d+) power=(\d+) special=(-?[\d.]+)',f.read_text()):shots[int(m[1])]=[(m[2],m[3],float(m[4]))]
for f in sorted(B.glob('nb*_*.txt')):
    last=f.read_text().strip().splitlines()[-1:] or ['']
    if last[0].startswith('WIN'):
        t=last[0].split()[1:-1];shots[int(f.stem.split('_')[-1])]=[(t[i],t[i+1],float(t[i+2])) for i in range(0,len(t),3)]
episode=json.loads((ROOT/'profiles/classic_episode.json').read_text())
pages=[(1,episode['levels'])]+[(p['number'],p['levels']) for p in episode['additional_pages']]
rows=[];index=0;missing=[]
for page,levels in pages:
    for n in range(1,len(levels)+1):
        d=json.loads((ROOT/f'ps2/data/level_{page}_{n}.json').read_text())
        waved=any(o.get('wave') for o in d['objects']);pigs=sum(bool(o['kind']==4 and not o.get('spawn_of') and (o.get('wave') or not waved)) for o in d['objects']);blocks=sum(o['kind'] not in (3,4) and not o.get('gun_bubble') and not o.get('cannon_ball') for o in d['objects'])
        if index in shots:sh='{'+','.join('{%s,%s,%.1ff}'%s for s in shots[index])+'}';c=len(shots[index])
        else:sh,c='{{0,0,-1.0f}}',0;missing.append(d['title'])
        rows.append(f' {{"{d["stage_script"]}",{len(d["objects"])},{pigs},{blocks},{len(d["birds"])},{sh},{c}}}, /* {d["title"]}{" (sem roteiro vencedor)" if not c else ""} */')
        index+=1
p=ROOT/'ps2/tests/campaign_test.cpp';s=p.read_text()
start=s.index('static const Stage stages[]={\n')+len('static const Stage stages[]={\n');end=s.index('\n};\n#define STAGES')
s=s[:start]+'\n'.join(rows)+s[end:]
s=re.sub(r'assert\(unverified==\d+\);',f'assert(unverified=={len(missing)});',s)
p.write_text(s);print(len(rows),'stages,',len(missing),'without script:',', '.join(missing))
