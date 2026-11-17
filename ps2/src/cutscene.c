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
static unsigned short *picture,*frame;static int pic_w,pic_h,start_ms,scroll_ms,end_ms,music_track=-1,options,target1,start2_ms,scroll2_ms,target2;
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void cutscene_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int cutscene_music(void){return music_track;}
void cutscene_stop(void){free(picture);picture=0;free(frame);frame=0;}
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
    ok=ok && ((opt&1)?(w==W && h>=H && h<=4096):(h==H && w>=W && w<=4096));
    if(ok){
        picture=(unsigned short*)memalign(64,w*h*2);frame=(unsigned short*)memalign(64,W*H*2);ok=picture && frame;
        if(ok){lseek(fd,(off_t)rd32(e+44),SEEK_SET);long want=(long)w*h*2,done=0;
            while(done<want){int r=(int)read(fd,(char*)picture+done,(size_t)(want-done>262144?262144:want-done));if(r<=0)break;done+=r;}
            ok=done==want;}
    }
    close(fd);
    if(!ok){cutscene_stop();return 0;}
    pic_w=(int)w;pic_h=(int)h;options=(int)opt;start_ms=(int)rd32(e+32);scroll_ms=(int)rd32(e+36);end_ms=(int)rd32(e+40);music_track=(int)rd32(e+48);
    target1=(int)rd32(e+56);start2_ms=(int)rd32(e+60);scroll2_ms=(int)rd32(e+64);target2=(int)rd32(e+68);
    printf("PS2Birds: cutscene %s loaded (%d x %d)\n",name,pic_w,pic_h);return 1;
}
int cutscene_draw(GSGLOBAL *gs,float time){
    if(!picture || time*1000>=end_ms)return 0;
    /* cubic_in_out over each scroll time: the first from the start to `target1`, an optional second from there to
       `target2` (fractions of the scroll range; 1 is the right edge, or the far end of a vertical scene). */
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
    }
    FlushCache(0);
    static const int bands[3][2]={{0,192},{192,192},{384,64}};
    for(int b=0;b<3;b++){
        unsigned base=gs->ScreenBuffer[gs->ActiveBuffer&1]+(unsigned)(bands[b][0]/64)*(W/64)*8192;
        gsKit_texture_send_inline(gs,(u32*)(frame+bands[b][0]*W),W,bands[b][1],base,gs->PSM,W/64,GS_CLUT_NONE);
    }
    return 1;
}
