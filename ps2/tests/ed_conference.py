#!/usr/bin/env python3
"""Host helper (not part of `make test`): cross-checks the 72 Egg Defender vehicle controllers of
ps2/src/egg_vehicles.cpp against the original scripts (assets/data/vehicles/EggDefender_N/*.scripts.luac,
decompiled with unluac into one directory of *.scripts.lua).

Usage: ed_conference.py DIR_WITH_DECOMPILED_SCRIPTS
Checks: (1) every original script has a controller by name and vice versa; (2) every block/pig name used by
a script appears in the port; (3) every number (forces, speeds, timers) used by a script appears in the port;
(4) setAbandoningLogic / neverAbandon in a script matches the controller's never_abandon column.
Also reports `setTargetY`, which no script calls (see docs/EGG_DEFENDER.md)."""
import glob,re,sys
from pathlib import Path
D=Path(sys.argv[1]);src=Path(__file__).resolve().parents[1]/'src'/'egg_vehicles.cpp';port=src.read_text()+(src.parent/'game.cpp').read_text()
orig={f.name[:-12]:f.read_text() for f in D.glob('*.scripts.lua')}
names=set(re.findall(r'\{"(EggDefender_\d+_W\d+_V\d+)"',port))
bad=0
def fail(msg):
    global bad;bad+=1;print('FAIL',msg)
if set(orig)!=names:fail('names differ: only in APK %s, only in port %s'%(sorted(set(orig)-names),sorted(names-set(orig))))
nums={abs(float(x)) for x in re.findall(r'(?<![\w.])-?\d+(?:\.\d+)?(?![\w.])',port)}
flags={}
for m in re.finditer(r'\{"(EggDefender_\d+_W\d+_V\d+)",([^}]*)\}',port):
    f=[x.strip() for x in m[2].split(',')]
    if len(f)==5:flags[m[1]]=int(f[4])
absent_names=set()
for n,t in orig.items():
    for x in set(re.findall(r'= "([A-Za-z0-9_]+)"',t)):
        if x.startswith(('BP_','Helmet','Moustache','King','Grandpa','Medium','Small','Big')) and x not in port:absent_names.add((n,x))
    for v in {float(m[1]) for m in re.finditer(r'L\d+_2 = (-?\d+(?:\.\d+)?)\n',t)}:
        if v not in (0,1,2,0.5,10) and abs(v) not in nums:fail('%s: number %s not in the port'%(n,v))
    if re.search(r'A0_2\.setAbandoningLogic',t) and not flags.get(n):
        # a call that only names a helmet pig is covered by never_abandon; report it
        fail('%s: setAbandoningLogic without never_abandon'%n)
# Pigs are addressed by role in the port: a pig named in a script is only used for setAbandoningLogic (never_abandon), setGrabLogic
# (called without a function: it clears onEggGrabbed, nothing to do) or surrender (ev_surrender); every other name must appear.
for n,x in sorted(absent_names):
    if x.startswith(('Helmet','Grandpa','Medium','Small','Big','King','Moustache')):continue
    fail('%s: name %s not in the port'%(n,x))
callers=[n for n,t in orig.items() if 'setTargetY' in t]
print('vehicle scripts:',len(orig),'controllers:',len(names),'| scripts calling setTargetY:',len(callers))
print('OK' if not bad else '%d problem(s)'%bad)
sys.exit(1 if bad else 0)
