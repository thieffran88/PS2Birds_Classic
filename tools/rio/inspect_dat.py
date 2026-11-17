import os
import zipfile,struct,collections,json,io
from pathlib import Path
from PIL import Image
ROOT=Path(__file__).resolve().parents[2];apk=os.environ.get('RIO_APK_PATH',str(ROOT/'input/com.rovio.angrybirdsrio.apk'));results=[];errors=[];types=collections.Counter();pvr=collections.Counter();formats=collections.Counter()
with zipfile.ZipFile(apk) as z:
 names=set(z.namelist())
 for name in sorted(names):
  if not name.endswith('.dat') or '/images/' not in name:continue
  d=z.read(name);types[d[8:12].decode('ascii',errors='replace')]+=1
  if d[:4]!=b'KA3D' or d[8:12]!=b'SPRT':continue
  try:
   if struct.unpack_from('>I',d,4)[0]!=len(d)-8:raise ValueError('KA3D length')
   pos=16
   def h():
    global pos
    v=struct.unpack_from('>H',d,pos)[0];pos+=2;return v
   def string():
    global pos
    n=h();s=d[pos:pos+n].decode();pos+=n;return s
   version=h();texture=string();count=h();frames=[]
   for _ in range(count):
    sprite=string();x,y,w,ht,px,py=struct.unpack_from('>4H2h',d,pos);pos+=12;frames.append({'name':sprite,'x':x,'y':y,'w':w,'h':ht,'px':px,'py':py})
   if pos!=len(d):raise ValueError(f'trailing {len(d)-pos} bytes')
   texture_path=name.rsplit('/',1)[0]+'/'+texture;alternatives=[texture_path.rsplit('.',1)[0]+ext for ext in ['.png','.webp','.pvr'] if texture_path.rsplit('.',1)[0]+ext in names]
   dimensions=None;selected=next((a for a in alternatives if not a.endswith('.pvr')),None)
   if selected:
    with Image.open(io.BytesIO(z.read(selected))) as img:
     dimensions=list(img.size);formats[img.format]+=1
     assert all(f['x']+f['w']<=img.width and f['y']+f['h']<=img.height for f in frames)
   results.append({'path':name,'version':version,'texture':texture,'available':alternatives,'dimensions':dimensions,'frames':frames})
  except Exception as e:errors.append({'path':name,'error':str(e)})
 for name in names:
  if name.endswith('.pvr'):
   data=z.read(name);pvr[data[:4].hex()]+=1
(ROOT/'work/rio/analysis/dat_inventory.json').write_text(json.dumps({'types':dict(types),'parsed':results,'errors':errors,'pvr_magic':dict(pvr),'verified_image_formats':dict(formats)},indent=2))
print('DAT types',dict(types),'parsed sheets',len(results),'frames',sum(len(r['frames']) for r in results),'errors',errors[:3]);print('image formats',dict(formats),'PVR magic',dict(pvr));print('image-backed sheets',sum(r['dimensions'] is not None for r in results),'PVR-only',sum(r['dimensions'] is None for r in results))
