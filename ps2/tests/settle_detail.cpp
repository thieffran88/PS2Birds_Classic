// Host helper: per-object displacement after the first 0.25 s and 2 s of settling.
#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
int main(int argc,char **argv){
    Game g;game_init(&g);g.unlocked=game_level_count();int level=atoi(argv[1]);game_load_level(&g,level);
    int n=game_object_count(&g);float x0[200],y0[200],a0[200],x1[200],y1[200];
    for(int i=0;i<n;i++){GameObject o;game_get_object(&g,i,&o);x0[i]=o.x;y0[i]=o.y;a0[i]=o.angle;}
    for(int t=0;t<30;t++)game_update(&g,1.0f/120);
    for(int i=0;i<n;i++){GameObject o;game_get_object(&g,i,&o);x1[i]=o.x;y1[i]=o.y;}
    for(int t=0;t<210;t++)game_update(&g,1.0f/120);
    float s=g.view_scale;
    for(int i=0;i<n;i++){GameObject o;game_get_object(&g,i,&o);
        float d1=hypotf(x1[i]-x0[i],y1[i]-y0[i])/s,d2=hypotf(o.x-x0[i],o.y-y0[i])/s;
        if(d2>0.5f)printf("%3d %-14s x=%6.1f y=%6.1f w=%4.1f h=%4.1f a=%5.2f  0.25s=%5.2f 2s=%5.2f\n",i,o.definition,(x0[i]-g.view_x)/s,(y0[i]-g.view_y)/s,o.width/s,o.height/s,a0[i],d1,d2);}
}
