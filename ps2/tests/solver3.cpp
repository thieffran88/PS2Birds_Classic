// Host helper (not part of the ELF): beam-search step for one stage.
// Usage: solver3 level [angle power special]...  Replays the given prefix shots, then
// scores every candidate for the next bird and prints the best ones as
// "value angle power special". Values above 1e12 are wins.
#include "game.h"
#include <algorithm>
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
    if(argc<2)return 1;
    int level=atoi(argv[1]);std::vector<Shot> prefix;
    for(int i=2;i+2<argc;i+=3)prefix.push_back(Shot{(float)atof(argv[i]),(float)atof(argv[i+1]),(float)atof(argv[i+2])});
    Game g;game_init(&g);g.unlocked=game_level_count();
    game_load_level(&g,level);settle(&g);for(const Shot &s:prefix)fire(&g,s);
    if(g.state!=AIMING){game_destroy(&g);return 0;}  // prefix already ended the stage
    struct Result{double value;Shot shot;};std::vector<Result> results;
    const float specials[]={-1,0.6f,0.8f,1.0f,1.2f,1.4f,1.6f,1.8f,2.0f,2.3f,2.6f};
    // Optional refinement window: A0/A1/AS and P0/P1/PS environment variables.
    auto env=[](const char *name,int fallback){const char *v=getenv(name);return v?atoi(v):fallback;};
    int a0=env("A0",-10),a1=env("A1",80),as=env("AS",5),p0=env("P0",30),p1=env("P1",100),ps=env("PS",5);
    for(float special:specials)for(int a=a0;a<=a1;a+=as)for(int p=p0;p<=p1;p+=ps){
        game_load_level(&g,level);settle(&g);for(const Shot &s:prefix)fire(&g,s);
        if(g.state!=AIMING)continue;
        Shot candidate{(float)a,(float)p,special};fire(&g,candidate);
        results.push_back(Result{evaluate(&g),candidate});
    }
    std::sort(results.begin(),results.end(),[](const Result &x,const Result &y){return x.value>y.value;});
    for(size_t i=0;i<results.size() && i<6;i++)printf("%.0f %.0f %.0f %.1f\n",results[i].value,results[i].shot.angle,results[i].shot.power,results[i].shot.special);
    game_destroy(&g);return 0;
}
