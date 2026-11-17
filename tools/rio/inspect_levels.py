import os
import sys,json,collections
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];base=ROOT;sys.path.insert(0,str(base/'tools'))
from lua51_data import load
root=ROOT/'work/rio/analysis';levels=[];errors=[];defs=collections.Counter();fields=collections.Counter();joints=collections.Counter()
for p in sorted((root/'chunks/assets/data/levels').rglob('*.lua')):
 try:
  d=load(p.read_bytes());world=d.get('world',{});objects=[v for v in world.values() if isinstance(v,dict) and 'definition' in v];counts=collections.Counter(o['definition'] for o in objects);defs.update(counts);fields.update(d.keys())
  for v in d.get('joints',{}).values():
   if isinstance(v,dict):joints.update([str(v.get('type',v.get('definition','unknown')))])
  levels.append({'path':str(p.relative_to(root/'chunks')),'objects':len(objects),'definitions':dict(counts),'globals':list(d),'joint_entries':len(d.get('joints',{}))})
  out=root/'levels'/p.parent.name/(p.stem+'.json');out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(d))
 except Exception as e:errors.append({'path':str(p),'error':str(e)})
profile=json.loads((base/'profiles/classic_level.json').read_text());known=set(profile['objects'])|set(profile['bird_definitions'])|{'Ground'}
result={'read_levels':len(levels),'errors':errors,'definitions':dict(defs),'common_definitions':sorted(set(defs)&known),'rio_only_definitions':sorted(set(defs)-known),'fields':dict(fields),'joint_types':dict(joints),'levels':levels}
(root/'levels_inventory.json').write_text(json.dumps(result,indent=2))
print('Levels',len(levels),'failures',len(errors),'unique defs',len(defs),'known names',len(set(defs)&known),'new names',len(set(defs)-known));print('first errors',errors[:3]);print('top defs',defs.most_common(20));print('new defs',sorted(set(defs)-known));print('globals',dict(fields))
