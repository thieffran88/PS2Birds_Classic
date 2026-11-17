"""Audit the supported Rio APK as data. No Lua runtime, Android library or PS2 executable is run."""
import argparse,hashlib,json,os,sys,zipfile,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from inspect_apk import recover_lua
from lua51_data import load,Unsupported
SHA=json.loads((ROOT/'profiles/projects.json').read_text())['rio']['sha256']
def investigate(apk):
 apk=Path(apk).resolve();digest=hashlib.sha256(apk.read_bytes()).hexdigest()
 if digest!=SHA:raise ValueError('Unsupported APK SHA-256; this audit is for Rio 2.6.13 only')
 out=ROOT/'work/rio/analysis';out.mkdir(parents=True,exist_ok=True);recovered=[];tables=[];refused=[]
 with zipfile.ZipFile(apk) as z:
  if z.testzip():raise ValueError('APK ZIP CRC failure')
  for name in z.namelist():
   if not name.endswith('.lua'):continue
   p=Path(name)
   if p.is_absolute() or '..' in p.parts:raise ValueError('Unsafe archive path')
   data=recover_lua(z.read(name),None,'rio');dest=out/'chunks'/p;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data)
   recovered.append({'path':name,'bytes':len(data),'header':data[:12].hex()})
   if name.startswith('assets/data/levels/'):continue
   try:d=load(data)
   except Unsupported as e:refused.append({'path':name,'reason':str(e)});continue
   dest=out/'data_tables'/p.with_suffix('.json');dest.parent.mkdir(parents=True,exist_ok=True);dest.write_text(json.dumps(d));tables.append({'path':name,'globals':list(d)})
 (out/'recovery.json').write_text(json.dumps({'sha256':digest,'recovered':recovered,'errors':[]},indent=2))
 (out/'data_tables_inventory.json').write_text(json.dumps({'parsed':tables,'not_data':refused},indent=2))
 env=dict(os.environ,RIO_APK_PATH=str(apk));logs=[]
 for script in ['inspect_levels.py','inspect_dat.py','inspect_comp.py','build_catalogue.py','audit_gaps.py']:
  p=subprocess.run([sys.executable,str(ROOT/'tools/rio'/script)],env=env,capture_output=True,text=True)
  logs.append('### '+script+'\n'+p.stdout+p.stderr)
  if p.returncode:(out/'investigation.log').write_text('\n'.join(logs));raise RuntimeError(script+' failed')
 (out/'investigation.log').write_text('\n'.join(logs))
 print('Recovered',len(recovered),'Lua chunks; data-only script tables',len(tables),'; non-data scripts safely refused',len(refused))
 print('Audit reports generated in',out)
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);a=p.parse_args();investigate(a.apk)
