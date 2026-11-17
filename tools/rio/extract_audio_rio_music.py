"""MUSIC.BIN ("PBMU" v1) for the Rio build, from the player's own APK. The runtime streams tracks by the
numbers of the Classic list (extract_audio.MUSIC), so the Rio file keeps the same count and order: each
track is filled with the Rio music that fits it (table below) and the others with one silent chunk.
Mapping by name is an approximation (no ear check): change the table to taste.
usage: extract_audio_rio_music.py APK OUTDIR [--ffmpeg ffmpeg]"""
import argparse,json,struct,subprocess,sys,tempfile,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
import extract_audio as classic
# Classic track id -> Rio file (without extension). Tracks not listed are silent.
RIO_MUSIC={'TITLE':'angry_birds_rio2_theme',    # the only theme-song file in the APK (Rio 2 title)
           'JUNGLE':'ambience jungle','BEACH':'ambience beach','CITY':'ambience airport',
           'BIRTHDAY':'ambience carnival','CAVE':'ab rio ambience warehouse',   # warehouse ambience of the first episode
           'OUTRO':'story_music','BOSS':'ab_theme_rio_boss_lite5','INTRO':'story_music'}
def main(apk,out,ffmpeg):
    tracks=[];pcm=bytearray();missing=[]
    with zipfile.ZipFile(apk) as z,tempfile.TemporaryDirectory(prefix='ps2birds-rio-music-') as folder:
        files={n.rsplit('/',1)[1][:-4]:n for n in z.namelist() if '/music/' in n and n.endswith('.mp3')}
        for name,_,loop,_clip in classic.MUSIC:
            base=RIO_MUSIC.get(name);data=bytes(2048)
            if base:
                if base not in files:missing.append(base)
                else:
                    p=Path(folder)/'m.mp3';raw=Path(folder)/'m.raw';p.write_bytes(z.read(files[base]))
                    subprocess.run([ffmpeg,'-v','error','-y','-i',str(p),'-ar',str(classic.MUSIC_RATE),'-ac','1','-f','s16le',str(raw)],check=True);data=raw.read_bytes()
            while len(pcm)%2048:pcm.append(0)
            tracks.append({'id':name,'source':files.get(base) if base else None,'offset':len(pcm),'bytes':len(data),'loop':loop});pcm+=data
    if missing:raise SystemExit('sem arquivo no APK: %s'%missing)
    start=(16+12*len(tracks)+2047)//2048*2048
    head=struct.pack('<4s3I',b'PBMU',1,len(tracks),start)+b''.join(struct.pack('<3I',t['offset'],t['bytes'],t['loop']) for t in tracks)
    out=Path(out);out.mkdir(parents=True,exist_ok=True);(out/'MUSIC.BIN').write_bytes(head+bytes(start-len(head))+bytes(pcm))
    (out/'music_manifest.json').write_text(json.dumps({'game':'rio','sample_rate':classic.MUSIC_RATE,'tracks':tracks},indent=1))
    print('MUSIC.BIN do Rio: %d faixas (%d com música), %d bytes'%(len(tracks),sum(1 for t in tracks if t['source']),len(head)+(start-len(head))+len(pcm)))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--ffmpeg',default='ffmpeg')
    a=p.parse_args();main(a.apk,a.output,a.ffmpeg)
