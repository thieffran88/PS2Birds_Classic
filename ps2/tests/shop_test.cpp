/* Shop power-ups chosen before a stage: Double Score, Extra Bird and Homing Hat. */
#include "game.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
static int first_piece(const Game *g){GameObject o;for(int i=0;i<game_object_count(g);i++)if(game_get_object(g,i,&o)&&o.alive&&o.kind!=3&&o.kind!=4)return i;return -1;}
static int break_piece(Game *g){int i=first_piece(g);assert(i>=0);game_damage_object(g,i);for(int t=0;t<20;t++)game_update(g,1.0f/60);return g->score;}
/* Smallest distance between the pig and the launched bird during the flight. */
static float closest(Game *g){
    GameObject pig;int p=-1;for(int i=0;i<game_object_count(g);i++)if(game_get_object(g,i,&pig)&&pig.kind==4){p=i;break;}
    assert(p>=0);float best=1e9f;
    for(int t=0;t<1200 && g->state!=AIMING;t++){
        if(t==20)game_use_homing(g);   /* no effect when the power-up is off */
        game_update(g,1.0f/120);GameBird b;GameObject q;
        if(game_get_projectile(g,0,&b)&&game_get_object(g,p,&q)&&q.alive){float d=hypotf(b.x-q.x,b.y-q.y);if(d<best)best=d;}
    }
    return best;
}
int main(){
    Game g;game_init(&g);g.unlocked=game_level_count();
    // Extra Bird: one more bird, of the chosen type, at the end of the queue; kept by a restart.
    assert(game_load_level(&g,0));int birds=g.birds,last=game_queue_type(&g,birds-1);
    g.shop_mask=SHOP_EXTRA_BIRD;g.shop_bird=BIRD_BLUE_TYPE;assert(game_load_level(&g,0));
    assert(g.birds==birds+1 && game_queue_type(&g,birds)==BIRD_BLUE_TYPE && game_queue_type(&g,birds-1)==last);
    game_reset(&g);assert(g.birds==birds+1 && g.shop_mask==SHOP_EXTRA_BIRD);
    g.angle=30;g.power=60;game_launch(&g);assert(g.birds==birds && game_queue_type(&g,birds-1)==BIRD_BLUE_TYPE);
    // Double Score: a broken piece scores twice.
    g.shop_mask=0;assert(game_load_level(&g,0));int single=break_piece(&g);assert(single>0);
    g.shop_mask=SHOP_DOUBLE;assert(game_load_level(&g,0));assert(break_piece(&g)==2*single);
    // Homing Hat: only in flight, once; the bird gets closer to the pig than the same shot without it.
    g.shop_mask=SHOP_HOMING;assert(game_load_level(&g,0));assert(!game_homing_available(&g));
    float best_gain=0;
    for(int angle=20;angle<=60;angle+=10){
        g.shop_mask=0;assert(game_load_level(&g,0));g.angle=(float)angle;g.power=70;game_launch(&g);float without=closest(&g);
        g.shop_mask=SHOP_HOMING;assert(game_load_level(&g,0));g.angle=(float)angle;g.power=70;game_launch(&g);
        assert(game_homing_available(&g));
        float with=closest(&g);assert(!game_homing_available(&g) || g.state==AIMING);
        if(without-with>best_gain)best_gain=without-with;
        assert(with<=without+0.5f);
    }
    printf("Shop OK: extra bird, double score and homing hat (best approach gain %.1f px).\n",best_gain);
    assert(best_gain>1.0f);
    return 0;
}
