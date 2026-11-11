#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef struct {float angle,power,special;} Shot;
/* One expected scene per campaign position, in the order declared by the recovered
   episodes.lua. Shot scripts were found by the host solver; the same shot is repeated
   for every bird unless several shots are listed. */
typedef struct {const char *script;int objects,pigs,blocks,birds;Shot shots[5];int shot_count;} Stage;
static const Stage stages[]={
 {"Level1",19,1,14,3,{{10,350,-1.0f}},1}, /* POACHED EGGS 1-1 */
 {"Level57",27,4,8,5,{{10,240,-1.0f}},1}, /* POACHED EGGS 1-2 */
 {"Level53",15,2,8,4,{{10,290,-1.0f}},1}, /* POACHED EGGS 1-3 */
 {"Level3",20,1,13,4,{{10,300,-1.0f}},1}, /* POACHED EGGS 1-4 */
 {"Level6",39,5,32,4,{{10,330,-1.0f}},1}, /* POACHED EGGS 1-5 */
 {"Level2",24,2,18,4,{{10,360,-1.0f}},1}, /* POACHED EGGS 1-6 */
 {"Level4",36,3,29,4,{{20,410,-1.0f}},1}, /* POACHED EGGS 1-7 */
 {"Level5",26,4,10,4,{{30,380,-1.0f}},1}, /* POACHED EGGS 1-8 */
 {"Level7",41,2,33,4,{{10,280,-1.0f}},1}, /* POACHED EGGS 1-9 */
 {"Level8",43,2,31,5,{{20,380,-1.0f}},1}, /* POACHED EGGS 1-10 */
 {"Level9",43,3,33,3,{{25,270,-1.0f}},1}, /* POACHED EGGS 1-11 */
 {"Level13",41,3,31,4,{{10,350,-1.0f}},1}, /* POACHED EGGS 1-12 */
 {"Level10",39,3,31,4,{{10,410,-1.0f}},1}, /* POACHED EGGS 1-13 */
 {"Level39",27,9,11,4,{{25,390,-1.0f}},1}, /* POACHED EGGS 1-14 */
 {"Level12",44,3,36,4,{{10,420,-1.0f}},1}, /* POACHED EGGS 1-15 */
 {"Level15",39,4,26,5,{{10,290,-1.0f}},1}, /* POACHED EGGS 1-16 */
 {"Level17",44,2,42,3,{{10,410,-1.0f}},1}, /* POACHED EGGS 1-17 */
 {"Level14",32,4,20,5,{{10,300,0.4f}},1}, /* POACHED EGGS 1-18 */
 {"Level16",42,1,32,3,{{10,300,-1.0f}},1}, /* POACHED EGGS 1-19 */
 {"Level23",36,2,29,5,{{10,280,-1.0f}},1}, /* POACHED EGGS 1-20 */
 {"Level44",40,2,30,3,{{10,350,-1.0f}},1}, /* POACHED EGGS 1-21 */
};
#define STAGES ((int)(sizeof(stages)/sizeof(stages[0])))
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
static void fire(Game *g,const Shot *s){
    g->angle=s->angle;g->power=s->power;game_launch(g);int used=0;
    for(int t=0;t<1320 && g->state==FLYING;t++){
        if(!used && s->special>=0 && g->flight_time>=s->special)used=game_special(g)!=0;
        game_update(g,1.0f/120);
    }
    assert(isfinite(g->x) && isfinite(g->y));
}
static void play(Game *g,const Stage *s){
    int birds=g->birds;
    for(int i=0;i<birds && g->state!=WON && g->state!=LOST;i++)fire(g,&s->shots[i<s->shot_count?i:s->shot_count-1]);
}
int main(){
    Game g;game_init(&g);int scores[32]={0};
    assert(game_level_count()==STAGES && STAGES==21 && g.unlocked==1 && g.level==0);
    void *world=g.physics;
    assert(!game_load_level(&g,-1) && !game_load_level(&g,1) && !game_load_level(&g,STAGES));
    assert(g.physics==world && !game_next_level(&g));
    for(int level=0;level<STAGES;level++){
        const Stage *s=&stages[level];
        if(level>0)assert(game_next_level(&g));
        assert(g.level==level && !strcmp(game_level_script(level),s->script));
        assert(game_object_count(&g)==s->objects && g.pig_alive==s->pigs && g.block_alive==s->blocks && g.birds==s->birds);
        assert(!game_load_level(&g,level+1));
        settle(&g);assert(g.pig_alive==s->pigs && g.score==0);
        play(&g,s);
        assert(g.state==WON && g.highscores[level]==g.score && g.score>0);
        int expected_unlock=level+2>STAGES?STAGES:level+2;assert(g.unlocked==expected_unlock);
        scores[level]=g.score;int before=g.score;game_update(&g,0.1f);assert(g.score==before);
    }
    assert(!game_next_level(&g));
    /* Stage families: round stones and chest in Level5, smiley in Level53, large/helmet pigs in Level39/Level44. */
    int chests=0,smileys=0,large=0,helmet=0,round_stones=0;
    for(int level=0;level<STAGES;level++){
        assert(game_load_level(&g,level));
        for(int i=0;i<game_object_count(&g);i++){GameObject o;assert(game_get_object(&g,i,&o));
            if(!strcmp(o.definition,"ExtraTreasureChest"))chests++;
            if(!strcmp(o.definition,"ExtraBlockSmiley")){smileys++;assert(o.kind==6);}
            if(!strcmp(o.definition,"LargePiglette")){large++;assert(o.kind==4);}
            if(!strcmp(o.definition,"HelmetPiglette")){helmet++;assert(o.kind==4);}
            if(!strcmp(o.definition,"StoneBlock7") || !strcmp(o.definition,"StoneBlock8"))round_stones++;
        }
    }
    assert(chests==1 && smileys==1 && large==1 && helmet==1 && round_stones>=3);
    /* A shot that never reaches the scene must not activate resting-pressure damage. */
    for(int level=0;level<STAGES;level+=4){
        assert(game_load_level(&g,level));int pigs=g.pig_alive,blocks=g.block_alive;settle(&g);Shot miss={10,160,-1};fire(&g,&miss);
        assert(g.state==AIMING && g.pig_alive==pigs && g.block_alive==blocks && g.score==0);
    }
    assert(game_load_level(&g,4));game_reset(&g);assert(g.level==4 && g.highscores[4]==scores[4] && g.score==0);
    assert(game_load_level(&g,0) && g.highscores[0]==scores[0] && g.highscores[20]==scores[20]);
    for(int n=0;n<50;n++)assert(game_load_level(&g,n%STAGES));
    game_destroy(&g);assert(g.physics==NULL);
    printf("Campaign OK: %d stages in episode order, locks, counts, wins (",STAGES);
    for(int i=0;i<STAGES;i++)printf("%s%d",i?"/":"",scores[i]);
    printf("), unlocks, session records, reset, miss regression and 50 transitions.\n");
}
