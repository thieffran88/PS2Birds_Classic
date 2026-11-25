#!/usr/bin/env python3
"""Convert selected original Ogg effects to mono SPU2 ADPCM; never execute game scripts."""
import argparse,hashlib,json,struct,subprocess,tempfile,wave,zipfile
from pathlib import Path
SOUNDS=[
 ('MENU_MOVE','menu select',0,4),('MENU_CONFIRM','menu confirm',0,4),('MENU_BACK','menu back',0,4),
 ('LAUNCH_RED','bird 01 flying',1,0),('LAUNCH_BLUE','bird 02 flying',1,0),('LAUNCH_YELLOW','bird 03 flying',1,0),
 ('SPLIT','special egg',2,0),('BOOST','special boost',2,0),('PIG','piglette destroyed',3,8),
 ('WOOD','wood destroyed a1',4,12),('GLASS','light destroyed a1',4,12),('STONE','rock destroyed a1',4,12),
 ('WIN','level clear military a1',5,0),('LOSE','level failed piglets a1',6,0),('START','level start military a1',7,0),
 ('STRETCH','slingshot streched',8,30),('LAUNCH_BLACK','bird 04 flying',1,0),('EXPLOSION','special egg explosion',9,0),('LAUNCH_WHITE','bird 05 flying',1,0),('DROP_EGG','special group',2,0),
 ('STAR_1','star_1',2,0),('STAR_2','star_2',3,0),('STAR_3','star_3',4,0),('SCORE_COUNT','gamescorescreen_score_count_loop',7,0),('NEW_HIGHSCORE','highscore',9,0)]
# Classic-only ids after the 25 shared ones (the Rio numbers its own after them): the impact sounds of the blocks (materials' collision
# sound) and each bird's own collision sound on its first touch (birds.lua sounds.collision).
CLASSIC_ONLY=[('HIT_WOOD','wood collision a1',5,8),('HIT_GLASS','ice light collision a1',5,8),('HIT_STONE','rock collision a1',5,8),
 ('HIT_RED','bird 01 collision a1',6,6),('HIT_BLUE','bird 02 collision a1',6,6),('HIT_YELLOW','bird 03 collision a1',6,6),
 ('HIT_BLACK','bird 04 collision a1',6,6),('HIT_WHITE','bird 05 collision a1',6,6),
 ('DMG_WOOD','wood damage a1',5,10),('DMG_GLASS','light damage a1',5,10),('DMG_STONE','rock damage a1',5,10),
 ('NEXT_BIRD','bird next military a1',7,0),
 ('HIT_PIG','piglette collision a1',5,8),('DMG_PIG','piglette damage a1',5,10),('OINK','piglette oink a1',2,0)]
# Variants (the originals pick one of several recordings at random): for each group the base id keeps `a1` and the extra ids are `a2`, `a3`..
# (id, file prefix, variants). The extra ids come after every other effect, group by group, in this order; ps2/src/audio_ids.h mirrors it (audio_variants).
VARIANT_GROUPS=[('WOOD','wood destroyed',3),('GLASS','light destroyed',3),('STONE','rock destroyed',3),
 ('HIT_WOOD','wood collision',3),('HIT_GLASS','ice light collision',3),('HIT_STONE','rock collision',3),
 ('DMG_WOOD','wood damage',3),('DMG_GLASS','light damage',3),('DMG_STONE','rock damage',3)]
CLASSIC_VARIANT_GROUPS=[('HIT_PIG','piglette collision',3),('DMG_PIG','piglette damage',3),('OINK','piglette oink',3)]   # after the shared groups; Classic only
def variant_sounds(entries,groups=None):
    """The extra entries of VARIANT_GROUPS, with the channel and cooldown of their base effect."""
    by={n:(ch,cd) for n,_,ch,cd in entries};out=[]
    for base,prefix,count in (VARIANT_GROUPS if groups is None else groups):
        for k in range(2,count+1):out.append((f'{base}_V{k}',f'{prefix} a{k}',by[base][0],by[base][1]))
    return out
# Music (audio.lua clip names): title_theme loops in the menus (gamelogic.lua currentMainMenuSong),
# level_complete plays once when the stage is won (level_end.lua LevelComplete.onEntry).
# Ambience of each theme inside the stages (themes.lua music -> audio.lua clip file), looped like
# gamelogic.lua does with currentMusic. The first two tracks stay in memory; one ambience at a time
# is read from the disc when a stage of another theme starts. Clip names in the third column.
MUSIC=[('TITLE','title_theme',1,'title_theme'),('LEVEL_COMPLETE','level_complete',0,'level_complete'),
 ('DRYFOREST','ambient_white_dryforest',1,'ambient_theme1'),('JUNGLE','ambient_green_jungleish',1,'ambient_theme2'),
 ('SAVANNAH','ambient_red_savannah',1,'ambient_theme3'),('CITY','ambient_city',1,'ambient_theme7'),
 ('CONSTRUCTION','ambient_construction',1,'construction_theme1'),('CAVE','ab_cave_ambient',1,'ambient_cave'),
 ('BIRTHDAY','birthday_ambience',1,'birthday_ambience'),('BEACH','BeachAmbience',1,'beach_ambience'),
 ('WATERFALL','ambience_waterfall',1,'ambience_waterfall'),('RAIN','RainAmbience',1,'rain_ambience'),
 ('BLACKBIRD','ambience_blackbird_01',1,'ambience_blackbird_01'),
 ('INTRO','birds_intro',0,'birds_intro'),
 ('BOSS','birds_boss',0,'birds_boss'),
 ('OUTRO','birds_outro',0,'birds_outro'),
 ('EGG1','level_0',1,'eggDefenderMusic1'),('EGG2','level_1',1,'eggDefenderMusic2'),('EGG3','level_2',1,'eggDefenderMusic3'),
 ('EGG4','level_3_1',1,'eggDefenderMusic4'),('EGG5','level_4_2',1,'eggDefenderMusic5')]   # Egg Defender (EggDefender.lua setMusic "eggDefenderMusic"..musicIntensityLevel): the music gets more intense with every wave   # cutscenes.lua: birds_intro (episode openings), birds_boss (middles), birds_outro (endings)   # cutscenes.lua pack1_intro playsound birds_intro (once)
MUSIC_OF_CLIP={clip:i for i,(_,_,_,clip) in enumerate(MUSIC)}
MUSIC_RATE=22050
def write_music_bin(apk,out,ffmpeg,folder):
 """MUSIC.BIN streamed by the game: "PBMU", version 1, count, data start, then (offset,bytes,loop)
 per track, then signed 16-bit little-endian mono PCM at 22050 Hz."""
 tracks=[];pcm=bytearray()
 with zipfile.ZipFile(apk) as archive:
  for name,filename,loop,_ in MUSIC:
   source='assets/data/audio/music/'+filename+'.ogg';ogg=archive.read(source)
   if not ogg.startswith(b'OggS'):raise ValueError('Unsupported audio container: '+source)
   p=Path(folder)/'music.ogg';raw=Path(folder)/'music.raw';p.write_bytes(ogg)
   subprocess.run([ffmpeg,'-v','error','-y','-i',str(p),'-ar',str(MUSIC_RATE),'-ac','1','-f','s16le',str(raw)],check=True)
   data=raw.read_bytes()
   while len(pcm)%2048:pcm.append(0)
   tracks.append({'id':name,'source':source,'source_sha256':hashlib.sha256(ogg).hexdigest(),'offset':len(pcm),'bytes':len(data),'loop':loop,'clip':MUSIC[len(tracks)][3],'seconds':round(len(data)/2/MUSIC_RATE,2),'pcm_sha256':hashlib.sha256(data).hexdigest()})
   pcm+=data
 start=(16+12*len(tracks)+2047)//2048*2048
 head=struct.pack('<4s3I',b'PBMU',1,len(tracks),start)+b''.join(struct.pack('<3I',t['offset'],t['bytes'],t['loop']) for t in tracks)
 (out/'MUSIC.BIN').write_bytes(head+bytes(start-len(head))+bytes(pcm))
 (out/'music_manifest.json').write_text(json.dumps({'format':'s16le mono PCM','sample_rate':MUSIC_RATE,'tracks':tracks},indent=2)+'\n')
 return tracks
def write_sounds_bin(entries,bank,path):
 """SOUNDS.BIN read by the game: "PBSN", version, count, data start, (offset,size) per effect, samples."""
 start=(16+8*len(entries)+63)//64*64
 head=struct.pack('<4s3I',b'PBSN',1,len(entries),start)+b''.join(struct.pack('<2I',e['offset'],e['bytes']) for e in entries)
 Path(path).write_bytes(head+bytes(start-len(head))+bytes(bank))
def extract(apk,out,ffmpeg,adpenc):
 bank=bytearray();entries=[]
 with zipfile.ZipFile(apk) as archive,tempfile.TemporaryDirectory(prefix='ps2birds-audio-') as folder:
  for name,filename,channel,cooldown in list(SOUNDS)+CLASSIC_ONLY+variant_sounds(list(SOUNDS)+CLASSIC_ONLY,VARIANT_GROUPS+CLASSIC_VARIANT_GROUPS):
   source='assets/data/audio/sfx/'+filename+'.ogg';ogg=archive.read(source)
   if not ogg.startswith(b'OggS'):raise ValueError('Unsupported audio container: '+source)
   p=Path(folder)/'input.ogg';wav=Path(folder)/'effect.wav';encoded=Path(folder)/'effect.adp';p.write_bytes(ogg)
   subprocess.run([ffmpeg,'-v','error','-y','-i',str(p),'-ar','22050','-ac','1','-c:a','pcm_s16le',str(wav)],check=True)
   if adpenc:subprocess.run([adpenc,str(wav),str(encoded)],capture_output=True,check=True)
   with wave.open(str(wav)) as w:
    frames=w.getnframes();pcm=w.readframes(frames)
    if w.getframerate()!=22050 or w.getnchannels()!=1 or w.getsampwidth()!=2:raise ValueError('Invalid converted PCM')
   if not adpenc:
    from spu_adpcm import encode;encoded.write_bytes(encode(pcm))
   b=encoded.read_bytes()
   if len(b)<32 or len(b)%16 or b[:4]!=b'APCM' or b[5]!=1 or b[6]!=0:raise ValueError('Invalid or looping SPU sample')
   while len(bank)%64:bank.append(0)
   entries.append({'id':name,'source':source,'source_sha256':hashlib.sha256(ogg).hexdigest(),'offset':len(bank),'bytes':len(b),'channel':channel,'cooldown_frames':cooldown,'sample_rate':22050,'frames':frames,'pcm_sha256':hashlib.sha256(pcm).hexdigest(),'adpcm_sha256':hashlib.sha256(b).hexdigest()})
   bank+=b
 if len(bank)>1048576:raise ValueError('Effects exceed the 1 MiB SPU2 budget')
 out.mkdir(parents=True,exist_ok=True);(out/'sounds.adp').write_bytes(bank)
 write_sounds_bin(entries,bank,out/'SOUNDS.BIN')
 with tempfile.TemporaryDirectory(prefix='ps2birds-music-') as folder:tracks=write_music_bin(apk,out,ffmpeg,folder)
 (out/'audio_manifest.json').write_text(json.dumps({'format':'PS2SDK APCM / SPU2 ADPCM','sample_rate':22050,'channels':1,'bytes':len(bank),'spu_budget':1048576,'voice_channels':10,'bank_sha256':hashlib.sha256(bank).hexdigest(),'sounds':entries},indent=2)+'\n')
 lines=['/* Generated by tools/extract_audio.py. */','#ifndef PS2BIRDS_AUDIO_DATA_H','#define PS2BIRDS_AUDIO_DATA_H']
 for i,e in enumerate(entries):lines.append('#define SFX_'+e['id']+' '+str(i))
 lines+=['#define SFX_CHANNELS 10','#define SFX_COUNT '+str(len(entries)),'typedef struct {int offset,size,channel,cooldown;} AudioAsset;','static const AudioAsset audio_assets[]={']
 for e in entries:lines.append(' {'+','.join(str(e[k]) for k in ['offset','bytes','channel','cooldown_frames'])+'},')
 lines+=['};','#endif',''];(out/'audio_data.h').write_text('\n'.join(lines))
 print(f'{len(entries)} original sound effects, {len(bank)} bytes, 22050 Hz mono SPU2 ADPCM; {len(tracks)} music tracks')
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--ffmpeg',default='ffmpeg');p.add_argument('--adpenc',default=None,help='optional PS2SDK adpenc; the built-in encoder is used by default');a=p.parse_args();extract(a.apk,a.output,a.ffmpeg,a.adpenc)
