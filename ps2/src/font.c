#include "font.h"
#include <gsInline.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>

#define MAX_GLYPHS 256
typedef struct {unsigned code;int font,x,y,w,h,ascent;} Glyph;
static char path[256]="FONT.BIN";
static GSTEXTURE pages[FONT_COUNT];static int ready,glyph_count,lines[FONT_COUNT];
static Glyph glyphs[MAX_GLYPHS];
static unsigned clut[FONT_COUNT][16] __attribute__((aligned(64)));
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
static int rd16(const unsigned char *p){return p[0]|(p[1]<<8);}
void font_set_data_path(const char *p){if(p){strncpy(path,p,sizeof(path)-1);path[sizeof(path)-1]=0;}}
int font_ready(void){return ready;}
int font_init(GSGLOBAL *gs){
    /* FONT.BIN: "PBFT", version 1, glyphs, page width, height, fonts, line height per font,
       16 CT32 colours per font, 20 bytes per glyph, padding to 64, PSMT4 page. */
    int fd=open(path,O_RDONLY);if(fd<0)return 0;
    long n=(long)lseek(fd,0,SEEK_END);lseek(fd,0,SEEK_SET);
    unsigned char *data=n>24 && n<=65536?(unsigned char*)memalign(64,(size_t)n):0;
    long done=0;while(data && done<n){int r=(int)read(fd,data+done,(size_t)(n-done));if(r<=0)break;done+=r;}
    close(fd);
    int ok=data && done==n && !memcmp(data,"PBFT",4) && rd32(data+4)==1 && rd32(data+20)==FONT_COUNT;
    unsigned count=ok?rd32(data+8):0,w=ok?rd32(data+12):0,h=ok?rd32(data+16):0;
    unsigned p=24+4*FONT_COUNT+64*FONT_COUNT+20*count;p=(p+63)/64*64;
    ok=ok && count<=MAX_GLYPHS && w==256 && h>0 && h<=256 && (unsigned long)n==p+w*h/2;
    for(int f=0;ok && f<FONT_COUNT;f++){lines[f]=(int)rd32(data+24+4*f);for(int k=0;k<16;k++)clut[f][k]=rd32(data+24+4*FONT_COUNT+64*f+4*k);}
    for(unsigned i=0;ok && i<count;i++){
        const unsigned char *q=data+24+4*FONT_COUNT+64*FONT_COUNT+20*i;
        Glyph *g=&glyphs[i];g->code=rd32(q);g->font=q[4];g->x=rd16(q+8);g->y=rd16(q+10);g->w=rd16(q+12);g->h=rd16(q+14);g->ascent=(short)rd16(q+16);
        ok=g->font<FONT_COUNT && g->x+g->w<=(int)w && g->y+g->h<=(int)h;
    }
    if(ok){
        /* VRAM is handed out in 8 KiB steps: both 64-byte palettes share one block, 256 bytes
           apart (the CLUT base is counted in 256-byte units). Page 24 KiB + 8 KiB = the 32 KiB left. */
        unsigned vram=gsKit_vram_alloc(gs,gsKit_texture_size(w,h,GS_PSM_T4),GSKIT_ALLOC_USERBUFFER);ok=vram!=GSKIT_ALLOC_ERROR;
        unsigned vram_clut=ok?gsKit_vram_alloc(gs,256*FONT_COUNT,GSKIT_ALLOC_USERBUFFER):GSKIT_ALLOC_ERROR;ok=ok && vram_clut!=GSKIT_ALLOC_ERROR;
        for(int f=0;ok && f<FONT_COUNT;f++){
            GSTEXTURE *t=&pages[f];memset(t,0,sizeof(*t));
            t->Width=w;t->Height=h;t->PSM=GS_PSM_T4;t->ClutPSM=GS_PSM_CT32;t->Filter=GS_FILTER_LINEAR;
            t->Vram=vram;t->Mem=(unsigned int*)(data+p);t->Clut=clut[f];
            t->VramClut=vram_clut+256*f;
            gsKit_texture_upload(gs,t);   /* the page is shared; each font sends its own palette */
            t->Mem=0;
        }
    }
    if(!ok)printf("PS2Birds: FONT.BIN rejected (VRAM pointer %u of 4194304)\n",(unsigned)gs->CurrentPointer);
    free(data);glyph_count=ok?(int)count:0;ready=ok;return ok;
}
static unsigned next_code(const char **s){
    const unsigned char *u=(const unsigned char*)*s;unsigned c=*u++;
    if(c>=0xC0 && c<0xE0 && (*u&0xC0)==0x80){c=((c&31)<<6)|(*u++&63);}
    else if(c>=0xE0 && c<0xF0 && (u[0]&0xC0)==0x80 && (u[1]&0xC0)==0x80){c=((c&15)<<12)|((u[0]&63)<<6)|(u[1]&63);u+=2;}
    *s=(const char*)u;return c;
}
static const Glyph *find(int font,unsigned code){
    for(int i=0;i<glyph_count;i++)if(glyphs[i].font==font && glyphs[i].code==code)return &glyphs[i];
    if(code>='a' && code<='z')return find(font,code-32);
    /* Accented letters the sheet lacks (FONT_BASIC has no Â) fall back to the plain letter. */
    static const char *from="ÀÁÂÃÄÈÉÊËÌÍÎÏÒÓÔÕÖÙÚÛÜÇàáâãäèéêëìíîïòóôõöùúûüç",*to="AAAAAEEEEIIIIOOOOOUUUUCaaaaaeeeeiiiiooooouuuuc";
    const char *f=from;for(int i=0;*f;i++){unsigned c=next_code(&f);if(c==code)return find(font,(unsigned char)to[i]);}
    return 0;
}
float font_line(int font,float scale){return ready && font>=0 && font<FONT_COUNT?lines[font]*scale:0;}
float font_width(int font,const char *s,float scale){
    float w=0;
    while(s && *s){const Glyph *g=find(font,next_code(&s));w+=g?g->w*scale*0.86f:lines[font]*scale*0.3f;}
    return w;
}
void font_draw(GSGLOBAL *gs,int font,float x,float y,float scale,int align,unsigned int rgba,const char *s){
    if(!ready || font<0 || font>=FONT_COUNT || !s)return;
    if(align==FONT_CENTER)x-=font_width(font,s,scale)/2;else if(align==FONT_RIGHT)x-=font_width(font,s,scale);
    /* Alpha test drops the transparent texels (the 16-bit frame buffer is dithered). */
    u64 *p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=GS_SETREG_TEST(1,6,0x08,0,0,0,1,1);*p++=GS_TEST_1;
    /* Glyph boxes carry the outline; neighbours overlap it a little (the original draws them so). */
    float base=y+lines[font]*scale*0.8f;
    while(*s){
        const Glyph *g=find(font,next_code(&s));
        if(!g){x+=lines[font]*scale*0.3f;continue;}
        float top=base-g->ascent*scale;
        gsKit_prim_sprite_texture(gs,&pages[font],x,top,g->x,g->y,x+g->w*scale,top+g->h*scale,g->x+g->w,g->y+g->h,4,rgba);
        x+=g->w*scale*0.86f;
    }
    p=(u64*)gsKit_heap_alloc(gs,1,16,GIF_AD);
    *p++=GIF_TAG_AD(1);*p++=GIF_AD;*p++=GS_SETREG_TEST(0,0,0,0,0,0,1,1);*p++=GS_TEST_1;
}
