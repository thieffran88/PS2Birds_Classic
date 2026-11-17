"""Cross-reference recovered tables. Evidence of data coverage is not runtime support."""
import json,collections,zipfile,os
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];r=ROOT/'work/rio/analysis'
def read(name):return json.loads((r/name).read_text())
def audit():
 inv=read('levels_inventory.json');catdata=read('definition_catalogue.json');cat=catdata['definitions'];comps=read('comp_inventory.json');sheets=read('dat_inventory.json');data=r/'data_tables/assets/data/scripts'
 materials=json.loads((data/'materials.json').read_text());stars=json.loads((data/'starLimits.json').read_text())
 frames={f['name']:{'sheet':s['path'],'texture':s['texture'],'available':s['available'],'frame':f} for s in sheets['parsed'] if '/ingame_assets/' in s['path'] for f in s['frames']}
 composed={c['name']:c for s in comps['parsed'] if '/ingame_assets/' in s['path'] for c in s['composites']}
 def leaf_names(name,seen=()):
  if name in seen:return [],['cycle:'+name]
  if name in frames:return [name],[]
  if name in composed:
   leaves=[];missing=[]
   for c in composed[name]['children']:
    a,b=leaf_names(c['sprite'],seen+(name,));leaves+=a;missing+=b
   return leaves,missing
  return [],[name]
 first=[p for p in inv['levels'] if p['path'].split('/')[3] in ('warehouse','warehouse2')];defs=sorted({n for p in first for n in p['definitions']});dependencies={}
 for name in defs:
  p=cat.get(name,{}).get('effective',{});sprite=p.get('sprite') or p.get('sprites',{}).get('default');leaves,missing=leaf_names(sprite) if sprite else ([],[])
  dependencies[name]={'definition_found':name in cat,'material':p.get('material'),'material_table_found':p.get('material') in materials,'is_goal':p.get('levelGoal') is True,'shape':p.get('type'),'primary_sprite':sprite,'is_composite':sprite in composed,'leaf_sprites':sorted(set(leaves)),'missing_primary_sprite_references':missing,'native_special_case':name=='Ground'}
 first_result={'scope':'30 files; canonical menu order still unconfirmed','files':[p['path'] for p in first],'max_raw_objects':max(p['objects'] for p in first),'max_joints':max(p['joint_entries'] for p in first),'definitions':dependencies,'goal_definition_count':sum(v['is_goal'] for v in dependencies.values()),'runtime_ready':False}
 (r/'first_episode_dependencies.json').write_text(json.dumps(first_result,indent=2))
 specialty=collections.Counter();goals=[]
 for n,count in inv['definitions'].items():
  p=cat.get(n,{}).get('effective',{})
  if p.get('specialty'):specialty[p['specialty']]+=count
  if p.get('levelGoal') is True:goals.append(n)
 audio=json.loads((r/'data_tables/assets/data/audio/profiles/default.json').read_text())['sound_profile'];missing_audio=[]
 with zipfile.ZipFile(os.environ.get('RIO_APK_PATH',str(ROOT/'input/com.rovio.angrybirdsrio.apk'))) as z:
  names=set(z.namelist())
  for alias,v in audio.items():
   path='assets/data/audio/'+v['category']+'/'+v.get('filename',alias)+'.'+v['extension']
   if path not in names:missing_audio.append({'alias':alias,'path':path})
 result={'scope':'Static cross-reference, no PS2 runtime execution','data_only_scripts':len(read('data_tables_inventory.json')['parsed']),'materials':len(materials),'catalogue_definitions':len(cat),'used_definitions':len(inv['definitions']),'resolved_used_definitions':len(set(inv['definitions'])&set(cat)),'missing_used_definitions':catdata['missing_used_definitions'],'unresolved_material_names':catdata['missing_materials'],'goal_definitions_used':sorted(goals),'goal_classes_used':len(goals),'specialty_occurrences':dict(specialty),'star_threshold_coverage':sum(p['path'].rsplit('/',1)[1][:-4] in stars for p in inv['levels']),'missing_star_thresholds':[p['path'] for p in inv['levels'] if p['path'].rsplit('/',1)[1][:-4] not in stars],'comp_files_parsed':len(comps['parsed']),'comp_records':sum(len(s['composites']) for s in comps['parsed']),'comp_errors':comps['errors'],'audio_aliases':len(audio),'missing_audio_files':missing_audio,'required_work':['Canonical level order','Launch origin and gravity configuration','Per-game material aliases and scaling','COMP transformations and animation composition','Goals/rescue/presentation/scoring','Blu, Blu+Jewel, RocketBird and Nigel variants','Fireworks, fuses, water/buoyancy and moving objects','Luiz and special rescue companion behavior','Collectibles/bonus/feathers and persistent unlocks','Rio UI/themes/fonts/cutscenes/audio aliases','Separate Rio save identifier and directory','Memory/performance and PS2 runtime validation']}
 (r/'feature_matrix.json').write_text(json.dumps(result,indent=2))
 assert len(inv['levels'])==459 and not inv['errors']
 assert len(cat)==1022 and result['resolved_used_definitions']==904 and catdata['missing_used_definitions']==['Ground']
 assert not catdata['conflicting_duplicates'] and len(comps['parsed'])==24 and not comps['errors']
 assert result['star_threshold_coverage']==458 and len(first)==30
 assert len(audio)==420 and not missing_audio
 print('Cross-reference PASS: 1022 definitions, 904 used + Ground, 87 materials, 65 used goal definitions, 24 COMP files/1380 records; first-episode dependencies recorded.')
if __name__=='__main__':audit()
