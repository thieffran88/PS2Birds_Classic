import importlib.util,json,lzma,re,struct,subprocess,sys,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'tools'))
from game_profiles import PROFILES,ROVIO_LZMA,decode_payload
from inspect_apk import recover_lua
from import_level import convert,read_assignments
assert len(PROFILES)==6 and all(len(p['aes_key_hex'])==64 for p in PROFILES.values())
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
# The checks below compare the port against decompiled scripts of the original game. Those files
# are not distributed; a developer keeps them locally in ps2/data (see README, "Desenvolvimento").
if not (root/'ps2/data/episode_page1.decompiled.lua').exists():
    print('Profiles OK: 6 declared keys, synthetic AES/LZMA round trips, wrong/unknown keys. Provenance checks skipped: decompiled reference scripts not present in ps2/data.');sys.exit(0)
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
    doc=json.loads(out.read_text());first=doc['birds'][0]
    assert '#define LEVEL_GRAVITY 10.00000000f' in config and doc['bird_queue_x']==-10
    # Slingshot rest point follows SlingshotSystem.setPositionByStartObject on the first bird.
    assert abs(doc['sling_x']-(first['x']-0.1))<1e-9 and abs(doc['sling_y']-(first['y']-169/20+0.75+0.1))<1e-9
    # The floor is the Ground slab's top surface, 5 units above its stored centre.
    assert doc['ground_object_y']==5 and doc['ground_y']==0
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
parameters=json.loads((root/'profiles/classic_actor_parameters.json').read_text())
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

# Every stage's resting objects sit on the corrected floor and the slingshot sits on its first bird.
import glob,math
for path in sorted(glob.glob(str(root/'ps2/data/level_*_*.json'))):
    d=json.loads(Path(path).read_text());assert d['ground_y']==d['ground_object_y']-5
    assert abs(d['sling_x']-(d['birds'][0]['x']-0.1))<1e-9
print('Ground and slingshot OK: floor at Ground.y - 5 and slingshot on the first queued bird in all stages.')

# ObjectCreation.lua: dynamic sprite-sized shapes are 0.92 of the sprite; static ones keep it.
metrics=json.loads((root/'profiles/sprite_metrics.json').read_text())
for path in sorted(glob.glob(str(root/'ps2/data/level_*_*.json'))):
    d=json.loads(Path(path).read_text())
    for o in d['objects']:
        spec=adapter['objects'][o['definition']]
        if 'sprite' in spec:
            factor=1.0 if o['density']==0 else 0.92
            w,h=(spec['size'][0],spec['size'][1]) if 'size' in spec else (metrics[spec['sprite']]['width']/20*factor,metrics[spec['sprite']]['height']/20*factor)
            assert abs(o['width']-w*abs(o.get('scaleX') or 1))<1e-9 and abs(o['height']-h*abs(o.get('scaleY') or 1))<1e-9
        m=adapter['materials']['by_kind'][str(o['kind'])]
        assert o['restitution']==adapter['materials'].get('restitution_overrides',{}).get(o['definition'],m['restitution']) and o['friction']==adapter['materials']['friction_overrides'].get(o['definition'],m['friction'])
assert adapter['materials']['by_kind']['1']['friction']==4.0 and adapter['materials']['by_kind']['5']['restitution']==0.1
print('Sizes and materials OK: 0.92 size factor on dynamic sprite-sized shapes, full size on static ones, explicit width/height/radius from blocks.lua where declared, friction/restitution from the materials table.')

# Mighty Hoax page 5 (world 5): order, bank and the three new definitions.
mh2=re.findall(r'^L\d+_1.name = "(LevelP2_\d+)"$',(root/'ps2/data/episode_mighty_hoax_page2.decompiled.lua').read_text(),re.M)
page5=episode['additional_pages'][3]
assert len(mh2)==21 and page5['levels']==mh2 and page5['bank']==2 and page5['number']==5
defs=(root/'ps2/data/page5_definitions_excerpt.lua').read_text()
def block(name):return defs.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
objs=json.loads((root/'profiles/classic_level.json').read_text())['objects']
for name,shape,sprite,material in [('LightBlock8','circle','BLOCK_LIGHT_1_8','light'),('ExtraBlockFlag01','box','BLOCK_EXTRA_FLAG_01','extras'),('ExtraApple','circle','BLOCK_APPLE','extras')]:
    b=block(name);assert f'"{shape}"' in b and f'"{sprite}"' in b and f'"{material}"' in b
    assert objs[name]['shape']==shape and objs[name]['sprite']==sprite and objs[name]['kind']==(2 if material=='light' else 6)
for number in range(1,22):
    d=json.loads((root/f'ps2/data/level_5_{number}.json').read_text());assert d['stage_script']==mh2[number-1] and d['texture_bank']==2 and d['title']==f'MIGHTY HOAX 5-{number}'
print('Mighty Hoax page 5 OK: 21 stages match the recovered episodes.lua excerpt; LightBlock8, ExtraBlockFlag01 and ExtraApple checked against blocks.lua; texture bank 2.')

# Danger Above page 6: order, bank and the recovered definitions of the new pieces and bird.
da=re.findall(r'^L\d+_1.name = "(LevelP3_\d+)"$',(root/'ps2/data/episode_danger_above_page1.decompiled.lua').read_text(),re.M)
page6=episode['additional_pages'][4]
assert len(da)==15 and page6['levels']==da and page6['bank']==3 and page6['number']==6
defs=(root/'ps2/data/page6_definitions_excerpt.lua').read_text()
def block6(name):return defs.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
objs=json.loads((root/'profiles/classic_level.json').read_text())
for name,sprite in [('StaticBalloon02','BLOCK_PIG_BALLOON_2'),('StaticBalloon03','BLOCK_BALLOON_1'),('StaticBalloon04','BLOCK_BALLOON_2'),('StaticBalloon05','BLOCK_BALLOON_3')]:
    b=block6(name);assert f'"{sprite}"' in b and '"clouds"' in b and '"staticFragile"' in b and 'destroyedScoreInc' in b
    assert objs['objects'][name]=={'sprite':sprite,'kind':8,'shape':'box'}
t=block6('ExtraTrampoline2');assert '"BLOCK_TRAMPOLINE_01"' in t and '"immovableRubber"' in t and 'L5_1 = 1.1' in t
assert objs['objects']['ExtraTrampoline2']['kind']==3 and objs['materials']['restitution_overrides']['ExtraTrampoline2']==1.1
e=block6('ExtraBoomerangBird');assert 'L5_1 = 1.1\nL4_1.radius' in e and 'L5_1 = 0.3\nL4_1.friction' in e and 'L5_1 = 0.25\nL4_1.restitution' in e
assert objs['objects']['ExtraBoomerangBird']['diameter']==2.2 and objs['materials']['friction_overrides']['ExtraBoomerangBird']==0.3 and objs['materials']['restitution_overrides']['ExtraBoomerangBird']==0.25
g=block6('BoomerangBird')
for field,value in [('radius','1.5'),('density','3'),('restitution','0.23')]:assert f'L5_1 = {value}\nL4_1.{field} = L5_1' in g,field
for field,value in [('boomerangHorizontalForce','-50'),('boomerangVerticalForce','-350'),('boomerangMaxHorizontalSpeed','50'),('flyVerticalAcceleration','-2.8')]:assert f'L5_1 = "{field}"\nL6_1 = {value}' in g,field
assert objs['bird_types']['BoomerangBird']==6
h=(root/'ps2/src/game.h').read_text()
for macro,value in [('GREEN_RADIUS','1.5f'),('GREEN_DENSITY','3.0f'),('GREEN_RESTITUTION','0.23f'),('GREEN_FLY_VERTICAL_ACCELERATION','-2.8f'),('GREEN_HORIZONTAL_FORCE','-50.0f'),('GREEN_VERTICAL_FORCE','-350.0f'),('GREEN_MAX_HORIZONTAL_SPEED','50.0f'),('FRAGILE_DESTROYED_SCORE','100')]:assert f'#define {macro} {value}' in h,macro
logic=(root/'ps2/data/boomerang_logic_excerpt.lua').read_text()
assert 'boomerangHorizontalForce' in logic and 'boomerangMaxHorizontalSpeed' in logic and 'flyVerticalAcceleration' in logic and 'if 5 < L7_2 then' in logic
for number in range(1,16):
    d=json.loads((root/f'ps2/data/level_6_{number}.json').read_text());assert d['stage_script']==da[number-1] and d['texture_bank']==3 and d['title']==f'DANGER ABOVE 6-{number}'
print('Danger Above page 6 OK: 15 stages match episodes.lua; boomerang bird body/specialty, balloons, trampoline and boomerang ornament checked against blocks.lua and gamelogic.lua; texture bank 3.')

# Danger Above page 7: order, bank and the three new static pieces.
da2=re.findall(r'"(LevelP3_\d+)"',(root/'ps2/data/episode_danger_above_page2.decompiled.lua').read_text())
page7=episode['additional_pages'][5]
assert len(da2)==15 and page7['levels']==da2 and page7['bank']==3 and page7['number']==7
defs7=(root/'ps2/data/page7_definitions_excerpt.lua').read_text()
def block7(name):return defs7.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
f=block7('StaticFragileBlock06');assert '"GROUND_BLOCK_06"' in f and '"immovableFragile"' in f and '"staticFragile"' in f and 'L6_1 = 0' in f
g=block7('StaticBlock_06');assert '"GROUND_BLOCK_06"' in g and '"immovable"' in g and '"staticBlocks"' in g
b=block7('StaticBalloon01');assert '"BLOCK_PIG_BALLOON_1"' in b and '"clouds"' in b and '"staticFragile"' in b
assert objs['objects']['StaticFragileBlock06']['kind']==9 and objs['objects']['StaticBlock_06']['kind']==3 and objs['objects']['StaticBalloon01']['kind']==8
assert objs['materials']['by_kind']['9']=={'name':'immovableFragile','friction':0.8,'restitution':0.0}
m=(root/'ps2/data/page7_definitions_excerpt.lua').read_text();assert 'L2_1.immovableFragile = L3_1' in m
for number in range(1,16):
    d=json.loads((root/f'ps2/data/level_7_{number}.json').read_text());assert d['stage_script']==da2[number-1] and d['texture_bank']==3 and d['title']==f'DANGER ABOVE 7-{number}'
assert max(len(json.loads((root/f'ps2/data/level_7_{n}.json').read_text())['objects']) for n in range(1,16))<=352
print('Danger Above page 7 OK: 15 stages match episodes.lua; StaticFragileBlock06, StaticBlock_06 and StaticBalloon01 checked against blocks.lua; texture bank 3; up to 224 objects.')

# Danger Above page 8: order, bank 4 and the three new extras.
da3=list(dict.fromkeys(re.findall(r'"(LevelP3_\d+)"',(root/'ps2/data/episode_danger_above_page3.decompiled.lua').read_text())))
page8=episode['additional_pages'][6]
assert len(da3)==15 and page8['levels']==da3 and page8['bank']==4 and page8['number']==8
defs8=(root/'ps2/data/page8_definitions_excerpt.lua').read_text()
def block8(name):return defs8.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
t=block8('ExtraTrampoline');assert '"BLOCK_SUPER_BALL"' in t and '"rubber"' in t and 'L5_1 = 5.5' in t
c=block8('BlockCarpet');assert '"BLOCK_CARPET"' in c and '"extras"' in c
b=block8('ExtraStrongBall');assert '"BLOCK_STEEL_BALL"' in b and '"rock"' in b and 'L5_1 = 2000' in b
assert objs['objects']['ExtraTrampoline']=={'sprite':'BLOCK_SUPER_BALL','kind':6,'shape':'circle'} and objs['materials']['restitution_overrides']['ExtraTrampoline']==5.5
assert objs['objects']['BlockCarpet']['kind']==6 and objs['objects']['ExtraStrongBall']=={'sprite':'BLOCK_STEEL_BALL','kind':5,'shape':'circle'}
for number in range(1,16):
    d=json.loads((root/f'ps2/data/level_8_{number}.json').read_text());assert d['stage_script']==da3[number-1] and d['texture_bank']==4 and d['title']==f'DANGER ABOVE 8-{number}'
# The stage data place whole stages far from the origin (7-13 sits at x=700..825): the floor must span them.
h=(root/'ps2/src/game.h').read_text();assert '#define FLOOR_HALF_WIDTH 1000.0f' in h
import glob as _g
assert max(abs(o['x']) for f in _g.glob(str(root/'ps2/data/level_*_*.json')) for o in json.loads(Path(f).read_text())['objects'])<1000
print('Danger Above page 8 OK: 15 stages match episodes.lua; super ball (restitution 5.5), carpet and steel ball checked against blocks.lua; bank 4; floor spans every stage.')

# Big Setup (pages 9-11): order, banks, Terence, polygon helmets, rope and instance scale.
pages_bs={9:'episode_big_setup_page1',10:'episode_big_setup_page2',11:'episode_big_setup_page3'}
for number,name in pages_bs.items():
    order=list(dict.fromkeys(re.findall(r'"(LevelP4_\d+)"',(root/f'ps2/data/{name}.decompiled.lua').read_text())))
    page=[pg for pg in episode['additional_pages'] if pg['number']==number][0]
    assert len(order)==15 and page['levels']==order and page['bank']==number-4 and page['folder']=='/levels/4_BigSetup'
    for n in range(1,16):
        d=json.loads((root/f'ps2/data/level_{number}_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==number-4
bs=(root/'ps2/data/big_setup_definitions_excerpt.lua').read_text()
def blockbs(name):return bs.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
t=blockbs('RedBigBird')
for field,value in [('radius','1.8'),('restitution','0.45'),('density','10')]:assert f'L5_1 = {value}\nL4_1.{field} = L5_1' in t,field
assert 'L4_1.wood = 0.3' in bs and 'L4_1.wood = 1.8' in bs and 'L2_1.RedBigBirdDamageFactors = L3_1' in bs
h=(root/'ps2/src/game.h').read_text()
for macro,value in [('BIG_RADIUS','1.8f'),('BIG_DENSITY','10.0f'),('BIG_RESTITUTION','0.45f'),('BIG_DAMAGE_FACTOR','0.3f')]:assert f'#define {macro} {value}' in h,macro
assert objs['bird_types']['RedBigBird']==7
hexv=[[0.2,0.3],[0.4,0.2],[0.6,0.2],[0.8,0.3],[0.8,0.5],[0.2,0.5]]
for name,spr,rest,dens in [('ExtraHelmetSmall','HELMET_SMALL','0.1','0.3'),('ExtraHelmetBig','HELMET_BIG','0','0.5'),('ExtraHelmetBoss','HELMET_BOSS','0','0.5')]:
    b=blockbs(name);nums=[float(v) for v in re.findall(r'"[xy]"\nL\d+_1 = (-?[\d.]+)',b)]
    assert [[nums[i],nums[i+1]] for i in range(0,len(nums),2)]==hexv and f'"{spr}"' in b and f'L5_1 = {rest}\nL4_1.restitution' in b and 'L5_1 = 1\nL4_1.friction' in b
    assert objs['objects'][name]=={'sprite':spr,'kind':6,'shape':'polygon','vertices':hexv}
    assert objs['materials']['friction_overrides'][name]==1.0 and objs['materials']['restitution_overrides'][name]==float(rest)
import build_campaign as _bc
campaign=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN')
n,v=campaign['polygons']['ExtraHelmetSmall'];assert n==6 and all(abs(a-b)<1e-6 for a,b in zip(v,[0.2,0.3,0.4,0.2,0.6,0.2,0.8,0.3,0.8,0.5,0.2,0.5]))
oc=(root/'ps2/data/object_creation_polygon_excerpt.lua').read_text()
assert 'L77_2 = L63_2 * 0.5' in oc and 'L77_2 = L64_2 * 0.5' in oc and 'scaleX' in oc and 'scaleY' in oc
r=blockbs('ExtraRopeThick01');assert '"BLOCK_ROPE_THICK_1"' in r and objs['objects']['ExtraRopeThick01']['kind']==6
# Instance scale: StaticBlock_01_1 of 6-14 is scaled 0.254775 x 0.883135.
d=json.loads((root/'ps2/data/level_6_14.json').read_text());sb=[o for o in d['objects'] if o['name']=='StaticBlock_01_1'][0]
metric=json.loads((root/'profiles/sprite_metrics.json').read_text())['GROUND_BLOCK_01']
assert abs(sb['width']-metric['width']/20*0.254775)<1e-6 and abs(sb['height']-metric['height']/20*0.883135)<1e-6
print('Big Setup OK: 45 stages match episodes.lua in banks 5-7; Terence body and damage factors, hexagonal helmets (ObjectCreation.lua vertex rule) and thick rope checked against blocks.lua; instance scaleX/scaleY applied.')

# Ham 'Em High (pages 12-14): order, banks 8-10, western hats (polygons), short rope and super bowl.
for number,name in {12:'episode_ham_em_high_page1',13:'episode_ham_em_high_page2',14:'episode_ham_em_high_page3'}.items():
    order=list(dict.fromkeys(re.findall(r'"(LevelP5_\d+)"',(root/f'ps2/data/{name}.decompiled.lua').read_text())))
    page=[pg for pg in episode['additional_pages'] if pg['number']==number][0]
    assert len(order)==15 and page['levels']==order and page['bank']==number-4 and page['folder']=='/levels/5_HamEmHigh'
    for n in range(1,16):
        d=json.loads((root/f'ps2/data/level_{number}_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==number-4
hh=(root/'ps2/data/ham_em_high_definitions_excerpt.lua').read_text()
def blockhh(name):return hh.split(f'L3_1 = "{name}"\n',1)[1].split('\nL3_1 = "',1)[0]
for name in ['ExtraCowboyHelmetSmall_1','ExtraCowboyHelmet_1','ExtraSheriffHat_1','ExtraSheriffHat_2','ExtraBanditoHat_1']:
    b=blockhh(name);nums=[float(v) for v in re.findall(r'"[xy]"\nL\d+_1 = (-?[\d.]+)',b)]
    spec=objs['objects'][name];assert spec['shape']=='polygon' and spec['kind']==6
    assert [[nums[i],nums[i+1]] for i in range(0,len(nums),2)]==spec['vertices'] and f'"{spec["sprite"]}"' in b
    rest=float(re.search(r'L5_1 = (-?[\d.]+)\nL4_1.restitution',b)[1]);assert objs['materials']['restitution_overrides'][name]==rest and objs['materials']['friction_overrides'][name]==1.0
assert '"SHORT_THICK_ROPE"' in blockhh('BP_ROPE_THICK') and objs['objects']['BP_ROPE_THICK']['kind']==6
assert '"B_EGG_SUPER_BOWL"' in blockhh('ExtraSuperBowl') and objs['objects']['ExtraSuperBowl']=={'sprite':'B_EGG_SUPER_BOWL','kind':3,'shape':'circle'}
print("Ham 'Em High OK: 45 stages match episodes.lua in banks 8-10; five western hats (polygon vertices, sprite, friction, restitution), short rope and super bowl checked against blocks.lua.")

# Mine and Dine (pages 15-17): order, banks 11-13, the 20 pieces added by tools/add_definitions.py,
# their materials, density-0 fixed pieces and the destroy-attached joints.
import importlib.util as _ilu
_spec=_ilu.spec_from_file_location('add_definitions',root/'tools/add_definitions.py');_ad=_ilu.module_from_spec(_spec);_spec.loader.exec_module(_ad)
md=(root/'ps2/data/mine_and_dine_definitions_excerpt.lua').read_text()
mine=['ExtraCaveHat_1','ExtraCaveHat_2','ExtraCaveHat_3','ExtraKingCaveHat_1','ExtraAmethyst','ExtraAmber','ExtraRuby','ExtraCaveShovel_1','ExtraCaveShovel_2','ExtraCavePickaxe_1','ExtraCavePickaxe_2','ExtraRopeThin01','ExtraRopeThin02','ExtraStalaktite_1','ExtraStalaktite_2','ExtraStalaktite_3','ExtraStalaktite_Tip1','ExtraStalaktite_Tip2','ExtraStalaktite_Tip3','ExtraStalaktite_Tip4']
for name in mine:
    d=_ad.definition(md,name);spec=objs['objects'][name];mat=_ad.material(md,d['material']) if f'L2_1.{d["material"]} = L3_1' in md else {}
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec.get('auto') and spec['kind']==6,name
    if d['type']=='polygon':assert spec['vertices']==d['vertices']
    assert objs['materials']['friction_overrides'][name]==d['props'].get('friction',mat.get('friction',0.7))
    assert objs['materials']['restitution_overrides'][name]==d['props'].get('restitution',mat.get('restitution',0.4))
assert _ad.material(md,'amethyst')['friction']==1 and _ad.material(md,'stalaktite')['restitution']==0.4
for number,name in {15:'episode_mine_and_dine_page1',16:'episode_mine_and_dine_page2',17:'episode_mine_and_dine_page3'}.items():
    order=list(dict.fromkeys(re.findall(r'"(Level[\w]+)"',(root/f'ps2/data/{name}.decompiled.lua').read_text())))
    page=[pg for pg in episode['additional_pages'] if pg['number']==number][0]
    assert len(order)==15 and page['levels']==order and page['bank']==number-4 and page['folder']=='/levels/6_MineAndDine'
    for n in range(1,16):
        d=json.loads((root/f'ps2/data/level_{number}_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==number-4
gen=(root/'ps2/src/definition_sprites.h').read_text()
for name in mine:assert '{"%s",SPR_%s,1}'%(name,objs['objects'][name]['sprite']) in gen
timers=[j['destroy_timer'] for j in campaign['joints'] if j['type']==5]
assert len(timers)>=2 and any(abs(t-2.18)<1e-5 for t in timers) and any(abs(t-4.63)<1e-5 for t in timers)
src=(root/'ps2/src/game.cpp').read_text();assert 'o.density==0)?b2_staticBody' in src
print("Mine and Dine OK: 45 stages in banks 11-13; 20 pieces (hats, gems, tools, ropes, stalactites) match blocks.lua and its materials; destroy-attached joints keep their timers; density-0 pieces are fixed.")

# Birdday 2 (page 18): order, bank 14, GlobeBird (blocks.lua + puffer_bird.lua), party pieces and balloon force.
bd=(root/'ps2/data/birdday2_definitions_excerpt.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(Level[\w]+)"',(root/'ps2/data/episode_birdday2.decompiled.lua').read_text())))
page=[pg for pg in episode['additional_pages'] if pg['number']==18][0]
assert len(order)==15 and page['levels']==order and page['bank']==14 and page['folder']=='/levels/7_Birdday2'
for n in range(1,16):
    d=json.loads((root/f'ps2/data/level_18_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==14
gb=bd.split('L3_1 = "GlobeBird"',1)[1].split('L3_1 = "',1)[0]
h=(root/'ps2/src/game.h').read_text()
for key,macro in [('radius','GLOBE_RADIUS'),('density','GLOBE_DENSITY'),('friction','GLOBE_FRICTION'),('restitution','GLOBE_RESTITUTION')]:
    v=re.search(r'L5_1 = (-?[\d.]+)\nL4_1.%s = L5_1'%key,gb)[1];assert re.search(r'#define %s %sf'%(macro,re.escape(str(float(v)))),h),macro
for key,macro in [('automaticPuffDelay','GLOBE_AUTO_PUFF_DELAY'),('puffTime','GLOBE_PUFF_TIME'),('deathTime','GLOBE_DEATH_TIME'),('directionChangeTime','GLOBE_DIRECTION_TIME'),('directionChangeMagnitude','GLOBE_DIRECTION_MAGNITUDE'),('explosionRadius','GLOBE_BLAST_RADIUS'),('explosionForce','GLOBE_BLAST_FORCE'),('explosionDamageRadius','GLOBE_DAMAGE_RADIUS')]:
    v=re.search(r'L5_1 = "%s"\nL6_1 = (-?[\d.]+)'%key,gb)[1];assert re.search(r'#define %s %sf'%(macro,re.escape(str(float(v)))),h),macro
pv=gb.split('"puffedValues"',1)[1]
for key,macro in [('radius','GLOBE_PUFFED_RADIUS'),('friction','GLOBE_PUFFED_FRICTION'),('restitution','GLOBE_PUFFED_RESTITUTION'),('density','GLOBE_PUFFED_DENSITY')]:
    v=re.search(r'L7_1 = (-?[\d.]+)\nL6_1.%s = L7_1'%key,pv)[1];assert re.search(r'#define %s %sf'%(macro,re.escape(str(float(v)))),h),macro
pl=(root/'ps2/data/puffer_bird_excerpt.lua').read_text();assert 'resizeCircle' in pl and 'makeExplosion' in pl and 'directionChangeMagnitude' in pl and 'deathTime' in pl
assert objs['bird_types']['GlobeBird']==8
party=['BirthdayBalloon_1','BirthdayBalloon_2','BirthdayBalloon_3','BirthdayBalloon_4','BirthdayCake_1','BirthdayCake_2','BirthdayCake_3','BirthdayCandle_1','PartyHat_1','PartyHat_2','PartyHat_3','PartyHat_4','SPECIAL_CAKE_1']
for name in party:
    d=_ad.definition(bd,name);mat=_ad.material(bd,d['material']) if f'L2_1.{d["material"]} = L3_1' in bd else {}
    spec=objs['objects'][name];assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec.get('auto')
    if 'forceY' in mat:assert spec['force']==[mat['forceX'],mat['forceY']]
assert _ad.material(bd,'balloon')['forceY']==-30 and objs['objects']['BirthdayBalloon_2']['force']==[-1.0,-30.0]
assert campaign['forces']['BirthdayBalloon_2']==(-1.0,-30.0)
print("Birdday 2 OK: 15 stages in bank 14; GlobeBird body, puff and death values match blocks.lua and puffer_bird.lua; 13 party pieces and the balloon force (forceX/forceY) match blocks.lua.")

# Explicit sizes: pieces whose blocks.lua entry declares width/height (box) or radius (circle) use them.
allb=Path('/dev/null')
for exc in ['mine_and_dine_definitions_excerpt.lua','birdday2_definitions_excerpt.lua','birdday34_definitions_excerpt.lua']:
    txt=(root/'ps2/data'/exc).read_text()
    for name,spec in objs['objects'].items():
        if not spec.get('auto') or f'L3_1 = "{name}"' not in txt:continue
        d=_ad.definition(txt,name)
        if d['type']=='box' and 'width' in d['props']:assert spec['size']==[d['props']['width'],d['props']['height']],name
        if d['type']=='circle' and 'radius' in d['props']:assert spec['size']==[2*d['props']['radius']]*2,name
assert objs['objects']['ExtraStalaktite_1']['size']==[3.0,13.9] and objs['objects']['BirthdayBalloon_2']['size']==[2.4,2.4]
# Birdday 3 and 4 (pages 19-20): order, banks 15-16, Stella body/bubbles and the new party pieces.
b34=(root/'ps2/data/birdday34_definitions_excerpt.lua').read_text()
for number,name,folder in [(19,'episode_birdday3','/levels/8_Birdday3'),(20,'episode_birdday4','/levels/9_Birdday4')]:
    order=list(dict.fromkeys(re.findall(r'"(Level[\w]+)"',(root/f'ps2/data/{name}.decompiled.lua').read_text())))
    page=[pg for pg in episode['additional_pages'] if pg['number']==number][0]
    assert len(order)==15 and page['levels']==order and page['bank']==number-4 and page['folder']==folder
    for n in range(1,16):
        d=json.loads((root/f'ps2/data/level_{number}_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==number-4
pb=b34.split('L3_1 = "PinkBird"',1)[1].split('L3_1 = "',1)[0]
for key,macro in [('radius','PINK_RADIUS'),('density','PINK_DENSITY'),('friction','PINK_FRICTION'),('restitution','PINK_RESTITUTION')]:
    v=re.search(r'L5_1 = (-?[\d.]+)\nL4_1.%s = L5_1'%key,pb)[1];assert re.search(r'#define %s %sf'%(macro,re.escape(str(float(v)))),h),macro
v=re.search(r'L5_1 = "specialtyRadius"\nL6_1 = (-?[\d.]+)',pb)[1];assert '#define PINK_BUBBLE_RADIUS %sf'%float(v) in h
assert 'L4_1.wood = 0.5' in b34 and '#define PINK_DAMAGE_FACTOR 0.5f' in h and objs['bird_types']['PinkBird']==9
conf=(root/'ps2/data/bubbles_excerpt.lua').read_text()
for macro,key in [('BUBBLE_ANTI_GRAVITY','bubbleAntiGravity'),('BUBBLE_TIME','bubbleAntiGravityTime'),('BUBBLE_MAX_VELOCITY','antiGravityMaxVelocity')]:
    v=re.search(r'L0_1 = (-?[\d.]+)\n%s = L0_1'%key,conf)[1];assert '#define %s %sf'%(macro,float(v)) in h,macro
assert '* 1.3' in conf and 'triggerPinkBirdSpecialty' in conf and 'pinkTimer = 1.5' in conf and 'lifeTime = L0_1' in conf
for name in ['Spawning_cake','CakeCream','PartyHat_5','PartyHat_6','PartyHat_blue']:
    d=_ad.definition(b34,name);spec=objs['objects'][name];assert spec['sprite']==d['sprite'] and spec['shape']==d['type']
assert _ad.definition(b34,'PartyHat_blue')['sprite']=='PARTY_HAT_BLUE' and 'deathSpawn' in b34
# deathSpawn: amount/types and the cream's own density/strength/defence/radius come from blocks.lua.
cake=b34.split('L3_1 = "Spawning_cake"',1)[1].split('L3_1 = "',1)[0];cream=b34.split('L3_1 = "CakeCream"',1)[1].split('L3_1 = "',1)[0]
assert 'L7_1 = 10\nL6_1.amount = L7_1' in cake and objs['objects']['Spawning_cake']['death_spawn']=={'amount':10,'types':['CakeCream','CakeCream']}
assert objs['objects']['CakeCream']['defaults']=={'density':float(re.search(r'L5_1 = ([\d.]+)\nL4_1.density',cream)[1]),'strength':float(re.search(r'L5_1 = ([\d.]+)\nL4_1.strength',cream)[1]),'defence':float(re.search(r'L5_1 = ([\d.]+)\nL4_1.defence',cream)[1])}
cobj=campaign['objects'];parents=[o for o in cobj if o['spawn_of']==0 and o['definition']=='Spawning_cake']
children=[o for o in cobj if o['spawn_of']];assert len(children)==10*len(parents)>0 and all(o['definition']=='CakeCream' for o in children)
print("Birdday 3 and 4 OK: 30 stages in banks 15-16; Stella body, bubble radius and damage from blocks.lua, bubble physics from bubbles.lua/default_conf.lua/mode_MuseumMode.lua; party pieces with explicit sizes; deathSpawn releases 10 creams per big cake.")

# Birdday 6 (page 21): order, bank 17, the four new pieces and the joints they rely on.
b6=(root/'ps2/data/birdday6_definitions_excerpt.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(BD6Level\d+)"',(root/'ps2/data/episode_birdday6.decompiled.lua').read_text())))
page=[pg for pg in episode['additional_pages'] if pg['number']==21][0]
assert len(order)==15 and page['levels']==order and page['bank']==17 and page['folder']=='/levels/11_Birdday6'
for n in range(1,16):
    d=json.loads((root/f'ps2/data/level_21_{n}.json').read_text());assert d['stage_script']==order[n-1] and d['texture_bank']==17
blk=lambda name:b6.split(f'L3_1 = "{name}"',1)[1].split('L3_1 = "',1)[0]
assert 'L4_1.forceY = L5_1' in blk('BP_SINGLE_BALLOON') and re.search(r'L5_1 = -80\nL4_1.forceY',blk('BP_SINGLE_BALLOON')) and '"targetVelocity"\nL6_1 = 20' in blk('BP_SINGLE_BALLOON')
assert objs['objects']['BP_SINGLE_BALLOON']['force']==[0.0,-80.0] and objs['objects']['BP_SINGLE_BALLOON']['force_target']==20.0
assert 'L5_1 = false\nL4_1.collision = L5_1' in blk('StaticNoCollisionBig') and objs['objects']['StaticNoCollisionBig']['no_collision'] and objs['objects']['StaticNoCollisionBig']['kind']==3
assert 'L5_1 = 1.472\nL4_1.radius' in blk('BP_WOODEN_WHEEL_WEAK') and objs['objects']['BP_WOODEN_WHEEL_WEAK']['size']==[2.944,2.944]
assert objs['objects']['BIRDDAY5_CAKE']['vertices']==[[0.1,1.0],[0.9,1.0],[0.75,0.2],[0.25,0.2]]
camp=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN')
assert sum(1 for n in range(1,16) for o in json.loads((root/f'ps2/data/level_21_{n}.json').read_text())['objects'] if o.get('no_collision') and o['definition']=='StaticNoCollisionBig')==3
bd6j=[j for n in range(1,16) for j in json.loads((root/f'ps2/data/level_21_{n}.json').read_text())['joints']]
prism=[j for j in bd6j if j['type']==4];assert len(prism)==9 and all(j['motor'] and j['limit'] for j in prism)
motors=[j for j in camp['joints'] if j['type']==3 and j['flags']&2];assert len(motors)>=3
print("Birdday 6 OK: 15 stages in bank 17; single balloon force/targetVelocity, wheel radius, cake polygon and collision-free scenery from blocks.lua; 9 prismatic joints and the motor hinges carried into CAMPAIGN.BIN.")

# Birdday 5 (pages 22-27, banks 18-23): 30 stages, 90 pieces read from blocks.lua by add_definitions.py.
b5=(root/'ps2/data/birdday5_definitions_excerpt.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(UGC1Level\d+)"',(root/'ps2/data/episode_birdday5_page1.decompiled.lua').read_text()+(root/'ps2/data/episode_birdday5_page2.decompiled.lua').read_text())))
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/10_Birdday5']
assert len(order)==30 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==list(range(18,24))
assert [pg.get('first',1) for pg in pages]==[1,6,11,16,21,26]
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',b5);assert len(names)==90
for name in names:
    d=_ad.definition(b5,name);spec=objs['objects'][name];mat=_ad.material(b5,d['material'])
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec.get('half_resolution'),name
    assert spec['kind']==(3 if d['material']=='immovable' else 6),name
    assert bool(spec.get('no_collision'))==bool(d['props'].get('no_collision')),name
    if d['type']=='polygon':assert spec['vertices']==d['vertices'],name
    assert objs['materials']['friction_overrides'][name]==d['props'].get('friction',mat.get('friction',0.7)),name
d=json.loads((root/'ps2/data/level_27_5.json').read_text());assert d['title']=='BIRDDAY 5-30' and d['texture_bank']==23
nocol=sum(1 for n in names if objs['objects'][n].get('no_collision'));assert nocol==39
print(f"Birdday 5 OK: 30 stages in banks 18-23 numbered 5-1 to 5-30; 90 pieces (sprite, shape, polygon, material, {nocol} without collision) match blocks.lua.")

# Birdday 7 (page 28, bank 24): 15 stages, 36 pieces; themed wood keeps the wood kind, island plants are fragile scenery.
b7=(root/'ps2/data/birdday7_definitions_excerpt.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(Level\d+_BirdDay7)"',(root/'ps2/data/episode_birdday7.decompiled.lua').read_text())))
page=[pg for pg in episode['additional_pages'] if pg['number']==28][0]
assert len(order)==15 and page['levels']==order and page['bank']==24
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',b7);assert len(names)==36
kinds={'wood':1,'light':2,'rock':5,'immovableFragile':9,'immovable':3}
for name in names:
    d=_ad.definition(b7,name);spec=objs['objects'][name]
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec['kind']==kinds.get(d['material'],6),name
    assert bool(spec.get('no_collision'))==bool(d['props'].get('no_collision')),name
    if 'forceY' in _ad.material(b7,d['material']):assert spec['force']==[_ad.material(b7,d['material'])['forceX'],_ad.material(b7,d['material'])['forceY']],name
print("Birdday 7 OK: 15 stages in bank 24; 36 pieces match blocks.lua (themed wood as wood, fragile island plants, balloon force).")

# Surf and Turf (pages 29-31, banks 25-27): 45 stages, 14 beach pieces with their material's kind.
st=(root/'ps2/data/surf_and_turf_definitions_excerpt.lua').read_text()
order=sum((list(dict.fromkeys(re.findall(r'"(LevelS\d+)"',(root/f'ps2/data/episode_surf_and_turf_page{k}.decompiled.lua').read_text()))) for k in (1,2,3)),[])
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/15_SurfnTurf']
assert len(order)==45 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==[25,26,27]
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',st);assert len(names)==14
for name in names:
    d=_ad.definition(st,name);spec=objs['objects'][name]
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec['kind']=={'wood':1,'light':2,'rock':5}.get(d['material'],6),name
print("Surf and Turf OK: 45 stages in banks 25-27; 14 beach pieces match blocks.lua (shape, sprite, kind by material).")

# Bad Piggies (pages 32-34, banks 28-30): 45 stages, 6 vehicle pieces.
bp=(root/'ps2/data/bad_piggies_definitions_excerpt.lua').read_text()
order=sum((list(dict.fromkeys(re.findall(r'"(Level\d+)"',(root/f'ps2/data/episode_bad_piggies_page{k}.decompiled.lua').read_text()))) for k in (1,2,3)),[])
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/16_BadPiggies']
assert len(order)==45 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==[28,29,30]
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',bp);assert len(names)==6
for name in names:
    d=_ad.definition(bp,name);spec=objs['objects'][name]
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec['kind']=={'wood':1}.get(d['material'],6),name
print("Bad Piggies OK: 45 stages in banks 28-30; 6 vehicle pieces match blocks.lua.")

# Short Fuse (pages 35-37, banks 31-33): 45 stages, 70 lab pieces and the shockwave black bird.
sf=(root/'ps2/data/short_fuse_definitions_excerpt.lua').read_text()
order=sum((list(dict.fromkeys(re.findall(r'"(BlackBird_\d+)"',(root/f'ps2/data/episode_short_fuse_page{k}.decompiled.lua').read_text()))) for k in (1,2,3)),[])
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/18_ShortFuse']
assert len(order)==45 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==[31,32,33]
names=[n for n in re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',sf) if n!='BlackBird_Shockwave'];assert len(names)==70
kinds={'wood':1,'light':2,'rock':5,'immovableFragile':9,'immovable':3}
for name in names:
    d=_ad.definition(sf,name);spec=objs['objects'][name]
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec['kind']==kinds.get(d['material'],6),name
bird=sf.split('L3_1 = "BlackBird_Shockwave"',1)[1].split('L3_1 = "',1)[0]
for key,macro in [('duration','SHOCK_DURATION'),('force','SHOCK_FORCE'),('minimumForce','SHOCK_MIN_FORCE'),('forceDropOffPoint','SHOCK_DROP_OFF'),('damage','SHOCK_DAMAGE'),('minimumDamage','SHOCK_MIN_DAMAGE'),('delay','SHOCK_DELAY')]:
    v=float(re.search(r'L7_1 = "%s"\nL8_1 = ([\d.]+)'%key,bird)[1]);assert re.search(r'#define %s %sf'%(macro,re.escape(str(v))),h),macro
assert 'L7_1 = 15\nL6_1.radius = L7_1' in bird and '#define SHOCK_RADIUS 15.0f' in h and 'triggerBlackBirdSpecialtyShockwave' in bird
assert objs['bird_types']['BlackBird_Shockwave']==10 and 'BlackBird_Shockwave' not in objs['objects']
print("Short Fuse OK: 45 stages in banks 31-33; 70 lab pieces and the shockwave bird (duration, radius, force, damage, delay) match blocks.lua.")

# Bird Island (page 38, bank 34): 21 stages, 17 pieces including the bearded king pig (a level goal).
bi=(root/'ps2/data/bird_island_definitions_excerpt.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(Level\w+_Movie)"',(root/'ps2/data/episode_bird_island.decompiled.lua').read_text())))
page=[pg for pg in episode['additional_pages'] if pg['number']==38][0]
assert len(order)==21 and page['levels']==order and page['bank']==34
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',bi);assert len(names)==17
kinds={'wood':1,'light':2,'rock':5,'immovableFragile':9,'immovable':3,'piglette':4}
for name in names:
    d=_ad.definition(bi,name);spec=objs['objects'][name]
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec['kind']==kinds.get(d['material'],6),name
king=bi.split('L3_1 = "BeardedKingPiglette"',1)[1].split('L3_1 = "',1)[0]
assert '"levelGoal"' in king and 'L5_1 = "ML_PIG_KING_01"\nL4_1.sprite = L5_1' in king and objs['objects']['BeardedKingPiglette']=={**objs['objects']['BeardedKingPiglette'],'kind':4,'sprite':'ML_PIG_KING_01','size':[5.65,5.65]}
print("Bird Island OK: 21 stages in bank 34; 17 pieces match blocks.lua, the bearded king is a pig (kind 4) with its own sprite, not the score sprite.")

# Piggy Farm (pages 39-44, banks 35-40): 45 stages, 73 pieces; decoration_new pieces have no collider;
# negative scaleX/scaleY in the levels mirror the piece.
pf=(root/'ps2/data/piggy_farm_definitions_excerpt.lua').read_text()
order=sum((list(dict.fromkeys(re.findall(r'"(Farm\d_\w+)"',(root/f'ps2/data/episode_piggy_farm_page{k}.decompiled.lua').read_text()))) for k in (1,2,3)),[])
order=[n for n in order if 'cutscene' not in n]
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/21_PiggyFarm']
assert len(order)==45 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==list(range(35,41))
names=re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',pf);assert len(names)==73
for name in names:
    d=_ad.definition(pf,name);spec=objs['objects'][name]
    nocol=_ad.material_has_no_collider(pf,d['material'])
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec.get('half_resolution'),name
    assert spec['kind']==(3 if nocol else {'wood':1,'light':2,'rock':5}.get(d['material'],6)) and bool(spec.get('no_collision'))==(nocol or bool(d['props'].get('no_collision'))),name
assert 'L4_1 = L4_1.none\nL3_1.collider = L4_1' in pf and objs['objects']['DecorationFarmDecoHay1']['no_collision']
camp=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN');mirrored=[o for o in camp['objects'] if o['mirror']]
assert len(mirrored)>=70 and all(o['mirror'] in (2,4,6) for o in mirrored)
print(f"Piggy Farm OK: 45 stages in banks 35-40; 73 pieces match blocks.lua (decoration_new without collider); {len(mirrored)} mirrored pieces.")

# Dino (pages 45-57, banks 41-53): 46 stages, 107 pieces; cages release their bird into the queue.
dn=(root/'ps2/data/dino_definitions_excerpt.lua').read_text()
order=sum((list(dict.fromkeys(re.findall(r'"(Dino\d_\w+)"',(root/f'ps2/data/episode_dino_page{k}.decompiled.lua').read_text()))) for k in (1,2,3)),[])
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/22_Dino']
assert len(order)==46 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==list(range(41,54))
names=list(dict.fromkeys(re.findall(r'L3_1 = "(\w+)"\nL4_1 = \{\}',dn)));assert len(names)==107
cages=0
for name in names:
    d=_ad.definition(dn,name);spec=objs['objects'][name]
    nocol=_ad.material_has_no_collider(dn,d['material'])
    assert spec['sprite']==d['sprite'] and spec['shape']==d['type'] and spec.get('half_resolution'),name
    assert spec['kind']==(3 if nocol else {'wood':1,'light':2,'rock':5,'immovable':3}.get(d['material'],6)) and bool(spec.get('no_collision'))==(nocol or bool(d['props'].get('no_collision'))),name
    assert spec.get('release_bird')==d['props'].get('release_bird'),name
    if spec.get('release_bird'):assert spec['release_bird'] in objs['bird_types'];cages+=1
camp=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN');released=[o for o in camp['objects'] if o['release_bird']>=0]
assert cages==6 and released and all(objs['objects'][o['definition']].get('release_bird') for o in released)
print(f"Dino OK: 46 stages in banks 41-53; 107 pieces match blocks.lua; {cages} cage types, {len(released)} caged birds released into the queue.")

# Ham 'Em High extra page (page 58, bank 54): the three LevelFB stages use the older level
# format: theme blocks resolve to StaticBlock_NN (dynamicassets.lua) and density, strength and
# defence come from the piece definition or its material (ObjectCreation.lua).
fb=(root/'ps2/data/episode_ham_extra_page.decompiled.lua').read_text()
order=list(dict.fromkeys(re.findall(r'"(LevelFB_\d)"',fb)))
page=[pg for pg in episode['additional_pages'] if pg['number']==58][0]
assert order==['LevelFB_1','LevelFB_2','LevelFB_3']==page['levels'] and page['bank']==54
for k in (1,2,3):
    d=json.loads((root/f'ps2/data/level_58_{k}.json').read_text())
    assert d['bird_queue_x']==d['sling_x'] and all(o['definition'] in objs['objects'] for o in d['objects'])
assert objs['objects']['WoodBlock5']['defaults']=={'density':1.5,'strength':80.0,'defence':2.5}
assert objs['objects']['StaticBlock_04']['defaults']=={'density':0.0,'strength':30.0,'defence':100000.0}
print("Ham 'Em High extra OK: 3 LevelFB stages in bank 54; theme blocks as StaticBlock_NN, material defaults and queue at the slingshot.")

# Flock Favorites (pages 59-60, banks 55-56): 25 of the 30 stages. Stages 12, 14, 25 and 28 use
# theme26, which FlockFavoritesMode.lua runs as the Egg Defender mode (RMFClassic, not ported);
# stage 9 falls apart while settling (welded ropes, as Short Fuse 2-13/3-3).
ff=''.join((root/f'ps2/data/episode_flock_favorites_page{k}.decompiled.lua').read_text() for k in (1,2))
order=list(dict.fromkeys(re.findall(r'"(FlockFavorites\d+)"',ff)));assert order==[f'FlockFavorites{k:02d}' for k in range(1,31)]
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/19_FlockFavorites']
left={'FlockFavorites09','FlockFavorites12','FlockFavorites14','FlockFavorites25','FlockFavorites28'}
assert sum((pg['levels'] for pg in pages),[])==[n for n in order if n not in left] and [pg['bank'] for pg in pages]==[55,56]
# Explosive pieces: every definition with specialty BOMB carries its blocks.lua blast values.
bl=(root/'ps2/data/flock_favorites_definitions_excerpt.lua').read_text()
for name in ['BP_TNT','BP_TNT_SMALL']:
    d=_ad.definition(bl,name);assert objs['objects'][name]['explosion']==d['props']['explosion']==[20.0,2000.0,15.0,500.0],name
camp=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN')
assert camp['explosions']['Farm_Campstove']==(10.0,30000.0,5.0,150.0) and camp['explosions']['EXPLOSIVE_BALLOON']==(10.0,30000.0,5.0,300.0)
print(f"Flock Favorites OK: 25 stages in banks 55-56 (4 Egg Defender stages and the unstable 1-9 left out); {len(camp['explosions'])} explosive piece types carry their own blast values.")

# Golden Eggs (pages 61-63, banks 57-59): 29 of the 31 stages, in episodes.lua order. LevelGE_31
# (gravity -20, upside down) and LevelGE_UGC1 (gravity 0) need gravity the port does not support yet.
# Stages without starLimits.lua entries give 3 stars on any win (the original gives the golden egg).
ge=''.join((root/f'ps2/data/episode_golden_eggs_page{k}.decompiled.lua').read_text() for k in (1,2))
order=[n for n in dict.fromkeys(re.findall(r'"((?:LevelGE_\w+|GE_Level\w+))"',ge)) if n not in ('LevelGE_31','LevelGE_UGC1')]+['Farm2_goldegg']
pages=[pg for pg in episode['additional_pages'] if pg.get('folder')=='/levels/goldeneggs1']
assert len(order)==29 and sum((pg['levels'] for pg in pages),[])==order and [pg['bank'] for pg in pages]==[57,58,59]
gb=(root/'ps2/data/golden_eggs_definitions_excerpt.lua').read_text()
assert objs['objects']['BP_TNT_WEAK']['explosion']==_ad.definition(gb,'BP_TNT_WEAK')['props']['explosion']==[10.0,30000.0,10.0,350.0]
assert objs['objects']['StaticFragileBlock01']['kind']==9 and 'half_resolution' not in objs['objects']['ExtraBlueBird']
camp=_bc.read_binary(root/'ps2/assets/CAMPAIGN.BIN');stars=json.loads((root/'profiles/star_limits.json').read_text())['limits']
nostar=[n for n in order if n not in stars];assert nostar==["LevelGE_25","LevelGE_30","LevelGE_RED"]
print(f"Golden Eggs OK: 29 stages in banks 57-59 (upside-down and zero-gravity stages left out); {len(nostar)} stages without star limits give 3 stars on any win.")
