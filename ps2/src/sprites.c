#include "sprites.h"
#include <gsInline.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
/* Bank 0 is linked into the ELF; banks 1+ are read from SPRITES.BIN (on the disc or
   next to the ELF) into one shared buffer when an episode needs them. */
extern unsigned int sprite_pixels[];
static GSTEXTURE atlases[SPRITE_PAGE_COUNT];
static int ready,bank=-1,loaded=-1,failed;
static unsigned char *bank_buffer;
static char data_path[256]="SPRITES.BIN";
void sprites_set_data_path(const char *path){if(path){strncpy(data_path,path,sizeof(data_path)-1);data_path[sizeof(data_path)-1]=0;}}
int sprites_load_failed(void){return failed;}
static unsigned bank_start(int b){return (unsigned)sprite_pages[b][0].offset;}
static unsigned bank_bytes(int b){unsigned n=0;for(int i=0;i<SPRITE_PAGE_COUNT;i++)n+=(unsigned)(sprite_pages[b][i].width*sprite_pages[b][i].height*4);return n;}
static unsigned char *bank_pixels(int b)
{
    if(b==0)return (unsigned char*)sprite_pixels;
    if(loaded==b)return bank_buffer;
    if(!bank_buffer){unsigned most=0;for(int i=1;i<SPRITE_BANK_COUNT;i++)if(bank_bytes(i)>most)most=bank_bytes(i);bank_buffer=(unsigned char*)memalign(64,most);if(!bank_buffer)return NULL;}
    FILE *f=fopen(data_path,"rb");if(!f)return NULL;
    unsigned want=bank_bytes(b);int ok=fseek(f,(long)(bank_start(b)-bank_bytes(0)),SEEK_SET)==0 && fread(bank_buffer,1,want,f)==want;
    fclose(f);if(!ok){loaded=-1;return NULL;}
    loaded=b;return bank_buffer;
}
int sprites_init(GSGLOBAL *gs)
{
    /* Each page slot is allocated once for its largest page; banks reuse the slots. */
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        memset(&atlases[i],0,sizeof(atlases[i]));
        atlases[i].Vram=gsKit_vram_alloc(gs,sprite_slot_bytes[i],GSKIT_ALLOC_USERBUFFER);
        if(atlases[i].Vram==GSKIT_ALLOC_ERROR)return 0;
    }
    ready=1;return sprites_select_bank(gs,0);
}
int sprites_select_bank(GSGLOBAL *gs,int next)
{
    if(!ready || next<0 || next>=SPRITE_BANK_COUNT)return 0;
    if(next==bank)return 1;
    unsigned char *pixels=bank_pixels(next);
    if(!pixels){failed=1;return 0;}
    failed=0;
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        const SpritePage *page=&sprite_pages[next][i];GSTEXTURE *atlas=&atlases[i];
        if(!page->width)continue;
        atlas->Width=page->width;atlas->Height=page->height;
        atlas->PSM=GS_PSM_CT32;atlas->Filter=GS_FILTER_NEAREST;
        atlas->Mem=(unsigned int*)(pixels+(page->offset-bank_start(next)));
        gsKit_texture_upload(gs,atlas);
    }
    bank=next;return 1;
}
int sprites_bank(void){return bank;}
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba)
{
    if(!ready || bank<0 || id<0 || id>=SPRITE_COUNT)return;
    const SpriteFrame *s=&sprite_frames[bank][id];
    if(s->page<0)return;
    GSTEXTURE *atlas=&atlases[s->page];
    float cs=cosf(angle),sn=sinf(angle),px[4],py[4];
    for(int i=0;i<4;i++) {
        float dx=((i==0||i==3)?-s->px:s->w-s->px)*scale;
        float dy=(i<2?-s->py:s->h-s->py)*scale;
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
