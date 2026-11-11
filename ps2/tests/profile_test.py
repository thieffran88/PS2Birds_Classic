import importlib.util,json,lzma,re,struct,subprocess,sys,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'tools'))
from game_profiles import PROFILES,ROVIO_LZMA,decode_payload
from inspect_apk import recover_lua
from import_level import convert
# Independently cross-check declarations against the supplied reference, not another copy of the manifest.
reference=(root/'profiles/REFERENCE_DECOMPILATION_README.md')
assert reference.exists()
if reference.exists():
    keys=re.findall(r'Hex = ([0-9A-F]+)',reference.read_text())
    assert len(keys)==6 and [k.lower() for k in keys]==[p['aes_key_hex'] for p in PROFILES.values()]
chunk=bytes.fromhex('1b4c75615100010404040400')+b'test-fixture-not-executed'
for game,profile in PROFILES.items():
    payload=ROVIO_LZMA+lzma.compress(chunk,format=lzma.FORMAT_ALONE)
    encrypted=subprocess.run(['openssl','enc','-aes-256-cbc','-e','-K',profile['aes_key_hex'],'-iv','0'*32],input=payload,capture_output=True,check=True).stdout
    assert recover_lua(encrypted,game=game)==chunk
    assert recover_lua(chunk,game=game)==chunk
    if game!='classic' and profile['aes_key_hex']!=PROFILES['classic']['aes_key_hex']:
        try:recover_lua(encrypted,game='classic')
        except (ValueError,lzma.LZMAError):pass
        else:raise AssertionError('Wrong key accepted as Lua')
assert decode_payload(b'{"frames": []}')==b'{"frames": []}'
try:decode_payload(b'{not-plaintext-json')
except ValueError as error:assert 'AES failed' in str(error)
else:raise AssertionError('Encrypted-looking brace treated as plaintext JSON')
try:decode_payload(b'payload',game='unknown')
except ValueError:pass
else:raise AssertionError('Unknown profile accepted')
with tempfile.TemporaryDirectory(prefix='ps2birds-adapter-') as folder:
    folder=Path(folder);source=folder/'alternate.lua';out=folder/'level.json';header=folder/'level_data.h'
    single=folder/'input.lua';result=folder/'output.luac'
    payload=ROVIO_LZMA+lzma.compress(chunk,format=lzma.FORMAT_ALONE)
    encrypted=subprocess.run(['openssl','enc','-aes-256-cbc','-e','-K',PROFILES['seasons']['aes_key_hex'],'-iv','0'*32],input=payload,capture_output=True,check=True).stdout
    single.write_bytes(encrypted)
    subprocess.run([sys.executable,str(root/'tools/recover_lua_file.py'),str(single),str(result),'--game','seasons'],check=True,capture_output=True)
    assert result.read_bytes()==chunk and single.read_bytes()==encrypted
    rejected=subprocess.run([sys.executable,str(root/'tools/recover_lua_file.py'),str(single),str(single),'--game','seasons'],capture_output=True)
    assert rejected.returncode!=0 and single.read_bytes()==encrypted
    original=(root/'ps2/data/Level1.decompiled.lua').read_text()
    # Prove constants and bird counts come from data, without pretending this fixture is a real game.
    source.write_text(original.replace('L0_1 = 20\nworldGravity','L0_1 = 10\nworldGravity').replace('L0_1 = -352.76\nbirdZeroPosX','L0_1 = -200\nbirdZeroPosX'))
    convert(source,out,header)
    config=(folder/'level_config.h').read_text()
    assert '#define LEVEL_GRAVITY 10.00000000f' in config and '#define LEVEL_SLING_X -10.00000000f' in config
    assert json.loads(out.read_text())['ground_y']==5
    two_birds=re.sub(r'L1_1 = {}\nL1_1.startNumber = 1\n.*?L0_1.RedBird_4 = L1_1\n','',original,flags=re.S)
    source.write_text(two_birds);convert(source,out,header)
    assert '#define LEVEL_BIRDS 2' in (folder/'level_config.h').read_text()
    custom=json.loads((root/'profiles/classic_level.json').read_text())
    custom['view']={'x':180,'y':300,'scale':6}
    custom['objects']['ImportedWoodAlias']=custom['objects']['WoodBlock6']
    adapter=folder/'adapter.json';adapter.write_text(json.dumps(custom))
    source.write_text(original.replace('"WoodBlock6"','"ImportedWoodAlias"'))
    convert(source,out,header,adapter)
    assert any(o['definition']=='ImportedWoodAlias' for o in json.loads(out.read_text())['objects'])
    assert '#define LEVEL_VIEW_SCALE 6.00000000f' in (folder/'level_config.h').read_text()
    source.write_text(original.replace('"WoodBlock6"','"UnsupportedPlanet"'))
    before=header.read_bytes()
    try:convert(source,out,header)
    except ValueError as error:assert 'Unsupported definition' in str(error)
    else:raise AssertionError('Unsupported body accepted')
    assert header.read_bytes()==before
    # Level 7 uses register-backed object fields. Verify independently known queue data.
    convert(root/'ps2/data/Level7.decompiled.lua',out,header)
    recovered=json.loads(out.read_text());bird=next(b for b in recovered['birds'] if b['name']=='RedBird_2')
    assert bird['startNumber']==2 and bird['x']==-10.6631 and bird['density']==6 and len(recovered['objects'])==41
    source.write_text(original.replace('L1_1.startNumber = 1','L1_1.startNumber = 2'))
    before=header.read_bytes()
    try:convert(source,out,header)
    except ValueError as error:assert 'Duplicate bird queue position' in str(error)
    else:raise AssertionError('Duplicate queue position accepted')
    assert header.read_bytes()==before
print('Profiles OK: 6 declared keys, synthetic AES/LZMA round trips, wrong/unknown keys, data-driven constants, unsupported-adapter rejection.')
