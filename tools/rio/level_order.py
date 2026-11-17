"""Order of the levels of each Rio pack, read from the string constants of gamelogic.lua.
The levelOrder table is built in code, so the generator cannot read it as data; the constants of
the function that builds it list the level names of each pack in table order (a constant that
repeats keeps its first place). Each name is checked against the folders of assets/data/levels:
the order is accepted only when every pack comes out as one run holding exactly its files."""
import json,re,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
CH=ROOT/'work/rio/analysis/chunks/assets/data'
def level_order():
    data=(CH/'scripts/gamelogic.lua').read_bytes()
    folders={d.name:sorted(p.stem for p in d.glob('*.lua')) for d in sorted((CH/'levels').iterdir()) if d.is_dir()}
    where={}
    for f,names in folders.items():
        for n in names:where.setdefault(n,[]).append(f)
    start=data.find(b'Level135')
    # the pack list sits before the levels: start at the 'episodes' constant that precedes them
    a=data.rfind(b'episodes',0,start);consts=re.findall(rb"[A-Za-z0-9_']{2,}",data[a:a+20000])
    seen=[];out={}
    for c in consts:
        n=c.decode()
        if n in where and n not in seen:seen.append(n)
    # cut the sequence into packs: a name belongs to the folder (or folders) that hold it
    for n in seen:
        for f in where[n]:out.setdefault(f,[]).append(n)
    return {f:out.get(f,[]) for f in folders},folders
if __name__=='__main__':
    order,folders=level_order();bad=0
    for f,names in order.items():
        ok=sorted(names)==folders[f]
        if not ok:bad+=1
        print('%-20s %3d/%3d %s'%(f,len(names),len(folders[f]),'ok' if ok else 'DIFERE'))
    out=ROOT/'work/rio/analysis/level_order.json';out.write_text(json.dumps(order,indent=1));print('gravado',out,'| packs que diferem:',bad)
