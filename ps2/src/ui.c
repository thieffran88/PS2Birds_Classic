#include "ui.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <kernel.h>
#include <dmaKit.h>

static char path[256]="UI.BIN";
static GSTEXTURE page;static int ready;static unsigned short rects[UI_COUNT][4];
static unsigned clut[256] __attribute__((aligned(64)));
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void ui_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int ui_ready(void){return ready;}
int ui_init(GSGLOBAL *gs){
    /* UI.BIN: "PBUI", version 2, count, width 128, height 1024, (x,y,w,h) u16 per sprite, padding to
       64, 256 CT32 palette entries (CSM1 order), then the 8-bit indices. Same 128 KiB as the former
       CT16 page; the palette takes the 8 KiB VRAM block that was left. */
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char *data=n>16 && n<=200000?(unsigned char*)memalign(64,(size_t)n):0;
    long done=0;while(data && done<n){int r=(int)read(fd,data+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);
    unsigned w=data && done==n?rd32(data+12):0,h=data && done==n?rd32(data+16):0,head=(20+8*UI_COUNT+63)/64*64;
    int ok=data && done==n && !memcmp(data,"PBUI",4) && rd32(data+4)==2 && rd32(data+8)==UI_COUNT && w==128 && h==1024 && (unsigned long)n==head+1024+w*h;
    for(int i=0;ok && i<UI_COUNT;i++)for(int k=0;k<4;k++)rects[i][k]=(unsigned short)(data[20+8*i+2*k]|(data[21+8*i+2*k]<<8));
    if(ok){
        memcpy(clut,data+head,1024);
        memset(&page,0,sizeof(page));page.Width=w;page.Height=h;page.PSM=GS_PSM_T8;page.ClutPSM=GS_PSM_CT32;page.TBW=w/64;page.Filter=GS_FILTER_LINEAR;
        page.Vram=gsKit_vram_alloc(gs,gsKit_texture_size(w,h,GS_PSM_T8),GSKIT_ALLOC_USERBUFFER);
        page.VramClut=page.Vram!=GSKIT_ALLOC_ERROR?gsKit_vram_alloc(gs,gsKit_texture_size(16,16,GS_PSM_CT32),GSKIT_ALLOC_USERBUFFER):GSKIT_ALLOC_ERROR;
        ok=page.Vram!=GSKIT_ALLOC_ERROR && page.VramClut!=GSKIT_ALLOC_ERROR;
    }
    if(ok){
        /* Sent by hand, 128 rows at a time (two 8 KiB PSMT8 pages of 128 x 64, one after the other),
           then the palette as a 16 x 16 CT32 image; each transfer finishes before the next. The page
           is one GS page wide so the rows of the transfer and of the texture use the same layout. */
        unsigned char *pixels=data+head+1024;FlushCache(0);
        for(int band=0;band<8;band++){
            gsKit_texture_send((u32*)(pixels+band*128*128),128,128,page.Vram+band*2*8192,GS_PSM_T8,2,GS_CLUT_TEXTURE);
            dmaKit_wait_fast();
        }
        gsKit_texture_send(clut,16,16,page.VramClut,GS_PSM_CT32,1,GS_CLUT_PALLETE);dmaKit_wait_fast();
        page.Clut=clut;
    }
    free(data);page.Mem=0;ready=ok;return ok;
}
static void alpha_test(GSGLOBAL *gs,int on){
    /* Drops transparent texels before blending (the 16-bit frame buffer is dithered). */
    u64 *p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=on?GS_SETREG_TEST(1,6,0x08,0,0,0,1,1):GS_SETREG_TEST(0,0,0,0,0,0,1,1);*p++=GS_TEST_1;
}
void ui_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba){
    if(!ready || id<0 || id>=UI_COUNT)return;
    float w=rects[id][2]*scale,h=rects[id][3]*scale;
    alpha_test(gs,1);
    gsKit_prim_sprite_texture(gs,&page,x-w/2,y-h/2,rects[id][0],rects[id][1],x+w/2,y+h/2,rects[id][0]+rects[id][2],rects[id][1]+rects[id][3],3,rgba);
    alpha_test(gs,0);
}
float ui_width(int id){return ready && id>=0 && id<UI_COUNT?(float)rects[id][2]:0;}
void ui_draw_rotated(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba){
    if(!ready || id<0 || id>=UI_COUNT)return;
    float hw=rects[id][2]*scale/2,hh=rects[id][3]*scale/2,cs=cosf(angle),sn=sinf(angle);
    float u0=rects[id][0],v0=rects[id][1],u1=u0+rects[id][2],v1=v0+rects[id][3];
    float cx[4]={-hw,hw,hw,-hw},cy[4]={-hh,-hh,hh,hh},cu[4]={u0,u1,u1,u0},cv[4]={v0,v0,v1,v1};
    GSPRIMUVPOINT vtx[6] __attribute__((aligned(16)));static const int order[6]={0,1,2,0,2,3};
    for(int i=0;i<6;i++){int k=order[i];
        vtx[i].rgbaq=rgba_to_RGBAQ(rgba,0);vtx[i].uv=vertex_to_UV(&page,cu[k],cv[k]);
        vtx[i].xyz2=vertex_to_XYZ2(gs,x+cx[k]*cs-cy[k]*sn,y+cx[k]*sn+cy[k]*cs,3);}
    alpha_test(gs,1);gsKit_prim_list_triangle_goraud_texture_uv_3d(gs,&page,6,vtx);alpha_test(gs,0);
}
