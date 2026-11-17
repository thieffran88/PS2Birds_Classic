import os
from pathlib import Path
import json,collections
r=Path(__file__).resolve().parents[2]/"work/rio/analysis"
names=['birds','levelgoals','scoreobjects','pigs','blocks_glass','blocks_misc','blocks_stone','blocks_staticfragile','blocks_decorations','blocks_static','blocks_misc2','blocks_rocks','blocks_misc3','blocks_pyrotechnics','blocks_hazard','blocks_wood']
cat={};duplicates=[]
for name in names:
 data=json.loads((r/f'data_tables/assets/data/scripts/{name}.json').read_text())
 for root in data.values():
  if not isinstance(root,dict):continue
  for v in ([root]+list(root.values())):
   if not isinstance(v,dict) or not isinstance(v.get('definition'),str):continue
   key=v['definition']
   if key in cat and cat[key]['properties']!=v:duplicates.append(key)
   cat[key]={'source':name+'.lua','properties':v}
used=json.loads((r/'levels_inventory.json').read_text())['definitions'];materials=json.loads((r/'data_tables/assets/data/scripts/materials.json').read_text())
missing=sorted(set(used)-set(cat));resolved={};missing_materials=[]
for key,item in cat.items():
 p=item['properties'];material=p.get('material');m=materials.get(material,{})
 if material and material not in materials:missing_materials.append((key,material))
 resolved[key]={'source':item['source'],'material':material,'effective':{**m,**p},'level_occurrences':used.get(key,0)}
(r/'definition_catalogue.json').write_text(json.dumps({'definitions':resolved,'missing_used_definitions':missing,'conflicting_duplicates':duplicates,'missing_materials':missing_materials},indent=2))
print('catalogue',len(cat),'resolved used',len(set(used)&set(cat)),'missing',missing,'conflicts',duplicates,'missing materials',missing_materials[:5]);print('used goal definitions',sum(resolved[k]['effective'].get('levelGoal') is True for k in used if k in resolved));print('birds',[(v['effective'].get('definition'),v['effective'].get('specialty')) for v in resolved.values() if v['source']=='birds.lua'])
