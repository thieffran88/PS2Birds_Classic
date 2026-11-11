#include "sprites.h"
#include <gsInline.h>
#include <math.h>
#include <string.h>
extern unsigned int sprite_pixels[];
static GSTEXTURE atlases[SPRITE_PAGE_COUNT];
static int ready;
int sprites_init(GSGLOBAL *gs)
{
    for(int i=0;i<SPRITE_PAGE_COUNT;i++){
        GSTEXTURE *atlas=&atlases[i];
        memset(atlas,0,sizeof(*atlas));
        atlas->Width=sprite_pages[i].width;atlas->Height=sprite_pages[i].height;
        atlas->PSM=GS_PSM_CT32;atlas->Filter=GS_FILTER_NEAREST;
        atlas->Mem=(unsigned int*)((unsigned char*)sprite_pixels+sprite_pages[i].offset);
        atlas->Vram=gsKit_vram_alloc(gs,gsKit_texture_size(atlas->Width,atlas->Height,atlas->PSM),GSKIT_ALLOC_USERBUFFER);
        if(atlas->Vram==GSKIT_ALLOC_ERROR)return 0;
        gsKit_texture_upload(gs,atlas);
    }
    ready=1;return 1;
}
void sprite_draw_tint(GSGLOBAL *gs,int id,float x,float y,float scale,float angle,unsigned int rgba)
{
    if(!ready || id<0 || id>=SPRITE_COUNT)return;
    const SpriteFrame *s=&sprite_frames[id];
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
