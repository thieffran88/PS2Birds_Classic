#include "ui.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>

static char path[256]="UI.BIN";
static GSTEXTURE page;static int ready;static unsigned short rects[UI_COUNT][4];
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
void ui_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int ui_ready(void){return ready;}
int ui_init(GSGLOBAL *gs){
    /* UI.BIN: "PBUI", version 1, count, page size, (x,y,w,h) u16 per sprite, padding to 64, RGBA5551 page. */
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char *data=n>16 && n<=262144?(unsigned char*)memalign(64,(size_t)n):0;
    long done=0;while(data && done<n){int r=(int)read(fd,data+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);
    unsigned size=data && done==n?rd32(data+12):0,head=(16+8*UI_COUNT+63)/64*64;
    int ok=data && done==n && !memcmp(data,"PBUI",4) && rd32(data+4)==1 && rd32(data+8)==UI_COUNT && size==256 && (unsigned long)n==head+size*size*2;
    for(int i=0;ok && i<UI_COUNT;i++)for(int k=0;k<4;k++){rects[i][k]=(unsigned short)(data[16+8*i+2*k]|(data[17+8*i+2*k]<<8));}
    if(ok){
        memset(&page,0,sizeof(page));page.Width=size;page.Height=size;page.PSM=GS_PSM_CT16;page.Filter=GS_FILTER_LINEAR;
        page.Vram=gsKit_vram_alloc(gs,gsKit_texture_size(size,size,GS_PSM_CT16),GSKIT_ALLOC_USERBUFFER);
        ok=page.Vram!=GSKIT_ALLOC_ERROR;
    }
    if(ok){page.Mem=(unsigned int*)(data+head);gsKit_texture_upload(gs,&page);}
    free(data);page.Mem=0;ready=ok;return ok;
}
void ui_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba){
    if(!ready || id<0 || id>=UI_COUNT)return;
    /* TEXA: alpha bit 0 -> 0, 1 -> 0x80; AEM makes the black transparent texels invisible. */
    /* The alpha test (A > 0x20) also drops them before blending: in the 16-bit dithered frame
       buffer a blended A=0 texel still leaves a dotted box. */
    u64 *p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=GS_SETREG_TEXA(0,1,0x80);*p++=GS_TEXA;
    p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=GS_SETREG_TEST(1,6,0x20,0,0,0,1,1);*p++=GS_TEST_1;
    float w=rects[id][2]*scale,h=rects[id][3]*scale;
    gsKit_prim_sprite_texture(gs,&page,x-w/2,y-h/2,rects[id][0],rects[id][1],x+w/2,y+h/2,rects[id][0]+rects[id][2],rects[id][1]+rects[id][3],3,rgba);
    p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=GS_SETREG_TEST(0,0,0,0,0,0,1,1);*p++=GS_TEST_1;
}
