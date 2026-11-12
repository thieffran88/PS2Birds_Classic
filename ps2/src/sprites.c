#include "sprites.h"
#include <gsInline.h>
#include <math.h>
#include <string.h>
extern unsigned int sprite_pixels[];
static GSTEXTURE atlases[SPRITE_PAGE_COUNT];
static int ready,bank=-1;
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
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        const SpritePage *page=&sprite_pages[next][i];GSTEXTURE *atlas=&atlases[i];
        if(!page->width)continue;
        atlas->Width=page->width;atlas->Height=page->height;
        atlas->PSM=GS_PSM_CT32;atlas->Filter=GS_FILTER_NEAREST;
        atlas->Mem=(unsigned int*)((unsigned char*)sprite_pixels+page->offset);
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
