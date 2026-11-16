#include "background.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <kernel.h>

#define W 640
#define SCREEN_H 448
#define MAX_THEMES 64
static char path[256]="THEMES.BIN";
static int themes,canvas_h,loaded=-1,frame_theme=-1,frame_ground=-1,ready;
static unsigned offsets[MAX_THEMES],sizes[MAX_THEMES];
static unsigned short *canvas,*frame;
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void background_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int background_init(void){
    /* THEMES.BIN: "PBTH", version 2, themes, width, height, then per theme a 32-byte name,
       offset and size; the pictures are 2048-byte aligned. */
    unsigned char head[20+40*MAX_THEMES];
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    int ok=read(fd,head,20)==20 && !memcmp(head,"PBTH",4) && rd32(head+4)==2 && rd32(head+12)==W;
    themes=ok?(int)rd32(head+8):0;canvas_h=ok?(int)rd32(head+16):0;
    ok=ok && themes>0 && themes<=MAX_THEMES && canvas_h>=SCREEN_H && canvas_h<=1024 && read(fd,head+20,40*themes)==40*themes;
    for(int i=0;ok && i<themes;i++){offsets[i]=rd32(head+20+40*i+32);sizes[i]=rd32(head+20+40*i+36);ok=sizes[i]==(unsigned)(W*canvas_h*2);}
    close(fd);
    if(ok){canvas=(unsigned short*)memalign(128,W*canvas_h*2);frame=(unsigned short*)memalign(128,W*SCREEN_H*2);ok=canvas && frame;}
    if(!ok){free(canvas);free(frame);canvas=frame=0;themes=0;}
    ready=ok;return ok;
}
int background_prepare(int theme,int ground_y){
    if(!ready || theme<0 || theme>=themes){frame_theme=-1;return 0;}
    if(theme!=loaded){
        /* Read once per theme change (consecutive stages usually share it). */
        int fd=open(path,O_RDONLY);if(fd<0){frame_theme=-1;return 0;}
        lseek(fd,(off_t)offsets[theme],SEEK_SET);unsigned done=0;unsigned char *to=(unsigned char*)canvas;
        while(done<sizes[theme]){int r=(int)read(fd,to+done,sizes[theme]-done);if(r<=0)break;done+=(unsigned)r;}
        close(fd);
        if(done!=sizes[theme]){loaded=-1;frame_theme=-1;return 0;}
        loaded=theme;frame_theme=-1;
    }
    if(theme!=frame_theme || ground_y!=frame_ground){
        /* Screen row y shows canvas row 448 - ground + y (the ground line on the stage's ground). */
        for(int y=0;y<SCREEN_H;y++){
            int src=SCREEN_H-ground_y+y;if(src<0)src=0;if(src>=canvas_h)src=canvas_h-1;
            memcpy(frame+y*W,canvas+src*W,W*2);
        }
        FlushCache(0);frame_theme=theme;frame_ground=ground_y;
    }
    return 1;
}
void background_draw(GSGLOBAL *gs){
    if(frame_theme<0)return;
    /* Queued with the frame's drawing (so it lands in the buffer being drawn, before the
       stage), in three bands of 192/192/64 rows: one GIF image packet holds at most 32767
       quadwords (512 KiB), less than the 560 KiB picture. Bands start on 64-row CT16 page rows,
       so each band's base is the frame buffer plus whole pages (10 pages of 8 KiB per row). */
    static const int bands[3][2]={{0,192},{192,192},{384,64}};
    for(int b=0;b<3;b++){
        /* gsKit takes the VRAM byte address and converts it to the GS block pointer itself. */
        unsigned base=gs->ScreenBuffer[gs->ActiveBuffer&1]+(unsigned)(bands[b][0]/64)*(W/64)*8192;
        gsKit_texture_send_inline(gs,(u32*)(frame+bands[b][0]*W),W,bands[b][1],base,gs->PSM,W/64,GS_CLUT_NONE);
    }
}
