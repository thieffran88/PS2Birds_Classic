/* Developer smoke test, not part of `make test`: loads a Rio CAMPAIGN.BIN (tools/rio/rio_campaign.py)
   through PS2BIRDS_CAMPAIGN, settles the first stage and fires one shot, printing the counters.
   Build and run: see ps2/tests/AUXILIARES.md. */
#include <cstdio>
#include <cmath>
#include "game.h"
int main(){
    Game g;game_init(&g);
    printf("fases %d, objetos %d, pássaros %d, alvos (tipo 4) %d, outras peças %d\n",game_level_count(),game_object_count(&g),g.birds,g.pig_alive,g.block_alive);
    for(int i=0;i<240;i++)game_update(&g,1.0f/120);
    printf("após assentar: alvos %d, peças %d, pontos %d, estado %d\n",g.pig_alive,g.block_alive,g.score,(int)g.state);
    g.angle=0.7f;g.power=0.8f;game_launch(&g);
    for(int t=0;t<2400 && g.state==FLYING;t++)game_update(&g,1.0f/120);
    for(int i=0;i<600;i++)game_update(&g,1.0f/120);
    printf("após um tiro: alvos %d, peças %d, pontos %d, estado %d, x=%.2f y=%.2f finito=%d\n",g.pig_alive,g.block_alive,g.score,(int)g.state,g.x,g.y,(int)(std::isfinite(g.x)&&std::isfinite(g.y)));
    return 0;
}
