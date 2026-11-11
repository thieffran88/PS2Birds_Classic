#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
static void shot(Game *g,float a,float power){
    g->angle=a;g->power=power;game_launch(g);
    for(int i=0;i<1320 && g->state==FLYING;i++)game_update(g,1.0f/120);
    assert(isfinite(g->x) && isfinite(g->y));
}
int main(){
    Game g;game_init(&g);
    assert(game_level_count()==8 && g.unlocked==1 && g.level==0);
    void *world=g.physics;
    assert(!game_load_level(&g,-1) && !game_load_level(&g,1) && !game_load_level(&g,8));
    assert(g.physics==world && !game_next_level(&g));
    settle(&g);shot(&g,30,291);assert(g.state==WON && g.unlocked==2 && g.highscores[0]==g.score);
    int first=g.score;game_update(&g,0.1f);assert(g.score==first);
    assert(game_next_level(&g) && g.level==1 && game_object_count(&g)==24 && g.pig_alive==2 && g.birds==4 && g.block_alive==18);
    assert(g.highscores[0]==first && !game_load_level(&g,2));
    settle(&g);assert(g.pig_alive==2 && g.score==0);
    for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,20,360);
    assert(g.state==WON && g.unlocked==3 && g.highscores[1]==g.score);
    int second=g.score;assert(game_next_level(&g));
    assert(g.level==2 && game_object_count(&g)==20 && g.pig_alive==1 && g.birds==4 && g.block_alive==13);
    settle(&g);assert(g.pig_alive==1 && g.score==0);
    for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,15,280);
    assert(g.state==WON && g.unlocked==4 && g.highscores[2]==g.score);
    int third=g.score;game_reset(&g);
    assert(g.level==2 && g.birds==4 && g.pig_alive==1 && g.score==0 && g.highscores[2]==third);
    assert(!game_load_level(&g,4));
    assert(game_load_level(&g,3) && g.level==3 && game_object_count(&g)==36 && g.pig_alive==3 && g.birds==4 && g.block_alive==29);
    settle(&g);assert(g.pig_alive==3 && g.score==0);
    for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,20,410);
    assert(g.state==WON && g.unlocked==5 && g.highscores[3]==g.score);
    int fourth=g.score;assert(game_next_level(&g));
    assert(g.level==4 && game_object_count(&g)==26 && g.pig_alive==4 && g.birds==4 && g.block_alive==10);
    int round_stones=0,chests=0;
    for(int i=0;i<game_object_count(&g);i++){GameObject o;assert(game_get_object(&g,i,&o));if(o.kind==6)chests++;if(!strcmp(o.definition,"StoneBlock7") || !strcmp(o.definition,"StoneBlock8"))round_stones++;}
    assert(round_stones==3 && chests==1);
    settle(&g);assert(g.pig_alive==4 && g.score==0);
    for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,30,380);
    assert(g.state==WON && g.unlocked==6 && g.highscores[4]==g.score);
    int fifth=g.score;
    assert(game_next_level(&g) && g.level==5 && game_object_count(&g)==39 && g.pig_alive==5 && g.block_alive==32 && g.birds==4);
    settle(&g);for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,10,330);
    assert(g.state==WON && g.unlocked==7 && g.highscores[5]==g.score);int sixth=g.score;
    assert(game_next_level(&g) && g.level==6 && game_object_count(&g)==41 && g.pig_alive==2 && g.block_alive==33 && g.birds==4);
    settle(&g);for(int i=0;i<4 && g.state!=WON && g.state!=LOST;i++)shot(&g,10,280);
    assert(g.state==WON && g.unlocked==8 && g.highscores[6]==g.score);int seventh=g.score;
    assert(game_next_level(&g) && g.level==7 && game_object_count(&g)==43 && g.pig_alive==2 && g.block_alive==31 && g.birds==5);
    settle(&g);for(int i=0;i<5 && g.state!=WON && g.state!=LOST;i++){
        g.angle=15;g.power=390;game_launch(&g);bool split=false;
        for(int t=0;t<1320 && g.state==FLYING;t++){
            if(!split && g.flight_time>=0.7f)split=game_special(&g)!=0;
            game_update(&g,1.f/120);
        }
    }
    assert(g.state==WON && g.highscores[7]==g.score && !game_next_level(&g));int eighth=g.score;
    // A shot that never reaches the scene must not activate resting-pressure damage.
    for(int level=5;level<8;level++){
        assert(game_load_level(&g,level));int pigs=g.pig_alive,blocks=g.block_alive;settle(&g);shot(&g,10,160);
        assert(g.state==AIMING && g.pig_alive==pigs && g.block_alive==blocks && g.score==0);
    }
    assert(game_load_level(&g,4));game_reset(&g);assert(g.level==4 && g.highscores[4]==fifth && g.score==0);
    assert(game_load_level(&g,0) && g.highscores[0]==first && g.highscores[1]==second);
    // Exercise world ownership during navigation without changing session progress.
    for(int n=0;n<50;n++)assert(game_load_level(&g,n%8));
    game_destroy(&g);assert(g.physics==NULL);
    printf("Campaign OK: locks, counts, 8 wins (%d/%d/%d/%d/%d/%d/%d/%d), unlocks, session records, reset and 50 transitions.\n",first,second,third,fourth,fifth,sixth,seventh,eighth);
}
