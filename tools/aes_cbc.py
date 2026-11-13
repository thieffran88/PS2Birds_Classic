#!/usr/bin/env python3
"""Pure-Python AES-256-CBC decryption (FIPS-197) with PKCS#7 padding, replacing the
OpenSSL command so the extraction runs on Windows without extra programs."""
def _xt(a):return ((a<<1)^0x1B)&0xFF if a&0x80 else a<<1
_S=[0]*256;_IS=[0]*256
def _init():
    p=q=1
    while True:
        p=p^(p<<1)^(0x1B if p&0x80 else 0);p&=0xFF
        q^=q<<1;q^=q<<2;q^=q<<4;q&=0xFF
        if q&0x80:q^=0x09
        x=q^((q<<1|q>>7)&0xFF)^((q<<2|q>>6)&0xFF)^((q<<3|q>>5)&0xFF)^((q<<4|q>>4)&0xFF)^0x63
        _S[p]=x;_IS[x]=p
        if p==1:break
    _S[0]=0x63;_IS[0x63]=0
_init()
def _mul(a,b):
    r=0
    while b:
        if b&1:r^=a
        a=_xt(a);b>>=1
    return r
_M9=[_mul(i,9) for i in range(256)];_M11=[_mul(i,11) for i in range(256)];_M13=[_mul(i,13) for i in range(256)];_M14=[_mul(i,14) for i in range(256)]
def _expand(key):
    nk=len(key)//4;nr=nk+6;w=[list(key[4*i:4*i+4]) for i in range(nk)];rc=1
    for i in range(nk,4*(nr+1)):
        t=list(w[i-1])
        if i%nk==0:t=[_S[t[1]]^rc,_S[t[2]],_S[t[3]],_S[t[0]]];rc=_xt(rc)
        elif nk>6 and i%nk==4:t=[_S[b] for b in t]
        w.append([w[i-nk][j]^t[j] for j in range(4)])
    return [sum(w[4*r:4*r+4],[]) for r in range(nr+1)]
def _decrypt_block(rk,b):
    nr=len(rk)-1;s=[b[i]^rk[nr][i] for i in range(16)]
    for r in range(nr-1,-1,-1):
        s=[s[(i-4*(i%4))%16] for i in range(16)]  # inverse shift rows
        s=[_IS[x]^rk[r][i] for i,x in enumerate(s)]
        if r:
            n=[]
            for c in range(4):
                a0,a1,a2,a3=s[4*c:4*c+4]
                n+=[_M14[a0]^_M11[a1]^_M13[a2]^_M9[a3],_M9[a0]^_M14[a1]^_M11[a2]^_M13[a3],_M13[a0]^_M9[a1]^_M14[a2]^_M11[a3],_M11[a0]^_M13[a1]^_M9[a2]^_M14[a3]]
            s=n
    return s
def decrypt(key,data,iv=bytes(16)):
    if len(data)%16 or not data:raise ValueError('AES input is not a whole number of blocks')
    rk=_expand(key);out=bytearray();prev=iv
    for o in range(0,len(data),16):
        block=data[o:o+16];out+=bytes(x^y for x,y in zip(_decrypt_block(rk,block),prev));prev=block
    pad=out[-1]
    if not 1<=pad<=16 or out[-pad:]!=bytes([pad])*pad:raise ValueError('AES failed: wrong key or damaged input')
    return bytes(out[:-pad])
