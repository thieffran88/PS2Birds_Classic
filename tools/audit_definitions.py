#!/usr/bin/env python3
"""Audit of the piece/bird definitions: which definitions the levels of the APK use, which of them have an
adapter in profiles/classic_level.json, and which levels (by folder) need the ones without.

Usage: python3 tools/audit_definitions.py com.rovio.angrybirds.apk [blocks.lua]
With a decompiled blocks.lua (unluac output) it also counts the definitions that no level uses. Lua is only read, never executed."""
import collections,glob,json,re,sys,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from game_profiles import decode_payload
from lua51_data import load
def definitions_of(node,out):
    if isinstance(node,dict):
        if isinstance(node.get('definition'),str):out.add(node['definition'])
        for v in node.values():definitions_of(v,out)
    elif isinstance(node,list):
        for v in node:definitions_of(v,out)
def main(apk,blocks=None):
    z=zipfile.ZipFile(apk);prof=json.loads((ROOT/'profiles/classic_level.json').read_text())
    have=set(prof['objects'])|set(prof['bird_definitions'])|{'Ground'}
    imported={json.loads(Path(f).read_text())['stage_script'] for f in glob.glob(str(ROOT/'ps2/data/level_*.json'))}
    used_in=collections.defaultdict(set);missing=collections.defaultdict(lambda:collections.defaultdict(set));levels=collections.Counter()
    for n in z.namelist():
        if not (n.startswith('assets/data/levels/') and n.endswith('.lua')):continue
        script=n.split('/')[-1][:-4];folder=n.split('/')[3] if n.count('/')>3 else n.split('/')[2];ported=script in imported
        d=set();definitions_of(load(decode_payload(z.read(n),'classic',None)),d);levels[(ported,folder)]+=1
        for df in d:
            used_in[df].add(ported)
            if df not in have and not df.startswith('StaticBlockTheme'):missing[ported][df].add(folder)
    print('levels: %d (%d imported into the port)'%(sum(levels.values()),sum(v for (p,_),v in levels.items() if p)))
    print('definitions used by the imported stages without an adapter: %d'%len(missing[True]))
    for df,f in sorted(missing[True].items()):print('  ',df,sorted(f))
    print('definitions used only by stages the port does not import, without an adapter: %d'%len([d for d in missing[False] if d not in missing[True]]))
    by_folder=collections.defaultdict(set)
    for df,f in missing[False].items():
        for x in f:by_folder[x].add(df)
    for f,v in sorted(by_folder.items()):print('  ',f,len(v))
    if blocks:
        t=Path(blocks).read_text();defs=set(re.findall(r'\nL3_1 = "([A-Za-z0-9_]+)"\nL4_1 = {}\n',t))
        unused=sorted(d for d in defs if d not in have and d not in used_in)
        print('definitions of blocks.lua: %d; without an adapter: %d; of these, no level uses %d'%(len(defs),len([d for d in defs if d not in have]),len(unused)))
    return 0 if not missing[True] else 1
if __name__=='__main__':sys.exit(main(sys.argv[1],sys.argv[2] if len(sys.argv)>2 else None))
