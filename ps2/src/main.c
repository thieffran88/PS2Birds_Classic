#include <tamtypes.h>
#include <kernel.h>
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
#include "save_mc.h"

static GSGLOBAL *gs;
static char pad_buffer[256] __attribute__((aligned(64)));
static int pad_open;

/* Small built-in 5x7 alphabet: no external fonts or files needed to boot. */
static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:.-/+";
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
 {0,0,0,31,0,0,0},{1,2,2,4,8,8,16},{0,4,4,31,4,4,0}
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
static void bird(float x,float y,float radius,int type,float angle,int collided)
{
    int id=type==BIRD_BLUE_TYPE?(collided?SPR_BIRD_BLUE_COLLISION:SPR_BIRD_BLUE):type==BIRD_YELLOW_TYPE?(collided?SPR_BIRD_YELLOW_COLLISION:SPR_BIRD_YELLOW):SPR_BIRD_RED;
    sprite_draw(gs,id,x,y,(render_scale/render_pixels)*(radius/6.0f),angle);
}
static void pig(const GameObject *o)
{
    typedef struct {const char *name;int normal,blink,hurt;} PigSprites;
    static const PigSprites pig_sprites[]={
     {"MediumPiglette",SPR_PIG_BASIC_MEDIUM_01,SPR_PIG_BASIC_MEDIUM_01_BLINK,SPR_PIG_BASIC_MEDIUM_02},
     {"LargePiglette",SPR_PIG_BASIC_BIG_01,SPR_PIG_BASIC_BIG_01_BLINK,SPR_PIG_BASIC_BIG_02},
     {"HelmetPiglette",SPR_PIG_HELMET_01,SPR_PIG_HELMET_01_BLINK,SPR_PIG_HELMET_02},
     {"SmallPiglette",SPR_PIG_BASIC_SMALL_01,SPR_PIG_BASIC_SMALL_01_BLINK,SPR_PIG_BASIC_SMALL_02},
    };
    const PigSprites *set=&pig_sprites[3];
    for(unsigned int i=0;i<sizeof(pig_sprites)/sizeof(pig_sprites[0]);i++)if(!strcmp(o->definition,pig_sprites[i].name)){set=&pig_sprites[i];break;}
    int id=o->health<0.5f?set->hurt:(visual_frame%240<8)?set->blink:set->normal;
    sprite_draw(gs,id,o->x,o->y,render_scale/render_pixels,0);
}
typedef struct {const char *name;int frames[4];} BlockSprites;
static const BlockSprites block_sprites[]={
 {"WoodBlock2",{SPR_BLOCK_WOOD_1_2,SPR_BLOCK_WOOD_2_2,SPR_BLOCK_WOOD_3_2,SPR_BLOCK_WOOD_4_2}},
 {"WoodBlock3",{SPR_BLOCK_WOOD_1_3,SPR_BLOCK_WOOD_2_3,SPR_BLOCK_WOOD_3_3,SPR_BLOCK_WOOD_4_3}},
 {"WoodBlock4",{SPR_BLOCK_WOOD_1_4,SPR_BLOCK_WOOD_2_4,SPR_BLOCK_WOOD_3_4,SPR_BLOCK_WOOD_4_4}},
 {"WoodBlock5",{SPR_BLOCK_WOOD_1_5,SPR_BLOCK_WOOD_2_5,SPR_BLOCK_WOOD_3_5,SPR_BLOCK_WOOD_4_5}},
 {"WoodBlock6",{SPR_BLOCK_WOOD_1_6,SPR_BLOCK_WOOD_2_6,SPR_BLOCK_WOOD_3_6,SPR_BLOCK_WOOD_4_6}},
 {"WoodBlock7",{SPR_BLOCK_WOOD_1_7,SPR_BLOCK_WOOD_2_7,SPR_BLOCK_WOOD_3_7,SPR_BLOCK_WOOD_4_7}},
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
 {"LightBlock6",{SPR_BLOCK_LIGHT_1_6,SPR_BLOCK_LIGHT_2_6,SPR_BLOCK_LIGHT_3_6,SPR_BLOCK_LIGHT_4_6}},
 {"StoneBlock4",{SPR_BLOCK_ROCK_1_4,SPR_BLOCK_ROCK_2_4,SPR_BLOCK_ROCK_3_4,SPR_BLOCK_ROCK_4_4}},
 {"StoneBlock5",{SPR_BLOCK_ROCK_1_5,SPR_BLOCK_ROCK_2_5,SPR_BLOCK_ROCK_3_5,SPR_BLOCK_ROCK_4_5}},
 {"StoneBlock6",{SPR_BLOCK_ROCK_1_6,SPR_BLOCK_ROCK_2_6,SPR_BLOCK_ROCK_3_6,SPR_BLOCK_ROCK_4_6}},
};
static void block(const GameObject *o)
{
    if(o->kind==3){
        int id=!strcmp(o->definition,"StaticBlock_01")?SPR_GROUND_BLOCK_01:!strcmp(o->definition,"StaticBlock_02")?SPR_GROUND_BLOCK_02:!strcmp(o->definition,"StaticBlock_03")?SPR_GROUND_BLOCK_03:!strcmp(o->definition,"StaticBlock_04")?SPR_GROUND_BLOCK_04:!strcmp(o->definition,"StaticBlock_05")?SPR_GROUND_BLOCK_05:!strcmp(o->definition,"Estrade01")?SPR_ESTRADE_01:SPR_ESTRADE_02;
        unsigned int tint=(id==SPR_ESTRADE_01 || id==SPR_ESTRADE_02)?0x80808080:0x80284466;
        sprite_draw_tint(gs,id,o->x,o->y,render_scale/render_pixels,o->angle,tint);return;
    }
    if(o->kind==6){
        int id=!strcmp(o->definition,"ExtraBlockSmiley")?(o->health<0.5f?SPR_BLOCK_SMILEY_3:SPR_BLOCK_SMILEY_1):SPR_BLOCK_TREASURE_CHEST;
        sprite_draw(gs,id,o->x,o->y,render_scale/render_pixels,o->angle);return;
    }
    int stage=o->health>0.75f?0:o->health>0.5f?1:o->health>0.25f?2:3;
    for(unsigned int i=0;i<sizeof(block_sprites)/sizeof(block_sprites[0]);i++)
        if(!strcmp(o->definition,block_sprites[i].name)){
            sprite_draw(gs,block_sprites[i].frames[stage],o->x,o->y,render_scale/render_pixels,o->angle);return;
        }
}

static void render(const Game *g, int connected, int paused,int menu,int selected)
{
    render_scale=g->view_scale;render_pixels=g->pixels;render_ground=g->ground_y;
    render_sling_x=g->sling_x;render_sling_y=g->sling_y;
    if(!paused && !menu)visual_frame++;
    u64 cream=color(248,240,210), dark=color(23,53,58), gold=color(246,191,65);
    gsKit_clear(gs,color(120,194,205));
    rect(0,0,640,109,dark);
    text(30,22,3,cream,"PS2BIRDS CLASSIC");
    text(31,55,1,color(135,192,181),"PROTOTIPO 0.9.0 / PLAYSTATION 2");
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
        GameBird projectile;if(game_get_projectile(g,i,&projectile))bird(projectile.x,projectile.y,6,projectile.type,projectile.angle,projectile.collided);
    }
    if(g->state==AIMING)bird(g->x,g->y,6,game_queue_type(g,0),0,0);
    sprite_draw(gs,SPR_SLING_SHOT_01_FRONT,render_sling_x,render_sling_y,render_scale/render_pixels,0);
    int waiting=g->birds-(g->state==AIMING?1:0);
    for (int i=0; i<waiting; i++) bird(49+i*24,render_ground-15,7,game_queue_type(g,i+(g->state==AIMING?1:0)),0,0);
    snprintf(line,sizeof(line),"ANGULO %04.1f  FORCA %05.1f",g->angle,g->power);
    text(30,119,1,dark,line);
    rect(30,135,110,4,color(76,137,137));
    rect(30,135,(g->power-160)*110/260,4,gold);
    rect(20,391,600,51,dark);
    text(30,400,1,cream,"DIRECIONAL/ANALOGICO: MIRA  X: LANCAR");
    text(30,415,1,color(156,195,178),"TRIANGULO: REINICIO  SELECT: FASES  START: PAUSA");
    GameBird airborne;int airborne_type=game_get_projectile(g,0,&airborne)?airborne.type:-1;
    text(30,430,1,color(156,195,178),!game_special_available(g)?"SEGURE QUADRADO PARA AJUSTE FINO":airborne_type==BIRD_YELLOW_TYPE?"X: ACELERAR AMARELO  QUADRADO: AJUSTE FINO":"X: DIVIDIR AZUL EM TRES  QUADRADO: AJUSTE FINO");
    if (g->state==WON || g->state==LOST || paused) {
        rect(106,185,428,86,dark);
        text(130,204,2,gold,paused?"PAUSADO":g->state==WON?"FASE CONCLUIDA":"TENTE NOVAMENTE");
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
            snprintf(line,sizeof(line),i<g->unlocked?"RECORDE %05d":"BLOQUEADA",g->highscores[i]);
            text(340,y,1,i<g->unlocked?gold:color(125,142,138),line);
        }
        snprintf(line,sizeof(line),"FASES %d-%d DE %d",first+1,first+3<game_level_count()?first+3:game_level_count(),game_level_count());
        text(95,307,1,color(156,195,178),line);
        text(95,328,1,cream,"CIMA/BAIXO: FASE  X: JOGAR  L1: SALVAR");
        text(95,347,1,color(156,195,178),save_mc_status());
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

int main(void)
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
    if(!sprites_init(gs)){printf("PS2Birds: texture VRAM allocation failed\n");SleepThread();return 1;}
    pad_open = init_pad();
    printf("PS2Birds Classic 0.9.0: GS ready, pad open=%d\n",pad_open);
    Game g; game_init(&g); save_mc_init(&g);
    u32 old_buttons=0;
    int paused=0, previously_connected=0, analog_requested=0,menu=1,selected=0;
    for (;;) {
        save_mc_poll();
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
        if(pressed & PAD_L1)save_mc_request();
        if(pressed & PAD_SELECT){menu=!menu;selected=g.level;}
        if(menu){
            if(pressed&PAD_UP)selected=(selected+game_level_count()-1)%game_level_count();
            if(pressed&PAD_DOWN)selected=(selected+1)%game_level_count();
            if((pressed&PAD_CROSS) && game_load_level(&g,selected)){menu=0;paused=0;}
            old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu,selected);continue;
        }
        if ((pressed & PAD_CROSS) && g.state==WON){
            if(!game_next_level(&g)){menu=1;selected=g.level;}
            paused=0;old_buttons=buttons;previously_connected=connected;render(&g,connected,paused,menu,selected);continue;
        }
        if (pressed & PAD_TRIANGLE) { game_reset(&g); paused=0; }
        if (connected && (pressed & PAD_START)) paused=!paused;
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
            game_aim(&g,vertical*0.5f*precision,horizontal*1.5f*precision);
            if (pressed & PAD_CROSS) {
                if(g.state==AIMING)game_launch(&g);else game_special(&g);
                printf("PS2Birds: launch state=%d angle=%.1f power=%.1f birds=%d\n",g.state,g.angle,g.power,g.birds);
            }
            GameState before=g.state;
            game_update(&g,1.0f/59.94f);
            if(before!=g.state && g.state==WON)save_mc_request();
            if (before!=g.state) printf("PS2Birds: state=%d score=%d\n",g.state,g.score);
        }
        old_buttons=buttons;
        previously_connected=connected;
        render(&g,connected,paused,menu,selected);
    }
}
