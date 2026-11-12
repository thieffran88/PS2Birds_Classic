// Host helper: how far each stage's objects move while settling, before any shot.
#include "game.h"
#include "campaign_types.h"
#include <cmath>
#include <cstdio>
#include <cstring>
int main(int argc,char **argv){
    Game g;game_init(&g);g.unlocked=game_level_count();
    int first=argc>1?atoi(argv[1]):0,last=argc>2?atoi(argv[2]):game_level_count()-1;
    for(int level=first;level<=last;level++){
        game_load_level(&g,level);int n=game_object_count(&g);float x0[MAX_LEVEL_OBJECTS],y0[MAX_LEVEL_OBJECTS],a0[MAX_LEVEL_OBJECTS];
        for(int i=0;i<n;i++){GameObject o;game_get_object(&g,i,&o);x0[i]=o.x;y0[i]=o.y;a0[i]=o.angle;}
        for(int t=0;t<240;t++)game_update(&g,1.0f/120);
        int moved=0,dead=0;float worst=0;const char *worst_def="";
        for(int i=0;i<n;i++){GameObject o;game_get_object(&g,i,&o);if(o.kind==3)continue;if(!o.alive){dead++;continue;}
            float d=hypotf(o.x-x0[i],o.y-y0[i])/g.view_scale;float r=fabsf(o.angle-a0[i]);
            if(d>0.5f||r>0.2f){moved++;if(d>worst){worst=d;worst_def=o.definition;}}}
        printf("%2d %-20s %-12s moved=%3d dead=%d worst=%.1f %s\n",level,game_level_title(level),game_level_script(level),moved,dead,worst,worst_def);
    }
    game_destroy(&g);
}
