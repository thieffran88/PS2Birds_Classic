import os
import struct,zipfile,json,collections
from pathlib import Path
r=Path(__file__).resolve().parents[2]/"work/rio/analysis";out=[];errors=[]
with zipfile.ZipFile(os.environ.get('RIO_APK_PATH',str(r.parent/'input/com.rovio.angrybirdsrio.apk'))) as z:
 for n in z.namelist():
  if not n.endswith('.dat') or '/images/' not in n:continue
  d=z.read(n)
  if d[8:12]!=b'COMP':continue
  try:
   pos=16
   def u16():
    global pos
    v=struct.unpack_from('>H',d,pos)[0];pos+=2;return v
   def string():
    global pos
    ln=u16();v=d[pos:pos+ln].decode();pos+=ln;return v
   version=u16();count=u16();composites=[]
   if version not in (1,2):raise ValueError('unhandled version '+str(version))
   for i in range(count):
    name=string();nc=u16();children=[]
    for j in range(nc):
     sprite=string()
     if version==2:
      dx,dy=struct.unpack_from('>hh',d,pos);pos+=4;children.append({'sprite':sprite,'dx_raw':dx,'dy_raw':dy})
     else:
      a,dx,dy,fx,fy,fz,b=struct.unpack_from('>HhhfffH',d,pos);pos+=20
      children.append({'sprite':sprite,'dx_raw':dx,'dy_raw':dy,'raw_float_fields':[fx,fy,fz],'raw_u16_fields':[a,b]})
    tail=u16() if version==2 else None;composites.append({'name':name,'children':children,'trailing_raw_u16':tail})
   if pos!=len(d):raise ValueError('trailing bytes '+str(len(d)-pos))
   out.append({'path':n,'version':version,'composites':composites})
  except Exception as e:errors.append({'path':n,'error':str(e),'cursor':pos})
(r/'comp_inventory.json').write_text(json.dumps({'parsed':out,'errors':errors,'semantics':'Structural reading only; offsets and trailing field require rendering/engine confirmation.'},indent=2))
print('COMP parsed',len(out),'errors',len(errors),errors[:3],'records',sum(len(x['composites']) for x in out))
