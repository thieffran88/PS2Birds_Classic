// Host helper (not part of the ELF): beam search over whole bird sequences in one process.
// Usage: solver4 level [width] [seconds]. Prints "WIN angle power special wait ..." (one quadruple per bird) or "none".
// WAITS=0,1.5,3,6 (seconds spent aiming before each launch) is for Egg Defender stages; without it the wait is 0.
// Faster than beam.py (which starts one process per prefix): coarser grid, a time budget, specials limited to four timings.
#include "game.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
struct Shot{float angle,power,special,wait;};
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
static void fire(Game *g,const Shot &s){
    for(int t=0;t<(int)(s.wait*120) && g->state==AIMING;t++)game_update(g,1.0f/120);   // Egg Defender: the vehicles arrive in waves
    if(g->state!=AIMING)return;
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
    if(argc<2)return 1;
    int level=atoi(argv[1]),width=argc>2?atoi(argv[2]):3;double budget=argc>3?atof(argv[3]):900;
    auto t0=std::chrono::steady_clock::now();
    auto elapsed=[&]{return std::chrono::duration<double>(std::chrono::steady_clock::now()-t0).count();};
    Game g;game_init(&g);g.unlocked=game_level_count();game_load_level(&g,level);settle(&g);int birds=g.birds;
    const float specials[]={-1,0.7f,1.1f,1.5f};
    std::vector<float> waits;{const char *w=getenv("WAITS");if(!w)waits.push_back(0);else for(const char *q=w;*q;){waits.push_back((float)atof(q));while(*q && *q!=',')q++;if(*q==',')q++;}}
    std::vector<std::vector<Shot> > beam(1);
    for(int depth=0;depth<birds+1;depth++){
        struct Cand{double value;std::vector<Shot> seq;};std::vector<Cand> cands;
        for(const std::vector<Shot> &prefix:beam)
            for(float wait:waits)for(float special:specials)for(int a=-10;a<=80;a+=4)for(int p=30;p<=100;p+=6){
                if(elapsed()>budget){printf("none (time)\n");return 0;}
                game_load_level(&g,level);settle(&g);for(const Shot &s:prefix)fire(&g,s);
                if(g.state!=AIMING)continue;
                Shot c{(float)a,(float)p,special,wait};fire(&g,c);
                std::vector<Shot> seq=prefix;seq.push_back(c);
                double v=evaluate(&g);
                if(v>=1e12){printf("WIN");for(const Shot &s:seq)printf(" %.0f %.0f %.1f %.1f",s.angle,s.power,s.special,s.wait);printf(" %.0f\n",v-1e12);return 0;}
                cands.push_back(Cand{v,seq});
            }
        if(cands.empty())break;
        std::sort(cands.begin(),cands.end(),[](const Cand &x,const Cand &y){return x.value>y.value;});
        beam.clear();
        for(const Cand &c:cands){
            bool dup=false;for(const std::vector<Shot> &b:beam){const Shot &x=b.back(),&y=c.seq.back();if(fabsf(x.angle-y.angle)<1 && fabsf(x.power-y.power)<1 && x.special==y.special && x.wait==y.wait && b.size()==c.seq.size()){dup=true;break;}}
            if(!dup)beam.push_back(c.seq);
            if((int)beam.size()>=width)break;
        }
    }
    printf("none\n");return 0;
}
