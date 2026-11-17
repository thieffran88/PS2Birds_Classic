#include "cutscene.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <kernel.h>

#define W 640
#define H 448
static char path[256]="CUTSCENE.BIN";
typedef struct{int t0,t1,x,y,w,h;unsigned short *px;}Overlay;
static Overlay *ovl;static int ovl_n;
static unsigned short *picture,*frame;static int pic_w,pic_h,start_ms,scroll_ms,end_ms,music_track=-1,options,target1,start2_ms,scroll2_ms,target2;
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void cutscene_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int cutscene_music(void){return music_track;}
void cutscene_stop(void){free(picture);picture=0;free(frame);frame=0;for(int i=0;i<ovl_n;i++)free(ovl[i].px);free(ovl);ovl=0;ovl_n=0;}
int cutscene_start(const char *name){
    cutscene_stop();
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    /* "PBCS", version 5, count; then 72 bytes per scene: name[24], width, height, scroll start ms,
       scroll ms, end ms, data offset, music track, options (bit 0: vertical scroll, bit 1: starts
       at the far end), where the first scroll ends (thousandths of the scroll range), then a second scroll:
       start ms, ms, end (thousandths). Horizontal: 448 rows, at least 640 wide; vertical: 640 wide. */
    unsigned char head[12],e[72];int ok=read(fd,head,12)==12 && !memcmp(head,"PBCS",4) && rd32(head+4)==5,found=0;
    unsigned count=ok?rd32(head+8):0;ok=ok && count<128;
    for(unsigned i=0;ok && i<count && !found;i++){ok=read(fd,e,72)==72;e[23]=0;found=ok && !strcmp((const char*)e,name);}
    ok=ok && found;
    unsigned w=ok?rd32(e+24):0,h=ok?rd32(e+28):0,opt=ok?rd32(e+52):0;
    if(ok && (opt&4)){w=W;h=H;}   /* panels: `target1` is the byte length of the layer block */
    ok=ok && ((opt&4)?(rd32(e+56)>4 && rd32(e+56)<(8u<<20)):(opt&1)?(w==W && h>=H && h<=4096):(h==H && w>=W && w<=4096));
    if(ok){
        long want=(opt&4)?(long)rd32(e+56):(long)w*h*2;
        picture=(unsigned short*)memalign(64,want);frame=(unsigned short*)memalign(64,W*H*2);ok=picture && frame;
        if(ok){lseek(fd,(off_t)rd32(e+44),SEEK_SET);long done=0;
            while(done<want){int r=(int)read(fd,(char*)picture+done,(size_t)(want-done>262144?262144:want-done));if(r<=0)break;done+=r;}
            ok=done==want;}
        if(ok && (opt&8)){   /* blinking sprites over the picture: count, then per sprite 8 ints (start ms, end ms, x, y, w, h, 0, 0) and RGBA5551 */
            unsigned n=0;ok=read(fd,&n,4)==4 && n>0 && n<32;
            if(ok){ovl=(Overlay*)calloc(n,sizeof(Overlay));ok=ovl!=0;}
            for(unsigned i=0;ok && i<n;i++){int hd[8];ok=read(fd,hd,32)==32 && hd[4]>0 && hd[5]>0 && hd[4]<=640 && hd[5]<=448;
                if(!ok)break;ovl_n=(int)i+1;ovl[i].t0=hd[0];ovl[i].t1=hd[1];ovl[i].x=hd[2];ovl[i].y=hd[3];ovl[i].w=hd[4];ovl[i].h=hd[5];
                size_t bytes=(size_t)hd[4]*hd[5]*2;ovl[i].px=(unsigned short*)malloc(bytes);ok=ovl[i].px && (size_t)read(fd,ovl[i].px,bytes)==bytes;}
        }
        if(ok && (opt&4)){unsigned n=rd32((unsigned char*)picture);ok=n>0 && n<16;}
    }
    close(fd);
    if(!ok){cutscene_stop();return 0;}
    pic_w=(int)w;pic_h=(int)h;options=(int)opt;start_ms=(int)rd32(e+32);scroll_ms=(int)rd32(e+36);end_ms=(int)rd32(e+40);music_track=(int)rd32(e+48);
    target1=(int)rd32(e+56);start2_ms=(int)rd32(e+60);scroll2_ms=(int)rd32(e+64);target2=(int)rd32(e+68);
    printf("PS2Birds: cutscene %s loaded (%d x %d)\n",name,pic_w,pic_h);return 1;
}
/* Panels (options bit 2): a block of layers, each 16 ints (x0 y0 x1 y1 center in screen pixels, scale from/to in
   thousandths, start ms, duration ms, easing 0 cubic out 1 bounce out 2 cubic in-out, fade start ms, fade ms, width,
   height, pivot x, pivot y, 0) followed by RGBA5551 pixels (bit 15 clear: transparent). Drawn in order over black. */
static float ease(int kind,float u){
    if(u<=0)return 0;if(u>=1)return 1;
    if(kind==1){const float n=7.5625f,d=2.75f;
        if(u<1/d)return n*u*u;if(u<2/d){u-=1.5f/d;return n*u*u+0.75f;}if(u<2.5f/d){u-=2.25f/d;return n*u*u+0.9375f;}u-=2.625f/d;return n*u*u+0.984375f;}
    if(kind==2)return u<0.5f?4*u*u*u:1-(-2*u+2)*(-2*u+2)*(-2*u+2)/2;
    return 1-(1-u)*(1-u)*(1-u);
}
static void draw_panels(float time){
    const unsigned char *b=(const unsigned char*)picture;unsigned n=rd32(b);b+=4;
    int ms=(int)(time*1000);memset(frame,0,W*H*2);
    for(unsigned i=0;i<n;i++){
        int v[16];for(int k=0;k<16;k++)v[k]=(int)rd32(b+4*k);
        const unsigned short *px=(const unsigned short*)(b+64);b+=64+(size_t)v[11]*v[12]*2;
        if(ms<v[6])continue;
        float u=v[7]>0?(float)(ms-v[6])/v[7]:1,e=ease(v[8],u);
        float sc=(v[4]+(v[5]-v[4])*e)/1000.0f;if(sc<=0.01f)continue;
        float cx=v[0]+(v[2]-v[0])*e,cy=v[1]+(v[3]-v[1])*e;
        int dw=(int)(v[11]*sc+0.5f),dh=(int)(v[12]*sc+0.5f);if(dw<1||dh<1)continue;
        int dx0=(int)(cx-v[13]*sc+0.5f),dy0=(int)(cy-v[14]*sc+0.5f);
        float al=1;if(v[10]>0 && ms>=v[9]){float a=ease(2,(float)(ms-v[9])/v[10]);al=1-a;}
        int fa=(int)(al*256);if(fa<=0)continue;
        for(int y=0;y<dh;y++){int dy=dy0+y;if(dy<0||dy>=H)continue;
            const unsigned short *row=px+(size_t)((long)y*v[12]/dh)*v[11];
            for(int x=0;x<dw;x++){int dx=dx0+x;if(dx<0||dx>=W)continue;
                unsigned short c=row[(long)x*v[11]/dw];if(!(c&0x8000))continue;
                if(fa<256){int r=((c&31)*fa)>>8,g=(((c>>5)&31)*fa)>>8,bl=(((c>>10)&31)*fa)>>8;c=0x8000|r|(g<<5)|(bl<<10);}
                frame[dy*W+dx]=c;}}
    }
}
int cutscene_draw(GSGLOBAL *gs,float time){
    if(!picture || time*1000>=end_ms)return 0;
    /* cubic_in_out over each scroll time: the first from the start to `target1`, an optional second from there to
       `target2` (fractions of the scroll range; 1 is the right edge, or the far end of a vertical scene). */
    if(options&4){draw_panels(time);goto send;}
    float p=0;
    {float u=scroll_ms>0?(time*1000-start_ms)/scroll_ms:1;if(u<0)u=0;if(u>1)u=1;
     float e=u<0.5f?4*u*u*u:1-(-2*u+2)*(-2*u+2)*(-2*u+2)/2;p=e*target1/1000.0f;}
    if(scroll2_ms>0 && time*1000>=start2_ms){
        float u=(time*1000-start2_ms)/scroll2_ms;if(u>1)u=1;
        float e=u<0.5f?4*u*u*u:1-(-2*u+2)*(-2*u+2)*(-2*u+2)/2;p=(target1+e*(target2-target1))/1000.0f;}
    if(options&1){   /* vertical: 640 wide, scrolls up (starting at the bottom) or down */
        int y0=(int)(((options&2)?1-p:p)*(pic_h-H)+0.5f);
        memcpy(frame,picture+y0*W,W*H*2);
    }else{
        int x=(int)(p*(pic_w-W)+0.5f);
        for(int y=0;y<H;y++)memcpy(frame+y*W,picture+y*pic_w+x,W*2);
        int ms=(int)(time*1000);
        for(int i=0;i<ovl_n;i++){const Overlay *o=&ovl[i];if(ms<o->t0||ms>=o->t1)continue;
            for(int y=0;y<o->h;y++){int dy=o->y+y;if(dy<0||dy>=H)continue;
                for(int xx=0;xx<o->w;xx++){int dx=o->x-x+xx;if(dx<0||dx>=W)continue;unsigned short c=o->px[y*o->w+xx];if(c&0x8000)frame[dy*W+dx]=c;}}}
    }
    send:
    FlushCache(0);
    static const int bands[3][2]={{0,192},{192,192},{384,64}};
    for(int b=0;b<3;b++){
        unsigned base=gs->ScreenBuffer[gs->ActiveBuffer&1]+(unsigned)(bands[b][0]/64)*(W/64)*8192;
        gsKit_texture_send_inline(gs,(u32*)(frame+bands[b][0]*W),W,bands[b][1],base,gs->PSM,W/64,GS_CLUT_NONE);
    }
    return 1;
}
