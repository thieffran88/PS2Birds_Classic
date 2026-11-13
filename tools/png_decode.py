#!/usr/bin/env python3
"""Small PNG reader (8-bit greyscale/RGB/palette/RGBA, with or without tRNS, non-interlaced)
returning RGBA bytes, so the extraction needs no compiled image library."""
import struct,zlib

def decode(data):
    if data[:8]!=b'\x89PNG\r\n\x1a\n':raise ValueError('Not a PNG file')
    p=8;idat=bytearray();palette=None;trns=None;head=None
    while p<len(data):
        n,kind=struct.unpack_from('>I4s',data,p);body=data[p+8:p+8+n];p+=12+n
        if kind==b'IHDR':head=struct.unpack('>IIBBBBB',body)
        elif kind==b'PLTE':palette=body
        elif kind==b'tRNS':trns=body
        elif kind==b'IDAT':idat+=body
        elif kind==b'IEND':break
    w,h,depth,color,_,_,interlace=head
    if depth!=8 or interlace or color not in (0,2,3,4,6):raise ValueError('Unsupported PNG layout')
    bpp={0:1,2:3,3:1,4:2,6:4}[color];stride=w*bpp
    raw=zlib.decompress(bytes(idat));out=bytearray(h*stride);prev=bytearray(stride)
    for y in range(h):
        f=raw[y*(stride+1)];line=bytearray(raw[y*(stride+1)+1:(y+1)*(stride+1)])
        if f==1:
            for i in range(bpp,stride):line[i]=(line[i]+line[i-bpp])&255
        elif f==2:
            for i in range(stride):line[i]=(line[i]+prev[i])&255
        elif f==3:
            for i in range(stride):line[i]=(line[i]+((line[i-bpp] if i>=bpp else 0)+prev[i])//2)&255
        elif f==4:
            for i in range(stride):
                a=line[i-bpp] if i>=bpp else 0;b=prev[i];c=prev[i-bpp] if i>=bpp else 0
                pa=abs(b-c);pb=abs(a-c);pc=abs(a+b-2*c)
                line[i]=(line[i]+(a if pa<=pb and pa<=pc else b if pb<=pc else c))&255
        elif f:raise ValueError('Bad PNG filter')
        out[y*stride:(y+1)*stride]=line;prev=line
    if color==6:return w,h,bytes(out)
    rgba=bytearray(w*h*4)
    if color==2:
        key=struct.unpack('>HHH',trns) if trns else None
        rgba[0::4]=out[0::3];rgba[1::4]=out[1::3];rgba[2::4]=out[2::3];rgba[3::4]=b'\xff'*(w*h)
        if key:
            for i in range(w*h):
                if (out[3*i],out[3*i+1],out[3*i+2])==key:rgba[4*i+3]=0
    elif color in (0,4):
        g=out[0::bpp];rgba[0::4]=g;rgba[1::4]=g;rgba[2::4]=g
        rgba[3::4]=out[1::2] if color==4 else b'\xff'*(w*h)
    else:
        alpha=bytes(trns or b'')+b'\xff'*256
        table=[bytes(palette[3*i:3*i+3])+alpha[i:i+1] if 3*i+3<=len(palette) else b'\0\0\0\xff' for i in range(256)]
        rgba=bytearray(b''.join(table[v] for v in out))
    return w,h,bytes(rgba)
