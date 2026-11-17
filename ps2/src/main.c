#include <tamtypes.h>
#include <kernel.h>
#include <timer.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "menu.h"
#include "level_end.h"
#include "music.h"
#include "ui.h"
#include "font.h"
#include "menu_gfx.h"
#include "background.h"
#include "cutscene.h"
#include "sprites.h"
/* Piece-to-sprite table: the Classic's, or the Rio's (make GAME=rio). */
#ifndef PS2BIRDS_DEFINITION_SPRITES
#define PS2BIRDS_DEFINITION_SPRITES "definition_sprites.h"
#endif
#include PS2BIRDS_DEFINITION_SPRITES
#include "save_mc.h"
#include "audio.h"
#include "save_assets.h"

static GSGLOBAL *gs;
static char pad_buffer[256] __attribute__((aligned(64)));
static int pad_open;

/* Small built-in 5x7 alphabet: no external fonts or files needed to boot. */
static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:.-/+*";
static const unsigned char glyphs[][7] = {
 {14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
 {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},
 {14,17,16,23,17,17,15},{17,17,17,31,17,17,17},{14,4,4,4,4,4,14},
 {7,2,2,2,2,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
 {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
 {15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},
 {17,17,17,17,17,10,4},{17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
 {17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},
 {30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},
 {14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},
 {14,17,17,15,1,1,14},{0,4,4,0,4,4,0},{0,0,0,0,0,4,4},
 {0,0,0,31,0,0,0},{1,2,2,4,8,8,16},{0,4,4,31,4,4,0},
 {4,4,31,14,14,27,17}
};

static u64 color(int r, int g, int b) { return GS_SETREG_RGBAQ(r,g,b,0x80,0); }
static void rect(float x, float y, float w, float h, u64 c)
{ gsKit_prim_sprite(gs, x,y,x+w,y+h,1,c); }

/* The built-in 5 x 7 pixel font: crisp at small sizes (used on the pills of the button prompts). */
static void pixel_text(float x, float y, int scale, u64 c, const char *s)
{
    for (; *s; s++, x += scale * 6) {
        const char *at = strchr(alphabet, *s);
        if (!at || *s == ' ') continue;
        const unsigned char *glyph = glyphs[at - alphabet];
        for (int row=0; row<7; row++)
            for (int col=0; col<5; col++)
                if (glyph[row] & (16 >> col))
                    rect(x+col*scale,y+row*scale,scale,scale,c);
    }
}
static void text(float x, float y, int scale, u64 c, const char *s)
{
    if(font_ready()){
        /* Original FONT_BASIC (white fill, black outline) tinted with the colour. */
        unsigned r=(unsigned)(c&255),gr=(unsigned)((c>>8)&255),b=(unsigned)((c>>16)&255);
        font_draw(gs,FONT_BASIC,x,y-2*scale,0.23f+0.48f*scale,FONT_LEFT,(r/2)|((gr/2)<<8)|((b/2)<<16)|(0x80u<<24),s);
        return;
    }
    pixel_text(x,y,scale,c,s);
}

static void disk(float x, float y, float radius, u64 c)
{
    /* Scanline sprites share the same GS path as text and UI rectangles. */
    for (int row=-(int)radius; row<(int)radius; row++) {
        float offset=row+0.5f;
        float half=sqrtf(fmaxf(0,radius*radius-offset*offset));
        rect(x-half,y+row,half*2,1,c);
    }
}

/* ---- Button prompts. The PlayStation glyphs are drawn with primitives (a coloured ring and symbol
   on a dark centre, like the face-button icons of a PlayStation prompt set; the d-pad and
   shoulder buttons in pale blue-grey), so they need no texture and work on every screen. A hint
   string marks them with braces: "{X} CONFIRMAR   {PADLR} ESCOLHER". ---- */
enum {G_CROSS,G_CIRCLE,G_TRIANGLE,G_SQUARE,G_PAD,G_PAD_UD,G_PAD_LR,G_L1,G_L2,G_R1,G_R2,G_R3,G_START,G_SELECT,G_NONE};
/* The functions below are kept out of line on purpose. Inlined into the very large render(), the
   build showed a black screen in PCSX2 from the first frame (the cause was not found; keeping them
   separate fixed it, and unrelated size changes to the program did not bring it back). */
static int glyph_of(const char *t,int n){
    static const struct {const char *name;int g;} tab[]={{"X",G_CROSS},{"O",G_CIRCLE},{"T",G_TRIANGLE},{"Q",G_SQUARE},
        {"PAD",G_PAD},{"PADUD",G_PAD_UD},{"PADLR",G_PAD_LR},{"L1",G_L1},{"L2",G_L2},{"R1",G_R1},{"R2",G_R2},{"R3",G_R3},
        {"START",G_START},{"SELECT",G_SELECT}};
    for(unsigned i=0;i<sizeof(tab)/sizeof(tab[0]);i++)if((int)strlen(tab[i].name)==n && !strncmp(tab[i].name,t,(size_t)n))return tab[i].g;
    return G_NONE;
}
static const char *const glyph_label[]={"L1","L2","R1","R2","R3","START","SELECT"};
static int glyph_pixels(float s){int ps=(int)(s/16+0.5f);return ps<1?1:ps;}   /* pixel-font scale that suits a glyph of size s */
static float glyph_width(int g,float s){
    if(g<G_L1 || g>G_SELECT)return s;
    int ps=glyph_pixels(s);return (float)strlen(glyph_label[g-G_L1])*6*ps+6*ps;
}
static void glyph_triangle(float cx,float cy,float r,u64 c){   /* apex up; r = circumradius */
    float top=cy-r,bottom=cy+r*0.5f,half=r*0.866f;
    for(float y=top;y<bottom;y+=1){float t=(y-top)/(bottom-top);rect(cx-half*t,y,half*2*t,1,c);}
}
__attribute__((noinline)) static void glyph_draw(int g,float cx,float cy,float s){
    const u64 dark=color(26,30,42),light=color(174,189,208),dim=color(92,106,128);
    if(g<=G_SQUARE){
        static const unsigned char rgb[4][3]={{122,132,196},{224,88,84},{0,190,168},{224,112,150}};   /* cross, circle, triangle, square */
        u64 c=color(rgb[g][0],rgb[g][1],rgb[g][2]);
        float ring=fmaxf(1.5f,s*0.09f),r=s*0.27f,t=fmaxf(1.6f,s*0.11f);
        disk(cx,cy,s/2,c);disk(cx,cy,s/2-ring,dark);
        if(g==G_CROSS){for(float i=-r;i<=r;i+=0.6f){rect(cx+i-t/2,cy+i-t/2,t,t,c);rect(cx+i-t/2,cy-i-t/2,t,t,c);}}
        else if(g==G_CIRCLE){disk(cx,cy,r,c);disk(cx,cy,r-t,dark);}
        else if(g==G_SQUARE){rect(cx-r,cy-r,2*r,2*r,c);rect(cx-r+t,cy-r+t,2*(r-t),2*(r-t),dark);}
        else{glyph_triangle(cx,cy+r*0.1f,r*1.15f,c);glyph_triangle(cx,cy+r*0.1f+t*0.6f,r*1.15f-t*1.7f,dark);}
        return;
    }
    if(g<=G_PAD_LR){   /* d-pad: a plus; the arms the hint stands for are bright, the others dim */
        float a=s*0.36f;
        rect(cx-s/2,cy-a/2,s,a,g==G_PAD_UD?dim:light);
        rect(cx-a/2,cy-s/2,a,s,g==G_PAD_LR?dim:light);
        return;
    }
    /* shoulder buttons, START and SELECT: a rounded pill with the name on it */
    const char *l=glyph_label[g-G_L1];int ps=glyph_pixels(s);float w=glyph_width(g,s),h=(float)(7*ps+6*ps),r=h/2;
    rect(cx-w/2+r,cy-h/2,w-2*r,h,light);disk(cx-w/2+r,cy,r,light);disk(cx+w/2-r,cy,r,light);
    pixel_text(cx-((float)strlen(l)*6*ps-ps)/2,cy-3.5f*ps,ps,dark,l);
}
static float text_px(int scale,const char *s){return font_ready()?font_width(FONT_BASIC,s,0.23f+0.48f*scale):(float)strlen(s)*6*scale;}
/* Draws (or only measures) a hint line and returns its width. y is the text() y. */
__attribute__((noinline)) static float hint_run(float x,float y,int scale,u64 col,const char *s,int draw){
    float size=16.0f*scale,cx=x;
    for(const char *p=s;*p;){
        if(*p=='{'){
            const char *e=strchr(p,'}');int g=e?glyph_of(p+1,(int)(e-p-1)):G_NONE;
            if(g!=G_NONE){float w=glyph_width(g,size);if(draw)glyph_draw(g,cx+w/2,y+5.5f*scale,size);cx+=w+size*0.2f;p=e+1;continue;}
        }
        char seg[96];int n=0;seg[n++]=*p++;
        while(*p && *p!='{' && n<95)seg[n++]=*p++;
        seg[n]=0;if(draw)text(cx,y,scale,col,seg);cx+=text_px(scale,seg);
    }
    return cx-x;
}
__attribute__((noinline)) static void hint(float x,float y,int scale,int center,u64 col,const char *s){
    float w=hint_run(0,0,scale,col,s,0);hint_run(center?x-w/2:x,y,scale,col,s,1);
}
/* The memory-card status ("... - L1: TENTAR") with the L1 prompt as a glyph. */
__attribute__((noinline)) static void mc_status(float x,float y,u64 col){
    char buf[96];size_t n=0;
    for(const char *p=save_mc_status();*p && n<88;){
        if(!strncmp(p,"L1:",3)){memcpy(buf+n,"{L1}",4);n+=4;p+=3;}
        else buf[n++]=*p++;
    }
    buf[n]=0;hint(x,y,1,0,col,buf);
}

static unsigned int visual_frame;
static float render_scale,render_pixels,render_ground,render_sling_x,render_sling_y;
#ifdef PS2BIRDS_RIO
/* Rio build (make GAME=rio): pieces are drawn from the Rio piece-to-sprite table (rio/definition_sprites.h),
   which the generator makes from the player's own APK; there are no pigs, and only the red bird exists. */
static int terrain_baked;
static int ground_mask(const GameObject *o){(void)o;return -1;}
static int rio_sprite_of(const char *definition){
    for(const DefinitionSprite *d=definition_sprites;d->definition;d++)if(!strcmp(definition,d->definition))return d->sprite;
    return -1;
}
static void bird(float x,float y,float radius,int type,float angle,int collided,float fuse,int special_used)
{
    (void)type;(void)collided;(void)fuse;(void)special_used;
    int id=rio_sprite_of("RedBird");if(id<0)return;
    sprite_draw(gs,id,x,y,(render_scale/render_pixels)*(radius/6.0f),angle);
}
static void block(const GameObject *o)
{
    for(const DefinitionSprite *d=definition_sprites;d->definition;d++)if(!strcmp(o->definition,d->definition)){
        sprites_set_flip(o->mirror&OBJECT_MIRROR_X,o->mirror&OBJECT_MIRROR_Y);
        sprite_draw(gs,d->sprite,o->x,o->y,(render_scale/render_pixels)*d->scale,o->angle);
        sprites_set_flip(0,0);return;
    }
}
static void pig(const GameObject *o){block(o);}   /* the Rio goals (cages) are pieces of the table */
#else
static void bird(float x,float y,float radius,int type,float angle,int collided,float fuse,int special_used)
{
    /* GlobeBird: puffed sprite is stored at half resolution; fuse carries the puffed size (0..1). */
    if(type==BIRD_PUFFER_TYPE && fuse>0){sprite_draw(gs,SPR_BIRD_PUFFER_2,x,y,(render_scale/render_pixels)*SPRITE_MASK_SCALE*fuse,angle);return;}
    int id=type==BIRD_SHOCK_TYPE?(collided?SPR_BIRD_GREY_BLUE_FLYING:SPR_BIRD_GREY_BLUE):type==BIRD_PINK_TYPE?(special_used?SPR_BIRD_PINK_RELEASE:SPR_BIRD_PINK):type==BIRD_PUFFER_TYPE?SPR_BIRD_PUFFER_1:type==BIRD_BIG_TYPE?SPR_BIRD_BIG_BROTHER:type==BIRD_EGG_TYPE?SPR_DROPPABLE_EGG:type==BIRD_WHITE_TYPE?((collided || special_used)?SPR_BIRD_GREEN_COLLISION:fuse==-2?SPR_BIRD_GREEN:SPR_BIRD_GREEN_FLYING):type==BIRD_BLACK_TYPE?(fuse>=0?(fuse<0.33f?SPR_BIRD_GREY_1:fuse<0.66f?SPR_BIRD_GREY_2:SPR_BIRD_GREY_3):fuse==-2?SPR_BIRD_GREY:SPR_BIRD_GREY_FLYING):type==BIRD_GREEN_TYPE?(collided?SPR_BIRD_BOOMERANG_COLLISION:special_used?SPR_BIRD_BOOMERANG_SPECIAL:SPR_BIRD_BOOMERANG):type==BIRD_BLUE_TYPE?(collided?SPR_BIRD_BLUE_COLLISION:SPR_BIRD_BLUE):type==BIRD_YELLOW_TYPE?(collided?SPR_BIRD_YELLOW_COLLISION:SPR_BIRD_YELLOW):SPR_BIRD_RED;
    sprite_draw(gs,id,x,y,(render_scale/render_pixels)*(radius/6.0f),angle);
}
static void pig(const GameObject *o)
{
    typedef struct {const char *name;int normal,blink,hurt;} PigSprites;
    static const PigSprites pig_sprites[]={
     {"MediumPiglette",SPR_PIG_BASIC_MEDIUM_01,SPR_PIG_BASIC_MEDIUM_01_BLINK,SPR_PIG_BASIC_MEDIUM_02},
     {"LargePiglette",SPR_PIG_BASIC_BIG_01,SPR_PIG_BASIC_BIG_01_BLINK,SPR_PIG_BASIC_BIG_02},
     {"HelmetPiglette",SPR_PIG_HELMET_01,SPR_PIG_HELMET_01_BLINK,SPR_PIG_HELMET_02},
     {"GrandpaPiglette",SPR_PIG_MUSTACHE_01,SPR_PIG_MUSTACHE_01_BLINK,SPR_PIG_MUSTACHE_02},
     {"KingPiglette",SPR_PIG_KING_01,SPR_PIG_KING_01,SPR_PIG_KING_02},
     {"SmallPiglette",SPR_PIG_BASIC_SMALL_01,SPR_PIG_BASIC_SMALL_01_BLINK,SPR_PIG_BASIC_SMALL_02},
    };
    /* Pigs added from blocks.lua (e.g. the bearded king of Bird Island) use their own sprite. */
    for(const DefinitionSprite *d=definition_sprites;d->definition;d++)if(!strcmp(o->definition,d->definition)){sprite_draw(gs,d->sprite,o->x,o->y,(render_scale/render_pixels)*d->scale,o->angle);return;}
    /* Unknown pig definitions fall back to the small pig (last entry). */
    const PigSprites *set=&pig_sprites[sizeof(pig_sprites)/sizeof(pig_sprites[0])-1];
    for(unsigned int i=0;i<sizeof(pig_sprites)/sizeof(pig_sprites[0]);i++)if(!strcmp(o->definition,pig_sprites[i].name)){set=&pig_sprites[i];break;}
    int id=o->health<0.5f?set->hurt:(visual_frame%240<8)?set->blink:set->normal;
    /* The king pig frames are stored at half resolution. */
    float scale=(render_scale/render_pixels)*(!strcmp(set->name,"KingPiglette")?SPRITE_MASK_SCALE:1);
    sprite_draw(gs,id,o->x,o->y,scale,0);
}
typedef struct {const char *name;int frames[4];} BlockSprites;
static const BlockSprites block_sprites[]={
 {"WoodBlock1",{SPR_BLOCK_WOOD_1_1,SPR_BLOCK_WOOD_2_1,SPR_BLOCK_WOOD_3_1,SPR_BLOCK_WOOD_4_1}},
 {"WoodBlock2",{SPR_BLOCK_WOOD_1_2,SPR_BLOCK_WOOD_2_2,SPR_BLOCK_WOOD_3_2,SPR_BLOCK_WOOD_4_2}},
 {"WoodBlock3",{SPR_BLOCK_WOOD_1_3,SPR_BLOCK_WOOD_2_3,SPR_BLOCK_WOOD_3_3,SPR_BLOCK_WOOD_4_3}},
 {"WoodBlock4",{SPR_BLOCK_WOOD_1_4,SPR_BLOCK_WOOD_2_4,SPR_BLOCK_WOOD_3_4,SPR_BLOCK_WOOD_4_4}},
 {"WoodBlock5",{SPR_BLOCK_WOOD_1_5,SPR_BLOCK_WOOD_2_5,SPR_BLOCK_WOOD_3_5,SPR_BLOCK_WOOD_4_5}},
 {"WoodBlock6",{SPR_BLOCK_WOOD_1_6,SPR_BLOCK_WOOD_2_6,SPR_BLOCK_WOOD_3_6,SPR_BLOCK_WOOD_4_6}},
 {"WoodBlock7",{SPR_BLOCK_WOOD_1_7,SPR_BLOCK_WOOD_2_7,SPR_BLOCK_WOOD_3_7,SPR_BLOCK_WOOD_4_7}},
 {"WoodBlock8",{SPR_BLOCK_WOOD_1_8,SPR_BLOCK_WOOD_2_8,SPR_BLOCK_WOOD_3_8,SPR_BLOCK_WOOD_4_8}},
 {"WoodBlock9",{SPR_BLOCK_WOOD_4X4_1,SPR_BLOCK_WOOD_4X4_2,SPR_BLOCK_WOOD_4X4_3,SPR_BLOCK_WOOD_4X4_4}},
 {"WoodBlock10",{SPR_BLOCK_WOOD_10X1_1,SPR_BLOCK_WOOD_10X1_2,SPR_BLOCK_WOOD_10X1_3,SPR_BLOCK_WOOD_10X1_4}},
 {"WoodBlock11",{SPR_BLOCK_WOOD_TRIANGLE_L_1,SPR_BLOCK_WOOD_TRIANGLE_L_2,SPR_BLOCK_WOOD_TRIANGLE_L_3,SPR_BLOCK_WOOD_TRIANGLE_L_4}},
 {"WoodBlock12",{SPR_BLOCK_WOOD_TRIANGLE_M_1,SPR_BLOCK_WOOD_TRIANGLE_M_2,SPR_BLOCK_WOOD_TRIANGLE_M_3,SPR_BLOCK_WOOD_TRIANGLE_M_4}},
 {"LightBlock9",{SPR_BLOCK_LIGHT_4X4_1,SPR_BLOCK_LIGHT_4X4_2,SPR_BLOCK_LIGHT_4X4_3,SPR_BLOCK_LIGHT_4X4_4}},
 {"LightBlock10",{SPR_BLOCK_LIGHT_10X1_1,SPR_BLOCK_LIGHT_10X1_2,SPR_BLOCK_LIGHT_10X1_3,SPR_BLOCK_LIGHT_10X1_4}},
 {"LightBlock11",{SPR_BLOCK_LIGHT_TRIANGLE_L_1,SPR_BLOCK_LIGHT_TRIANGLE_L_2,SPR_BLOCK_LIGHT_TRIANGLE_L_3,SPR_BLOCK_LIGHT_TRIANGLE_L_4}},
 {"LightBlock12",{SPR_BLOCK_LIGHT_TRIANGLE_M_1,SPR_BLOCK_LIGHT_TRIANGLE_M_2,SPR_BLOCK_LIGHT_TRIANGLE_M_3,SPR_BLOCK_LIGHT_TRIANGLE_M_4}},
 {"StoneBlock9",{SPR_BLOCK_ROCK_4X4_1,SPR_BLOCK_ROCK_4X4_2,SPR_BLOCK_ROCK_4X4_3,SPR_BLOCK_ROCK_4X4_4}},
 {"StoneBlock10",{SPR_BLOCK_ROCK_10X1_1,SPR_BLOCK_ROCK_10X1_2,SPR_BLOCK_ROCK_10X1_3,SPR_BLOCK_ROCK_10X1_4}},
 {"StoneBlock11",{SPR_BLOCK_ROCK_TRIANGLE_L_1,SPR_BLOCK_ROCK_TRIANGLE_L_2,SPR_BLOCK_ROCK_TRIANGLE_L_3,SPR_BLOCK_ROCK_TRIANGLE_L_4}},
 {"StoneBlock12",{SPR_BLOCK_ROCK_TRIANGLE_M_1,SPR_BLOCK_ROCK_TRIANGLE_M_2,SPR_BLOCK_ROCK_TRIANGLE_M_3,SPR_BLOCK_ROCK_TRIANGLE_M_4}},
 {"StoneBlock1",{SPR_BLOCK_ROCK_1_1,SPR_BLOCK_ROCK_2_1,SPR_BLOCK_ROCK_3_1,SPR_BLOCK_ROCK_4_1}},
 {"StoneBlock2",{SPR_BLOCK_ROCK_1_2,SPR_BLOCK_ROCK_2_2,SPR_BLOCK_ROCK_3_2,SPR_BLOCK_ROCK_4_2}},
 {"LightBlock1",{SPR_BLOCK_LIGHT_1_1,SPR_BLOCK_LIGHT_2_1,SPR_BLOCK_LIGHT_3_1,SPR_BLOCK_LIGHT_4_1}},
 {"LightBlock2",{SPR_BLOCK_LIGHT_1_2,SPR_BLOCK_LIGHT_2_2,SPR_BLOCK_LIGHT_3_2,SPR_BLOCK_LIGHT_4_2}},
 {"LightBlock3",{SPR_BLOCK_LIGHT_1_3,SPR_BLOCK_LIGHT_2_3,SPR_BLOCK_LIGHT_3_3,SPR_BLOCK_LIGHT_4_3}},
 {"StoneBlock3",{SPR_BLOCK_ROCK_1_3,SPR_BLOCK_ROCK_2_3,SPR_BLOCK_ROCK_3_3,SPR_BLOCK_ROCK_4_3}},
 {"StoneBlock7",{SPR_BLOCK_ROCK_1_7,SPR_BLOCK_ROCK_2_7,SPR_BLOCK_ROCK_3_7,SPR_BLOCK_ROCK_4_7}},
 {"StoneBlock8",{SPR_BLOCK_ROCK_1_8,SPR_BLOCK_ROCK_2_8,SPR_BLOCK_ROCK_3_8,SPR_BLOCK_ROCK_4_8}},
 {"LightBlock4",{SPR_BLOCK_LIGHT_1_4,SPR_BLOCK_LIGHT_2_4,SPR_BLOCK_LIGHT_3_4,SPR_BLOCK_LIGHT_4_4}},
 {"LightBlock5",{SPR_BLOCK_LIGHT_1_5,SPR_BLOCK_LIGHT_2_5,SPR_BLOCK_LIGHT_3_5,SPR_BLOCK_LIGHT_4_5}},
 {"LightBlock7",{SPR_BLOCK_LIGHT_1_7,SPR_BLOCK_LIGHT_2_7,SPR_BLOCK_LIGHT_3_7,SPR_BLOCK_LIGHT_4_7}},
 {"LightBlock8",{SPR_BLOCK_LIGHT_1_8,SPR_BLOCK_LIGHT_2_8,SPR_BLOCK_LIGHT_3_8,SPR_BLOCK_LIGHT_4_8}},
 {"LightBlock6",{SPR_BLOCK_LIGHT_1_6,SPR_BLOCK_LIGHT_2_6,SPR_BLOCK_LIGHT_3_6,SPR_BLOCK_LIGHT_4_6}},
 {"StoneBlock4",{SPR_BLOCK_ROCK_1_4,SPR_BLOCK_ROCK_2_4,SPR_BLOCK_ROCK_3_4,SPR_BLOCK_ROCK_4_4}},
 {"StoneBlock5",{SPR_BLOCK_ROCK_1_5,SPR_BLOCK_ROCK_2_5,SPR_BLOCK_ROCK_3_5,SPR_BLOCK_ROCK_4_5}},
 {"StoneBlock6",{SPR_BLOCK_ROCK_1_6,SPR_BLOCK_ROCK_2_6,SPR_BLOCK_ROCK_3_6,SPR_BLOCK_ROCK_4_6}},
 {"ExtraStrongBall",{SPR_BLOCK_STEEL_BALL,SPR_BLOCK_STEEL_BALL,SPR_BLOCK_STEEL_BALL,SPR_BLOCK_STEEL_BALL}},
};
static void block_unflipped(const GameObject *o);
static void block(const GameObject *o)
{
    sprites_set_flip(o->mirror&OBJECT_MIRROR_X,o->mirror&OBJECT_MIRROR_Y);block_unflipped(o);sprites_set_flip(0,0);
}
static int terrain_baked;   /* the stage's fixed ground masks are painted into the background picture */
/* GROUND_BLOCK mask of a fixed (kind 3) terrain piece, -1 for every other piece. */
static int ground_mask(const GameObject *o)
{
    if(o->kind!=3)return -1;
    for(const DefinitionSprite *d=definition_sprites;d->definition;d++)
        if(!strcmp(o->definition,d->definition) && d->sprite!=SPR_GROUND_BLOCK_05 && d->sprite!=SPR_GROUND_BLOCK_01 && d->sprite!=SPR_GROUND_BLOCK_02 && d->sprite!=SPR_GROUND_BLOCK_03 && d->sprite!=SPR_GROUND_BLOCK_04 && d->sprite!=SPR_GROUND_BLOCK_06)return -1;
    if(!strcmp(o->definition,"StaticBlock_06"))return SPR_GROUND_BLOCK_06;
    if(!strcmp(o->definition,"StaticBlock_01"))return SPR_GROUND_BLOCK_01;
    if(!strcmp(o->definition,"StaticBlock_02"))return SPR_GROUND_BLOCK_02;
    if(!strcmp(o->definition,"StaticBlock_03"))return SPR_GROUND_BLOCK_03;
    if(!strcmp(o->definition,"StaticBlock_04"))return SPR_GROUND_BLOCK_04;
    if(!strcmp(o->definition,"StaticBlock_05"))return SPR_GROUND_BLOCK_05;
    return -1;
}
static void block_unflipped(const GameObject *o)
{
    if(terrain_baked && ground_mask(o)>=0)return;
    if(o->kind==3 || o->kind==OBJECT_KIND_FRAGILE_GROUND){
        /* Fixed pieces added from blocks.lua with their own colour sprite (rulers, pencils...). */
        for(const DefinitionSprite *d=definition_sprites;d->definition;d++)
            if(!strcmp(o->definition,d->definition) && d->sprite!=SPR_GROUND_BLOCK_05 && d->sprite!=SPR_GROUND_BLOCK_01 && d->sprite!=SPR_GROUND_BLOCK_02 && d->sprite!=SPR_GROUND_BLOCK_03 && d->sprite!=SPR_GROUND_BLOCK_04 && d->sprite!=SPR_GROUND_BLOCK_06){
                sprite_draw(gs,d->sprite,o->x,o->y,(render_scale/render_pixels)*d->scale,o->angle);return;}
        int id=(!strcmp(o->definition,"StaticBlock_06")||!strcmp(o->definition,"StaticFragileBlock06"))?SPR_GROUND_BLOCK_06:!strcmp(o->definition,"StaticBlock_01")?SPR_GROUND_BLOCK_01:!strcmp(o->definition,"StaticBlock_02")?SPR_GROUND_BLOCK_02:!strcmp(o->definition,"StaticBlock_03")?SPR_GROUND_BLOCK_03:!strcmp(o->definition,"StaticBlock_04")?SPR_GROUND_BLOCK_04:!strcmp(o->definition,"StaticBlock_05")?SPR_GROUND_BLOCK_05:!strcmp(o->definition,"Estrade01")?SPR_ESTRADE_01:!strcmp(o->definition,"Estrade03")?SPR_ESTRADE_03:!strcmp(o->definition,"ExtraTrampoline2")?SPR_BLOCK_TRAMPOLINE_01:!strcmp(o->definition,"ExtraSuperBowl")?SPR_B_EGG_SUPER_BOWL:SPR_ESTRADE_02;
        int estrade=id==SPR_ESTRADE_01 || id==SPR_ESTRADE_02 || id==SPR_ESTRADE_03 || id==SPR_BLOCK_TRAMPOLINE_01 || id==SPR_B_EGG_SUPER_BOWL;
        unsigned int tint=estrade?0x80808080:0x80284466;
        /* Ground masks are stored at 1/SPRITE_MASK_SCALE resolution. */
        sprite_draw_tint(gs,id,o->x,o->y,(render_scale/render_pixels)*(estrade?1:SPRITE_MASK_SCALE),o->angle,tint);return;
    }
    if(o->kind==OBJECT_KIND_FRAGILE){
        int id=!strcmp(o->definition,"StaticBalloon01")?SPR_BLOCK_PIG_BALLOON_1:!strcmp(o->definition,"StaticBalloon02")?SPR_BLOCK_PIG_BALLOON_2:!strcmp(o->definition,"StaticBalloon04")?SPR_BLOCK_BALLOON_2:!strcmp(o->definition,"StaticBalloon05")?SPR_BLOCK_BALLOON_3:SPR_BLOCK_BALLOON_1;
        sprite_draw(gs,id,o->x,o->y,render_scale/render_pixels,o->angle);return;
    }
    if(o->kind==OBJECT_KIND_TNT){sprite_draw(gs,SPR_BLOCK_EXTRA_TNT,o->x,o->y,render_scale/render_pixels,o->angle);return;}
    if(o->kind==6){
        for(const DefinitionSprite *d=definition_sprites;d->definition;d++)if(!strcmp(o->definition,d->definition)){sprite_draw(gs,d->sprite,o->x,o->y,(render_scale/render_pixels)*d->scale,o->angle);return;}
        int id=!strcmp(o->definition,"ExtraHam")?SPR_BLOCK_HAM:!strcmp(o->definition,"ExtraGoldenEgg")?SPR_BLOCK_GOLDEN_EGG:!strcmp(o->definition,"ExtraBlockFlag02")?SPR_BLOCK_EXTRA_FLAG_02:!strcmp(o->definition,"ExtraBlockFlag01")?SPR_BLOCK_EXTRA_FLAG_01:!strcmp(o->definition,"ExtraBlockStairs")?SPR_ESTRADE_STAIRS:!strcmp(o->definition,"ExtraTire02")?SPR_BLOCK_TIRE_02:!strcmp(o->definition,"ExtraBlockDice")?SPR_BLOCK_EXTRA_DICE_6:!strcmp(o->definition,"ExtraStolenEgg")?SPR_BLOCK_STOLEN_EGG:!strcmp(o->definition,"ExtraDonut01")?SPR_BLOCK_DONUT:!strcmp(o->definition,"ExtraDiamond")?SPR_BLOCK_DIAMOND:!strcmp(o->definition,"ExtraHolyGrail")?SPR_BLOCK_HOLYGRAIL:!strcmp(o->definition,"ExtraPillar")?SPR_BLOCK_PILLAR:!strcmp(o->definition,"ExtraBanana")?SPR_BLOCK_BANANA:!strcmp(o->definition,"ExtraMelon")?SPR_BLOCK_WATERMELON:!strcmp(o->definition,"ExtraTire01")?SPR_BLOCK_TIRE_01:!strcmp(o->definition,"ExtraTire03")?SPR_BLOCK_TIRE_03:!strcmp(o->definition,"ExtraStrawberry")?SPR_BLOCK_STRAWBERRY:!strcmp(o->definition,"ExtraApple")?SPR_BLOCK_APPLE:!strcmp(o->definition,"ExtraRopeThick01")?SPR_BLOCK_ROPE_THICK_1:!strcmp(o->definition,"ExtraCowboyHelmetSmall_1")?SPR_COWBOY_HAT_SMALL_1:!strcmp(o->definition,"ExtraCowboyHelmet_1")?SPR_COWBOY_HAT_BIG_1:!strcmp(o->definition,"ExtraSheriffHat_1")?SPR_SHERIFF_HAT_1:!strcmp(o->definition,"ExtraSheriffHat_2")?SPR_SHERIFF_HAT_2:!strcmp(o->definition,"ExtraBanditoHat_1")?SPR_BANDITO_HAT_1:!strcmp(o->definition,"BP_ROPE_THICK")?SPR_SHORT_THICK_ROPE:!strcmp(o->definition,"ExtraHelmetSmall")?SPR_HELMET_SMALL:!strcmp(o->definition,"ExtraHelmetBig")?SPR_HELMET_BIG:!strcmp(o->definition,"ExtraHelmetBoss")?SPR_HELMET_BOSS:!strcmp(o->definition,"ExtraTrampoline")?SPR_BLOCK_SUPER_BALL:!strcmp(o->definition,"BlockCarpet")?SPR_BLOCK_CARPET:!strcmp(o->definition,"ExtraBoomerangBird")?SPR_BIRD_BOOMERANG:!strcmp(o->definition,"ExtraBeachBall")?SPR_BLOCK_BEACHBALL:!strcmp(o->definition,"ExtraRubberDuck")?SPR_BLOCK_RUBBERDUCK:!strcmp(o->definition,"ExtraBlockSmiley")?(o->health<0.5f?SPR_BLOCK_SMILEY_3:SPR_BLOCK_SMILEY_1):SPR_BLOCK_TREASURE_CHEST;
        sprite_draw(gs,id,o->x,o->y,render_scale/render_pixels,o->angle);return;
    }
    /* Themed blocks added from blocks.lua (e.g. the Birdday 7 wood set) use their own sprite. */
    for(const DefinitionSprite *d=definition_sprites;d->definition;d++)if(!strcmp(o->definition,d->definition)){sprite_draw(gs,d->sprite,o->x,o->y,(render_scale/render_pixels)*d->scale,o->angle);return;}
    int stage=o->health>0.75f?0:o->health>0.5f?1:o->health>0.25f?2:3;
    for(unsigned int i=0;i<sizeof(block_sprites)/sizeof(block_sprites[0]);i++)
        if(!strcmp(o->definition,block_sprites[i].name)){
            sprite_draw(gs,block_sprites[i].frames[stage],o->x,o->y,render_scale/render_pixels,o->angle);return;
        }
}

#endif
static Menu menu_state;
static int menu_yell_request;
/* cutscenes.lua pack1_intro: plays before Poached Eggs 1-1 until that stage has a score; X/Start skips. */
static float cutscene_time=-1;static int cutscene_music_pending;   /* start the music after the first frame is up: reading the track stalls the game for seconds */
static int world_pending;   /* stage index + 1 whose first win may complete a world */
/* cutscenes.lua scenes, tied to the port's stages. Entering the first stage of an episode with no
   score yet plays its opening; the first score that completes a world (every stage with the same
   title prefix "<EPISODE> N-") plays that world's scene after the win screen. The world numbers of
   the Poached Eggs to Mine and Dine, Birdday 3 and Surf and Turf scenes match the titles; Short
   Fuse, Piggy Farm and Dino stories are WebP and are not in CUTSCENE.BIN (docs/CUTSCENES.md).
   A scene that is not in CUTSCENE.BIN is skipped. */
static const char *const episode_scene[][2]={{"POACHED EGGS","pack1_intro"},{"MIGHTY HOAX","pack2_intro"},
    {"DANGER ABOVE","pack3_intro"},{"BIG SETUP","pack4_intro"},{"HAM EM HIGH","pack5_intro"},{"MINE AND DINE","pack6_intro"},
    {"BIRDDAY","pack7_intro"},{"SURF AND TURF","pack8_intro"}};
static const char *const world_scene[][2]={
    {"POACHED EGGS 1-","world1_complete"},{"POACHED EGGS 2-","world2_complete"},{"POACHED EGGS 3-","world3_complete"},
    {"MIGHTY HOAX 4-","world4_complete"},{"MIGHTY HOAX 5-","world5_complete"},
    {"DANGER ABOVE 6-","world6_complete"},{"DANGER ABOVE 7-","world7_complete"},{"DANGER ABOVE 8-","world8_complete"},
    {"BIG SETUP 9-","world9_complete"},{"BIG SETUP 10-","world10_complete"},{"BIG SETUP 11-","world11_complete"},
    {"HAM EM HIGH 12-","world12_complete"},{"HAM EM HIGH 13-","world13_complete"},{"HAM EM HIGH 14-","world14_complete"},
    {"MINE AND DINE 15-","world15_complete"},{"MINE AND DINE 16-","world16_complete"},{"MINE AND DINE 17-","world17_complete"},
    {"BIRDDAY 3-","birdday3_complete"},
    {"SURF AND TURF 1-","worldS1_complete"},{"SURF AND TURF 2-","worldS2_complete"},{"SURF AND TURF 3-","worldS3_complete"}};
static int play_scene(const char *name){
    if(!cutscene_start(name))return 0;
    cutscene_time=0;cutscene_music_pending=1;return 1;
}
/* Returns 1 when the stage starts right away (no scene). */
static int episode_intro(int level,const int *highscores){
    int e=menu_episode_of(level);if(e<0 || level!=menu_episode_first(e))return 1;
    for(int l=level;l<level+menu_episode_size(e);l++)if(highscores[l]>0)return 1;
    for(unsigned k=0;k<sizeof(episode_scene)/sizeof(episode_scene[0]);k++)if(!strcmp(menu_episode_name(e),episode_scene[k][0]))return !play_scene(episode_scene[k][1]);
    return 1;
}
/* After a stage's first score: if that completes its world, the world's scene. */
static void world_complete(int level,const int *highscores){
    const char *t=game_level_title(level),*dash=strrchr(t,'-');if(!dash)return;
    size_t prefix=(size_t)(dash-t)+1;
    for(int l=0;l<game_level_count();l++)if(!strncmp(game_level_title(l),t,prefix) && highscores[l]<=0)return;
    for(unsigned k=0;k<sizeof(world_scene)/sizeof(world_scene[0]);k++)
        if(strlen(world_scene[k][0])==prefix && !strncmp(world_scene[k][0],t,prefix)){play_scene(world_scene[k][1]);return;}
}   /* main menu: Triangle stands for the touch that makes a bird yell */
static LevelEnd level_end;
/* gamelogic.lua: each bird left gives 10000 shown over it, 0.5 s apart (birdBuffTimer), then
   the level-complete screen opens; won_wait counts down that pause. */
static float won_wait;
/* Pause panel cursor: 0 resume, 1 restart, 2 menu, 3 sound. */
static int pause_choice;
/* Win/lose pop-up cursor, like the menus: the d-pad chooses, X confirms. 0 menu, 1 restart, 2 next (win only). */
static int end_choice;
__attribute__((noinline)) static void end_buttons(const int *id,const char *const *label,int n,const float *x,float y){
    /* The chosen button is a little bigger, with a slim light ring, and its name under it. */
    for(int b=0;b<n;b++){
        int sel=b==end_choice;float k=sel?1.12f:1.0f;
        if(sel)disk(x[b],y,ui_width(id[b])*k*0.37f+3,color(255,236,150));   /* the button art has a transparent margin: ring just wider than the visible circle */
        ui_draw(gs,id[b],x[b],y,k,sel?0x80909090:0x80606060);
    }
    text(x[end_choice]-text_px(1,label[end_choice])/2,y+30,1,color(246,191,65),label[end_choice]);
}
/* main_menu.lua: 0 PLAY, 1 the options gear; with the gear open also 2 sound and 3 about. */
static int main_choice,options_open;

/* Level button of each episode's first page (episodes.lua level_button, translated by hand). */
static const unsigned char episode_button[]={1,4,6,1,1,1,6,5,6,6,7,1,1,1,1,1,1,1,1};
static void sunburst(u64 a,u64 b,float cx,float cy)
{
    /* level_selection.lua / episode_selection.lua backdrop: rays from the sun at the bottom. */
    gsKit_clear(gs,a);
    /* Drawn as 2-pixel scanline spans (like disk()): ray i covers angles [a0,a1) above the sun. */
    for(int y=0;y<(int)cy && y<448;y+=2){
        float h=cy-y;
        for(int i=0;i<18;i+=2){
            float a0=3.14159f-(i+1)*3.14159f/18,a1=3.14159f-i*3.14159f/18;   /* angle from +x, upward */
            float x0=cx+h*cosf(a1)/sinf(a1),x1=cx+h*cosf(a0)/sinf(a0);
            if(x0>x1){float t=x0;x0=x1;x1=t;}
            if(x1<0||x0>640)continue;
            if(x0<0)x0=0;
            if(x1>640)x1=640;
            rect(x0,y,x1-x0,2,b);
        }
    }
    disk(cx,cy,70,color(255,236,120));disk(cx,cy,56,color(255,250,190));
}
static void menu_screens(const Game *g)
{
    const Menu *m=&menu_state;char line[64];
    if(m->screen==MENU_MAIN){
        /* main_menu.lua: sky, clouds, ground, MENU_LOGO and the PLAY button. */
        /* The main menu stands on the first theme's picture, as in the original. */
        if(background_prepare(0,388,-1,0,0))background_draw(gs);
        else{
        for(int y=0;y<400;y+=8){float t=y/400.0f;rect(0,y,640,8,color((int)(122+110*t),(int)(196+44*t),(int)(222+26*t)));}
        for(int i=0;i<8;i++){disk(40+i*85,250+(i%2)*10,42,color(236,244,250));}
        rect(0,262,640,140,color(236,244,250));
        for(int i=0;i<6;i++){float x=60+i*110;rect(x,300-(i%3)*18,26,100,color(196,212,236));disk(x+13,292-(i%3)*18,30,color(196,212,236));}
        rect(0,394,640,54,color(32,44,96));rect(0,388,640,8,color(105,190,40));
        }
        /* main_menu.lua animateBirds: while fewer than 0.75 x the bird list fly, a 1-in-5 chance
           per frame throws one from below the screen (layer 3..5, scale 0.2 x layer, speed
           100..350 sideways and 150..400 up scaled by the layer), pulled down at 150 px/s^2. */
        {
            enum {FLY_MAX=12,BIRD_KINDS=7};
            static struct {float x,y,vx,vy,scale,yell;int kind,layer,alive;} fly[FLY_MAX];
            const float W=640,H=448,dt=1.0f/60;int n=0;
            /* buildBirdList: the birds, plus each episode's reward_completion sprite once every
               stage has a score, or its reward_three_stars sprite once every stage has 3 stars. */
            static const char *reward_episode[MG_REWARD_COUNT]={"POACHED EGGS","MIGHTY HOAX","DANGER ABOVE","BIG SETUP",
                "HAM EM HIGH","MINE AND DINE","BIRDDAY","BAD PIGGIES","SHORT FUSE","SURF AND TURF","PIGGY FARM","DINO"};
            int kinds[BIRD_KINDS+MG_REWARD_COUNT],nk=0;
            for(int k=0;k<BIRD_KINDS;k++)kinds[nk++]=MG_FLY_BIRD+k;
            for(int e=0;e<menu_episode_count();e++){
                const char *name=menu_episode_name(e);int r=-1;
                for(int k=0;k<MG_REWARD_COUNT;k++)if(!strcmp(name,reward_episode[k]))r=k;
                if(r<0)continue;
                int f=menu_episode_first(e),size=menu_episode_size(e),done=size>0,three=size>0;
                for(int l=f;l<f+size;l++){int hs=g->highscores[l];if(hs<=0)done=0;if(game_level_stars(l,hs)<3)three=0;}
                if(three)kinds[nk++]=MG_REWARD_STAR(r);else if(done)kinds[nk++]=MG_REWARD(r);
            }
            for(int i=0;i<FLY_MAX;i++)n+=fly[i].alive;
            if(n<0.75f*nk && rand()%5==0)for(int i=0;i<FLY_MAX;i++)if(!fly[i].alive){
                int layer=3+rand()%3;float sc=layer*0.2f;
                fly[i].alive=1;fly[i].yell=0;fly[i].layer=layer;fly[i].kind=kinds[rand()%nk];fly[i].scale=sc;
                fly[i].x=-0.75f*W+(rand()%1000)/1000.0f*1.5f*W;
                fly[i].y=H+30*H/320+(H/320*20)*(5-layer);
                fly[i].vx=(100+rand()%251)*sc*(W/480+1)/2;
                fly[i].vy=-(150+rand()%251)*sc*(H/320+1)/2;
                break;
            }
            /* Touching a flying bird in the original swaps it to its <bird>_YELL sprite with its cry;
               here Triangle picks the nearest bird to the screen centre that is on screen. */
            if(menu_yell_request){
                menu_yell_request=0;int best=-1;float bd=1e9f;
                for(int i=0;i<FLY_MAX;i++){
                    if(!fly[i].alive || fly[i].kind<MG_FLY_BIRD || fly[i].kind>=MG_FLY_BIRD+BIRD_KINDS || fly[i].y<0 || fly[i].y>H)continue;
                    float d=(fly[i].x-W/2)*(fly[i].x-W/2)+(fly[i].y-H/2)*(fly[i].y-H/2);if(d<bd){bd=d;best=i;}
                }
                if(best>=0){
                    static const int cry[BIRD_KINDS]={SFX_LAUNCH_RED,SFX_LAUNCH_BLUE,SFX_LAUNCH_YELLOW,SFX_LAUNCH_BLACK,SFX_LAUNCH_RED,SFX_LAUNCH_RED,SFX_LAUNCH_RED};
                    fly[best].yell=0.7f;audio_play(cry[fly[best].kind-MG_FLY_BIRD]);
                }
            }
            for(int layer=3;layer<=5;layer++)for(int i=0;i<FLY_MAX;i++){
                if(!fly[i].alive || fly[i].layer!=layer)continue;
                fly[i].vy+=150*dt;fly[i].x+=fly[i].vx*dt;fly[i].y+=fly[i].vy*dt;
                if(fly[i].y>H+80*H/320){fly[i].alive=0;continue;}
                if(fly[i].yell>0)fly[i].yell-=dt;
                menu_gfx_draw(gs,fly[i].yell>0?fly[i].kind+(MG_YELL_BIRD-MG_FLY_BIRD):fly[i].kind,fly[i].x,fly[i].y,fly[i].scale*(fly[i].kind>=MG_FLY_BIRD+BIRD_KINDS?1.6f:fly[i].yell>0?1.0f:0.8f),0x80808080);
            }
        }
        menu_gfx_draw(gs,MG_LOGO,320,96,1.0f,0x80808080);
        font_draw(gs,FONT_BASIC,320,150,1.25f,FONT_CENTER,0x60707880,"CLASSIC");
        float k=main_choice==0?1.1f:1.0f,half=74*k,cy=262;int lw=menu_gfx_width(MG_PLAY_LEFT);
        unsigned play_tint=main_choice==0?0x80909090:0x80707070;
        menu_gfx_draw(gs,MG_PLAY_LEFT,320-half,cy,k,play_tint);
        menu_gfx_draw_span(gs,MG_PLAY_MIDDLE,320-half+lw*k/2.0f,320+half-lw*k/2.0f,cy,k);
        menu_gfx_draw(gs,MG_PLAY_RIGHT,320+half,cy,k,play_tint);
        font_draw(gs,FONT_BASIC,320,cy-24*k,1.9f*k,FONT_CENTER,0x80808080,"JOGAR");
        /* The options gear at the bottom left opens sound and about above it (TOGGLE_OPTIONS). */
        static const int ids[4]={0,MG_OPTIONS,MG_SOUNDS,MG_ABOUT};static const float ys[4]={0,404,330,256};
        for(int b=1;b<4;b++){
            if(b>1 && !options_open)continue;
            int sel=b==main_choice;float kk=sel?1.15f:1.0f;
            if(sel)disk(46,ys[b],32*kk,color(255,236,150));
            menu_gfx_draw(gs,ids[b],46,ys[b],kk,sel?0x80909090:0x80707070);
            if(b==2 && strcmp(audio_status(),"SOM LIGADO"))menu_gfx_draw(gs,MG_OFF,46,ys[b],kk,0x80808080);
        }
        hint(360,416,1,1,color(255,255,255),"{PADUD} ESCOLHER   {X} CONFIRMAR   {T} GRITO");
        return;
    }
    if(m->screen==MENU_CREDITS){
        /* credits.lua: ABOUT_BIRDS_3, the TEXT_ABOUT of the APK and the back arrow. */
        if(background_prepare(0,388,-1,0,0))background_draw(gs);else gsKit_clear(gs,color(120,194,205));
        gsKit_prim_sprite(gs,40,40,600,380,1,GS_SETREG_RGBAQ(10,24,30,0x60,0));
        menu_gfx_draw(gs,MG_ABOUT_BIRDS,130,200,1.0f,0x80808080);
        float y=70;const char *t=menu_gfx_about();char row[96];
        while(*t && y<340){
            /* One line per text line, wrapped near 34 characters, the version filled in. */
            int n=0;while(t[n] && t[n]!='\n')n++;
            char src[300];int m2=n<299?n:299;memcpy(src,t,m2);src[m2]=0;
            char out[320];const char *v=strstr(src,"$GAME_VERSION");
            if(v){snprintf(out,sizeof(out),"%.*s%s%s",(int)(v-src),src,"0.93.2 (PS2Birds Classic)",v+13);}else snprintf(out,sizeof(out),"%s",src);
            const char *o=out;
            if(!*o)y+=12;
            while(*o){
                int len=(int)strlen(o),cut=len;
                if(len>34){cut=34;while(cut>0 && o[cut]!=' ')cut--;if(cut==0)cut=34;}
                snprintf(row,sizeof(row),"%.*s",cut,o);
                font_draw(gs,FONT_BASIC,240,y,0.75f,FONT_LEFT,0x80808080,row);y+=20;
                o+=cut;while(*o==' ')o++;
            }
            t+=n;if(*t=='\n')t++;
        }
        font_draw(gs,FONT_BASIC,240,y+10,0.7f,FONT_LEFT,0x80c0d0b0,"Port não oficial para PlayStation 2:");
        font_draw(gs,FONT_BASIC,240,y+30,0.7f,FONT_LEFT,0x80c0d0b0,"PS2Birds Classic (usa o APK do jogador).");
        menu_gfx_draw(gs,MG_BACK,44,404,1.0f,0x80808080);
        hint(360,416,1,1,color(255,255,255),"{O} VOLTAR");
        return;
    }
    sunburst(color(70,150,40),color(96,176,52),320,470);
    menu_gfx_draw(gs,MG_BACK,44,404,1.0f,0x80808080);
    if(m->screen==MENU_EPISODES){
        /* episode_selection.lua: one card per episode (EPx background + title), score and stars. */
        int n=menu_episode_count();
        for(int d=-3;d<=3;d++){
            int e=m->episode+d;if(e<0||e>=n)continue;
            int f=menu_episode_first(e),size=menu_episode_size(e),score=0,stars=0,open=f<g->unlocked;
            for(int i=f;i<f+size;i++){score+=g->highscores[i];stars+=game_level_stars(i,g->highscores[i]);}
            float x=320+d*124,y=202,k=d==0?1.08f:0.94f;unsigned tint=open?0x80808080:0x80404040;
            menu_gfx_draw(gs,MG_CARD(e),x,y,k,tint);
            if(menu_gfx_width(MG_TITLE(e)))menu_gfx_draw(gs,MG_TITLE(e),x,y,k,tint);
            else font_draw(gs,FONT_BASIC,x,y-112*k,0.95f*k,FONT_CENTER,tint,menu_episode_name(e));
            snprintf(line,sizeof(line),"%d.",e+1);font_draw(gs,FONT_BASIC,x-38*k,y-128*k,0.8f*k,FONT_LEFT,0x80808080,line);
            if(!open){menu_gfx_draw(gs,MG_LEVEL_LOCK,x,y+20,k,0x80808080);continue;}
            menu_gfx_draw(gs,MG_SCOREBOX,x,y+44*k,k,0x80808080);
            menu_gfx_draw(gs,MG_SCOREBOX_TEXT,x,y+38*k,k*0.8f,0x80808080);
            snprintf(line,sizeof(line),"%d",score);font_draw(gs,FONT_BASIC,x,y+44*k,0.75f*k,FONT_CENTER,0x80808080,line);
            menu_gfx_draw(gs,MG_STARBOX,x,y+78*k,k,0x80808080);
            snprintf(line,sizeof(line),"%d/%d",stars,size*3);font_draw(gs,FONT_BASIC,x,y+80*k,0.75f*k,FONT_CENTER,0x80808080,line);
        }
        if(m->episode>0)menu_gfx_draw(gs,MG_ARROW_LEFT,30,202,1.0f,0x80808080);
        if(m->episode<n-1)menu_gfx_draw(gs,MG_ARROW_RIGHT,610,202,1.0f,0x80808080);
        hint(380,412,1,1,color(255,255,255),"{PADLR} EPISÓDIO   {X} ABRIR   {O} VOLTAR");
        return;
    }
    /* level_selection.lua: 5 x 3 buttons (the episode's LS_LEVEL_BG_NORMAL_OPEN_n), number,
       LS_STARS_n, closed button with a lock, page dots. */
    int f=menu_episode_first(m->episode),size=menu_episode_size(m->episode);
    int page=m->level/MENU_PER_PAGE,pages=(size+MENU_PER_PAGE-1)/MENU_PER_PAGE;
    int button=MG_LEVEL_OPEN_1+(m->episode<(int)sizeof(episode_button)?episode_button[m->episode]:1)-1;
    font_draw(gs,FONT_BASIC,320,14,1.1f,FONT_CENTER,0x80808080,menu_episode_name(m->episode));
    for(int c=0;c<MENU_PER_PAGE;c++){
        int l=page*MENU_PER_PAGE+c;if(l>=size)break;
        int i=f+l,open=i<g->unlocked,sel=l==m->level;
        float x=140+(c%MENU_COLUMNS)*90,y=110+(c/MENU_COLUMNS)*98,k=sel?1.14f:1.0f;
        if(open){
            menu_gfx_draw(gs,button,x,y,k,sel?0x80909090:0x80808080);
            font_draw(gs,FONT_BASIC,x,y-26*k,1.35f*k,FONT_CENTER,0x80808080,menu_level_label(i));
            int st=game_level_stars(i,g->highscores[i]);
            if(st)menu_gfx_draw(gs,MG_STARS_1+st-1,x,y+28*k,k,0x80808080);
        }else{
            menu_gfx_draw(gs,MG_LEVEL_CLOSED,x,y,k,sel?0x80909090:0x80808080);
            menu_gfx_draw(gs,MG_LEVEL_LOCK,x,y,k*0.75f,0x80808080);
        }
    }
    for(int d=0;d<pages && d<24;d++)menu_gfx_draw(gs,d==page?MG_DOT_WHITE:MG_DOT_BLACK,320-(pages-1)*8+d*16,382,1.0f,0x80808080);
    int i=f+m->level;
    if(i<g->unlocked)snprintf(line,sizeof(line),"%s   Recorde: %d",game_level_title(i),g->highscores[i]);
    else snprintf(line,sizeof(line),"%s   (bloqueada)",game_level_title(i));
    font_draw(gs,FONT_BASIC,320,346,0.8f,FONT_CENTER,0x80808080,line);
    hint(380,412,1,1,color(255,255,255),"{PAD} FASE   {L2}{R2} PÁGINA   {X} JOGAR   {O} VOLTAR");
}

static void render(const Game *g, int connected, int paused,int menu)
{
    /* Menus with the original art take the texture slots; a stage brings its bank back. */
    /* Clip to the 640 x 448 frame: pieces of sprites drawn off the bottom (the menu birds start
       below the screen) would otherwise land in the VRAM after the frame buffer. */
    gsKit_set_scissor(gs,GS_SETREG_SCISSOR(0,639,0,447));
    if(menu && font_ready() && menu_gfx_show(gs)){menu_screens(g);gsKit_queue_exec(gs);gsKit_sync_flip(gs);return;}
    /* Swap texture banks when the active stage belongs to another episode. */
    sprites_select_bank(gs,game_level_bank(g->level));menu_gfx_release();
    render_scale=g->view_scale;render_pixels=g->pixels;render_ground=g->ground_y;
    render_sling_x=g->sling_x;render_sling_y=g->sling_y;
    if(!paused && !menu)visual_frame++;
    u64 cream=color(248,240,210), dark=color(23,53,58), gold=color(246,191,65);
    char line[64];
    /* themes.lua: the stage's theme picture (sky, layers, ground) copied into the frame. */
    {
        /* The theme's ground texture on the stage's fixed terrain pieces (painted once per stage). */
        static BackgroundPiece pieces[MAX_LEVEL_OBJECTS];int count=0;
        for(int i=0;i<game_object_count(g) && count<MAX_LEVEL_OBJECTS;i++){
            GameObject o;if(!game_get_object(g,i,&o)||!o.alive)continue;
            int id=ground_mask(&o);if(id<0)continue;
            BackgroundPiece *p=&pieces[count++];p->sprite=id;p->x=o.x;p->y=o.y;p->angle=o.angle;
            p->scale=(render_scale/render_pixels)*SPRITE_MASK_SCALE;p->flip_x=(o.mirror&OBJECT_MIRROR_X)!=0;p->flip_y=(o.mirror&OBJECT_MIRROR_Y)!=0;
        }
        terrain_baked=background_prepare(game_level_theme(g->level),(int)render_ground,g->level,pieces,count);
    }
    if(terrain_baked)background_draw(gs);
    else{
    gsKit_clear(gs,color(120,194,205));
    disk(555,158,24,color(255,221,121));
    disk(75,156,16,color(219,238,219)); disk(96,148,21,color(219,238,219));
    disk(120,157,15,color(219,238,219));
    for (int y=245;y<render_ground;y++) {
        float t=(y-245)/(render_ground-245);
        rect(200-200*t,y,380*t,1,color(97,158,129));
    }
    for (int y=271;y<render_ground;y++) {
        float t=(y-271)/(render_ground-271);
        rect(448-183*t,y,375*t,1,color(81,144,119));
    }
    rect(0,render_ground,640,448-render_ground,color(119,81,53));
    rect(0,render_ground,640,8,color(133,180,67));
    rect(0,render_ground+8,640,3,color(69,113,56));
    }
    sprite_draw(gs,SPR_SLING_SHOT_01_BACK,render_sling_x,render_sling_y,render_scale/render_pixels,0);
    for(int pass=0;pass<2;pass++) for(int i=0;i<game_object_count(g);i++) {
        GameObject o;if(!game_get_object(g,i,&o)||!o.alive)continue;
        if(o.kind==4){if(pass==1)pig(&o);}
        else if(pass==0)block(&o);
    }
    if (g->state==AIMING) {
        for (int i=1; i<=28; i++) {
            float x,y; game_preview(g,i*0.1f,&x,&y);
            if (y>render_ground-3 || x>620) break;
            disk(x,y,2,color(242,240,208));
        }
    }
    for(int i=0;i<game_projectile_count(g);i++){
        GameBird projectile;if(game_get_projectile(g,i,&projectile))bird(projectile.x,projectile.y,6*projectile.scale,projectile.type,projectile.angle,projectile.collided,projectile.fuse,projectile.special_used);
    }
    GameBlast blast;
    if(game_get_blast(g,&blast)){
        float t=1-blast.life/BLACK_VISUAL_SECONDS,r=blast.radius*(0.2f+0.8f*t);
        if(blast.y>130 && blast.y<370)disk(blast.x,blast.y,6+12*t,color(69,64,58));
        for(int i=0;i<24;i++){
            float a=i*6.2831853f/24,b=(i+1)*6.2831853f/24;
            float y1=blast.y+sinf(a)*r,y2=blast.y+sinf(b)*r;
            if(y1>=109 && y1<391 && y2>=109 && y2<391)gsKit_prim_line(gs,blast.x+cosf(a)*r,y1,blast.x+cosf(b)*r,y2,3,GS_SETREG_RGBAQ(255,188,65,(int)(128*(1-t)),0));
        }
    }
    /* Particles (PARTICLES_1): fragments of the destroyed piece's material, a smoke puff for a
       pig, feathers of the bird's colour on its first impact. Drawn only: they do not touch the
       physics. A fresh popup (time 0) marks a destruction this frame. */
    {
        enum {PARTICLES=128};
        static struct {float x,y,vx,vy,angle,spin,life,max,scale;int sprite;} part[PARTICLES];static int next;
        static int prev_hit[16];
        #define SPAWN(spr,px,py,sp,sc,lf) do{int _i=next;next=(next+1)%PARTICLES;float _a=(rand()%628)/100.0f,_v=(sp)*(0.4f+(rand()%60)/100.0f);\
            part[_i].x=(px);part[_i].y=(py);part[_i].vx=cosf(_a)*_v;part[_i].vy=sinf(_a)*_v-(sp)*0.5f;part[_i].angle=_a;\
            part[_i].spin=((rand()%200)-100)/20.0f;part[_i].life=part[_i].max=(lf);part[_i].scale=(sc);part[_i].sprite=(spr);}while(0)
        for(int k=0;k<GAME_POPUPS;k++){
            const GamePopup *pp=&g->popups[k];if(pp->time!=0 || pp->life<=0)continue;
            int m=pp->material;
            if(pp->kind==POPUP_PIG)for(int i=0;i<4;i++)SPAWN(UI_SMOKE+rand()%3,pp->x,pp->y,40,1.0f,0.7f);
            else if(pp->kind==POPUP_PIECE){
                int base=m==2?UI_LIGHT:m==5?UI_ROCK:UI_WOOD,kinds=m==2?5:3;
                for(int i=0;i<6;i++)SPAWN(base+rand()%kinds,pp->x,pp->y,160,0.9f,0.9f);
            }
        }
        for(int i=0;i<game_projectile_count(g) && i<16;i++){
            GameBird b;if(!game_get_projectile(g,i,&b))continue;
            if(b.collided && !prev_hit[i]){
                static const int feather[11]={0,1,2,3,-1,-1,6,5,0,0,3};
                int f=b.type>=0 && b.type<11?feather[b.type]:0;
                if(f>=0)for(int j=0;j<6;j++)SPAWN(UI_FEATHER+f*3+rand()%3,b.x,b.y,120,0.8f,1.2f);
            }
            prev_hit[i]=b.collided;
        }
        for(int i=game_projectile_count(g);i<16;i++)prev_hit[i]=0;
        /* explosionBuff: clouds EXPLOSION_CLOUD_1-6 spread from the blast, growing as they fade. */
        if((g->events&GAME_EVENT_EXPLOSION) && g->explosion_x>-200 && g->explosion_x<840 && g->explosion_y>-200 && g->explosion_y<650)for(int i=0;i<8;i++)SPAWN(UI_CLOUD+rand()%6,g->explosion_x,g->explosion_y,70,2.2f,0.8f);
        #undef SPAWN
        const float dt=1.0f/60;
        for(int i=0;i<PARTICLES;i++){
            if(part[i].life<=0)continue;
            if(!paused && !menu){part[i].life-=dt;part[i].vy+=300*dt;part[i].x+=part[i].vx*dt;part[i].y+=part[i].vy*dt;part[i].angle+=part[i].spin*dt;}
            float t=part[i].life/part[i].max;unsigned a=(unsigned)(0x80*(t<0.3f?t/0.3f:1));
            ui_draw_rotated(gs,part[i].sprite,part[i].x,part[i].y,part[i].scale*((part[i].sprite>=UI_SMOKE && part[i].sprite<UI_SMOKE+3)||part[i].sprite>=UI_CLOUD?1.6f-0.6f*t:1),part[i].angle,0x00808080u|(a<<24));
        }
    }
    /* gamelogic.lua floating scores: pieces in FONT_SCORE; pigs (5K_GREEN) and the birds left
       (10K_<bird>) with the original score sprites. */
    for(int k=0;k<GAME_POPUPS && font_ready();k++){
        const GamePopup *pp=&g->popups[k];float sc=game_popup_scale(pp);if(sc<=0)continue;
        if(pp->kind!=POPUP_PIECE && ui_ready()){
            static const int ten_k[11]={0,1,2,3,4,4,5,0,0,0,3};
            int id=pp->kind==POPUP_PIG?UI_SCORE_5K:UI_SCORE_10K+(pp->material>=0 && pp->material<11?ten_k[pp->material]:0);
            ui_draw(gs,id,pp->x,pp->y,sc*0.9f,0x80808080);continue;
        }
        char num[16];snprintf(num,sizeof(num),"%d",pp->score);
        font_draw(gs,FONT_SCORE,pp->x,pp->y-font_line(FONT_SCORE,1.8f*sc)/2,1.8f*sc,FONT_CENTER,pp->kind==POPUP_PIG?0x80208030u:pp->kind==POPUP_BIRD?0x80303080u:0x80808080u,num);
    }
    if(g->state==AIMING)bird(g->x,g->y,6,game_queue_type(g,0),0,0,-2,0);
    sprite_draw(gs,SPR_SLING_SHOT_01_FRONT,render_sling_x,render_sling_y,render_scale/render_pixels,0);
    int waiting=g->birds-(g->state==AIMING?1:0);
    /* gamelogic.lua jumpTimer: the birds waiting by the slingshot hop now and then. The native
       timings are in compiled code; here each bird waits 1-4 s at random, then hops 10 px for
       0.35 s (approximation). */
    {
        enum {HOPPERS=16};static float wait[HOPPERS],hop[HOPPERS];
        for(int i=0;i<waiting;i++){
            float lift=0;
            if(i<HOPPERS){
                if(hop[i]>0){hop[i]-=1.0f/60;lift=hop[i]>0?10*sinf(3.14159265f*(1-hop[i]/0.35f)):0;}
                else if((wait[i]-=1.0f/60)<=0){hop[i]=0.35f;wait[i]=1.0f+(rand()%300)/100.0f;}
            }
            bird(49+i*24,render_ground-15-lift,7,game_queue_type(g,i+(g->state==AIMING?1:0)),0,0,-2,0);
        }
    }
    /* GameHud.lua: score at the top right ("Pontuação", then the number), the highscore below at
       70 %; the stage name at the top left. Texts are the original pt_BR TEXTS_BASIC entries. */
    if(font_ready()){
        font_draw(gs,FONT_BASIC,624,8,0.85f,FONT_RIGHT,0x80808080,"Pontuação");
        snprintf(line,sizeof(line),"%d",g->score);font_draw(gs,FONT_BASIC,624,26,1.5f,FONT_RIGHT,0x80808080,line);
        snprintf(line,sizeof(line),"Recorde: %d",g->highscores[g->level]);font_draw(gs,FONT_BASIC,624,60,0.8f,FONT_RIGHT,0x80808080,line);
        font_draw(gs,FONT_BASIC,16,8,1.0f,FONT_LEFT,0x80808080,game_level_title(g->level));
    }else{
        text(31,20,2,gold,game_level_title(g->level));
        snprintf(line,sizeof(line),"PONTOS %05d",g->score);text(430,22,1,cream,line);
    }
    if(sprites_load_failed())text(16,40,1,gold,"SPRITES.BIN AUSENTE: COPIE-O PARA A PASTA DO ELF");
    if(save_mc_busy())mc_status(16,40,gold);
    /* Shop tutorials: the stage's power-up (King Sling is always on; Bird Quake on L2, once). */
    if(game_powerup(g)==POWERUP_KING_SLING)text(16,58,1,cream,"ESTILINGUE REAL");
    else if(game_powerup_available(g))hint(16,58,1,0,cream,game_powerup(g)==POWERUP_POWER_POTION?"{L2} POÇÃO":game_powerup(g)==POWERUP_SHOCKWAVE?"{L2} ONDA DE CHOQUE":"{L2} TERREMOTO");
    if ((g->state==WON && won_wait<=0) || g->state==LOST || paused) {
        if(!paused && g->state==WON && ui_ready()){
            /* level_end.lua LevelComplete with the original sprites: stars at 0.4 of the height,
               spaced 1/8 of the width, the score below, the badge to its right, buttons at 0.8. */
            char line[48];
            rect(90,128,460,272,dark);
            text(172,138,2,gold,"NÍVEL CONCLUÍDO!");
            int shown=level_end_shown_stars(&level_end);
            for(int i=0;i<3;i++)ui_draw(gs,UI_STAR_1+i,220+i*100,i==1?192:200,i==1?0.95f:0.8f,i<shown?0x80808080:0x80202020);
            /* levelCompleteStars1-3: a burst of P_STAR_1-5 as each star appears (circle emitter,
               fast start, gravity pulling them down, spinning, fading out). */
            {
                enum {STARS=60};static struct {float x,y,vx,vy,a,spin,life;int s;} sp[STARS];static int sn,last;
                if(shown<last)last=0;
                for(;last<shown;last++)for(int k=0;k<20;k++){
                    int i=sn;sn=(sn+1)%STARS;float an=(rand()%628)/100.0f,v=120+rand()%260;
                    sp[i].x=220+last*100;sp[i].y=last==1?192:200;sp[i].vx=cosf(an)*v;sp[i].vy=sinf(an)*v-80;
                    sp[i].a=an;sp[i].spin=((rand()%200)-100)/15.0f;sp[i].life=0.9f+(rand()%40)/100.0f;sp[i].s=UI_PSTAR+rand()%5;
                }
                for(int i=0;i<STARS;i++)if(sp[i].life>0){
                    float dt=1.0f/60;sp[i].vy+=400*dt;sp[i].x+=sp[i].vx*dt;sp[i].y+=sp[i].vy*dt;sp[i].a+=sp[i].spin*dt;sp[i].life-=dt;
                    float al=sp[i].life<0.4f?sp[i].life/0.4f:1;
                    if(sp[i].y>110 && sp[i].y<440)ui_draw_rotated(gs,sp[i].s,sp[i].x,sp[i].y,0.8f,sp[i].a,((unsigned)(0x80*al)<<24)|0x808080);
                }
            }
            snprintf(line,sizeof(line),"%d",level_end_shown_score(&level_end));text(320-(int)strlen(line)*12,262,2,cream,line);
            if(level_end_new_highscore(&level_end))ui_draw(gs,UI_NEW_HIGHSCORE,470,270,1.0f,0x80808080);
            else if(!level_end_counting(&level_end)){snprintf(line,sizeof(line),"Recorde: %d",g->highscores[g->level]);text(320-(int)strlen(line)*6,292,1,cream,line);}
            {static const int id[3]={UI_MENU,UI_RESTART,UI_NEXT};static const char *const label[3]={"MENU","REINICIAR","PRÓXIMA"};
             static const float x[3]={230,320,410};end_buttons(id,label,3,x,330);}
            hint(320,378,1,1,cream,"{PADLR} ESCOLHER   {X} CONFIRMAR");
        } else if(!paused && g->state==LOST && ui_ready()){
            /* level_end.lua LevelFailed: LEVEL_FAILED_PIG, menu and restart buttons. */
            rect(150,150,340,250,dark);
            text(178,160,2,gold,"NÍVEL NÃO CONCLUÍDO!");
            ui_draw(gs,UI_FAILED_PIG,320,250,1.0f,0x80808080);
            {static const int id[2]={UI_MENU,UI_RESTART};static const char *const label[2]={"MENU","REINICIAR"};
             static const float x[2]={270,370};end_buttons(id,label,2,x,322);}
            hint(320,378,1,1,cream,"{PADLR} ESCOLHER   {X} CONFIRMAR");
        } else if(!paused && g->state==WON){
            /* level_end.lua LevelComplete: stars, score counting up, new-highscore badge, buttons. */
            static const char *stars[4]={"","*","* *","* * *"};char line[48];
            rect(106,165,428,126,dark);
            text(130,178,2,gold,"NÍVEL CONCLUÍDO!");
            text(400,174,3,gold,stars[level_end_shown_stars(&level_end)]);
            snprintf(line,sizeof(line),"%d",level_end_shown_score(&level_end));text(130,212,2,cream,line);
            if(level_end_new_highscore(&level_end))text(300,214,1,gold,"Novo recorde!");
            else if(!level_end_counting(&level_end)){snprintf(line,sizeof(line),"RECORDE %d",g->highscores[g->level]);text(300,214,1,cream,line);}
            {static const char *const name[3]={"MENU","REINICIAR","PRÓXIMA"};
             for(int b=0;b<3;b++)text(130+b*110,250,1,b==end_choice?gold:cream,name[b]);
             hint(130,270,1,0,cream,"{PADLR} ESCOLHER   {X} CONFIRMAR");}
        } else if(paused && ui_ready() && font_ready()){
            /* PausePage.lua: a side panel with resume at the top, restart and menu below it and
               the sound toggle at the bottom (the cross of BUTTON_OFF over it when muted). */
            gsKit_prim_sprite(gs,0,0,150,448,1,GS_SETREG_RGBAQ(10,24,30,0x60,0));
            gsKit_prim_sprite(gs,150,0,640,448,1,GS_SETREG_RGBAQ(0,0,0,0x30,0));
            gsKit_prim_sprite(gs,186,236,640,368,1,GS_SETREG_RGBAQ(10,24,30,0x60,0));   /* behind the hints */
            /* The chosen button is bigger and brighter, with a light ring behind it. */
            static const float by[4]={66,152,238,324},bs[4]={1.7f,1.45f,1.45f,1.6f};
            static const char *label[4]={"CONTINUAR","REINICIAR","MENU","SOM"};
            static const int id[4]={UI_RESUME,UI_RESTART,UI_MENU,UI_SOUND};
            int sound_on=!strcmp(audio_status(),"SOM LIGADO");
            for(int b=0;b<4;b++){
                int sel=b==pause_choice;float k=bs[b]*(sel?1.1f:1.0f);
                if(sel)disk(62,by[b],ui_width(id[b])*k*0.37f+3,color(255,236,150));   /* a slim ring, just wider than the visible button */
                ui_draw(gs,id[b],62,by[b],k,sel?0x80909090:0x80606060);
                if(b==3 && !sound_on)ui_draw(gs,UI_SOUND_OFF,62,by[b],k*0.9f,0x80808080);
                text(16,by[b]+32,1,sel?gold:cream,label[b]);
            }
            hint(16,396,1,0,color(135,192,181),"{PADUD} NAVEGAR");
            hint(16,414,1,0,color(135,192,181),"{X} ESCOLHER   {O} VOLTAR");
            font_draw(gs,FONT_BASIC,395,150,2.2f,FONT_CENTER,0x80808080,"pausa");
            hint(200,250,1,0,cream,"{PAD} MIRA   {X} LANÇAR");
            hint(200,270,1,0,cream,"{Q} SEGURAR: AJUSTE FINO");
            hint(200,290,1,0,cream,"{X} NO AR: HABILIDADE   {SELECT} FASES");
            text(200,330,1,color(135,192,181),"PROTÓTIPO 0.93.2 / PLAYSTATION 2");
            mc_status(200,350,color(135,192,181));
        } else if(paused){
            /* MT_PAUSE; the controls, version and memory-card state live here (the original HUD has none). */
            rect(70,110,500,250,dark);
            text(250,122,2,gold,"PAUSA");
            hint(90,160,1,0,cream,"{PAD} MIRA   {X} LANÇAR");
            hint(90,180,1,0,cream,"{Q} SEGURAR: AJUSTE FINO");
            hint(90,200,1,0,cream,"{X} NO AR: HABILIDADE DO PÁSSARO");
            hint(90,220,1,0,cream,"{T} REINICIAR   {SELECT} FASES");
            hint(90,240,1,0,cream,"{R3} SOM   {START} CONTINUAR");
            text(90,278,1,color(135,192,181),"PROTÓTIPO 0.93.2 / PLAYSTATION 2");
            text(90,298,1,color(135,192,181),audio_status());
            mc_status(90,318,color(135,192,181));
        } else {
            rect(106,185,428,86,dark);
            text(130,204,2,gold,"NÍVEL NÃO CONCLUÍDO!");
            {static const char *const name[2]={"MENU","REINICIAR"};
             for(int b=0;b<2;b++)text(130+b*120,232,1,b==end_choice?gold:cream,name[b]);
             hint(130,252,1,0,cream,"{PADLR} ESCOLHER   {X} CONFIRMAR");}
        }
    }
    if (!connected) text(300,119,1,color(128,48,41),"CONECTE O CONTROLE 1");
    if(menu){
        /* episode_selection.lua / level_selection.lua: episodes, then 15 stages per page (5 x 3). */
        const Menu *m=&menu_state;u64 grey=color(125,142,138),panel=color(44,82,79);
        rect(40,140,560,250,dark);
        if(m->screen==MENU_EPISODES){
            text(60,152,2,gold,"EPISODIOS");
            int n=menu_episode_count(),first=(m->episode/MENU_EPISODES_PER_PAGE)*MENU_EPISODES_PER_PAGE;
            for(int e=first;e<n && e<first+MENU_EPISODES_PER_PAGE;e++){
                float y=186+(e-first)*28;int f=menu_episode_first(e),size=menu_episode_size(e),done=0,stars=0;
                for(int i=f;i<f+size;i++)if(g->highscores[i]>0){done++;stars+=game_level_stars(i,g->highscores[i]);}
                int open=f<g->unlocked;
                if(e==m->episode)rect(52,y-5,536,25,panel);
                text(64,y,1,open?cream:grey,menu_episode_name(e));
                if(open)snprintf(line,sizeof(line),"%3d/%-3d FASES  %3d/%-3d *",done,size,stars,size*3);
                else snprintf(line,sizeof(line),"BLOQUEADO");
                text(340,y,1,open?gold:grey,line);
            }
            snprintf(line,sizeof(line),"EPISODIO %d DE %d",m->episode+1,n);
            text(60,358,1,color(156,195,178),line);
            hint(60,372,1,0,cream,"{PADUD} EPISODIO  {X} ABRIR  {O}/{SELECT} VOLTAR AO JOGO");
        }else{
            int f=menu_episode_first(m->episode),size=menu_episode_size(m->episode);
            int page=m->level/MENU_PER_PAGE,pages=(size+MENU_PER_PAGE-1)/MENU_PER_PAGE;
            text(60,152,2,gold,menu_episode_name(m->episode));
            for(int c=0;c<MENU_PER_PAGE;c++){
                int l=page*MENU_PER_PAGE+c;if(l>=size)break;
                int i=f+l,open=i<g->unlocked;float x=70+(c%MENU_COLUMNS)*102,y=186+(c/MENU_COLUMNS)*52;
                rect(x,y,88,44,l==m->level?gold:open?panel:color(30,45,44));
                text(x+8,y+6,2,l==m->level?dark:open?cream:grey,open?menu_level_label(i):"--");
                static const char *star_text[4]={"","*","* *","* * *"};
                if(open)text(x+8,y+30,1,l==m->level?dark:gold,star_text[game_level_stars(i,g->highscores[i])]);
            }
            for(int d=0;d<pages && d<24;d++)rect(320-pages*7+d*14,344,8,8,d==page?cream:grey);
            int i=f+m->level;
            if(i<g->unlocked)snprintf(line,sizeof(line),"%s  RECORDE %05d",game_level_title(i),g->highscores[i]);
            else snprintf(line,sizeof(line),"%s  BLOQUEADA",game_level_title(i));
            text(60,358,1,color(156,195,178),line);
            hint(60,372,1,0,cream,"{PAD} FASE  {L2}{R2} PAGINA  {X} JOGAR  {O} EPISODIOS");
        }
        mc_status(430,152,color(156,195,178));
    }
    gsKit_queue_exec(gs);
    gsKit_sync_flip(gs);
}

static int init_pad(void)
{
    int sio = SifLoadModule("rom0:SIO2MAN",0,NULL);
    int pad = SifLoadModule("rom0:PADMAN",0,NULL);
    if (sio<0 || pad<0) { printf("PS2Birds: pad modules %d %d\n",sio,pad); return 0; }
    if (padInit(0)<0) return 0;
    return padPortOpen(0,0,pad_buffer) != 0;
}

/* The data files (SPRITES.BIN, SOUNDS.BIN, CAMPAIGN.BIN, made from the player's APK by
   tools/gerar_iso.py) sit next to the ELF: on the disc (cdrom0:\\NAME;1) or in the folder
   the ELF was started from (mass:, host:, hdd...). */
static char data_dir[256];static int data_on_disc;
static void set_data_dir(int argc,char **argv)
{
    const char *self=argc>0 && argv && argv[0]?argv[0]:"";
    if(!strncmp(self,"cdrom",5)){data_on_disc=1;return;}
    size_t cut=0;for(size_t i=0;self[i];i++)if(self[i]=='/'||self[i]=='\\'||self[i]==':')cut=i+1;
    if(cut==0 || cut>=sizeof(data_dir)-16)strcpy(data_dir,"host:");
    else{memcpy(data_dir,self,cut);data_dir[cut]=0;}
}
static const char *data_file(const char *name,char *out)
{
    if(data_on_disc)sprintf(out,"cdrom0:\\%s;1",name);else sprintf(out,"%s%s",data_dir,name);
    return out;
}
int main(int argc,char **argv)
{
    SifInitRpc(0);
    /* 3 MiB one-shot queue (two halves): each frame carries the 560 KiB stage background. */
    gs = gsKit_init_global_custom(3*1024*1024,GS_RENDER_QUEUE_PER_POOLSIZE);
    if (!gs) { printf("PS2Birds: GS allocation failed\n"); SleepThread(); return 1; }
    gs->Mode = GS_MODE_NTSC;
    gs->Width = 640; gs->Height = 448;
    /* 16-bit frame buffers leave about 2.9 MiB of VRAM for the embedded texture pages. */
    gs->PSM = GS_PSM_CT16;
    gs->Dithering = GS_SETTING_ON;
    gs->ZBuffering = GS_SETTING_OFF;
    gs->DoubleBuffering = GS_SETTING_ON;
    dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF,D_CTRL_STS_UNSPEC,D_CTRL_STD_OFF,D_CTRL_RCYC_8,1<<DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);
    gsKit_init_screen(gs);
    gsKit_set_dither_matrix(gs);
    gsKit_set_dither(gs);
    gsKit_mode_switch(gs,GS_ONESHOT);
    gs->PrimAlphaEnable=GS_SETTING_ON;
    gsKit_set_primalpha(gs,ALPHA_BLEND_NORMAL,0);
    char file[300];set_data_dir(argc,argv);
    sprites_set_data_path(data_file("SPRITES.BIN",file));
    audio_set_data_path(data_file("SOUNDS.BIN",file));
    music_set_data_path(data_file("MUSIC.BIN",file));
    ui_set_data_path(data_file("UI.BIN",file));
    font_set_data_path(data_file("FONT.BIN",file));
    menu_gfx_set_data_path(data_file("MENU.BIN",file));
    background_set_data_path(data_file("THEMES.BIN",file));
    cutscene_set_data_path(data_file("CUTSCENE.BIN",file));
    printf("PS2Birds: started as '%s'\n",argc>0 && argv && argv[0]?argv[0]:"");
    if(!sprites_init(gs)){printf("PS2Birds: texture VRAM allocation failed\n");SleepThread();return 1;}
    printf("PS2Birds: interface sprites %s (VRAM %u)\n",ui_init(gs)?"loaded":"unavailable",(unsigned)gs->CurrentPointer);
    printf("PS2Birds: fonts %s (VRAM %u)\n",font_init(gs)?"loaded":"unavailable",(unsigned)gs->CurrentPointer);
    printf("PS2Birds: backgrounds %s\n",background_init()?"found":"unavailable");
    pad_open = init_pad();
    printf("PS2Birds Classic 0.93.2: GS ready, pad open=%d\n",pad_open);
    if(game_campaign_load_file(data_file("CAMPAIGN.BIN",file))<=0 || sprites_load_failed()){
        /* No data files: they are generated from the APK by tools/gerar_iso.py. */
        printf("PS2Birds: data files missing (%s); campaign error %d, sprites error %d\n",file,game_campaign_error(),sprites_error());
        char detail[3][96];
        snprintf(detail[0],sizeof(detail[0]),"CAMPAIGN.BIN: ERRO %d  SPRITES.BIN: ERRO %d",game_campaign_error(),sprites_error());
        snprintf(detail[1],sizeof(detail[1]),"VERSAO 0.93.2");
        snprintf(detail[2],sizeof(detail[2]),"%.90s",data_on_disc?"LENDO DO DISCO (CDROM0)":data_dir);
        for(char *q=detail[2];*q;q++)if(*q>='a'&&*q<='z')*q=(char)(*q-32);
        for(;;){
            gsKit_clear(gs,color(16,40,48));
            text(40,140,2,color(240,200,80),"ARQUIVOS DE DADOS AUSENTES");
            text(40,190,1,color(220,230,230),"GERE A ISO COM A FERRAMENTA GERAR_ISO");
            text(40,214,1,color(220,230,230),"E O APK ORIGINAL (VEJA O LEIA-ME).");
            text(40,250,1,color(160,190,190),"FALTA: CAMPAIGN.BIN, SPRITES.BIN OU SOUNDS.BIN");
            text(40,282,1,color(240,200,80),detail[0]);
            text(40,304,1,color(160,190,190),detail[1]);
            text(40,326,1,color(160,190,190),"PASTA:");
            text(104,326,1,color(160,190,190),detail[2]);
            text(40,360,1,color(160,190,190),"ERRO 1 ABRIR  2 LER  3 MEMORIA  4 FORMATO  5 TAMANHO  6 LIMITES");
            gsKit_queue_exec(gs);gsKit_sync_flip(gs);
        }
    }
    {extern unsigned char save_icon[];sprites_read_icon(save_icon,SAVE_ICON_BYTES);}
    Game g; game_init(&g); save_mc_init(&g); audio_init(); printf("PS2Birds: music %s\n",music_init()?"loaded":"unavailable");
    u32 old_buttons=0;
    int paused=0, previously_connected=0, analog_requested=0,menu=1;menu_state.screen=MENU_MAIN;menu_state.episode=0;menu_state.level=0;
    u64 previous_time=GetTimerSystemTime();
    for (;;) {
        u64 now=GetTimerSystemTime();u32 elapsed_seconds,elapsed_us;
        TimerBusClock2USec(now-previous_time,&elapsed_seconds,&elapsed_us);previous_time=now;
        float frame_seconds=(float)elapsed_seconds+(float)elapsed_us/1000000.0f;
        if(frame_seconds>0.1f)frame_seconds=0.1f;
        save_mc_poll();audio_tick();
        /* Music: title_theme in the menus (gamelogic.lua currentMainMenuSong); in a stage, the looped
           ambience of its theme (themes.lua music, gamelogic.lua currentMusic); level_complete once on a win. */
        if(menu){if(music_current()!=MUSIC_TITLE)music_play(MUSIC_TITLE);}
        else if(g.state!=WON){int ambience=game_level_music(g.level);if(music_current()!=ambience){music_play(ambience);printf("PS2Birds: ambience %d %s\n",ambience,music_current()==ambience?"playing":"unavailable");}}
        music_tick();
        struct padButtonStatus status;
        u32 buttons=0;
        int state=pad_open?padGetState(0,0):PAD_STATE_DISCONN;
        int connected=(state==PAD_STATE_STABLE || state==PAD_STATE_FINDCTP1) && padRead(0,0,&status)>0;
        if (connected) {
            buttons=(u16)~status.btns;
            if (!analog_requested && state==PAD_STATE_STABLE) {
                padSetMainMode(0,0,PAD_MMODE_DUALSHOCK,PAD_MMODE_LOCK);
                analog_requested=1;
            }
        } else if (state==PAD_STATE_DISCONN) analog_requested=0;
        /* Ignore held buttons on reconnection; require a fresh press. */
        if (connected && !previously_connected) old_buttons=buttons;
        u32 pressed=buttons & ~old_buttons;
        if(menu && menu_state.screen==MENU_MAIN && (pressed&PAD_TRIANGLE))menu_yell_request=1;
        if(cutscene_time>=0){
            cutscene_time+=frame_seconds;
            int over=(pressed&(PAD_CROSS|PAD_START)) || !cutscene_draw(gs,cutscene_time);
            if(over){cutscene_stop();cutscene_time=-1;music_play(MUSIC_NONE);audio_play(SFX_START);}
            else{gsKit_queue_exec(gs);gsKit_sync_flip(gs);if(cutscene_music_pending){cutscene_music_pending=0;music_play(cutscene_music());}}
            old_buttons=buttons;previously_connected=connected;continue;
        }
        if ((state==PAD_STATE_DISCONN || state==PAD_STATE_ERROR) && previously_connected) paused=1;
        if(pressed & PAD_R3){audio_toggle();music_set_enabled(!strcmp(audio_status(),"SOM LIGADO"));}
        if(pressed & PAD_L1)save_mc_request();
        if(!menu && !paused && (pressed & PAD_L2) && game_use_powerup(&g))audio_play(SFX_EXPLOSION);
        if(pressed & PAD_SELECT){menu=!menu;menu_open(&menu_state,g.level);audio_play(menu?SFX_MENU_MOVE:SFX_MENU_BACK);}
        if(menu){
            unsigned keys=((pressed&PAD_UP)?MENU_KEY_UP:0)|((pressed&PAD_DOWN)?MENU_KEY_DOWN:0)|((pressed&PAD_LEFT)?MENU_KEY_LEFT:0)|((pressed&PAD_RIGHT)?MENU_KEY_RIGHT:0)
                |((pressed&PAD_CROSS)?MENU_KEY_OK:0)|((pressed&PAD_CIRCLE)?MENU_KEY_BACK:0)|((pressed&PAD_L2)?MENU_KEY_PREV:0)|((pressed&PAD_R2)?MENU_KEY_NEXT:0);
            if(menu_state.screen==MENU_CREDITS){
                if(pressed&(PAD_CIRCLE|PAD_CROSS)){menu_state.screen=MENU_MAIN;audio_play(SFX_MENU_BACK);}
                old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;
            }
            if(menu_state.screen==MENU_MAIN){
                /* Screen order from the top: PLAY, about, sound, gear (the last three only when open). */
                static const int open_order[4]={0,3,2,1},closed_order[2]={0,1};
                const int *order=options_open?open_order:closed_order;int n=options_open?4:2,at=0;
                for(int i=0;i<n;i++)if(order[i]==main_choice)at=i;
                if(pressed&PAD_DOWN){main_choice=order[(at+1)%n];audio_play(SFX_MENU_MOVE);}
                if(pressed&PAD_UP){main_choice=order[(at+n-1)%n];audio_play(SFX_MENU_MOVE);}
                if(pressed&PAD_CROSS){
                    if(main_choice==1){options_open=!options_open;audio_play(SFX_MENU_CONFIRM);}
                    else if(main_choice==2){audio_toggle();music_set_enabled(!strcmp(audio_status(),"SOM LIGADO"));audio_play(SFX_MENU_CONFIRM);}
                    else if(main_choice==3){menu_state.screen=MENU_CREDITS;audio_play(SFX_MENU_CONFIRM);}
                    else keys=MENU_KEY_OK;   /* PLAY */
                    if(main_choice){old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;}
                }else if(pressed&PAD_CIRCLE && options_open){options_open=0;if(main_choice>1)main_choice=1;audio_play(SFX_MENU_BACK);old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;}
                else keys&=MENU_KEY_BACK;
            }
            int was=menu_state.screen,result=menu_input(&menu_state,keys,g.unlocked);
            if(result==MENU_MOVED)audio_play(SFX_MENU_MOVE);
            else if(result==MENU_ENTERED)audio_play(SFX_MENU_CONFIRM);
            else if(result==MENU_LOCKED)audio_play(SFX_MENU_BACK);
            else if(result==MENU_LEFT_SCREEN){audio_play(SFX_MENU_BACK);if(was==MENU_MAIN)menu=0;}
            else if(result==MENU_PLAY && game_load_level(&g,menu_episode_first(menu_state.episode)+menu_state.level)){menu=0;paused=0;audio_play(SFX_MENU_CONFIRM);
                if(episode_intro(g.level,g.highscores))audio_play(SFX_START);}
            old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;
        }
        if(g.state==WON && won_wait>0)won_wait-=frame_seconds;
        else if(g.state==WON){
            /* level_end.lua: star_N as each star appears, score_count while counting, new_highscore with the badge. */
            int stars=level_end_shown_stars(&level_end),badge=level_end_new_highscore(&level_end),counting=level_end_counting(&level_end);
            level_end_update(&level_end,frame_seconds);
            int now=level_end_shown_stars(&level_end);
            if(now>stars)audio_play(now==1?SFX_STAR_1:now==2?SFX_STAR_2:SFX_STAR_3);
            if(counting && level_end_counting(&level_end) && level_end.timer>=level_end.next_count){audio_play(SFX_SCORE_COUNT);level_end.next_count+=0.36f;  /* length of gamescorescreen_score_count_loop */}
            if(!badge && level_end_new_highscore(&level_end))audio_play(SFX_NEW_HIGHSCORE);
        }
        if(paused && connected && !menu){
            /* PausePage navigation: up/down choose, X confirms, O goes back to the game. */
            if(pressed & PAD_UP){pause_choice=(pause_choice+3)%4;audio_play(SFX_MENU_MOVE);}
            if(pressed & PAD_DOWN){pause_choice=(pause_choice+1)%4;audio_play(SFX_MENU_MOVE);}
            int choose=(pressed & PAD_CROSS)?pause_choice:(pressed & PAD_CIRCLE)?0:-1;
            if(choose==0){paused=0;audio_play(SFX_MENU_BACK);}
            else if(choose==1){game_reset(&g);paused=0;audio_play(SFX_START);}
            else if(choose==2){menu=1;paused=0;menu_open(&menu_state,g.level);audio_play(SFX_MENU_BACK);}
            else if(choose==3){audio_toggle();music_set_enabled(!strcmp(audio_status(),"SOM LIGADO"));audio_play(SFX_MENU_CONFIRM);}
            if(choose>=0){old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;}
        }
        if((g.state==WON && won_wait<=0) || g.state==LOST){
            /* The win/lose pop-up works like the menus: the d-pad chooses, X confirms (O: menu). */
            int n=g.state==WON?3:2;
            if(pressed & (PAD_LEFT|PAD_UP)){end_choice=(end_choice+n-1)%n;audio_play(SFX_MENU_MOVE);}
            if(pressed & (PAD_RIGHT|PAD_DOWN)){end_choice=(end_choice+1)%n;audio_play(SFX_MENU_MOVE);}
            int choose=(pressed & PAD_CROSS)?end_choice:(pressed & PAD_CIRCLE)?0:-1;
            if(choose>=0){
                if(g.state==WON && world_pending){world_complete(world_pending-1,g.highscores);world_pending=0;}
                if(choose==0){menu=1;paused=0;menu_open(&menu_state,g.level);audio_play(SFX_MENU_BACK);}
                else if(choose==1){game_reset(&g);paused=0;audio_play(SFX_START);}
                else{if(!game_next_level(&g)){menu=1;menu_open(&menu_state,g.level);audio_play(SFX_MENU_BACK);}else audio_play(SFX_START);paused=0;}
                old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu);continue;
            }
        }
        if ((pressed & PAD_TRIANGLE) && g.state!=WON && g.state!=LOST) { game_reset(&g); paused=0;audio_play(SFX_START); }
        if (connected && (pressed & PAD_START)){paused=!paused;pause_choice=0;audio_play(SFX_MENU_BACK);}
        if (connected && !paused) {
            float horizontal=0, vertical=0;
            if ((status.mode >> 4)==7) {
                int ax=(int)status.ljoy_h-128, ay=(int)status.ljoy_v-128;
                if (abs(ax)>24) horizontal=(ax>0?ax-24:ax+24)/104.0f;
                if (abs(ay)>24) vertical=-(ay>0?ay-24:ay+24)/104.0f;
            }
            if (buttons & (PAD_UP|PAD_DOWN)) vertical=((buttons&PAD_UP)?1:0)-((buttons&PAD_DOWN)?1:0);
            if (buttons & (PAD_RIGHT|PAD_LEFT)) horizontal=((buttons&PAD_RIGHT)?1:0)-((buttons&PAD_LEFT)?1:0);
            float precision=(buttons&PAD_SQUARE)?0.25f:1.0f;
            if(g.state==AIMING && (horizontal!=0 || vertical!=0))audio_play(SFX_STRETCH);
            game_aim(&g,vertical*29.97f*precision*frame_seconds,horizontal*89.91f*precision*frame_seconds);
            if (pressed & PAD_CROSS) {
                if(g.state==AIMING){int type=game_queue_type(&g,0);game_launch(&g);audio_play(type==BIRD_WHITE_TYPE?SFX_LAUNCH_WHITE:type==BIRD_GREEN_TYPE?SFX_LAUNCH_RED:(type==BIRD_BLACK_TYPE||type==BIRD_SHOCK_TYPE)?SFX_LAUNCH_BLACK:type==BIRD_BLUE_TYPE?SFX_LAUNCH_BLUE:type==BIRD_YELLOW_TYPE?SFX_LAUNCH_YELLOW:SFX_LAUNCH_RED);}
                else if(game_special(&g)){GameBird active;if(game_get_projectile(&g,0,&active)){if(active.type==BIRD_BLUE_TYPE)audio_play(SFX_SPLIT);else if(active.type==BIRD_YELLOW_TYPE)audio_play(SFX_BOOST);else if(active.type==BIRD_WHITE_TYPE)audio_play(SFX_DROP_EGG);}}
                printf("PS2Birds: launch state=%d angle=%.1f power=%.1f birds=%d\n",g.state,g.angle,g.power,g.birds);
            }
            GameState before=g.state;int old_record=g.highscores[g.level];
            game_update(&g,frame_seconds);
            if(g.events&GAME_EVENT_EXPLOSION)audio_play(SFX_EXPLOSION);
            if(g.events&GAME_EVENT_PIG)audio_play(SFX_PIG);
            if(g.events&GAME_EVENT_GLASS)audio_play(SFX_GLASS);
            else if(g.events&GAME_EVENT_STONE)audio_play(SFX_STONE);
            else if(g.events&(GAME_EVENT_WOOD|GAME_EVENT_EXTRA))audio_play(SFX_WOOD);
            if(before!=g.state && g.state==WON){music_play(MUSIC_LEVEL_COMPLETE);for(int b=0;b<g.birds;b++){game_add_popup(&g,49+b*24,g.ground_y-15,10000,POPUP_BIRD,game_queue_type(&g,b));g.popups[(g.popup_next+GAME_POPUPS-1)%GAME_POPUPS].time=-0.5f*b;}
                if(old_record<=0){world_pending=g.level+1;}
                end_choice=2;   /* the cursor starts on next */
                won_wait=1.0f+0.5f*g.birds;level_end_start(&level_end,g.score,old_record,game_level_stars(g.level,g.score),game_level_silver(g.level));save_mc_request();audio_play(SFX_WIN);}
            if(before!=g.state && g.state==LOST){audio_play(SFX_LOSE);end_choice=1;}   /* the cursor starts on restart */
            if (before!=g.state) printf("PS2Birds: state=%d score=%d\n",g.state,g.score);
        }
        old_buttons=buttons;
        previously_connected=connected;
        render(&g,connected,paused,menu);
    }
}
