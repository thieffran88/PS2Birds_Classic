#!/usr/bin/env python3
"""Add each piece's destroyedScoreInc (points when destroyed) from a decompiled blocks.lua to
profiles/classic_level.json ("score" next to "defaults"). Pieces without one keep the default
(blockDestroyedScoreIncrement, 500; pigs 5000). Lua is only read, never executed.

Uso: python3 tools/add_scores.py blocks.lua [profile.json]
"""
import json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def scores(blocks):
    out={};name=None
    lines=blocks.split('\n')
    for i,l in enumerate(lines):
        m=re.match(r'\s*L3_1 = "([A-Za-z0-9_]+)"$',l)
        if m:name=m[1]
        if '"destroyedScoreInc"' in l and name:
            for j in range(i+1,i+4):
                v=re.match(r'\s*L6_1 = (-?[\d.]+)',lines[j])
                if v:out[name]=float(v[1]);break
    return out
if __name__=='__main__':
    table=scores(Path(sys.argv[1]).read_text());path=Path(sys.argv[2]) if len(sys.argv)>2 else ROOT/'profiles/classic_level.json'
    prof=json.loads(path.read_text());n=0
    for k,v in prof['objects'].items():
        if k in table:v['score']=table[k];n+=1
        else:v.pop('score',None)
    path.write_text(json.dumps(prof,indent=2)+'\n');print(n,'of',len(prof['objects']),'definitions carry a score;',len(table)-n,'definitions of blocks.lua are not in the profile')
