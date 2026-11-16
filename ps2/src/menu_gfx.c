#include "menu_gfx.h"
#include "sprites.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>

typedef struct {int page,x,y,w,h,dx,dy;} MenuSprite;
static char path[256]="MENU.BIN";
static MenuSprite sprites[MG_MAX];static int count;static unsigned char *pixels;
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
static int rd16(const unsigned char *p){return p[0]|(p[1]<<8);}
void menu_gfx_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int menu_gfx_active(void){return count>0 && sprites_bank()==SPRITES_MENU_BANK;}
int menu_gfx_count(void){return count;}
void menu_gfx_release(void){if(!menu_gfx_active()){free(pixels);pixels=0;}}
int menu_gfx_show(GSGLOBAL *gs){
    if(menu_gfx_active())return 1;
    /* MENU.BIN: "PBMN", version 1, sprites, pages, (w,h) per page, per sprite page,x,y,w,h (u16)
       and the composition offset dx,dy (s16), padding to 64, then the CT32 pages. */
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    free(pixels);pixels=n>64 && n<=4*1048576?(unsigned char*)memalign(128,(size_t)n):0;
    long done=0;while(pixels && done<n){int r=(int)read(fd,pixels+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);
    unsigned c=pixels && done==n?rd32(pixels+8):0,np=c?rd32(pixels+12):0,w[4],h[4];
    int ok=pixels && done==n && !memcmp(pixels,"PBMN",4) && rd32(pixels+4)==1 && c>0 && c<=MG_MAX && np==4;
    unsigned head=16+8*np+16*c;head=(head+63)/64*64;unsigned long total=head;
    for(unsigned i=0;ok && i<np;i++){w[i]=rd32(pixels+16+8*i);h[i]=rd32(pixels+20+8*i);total+=w[i]*h[i]*4;}
    ok=ok && total==(unsigned long)n;
    for(unsigned i=0;ok && i<c;i++){
        const unsigned char *q=pixels+16+8*np+16*i;MenuSprite *s=&sprites[i];
        s->page=rd16(q);s->x=rd16(q+2);s->y=rd16(q+4);s->w=rd16(q+6);s->h=rd16(q+8);s->dx=(short)rd16(q+10);s->dy=(short)rd16(q+12);
        ok=s->page<(int)np && (unsigned)(s->x+s->w)<=w[s->page] && (unsigned)(s->y+s->h)<=h[s->page];
    }
    if(ok)ok=sprites_upload_pages(gs,pixels+head,w,h,(int)np);
    if(!ok){printf("PS2Birds: MENU.BIN rejected\n");free(pixels);pixels=0;count=0;return 0;}
    count=(int)c;return 1;
}
static void alpha_test(GSGLOBAL *gs,int on){
    u64 *p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=on?GS_SETREG_TEST(1,6,0x04,0,0,0,1,1):GS_SETREG_TEST(0,0,0,0,0,0,1,1);*p++=GS_TEST_1;
}
int menu_gfx_width(int id){return id>=0 && id<count?sprites[id].w:0;}
int menu_gfx_height(int id){return id>=0 && id<count?sprites[id].h:0;}
void menu_gfx_draw(GSGLOBAL *gs,int id,float x,float y,float scale,unsigned int rgba){
    if(!menu_gfx_active() || id<0 || id>=count || !sprites[id].w)return;
    const MenuSprite *s=&sprites[id];GSTEXTURE *t=sprites_slot(s->page);
    x+=s->dx*scale;y+=s->dy*scale;float w=s->w*scale/2,h=s->h*scale/2;
    alpha_test(gs,1);
    gsKit_prim_sprite_texture(gs,t,x-w,y-h,s->x,s->y,x+w,y+h,s->x+s->w,s->y+s->h,3,rgba);
    alpha_test(gs,0);
}
void menu_gfx_draw_span(GSGLOBAL *gs,int id,float x0,float x1,float y,float scale){
    if(!menu_gfx_active() || id<0 || id>=count || !sprites[id].w)return;
    const MenuSprite *s=&sprites[id];GSTEXTURE *t=sprites_slot(s->page);float h=s->h*scale/2;
    alpha_test(gs,1);
    gsKit_prim_sprite_texture(gs,t,x0,y-h,s->x+1,s->y,x1,y+h,s->x+s->w-1,s->y+s->h,3,0x80808080);
    alpha_test(gs,0);
}
