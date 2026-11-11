#!/usr/bin/env python3
"""Probe an APK's script format. Successful recovery does not establish a port."""
import argparse,hashlib,json,lzma,shutil,zipfile
from pathlib import Path
from game_profiles import PROFILES
from inspect_apk import recover_lua

def check(apk,game,sevenzip,limit):
    sha=hashlib.sha256()
    with apk.open('rb') as stream:
        for chunk in iter(lambda:stream.read(1024*1024),b''):sha.update(chunk)
    with zipfile.ZipFile(apk) as archive:
        names=archive.namelist();scripts=sorted(n for n in names if n.endswith('.lua'))
        recovered=[];errors=[]
        for name in scripts[:limit]:
            try:
                data=recover_lua(archive.read(name),sevenzip,game)
                recovered.append({'path':name,'lua_version_hex':f'{data[4]:02x}','header_hex':data[:12].hex()})
            except (ValueError,RuntimeError,lzma.LZMAError) as error:errors.append({'path':name,'error':str(error)})
        return {'file':apk.name,'sha256':sha.hexdigest(),'selected_profile':game,'declared_in_reference':True,
                'script_count':len(scripts),'sample_count':min(limit,len(scripts)),'recovered_samples':recovered,'sample_errors':errors,
                'native_libraries':[n for n in names if n.startswith('lib/') and n.endswith('.so')],
                'stream_7z_count':sum(n.endswith('.stream.7z') for n in names),
                'has_classic_level_layout_paths':any(n.startswith('assets/data/levels/') for n in names),
                'runtime_compatibility':'NOT ESTABLISHED: level adapters, native APIs, assets and gameplay must be verified independently',
                'known_fixture_game':'classic' if sha.hexdigest()=='8cdea1ef39a2a5c3bad5ac27050b491dbdf68050754241390cf9f0d3d24979b8' else None}
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('--game',choices=sorted(PROFILES),required=True)
    p.add_argument('--limit',type=int,default=12);p.add_argument('--sevenzip',default=shutil.which('7z'));p.add_argument('--output',type=Path);a=p.parse_args()
    if a.limit<1:p.error('--limit must be positive')
    result=check(a.apk,a.game,a.sevenzip,a.limit);text=json.dumps(result,indent=2)+'\n'
    if a.output:a.output.write_text(text)
    print(text)
