#!/usr/bin/env python3
"""Minimal reader for single-file, LZMA-only 7z archives (the APK's *.stream.7z sheets),
so the extraction needs neither 7-Zip nor compiled Python packages."""
import lzma,struct,sys

def _num(d,p):
    first=d[p];mask=0x80;value=0
    for i in range(8):
        if not first&mask:
            return value|((first&(mask-1))<<(8*i)),p+1+i
        value|=d[p+1+i]<<(8*i);mask>>=1
    return value,p+9

def extract(data):
    if data[:6]!=b'7z\xbc\xaf\x27\x1c':raise ValueError('Not a 7z archive')
    off,size,_=struct.unpack_from('<QQI',data,12);h=data[32+off:32+off+size]
    if h[:3]!=b'\x01\x04\x06':raise ValueError('Unsupported 7z header (encoded or multi-part)')
    p=3;pack_pos,p=_num(h,p);streams,p=_num(h,p)
    if streams!=1 or h[p]!=0x09:raise ValueError('Unsupported 7z pack info')
    pack_size,p=_num(h,p+1)
    method=h.find(b'\x03\x01\x01\x05')
    if method<0:raise ValueError('7z method is not LZMA')
    props=h[method+4:method+9];unpack=h.find(b'\x0c',method+9)
    unpack_size,_=_num(h,unpack+1)
    lc=props[0]%9;lp=(props[0]//9)%5;pb=props[0]//45;dict_size=struct.unpack('<I',props[1:5])[0]
    dec=lzma.LZMADecompressor(lzma.FORMAT_RAW,filters=[{'id':lzma.FILTER_LZMA1,'dict_size':dict_size,'lc':lc,'lp':lp,'pb':pb}])
    out=dec.decompress(data[32+pack_pos:32+pack_pos+pack_size],max_length=unpack_size)
    if len(out)!=unpack_size:raise ValueError('7z stream shorter than declared')
    return out

if __name__=='__main__':
    # Drop-in for "7z x -so archive" as used by the extraction tools.
    if len(sys.argv)<4 or sys.argv[1:3]!=['x','-so']:sys.exit('usage: mini7z.py x -so archive.7z')
    sys.stdout.buffer.write(extract(open(sys.argv[3],'rb').read()))
