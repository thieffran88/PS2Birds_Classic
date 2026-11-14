#include "sprites.h"
#include <gsInline.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
/* Nothing of the original art is linked into the ELF: SPRITES.BIN (made from the player's
   APK by tools/gerar_iso.py, on the disc or next to the ELF) holds the frame tables, the
   pixels of every texture bank and the memory-card icon. The tables are read once; a
   bank's pages are read into one shared buffer when an episode needs them. */
typedef struct { int width,height,offset; } SpritePage;
typedef struct { short x,y,w,h,px,py,page; } SpriteFrame;
static SpritePage pages[SPRITE_BANK_COUNT][SPRITE_PAGE_COUNT];
static SpriteFrame (*frames)[SPRITE_COUNT];
static unsigned pixel_start,icon_offset,icon_size;
static GSTEXTURE atlases[SPRITE_PAGE_COUNT];
static int flip_x=1,flip_y=1;
void sprites_set_flip(int x,int y){flip_x=x?-1:1;flip_y=y?-1:1;}
static int ready,bank=-1,loaded=-1,failed,header_ok;
static unsigned char *bank_buffer;
static char data_path[256]="SPRITES.BIN";
void sprites_set_data_path(const char *path){if(path){strncpy(data_path,path,sizeof(data_path)-1);data_path[sizeof(data_path)-1]=0;}}
int sprites_load_failed(void){return failed;}
static unsigned rd32(const unsigned char *p){return p[0]|(p[1]<<8)|(p[2]<<16)|((unsigned)p[3]<<24);}
static unsigned bank_bytes(int b){unsigned n=0;for(int i=0;i<SPRITE_PAGE_COUNT;i++)n+=(unsigned)(pages[b][i].width*pages[b][i].height*4);return n;}
static int read_header(void)
{
    if(header_ok)return 1;
    FILE *f=fopen(data_path,"rb");if(!f)return 0;
    unsigned char h[32];int ok=fread(h,1,32,f)==32 && !memcmp(h,"PBSP",4) && rd32(h+4)==2 && rd32(h+8)==SPRITE_BANK_COUNT && rd32(h+12)==SPRITE_PAGE_COUNT && rd32(h+16)==SPRITE_COUNT;
    if(ok){
        pixel_start=rd32(h+20);icon_offset=rd32(h+24);icon_size=rd32(h+28);
        unsigned char *t=(unsigned char*)malloc(SPRITE_BANK_COUNT*SPRITE_PAGE_COUNT*12);
        frames=(SpriteFrame(*)[SPRITE_COUNT])malloc(sizeof(SpriteFrame)*SPRITE_BANK_COUNT*SPRITE_COUNT);
        unsigned char *fr=(unsigned char*)malloc(SPRITE_BANK_COUNT*SPRITE_COUNT*14);
        ok=t && frames && fr && fread(t,1,SPRITE_BANK_COUNT*SPRITE_PAGE_COUNT*12,f)==SPRITE_BANK_COUNT*SPRITE_PAGE_COUNT*12 && fread(fr,1,SPRITE_BANK_COUNT*SPRITE_COUNT*14,f)==SPRITE_BANK_COUNT*SPRITE_COUNT*14;
        if(ok){
            for(int b=0;b<SPRITE_BANK_COUNT;b++)for(int i=0;i<SPRITE_PAGE_COUNT;i++){const unsigned char *q=t+(b*SPRITE_PAGE_COUNT+i)*12;pages[b][i].width=(int)rd32(q);pages[b][i].height=(int)rd32(q+4);pages[b][i].offset=(int)rd32(q+8);
                if(pages[b][i].width*pages[b][i].height*4>sprite_slot_bytes[i])ok=0;}
            for(int b=0;b<SPRITE_BANK_COUNT;b++)for(int i=0;i<SPRITE_COUNT;i++){const unsigned char *q=fr+(b*SPRITE_COUNT+i)*14;short v[7];for(int k=0;k<7;k++)v[k]=(short)(q[2*k]|(q[2*k+1]<<8));
                SpriteFrame *s=&frames[b][i];s->x=v[0];s->y=v[1];s->w=v[2];s->h=v[3];s->px=v[4];s->py=v[5];s->page=v[6];if(s->page>=SPRITE_PAGE_COUNT)ok=0;}
        }
        free(t);free(fr);
        if(!ok){free(frames);frames=0;}
    }
    fclose(f);header_ok=ok;return ok;
}
static unsigned char *bank_pixels(int b)
{
    if(loaded==b)return bank_buffer;
    if(!read_header())return NULL;
    if(!bank_buffer){unsigned most=0;for(int i=0;i<SPRITE_BANK_COUNT;i++)if(bank_bytes(i)>most)most=bank_bytes(i);bank_buffer=(unsigned char*)memalign(64,most);if(!bank_buffer)return NULL;}
    /* Plain open/read in 256 KiB blocks: stdio splits reads into tiny requests, which made a
       2.9 MB bank take about 17 s from the disc in PCSX2. */
    int fd=open(data_path,O_RDONLY);if(fd<0)return NULL;
    unsigned want=bank_bytes(b),done=0;int ok=lseek(fd,(off_t)(pixel_start+pages[b][0].offset),SEEK_SET)==(off_t)(pixel_start+pages[b][0].offset);
    while(ok && done<want){unsigned n=want-done>262144?262144:want-done;int r=read(fd,bank_buffer+done,n);if(r<=0)ok=0;else done+=(unsigned)r;}
    close(fd);if(!ok){loaded=-1;return NULL;}
    loaded=b;return bank_buffer;
}
int sprites_read_icon(unsigned char *out,unsigned capacity)
{
    if(!read_header() || icon_size>capacity)return -1;
    FILE *f=fopen(data_path,"rb");if(!f)return -1;
    int ok=fseek(f,(long)icon_offset,SEEK_SET)==0 && fread(out,1,icon_size,f)==icon_size;
    fclose(f);return ok?(int)icon_size:-1;
}
int sprites_init(GSGLOBAL *gs)
{
    /* Each page slot is allocated once for its largest page; banks reuse the slots. */
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        memset(&atlases[i],0,sizeof(atlases[i]));
        atlases[i].Vram=gsKit_vram_alloc(gs,sprite_slot_bytes[i],GSKIT_ALLOC_USERBUFFER);
        if(atlases[i].Vram==GSKIT_ALLOC_ERROR)return 0;
    }
    ready=1;sprites_select_bank(gs,0);return 1;
}
int sprites_select_bank(GSGLOBAL *gs,int next)
{
    if(!ready || next<0 || next>=SPRITE_BANK_COUNT)return 0;
    if(next==bank)return 1;
    unsigned char *pixels=bank_pixels(next);
    if(!pixels){failed=1;return 0;}
    failed=0;
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        const SpritePage *page=&pages[next][i];GSTEXTURE *atlas=&atlases[i];
        if(!page->width)continue;
        atlas->Width=page->width;atlas->Height=page->height;
        atlas->PSM=GS_PSM_CT32;atlas->Filter=GS_FILTER_NEAREST;
        atlas->Mem=(unsigned int*)(pixels+(page->offset-pages[next][0].offset));
        gsKit_texture_upload(gs,atlas);
    }
    bank=next;return 1;
}
int sprites_bank(void){return bank;}
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba)
{
    if(!ready || bank<0 || id<0 || id>=SPRITE_COUNT)return;
    const SpriteFrame *s=&frames[bank][id];
    if(s->page<0)return;
    GSTEXTURE *atlas=&atlases[s->page];
    float cs=cosf(angle),sn=sinf(angle),px[4],py[4];
    for(int i=0;i<4;i++) {
        float dx=((i==0||i==3)?-s->px:s->w-s->px)*scale*flip_x;
        float dy=(i<2?-s->py:s->h-s->py)*scale*flip_y;
        px[i]=x+dx*cs-dy*sn;py[i]=y+dx*sn+dy*cs;
    }
    const int order[6]={0,1,2,0,2,3};
    GSPRIMUVPOINT vertices[6] __attribute__((aligned(16)));
    for(int i=0;i<6;i++) {
        int k=order[i];
        vertices[i].rgbaq=rgba_to_RGBAQ(rgba,0);
        vertices[i].uv=vertex_to_UV(atlas,s->x+((k==0||k==3)?0:s->w),s->y+(k<2?0:s->h));
        vertices[i].xyz2=vertex_to_XYZ2(gs,px[k],py[k],2);
    }
    gsKit_prim_list_triangle_goraud_texture_uv_3d(gs,atlas,6,vertices);
}

void sprite_draw(GSGLOBAL *gs,int id,float x,float y,float scale,float angle)
{sprite_draw_tint(gs,id,x,y,scale,angle,0x80808080);}
