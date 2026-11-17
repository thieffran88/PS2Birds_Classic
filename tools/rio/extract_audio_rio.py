"""SOUNDS.BIN for the Rio build, from the player's own APK. The runtime plays effects by the ids of
ps2/src/audio_ids.h (the Classic's list), so the Rio bank keeps the same ids and order and fills each one
with the equivalent Rio file: the effect file of the same name when the Rio has it, or the file named in the
table below (menu sounds, defeat, goal destroyed). Same conversion as the Classic (22050 Hz mono SPU2 ADPCM).
usage: extract_audio_rio.py APK OUTDIR [--ffmpeg ffmpeg]"""
import argparse,hashlib,json,subprocess,sys,tempfile,wave,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
import extract_audio as classic
from spu_adpcm import encode
# Classic sound id -> Rio file (without extension) where the Classic name does not exist in the Rio.
OVERRIDE={'MENU_MOVE':'ui_select','MENU_CONFIRM':'ui_select','MENU_BACK':'ui_select',
          'PIG':'bird cage breaks 1',            # the goal of a Rio stage is a cage
          'LOSE':'level_failed_birds'}
def main(apk,out,ffmpeg):
    bank=bytearray();entries=[];missing=[]
    with zipfile.ZipFile(apk) as z,tempfile.TemporaryDirectory(prefix='ps2birds-rio-audio-') as folder:
        files={n.rsplit('/',1)[1][:-4]:n for n in z.namelist() if '/audio/' in n and n.endswith('.mp3')}
        for name,filename,channel,cooldown in classic.SOUNDS:
            base=OVERRIDE.get(name,filename)
            if base not in files:missing.append((name,base));continue
            src=files[base];p=Path(folder)/'in.mp3';wav=Path(folder)/'e.wav';p.write_bytes(z.read(src))
            subprocess.run([ffmpeg,'-v','error','-y','-i',str(p),'-ar','22050','-ac','1','-c:a','pcm_s16le',str(wav)],check=True)
            with wave.open(str(wav)) as w:frames=w.getnframes();pcm=w.readframes(frames)
            b=encode(pcm)
            while len(bank)%64:bank.append(0)
            entries.append({'id':name,'source':src,'offset':len(bank),'bytes':len(b),'channel':channel,'cooldown_frames':cooldown,'frames':frames});bank+=b
    if missing:raise SystemExit('sem arquivo no APK: %s'%missing)
    if len(bank)>1048576:raise SystemExit('os efeitos passam de 1 MiB de SPU2')
    out=Path(out);out.mkdir(parents=True,exist_ok=True);classic.write_sounds_bin(entries,bank,out/'SOUNDS.BIN')
    (out/'audio_manifest.json').write_text(json.dumps({'game':'rio','bytes':len(bank),'effects':entries},indent=1))
    print('SOUNDS.BIN do Rio: %d efeitos, %d bytes (limite 1048576)'%(len(entries),len(bank)))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('apk',type=Path);p.add_argument('output',type=Path);p.add_argument('--ffmpeg',default='ffmpeg')
    a=p.parse_args();main(a.apk,a.output,a.ffmpeg)
