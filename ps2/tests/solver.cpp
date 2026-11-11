// Host helper (not part of the ELF): search a winning shot script per stage.
// Usage: solver [first_level] [last_level]. Prints angle/power/special timing.
#include "game.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
static void settle(Game *g){for(int i=0;i<240;i++)game_update(g,1.0f/120);}
// Repeats the same shot for every bird; the special is triggered at `special` seconds of flight (<0: never).
static int run(Game *g,int level,float angle,float power,float special){
    g->unlocked=32;if(!game_load_level(g,level))return -1;settle(g);
    int birds=g->birds;
    for(int i=0;i<birds && g->state!=WON && g->state!=LOST;i++){
        g->angle=angle;g->power=power;game_launch(g);bool used=false;
        for(int t=0;t<1320 && g->state==FLYING;t++){
            if(!used && special>=0 && g->flight_time>=special)used=game_special(g)!=0;
            game_update(g,1.0f/120);
        }
    }
    return g->state==WON?g->score:0;
}
int main(int argc,char **argv){
    int first=argc>1?atoi(argv[1]):0,last=argc>2?atoi(argv[2]):game_level_count()-1;
    Game g;game_init(&g);
    for(int level=first;level<=last;level++){
        int best=0;float ba=0,bp=0,bs=-1;
        const float specials[]={-1,0.4f,0.7f,1.0f,1.3f};
        for(int si=0;si<5 && !best;si++)for(int a=10;a<=70 && !best;a+=5)for(int p=160;p<=420 && !best;p+=10){
            int score=run(&g,level,(float)a,(float)p,specials[si]);
            if(score>0){best=score;ba=a;bp=p;bs=specials[si];}
        }
        printf("level %d (%s %s): %s angle=%.0f power=%.0f special=%.1f score=%d\n",level,game_level_title(level),game_level_script(level),best?"WIN":"none",ba,bp,bs,best);
        fflush(stdout);
    }
    game_destroy(&g);return 0;
}
