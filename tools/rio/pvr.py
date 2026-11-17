"""PVR v3 reader for the uncompressed textures of the Rio APK (RGBA4444, RGB565, alpha8).
Pure Python; ETC1 and the other compressed formats are reported, not decoded."""
import struct
def read_pvr(b):
    if b[:4]!=b'PVR\x03':raise ValueError('não é PVR v3')
    flags,pf,cs,ct,h,w,depth,surf,faces,mips,meta=struct.unpack_from("<IQIIIIIIIII",b,4)
    data=b[52+meta:]
    chans=bytes(pf.to_bytes(8,'little')[:4]).decode('latin1'),tuple(pf.to_bytes(8,'little')[4:])
    if pf>>32==0:raise ValueError('PVR comprimido (%d)'%pf)
    names,bits=chans
    if names=='rgba' and bits==(4,4,4,4):
        out=bytearray(w*h*4)
        for i in range(w*h):
            v=data[i*2]|data[i*2+1]<<8
            # checked on the sheets: red in the top nibble, then green, blue and alpha at the bottom
            out[i*4]=(v>>12&15)*17;out[i*4+1]=(v>>8&15)*17;out[i*4+2]=(v>>4&15)*17;out[i*4+3]=(v&15)*17
        return w,h,bytes(out)
    if names=='rgb\x00' and bits==(5,6,5,0):
        out=bytearray(w*h*4)
        for i in range(w*h):
            v=data[i*2]|data[i*2+1]<<8
            out[i*4]=(v&31)*255//31;out[i*4+1]=(v>>5&63)*255//63;out[i*4+2]=(v>>11&31)*255//31;out[i*4+3]=255
        return w,h,bytes(out)
    if names[0]=='a' and bits[0]==8:
        out=bytearray(w*h*4)
        for i in range(w*h):out[i*4:i*4+4]=bytes((255,255,255,data[i]))
        return w,h,bytes(out)
    raise ValueError('formato PVR sem suporte: %s %s'%(names,bits))

if __name__=='__main__':
    import sys,zipfile,collections
    # usage: pvr.py APK  -> decodes every .pvr of the APK and counts formats
    ok=collections.Counter();bad=collections.Counter()
    with zipfile.ZipFile(sys.argv[1]) as z:
        for n in z.namelist():
            if not n.endswith('.pvr'):continue
            try:read_pvr(z.read(n));ok['legível']+=1
            except ValueError as e:bad[str(e).split(' (')[0]]+=1
    print('decodificadas:',dict(ok),'| não decodificadas:',dict(bad))
