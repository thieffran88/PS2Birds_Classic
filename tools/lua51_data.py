#!/usr/bin/env python3
"""Read data-only Lua 5.1 chunks (level tables) straight from bytecode, without a Lua
runtime or decompiler. Only table/constant opcodes are evaluated; any call, closure,
arithmetic other than unary minus, or jump makes the reader refuse the chunk, so no
game code can run. Returns the globals the chunk assigns, in assignment order."""
import struct

class Unsupported(ValueError):pass

def _float32(raw):
    value=struct.unpack('<f',raw)[0]
    # Shortest decimal that maps back to the same float32 (what a decompiler prints).
    for digits in range(1,18):
        text='%.*g'%(digits,value)
        if struct.unpack('<f',struct.pack('<f',float(text)))[0]==value:return float(text)
    return value

class _Reader:
    def __init__(self,data):self.d=data;self.p=0
    def take(self,n):
        if self.p+n>len(self.d):raise Unsupported('Truncated chunk')
        b=self.d[self.p:self.p+n];self.p+=n;return b
    def u8(self):return self.take(1)[0]
    def u32(self):return struct.unpack('<I',self.take(4))[0]
    def string(self):
        n=self.u32()
        return None if n==0 else self.take(n)[:-1].decode('utf-8')

def _function(r):
    r.string();r.u32();r.u32();r.u8();r.u8();r.u8();r.u8()
    code=[r.u32() for _ in range(r.u32())]
    consts=[]
    for _ in range(r.u32()):
        t=r.u8()
        if t==0:consts.append(None)
        elif t==1:consts.append(bool(r.u8()))
        elif t==3:consts.append(_float32(r.take(4)))
        elif t==4:consts.append(r.string())
        else:raise Unsupported('Constant type %d'%t)
    if r.u32():raise Unsupported('Nested functions are not data')
    for _ in range(r.u32()):r.u32()
    for _ in range(r.u32()):r.string();r.u32();r.u32()
    for _ in range(r.u32()):r.string()
    return code,consts

def load(data):
    """Return the dict of globals assigned by a data-only Lua 5.1 chunk."""
    if data[:5]!=b'\x1bLuaQ' or data[5:12]!=bytes([0,1,4,4,4,4,0]):raise Unsupported('Not a Lua 5.1 float32 little-endian chunk')
    r=_Reader(data);r.p=12;code,K=_function(r)
    reg={};G={}
    def RK(x):return K[x-256] if x>=256 else reg.get(x)
    for i in code:
        op=i&63;A=(i>>6)&255;C=(i>>14)&511;B=(i>>23)&511;Bx=i>>14
        if op==0:reg[A]=reg.get(B)
        elif op==1:reg[A]=K[Bx]
        elif op==2:
            if C:raise Unsupported('Conditional skip')
            reg[A]=bool(B)
        elif op==3:
            for k in range(A,B+1):reg[k]=None
        elif op==5:reg[A]=G.get(K[Bx])
        elif op==6:reg[A]=reg[B].get(RK(C)) if isinstance(reg.get(B),dict) else None
        elif op==7:G[K[Bx]]=reg.get(A)
        elif op==9:
            t=reg.get(A)
            if not isinstance(t,dict):raise Unsupported('Index of non-table')
            t[RK(B)]=RK(C)
        elif op==10:reg[A]={}
        elif op==19:
            v=reg.get(B)
            if not isinstance(v,float):raise Unsupported('Unary minus of non-number')
            reg[A]=-v
        elif op==34:
            if B==0 or C==0:raise Unsupported('Open SETLIST')
            t=reg[A]
            for k in range(1,B+1):t[float((C-1)*50+k)]=reg.get(A+k)
        elif op==30:break
        else:raise Unsupported('Opcode %d is not data'%op)
    return G
