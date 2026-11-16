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
static MenuSprite sprites[MG_MAX];static int count;
static int failed;
static char about[1024];   /* no retry every frame after a failure; cleared when the menu closes */
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
static int rd16(const unsigned char *p){return p[0]|(p[1]<<8);}
void menu_gfx_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int menu_gfx_active(void){return count>0 && sprites_bank()==SPRITES_MENU_BANK;}
int menu_gfx_count(void){return count;}
const char *menu_gfx_about(void){return about;}
void menu_gfx_release(void){failed=0;}
int menu_gfx_show(GSGLOBAL *gs){
    if(menu_gfx_active())return 1;
    if(failed)return 0;
    /* MENU.BIN: "PBMN", version 1, sprites, pages, (w,h) per page, per sprite page,x,y,w,h (u16)
       and the composition offset dx,dy (s16), padding to 64, then the CT32 pages. The pages are
       read one at a time into the stage bank's RAM buffer (no 2.8 MiB allocation: after a stage
       the theme ambience and background take that room) and sent into the texture slots. */
    int fd=open(path,O_RDONLY);if(fd<0){failed=1;return 0;}
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char head[16+8*4+16*MG_MAX+64];
    int ok=n>64 && read(fd,head,32)==32 && !memcmp(head,"PBMN",4) && rd32(head+4)==2 && rd32(head+12)==4;
    unsigned c=ok?rd32(head+8):0,w[4],h[4],biggest=0;
    ok=ok && c>0 && c<=MG_MAX;
    unsigned headsize=ok?(16+8*4+16*c+63)/64*64:0;
    ok=ok && headsize<=sizeof(head) && read(fd,head+32,headsize-32)==(int)(headsize-32);
    unsigned long total=headsize;
    for(unsigned i=0;ok && i<4;i++){w[i]=rd32(head+16+8*i);h[i]=rd32(head+20+8*i);total+=w[i]*h[i]*4;if(w[i]*h[i]*4>biggest)biggest=w[i]*h[i]*4;}
    ok=ok && total+4<=(unsigned long)n;
    for(unsigned i=0;ok && i<c;i++){
        const unsigned char *q=head+16+8*4+16*i;MenuSprite *s=&sprites[i];
        s->page=rd16(q);s->x=rd16(q+2);s->y=rd16(q+4);s->w=rd16(q+6);s->h=rd16(q+8);s->dx=(short)rd16(q+10);s->dy=(short)rd16(q+12);
        ok=s->page<4 && (unsigned)(s->x+s->w)<=w[s->page] && (unsigned)(s->y+s->h)<=h[s->page];
    }
    unsigned char *buffer=ok?sprites_borrow_buffer(biggest,0):0;ok=ok && buffer;
    for(unsigned i=0;ok && i<4;i++){
        unsigned bytes=w[i]*h[i]*4,done=0;
        while(done<bytes){int r=(int)read(fd,buffer+done,bytes-done);if(r<=0)break;done+=(unsigned)r;}
        ok=done==bytes && sprites_upload_page(gs,(int)i,buffer,w[i],h[i]);
    }
    /* Version 2 tail: length and the about text. */
    unsigned char len[4];about[0]=0;
    if(ok && read(fd,len,4)==4){unsigned l=rd32(len);if(l<sizeof(about) && (unsigned long)n==total+4+l && read(fd,about,l)==(int)l)about[l]=0;else about[0]=0;}
    close(fd);
    if(!ok){printf("PS2Birds: MENU.BIN rejected\n");count=0;failed=1;return 0;}
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
