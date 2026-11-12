import importlib.util,json,lzma,re,struct,subprocess,sys,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'tools'))
from game_profiles import PROFILES,ROVIO_LZMA,decode_payload
from inspect_apk import recover_lua
from import_level import convert,read_assignments
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
    wrapper=folder/'archive helper.py'
    wrapper.write_text("import sys\nassert sys.argv[1:3]==['x','-so']\nsys.stdout.buffer.write("+repr(chunk)+")\n")
    wrapper.chmod(0o600)
    assert decode_payload(b'7z\xbc\xaf\x27\x1cfixture',sevenzip=wrapper)==chunk
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
    assert bird['startNumber']==2 and bird['x']==-10.6631 and bird['density']==6 and len(recovered['objects'])==45
    source.write_text(original.replace('L1_1.startNumber = 1','L1_1.startNumber = 2'))
    before=header.read_bytes()
    try:convert(source,out,header)
    except ValueError as error:assert 'Duplicate bird queue position' in str(error)
    else:raise AssertionError('Duplicate queue position accepted')
    assert header.read_bytes()==before
print('Profiles OK: 6 declared keys, synthetic AES/LZMA round trips, wrong/unknown keys, data-driven constants, unsupported-adapter rejection.')

# Independent source excerpt, recovered again from the APK for the 0.10.0 review.
episode=(root/'ps2/data/episode_page1.decompiled.lua').read_text()
official=re.findall(r'^L\d+_1.name = "(Level\d+)"$',episode,re.M)
profile=json.loads((root/'profiles/classic_episode.json').read_text())
assert official==profile['levels'] and len(official)==21
assert 'L4_1.folder_name = "/levels/1_PoachedEggs"' in episode
print('Episode order OK: 21 stages match the independently recovered episodes.lua excerpt.')

page2=(root/'ps2/data/episode_page2.decompiled.lua').read_text()
assert re.findall(r'^L\d+_1.name = "(Level\d+)"$',page2,re.M)==profile['additional_pages'][0]['levels']
# Different registers and computed keys must preserve every world object.
env=read_assignments('local L0_1, L1_1, L2_1\nL0_1 = {}\nL2_1 = {}\nL2_1.name = "second"\nL1_1 = "computed"\nL0_1[L1_1] = L2_1\nworld = L0_1')
assert env['world']['computed']['name']=='second'
for bad in ['os.execute("touch /tmp/never")','world = require("x")','while true do']:
    try:read_assignments(bad)
    except ValueError:pass
    else:raise AssertionError('Executable Lua accepted')
for name,expected in [('Level7',50),('Level8',51),('Level9',55),('Level17',51),('Level16',54),('Level44',89),('Level88',82)]:
    text=(root/f'ps2/data/{name}.decompiled.lua').read_text()
    assert len(read_assignments(text)['world'])==expected
print('Full world tables OK: register aliases/computed keys, seven formerly truncated scenes, executable Lua rejected; second-page official prefix OK.')

# Shape declarations are checked against a recovered blocks.lua excerpt.
excerpt=(root/'ps2/data/circle_definitions_excerpt.lua').read_text()
adapter=json.loads((root/'profiles/classic_level.json').read_text())
for name in ['WoodBlock7','LightBlock7','ExtraStrawberry']:
    part=excerpt.split('L3_1 = "'+name+'"',1)[1].split('\n\n',1)[0]
    assert 'L4_1.type = "circle"' in part and adapter['objects'][name]['shape']=='circle'
for name in ['WoodBlock7','LightBlock7']:
    all_objects=[o for p in (root/'ps2/data').glob('level_*.json') for o in json.loads(p.read_text())['objects'] if o['definition']==name]
    assert all_objects and all(o['shape']=='circle' for o in all_objects)
print('Circle shapes OK: recovered WoodBlock7, LightBlock7 and strawberry declarations match adapter and generated scenes.')

white=json.loads((root/'ps2/data/level_2_14.json').read_text())
assert white['stage_script']=='Level26' and len(white['objects'])==100
assert [b['definition'] for b in white['birds']]==['BasicBird2']*4 and [b['type'] for b in white['birds']]==[4]*4
assert len(read_assignments((root/'ps2/data/Level26.decompiled.lua').read_text())['world'])==105
parameters=json.loads((root/'ps2/data/classic_actor_parameters.json').read_text())
excerpt=(root/'ps2/data/white_definitions_excerpt.lua').read_text().split('L3_1 = "EggGranade"',1)[1]
assert 'L5_1 = "explosionForce"\nL6_1 = 10000' in excerpt and parameters['EggGranade']['explosionForce']==10000
assert 'L5_1 = "explosionDamage"\nL6_1 = 400' in excerpt and parameters['EggGranade']['explosionDamage']==400
print('White provenance OK: complete 105-entry world, four BasicBird2 birds, EggGranade blast fields checked against recovered source.')

# TNT provenance and the complete second page.
tnt=(root/'ps2/data/tnt_definitions_excerpt.lua').read_text().split('L3_1 = "ExtraBlockTNT"',1)[1].split('L3_1 = "',1)[0]
for field,value in [('explosionRadius',10),('explosionForce',30000),('explosionDamageRadius',5),('explosionDamage',150)]:
    assert f'L5_1 = "{field}"\nL6_1 = {value}' in tnt and parameters['ExtraBlockTNT'][field]==value
assert adapter['objects']['ExtraBlockTNT']['kind']==7 and adapter['objects']['WoodBlock8']['shape']=='circle' and adapter['objects']['GrandpaPiglette']['diameter']==4.4
game_h=(root/'ps2/src/game.h').read_text()
for text in ['TNT_BLAST_RADIUS 10.0f','TNT_BLAST_FORCE 30000.0f','TNT_DAMAGE_RADIUS 5.0f','TNT_BLAST_DAMAGE 150.0f']:assert text in game_h
page2=re.findall(r'\.name = "(Level\d+)"',(root/'ps2/data/episode_page2.decompiled.lua').read_text())
episode=json.loads((root/'profiles/classic_episode.json').read_text())
assert len(page2)==21 and episode['additional_pages'][0]['levels']==page2
kinds={}
for number in range(15,22):
    d=json.loads((root/f'ps2/data/level_2_{number}.json').read_text());assert d['stage_script']==page2[number-1]
    for o in d['objects']:kinds.setdefault(o['definition'],set()).add(o['kind'])
assert kinds['ExtraBlockTNT']=={7} and kinds['GrandpaPiglette']=={4} and kinds['WoodBlock8']=={1}
print('Second page OK: all 21 stages match the recovered episodes.lua excerpt; TNT blast fields checked against recovered source.')

page3=re.findall(r'^L\d+_1.name = "(Level\d+)"$',(root/'ps2/data/episode_page3.decompiled.lua').read_text(),re.M)
assert len(page3)==21 and episode['additional_pages'][1]['levels']==page3
king=(root/'ps2/data/page3_definitions_excerpt.lua').read_text().split('L3_1 = "KingPiglette"',1)[1].split('L3_1 = "',1)[0]
assert 'L5_1 = 2.825\nL4_1.radius = L5_1' in king and adapter['objects']['KingPiglette']['diameter']==5.65
assert json.loads((root/'ps2/data/level_3_21.json').read_text())['stage_script']=='Level81'
assert len(json.loads((root/'ps2/data/level_3_21.json').read_text())['objects'])==135
print('Third page OK: 21 stages match the recovered episodes.lua excerpt; king pig radius checked; 135-object final stage.')

mh=re.findall(r'^L\d+_1.name = "(LevelP2_\d+)"$',(root/'ps2/data/episode_mighty_hoax_page1.decompiled.lua').read_text(),re.M)
assert len(mh)==21 and episode['additional_pages'][2]['levels']==mh and episode['additional_pages'][2]['bank']==1
excerpt=(root/'ps2/data/mighty_hoax_definitions_excerpt.lua').read_text()
for name,shape in [('WoodBlock11','triangle_l'),('WoodBlock12','triangle_m'),('StoneBlock11','triangle_l'),('LightBlock12','triangle_m')]:
    part=excerpt.split('L3_1 = "'+name+'"',1)[1].split('L3_1 = "',1)[0]
    assert 'L5_1 = "polygon"' in part and adapter['objects'][name]['shape']==shape
    assert ('L9_1 = 0.5' in part)==(shape=='triangle_m')
for name in ['WoodBlock9','StoneBlock10']:
    assert 'L5_1 = "box"' in excerpt.split('L3_1 = "'+name+'"',1)[1].split('L3_1 = "',1)[0]
shapes={o['shape'] for n in range(1,22) for o in json.loads((root/f'ps2/data/level_4_{n}.json').read_text())['objects']}
assert {'triangle_l','triangle_m'}<=shapes and all(json.loads((root/f'ps2/data/level_4_{n}.json').read_text())['texture_bank']==1 for n in range(1,22))
print('Mighty Hoax OK: 21 stages match the recovered episodes.lua excerpt; polygon triangles and boxes checked against blocks.lua; texture bank 1.')
