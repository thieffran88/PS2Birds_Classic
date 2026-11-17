"""Sound groups of the Rio stages (e.g. `wood_collision` -> its variants `wood collision a1`, `a2`, ...).
The groups are built in code in gamelogic.lua, so they cannot be read as data: the string constants of the
function that builds them list each group name followed by its variants, which are aliases of the sound
profile (audio/profiles/default.lua, read as data). A group is a name that is not a profile alias, followed
by the aliases that come right after it.
usage: sound_groups.py  -> work/rio/analysis/sound_groups.json and a summary"""
import json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
from lua51_data import load
CH=ROOT/'work/rio/analysis/chunks/assets/data'
WANTED=['cage_collision','cage_destroyed','cardboard_collision','cardboard_destroyed','chain_collision','chain_destroyed','collectableFruit','fruit_collision','fruit_destroyed','ground_collision','lamp breaking','lamp_collision','light_collision','light_damage','light_destroyed','light_rolling','rock_collision','rock_damage','rock_destroyed','rock_rolling','rubber_collision','wood damage a1','wood_collision','wood_damage','wood_destroyed','wood_rolling']
def groups():
    prof=load((CH/'audio/profiles/default.lua').read_bytes())['sound_profile']
    data=(CH/'scripts/gamelogic.lua').read_bytes()
    start=data.find(b'cage_collision');a=data.rfind(b'falling a4',0,start)
    consts=[c.decode() for c in re.findall(rb"[A-Za-z0-9_' ]{4,}",data[a:a+9000])]
    out={};cur=None
    for c in consts:
        if c in prof:
            if cur:out[cur].append(c)
        elif re.fullmatch(r'[a-z][A-Za-z0-9]*(_[A-Za-z0-9]+)+',c):cur=c;out.setdefault(cur,[])
        else:cur=None if c.startswith(('tick','pairs')) else cur
    out={g:v for g,v in out.items() if v}
    # Groups the code does not list: the variants are the profile aliases named "<group with spaces> aN"
    # (wood_collision -> "wood collision a1", "a2", ...); the engine builds the name the same way.
    for w in WANTED:
        if w not in out and w not in prof:
            v=sorted(k for k in prof if re.fullmatch(re.escape(w.replace('_',' '))+r' a\d+',k))
            if v:out[w]=v
    return out,prof
if __name__=='__main__':
    g,prof=groups();(ROOT/'work/rio/analysis/sound_groups.json').write_text(json.dumps(g,indent=1))
    want=WANTED
    _unused=['cage_collision','cage_destroyed','cardboard_collision','cardboard_destroyed','chain_collision','chain_destroyed','collectableFruit','fruit_collision','fruit_destroyed','ground_collision','lamp breaking','lamp_collision','light_collision','light_damage','light_destroyed','light_rolling','rock_collision','rock_damage','rock_destroyed','rock_rolling','rubber_collision','wood damage a1','wood_collision','wood_damage','wood_destroyed','wood_rolling']
    ok=[w for w in want if w in g or w in prof]
    print('grupos',len(g),'| dos 26 sons da primeira fase resolvidos:',len(ok),'| sem grupo:',[w for w in want if w not in ok])
