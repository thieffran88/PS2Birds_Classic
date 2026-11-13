#!/usr/bin/env python3
"""SPU2 ADPCM encoder producing the APCM files audsrv loads (header + 16-byte blocks), so the
extraction does not need the PS2SDK adpenc program. Standard PS-ADPCM: per 28-sample block
the predictor and shift with the smallest error are chosen, with feedback of the decoded
signal; the last block carries the end flag (7)."""
import struct
COEF=[(0,0),(60,0),(115,-52),(98,-55),(122,-60)]

def _encode_block(samples,h1,h2):
    best=None
    for f,(a,b) in enumerate(COEF):
        for shift in range(13):
            p1,p2=h1,h2;nibbles=[];err=0
            for s in samples:
                pred=(p1*a+p2*b+32)>>6
                q=((s-pred)<<shift)
                q=(q+2048)>>12
                q=max(-8,min(7,q))
                dec=max(-32768,min(32767,((q<<12)>>shift)+pred))
                err+=(s-dec)*(s-dec);nibbles.append(q&15);p2=p1;p1=dec
                if best and err>=best[0]:break
            else:
                if best is None or err<best[0]:best=(err,f,shift,nibbles,p1,p2)
    _,f,shift,nibbles,p1,p2=best
    data=bytes([(f<<4)|shift,0])+bytes(nibbles[i]|(nibbles[i+1]<<4) for i in range(0,28,2))
    return data,p1,p2

def encode(pcm,sample_rate=22050):
    """pcm: mono 16-bit little-endian samples. Returns an APCM file (no loop)."""
    samples=list(struct.unpack('<%dh'%(len(pcm)//2),pcm[:len(pcm)//2*2]))
    frames=len(samples);samples+=[0]*((-len(samples))%28)
    out=bytearray(struct.pack('<4s4BII',b'APCM',1,1,0,0,sample_rate*4096//48000,frames))
    h1=h2=0
    for i in range(0,len(samples),28):
        block,h1,h2=_encode_block(samples[i:i+28],h1,h2);out+=block
    out+=bytes([0,7])+bytes(14)
    return bytes(out)
