#include "background.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <kernel.h>
#include <math.h>
#include "sprites.h"

#define W 640
#define SCREEN_H 448
#define MAX_THEMES 64
static char path[256]="THEMES.BIN";
static int themes,canvas_h,loaded=-1,frame_theme=-1,frame_ground=-1,frame_stage=-1,ready;
static unsigned offsets[MAX_THEMES],sizes[MAX_THEMES];
static unsigned short *canvas,*frame,*texture;   /* texture: the theme's 256 x 256 ground tile */
#define TEXTURE_BYTES (256*256*2)
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void background_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int background_init(void){
    /* THEMES.BIN: "PBTH", version 2, themes, width, height, then per theme a 32-byte name,
       offset and size; the pictures are 2048-byte aligned. */
    unsigned char head[20+40*MAX_THEMES];
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    int ok=read(fd,head,20)==20 && !memcmp(head,"PBTH",4) && rd32(head+4)==3 && rd32(head+12)==W;
    themes=ok?(int)rd32(head+8):0;canvas_h=ok?(int)rd32(head+16):0;
    ok=ok && themes>0 && themes<=MAX_THEMES && canvas_h>=SCREEN_H && canvas_h<=1024 && read(fd,head+20,40*themes)==40*themes;
    for(int i=0;ok && i<themes;i++){offsets[i]=rd32(head+20+40*i+32);sizes[i]=rd32(head+20+40*i+36);ok=sizes[i]==(unsigned)(W*canvas_h*2+TEXTURE_BYTES);}
    close(fd);
    if(ok){canvas=(unsigned short*)memalign(128,W*canvas_h*2+TEXTURE_BYTES);frame=(unsigned short*)memalign(128,W*SCREEN_H*2);ok=canvas && frame;}
    if(ok)texture=canvas+W*canvas_h;
    if(!ok){free(canvas);free(frame);canvas=frame=0;themes=0;}
    ready=ok;return ok;
}
static unsigned short blend(unsigned short dst,unsigned short src,int a){
    /* RGBA5551 mix, a in 0..128 */
    int r=((src&31)*a+(dst&31)*(128-a))>>7,g=(((src>>5)&31)*a+((dst>>5)&31)*(128-a))>>7,b=(((src>>10)&31)*a+((dst>>10)&31)*(128-a))>>7;
    return (unsigned short)(r|(g<<5)|(b<<10)|0x8000);
}
static void paint_piece(const BackgroundPiece *p){
    /* Inverse of sprite_draw_tint's transform: each screen pixel of the piece's box looks up the
       ground mask (alpha) and takes the texture, anchored to the screen at the 1024 x 768 scale. */
    int w,h,px,py,stride;const unsigned char *mask=sprites_frame_rgba(p->sprite,&w,&h,&px,&py,&stride);
    if(!mask || p->scale<=0)return;
    float cs=cosf(p->angle),sn=sinf(p->angle),kx=p->scale*(p->flip_x?-1:1),ky=p->scale*(p->flip_y?-1:1);
    float minx=1e9f,maxx=-1e9f,miny=1e9f,maxy=-1e9f;
    for(int c=0;c<4;c++){
        float dx=((c&1)?w-px:-px)*kx,dy=((c&2)?h-py:-py)*ky,X=p->x+dx*cs-dy*sn,Y=p->y+dx*sn+dy*cs;
        if(X<minx)minx=X;if(X>maxx)maxx=X;if(Y<miny)miny=Y;if(Y>maxy)maxy=Y;
    }
    int x0=minx<0?0:(int)minx,x1=maxx>W-1?W-1:(int)maxx+1,y0=miny<0?0:(int)miny,y1=maxy>SCREEN_H-1?SCREEN_H-1:(int)maxy+1;
    for(int Y=y0;Y<=y1;Y++)for(int X=x0;X<=x1;X++){
        float dx=X+0.5f-p->x,dy=Y+0.5f-p->y,u=(dx*cs+dy*sn)/kx+px,v=(-dx*sn+dy*cs)/ky+py;
        int su=(int)u,sv=(int)v;
        if(u<0 || v<0 || su>=w || sv>=h)continue;
        int a=mask[((unsigned)sv*stride+su)*4+3];if(a<=0)continue;if(a>128)a=128;
        unsigned short t=texture[(((int)(Y*1.7142857f))&255)*256+(((int)(X*1.7142857f))&255)];
        frame[Y*W+X]=blend(frame[Y*W+X],t,a);
    }
}
static int scroll_px,frame_scroll;
void background_set_scroll(int pixels){scroll_px=((pixels%W)+W)%W;}
int background_prepare(int theme,int ground_y,int stage,const BackgroundPiece *pieces,int count){
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
    if(theme!=frame_theme || ground_y!=frame_ground || stage!=frame_stage || scroll_px!=frame_scroll){
        /* Screen row y shows canvas row 448 - ground + y (the ground line on the stage's ground). */
        for(int y=0;y<SCREEN_H;y++){
            int src=SCREEN_H-ground_y+y;if(src<0)src=0;if(src>=canvas_h)src=canvas_h-1;
            if(scroll_px){memcpy(frame+y*W,canvas+src*W+scroll_px,(W-scroll_px)*2);memcpy(frame+y*W+(W-scroll_px),canvas+src*W,scroll_px*2);}
            else memcpy(frame+y*W,canvas+src*W,W*2);
        }
        for(int i=0;i<count;i++)paint_piece(&pieces[i]);
        FlushCache(0);frame_theme=theme;frame_ground=ground_y;frame_stage=stage;frame_scroll=scroll_px;
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
