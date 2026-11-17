"""WebP reader in plain Python (no library): the lossy "VP8 " key-frame format (RFC 6386) of the Rio sheets,
inside a RIFF/WebP container, with the "ALPH" transparency chunk of the VP8X files.
Written from the format description and checked against a reference decoder: tools/rio/webp_check.py compares the
output with Pillow's, when Pillow is installed (the check is a development aid; the reader itself needs nothing).
decode_webp(bytes) -> (width, height, RGBA bytes)."""
import struct
from vp8_tables import COEFF_DEFAULT,COEFF_UPDATE,BMODE_PROBS,DC_TABLE,AC_TABLE

ZIGZAG=(0,1,4,8,5,2,3,6,9,12,13,10,7,11,14,15)
BANDS=(0,1,2,3,6,4,5,6,6,6,6,6,6,6,6,7,0)
CAT3456=((173,148,140),(176,155,140,135),(180,157,141,134,130),(254,254,243,230,196,177,153,140,133,130,129))
# Sub-block modes (libwebp order): 0 DC, 1 TM, 2 VE, 3 HE, 4 RD, 5 VR, 6 LD, 7 VL, 8 HD, 9 HU
INTRA4_TREE=(-0,1,-1,2,-2,3,4,6,-3,5,-4,-5,-6,7,-7,8,-8,-9)

class Bool:
    """Boolean entropy decoder of the VP8 partitions."""
    def __init__(self,data,pos,end):
        self.d=data;self.p=pos;self.end=end;self.range=255;self.bits=0
        self.value=0
        for _ in range(2):self.value=(self.value<<8)|self._byte()
    def _byte(self):
        if self.p<self.end:
            v=self.d[self.p];self.p+=1;return v
        return 0
    def bit(self,prob):
        split=1+(((self.range-1)*prob)>>8);big=split<<8
        if self.value>=big:self.range-=split;self.value-=big;r=1
        else:self.range=split;r=0
        while self.range<128:
            self.value<<=1;self.range<<=1;self.bits+=1
            if self.bits==8:self.bits=0;self.value|=self._byte()
        return r
    def value_n(self,n):
        v=0
        for _ in range(n):v=(v<<1)|self.bit(128)
        return v
    def signed(self,n):
        v=self.value_n(n);return -v if self.bit(128) else v
    def flag_signed(self,n):
        return self.signed(n) if self.bit(128) else 0

def _clip(v):return 0 if v<0 else 255 if v>255 else v

def parse_riff(data):
    """-> dict of the chunks of a WebP file: {'VP8 ':bytes,'ALPH':bytes,...}."""
    if data[:4]!=b'RIFF' or data[8:12]!=b'WEBP':raise ValueError('não é WebP')
    chunks={};p=12
    while p+8<=len(data):
        k=data[p:p+4].decode('latin1');n=struct.unpack_from('<I',data,p+4)[0];chunks[k]=data[p+8:p+8+n];p+=8+n+(n&1)
    return chunks

class Header:pass

def decode_vp8(data):
    """The key frame of a "VP8 " chunk -> (width, height, Y plane, U plane, V plane, y stride, uv stride)."""
    tag=data[0]|(data[1]<<8)|(data[2]<<16)
    if tag&1:raise ValueError('não é um quadro-chave VP8')
    first=tag>>5
    if data[3:6]!=b'\x9d\x01\x2a':raise ValueError('código de início VP8 ausente')
    width=struct.unpack_from('<H',data,6)[0]&0x3fff;height=struct.unpack_from('<H',data,8)[0]&0x3fff
    br=Bool(data,10,10+first)
    br.bit(128);br.bit(128)                          # colour space, clamping type
    # --- segment header
    use_seg=br.bit(128);update_map=0;abs_delta=0;seg_quant=[0]*4;seg_filter=[0]*4;seg_proba=[255]*3
    if use_seg:
        update_map=br.bit(128);update_data=br.bit(128)
        if update_data:
            abs_delta=br.bit(128)
            seg_quant=[br.flag_signed(7) for _ in range(4)];seg_filter=[br.flag_signed(6) for _ in range(4)]
        if update_map:seg_proba=[br.value_n(8) if br.bit(128) else 255 for _ in range(3)]
    # --- filter header
    simple=br.bit(128);level=br.value_n(6);sharp=br.value_n(3);use_delta=br.bit(128);ref_d=[0]*4;mode_d=[0]*4
    if use_delta and br.bit(128):
        for i in range(4):
            if br.bit(128):ref_d[i]=br.signed(6)
        for i in range(4):
            if br.bit(128):mode_d[i]=br.signed(6)
    ftype=0 if level==0 else (1 if simple else 2)
    # --- partitions
    nparts=1<<br.value_n(2);pos=10+first;sizes_at=pos;pos+=3*(nparts-1);parts=[]
    for i in range(nparts):
        if i<nparts-1:size=data[sizes_at+3*i]|(data[sizes_at+3*i+1]<<8)|(data[sizes_at+3*i+2]<<16)
        else:size=len(data)-pos
        parts.append(Bool(data,pos,min(len(data),pos+size)));pos+=size
    # --- quantizers
    base=br.value_n(7);dq=[br.flag_signed(4) for _ in range(5)]   # y1 dc, y2 dc, y2 ac, uv dc, uv ac
    dqy1dc,dqy2dc,dqy2ac,dquvdc,dquvac=dq
    quant=[]
    for s in range(4):
        q=(seg_quant[s] if abs_delta else base+seg_quant[s]) if use_seg else base
        c=lambda v,hi=127:max(0,min(hi,v))
        y1=(DC_TABLE[c(q+dqy1dc)],AC_TABLE[c(q)])
        y2=(DC_TABLE[c(q+dqy2dc)]*2,max(8,(AC_TABLE[c(q+dqy2ac)]*101581)>>16))
        uv=(DC_TABLE[c(q+dquvdc,117)],AC_TABLE[c(q+dquvac)])
        quant.append((y1,y2,uv))
    br.bit(128)                                      # refresh entropy probs (ignored)
    probs=[[[[0]*11 for _ in range(3)] for _ in range(8)] for _ in range(4)]
    for t in range(4):
        for b in range(8):
            for c in range(3):
                for p in range(11):
                    i=((t*8+b)*3+c)*11+p
                    probs[t][b][c][p]=br.value_n(8) if br.bit(COEFF_UPDATE[i]) else COEFF_DEFAULT[i]
    use_skip=br.bit(128);skip_p=br.value_n(8) if use_skip else 0
    # --- per-macroblock modes (first partition), raster order
    mbw=(width+15)>>4;mbh=(height+15)>>4
    top_modes=[[0]*4 for _ in range(mbw)];left_modes=[0]*4
    mbs=[]
    for my in range(mbh):
        left_modes=[0]*4
        for mx in range(mbw):
            seg=0
            if update_map:seg=(br.bit(seg_proba[1]) if not br.bit(seg_proba[0]) else br.bit(seg_proba[2])+2)
            skip=br.bit(skip_p) if use_skip else 0
            i4=not br.bit(145);modes=None
            if not i4:
                if br.bit(156):ym=1 if br.bit(128) else 3          # TM / HE
                else:ym=2 if br.bit(163) else 0                    # VE / DC
                top_modes[mx]=[ym]*4;left_modes=[ym]*4
            else:
                modes=[0]*16;top=top_modes[mx]
                for y in range(4):
                    ym=left_modes[y]
                    for x in range(4):
                        pr=BMODE_PROBS[(top[x]*10+ym)*9:(top[x]*10+ym)*9+9]
                        i=INTRA4_TREE[br.bit(pr[0])]
                        while i>0:i=INTRA4_TREE[2*i+br.bit(pr[i])]
                        ym=-i;top[x]=ym;modes[y*4+x]=ym
                    left_modes[y]=ym
                ym=0
            if not br.bit(142):uv=0
            elif not br.bit(114):uv=2
            else:uv=1 if br.bit(183) else 3
            mbs.append((seg,skip,i4,ym if not i4 else None,modes,uv))
    # --- frame buffers (borders: 127 above, 129 left, as the format defines)
    ys=mbw*16;uvs=mbw*8;Y=bytearray(ys*mbh*16);U=bytearray(uvs*mbh*8);V=bytearray(uvs*mbh*8)
    top_nz=[[0]*9 for _ in range(mbw)];top_dc=[0]*mbw;infos=[]
    for my in range(mbh):
        tbr=parts[my&(nparts-1)];left_nz=[0]*9;left_dc=0
        for mx in range(mbw):
            seg,skip,i4,ym,modes,uv=mbs[my*mbw+mx]
            q=quant[seg];coeffs=[None]*24;nz_any=0
            if not skip:
                tnz=top_nz[mx];lnz=left_nz
                if not i4:
                    dcs=[0]*16;ctx=top_dc[mx]+left_dc
                    n=_coeffs(tbr,probs[1],ctx,q[1],0,dcs)
                    top_dc[mx]=left_dc=1 if n>0 else 0
                    nz_any|=1 if n>0 else 0
                    dc=_wht(dcs) if n>1 else [(dcs[0]+3)>>3]*16
                    first=1;ptype=0
                else:first=0;ptype=3
                for b in range(16):
                    bx=b&3;by=b>>2;arr=[0]*16
                    if not i4:arr[0]=dc[b]
                    ctx=lnz[by]+tnz[bx]
                    n=_coeffs(tbr,probs[ptype],ctx,q[0],first,arr)
                    nzf=1 if n>first else 0;tnz[bx]=lnz[by]=nzf
                    coeffs[b]=arr;nz_any|=1 if (n>first or any(arr)) else 0
                for ch in range(2):
                    for b in range(4):
                        bx=b&1;by=b>>1;arr=[0]*16;ti=4+ch*2+bx;li=4+ch*2+by
                        ctx=lnz[li]+tnz[ti]
                        n=_coeffs(tbr,probs[2],ctx,q[2],0,arr)
                        nzf=1 if n>0 else 0;tnz[ti]=lnz[li]=nzf
                        coeffs[16+ch*4+b]=arr;nz_any|=nzf
            else:
                top_nz[mx]=[0]*9;left_nz=[0]*9
                if not i4:top_dc[mx]=left_dc=0
            if not skip:left_nz=left_nz
            _reconstruct(Y,U,V,ys,uvs,mx,my,mbw,i4,ym,modes,uv,coeffs)
            infos.append((seg,i4,bool(nz_any)))
        # left context resets per row (kept per row above)
    # --- loop filter
    if ftype:
        strengths=[]
        for s in range(4):
            bl=(seg_filter[s] if abs_delta else seg_filter[s]+level) if use_seg else level
            row=[]
            for i4 in (0,1):
                lv=bl
                if use_delta:lv+=ref_d[0]+(mode_d[0] if i4 else 0)
                lv=max(0,min(63,lv))
                if lv>0:
                    il=lv
                    if sharp>0:
                        il>>=2 if sharp>4 else 1
                        if il>9-sharp:il=9-sharp
                    if il<1:il=1
                    row.append((2*lv+il,il,2 if lv>=40 else 1 if lv>=15 else 0))
                else:row.append((0,0,0))
            strengths.append(row)
        for my in range(mbh):
            for mx in range(mbw):
                seg,i4,nz=infos[my*mbw+mx];limit,il,hev=strengths[seg][1 if i4 else 0]
                if limit==0:continue
                inner=i4 or nz
                _filter_mb(Y,U,V,ys,uvs,mx,my,ftype,limit,il,hev,inner)
    return width,height,Y,U,V,ys,uvs

def _coeffs(br,prob,ctx,dq,n,out):
    """Tokens of one 4x4 block -> dequantized coefficients in `out` (raster order); returns the end position."""
    p=prob[BANDS[n]][ctx]
    while n<16:
        if not br.bit(p[0]):return n
        while not br.bit(p[1]):
            n+=1;p=prob[BANDS[n]][0]
            if n==16:return 16
        pc=prob[BANDS[n+1]]
        if not br.bit(p[2]):v=1;p=pc[1]
        else:
            if not br.bit(p[3]):
                v=2 if not br.bit(p[4]) else 3+br.bit(p[5])
            elif not br.bit(p[6]):
                if not br.bit(p[7]):v=5+br.bit(159)
                else:v=7+2*br.bit(165);v+=br.bit(145)
            else:
                b1=br.bit(p[8]);b0=br.bit(p[9+b1]);cat=2*b1+b0;v=0
                for t in CAT3456[cat]:v+=v+br.bit(t)
                v+=3+(8<<cat)
            p=pc[2]
        out[ZIGZAG[n]]=(-v if br.bit(128) else v)*dq[1 if n>0 else 0]
        n+=1
    return 16

def _wht(i):
    tmp=[0]*16
    for k in range(4):
        a0=i[k]+i[12+k];a1=i[4+k]+i[8+k];a2=i[4+k]-i[8+k];a3=i[k]-i[12+k]
        tmp[k]=a0+a1;tmp[8+k]=a0-a1;tmp[4+k]=a3+a2;tmp[12+k]=a3-a2
    out=[0]*16
    for k in range(4):
        dc=tmp[k*4]+3;a0=dc+tmp[3+k*4];a1=tmp[1+k*4]+tmp[2+k*4];a2=tmp[1+k*4]-tmp[2+k*4];a3=dc-tmp[3+k*4]
        out[k*4]=(a0+a1)>>3;out[k*4+1]=(a3+a2)>>3;out[k*4+2]=(a0-a1)>>3;out[k*4+3]=(a3-a2)>>3
    return out

def _idct_add(c,buf,pos,stride):
    """Inverse 4x4 DCT of `c` added to buf at pos (clipped)."""
    if not any(c):return
    m1=lambda a:((a*20091)>>16)+a;m2=lambda a:(a*35468)>>16
    tmp=[0]*16
    for i in range(4):
        a=c[i]+c[8+i];b=c[i]-c[8+i];cc=m2(c[4+i])-m1(c[12+i]);d=m1(c[4+i])+m2(c[12+i])
        tmp[i*4]=a+d;tmp[i*4+1]=b+cc;tmp[i*4+2]=b-cc;tmp[i*4+3]=a-d
    for i in range(4):
        dc=tmp[i]+4;a=dc+tmp[8+i];b=dc-tmp[8+i];cc=m2(tmp[4+i])-m1(tmp[12+i]);d=m1(tmp[4+i])+m2(tmp[12+i])
        o=pos+i*stride
        buf[o]=_clip(buf[o]+((a+d)>>3));buf[o+1]=_clip(buf[o+1]+((b+cc)>>3))
        buf[o+2]=_clip(buf[o+2]+((b-cc)>>3));buf[o+3]=_clip(buf[o+3]+((a-d)>>3))

def _avg2(a,b):return (a+b+1)>>1
def _avg3(a,b,c):return (a+2*b+c+2)>>2

def _pred4(mode,top,left,tl):
    """4x4 prediction from top[0..7], left[0..3] and the corner; -> 16 values in raster order."""
    A,B,C,D,E,F,G,H=top;I,J,K,L=left;X=tl;o=[0]*16
    def S(x,y,v):o[y*4+x]=v
    if mode==0:
        dc=(sum(top[:4])+sum(left)+4)>>3;return [dc]*16
    if mode==1:return [_clip(top[x]+left[y]-tl) for y in range(4) for x in range(4)]
    if mode==2:
        v=[_avg3(X,A,B),_avg3(A,B,C),_avg3(B,C,D),_avg3(C,D,E)];return v*4
    if mode==3:
        r=[_avg3(X,I,J),_avg3(I,J,K),_avg3(J,K,L),_avg3(K,L,L)];return [r[y] for y in range(4) for _ in range(4)]
    if mode==4:
        S(0,3,_avg3(J,K,L));v=_avg3(I,J,K);S(1,3,v);S(0,2,v);v=_avg3(X,I,J);S(2,3,v);S(1,2,v);S(0,1,v)
        v=_avg3(A,X,I);S(3,3,v);S(2,2,v);S(1,1,v);S(0,0,v);v=_avg3(B,A,X);S(3,2,v);S(2,1,v);S(1,0,v)
        v=_avg3(C,B,A);S(3,1,v);S(2,0,v);S(3,0,_avg3(D,C,B))
    elif mode==5:
        v=_avg2(X,A);S(0,0,v);S(1,2,v);v=_avg2(A,B);S(1,0,v);S(2,2,v);v=_avg2(B,C);S(2,0,v);S(3,2,v);S(3,0,_avg2(C,D))
        S(0,3,_avg3(K,J,I));S(0,2,_avg3(J,I,X));v=_avg3(I,X,A);S(0,1,v);S(1,3,v);v=_avg3(X,A,B);S(1,1,v);S(2,3,v)
        v=_avg3(A,B,C);S(2,1,v);S(3,3,v);S(3,1,_avg3(B,C,D))
    elif mode==6:
        S(0,0,_avg3(A,B,C));v=_avg3(B,C,D);S(1,0,v);S(0,1,v);v=_avg3(C,D,E);S(2,0,v);S(1,1,v);S(0,2,v)
        v=_avg3(D,E,F);S(3,0,v);S(2,1,v);S(1,2,v);S(0,3,v);v=_avg3(E,F,G);S(3,1,v);S(2,2,v);S(1,3,v)
        v=_avg3(F,G,H);S(3,2,v);S(2,3,v);S(3,3,_avg3(G,H,H))
    elif mode==7:
        S(0,0,_avg2(A,B));v=_avg2(B,C);S(1,0,v);S(0,2,v);v=_avg2(C,D);S(2,0,v);S(1,2,v);v=_avg2(D,E);S(3,0,v);S(2,2,v)
        S(0,1,_avg3(A,B,C));v=_avg3(B,C,D);S(1,1,v);S(0,3,v);v=_avg3(C,D,E);S(2,1,v);S(1,3,v);v=_avg3(D,E,F);S(3,1,v);S(2,3,v)
        S(3,2,_avg3(E,F,G));S(3,3,_avg3(F,G,H))
    elif mode==8:
        v=_avg2(I,X);S(0,0,v);S(2,1,v);v=_avg2(J,I);S(0,1,v);S(2,2,v);v=_avg2(K,J);S(0,2,v);S(2,3,v);S(0,3,_avg2(L,K))
        S(3,0,_avg3(A,B,C));S(2,0,_avg3(X,A,B));v=_avg3(I,X,A);S(1,0,v);S(3,1,v);v=_avg3(J,I,X);S(1,1,v);S(3,2,v)
        v=_avg3(K,J,I);S(1,2,v);S(3,3,v);S(1,3,_avg3(L,K,J))
    else:
        S(0,0,_avg2(I,J));v=_avg2(J,K);S(2,0,v);S(0,1,v);v=_avg2(K,L);S(2,1,v);S(0,2,v);S(1,0,_avg3(I,J,K))
        v=_avg3(J,K,L);S(3,0,v);S(1,1,v);v=_avg3(K,L,L);S(3,1,v);S(1,2,v)
        for x,y in ((3,2),(2,2),(0,3),(1,3),(2,3),(3,3)):S(x,y,L)
    return o

def _predict_block(buf,stride,x0,y0,size,mode,mx,my,corner_top=None):
    """16x16 luma or 8x8 chroma prediction written into buf (borders: 127 above, 129 left)."""
    top=[127]*size if my==0 else list(buf[(y0-1)*stride+x0:(y0-1)*stride+x0+size])
    left=[129]*size if mx==0 else [buf[(y0+j)*stride+x0-1] for j in range(size)]
    if my==0:tl=127
    elif mx==0:tl=129
    else:tl=buf[(y0-1)*stride+x0-1]
    if mode==0:
        if mx>0 and my>0:dc=(sum(top)+sum(left)+size)>>(5 if size==16 else 4)
        elif my>0:dc=(sum(top)+(size>>1))>>(4 if size==16 else 3)
        elif mx>0:dc=(sum(left)+(size>>1))>>(4 if size==16 else 3)
        else:dc=128
        rows=[[dc]*size for _ in range(size)]
    elif mode==1:rows=[[_clip(left[y]+top[x]-tl) for x in range(size)] for y in range(size)]
    elif mode==2:rows=[top[:] for _ in range(size)]
    else:rows=[[left[y]]*size for y in range(size)]
    for y in range(size):
        o=(y0+y)*stride+x0;buf[o:o+size]=bytes(rows[y])

def _reconstruct(Y,U,V,ys,uvs,mx,my,mbw,i4,ym,modes,uvmode,coeffs):
    x0=mx*16;y0=my*16
    if not i4:
        _predict_block(Y,ys,x0,y0,16,ym,mx,my)
        for b in range(16):_idct_add(coeffs[b],Y,(y0+(b>>2)*4)*ys+x0+(b&3)*4,ys) if coeffs[b] else None
    else:
        # top-right of the macroblock row above (127 on the first row; the last macroblock repeats its own corner)
        if my==0:tr=[127]*4
        elif mx<mbw-1:tr=list(Y[(y0-1)*ys+x0+16:(y0-1)*ys+x0+20])
        else:tr=[Y[(y0-1)*ys+x0+15]]*4
        for b in range(16):
            bx=b&3;by=b>>2;px=x0+bx*4;py=y0+by*4
            if py==0:top=[127]*8
            else:
                top=list(Y[(py-1)*ys+px:(py-1)*ys+px+4])
                if bx<3:top+=list(Y[(py-1)*ys+px+4:(py-1)*ys+px+8]) if (by>0 or my>0) else [127]*4
                else:top+=tr
            left=[129]*4 if px==0 else [Y[(py+j)*ys+px-1] for j in range(4)]
            if py==0:tl=127
            elif px==0:tl=129
            else:tl=Y[(py-1)*ys+px-1]
            if my==0 and by==0:top=[127]*8;tl=127 if True else tl
            pred=_pred4(modes[b],top,left,tl)
            for j in range(4):
                o=(py+j)*ys+px;Y[o:o+4]=bytes(pred[j*4:j*4+4])
            if coeffs[b]:_idct_add(coeffs[b],Y,py*ys+px,ys)
    for ch,P in ((0,U),(1,V)):
        _predict_block(P,uvs,mx*8,my*8,8,uvmode,mx,my)
        for b in range(4):
            c=coeffs[16+ch*4+b]
            if c:_idct_add(c,P,(my*8+(b>>1)*4)*uvs+mx*8+(b&1)*4,uvs)

# ---------------------------------------------------------------- loop filter
def _sc1(v):return -128 if v<-128 else 127 if v>127 else v
def _sc2(v):return -16 if v<-16 else 15 if v>15 else v
def _do2(P,i,s):
    p1=P[i-2*s];p0=P[i-s];q0=P[i];q1=P[i+s]
    a=3*(q0-p0)+_sc1(p1-q1);a1=_sc2((a+4)>>3);a2=_sc2((a+3)>>3);P[i-s]=_clip(p0+a2);P[i]=_clip(q0-a1)
def _do4(P,i,s):
    p1=P[i-2*s];p0=P[i-s];q0=P[i];q1=P[i+s]
    a=3*(q0-p0);a1=_sc2((a+4)>>3);a2=_sc2((a+3)>>3);a3=(a1+1)>>1
    P[i-2*s]=_clip(p1+a3);P[i-s]=_clip(p0+a2);P[i]=_clip(q0-a1);P[i+s]=_clip(q1-a3)
def _do6(P,i,s):
    p2=P[i-3*s];p1=P[i-2*s];p0=P[i-s];q0=P[i];q1=P[i+s];q2=P[i+2*s]
    a=_sc1(3*(q0-p0)+_sc1(p1-q1));a1=(27*a+63)>>7;a2=(18*a+63)>>7;a3=(9*a+63)>>7
    P[i-3*s]=_clip(p2+a3);P[i-2*s]=_clip(p1+a2);P[i-s]=_clip(p0+a1);P[i]=_clip(q0-a1);P[i+s]=_clip(q1-a2);P[i+2*s]=_clip(q2-a3)
def _hev(P,i,s,t):return abs(P[i-2*s]-P[i-s])>t or abs(P[i+s]-P[i])>t
def _needs(P,i,s,t):return 4*abs(P[i-s]-P[i])+abs(P[i-2*s]-P[i+s])<=t
def _needs2(P,i,s,t,it):
    if 4*abs(P[i-s]-P[i])+abs(P[i-2*s]-P[i+s])>t:return False
    p3=P[i-4*s];p2=P[i-3*s];p1=P[i-2*s];p0=P[i-s];q0=P[i];q1=P[i+s];q2=P[i+2*s];q3=P[i+3*s]
    return abs(p3-p2)<=it and abs(p2-p1)<=it and abs(p1-p0)<=it and abs(q3-q2)<=it and abs(q2-q1)<=it and abs(q1-q0)<=it
def _loop(P,i,hs,vs,size,thresh,ithresh,hev,wide):
    t2=2*thresh+1
    for _ in range(size):
        if _needs2(P,i,hs,t2,ithresh):
            if _hev(P,i,hs,hev):_do2(P,i,hs)
            elif wide:_do6(P,i,hs)
            else:_do4(P,i,hs)
        i+=vs
def _simple(P,i,hs,vs,size,thresh):
    t2=2*thresh+1
    for _ in range(size):
        if _needs(P,i,hs,t2):_do2(P,i,hs)
        i+=vs
def _filter_mb(Y,U,V,ys,uvs,mx,my,ftype,limit,il,hev,inner):
    yp=my*16*ys+mx*16;up=my*8*uvs+mx*8
    if ftype==1:
        if mx>0:_simple(Y,yp,1,ys,16,limit+4)
        if inner:
            for k in (4,8,12):_simple(Y,yp+k,1,ys,16,limit)
        if my>0:_simple(Y,yp,ys,1,16,limit+4)
        if inner:
            for k in (4,8,12):_simple(Y,yp+k*ys,ys,1,16,limit)
        return
    if mx>0:
        _loop(Y,yp,1,ys,16,limit+4,il,hev,True);_loop(U,up,1,uvs,8,limit+4,il,hev,True);_loop(V,up,1,uvs,8,limit+4,il,hev,True)
    if inner:
        for k in (4,8,12):_loop(Y,yp+k,1,ys,16,limit,il,hev,False)
        _loop(U,up+4,1,uvs,8,limit,il,hev,False);_loop(V,up+4,1,uvs,8,limit,il,hev,False)
    if my>0:
        _loop(Y,yp,ys,1,16,limit+4,il,hev,True);_loop(U,up,uvs,1,8,limit+4,il,hev,True);_loop(V,up,uvs,1,8,limit+4,il,hev,True)
    if inner:
        for k in (4,8,12):_loop(Y,yp+k*ys,ys,1,16,limit,il,hev,False)
        _loop(U,up+4*uvs,uvs,1,8,limit,il,hev,False);_loop(V,up+4*uvs,uvs,1,8,limit,il,hev,False)

# ---------------------------------------------------------------- colour conversion
def _mh(v,c):return (v*c)>>8
def _rgb(y,u,v):
    yy=_mh(y,19077)
    return (_clip((yy+_mh(v,26149)-14234)>>6),_clip((yy-_mh(u,6419)-_mh(v,13320)+8708)>>6),_clip((yy+_mh(u,33050)-17685)>>6))

def yuv_to_rgba(w,h,Y,U,V,ys,uvs):
    """Fancy (bilinear) chroma upsampling then the BT.601 conversion, as the reference decoder does."""
    out=bytearray(w*h*4)
    cw=(w+1)>>1
    def pair(top_row,bot_row,tu,tv,cu,cv):
        # tu/tv: chroma row above, cu/cv: chroma row below, as lists of length cw
        def put(row,x,uv):
            if row is None:return
            r,g,b=_rgb(Y[row*ys+x],uv&0xff,uv>>16);o=(row*w+x)*4;out[o]=r;out[o+1]=g;out[o+2]=b;out[o+3]=255
        tl=tu[0]|(tv[0]<<16);l=cu[0]|(cv[0]<<16)
        put(top_row,0,(3*tl+l+0x00020002)>>2)
        if bot_row is not None:put(bot_row,0,(3*l+tl+0x00020002)>>2)
        for x in range(1,cw):
            t=tu[x]|(tv[x]<<16);c=cu[x]|(cv[x]<<16)
            avg=tl+t+l+c+0x00080008;d12=(avg+2*(t+l))>>3;d03=(avg+2*(tl+c))>>3
            put(top_row,2*x-1,(d12+tl)>>1)
            if 2*x<w:put(top_row,2*x,(d03+t)>>1)
            if bot_row is not None:
                put(bot_row,2*x-1,(d03+l)>>1)
                if 2*x<w:put(bot_row,2*x,(d12+c)>>1)
            tl=t;l=c
        if not (w&1):
            put(top_row,w-1,(3*tl+l+0x00020002)>>2)
            if bot_row is not None:put(bot_row,w-1,(3*l+tl+0x00020002)>>2)
    def crow(P,r):return list(P[r*uvs:r*uvs+cw])
    # row 0 alone, then pairs (2j-1, 2j), then the last odd row alone
    pair(0,None,crow(U,0),crow(V,0),crow(U,0),crow(V,0))
    for j in range(1,(h+1)//2):
        top=2*j-1;bot=2*j if 2*j<h else None
        if bot is None:
            c=(h-1)>>1;pair(top,None,crow(U,c),crow(V,c),crow(U,c),crow(V,c))
        else:pair(top,bot,crow(U,j-1),crow(V,j-1),crow(U,j),crow(V,j))
    if h>1 and h%2==0:
        c=(h-1)>>1;pair(h-1,None,crow(U,c),crow(V,c),crow(U,c),crow(V,c))
    return out

def decode_webp(data):
    ch=parse_riff(data)
    if 'VP8 ' not in ch:raise ValueError('WebP sem quadro VP8 com perdas (VP8L não suportado)')
    w,h,Y,U,V,ys,uvs=decode_vp8(ch['VP8 ']);rgba=yuv_to_rgba(w,h,Y,U,V,ys,uvs)
    if 'ALPH' in ch:
        a=decode_alpha(ch['ALPH'],w,h)
        for i in range(w*h):rgba[i*4+3]=a[i]
    return w,h,bytes(rgba)

def decode_alpha(chunk,w,h):
    method=chunk[0]&3;filt=(chunk[0]>>2)&3
    if method==1:
        import vp8l
        a=bytearray(vp8l.decode_alpha_stream(chunk[1:],w,h))
        if filt:_unfilter(a,w,h,filt)
        return a
    if method!=0:raise ValueError('método de ALPH desconhecido')
    a=bytearray(chunk[1:1+w*h])
    if len(a)<w*h:a+=bytes(w*h-len(a))
    if filt:_unfilter(a,w,h,filt)
    return a

def _unfilter(a,w,h,filt):
    for y in range(h):
        for x in range(w):
            i=y*w+x
            if y==0 and x==0:continue
            if y==0:pred=a[i-1]
            elif x==0:pred=a[i-w]
            elif filt==1:pred=a[i-1]
            elif filt==2:pred=a[i-w]
            else:pred=max(0,min(255,a[i-1]+a[i-w]-a[i-w-1]))
            a[i]=(a[i]+pred)&255
