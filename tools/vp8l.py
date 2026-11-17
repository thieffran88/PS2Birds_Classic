"""Lossless WebP bitstream (VP8L) reader in plain Python, for the transparency of the lossy WebP sheets: the
"ALPH" chunk stores its alpha plane as a VP8L image without header (the size comes from the picture) whose
green channel is the alpha. decode_alpha_stream(data, width, height) -> bytes (one alpha value per pixel).
Written from the format description; tools/rio/webp_check.py compares the result with a reference decoder."""

class Bits:
    """LSB-first bit reader."""
    def __init__(self,data):
        self.d=data;self.p=0;self.buf=0;self.n=0
    def _fill(self,need):
        while self.n<need:
            b=self.d[self.p] if self.p<len(self.d) else 0
            self.p+=1;self.buf|=b<<self.n;self.n+=8
    def read(self,k):
        if k==0:return 0
        self._fill(k);v=self.buf&((1<<k)-1);self.buf>>=k;self.n-=k;return v
    def peek(self,k):
        self._fill(k);return self.buf&((1<<k)-1)
    def skip(self,k):
        self.buf>>=k;self.n-=k

class Huff:
    """Canonical prefix code with a direct lookup table indexed by the next `maxlen` bits."""
    def __init__(self,lengths):
        nz=[(l,s) for s,l in enumerate(lengths) if l]
        self.single=None;self.maxlen=0;self.table=None
        if len(nz)==1:self.single=nz[0][1];return
        if not nz:self.single=0;return
        self.maxlen=max(l for l,_ in nz)
        count=[0]*(self.maxlen+2)
        for l,_ in nz:count[l]+=1
        nxt=[0]*(self.maxlen+2);code=0
        for l in range(1,self.maxlen+1):
            code=(code+count[l-1])<<1 if l>1 else 0
            nxt[l]=code
        size=1<<self.maxlen;table=[None]*size
        for s,l in enumerate(lengths):
            if not l:continue
            c=nxt[l];nxt[l]+=1
            r=int(format(c,'0%db'%l)[::-1],2)
            e=(s,l)
            for k in range(0,size,1<<l):table[r|k]=e
        self.table=table
    def read(self,br):
        if self.single is not None:return self.single
        e=self.table[br.peek(self.maxlen)]
        if e is None:raise ValueError('código de prefixo inválido')
        br.skip(e[1]);return e[0]

CODE_LENGTH_ORDER=(17,18,0,1,2,3,4,5,16,6,7,8,9,10,11,12,13,14,15)
DIST_MAP=((0,1),(1,0),(1,1),(-1,1),(0,2),(2,0),(1,2),(-1,2),(2,1),(-2,1),(2,2),(-2,2),(0,3),(3,0),(1,3),(-1,3),(3,1),(-3,1),(2,3),(-2,3),(3,2),(-3,2),
 (0,4),(4,0),(1,4),(-1,4),(4,1),(-4,1),(3,3),(-3,3),(2,4),(-2,4),(4,2),(-4,2),(0,5),(3,4),(-3,4),(4,3),(-4,3),(5,0),(1,5),(-1,5),(5,1),(-5,1),(2,5),(-2,5),
 (5,2),(-5,2),(4,4),(-4,4),(3,5),(-3,5),(5,3),(-5,3),(0,6),(6,0),(1,6),(-1,6),(6,1),(-6,1),(2,6),(-2,6),(6,2),(-6,2),(4,5),(-4,5),(5,4),(-5,4),(3,6),(-3,6),
 (6,3),(-6,3),(0,7),(7,0),(1,7),(-1,7),(5,5),(-5,5),(7,1),(-7,1),(4,6),(-4,6),(6,4),(-6,4),(2,7),(-2,7),(7,2),(-7,2),(3,7),(-3,7),(7,3),(-7,3),(5,6),(-5,6),
 (6,5),(-6,5),(8,0),(4,7),(-4,7),(7,4),(-7,4),(8,1),(8,2),(6,6),(-6,6),(8,3),(5,7),(-5,7),(7,5),(-7,5),(8,4),(6,7),(-6,7),(7,6),(-7,6),(8,5),(7,7),(-7,7),
 (8,6),(8,7))
assert len(DIST_MAP)==120

def read_code(br,alphabet):
    if br.read(1):                                   # simple code
        n=br.read(1)+1;first8=br.read(1);s0=br.read(8 if first8 else 1);lengths=[0]*alphabet;lengths[s0]=1
        if n==2:lengths[br.read(8)]=1
        return Huff(lengths)
    clen=[0]*19;num=4+br.read(4)
    for i in range(num):clen[CODE_LENGTH_ORDER[i]]=br.read(3)
    cl=Huff(clen)
    if br.read(1):
        k=2+2*br.read(3);max_sym=2+br.read(k)
    else:max_sym=alphabet
    lengths=[0]*alphabet;sym=0;prev=8
    while sym<alphabet:
        if max_sym==0:break
        max_sym-=1
        c=cl.read(br)
        if c<16:lengths[sym]=c;sym+=1;prev=c if c else prev
        else:
            if c==16:rep=3+br.read(2);val=prev
            elif c==17:rep=3+br.read(3);val=0
            else:rep=11+br.read(7);val=0
            if sym+rep>alphabet:raise ValueError('repetição além do alfabeto')
            for _ in range(rep):lengths[sym]=val;sym+=1
    return Huff(lengths)

def prefix_value(br,sym):
    if sym<4:return sym+1
    extra=(sym-2)>>1;off=(2+(sym&1))<<extra
    return off+br.read(extra)+1

def decode_image(br,w,h,level0):
    """Entropy-coded ARGB image (list of 32-bit ints) of size w x h."""
    cache_bits=0
    if br.read(1):cache_bits=br.read(4)
    meta_bits=0;meta=None;groups=1
    if level0 and br.read(1):
        meta_bits=br.read(3)+2
        mw=(w+(1<<meta_bits)-1)>>meta_bits;mh=(h+(1<<meta_bits)-1)>>meta_bits
        meta=decode_image(br,mw,mh,False);groups=max(((p>>8)&0xffff) for p in meta)+1
    cache_size=1<<cache_bits if cache_bits else 0
    codes=[]
    for _ in range(groups):
        codes.append((read_code(br,256+24+cache_size),read_code(br,256),read_code(br,256),read_code(br,256),read_code(br,40)))
    total=w*h;out=[0]*total;cache=[0]*cache_size if cache_size else None;i=0;mw=(w+(1<<meta_bits)-1)>>meta_bits if meta else 0
    shift=32-cache_bits
    g=codes[0]
    while i<total:
        if meta and (i%w==0 or True):
            x=i%w;y=i//w;g=codes[(meta[(y>>meta_bits)*mw+(x>>meta_bits)]>>8)&0xffff]
        s=g[0].read(br)
        if s<256:
            r=g[1].read(br);b=g[2].read(br);a=g[3].read(br)
            px=(a<<24)|(r<<16)|(s<<8)|b;out[i]=px;i+=1
            if cache:cache[(0x1e35a7bd*px&0xffffffff)>>shift]=px
        elif s<280:
            length=prefix_value(br,s-256);dc=prefix_value(br,g[4].read(br))
            if dc>120:dist=dc-120
            else:
                dx,dy=DIST_MAP[dc-1];dist=dx+dy*w
                if dist<1:dist=1
            if dist>i or i+length>total:raise ValueError('referência fora da imagem')
            for k in range(length):
                px=out[i-dist];out[i]=px;i+=1
                if cache:cache[(0x1e35a7bd*px&0xffffffff)>>shift]=px
        else:
            px=cache[s-280];out[i]=px;i+=1
            if cache:cache[(0x1e35a7bd*px&0xffffffff)>>shift]=px
    return out

def _add(a,b):
    return ((((a>>24)+(b>>24))&255)<<24)|((((a>>16)&255)+((b>>16)&255))&255)<<16|((((a>>8)&255)+((b>>8)&255))&255)<<8|(((a&255)+(b&255))&255)
def _avg2(a,b):
    return ((((a>>24)+(b>>24))>>1)<<24)|(((((a>>16)&255)+((b>>16)&255))>>1)<<16)|(((((a>>8)&255)+((b>>8)&255))>>1)<<8)|(((a&255)+(b&255))>>1)
def _clamp(v):return 0 if v<0 else 255 if v>255 else v
def _select(L,T,TL):
    pl=pt=0
    for sh in (24,16,8,0):
        l=(L>>sh)&255;t=(T>>sh)&255;tl=(TL>>sh)&255;p=l+t-tl
        pl+=abs(p-l);pt+=abs(p-t)
    return L if pl<pt else T
def _cas_full(a,b,c):
    r=0
    for sh in (24,16,8,0):r|=_clamp(((a>>sh)&255)+((b>>sh)&255)-((c>>sh)&255))<<sh
    return r
def _cas_half(a,b):
    r=0
    for sh in (24,16,8,0):
        x=(a>>sh)&255;y=(b>>sh)&255;d=x-y;d=int(d/2)      # C division truncates toward zero
        r|=_clamp(x+d)<<sh
    return r
def _predict(mode,L,T,TR,TL):
    if mode==0:return 0xff000000
    if mode==1:return L
    if mode==2:return T
    if mode==3:return TR
    if mode==4:return TL
    if mode==5:return _avg2(_avg2(L,TR),T)
    if mode==6:return _avg2(L,TL)
    if mode==7:return _avg2(L,T)
    if mode==8:return _avg2(TL,T)
    if mode==9:return _avg2(T,TR)
    if mode==10:return _avg2(_avg2(L,TL),_avg2(T,TR))
    if mode==11:return _select(L,T,TL)
    if mode==12:return _cas_full(L,T,TL)
    if mode==13:return _cas_half(_avg2(L,T),TL)
    return 0xff000000

def _sx8(v):return v-256 if v>127 else v

def decode_alpha_stream(data,w,h):
    br=Bits(data);transforms=[];width=w
    while br.read(1):
        t=br.read(2)
        if t in (0,1):
            bits=br.read(3)+2;sw=(width+(1<<bits)-1)>>bits;sh=(h+(1<<bits)-1)>>bits
            transforms.append((t,bits,decode_image(br,sw,sh,False),width))
        elif t==3:
            size=br.read(8)+1;table=decode_image(br,size,1,False)
            for k in range(1,size):table[k]=_add(table[k],table[k-1])
            wb=3 if size<=2 else 2 if size<=4 else 1 if size<=16 else 0
            transforms.append((3,wb,table,width));width=(width+(1<<wb)-1)>>wb
        else:transforms.append((2,0,None,width))
    pix=decode_image(br,width,h,True)
    for t,bits,sub,wd in reversed(transforms):
        if t==2:
            pix=[(p&0xff00ff00)|((((p>>16)&255)+((p>>8)&255))&255)<<16|(((p&255)+((p>>8)&255))&255) for p in pix]
        elif t==3:
            n=len(sub);out=[0]*(wd*h);per=1<<bits;bpp=8>>bits;mask=(1<<bpp)-1;xs=(wd+per-1)>>bits if bits else wd
            for y in range(h):
                for x in range(wd):
                    p=pix[y*xs+(x>>bits)]>>8&255
                    idx=(p>>(bpp*(x&(per-1))))&mask if bits else p
                    out[y*wd+x]=sub[idx] if idx<n else 0
            pix=out
        elif t==1:
            sw=(wd+(1<<bits)-1)>>bits;out=pix
            for y in range(h):
                for x in range(wd):
                    e=sub[(y>>bits)*sw+(x>>bits)];g2r=_sx8(e&255);g2b=_sx8((e>>8)&255);r2b=_sx8((e>>16)&255)
                    p=out[y*wd+x];g=_sx8((p>>8)&255);r=(p>>16)&255;b=p&255
                    r=(r+((g2r*g)>>5))&255;b=(b+((g2b*g)>>5))&255;b=(b+((r2b*_sx8(r))>>5))&255
                    out[y*wd+x]=(p&0xff00ff00)|(r<<16)|b
        else:
            sw=(wd+(1<<bits)-1)>>bits;out=[0]*(wd*h)
            for y in range(h):
                for x in range(wd):
                    i=y*wd+x;res=pix[i]
                    if y==0 and x==0:pred=0xff000000
                    elif y==0:pred=out[i-1]
                    elif x==0:pred=out[i-wd]
                    else:
                        mode=(sub[(y>>bits)*sw+(x>>bits)]>>8)&15
                        L=out[i-1];T=out[i-wd];TL=out[i-wd-1];TR=out[i-wd+1] if x<wd-1 else out[y*wd]
                        pred=_predict(mode,L,T,TR,TL)
                    out[i]=_add(res,pred)
            pix=out
    return bytes((p>>8)&255 for p in pix)
