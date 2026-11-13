"""Tests of the pure-Python readers used by the ISO generator; no APK needed."""
import lzma,math,os,struct,subprocess,sys,tempfile,zlib
from pathlib import Path
root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'tools'))
import aes_cbc,lua51_data,mini7z,png_decode,spu_adpcm

# AES-256-CBC: FIPS-197 C.3 known answer (single block, PKCS#7 block appended) and padding check.
key=bytes(range(32));plain=bytes.fromhex('00112233445566778899aabbccddeeff')
cipher=bytes.fromhex('8ea2b7ca516745bfeafc49904b496089')
assert aes_cbc._decrypt_block(aes_cbc._expand(key),cipher)==list(plain)
try:aes_cbc.decrypt(key,cipher)
except ValueError:pass
else:raise AssertionError('bad padding accepted')

# Lua 5.1 data reader: hand-assembled chunk  t={}; t.a=1.5; t[2]=-3; g=t; h="x"
def instr(op,a=0,b=0,c=0,bx=None):return op|(a<<6)|((bx<<14) if bx is not None else (c<<14)|(b<<23))
def s(x):b=x.encode()+b'\0';return struct.pack('<I',len(b))+b
consts=[(4,'t'),(4,'a'),(3,1.5),(3,2.0),(3,3.0),(4,'g'),(4,'h'),(4,'x')]
code=[instr(10,0),instr(9,0,256+1,256+2),instr(1,1,bx=4),instr(19,1,1),instr(9,0,256+3,1),instr(7,0,bx=5),instr(1,2,bx=7),instr(7,2,bx=6),instr(30,0,1)]
body=s('@t')+struct.pack('<II4B',0,0,0,0,2,3)+struct.pack('<I',len(code))+b''.join(struct.pack('<I',i) for i in code)
body+=struct.pack('<I',len(consts))+b''.join(bytes([t])+(struct.pack('<f',v) if t==3 else s(v)) for t,v in consts)
body+=struct.pack('<IIII',0,0,0,0)
chunk=b'\x1bLuaQ\x00\x01\x04\x04\x04\x04\x00'+body
g=lua51_data.load(chunk);assert g=={'g':{'a':1.5,2.0:-3.0},'h':'x'},g
bad=chunk.replace(struct.pack('<I',instr(7,0,bx=5)),struct.pack('<I',28|(0<<6)))  # CALL
try:lua51_data.load(bad)
except lua51_data.Unsupported:pass
else:raise AssertionError('executable chunk accepted')
assert lua51_data._float32(struct.pack('<f',23.9847))==23.9847

# 7z: single LZMA stream built by hand (header layout of the APK sheets).
payload=os.urandom(3000)+b'A'*5000
props=b'\x5d'+struct.pack('<I',1<<16)
packed=lzma.compress(payload,format=lzma.FORMAT_RAW,filters=[{'id':lzma.FILTER_LZMA1,'dict_size':1<<16,'lc':3,'lp':0,'pb':2}])
def num(v):
    if v<0x80:return bytes([v])
    if v<0x4000:return bytes([0x80|(v>>8),v&255])
    return bytes([0xe0])+struct.pack('<I',v)[:3] if v<0x200000 else bytes([0xf0])+struct.pack('<I',v)
header=b'\x01\x04\x06'+num(0)+num(1)+b'\x09'+num(len(packed))+b'\x00\x07\x0b\x01\x00\x01\x23\x03\x01\x01\x05'+props+b'\x0c'+num(len(payload))+b'\x00\x00\x00'
start=struct.pack('<QQI',len(packed),len(header),zlib.crc32(header))
archive=b'7z\xbc\xaf\x27\x1c\x00\x04'+struct.pack('<I',zlib.crc32(start))+start+packed+header
assert mini7z.extract(archive)==payload

# PNG: palette+tRNS and RGBA with every filter type.
def png(w,h,color,rows,extra=b''):
    def chunk(k,d):return struct.pack('>I',len(d))+k+d+struct.pack('>I',zlib.crc32(k+d))
    return b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,color,0,0,0))+extra+chunk(b'IDAT',zlib.compress(rows))+chunk(b'IEND',b'')
w,h,px=png_decode.decode(png(2,1,3,b'\x00\x00\x01',b'\x00\x00\x00\x06PLTE\xff\x00\x00\x00\xff\x00'+struct.pack('>I',zlib.crc32(b'PLTE\xff\x00\x00\x00\xff\x00'))+b'\x00\x00\x00\x01tRNS\x80'+struct.pack('>I',zlib.crc32(b'tRNS\x80'))))
assert (w,h,px)==(2,1,bytes([255,0,0,128,0,255,0,255]))
rows=b''.join(bytes([f])+bytes([(f*40+i*7)&255 for i in range(8)]) for f in range(5))
w,h,px=png_decode.decode(png(2,5,6,rows));assert (w,h,len(px))==(2,5,40)
try:
    from PIL import Image;import io
    assert Image.open(io.BytesIO(png(2,5,6,rows))).convert('RGBA').tobytes()==px
except ImportError:pass

# SPU2 ADPCM: header, block count, end flag and a faithful decode.
pcm=b''.join(struct.pack('<h',int(9000*math.sin(i/9))) for i in range(1000))
apcm=spu_adpcm.encode(pcm)
assert apcm[:4]==b'APCM' and struct.unpack_from('<II',apcm,8)==(1881,1000) and len(apcm)==16+16*(36+1) and apcm[-15]==7
h1=h2=0;out=[]
for o in range(16,len(apcm)-16,16):
    blk=apcm[o:o+16];a,b=spu_adpcm.COEF[blk[0]>>4];sh=blk[0]&15
    for v in blk[2:]:
        for n in (v&15,v>>4):
            n=n-16 if n>7 else n;x=max(-32768,min(32767,((n<<12)>>sh)+((h1*a+h2*b+32)>>6)));h2=h1;h1=x;out.append(x)
orig=struct.unpack('<1000h',pcm);rms=math.sqrt(sum((x-y)**2 for x,y in zip(orig,out))/1000)
assert rms<120,rms
print(f'Tools OK: AES-256 known answer and padding, data-only Lua 5.1 reader (rejects calls), 7z LZMA, PNG filters/palette/tRNS, SPU2 ADPCM (rms error {rms:.0f}).')
