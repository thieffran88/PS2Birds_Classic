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
static unsigned short *picture,*frame;static int pic_w,start_ms,scroll_ms,end_ms,music_track=-1;
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void cutscene_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int cutscene_music(void){return music_track;}
void cutscene_stop(void){free(picture);picture=0;free(frame);frame=0;}
int cutscene_start(const char *name){
    cutscene_stop();
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    /* "PBCS", version 3, count; then 52 bytes per scene: name[24], width, height, scroll start ms,
       scroll ms, end ms, data offset, music track. */
    unsigned char head[12],e[52];int ok=read(fd,head,12)==12 && !memcmp(head,"PBCS",4) && rd32(head+4)==3,found=0;
    unsigned count=ok?rd32(head+8):0;ok=ok && count<128;
    for(unsigned i=0;ok && i<count && !found;i++){ok=read(fd,e,52)==52;e[23]=0;found=ok && !strcmp((const char*)e,name);}
    ok=ok && found;
    unsigned w=ok?rd32(e+24):0,h=ok?rd32(e+28):0;ok=ok && h==H && w>=W && w<=4096;
    if(ok){
        picture=(unsigned short*)memalign(64,w*h*2);frame=(unsigned short*)memalign(64,W*H*2);ok=picture && frame;
        if(ok){lseek(fd,(off_t)rd32(e+44),SEEK_SET);long want=(long)w*h*2,done=0;
            while(done<want){int r=(int)read(fd,(char*)picture+done,(size_t)(want-done>262144?262144:want-done));if(r<=0)break;done+=r;}
            ok=done==want;}
    }
    close(fd);
    if(!ok){cutscene_stop();return 0;}
    pic_w=(int)w;start_ms=(int)rd32(e+32);scroll_ms=(int)rd32(e+36);end_ms=(int)rd32(e+40);music_track=(int)rd32(e+48);
    printf("PS2Birds: cutscene %s loaded (%d x %d)\n",name,pic_w,H);return 1;
}
int cutscene_draw(GSGLOBAL *gs,float time){
    if(!picture || time*1000>=end_ms)return 0;
    /* cubic_in_out over the scroll time, from the left edge to the right edge. */
    float u=scroll_ms>0?(time*1000-start_ms)/scroll_ms:1;if(u<0)u=0;if(u>1)u=1;
    float e=u<0.5f?4*u*u*u:1-(-2*u+2)*(-2*u+2)*(-2*u+2)/2;
    int x=(int)(e*(pic_w-W)+0.5f);
    for(int y=0;y<H;y++)memcpy(frame+y*W,picture+y*pic_w+x,W*2);
    FlushCache(0);
    static const int bands[3][2]={{0,192},{192,192},{384,64}};
    for(int b=0;b<3;b++){
        unsigned base=gs->ScreenBuffer[gs->ActiveBuffer&1]+(unsigned)(bands[b][0]/64)*(W/64)*8192;
        gsKit_texture_send_inline(gs,(u32*)(frame+bands[b][0]*W),W,bands[b][1],base,gs->PSM,W/64,GS_CLUT_NONE);
    }
    return 1;
}
