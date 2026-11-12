// Host helper (not part of the ELF): greedy per-bird search for one stage.
#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
struct Shot{float angle,power,special;};
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
static void fire(Game *g,const Shot &s){
    g->angle=s.angle;g->power=s.power;game_launch(g);bool used=false;
    for(int t=0;t<2400 && g->state==FLYING;t++){
        if(!used && s.special>=0 && g->flight_time>=s.special)used=game_special(g)!=0;
        game_update(g,1.0f/120);
    }
}
static double evaluate(Game *g){
    if(g->state==WON)return 1e12+g->score;
    double damage=0;int n=game_object_count(g);
    for(int i=0;i<n;i++){GameObject o;game_get_object(g,i,&o);if(o.kind==4)damage+=o.alive?(1-o.health):1.5;}
    return damage*1e6+g->score;
}
int main(int argc,char **argv){
    int level=argc>1?atoi(argv[1]):0;Game g;game_init(&g);g.unlocked=game_level_count();
    game_load_level(&g,level);int birds=g.birds;std::vector<Shot> plan;
    const float specials[]={-1,0.3f,0.5f,0.7f,0.9f,1.2f};
    for(int k=0;k<birds;k++){
        Shot best{0,0,-1};double best_value=-1;
        for(float special:specials)for(int a=-10;a<=80;a+=5)for(int p=30;p<=100;p+=4){
            game_load_level(&g,level);settle(&g);for(const Shot &s:plan)fire(&g,s);
            if(g.state!=AIMING)break;
            Shot cand{(float)a,(float)p,special};fire(&g,cand);double value=evaluate(&g);
            if(value>best_value){best_value=value;best=cand;}
            if(g.state==WON)goto done;
        }
        done:
        plan.push_back(best);printf("bird %d: angle=%.0f power=%.0f special=%.1f value=%.0f\n",k,best.angle,best.power,best.special,best_value);fflush(stdout);
        game_load_level(&g,level);settle(&g);for(const Shot &s:plan)fire(&g,s);
        if(g.state==WON){printf("WIN level %d score=%d after %zu birds\n",level,g.score,plan.size());break;}
    }
    game_destroy(&g);return 0;
}
