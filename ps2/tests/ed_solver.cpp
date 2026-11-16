// Host helper (not part of the ELF): beam-search step for an Egg Defender stage.
// Usage: ed_solver level [wait angle power special]...  Replays the prefix, scores every
// candidate for the next bird and prints the best as "value wait angle power special".
// The wait (seconds spent aiming before the launch) matters here: the vehicles arrive in waves.
// Values above 1e12 are wins (the stage is played out until it ends or 60 s pass).
#include "game.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>
struct Shot{float wait,angle,power,special;};
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
static void fire(Game *g,const Shot &s){
    for(int t=0;t<(int)(s.wait*120) && g->state==AIMING;t++)game_update(g,1.0f/120);
    if(g->state!=AIMING)return;
    g->angle=s.angle;g->power=s.power;game_launch(g);bool used=false;
    for(int t=0;t<2400 && g->state==FLYING;t++){
        if(!used && s.special>=0 && g->flight_time>=s.special)used=game_special(g)!=0;
        game_update(g,1.0f/120);
    }
}
static double evaluate(Game *g){
    if(g->state==AIMING && g->birds==0)for(int t=0;t<7200 && g->state==AIMING;t++)game_update(g,1.0f/120);
    if(g->state==WON)return 1e12+g->score;
    if(g->state==LOST)return 0;
    // still playing: survive as long as possible with the most pigs down
    double damage=0;int n=game_object_count(g);
    for(int i=0;i<n;i++){GameObject o;game_get_object(g,i,&o);if(o.kind==4)damage+=o.alive?(1-o.health):1.5;}
    return 1e6+damage*1e6+g->score;
}
int main(int argc,char **argv){
    if(argc<2)return 1;
    int level=atoi(argv[1]);std::vector<Shot> prefix;
    for(int i=2;i+3<argc;i+=4)prefix.push_back(Shot{(float)atof(argv[i]),(float)atof(argv[i+1]),(float)atof(argv[i+2]),(float)atof(argv[i+3])});
    Game g;game_init(&g);g.unlocked=game_level_count();
    struct Result{double value;Shot shot;};std::vector<Result> results;
    const float waits[]={0,3},specials[]={-1,0.5f};
    for(float w:waits)for(float special:specials)for(int a=-10;a<=60;a+=5)for(int p=40;p<=100;p+=6){
        game_load_level(&g,level);settle(&g);for(const Shot &s:prefix)fire(&g,s);
        if(g.state!=AIMING){printf("%s\n",g.state==WON?"PREFIX_WON":"PREFIX_ENDED");game_destroy(&g);return 0;}
        Shot c{w,(float)a,(float)p,special};fire(&g,c);
        results.push_back(Result{evaluate(&g),c});
    }
    std::sort(results.begin(),results.end(),[](const Result &x,const Result &y){return x.value>y.value;});
    for(size_t i=0;i<results.size() && i<6;i++)printf("%.0f %.1f %.0f %.0f %.1f\n",results[i].value,results[i].shot.wait,results[i].shot.angle,results[i].shot.power,results[i].shot.special);
    game_destroy(&g);return 0;
}
