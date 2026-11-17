"""Development check of tools/webp.py against Pillow's decoder (needs Pillow; the reader itself does not).
usage: webp_check.py APK [NAME ...]  -> mean and largest difference per channel for each sheet"""
import io,sys,zipfile
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import webp
def check(apk,names):
    from PIL import Image
    with zipfile.ZipFile(apk) as z:
        for n in z.namelist():
            if not n.endswith('.webp') or (names and not any(k in n for k in names)):continue
            data=z.read(n);ref=Image.open(io.BytesIO(data)).convert('RGBA');w,h,px=webp.decode_webp(data)
            if (w,h)!=ref.size:print(n.split('/')[-1],'tamanho difere',(w,h),ref.size);continue
            r=ref.tobytes();diff=[0,0,0,0];mx=[0,0,0,0]
            for i in range(0,len(r),4):
                for k in range(4):
                    d=abs(r[i+k]-px[i+k]);diff[k]+=d
                    if d>mx[k]:mx[k]=d
            n_px=w*h;print(n.split('/')[-1],w,'x',h,'| média',[round(v/n_px,3) for v in diff],'| máx',mx)
if __name__=='__main__':check(sys.argv[1],sys.argv[2:])
