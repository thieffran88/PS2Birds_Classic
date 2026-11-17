"""PVR v3 reader for the uncompressed textures of the Rio APK (RGBA4444, RGB565, alpha8).
Pure Python; ETC1 and the other compressed formats are reported, not decoded."""
import struct
_ETC1_MOD=((2,8),(5,17),(9,29),(13,42),(18,60),(24,80),(33,106),(47,183))
def _clamp(v):return 0 if v<0 else 255 if v>255 else v
def etc1(data,w,h):
    """ETC1 (4x4 blocks of 8 bytes, rows of blocks left to right, big-endian fields) -> RGBA, opaque."""
    bw=(w+3)//4;bh=(h+3)//4;out=bytearray(w*h*4)
    for by in range(bh):
        for bx in range(bw):
            o=(by*bw+bx)*8;blk=data[o:o+8]
            if len(blk)<8:continue
            r,g,b=blk[0],blk[1],blk[2];cw1=blk[3]>>5&7;cw2=blk[3]>>2&7;diff=blk[3]>>1&1;flip=blk[3]&1
            if diff:
                r1,g1,b1=r>>3,g>>3,b>>3
                sd=lambda v:(v&7)-8 if v&4 else v&7
                r2,g2,b2=r1+sd(r&7),g1+sd(g&7),b1+sd(b&7)
                c1=tuple((v<<3)|(v>>2) for v in (r1,g1,b1));c2=tuple((v<<3)|(v>>2) for v in (r2&31,g2&31,b2&31))
            else:
                c1=tuple((v>>4)*17 for v in (r,g,b));c2=tuple((v&15)*17 for v in (r,g,b))
            word=(blk[4]<<24)|(blk[5]<<16)|(blk[6]<<8)|blk[7]
            for x in range(4):
                for y in range(4):
                    px=bx*4+x;py=by*4+y
                    if px>=w or py>=h:continue
                    k=x*4+y;sub=(y>=2) if flip else (x>=2)
                    base=c2 if sub else c1;t=_ETC1_MOD[cw2 if sub else cw1]
                    idx=((word>>(k+16))&1)<<1|((word>>k)&1)
                    m=(t[0],t[1],-t[0],-t[1])[idx];i=(py*w+px)*4
                    out[i]=_clamp(base[0]+m);out[i+1]=_clamp(base[1]+m);out[i+2]=_clamp(base[2]+m);out[i+3]=255
    return bytes(out)
def read_pvr(b):
    if b[:4]!=b'PVR\x03':raise ValueError('não é PVR v3')
    flags,pf,cs,ct,h,w,depth,surf,faces,mips,meta=struct.unpack_from("<IQIIIIIIIII",b,4)
    data=b[52+meta:]
    chans=bytes(pf.to_bytes(8,'little')[:4]).decode('latin1'),tuple(pf.to_bytes(8,'little')[4:])
    if pf==6:return w,h,etc1(data,w,h)   # ETC1 (PVR pixel format 6)
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
