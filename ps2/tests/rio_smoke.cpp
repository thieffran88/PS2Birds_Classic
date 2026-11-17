/* Developer smoke test, not part of `make test`: loads a Rio CAMPAIGN.BIN (tools/rio/rio_campaign.py)
   through PS2BIRDS_CAMPAIGN, settles the first stage and fires one shot, printing the counters.
   Build and run: see ps2/tests/AUXILIARES.md. */
#include <cstdio>
#include <cmath>
#include "game.h"
int main(){
    Game g;game_init(&g);int n=game_level_count();printf("fases %d\n",n);
    for(int l=0;l<n;l++){
        if(l>0 && !game_next_level(&g)){g.unlocked=l+1;game_load_level(&g,l);}
        printf("fase %d (%s): objetos %d, pássaros %d, alvos %d, peças %d | ",l+1,game_level_script(l),game_object_count(&g),g.birds,g.pig_alive,g.block_alive);
        for(int i=0;i<240;i++)game_update(&g,1.0f/120);
        printf("assentou: alvos %d, pontos %d | ",g.pig_alive,g.score);
        g.angle=0.7f;g.power=0.8f;game_launch(&g);
        for(int t=0;t<2400 && g.state==FLYING;t++)game_update(&g,1.0f/120);
        for(int i=0;i<600;i++)game_update(&g,1.0f/120);
        printf("1 tiro: alvos %d, pontos %d, finito=%d\n",g.pig_alive,g.score,(int)(std::isfinite(g.x)&&std::isfinite(g.y)));
    }
    return 0;
}
