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
#include "sprites.h"
#include "definition_sprites.h"
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

static void text(float x, float y, int scale, u64 c, const char *s)
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

static void disk(float x, float y, float radius, u64 c)
{
    /* Scanline sprites share the same GS path as text and UI rectangles. */
    for (int row=-(int)radius; row<(int)radius; row++) {
        float offset=row+0.5f;
        float half=sqrtf(fmaxf(0,radius*radius-offset*offset));
        rect(x-half,y+row,half*2,1,c);
    }
}

static unsigned int visual_frame;
static float render_scale,render_pixels,render_ground,render_sling_x,render_sling_y;
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
static void block_unflipped(const GameObject *o)
{
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

static void render(const Game *g, int connected, int paused,int menu,int selected)
{
    /* Swap texture banks when the active stage belongs to another episode. */
    sprites_select_bank(gs,game_level_bank(g->level));
    render_scale=g->view_scale;render_pixels=g->pixels;render_ground=g->ground_y;
    render_sling_x=g->sling_x;render_sling_y=g->sling_y;
    if(!paused && !menu)visual_frame++;
    u64 cream=color(248,240,210), dark=color(23,53,58), gold=color(246,191,65);
    gsKit_clear(gs,color(120,194,205));
    rect(0,0,640,109,dark);
    text(30,22,3,cream,"PS2BIRDS CLASSIC");
    if(sprites_load_failed())text(31,55,1,gold,"SPRITES.BIN AUSENTE: COPIE-O PARA A PASTA DO ELF");
    else text(31,55,1,color(135,192,181),"PROTOTIPO 0.46.0 / PLAYSTATION 2");
    text(430,55,1,color(135,192,181),audio_status());
    text(31,68,1,save_mc_busy()?gold:color(135,192,181),save_mc_status());
    text(31,80,2,gold,game_level_title(g->level));
    char line[64];
    snprintf(line,sizeof(line),"PONTOS %05d",g->score);
    text(430,82,1,cream,line);
    snprintf(line,sizeof(line),"PORCOS %d BLOCOS %d",g->pig_alive,g->block_alive);
    text(430,96,1,color(156,195,178),line);
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
        GameBird projectile;if(game_get_projectile(g,i,&projectile))bird(projectile.x,projectile.y,6,projectile.type,projectile.angle,projectile.collided,projectile.fuse,projectile.special_used);
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
    if(g->state==AIMING)bird(g->x,g->y,6,game_queue_type(g,0),0,0,-2,0);
    sprite_draw(gs,SPR_SLING_SHOT_01_FRONT,render_sling_x,render_sling_y,render_scale/render_pixels,0);
    int waiting=g->birds-(g->state==AIMING?1:0);
    for (int i=0; i<waiting; i++) bird(49+i*24,render_ground-15,7,game_queue_type(g,i+(g->state==AIMING?1:0)),0,0,-2,0);
    snprintf(line,sizeof(line),"ANGULO %05.1f  FORCA %05.1f",g->angle,g->power);
    text(30,119,1,dark,line);
    /* Shop tutorials: the stage's power-up (King Sling is always on; Bird Quake on L2, once). */
    if(game_powerup(g)==POWERUP_KING_SLING)text(330,119,1,dark,"ESTILINGUE REAL");
    else if(game_powerup_available(g))text(330,119,1,dark,"L2: TERREMOTO");
    rect(30,135,110,4,color(76,137,137));
    rect(30,135,(g->power-POWER_MIN)*110/(POWER_MAX-POWER_MIN),4,gold);
    rect(20,391,600,51,dark);
    text(30,400,1,cream,"DIRECIONAL/ANALOGICO: MIRA  X: LANCAR");
    text(30,415,1,color(156,195,178),"TRIANGULO: REINICIO  SELECT: FASES  START: PAUSA");
    GameBird airborne;int airborne_type=game_get_projectile(g,0,&airborne)?airborne.type:-1;
    text(30,430,1,color(156,195,178),!game_special_available(g)?"SEGURE QUADRADO PARA AJUSTE FINO":airborne_type==BIRD_WHITE_TYPE?"X: SOLTAR OVO  QUADRADO: AJUSTE FINO":(airborne_type==BIRD_BLACK_TYPE||airborne_type==BIRD_SHOCK_TYPE)?"X: EXPLODIR PRETO  QUADRADO: AJUSTE FINO":airborne_type==BIRD_YELLOW_TYPE?"X: ACELERAR AMARELO  QUADRADO: AJUSTE FINO":"X: DIVIDIR AZUL EM TRES  QUADRADO: AJUSTE FINO");
    if (g->state==WON || g->state==LOST || paused) {
        rect(106,185,428,86,dark);
        text(130,204,2,gold,paused?"PAUSADO":g->state==WON?"FASE CONCLUIDA":"TENTE NOVAMENTE");
        if(!paused && g->state==WON){static const char *stars[4]={"","*","* *","* * *"};text(400,200,3,gold,stars[game_level_stars(g->level,g->score)]);}
        text(130,241,1,cream,paused?"START PARA CONTINUAR":g->state==WON?(g->level+1<game_level_count()?"X: PROXIMA FASE  TRIANGULO: REINICIAR":"X: SELECAO  TRIANGULO: REINICIAR"):"TRIANGULO PARA REINICIAR");
    }
    if (!connected) text(300,119,1,color(128,48,41),"CONECTE O CONTROLE 1");
    if(menu){
        rect(75,154,490,217,dark);text(95,170,2,gold,"SELECIONE A FASE");
        int first=(selected/3)*3;
        for(int i=first;i<game_level_count() && i<first+3;i++){
            float y=207+(i-first)*37;
            if(i==selected)rect(89,y-5,460,31,color(44,82,79));
            text(101,y,1,i<g->unlocked?cream:color(125,142,138),game_level_title(i));
            static const char *star_text[4]={"","*","**","***"};
            if(i<g->unlocked)snprintf(line,sizeof(line),"RECORDE %05d %s",g->highscores[i],star_text[game_level_stars(i,g->highscores[i])]);
            else snprintf(line,sizeof(line),"BLOQUEADA");
            text(340,y,1,i<g->unlocked?gold:color(125,142,138),line);
        }
        snprintf(line,sizeof(line),"LISTA %d-%d DE %d",first+1,first+3<game_level_count()?first+3:game_level_count(),game_level_count());
        text(95,307,1,color(156,195,178),line);
        text(95,321,1,cream,"CIMA/BAIXO: FASE  ESQ/DIR: PAGINA");
        text(95,335,1,cream,"X: JOGAR  L1: SALVAR  R3: SOM");
        text(95,351,1,color(156,195,178),save_mc_status());
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
    gs = gsKit_init_global();
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
    printf("PS2Birds: started as '%s'\n",argc>0 && argv && argv[0]?argv[0]:"");
    if(!sprites_init(gs)){printf("PS2Birds: texture VRAM allocation failed\n");SleepThread();return 1;}
    pad_open = init_pad();
    printf("PS2Birds Classic 0.46.0: GS ready, pad open=%d\n",pad_open);
    if(game_campaign_load_file(data_file("CAMPAIGN.BIN",file))<=0 || sprites_load_failed()){
        /* No data files: they are generated from the APK by tools/gerar_iso.py. */
        printf("PS2Birds: data files missing (%s)\n",file);
        for(;;){
            gsKit_clear(gs,color(16,40,48));
            text(40,140,2,color(240,200,80),"ARQUIVOS DE DADOS AUSENTES");
            text(40,190,1,color(220,230,230),"GERE A ISO COM A FERRAMENTA GERAR_ISO");
            text(40,214,1,color(220,230,230),"E O APK ORIGINAL (VEJA O LEIA-ME).");
            text(40,250,1,color(160,190,190),"FALTA: CAMPAIGN.BIN, SPRITES.BIN OU SOUNDS.BIN");
            gsKit_queue_exec(gs);gsKit_sync_flip(gs);
        }
    }
    {extern unsigned char save_icon[];sprites_read_icon(save_icon,SAVE_ICON_BYTES);}
    Game g; game_init(&g); save_mc_init(&g); audio_init();
    u32 old_buttons=0;
    int paused=0, previously_connected=0, analog_requested=0,menu=1,selected=0;
    u64 previous_time=GetTimerSystemTime();
    for (;;) {
        u64 now=GetTimerSystemTime();u32 elapsed_seconds,elapsed_us;
        TimerBusClock2USec(now-previous_time,&elapsed_seconds,&elapsed_us);previous_time=now;
        float frame_seconds=(float)elapsed_seconds+(float)elapsed_us/1000000.0f;
        if(frame_seconds>0.1f)frame_seconds=0.1f;
        save_mc_poll();audio_tick();
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
        if ((state==PAD_STATE_DISCONN || state==PAD_STATE_ERROR) && previously_connected) paused=1;
        if(pressed & PAD_R3)audio_toggle();
        if(pressed & PAD_L1)save_mc_request();
        if(!menu && !paused && (pressed & PAD_L2) && game_use_powerup(&g))audio_play(SFX_EXPLOSION);
        if(pressed & PAD_SELECT){menu=!menu;selected=g.level;audio_play(menu?SFX_MENU_MOVE:SFX_MENU_BACK);}
        if(menu){
            if(pressed&PAD_UP){selected=(selected+game_level_count()-1)%game_level_count();audio_play(SFX_MENU_MOVE);}
            if(pressed&PAD_DOWN){selected=(selected+1)%game_level_count();audio_play(SFX_MENU_MOVE);}
            if(pressed&(PAD_LEFT|PAD_RIGHT)){
                int count=game_level_count(),pages=(count+2)/3,row=selected%3;
                int page=(selected/3+((pressed&PAD_RIGHT)?1:pages-1))%pages;
                selected=page*3+row;if(selected>=count)selected=count-1;audio_play(SFX_MENU_MOVE);
            }
            if((pressed&PAD_CROSS) && game_load_level(&g,selected)){menu=0;paused=0;audio_play(SFX_MENU_CONFIRM);audio_play(SFX_START);}
            old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu,selected);continue;
        }
        if ((pressed & PAD_CROSS) && g.state==WON){
            if(!game_next_level(&g)){menu=1;selected=g.level;audio_play(SFX_MENU_BACK);}else audio_play(SFX_START);
            paused=0;old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu,selected);continue;
        }
        if (pressed & PAD_TRIANGLE) { game_reset(&g); paused=0;audio_play(SFX_START); }
        if (connected && (pressed & PAD_START)){paused=!paused;audio_play(SFX_MENU_BACK);}
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
            GameState before=g.state;
            game_update(&g,frame_seconds);
            if(g.events&GAME_EVENT_EXPLOSION)audio_play(SFX_EXPLOSION);
            if(g.events&GAME_EVENT_PIG)audio_play(SFX_PIG);
            if(g.events&GAME_EVENT_GLASS)audio_play(SFX_GLASS);
            else if(g.events&GAME_EVENT_STONE)audio_play(SFX_STONE);
            else if(g.events&(GAME_EVENT_WOOD|GAME_EVENT_EXTRA))audio_play(SFX_WOOD);
            if(before!=g.state && g.state==WON){save_mc_request();audio_play(SFX_WIN);}
            if(before!=g.state && g.state==LOST)audio_play(SFX_LOSE);
            if (before!=g.state) printf("PS2Birds: state=%d score=%d\n",g.state,g.score);
        }
        old_buttons=buttons;
        previously_connected=connected;
        render(&g,connected,paused,menu,selected);
    }
}
